using System;
using System.Collections.ObjectModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Windows;
using System.Windows.Data;
using System.Windows.Media.Imaging;
using Newtonsoft.Json;
using System.Globalization;

namespace Client
{
    /// <summary>
    /// Logica di interazione per MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private TcpClient Client { get; set; }                          //Fornisce connessioni client per i servizi di rete TCP.
        private NetworkStream Stream { get; set; }                      //Stream per leggere e scrivere dal server
        private bool Connesso { get; set; }                             //Variabile booleana che identifica la connessione attiva
        private Thread Ascolta { get; set; }                            //Thread in ascolto sul server
        private Thread Riassunto { get; set; }                          //Thread che gestisce il riassunto
        private Thread Verifica { get; set; }
        private ObservableCollection<Applicazione> Apps { get; set; }   //Collezione che contiene le applicazioni del server
        private byte[] Combinazione { get; set; }                       //Combinazione da inviare
        private uint ProcessoFocus { get; set; }                        //Codice del processo in focus
        private Stopwatch TempoC { get; set; }                          //Cronometro di connessione al server
        readonly object key = new object();
        readonly object _lock = new object();

        public MainWindow()
        {
            InitializeComponent();
            Show();
            Thread.CurrentThread.Name = "Main";
            SetUp();
        }

        private void SetUp()
        {
            Client = new TcpClient();
            Stream = null;
            Connesso = false;
            Combinazione = new Byte[4];
            ProcessoFocus = 0;
            Apps = new ObservableCollection<Applicazione>();
            BindingOperations.EnableCollectionSynchronization(Apps, _lock);
            listView.ItemsSource = Apps;
            TempoC = new Stopwatch();
        }

        /// <summary>
        /// Questo metodo gestisce la connessione al server all'indirizzo
        /// IP e alla porta forniti nelle textBox corrispondenti.
        /// Prima di iniziare controlla che questi campi contengano dati.
        /// In tal caso, inizia il tentativo di connessione. 
        /// Alla fine della connessione attiva i thread paralleli che si
        /// occupano delle altre funzioni richieste.
        /// Se il client è già connesso, provvede alla disconnessione.
        /// </summary>
        private void connetti_Click(object sender, RoutedEventArgs e)
        {
            if (!Connesso)
            {
                testo.Text = "";
                if (string.IsNullOrWhiteSpace(indirizzo.Text))
                {
                    testo.AppendText("Il campo Indirizzo non può essere vuoto!\n");
                    return;
                }
                if (string.IsNullOrWhiteSpace(porta.Text))
                {
                    testo.AppendText("Il campo Porta non può essere vuoto.\n");
                    return;
                }
                try
                {
                    IPAddress ind = IPAddress.Parse(indirizzo.Text);
                    int port = int.Parse(porta.Text);
                    Client = new TcpClient();
                    var result = Client.BeginConnect(ind, port, null, null);
                    var success = result.AsyncWaitHandle.WaitOne(TimeSpan.FromSeconds(10));
                    if (success)
                    {
                        TempoC.Restart();
                        connetti.Content = "Disconnetti";
                        testo.AppendText("Connesso a " + indirizzo.Text + ":" + porta.Text + ".\n");
                        Connesso = true;
                        Stream = Client.GetStream();
                        Ascolta = new Thread(ascoltaServer);
                        Ascolta.IsBackground = true;
                        Ascolta.Start();
                        Riassunto = new Thread(monitora);
                        Riassunto.IsBackground = true;
                        Riassunto.Start();
                        indirizzo.IsReadOnly = true;
                        porta.IsReadOnly = true;
                        Verifica = new Thread(controllaConnessione);
                        Verifica.IsBackground = true;
                        Verifica.Start();
                    }
                    else
                    {
                        testo.AppendText("Connessione fallita.\n");
                    }
                }
                catch (FormatException ex)
                {
                    testo.AppendText("Errore nella specifica di indirizzo IP o porta.\n");
                    testo.AppendText(ex.StackTrace);
                }
                catch (Exception ex)
                {
                    testo.AppendText("Errore: " + ex.StackTrace);
                    if (Client != null)
                    {
                        Client.Close();
                        Client = null;
                    }
                    if (Ascolta != null)
                        Ascolta.Abort();
                    if (Riassunto != null)
                        Riassunto.Abort();
                    Connesso = false;
                    connetti.Content = "Connetti";
                }
            }
            else
            {
                disconnetti();
            }
        }

        private void disconnetti()
        {
            if (Connesso)
            {
                TempoC.Stop();
                Connesso = false;
                Ascolta.Join(500);
                Riassunto.Join(500);
                Client.Close();
                ProcessoFocus = 0;
                Client = null;
                Apps.Clear();
                connetti.Content = "Connetti";
                indirizzo.IsReadOnly = false;
                porta.IsReadOnly = false;
            }
        }

        /// <summary>
        /// Il metodo controlla la connessione al server, scatenando
        /// un'eccezione quando questa connessione viene a mancare.
        /// Se la causa della disconnessione è una "disgraceful
        /// disconnection" da parte del server stampa un messaggio
        /// che comunica all'utente la disconnessione.
        /// In caso, invece, di disconnessione dell'utente
        /// chiude la connessione senza particolari avvisi.
        /// </summary>
        private void controllaConnessione()
        {
            Action action;
            while (Connesso)
            {
                bool blockingState = Client.Client.Blocking;
                try
                {
                    byte[] tmp = new byte[1];
                    Client.Client.Blocking = false;
                    if (Connesso)
                        Client.Client.Send(tmp, 0, 0);
                }
                catch (SocketException e)
                {
                    if (e.NativeErrorCode.Equals(10035))
                    {
                        action = () => testo.AppendText("Il server è connesso, ma la Send blocca.");
                        Dispatcher.Invoke(action);
                    }
                    else if (e.NativeErrorCode.Equals(10058))
                    {
                        //Disconnessione del client
                        return;
                    }
                    else
                    {
                        action = () => testo.AppendText("Il server si è disconnesso.");
                        Dispatcher.Invoke(action);
                        action = () => disconnetti();
                        Dispatcher.Invoke(action);
                        return;
                    }
                }
            }
        }

        /// <summary>
        /// Il metodo ascolta il server fin tanto che il client vi è connesso
        /// All'apertura istanzia il buffer di lettura, poi se lo stream
        /// è disponibile, ne legge un byte che conterrà un codice identificativo
        /// dell'operazione:
        /// n -> nuova applicazione
        /// a -> l'applicazione cambia nome e/o icona
        /// f -> focus cambiato
        /// r -> applicazione chiusa
        /// In tutti e tre i casi, legge la lunghezza del messaggio inviato
        /// e poi procede agli aggiornamenti individuati dalla notifica.
        /// </summary>
        private void ascoltaServer()
        {
            Thread.CurrentThread.Name = "Ascolto";
           
            Action action;
            Applicazione a;
            bool pronto = false;
            byte[] readBuffer = new byte[Client.ReceiveBufferSize];
            int len;
            uint processo;

            try
            {
                while (Connesso)
                {
                    leggiStream(Stream, readBuffer, 4);
                    len = BitConverter.ToInt32(readBuffer, 0);
                    Array.Clear(readBuffer, 0, 4);
                    leggiStream(Stream, readBuffer, 1);
                    char c = BitConverter.ToChar(readBuffer, 0);
                    Array.Clear(readBuffer, 0, 1);
                    switch (c)
                    {
                        //Inserimento nuova applicazione
                        //Messaggio: len-'a'-JSON(App)
                        case 'n':
                            leggiStream(Stream, readBuffer, len);
                            string lettura = Encoding.Default.GetString(readBuffer);
                            Array.Clear(readBuffer, 0, len);
                            a = JsonConvert.DeserializeObject<Applicazione>(lettura);
                            if (a.IconLength != 0)
                            {
                                byte[] iconByte = new byte[a.IconLength];
                                leggiStream(Stream, iconByte, a.IconLength);
                                ottieniIcona(a, iconByte);
                                Array.Clear(iconByte, 0, a.IconLength);
                            }
                            if (a.Focus)
                            {
                                if (ProcessoFocus != 0)
                                {
                                    lock (_lock)
                                    {
                                        Apps.Where(i => i.Process == ProcessoFocus).Single().Focus = false;
                                        Apps.Where(i => i.Process == ProcessoFocus).Single().TempoF.Stop();
                                    }
                                }
                                a.TempoF.Start();
                                ProcessoFocus = a.Process;
                            }
                            lock (_lock)
                            {
                                if (Apps.Where(i=>i.Process==a.Process).Count() == 0)
                                    Apps.Add(a);
                            }
                            action = () => testo.AppendText("Nuova applicazione: " + a.Name + ".\n");
                            Dispatcher.Invoke(action);
                            if (!pronto)
                            {
                                pronto = true;
                                lock (key)
                                    Monitor.Pulse(key);
                            }
                            break;
                        
                        //Aggiornamento di un'applicazione
                        //Messaggio: len-'a'-JSON(app)
                        case 'a':
                            leggiStream(Stream, readBuffer, len);
                            lettura = Encoding.Default.GetString(readBuffer);
                            Array.Clear(readBuffer, 0, len);
                            Applicazione app = JsonConvert.DeserializeObject<Applicazione>(lettura);
                            if (app.IconLength != 0)
                            {
                                byte[] iconByte = new byte[app.IconLength];
                                leggiStream(Stream, iconByte, app.IconLength);
                                ottieniIcona(app, iconByte);
                                Array.Clear(iconByte, 0, app.IconLength);
                            }
                            lock (_lock)
                            {
                                a = Apps.Where(i => i.Process == app.Process).Single();
                                a.Name = app.Name;
                                a.Icona = app.Icona;
                            }
                            if (app.Focus) // se ora ha il focus ma prima no
                            {
                                if (!a.Focus)
                                {
                                    if (ProcessoFocus != 0)
                                    {
                                        lock (_lock)
                                        {
                                            Applicazione af = Apps.Where(i => i.Process == ProcessoFocus).Single();
                                            af.Focus = false;
                                            af.TempoF.Stop();
                                        }
                                    }
                                    ProcessoFocus = a.Process;
                                    a.Focus = true;
                                    a.TempoF.Start();
                                    action = () => testo.AppendText("Nuovo focus: " + a.Name + ".\n");
                                    Dispatcher.Invoke(action);
                                }
                            }
                            action = () => testo.AppendText("Applicazione aggiornata: " + a.Name + ".\n");
                            Dispatcher.Invoke(action);
                            break;
                        //Rimozione di un'applicazione
                        //Messaggio: len-'r'-Codice processo App
                        case 'r':
                            leggiStream(Stream, readBuffer, 4);
                            processo = BitConverter.ToUInt32(readBuffer, 0);
                            lock (_lock)
                            {
                                a = Apps.Where(i => i.Process == processo).Single();
                                if (a.Focus)
                                {
                                    a.Focus = false;
                                    a.TempoF.Stop();
                                    ProcessoFocus = 0;
                                }
                                Apps.Remove(a);
                            }
                            action = () => testo.AppendText("Applicazione chiusa: " + a.Name + ".\n");
                            Dispatcher.Invoke(action);
                            Array.Clear(readBuffer, 0, 4);
                            break;
                        //Focus cambiato
                        //Messaggio: len-'f'-Codice processo App
                        case 'f':
                            if (ProcessoFocus != 0)
                            {
                                lock(_lock)
                                {
                                    a = Apps.Where(i => i.Process == ProcessoFocus).Single();
                                }
                                a.Focus = false;
                                a.TempoF.Stop();
                            }
                            leggiStream(Stream, readBuffer, 4);
                            processo = BitConverter.ToUInt32(readBuffer, 0);
                            lock (_lock)
                            {
                                    a = Apps.Where(i => i.Process == processo).Single();
                            }
                            a.Focus = true;
                            a.TempoF.Start();
                            ProcessoFocus = processo;
                            action = () => testo.AppendText("Nuovo focus: " + a.Name + ".\n");
                            Dispatcher.Invoke(action);
                            Array.Clear(readBuffer, 0, 4);
                            break;
                        case 'z':
                            leggiStream(Stream, readBuffer, 4);
                            processo = BitConverter.ToUInt32(readBuffer, 0);
                            if (processo == ProcessoFocus)
                            {
                                lock(_lock)
                                {
                                    a = Apps.Where(i => i.Process == processo).Single();
                                }
                                a.Focus = false;
                                a.TempoF.Stop();
                                ProcessoFocus = 0;
                            }
                            break;
                    default:
                        break;
                    }
                }
                return;
            }
            catch (IOException ex)
            {
                action = () => testo.AppendText(ex + ex.StackTrace + "\n");
                Dispatcher.Invoke(action);
            }
            catch(Exception ex)
            {
                action = () => testo.AppendText(ex + ex.StackTrace + "\n");
                Dispatcher.Invoke(action);
            }
        }

        /// <summary>
        /// La funzione legge dallo stream len bytes e li mette in readBuffer
        /// </summary>
        /// <param name="stream">Stream da cui leggere i dati</param>
        /// <param name="readBuffer">Buffer su cui scrivere i dati letti</param>
        /// <param name="len">Numero di byte da leggere</param>
        private void leggiStream(NetworkStream stream, byte[] readBuffer, int len)
        {
            int bytesLetti = 0;
            int bytesDaLeggere = len;
            while (bytesDaLeggere > 0)
            {
                if (stream.CanRead)
                {
                    if (stream.DataAvailable)
                    {
                        int n = stream.Read(readBuffer, bytesLetti, bytesDaLeggere);
                        bytesLetti += n;
                        bytesDaLeggere -= n;
                    }
                }
            }
        }

        private void ottieniIcona(Applicazione a, byte[] iconByte)
        {
            using (MemoryStream ms = new MemoryStream(iconByte))
            {
                a.Icona = new Icon(ms);
            }
        }

        /// <summary>
        /// Il metodo gestisce il riassunto grafico, aggiornando con
        /// un thread dedicato le informazioni nel datagrid in base
        /// alle informazioni ricevute dal server, che sono trasmesse
        /// alla struttura applicazioni. Ogni volta che in quella struttura
        /// si verificherà una modifica, questa sarà riportata sul riassunto.
        /// </summary>
        private void monitora()
        {
            Thread.CurrentThread.Name = "Riassunto";
            //Finché non ci sono applicazioni nel dizionario è inutile monitorare.
            lock (key)
                Monitor.Wait(key);
            //Il monitoraggio dura fino alla fine della connessione
            while (Connesso)
            {
                lock (_lock)
                {
                    foreach (var a in Apps.ToList())
                    {
                            a.Percentuale = (a.TempoF.Elapsed.TotalMilliseconds / TempoC.Elapsed.TotalMilliseconds) * 100;
                    }
                }
            }
            return;
        }
        private void invia_Click(object sender, RoutedEventArgs e)
        {
            if (!Connesso)
            {
                testo.AppendText("Devi essere connesso per poter inviare tasti all'applicazione in focus!\n");
                return;
            }
            Thread inviaTasti = new Thread(invioFocus);
            inviaTasti.IsBackground = true;
            inviaTasti.Start();
        }

        /// <summary>
        /// Il metodo si occupa di inviare la combinazione
        /// all'applicazione in focus.
        /// </summary>
        private void invioFocus()
        {
            Thread.CurrentThread.Name = "Invio";
            ConsoleKeyInfo cki;
            Action action;
            ConsoleManager.AllocaConsole();
            ConsoleManager.Show();
            Array.Clear(Combinazione, 0, Combinazione.Length);
            cki = Console.ReadKey();
            action = () => testo.AppendText("--- Hai premuto ");
            Dispatcher.Invoke(action);
            if ((cki.Modifiers & ConsoleModifiers.Alt) != 0)
            {
                action = () => testo.AppendText("ALT+");
                Dispatcher.Invoke(action);
                Combinazione[1] = 0x12;
            }
            if ((cki.Modifiers & ConsoleModifiers.Shift) != 0)
            {
                action = () => testo.AppendText("SHIFT+");
                Dispatcher.Invoke(action);
                Combinazione[2] = 0x10;
            }
            if ((cki.Modifiers & ConsoleModifiers.Control) != 0)
            {
                action = () => testo.AppendText("CTRL+");
                Dispatcher.Invoke(action);
                Combinazione[3] = 0x11;
            }
            action = () => testo.AppendText(cki.Key.ToString() + "\n");
            Dispatcher.Invoke(action);
            Combinazione[0] = Convert.ToByte(cki.Key);
            ConsoleManager.Hide();
            action = () => testo.AppendText("Sto inviando la combinazione!\n");
            Dispatcher.Invoke(action);
            if (Combinazione != null && ProcessoFocus != 0)
            {
                byte[] cod_proc = BitConverter.GetBytes(ProcessoFocus);
                int lung = Combinazione.Length;
                byte[] cod_lung = BitConverter.GetBytes(lung);
                byte[] buffer = new byte[cod_lung.Length + cod_proc.Length + Combinazione.Length];
                if (BitConverter.IsLittleEndian)
                {
                    Array.Reverse(cod_lung);
                    uint proc = BitConverter.ToUInt32(cod_proc, 0);
                }
                Buffer.BlockCopy(cod_lung, 0, buffer, 0, cod_lung.Length);
                Buffer.BlockCopy(cod_proc, 0, buffer, cod_lung.Length, cod_proc.Length);
                Buffer.BlockCopy(Combinazione, 0, buffer, cod_lung.Length + cod_proc.Length, Combinazione.Length);
                if (Stream.CanWrite)
                {
                    Stream.Write(buffer, 0, buffer.Length);
                }
            }
            return;
        }
    }
}

namespace System.Windows.Media
{
    /// <summary>
    /// One-way converter from System.Drawing.Image to System.Windows.Media.ImageSource
    /// </summary>
    [ValueConversion(typeof(System.Drawing.Icon), typeof(System.Windows.Media.ImageSource))]
    public class ImageConverter : IValueConverter
    {
        public object Convert(object value, Type targetType,
            object parameter, CultureInfo culture)
        {
            // empty images are empty...
            if (value == null) { return null; }

            var image = (System.Drawing.Icon)value;
            // Winforms Image we want to get the WPF Image from...
            Bitmap bitmap = image.ToBitmap();
            IntPtr hBitmap = bitmap.GetHbitmap();

            ImageSource wpfBitmap = Interop.Imaging.CreateBitmapSourceFromHBitmap(
                                    hBitmap, IntPtr.Zero, Int32Rect.Empty,
                                    BitmapSizeOptions.FromEmptyOptions());

            return wpfBitmap;
        }

        public object ConvertBack(object value, Type targetType,
            object parameter, CultureInfo culture)
        {
            return null;
        }
    }
}
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Newtonsoft.Json;

namespace Client
{
    /// <summary>
    /// Classe dell'applicazione in esecuzione sul server
    /// </summary>
    public class Applicazione
    {
        public string Name { get; set; }                //Nome dell'applicazione
        public uint Process { get; set; }               //Identificativo del processo
        //public bool ExistIcon { get; set; }             //Abbiamo l'icona?
        //public bool ColoredIcon { get; set; }           //L'icona è colorata?
        public Bitmap Icona { get; set; }               //Definizione dell'icona
        public string BitmaskBuffer { get; set; }       //Buffer di bit per la maschera
        public string ColorBuffer { get; set; }         //Buffer di bit per i colori
        public bool Focus { get; set; }                 //L'app ha focus?
        public Stopwatch TempoF { get; set; }           //Cronometro di focus dell'app
        public float Percentuale { get; set; }          //Percentuale di focus
    }

    /// <summary>
    /// Logica di interazione per MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private TcpClient client;                               //Fornisce connessioni client per i servizi di rete TCP.
        private NetworkStream stream;                           //Stream per leggere e scrivere dal server
        private bool connesso;                                  //Variabile booleana che identifica la connessione attiva
        private Thread ascolta;                                 //Thread in ascolto sul server
        private Thread riassunto;                               //Thread che gestisce il riassunto
        private Dictionary<uint, Applicazione> applicazioni;    //Dizionario che contiene le applicazioni del server
        private ObservableCollection<Applicazione> apps;        //Collezione che contiene le applicazioni del server
        private string combinazione;                            //Combinazione da inviare
        private uint processoFocus;                             //Codice del processo in focus
        private Stopwatch tempoC;                               //Cronometro di connessione al server
        readonly object key = new object();

        public ObservableCollection<Applicazione> Apps { get => apps; set => apps = value; }

        public MainWindow()
        {
            InitializeComponent();
            Thread.CurrentThread.Name = "Main";
            SetUp();
        }

        private void SetUp()
        {
            client = null;
            stream = null;
            connesso = false;
            combinazione = null;
            applicazioni = new Dictionary<uint, Applicazione>();
            tempoC = new Stopwatch();
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
            if (!connesso)
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
                    client = new TcpClient();
                    var result = client.BeginConnect(ind, port, null, null);
                    var success = result.AsyncWaitHandle.WaitOne(TimeSpan.FromSeconds(5));
                    if (success)
                    {
                        tempoC.Restart();
                        connetti.Content = "Disconnetti";
                        testo.AppendText("Connesso a " + indirizzo.Text + ":" + porta.Text + ".\n");
                        connesso = true;
                        stream = client.GetStream();
                        ascolta = new Thread(ascoltaServer);
                        ascolta.Start();
                        riassunto = new Thread(monitora);
                        riassunto.Start();
                        indirizzo.IsReadOnly = true;
                        porta.IsReadOnly = true;
                        //controllaConnessione();
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
                    if (client != null)
                    {
                        client.Close();
                        client = null;
                    }
                    if (ascolta != null)
                        ascolta.Abort();
                    if (riassunto != null)
                        riassunto.Abort();
                    connesso = false;
                    connetti.Content = "Connetti";
                }
            }
            else
            {
                tempoC.Stop();
                connesso = false;
                ascolta.Abort();
                ascolta.Join();
                riassunto.Join();
                client.Close();
                client = null;
                connetti.Content = "Connetti";
                indirizzo.IsReadOnly = false;
                porta.IsReadOnly = false;
            }
        }

        /// <summary>
        /// Il metodo permette al thread principale di controllare
        /// se il socket ha ancora una connessione attiva al server.
        /// In caso di disconnessione brutale da parte del server,
        /// avvia il processo di disconnessione.
        /// </summary>
        private void controllaConnessione()
        {
            while (client.Connected) ;
            testo.AppendText("Il server si è disconnesso.\n");
            tempoC.Stop();
            connesso = false;
            ascolta.Abort();
            ascolta.Join();
            riassunto.Join();
            client.Close();
            client = null;
            connetti.Content = "Connetti";
            indirizzo.IsReadOnly = false;
            porta.IsReadOnly = false;
        }

        /// <summary>
        /// Il metodo ascolta il server fin tanto che il client vi è connesso
        /// All'apertura istanzia il buffer di lettura, poi se lo stream
        /// è disponibile, ne legge un byte che conterrà un codice identificativo
        /// dell'operazione:
        /// a -> nuova applicazione
        /// f -> focus cambiato
        /// r -> applicazione chiusa
        /// In tutti e tre i casi, legge la lunghezza del messaggio inviato
        /// e poi procede agli aggiornamenti individuati dalla notifica.
        /// </summary>
        private void ascoltaServer()
        {
            Thread.CurrentThread.Name = "Ascolto";
            bool pronto = false;
            try
            {
                byte[] readBuffer = new byte[client.ReceiveBufferSize];
                int len;
                while (connesso)
                {
                    if (stream.CanRead)
                    {
                        stream.Read(readBuffer, 0, 4);
                        len = BitConverter.ToInt32(readBuffer, 0);
                        stream.Read(readBuffer, 0, 1);
                        char c = BitConverter.ToChar(readBuffer, 0);
                        switch(c)
                        {
                            //Inserimento nuova applicazione
                            //Messaggio: len-'a'-JSON(App)
                            case 'a':
                                stream.Read(readBuffer, 0, len);
                                string res = Encoding.Default.GetString(readBuffer);
                                Applicazione a = new Applicazione();
                                a.TempoF = new Stopwatch();
                                if (a.BitmaskBuffer != "")
                                {
                                    IntPtr bitmask = new IntPtr(Convert.ToInt32(a.BitmaskBuffer, 2));
                                    if (a.ColorBuffer != "")
                                    {
                                        IntPtr color = new IntPtr(Convert.ToInt32(a.ColorBuffer, 2));
                                        a.Icona = System.Drawing.Image.FromHbitmap(bitmask, color);
                                    }
                                    else
                                        a.Icona = System.Drawing.Image.FromHbitmap(bitmask);
                                }
                                if (a.Focus)
                                    a.TempoF.Start();
                                a = JsonConvert.DeserializeObject<Applicazione>(res);
                                applicazioni.Add(key: a.Process, value: a);
                                Apps.Add(a);
                                if (!pronto)
                                {
                                    pronto = true;
                                    lock (key)
                                        Monitor.Pulse(key);
                                }
                                stream.Flush();
                                res = "";
                                break;
                            //Rimozione di un'applicazione
                            //Messaggio: len-'r'-Codice processo App
                            case 'r':
                                stream.Read(readBuffer, 0, len);
                                uint prox = BitConverter.ToUInt32(readBuffer, 0);
                                Apps.Remove(Apps.Where(i => i.Process == prox).Single());
                                applicazioni.Remove(prox);
                                break;
                            //Focus cambiato
                            //Messaggio: len-'f'-Codice processo App
                            case 'f':
                                stream.Read(readBuffer, 0, len);
                                uint proc = BitConverter.ToUInt32(readBuffer, 0);
                                foreach (var app in Apps)
                                { 
                                    if (app.Process != proc)
                                    {
                                        app.Focus = false;
                                        app.TempoF.Stop();
                                    }
                                }
                                Apps.Where(i => i.Process == proc).Single().Focus = true;
                                Apps.Where(i => i.Process == proc).Single().TempoF.Start();
                                processoFocus = proc;
                                break;
                            default:
                                break;
                        }
                    }
                }
                return;
            }
            catch (IOException ex)
            {
                testo.AppendText("Errore: " + ex.StackTrace + "\n");
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
            while (connesso)
            {
                Applicazione app = Apps.Where(i => i.Process == processoFocus).Single();
                app.Percentuale = (app.TempoF.Elapsed.Seconds / tempoC.Elapsed.Seconds) * 100;
            }

            //Alla fine svuota la tabella
            Apps.Clear();
            return;
        }

        /// <summary>
        /// Questo metodo fa partire il thread che si occupa
        /// di inviare i tasti all'applicazione in focus 
        /// e aspetta la sua terminazione. Dopodiché chiama
        /// la funzione invioFocus, che gestisce l'invio al 
        /// server della combinazione.
        /// </summary>
        private void invia_Click(object sender, RoutedEventArgs e)
        {
            Thread.CurrentThread.Name = "Invio";
            if (!connesso)
            {
                testo.AppendText("Devi essere connesso per poter inviare tasti all'applicazione in focus!\n");
                return;
            }
            Window1 w = new Window1();
            w.RaiseCustomEvent += new EventHandler<CustomEventArgs>(w_RaiseCustomEvent);
            w.ShowDialog();
            Thread inviaTasti = new Thread(invioFocus);
            inviaTasti.Start();
        }

        /// <summary>
        /// Il metodo si occupa di inviare la combinazione
        /// all'applicazione in focus.
        /// </summary>
        private void invioFocus()
        {
            if (combinazione != null)
            {
                byte[] cod_proc = BitConverter.GetBytes(processoFocus);
                byte[] codifica = ASCIIEncoding.ASCII.GetBytes(combinazione);
                int lung = codifica.Length;
                byte[] cod_lung = BitConverter.GetBytes(lung);
                if (BitConverter.IsLittleEndian)
                    Array.Reverse(cod_lung);
                Action act = () => { testo.AppendText("Sto inviando la combinazione all'applicazione in focus.\n"); };
                testo.Dispatcher.Invoke(act);
                stream.Write(cod_lung, 0, 4);
                stream.Write(cod_proc, 0, 4);
                stream.Write(codifica, 0, lung);
            }
            return;
        }
        

        /// <summary>
        /// Evento che si occupa di prendere la combinazione codificata da Window1.
        /// </summary>
        void w_RaiseCustomEvent(object sender, CustomEventArgs e)
        {
            combinazione = e.Message;
            testo.AppendText("Combinazione: " + combinazione + ".\n");
        }
    }
}

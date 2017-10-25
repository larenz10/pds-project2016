using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;



namespace Client
{
    /// <summary>
    /// Logica di interazione per Window1.xaml
    /// </summary>
    public partial class Window1 : Window
    {

        public event EventHandler<CustomEventArgs> RaiseCustomEvent;

        public Window1()
        {
            InitializeComponent();
        }

        private void ctrl_Checked(object sender, RoutedEventArgs e)
        {
            combinazione.Text = "CTRL";
            listbox.IsEnabled = true; 
        }

        private void alt_Checked(object sender, RoutedEventArgs e)
        {
            combinazione.Text = "ALT";
            listbox.IsEnabled = true;
        }

        private void shift_Checked(object sender, RoutedEventArgs e)
        {
            combinazione.Text = "SHIFT";
            listbox.IsEnabled = true;
        }

        /// <summary>
        /// Gestisce la chiusura della finestra Window1.
        /// Prima, codifica la combinazione per renderla pronta all'invio.
        /// Dopodiché, solleva l'evento che passa la combinazione a MainWindow.
        /// Infine, chiude la finestra.
        /// </summary>
        private void conferma_Click(object sender, RoutedEventArgs e)
        {
            string[] elem = combinazione.Text.Split(' ');
            string comb = "";
            foreach (var key in elem)
            {
                if (key.Equals("CTRL"))
                    comb += '^';
                else if (key.Equals("ALT"))
                    comb += '%';
                else if (key.Equals("SHIFT"))
                    comb += '+';
                else if (key.Equals("Backspace"))
                    comb += "{BS}";
                else if (key.Equals("Delete"))
                    comb += "{DEL}";
                else if (key.Equals("Esc"))
                    comb += "{ESC}";
                else if (key.Equals("Ins"))
                    comb += "{INS}";
                else if (key.Equals("Invio"))
                    comb += "~";
                else if (key.Equals("Fine"))
                    comb += "{END}";
                else if (key.Equals("Tab"))
                    comb += "{TAB}";
                else if (key.Equals("Spazio"))
                    comb += "{SPAZIO}";
                else
                    comb += key.ToLower();
            }
            RaiseCustomEvent(this, new CustomEventArgs(comb));
            Close();
        }

        private void annulla_Click(object sender, RoutedEventArgs e)
        {
            combinazione.Undo();
            if (combinazione.Text == "")
            {
                ctrl.IsChecked = false;
                alt.IsChecked = false;
                shift.IsChecked = false;
                listbox.IsEnabled = false;
            }

            //combinazione.Text = combinazione.Text.Remove(combinazione.Text.Length - 2);
        }

        private void listbox_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            ListBoxItem item = listbox.SelectedItem as ListBoxItem;
            if (item != null)
            {
                combinazione.Text += " ";
                combinazione.Text += item.Content.ToString();
            }
        }
    }
}

public class CustomEventArgs : EventArgs
{
    public CustomEventArgs(string s)
    {
        msg = s;
    }
    private string msg;
    public string Message
    {
        get { return msg; }
    }
}
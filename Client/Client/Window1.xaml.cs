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
        public Window1()
        {
            InitializeComponent();
        }

        private void radioButton_Checked(object sender, RoutedEventArgs e)
        {
            textBox.Text = "CTRL";
        }

        private void radioButton1_Checked(object sender, RoutedEventArgs e)
        {
            textBox.Text = "ALT";
        }

        private void radioButton2_Checked(object sender, RoutedEventArgs e)
        {
            textBox.Text = "SHIFT";
        }

        /// <summary>
        /// Chiude la finestra Window1, ma prima passa la stringa
        /// contenuta in textBox alla MainWindow che la codificherà
        /// per mandarla al server.
        /// </summary>
        private void button_Click(object sender, RoutedEventArgs e)
        {

        }

        private void ListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (textBox.Text == "")
            {
                textBox.Text = "Devi prima selezionare un modificatore.\n";
                return;
            }
            textBox.Text += " ";
            textBox.Text += listbox.SelectedItem;
        }
    }
}

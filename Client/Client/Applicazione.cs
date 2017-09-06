using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace Client
{
    /// <summary>
    /// Classe dell'applicazione in esecuzione sul server
    /// </summary>
    public class Applicazione : INotifyPropertyChanged
    {
        private bool _focus;
        private double _percentuale;
        private Icon _icon;

        public Applicazione()
        {
            TempoF = new Stopwatch();
        }

        public string Name { get; set; }                //Nome dell'applicazione
        public uint Process { get; set; }               //Identificativo del processo
        public string IconStr { get; set; }             //Stringa dell'icona
        public Icon Icona { get; set; }
        public bool Focus                               //L'app ha focus?
        {
            get
            {
                return _focus;
            }
            set
            {
                if (_focus != value)
                {
                    _focus = value;
                    OnPropertyChanged("Focus");
                }
            }
        }                 
        public Stopwatch TempoF { get; set; }           //Cronometro di focus dell'app
        public double Percentuale {
            get
            {
                return _percentuale;
            }
            set
            {
                if (_percentuale!= value)
                {
                    _percentuale = value;
                    _percentuale = Math.Round(_percentuale, 2);
                    OnPropertyChanged("Percentuale");
                }
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        public void OnPropertyChanged(String propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
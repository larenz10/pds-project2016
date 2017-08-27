using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client
{
    /// <summary>
    /// Classe dell'applicazione in esecuzione sul server
    /// </summary>
    public class Applicazione : INotifyPropertyChanged
    {
        private bool _focus;
        private double _percentuale;

        public string Name { get; set; }                //Nome dell'applicazione
        public uint Process { get; set; }               //Identificativo del processo
        public Bitmap Icona { get; set; }               //Definizione dell'icona
        public string BitmaskBuffer { get; set; }       //Buffer di bit per la maschera
        public string ColorBuffer { get; set; }         //Buffer di bit per i colori
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
        }         //Percentuale di focus

        public event PropertyChangedEventHandler PropertyChanged;

        public void OnPropertyChanged(String propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}

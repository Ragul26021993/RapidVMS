using System.ComponentModel;

namespace Rapid.DGItemClasses
{
    class PartProgramEntities : INotifyPropertyChanged
    {
        //Entity id
        //public int ID;

        public string PPNameStr
        {
            set
            {
                _PPNameStr = value;
                OnPropertyChanged("PPNameStr");
            }
            get
            {
                return _PPNameStr;
            }
        }
        public string PPUserNameStr
        {
            set
            {
                _PPUserNameStr = value;
                OnPropertyChanged("PPUserNameStr");
            }
            get
            {
                return _PPUserNameStr;
            }
        }

        private string _PPNameStr;
        private string _PPUserNameStr;

        public event PropertyChangedEventHandler PropertyChanged;

        private void OnPropertyChanged(string prop)
        {
            if (PropertyChanged != null)
            {
                var e = new PropertyChangedEventArgs(prop);
                PropertyChanged(this, e);
            }
        }
    }
}

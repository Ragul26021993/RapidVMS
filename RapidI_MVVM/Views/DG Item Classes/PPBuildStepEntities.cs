using System.ComponentModel;

namespace Rapid.DGItemClasses
{
    class PPBuildStepEntities : INotifyPropertyChanged
    {
        public int SlNo
        {
            set
            {
                _SlNo = value;
                OnPropertyChanged("SlNo");
            }
            get
            {
                return _SlNo;
            }
        }
        public int ID
        {
            set
            {
                _ID = value;
                OnPropertyChanged("ID");
            }
            get
            {
                return _ID;
            }
        }
        public string ShapeNameStr
        {
            set
            {
                _ShapeNameStr = value;
                OnPropertyChanged("ShapeNameStr");
            }
            get
            {
                return _ShapeNameStr;
            }
        }
        public string NameStr
        {
            set
            {
                _NameStr = value;
                OnPropertyChanged("NameStr");
            }
            get
            {
                return _NameStr;
            }
        }
        public bool IsCriticalStep
        {
            set
            {
                _IsCriticalStep = value;
                OnPropertyChanged("IsCriticalStep");
            }
            get
            {
                return _IsCriticalStep;
            }
        }
        
        private int _SlNo;
        private int _ID;
        private string _ShapeNameStr;
        private string _NameStr;
        private bool _IsCriticalStep;
        
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

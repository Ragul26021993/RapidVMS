using System.ComponentModel;

namespace Rapid.DGItemClasses
{
    class ActionEntities : INotifyPropertyChanged
    {
        //Entity id
        public int ID;
        
        //public properties that can be accessed externally
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
        public string ParentShapeStr
        {
            set
            {
                _ParentShapeStr = value;
                OnPropertyChanged("ParentShapeStr");
            }
            get
            {
                return _ParentShapeStr;
            }
        }
        public bool IsCriticalAction
        {
            set
            {
                _IsCriticalAction = value;
                OnPropertyChanged("IsCriticalAction");
            }
            get
            {
                return _IsCriticalAction;
            }
        }
        public string NoOfPtsStr
        {
            set
            {
                _NoOfPtsStr = value;
                OnPropertyChanged("NoOfPtsStr");
            }
            get
            {
                return _NoOfPtsStr;
            }
        }
        public string PassFailStatusStr
        {
            set
            {
                _PassFailStatusStr = value;
                OnPropertyChanged("PassFailStatusStr");
            }
            get
            {
                return _PassFailStatusStr;
            }
        }
        public bool IsUserEditEnabled
        {
            set
            {
                _IsUserEditEnabled = value;
                OnPropertyChanged("IsUserEditEnabled");
                if (!value){ NoOfPtsStr = ""; PassFailStatusStr = ""; }
            }
            get
            {
                return _IsUserEditEnabled;
            }
        }
        public int NoOfPtsValue
        {
            set
            {
                _NoOfPtsValue = value;
                OnPropertyChanged("NoOfPtsValue");
                NoOfPtsStr = value.ToString();
            }
            get
            {
                return _NoOfPtsValue;
            }
        }
        public bool PassFailStatus
        {
            set
            {
                _PassFailStatus = value;
                OnPropertyChanged("PassFailStatus");
                PassFailStatusStr = value ? "Pass" : "Fail";
            }
            get
            {
                return _PassFailStatus;
            }
        }
        public bool IsProgramNotLoaded
        {
            set
            {
                _IsProgramNotLoaded = value;
                OnPropertyChanged("IsProgramNotLoaded");
            }
            get
            {
                return _IsProgramNotLoaded;
            }
        }
        //public int 
        
        //private variables that contain values for their public counterparts
        private string _NameStr;
        private string _ParentShapeStr;
        private bool _IsCriticalAction;
        private string _NoOfPtsStr;
        private string _PassFailStatusStr;
        private bool _IsUserEditEnabled;
        private int _NoOfPtsValue;
        private bool _PassFailStatus;
        private bool _IsProgramNotLoaded;

        public ActionEntities()
        {
        }

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

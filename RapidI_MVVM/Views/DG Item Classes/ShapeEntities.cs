using System.ComponentModel;

namespace Rapid.DGItemClasses
{
    class ShapeEntities : INotifyPropertyChanged
    {
        public int ID;

        public string NameStr
        {
            get
            {
                return _Name;
            }
            set
            {
                _Name = value;
                OnPropertyChanged("NameStr");
            }
        }
        public string TypeStr
        {
            get
            {
                return _Type;
            }
            set
            {
                _Type = value;
                OnPropertyChanged("TypeStr");
            }
        }
        public bool IsHiddenShape
        {
            get
            {
                return _IsHiddenShape;
            }
            set
            {
                _IsHiddenShape = value;
                OnPropertyChanged("IsHiddenShape");
            }
        }
        public string Param1Str
        {
            get
            {
                return _Param1;
            }
            set
            {
                _Param1 = value;
                OnPropertyChanged("Param1Str");
            }
        }
        public string Param2Str
        {
            get
            {
                return _Param2;
            }
            set
            {
                _Param2 = value;
                OnPropertyChanged("Param2Str");
            }
        }
        public string Param3Str
        {
            get
            {
                return _Param3;
            }
            set
            {
                _Param3 = value;
                OnPropertyChanged("Param3Str");
            }
        }
        public string Param4Str
        {
            get
            {
                return _Param4;
            }
            set
            {
                _Param4 = value;
                OnPropertyChanged("Param4Str");
            }
        }
        public string Param5Str
        {
            get
            {
                return _Param5;
            }
            set
            {
                _Param5 = value;
                OnPropertyChanged("Param5Str");
            }
        }
        public string Param6Str
        {
            get
            {
                return _Param6;
            }
            set
            {
                _Param6 = value;
                OnPropertyChanged("Param6Str");
            }
        }
        public string Param7Str
        {
            get
            {
                return _Param7;
            }
            set
            {
                _Param7 = value;
                OnPropertyChanged("Param7Str");
            }
        }
        public string Param8Str
        {
            get
            {
                return _Param8;
            }
            set
            {
                _Param8 = value;
                OnPropertyChanged("Param8Str");
            }
        }

        private string _Name;
        private string _Type;
        private bool _IsHiddenShape;
        private string _Param1;
        private string _Param2;
        private string _Param3;
        private string _Param4;
        private string _Param5;
        private string _Param6;
        private string _Param7;
        private string _Param8;
        
        public event PropertyChangedEventHandler PropertyChanged;

        private void OnPropertyChanged(string prop)
        {
            if (PropertyChanged != null)
            {
                var e = new PropertyChangedEventArgs(prop);
                PropertyChanged(this, e);
            }
        }

        public int NumberOfParameters()
        {
            if (Param8Str != "")
                return 8;
            if (Param7Str != "")
                return 7;
            if (Param6Str != "")
                return 6;
            if (Param5Str != "")
                return 5;
            if (Param4Str != "")
                return 4;
            if (Param3Str != "")
                return 3;
            if (Param2Str != "")
                return 2;
            if (Param1Str != "")
                return 1;
            return 0;
        }
    }
}

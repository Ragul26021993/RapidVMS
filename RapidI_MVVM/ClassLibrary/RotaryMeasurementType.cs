using System.Collections.ObjectModel;
using System.ComponentModel;

namespace RapidI
{
    class RotaryMeasurementType
    {
        public RotaryMeasurementType()
        {
        }
        string _ProbeTypeName;
        public string ProbtypeName
        {
            get
            {
                return _ProbeTypeName;
            }
            set
            {
                _ProbeTypeName = value;
            }
        }
        public ObservableCollection<RotaryMeasurementTypetails> RMeasurementDetails { get; set; }
    }
    public class RotaryMeasurementTypetails : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        void Notify(string PropName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(PropName));
            }
        }

        private double _ZeroingAngle = 370;

        private string _ComponentName;

        private string _MachineNo;

        public string ComponentName
        {
            get
            {
                return _ComponentName;
            }
            set
            {
                _ComponentName = value;
                Notify("ComponentName");
            }
        }

        public double ZeroingAngle
        {
            get
            {
                return _ZeroingAngle;
            }
            set
            {
                if (this._ZeroingAngle == value) return;
                _ZeroingAngle = value;
                Notify("ZeroingAngle");
            }
        }

        public string MachineNo
        {
            get
            {
                return _MachineNo;
            }
            set
            {
                if (this._MachineNo == value) return;
                _MachineNo = value;
                Notify("MachineNo");
            }
        }

        public long ID = 0;

    }
}

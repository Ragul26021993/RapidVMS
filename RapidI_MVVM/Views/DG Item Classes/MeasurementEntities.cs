using System;
using System.ComponentModel;

namespace Rapid.DGItemClasses
{
    class MeasurementEntities : INotifyPropertyChanged
    {
        //Entity id
        public int ID;
        //Id of entity as Child
        public int ChildID;
        //Distance mode
        public bool DistanceMode;
        public ValueMode StartingMode;
        public ValueMode CurrentMode;
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
        public string TypeStr
        {
            set
            {
                _TypeStr = value;
                OnPropertyChanged("TypeStr");
            }
            get
            {
                return _TypeStr;
            }
        }
        public string ValueStr
        {
            set
            {
                _ValueStr = value;
                SetActualValue(value);
                OnPropertyChanged("ValueStr");
            }
            get
            {
                return _ValueStr;
            }
        }
        public string NominalStr
        {
            set
            {
                if (_NominalStr != value)
                {
                    _NominalStr = value;
                    SetNominalValue(value);
                    OnPropertyChanged("NominalStr");
                }
            }
            get
            {
                return _NominalStr;
            }
        }
        public string LoToleranceStr
        {
            set
            {
                if (_LoToleranceStr != value)
                {
                    _LoToleranceStr = value;
                    SetLoToleranceValue(value);
                    OnPropertyChanged("LoToleranceStr");
                }
            }
            get
            {
                return _LoToleranceStr;
            }
        }
        public string UpToleranceStr
        {
            set
            {
                if (_UpToleranceStr != value)
                {
                    _UpToleranceStr = value;
                    SetUpToleranceValue(value);
                    OnPropertyChanged("UpToleranceStr");
                }
            }
            get
            {
                return _UpToleranceStr;
            }
        }
        public string DeviationStr
        {
            set
            {
                _DeviationStr = value;
                OnPropertyChanged("DeviationStr");
            }
            get
            {
                return _DeviationStr;
            }
        }
        public bool IsCriticalMeasure
        {
            set
            {
                _IsCriticalMeasure = value;
                DecideToleranceVisibility();
                OnPropertyChanged("IsCriticalMeasure");
            }
            get
            {
                return _IsCriticalMeasure;
            }
        }
        public bool ToleranceMode
        {
            set
            {
                _ToleranceMode = value;
                SetToleranceMode(value);
                OnPropertyChanged("ToleranceMode");
            }
            get
            {
                return _ToleranceMode;
            }
        }
        public bool NoGoMeasurement
        {
            get
            {
                return _NoGoMeasurement;
            }
            set
            {
                _NoGoMeasurement = value;
                OnPropertyChanged("NoGoMeasurement");
            }
        }
        public System.Windows.Visibility IsNominalToleranceVisible
        {
            set
            {
                _IsNominalToleranceVisible = value;
                OnPropertyChanged("IsNominalToleranceVisible");
            }
            get
            {
                return _IsNominalToleranceVisible;
            }
        }
        public System.Windows.Visibility IsCriticalChkBxVisible
        {
            set
            {
                _IsCriticalChkBxVisible = value;
                OnPropertyChanged("IsCriticalChkBxVisible");
            }
            get
            {
                return _IsCriticalChkBxVisible;
            }
        }
        public double ActualValue
        {
            set
            {
                _ActualValue = value;
                OnPropertyChanged("ActualValue");
            }
            get
            {
                return _ActualValue;
            }
        }
        public double NominalValue
        {
            set
            {
                _NominalValue = value;
                OnPropertyChanged("NominalValue");
            }
            get
            {
                return _NominalValue;
            }
        }
        public double LoToleranceValue
        {
            set
            {
                _LoToleranceValue = value;
                OnPropertyChanged("LoToleranceValue");
            }
            get
            {
                return _LoToleranceValue;
            }
        }
        public double UpToleranceValue
        {
            set
            {
                _UpToleranceValue = value;
                OnPropertyChanged("UpToleranceValue");
            }
            get
            {
                return _UpToleranceValue;
            }
        }

        //private variables that contain values for their public counterparts
        private string _NameStr;
        private string _TypeStr;
        private string _ValueStr;
        private string _NominalStr;
        private string _LoToleranceStr;
        private string _UpToleranceStr;
        private string _DeviationStr;
        private bool _IsCriticalMeasure;
        private bool _ToleranceMode;
        private System.Windows.Visibility _IsNominalToleranceVisible;
        private System.Windows.Visibility _IsCriticalChkBxVisible;
        private double _ActualValue;
        private double _NominalValue;
        private double _LoToleranceValue;
        private double _UpToleranceValue;
        private bool _NoGoMeasurement;

        private double ConvertedLoToleranceValue;
        private double ConvertedUpToleranceValue;
        private double ActualLoToleranceValue;
        private double ActualUpToleranceValue;

        public bool OverrideToleranceGraphicHiddenState = false;

        public MeasurementEntities()
        {
            IsNominalToleranceVisible = System.Windows.Visibility.Hidden;
            IsCriticalChkBxVisible = System.Windows.Visibility.Hidden;
        }

        public void SetValues(int id, int Cid, string MName, string MType, string MVal, string NomVal, string UpTol, string LoTol, bool CriticalM, bool DistMode, bool _TolMode)
        {
            try
            {
                ID = id;
                ChildID = Cid;
                NameStr = MName;
                TypeStr = MType;
                ValueStr = MVal;
                NominalStr = NomVal;
                UpToleranceStr = UpTol;
                LoToleranceStr = LoTol;
                ToleranceMode = false;
                StartingMode = ValueMode.ToleranceMode;
                CurrentMode = ValueMode.ToleranceMode;
                ToleranceMode = _TolMode;
                IsCriticalMeasure = CriticalM;
                DistanceMode = DistMode;

                DecideToleranceVisibility();
                SetDeviationValue();


            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:ME01", ex); }
        }
        public void SetValues(string MName, string MType, string MVal, string NomVal, string UpTol, string LoTol, bool CriticalM, bool DistMode, bool _TolMode)
        {
            try
            {
                NameStr = MName;
                TypeStr = MType;
                ValueStr = MVal;
                NominalStr = NomVal;
                UpToleranceStr = UpTol;
                LoToleranceStr = LoTol;
                ToleranceMode = false;
                StartingMode = ValueMode.ToleranceMode;
                CurrentMode = ValueMode.ToleranceMode;

                ToleranceMode = _TolMode;
                IsCriticalMeasure = CriticalM;
                DistanceMode = DistMode;

                DecideToleranceVisibility();
                SetDeviationValue();


            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:ME01", ex); }
        }
        public void SetNominalToleranceValues(string NomVal, string UpTol, string LoTol)
        {
            try
            {
                NominalStr = NomVal;
                UpToleranceStr = UpTol;
                LoToleranceStr = LoTol;

                DecideToleranceVisibility();
                SetDeviationValue();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:ME01", ex); }
        }

        private void SetToleranceMode(bool __ToleranceMode)
        {
            if (__ToleranceMode)
            {
                CurrentMode = ValueMode.ToleranceMode;
                ConvertTheValues();
            }
            else
            {
                CurrentMode = ValueMode.AbsoluteMode;
                ConvertTheValues();
            }
            OnPropertyChanged("LoToleranceStr");
            OnPropertyChanged("UpToleranceStr");
        }
        private void SetActualValue(string __ActualVal)
        {
            double Tmp;
            int DegSignIndx = __ActualVal.IndexOf('\u00B0');
            OverrideToleranceGraphicHiddenState = false;
            if (DegSignIndx > -1 && GlobalSettings.CurrentAngleMode == GlobalSettings.AngleMode.Degree_Minute_Second)
            {
                ActualValue = ConvertDegMinSecToDecimal(__ActualVal) * Math.PI / 180;
            }
            else if (double.TryParse(__ActualVal, out Tmp))
            {
                ActualValue = Tmp;
            }
            else
            {
                OverrideToleranceGraphicHiddenState = true;
            }
            //DecideToleranceVisibility();
        }
        private void SetNominalValue(string __NominalVal)
        {
            double Tmp;
            int DegSignIndx = __NominalVal.IndexOf('\u00B0');
            if (DegSignIndx > -1 && GlobalSettings.CurrentAngleMode == GlobalSettings.AngleMode.Degree_Minute_Second)
            {
                NominalValue = ConvertDegMinSecToDecimal(__NominalVal) * Math.PI / 180;
            }
            else if (double.TryParse(__NominalVal, out Tmp))
            {
                NominalValue = Tmp;
            }
            //DecideToleranceVisibility();
        }
        private void SetUpToleranceValue(string __UpToleranceVal)
        {
            if (ToleranceMode)
            {
                StartingMode = ValueMode.ToleranceMode;
                CurrentMode = ValueMode.ToleranceMode;
            }
            else
            {
                StartingMode = ValueMode.AbsoluteMode;
                CurrentMode = ValueMode.AbsoluteMode;
            }
            double Tmp;
            int DegSignIndx = __UpToleranceVal.IndexOf('\u00B0');
            if (DegSignIndx > -1 && GlobalSettings.CurrentAngleMode == GlobalSettings.AngleMode.Degree_Minute_Second)
            {
                UpToleranceValue = ConvertDegMinSecToDecimal(__UpToleranceVal) * Math.PI / 180;
            }
            else if (double.TryParse(__UpToleranceVal, out Tmp))
            {
                UpToleranceValue = Tmp;
            }
            ActualUpToleranceValue = UpToleranceValue;
            ConvertedUpToleranceValue = UpToleranceValue;
            //DecideToleranceVisibility();
        }
        private void SetLoToleranceValue(string __LoToleranceVal)
        {
            double Tmp;

            if (ToleranceMode)
            {
                StartingMode = ValueMode.ToleranceMode;
                CurrentMode = ValueMode.ToleranceMode;
            }
            else
            {
                StartingMode = ValueMode.AbsoluteMode;
                CurrentMode = ValueMode.AbsoluteMode;
            }

            int DegSignIndx = __LoToleranceVal.IndexOf('\u00B0');
            if (DegSignIndx > -1 && GlobalSettings.CurrentAngleMode == GlobalSettings.AngleMode.Degree_Minute_Second)
            {
                LoToleranceValue = ConvertDegMinSecToDecimal(__LoToleranceVal) * Math.PI / 180;
            }
            else if (double.TryParse(__LoToleranceVal, out Tmp))
            {
                LoToleranceValue = Tmp;
            }
            ActualLoToleranceValue = LoToleranceValue;
            ConvertedLoToleranceValue = LoToleranceValue;
        }

        private double ConvertDegMinSecToDecimal(string DegMinSecStr)
        {
            try
            {
                int sign = 1;
                int DegSignIndx = DegMinSecStr.IndexOf('\u00B0'), MinSignIndx = DegMinSecStr.IndexOf('\''), SecSignIndx = DegMinSecStr.IndexOf("\'\'"); //Get the indexes of degree, minute and second symbols
                double DegVal = 0, MinVal = 0, SecVal = 0;
                if (DegSignIndx == -1 && MinSignIndx == -1 && SecSignIndx == -1) return 0;
                if (DegMinSecStr.Contains("-")) sign = -1;
                double returnVal;

                if (DegSignIndx >= 0)
                {
                    string ValStr = DegMinSecStr.Substring(0, DegSignIndx);
                    double.TryParse(ValStr, out DegVal);
                    DegVal = Math.Abs(DegVal);

                    DegMinSecStr = DegMinSecStr.Substring(DegSignIndx + 1, DegMinSecStr.Length - (DegSignIndx + 1));
                    MinSignIndx = DegMinSecStr.IndexOf('\'');
                    SecSignIndx = DegMinSecStr.IndexOf("\'\'");
                }
                if (MinSignIndx >= 0 && MinSignIndx != SecSignIndx)
                {
                    string ValStr = DegMinSecStr.Substring(0, MinSignIndx);
                    double.TryParse(ValStr, out MinVal);
                    MinVal = Math.Abs(MinVal);

                    DegMinSecStr = DegMinSecStr.Substring(MinSignIndx + 1, DegMinSecStr.Length - (MinSignIndx + 1));
                    SecSignIndx = DegMinSecStr.IndexOf("\'\'");
                }
                if (SecSignIndx >= 0)
                {
                    string ValStr = DegMinSecStr.Substring(0, SecSignIndx);
                    double.TryParse(ValStr, out SecVal);
                    SecVal = Math.Abs(SecVal);
                }
                returnVal = (DegVal + MinVal / 60 + SecVal / 3600) * sign;//return value with proper sign
                return returnVal;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:ME02", ex);
                return 0;
            }
        }
        private void SetDeviationValue()
        {
            if (IsNominalToleranceVisible == System.Windows.Visibility.Visible)
            {

                int DegSignIndx = _LoToleranceStr.IndexOf('\u00B0');
                if (DegSignIndx > -1)
                {
                    DeviationStr = DecimalToDeg(ActualValue - NominalValue);
                }
                else
                {
                    DeviationStr = String.Format("{0:0.0000}", ActualValue - NominalValue);
                }
            }
            else
            {
                DeviationStr = "";
            }
        }
        private void DecideToleranceVisibility()
        {
            if (OverrideToleranceGraphicHiddenState)
            {
                if (ValueStr.ToUpper().Contains("PASS"))
                {
                    ActualValue = NominalValue;
                }
                else
                {
                    ActualValue = NominalValue + LoToleranceValue - 1;
                }
                IsNominalToleranceVisible = System.Windows.Visibility.Visible;
                IsCriticalChkBxVisible = System.Windows.Visibility.Visible;
                RWrapper.RW_MeasureParameter.MYINSTANCE().SetMeasureIncludePF(IsCriticalMeasure, ID);
            }
            else
            {
                if (UpToleranceValue == LoToleranceValue && NominalValue == 0)
                {
                    IsNominalToleranceVisible = System.Windows.Visibility.Hidden;
                    IsCriticalChkBxVisible = System.Windows.Visibility.Hidden;
                    RWrapper.RW_MeasureParameter.MYINSTANCE().SetMeasureIncludePF(false, ID);
                }
                else
                {
                    IsNominalToleranceVisible = System.Windows.Visibility.Visible;
                    IsCriticalChkBxVisible = System.Windows.Visibility.Visible;
                    RWrapper.RW_MeasureParameter.MYINSTANCE().SetMeasureIncludePF(IsCriticalMeasure, ID);
                }
            }
        }

        private string DecimalToDeg(double Val)
        {
            Val = Val * 180 / Math.PI;
            string sign = Val < 0 ? "-" : "";
            Val = Math.Abs(Val);
            string RVal = sign + Math.Floor(Val).ToString() + '\u00B0';
            Val = (Val - Math.Floor(Val)) * 60;
            RVal += Math.Floor(Val).ToString("00") + '\'';
            Val = (Val - Math.Floor(Val)) * 60;
            RVal += Math.Floor(Val).ToString("00") + @"''";
            return RVal;
        }

        private void ConvertTheValues()
        {
            try
            {
                if (StartingMode == ValueMode.AbsoluteMode && CurrentMode == ValueMode.ToleranceMode)
                {
                    ConvertedLoToleranceValue = ActualLoToleranceValue - NominalValue;
                    ConvertedUpToleranceValue = ActualUpToleranceValue - NominalValue;
                    int DegSignIndx = _LoToleranceStr.IndexOf('\u00B0');
                    if (DegSignIndx > -1)
                    {

                        LoToleranceStr = DecimalToDeg(ConvertedLoToleranceValue);
                        UpToleranceStr = DecimalToDeg(ConvertedUpToleranceValue);
                    }
                    else
                    {
                        LoToleranceStr = String.Format("{0:0.0000}", ConvertedLoToleranceValue);
                        UpToleranceStr = String.Format("{0:0.0000}", ConvertedUpToleranceValue);
                    }
                }
                else if (StartingMode == ValueMode.ToleranceMode && CurrentMode == ValueMode.AbsoluteMode)
                {
                    ConvertedLoToleranceValue = ActualLoToleranceValue + NominalValue;
                    ConvertedUpToleranceValue = ActualUpToleranceValue + NominalValue;
                    int DegSignIndx = _LoToleranceStr.IndexOf('\u00B0');
                    if (DegSignIndx > -1)
                    {
                        LoToleranceStr = DecimalToDeg(ConvertedLoToleranceValue);
                        UpToleranceStr = DecimalToDeg(ConvertedUpToleranceValue);
                    }
                    else
                    {
                        LoToleranceStr = String.Format("{0:0.0000}", ConvertedLoToleranceValue);
                        UpToleranceStr = String.Format("{0:0.0000}", ConvertedUpToleranceValue);
                    }
                }
                else
                {
                    int DegSignIndx = _LoToleranceStr.IndexOf('\u00B0');
                    if (DegSignIndx > -1)
                    {
                        LoToleranceStr = DecimalToDeg(ActualLoToleranceValue);
                        UpToleranceStr = DecimalToDeg(ActualUpToleranceValue);
                    }
                    else
                    {
                        LoToleranceStr = String.Format("{0:0.0000}", ActualLoToleranceValue);
                        UpToleranceStr = String.Format("{0:0.0000}", ActualUpToleranceValue);
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:ME04", ex);
            }
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
    enum ValueMode
    {
        ToleranceMode,
        AbsoluteMode
    }
}

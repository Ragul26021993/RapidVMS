using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.ComponentModel;
using Rapid.Interface;
using Rapid.IO;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for AxialRunOutWin.xaml
    /// </summary>
    public partial class AxialRunOutWin : Window, INotifyPropertyChanged,ISaveWindowsParameters
    {
        private double _AxialRadius = 2;
        private int  _FluteValue = 2;
        public double AxialRadius
        {
            get
            {
                return _AxialRadius;
            }
            set
            {                
                _AxialRadius = value;
                Notify("AxialRadius");
            }
        }
        public int FluteValue
        {
            get
            {
                return _FluteValue;
            }
            set
            {
                if (Convert.ToInt32(value) < 2)
                {
                    _FluteValue = 2;
                }
                else
                {
                    _FluteValue = value;
                }
                Notify("FluteValue");
            }
        }
        
        private static AxialRunOutWin MyObject;
        public static AxialRunOutWin MyInstance()
        {
            if (MyObject == null)
            {
                MyObject = new AxialRunOutWin();
            }
            return MyObject;
        }
        public AxialRunOutWin()
        {
            InitializeComponent();
            ReadParameters();
        }
          
        protected void btnOK_Click(object sender, EventArgs e)
        {
            AxialRadius = Convert.ToDouble(RadiusTb.Text.ToString());
            FluteValue = Convert.ToInt32(NoOfFlutesTb.Text.ToString());
            RWrapper.RW_MainInterface.MYINSTANCE().SetRunOutFlute_Radius(FluteValue, AxialRadius);
            RWrapper.RW_MainInterface.MYINSTANCE().HandleMeasure_Click("Intersection Runout");
            this.Close();
        }
        protected void btnCancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }
        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            this.Hide();
            SaveParameters();
            e.Cancel = true;            
        }
        public event PropertyChangedEventHandler PropertyChanged;
        void Notify(string propName)
        {
            if (propName != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propName));
            }
        }

        #region To save the Windows Parameters
        public string TextFilePath { get; set; }
        public void SaveParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\AxialRunOutWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("NoOfFlutes", NoOfFlutesTb.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("Radius", RadiusTb.Text.ToString()));
                    ReadWriteText.WriteFileWhenWindowClose(TextFilePath, ListOfParameters);
                    ListOfParameters.Clear();
                }
            }
            catch (Exception)
            {
            }
        }
        public void ReadParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\AxialRunOutWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("NoOfFlutes", NoOfFlutesTb.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("Radius", RadiusTb.Text.ToString()));
                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "NoOfFlutes")
                                {
                                    NoOfFlutesTb.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "Radius")
                                {
                                    RadiusTb.Text = ListOfParameters[i].Value.ToString();
                                }
                            }
                        }
                        ListOfParameters = null;
                        ListOfParameters.Clear();
                    }
                }
            }
            catch (Exception)
            {

            }
        }
        #endregion
    }
}

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
using System.Collections.ObjectModel;
using System.ComponentModel;
using Rapid.IO;
using Rapid.Interface;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for CloudPointMeasurement.xaml
    /// </summary>
    public partial class CloudPointMeasurement : Window
    {
        static CloudPointMeasurement instance;
        public static CloudPointMeasurement Myinstance()
        {
            if (instance == null)
            {
                instance = new CloudPointMeasurement();
            }
            return instance;
        }

        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            instance = null;
            SaveParameters();
        }

        MeasurementParameters MeasurePram;
        System.Collections.ArrayList arrLst;
        public CloudPointMeasurement()
        {
            InitializeComponent();
            ReadParameters();
            MeasurePram = new MeasurementParameters();
            LayoutRoot.DataContext = MeasurePram;
        }
        protected void OKBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                arrLst = new System.Collections.ArrayList();
                SetArrayListBasedonParamSelected(ref arrLst);
                RWrapper.RW_MainInterface.MYINSTANCE().CreateCloudPointsMeasurement(arrLst);
                this.Hide();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CPM01", ex);
            }
        }
        private void SetArrayListBasedonParamSelected(ref System.Collections.ArrayList arlist)
        {
            if (MeasurePram.Sa)
            {
                arlist.Add("Sa");
            }
            if (MeasurePram.Sku)
            {
                arlist.Add("Sku");
            }
            if (MeasurePram.Sp)
            {
                arlist.Add("Sp");
            }
            if (MeasurePram.Sq)
            {
                arlist.Add("Sq");
            }
            if (MeasurePram.Ssk)
            {
                arlist.Add("Ssk");
            }
            if (MeasurePram.Sv)
            {
                arlist.Add("Sv");
            }
            if (MeasurePram.Sz)
            {
                arlist.Add("Sz");
            }
        }

        #region To save the Windows Parameters
        public string TextFilePath { get; set; }
        public void ReadParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\CloudPointMeasurement.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("RMSSurfaceRoughness", RMSSurfaceRoughnessChbk.IsChecked.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("Meansurfaceroughness", MeansurfaceroughnessChbk.IsChecked.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("Maxpeakheight", MaxpeakheightChbk.IsChecked.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("Maxvalleyheight", MaxvalleyheightChbk.IsChecked.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("Peaktovalleyheight", PeaktovalleyheightChbk.IsChecked.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("skewness", skewnessChbk.IsChecked.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("kurtosis", kurtosisChbk.IsChecked.ToString()));

                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "RMSSurfaceRoughness")
                                {
                                    RMSSurfaceRoughnessChbk.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "Meansurfaceroughness")
                                {
                                    MeansurfaceroughnessChbk.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "Maxpeakheight")
                                {
                                    MaxpeakheightChbk.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "Maxvalleyheight")
                                {
                                    MaxvalleyheightChbk.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "Peaktovalleyheight")
                                {
                                    PeaktovalleyheightChbk.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "skewness")
                                {
                                    skewnessChbk.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "kurtosis")
                                {
                                    kurtosisChbk.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                            }
                        }
                    }
                }
            }
            catch (Exception)
            {

            }
        }
        public void SaveParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\CloudPointMeasurement.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("RMSSurfaceRoughness", RMSSurfaceRoughnessChbk.IsChecked.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("Meansurfaceroughness", MeansurfaceroughnessChbk.IsChecked.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("Maxpeakheight", MaxpeakheightChbk.IsChecked.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("Maxvalleyheight", MaxvalleyheightChbk.IsChecked.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("Peaktovalleyheight", PeaktovalleyheightChbk.IsChecked.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("skewness", skewnessChbk.IsChecked.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("kurtosis", kurtosisChbk.IsChecked.ToString()));
                    ReadWriteText.WriteFileWhenWindowClose(TextFilePath, ListOfParameters);
                    ListOfParameters.Clear();
                }
            }
            catch (Exception)
            {
            }
        }
        #endregion
    }
    class MeasurementParameters : INotifyPropertyChanged
    {
        bool _RMSSurfaceRoughness = true, _MeanSurfaceRoughness = true, _MaxPeakHeight = true, _MaxValleyHeight = true, _PeakToValleyHeight = true, _Skewness = true, _Kurtosis = true;
        public event PropertyChangedEventHandler PropertyChanged;
        private void Notify(string PropName)
        {
            if (PropName != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(PropName));
            }
        }
        public bool Sq
        {
            get
            {
                return _RMSSurfaceRoughness;
            }
            set
            {
                _RMSSurfaceRoughness = value;
                Notify("Sq");
            }
        }
        public bool Sa
        {
            get
            {
                return _MeanSurfaceRoughness;
            }
            set
            {
                _MeanSurfaceRoughness = value;
                Notify("Sa");
            }
        }
        public bool Sp
        {
            get
            {
                return _MaxPeakHeight;
            }
            set
            {
                _MaxPeakHeight = value;
                Notify("Sp");
            }
        }
        public bool Sv
        {
            get
            {
                return _MaxValleyHeight;
            }
            set
            {
                _MaxValleyHeight = value;
                Notify("Sv");
            }
        }
        public bool Sz
        {
            get
            {
                return _PeakToValleyHeight;
            }
            set
            {
                _PeakToValleyHeight = value;
                Notify("Sz");
            }
        }
        public bool Ssk
        {
            get
            {
                return _Skewness;
            }
            set
            {
                _Skewness = value;
                Notify("Ssk");
            }
        }
        public bool Sku
        {
            get
            {
                return _Kurtosis;
            }
            set
            {
                _Kurtosis = value;
                Notify("Sku");
            }
        }
      
    }
}

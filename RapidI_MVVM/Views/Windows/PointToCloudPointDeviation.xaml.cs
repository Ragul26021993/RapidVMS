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
using Rapid.IO;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for PointToCloudPointDeviation.xaml
    /// </summary>
    public partial class PointToCloudPointDeviation : Window
    {
        private static PointToCloudPointDeviation _MyInstance;
        public static PointToCloudPointDeviation MyInstance()
        {
            if (_MyInstance == null)
            {
                _MyInstance = new PointToCloudPointDeviation();
            }
            return _MyInstance;
        }

        private PointToCloudPointDeviation()
        {
            InitializeComponent();
        }
        void OkBtn_Click(object sender, RoutedEventArgs e)
        {
            InterpretTxtBxVals();
        }
        void CancelBtn_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        void InterpretTxtBxVals()
        {
            try
            {
            double angle = 0;
            int numberofpoints;
            bool ArcBestFitType = false;
            if (SkipIntervalTxtBx.Text.Length > 0)
                angle = Convert.ToDouble(SkipIntervalTxtBx.Text);
            else angle = 0;
            if ((bool)ArcBFRBtn.IsChecked)
                ArcBestFitType = true;
            if (NumberOfPtsToBundleTxtBx.Text.Length > 0)
                numberofpoints = Convert.ToInt32(NumberOfPtsToBundleTxtBx.Text);
            else
                numberofpoints = 0;
            RWrapper.RW_MainInterface.MYINSTANCE().PointToCloudPointsMeasurement(angle, ArcBestFitType, numberofpoints);

            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:P2CPD82", ex); }
        }

        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            SaveParameters();
            _MyInstance = null;
        }

        #region To save the Windows Parameters
        public string TextFilePath { get; set; }
        public void ReadParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\PointToCloudPointDeviation.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("SkipInterval", SkipIntervalTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("NumberOfPtsToBundle", NumberOfPtsToBundleTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("ArcBF", ArcBFRBtn.IsChecked.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("LineBF", LineBFRBtn.IsChecked.ToString()));

                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "SkipInterval")
                                {
                                    SkipIntervalTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "NumberOfPtsToBundle")
                                {
                                    NumberOfPtsToBundleTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "ArcBF")
                                {
                                    ArcBFRBtn.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "LineBF")
                                {
                                    LineBFRBtn.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\PointToCloudPointDeviation.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("SkipInterval", SkipIntervalTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("NumberOfPtsToBundle", NumberOfPtsToBundleTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("ArcBF", ArcBFRBtn.IsChecked.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("LineBF", LineBFRBtn.IsChecked.ToString()));
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
}

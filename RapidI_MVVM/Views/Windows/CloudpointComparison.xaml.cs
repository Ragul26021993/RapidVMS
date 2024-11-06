using System;
using System.Collections.Generic;
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
using Rapid.Utilities;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for CalibrationWin.xaml
    /// </summary>
    public partial class CloudpointComparison : Window
    {
        public CloudpointComparison()
        {
            this.InitializeComponent();
            ReadParameters();
            // Insert code required on object creation below this point.
            ClosedLoopChkBx.Checked += new RoutedEventHandler(ClosedLoopChkBx_Checked);
            ClosedLoopChkBx.Unchecked += new RoutedEventHandler(ClosedLoopChkBx_Unchecked);
        }

        void OkBtn_Click(object sender, RoutedEventArgs e)
        {
            InterpretTxtBxVals();
        }
        void CancelBtn_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
        void ClosedLoopChkBx_Checked(object sender, RoutedEventArgs e)
        {
            SkipIntervalLbl.Text = "Measurement Count";
            SkipIntervalTxtBx.Visibility = System.Windows.Visibility.Hidden;
            MeasurementCountTxtBx.Visibility = System.Windows.Visibility.Visible;
        }
        void ClosedLoopChkBx_Unchecked(object sender, RoutedEventArgs e)
        {
            SkipIntervalLbl.Text = "Skip Interval";
            SkipIntervalTxtBx.Visibility = System.Windows.Visibility.Visible;
            MeasurementCountTxtBx.Visibility = System.Windows.Visibility.Hidden;
        }

        void InterpretTxtBxVals()
        {
            double interval = 0;
            int MeasurementCount, numberofpoints;
            bool ArcBestFitType = false;
            if (SkipIntervalTxtBx.Text.Length > 0)
                interval = Convert.ToDouble(SkipIntervalTxtBx.Text);
            else interval = 0;
            if (MeasurementCountTxtBx.Text.Length > 0)
                MeasurementCount = Convert.ToInt32(MeasurementCountTxtBx.Text);
            else MeasurementCount = 0;
            if ((bool)ArcBFRBtn.IsChecked)
                ArcBestFitType = true;
            if (NumberOfPtsToBundleTxtBx.Text.Length > 0)
                numberofpoints = Convert.ToInt32(NumberOfPtsToBundleTxtBx.Text);
            else
                numberofpoints = 0;
            if ((bool)ClosedLoopChkBx.IsChecked)
                RWrapper.RW_MainInterface.MYINSTANCE().CloudPointComparator(MeasurementCount, ArcBestFitType, numberofpoints);
            else
                RWrapper.RW_MainInterface.MYINSTANCE().CloudPointComparator(interval, ArcBestFitType, numberofpoints);
        }

        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            SaveParameters();
        }

        #region To save the Windows Parameters
        public string TextFilePath { get; set; }
        public void ReadParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\CloudpointComparison.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("SkipInterval", SkipIntervalTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("NumberOfPtsToBundle", NumberOfPtsToBundleTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("ClosedLoop", ClosedLoopChkBx.IsChecked.ToString()));
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
                                else if (ListOfParameters[i].Name == "ClosedLoop")
                                {
                                    ClosedLoopChkBx.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\CloudpointComparison.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("SkipInterval", SkipIntervalTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("NumberOfPtsToBundle", NumberOfPtsToBundleTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("ClosedLoop", ClosedLoopChkBx.IsChecked.ToString()));
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
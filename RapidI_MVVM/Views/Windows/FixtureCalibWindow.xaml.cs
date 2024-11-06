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
using Rapid.Utilities;
using Rapid.IO;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class FixtureCalibWindow : Window
    {
        private bool ReferencePtTaken;

        public FixtureCalibWindow()
        {
            this.InitializeComponent();
            ReadParameters();
            // Insert code required on object creation below this point.
            init();
        }

        void init()
        {
            try
            {
                FixtureListCBx.ItemsSource = GlobalSettings.FixtureNameList;
                FixtureListCBx.SelectionChanged += new SelectionChangedEventHandler(FixtureListCBx_SelectionChanged);
                CalibrateAndGoFwdBtn.Click += new RoutedEventHandler(CalibrateAndGoFwdBtn_Click);
                BtnFinishRecording.Click += new RoutedEventHandler(BtnFinishRecording_Click);
                BtnStartRecording.Click += new RoutedEventHandler(BtnStartRecording_Click);
                StartCalibrationBtn.Click += new RoutedEventHandler(StartCalibrationBtn_Click);
                this.TakeReferenceLineBtn.Click += new RoutedEventHandler(TakeReferenceLineBtn_Click);
                this.ReOrientFixtureBtn.Click += new RoutedEventHandler(ReOrientFixtureBtn_Click);
                RWrapper.RW_FixtureCalibration.MYINSTANCE().Fixture_RferencePointEvent += new RWrapper.RW_FixtureCalibration.Fixture_ReferencePointTakenEventHandler(FixtureCalibWindow_Fixture_RferencePointEvent);
                FixtureListCBx.SelectedIndex = -1;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FCALIB01", ex);
            }
        }

        void FixtureListCBx_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            string selectedFixture = FixtureListCBx.SelectedItem.ToString();
            int index = GlobalSettings.FixtureNameList.IndexOf(selectedFixture);
            RWrapper.RW_FixtureCalibration.MYINSTANCE().UpdateFixureDetails_Frontend(selectedFixture, 
                int.Parse(GlobalSettings.FixRowNum[index]), int.Parse(GlobalSettings.FixColNum[index]), 
                double.Parse(GlobalSettings.FixRowGap[index]), double.Parse(GlobalSettings.FixColGap[index]));
            //RWrapper.RW_FixtureCalibration.MYINSTANCE().getFixtureName();
        }

        void CalibrateAndGoFwdBtn_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_FixtureCalibration.MYINSTANCE().StartFixtureCallibration();
        }

        void BtnFinishRecording_Click(object sender, RoutedEventArgs e)
        {
            //Choose the first position reference point
            //if (!RecordStarted) return;
            RWrapper.RW_FixtureCalibration.MYINSTANCE().TakeReferencePoint_FixtureCallibration();
            ReferencePtTaken = true;
            //Grid.SetColumnSpan(CalibProgressBdr, 2);
        }

        void BtnStartRecording_Click(object sender, RoutedEventArgs e)
        {
            //Switch on Auto Calibration mode
            RWrapper.RW_FixtureCalibration.MYINSTANCE().SetFixtureCallibrationMode();
            bdRecordingStarted.Visibility = System.Windows.Visibility.Visible;
        }

        void StartCalibrationBtn_Click(object sender, RoutedEventArgs e)
        {
            if (!ReferencePtTaken) return;
            RWrapper.RW_FixtureCalibration.MYINSTANCE().StartAutomatedFixtureCallibration(true);
            //Grid.SetColumnSpan(CalibProgressBdr, 3);
        }

        void FixtureCalibWindow_Fixture_RferencePointEvent(int param)
        {
            if (param == 0)
                CalibProgressBdr2.Visibility = System.Windows.Visibility.Hidden;
            else if (param == 1)
                CalibProgressBdr3.Visibility = System.Windows.Visibility.Visible;
            else if (param == 2)
                CalibProgressBdr4.Visibility = System.Windows.Visibility.Visible;
        }

        protected void TakeReferenceLineBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                CalibProgressBdr3.Visibility = System.Windows.Visibility.Hidden;
                //RWrapper.RW_FixtureCalibration.MYINSTANCE().SetFixtureOrientation(false);                           
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FixCP05", ex);
            }
        }

        protected void ReOrientFixtureBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                CalibProgressBdr4.Visibility = System.Windows.Visibility.Hidden;
                //RWrapper.RW_FixtureCalibration.MYINSTANCE().SetFixtureOrientation(true);
            }
            catch (Exception Ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FixCP06", Ex);
            }
        }

        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            try
            {
                RWrapper.RW_FixtureCalibration.Close_CallibrationWindow();
                SaveParameters();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FixCP07", ex);
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\FixtureCalibWindow.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("FixtureList", FixtureListCBx.Text.ToString()));

                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "Radius1")
                                {
                                    FixtureListCBx.Text = ListOfParameters[i].Value.ToString();
                                }
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FixCP08", ex);
            }
        }

        public void SaveParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\FixtureCalibWindow.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("FixtureList", FixtureListCBx.Text.ToString()));
                    ReadWriteText.WriteFileWhenWindowClose(TextFilePath, ListOfParameters);
                    ListOfParameters.Clear();
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FixCP09", ex);
            }
        }
        #endregion
    }
}

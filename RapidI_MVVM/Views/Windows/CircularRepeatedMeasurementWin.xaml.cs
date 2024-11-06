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
    public partial class CircularRepeatedMeasurementWin : Window
    {
        private bool AutomatedMode = false;

        public CircularRepeatedMeasurementWin()
        {
            this.InitializeComponent();
            // Insert code required on object creation below this point.
            ReadParameters();
            RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32);
            RunPauseBtn.ToolTip = "Run";
            RunPauseBtn.Tag = "Run";
            RunPauseBtn.Click += new RoutedEventHandler(RunPauseBtn_Click);

            StopBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Stop", "Part Program", 32, 32);
            StopBtn.ToolTip = "Stop";
            StopBtn.Tag = "Stop";
            StopBtn.Click += new RoutedEventHandler(StopBtn_Click);

            PtOfRotationTBtn.Click += new RoutedEventHandler(PtOfRotationTBtn_Click);
            AutomatedModeBtn.Click += new RoutedEventHandler(AutomatedModeBtn_Click);
            this.Closed += new EventHandler(CircularRepeatedMeasurementWin_Closed);

            RWrapper.RW_RepeatAutoMeasure.MYINSTANCE().AutoMeasureEvent += new RWrapper.RW_RepeatAutoMeasure.RepeatAutoMeasureEventHandler(CircularRepeatedMeasurementWin_AutoMeasureEvent);
        }
        void RunPauseBtn_Click(object sender, RoutedEventArgs e)
        {
            string BtnTag = RunPauseBtn.Tag as string;
            if (BtnTag == "Run")
            {
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Pause", "Pause", 32, 32);
                RunPauseBtn.ToolTip = "Pause";
                RunPauseBtn.Tag = "Pause";
                RWrapper.RW_RepeatAutoMeasure.MYINSTANCE().StartAutoMeasurement(Convert.ToDouble(AngularIncrementTxtBx.Text) * Math.PI / 180,
                    Convert.ToInt32(NoOfStepsTxtBx.Text),
                    (bool)AutoFocusChkBx.IsChecked);
            }
            else if (BtnTag == "Continue")
            {
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Pause", "Pause", 32, 32);
                RunPauseBtn.ToolTip = "Pause";
                RunPauseBtn.Tag = "Pause";
                RWrapper.RW_RepeatAutoMeasure.MYINSTANCE().Continue_PauseAutoMeasure(false);
            }
            else if (BtnTag == "Pause")
            {
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Continue", "Continue", 32, 32);
                RunPauseBtn.ToolTip = "Continue";
                RunPauseBtn.Tag = "Continue";
                RWrapper.RW_RepeatAutoMeasure.MYINSTANCE().Continue_PauseAutoMeasure(true);
            }
        }
        void StopBtn_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_RepeatAutoMeasure.MYINSTANCE().StopAutoMeasure();
            RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32);
            RunPauseBtn.ToolTip = "Run";
            RunPauseBtn.Tag = "Run";
            PtOfRotationTBtn.IsChecked = false;
        }
        void PtOfRotationTBtn_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_RepeatAutoMeasure.MYINSTANCE().takePointOfRotation((bool)PtOfRotationTBtn.IsChecked);
            if ((bool)PtOfRotationTBtn.IsChecked)
            {
                PtOfRotationTakenBdr.Visibility = System.Windows.Visibility.Hidden;
            }
        }
        void AutomatedModeBtn_Click(object sender, RoutedEventArgs e)
        {
            if (AutomatedMode)
            {
                AutomatedMode = false;
                RWrapper.RW_RepeatAutoMeasure.MYINSTANCE().SetAutomatedMode(false);
                AutomatedModeBtn.Content = "Switch on Automated Mode";
                AutomatedModeOnBdr.Visibility = System.Windows.Visibility.Hidden;
            }
            else
            {
                AutomatedMode = true;
                RWrapper.RW_RepeatAutoMeasure.MYINSTANCE().SetAutomatedMode(true);
                AutomatedModeBtn.Content = "Switch off Automated Mode";
                AutomatedModeOnBdr.Visibility = System.Windows.Visibility.Visible;
            }
        }
        void CircularRepeatedMeasurementWin_Closed(object sender, EventArgs e)
        {
            RWrapper.RW_RepeatAutoMeasure.Close_AutoMeasureWindow();
            SaveParameters();
        }

        void CircularRepeatedMeasurementWin_AutoMeasureEvent(int EventType)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    switch (EventType)
                    {
                        case 0:
                            AutomatedMode = false;
                            AutomatedModeBtn.Content = "Switch on Automated Mode";
                            AutomatedModeOnBdr.Visibility = System.Windows.Visibility.Hidden;
                            break;
                        case 1:
                            PtOfRotationTakenBdr.Visibility = System.Windows.Visibility.Hidden;
                            PtOfRotationTBtn.IsChecked = false;
                            break;
                        case 2:
                            PtOfRotationTakenBdr.Visibility = System.Windows.Visibility.Visible;
                            PtOfRotationTBtn.IsChecked = false;
                            break;
                        case 3:
                            StopBtn_Click(null, null);
                            break;
                    }
                }
                else
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_RepeatAutoMeasure.RepeatAutoMeasureEventHandler(CircularRepeatedMeasurementWin_AutoMeasureEvent), EventType);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CRM01", ex);
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\CircularRepeatedMeasurementWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("AngularIncrement", AngularIncrementTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("NoOfSteps", NoOfStepsTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("AutoFocus", AutoFocusChkBx.IsChecked.ToString()));

                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "AngularIncrement")
                                {
                                    AngularIncrementTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "NoOfSteps")
                                {
                                    NoOfStepsTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "NoOfSteps")
                                {
                                    AutoFocusChkBx.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\CircularRepeatedMeasurementWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("AngularIncrement", AngularIncrementTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("NoOfSteps", NoOfStepsTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("AutoFocus", AutoFocusChkBx.IsChecked.ToString()));
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
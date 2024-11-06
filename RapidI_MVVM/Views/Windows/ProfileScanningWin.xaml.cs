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
using Rapid.Interface;
using Rapid.IO;

namespace Rapid.Windows
{
	/// <summary>
	/// Interaction logic for CalibrationWin.xaml
	/// </summary>
	public partial class ProfileScanningWin : Window
	{
        public ProfileScanningWin()
		{
			this.InitializeComponent();
            ReadParameters();
			// Insert code required on object creation below this point.
            RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32);
            RunPauseBtn.ToolTip = "Run";
            RunPauseBtn.Tag = "Run";
            RunPauseBtn.Click += new RoutedEventHandler(RunPauseBtn_Click);
            
            StopBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Stop", "Part Program", 32, 32);
            StopBtn.ToolTip = "Stop";
            StopBtn.Tag = "Stop";
            StopBtn.Click += new RoutedEventHandler(StopBtn_Click);

            PickFinishPtBtn.Click += new RoutedEventHandler(PickFinishPtBtn_Click);
            RWrapper.RW_MainInterface.MYINSTANCE().ProfileScanEndPtEvent += new RWrapper.RW_MainInterface.ProfileScanEndPtsEventHandler(ProfileScanningWin_ProfileScanEndPtEvent);
        }
        void RunPauseBtn_Click(object sender, RoutedEventArgs e)
        {
            string BtnTag = RunPauseBtn.Tag as string;
            if (BtnTag == "Run")
            {
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Pause", "Pause", 32, 32);
                RunPauseBtn.ToolTip = "Pause";
                RunPauseBtn.Tag = "Pause";
                RWrapper.RW_MainInterface.MYINSTANCE().SetProfileScanParam(Convert.ToInt32(NoOfPtsToSkipTxtBx.Value));
            }
            else if (BtnTag == "Continue")
            {
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Pause", "Pause", 32, 32);
                RunPauseBtn.ToolTip = "Pause";
                RunPauseBtn.Tag = "Pause";
                RWrapper.RW_MainInterface.MYINSTANCE().ContinueProfileScan(Convert.ToInt32(NoOfPtsToSkipTxtBx.Value));   
            }
            else if (BtnTag == "Pause")
            {
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32); //"Continue", "Continue"
                RunPauseBtn.ToolTip = "Continue";
                RunPauseBtn.Tag = "Continue";
                RWrapper.RW_MainInterface.MYINSTANCE().SetProfileScanPause_StoppedFlag(false);
                RWrapper.RW_CNC.MYINSTANCE().Activate_ManualModeDRO();
                //RunPauseBtn.IsEnabled = false;
            }
        }
        void StopBtn_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_MainInterface.MYINSTANCE().SetProfileScanPause_StoppedFlag(true);
            RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32);
            RunPauseBtn.ToolTip = "Run";
            RunPauseBtn.Tag = "Run";
            this.Hide();
        }
        void PickFinishPtBtn_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_MainInterface.MYINSTANCE().PickProfileScanEndPoint();
        }
        void ProfileScanningWin_ProfileScanEndPtEvent(int Type)
        {
            switch (Type)
            {
                case 0:
                    PtTakenBdr.Visibility = System.Windows.Visibility.Visible;
                    break;
                case 1:
                    RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32);
                    RunPauseBtn.ToolTip = "Run";
                    RunPauseBtn.Tag = "Run";
                    RunPauseBtn.IsEnabled = true;
                    PtTakenBdr.Visibility = System.Windows.Visibility.Hidden;
                    break;
                case 2:
                    RunPauseBtn.IsEnabled = true;
                    RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Pause", "Pause", 32, 32);
                    RunPauseBtn.ToolTip = "Pause";
                    RunPauseBtn.Tag = "Pause";
                    break;
            }
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\ProfileScanningWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("NoOfPtsToSkip", NoOfPtsToSkipTxtBx.Value.ToString()));
                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "NoOfPtsToSkip")
                                {
                                    NoOfPtsToSkipTxtBx.Value = Convert.ToDouble(ListOfParameters[i].Value);
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\ProfileScanningWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("NoOfPtsToSkip", NoOfPtsToSkipTxtBx.Value.ToString()));
                    ReadWriteText.WriteFileWhenWindowClose(TextFilePath, ListOfParameters);
                    ListOfParameters = null;
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
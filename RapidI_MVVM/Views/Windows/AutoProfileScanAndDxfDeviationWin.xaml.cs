using System;
using System.Collections.Generic;
using System.Windows;
using Rapid.Interface;
using Rapid.IO;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for CalibrationWin.xaml
    /// </summary>
    public partial class AutoProfileScanAndDxfDeviationWin : Window,ISaveWindowsParameters
	{
        public AutoProfileScanAndDxfDeviationWin()
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

            NextBtn.ToolTip = "Next";
            NextBtn.Click += new RoutedEventHandler(NextBtn_Click);
            
        }
        void RunPauseBtn_Click(object sender, RoutedEventArgs e)
        {
            string BtnTag = RunPauseBtn.Tag as string;
            if (BtnTag == "Run")
            {
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Pause", "Pause", 32, 32);
                RunPauseBtn.ToolTip = "Pause";
                RunPauseBtn.Tag = "Pause";
                RWrapper.RW_MainInterface.MYINSTANCE().SetProfileScanParam_MultiFeature(Convert.ToInt32(NoOfPtsToSkipTxtBx.Text));
            }
            else if (BtnTag == "Continue")
            {
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Pause", "Pause", 32, 32);
                RunPauseBtn.ToolTip = "Pause";
                RunPauseBtn.Tag = "Pause";
                RWrapper.RW_MainInterface.MYINSTANCE().ContinueProfileScan();
            }
            else if (BtnTag == "Pause")
            {
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Continue", "Continue", 32, 32);
                RunPauseBtn.ToolTip = "Continue";
                RunPauseBtn.Tag = "Continue";
                RWrapper.RW_MainInterface.MYINSTANCE().SetProfileScanPause_StoppedFlag(false);
            }
        }
        void StopBtn_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_MainInterface.MYINSTANCE().SetProfileScanPause_StoppedFlag(true);
            RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32);
            RunPauseBtn.ToolTip = "Run";
            RunPauseBtn.Tag = "Run";
        }
        void NextBtn_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_MainInterface.MYINSTANCE().ContinueMultiFeatureProfileScan();
        }
        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            SaveParameters();
        }

        #region To save the Windows Parameters
        public string TextFilePath { get; set; }
        public void SaveParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\AutoProfileScanAndDxfDeviationWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("NoOfPtsToSkip", NoOfPtsToSkipTxtBx.Text.ToString()));                   
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\AutoProfileScanAndDxfDeviationWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("NoOfPtsToSkip", NoOfPtsToSkipTxtBx.Text.ToString())); 
                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "NoOfPtsToSkip")
                                {
                                    NoOfPtsToSkipTxtBx.Text = ListOfParameters[i].Value.ToString();
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
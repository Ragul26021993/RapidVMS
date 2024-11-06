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

namespace Rapid.Windows
{
	/// <summary>
	/// Interaction logic for CalibrationWin.xaml
	/// </summary>
	public partial class CylinderScanningWithProbeWin : Window
	{
        private static CylinderScanningWithProbeWin _MyInstance;
        public static CylinderScanningWithProbeWin GetInstance()
        {
            if (_MyInstance == null)
                _MyInstance = new CylinderScanningWithProbeWin();
            return _MyInstance;
        }

        private CylinderScanningWithProbeWin()
		{
			this.InitializeComponent();
			// Insert code required on object creation below this point.
            RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Pause", "Pause", 32, 32);
            RunPauseBtn.ToolTip = "Pause";
            RunPauseBtn.Tag = "Pause";
            RunPauseBtn.Click += new RoutedEventHandler(RunPauseBtn_Click);
            
            StopBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Stop", "Part Program", 32, 32);
            StopBtn.ToolTip = "Stop";
            StopBtn.Tag = "Stop";
            StopBtn.Click += new RoutedEventHandler(StopBtn_Click);

            ShowDeviationBtn.Click += new RoutedEventHandler(ShowDeviationBtn_Click);
        }
        void RunPauseBtn_Click(object sender, RoutedEventArgs e)
        {
            string BtnTag = RunPauseBtn.Tag as string;
            if (BtnTag == "Continue")
            {
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Pause", "Pause", 32, 32);
                RunPauseBtn.ToolTip = "Pause";
                RunPauseBtn.Tag = "Pause";
                RWrapper.RW_MainInterface.MYINSTANCE().Continue_PauseProbeScanning(true);
            }
            else if (BtnTag == "Pause")
            {
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Continue", "Continue", 32, 32);
                RunPauseBtn.ToolTip = "Continue";
                RunPauseBtn.Tag = "Continue";
                RWrapper.RW_MainInterface.MYINSTANCE().Continue_PauseProbeScanning(false);
            }
        }
        void StopBtn_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_MainInterface.MYINSTANCE().AbortProbeScanningProcess();
        }
        void ShowDeviationBtn_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_MainInterface.MYINSTANCE().ShowDeviation_ProbeScanPts();
        }

        public event RoutedEventHandler WinClosing;
        private void RaiseEvent()
        {
            if (WinClosing != null)
                WinClosing(this, null);
        }
        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            RaiseEvent();
            this.Hide();
            e.Cancel = true;
        }
	}
}
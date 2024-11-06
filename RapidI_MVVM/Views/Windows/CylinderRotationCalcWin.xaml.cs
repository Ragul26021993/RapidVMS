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
	public partial class CylinderRotationCalcWin : Window
	{
        private static CylinderRotationCalcWin _MyInstance = null;
        public static CylinderRotationCalcWin MyInstance()
        {
            if (_MyInstance == null)
                _MyInstance = new CylinderRotationCalcWin();
            return _MyInstance;
        }

        private bool ScanStarted = false;
        private CylinderRotationCalcWin()
		{
			this.InitializeComponent();
			// Insert code required on object creation below this point.
            RunStopBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32);
            RunStopBtn.ToolTip = "Run";
            RunStopBtn.Tag = "Run";
            RunStopBtn.Click += new RoutedEventHandler(RunStopBtn_Click);

            this.Closed += new EventHandler(CylinderRotationCalcWin_Closed);
        }
        void RunStopBtn_Click(object sender, RoutedEventArgs e)
        {
            if (!ScanStarted)
            {
                ScanStarted = true;
                RunStopBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Stop", "Part Program", 32, 32);
                RunStopBtn.ToolTip = "Stop";
                RunStopBtn.Tag = "Stop";
                RWrapper.RW_MainInterface.MYINSTANCE().Start_StopRotationalScan(true);
            }
            else
            {
                RunStopBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32);
                RunStopBtn.ToolTip = "Run";
                RunStopBtn.Tag = "Run";
                RWrapper.RW_MainInterface.MYINSTANCE().Start_StopRotationalScan(false);
            }
        }
        void CylinderRotationCalcWin_Closed(object sender, EventArgs e)
        {
            if (ScanStarted)
                RWrapper.RW_MainInterface.MYINSTANCE().Start_StopRotationalScan(false);
            _MyInstance = null;
        }
        public void StartScan()
        {
            ScanStarted = true;
            RunStopBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Stop", "Part Program", 32, 32);
            RunStopBtn.ToolTip = "Stop";
            RunStopBtn.Tag = "Stop";
            RWrapper.RW_MainInterface.MYINSTANCE().Start_StopRotationalScan(true);
        }
    }
}
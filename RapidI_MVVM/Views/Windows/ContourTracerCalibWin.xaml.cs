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
	public partial class ContourTracerCalibWin : Window
	{
        public ContourTracerCalibWin()
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

            this.Closed += new EventHandler(ContourTracerCalibWin_Closed);
            RWrapper.RW_CT_SphereCallibration.MYINSTANCE().UpdateCallibrationValues += new RWrapper.RW_CT_SphereCallibration.UpdateCallibrationValuesEventHandler(ContourTracerCalibWin_UpdateCallibrationValues);
        }
        void RunPauseBtn_Click(object sender, RoutedEventArgs e)
        {
            string BtnTag = RunPauseBtn.Tag as string;
            if (BtnTag == "Run")
            {
                double[] RVals = { double.Parse(Radius1TxtBx.Text), double.Parse(Radius2TxtBx.Text), double.Parse(Radius3TxtBx.Text) };
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Pause", "Pause", 32, 32);
                RunPauseBtn.ToolTip = "Pause";
                RunPauseBtn.Tag = "Pause";
                RWrapper.RW_CT_SphereCallibration.MYINSTANCE().StartSphereCallibration(RVals, double.Parse(SphereRadiusTxtBx.Text));
            }
            else if (BtnTag == "Continue")
            {
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Pause", "Pause", 32, 32);
                RunPauseBtn.ToolTip = "Pause";
                RunPauseBtn.Tag = "Pause";
                RWrapper.RW_CT_SphereCallibration.MYINSTANCE().Continue_Pause(true);
            }
            else if (BtnTag == "Pause")
            {
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Continue", "Continue", 32, 32);
                RunPauseBtn.ToolTip = "Continue";
                RunPauseBtn.Tag = "Continue";
                RWrapper.RW_CT_SphereCallibration.MYINSTANCE().Continue_Pause(false);
            }
        }
        void StopBtn_Click(object sender, RoutedEventArgs e)
        {
            RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32);
            RunPauseBtn.ToolTip = "Run";
            RunPauseBtn.Tag = "Run";
        }
        void ContourTracerCalibWin_Closed(object sender, EventArgs e)
        {
            SaveParameters();
            RWrapper.RW_CT_SphereCallibration.Close_CallibrationWindow();
        }
        void ContourTracerCalibWin_UpdateCallibrationValues()
        {
            //RWrapper.RW_CT_SphereCallibration.MYINSTANCE().NormalVector[0]
        }

        #region To save the Windows Parameters
        public string TextFilePath { get; set; }
        public void ReadParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\ContourTracerCalibWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("Radius1", Radius1TxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("Radius2", Radius2TxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("Radius3", Radius3TxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("SphereRadius", SphereRadiusTxtBx.Text.ToString()));

                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "Radius1")
                                {
                                    Radius1TxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "Radius2")
                                {
                                    Radius2TxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "Radius3")
                                {
                                    Radius3TxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "SphereRadius")
                                {
                                    SphereRadiusTxtBx.Text = ListOfParameters[i].Value.ToString();
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\ContourTracerCalibWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("Radius1", Radius1TxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("Radius2", Radius2TxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("Radius3", Radius3TxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("SphereRadius", SphereRadiusTxtBx.Text.ToString()));
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
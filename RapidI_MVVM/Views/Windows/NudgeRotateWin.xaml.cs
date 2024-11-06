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
using Rapid.Interface;

namespace Rapid.Windows
{
	/// <summary>
	/// Interaction logic for ShapeInfoWin.xaml
	/// </summary>
	public partial class PPNudgeRotateWin : Window
	{
        public PPNudgeRotateWin()
		{
			this.InitializeComponent();

            ReadParameters();
			// Insert code required on object creation below this point.
            NudgeRBtn.Checked += new RoutedEventHandler(NudgeRBtn_Checked);
            RotateRBtn.Checked += new RoutedEventHandler(RotateRBtn_Checked);
            ContinuePPBtn.Click += new RoutedEventHandler(ContinuePPBtn_Click);
		}

        void NudgeRBtn_Checked(object sender, RoutedEventArgs e)
        {
            double StepValue = 0;
            RWrapper.RW_MainInterface.MYINSTANCE().SetNudge_RotateForRCad(true, false);
            txtUnitType.Text = "µm";
            if (double.TryParse(ValTxtBx.Text, out StepValue))
                RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(StepValue, 0, 0);
        }
        void RotateRBtn_Checked(object sender, RoutedEventArgs e)
        {
            double StepValue = 0;
            RWrapper.RW_MainInterface.MYINSTANCE().SetNudge_RotateForRCad(false, true);
            txtUnitType.Text = "min";
            if (double.TryParse(ValTxtBx.Text, out StepValue))
                RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(StepValue, 0, 0);
        }
        void ContinuePPBtn_Click(object sender, RoutedEventArgs e)
        {
            GlobalSettings.MainWin.OnContinueAfterPPNudgeRotate();
            RWrapper.RW_MainInterface.MYINSTANCE().SetNudge_RotateForRCad(false, false);
            RWrapper.RW_PartProgram.MYINSTANCE().ContinuePartProgram_AfterUserInteraction();
            this.Close();
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\PPNudgeRotateWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("Val", ValTxtBx.Text.ToString()));                        

                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "Val")
                                {
                                    ValTxtBx.Text = ListOfParameters[i].Value.ToString();
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\PPNudgeRotateWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("Val", ValTxtBx.Text.ToString()));                    
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
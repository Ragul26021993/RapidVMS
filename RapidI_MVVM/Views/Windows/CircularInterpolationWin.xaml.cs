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
using System.Linq;
using Rapid.IO;
using Rapid.Interface;

namespace Rapid.Windows
{
	/// <summary>
	/// Interaction logic for CalibrationWin.xaml
	/// </summary>
	public partial class CircularInterpolationWin : Window,ISaveWindowsParameters
	{
        public CircularInterpolationWin()
		{
			this.InitializeComponent();
			// Insert code required on object creation below this point.
            ReadParameters();
            IntermediatePtBtn.Click += new RoutedEventHandler(IntermediatePtBtn_Click);
            TargetPtBtn.Click += new RoutedEventHandler(TargetPtBtn_Click);
            GoBtn.Click += new RoutedEventHandler(GoBtn_Click);
        }

        void IntermediatePtBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_CircularInterPolation.MYINSTANCE().SetIntermediate_TargetPosition(true);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CIW01", ex);
            }
        }

        void TargetPtBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_CircularInterPolation.MYINSTANCE().SetIntermediate_TargetPosition(false);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CIW01", ex);
            }
        }

        void GoBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                double SweepAng = Convert.ToDouble(SweepAngTxtBx.Text) * Math.PI / 180;
                int NoOfHops = Convert.ToInt32(NoOfHopsTxtBx.Text);
                RWrapper.RW_CircularInterPolation.MYINSTANCE().StartCircularInterpolation(SweepAng, NoOfHops);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CIW01", ex);
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\CircularInterpolationWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("SweepAng", SweepAngTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("NoOfHops", NoOfHopsTxtBx.Text.ToString()));

                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "SweepAng")
                                {
                                    SweepAngTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "NoOfHops")
                                {
                                    NoOfHopsTxtBx.Text = ListOfParameters[i].Value.ToString();
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\CircularInterpolationWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("SweepAng", SweepAngTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("NoOfHops", NoOfHopsTxtBx.Text.ToString()));
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
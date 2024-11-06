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
	public partial class PathInterpolationWin : Window
	{
        private int NoOfPtsTaken = 0;

        public PathInterpolationWin()
		{
			this.InitializeComponent();
            ReadParameters();
			// Insert code required on object creation below this point.
            TakePtBtn.Click += new RoutedEventHandler(TakePtBtn_Click);
            AddPtsToLineBtn.Click += new RoutedEventHandler(AddPtsToLineBtn_Click);
            AddPtsToArcBtn.Click += new RoutedEventHandler(AddPtsToArcBtn_Click);
            GoBtn.Click += new RoutedEventHandler(GoBtn_Click);
            ResetBtn.Click += new RoutedEventHandler(ResetBtn_Click);
        }

        void TakePtBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (NoOfPtsTaken == 3) return;
                RWrapper.RW_CircularInterPolation.MYINSTANCE().AddPathPoint(++NoOfPtsTaken);
                NoOfPtsTakenTxtBlk.Text = NoOfPtsTaken.ToString();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:PIW01", ex);
            }
        }

        void AddPtsToLineBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (NoOfPtsTaken < 2)
                {
                    MessageBox.Show("Take a minimum of 2 points", "Rapid-I");
                    return;
                }
                RWrapper.RW_CircularInterPolation.MYINSTANCE().AddLine_CircularPath(false);
                NoOfPtsTaken = 1;
                NoOfPtsTakenTxtBlk.Text = NoOfPtsTaken.ToString();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:PIW02", ex);
            }
        }

        void AddPtsToArcBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (NoOfPtsTaken < 3)
                {
                    MessageBox.Show("Take a minimum of 3 points", "Rapid-I");
                    return;
                }
                RWrapper.RW_CircularInterPolation.MYINSTANCE().AddLine_CircularPath(true);
                NoOfPtsTaken = 1;
                NoOfPtsTakenTxtBlk.Text = NoOfPtsTaken.ToString();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:PIW03", ex);
            }
        }

        void GoBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_CircularInterPolation.MYINSTANCE().CreateInterPolationCommands((bool)DxfAsPathSourceChkBx.IsChecked, !(bool)AddPtsToSelectedCldPtsChkBx.IsChecked);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndErrorPCIW04", ex);
            }
        }

        void ResetBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_CircularInterPolation.MYINSTANCE().ResetPath();
                NoOfPtsTaken = 0;
                NoOfPtsTakenTxtBlk.Text = NoOfPtsTaken.ToString();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndErrorPCIW05", ex);
            }
        }

        public void ClearAll()
        {
            NoOfPtsTaken = 0;
            NoOfPtsTakenTxtBlk.Text = NoOfPtsTaken.ToString();
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
            SaveParameters();
            this.Hide();
            e.Cancel = true;
        }

        #region To save the Windows Parameters
        public string TextFilePath { get; set; }
        public void ReadParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\PathInterpolationWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("DxfAsPathSource", DxfAsPathSourceChkBx.IsChecked.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("AddPtsToSelectedCldPts", AddPtsToSelectedCldPtsChkBx.IsChecked.ToString()));

                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "DxfAsPathSource")
                                {
                                    DxfAsPathSourceChkBx.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "AddPtsToSelectedCldPts")
                                {
                                    AddPtsToSelectedCldPtsChkBx.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\PathInterpolationWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("DxfAsPathSource", DxfAsPathSourceChkBx.IsChecked.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("AddPtsToSelectedCldPts", AddPtsToSelectedCldPtsChkBx.IsChecked.ToString()));
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
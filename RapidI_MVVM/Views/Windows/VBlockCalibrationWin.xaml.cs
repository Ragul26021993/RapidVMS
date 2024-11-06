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
    /// Interaction logic for CalibrationWin.xaml
    /// </summary>
    public partial class VBlockCalibrationWin : Window
    {
        public VBlockCalibrationWin()
        {
            this.InitializeComponent();
            init();
        }
        void init()
        {
            try
            {
                ReadParameters();
                UpperLnBtn.Click += new RoutedEventHandler(UpperLnBtn_Click);
                DoFocusBtn.Click += new RoutedEventHandler(DoFocusBtn_Click);
                ResetAllBtn.Click += new RoutedEventHandler(ResetAllBtn_Click);
                PauseContinueBtn.Click += new RoutedEventHandler(PauseContinueBtn_Click);
                AddToolAxisBtn.Click += new RoutedEventHandler(AddToolAxisBtn_Click);

                XZeroBtn.Click += new RoutedEventHandler(GoToBtn_Click);
                YPlusBtn.Click += new RoutedEventHandler(GoToBtn_Click);
                YZeroBtn.Click += new RoutedEventHandler(GoToBtn_Click);
                YMinusBtn.Click += new RoutedEventHandler(GoToBtn_Click);
                ZPlusBtn.Click += new RoutedEventHandler(GoToBtn_Click);
                ZZeroBtn.Click += new RoutedEventHandler(GoToBtn_Click);

                RWrapper.RW_VBlockCallibration.MYINSTANCE().VblockCallibrationCompletedEvent += new RWrapper.RW_VBlockCallibration.VblockCallibrationEventHandler(Handle_VblockCallibrationCompleted);
                //UpdateUIStates();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:VBC01", ex); }
        }

        void UpdateUIStates(bool CalibDone)
        {
            try
            {
                //bool CalibDone = RWrapper.RW_VBlockCallibration.MYINSTANCE().CallibrationDoneFlag;
                if (CalibDone)
                {
                    DiaEntryLbl.Text = "Enter Diameter of Tool Cylinder";
                    AddToolAxisBtn.IsEnabled = true;
                    GoToBtnsGrid.IsEnabled = true;
                    CalibDonebdr.Visibility = System.Windows.Visibility.Visible;
                }
                else
                {
                    DiaEntryLbl.Text = "Enter Diameter of Calibration Cylinder";
                    AddToolAxisBtn.IsEnabled = false;
                    GoToBtnsGrid.IsEnabled = false;
                    CalibDonebdr.Visibility = System.Windows.Visibility.Hidden;
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:VBC02", ex); }
        }

        void Handle_VblockCallibrationCompleted(bool Status)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    UpdateUIStates(Status);
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_VBlockCallibration.VblockCallibrationEventHandler(Handle_VblockCallibrationCompleted));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:VBC03", ex); }
        }

        #region Handling Button Clicks

        void UpperLnBtn_Click(object sender, RoutedEventArgs e)
        {
            double CyDiaVal = 0;
            double.TryParse(DiaValTxtBx.Text, out CyDiaVal);
            if (CyDiaVal == 0)
            {
                MessageBox.Show("Sorry! Diameter value cannot be null or zero.", "Rapid-I");
                return;
            }
            double FixCyDia = 0, CyOffset = 0;
            if ((bool)HolderCalibrationChkBx.IsChecked)
            {
                FixCyDia = double.Parse(FixtureCyDiaTxtBx.Text);
                CyOffset = double.Parse(CyOverlapTxtBx.Text);
            }
            RWrapper.RW_VBlockCallibration.MYINSTANCE().SetCylinderRadius(CyDiaVal, (bool)HolderCalibrationChkBx.IsChecked, FixCyDia, CyOffset);
            HolderCalibrationChkBx.IsEnabled = false;
        }
        void DoFocusBtn_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_VBlockCallibration.MYINSTANCE().Get_ZvalueOfCylinderAxis();
        }
        void ResetAllBtn_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_VBlockCallibration.MYINSTANCE().ClearAll();
            //UpdateUIStates();
            HolderCalibrationChkBx.IsEnabled = true;
        }
        void AddToolAxisBtn_Click(object sender, RoutedEventArgs e)
        {
            double CyDiaVal = 0;
            double.TryParse(DiaValTxtBx.Text, out CyDiaVal);
            if (CyDiaVal == 0)
            {
                MessageBox.Show("Sorry! Diameter value cannot be null or zero.", "Rapid-I");
                return;
            }
            RWrapper.RW_VBlockCallibration.MYINSTANCE().Create_AxisLine(CyDiaVal);
        }
        void GoToBtn_Click(object sender, RoutedEventArgs e)
        {
            if (sender.Equals(YPlusBtn))
                RWrapper.RW_VBlockCallibration.MYINSTANCE().GotoCommandForVblock(0);
            else if (sender.Equals(YZeroBtn))
                RWrapper.RW_VBlockCallibration.MYINSTANCE().GotoCommandForVblock(1);
            else if (sender.Equals(YMinusBtn))
                RWrapper.RW_VBlockCallibration.MYINSTANCE().GotoCommandForVblock(2);
            else if (sender.Equals(ZPlusBtn))
                RWrapper.RW_VBlockCallibration.MYINSTANCE().GotoCommandForVblock(3);
            else if (sender.Equals(ZZeroBtn))
                RWrapper.RW_VBlockCallibration.MYINSTANCE().GotoCommandForVblock(4);
            else if (sender.Equals(XZeroBtn))
                RWrapper.RW_VBlockCallibration.MYINSTANCE().GotoCommandForVblock(5);
        }
        void PauseContinueBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Button ContPauseBtn = (sender as Button);
                string Content = ContPauseBtn.Content.ToString();
                if (Content == "Continue")
                {
                    ContPauseBtn.Content = "Pause";
                    RWrapper.RW_VBlockCallibration.MYINSTANCE().ContinuePauseVblockCallibration(false);
                }
                else
                {
                    ContPauseBtn.Content = "Continue";
                    RWrapper.RW_VBlockCallibration.MYINSTANCE().ContinuePauseVblockCallibration(true);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:VBC_PC01", ex);
            }
        }

        #endregion

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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\VBlockCalibrationWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("DiaVal", DiaValTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("HolderCalibration", HolderCalibrationChkBx.IsChecked.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("FixtureCyDia", FixtureCyDiaTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("CyOverlap", CyOverlapTxtBx.Text.ToString()));

                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "DiaVal")
                                {
                                    DiaValTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "HolderCalibration")
                                {
                                    HolderCalibrationChkBx.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                if (ListOfParameters[i].Name == "FixtureCyDia")
                                {
                                    FixtureCyDiaTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "CyOverlap")
                                {
                                    CyOverlapTxtBx.Text = ListOfParameters[i].Value.ToString();
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\VBlockCalibrationWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("DiaVal", DiaValTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("HolderCalibration", HolderCalibrationChkBx.IsChecked.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("FixtureCyDia", FixtureCyDiaTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("CyOverlap", CyOverlapTxtBx.Text.ToString()));
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
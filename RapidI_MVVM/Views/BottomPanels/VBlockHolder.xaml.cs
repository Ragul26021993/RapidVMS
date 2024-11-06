using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;

namespace Rapid.Panels
{
    /// <summary>
    /// Interaction logic for VBlockHolder.xaml
    /// </summary>
    public partial class VBlockHolder : Grid
    {
        Image PauseImg, ContinueImg;
        public VBlockHolder()
        {
            InitializeComponent();
            init();
        }

        void init()
        {
            try
            {
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

                //Assign CNC Goto & back images from databases.
                List<Image> CNCImages = GlobalSettings.SettingsReader.UpdateBtnInfoList("VBlockCalibration", 48, 48);
                List<string> CNCToolTipList = GlobalSettings.SettingsReader.ButtonToolTipList;
                Button[] cncImageButtons = { UpperLnBtn, PauseContinueBtn, DoFocusBtn, AddToolAxisBtn, ResetAllBtn };
                for (int i = 0; i < cncImageButtons.Length; i++)
                {
                    cncImageButtons[i].Content = CNCImages[i];
                    cncImageButtons[i].ToolTip = CNCToolTipList[i];
                    if (cncImageButtons[i] == PauseContinueBtn)
                    {
                        PauseImg = CNCImages[i];
                    }
                }
                CNCImages[5].Width = 40;
                CNCImages[5].Height = 40;
                VBlockCaliberationRBtn.Content = CNCImages[5];
                VBlockCaliberationRBtn.ToolTip = CNCToolTipList[5];
                if (CNCImages.Count > 7)
                {
                    CNCImages[6].Width = 40;
                    CNCImages[6].Height = 40;
                    HolderCalibrationRBtn.Content = CNCImages[6];
                    HolderCalibrationRBtn.ToolTip = CNCToolTipList[6];
                    ContinueImg = CNCImages[7];
                }
                else
                {
                    ContinueImg = CNCImages[6];
                    HolderCalibrationRBtn.Visibility = Visibility.Hidden;
                }
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
                    //DiaEntryLbl.Text = "Enter Diameter of Tool Cylinder";
                    AddToolAxisBtn.IsEnabled = true;
                    GoToBtnsGrid.IsEnabled = true;
                    CalibDonebdr.Visibility = System.Windows.Visibility.Visible;
                }
                else
                {
                    //DiaEntryLbl.Text = "Enter Diameter of Calibration Cylinder";
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
            if ((bool)HolderCalibrationRBtn.IsChecked)
            {
                FixCyDia = double.Parse(FixtureCyDiaTxtBx.Text);
                CyOffset = double.Parse(CyOverlapTxtBx.Text);
            }
            RWrapper.RW_VBlockCallibration.MYINSTANCE().SetCylinderRadius(CyDiaVal, (bool)HolderCalibrationRBtn.IsChecked, FixCyDia, CyOffset);
            HolderCalibrationRBtn.IsEnabled = false;
        }
        void DoFocusBtn_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_VBlockCallibration.MYINSTANCE().Get_ZvalueOfCylinderAxis();
        }
        void ResetAllBtn_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_VBlockCallibration.MYINSTANCE().ClearAll();
            HolderCalibrationRBtn.IsEnabled = true;
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
                string Content = ContPauseBtn.Tag.ToString();
                if (Content == "Continue")
                {
                    ContPauseBtn.Content = PauseImg;
                    ContPauseBtn.Tag = "Pause";
                    ContPauseBtn.ToolTip = "Pause";
                    RWrapper.RW_VBlockCallibration.MYINSTANCE().ContinuePauseVblockCallibration(false);
                }
                else
                {
                    ContPauseBtn.Content = ContinueImg;
                    ContPauseBtn.Tag = "Continue";
                    ContPauseBtn.ToolTip = "Continue";
                    RWrapper.RW_VBlockCallibration.MYINSTANCE().ContinuePauseVblockCallibration(true);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:VBC_PC01", ex);
            }
        }

        #endregion

        private void VBlockCaliberationRBtn_Checked(object sender, RoutedEventArgs e)
        {

        }

        public void ClearAll()
        {
            AddToolAxisBtn.IsEnabled = true;
            GoToBtnsGrid.IsEnabled = true;
            HolderCalibrationRBtn.IsEnabled = true;
            VBlockCaliberationRBtn.IsChecked = true;
            CalibDonebdr.Visibility = System.Windows.Visibility.Hidden;
            DiaValTxtBx.Text = 10.ToString();
            ShankLenTxtBbx.Text = 0.ToString();
            FixtureCyDiaTxtBx.Text = 0.ToString();
            CyOverlapTxtBx.Text = 0.ToString();
        }
    }
}

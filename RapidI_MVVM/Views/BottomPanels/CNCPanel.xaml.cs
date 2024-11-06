using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Controls.Primitives;
using Rapid.Utilities;

namespace Rapid.Panels
{
    /// <summary>
    /// Interaction logic for CNCPanel.xaml
    /// </summary>
    public partial class CNCPanel : Grid
    {
        public ToggleButton CNCRBtn;

        public CNCPanel()
        {
            InitializeComponent();
            
            txtgotodroX.KeyDown += new KeyEventHandler(TxtBx_KeyDown);
            txtgotodroY.KeyDown += new KeyEventHandler(TxtBx_KeyDown);
            txtgotodroZ.KeyDown += new KeyEventHandler(TxtBx_KeyDown);
            txtgotodroR.KeyDown += new KeyEventHandler(TxtBx_KeyDown);
            txtmmX.KeyDown += new KeyEventHandler(TxtBx_KeyDown);
            txtmmY.KeyDown += new KeyEventHandler(TxtBx_KeyDown);
            txtmmZ.KeyDown += new KeyEventHandler(TxtBx_KeyDown);
            txtmmR.KeyDown += new KeyEventHandler(TxtBx_KeyDown);
            init();
        }
        private void init()
        {
            try
            {
                this.PreviewMouseDown += new MouseButtonEventHandler(CNCPanel_PreviewMouseDown);
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("GoToGrpBx", "GoToGrpBx", 32, 32);
                grpGoTo.Header = GlobalSettings.SettingsReader.ButtonToolTip;
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("MoveByGrpBx", "MoveByGrpBx", 32, 32);
                grpMoveBy.Header = GlobalSettings.SettingsReader.ButtonToolTip;
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("SpeedsGrpBx", "SpeedsGrpBx", 32, 32);
                grpFeedrates.Header = GlobalSettings.SettingsReader.ButtonToolTip;

                if (GlobalSettings.IsRAxisPresent || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI) ChangeForRAxis();
                Button[] cncButtons = { btnDroBack, btnDroGoto, btnmmBack, btnmmGoto };
                for (int i = 0; i < cncButtons.Length; i++)
                {
                    cncButtons[i].Tag = new string[] { "CNC", "Tooltip", "Name" };
                    cncButtons[i].Click += new RoutedEventHandler(HandleButtonsClick);
                }
                //Assign CNC Goto & back images from databases.
                List<Image> CNCImages = GlobalSettings.SettingsReader.UpdateBtnInfoList("CNC", 48, 48);
                List<string> CNCToolTipList = GlobalSettings.SettingsReader.ButtonToolTipList;
                Button[] cncImageButtons = { btnDroGoto, btnDroBack, btnmmGoto, btnmmBack };
                for (int i = 0; i < cncImageButtons.Length; i++)
                {
                    cncImageButtons[i].Content = CNCImages[i];
                    cncImageButtons[i].ToolTip = CNCToolTipList[i];
                }

                UpdateFeedrateSettings(true);
                //Do this to get proper height & width
                Measure(new Size(double.PositiveInfinity, double.PositiveInfinity));
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CNC001", ex);
            }
        }
        void CNCPanel_PreviewMouseDown(object sender, RoutedEventArgs e)
        {
            if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
            {
                if (!(bool)GlobalSettings.FeatureLicenseInfo[6])
                {
                    MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                    e.Handled = true;
                }
            }
        }

        private void HandleButtonsClick(object sender, RoutedEventArgs e)
        {
            try
            {
                ContentControl b = (ContentControl)sender;
                string[] tagStr = (string[])b.Tag;
                string[] CncGoTotxts = { txtgotodroX.Text, txtgotodroY.Text, txtgotodroZ.Text, txtgotodroR.Text };
                string[] CncMoveBytxts = { txtmmX.Text, txtmmY.Text, txtmmZ.Text, txtmmR.Text };
                double[] Cncfeeds = {txtFeedX.Value,txtFeedY.Value,txtFeedZ.Value,
                            txtFeedR.Value,txtFeedR.Value};
                if (b.Equals(btnDroGoto))
                {
                    for (int i = 0; i < CncGoTotxts.Length; i++)
                        if (CncGoTotxts[i] == "")
                            CncGoTotxts[i] = "0";
                    if (!RWrapper.RW_CNC.MYINSTANCE().getCNCMode())
                        RWrapper.RW_CNC.MYINSTANCE().Activate_CNCModeDRO();
           
                    RWrapper.RW_CNC.MYINSTANCE().GotoPosition(CncGoTotxts);
                    
                }
                else if (b.Equals(btnmmGoto))
                {
                    for (int i = 0; i < CncMoveBytxts.Length; i++)
                        if (CncMoveBytxts[i] == "")
                            CncMoveBytxts[i] = "0";
                    if (!RWrapper.RW_CNC.MYINSTANCE().getCNCMode())
                        RWrapper.RW_CNC.MYINSTANCE().Activate_CNCModeDRO(); 
                    RWrapper.RW_CNC.MYINSTANCE().MovetoPosition(CncMoveBytxts);
                }
                else if (b.Equals(btnDroBack))
                    RWrapper.RW_CNC.MYINSTANCE().GoBackToOldPosition();
                else if (b.Equals(btnmmBack))
                    RWrapper.RW_CNC.MYINSTANCE().GoBackToOldPosition();
                CncGoTotxts = null; CncMoveBytxts = null; Cncfeeds = null;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CNC002", ex);
            }
        }
        void HandleCNCFeedrate_ValueChanged(object sender, RoutedEventArgs e)
        {
            try
            {
                double[] Cncfeeds = {txtFeedX.Value,txtFeedY.Value,txtFeedZ.Value,
                                     txtFeedR.Value};
                RWrapper.RW_CNC.MYINSTANCE().SetCNCFeedRate(Cncfeeds);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CNC003", ex);
            }
        }
        
        public void ChangeForRAxis()
        {
            try
            {
                txtgotodroR.Visibility = System.Windows.Visibility.Visible;
                txtmmR.Visibility = System.Windows.Visibility.Visible;
                txtFeedR.Visibility = System.Windows.Visibility.Visible;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CNC004", ex);
            }
        }

        void TxtBx_KeyDown(Object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Delete)
                e.Handled = true;
        }
        public void ClickGoTo()
        {
            HandleButtonsClick(btnDroGoto, null);
        }

        public void UpdateFeedrateSettings(bool AddDelegate)
        {
            //Now CNC comboboxes should have items in double from 1.0 to 20.0.
            //Hence following for loop does that.
            NumericUpDown[] cbs = { txtFeedX, txtFeedY, txtFeedZ, txtFeedR };
            double[] CNCFeedRates = { GlobalSettings.DROSpeeds[0], GlobalSettings.DROSpeeds[1], GlobalSettings.DROSpeeds[2], GlobalSettings.DROSpeeds[3] };

            for (int a = 0; a < cbs.Length; a++)
            {
                cbs[a].StepValue = GlobalSettings.SpeedStepVal;
                cbs[a].Minimum = GlobalSettings.SpeedMinVal;
                cbs[a].Maximum = GlobalSettings.SpeedMaxVal;
                cbs[a].Value = CNCFeedRates[a];
                if (AddDelegate)
                    cbs[a].ValueChanged += new RoutedEventHandler(HandleCNCFeedrate_ValueChanged);
            }
        }
    }
}

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
using CalibrationModule;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for CalibrationWin.xaml
    /// </summary>
    public partial class CalibrationWin : Window, ISaveWindowsParameters
    {
        ProgressBar currentProgressBar; 
        public CalibrationWin()
        {
            this.InitializeComponent();
            ReadParameters();
            // Insert code required on object creation below this point.
            init();
        }
        private void init()
        {
            try
            {
                ShwBothLocalNNormalCorrChkBx.Checked += new RoutedEventHandler(ShwBothLocalNNormalCorrChkBx_Checked);
                ShwBothLocalNNormalCorrChkBx.Unchecked += new RoutedEventHandler(ShwBothLocalNNormalCorrChkBx_Unchecked);
                ShiftTxtBx.Text = "1";
                MedianRBtn.IsChecked = true;
                MedianRBtn.Checked += new RoutedEventHandler(MedianOrAvgRBtn_Checked);
                AvgRBtn.Checked += new RoutedEventHandler(MedianOrAvgRBtn_Checked);
                ShiftTxtBx.TextChanged += new TextChangedEventHandler(ShiftTxtBx_TextChanged);
                RWrapper.RW_MainInterface.MYINSTANCE().ClassControl(RWrapper.ClassNames.AutoCalibration, true);


                Button[] caliButtons = { btnHome, btnResetDRO, btnRun, btnMeasureWidth, btnCalibrate, btnSlipGWidth, btnDone, btnClear, PlcSGWidthInfoBtn, ShwSGWidthInfoTableBtn, GoToLastPositionBtn };
                for (int i = 0; i < caliButtons.Length; i++)
                    caliButtons[i].Click += new RoutedEventHandler(Handle_CalibrationButtonsClick);
                //populate the machine types & slipgauge widths for Calibration window
                for (int i = 0; i < RWrapper.RW_AutoCalibration.MYINSTANCE().MachineTypeColl.Count; i++)
                    cbMachineType.Items.Add(RWrapper.RW_AutoCalibration.MYINSTANCE().MachineTypeColl[i]);
                for (int i = 0; i < RWrapper.RW_AutoCalibration.MYINSTANCE().SlipGaugeTypeColl.Count; i++)
                    cbSlipGaugeWidth.Items.Add(RWrapper.RW_AutoCalibration.MYINSTANCE().SlipGaugeTypeColl[i]);
                cbMachineType.SelectionChanged += new SelectionChangedEventHandler(CalibrationWindow_SelectionChanged);
                cbSlipGaugeWidth.SelectionChanged += new SelectionChangedEventHandler(CalibrationWindow_SelectionChanged);

                LBl_Params = new double[6];
                LB_Positions = new double[6];
                //RWrapper.RW_MainInterface.MYINSTANCE().ClassControl(RWrapper.ClassNames.L_Block, true);

                //RWrapper.RW_LBlock.MYINSTANCE().LBlockStepCompleted += new RWrapper.RW_LBlock.CompletedthisStep(LBlock_StepEventsHandler);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CalibWin001", ex);
            }
        }
        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            RWrapper.RW_AutoCalibration.MYINSTANCE().Handle_CloseClicked();
            SaveParameters();
            //RWrapper.RW_MainInterface.MYINSTANCE().ClassControl(RWrapper.ClassNames.AutoCalibration, false);
            //RWrapper.RW_MainInterface.MYINSTANCE().ClassControl(RWrapper.ClassNames.L_Block, false);

            //e.Cancel = true;
            //this.Hide();
        }

        #region Slip Gauge
        private void ShwBothLocalNNormalCorrChkBx_Checked(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_AutoCalibration.MYINSTANCE().Handle_CheckBoxChanged(true);
        }
        private void ShwBothLocalNNormalCorrChkBx_Unchecked(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_AutoCalibration.MYINSTANCE().Handle_CheckBoxChanged(false);
        }
        private void MedianOrAvgRBtn_Checked(object sender, RoutedEventArgs e)
        {
            double ShiftVal = 0;
            double.TryParse(ShiftTxtBx.Text, out ShiftVal);
            if ((bool)MedianRBtn.IsChecked)
                RWrapper.RW_AutoCalibration.MYINSTANCE().SetMeasurementAsMedian(true, ShiftVal);
            else
                RWrapper.RW_AutoCalibration.MYINSTANCE().SetMeasurementAsMedian(false, ShiftVal);
        }
        private void ShiftTxtBx_TextChanged(object sender, TextChangedEventArgs e)
        {
            double ShiftVal = 0;
            double.TryParse(ShiftTxtBx.Text, out ShiftVal);
            RWrapper.RW_AutoCalibration.MYINSTANCE().SetMeasurementAsMedian((bool)MedianRBtn.IsChecked, ShiftVal);
        }
        //Handling the Calibration window buttons click events
        void Handle_CalibrationButtonsClick(object sender, RoutedEventArgs e)
        {
            try
            {
                string b = ((Button)sender).Content.ToString();
                switch (b)
                {
                    case "Home":
                        GlobalSettings.MainWin.AssignShapePanelToShapeInfoWindow(8, "Select Axis for going to home position;X;Y;Z", "CalibWinGoHomeAxisSelection", true);
                        break;
                    case "Reset DRO":
                        RWrapper.RW_AutoCalibration.MYINSTANCE().Handle_ResetDROClicked();
                        break;
                    case "Run":
                        ((Button)sender).Content = "Stop";
                        RWrapper.RW_AutoCalibration.MYINSTANCE().Handle_RunClicked();
                        break;
                    case "Stop":
                        ((Button)sender).Content = "Run";
                        RWrapper.RW_AutoCalibration.MYINSTANCE().Handle_StopClicked();
                        break;
                    case "Measure Width":
                        RWrapper.RW_AutoCalibration.MYINSTANCE().Handle_MeasureWidthClicked();
                        break;
                    case "Calibrate":
                        RWrapper.RW_AutoCalibration.MYINSTANCE().Handle_CalibrateClicked();
                        break;
                    case "SG Width":
                        {
                            //ThreadNumWin TmpCountWin = new ThreadNumWin();
                            //TmpCountWin.txtHeading.Text = "No. of times you want to repeat:";
                            int RepeatCnt = 0;
                            //TmpCountWin.Owner = this;
                            //TmpCountWin.NoOfThreadsNUpDown.Minimum = 1;
                            //TmpCountWin.NoOfThreadsNUpDown.Maximum = 30;
                            //TmpCountWin.NoOfThreadsNUpDown.Value = 1;
                            //TmpCountWin.ShowDialog();
                            //if (TmpCountWin.Result)
                            //    RepeatCnt = TmpCountWin.NumOfThreads;
                            RepeatCnt = int.Parse(txtRepeatCount.Text);
                            //RWrapper.RW_AutoCalibration.MYINSTANCE().Handle_MeasureSlipGaugeWidthClicked();
                            if (double.Parse(txtSlipLength.Text) != 0)
                                RWrapper.RW_AutoCalibration.MYINSTANCE().SlipGaugeWidth_Frame = double.Parse(txtSlipLength.Text);
                            RWrapper.RW_AutoCalibration.MYINSTANCE().DoSingleFramePerSide = (bool)chkDoSingleFrame.IsChecked;
                            RWrapper.RW_AutoCalibration.MYINSTANCE().Handle_MeasureSlipGaugeWidthOnClick(RepeatCnt);
                        }
                        break;
                    case "Update From File":
                        RWrapper.RW_AutoCalibration.MYINSTANCE().Handle_DoneClicked();
                        //this.CalibWin.Hide();
                        break;
                    case "Clear":
                        RWrapper.RW_AutoCalibration.MYINSTANCE().Handle_ClearClicked();
                        break;
                    case "Place SG Width Info":
                        {
                            CalibInfoWin TmpInfoWin = new CalibInfoWin();
                            TmpInfoWin.Owner = this;
                            TmpInfoWin.ShowDialog();
                        }
                        break;
                    case "Show SG Width Table":
                        RWrapper.RW_MainInterface.MYINSTANCE().PLaceCalibrationTable();
                        break;
                    case "Go To Last Position":
                        RWrapper.RW_AutoCalibration.MYINSTANCE().GotoLastPostion();
                        break;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CalibWin10", ex);
            }
        }
        //handling the chnage in Machine type & Slipgauge widths of calibration window
        void CalibrationWindow_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                if (e.AddedItems.Count > 0)
                {
                    if (sender.Equals(cbMachineType))
                        RWrapper.RW_AutoCalibration.MYINSTANCE().Handle_SetMachineType(cbMachineType.SelectedItem.ToString());
                    else
                        RWrapper.RW_AutoCalibration.MYINSTANCE().Handle_SetSlipGaugeWidth(cbSlipGaugeWidth.SelectedItem.ToString());
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CalibWin11", ex); }
        }

        private void CellsOverlapTxtBx_TextChanged(object sender, System.Windows.Controls.TextChangedEventArgs e)
        {
            try
            {
                double CellOverlapWidth;
                if (double.TryParse(CellsOverlapTxtBx.Text, out CellOverlapWidth))
                {
                    RWrapper.RW_AutoCalibration.MYINSTANCE().SetOverlapWidth(Convert.ToDouble(CellOverlapWidth));
                }
                else
                {
                    CellsOverlapTxtBx.Text = "5";
                    CellsOverlapTxtBx.ToolTip = "Only numeric value allowed";
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CalibWin12", ex);
            }
        }

        #region To save the Windows Parameters
        public string TextFilePath { get; set; }
        public void ReadParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\CalibrationWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("Shift", ShiftTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("CellsOverlap", CellsOverlapTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("ShwBothLocalNNormalCorr", (ShwBothLocalNNormalCorrChkBx.IsChecked).ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("Median", MedianRBtn.IsChecked.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("Avg", AvgRBtn.IsChecked.ToString()));

                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "Shift")
                                {
                                    ShiftTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "CellsOverlap")
                                {
                                    CellsOverlapTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "ShwBothLocalNNormalCorr")
                                {
                                    ShwBothLocalNNormalCorrChkBx.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "Median")
                                {
                                    MedianRBtn.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "Avg")
                                {
                                    AvgRBtn.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "MachineType")
                                {
                                    cbMachineType.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "SlipGaugeWidth")
                                {
                                    cbSlipGaugeWidth.Text = ListOfParameters[i].Value.ToString();
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\CalibrationWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("Shift", ShiftTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("CellsOverlap", CellsOverlapTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("ShwBothLocalNNormalCorr", (ShwBothLocalNNormalCorrChkBx.IsChecked).ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("Median", MedianRBtn.IsChecked.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("Avg", AvgRBtn.IsChecked.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("MachineType", cbMachineType.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("SlipGaugeWidth", cbSlipGaugeWidth.Text.ToString()));
                    ReadWriteText.WriteFileWhenWindowClose(TextFilePath, ListOfParameters);
                    ListOfParameters.Clear();
                }
            }
            catch (Exception)
            {
            }
        }
        #endregion

        #endregion

        #region L-Block Tab

        DB Settings_DataB;
        double[] LBl_Params;
        double[] LB_Positions;
        private void GetDatafromdB(object sender, EventArgs e)
        {
            if (cbLBlockType.Items.Count == 0)
            {
                string ConnStr;
                if (System.Environment.Is64BitOperatingSystem)
                    ConnStr = "Provider=Microsoft.Ace.OLEDB.12.0;Data Source=" + Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Database\\RapidSettings.mdb;Persist Security Info=False;Jet OLEDB:Database Password=cTpL_4577ri";
                else
                    ConnStr = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Database\\RapidSettings.mdb;Persist Security Info=False;Jet OLEDB:Database Password=cTpL_4577ri";

                Settings_DataB = new DB(ConnStr);
                Settings_DataB.FillTable("LBlockSettings");
                for (int i = 0; i < Settings_DataB.GetTable("LBlockSettings").Rows.Count; i++)
                {
                    cbLBlockType.Items.Add(Settings_DataB.GetTable("LBlockSettings").Rows[i]["LB_SerialNumber"]);
                }
            }
        }


        private void cbLBlock_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBox cb = (ComboBox)sender;
            System.Data.DataRow dr = Settings_DataB.GetTable("LBlockSettings").Rows[cb.SelectedIndex];
            for (int i = 0; i < 6; i++)
            {
                LBl_Params[i] = Convert.ToDouble(dr[i + 2]);
            }
        }
        private void StartLBlockClick(object sender, EventArgs e)
        {
            int SelectedAxis = 0;
            ClearBoxes();
            currentProgressBar = LB_CycleProgress;
            if ((bool)rb_Y.IsChecked) SelectedAxis = 1;
            if (cbLBlockType.SelectedIndex < 0) return;
            RWrapper.RW_LBlock.MYINSTANCE().Initialise_LBlockSettings(LBl_Params, SelectedAxis, (bool)BtnLB_Orientation.IsChecked, true);
        }

        private void StopLBlockCyle(object sender, EventArgs e)
        {
            RWrapper.RW_LBlock.MYINSTANCE().Stop_LBlock();
        }

        private void LBlock_StepEventsHandler(double ProgressVal)
        {
            currentProgressBar.Value = Convert.ToInt32(ProgressVal * 100);
            //if (ProgressVal == 1)
            //{
            //Try and see if we have taken a point. If yes, let us display in FE
            int PointsDone = RWrapper.RW_LBlock.MYINSTANCE().GetAnswer(LB_Positions);
            if (PointsDone > 0)
            {
                //Enter the value of the first point
                txt_Z1.Content = string.Format("{0:F4}", LB_Positions[0]);
                txt_A1.Content = string.Format("{0:F4}", LB_Positions[1]);
                //If we got second point as well, Let us complete the measurements...
                if (PointsDone == 2)
                {
                    txt_Z2.Content = string.Format("{0:F4}", LB_Positions[2]);
                    txt_A2.Content = string.Format("{0:F4}", LB_Positions[3]);
                    txt_ZMove.Content = string.Format("{0:F4}", Math.Abs(LB_Positions[0] - LB_Positions[2]));
                    txt_AMove.Content = string.Format("{0:F4}", Math.Abs(LB_Positions[1] - LB_Positions[3]));
                }
            }
            //}
        }

        private void ClearBoxes()
        {
            foreach (Label ll in LabelGridDisplay.Children)
            {
                ll.Content = "";
            }
        }
        int CurrentIconFlipState_X = 1;
        private void FlipIconAboutVerticalAxis(object sender, EventArgs e)
        {
            System.Windows.Controls.Primitives.ToggleButton btn = (System.Windows.Controls.Primitives.ToggleButton)sender;
            CurrentIconFlipState_X *= -1;
            ScaleTransform st = new ScaleTransform(CurrentIconFlipState_X, 1, 0.5, 0.5);
            btn.RenderTransformOrigin = new Point(0.5, 0.5);
            btn.RenderTransform = st;
        }

        #endregion

        #region Cone Tab
        int ConeIconOrientation;
        private void ConeDirectionClick(object sender, EventArgs e)
        {
            System.Windows.Controls.Primitives.ToggleButton btn = (System.Windows.Controls.Primitives.ToggleButton)sender;
            if (ConeIconOrientation == 0) 
                ConeIconOrientation = 1;
            else 
                ConeIconOrientation = 0;
            RotateTransform rt = new RotateTransform(ConeIconOrientation * 180);
            btn.RenderTransformOrigin = new Point(0.5, 0.5);
            btn.RenderTransform = rt;
        }

        private void ConeStartClick(object sender, EventArgs e)
        {
            try
            {
                currentProgressBar = Cone_CycleProgress;
                int Steps_No = (int)nup_ConeStepsNumber.Value;
                double ConeAngle;
                double Z_Distance;
                if (double.TryParse(txtConeAngle.Text, out ConeAngle))
                {
                    ConeAngle = ConeAngle * Math.PI / 180;//Convert angle into radians first
                    if(double.TryParse(txtConeLevelNumber.Text, out Z_Distance))
                        RWrapper.RW_LBlock.MYINSTANCE().CalculateConeSteps(Steps_No, Z_Distance, ConeAngle);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FE_Cone:0002", ex);
            }
        }

        #endregion

    }
}
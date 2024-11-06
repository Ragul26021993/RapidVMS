using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.ComponentModel;
using Rapid.Interface;
using Rapid.IO;

namespace Rapid.Windows
{
    public partial class AutoFocusScanWin : Window, ISaveWindowsParameters
    {
        private static AutoFocusScanWin MyInst = null;
        public static AutoFocusScanWin MyInstance()
        {
            if (MyInst == null)
                MyInst = new AutoFocusScanWin();
            return MyInst;
        }

        public bool NotifyClose = true;
        public bool WinUsedForFocusScan = true;

        private AutoFocusScanWin()
        {
            this.InitializeComponent();
            // Insert code required on object creation below this point.
            RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32);
            RunPauseBtn.ToolTip = "Run";
            RunPauseBtn.Tag = "Run";
            RunPauseBtn.Click += new RoutedEventHandler(RunPauseBtn_Click);

            StopBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Stop", "Part Program", 32, 32);
            StopBtn.ToolTip = "Stop";
            StopBtn.Tag = "Stop";
            StopBtn.Click += new RoutedEventHandler(StopBtn_Click);

            RotateSurfaceBtn.Click += new RoutedEventHandler(RotateSurfaceBtn_Click);

            RowsTxtBx.TextChanged += new TextChangedEventHandler(txtValues_TextChanged);
            ColumnsTxtBx.TextChanged += new TextChangedEventHandler(txtValues_TextChanged);
            WidthTxtBx.TextChanged += new TextChangedEventHandler(txtValues_TextChanged);
            HeightTxtBx.TextChanged += new TextChangedEventHandler(txtValues_TextChanged);
            GapTxtBx.TextChanged += new TextChangedEventHandler(txtValues_TextChanged);
            SpanTxtBx.TextChanged += new TextChangedEventHandler(txtValues_TextChanged);
            SpeedTxtBx.TextChanged += new TextChangedEventHandler(txtValues_TextChanged);

            this.Loaded += new RoutedEventHandler(AutoFocusScanWin_Loaded);
            //this.Closing += new System.ComponentModel.CancelEventHandler(AutoFocusScanWin_Closing);

        }
        void AutoFocusScanWin_Loaded(object sender, RoutedEventArgs e)
        {
            txtValues_TextChanged(null, null);
            if (WinUsedForFocusScan)
                this.Title = "Auto Focus Scan";
            else
                this.Title = "Auto Contour Scan";
            ReadParameters();
        }
        void RunPauseBtn_Click(object sender, RoutedEventArgs e)
        {
            string BtnTag = RunPauseBtn.Tag as string;
            if (BtnTag == "Run")
            {
                if (WinUsedForFocusScan)
                {
                    if (!RWrapper.RW_FocusDepth.MYINSTANCE().Start_AutoFocusScan())
                        return;
                }
                else
                {
                    if (!RWrapper.RW_FocusDepth.MYINSTANCE().Start_AutoContourScan())
                        return;
                }
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Pause", "Pause", 32, 32);
                RunPauseBtn.ToolTip = "Pause";
                RunPauseBtn.Tag = "Pause";
            }
            else if (BtnTag == "Continue")
            {
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Pause", "Pause", 32, 32);
                RunPauseBtn.ToolTip = "Pause";
                RunPauseBtn.Tag = "Pause";
                if (WinUsedForFocusScan)
                    RWrapper.RW_FocusDepth.MYINSTANCE().Continue_AutoFocusScan(true);
                else
                    RWrapper.RW_FocusDepth.MYINSTANCE().Continue_AutoContourScan(true);
            }
            else if (BtnTag == "Pause")
            {
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Continue", "Continue", 32, 32);
                RunPauseBtn.ToolTip = "Continue";
                RunPauseBtn.Tag = "Continue";
                if (WinUsedForFocusScan)
                    RWrapper.RW_FocusDepth.MYINSTANCE().Continue_AutoFocusScan(false);
                else
                    RWrapper.RW_FocusDepth.MYINSTANCE().Continue_AutoContourScan(false);
            }
        }
        void StopBtn_Click(object sender, RoutedEventArgs e)
        {
            if (WinUsedForFocusScan)
                RWrapper.RW_FocusDepth.MYINSTANCE().Stop_AutoFocusScan();
            else
                RWrapper.RW_FocusDepth.MYINSTANCE().Stop_AutoContourScan();
            RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32);
            RunPauseBtn.ToolTip = "Run";
            RunPauseBtn.Tag = "Run";
        }
        void RotateSurfaceBtn_Click(object sender, RoutedEventArgs e)
        {
            if (WinUsedForFocusScan)
                RWrapper.RW_FocusDepth.MYINSTANCE().Rotate_AutoFocusScan(double.Parse(AngularIncrementTxtBx.Text), int.Parse(NoOfStepsTxtBx.Text));
            else
                RWrapper.RW_FocusDepth.MYINSTANCE().Rotate_AutoContourScan(double.Parse(AngularIncrementTxtBx.Text), int.Parse(NoOfStepsTxtBx.Text));
        }

        void txtValues_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                double TmpRow = 0, TmpColumns = 0, TmpWidth = 0, TmpHeight = 0, TmpGap = 0, TmpSpan = 0, TmpSpeed = 0;
                if (RowsTxtBx.Text != "")
                    TmpRow = Convert.ToDouble(RowsTxtBx.Text);
                if (ColumnsTxtBx.Text != "")
                    TmpColumns = Convert.ToDouble(ColumnsTxtBx.Text);
                if (WidthTxtBx.Text != "")
                    TmpWidth = Convert.ToDouble(WidthTxtBx.Text);
                if (HeightTxtBx.Text != "")
                    TmpHeight = Convert.ToDouble(HeightTxtBx.Text);
                if (GapTxtBx.Text != "")
                    TmpGap = Convert.ToDouble(GapTxtBx.Text);
                if (SpanTxtBx.Text != "")
                    TmpSpan = Convert.ToDouble(SpanTxtBx.Text);
                if (SpeedTxtBx.Text != "")
                    TmpSpeed = Convert.ToDouble(SpeedTxtBx.Text);
                RWrapper.RW_FocusDepth.MYINSTANCE().SetFocusRectangleProperties(TmpRow, TmpColumns, TmpGap, TmpWidth, TmpHeight, TmpSpan, TmpSpeed);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:AF03", ex); }
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            MyInst = null;
            if (NotifyClose)
            {
                string BtnName;
                if (WinUsedForFocusScan)
                    BtnName = "AFS Triangulation";
                else
                    BtnName = "ACS PointsSelection";
                //e.Cancel = true;
                GlobalSettings.MainWin.CrosshairScrollList.Check_Togg_or_Radio_Btn(BtnName, false);
                RWrapper.RW_MainInterface.MYINSTANCE().HandleCrossHairToolbar_Click(BtnName);
                GlobalSettings.MainWin.CHairRBtnChecked = false;
                //return;
            }
            SaveParameters();
        }

        public string TextFilePath { get; set; }
        public void SaveParameters()
        {
            try
            {

                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\AutoFocusScanWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("Columns", ColumnsTxtBx.Text));
                    ListOfParameters.Add(new WindowsParameters<string>("Rows", RowsTxtBx.Text));
                    ListOfParameters.Add(new WindowsParameters<string>("Width", WidthTxtBx.Text));
                    ListOfParameters.Add(new WindowsParameters<string>("Height", HeightTxtBx.Text));
                    ListOfParameters.Add(new WindowsParameters<string>("Span", SpanTxtBx.Text));
                    ListOfParameters.Add(new WindowsParameters<string>("Speed", SpeedTxtBx.Text));
                    ListOfParameters.Add(new WindowsParameters<string>("Gap", GapTxtBx.Text));
                    ListOfParameters.Add(new WindowsParameters<string>("AngularIncrementInDegree", AngularIncrementTxtBx.Text));
                    ListOfParameters.Add(new WindowsParameters<string>("NumberOfSteps", NoOfStepsTxtBx.Text));
                    ReadWriteText.WriteFileWhenWindowClose(TextFilePath, ListOfParameters);
                    ListOfParameters.Clear();
                }
            }
            catch (Exception)
            {
            }
        }
        public void ReadParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\AutoFocusScanWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("Columns", ColumnsTxtBx.Text));
                        ListOfParameters.Add(new WindowsParameters<string>("Rows", RowsTxtBx.Text));
                        ListOfParameters.Add(new WindowsParameters<string>("Width", WidthTxtBx.Text));
                        ListOfParameters.Add(new WindowsParameters<string>("Height", HeightTxtBx.Text));
                        ListOfParameters.Add(new WindowsParameters<string>("Span", SpanTxtBx.Text));
                        ListOfParameters.Add(new WindowsParameters<string>("Speed", SpeedTxtBx.Text));
                        ListOfParameters.Add(new WindowsParameters<string>("Gap", GapTxtBx.Text));
                        ListOfParameters.Add(new WindowsParameters<string>("AngularIncrementInDegree", AngularIncrementTxtBx.Text));
                        ListOfParameters.Add(new WindowsParameters<string>("NumberOfSteps", NoOfStepsTxtBx.Text));
                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "Columns")
                                {
                                    ColumnsTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "Rows")
                                {
                                    RowsTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "Width")
                                {
                                    WidthTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "Height")
                                {
                                    HeightTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "Span")
                                {
                                    SpanTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "Speed")
                                {
                                    SpeedTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "Gap")
                                {
                                    GapTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "AngularIncrementInDegree")
                                {
                                    AngularIncrementTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "NumberOfSteps")
                                {
                                    NoOfStepsTxtBx.Text = ListOfParameters[i].Value.ToString();
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
    }
}
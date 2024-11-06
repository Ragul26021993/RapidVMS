using System;
using System.Globalization;
using System.Collections.Generic;
using System.Windows;
using System.IO;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Controls.Primitives;
using System.Linq;
//using System.ComponentModel;
using LicensingModuleDLL;
using Rapid.Windows;
using Rapid.Panels;
using Rapid.TabEntities;
using Rapid.Utilities;
using Rapid.AttachedProperties;
using RapidI.Help;
using System.Threading;
using System.Data;
using System.Data.OleDb;
//using System.Management;

namespace Rapid
{
    public partial class MainView : Window
    {
        #region On Load
        //Main window's load event handling
        private void Window_Loaded(object sender, System.Windows.RoutedEventArgs e)
        {
            //MessageBox.Show("0");
            try
            {
                //GlobalSettings.CurrentDroPrecision = GlobalSettings.DroPrecision.DoubleDigit;
                #region Initialize few of the front end stuff

                this.Hide(); //Hide the main window till loading cmpletes.
                //this.WindowState = WindowState.Minimized;
                //MessageBox.Show("0.07");
                splash = new SplashScreen(); //Initialize the Spalsh screen
                //splash.Topmost = true;
                //splash.Owner = this;
                splash.btnClose.Visibility = System.Windows.Visibility.Hidden;
                this.ShowSettingsWindowAfterLogin = true;
                GlobalSettings.VideoHeightThresholdForFrontEndLayout = 700;

                //Get the System working area & fit this window to it.
                this.Width = SystemParameters.WorkArea.Width;
                this.Height = SystemParameters.WorkArea.Height - 2;

                this.Measure(SystemParameters.WorkArea.Size); //Call Measure on window to get calculated the desired areas of child controls
                this.StateChanged += new EventHandler(MainView_StateChanged); //Handling the changes in the window states.
                this.PreviewKeyDown += new KeyEventHandler(MainView_PreviewKeyDown); //handling the previewKeyDown & previewKeyUp events.
                this.PreviewKeyUp += new KeyEventHandler(MainView_PreviewKeyUp);

                AboutRect.PreviewMouseLeftButtonDown += new MouseButtonEventHandler(btnAbout_Click);

                GlobalSettings.MainWin = this;

                //Initialize the template resources for styles.
                GlobalSettings.ToggleButtonStyle = (Style)TryFindResource("ScrollToolbar_ToggButton2");
                GlobalSettings.NormalButtonStyle = (Style)TryFindResource("ScrollToolbar_NormalButton2");
                GlobalSettings.RadioButtonStyle = (Style)TryFindResource("ScrollToolbar_RadioButton2");
                ChildWinBorderStyle = (Style)TryFindResource("ChildWinBorderStyle");
                ShapeListButtonStyle = (Style)TryFindResource("ShapeList_ToggButton");
                VerticalSep = (Style)TryFindResource("VerticalSeparator");
                ListItemStyle = (Style)TryFindResource("ListItemStyle");
                UserTabletBtnStyle = (Style)TryFindResource("PictureTextButton");
                //Initializing the required hash tables.
                ShapeListHashTable = new System.Collections.Hashtable();
                DXFListHashTable = new System.Collections.Hashtable();
                UCSHashTable = new System.Collections.Hashtable();

                GlobalSettings.CurrentUserLevel = GlobalSettings.UserPrevilageLevel.None;
                GlobalSettings.CurrentAngleMode = GlobalSettings.AngleMode.Degree_Minute_Second;

                GlobalSettings.SettingsReader = new DBReader();
                //MessageBox.Show(GlobalSettings.SettingsReader.StageFinished.ToString());

                if (GlobalSettings.MachineNo == "0")
                {
                    MessageBox.Show("The software could not find Settings for the chosen Machine Number. Loading default basic system. Kindly contact the manufacturer immediately for assistance.", "Rapid-I 5.0", MessageBoxButton.OK, MessageBoxImage.Error);
                }
                #endregion
                //MessageBox.Show("0.08");
                //StreamWriter ssw = new StreamWriter("E:\\TImeLog.txt", true);
                //DateTime stTime = DateTime.Now;
                RWDRO_Instantiated = false;
                //ssw.WriteLine("RapidCam3 Programming: " + (DateTime.Now - stTime).TotalSeconds);
                splash.StatusTxt.Text = "Initialising the Rapid-i platform...";
                #region Initialize the framework

                //Initialize the Framework wrapper class.
                new RWrapper.RW_MainInterface();
                String ErrorLogFileFolderPath = RWrapper.RW_MainInterface.MYINSTANCE().AppDataFolderPath + "\\Log\\ErrorLog";
                String CommandSendLogFileFolderPath = RWrapper.RW_MainInterface.MYINSTANCE().AppDataFolderPath + "\\Log\\CommandSend";
                String ModuleInitLogFileFolderPath = RWrapper.RW_MainInterface.MYINSTANCE().AppDataFolderPath + "\\Log\\ModuleInit";
                String PointsLogFileFolderPath = RWrapper.RW_MainInterface.MYINSTANCE().AppDataFolderPath + "\\Log\\PointsLog";
                String ProgramLogFileFolderPath = RWrapper.RW_MainInterface.MYINSTANCE().AppDataFolderPath + "\\Log\\ProgramsLog";
                String UserLogFileFolderPath = RWrapper.RW_MainInterface.MYINSTANCE().AppDataFolderPath + "\\Log\\UserLog";
                String LogFilePath = RWrapper.RW_MainInterface.MYINSTANCE().AppDataFolderPath + "\\Database";
                LicensingDLL LicenseMod = new LicensingDLL(RWrapper.RW_MainInterface.DBConnectionString, LogFilePath);
                LicenseMod.DoNewLogFileEntry();
                LicenseMod.GetInstallationKey();

                //Check for license=====================================================================
                LicensingModule LicenseUI = new LicensingModule(RWrapper.RW_MainInterface.DBConnectionString, LogFilePath);
                if (LicenseUI.InteractWithUser())
                {
                    UnconditionalShutDown = false;
                }
                else
                {
                    UnconditionalShutDown = true;
                    this.Close();
                    return;
                }
                //=====================================================================================
                //MessageBox.Show("0.1");
                //MessageBox.Show(GlobalSettings.MachineNo);// CompanyName + "    " + GlobalSettings.CompanyLocation);

                splash.txtCustInfo.Text = GlobalSettings.CompanyName + Environment.NewLine + GlobalSettings.CompanyLocation;
                //splash.VersionTxt.Text = "Version " + GlobalSettings.SoftwareVersion;
                splash.ReleaseVersiontxt.Text = "Release Version : " + "3515" + ", Release Date : " + "23-10-2024";
                splash.Show();

                splash.StatusTxt.Text = "Checking operating system...";
                //Handle RapidCam3 programming and killing restarter
                if (System.Environment.Is64BitOperatingSystem)
                {
                    if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.ProbeOnly)
                    {
                        switch (GlobalSettings.RapidCameraType)
                        {
                            case GlobalSettings.CameraType.FiveFourtyP:
                            case GlobalSettings.CameraType.SevenTwentyP:
                            case GlobalSettings.CameraType.TenEightyP:
                            case GlobalSettings.CameraType.TenM:
                            case GlobalSettings.CameraType.TwoM_Double:
                                CameraIsOK = Program_RapidCam3(false);
                                break;
                        }
                    }
                }
                else
                    GlobalSettings.ExcelDocConnString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + System.Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + @"\Rapid-I 5.0\Database\ExcelDoc.mdb;Persist Security Info=False;Jet OLEDB:Database Password=excelforctpl";

                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HobChecker)
                    CameraIsOK = false;

                DriveSelectionWin DriveWin = new DriveSelectionWin();
                if (!DriveWin.CheckProgramDrive(GlobalSettings.ProgramPath))
                {
                    DriveWin.ShowDialog();
                    GlobalSettings.ProgramPath = DriveWin.SelectedProgramPath + "Programs";
                    GlobalSettings.ReportPath = DriveWin.SelectedReportPath;// +"Reports";
                    GlobalSettings.TemplatePath = DriveWin.SelectedProgramPath + "Templates";
                    GlobalSettings.ShapePointsPath = DriveWin.SelectedProgramPath + "Shape Points";
                }
                RWrapper.RW_PartProgram.MYINSTANCE().SetPointsDirectoryPath(GlobalSettings.ShapePointsPath);

                #endregion


                #region Handle the events raised.

                #region Handling ContextMenu events
                //handling the all context menu in a single event.
                RWrapper.RW_MainInterface.MYINSTANCE().RightClickedOnEntity += new RWrapper.RW_MainInterface.RightClickOnEntityEventHandler(RightClickedOnEntity);
                #endregion

                #region Handling the UCS related events.
                RWrapper.RW_UCSParameter.MYINSTANCE().UcsUpdateEvent += new RWrapper.RW_UCSParameter.UCSParamEventHandler(HandleUCSUpdate);
                #endregion

                #region Shape Input information window
                RWrapper.RW_MainInterface.MYINSTANCE().CircleDiameter += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_CircleDiameter);
                RWrapper.RW_MainInterface.MYINSTANCE().ParallelLineOffset += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_ParallelLineOffset);
                RWrapper.RW_MainInterface.MYINSTANCE().PerpediclaurityLand += new RWrapper.RW_MainInterface.RWrapperEventHandlerWithDouble(Handle_PerpediclaurityLand);
                RWrapper.RW_MainInterface.MYINSTANCE().AngularityLand += new RWrapper.RW_MainInterface.RWrapperEventHandlerWithDouble(Handle_AngularityLand);
                RWrapper.RW_MainInterface.MYINSTANCE().ParallelismLand += new RWrapper.RW_MainInterface.RWrapperEventHandlerWithDouble(Handle_ParallelismLand);
                RWrapper.RW_MainInterface.MYINSTANCE().EnterDerivedLineLength += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_DerivedLineLength);
                RWrapper.RW_MainInterface.MYINSTANCE().EnterTruePosition += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_EnterTruePosition);
                RWrapper.RW_MainInterface.MYINSTANCE().EnterTruePositionForPoint += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_EnterTruePositionForPoint);
                RWrapper.RW_MainInterface.MYINSTANCE().Enter3DTruePosition += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_Enter3DTruePosition);
                RWrapper.RW_MainInterface.MYINSTANCE().Enter3DTruePositionPoint += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_Enter3DTruePositionFor3DPoint);
                RWrapper.RW_MainInterface.MYINSTANCE().OnePtAngleLine += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_OnePtAngleLine);
                RWrapper.RW_MainInterface.MYINSTANCE().OnePtOffsetLine += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_OnePtOffsetLine);
                RWrapper.RW_MainInterface.MYINSTANCE().HideEntityPanel += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_HideEntityPanel);
                RWrapper.RW_MainInterface.MYINSTANCE().RelativePointOffset += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_RelativePointOffset);
                RWrapper.RW_MainInterface.MYINSTANCE().ParallelLine3DOffset += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_ParallelLine3DOffset);
                RWrapper.RW_MainInterface.MYINSTANCE().ParallelPlaneOffset += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_ParallelPlaneOffset);
                RWrapper.RW_MainInterface.MYINSTANCE().PerpendicularPlaneOffset += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_PerpendicularPlaneOffset);
                RWrapper.RW_MainInterface.MYINSTANCE().Translationevent += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_Translate);
                RWrapper.RW_MainInterface.MYINSTANCE().RotationAngleevent += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_Rotate);
                RWrapper.RW_MainInterface.MYINSTANCE().TruePositionEvent += new RWrapper.RW_MainInterface.UpdateTruePositionWindowParam(Handle_TruePositionEvent);
                RWrapper.RW_MainInterface.MYINSTANCE().FixtureAlign_RefPt_Taken += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_FixtureAlign_RefPt_Taken);
                //handling user input of thread count
                RWrapper.RW_MainInterface.MYINSTANCE().EnterThreadCount += new RWrapper.RW_MainInterface.RWrapperEventHandler(MainView_EnterThreadCount);
                #endregion

                #region Part program events
                //Handle the build finished
                RWrapper.RW_PartProgram.MYINSTANCE().GenerateProgramReport += new RWrapper.RW_PartProgram.PartprogramEventHandler(Handle_MeasurementFinished);
                //As added by Sathya 11 Apr 2011 to handle an event to rerun part program for grid run
                RWrapper.RW_PartProgram.MYINSTANCE().RuntheProgramAgain += new RWrapper.RW_PartProgram.PartprogramEventHandler(Handle_RuntheProgramAgain);
                //When program is not loaded & run is clicked, Framework raises this event to make the Run button again to RUN mode only
                RWrapper.RW_PartProgram.MYINSTANCE().RunIsnotValid += new RWrapper.RW_PartProgram.PartprogramEventHandler(Handle_RunIsnotValid);
                //To copy the image taken during program build
                RWrapper.RW_PartProgram.MYINSTANCE().MoveImageEvent += new RWrapper.RW_PartProgram.PPImageEventHandler(MainView_MoveImageEvent);
                //function to handle partprogram finish event..........
                RWrapper.RW_PartProgram.MYINSTANCE().BuildProgramCompleted += new RWrapper.RW_PartProgram.PartprogramEventHandler(Handle_PartProgramBuild);
                //Change in mm/Inch unit
                RWrapper.RW_PartProgram.MYINSTANCE().UpdateMeasureUnit += new RWrapper.RW_PartProgram.MeasureUnitEventHandler(Handle_MeasureUnitUpdate);
                //To nudge and rotate during part program
                RWrapper.RW_MainInterface.MYINSTANCE().NudgeRotatePPevent += new RWrapper.RW_MainInterface.RWrapperEventHandler(MainView_NudgeRotatePPevent);
                //To get deviation values during part program edit
                RWrapper.RW_PartProgram.MYINSTANCE().PP_ParamResetEvent += new RWrapper.RW_PartProgram.PP_ParameterResetEventHandler(PP_ParamResetEvent);
                #endregion

                #region Other remaining events

                //Handling the CNC target reached event.
                RWrapper.RW_CNC.MYINSTANCE().CNCCommandTargetReached += new RWrapper.RW_CNC.CNCEventHandler(HandleCNC_DROTargetReached);
                //Handling the cnc mode change to click cnc button
                RWrapper.RW_CNC.MYINSTANCE().GenerateCNCBtnClick += new RWrapper.RW_CNC.CNCButtonClickEventHandler(HandleCNC_GenerateCNCBtnClick);
                RWrapper.RW_CNC.MYINSTANCE().CNCModeChange_Event += new RWrapper.RW_CNC.CNCModeChangeEventHandler(Handle_CNCModeChanged);
                //Handling axis lock event from framework
                RWrapper.RW_CNC.MYINSTANCE().AxisLockevent += new RWrapper.RW_CNC.AxisLockEventHandler(HandleAxisLock);

                //Handling the clear everything event to make application to start afresh.
                RWrapper.RW_MainInterface.MYINSTANCE().ClearEverything += new RWrapper.RW_MainInterface.ClearAllEventHandler(HandleClearEverything);

                //Handling cursor changes
                RWrapper.RW_UCSParameter.MYINSTANCE().RapidCursorChanged += new RWrapper.RW_UCSParameter.CursorTypeEventHandler(MainView_RapidCursorChanged);
                RWrapper.RW_MainInterface.MYINSTANCE().WaitCursor += new RWrapper.RW_MainInterface.ShowHideWaitCursrEventHandler(MainView_WaitCursor);

                //Handling the DXf related events
                RWrapper.RW_UCSParameter.MYINSTANCE().DxfUpdateEvent += new RWrapper.RW_UCSParameter.DXFParamEventHandler(HandleDxfUpdate);

                //Show an Input window where user can write text & click OK.This is the rasied event for that 
                RWrapper.RW_MainInterface.MYINSTANCE().EnterTextValue += new RWrapper.RW_MainInterface.RWrapperEventHandler(HandleEnterTextValue);
                //Clear all the checked radiobuttons from toolbar whenever this event happens
                RWrapper.RW_MainInterface.MYINSTANCE().ClearAllRadioButtonCheckedProperty += new RWrapper.RW_MainInterface.RWrapperEventHandler(HandleClearAllRadioButton);
                //Handling the font measurement font size window show
                RWrapper.RW_MainInterface.MYINSTANCE().ChangeMeasurementFontSize += new RWrapper.RW_MainInterface.RightClickOnEntityEventHandler(HandleMeasurementFSizeDialog);
                //handling the event to show the earlier/next or anything else selected mode before Framegrabs.
                RWrapper.RW_MainInterface.MYINSTANCE().GenerateToolbarClickEvent += new RWrapper.RW_MainInterface.RaiseToolbarClickEventHandler(Simulate_ToolbarBtnClick);

                RWrapper.RW_MainInterface.MYINSTANCE().GenerateToolbarBtnCheckEvent += new RWrapper.RW_MainInterface.RaiseToolbarClickEventHandler(Simulate_ToolbarBtnCheck);

                //handling the thread measurement selection changes
                RWrapper.RW_Thread.MYINSTANCE().ThreadMeasureSelectionChanged += new RWrapper.RW_Thread.ThreadEventHandler(MainView_ThreadMeasureSelectionChanged);

                //Handle the events for the Message window show/hide
                RWrapper.RW_MainInterface.MYINSTANCE().ShowHideStatusWindow += new RWrapper.RW_MainInterface.WaitMessageEventHandler(MainView_ShowHideStatusWindow);

                //handle the PCD related event
                RWrapper.RW_PCD.MYINSTANCE().PCDMeasureSelectionChanged += new RWrapper.RW_PCD.PCDEventHandler(MainView_PCDMeasureSelectionChanged);

                //Event to handle user entry of surface frame grab entity
                RWrapper.RW_MainInterface.MYINSTANCE().GetSurfaceEDUserChoice += new RWrapper.RW_MainInterface.SurfaceEDEventHandler(MainView_GetSurfaceEDUserChoice);
                //Event to handle user entry of line arc parameters
                RWrapper.RW_MainInterface.MYINSTANCE().LineArcParameters += new RWrapper.RW_MainInterface.LineArcParametersEventHandler(MainView_LineArcParameters);
                //Event to handle user entry of surface bestfit parameters
                RWrapper.RW_MainInterface.MYINSTANCE().BestFitSurfaceParam += new RWrapper.RW_MainInterface.BestFitSurfaceEventHandler(MainView_BestFitSurfaceParam);
                //Event to show the newely saved image in pic viewer
                RWrapper.RW_MainInterface.MYINSTANCE().SavedImagePath += new RWrapper.RW_MainInterface.SurfaceEDEventHandler(ShowFileInPicViewer);
                RWrapper.RW_MainInterface.MYINSTANCE().ShowMsgBoxText += new RWrapper.RW_MainInterface.MsgBoxEventHandler(MainView_ShowMsgBoxText);
                RWrapper.RW_MainInterface.MYINSTANCE().DisplayStatusMessage += new RWrapper.RW_MainInterface.HelpTextStatusEventHandler(MainView_DisplayHelpTextMessage);

                RWrapper.RW_MainInterface.MYINSTANCE().ShowWindowForAutoFocus += new RWrapper.RW_MainInterface.ShowAutoFocusWindow(MainView_ShowWindowForAutoFocus);
                RWrapper.RW_MainInterface.MYINSTANCE().ShowWindowForAutoContour += new RWrapper.RW_MainInterface.ShowAutoFocusWindow(MainView_ShowWindowForAutoContour);
                RWrapper.RW_MainInterface.MYINSTANCE().ShowRotateScanEvent += new RWrapper.RW_MainInterface.ShowRotateScanEventHandler(MainView_ShowRotateScanEvent);

                //Event to handle Dxf AAuto Allign
                RWrapper.RW_MainInterface.MYINSTANCE().AutoDxfAlign += new RWrapper.RW_MainInterface.RWrapperEventHandler(MainView_AutoDxfAlign);
                //Event to get and pass deviation data from framework to excel
                RWrapper.RW_MainInterface.MYINSTANCE().MeasureDeviationEvent += new RWrapper.RW_MainInterface.DeviationEventHandler(MainView_MeasureDeviationEvent);

                RWrapper.RW_MainInterface.MYINSTANCE().ShowOuterMostLineEvent += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_ShowOuterMostLinePosition);
                RWrapper.RW_MainInterface.MYINSTANCE().ShowDeviationWindow += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_ShowDeviationWindow);
                RWrapper.RW_MainInterface.MYINSTANCE().DoubleClickedOnEntity += new RWrapper.RW_MainInterface.DoubleClickOnEntityEventHandler(Handle_DoubleClickedOnEntityRcad);
                RWrapper.RW_MainInterface.MYINSTANCE().ShowCloudPointMeasureWindow += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_ShowCloudPointMeasureWindow);
                RWrapper.RW_PartProgram.MYINSTANCE().GridRunCountEvent += new RWrapper.RW_PartProgram.PPRunCount(MainView_GridRunCountEvent);
                RWrapper.RW_CNC.MYINSTANCE().GotoProbePointEvent += new RWrapper.RW_CNC.TakeProbePointEventHandler(TakeProbePoint);

                RWrapper.RW_PartProgram.MYINSTANCE().ChangeZoomSelection += new RWrapper.RW_PartProgram.ChangeZoom(MainView_ChangeZoomSelection);
                #endregion
                #endregion

                #region Handle the Circular Interpolation complete event raised
                RWrapper.RW_CircularInterPolation.MYINSTANCE().DisableAddpPointEvent += new RWrapper.RW_CircularInterPolation.DisableAddPointFlagEvent(MainView_DisableAddpPointEvent);
                #endregion
                //MessageBox.Show("1");

                ReadValuesFromDatabase();

                //Now check the Avaliable working area size & Resize panels according to that.
                System.Drawing.Rectangle resolution = System.Windows.Forms.Screen.PrimaryScreen.Bounds;
                //stTime = DateTime.Now;
                ArrangeOpenGlWindows(resolution.Width, resolution.Height); // SystemParameters.WorkArea.Width, SystemParameters.WorkArea.Height);
                ArrangeUIAccordingToBtnSize();
                FillToggleBtnContent();


                #region LeftPanels initializations

                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Measure", "Up Toolbar", 32, 32);
                MeasureGrpBx.Header = GlobalSettings.SettingsReader.ButtonToolTip;
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Draw", "Up Toolbar", 32, 32);
                DrawGrpBx.Header = GlobalSettings.SettingsReader.ButtonToolTip;
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Derived Shapes", "Up Toolbar", 32, 32);
                DerivedShpGrpBx.Header = GlobalSettings.SettingsReader.ButtonToolTip;
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Crosshair", "Up Toolbar", 32, 32);
                CrossHairGrpBx.Header = GlobalSettings.SettingsReader.ButtonToolTip;
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Edit", "Up Toolbar", 32, 32);
                EditGrpBx.Header = GlobalSettings.SettingsReader.ButtonToolTip;
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Tools", "Up Toolbar", 32, 32);
                ToolsGrpBx.Header = GlobalSettings.SettingsReader.ButtonToolTip;
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("UCS", "Up Toolbar", 32, 32);
                UCSGrpBx.Header = GlobalSettings.SettingsReader.ButtonToolTip;

                //Handling DRO Button click
                if (GlobalSettings.IsRAxisPresent || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
                {
                    RdroGrid.Visibility = System.Windows.Visibility.Visible;
                }

                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HobChecker)
                {
                    RdroGrid.Visibility = System.Windows.Visibility.Visible;
                    btnR.Content = "C";
                }

                btnLockX.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("AxisUnlocked", "AxisUnlocked", 35, 35);
                btnLockX.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                btnLockY.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("AxisUnlocked", "AxisUnlocked", 35, 35);
                btnLockY.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                btnLockZ.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("AxisUnlocked", "AxisUnlocked", 35, 35);
                btnLockZ.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                btnLockR.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("AxisUnlocked", "AxisUnlocked", 35, 35);
                btnLockR.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;

                Button[] DroButtons = { btnX, btnY, btnZ, btnR };
                Button[] DroLockButtons = { btnLockX, btnLockY, btnLockZ, btnLockR };
                for (int i = 0; i < DroButtons.Length; i++)
                {
                    DroButtons[i].PreviewMouseLeftButtonDown += new MouseButtonEventHandler(DroButton_Click);
                    DroButtons[i].PreviewMouseRightButtonDown += new MouseButtonEventHandler(DroButton_Click);
                    DroLockButtons[i].Click += new RoutedEventHandler(AxisLockButton_Click);
                    DroLockButtons[i].Tag = "Unlocked";
                }
                btnA.Click += new RoutedEventHandler(DRObtnA_Click);

                ScanOff = (Brush)TryFindResource("SnapRedStyle");
                ScanOn = (Brush)TryFindResource("SnapGreenStyle");
                ModeIndicatorRectForScanOnOff = (Rectangle)ScanOnOffBtn.Template.FindName("ModeIndicator", ScanOnOffBtn);
                ModeIndicatorRectForScanOnOff.Fill = ScanOff;
                ScanOnOffBtn.Click += new RoutedEventHandler(ScanOnOffBtn_Click);

                List<string>[] ComboBxTxts = { new List<string>(), new List<string>(), new List<string>(), new List<string>(), new List<string>(), new List<string>(), new List<string>() };
                MeasureScrollList.FillScrollBarButtons("Measure", "Measure", MeasureScrollBarButtons_Click, true, ComboBxTxts[0]);
                DrawScrollList.FillScrollBarButtons("Draw", "Draw", DrawScrollBarButtons_Click, true, ComboBxTxts[1]);
                DerivedShapeScrollList.FillScrollBarButtons("DerivedShape", "Derived Shapes", DerivedShapeScrollBarButtons_Click, true, ComboBxTxts[2]);
                CrosshairScrollList.FillScrollBarButtons("Crosshair", "Crosshair", CrosshairScrollBarButtons_Click, true, ComboBxTxts[3]);
                EditScrollList.FillScrollBarButtons("Edit", "Edit", EditScrollBarButtons_Click, true, ComboBxTxts[4]);
                ToolsScrollList.FillScrollBarButtons("Tools", "Tools", ToolsScrollBarButtons_Click, true, ComboBxTxts[5]);

                //Hide Video Flipping buttons
                ToolsScrollList.Get_Button("Flip Vertical").Visibility = Visibility.Collapsed;
                ToolsScrollList.Get_Button("Flip Horizontal").Visibility = Visibility.Collapsed;

                if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.VisionHeadOnly)
                    UCSScrollList.FillScrollBarButtons("UCS", "UCS", UCSScrollBarButtons_Click, true, ComboBxTxts[6]);
                else
                {
                    UCSGrpBx.Header = "Other Tools";
                    UCSScrollList.FillScrollBarButtons("VideoHead", "", UCSScrollBarButtons_Click, true, ComboBxTxts[6]);
                }

                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
                {
                    if (!GlobalSettings.DelfiRotaryZeroingFloatingWindowView)
                    {
                        MeasureScrollList.AddAdditionalButtons();
                    }
                    else
                    {
                        Button Btn = new Button();
                        Btn.Content = "Zero";
                        Btn.Style = GlobalSettings.NormalButtonStyle;
                        Btn.Height = GlobalSettings.ToolbarBtnStdHeight;
                        Btn.Tag = new ButtonProperties("OpenZelfiZeroingWindowBtn", false, false, "Measure");
                        Btn.Click += new RoutedEventHandler(MeasureScrollBarButtons_Click);
                        MeasureScrollList.Children.Add(Btn);
                    }
                }
                //Set Right click Event
                ScrToolbar[] ToolBarList = { MeasureScrollList, DrawScrollList, DerivedShapeScrollList, CrosshairScrollList, EditScrollList, ToolsScrollList, UCSScrollList };

                for (int i = 0; i < ToolBarList.Length; i++)
                {
                    for (int j = 0; j < ToolBarList[i].Children.Count; j++)
                    {
                        if (ToolBarList[i].Children[j].GetType() == typeof(Button) || ToolBarList[i].Children[j].GetType() == typeof(ToggleButton) || ToolBarList[i].Children[j].GetType() == typeof(RadioButton))
                            ToolBarList[i].Children[j].PreviewMouseRightButtonDown += new MouseButtonEventHandler(ScrollBarButtonHelp_click);
                    }

                    //Let us get the list of buttons for this toolbar that need to be hidden
                }


                //btnA.Content = "V";
                //btnA.Width = 48;
                //btnA.Height = 48;
                //btnA.Style = GlobalSettings.ScrBar_NormalButtonStyle;
                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.TIS)
                {
                    btnA.Visibility = System.Windows.Visibility.Visible;
                    btnA.Content = "A";
                    btnA.SetValue(AttachedDROProperties.DROTextProperty, "|  ");
                    //ToolsScrollList.Children.Add(btnA);
                }


                ComboBox[] SettingsCBList = { MeasureSettingsCB, DrawSettingsCB, DerivedShpSettingsCB, CrosshairSettingsCB, EditSettingsCB, ToolsSettingsCB, UCSSettingsCB };
                GroupBox[] ToolsGrpBxList = { MeasureGrpBx, DrawGrpBx, DerivedShpGrpBx, CrossHairGrpBx, EditGrpBx, ToolsGrpBx, UCSGrpBx };
                string[] TagStrs = { "Measure", "Draw", "DerivedShape", "Crosshair", "Edit", "Tools", "UCS" };
                for (int i = 0; i < ComboBxTxts.Length; i++)
                {
                    SettingsCBList[i] = (ComboBox)ToolsGrpBxList[i].Template.FindName("SettingsComboBx", ToolsGrpBxList[i]);
                    SettingsCBList[i].ItemsSource = ComboBxTxts[i];
                    SettingsCBList[i].Tag = TagStrs[i];
                    SettingsCBList[i].KeyDown += new KeyEventHandler(SettingsCB_KeyDown);
                    SettingsCBList[i].StaysOpenOnEdit = true;
                }

                RapidDropDownControl RDD = new RapidDropDownControl();
                RDD.Width = 70;
                RDD.Height = 60;

                RDD.Header = "MC";
                RDD.MaximumDropDownHeight = 100;
                RadioButton[] MachineTools = { new RadioButton(), new RadioButton() };
                string[] ButtonTag = { "MMC", "LMC" };
                for (int i = 0; i < MachineTools.Length; i++)
                {
                    MachineTools[i].Content = ButtonTag[i];
                    MachineTools[i].Tag = ButtonTag[i];
                    MachineTools[i].Height = 50;
                    MachineTools[i].Style = GlobalSettings.RadioButtonStyle;
                    MachineTools[i].Checked += new RoutedEventHandler(ToolsCheckboxBtnClick);
                    RDD.Scr.Items.Add(MachineTools[i]);
                }
                RDD.OnButtonClick += new RoutedEventHandler(HeaderButtonClick);
                RDD.PreviewMouseRightButtonDown += new MouseButtonEventHandler(ClearRDDMaterialControl);
                ToolsScrollList.Children.Add(RDD);

                #endregion

                #region LowerPanels Initialization
                //if ()
                //Initialising the windows that displays the lower panels
                LowerPanelWin1 = new LowerPanelWindow();
                LowerPanelWin1.IsAllowFading = true;
                LowerPanelWin1.WinClosing += new RoutedEventHandler(LowerPanelHolder_MouseLeave);
                //LowerPanelWin1.MouseLeave += new MouseEventHandler(LowerPanelHolder_MouseLeave);
                LowerPanelWin2 = new LowerPanelWindow();
                LowerPanelWin2.IsAllowFading = true;
                LowerPanelWin2.WinClosing += new RoutedEventHandler(LowerPanelHolder_MouseLeave);
                //LowerPanelWin2.MouseLeave += new MouseEventHandler(LowerPanelHolder_MouseLeave);
                LowerPanelWin3 = new LowerPanelWindow();
                LowerPanelWin3.IsAllowFading = true;
                LowerPanelWin3.WinClosing += new RoutedEventHandler(LowerPanelHolder_MouseLeave);
                //LowerPanelWin3.MouseLeave += new MouseEventHandler(LowerPanelHolder_MouseLeave);
                LowerPanelWin4 = new LowerPanelWindow();
                LowerPanelWin4.IsAllowFading = true;
                LowerPanelWin4.WinClosing += new RoutedEventHandler(LowerPanelHolder_MouseLeave);
                //LowerPanelWin4.MouseLeave += new MouseEventHandler(LowerPanelHolder_MouseLeave)
                VBlockHolderWin = new LowerPanelWindow();
                VBlockHolderWin.IsAllowFading = true;
                VBlockHolderWin.WinClosing += new RoutedEventHandler(LowerPanelHolder_MouseLeave);

                LowerPanelWin1.Width = 410;
                LowerPanelWin1.Height = 270;
                LowerPanelWin1.Top = MainLayout.RowDefinitions[0].ActualHeight - LowerPanelWin1.Height; // VideoTop + 600 - 265;
                LowerPanelWin1.Left = LowerGrid1.Margin.Left + 8; // 8;
                if (Videoheight > GlobalSettings.VideoHeightThresholdForFrontEndLayout && !OtherWinToolbarRotated) //GlobalSettings.RapidCameraType != GlobalSettings.CameraType.TwoM
                {
                    LowerPanelWin2.Width = 410;
                    LowerPanelWin2.Height = 270;
                    LowerPanelWin2.Top = VideoTop + 600 - 265 + TempHeight;
                    LowerPanelWin2.Left = 380;

                    LowerPanelWin3.Width = 410;
                    LowerPanelWin3.Height = 270;
                    LowerPanelWin3.Top = VideoTop + 600 - 265 + TempHeight;
                    LowerPanelWin3.Left = 750 + 224;

                    //VBlock Holder
                    VBlockHolderWin.Width = 410;
                    VBlockHolderWin.Height = 270;
                    VBlockHolderWin.Top = VideoTop + 600 - 265 + TempHeight;
                    VBlockHolderWin.Left = 750 + 224;
                }
                else
                {
                    LowerPanelWin2.Width = 410;
                    LowerPanelWin2.Height = 270;
                    LowerPanelWin2.Top = MainLayout.RowDefinitions[0].ActualHeight - LowerPanelWin2.Height; // VideoTop + 600 - 265;
                    LowerPanelWin2.Left = VideoLeft + Videowidth - LowerPanelWin2.Width;  // 380;

                    LowerPanelWin3.Width = 410;
                    LowerPanelWin3.Height = 270;
                    LowerPanelWin3.Top = MainLayout.RowDefinitions[0].ActualHeight - LowerPanelWin3.Height; // VideoTop + 600 - 265;
                    LowerPanelWin3.Left = MainLayout.ColumnDefinitions[1].ActualWidth + LowerGrid3.Margin.Left + 16; // 750;

                    //VBlock Holder popup Window
                    VBlockHolderWin.Width = 410;
                    VBlockHolderWin.Height = 270;
                    VBlockHolderWin.Top = MainLayout.RowDefinitions[0].ActualHeight - LowerPanelWin3.Height; // VideoTop + 600 - 265;
                    VBlockHolderWin.Left = MainLayout.ColumnDefinitions[1].ActualWidth + LowerGrid3.Margin.Left + 16; //750;
                }
                LowerPanelWin4.Width = 410;
                LowerPanelWin4.Height = 300;
                LowerPanelWin4.Top = VideoTop + 600 - 265;
                LowerPanelWin4.Left = VideoLeft + 800 - 397;

                //Now initializing the lower Drop Down contents.
                lightingPopup = new LightingPanel();
                lightingPopup.HorizontalAlignment = System.Windows.HorizontalAlignment.Stretch;
                lightingPopup.Margin = new Thickness(4, 0, 0, 4);
                LowerPanelHolder2.Content = lightingPopup;
                //lightingPopup.Width = LowerPanelHolder2.Width - 8;

                //Saoi Panel
                SaoiPopUp = new SAOI();
                GlobalSettings.SaoiModule = SaoiPopUp;
                SaoiPopUp.SAOIRBtn = SAOIRBtn;

                //CNC Panel
                CNCPopUp = new CNCPanel();
                GlobalSettings.CNCModule = CNCPopUp;
                CNCPopUp.CNCRBtn = CNCRBtn;
                if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HobChecker)
                    LowerPanelHolder3.Content = CNCPopUp;                                   //1 Written by Harphool

                //Holder Panel
                if (HolderRBtn.Visibility == Visibility.Visible)
                {
                    VBlockPopup = new VBlockHolder();
                    GlobalSettings.HolderModule = VBlockPopup;
                }

                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.DSP ||
                    GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Magnifier_DSP ||
                    GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Horizontal_DSP ||
                    GlobalSettings.RapidMachineType == GlobalSettings.MachineType.AutoFocus)
                {
                    CNCRBtn.IsEnabled = false;
                    CNCPopUp.IsEnabled = false;
                    HolderRBtn.IsEnabled = false;
                }

                //Part- Program panel
                PartProgramPopUp = new MyPartProgram();
                PartProgramPopUp.SelectedProgIndex = -1;
                PartProgramPopUp.txtReportPath.Text = GlobalSettings.ReportPath;
                List<Image> PP_ImageList;
                if (Videoheight > GlobalSettings.VideoHeightThresholdForFrontEndLayout) // GlobalSettings.RapidCameraType != GlobalSettings.CameraType.TwoM )
                {
                    PP_ImageList = GlobalSettings.SettingsReader.UpdateBtnInfoList("Part Program", 30, 30);
                }
                else
                {
                    PP_ImageList = GlobalSettings.SettingsReader.UpdateBtnInfoList("Part Program", 32, 32);
                }
                List<string> PP_ToolTipList = GlobalSettings.SettingsReader.ButtonToolTipList;
                List<int> PPbuttonType = GlobalSettings.SettingsReader.ButtontypeList;
                List<string> PP_ButtonNameList = GlobalSettings.SettingsReader.ButtonBaseNameList;

                //StartStopBtn = new Button();
                //StartStopBtn.Content = "Start";
                //StartStopBtn.Style = (System.Windows.Style)GlobalSettings.MainWin.TryFindResource("ScrollToolbar_NormalButton2");
                //StartStopBtn.Width = 60;
                //StartStopBtn.Tag = new string[] { "PartProgram", "Start", "Start", "Partprogram", "0" };
                //StartStopBtn.Click += new RoutedEventHandler(StartStopBtn_Click);
                //StartStopBtn.Margin = new Thickness(5, 0, 0, 0);

                //CircularInterpolationBtn = new Button();
                //CircularInterpolationBtn.Content = "Circular Interpolation";
                //CircularInterpolationBtn.Style = (System.Windows.Style)GlobalSettings.MainWin.TryFindResource("ScrollToolbar_NormalButton2");
                //CircularInterpolationBtn.Width = 125;
                //CircularInterpolationBtn.Tag = new string[] { "PartProgram", "Circular Interpolation", "Circular Interpolation", "Partprogram", "0" };
                //CircularInterpolationBtn.Click += new RoutedEventHandler(CircularInterpolationBtn_Click);
                //CircularInterpolationBtn.Margin = new Thickness(5, 0, 0, 0);

                //LinearInterpolationBtn = new Button();
                //LinearInterpolationBtn.Content = "Linear Interpolation";
                //LinearInterpolationBtn.Style = (System.Windows.Style)GlobalSettings.MainWin.TryFindResource("ScrollToolbar_NormalButton2");
                //LinearInterpolationBtn.Width = 125;
                //LinearInterpolationBtn.Tag = new string[] { "PartProgram", "Linear Interpolation", "Linear Interpolation", "Partprogram", "0" };
                //LinearInterpolationBtn.Click += new RoutedEventHandler(LinearInterpolationBtn_Click);
                //LinearInterpolationBtn.Margin = new Thickness(5, 0, 0, 0);

                //PathInterpolationBtn = new Button();
                //PathInterpolationBtn.Content = "Path Interpolation";
                //PathInterpolationBtn.Style = (System.Windows.Style)GlobalSettings.MainWin.TryFindResource("ScrollToolbar_NormalButton2");
                //PathInterpolationBtn.Width = 125;
                //PathInterpolationBtn.Tag = new string[] { "PartProgram", "Path Interpolation", "Path Interpolation", "Partprogram", "0" };
                //PathInterpolationBtn.Click += new RoutedEventHandler(PathInterpolationBtn_Click);
                //PathInterpolationBtn.Margin = new Thickness(5, 0, 0, 0);

                //ReferancePointBtn = new Button();
                //ReferancePointBtn.Content = "Referance Point";
                //ReferancePointBtn.Style = (System.Windows.Style)GlobalSettings.MainWin.TryFindResource("ScrollToolbar_NormalButton2");
                //ReferancePointBtn.Width = 100;
                //ReferancePointBtn.Tag = new string[] { "PartProgram", "Referance Point", "Referance Point", "Partprogram", "0" };
                //ReferancePointBtn.Click += new RoutedEventHandler(ReferancePointBtn_Click);
                //ReferancePointBtn.Margin = new Thickness(5, 0, 0, 0);

                ScrToolbar PPPanel = St;
                St.ToolBarName = "Part Program";
                PPPanel.FillScrollBarButtons("Part Program", "Partprogram", HandleLowerPanelsButtonsClick, false);
                //for (int i = 0; i < PP_ImageList.Count; i++)
                //{
                //    if (PPbuttonType[i] == 10)
                //    {
                //        Image Im = new Image();
                //        Im = PP_ImageList[i];
                //        Im.Height = 48;
                //        if (Videoheight > GlobalSettings.VideoHeightThresholdForFrontEndLayout) // if (GlobalSettings.RapidCameraType != GlobalSettings.CameraType.TwoM )
                //            Im.Width = 3;
                //        else
                //            Im.Width = 5;

                //        Im.VerticalAlignment = System.Windows.VerticalAlignment.Center;
                //        PPPanel.Children.Add(Im);
                //    }
                //    else if (PPbuttonType[i] == 0)
                //    {
                //        Button btn = new Button();

                //        if (Videoheight > GlobalSettings.VideoHeightThresholdForFrontEndLayout) // if (GlobalSettings.RapidCameraType != GlobalSettings.CameraType.TwoM)
                //        {
                //            btn.Margin = new Thickness(1, 0, 1, 0);
                //            btn.Width = 36;
                //            btn.Height = 36;

                //        }
                //        else
                //        {
                //            btn.Margin = new Thickness(5, 0, 5, 0);
                //            if (GlobalSettings.IsButtonWithNames)
                //            {
                //                btn.Width = 78;
                //                btn.Height = 57;
                //            }
                //            else
                //            {
                //                btn.Width = 48;
                //                btn.Height = 48;
                //            }

                //        }
                //        btn.VerticalAlignment = System.Windows.VerticalAlignment.Center;
                //        btn.Style = GlobalSettings.ScrBar_NormalButtonStyle;
                //        btn.Content = PP_ImageList[i];
                //        btn.Tag = new string[] { "PartProgram", PP_ToolTipList[i], PP_ButtonNameList[i], "Partprogram", GlobalSettings.SettingsReader.ButtonFeatureId[i].ToString() };
                //        btn.Click += new RoutedEventHandler(HandleLowerPanelsButtonsClick);
                //        btn.PreviewMouseRightButtonDown += new MouseButtonEventHandler(btn_PreviewMouseRightButtonDown);
                //        btn.ToolTip = PP_ToolTipList[i];
                //        btn.SetValue(AttachedButtonName.ButtonName, btn.ToolTip);
                //        PPPanel.Children.Add(btn);
                //    }
                //    else if (PPbuttonType[i] == 2)
                //    {
                //        ToggleButton btn = new ToggleButton();

                //        if (Videoheight > GlobalSettings.VideoHeightThresholdForFrontEndLayout) // if (GlobalSettings.RapidCameraType != GlobalSettings.CameraType.TwoM)
                //        {
                //            btn.Margin = new Thickness(1, 0, 1, 0);
                //            btn.Width = 36;
                //            btn.Height = 36;

                //        }
                //        else
                //        {
                //            //btn.Margin = new Thickness(5, 0, 5, 0);
                //            //btn.Width = 48;
                //            //btn.Height = 48;
                //            btn.Margin = new Thickness(5, 0, 5, 0);
                //            if (GlobalSettings.IsButtonWithNames)
                //            {
                //                btn.Width = 78;
                //                btn.Height = 57;
                //            }
                //            else
                //            {
                //                btn.Width = 48;
                //                btn.Height = 48;
                //            }
                //        }
                //        btn.VerticalAlignment = System.Windows.VerticalAlignment.Center;
                //        btn.Style = GlobalSettings.ScrBar_ToggleButtonStyle;
                //        btn.Content = PP_ImageList[i];
                //        btn.Tag = new string[] { "PartProgram", PP_ToolTipList[i], PP_ButtonNameList[i], "Partprogram", GlobalSettings.SettingsReader.ButtonFeatureId[i].ToString() };
                //        btn.Click += new RoutedEventHandler(HandleLowerPanelsButtonsClick);
                //        btn.PreviewMouseRightButtonDown += new MouseButtonEventHandler(btn_PreviewMouseRightButtonDown);
                //        btn.ToolTip = PP_ToolTipList[i];
                //        PPPanel.Children.Add(btn);
                //    }
                //}

                //PPPanel.Children.Add(StartStopBtn);
                //PPPanel.Children.Add(CircularInterpolationBtn);
                //PPPanel.Children.Add(LinearInterpolationBtn);
                //PPPanel.Children.Add(PathInterpolationBtn);
                //PPPanel.Children.Add(ReferancePointBtn);

                List<Image> PPOptions_ImageList;
                if (Videoheight > GlobalSettings.VideoHeightThresholdForFrontEndLayout) //  if (GlobalSettings.RapidCameraType != GlobalSettings.CameraType.TwoM)
                    PPOptions_ImageList = GlobalSettings.SettingsReader.UpdateBtnInfoList("Part Program Options", 30, 30);
                else
                    PPOptions_ImageList = GlobalSettings.SettingsReader.UpdateBtnInfoList("Part Program Options", 32, 32);

                List<string> PPOptions_ToolTipList = GlobalSettings.SettingsReader.ButtonToolTipList;
                List<int> PPOptionsbuttonType = GlobalSettings.SettingsReader.ButtontypeList;
                List<string> PPOptions_ButtonNameList = GlobalSettings.SettingsReader.ButtonBaseNameList;
                List<bool> PPOptions_ButtonCheckedList = GlobalSettings.SettingsReader.ButtonCheckedPropertyList;
                List<bool> PPOptions_ButtonRevertToInitialPropertyList = GlobalSettings.SettingsReader.ButtonRevertToInitialPropertyList;
                for (int i = 0; i < PPOptions_ImageList.Count; i++)
                {
                    ToggleButton tb = new ToggleButton();
                    AttachedCorner.SetCorner(tb, new CornerRadius(5));
                    tb.Style = GlobalSettings.ToggleButtonStyle;
                    //tb.Width = PartProgramPopUp.stOptions.Height;

                    //tb.Background = BackColor;
                    tb.BorderBrush = BorderBrush;
                    tb.BorderThickness = new Thickness(1);
                    tb.Margin = new Thickness(1, 2, 1, 2);
                    if (Videoheight > GlobalSettings.VideoHeightThresholdForFrontEndLayout) //  if (GlobalSettings.RapidCameraType != GlobalSettings.CameraType.TwoM )
                    {
                        //tb.Margin = new Thickness(1, 2, 1, 2);
                        tb.Width = 36;
                        tb.Height = 36;
                    }
                    else
                    {
                        //tb.Margin = new Thickness(4, 2, 4, 2);
                        tb.Height = 36;
                        tb.Width = 48;
                        if (GlobalSettings.IsButtonWithNames)
                            tb.Height = 60;
                    }
                    tb.VerticalAlignment = System.Windows.VerticalAlignment.Center;
                    tb.Content = PPOptions_ImageList[i];
                    tb.Tag = new string[] { "Part Program Options", PPOptions_ToolTipList[i], PPOptions_ButtonNameList[i], PPOptions_ButtonCheckedList[i].ToString(), PPOptions_ButtonRevertToInitialPropertyList[i].ToString() };
                    tb.ToolTip = PPOptions_ToolTipList[i];
                    tb.SetValue(AttachedButtonName.ButtonName, tb.ToolTip);
                    tb.Click += new RoutedEventHandler(HandleLowerPanelsButtonsClick);

                    stOptions.Children.Add(tb);
                    //make the return to home position to be checked
                    //(sathya told me that he is making the framework flag to be true.)11 Apr 2011
                    // if (PPOptions_ButtonNameList[i] == "Return to Home")
                    if (PPOptions_ButtonCheckedList[i])
                    {
                        tb.IsChecked = PPOptions_ButtonCheckedList[i];
                        RWrapper.RW_MainInterface.MYINSTANCE().HandleCheckedProperty_Load("Part Program Options", PPOptions_ButtonNameList[i]);
                    }
                    if (PPOptions_ButtonNameList[i] == "Grid Run")
                    {
                        ContextMenu FixtureCntxtMenu = new ContextMenu();
                        GlobalSettings.FixtureNameCBx.Width = 100;
                        GlobalSettings.FixtureNameCBx.SelectionChanged += new SelectionChangedEventHandler(FixtureNameCBx_SelectionChanged);
                        FixtureCntxtMenu.Items.Add(GlobalSettings.FixtureNameCBx);
                        tb.ContextMenu = FixtureCntxtMenu;
                    }
                }
                //ToggleButton ReturnToHomePos = (ToggleButton)PartProgramPopUp.stOptions.Children[0];
                //ReturnToHomePos.IsChecked = true;
                PartProgramPopUp.btnNewReport.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("New Report", "New Report", 30, 30);
                PartProgramPopUp.btnNewReport.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                PartProgramPopUp.btnNewReportStyle.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("New Report Style", "New Report Style", 30, 30);
                PartProgramPopUp.btnNewReportStyle.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                PartProgramPopUp.btnOK.Click += new RoutedEventHandler(HandlePartProgramNewReportChoosen);
                PartProgramPopUp.btnNewReportStyle.Click += new RoutedEventHandler(HandlePartProgramNewReportStyle_Click);
                PartProgramPopUp.dgvProgramSteps.SelectionChanged += new SelectionChangedEventHandler(dgvProgramSteps_SelectionChanged);

                if (OtherWinToolbarRotated)
                {  //VideoDisplayGrpBx.Content = PartProgramPopUp;
                    gd_MessageAndVideoGrid.Children.Add(PartProgramPopUp);
                    Grid.SetRow(PartProgramPopUp, 0);
                    Grid.SetRowSpan(PartProgramPopUp, 2);
                }
                else
                    RapidCADHolder.Content = PartProgramPopUp;

                PartProgramPopUp.Visibility = System.Windows.Visibility.Visible;
                if (Videoheight > GlobalSettings.VideoHeightThresholdForFrontEndLayout) // if (GlobalSettings.RapidCameraType != GlobalSettings.CameraType.TwoM)
                {
                    bdrPP.Margin = new Thickness(0, 0, bdrPPRightMargin, 0);
                    //GridLength tmpL = new GridLength(0);
                    //if (OtherWinPanelWidth - 190 >= 0)
                    //    tmpL = new GridLength(OtherWinPanelWidth - 190);
                    PartProgramPopUp.ContainerGrid.ColumnDefinitions[0].Width = new GridLength(OtherWinPanelWidth - 190); // tmpL;
                }

                //The thread panel
                ThreadPopup = new ThreadPanel();
                //Button[] ThreadButtons = { ThreadPopup.btnNew, ThreadPopup.btnGetResult };

                //for (int i = 0; i < ThreadButtons.Length; i++)
                //{
                ThreadPopup.btnNew.Tag = new string[] { "Thread", "Create a Thread Measurement", "New Thread", false.ToString(), true.ToString() };
                ThreadPopup.btnNew.Click += new RoutedEventHandler(HandleLowerPanelsButtonsClick);
                ThreadPopup.btnGetResult.Tag = new string[] { "Thread", "Calculate Thread Parameters", "Calculate", false.ToString(), true.ToString() };
                ThreadPopup.btnGetResult.Click += new RoutedEventHandler(HandleLowerPanelsButtonsClick);

                //}
                ThreadPopup.txtThreadCount.ValueChanged += new RoutedEventHandler(txtThreadCount_ValueChanged);
                //ThreadPopup.rdbIncludeMajMinDia.Checked += new RoutedEventHandler(rdbIncludeMajMinDia_CheckedChanged);
                //ThreadPopup.rdbIncludeMajMinDia.Unchecked += new RoutedEventHandler(rdbIncludeMajMinDia_CheckedChanged);
                ThreadPopup.Measure(new Size(double.PositiveInfinity, double.PositiveInfinity));
                //The PCD panel
                PCDPopup = new PCDPanel();
                Button[] PcdButtons = { PCDPopup.btnNew, PCDPopup.btnMeasure, PCDPopup.btnExport };
                for (int i = 0; i < PcdButtons.Length; i++)
                {
                    PcdButtons[i].Tag = new string[] { "PCD", "ToolTip", "Name", false.ToString(), true.ToString() };
                    PcdButtons[i].Click += new RoutedEventHandler(HandleLowerPanelsButtonsClick);
                }
                PCDPopup.cbAdd.SelectionChanged += new SelectionChangedEventHandler(PCD_SelectionChanged);
                PCDPopup.cbRemove.SelectionChanged += new SelectionChangedEventHandler(PCD_SelectionChanged);

                //Focus popup panel
                FocusPopUp = new FdControl();

                //Fixed graphics 
                FixedGraphicsPopup = new FixedGraphicsPanel();

                //Shape parameter window initialization.
                ShapeParameterWin = new ShapeStatusWin();
                ShapeParameterWin.ShowHideFG += new ShapeStatusWin.FGPtsShowHideEventHandler(ShapeParameterWin_ShowHideFG);

                //Measurement parameter window initialization.
                MeasurementsPopup = new MeasurementStatusWin();
                MeasurementsPopup.MeasurementsDG.ContextMenuOpening += new ContextMenuEventHandler(MeasurementPanel_PreviewMouseRightButtonDown);

                //Actions list window initialisation
                ActionsPopup = new ActionStatusWin();

                //Arranging the lower panels on the UI
                ArrangeLowerPanels(SystemParameters.WorkArea.Height);

                #endregion

                #region ContextMenu Initializations

                //Initilaizing the Choose Algorithm context menus.
                ChooseAlgoContextMenu = new System.Windows.Controls.ContextMenu();
                ChangeRcadContextMenuContents("ChooseAlgoContextMenu", ref ChooseAlgoContextMenu, "SurfaceFrameGrab");
                //VideoWin.PreviewMouseRightButtonUp += new MouseButtonEventHandler(VideoWin_PreviewMouseRightButtonUp);
                //VideoWin.ContextMenu = ChooseAlgoContextMenu;

                //Video Shapes Context menu
                VideoContextMenu = new System.Windows.Controls.ContextMenu();
                ChangeRcadContextMenuContents("VideoContextMenu", ref VideoContextMenu, "RightClickOnShape");
                //VideoContextMenu.Items.Insert(4, new Separator());
                //Assign this context menu to the Scrollable  ShapeList control.
                //ShapesScrList.ContextMenu = VideoContextMenu;
                ShapeParameterWin.ShapesDG.ContextMenu = VideoContextMenu;
                //Initialize the RCAD Context Menu.
                RcadContextMenu = new System.Windows.Controls.ContextMenu();
                //Set the default contents for the context menu.
                ChangeRcadContextMenuContents("RcadContextMenu", ref RcadContextMenu, "RightClickOnMeasure");
                //Assign this context menu to the Scrollable  MeasureList control.
                //MeasurementsScrList.ContextMenu = RcadContextMenu;
                MeasurementsPopup.MeasurementsDG.ContextMenu = RcadContextMenu;

                PPContextMenu = new System.Windows.Controls.ContextMenu();
                ChangeRcadContextMenuContents("PPContextMenu", ref PPContextMenu, "RightClickOnProgram");
                PartProgramPopUp.dgvPrograms.ContextMenu = PPContextMenu;

                RcadPointClickContextMenu = new System.Windows.Controls.ContextMenu();
                ChangeRcadContextMenuContents("TakeProbePoint", ref RcadPointClickContextMenu, "TakeProbePoint");


                #endregion

                //MessageBox.Show("2.2");

                #region Rcad/Dxf Toolbar Initialization
                //The Racd Toolbar window
                RcadToolbar = new ToolbarGrid();
                ScrToolbar st = new ScrToolbar();
                if (GlobalSettings.IsButtonWithNames)
                {
                    st.Height = 80;
                }
                RcadToolbar.SetContent(st);

                st.FillScrollBarButtons("RCAD Toolbar", "Rapidcad", RcadScrollBarButtons_Click, false, null, RCADDropDownItems_Click);
                for (int j = 0; j < st.Children.Count; j++)
                {
                    if (st.Children[j].GetType() == typeof(Button) || st.Children[j].GetType() == typeof(ToggleButton) || st.Children[j].GetType() == typeof(RadioButton))
                        st.Children[j].PreviewMouseRightButtonDown += new MouseButtonEventHandler(ScrollBarButtonHelp_click);
                }
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Maximize", "Maximize", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                st.AssignSingleBtnProp(RcadToolbar.btnMaxMin, "Maximize", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                RcadToolbar.btnMaxMin.Click += new RoutedEventHandler(RcadScrollBarButtons_Click);
                if (GlobalSettings.MultilevelZoomMahine)
                {
                    MagnificationList.MakeMultiLevelSelectorVisible();
                }


                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
                {
                    if (!GlobalSettings.DelfiRotaryZeroingFloatingWindowView)
                    {
                        if (this.DropDownPanel.Children.Contains(MagnificationList))
                        {
                            this.DropDownPanel.Children.Remove(MagnificationList);
                        }
                        if (this.DropDownPanel.Children.Contains(mmInchesList))
                        {
                            this.DropDownPanel.Children.Remove(mmInchesList);
                        }
                        if (this.DropDownPanel.Children.Contains(UCSList))
                        {
                            this.DropDownPanel.Children.Remove(UCSList);
                        }
                        if (this.DropDownPanel.Children.Contains(SnapStateButton))
                        {
                            this.DropDownPanel.Children.Remove(SnapStateButton);
                        }
                        SnapStateButton.Tag = new ButtonProperties(SnapStateButton.Name, false, false, "Rapidcad");
                        UCSList.Tag = new ButtonProperties(UCSList.Name, false, false, "Rapidcad");
                        mmInchesList.Tag = new ButtonProperties(mmInchesList.Name, false, false, "Rapidcad");
                        MagnificationList.Tag = new ButtonProperties(MagnificationList.Name, false, false, "Rapidcad");

                        SnapStateButton.Height = GlobalSettings.ToolbarBtnStdHeight;
                        UCSList.Height = GlobalSettings.ToolbarBtnStdHeight;
                        mmInchesList.Height = GlobalSettings.ToolbarBtnStdHeight;
                        MagnificationList.Height = GlobalSettings.ToolbarBtnStdHeight;

                        MagnificationList.btnTg.Foreground = Brushes.Black;
                        mmInchesList.btnTg.Foreground = Brushes.Black;
                        UCSList.btnTg.Foreground = Brushes.Black;
                        SnapStateButton.Foreground = Brushes.Black;

                        st.Children.Insert(2, SnapStateButton);
                        UCSScrollList.Children.Add(UCSList);
                        UCSScrollList.Children.Add(mmInchesList);
                        UCSScrollList.Children.Add(MagnificationList);
                    }
                }

                //The DXF Toolbar window
                DXFToolbar = new ToolbarGrid();
                ScrToolbar st1 = new ScrToolbar();
                DXFToolbar.SetContent(st1);

                if (GlobalSettings.IsButtonWithNames)
                {
                    st1.Height = 75;
                }
                st1.FillScrollBarButtons("DXF Toolbar", "DXF", DxfScrollBarButtons_Click, false, null, DxfDropDownItems_Click);
                for (int j = 0; j < st1.Children.Count; j++)
                {
                    if (st1.Children[j].GetType() == typeof(Button) || st1.Children[j].GetType() == typeof(ToggleButton) || st1.Children[j].GetType() == typeof(RadioButton))
                        st1.Children[j].PreviewMouseRightButtonDown += new MouseButtonEventHandler(ScrollBarButtonHelp_click);
                }
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Maximize", "Maximize", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                st1.AssignSingleBtnProp(DXFToolbar.btnMaxMin, "Maximize", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                DXFToolbar.btnMaxMin.Click += new RoutedEventHandler(DxfScrollBarButtons_Click);


                DropDownList MultiDxfDD = new DropDownList();
                MultiDxfDD.btnTg.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("DxfList", "DxfList", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                MultiDxfDD.btnTg.Background = Brushes.Transparent;
                MultiDxfDD.btnTg.Foreground = (Brush)TryFindResource("UpperLowerMainToolbarTextcolor");
                MultiDxfDD.lstDropDownItems.ItemContainerStyle = (Style)TryFindResource("DropDownListBoxCheckedItem");
                MultiDxfDD.Width = 100;
                st1.Children.Insert(1, MultiDxfDD);


                if (OtherWinToolbarRotated)
                {
                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge)
                    {
                        ToolbarBorder_AboveVideo.Child = RcadToolbar;
                        RcadToolbarBorder.Child = DXFToolbar;
                    }
                    else
                    {
                        RcadToolbarBorder.Child = RcadToolbar;
                        //if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HobChecker)
                        DxfToolbarBorder.Child = DXFToolbar;
                    }
                }
                #endregion
                //MessageBox.Show("2.4");

                splash.StatusTxt.Text = "Setting up camera...";
                #region Upper Dropdown Initializations
                if (CameraIsOK)
                {
                    MagnificationList.lstDropDownItems.ItemsSource = GlobalSettings.MagList;

                    MagnificationList.lstDropDownItems.Name = "MagLevels";
                    if (GlobalSettings.MagList.Contains(GlobalSettings.DefaultMagLevel))
                        MagnificationList.lstDropDownItems.SelectedItem = GlobalSettings.DefaultMagLevel;
                    else
                        MagnificationList.lstDropDownItems.SelectedIndex = 0;

                    MagnificationList.lstDropDownItems.SelectionChanged += new SelectionChangedEventHandler(UpperDropDwonList_SelectionChanged);
                }
                //MM/Inches
                string[] LenghtUnits = { "mm", "In" };
                for (int i = 0; i < LenghtUnits.Length; i++)
                    mmInchesList.lstDropDownItems.Items.Add(LenghtUnits[i]);
                mmInchesList.lstDropDownItems.Name = "MMInches";
                mmInchesList.lstDropDownItems.SelectionChanged += new SelectionChangedEventHandler(UpperDropDwonList_SelectionChanged);

                //UCS
                UCSList.lstDropDownItems.Name = "UCS";
                UCSList.DropDownBorder.Height = 200;
                string MCSSTring = GenerateMCSString();
                //UCSList.lstDropDownItems.Items.Add("MCS | XY");
                UCSList.lstDropDownItems.Items.Add(MCSSTring);

                //Made an entry for MCS | XY & add it to the dropdown list
                //UCSHashTable.Add("MCS | XY", 0);
                UCSHashTable.Add(MCSSTring, 0);
                UCSList.lstDropDownItems.SelectedIndex = 0;
                UCSList.lstDropDownItems.SelectionChanged += new SelectionChangedEventHandler(UpperDropDwonList_SelectionChanged);

                //Extended Snap on Off mode control
                SnapOff = (Brush)TryFindResource("SnapRedStyle");
                SnapOn = (Brush)TryFindResource("SnapOrangeStyle");
                VSnapOn = (Brush)TryFindResource("SnapGreenStyle");
                ModeIndicatorRect = (Rectangle)SnapStateButton.Template.FindName("ModeIndicator", SnapStateButton);
                ModeIndicatorRect.Fill = SnapOn;
                SnapStateButton.ToolTip = "Snap ON";
                SnapStateButton.Click += new RoutedEventHandler(SnapStateButton_Click);
                //ExtendedDD.btnSnapMode.Click += new RoutedEventHandler(btnSnapMode_Click);
                //ExtendedDD.btnVSnapMode.Click += new RoutedEventHandler(btnVSnapMode_Click);

                #endregion

                #region Other Windows initializations

                //Window for entering shape related informations(like radius for circle shape etc)
                shapeWin = new ShapeInfoWin();
                shapeWin.Left = RcadLeft;
                shapeWin.Top = RcadTop;
                shapeWin.WinClosing += new RoutedEventHandler(shapeWin_WinClosing);

                //window to show the DXF Nudge/Rotate Step value 
                DxfNudgeRotateStepWin = new NudgeRotateWin();
                DxfNudgeRotateStepWin.Top = RcadTop;
                if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.VisionHeadOnly)
                    DxfNudgeRotateStepWin.Left = RcadLeft + DxfWin.Width - DxfNudgeRotateStepWin.Width;
                else
                    DxfNudgeRotateStepWin.Left = RcadLeft; // + DxfWin.Width - DxfNudgeRotateStepWin.Width;

                DxfNudgeRotateStepWin.PreviewKeyDown += new KeyEventHandler(MainView_PreviewKeyUp);
                DxfNudgeRotateStepWin.PreviewKeyUp += new KeyEventHandler(MainView_PreviewKeyDown);
                DxfNudgeRotateStepWin.ValTxtBx.Text = "10";
                DxfNudgeRotateStepWin.ValTxtBx.TextChanged += new TextChangedEventHandler(DxfNudgeRotateStepValue_TextChanged);
                DxfNudgeRotateStepWin.SaveAlignmentBtn.Click += new RoutedEventHandler(SaveAlignmentBtn_Click);

                //Initializing nominal tolerance window.
                GlobalSettings.NomTolWin = new ToleranceEnteringWin();
                GlobalSettings.NomTolWin.Left = RcadLeft;
                GlobalSettings.NomTolWin.Top = RcadTop;
                GlobalSettings.NomTolWin.WinClosing += new RoutedEventHandler(NomTolWin_WinClosing);

                //Initialising thread number entering window
                UserThreadEnteringWin = new ThreadNumWin();
                UserThreadEnteringWin.WinClosing += new RoutedEventHandler(UserThreadEnteringWin_WinClosing);

                //Initialize the Build window for Part-Program
                PartProgramBuildWin = new PPBuildWin();
                PartProgramBuildWin.btnBuild.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Build", "Build Window", 30, 30);
                PartProgramBuildWin.btnBuild.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                PartProgramBuildWin.btnDesignTemplate.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Design Template", "Build Window", 30, 30);
                PartProgramBuildWin.btnDesignTemplate.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                PartProgramBuildWin.btnBuild.Click += new RoutedEventHandler(HandlePartProgramBuildWin_Click);
                PartProgramBuildWin.btnDesignTemplate.Click += new RoutedEventHandler(HandlePartProgramBuildWin_Click);
                PartProgramBuildWin.btnClose.Click += new RoutedEventHandler(HandlePartProgramBuildWin_Click);
                PartProgramBuildWin.InputBindings.Add(new KeyBinding((ICommand)TryFindResource("DoNothingCommandRefrence"), Key.F4, ModifierKeys.Alt));
                PartProgramBuildWin.ExportToCsv = GlobalSettings.PartProgBuildGenerateCSV;

                //Font size window
                FontWin = new FontSizeWin(10, 60);

                //Picture viewer images
                List<Image> PicViewImages = GlobalSettings.SettingsReader.UpdateBtnInfoList("PictureViewer Toolbar", 40, 40);
                Button[] picButtons = { PictureViewerWin.OpenButton, PictureViewerWin.btnActualSize, PictureViewerWin.btnReset };
                for (int i = 0; i < picButtons.Length; i++)
                    picButtons[i].Content = PicViewImages[i];

                //Login window initializations
                SettingsLoginWindow = new LoginWin();
                SettingsLoginWindow.WindowState = System.Windows.WindowState.Normal;

                //calibration window initializations 
                //CalibWin = new CalibrationWin();

                //Surface edge detection user choice window
                SEDEntitiesWindow = new SurfaceFGEntityWin();
                SEDEntitiesWindow.Left = VideoLeft + 5;
                SEDEntitiesWindow.Top = VideoTop + 5;

                TransparencyWindow = new TransparencySettingWin();
                TransparencyWindow.Left = RcadLeft;
                TransparencyWindow.Top = RcadTop;

                GlobalSettings.PtSizeWin = new FGandCldPtSizeWin();
                GlobalSettings.PtSizeWin.Left = RcadLeft;
                GlobalSettings.PtSizeWin.Top = RcadTop + Rcadheight - 110;

                //GlobalSettings.PPLightControlWin = new PPLightControl();
                //SetOwnerofWindow(GlobalSettings.PPLightControlWin);
                //GlobalSettings.PPLightControlWin.Left = VideoLeft + lightingPopup.Width - GlobalSettings.PPLightControlWin.Width - 4;
                //GlobalSettings.PPLightControlWin.Top = SystemParameters.WorkArea.Height - 180;

                //Help Window initialisation
                if (System.Environment.Is64BitOperatingSystem)
                    HelpWindow.dbPath = "Provider=Microsoft.Ace.OLEDB.12.0;Data Source=" + Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Database\\RapidSettings.mdb;Persist Security Info=False;Jet OLEDB:Database Password=cTpL_4577ri";
                else
                    HelpWindow.dbPath = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Database\\RapidSettings.mdb;Persist Security Info=False;Jet OLEDB:Database Password=cTpL_4577ri";
                //SetOwnerofWindow(HelpWindow.GetObj());
                btnHelp.Click += new RoutedEventHandler(btnHelp_Click);

                //Window for user selection of stylus type for T-probe hit
                PrbStylusSelectionWin = new ProbeStylusTypeSelectionWin();
                PrbStylusSelectionWin.Left = VideoLeft + 5;
                PrbStylusSelectionWin.Top = VideoTop + 5;
                SetOwnerofWindow(PrbStylusSelectionWin);
                //
                //GenericProbSelectionWin = new GenericProb();
                //GenericProbSelectionWin.Left = VideoLeft + 5;
                //GenericProbSelectionWin.Top = VideoTop + 5;

                //Path interpolation window for height guage
                PathInterpolationWindow = new PathInterpolationWin();
                PathInterpolationWindow.Left = RcadLeft + 5;
                PathInterpolationWindow.Top = RcadTop + 5;

                FilterLnTypeWin = new FilterLineTypeSelectionWin();
                SetOwnerofWindow(FilterLnTypeWin);
                FilterLnTypeWin.Left = VideoLeft + 5;
                FilterLnTypeWin.Top = VideoTop + 5;
                #endregion
                //MessageBox.Show("2.6");

                #region ExcelModule initializations
                try
                {
                    //Initilaize the Excel module
                    GlobalSettings.MainExcelInstance = new rrm.Form1();
                    //GlobalSettings.MainExcelInstance.Owner = this; // VideoFormsWin;
                    GlobalSettings.MainExcelInstance.DefaultReportSelection(!GlobalSettings.XLExportRowWise, !GlobalSettings.XLExportPortraitLayout);
                    GlobalSettings.MainExcelInstance.ToleranceFormat = GlobalSettings.XLDataTransferTolMode;
                    GlobalSettings.MainExcelInstance.ShowAvging = GlobalSettings.ShowAveragingInExcel;
                    GlobalSettings.LoadExcelInstance = new rrm.LoadFrm();
                    GlobalSettings.MainExcelInstance.NewStyleCreated += new rrm.Form1.NewStyleCreatedEventHandler(MainExcelInstance_NewStyleCreated);
                    GlobalSettings.MainExcelInstance.Password = GlobalSettings.XLPassword;
                    GlobalSettings.MainExcelInstance.TemplateFolderName = GlobalSettings.TemplatePath;
                    GlobalSettings.MainExcelInstance.MakePDFReport = GlobalSettings.MakePDFReport;
                    GlobalSettings.MainExcelInstance.IsDoPagination = GlobalSettings.PaginateReports;
                    GlobalSettings.MainExcelInstance.MultipleReportsInDifferentPages = !GlobalSettings.MultiReportsContinuousMode;
                    GlobalSettings.MainExcelInstance.RefreshDatabase();

                    if (GlobalSettings.XLExportImg1DuringPP > 0)
                    {
                        if (GlobalSettings.XLExportImg2DuringPP > 0)
                        {
                            GlobalSettings.MainExcelInstance.InsertDoublePicture = true;
                        }
                        else
                            GlobalSettings.MainExcelInstance.InsertImage = true;
                    }
                }
                catch (Exception ex)
                {
                    //string ss = ex.Message + "\r\n and the stack trace is \r\n"
                    MessageBox.Show(ex.Message + "\r\n" + ex.StackTrace);
                    //MessageBox.Show("The initialization of excel module had failed.\r\nHowever exe will attempt to load.\r\n\r\n Possible Causes : Please ensure that DSOFramer is installed properly in your system.",
                    //    this.Name, MessageBoxButton.OK, MessageBoxImage.Asterisk);
                }
                rrm.Form1.OnErrorHappen += new rrm.Form1.ErrorEventHandler_ExcelModule(Form1_OnErrorHappen);
                //Load all the program names in part-program panel
                // if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.OnlineSystem && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HobChecker)
                LoadProgramReportNames();
                //get the max count of the program in the program folder
                //This will serve as default name for a next build of program.
                CheckPartprogramDir_GetMaxCountOfProgramsMade();
                #endregion
                //MessageBox.Show("2.8");

                #region Settings Window's Owner properties
                // Make the main window & all owned windows visible
                this.Visibility = System.Windows.Visibility.Visible;
                //Now make the list of all windows avaliable
                Window[] MainWindows = { CalibWin,
                                         shapeWin,DxfNudgeRotateStepWin,PictureViewerWin};
                Window[] Owners = { this,
                                    RcadWin,DxfWin, this};
                //VideoFormsWin.Show(); VideoFormsWin.Hide();
                RcadWin.Show(); RcadWin.Hide();
                DxfWin.Show(); DxfWin.Hide();
                if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.VisionHeadOnly)
                {
                    for (int i = 0; i < MainWindows.Length; i++)
                        if (MainWindows[i] != null) MainWindows[i].Owner = Owners[i];
                }
                //OverlapImgWin.Show();
                //VideoFormsWin.SetOwner(this);
                SetOwnerofWindow(LowerPanelWin1);
                SetOwnerofWindow(LowerPanelWin2);
                SetOwnerofWindow(LowerPanelWin3);
                SetOwnerofWindow(LowerPanelWin4);
                SetOwnerofWindow(VBlockHolderWin);
                //Set the owners of the windows.

                this.UpdateLayout();
                //VideoWin.Show();
                //RcadWin.Show();
                #endregion

                //Take care of number of axis
                btnZ.SetValue(AttachedDROProperties.DROTextProperty, String.Format("{0:0.0000}", 0));
                if (GlobalSettings.NumberOfAxes <= 2)
                    ZdroGrid.IsEnabled = false;

                //MessageBox.Show("3");
                splash.StatusTxt.Text = "Setting up CAD windows...";
                #region OpenGl windows Initialization

                //Initializing the videos threads from framework
                //Video window
                //RcadWin.Background = Brushes.Red;
                //DxfWin.Background = Brushes.Red;
                //RWrapper.RW_MainInterface.MYINSTANCE().IntializeOpengl(VideoWin.Handle, (int)VideoWin.Width, (int)VideoWin.Height, 0);
                //Program RapidCam3 if its present. 
                RWrapper.RW_MainInterface.MYINSTANCE().IntializeOpengl(RcadWin.Handle, (int)RcadWin.Width, (int)RcadWin.Height, 1, 800, 600);
                //Dxf Window
                RWrapper.RW_MainInterface.MYINSTANCE().IntializeOpengl(DxfWin.Handle, (int)DxfWin.Width, (int)DxfWin.Height, 2, 800, 600);

                //if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.ProbeOnly &&
                //    GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HeightGauge
                //    )
                //{
                //    //VideoFormsWin.Show();
                //    VideoFormsWinSet();
                //}
                VideoFormsWinSet((int)NewVideoWidth, (int)NewVideoHeight);


                //VideoFormsWin.SetOwned(RcadWin);
                RcadWin.Owner = this;
                SetOwnerofWindow(DxfWin);
                SetOwnerofWindow(OverlapImgWin);
                this.Visibility = System.Windows.Visibility.Hidden;
                SetOwnerofWindow(splash);
                //SetOwnerofWindow(splash);
                //splash.Owner = OverlapImgWin;
                splash.Owner = this; // RcadWin;

                if (OtherWinToolbarRotated)// && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HobChecker)
                {
                    SetOwnerofWindow(ToolBoxHolder);
                    PPToolBoxHolder.Owner = DxfWin;
                }
                //UpdateOnSeparateThread();
                #endregion
                //MessageBox.Show("4");

                splash.StatusTxt.Text = "Finalising the environment...";
                #region Some final stuff.
                //Call this function to clear all the database resources used by Rwapper class.
                RWrapper.RW_DBSettings.MYINSTANCE().FinaliseAll();
                //this.Show();
                //for (int i = 0; i < MainWindows.Length; i++)
                //    MainWindows[i].Visibility = visi[i];
                // MessageBox.Show("4.1");

                //LowerPanelHolder2.Content = null;                                                                     //Written by Harphool
                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.One_Shot)
                {
                    if (!OtherWinToolbarRotated && !LowerPanelsMinimized)
                    {
                        ProgramRBtn.IsChecked = true;
                        //ShapesRBtn.IsChecked = true;
                        DXFTBtn.IsChecked = true;
                        LowerPanelHolder2.Content = null;
                        LowerPanelHolder2.HorizontalContentAlignment = System.Windows.HorizontalAlignment.Stretch;
                        LowerPanelHolder1.Content = lightingPopup;
                    }
                    if (OtherWinToolbarRotated)
                    {
                        RcadWin.Show();
                        DxfWin.Show();
                        //UpdateOnSeparateThread();
                    }
                }
                else if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.OnlineSystem)
                {
                    if (!OtherWinToolbarRotated && !LowerPanelsMinimized)
                    {
                        DXFTBtn.IsChecked = true;
                        RapidCADRBtn.IsChecked = true;
                        LowerPanelHolder2.Content = null;
                        LowerPanelHolder2.HorizontalContentAlignment = System.Windows.HorizontalAlignment.Stretch;
                        LowerPanelHolder1.Content = lightingPopup;
                    }
                    if (OtherWinToolbarRotated)
                    {
                        RcadWin.Show();
                        DxfWin.Show();
                    }
                }
                else
                {
                    if (OtherWinToolbarRotated)
                    {
                        RcadWin.Show();
                        if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HobChecker)
                            DxfWin.Show();
                        //UpdateOnSeparateThread();
                    }
                    else
                    {
                        if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge)
                        {
                            ProgramRBtn.IsChecked = true;
                            ToolbarBorder_AboveVideo.Child = RcadToolbar;
                            RcadWin.Show();
                        }
                        else
                        {
                            if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.VisionHeadOnly)
                                RapidCADRBtn.IsChecked = true;
                        }
                    }
                    //MessageBox.Show(LowerPanelsMinimized.ToString());
                    if (DxfWin != null && DxfWin.Visibility == System.Windows.Visibility.Visible)
                        splash.Owner = DxfWin;

                    if (!LowerPanelsMinimized && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.VisionHeadOnly)
                    {
                        if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly
                            || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge)
                            CNCRBtn.IsChecked = true;
                        else
                        {
                            FocusRBtn.IsChecked = true;
                            if (Videoheight > GlobalSettings.VideoHeightThresholdForFrontEndLayout)//if (GlobalSettings.RapidCameraType != GlobalSettings.CameraType.TwoM )
                            {
                                LightingRBtn.IsChecked = true;
                                LightingRBtn.IsEnabled = false;
                                LowerPanelHolder2.HorizontalContentAlignment = System.Windows.HorizontalAlignment.Stretch;
                                LowerPanelHolder2.Content = lightingPopup;
                                CNCRBtn.IsChecked = true;
                                //CNCRBtn.IsEnabled = false;
                                LowerPanelHolder3.Content = CNCPopUp;
                            }
                        }
                        if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HobChecker)
                            ShapesRBtn.IsChecked = true;
                        else if (!OtherWinToolbarRotated)
                            //ShapesRBtn.IsChecked = true;
                            DXFTBtn.IsChecked = true;
                    }
                }
                //MessageBox.Show(OtherWinToolbarRotated.ToString());
                //MessageBox.Show(GlobalSettings.RapidMachineType.ToString());
                //MessageBox.Show("4.2");

                try
                {
                    //if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HobChecker)
                    //{GlobalSettings.RapidCameraType != GlobalSettings.CameraType.TwoM
                    if (((LowerPanelsMinimized || Videoheight > GlobalSettings.VideoHeightThresholdForFrontEndLayout) && OtherWinToolbarRotated))
                    {
                        LowerPanelHolder2.Content = null;
                        LowerPanelWin2.LowerPanelWinContainer.HorizontalContentAlignment = System.Windows.HorizontalAlignment.Stretch;
                        LowerPanelWin2.LowerPanelWinContainer.Content = lightingPopup;
                    }
                    if (GlobalSettings.Light_Zone_Nos[0] > 8)
                    {
                        LowerPanelWin2.Width = 472;
                        LowerPanelWin2.Height = 356;
                        LowerPanelWin2.Left = VideoLeft;
                        LowerPanelHolder2.Content = null;
                        LowerPanelWin2.HorizontalContentAlignment = System.Windows.HorizontalAlignment.Stretch;
                        LowerPanelWin2.VerticalContentAlignment = System.Windows.VerticalAlignment.Stretch;
                        LowerPanelWin2.LowerPanelWinContainer.HorizontalContentAlignment = System.Windows.HorizontalAlignment.Stretch;
                        LowerPanelWin2.LowerPanelWinContainer.VerticalContentAlignment = System.Windows.VerticalAlignment.Center;
                        LowerPanelWin2.LowerPanelWinContainer.Content = lightingPopup;
                        lightingPopup.Height = 320; lightingPopup.Width = 448;
                        LowerPanelWin2.Top = this.Height - LowerPanelWin2.Height - 12;
                    }

                    if ((LowerPanelsMinimized || Videoheight > GlobalSettings.VideoHeightThresholdForFrontEndLayout) &&
                            GlobalSettings.RapidMachineType != GlobalSettings.MachineType.One_Shot)
                    {
                        if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.ProbeOnly &&
                            GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HeightGauge &&
                            GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HobChecker)
                        {
                            LowerPanelHolder3.Content = null;
                            LowerPanelWin3.LowerPanelWinContainer.Content = CNCPopUp;
                        }
                        LowerPanelWin4.LowerPanelWinContainer.Content = SaoiPopUp;

                        VBlockHolderWin.LowerPanelWinContainer.Content = VBlockPopup;

                    }
                    //}
                }
                catch (Exception ex422)
                {
                    MessageBox.Show(ex422.Message + "  " + ex422.StackTrace);
                }

                UpdateOnSeparateThread();

                //Show the login window if it is being set to do so in database
                if (GlobalSettings.LockUIDuringStartup)
                {
                    ShowSettingsWindowAfterLogin = false;
                    Simulate_ToolbarBtnClick("Tools", "Login", false);
                    ShowSettingsWindowAfterLogin = true;
                }

                #endregion
                //MessageBox.Show("4.4");

                if (GlobalSettings.DelfiRotaryZeroingFloatingWindowView)
                {
                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
                    {
                        MagnificationList.btnTg.Foreground = Brushes.Black;
                        mmInchesList.btnTg.Foreground = Brushes.Black;
                        UCSList.btnTg.Foreground = Brushes.Black;
                        SnapStateButton.Foreground = Brushes.Black;
                        SetOwnerofWindow(DelfiRotaryContainerWindow.MyInstance());
                        if (!OtherWinToolbarRotated)
                        {
                            DelfiRotaryContainerWindow.MyInstance().Left = VideoLeft - LeftPanelGrid.ActualWidth - 15;
                            DelfiRotaryContainerWindow.MyInstance().Top = VideoTop - 48 - 5;
                        }
                        else
                        {
                            DelfiRotaryContainerWindow.MyInstance().Left = RcadLeft;
                            DelfiRotaryContainerWindow.MyInstance().Top = RcadTop - GlobalSettings.ToolbarBtnStdHeight - 10;
                        }
                        DelfiRotaryContainerWindow.MyInstance().Show();
                    }
                }
                //MessageBox.Show("4.5");
                //SetOwnerofWindow(splash);

                //MessageBox.Show("5");
                #region Instantiate DRO
                //creating a new instance of DRO)
                new RWrapper.RW_DRO();
                //Handling the DRO changed events.
                RWrapper.RW_DRO.MYINSTANCE().DROChanged += new RWrapper.RW_DRO.DROEventHandler(HandleDroValueChanged);
                RWrapper.RW_DRO.MYINSTANCE().FinishedSwivelling_A_Axis += new RWrapper.RW_DRO.SwivelFinishedEventHandler(HandleSwivelFinishedEvent);
                RWrapper.RW_DRO.MYINSTANCE().DROTargetReached += new RWrapper.RW_DRO.DROTargetReachedEventHandler(HandleDroTargetReached);
                RWrapper.RW_DRO.MYINSTANCE().SendCommandAcknowledge += new RWrapper.RW_DRO.SendCommandAtStartup(MainView_SendCommandAcknowledge);
                RWrapper.RW_DRO.MYINSTANCE().SendMachineSwitchedOff += new RWrapper.RW_DRO.SendCommandAtStartup(MainView_MachineSwithedOff);

                //Event to take user feedback on the current hit of probe stylus
                RWrapper.RW_DRO.MYINSTANCE().TouchedProbePostion += new RWrapper.RW_DRO.TouchedProbeEventHandler(MainView_TouchedProbePostion);
                //if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.VisionHeadOnly)
                ConnectedToHardware = RWrapper.RW_DRO.MYINSTANCE().ConnectTo_RapidIhardware();

                //To pause the partprogram waiting for user to click continue
                RWrapper.RW_DRO.MYINSTANCE().PausePartPartProgram += new RWrapper.RW_DRO.PausePartProgramEventHandler(MainView_PausePartProgram);
                //To play camera click sound
                RWrapper.RW_DRO.MYINSTANCE().CameraSoundEvent += new RWrapper.RW_DRO.CameraClickSoundEventHandler(MainView_CameraSoundEvent);

                //Delfi Dro Zeroing Offset
                RWrapper.RW_DRO.MYINSTANCE().ZeroingOffsetEvent += new RWrapper.RW_DRO.ZeroingOffsetEventHandler(MainView_ZeroingOffsetEvent);
                #endregion
                //MessageBox.Show("7");
                RWDRO_Instantiated = true;
                bool Restarter_Started = false;

                if (GlobalSettings.InchModeAsDefault)
                {
                    mmInchesList.lstDropDownItems.SelectedItem = LenghtUnits[1];
                    GlobalSettings.CurrentUnitType = GlobalSettings.UnitType.Inch;
                    GlobalSettings.CurrentDroPrecision = GlobalSettings.CurrentDroPrecision_Ori + 1;
                }
                else
                {
                    mmInchesList.lstDropDownItems.SelectedItem = LenghtUnits[0];
                    GlobalSettings.CurrentUnitType = GlobalSettings.UnitType.mm;
                    GlobalSettings.CurrentDroPrecision = GlobalSettings.CurrentDroPrecision_Ori;
                }

                if (System.Environment.Is64BitOperatingSystem && !CameraIsOK)
                {
                    if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.ProbeOnly)
                    {
                        switch (GlobalSettings.RapidCameraType)
                        {
                            case GlobalSettings.CameraType.FiveFourtyP:
                            case GlobalSettings.CameraType.SevenTwentyP:
                            case GlobalSettings.CameraType.TenEightyP:
                            case GlobalSettings.CameraType.TenM:
                            case GlobalSettings.CameraType.TwoM_Double:
                                CameraIsOK = Program_RapidCam3(false);
                                if (CameraIsOK)
                                {
                                    string[] resourceNames = System.Reflection.Assembly.GetExecutingAssembly().GetManifestResourceNames();
                                    foreach (string res in resourceNames) // (int rr = 0; rr < resourceNames.Length; rr++)
                                    {
                                        if (res.Contains("starter"))
                                        {
                                            if (!System.IO.File.Exists(System.AppDomain.CurrentDomain.BaseDirectory + "\\Rapid-I_5_Restarter.exe"))
                                            {
                                                int fileLength = (int)System.Reflection.Assembly.GetExecutingAssembly().GetManifestResourceStream(res).Length;
                                                System.IO.FileStream output = new System.IO.FileStream(System.AppDomain.CurrentDomain.BaseDirectory + "\\Rapid-I_5_Restarter.exe", System.IO.FileMode.Create, System.IO.FileAccess.Write);
                                                byte[] buffer = new byte[fileLength];
                                                System.Reflection.Assembly.GetExecutingAssembly().GetManifestResourceStream(res).Read(buffer, 0, fileLength);
                                                output.Write(buffer, 0, fileLength);
                                                System.Reflection.Assembly.GetExecutingAssembly().GetManifestResourceStream(res).Close();
                                                output.Close();
                                            }
                                            //UnconditionalShutDown = true;
                                            System.Diagnostics.Process.Start(System.AppDomain.CurrentDomain.BaseDirectory + "\\Rapid-I_5_Restarter.exe");
                                            Restarter_Started = true;
                                            System.Threading.Thread.Sleep(500);
                                            //System.Diagnostics.Process.GetCurrentProcess().Kill();
                                            //Application.Current.Shutdown();
                                            break;
                                        }
                                    }
                                    //MessageBox.Show("Camera has been successfully re-activated. Software will close. Kindly re-open the software to work with Rapid-I. Thank you!");
                                }
                                else
                                    MessageBox.Show("Camera could not be automatically re-activated. Please disconnect the RapidCam3 (blue USB 3.0 cable) from the computer, wait 10 seconds, and then reconnect. Kindly re-open the software to work with Rapid-I. Thank you!");
                                break;
                        }
                    }
                }
                #region On Software startup
                OnSoftwareStartup();
                if (!System.IO.Directory.Exists(GlobalSettings.SaveWindowsParameterFolderPath))
                {
                    System.IO.Directory.CreateDirectory(GlobalSettings.SaveWindowsParameterFolderPath);
                }
                #endregion


                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HobChecker)
                {
                    try
                    {
                        //hobWindow = new HobCheckerWin();
                        //SetOwnerofWindow(hobWindow);
                        //hobWindow.Show();
                        //splash.Owner = hobWindow;
                        Init_HobFrontEnd();
                        VideoHolder.Visibility = Visibility.Hidden;
                        GridHobChecker.Visibility = Visibility.Visible;
                        //hobWindow.Left = VideoLeft;
                        //hobWindow.Top = VideoTop;
                        //hobWindow.ShowInTaskbar = false;
                        //   MessageBox.Show("4.6");

                        //hobWindow.Owner = this;
                        //VideoFormsWin.Visible = false;
                        DxfWin.Visibility = System.Windows.Visibility.Hidden;
                        LowerPanelWin3.Top = VideoTop + Rcadheight + 4;
                        LowerPanelWin3.Left = RcadLeft;
                        //    MessageBox.Show("4.7");

                        LowerPanelHolder3.Content = CNCPopUp;
                        MagnificationList.Visibility = System.Windows.Visibility.Hidden;
                    }
                    catch (Exception ex22)
                    {
                        MessageBox.Show(ex22.Message + "                     " + ex22.StackTrace);
                    }
                    try
                    {
                        if (this.Width == 1920)
                        {
                            OtherWinPanelBorder.Visibility = System.Windows.Visibility.Visible;
                            LowerWinPanel4.Visibility = System.Windows.Visibility.Visible;
                            DxfHolder.Visibility = System.Windows.Visibility.Visible;
                            DxfLowerToolbarBorder.Visibility = System.Windows.Visibility.Visible;
                            RapidCADGrid.Visibility = System.Windows.Visibility.Visible;
                            LowerGrid4.Visibility = System.Windows.Visibility.Visible;
                        }
                    }
                    catch (Exception ex23)
                    {
                        MessageBox.Show(ex23.Message + "                     " + ex23.StackTrace);
                    }
                    //SetOwnerofWindow(splash);
                }
                else if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.TIS)
                {
                    TISControl = new TIS_Control();
                    TISControl.Owner = this;
                    TISControl.Left = 8;
                    TISControl.Top = Tools_Grid.RowDefinitions[0].ActualHeight + Tools_Grid.Margin.Top;
                    TISControl.ShowInTaskbar = false;
                    TISControl.Show();
                    splash.Owner = TISControl;
                    RdroGrid.Visibility = System.Windows.Visibility.Visible;
                    AdroGrid.Visibility = System.Windows.Visibility.Visible;
                }
                else if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.OnlineSystem)
                {
                    //We instantiate and show the Control Centre window
                    OnlineControlWindow = new Views.Windows.OnlineSystemController();
                    OnlineControlWindow.Owner = this;
                    OnlineControlWindow.Width = MainLayout.ColumnDefinitions[0].ActualWidth - Tools_Grid.Margin.Left - Tools_Grid.Margin.Right;
                    OnlineControlWindow.Height = Tools_Grid.ActualHeight + LowerGrid1.ActualHeight + Tools_Grid.Margin.Bottom + LowerGrid1.Margin.Top;
                    OnlineControlWindow.Left = Tools_Grid.Margin.Left;
                    OnlineControlWindow.Top = Tools_Grid.RowDefinitions[0].ActualHeight + Tools_Grid.Margin.Top;
                    OnlineControlWindow.ShowInTaskbar = false;
                    LoadProgramReportNames();
                    RWrapper.RW_DRO.MYINSTANCE().ConnectOnlineSystem(false);
                    GlobalSettings.OnlineSystemStates = new List<GlobalSettings.OnlineRunningState>();
                    OnlineControlWindow.Show();
                }
                else if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.VisionHeadOnly)
                {
                    if (GlobalSettings.RapidCameraType == GlobalSettings.CameraType.TenEightyP)
                    {
                        MainLayout.ColumnDefinitions[0].Width = new GridLength(0);
                        MainLayout.ColumnDefinitions[2].Width = new GridLength(0);
                        MainLayout.RowDefinitions[0].Height = new GridLength(this.Height);
                        MainLayout.RowDefinitions[1].Height = new GridLength(0);
                        Video__Grid.RowDefinitions[0].Height = new GridLength(40);
                        //Video__Grid.RowDefinitions[1].Height = new GridLength(0);
                        //DxfWin.Visibility = Visibility.Hidden; RcadWin.Visibility = Visibility.Hidden;
                        DROPanel.Visibility = Visibility.Hidden; DropDownPanel.Visibility = Visibility.Hidden;
                        UserNameBorder.Visibility = Visibility.Hidden;
                        Tools_Grid.Children.Remove(TopLeftGrid);
                        Grid.SetRow(Video__Grid, 0); Grid.SetRowSpan(Video__Grid, 3);
                        Video__Grid.Children.Add(TopLeftGrid); MainLayout.VerticalAlignment = VerticalAlignment.Stretch;
                        Video__Grid.Margin = new Thickness(0); Video__Grid.VerticalAlignment = VerticalAlignment.Top;
                        gd_UserName.Visibility = Visibility.Hidden; VideoHolder.Margin = new Thickness(0);
                        Grid.SetRow(WindowStackPanel, 0);
                        Grid.SetRow(Rapid_I, 0);
                        Grid.SetRow(Dashes, 0);
                        //Grid.SetRow(VIS_Tblock,2); Grid.SetRowSpan(VIS_Tblock, 3); 
                        TopLeftGrid.RowDefinitions.RemoveAt(1);
                        TopLeftGrid.RowDefinitions.RemoveAt(1);
                        Grid.SetColumn(VIS_Tblock, 1); VIS_Tblock.HorizontalAlignment = HorizontalAlignment.Left; VIS_Tblock.VerticalAlignment = VerticalAlignment.Bottom;
                        gd_MessageAndVideoGrid.RowDefinitions[0].Height = new GridLength(0);
                        Grid.SetRow(TopLeftGrid, 0);

                        Grid.SetColumn(modifierButtonViewer, 1); modifierButtonViewer.HorizontalAlignment = HorizontalAlignment.Right;
                        modifierButtonViewer.VerticalAlignment = VerticalAlignment.Center; modifierButtonViewer.Margin = new Thickness(0, 0, 20, 0);
                        //LowerPanelWin1.Close();
                        //LowerPanelWin2.Close();
                        //LowerPanelWin3.Close();
                        //LowerPanelWin4.Close();
                        //MessageBox.Show("Welcome to the DressCheck");
                        //System.Threading.Thread.Sleep(30000);
                        RWrapper.RW_MainInterface.MYINSTANCE().InternalClearAll(0);
                    }
                }
                else if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.One_Shot)
                {
                    ContentControl Btn = ToolsScrollList.Children.OfType<ContentControl>()
                                                            .Where(T => (((ButtonProperties)T.Tag).ButtonName) == "Sigma Mode").First();
                    if (Btn != null)
                        ((ButtonProperties)Btn.Tag).InitialChkStatus = true;

                    stOptions.Visibility = Visibility.Collapsed;
                    bdrPP.HorizontalAlignment = HorizontalAlignment.Stretch; bdrPP.Margin = new Thickness(0);
                    RWrapper.RW_MainInterface.MYINSTANCE().InternalClearAll(0);
                }
                else if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly)
                {
                    MagnificationList.Visibility = Visibility.Hidden;
                    LeftPanelGrid.RowDefinitions[3].Height = new GridLength(0);
                }

                //Hide buttons that are not relevant for this machine type
                try
                {
                    ScrToolbar[] tbList = { MeasureScrollList, DrawScrollList, DerivedShapeScrollList, CrosshairScrollList, EditScrollList, ToolsScrollList, UCSScrollList, st, st1, St };
                    CalibrationModule.DB Settings_DataB = new CalibrationModule.DB(RWrapper.RW_MainInterface.DBConnectionString);
                    Settings_DataB.FillTable("HideButtonsList");
                    for (int i = 0; i < tbList.Length; i++)
                    {
                        System.Data.DataRow[] HideButtonsList = Settings_DataB.Select_ChildControls("HideButtonsList", "Alignment", tbList[i].ToolBarName);
                        for (int k = 0; k < HideButtonsList.Length; k++)
                        {
                            GlobalSettings.MachineType smt = (GlobalSettings.MachineType)Enum.Parse(typeof(GlobalSettings.MachineType), HideButtonsList[k]["MachineType"].ToString());
                            bool HideForall = (bool)HideButtonsList[k]["HideForAll"];
                            if ((!HideForall && GlobalSettings.RapidMachineType == smt) || (HideForall && GlobalSettings.RapidMachineType != smt))
                                tbList[i].Hide_Button(HideButtonsList[k]["ButtonName"].ToString());
                        }
                    }
                }
                catch (Exception ex)
                {
                    RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV01_Hide", ex);

                }
                PPToolbarGrid.Margin = new System.Windows.Thickness(5, RapidCADGrid.ActualHeight + 12, 15, 0);

                if (!Restarter_Started)
                {
                    System.Diagnostics.Process[] runningProcs = System.Diagnostics.Process.GetProcesses(); // .GetProcessesByName("Rapid_I_5_Restarter");
                    foreach (System.Diagnostics.Process pp in runningProcs)
                    {
                        if (pp.ProcessName == "Rapid-I_5_Restarter")
                            pp.Kill();
                    }
                    bool restarterRunning = true;
                    while (restarterRunning)
                    {
                        System.Threading.Thread.Sleep(200);
                        runningProcs = System.Diagnostics.Process.GetProcessesByName("Rapid_I_5_Restarter");
                        if (runningProcs.Length > 0)
                            restarterRunning = true;
                        else
                            restarterRunning = false;
                    }
                    try
                    {
                        if (System.IO.File.Exists(System.AppDomain.CurrentDomain.BaseDirectory + "\\Rapid-I_5_Restarter.exe"))
                            System.IO.File.Delete(System.AppDomain.CurrentDomain.BaseDirectory + "\\Rapid-I_5_Restarter.exe");
                    }
                    catch (Exception excx)
                    {
                        RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV01rr", excx);
                    }
                }

                //if (GlobalSettings.CNC_Assisted_FG) CNCAssistButtonHolderGrid.Visibility = Visibility.Visible;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV01", ex);
                if (!GlobalSettings.CompletedReadingAllDatafromDB) MessageBox.Show("There seems to have been an error in reading data from the database and hence Rapid-I cannot start!", "Rapid-I");
                Application.Current.Shutdown();
            }
            finally
            {
                if (!ConnectedToHardware || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.OnlineSystem)
                {
                    splash.Hide();
                    //this.Show();
                    this.Focus();
                    this.Show();
                    //if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.VisionHeadOnly)
                    //    this.WindowState = WindowState.Maximized;
                    //GlobalSettings.SettingsReader = null;
                }

                //if (!GlobalSettings.VideoWindowInitialised && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HobChecker)
                //{
                //    BtnInsertImageinVideoWindow.Visibility = System.Windows.Visibility.Visible;
                //    BtnSaveBinarisedImage.Visibility = System.Windows.Visibility.Visible;
                //}
                //if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HobChecker)
                //    hobWindow.Show();
            }
        }

        void MainView_SendCommandAcknowledge()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    if (RWrapper.RW_MainInterface.MYINSTANCE().MachineCardFlag == 2) // && ConnectedToHardware)
                    {
                        GlobalSettings.UserPrevilageLevel thisLevel = GlobalSettings.CurrentUserLevel;
                        GlobalSettings.CurrentUserLevel = GlobalSettings.UserPrevilageLevel.System;
                        SettingsWin ThisSettings = new SettingsWin();
                        ThisSettings.FeedRateChangedEvent += new AsyncSimpleDelegate(this.HandleCNCFeedRateSettingsChanged);
                        ThisSettings.TabEEPROMSettingsFill();
                        ThisSettings.UpdatingEEPROMValuesNow = true;
                        ThisSettings.UpdateAll();
                        ThisSettings.Close();
                        RWrapper.RW_DBSettings.MYINSTANCE().UpdateProbeApproachGap_Speed(GlobalSettings.PApproachDistanceOnwards, GlobalSettings.PApproachSpeedOnwards);
                        GlobalSettings.CurrentUserLevel = thisLevel;
                    }
                    else if (RWrapper.RW_MainInterface.MYINSTANCE().MachineCardFlag != 3)
                    {
                        if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.One_Shot)
                        {
                            //ProbeMoveUp was an ill-conceived idea. We shall now hijack it to program the controller to switch between Forward touch and retract probe touch methods!
                            if (GlobalSettings.RetractProbePointMode)
                            {
                                RWrapper.RW_DBSettings.MYINSTANCE().UpdateProbeApproachGap_Speed(GlobalSettings.ProbebackDist, GlobalSettings.ProbeBackSpeed);
                                RWrapper.RW_DRO.MYINSTANCE().SetProbeRevertDistSpeed(0, GlobalSettings.ProbebackDist, GlobalSettings.ProbeBackSpeed, GlobalSettings.PApproachDistance, GlobalSettings.PApproachSpeed, GlobalSettings.PSensitivityValue, GlobalSettings.RetractProbePt_Enabled);
                            }
                            else
                            {
                                RWrapper.RW_DBSettings.MYINSTANCE().UpdateProbeApproachGap_Speed(GlobalSettings.PApproachDistanceOnwards, GlobalSettings.PApproachSpeedOnwards);
                                RWrapper.RW_DRO.MYINSTANCE().SetProbeRevertDistSpeed(0, GlobalSettings.ProbebackDist, GlobalSettings.ProbeBackSpeedOnwards, GlobalSettings.PApproachDistance, GlobalSettings.PApproachSpeedOnwards, GlobalSettings.PSensitivityValueOnwards, GlobalSettings.RetractProbePt_Enabled);
                            }

                            //set probe revertback dist and speed every time on exe run
                            RWrapper.RW_DRO.MYINSTANCE().SetInterPolationParameter(GlobalSettings.AllowInterpolation, GlobalSettings.SmoothCircularInterpolation);
                            RWrapper.RW_DRO.MYINSTANCE().SetProbeReractModeForManual(GlobalSettings.SetProbeRetractMode);
                            //RWrapper.RW_DRO.MYINSTANCE().Activate_Deactivate_Zoom_Cnc(GlobalSettings.AllowCNC, GlobalSettings.AllowAutoZoom, GlobalSettings.RouteProbePath, GlobalSettings.RetractProbePointMode, true);
                            //RWrapper.RW_DRO.MYINSTANCE().Activate_DeactivateProbe(GlobalSettings.ProbeActivation_X, GlobalSettings.ProbeActivation_Y, GlobalSettings.ProbeActivation_Z, GlobalSettings.ProbeActivation_R, true);
                            //Release axis lock if any
                            RWrapper.RW_CNC.MYINSTANCE().Unlock_Axis("All");
                            //Call the Manual mode
                            RWrapper.RW_CNC.MYINSTANCE().Activate_ManualModeDRO();
                        }
                    }
                    //Now call lighting command at the time of loading.
                    RWrapper.RW_CNC.MYINSTANCE().SwitchOnOffLight(true);
                    if (RWrapper.RW_MainInterface.MYINSTANCE().Do_Homing_At_Startup)
                        RWrapper.RW_AutoCalibration.MYINSTANCE().Handle_HomeClicked(true, true, false);

                    splash.HardWareVersiontxt.Text = "Hardware Version : " +
                        String.Format("{0:0.00}", RWrapper.RW_DRO.MYINSTANCE().HardwareVersionNumber[0]) + ", " +
                        String.Format("{0:0.00}", RWrapper.RW_DRO.MYINSTANCE().HardwareVersionNumber[1]) + ", " +
                        String.Format("{0:0.00}", RWrapper.RW_DRO.MYINSTANCE().HardwareVersionNumber[2]) + ", " +
                        String.Format("{0:0.00}", RWrapper.RW_DRO.MYINSTANCE().HardwareVersionNumber[3]) + ", " +
                        String.Format("{0:0.00}", RWrapper.RW_DRO.MYINSTANCE().HardwareVersionNumber[4]) + ", ";// +
                    //(RWrapper.RW_MainInterface.MYINSTANCE().MachineCardFlag ? "1" : "0");
                    splash.Hide();
                    this.Show();
                    //GlobalSettings.PPLightControlWin.Left = VideoLeft + lightingPopup.Width - GlobalSettings.PPLightControlWin.Width - 4;
                    //GlobalSettings.PPLightControlWin.Top = this.Height - GlobalSettings.PPLightControlWin.Height - 20;
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_DRO.SendCommandAtStartup(MainView_SendCommandAcknowledge));
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV01a", ex);
                splash.Hide();
            }
        }

        void MainView_MachineSwithedOff()
        {
            if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
            {
                ConnectedToHardware = false;
                //splash.Hide();
                //this.Show();
                //this.WindowState = WindowState.Maximized;

            }
            else
            {
                this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_DRO.SendCommandAtStartup(MainView_MachineSwithedOff));
            }
        }

        public void OnSoftwareStartup()
        {
            try
            {
                RWrapper.RW_MainInterface.MYINSTANCE().SetCriticalShapeParameter(GlobalSettings.SetCriticalShapes, GlobalSettings.AngleTolerance * (Math.PI / 180), GlobalSettings.PercentageTolOnRadius);
                RWrapper.RW_MainInterface.MYINSTANCE().SetShapeMeasureHideDuringProgramRun(GlobalSettings.ShapeMeasurementHidden);
                RWrapper.RW_MainInterface.MYINSTANCE().SetPixelWidthCorrectionStatusForOneShot(GlobalSettings.PixelWidthCorrectionEnable);
                //RWrapper.RW_MainInterface.MYINSTANCE().SetPixelWidthCorrectionConstants(GlobalSettings.PixelWidthCorrectionParameterQ, GlobalSettings.PixelWidthCorrectionParameterLX1, GlobalSettings.PixelWidthCorrectionParameterLY, GlobalSettings.PixelWidthX0, GlobalSettings.PixelWidthY0);
                //RWrapper.RW_MainInterface.MYINSTANCE().GetvalueofBCXBCY(GlobalSettings.ValueofBCX, GlobalSettings.ValueofBCY);                      //vinod..

                string[] TmpAr = { "Tahoma", "Times Roman 24", "Helvetica 18", "Calibri 9", "Stroke Roman", "Stroke Mono Roman" };
                RWrapper.RW_MainInterface.MYINSTANCE().SetOGLFontFamily(TmpAr[GlobalSettings.FontSelectedIndx]);

                if (GlobalSettings.MeasurementColorMode == "Gradient")
                    RWrapper.RW_MainInterface.MYINSTANCE().SetMeasurementColorGradient(true);
                else
                    RWrapper.RW_MainInterface.MYINSTANCE().SetMeasurementColorGradient(false);
                RWrapper.RW_MainInterface.MYINSTANCE().CreateSurfaceForClosedSurface(GlobalSettings.CloseOpenSurface);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MVOnSoftwareStartUP01", ex);
            }
        }

        public void StartStopBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Button btn = (Button)sender;
                if (btn.Content.ToString() == "Start")
                {
                    btn.Content = "Stop";
                    RWrapper.RW_CircularInterPolation.MYINSTANCE().SetAddPointFlag_HeightGauge(true);
                }
                else
                {
                    btn.Content = "Start";
                    RWrapper.RW_CircularInterPolation.MYINSTANCE().SetAddPointFlag_HeightGauge(false);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:StartStopBtn01", ex);
            }
        }

        void MainView_DisableAddpPointEvent()
        {
            try
            {
                StartStopBtn.Content = "Start";
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError: MainView_DisableAddpPointEvent", ex);
            }
        }

        public void btn_PreviewMouseRightButtonDown(object sender, MouseEventArgs e)
        {
            try
            {
                ContentControl b = (ContentControl)sender;
                string[] tagStr = (string[])b.Tag;
                string BtnName = tagStr[2];
                string BtnLocationName = tagStr[3];
                if (HelpWindow.GetObj().ShowTopic(BtnLocationName, BtnName))
                    HelpWindow.GetObj().Show();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS07a", ex); }
        }

        //Right Click Help Event Handler
        public void ScrollBarButtonHelp_click(object sender, MouseEventArgs e)
        {
            try
            {
                ContentControl Btn = (ContentControl)sender;
                string BtnName = ((ButtonProperties)Btn.Tag).ButtonName;
                string BtnLocationName = ((ButtonProperties)Btn.Tag).ToolBarName;
                if (HelpWindow.GetObj().ShowTopic(BtnLocationName, BtnName))
                    HelpWindow.GetObj().Show();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0582", ex);
            }

        }

        //Fill Toggle button with appropriate content according to language
        void FillToggleBtnContent()
        {
            try
            {
                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.One_Shot || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.OnlineSystem)
                {
                    GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Lighting", "Window", 32, 32);
                    FocusRBtn.Tag = "Lighting";
                    FocusRBtn.Content = GlobalSettings.SettingsReader.ButtonToolTip;
                }
                else
                {
                    //GlobalSettings.SettingsReader.ButtonToolTip
                    GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Focus", "Window", 32, 32);
                    FocusRBtn.Tag = "Focus";
                    FocusRBtn.Content = GlobalSettings.SettingsReader.ButtonToolTip;
                }
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Thread", "Window", 32, 32);
                ThreadRBtn.Tag = "Thread";
                ThreadRBtn.Content = GlobalSettings.SettingsReader.ButtonToolTip;
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Fixed Graphics", "Window", 32, 32);
                FixedGraphicsRBtn.Tag = "Fixed Graphics";
                FixedGraphicsRBtn.Content = GlobalSettings.SettingsReader.ButtonToolTip;
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Lighting", "Window", 32, 32);
                LightingRBtn.Tag = "Lighting";
                LightingRBtn.Content = GlobalSettings.SettingsReader.ButtonToolTip;
                //GlobalSettings.SettingsReader.UpdateSingleBtnInfo("CNC", "Window", 32, 32);
                CNCRBtn.Tag = "CNC";
                SAOIRBtn.Tag = "SAOI";
                HolderRBtn.Tag = "VBlockHolder";
                //CNCRBtn.Content = GlobalSettings.SettingsReader.ButtonToolTip;
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Shape", "Window", 32, 32);
                ShapesRBtn.Tag = "Shapes";
                ShapesRBtn.Content = GlobalSettings.SettingsReader.ButtonToolTip;
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Measurement", "Window", 32, 32);
                MeasurementsRBtn.Tag = "Measurements";
                MeasurementsRBtn.Content = GlobalSettings.SettingsReader.ButtonToolTip;
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Dxf", "Window", 32, 32);
                DXFTBtn.Tag = "DXF";
                DXFTBtn.Content = GlobalSettings.SettingsReader.ButtonToolTip;

                ActionsRBtn.Tag = "Actions";

                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("RapidCAD", "Window", 32, 32);
                RapidCADRBtn.Tag = "RapidCAD";
                RapidCADRBtn.Content = GlobalSettings.SettingsReader.ButtonToolTip;
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("PartProgram", "Window", 32, 32);
                ProgramRBtn.Tag = "Program";
                ProgramRBtn.Content = GlobalSettings.SettingsReader.ButtonToolTip;
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Picture Viewer", "Window", 32, 32);
                PicViewerRBtn.Tag = "Pic Viewer";
                PicViewerRBtn.Content = GlobalSettings.SettingsReader.ButtonToolTip;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV02", ex);
            }
        }

        //Button Name in buttons
        void ArrangeUIAccordingToBtnSize()
        {
            if (GlobalSettings.IsButtonWithNames)
            {
                //For Styling of Button
                GlobalSettings.ScrBar_NormalButtonStyle = (System.Windows.Style)GlobalSettings.MainWin.TryFindResource("ScrollToolbar_NormalButton2WithIconName");
                GlobalSettings.ScrBar_RadioButtonStyle = (System.Windows.Style)GlobalSettings.MainWin.TryFindResource("ScrollToolbar_RadioButton2WithIconName");
                GlobalSettings.ScrBar_ToggleButtonStyle = (System.Windows.Style)GlobalSettings.MainWin.TryFindResource("ScrollToolbar_ToggButton2WithIconName");

                //for PP ScrollBar
                PPToolbarGrid.Height = 75;
                // GlobalSettings.MainWin.PPToolbarGrid.Height = 78;
                St.Height = 65;

                //For RCad
                ToolbarBorder.Height = 70;
                ToolbarBorder.Margin = new System.Windows.Thickness(0, 0, 0, 0);
                ToolbarBorder_AboveVideo.Height = 70;

                //For DXF
                DxfLowerToolbarBorder.Height = 65;

                //For Button Icon Height , width  and button height width
                GlobalSettings.BtnIcoStdWidth = 28;
                GlobalSettings.BtnIcoStdHeight = 28; //GlobalSettings.ToolbarBtnStdWidth = 70;
                GlobalSettings.ToolbarBtnStdWidth = 78;
                //GlobalSettings.ToolbarBtnStdHeight = 60;
                GlobalSettings.ToolbarBtnStdHeight = 62;
            }
            else
            {
                GlobalSettings.ScrBar_NormalButtonStyle = (System.Windows.Style)GlobalSettings.MainWin.TryFindResource("ScrollToolbar_NormalButton2");
                GlobalSettings.ScrBar_ToggleButtonStyle = (System.Windows.Style)GlobalSettings.MainWin.TryFindResource("ScrollToolbar_ToggButton2");
                GlobalSettings.ScrBar_RadioButtonStyle = (System.Windows.Style)GlobalSettings.MainWin.TryFindResource("ScrollToolbar_RadioButton2");

                MeasureGrpBx.Height = 80;
                DrawGrpBx.Height = 80;
                CrossHairGrpBx.Height = 80;
                EditGrpBx.Height = 80;
                ToolsGrpBx.Height = 80;
                DerivedShpGrpBx.Height = 80;
                UCSGrpBx.Height = 80;
            }
        }

        #region Arrange OGL Window and Related UI Windows
        //All the arrangements code of UI for Opengl windows.
        void ArrangeOpenGlWindows(double width, double height)
        {
            try
            {
                bool resizeleft = false, resizeright = false;
                double changeInLeft = 0, changeInRight = 0;
                //Initialize the opengl video thread windows
                //VideoFormsWin = new FormsOpenGlWin();
                RcadWin = new OpenGlWin();
                DxfWin = new OpenGlWin();
                ThreadWin = new OpenGlWin();
                OverlapImgWin = new OpenGlWin();
                //PCDWin = new OpenGlWin();
                PictureViewerWin = PictureViewer.GetInstance;
                //Set the Row & Col span for the TabControls.
                this.MainLayout.Children.Remove(scrHelp);
                //Initialize the Tab item for  Video  TabControl.
                TabItem VideoTabItem = new TabItem();
                //videoStatus = new AboveVideoControl();

                //LowerTabWin1.InputBindings.Add(new KeyBinding((ICommand)TryFindResource("DoNothingCommandRefrence"), Key.F4, ModifierKeys.Alt));

                //For Rapid I one shot
                //First initialise normal front end positions

                //Point TmpPos = VideoHolder.TranslatePoint(new Point(0, 0), this);
                //Point TmpPos = new Point(MainLayout.ColumnDefinitions[0].ActualWidth + Video__Grid.Margin.Left + VideoDisplayGrpBx.Margin.Left + VideoHolder.Margin.Left,
                //                            Video__Grid.RowDefinitions[0].ActualHeight + VideoDisplayGrpBx.Margin.Top + VideoHolder.Margin.Top);
                Point TmpPos = new Point(MainLayout.ColumnDefinitions[0].ActualWidth + Video__Grid.Margin.Left + VideoDisplayGrpBx.Margin.Left + VideoHolder.Margin.Left,
                            Video__Grid.RowDefinitions[0].ActualHeight + gd_MessageAndVideoGrid.RowDefinitions[0].ActualHeight + gd_MessageAndVideoGrid.Margin.Top + VideoDisplayGrpBx.Margin.Top + VideoHolder.Margin.Top);

                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.VisionHeadOnly)
                {
                    TmpPos = new Point(width - 400, 40);
                }

                Videowidth = 800;
                Videoheight = 600;
                VideoLeft = TmpPos.X;
                VideoTop = TmpPos.Y;


                //

                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly ||
                    GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge)
                {   //case GlobalSettings.MachineType.ProbeOnly: 
                    //case GlobalSettings.MachineType.HeightGauge:
                    LowerGrid2.Visibility = System.Windows.Visibility.Collapsed;
                    LowerGrid3.Visibility = System.Windows.Visibility.Collapsed;
                    FocusRBtn.Visibility = System.Windows.Visibility.Collapsed;
                    FixedGraphicsRBtn.Visibility = System.Windows.Visibility.Collapsed;
                    RapidCADRBtn.Visibility = System.Windows.Visibility.Collapsed;
                    LowerWinPanel3.Children.Remove(CNCRBtn);
                    LowerWinPanel1.Children.Add(CNCRBtn);
                    Video__Grid.VerticalAlignment = VerticalAlignment.Stretch;
                    Grid.SetRowSpan(Video__Grid, 2);

                    Video__Grid.Margin = new Thickness(4, 8, 8, 8);
                    RcadLeft = MainLayout.ColumnDefinitions[0].ActualWidth + Video__Grid.Margin.Left + 4;
                    RcadTop = Video__Grid.Margin.Top + Video__Grid.RowDefinitions[0].ActualHeight + ToolbarBorder_AboveVideo.Height + 10;
                    Rcadheight = Video__Grid.RowDefinitions[1].ActualHeight + MainLayout.RowDefinitions[1].ActualHeight - ToolbarBorder_AboveVideo.ActualHeight - 20; //  285 + VideoDisplayGrpBx.ActualHeight - TxtHelperGrid.ActualHeight - ToolbarBorder_AboveVideo.Height;
                    if (GlobalSettings.IsButtonWithNames)
                    {
                        Rcadheight -= 67; // = 600 - 67 + 285;
                        RcadTop += 67; // = TmpPos.Y + 67;
                    }
                    //else
                    //{
                    //    Rcadheight = 600 - 46 + 285;
                    //    RcadTop = TmpPos.Y + 46;
                    //}
                    //RcadLeft = TmpPos.X;
                    Rcadwidth = (int)VideoHolder.ActualWidth;

                    TmpPos = RapidCADHolder.TranslatePoint(new Point(0, 0), this);
                    PicViewerwidth = (int)RapidCADHolder.ActualWidth - 8;
                    PicViewerLeft = TmpPos.X + 4;
                    PicViewerheight = (int)RapidCADHolder.ActualHeight - 4;
                    PicViewerTop = TmpPos.Y;
                }
                else
                {
                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HobChecker && width < 1920)
                    {
                        LowerGrid1.Visibility = System.Windows.Visibility.Hidden;
                        LowerGrid2.Visibility = System.Windows.Visibility.Hidden;
                        //VideoFormsWin.Visible = false; 
                        HolderRBtn.Visibility = System.Windows.Visibility.Hidden;
                        PicViewerRBtn.Visibility = System.Windows.Visibility.Hidden;
                        RapidCADGrid.Visibility = System.Windows.Visibility.Hidden;
                        LowerGrid4.Visibility = System.Windows.Visibility.Hidden;
                    }

                    TmpPos = RapidCADHolder.TranslatePoint(new Point(0, 0), this);
                    if (GlobalSettings.IsButtonWithNames)
                    {
                        Rcadheight = (int)RapidCADHolder.ActualHeight - 4 - 78;
                        if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.VisionHeadOnly)
                            RcadTop = TmpPos.Y + 78;

                        PicViewerheight = Rcadheight + 78;
                        PicViewerTop = RcadTop - 78;
                    }
                    else
                    {
                        Rcadheight = (int)RapidCADHolder.ActualHeight - 4 - 57;
                        if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.VisionHeadOnly)
                            RcadTop = TmpPos.Y + 57;

                        PicViewerheight = Rcadheight + 57;
                        PicViewerTop = RcadTop - 57;
                    }
                    RcadLeft = TmpPos.X + 4;
                    Rcadwidth = (int)RapidCADHolder.ActualWidth - 8;

                    PicViewerwidth = Rcadwidth;
                    PicViewerLeft = RcadLeft;
                }

                TmpPos = DxfHolder.TranslatePoint(new Point(0, 0), this);
                if (GlobalSettings.IsButtonWithNames)
                {
                    DxfHeight = (int)DxfHolder.ActualHeight - 4 - 78;
                    DxfTop = TmpPos.Y + 78;
                }
                else
                {
                    DxfHeight = (int)DxfHolder.ActualHeight - 4 - 57;
                    DxfTop = TmpPos.Y + 57;
                }
                DxfWidth = (int)DxfHolder.ActualWidth - 8;
                DxfLeft = TmpPos.X + 4;

                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI && (width < 1920 || height < 1040))//This UI is exclusively for 1690 * 1080 or higher Size monitors.but it will also extend to any greater size,but not lower sizes.
                {
                    MessageBox.Show("Sorry, This machine type can only run on screen size of 1920 x 1080 or above.");
                }

                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)//This UI is exclusively for 1690 * 1080 or higher Size monitors.but it will also extend to any greater size,but not lower sizes.
                {
                    //this.Video__Grid.RowDefinitions[0].Height = new GridLength(118);
                    //this.TxtHelperGrid.Visibility = System.Windows.Visibility.Hidden;

                    //this.DropDownPanel.Children.Remove(SnapStateButton);
                    //this.DropDownPanel.Children.Remove(UCSList);
                    //this.DropDownPanel.Children.Remove(mmInchesList);
                    //this.DropDownPanel.Children.Remove(MagnificationList);
                    //this.DropDownPanel.Width = 0;
                    this.DropDownPanel.Visibility = System.Windows.Visibility.Collapsed;
                    double SpareWidth = SnapStateButton.Width + UCSList.Width + mmInchesList.Width + MagnificationList.Width;

                    this.DROPanel.Width = this.DROPanel.Width + SpareWidth;
                    DROPanel.Height = this.Video__Grid.RowDefinitions[0].ActualHeight - 12;

                    double FontSize = 25;
                    Button[] AxisButtons = { btnX, btnY, btnZ, btnR };
                    Grid[] DroGrids = { XdroGrid, YdroGrid, ZdroGrid, RdroGrid };
                    System.Windows.Style DROButtonStyle = (Style)TryFindResource("DROButton2ForDel_FI");
                    for (int i = 0; i < AxisButtons.Count(); i++)
                    {
                        AxisButtons[i].FontSize = FontSize;
                        if (AxisButtons[i] == btnR)
                            AxisButtons[i].Width = 170;
                        else
                            AxisButtons[i].Width = 160;
                        AxisButtons[i].Style = DROButtonStyle;
                    }

                    Button[] LoxkButton = { btnLockX, btnLockY, btnLockZ, btnLockR };
                    foreach (Button LockBtn in AxisButtons)
                    {
                        LockBtn.FontSize = FontSize;
                    }

                    SnapStateButton.Tag = new ButtonProperties(SnapStateButton.Name, false, false, "Rapidcad");
                }

                //Other Screen sizes & conditions
                //First the width
                if (width > 1920)
                {
                    resizeleft = true; resizeright = true;
                    if (width > 2048)
                        changeInLeft = changeInRight = (2048 - 1920) / 2;
                    else
                        changeInLeft = changeInRight = (width - 1920) / 2;
                }
                else if (width < 1920 && width >= 1720)
                {
                    resizeright = true;
                    changeInRight = width - 1920;
                }
                else if (width < 1720 && width >= 1660)
                {
                    resizeright = true; resizeleft = true;
                    changeInRight = +40;
                    changeInLeft = width - 1920 + 200;
                }
                else if (width < 1660 && width >= 1280)
                {
                    OtherWinToolbarRotated = true;
                    resizeleft = true;
                    changeInLeft = width - MainLayout.ColumnDefinitions[0].ActualWidth - MainLayout.ColumnDefinitions[1].ActualWidth - 75;
                }

                if (resizeleft)
                {
                    GridLength TmpL = new GridLength(this.MainLayout.ColumnDefinitions[0].ActualWidth + changeInLeft);
                    this.MainLayout.ColumnDefinitions[0].Width = TmpL;
                    VideoLeft += changeInLeft;
                    RcadLeft += changeInLeft;
                    //int itemscnt = LowerWinPanel1.Children.Count;
                    //double itemsChange = changeInLeft / itemscnt;
                    //for (int i = 0; i < itemscnt; i++)
                    //    ((ToggleButton)LowerWinPanel1.Children[i]).Width += itemsChange;

                }
                if (resizeright)
                {
                    GridLength TmpL = new GridLength(this.MainLayout.ColumnDefinitions[2].ActualWidth + changeInRight);
                    this.MainLayout.ColumnDefinitions[2].Width = TmpL;
                    Rcadwidth += changeInRight;
                    DxfWidth += changeInRight;
                    //int itemscnt = OtherWinPanel.Children.Count;
                    //double itemsChange = changeInRight / itemscnt;
                    //for (int i = 0; i < itemscnt; i++)
                    //    ((ToggleButton)OtherWinPanel.Children[i]).Width += itemsChange;
                }

                //if (!OtherWinToolbarRotated && (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge))
                //{
                //    Grid.SetRowSpan(Video__Grid, 1);
                //    Video__Grid.Margin = new Thickness(5, 8, 10, 16);
                //}

                if (OtherWinToolbarRotated)
                {
                    TheNewRearrangement();

                    //Rotate the panel
                    RapidCADGrid.Margin = new Thickness(5, 8, 16, 70);
                    RapidCADGrid.Children.Remove(OtherWinPanel);
                    OtherWinPanelBorder.Child = OtherWinPanel;
                    OtherWinPanel.Width = double.NaN;
                    OtherWinPanel.HorizontalAlignment = System.Windows.HorizontalAlignment.Stretch;
                    Grid.SetRow(OtherWinPanelBorder, 2);
                    //OtherWinPanelBorder.Margin = new Thickness(15, VideoDisplayGrpBx.Margin.Top, 0, 0);
                    OtherWinPanelBorder.Margin = new Thickness(0, 0, 0, 0);
                    OtherWinPanelBorder.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
                    OtherWinPanelBorder.VerticalAlignment = System.Windows.VerticalAlignment.Top;
                    OtherWinPanelBorder.Width = VideoDisplayGrpBx.ActualHeight - 10;
                    OtherWinPanelBorder.LayoutTransform = new RotateTransform(90);
                    OWinPanelShadow.Direction = 45;

                    //Hide appropriate controls
                    RapidCADHolder.Visibility = System.Windows.Visibility.Hidden;
                    LowerGrid4.Visibility = System.Windows.Visibility.Hidden;
                    ShapesNMeasurementsVisible = false;
                    Rcadheight = 267;
                    Rcadwidth = SystemParameters.WorkArea.Width - 1 - 75 - 863 - 8;
                    Rcadheight = 267;
                    RcadTop = Tools_Grid.RowDefinitions[0].Height.Value + 10 + 48 + 4;
                    RcadLeft = 857;
                    DxfWidth = Rcadwidth;
                    DxfHeight = 280;
                    DxfTop = RcadTop + Rcadheight + 48 + 8;
                    DxfLeft = RcadLeft;

                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly ||
                        GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge)
                    {

                        DxfHeight += (DxfTop - RcadTop);
                        DxfTop = RcadTop;
                        if (GlobalSettings.IsButtonWithNames)
                        {
                            DxfHeight -= 21;
                            DxfTop += 21;
                            RcadTop = VideoTop + 67;
                            Rcadheight = Videoheight - 67;
                        }
                        else
                        {
                            RcadTop = VideoTop + 46;
                            Rcadheight = Videoheight - 46;
                        }
                        RcadLeft = VideoLeft;
                        Rcadwidth = Videowidth;
                    }

                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HobChecker)
                        Set_HobChecker_UI();

                    RapidCADGrid.Children.Remove(ToolbarBorder);
                    Video__Grid.Children.Add(ToolbarBorder);

                    //get the shapes and measurements windows to appear with the other windows
                    ShpsNMeasurementsTBtn = new ToggleButton();
                    ShpsNMeasurementsTBtn.Style = RapidCADRBtn.Style;
                    OtherWinPanel.Children.Add(ShpsNMeasurementsTBtn);
                    ShpsNMeasurementsTBtn.Width = 230;
                    ShpsNMeasurementsTBtn.FontSize = RapidCADRBtn.FontSize;
                    ShpsNMeasurementsTBtn.Content = "Shapes, Measurements & Actions";
                    ShpsNMeasurementsTBtn.Tag = "Shapes, Measurements & Actions";
                    ShpsNMeasurementsTBtn.ToolTip = "Shapes, Measurements and Actions";
                    ShpsNMeasurementsTBtn.Checked += new RoutedEventHandler(OtherWindowsTBtn_Checked);
                    ShpsNMeasurementsTBtn.Unchecked += new RoutedEventHandler(OtherWindowsTBtn_Unchecked);
                }


                //Next the height
                if (height < 1024 && height >= 760)
                {
                    //Set the minimized flag
                    LowerPanelsMinimized = true;

                    //Hide necessary panels
                    LowerPanelHolder1.Visibility = System.Windows.Visibility.Hidden;
                    LowerPanelHolder2.Visibility = System.Windows.Visibility.Hidden;
                    if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HobChecker)
                    {
                        LowerPanelHolder3.Visibility = System.Windows.Visibility.Hidden;
                        LowerGrid3.Margin = new Thickness(5, 0, 10, 0);
                    }
                    LowerGrid1.Margin = new Thickness(5, 0, 10, 0);
                    LowerGrid2.Margin = new Thickness(5, 0, 10, 0);
                    int hju = Grid.GetRow(LowerGrid2);
                    if (height < 795)
                    {
                        Tools_Grid.RowDefinitions[0].Height = new GridLength(Tools_Grid.RowDefinitions[0].ActualHeight - 10);
                        Video__Grid.RowDefinitions[0].Height = new GridLength(Video__Grid.RowDefinitions[0].ActualHeight - 6);
                        Video__Grid.RowDefinitions[1].Height = new GridLength(Video__Grid.RowDefinitions[1].ActualHeight - 4);
                        RapidCADGrid.RowDefinitions[0].Height = new GridLength(RapidCADGrid.RowDefinitions[0].ActualHeight - 10);
                        //TxtHelperGrid.Height = TxtHelperGrid.ActualHeight + 10; // - 10;
                        VideoDisplayGrpBx.VerticalAlignment = VerticalAlignment.Stretch;
                        Video__Grid.VerticalAlignment = VerticalAlignment.Stretch;
                        Grid.SetRow(gd_MessageAndVideoGrid, 0);
                        gd_MessageAndVideoGrid.Margin = new Thickness(0); gd_MessageAndVideoGrid.VerticalAlignment = VerticalAlignment.Stretch;
                        gd_MessageAndVideoGrid.RowDefinitions[0].Height = new GridLength(gd_MessageAndVideoGrid.RowDefinitions[0].ActualHeight - 6);
                        MainLayout.RowDefinitions[0].Height = new GridLength(MainLayout.RowDefinitions[0].ActualHeight - 12);
                        VideoDisplayGrpBx.Height = Video__Grid.RowDefinitions[1].ActualHeight - 12;

                        LowerGrid1.RowDefinitions[0].Height = new GridLength(LowerGrid1.RowDefinitions[0].ActualHeight - 10);
                        LowerGrid2.RowDefinitions[0].Height = new GridLength(LowerGrid2.RowDefinitions[0].ActualHeight - 10);
                        if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HobChecker)
                        {
                            LowerGrid3.RowDefinitions[0].Height = new GridLength(LowerGrid3.RowDefinitions[0].ActualHeight - 10);
                            LowerGrid1.VerticalAlignment = VerticalAlignment.Top;
                            LowerGrid2.VerticalAlignment = VerticalAlignment.Top;
                            LowerGrid3.VerticalAlignment = VerticalAlignment.Top;
                        }

                        if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge)
                        {
                            VideoTop -= 20;
                            RcadTop -= 20;
                            ToolbarBorder_AboveVideo.Margin = new Thickness(2, 40, 2, 0);
                            if (OtherWinToolbarRotated)
                            {
                                DxfTop -= 10;
                                DxfHeight -= 10;
                            }
                        }
                        else
                        {
                            VideoTop -= 20;
                            RcadTop -= 20;
                            if (OtherWinToolbarRotated)
                            {
                                //VideoTop -= 12;
                                RcadTop += 8;
                                //Rcadheight -= 5;
                                DxfTop -= 12;
                                //DxfHeight -= 6;
                            }
                            if (GlobalSettings.IsButtonWithNames)
                            {
                                RcadTop += 20;
                                Rcadheight -= 20;
                                DxfTop += 20;
                                DxfHeight -= 20;
                                RcadToolbarBorder.Height += 20;
                                DxfToolbarBorder.Height += 20;

                            }
                        }
                    }
                }
                ToggleButton[] Btns = { CNCRBtn, ThreadRBtn };
                String[] Tips1 = { "CNC Panel", "Thread Panel" };
                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge)
                {
                    Grid.SetRowSpan(VideoDisplayGrpBx, 2);
                    for (int i = 0; i < Btns.Length; i++)
                    {
                        Btns[i].ToolTip = Tips1[i];

                        Btns[i].Checked += new RoutedEventHandler(LowerGroupTab2TBtn_Checked);
                        Btns[i].Unchecked += new RoutedEventHandler(LowerGroupTab2TBtn_Unchecked);
                        Btns[i].PreviewMouseLeftButtonDown += new MouseButtonEventHandler(LowerGroupTabTBtn_PreviewMouseLeftButtonDown);
                    }

                    CNCRBtn.MouseRightButtonUp += new MouseButtonEventHandler(CNCRBtn_MouseRightButtonUp);
                    if (!LowerPanelsMinimized)
                    {
                        ToggleButton[] Btns2 = { ShapesRBtn, MeasurementsRBtn, DXFTBtn, ActionsRBtn };
                        String[] Tips3 = { "Shapes Panel", "Measurement Panel", "DXF Window", "Actions Panel" };
                        for (int i = 0; i < Btns2.Length; i++)
                        {
                            Btns2[i].ToolTip = Tips3[i];
                            Btns2[i].Checked += new RoutedEventHandler(LowerGroupTab3TBtn_Checked);
                            Btns2[i].PreviewMouseLeftButtonDown += new MouseButtonEventHandler(LowerGroupTabTBtn_PreviewMouseLeftButtonDown);
                        }
                    }
                }
                else
                {

                    switch (GlobalSettings.RapidCameraType)
                    {
                        case GlobalSettings.CameraType.ThreeM:
                            TempHeight = 168;
                            TempWidth = 224;
                            OtherWinPanelWidth = 440;
                            NewVideoWidth = 1024;
                            NewVideoHeight = 768;
                            bdrPPRightMargin = 155;
                            FullVideoWidth = 2048;
                            FullVideoHeight = 1536;

                            break;
                        case GlobalSettings.CameraType.TenM:
                            TempHeight = 87;
                            TempWidth = 116;
                            OtherWinPanelWidth = 548;
                            NewVideoWidth = 916;
                            NewVideoHeight = 687;
                            bdrPPRightMargin = 195;
                            FullVideoWidth = 3664;
                            FullVideoHeight = 2748;
                            break;
                        case GlobalSettings.CameraType.FiveFourtyP:
                            TempHeight = 0; // -60;
                            TempWidth = 160;
                            OtherWinPanelWidth = 504;
                            NewVideoWidth = 960;
                            NewVideoHeight = 540; //600; // 
                            bdrPPRightMargin = 195;
                            FullVideoWidth = 1920;
                            FullVideoHeight = 1080; //1200; //
                            LowerGrid2.Width += 52;
                            break;
                        case GlobalSettings.CameraType.SevenTwentyP:
                            TempHeight = 120;
                            TempWidth = 480;
                            OtherWinPanelWidth = 604;
                            NewVideoWidth = 1280;
                            NewVideoHeight = 720;
                            bdrPPRightMargin = 155;
                            FullVideoHeight = 720;
                            FullVideoWidth = 1280;
                            break;
                        case GlobalSettings.CameraType.ThreeSixtyP:
                            TempHeight = -240;
                            TempWidth = -160;
                            OtherWinPanelWidth = 504;
                            NewVideoWidth = 960; // 640;
                            NewVideoHeight = 540; // 360;
                            bdrPPRightMargin = 155;
                            FullVideoWidth = 1280;
                            FullVideoHeight = 720;
                            break;
                        case GlobalSettings.CameraType.TwoM_Double:
                            TempHeight = -240;
                            TempWidth = -160;
                            OtherWinPanelWidth = 504;
                            NewVideoWidth = 800;
                            NewVideoHeight = 600;
                            FullVideoWidth = 1600;
                            FullVideoHeight = 1200;
                            //GlobalSettings.RapidCameraType = GlobalSettings.CameraType.TwoM;
                            break;

                        case GlobalSettings.CameraType.TenEightyP:
                            TempHeight = 300;
                            TempWidth = 1100;
                            OtherWinPanelWidth = 504;
                            NewVideoWidth = 1920;
                            NewVideoHeight = 1080;
                            FullVideoWidth = 1920;
                            FullVideoHeight = 1080;
                            //GlobalSettings.RapidCameraType = GlobalSettings.CameraType.TwoM;
                            break;

                        default:
                            break;
                    }
                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.One_Shot || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.OnlineSystem)
                    {
                        if (height >= 1040 && width >= 1680)
                        {
                            OneShot_NormalSreenSize(width);
                        }
                        else if (OtherWinToolbarRotated && LowerPanelsMinimized)
                        {
                            OneShot_SmallScreenSize();
                        }
                        else
                            MessageBox.Show("Sorry, The screen size is not supported for the current camera resolution.");
                    }
                    //if (Videoheight > GlobalSettings.VideoHeightThresholdForFrontEndLayout)
                    if (GlobalSettings.RapidCameraType != GlobalSettings.CameraType.TwoM && GlobalSettings.RapidCameraType != GlobalSettings.CameraType.TwoM_Double)
                    {
                        if (height >= 1040 && width >= 1680)
                            GenericMCam_NormalScreenSize(TempHeight, TempWidth, OtherWinPanelWidth, NewVideoWidth, NewVideoHeight);
                        else
                            GenericMCam_SmallScreenSize(TempHeight, TempWidth, OtherWinPanelWidth, NewVideoWidth, NewVideoHeight);
                    }

                    if (Videoheight > GlobalSettings.VideoHeightThresholdForFrontEndLayout && OtherWinToolbarRotated && LowerPanelsMinimized)
                    {
                        VideoHolder.HookupEventsAndInitialise((int)NewVideoWidth, (int)NewVideoHeight, (int)NewVideoWidth, (int)NewVideoHeight, CameraIsOK);
                    }
                    else
                    {
                        if (GlobalSettings.RapidCameraType == GlobalSettings.CameraType.TwoM)
                            VideoHolder.HookupEventsAndInitialise((int)Videowidth, (int)Videoheight, (int)Videowidth, (int)Videoheight, CameraIsOK);
                        else
                            VideoHolder.HookupEventsAndInitialise((int)Videowidth, (int)Videoheight, (int)FullVideoWidth, (int)FullVideoHeight, CameraIsOK);
                    }
                    if (GlobalSettings.RapidCameraType == GlobalSettings.CameraType.TwoM_Double && CameraIsOK)
                    {
                        //MessageBox.Show("Resetting CameraType");
                        GlobalSettings.RapidCameraType = GlobalSettings.CameraType.TwoM;
                    }
                }

                if (width < 1280 || height < 760)
                {
                    if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HobChecker)
                        MessageBox.Show("Sorry, The screen size is too small and is not supported in the current version.");
                }

                ToggleButton[] Btn = { RapidCADRBtn, ProgramRBtn, PicViewerRBtn };
                String[] Tips = { "RapidCAD Window", "Part Program Window", "Picture Viewer" };
                for (int i = 0; i < Btn.Length; i++)
                {
                    Btn[i].ToolTip = Tips[i];

                    Btn[i].Checked += new RoutedEventHandler(OtherWindowsTBtn_Checked);

                    //Btn[i].MouseUp += new MouseButtonEventHandler(OtherWindowsTBtn_Checked);
                    Btn[i].Unchecked += new RoutedEventHandler(OtherWindowsTBtn_Unchecked);
                    Btn[i].PreviewMouseLeftButtonDown += new MouseButtonEventHandler(OtherWindowsTBtn_PreviewMouseLeftButtonDown);
                }
                //RapidCADRBtn.LayoutUpdated += new EventHandler(RcadTBtn_LayoutUpdated);
                //DXFRBtn.LayoutUpdated += new EventHandler(dxfTBtn_LayoutUpdated);
                //RapidCADGrid.LayoutUpdated += new EventHandler(RcadGrid_LayoutUpdated);
                //this.LayoutUpdated += new EventHandler(WholeWin_LayoutUpdated);

                ToggleButton[] Btn2 = { FocusRBtn, ThreadRBtn, FixedGraphicsRBtn }; // 
                //String[]  Tips1 = { "Focus Panel", "Thread Panel", "Fixed Graphics Panel" }; //
                for (int i = 0; i < Btn2.Length; i++)
                {
                    //Btns[i].ToolTip = Tips1[i];

                    Btn2[i].Checked += new RoutedEventHandler(LowerGroupTab1TBtn_Checked);
                    Btn2[i].Unchecked += new RoutedEventHandler(LowerGroupTab1TBtn_Unchecked);
                    Btn2[i].PreviewMouseLeftButtonDown += new MouseButtonEventHandler(LowerGroupTabTBtn_PreviewMouseLeftButtonDown);
                }


                if (LowerPanelsMinimized)
                {
                    ToggleButton[] Btns1 = { LightingRBtn, CNCRBtn, SAOIRBtn, HolderRBtn };
                    String[] Tips2 = { "Lighting Panel", "CNC Panel", "SAOI Panel", "Holder Panel" };
                    for (int i = 0; i < Btns1.Length; i++)
                    {
                        if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HobChecker && i == 1) continue;
                        Btns1[i].ToolTip = Tips2[i];
                        Btns1[i].Checked += new RoutedEventHandler(LowerGroupTab4TBtn_Checked);
                        Btns1[i].Unchecked += new RoutedEventHandler(LowerGroupTab4TBtn_Unchecked);
                        //Btns1[i].PreviewMouseLeftButtonDown += new MouseButtonEventHandler(LowerGroupTabTBtn_PreviewMouseLeftButtonDown);
                    }
                    CNCRBtn.MouseRightButtonUp += new MouseButtonEventHandler(CNCRBtn_MouseRightButtonUp);
                }
                else if (Videoheight > GlobalSettings.VideoHeightThresholdForFrontEndLayout) // GlobalSettings.RapidCameraType != GlobalSettings.CameraType.TwoM )
                {
                    ToggleButton[] Btns1 = { LightingRBtn, CNCRBtn, HolderRBtn };
                    String[] Tips2 = { "Lighting Panel", "CNC Panel", "Holder Panel" };
                    for (int i = 0; i < Btns1.Length; i++)
                    {
                        Btns1[i].ToolTip = Tips2[i];

                        Btns1[i].Checked += new RoutedEventHandler(LowerGroupTab4TBtn_Checked);
                        Btns1[i].Unchecked += new RoutedEventHandler(LowerGroupTab4TBtn_Unchecked);
                        //Btns1[i].PreviewMouseLeftButtonDown += new MouseButtonEventHandler(LowerGroupTabTBtn_PreviewMouseLeftButtonDown);
                    }
                    CNCRBtn.MouseRightButtonUp += new MouseButtonEventHandler(CNCRBtn_MouseRightButtonUp);

                    ToggleButton[] Btns2 = { ShapesRBtn, MeasurementsRBtn, DXFTBtn, ActionsRBtn };
                    String[] Tips3 = { "Shapes Panel", "Measurement Panel", "DXF Window", "Actions Panel" };
                    for (int i = 0; i < Btns2.Length; i++)
                    {
                        Btns2[i].ToolTip = Tips3[i];
                        Btns2[i].Checked += new RoutedEventHandler(LowerGroupTab3TBtn_Checked);
                        Btns2[i].PreviewMouseLeftButtonDown += new MouseButtonEventHandler(LowerGroupTabTBtn_PreviewMouseLeftButtonDown);
                    }
                }
                else
                {
                    CNCRBtn.PreviewMouseLeftButtonDown += new MouseButtonEventHandler(CNCRBtn_MouseLeftButtonDown);
                    HolderRBtn.PreviewMouseLeftButtonDown += new MouseButtonEventHandler(HolderRBtn_PreviewMouseLeftButtonDown);

                    ToggleButton[] Btns2 = { ShapesRBtn, MeasurementsRBtn, DXFTBtn, ActionsRBtn };
                    String[] Tips3 = { "Shapes Panel", "Measurement Panel", "DXF Window", "Actions Panel" };
                    for (int i = 0; i < Btns2.Length; i++)
                    {
                        Btns2[i].ToolTip = Tips3[i];
                        Btns2[i].Checked += new RoutedEventHandler(LowerGroupTab3TBtn_Checked);
                        Btns2[i].PreviewMouseLeftButtonDown += new MouseButtonEventHandler(LowerGroupTabTBtn_PreviewMouseLeftButtonDown);
                    }
                }

                TxtHelper.TextWrapping = TextWrapping.Wrap;

                InitializeWindows(RcadWin, Rcadheight, Rcadwidth, null, true, RcadLeft, RcadTop, true);
                InitializeWindows(DxfWin, DxfHeight, DxfWidth, null, true, DxfLeft, DxfTop, true);

                if (OtherWinToolbarRotated)
                {
                    InitializeWindows(PictureViewerWin, Videoheight + 48, Videowidth, this, true, VideoLeft - 4, VideoTop - 24, false);
                }
                else
                {
                    InitializeWindows(PictureViewerWin, PicViewerheight, PicViewerwidth, this, true, PicViewerLeft, PicViewerTop, false);
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV03", ex); }
        }
        void TheNewRearrangement()
        {
            try
            {
                Grid.SetColumn(Video__Grid, 0);
                Grid.SetColumnSpan(Video__Grid, 2);
                Grid.SetRowSpan(Video__Grid, 1);
                Grid.SetRow(Video__Grid, 0);
                //Video__Grid.RowDefinitions[1].Height = new GridLength(Video__Grid.ActualHeight - Video__Grid.RowDefinitions[0].ActualHeight);
                VideoDisplayGrpBx.VerticalAlignment = System.Windows.VerticalAlignment.Stretch;
                Video__Grid.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
                Video__Grid.Margin = new Thickness(30, 4, 0, 4);
                //Video__Grid.Width = 804;
                Grid.SetColumn(Tools_Grid, 1);
                Tools_Grid.HorizontalAlignment = System.Windows.HorizontalAlignment.Right;
                Tools_Grid.Width = SystemParameters.WorkArea.Width - 1 - 75 - 863;
                Tools_Grid.Margin = new Thickness(0, 8, 10, 10);
                RcadLeft = VideoLeft = 38;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV04", ex);
            }
        }

        void TheNewRearrangement1()
        {
            try
            {
                ToolBoxHolder = new LowerPanelWindow();
                ToolBoxHolder.IsAllowFading = true;
                SetOwnerofWindow(ToolBoxHolder);
                //ToolBoxHolder.MouseLeave += new MouseEventHandler(ToolBoxHolder_MouseLeave);
                if (GlobalSettings.IsButtonWithNames)
                    ToolBoxHolder.Width = 500; // 340;
                else
                    ToolBoxHolder.Width = 410; // 250;
                ToolBoxHolder.Height = 680;
                ToolBoxHolder.Left = 25;
                ToolBoxHolder.Top = 65;
                Tools_Grid.Children.Remove(LeftPanelGrid);
                ToolBoxHolder.LowerPanelWinContainer.Content = LeftPanelGrid;
                //if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.VisionHeadOnly)
                //{
                //    RotateTransform rt = new RotateTransform(90.0, 0.0, ToolBxBtn.ActualHeight);
                //    ToolBxBtn.RenderTransform = rt;
                //}
                ToolBarStkPanel.Visibility = System.Windows.Visibility.Visible;
                ToolBxBtn.MouseEnter += new MouseEventHandler(ToolBxBtn_MouseEnter);
                ToolBxBtn.Click += new RoutedEventHandler(ToolBxBtn_MouseEnter);

                PPToolBoxHolder = new LowerPanelWindow();
                PPToolBoxHolder.IsAllowFading = true;
                //PPToolBoxHolder.Owner = VideoWin;
                //PPToolBoxHolder.MouseLeave += new MouseEventHandler(ToolBoxHolder_MouseLeave);
                PPToolBoxHolder.Width = 700;
                if (GlobalSettings.IsButtonWithNames)
                {
                    PPToolBoxHolder.Height = 100;
                    PPToolBoxHolder.Top = MainLayout.RowDefinitions[0].ActualHeight - 120;
                }
                else
                {
                    PPToolBoxHolder.Height = 80;
                    PPToolBoxHolder.Top = MainLayout.RowDefinitions[0].ActualHeight - 100;
                }
                PPToolBoxHolder.Left = MainLayout.ColumnDefinitions[0].ActualWidth + MainLayout.ColumnDefinitions[1].ActualWidth - 720;
                MainLayout.Children.Remove(PPToolbarGrid);
                PPToolbarGrid.Margin = new Thickness(0);
                PPToolbarGrid.VerticalAlignment = System.Windows.VerticalAlignment.Stretch;
                PPToolBoxHolder.LowerPanelWinContainer.Content = PPToolbarGrid;
                PPToolBxBtn.Visibility = System.Windows.Visibility.Visible;
                PPToolBxBtn.MouseEnter += new MouseEventHandler(ToolBxBtn_MouseEnter);

                OtherWinPanel.Children.Remove(RapidCADRBtn);
                //OtherWinPanel.Children.Remove(DXFRBtn);
                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HobChecker)
                    PPToolBxBtn.Visibility = System.Windows.Visibility.Hidden;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV05", ex);
            }
        }

        void GenericMCam_NormalScreenSize(double tempHeight, double tempWidth, double otherWinPanelWidth, double newVideoWidth, double newVideoHeight)
        {
            //Set the size of the video display and RCAD windows
            //Video__Grid.VerticalAlignment = System.Windows.VerticalAlignment.Top;
            try
            {
                Video__Grid.Height = Video__Grid.ActualHeight + tempHeight;
                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.One_Shot || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.OnlineSystem)
                {
                    Video__Grid.VerticalAlignment = VerticalAlignment.Top;
                    Video__Grid.Margin = new Thickness(4, 8, 8, 16);
                    Grid.SetRowSpan(Video__Grid, 3);
                    //Grid.SetColumnSpan(Video__Grid, 1);
                    LowerPanelHolder2.Visibility = System.Windows.Visibility.Hidden;
                    LowerPanelHolder3.Visibility = System.Windows.Visibility.Hidden;
                    LowerGrid2.Margin = new Thickness(5, tempHeight, 10, 0);
                    LowerGrid3.Margin = new Thickness(5, tempHeight, 10, 0);
                }
                else
                {
                    if (GlobalSettings.RapidCameraType == GlobalSettings.CameraType.TenM)
                    {
                        this.MainLayout.RowDefinitions[0].Height = new GridLength(780);
                        Grid.SetRow(VideoHolder, 0);
                        Grid.SetRowSpan(VideoHolder, 2);
                        TxtHelperGrid.Children.Remove(TxtGridRunCount);
                        LowerWinPanel3.Children.Add(TxtGridRunCount);
                        TxtGridRunCount.HorizontalAlignment = HorizontalAlignment.Right;
                        TxtGridRunCount.Foreground = new SolidColorBrush(Colors.White);
                        Rcadheight += 28;
                        bdDisplay.Visibility = Visibility.Hidden;
                    }
                }
                //else
                //    Video__Grid.Height = ;
                GridLength TmpL = new GridLength(this.MainLayout.ColumnDefinitions[1].ActualWidth + tempWidth);
                this.MainLayout.ColumnDefinitions[1].Width = TmpL;
                //Video__Grid.Width = this.MainLayout.ColumnDefinitions[1].ActualWidth + tempWidth;
                TmpL = new GridLength(0);
                if (this.MainLayout.ColumnDefinitions[2].ActualWidth - tempWidth >= 0)
                    TmpL = new GridLength(this.MainLayout.ColumnDefinitions[2].ActualWidth - tempWidth);        //1 Written by Harphool

                this.MainLayout.ColumnDefinitions[2].Width = TmpL;                                          //2

                OtherWinPanel.Width = otherWinPanelWidth;
                //Reduce the size of Toggle buttons
                RapidCADRBtn.Width = RapidCADRBtn.ActualWidth - 30;
                //DXFRBtn.Width = DXFRBtn.ActualWidth - 85;
                ProgramRBtn.Width = ProgramRBtn.ActualWidth - 10;
                PicViewerRBtn.Width = PicViewerRBtn.ActualWidth - 20;
                //PicViewerRBtn.Visibility = System.Windows.Visibility.Collapsed;

                //Point TmpPos1 = VideoHolder.TranslatePoint(new Point(0, 0), this);
                Videowidth = newVideoWidth;
                Videoheight = newVideoHeight;
                //VideoLeft = TmpPos1.X;
                //VideoTop = TmpPos1.Y;
                //TmpPos1 = RapidCADHolder.TranslatePoint(new Point(0, 0), this);
                if (Rcadwidth - tempWidth > 10)
                    Rcadwidth -= tempWidth;
                //Rcadheight = RapidCADHolder.Height - 4;
                if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.VisionHeadOnly)
                    RcadLeft += tempWidth;

                if (DxfWidth - tempWidth > 10)
                    DxfWidth -= tempWidth;
                DxfLeft += tempWidth;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV03a", ex);
            }
        }

        void GenericMCam_SmallScreenSize(double tempHeight, double tempWidth, double otherWinPanelWidth, double newVideoWidth, double newVideoHeight)
        {
            Videowidth = newVideoWidth;
            Videoheight = newVideoHeight;
            //ScrollBarHeight=20.0; ScrollBarWidth=20.0;
            //if((int)Videoheight<600)
            //{
            //ScrollBarWidth = 0.0;
            Video__Grid.Margin = new Thickness(30, 8, 0, 610 - (int)Videoheight);
            // }
            //if ((int)Videowidth < 800)
            //{
            //ScrollBarHeight = 0.0;
            //Video__Grid.Width = Videowidth + 4; // 16.0;
            Tools_Grid.Width = SystemParameters.WorkArea.Width - 1 - 75 - (Videowidth + 63);
            Rcadwidth -= tempWidth;
            RcadLeft += tempWidth;
            DxfWidth -= tempWidth;
            DxfLeft += tempWidth;
            //}
        }

        void OneShot_NormalSreenSize(double width)
        {
            //Hide panels that are not required
            LowerGrid2.Visibility = System.Windows.Visibility.Hidden;
            LowerGrid3.Visibility = System.Windows.Visibility.Hidden;
            GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Lighting", "Window", 32, 32);
            FocusRBtn.Tag = "Lighting";
            FocusRBtn.Content = GlobalSettings.SettingsReader.ButtonToolTip;
            FocusRBtn.IsChecked = true;
            //FocusRBtn.IsEnabled = false;
            //ThreadRBtn.Visibility = System.Windows.Visibility.Hidden;
            //FixedGraphicsRBtn.Visibility = System.Windows.Visibility.Hidden;

            //Set the DRO buttons panel
            double TmpWidth = 150 / 4;
            ZdroGrid.Visibility = System.Windows.Visibility.Hidden;
            XdroGrid.Margin = new Thickness(XdroGrid.Margin.Left + TmpWidth, XdroGrid.Margin.Top, XdroGrid.Margin.Right + TmpWidth, XdroGrid.Margin.Bottom);
            YdroGrid.Margin = new Thickness(YdroGrid.Margin.Left + TmpWidth, YdroGrid.Margin.Top, YdroGrid.Margin.Right + TmpWidth, YdroGrid.Margin.Bottom);

            if (width == 1680)
            {
                int WidthDiff = 100;
                GridLength TmpLn = new GridLength(this.MainLayout.ColumnDefinitions[0].ActualWidth - WidthDiff);
                this.MainLayout.ColumnDefinitions[0].Width = TmpLn;

                VideoLeft -= WidthDiff;
                RcadLeft -= WidthDiff;
                Rcadwidth += WidthDiff;
            }
            //Arranging upper scroll controls
            TmpWidth = TmpWidth / 2;
            DropDownPanel.Width = DropDownPanel.ActualWidth + 224;
            //DropDownPanel.Margin = new Thickness(DropDownPanel.Margin.Left, DropDownPanel.Margin.Top, DropDownPanel.Margin.Right - 224, DropDownPanel.Margin.Bottom);
            MagnificationList.Margin = new Thickness(MagnificationList.Margin.Left + TmpWidth, MagnificationList.Margin.Top, MagnificationList.Margin.Right + TmpWidth, MagnificationList.Margin.Bottom);
            mmInchesList.Margin = new Thickness(mmInchesList.Margin.Left + TmpWidth, mmInchesList.Margin.Top, mmInchesList.Margin.Right + TmpWidth, mmInchesList.Margin.Bottom);
            UCSList.Margin = new Thickness(UCSList.Margin.Left + TmpWidth, UCSList.Margin.Top, UCSList.Margin.Right + TmpWidth, UCSList.Margin.Bottom);
            SnapStateButton.Margin = new Thickness(SnapStateButton.Margin.Left + TmpWidth, SnapStateButton.Margin.Top, SnapStateButton.Margin.Right + TmpWidth, SnapStateButton.Margin.Bottom);
        }

        void OneShot_SmallScreenSize()
        {
            //LowerPanelHeader2.Visibility = System.Windows.Visibility.Hidden;
            LowerGrid3.Visibility = System.Windows.Visibility.Hidden;
            LowerWinPanel1.Children.Remove(FocusRBtn);
            //FocusRBtn.Visibility = System.Windows.Visibility.Hidden;

            //Set the DRO buttons panel
            double TmpWidth = 150 / 4;
            ZdroGrid.Visibility = System.Windows.Visibility.Hidden;
            XdroGrid.Margin = new Thickness(XdroGrid.Margin.Left + TmpWidth, XdroGrid.Margin.Top, XdroGrid.Margin.Right + TmpWidth, XdroGrid.Margin.Bottom);
            YdroGrid.Margin = new Thickness(YdroGrid.Margin.Left + TmpWidth, YdroGrid.Margin.Top, YdroGrid.Margin.Right + TmpWidth, YdroGrid.Margin.Bottom);
        }

        void Set_HobChecker_UI()
        {
            try
            {
                this.MainLayout.ColumnDefinitions[2].Width = new GridLength(4, GridUnitType.Pixel);

                LowerGrid4.Visibility = System.Windows.Visibility.Hidden;
                RapidCADGrid.Margin = new Thickness(5, 8, 8, 60);

                //////////this.MainBorder.Width = this.Width;
                //////////this.MainLayout.Width = this.Width;

                this.MainLayout.ColumnDefinitions[0].Width = new GridLength(0.62, GridUnitType.Star);
                this.MainLayout.ColumnDefinitions[1].Width = new GridLength(0.38, GridUnitType.Star); //this.Width - MainLayout.ColumnDefinitions[0].ActualWidth);

                LowerGrid3.SetValue(Grid.RowProperty, 0);
                LowerGrid3.Visibility = System.Windows.Visibility.Visible;
                LowerGrid3.SetValue(Grid.HorizontalAlignmentProperty, HorizontalAlignment.Left);
                LowerGrid3.SetValue(Grid.VerticalAlignmentProperty, VerticalAlignment.Bottom);
                LowerGrid3.SetValue(Grid.MarginProperty, new Thickness(4, 0, 8, 30));//Rcadheight + RcadToolbarBorder.Height + 16
                LowerGrid3.Width = MainLayout.ColumnDefinitions[1].Width.Value * MainLayout.ActualWidth - 16;

                Tools_Grid.SetValue(Grid.HorizontalAlignmentProperty, HorizontalAlignment.Stretch);
                Tools_Grid.SetValue(Grid.VerticalAlignmentProperty, VerticalAlignment.Top);
                Tools_Grid.Margin = new Thickness(4, 4, 8, 0);
                Tools_Grid.Width = LowerGrid3.Width;
                Tools_Grid.Height = 436; // MainLayout.RowDefinitions[0].ActualHeight - LowerGrid3.ActualHeight - Tools_Grid.Margin.Top - LowerGrid3.Margin.Bottom - 36;
                Video__Grid.Height = Video__Grid.ActualHeight - 16;
                Video__Grid.Margin = new Thickness(28, 4, 0, 20);


                DxfToolbarBorder.Visibility = System.Windows.Visibility.Hidden;
                DxfLowerToolbarBorder.Visibility = System.Windows.Visibility.Hidden;

                Rcadwidth = Tools_Grid.Width - Tools_Grid.Margin.Left - 8;// Tools_Grid.Margin.Right - 4;
                Rcadheight = Tools_Grid.Height - RcadToolbarBorder.Height - Tools_Grid.RowDefinitions[0].ActualHeight - 8;//gb_ToolsGrid.Height - RcadToolbarBorder.Height;

                RcadLeft = MainLayout.ColumnDefinitions[0].Width.Value * MainLayout.ActualWidth + Tools_Grid.Margin.Left + 4;
                RcadTop = Tools_Grid.Margin.Top + RcadToolbarBorder.Height + Tools_Grid.RowDefinitions[0].ActualHeight + 4;
                //The line below causes combobox dropdown to be drawn at the topleft.
                //RapidCADGrid.Width = Rcadwidth;

                DxfWidth = Rcadwidth;

                RapidCADHolder.Height = Rcadheight + 4;

                btnR.Width = 140;
                this.DROPanel.Width += 40;
                //this.DROPanel.Margin = new Thickness(8, 4, 0, 4);
                this.DROPanel.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
                this.DROPanel.VerticalAlignment = System.Windows.VerticalAlignment.Center;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV03a", ex);
            }
        }
        #endregion

        //For the small screen, toolbox button hover functions
        void ToolBxBtn_MouseEnter(object sender, RoutedEventArgs e)
        {
            try
            {
                //if (sender.Equals(ToolBxBtn) )
                //{
                //}
                if (sender.Equals(PPToolBxBtn))
                    PPToolBoxHolder.ShowWindow();
                else
                    ToolBoxHolder.ShowWindow();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV06", ex);
            }
        }

        void ArrangeLowerPanels(double height)
        {
            try
            {
                PartProgramPopUp.Visibility = System.Windows.Visibility.Hidden;

                if (OtherWinToolbarRotated)
                {
                    TheNewRearrangement1();

                    ShapeParameterWin.Width = 804;
                    ShapeParameterWin.Height = 320;
                    ShapeParameterWin.Margin = new Thickness(2, 2, 0, 0);
                    ShapeParameterWin.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
                    ShapeParameterWin.VerticalAlignment = System.Windows.VerticalAlignment.Top;
                    ShapeParameterWin.Visibility = System.Windows.Visibility.Hidden;
                    Video__Grid.Children.Add(ShapeParameterWin);
                    Grid.SetRow(ShapeParameterWin, 1);
                    MeasurementsPopup.Width = 500;
                    MeasurementsPopup.Height = 320;
                    MeasurementsPopup.Margin = new Thickness(2, 0, 0, 2);
                    MeasurementsPopup.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
                    MeasurementsPopup.VerticalAlignment = System.Windows.VerticalAlignment.Bottom;
                    MeasurementsPopup.Visibility = System.Windows.Visibility.Hidden;
                    Video__Grid.Children.Add(MeasurementsPopup);
                    Grid.SetRow(MeasurementsPopup, 1);
                    ActionsPopup.Width = 304;
                    ActionsPopup.Height = 320;
                    ActionsPopup.Margin = new Thickness(0, 0, 2, 2);
                    ActionsPopup.HorizontalAlignment = System.Windows.HorizontalAlignment.Right;
                    ActionsPopup.VerticalAlignment = System.Windows.VerticalAlignment.Bottom;
                    ActionsPopup.Visibility = System.Windows.Visibility.Hidden;
                    Video__Grid.Children.Add(ActionsPopup);
                    Grid.SetRow(ActionsPopup, 1);
                }
                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.VisionHeadOnly)
                {
                    MainLayout.Children.Remove(ToolBarStkPanel);
                    TopLeftGrid.Children.Add(ToolBarStkPanel);
                    //ToolBxBtn.LayoutTransform.SetCurrentValue(RotateTransform.AngleProperty, 0.0);
                    //Visual vv = ToolBxBtn;
                    //Transform tt = VisualTreeHelper.GetTransform(vv);
                    //tt.SetCurrentValue(RotateTransform.AngleProperty, 0.0);
                    //ToolBarStkPanel.VerticalAlignment = VerticalAlignment.Bottom; ToolBarStkPanel.Orientation = Orientation.Horizontal; ToolBarStkPanel.Width = 200;
                    //Grid.SetColumn(ToolBarStkPanel, 1); ToolBarStkPanel.HorizontalAlignment = HorizontalAlignment.Left; ToolBarStkPanel.Margin = new Thickness(180, 0, 0, 0);
                    //TheNewRearrangement1();
                    ToolBoxHolder = new LowerPanelWindow();
                    ToolBoxHolder.IsAllowFading = true;
                    SetOwnerofWindow(ToolBoxHolder);
                    //ToolBoxHolder.MouseLeave += new MouseEventHandler(ToolBoxHolder_MouseLeave);
                    if (GlobalSettings.IsButtonWithNames)
                        ToolBoxHolder.Width = 500; // 340;
                    else
                        ToolBoxHolder.Width = 410; // 250;
                    ToolBoxHolder.Height = 680;
                    ToolBoxHolder.Left = 25;
                    ToolBoxHolder.Top = 65;
                    Tools_Grid.Children.Remove(LeftPanelGrid);
                    ToolBoxHolder.LowerPanelWinContainer.Content = LeftPanelGrid;
                    //if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.VisionHeadOnly)
                    //{
                    //    RotateTransform rt = new RotateTransform(90.0, 0.0, ToolBxBtn.ActualHeight);
                    //    ToolBxBtn.RenderTransform = rt;
                    //}
                    Button btn_ToolBarShow = new Button();
                    TopLeftGrid.Children.Add(btn_ToolBarShow);
                    btn_ToolBarShow.Content = "Tools";
                    Grid.SetColumn(btn_ToolBarShow, 1); btn_ToolBarShow.HorizontalAlignment = HorizontalAlignment.Left; btn_ToolBarShow.VerticalAlignment = VerticalAlignment.Center;
                    btn_ToolBarShow.Margin = new Thickness(200, 0, 0, 0);
                    btn_ToolBarShow.Width = 200; btn_ToolBarShow.Height = 32;
                    btn_ToolBarShow.Style = (Style)FindResource("ScrollToolbar_NormalButton2");
                    btn_ToolBarShow.MouseEnter += new MouseEventHandler(ToolBxBtn_MouseEnter);
                    btn_ToolBarShow.Click += new RoutedEventHandler(ToolBxBtn_MouseEnter);

                    //OtherWinPanel.Children.Remove(RapidCADRBtn);
                    ////OtherWinPanel.Children.Remove(DXFRBtn);
                    //if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HobChecker)
                    //    PPToolBxBtn.Visibility = System.Windows.Visibility.Hidden;

                }

                if (Videoheight > GlobalSettings.VideoHeightThresholdForFrontEndLayout && SystemParameters.WorkArea.Width - 1 == 1680) //GlobalSettings.RapidCameraType != GlobalSettings.CameraType.TwoM
                {
                    MeasureSettingsCB.Visibility = System.Windows.Visibility.Hidden;
                    DrawSettingsCB.Visibility = System.Windows.Visibility.Hidden;
                    DerivedShpSettingsCB.Visibility = System.Windows.Visibility.Hidden;
                    CrosshairSettingsCB.Visibility = System.Windows.Visibility.Hidden;
                    EditSettingsCB.Visibility = System.Windows.Visibility.Hidden;
                    ToolsSettingsCB.Visibility = System.Windows.Visibility.Hidden;
                    UCSSettingsCB.Visibility = System.Windows.Visibility.Hidden;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV08", ex);
            }
        }

        void LowerPanelHolder_MouseLeave(object sender, RoutedEventArgs e)
        {
            try
            {
                if (sender.Equals(LowerPanelWin1))
                {
                    FocusRBtn.IsChecked = false;
                    ThreadRBtn.IsChecked = false;
                    FixedGraphicsRBtn.IsChecked = false;
                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge)
                        CNCRBtn.IsChecked = false;
                    //LowerPanelWin1.HideWindow();
                }
                else if (sender.Equals(LowerPanelWin2))
                {
                    LightingRBtn.IsChecked = false;
                    //LowerPanelWin2.HideWindow();
                }
                else if (sender.Equals(LowerPanelWin3))
                {
                    CNCRBtn.IsChecked = false;
                    //LowerPanelWin3.HideWindow();
                }
                else if (sender.Equals(LowerPanelWin4))
                {
                    SAOIRBtn.IsChecked = false;
                    //LowerPanelWin3.HideWindow();
                }
                else if (sender.Equals(VBlockHolderWin))
                {
                    HolderRBtn.IsChecked = false;
                    //LowerPanelWin3.HideWindow();
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV08.1", ex);
            }
        }

        public void SetOwnerofWindow(Window ClientWindow)
        {
            //VideoFormsWin.SetOwned(ClientWindow);
            ClientWindow.Owner = this;
        }

        #endregion
    }
}

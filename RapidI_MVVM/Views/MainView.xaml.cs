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
        #region constructor & Variables
        //The splash screen instance that double as About window as well
        SplashScreen splash;
        //The Video, Rcad & Dxf windows.
        OpenGlWin RcadWin, DxfWin, ThreadWin, OverlapImgWin;//, VideoWin;//, PCDWin;, ppDrawWin
        //public FormsOpenGlWin VideoFormsWin;
        
        //The toolbar windows
        public ToolbarGrid RcadToolbar, DXFToolbar;

        LightingPanel lightingPopup;
        public CNCPanel CNCPopUp;
        SAOI SaoiPopUp;
        VBlockHolder VBlockPopup;
        public MyPartProgram PartProgramPopUp;
        FdControl FocusPopUp;
        //PPImageViewer PartProgImageVewer;
        FixedGraphicsPanel FixedGraphicsPopup;
        ThreadPanel ThreadPopup;
        PCDPanel PCDPopup;
        MeasurementStatusWin MeasurementsPopup;
        ShapeStatusWin ShapeParameterWin;
        ActionStatusWin ActionsPopup;
        ShapeInfoWin shapeWin;
        NudgeRotateWin DxfNudgeRotateStepWin;
        //HobCheckerWin hobWindow;
        TIS_Control TISControl;
        Rapid.Views.Windows.OnlineSystemController OnlineControlWindow;
        ProfileScanningWin ProfileScanWin;

        //Temporary for Urgent Requirement
        //Button btnA = new Button();

        //This is to allow the user to enter the number of threads during automatic thread module
        ThreadNumWin UserThreadEnteringWin;
        //This window is to get the user choice of surface edge detection entities
        SurfaceFGEntityWin SEDEntitiesWindow;
        //Function to set the line arc parameters
        LineArcPropertiesWin LineArcPropWindow;
        //Window to set the bestfit surface parameters
        BestFitSurfacePropertiesWin BFSurfacePropWin;
        //This window is used to set the transparency values of shapes
        TransparencySettingWin TransparencyWindow;
        //This window is used to take input from user about font size
        FontSizeWin FontWin;
        //Picture viewer window instance.
        PictureViewer PictureViewerWin;
        //The Build window for Part-Program panel.
        PPBuildWin PartProgramBuildWin;
        //Window for user selection of stylus type for T-probe hit
        ProbeStylusTypeSelectionWin PrbStylusSelectionWin;
        //Path interpolation window for height guage
        PathInterpolationWin PathInterpolationWindow;
        //Window to select filter line side
        FilterLineTypeSelectionWin FilterLnTypeWin;
        //This is for the Rotatry axis panel for recording DRO values.
        RAxisRecordPanel RRecordPanel;
        public LoginWin SettingsLoginWindow;
        MessageWin StatusMessageWin;
        //Window shown for doing automatic calibration.
        CalibrationWin CalibWin;
        FixtureCalibWindow FixtureCalibWin;
        StarProbeAutoCalibWin StarProbeCalibWin;

        //Maintaining the ShapeList hash table(as discussed by sathya & suggested as well)
        public System.Collections.Hashtable ShapeListHashTable, DXFListHashTable, UCSHashTable;
        System.Windows.Controls.ContextMenu VideoContextMenu, RcadContextMenu, ChooseAlgoContextMenu, PPContextMenu, RcadPointClickContextMenu; //UCSControlMenu,
        //Maintaing an array of double to keep track of old values of RCAd & DXF windows before maximizing.
        double[] RcadDxfPositionings = new double[4];
        //maintainig an list of windows which were active when MainWindows's state changes(Minimize/Maximize)
        List<Window> ActiveWindows = new List<Window>();

        bool DesignTemplateClicked;

        bool CameraIsOK = true, RWDRO_Instantiated;

        public int MaxCountOfProgram = 0;
        Size lowerPanelOldSize = new Size(454, 254);
        //Refrence to the button styles.They will be initialized(in Load of window) to resources defined in ResourceDictionary.

        Style ChildWinBorderStyle;
        Style ShapeListButtonStyle;
        Style VerticalSep;
        Style ListItemStyle;
        Style UserTabletBtnStyle;
        //For DRO Color 
        byte[] UCSColorList;
        ScrToolbar VisionHeadScrollList;

        public delegate void AsyncSimpleDelegate();


        //Main window constructor
        public MainView()
        {
            //MessageBox.Show("0000");            
            InitializeComponent();
            //MessageBox.Show(Convert.ToString(this.IsInitialized));

            //Just making an array that will contain the colors to be shown for DRO backgrounds/Foregrounds whenever an new UCS is added.
            UCSColorList = new byte[]{0, 0, 0,0, 128, 0,128, 0, 0,128, 255, 255, 255, 170, 255, 
                                      255, 255, 0,255, 169, 83,192, 192, 192,128, 0, 128,169, 196, 
                                      106,88, 207, 141,0, 0, 0};

            
        }
        //Property to State the status of FG points in frontend.
        public string ShowHideFGpointsStatus
        {
            get
            {
                string Status = string.Empty;
                try
                {
                    MenuItem M = (MenuItem)VideoContextMenu.Items.OfType<MenuItem>().Where
                          (T => T.Tag.ToString() == "FG Points Hidden" || T.Tag.ToString() == "FG Points Visible").First();
                    Status = ((string[])M.Tag)[1];
                }
                catch(Exception ex)
                { }
                return Status;
            }
            set
            {
                try
                {
                    MenuItem M = (MenuItem)VideoContextMenu.Items.OfType<MenuItem>().Where
                                          (T => ((string[])T.Tag)[1].ToString() == "FG Points Hidden" || ((string[])T.Tag)[1].ToString() == "FG Points Visible").First();
                    M.Icon = GlobalSettings.SettingsReader.UpdateSingleBtnInfo(value, value, 24, 24);
                    M.Header = GlobalSettings.SettingsReader.ButtonToolTip;
                    M.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                    M.Tag = new string[] { "RightClickOnShape", value };

                    Button btn = (Button)ShapeParameterWin.ButtonsPanel.Children.OfType<Button>().Where
                        (T => ((string[])T.Tag)[2] == "FG Points Hidden" || ((string[])T.Tag)[2] == "FG Points Visible").First();
                    string[] ExistingTag = (string[])btn.Tag;
                    btn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo(value, value, 24, 24);
                    btn.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                    btn.Tag = new string[] { "ShapeParameters", btn.ToolTip.ToString(), value, ExistingTag[3], ExistingTag[4] };
                }
                catch (Exception ex) { }
            }
        }
        //Property to determine whether titles in Part-program has been changed by user or not.
        private bool TitleValuesChanged { get; set; }
        //to determine whether picture viewer should be added to Tools dropdown.
        private bool AddPictureViewerToTools { get; set; }
        //to determine whether Should make lower panel hideable or not.
        private bool MakeLowerPanelHideable { get; set; }
        //Just to determine for first time whether part-program window is initialized from framework.
        private bool PartprogramOpenGlWindowInitialized { get; set; }
        //To determine if thread window is initialised or not
        private bool ThreadOpenGlWindowInitialized { get; set; }
        //In UI to detrmine whether part-program window is in Expanded mode or not.
        private bool IsPartProgramPanelIsInExpandedMode { get; set; }
        //Whether to adjus the width of UCS Dro down in UI.
        private bool AdjustUCSDropDown { get; set; }
        //
        private bool IsLoadErrorGenerated { get; set; }
        //property to determine whether Login window has to be shown or not.
        private bool ShowSettingsWindowAfterLogin { get; set; }
        //if license module says to exit, exit unconditionally
        private bool UnconditionalShutDown;
        //
        public bool AllowAxisLock
        {
            set
            {
                AllowingAxisLock(value);
            }
        }
        //
        public bool AuxillaryLightIsON = false;

        //================================================================================================================================
        //New Variables with the new front end
        //================================================================================================================================

        ComboBox MeasureSettingsCB, DrawSettingsCB, DerivedShpSettingsCB, CrosshairSettingsCB, EditSettingsCB, ToolsSettingsCB, UCSSettingsCB;
        double Videowidth = 0, Videoheight = 0, VideoLeft = 0, VideoTop = 0;
        double Rcadwidth = 0, Rcadheight = 0, RcadLeft = 0, RcadTop = 0;
        double TempHeight, TempWidth, OtherWinPanelWidth, NewVideoWidth, NewVideoHeight, bdrPPRightMargin, FullVideoWidth, FullVideoHeight; //ScrollBarHeight, ScrollBarWidth,
        double DxfWidth, DxfHeight, DxfLeft, DxfTop;
        double PicViewerwidth = 0, PicViewerheight = 0, PicViewerLeft = 0, PicViewerTop = 0;
        public bool OtherWinToolbarRotated = false, LowerPanelsMinimized = false, ShapesNMeasurementsVisible = true;
        Brush SnapOff, SnapOn, VSnapOn, ScanOn, ScanOff;
        Rectangle ModeIndicatorRect, ModeIndicatorRectForScanOnOff;
        LowerPanelWindow LowerPanelWin1, LowerPanelWin2, LowerPanelWin3, LowerPanelWin4, ToolBoxHolder, PPToolBoxHolder, VBlockHolderWin;
        bool IsAdmin = false;
        bool TransformSelectedPtsOnly = false;
        ToggleButton ShpsNMeasurementsTBtn;
        delegate void ButtonHandler_Pointer(object obj, RoutedEventArgs e);

        Button StartStopBtn, CircularInterpolationBtn, LinearInterpolationBtn, PathInterpolationBtn, ReferancePointBtn; //ImportCADBtn, EditCADBtn, 

        private bool ScanStarted = false;

        Thickness RememberPPToolbarGridPosition;

        public bool CHairRBtnChecked = false;
        string CHairRBtnName = "";

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        #endregion

        bool ConnectedToHardware = false;
        bool ReadyToRunNextOnlineComponent = false;
        bool ReadyToMovetoNextOnlineSlot = false;

       

        #region Other Windows And Framework Handling

        #region All Scroll Toolbar click functions
        
        void MeasureScrollBarButtons_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (!InitialCheckAndClear(sender, e)) return;
                ContentControl Btn = (ContentControl)sender;
                string BtnName = ((ButtonProperties)Btn.Tag).ButtonName;

                if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                {
                    if (!(bool)GlobalSettings.FeatureLicenseInfo[((ButtonProperties)Btn.Tag).FeatureID])
                    {
                        MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                        if (Btn.GetType() == typeof(ToggleButton))
                            ((ToggleButton)Btn).IsChecked = false;
                        if (Btn.GetType() == typeof(RadioButton))
                            ((RadioButton)Btn).IsChecked = false;
                        return;
                    }
                }
                if (BtnName != "Parallel Runout" && BtnName != "Intersection Runout")
                    RWrapper.RW_MainInterface.MYINSTANCE().HandleMeasure_Click(BtnName);
                switch (BtnName)
                {
                    case "SplineMeasure":
                        AssignShapePanelToShapeInfoWindow(0, "Enter the number of teeth", "SplineCount", true);
                        break;
                    case "AutoProbePath":
                        {
                            AutoProbePathPropWin PropEntryWin = new AutoProbePathPropWin();
                            List<string> PathTypeList = new List<string>();
                            List<double> PathValList = new List<double>();
                            List<RWrapper.RW_Probe_Entity> PathObjList = new List<RWrapper.RW_Probe_Entity>();

                            PropEntryWin.ShowDialog();
                            if (!PropEntryWin.Result)
                                return;

                            System.Windows.Forms.OpenFileDialog sd = new System.Windows.Forms.OpenFileDialog();
                            sd.Filter = "Old Excel Files(*.xls)|*.xls|New Excel Files(*.xlsx)|*.xlsx";
                            sd.Title = "Open File";
                            sd.DefaultExt = ".xls";
                            if (sd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                            {
                                GlobalSettings.MainExcelInstance.ProbePathFileRead(sd.FileName, ref PathTypeList, ref PathValList);
                                for (int i = 0; i < PathTypeList.Count; i++)
                                {
                                    RWrapper.RW_Probe_Entity TmpPathObj = new RWrapper.RW_Probe_Entity();
                                    TmpPathObj.PtType = PathTypeList[i];
                                    TmpPathObj.Val1 = PathValList[3 * i];
                                    TmpPathObj.Val2 = PathValList[3 * i + 1];
                                    TmpPathObj.Val3 = PathValList[3 * i + 2];
                                    PathObjList.Add(TmpPathObj);
                                }
                                System.Collections.ArrayList PropArrayList = new System.Collections.ArrayList();
                                PropArrayList.Add(PropEntryWin.RadiusVal);
                                PropArrayList.Add(PropEntryWin.ToleranceVal);
                                PropArrayList.Add((bool)PropEntryWin.ShwMeasureWithDeviationChkBx.IsChecked);
                                PropArrayList.Add(PropEntryWin.CompanyNameTxtBx.Text);
                                PropArrayList.Add(PropEntryWin.CompDrawingNumTxtBx.Text);
                                PropArrayList.Add(PropEntryWin.CompSerialNumTxtBx.Text);
                                PropArrayList.Add(PropEntryWin.CompRevisionNumTxtBx.Text);
                                RWrapper.RW_MainInterface.MYINSTANCE().setCoordinate_ProbeMeasurement(PathObjList, PropArrayList);
                            }
                            CylinderScanningWithProbeWin.GetInstance().Topmost = true;
                            CylinderScanningWithProbeWin.GetInstance().Show();
                            CylinderScanningWithProbeWin.GetInstance().Owner = this;
                        }
                        break;
                    case "Auto Profile Scan and Deviation":
                        {
                            AutoProfileScanAndDxfDeviationWin AutoProfileNDeviation = new AutoProfileScanAndDxfDeviationWin();
                            SetOwnerofWindow(AutoProfileNDeviation);
                            AutoProfileNDeviation.Left = VideoLeft + 5;
                            AutoProfileNDeviation.Top = VideoTop + 5;
                            AutoProfileNDeviation.Show();
                        }
                        break;
                    case "Circular Repeat Measurement":
                        {
                            CircularRepeatedMeasurementWin CircularRepeatMeasure = new CircularRepeatedMeasurementWin();
                            SetOwnerofWindow(CircularRepeatMeasure);
                            CircularRepeatMeasure.Left = VideoLeft + 5;
                            CircularRepeatMeasure.Top = VideoTop + 5;
                            CircularRepeatMeasure.Show();
                        }
                        break;
                    case "Profile Scan Width":
                        {
                            ProfileScanWidthWin ProfileScanWidthWindow = new ProfileScanWidthWin();
                            SetOwnerofWindow(ProfileScanWidthWindow);
                            ProfileScanWidthWindow.Left = VideoLeft + 5;
                            ProfileScanWidthWindow.Top = VideoTop + 5;
                            ProfileScanWidthWindow.ShowDialog();
                        }
                        break;
                    case "Parallel Runout":
                        //AssignShapePanelToShapeInfoWindow(0, "Enter the No. of Flutes", "ParallelRunout", true);
                        ParallelRunoutWin.MyInstance().txtValue.Text = "0";
                        ParallelRunoutWin.MyInstance().Left = RcadLeft;
                        ParallelRunoutWin.MyInstance().Top = RcadTop;
                        ParallelRunoutWin.MyInstance().ShowDialog();
                        break;
                    case "Intersection Runout":
                        double val = 1;
                        bool CheckAccepted = false;
                        val = RWrapper.RW_MainInterface.MYINSTANCE().GetRunOutAxialRadius(ref CheckAccepted);
                        if (CheckAccepted)
                        {
                            AxialRunOutWin.MyInstance().Owner = GlobalSettings.MainWin;
                            AxialRunOutWin.MyInstance().Show();
                            AxialRunOutWin.MyInstance().AxialRadius = val;
                        }
                        else
                        {
                            MessageBox.Show("Create tool axis first");
                        }
                        break;
                    case "ShowAllRarelyBtnInMeasure":
                        MeasureScrollList.ShowHideRarelyUsedButtons();
                        break;
                    case "ZeroingAngle":
                        RadioButton Rbtn = (RadioButton)Btn;
                        if ((bool)Rbtn.IsChecked)
                        {
                            ButtonProperties BtnProp = (ButtonProperties)Btn.Tag;
                            double ZeroingAngle = 0.0;
                            Double.TryParse(BtnProp.ToolBarName, out ZeroingAngle);
                            GlobalSettings.ZeroingAngle = ZeroingAngle;
                        }
                        else
                        {
                            GlobalSettings.ZeroingAngle = 0;
                        }
                        break;
                    case "Finalize Zeroing Angle":
                        if (GlobalSettings.DROIsLinearList[3])
                        {
                            double xvalue = Convert.ToDouble(GlobalSettings.MainWin.btnX.GetValue(AttachedDROProperties.DROTextProperty));
                            double yvalue = Convert.ToDouble(GlobalSettings.MainWin.btnY.GetValue(AttachedDROProperties.DROTextProperty));
                            double zvalue = Convert.ToDouble(GlobalSettings.MainWin.btnZ.GetValue(AttachedDROProperties.DROTextProperty));

                            double[] DroValue = { xvalue, yvalue, zvalue, GlobalSettings.ZeroingAngle };
                            RWrapper.RW_DRO.MYINSTANCE().ResetDroValue(DroValue, true);
                        }
                        else
                        {
                            double xvalue = Convert.ToDouble(GlobalSettings.MainWin.btnX.GetValue(AttachedDROProperties.DROTextProperty));
                            double yvalue = Convert.ToDouble(GlobalSettings.MainWin.btnY.GetValue(AttachedDROProperties.DROTextProperty));
                            double zvalue = Convert.ToDouble(GlobalSettings.MainWin.btnZ.GetValue(AttachedDROProperties.DROTextProperty));

                            double[] DroValue = { xvalue, yvalue, zvalue, GlobalSettings.ZeroingAngle * Math.PI / 180 };
                            RWrapper.RW_DRO.MYINSTANCE().ResetDroValue(DroValue, true);
                        }
                        break;
                    case "CamProfileMeasurement":                        
                        SetOwnerofWindow(CamProfileMeasurementWin.MyInstance());
                        CamProfileMeasurementWin.MyInstance().Show();
                        break;
                    case "CamProfileShapeandMeasurement":           //vinod.. for new cam shape and measurement in 1 window...in measure alignment toolbar
                        SetOwnerofWindow(CamProfileShapeandMeasurementWin.MyInstance());
                        CamProfileShapeandMeasurementWin.MyInstance().Show();
                        break;
                    //case "HobChecker":                                  //vinod.. for new Hob OD shape and measurement...in measure alignment toolbar 15/02/2014
                    //    HobCheckerWin.MyInstance().Owner = this;
                    //    //SetOwnerofWindow(HobCheckerWin.MyInstance());
                    //    HobCheckerWin.MyInstance().Show();
                    //    HobCheckerWin.MyInstance().Left = VideoFormsWin.Left;
                    //    HobCheckerWin.MyInstance().Top = VideoFormsWin.Top;
                    //    HobCheckerWin.MyInstance().WindowStyle = System.Windows.WindowStyle.None;
                    //    VideoFormsWin.Visible = false;
                    //    break;
                    case "OpenZelfiZeroingWindowBtn":
                        DelfiRotaryContainerWindow.MyInstance().Show();
                        break;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV09", ex);
            }
        }
        
        void DrawScrollBarButtons_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (!InitialCheckAndClear(sender, e, 1)) return;
                ContentControl Btn = (ContentControl)sender;
                string BtnName = ((ButtonProperties)Btn.Tag).ButtonName;

                if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                {
                    if (!(bool)GlobalSettings.FeatureLicenseInfo[((ButtonProperties)Btn.Tag).FeatureID])
                    {
                        MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                        if (Btn.GetType() == typeof(ToggleButton))
                            ((ToggleButton)Btn).IsChecked = false;
                        if (Btn.GetType() == typeof(RadioButton))
                            ((RadioButton)Btn).IsChecked = false;
                        return;
                    }
                }
                if (BtnName != "Circle Interpolation")
                    RWrapper.RW_MainInterface.MYINSTANCE().HandleDrawToolbar_Click(BtnName);
                switch (BtnName)
                {
                    case "Circle Interpolation":
                        {
                            CircularInterpolationWin CircularMovementWin = new CircularInterpolationWin();
                            SetOwnerofWindow(CircularMovementWin);
                            CircularMovementWin.Show();
                        }
                        break;
                    case "Area":
                        if ((bool)((RadioButton)Btn).IsChecked)
                            AssignShapePanelToShapeInfoWindow(9, "Select type of Area;White Pixels;Black Pixels", "AreaShape", true);
                        break;
                    case "ShowAllRarelyBtnInDraw":
                        DrawScrollList.ShowHideRarelyUsedButtons();
                        break;
                    case "CreateCamProfileShape":
                        SetOwnerofWindow(CamProfileShapeWin.MyInstance());
                        CamProfileShapeWin.MyInstance().Show();
                        break;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV10", ex);
            }
        }
        void DerivedShapeScrollBarButtons_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (!InitialCheckAndClear(sender, e)) return;
                ContentControl Btn = (ContentControl)sender;
                string BtnName = ((ButtonProperties)Btn.Tag).ButtonName;

                if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                {
                    if (!(bool)GlobalSettings.FeatureLicenseInfo[((ButtonProperties)Btn.Tag).FeatureID])
                    {
                        MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                        if (Btn.GetType() == typeof(ToggleButton))
                            ((ToggleButton)Btn).IsChecked = false;
                        if (Btn.GetType() == typeof(RadioButton))
                            ((RadioButton)Btn).IsChecked = false;
                        return;
                    }
                }
                RWrapper.RW_MainInterface.MYINSTANCE().HandleDerivedShapeToolbar_Click(BtnName);
                switch (BtnName)
                {
                    case "Show Cylinder Axis":
                        AssignShapePanelToShapeInfoWindow(0, "Enter the Diameter of the Cylinder", "CylinderDiameter", true);
                        break;
                    case "ShowAllRarelyBtnInDerShps":
                        DerivedShapeScrollList.ShowHideRarelyUsedButtons();
                        break;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV11", ex);
            }
        }
        void CrosshairScrollBarButtons_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (!InitialCheckAndClear(sender, e, 2)) return;
                ContentControl Btn = (ContentControl)sender;
                string BtnName = ((ButtonProperties)Btn.Tag).ButtonName;

                if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                {
                    if (!(bool)GlobalSettings.FeatureLicenseInfo[((ButtonProperties)Btn.Tag).FeatureID])
                    {
                        MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                        if (Btn.GetType() == typeof(ToggleButton))
                            ((ToggleButton)Btn).IsChecked = false;
                        if (Btn.GetType() == typeof(RadioButton))
                            ((RadioButton)Btn).IsChecked = false;
                        return;
                    }
                }
                RWrapper.RW_MainInterface.MYINSTANCE().HandleCrossHairToolbar_Click(BtnName);
                switch (BtnName)
                {
                    case "Cross-Hair Hidden":
                        {
                            GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Cross-Hair Visible", "Cross-Hair Visible", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                            CrosshairScrollList.AssignSingleBtnProp(Btn, "Cross-Hair Visible", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        }
                        break;
                    case "Cross-Hair Visible":
                        {
                            GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Cross-Hair Hidden", "Cross-Hair Hidden", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                            CrosshairScrollList.AssignSingleBtnProp(Btn, "Cross-Hair Hidden", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        }
                        break;
                    case "Focus Scan":
                        {
                            if ((bool)((ToggleButton)Btn).IsChecked)
                                FocusPopUp.DoFocusOn();
                            else
                                FocusPopUp.FocusOff();
                        }
                        break;
                    case "Profile Scan":
                        {
                            if (ProfileScanWin == null)
                                ProfileScanWin = new ProfileScanningWin();
                            ProfileScanWin.Left = VideoLeft + Videowidth - ProfileScanWin.Width - 8;
                            ProfileScanWin.Top = VideoTop - ProfileScanWin.Height;
                            SetOwnerofWindow(ProfileScanWin);
                            ProfileScanWin.Show();
                        }
                        break;
                    case "Scan On-Off":
                        {
                            bool CheckStatus = (bool)((ToggleButton)Btn).IsChecked;
                            if (Btn.GetType() == typeof(ToggleButton))
                            {
                                RWrapper.RW_MainInterface.MYINSTANCE().SetContinuousScanMode(CheckStatus);
                                if (CheckStatus)
                                {
                                    ScanOnOffBtn.Visibility = Visibility.Visible;
                                }
                                else
                                    ScanOnOffBtn.Visibility = Visibility.Hidden;
                            }

                        }
                        break;
                    case "ShowAllRarelyBtnInCrossHair":
                        CrosshairScrollList.ShowHideRarelyUsedButtons();
                        break;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV12", ex);
            }
        }
        void EditScrollBarButtons_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (!InitialCheckAndClear(sender, e)) return;
                ContentControl Btn = (ContentControl)sender;
                string BtnName = ((ButtonProperties)Btn.Tag).ButtonName;

                if (GlobalSettings.SettingsReader == null)
                {
                    //MessageBox.Show("Lost instance. Creating now");
                    GlobalSettings.SettingsReader = new DBReader();
                }

                //MessageBox.Show(BtnName);
                if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                {
                    if (!(bool)GlobalSettings.FeatureLicenseInfo[((ButtonProperties)Btn.Tag).FeatureID])
                    {
                        MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                        if (Btn.GetType() == typeof(ToggleButton))
                            ((ToggleButton)Btn).IsChecked = false;
                        if (Btn.GetType() == typeof(RadioButton))
                            ((RadioButton)Btn).IsChecked = false;
                        return;
                    }
                }
                RWrapper.RW_MainInterface.MYINSTANCE().HandleEditToolbar_Click(BtnName);
                if (BtnName == "Rewrite Data To Excel")
                {
                    if (GlobalSettings.PartProgramLoaded)
                    {
                        GlobalSettings.MainExcelInstance.UpdateLastReport = true;
                        System.Threading.Thread TmpTh = new System.Threading.Thread(GlobalSettings.MainExcelInstance.OpenExcel);
                        TmpTh.Start();
                        GlobalSettings.MainExcelInstance.ToleranceFormat = GlobalSettings.XLDataTransferTolMode;
                        GlobalSettings.MainExcelInstance.MeasurementStringList = RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementListWithNT();
                        GlobalSettings.MainExcelInstance.ProgramName = GlobalSettings.PartProgramPaths[PartProgramPopUp.SelectedProgIndex] + ((System.Data.DataRowView)PartProgramPopUp.dgvPrograms.Items[PartProgramPopUp.SelectedProgIndex]).Row[0].ToString();
                        GlobalSettings.MainExcelInstance.ReportName = PartProgramPopUp.lstReports.SelectedItem.ToString();
                        GlobalSettings.MainExcelInstance.TemplateFolderName = GlobalSettings.TemplatePath;
                        GlobalSettings.MainExcelInstance.Reset_All(true);
                        while (GlobalSettings.MainExcelInstance.IsOpening)
                            continue;
                        GlobalSettings.MainExcelInstance.GenerateExcel(true);
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV13", ex);
            }
        }
        void ToolsScrollBarButtons_Click(object sender, RoutedEventArgs e)
        {
            ToolsScrollList.Get_Button("Login").Visibility = Visibility.Collapsed;
            try
            {
                if (!InitialCheckAndClear(sender, e)) return;
                SaveCheckStatus(sender, "Tools");
                ContentControl Btn = (ContentControl)sender;
                string BtnName = ((ButtonProperties)Btn.Tag).ButtonName;
                if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                {
                    if (!(bool)GlobalSettings.FeatureLicenseInfo[((ButtonProperties)Btn.Tag).FeatureID])
                    {
                        MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                        if (Btn.GetType() == typeof(ToggleButton))
                            ((ToggleButton)Btn).IsChecked = false;
                        if (Btn.GetType() == typeof(RadioButton))
                            ((RadioButton)Btn).IsChecked = false;
                        return;
                    }
                }
                if (BtnName == "Reset Video") // && RWrapper.RW_MainInterface.MYINSTANCE().GetCurrentCameraType() == 3)
                {
                    if (System.Environment.Is64BitOperatingSystem)
                    {
                        if (GlobalSettings.RapidCameraType == GlobalSettings.CameraType.TwoM) GlobalSettings.RapidCameraType = GlobalSettings.CameraType.TwoM_Double;
                        Program_RapidCam3(false);
                        RWrapper.RW_MainInterface.MYINSTANCE().HandleToolsToolbar_Click(BtnName);
                        GlobalSettings.RapidCameraType = GlobalSettings.CameraType.TwoM;
                        return;
                    }
                }
                string[] IndirectButtons_ToHandle = { "Home Position" , "Deg Min Sec" , "Decimal Deg" , "Radian" , "Digital Zoom ON",
                                                        "Digital Zoom ON", "Digital Zoom OFF", "TIS_Control", "Auto Focus", "Flip Horizontal", "Flip Vertical"  };
                //if ((BtnName != "Reset DRO") && BtnName != "Reconnect USB" && BtnName != "Home Position" && BtnName != "Deg Min Sec" &&
                //BtnName != "Decimal Deg" && BtnName != "Radian" && BtnName != "Digital Zoom ON" && !BtnName.Contains("Digital Zoom") &&
                //BtnName != "TIS_Control")
                bool GoDirectto_MainInterface = false;
                for (int j = 0; j < IndirectButtons_ToHandle.Length; j++)
                {
                    if (BtnName == IndirectButtons_ToHandle[j])
                    {
                        GoDirectto_MainInterface = true; break;
                    }
                }
                if (GoDirectto_MainInterface) RWrapper.RW_MainInterface.MYINSTANCE().HandleToolsToolbar_Click(BtnName);

                switch (BtnName)
                {
                    case "Freeze":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Resume", "Resume", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        ToolsScrollList.AssignSingleBtnProp(Btn, "Resume", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        break;
                    case "Resume":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Freeze", "Freeze", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        ToolsScrollList.AssignSingleBtnProp(Btn, "Freeze", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        break;
                    case "Video Source":
                        {
                            if (RWrapper.RW_MainInterface.MYINSTANCE().GetCurrentCameraType() > 0)
                            {
                                if (GlobalSettings.RapidCameraType != GlobalSettings.CameraType.TwoM || GlobalSettings.RapidCameraType != GlobalSettings.CameraType.ThreeM)
                                {
                                    CamPropertiesWindow CamPropWin = new CamPropertiesWindow(IsAdmin);
                                    CamPropWin.Owner = this; // RcadWin;
                                    CamPropWin.Left = RcadLeft - 2;
                                    CamPropWin.Top = RcadTop - 2;
                                    CamPropWin.Show();
                                }
                            }
                        }
                        break;

                    case "Save Frame":
                        {
                            System.Windows.Forms.SaveFileDialog sd = new System.Windows.Forms.SaveFileDialog();
                            //sd.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.Desktop);
                            sd.Filter = "(*.jpg)JPEG Files|*.jpg|(*.bmp)Bitmap Files|*.bmp";
                            sd.Title = "Save";
                            sd.DefaultExt = ".jpg";
                            if (sd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                            {
                                PictureViewerWin.ClearAll();
                                RWrapper.RW_MainInterface.MYINSTANCE().SaveVideoWindowImage(sd.FileName, false, true);
                                GlobalSettings.SettingsReader.GetSoundPlayer("CameraClick").Play();
                            }
                        }
                        break;
                    case "Save Frame With Graphics":
                        {
                            System.Windows.Forms.SaveFileDialog sd = new System.Windows.Forms.SaveFileDialog();
                            //sd.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.Desktop);
                            sd.Filter = "(*.jpg)JPEG Files|*.jpg|(*.bmp)Bitmap Files|*.bmp";
                            sd.Title = "Save";
                            sd.DefaultExt = ".jpg";
                            if (sd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                            {
                                PictureViewerWin.ClearAll();
                                RWrapper.RW_MainInterface.MYINSTANCE().SaveVideoWindowImage(sd.FileName, true, true);
                                GlobalSettings.SettingsReader.GetSoundPlayer("CameraClick").Play();
                            }
                        }
                        break;

                    case "Flip horizontal":
                        
                        break;
                    case "Flip Vertical":
                        
                        break;
                    case "Measurement in Video Hidden":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Measurement in Video Visible", "Measurement in Video Visible", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        ToolsScrollList.AssignSingleBtnProp(Btn, "Measurement in Video Visible", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        break;
                    case "Measurement in Video Visible":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Measurement in Video Hidden", "Measurement in Video Hidden", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        ToolsScrollList.AssignSingleBtnProp(Btn, "Measurement in Video Hidden", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        break;
                    case "Graphics On Video Visible":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Graphics On Video Hidden", "Graphics On Video Hidden", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        ToolsScrollList.AssignSingleBtnProp(Btn, "Graphics On Video Hidden", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        break;
                    case "Graphics On Video Hidden":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Graphics On Video Visible", "Graphics On Video Visible", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        ToolsScrollList.AssignSingleBtnProp(Btn, "Graphics On Video Visible", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        break;
                    case "Digital Zoom 1X":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Digital Zoom 2X", "Digital Zoom", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        ToolsScrollList.AssignSingleBtnProp(Btn, "Digital Zoom 2X", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        RWrapper.RW_MainInterface.MYINSTANCE().HandleToolsToolbar_Click(BtnName);
                        break;
                    case "Digital Zoom 2X":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Digital Zoom 4X", "Digital Zoom", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        ToolsScrollList.AssignSingleBtnProp(Btn, "Digital Zoom 4X", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        RWrapper.RW_MainInterface.MYINSTANCE().HandleToolsToolbar_Click(BtnName);
                        break;

                    case "Digital Zoom 4X":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Digital Zoom 1X", "Digital Zoom", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        ToolsScrollList.AssignSingleBtnProp(Btn, "Digital Zoom 1X", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        RWrapper.RW_MainInterface.MYINSTANCE().HandleToolsToolbar_Click(BtnName);
                        //DigitalZoomOff();
                        //ToolsScrollList.AssignSingleBtnProp(Btn, "Digital Zoom OFF", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        //RWrapper.RW_MainInterface.MYINSTANCE().HandleToolsToolbar_Click("Digital Zoom OFF");
                        //RWrapper.RW_DBSettings.MYINSTANCE().UpdateVideoPixelWidthForDigitalZoom(MagnificationList.lstDropDownItems.SelectedItem.ToString(), false);
                        break;
                    case "Digital Zoom OFF":
                        //DigitalZoomOn();
                        //GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Digital Zoom ON", "Digital Zoom ON", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        //ToolsScrollList.AssignSingleBtnProp(Btn, "Digital Zoom ON", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        //RWrapper.RW_DBSettings.MYINSTANCE().UpdateVideoPixelWidthForDigitalZoom(MagnificationList.lstDropDownItems.SelectedItem.ToString(), true);
                        //RWrapper.RW_MainInterface.MYINSTANCE().HandleToolsToolbar_Click("Digital Zoom ON");
                        break;
                    case "DRO Visible":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("DRO Hidden", "DRO Hidden", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        ToolsScrollList.AssignSingleBtnProp(Btn, "DRO Hidden", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        DROPanel.Visibility = System.Windows.Visibility.Hidden;
                        break;
                    case "DRO Hidden":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("DRO Visible", "DRO Visible", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        ToolsScrollList.AssignSingleBtnProp(Btn, "DRO Visible", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        DROPanel.Visibility = System.Windows.Visibility.Visible;
                        break;
                    case "Deg Min Sec":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Decimal Deg", "Decimal Deg", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        ToolsScrollList.AssignSingleBtnProp(Btn, "Decimal Deg", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        GlobalSettings.CurrentAngleMode = GlobalSettings.AngleMode.Decimal_Degree;
                        RWrapper.RW_MainInterface.MYINSTANCE().HandleToolsToolbar_Click("Decimal Deg");
                        break;
                    case "Decimal Deg":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Radian", "Radian", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        ToolsScrollList.AssignSingleBtnProp(Btn, "Radian", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        GlobalSettings.CurrentAngleMode = GlobalSettings.AngleMode.Radians;
                        RWrapper.RW_MainInterface.MYINSTANCE().HandleToolsToolbar_Click("Radian");
                        break;
                    case "Radian":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Deg Min Sec", "Deg Min Sec", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        ToolsScrollList.AssignSingleBtnProp(Btn, "Deg Min Sec", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        GlobalSettings.CurrentAngleMode = GlobalSettings.AngleMode.Degree_Minute_Second;
                        RWrapper.RW_MainInterface.MYINSTANCE().HandleToolsToolbar_Click("Deg Min Sec");
                        break;
                    case "Reconnect USB":
                        CheckPermission(BtnName);
                        RWrapper.RW_MainInterface.MYINSTANCE().HandleToolsToolbar_Click(BtnName);
                        System.Threading.Thread.Sleep(100);
                        //for (int i = 0; i < 4; i++)
                        RWrapper.RW_DRO.MYINSTANCE().SetProbeRevertDistSpeed(0, GlobalSettings.ProbebackDist, GlobalSettings.ProbeBackSpeed, GlobalSettings.PApproachDistance, GlobalSettings.PApproachSpeed, GlobalSettings.PSensitivityValue, GlobalSettings.RetractProbePt_Enabled);
                        break;
                    case "Reset DRO":
                        CheckPermission(BtnName);
                        RWrapper.RW_MainInterface.MYINSTANCE().HandleToolsToolbar_Click(BtnName);
                        break;
                    case "Home Position":
                        if (GlobalSettings.LocalisedCorrectionOn)
                            RWrapper.RW_MainInterface.MYINSTANCE().HandleToolsToolbar_Click(BtnName);
                        else
                        {
                            CheckPermission(BtnName);
                            RWrapper.RW_MainInterface.MYINSTANCE().HandleToolsToolbar_Click(BtnName);
                        }
                            break;
                    case "Generate C Axis":
                        RWrapper.RW_MainInterface.MYINSTANCE().CaptureCAxis();
                        break;
                    case "V Block Calibration":
                        {
                            VBlockCalibrationWin VBlkCalibWin = new VBlockCalibrationWin();
                            //VBlkCalibWin.Topmost = true;
                            VBlkCalibWin.Owner = RcadWin;
                            VBlkCalibWin.Show();
                        }
                        break;
                    //case "Login":
                    //    SettingsLoginWindow.txtInfo.Text = "";
                    //ShowLoginWindow:
                    //    SettingsLoginWindow.txtPassword.Password = "";
                    //    string EnteredUserName = SettingsLoginWindow.LoggedInUser;
                    //    SettingsLoginWindow.UserTabletList.Children.Clear();

                    //    foreach (string sr in SettingsLoginWindow.LoginDetailsNameColl)
                    //    {
                    //        RadioButton Rb = new RadioButton();
                    //        Rb.Style = UserTabletBtnStyle;
                    //        Rb.Height = Rb.Width = 120;
                    //        Rb.Content = sr;
                    //        Rb.Tag = sr;
                    //        Rb.Click += new RoutedEventHandler(SettingsLoginWindow.HandleUserTabletClick);
                    //        SettingsLoginWindow.UserTabletList.Children.Add(Rb);
                    //    }
                    //    SettingsLoginWindow.LoggedInUser = EnteredUserName;
                    //    SettingsLoginWindow.ShowDialog();
                    //    if (SettingsLoginWindow.InfoStatus)
                    //    {
                    //        string username = SettingsLoginWindow.LoggedInUser;
                    //        string password = SettingsLoginWindow.txtPassword.Password;
                    //        //System.Collections.Hashtable loginDetails = RWrapper.RW_DBSettings.MYINSTANCE().LoginDetailsCollection;
                    //        if (SettingsLoginWindow.LoginDetailsNameColl.Contains(username))
                    //        {
                    //            if (password == SettingsLoginWindow.getPassword(username))
                    //            {
                    //                //RWrapper.RW_MainInterface.MYINSTANCE().LoginSuccessFull(username);
                    //                // b.Content = "Logout";
                    //                UserNameTxt.Text = username;
                    //                UserLevelTxt.Text = SettingsLoginWindow.getPrevilegeLevel(username);
                    //                switch (SettingsLoginWindow.getPrevilegeLevel(username))
                    //                {
                    //                    case "System":
                    //                        UserNameTxt.Foreground = VSnapOn;
                    //                        GlobalSettings.CurrentUserLevel = GlobalSettings.UserPrevilageLevel.System;
                    //                        //HelpWindow.GetObj().IsAdmminMode = true;
                    //                        DelfiRotaryContainerWindow.MyInstance().SaveOffset.Visibility = System.Windows.Visibility.Visible;
                    //                        break;
                    //                    case "Admin":
                    //                        UserNameTxt.Foreground = VSnapOn;
                    //                        if (username == "admin")
                    //                        {
                    //                            GlobalSettings.CurrentUserLevel = GlobalSettings.UserPrevilageLevel.Admin;
                    //                            if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
                    //                                DelfiRotaryContainerWindow.MyInstance().SaveOffset.Visibility = System.Windows.Visibility.Visible;
                    //                        }
                    //                        else
                    //                            GlobalSettings.CurrentUserLevel = GlobalSettings.UserPrevilageLevel.AdminUser;
                    //                        break;
                    //                    case "User":
                    //                        UserNameTxt.Foreground = SnapOn;
                    //                        GlobalSettings.CurrentUserLevel = GlobalSettings.UserPrevilageLevel.User;
                    //                        break;
                    //                }

                    //                if (SettingsLoginWindow.getPrevilegeLevel(username) == "Admin" || SettingsLoginWindow.getPrevilegeLevel(username) == "System")
                    //                {
                    //                    ToolsScrollList.AddNewButton("Fixture Calibration", "Fixture Calibration", GlobalSettings.ToolbarBtnStdWidth, GlobalSettings.ToolbarBtnStdHeight,
                    //                            GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight, new Thickness(2, 0, 2, 0));
                    //                    //ToolsScrollList.AddNewButton("StarProbeCalibration", "StarProbeCalibration", GlobalSettings.ToolbarBtnStdWidth, GlobalSettings.ToolbarBtnStdHeight,
                    //                    //       GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight, new Thickness(2, 0, 2, 0));
                    //                    //ToolsScrollList.AddNewButton("ProbeInspection", "ProbeInspection", GlobalSettings.ToolbarBtnStdWidth, GlobalSettings.ToolbarBtnStdHeight,
                    //                    //    GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight, new Thickness(2, 0, 2, 0));
                    //                }
                    //                //Check if system has logged in then show another button into the tools dropdown for the calibation window.
                    //                if (SettingsLoginWindow.getPrevilegeLevel(username) == "System")
                    //                {
                    //                    ToolsScrollList.AddNewButton("Calibration", "Calibration", GlobalSettings.ToolbarBtnStdWidth, GlobalSettings.ToolbarBtnStdHeight,
                    //                        GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight, new Thickness(2, 0, 2, 0));
                    //                    ToolsScrollList.AddNewButton("FocusScan Calibration", "FocusScan Calibration", GlobalSettings.ToolbarBtnStdWidth, GlobalSettings.ToolbarBtnStdHeight,
                    //                        GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight, new Thickness(2, 0, 2, 0));
                    //                    ToolsScrollList.Get_Button("Flip Vertical").Visibility = Visibility.Visible;
                    //                    ToolsScrollList.Get_Button("Flip Horizontal").Visibility = Visibility.Visible;
                    //                }
                    //                if (SettingsLoginWindow.getPrevilegeLevel(username) == "Admin" || SettingsLoginWindow.getPrevilegeLevel(username) == "System")
                    //                {
                    //                    IsAdmin = true;
                    //                }
                    //                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Logout", "Logout", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                    //                ToolsScrollList.AssignSingleBtnProp(Btn, "Logout", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);

                    //                ContentControl SetBtn = ToolsScrollList.AddNewButton("Settings", "Settings", GlobalSettings.ToolbarBtnStdWidth, GlobalSettings.ToolbarBtnStdHeight,
                    //                        GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight, new Thickness(2, 0, 2, 0));
                    //                ToolsScrollBarButtons_Click(SetBtn, null);
                    //            }
                    //            else
                    //            {
                    //                SettingsLoginWindow.txtInfo.Text = "Entered 'Password' is wrong.Please try again.";
                    //                goto ShowLoginWindow;
                    //            }
                    //        }
                    //        else
                    //        {
                    //            SettingsLoginWindow.txtInfo.Text = "Entered username does'nt exist.Please try again.";
                    //            goto ShowLoginWindow;
                    //        }
                    //    }
                    //    break;
                    //case "Logout":
                    //    {
                    //        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Login", "Login", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                    //        ToolsScrollList.AssignSingleBtnProp(Btn, "Login", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);

                    //        GlobalSettings.CurrentUserLevel = GlobalSettings.UserPrevilageLevel.None;

                    //        ToolsScrollList.Remove_Button_from_End();
                    //        if (UserLevelTxt.Text == "System")
                    //        {
                    //            ToolsScrollList.Remove_Button_from_End();
                    //            ToolsScrollList.Remove_Button_from_End();
                    //            HelpWindow.GetObj().IsAdmminMode = false;
                    //            DelfiRotaryContainerWindow.MyInstance().SaveOffset.Visibility = System.Windows.Visibility.Collapsed;
                    //        }
                    //        if (UserLevelTxt.Text == "Admin" || UserLevelTxt.Text == "System")
                    //        {
                    //            //ToolsScrollList.Remove_Button_from_End();
                    //            //ToolsScrollList.Remove_Button_from_End();
                    //            ToolsScrollList.Remove_Button_from_End();
                    //            DelfiRotaryContainerWindow.MyInstance().SaveOffset.Visibility = System.Windows.Visibility.Collapsed;
                    //            ToolsScrollList.Get_Button("Flip Vertical").Visibility = Visibility.Collapsed;
                    //            ToolsScrollList.Get_Button("Flip Horizontal").Visibility = Visibility.Collapsed;
                    //        }
                    //        if (UserLevelTxt.Text == "Admin" || UserLevelTxt.Text == "System")
                    //        {
                    //            IsAdmin = false;
                    //        }
                    //        UserNameTxt.Text = "Not Logged In";
                    //        UserLevelTxt.Text = "";
                    //        UserNameTxt.Foreground = Brushes.White;
                    //    }
                    //    break;
                    case "Settings":
                        if (ShowSettingsWindowAfterLogin)
                        {
                            //RWrapper.RW_MainInterface.MYINSTANCE().SettingsClicked();
                            SettingsWin ThisSettings = new SettingsWin();
                            ThisSettings.FeedRateChangedEvent += new AsyncSimpleDelegate(this.HandleCNCFeedRateSettingsChanged);
                            SetOwnerofWindow(ThisSettings);
                            ThisSettings.ShowDialog();
                        }
                        break;
                    case "Calibration":
                        CalibWin = new CalibrationWin();
                        //handle the Calibration related events
                        RWrapper.RW_AutoCalibration.MYINSTANCE().DisableButtonState += new RWrapper.RW_AutoCalibration.CalibrationEventHandler(MainView_DisableButtonState);
                        RWrapper.RW_AutoCalibration.MYINSTANCE().ResetButtonState += new RWrapper.RW_AutoCalibration.CalibrationEventHandler(MainView_ResetButtonState);

                        CalibWin.Owner = this;
                        CalibWin.Topmost = true;
                        CalibWin.Show();
                        break;
                    case "FocusScan Calibration":
                        {
                            FocusCalibrationWin FocusCalibWin = new FocusCalibrationWin();
                            FocusCalibWin.ShowDialog();
                        }
                        break;
                    case "Fixture Calibration":
                        FixtureCalibWin = new FixtureCalibWindow();
                        FixtureCalibWin.Owner = this;
                        FixtureCalibWin.Topmost = true;
                        FixtureCalibWin.Show();
                        break;
                    case "ContourTracerCalibration":
                        {
                            ContourTracerCalibWin ContourTracerCalibration = new ContourTracerCalibWin();
                            SetOwnerofWindow(ContourTracerCalibration);
                            ContourTracerCalibration.Show();
                        }
                        break;
                    case "StarProbeCalibration":
                    case "ProbeInspection":
                    case "Auto Sphere":
                    case "Touch-Vision Caliberation":
                    case "ProbeOffSetInspection":
                        {
                            StarProbeCalibWin = new StarProbeAutoCalibWin();
                            RWrapper.RW_SphereCalibration.MYINSTANCE().PointsCountEvent += new RWrapper.RW_SphereCalibration.PointsCountEventHandler(MainView_PointsCountEvent);
                            RWrapper.RW_SphereCalibration.MYINSTANCE().ProbeOffsetEvent += new RWrapper.RW_SphereCalibration.ProbeOffsetEventHandler(MainView_ProbeOffsetEvent);
                            RWrapper.RW_SphereCalibration.MYINSTANCE().FinishedSphereEvent += new RWrapper.RW_SphereCalibration.FinishedSphereCycleHandler(MainView_FinishedSphereUpdate);                
                            StarProbeCalibWin.Title = "Calibrating with Master Sphere";
                            SetOwnerofWindow(StarProbeCalibWin);
                            StarProbeCalibWin.Show();
                        }
                        break;
                    case "PPImageViewer":
                        {
                            PPImageViewer PpImageviewerWindow = new PPImageViewer();
                            SetOwnerofWindow(PpImageviewerWindow);
                            PpImageviewerWindow.Show();
                        }
                        break;
                    case "ImageCaptureGridProgram":
                        {
                            ImageCaptureGridWin ImgCaptureWin = new ImageCaptureGridWin();
                            ImgCaptureWin.Left = VideoLeft + Videowidth + 8;
                            ImgCaptureWin.Top = VideoTop;

                            SetOwnerofWindow(ImgCaptureWin);
                            ImgCaptureWin.Show();
                        }
                        break;
                    case "ImageSuperImpose":
                        {
                            // ImageSuperImpose.MyInstance();
                            SetOwnerofWindow(ImageSuperImpose.MyInstance());
                            ImageSuperImpose.MyInstance().Show();
                            //ImageSuperImpose ImgSuperImpWin = new ImageSuperImpose();                            
                            //ImgSuperImpWin.Show();
                        }
                        break;
                    case "Generic Prob":
                        {
                            SetOwnerofWindow(GenericProb.Myinstance());
                            GenericProb.Myinstance().Show();
                            //SurfaceFrameGrabWin.MyInstance().Show();
                        }
                        break;
                    case "Filter":
                        {
                            if (Btn.GetType() == typeof(ToggleButton))
                            {
                                ToggleButton TBtn = (ToggleButton)Btn;
                                bool CheckStatus = (bool)TBtn.IsChecked;
                                RWrapper.RW_MainInterface.MYINSTANCE().SetFilterStatus(CheckStatus);
                            }
                        }
                        break;
                    case "ShowAllRarelyBtnInTools":
                        ToolsScrollList.ShowHideRarelyUsedButtons();
                        break;
                    case "CamComponent":
                        //SetOwnerofWindow(CamComponentWin.MyInstance());
                        //CamComponentWin.MyInstance().Show();
                        break;
                    case "ImageDelayCaliberation":
                        SetOwnerofWindow(ImageDelayCaliberation.MyInstance());
                        ImageDelayCaliberation.MyInstance().Show();
                        break;
                    case "TIS_Control":
                        TISControl.Show();
                        break;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV14", ex);
            }
        }

        void UCSScrollBarButtons_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (!InitialCheckAndClear(sender, e)) return;
                ContentControl Btn = (ContentControl)sender;
                string BtnName = ((ButtonProperties)Btn.Tag).ButtonName;
                if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                {
                    if (!(bool)GlobalSettings.FeatureLicenseInfo[((ButtonProperties)Btn.Tag).FeatureID])
                    {
                        MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                        if (Btn.GetType() == typeof(ToggleButton))
                            ((ToggleButton)Btn).IsChecked = false;
                        if (Btn.GetType() == typeof(RadioButton))
                            ((RadioButton)Btn).IsChecked = false;
                        return;
                    }
                }
                if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.VisionHeadOnly)
                {
                    
                    RWrapper.RW_MainInterface.MYINSTANCE().HandleUCSProperties_Click(BtnName);
                    
                }
                    
                else
                {
                    if (BtnName.Contains("Nudge") || BtnName.Contains("Rotate"))
                    {
                        InitialCheckAndClear(sender, e);
                        DxfScrollBarButtons_Click(Btn, e);
                    }
                    else
                        RWrapper.RW_MainInterface.MYINSTANCE().HandleVisionHeadToolBar_Click(BtnName);
                }

            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV15", ex);
            }
        }
        void RcadScrollBarButtons_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (!InitialCheckAndClear(sender, e)) return;
                SaveCheckStatus(sender, "RCAD Toolbar");
                ContentControl Btn = (ContentControl)sender;
                ScrToolbar RcadScrollbar = RcadToolbar.GetContent();
                string BtnName = ((ButtonProperties)Btn.Tag).ButtonName;

                if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                {
                    if (!(bool)GlobalSettings.FeatureLicenseInfo[((ButtonProperties)Btn.Tag).FeatureID])
                    {
                        MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                        if (Btn.GetType() == typeof(ToggleButton))
                            ((ToggleButton)Btn).IsChecked = false;
                        if (Btn.GetType() == typeof(RadioButton))
                            ((RadioButton)Btn).IsChecked = false;
                        return;
                    }
                }
                if (BtnName != "Bounded Partial Shapes" && BtnName != "Localised Partial Shapes" && BtnName != "Whole Shape" && BtnName != "2D Mode" && BtnName != "3D Mode")
                    RWrapper.RW_MainInterface.MYINSTANCE().HandleRcadToolbar_Click(BtnName);
                switch (BtnName)
                {
                    case "Maximize":
                        {
                            double[] TmpPosInfo = new double[4];
                            if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HobChecker) return;
                            PrepareForRcadMaximise(ref TmpPosInfo);
                            if (GlobalSettings.IsButtonWithNames)
                            {
                                TmpPosInfo[1] += 25;
                                TmpPosInfo[3] -= 25;
                            }
                            ChangeRcadDxfWidthHeight_Positioning(ref RcadWin, ref RcadToolbar, TmpPosInfo[0],
                                TmpPosInfo[1], TmpPosInfo[2], TmpPosInfo[3], 1, true);
                            //RcadWin.Owner = VideoWin;
                            GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Minimize", "Minimize", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                            RcadScrollbar.AssignSingleBtnProp(Btn, "Minimize", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                            RWrapper.RW_MainInterface.MYINSTANCE().UpdateRCadGraphics();
                          
                        }
                        break;
                    case "Minimize":
                        {
                            if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HobChecker) return;
                            PrepareForRcadMinimise();
                            ChangeRcadDxfWidthHeight_Positioning(ref RcadWin, ref RcadToolbar, RcadDxfPositionings[0],
                                    RcadDxfPositionings[1], RcadDxfPositionings[2], RcadDxfPositionings[3], 1, false);

                            GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Maximize", "Maximize", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                            RcadScrollbar.AssignSingleBtnProp(Btn, "Maximize", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                            RWrapper.RW_MainInterface.MYINSTANCE().UpdateVideoGraphics();
                            RWrapper.RW_MainInterface.MYINSTANCE().UpdateRCadGraphics();
                        }
                        break;

                    case "Data Transfer to Excel":
                        {
                            System.Windows.Forms.SaveFileDialog sd = new System.Windows.Forms.SaveFileDialog();
                            //sd.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.Desktop);
                            sd.Filter = "(*.xls)Excel Files|*.xls";
                            sd.Title = "Save as";
                            sd.DefaultExt = ".xls";
                            if (sd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                            {
                                GlobalSettings.MainExcelInstance.UseExistingReport = false;
                                GlobalSettings.MainExcelInstance.ToleranceFormat = GlobalSettings.XLDataTransferTolMode;
                                //GlobalSettings.MainExcelInstance.MeasurementStringList = RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementListWithNT();
                                GlobalSettings.MainExcelInstance.DefaultReportSelection(!GlobalSettings.XLExportRowWise, !GlobalSettings.XLExportPortraitLayout);
                                GlobalSettings.MainExcelInstance.InsertImageForDataTransfer = GlobalSettings.InsertImageForDataTransfer;
                                GlobalSettings.MainExcelInstance.ReportName = sd.FileName;
                                string PicFile = "", Pic2File = "";
                                PicFile = System.IO.Path.GetTempPath() + "RRMBuildPic.jpg";
                                if (GlobalSettings.XLExportRCadImgWithData)
                                {
                                    Pic2File = System.IO.Path.GetTempPath() + "RRMBuildPic1.jpg";
                                    RWrapper.RW_MainInterface.MYINSTANCE().ClearShapeAndMeasure_Selections();
                                    System.Threading.Thread.Sleep(50);
                                    RWrapper.RW_MainInterface.MYINSTANCE().ZoomToExtentsRcadorDxf(1);
                                    System.Threading.Thread.Sleep(50);
                                    RWrapper.RW_MainInterface.MYINSTANCE().SaveRcadWindowImage(PicFile, false);
                                }
                                GlobalSettings.MainExcelInstance.PicturePath = PicFile;
                                GlobalSettings.MainExcelInstance.Picture2Path = Pic2File;

                                GlobalSettings.MainExcelInstance.Reset_All();
                                GlobalSettings.MainExcelInstance.ShowAvging = false; // GlobalSettings.ShowAveragingInExcel;
                                GlobalSettings.MainExcelInstance.InsertImageForDataTransfer = GlobalSettings.InsertImageForDataTransfer;
                                //GlobalSettings.MainExcelInstance.MakePDFReport = GlobalSettings.MakePDFReport;
                                GlobalSettings.MainExcelInstance.GenerateExcel(false, true);
                                if (PicFile != "")
                                    System.IO.File.Delete(PicFile);
                                if (!GlobalSettings.MakePDFReport)
                                {
                                    if (File.Exists(sd.FileName))
                                        System.Diagnostics.Process.Start(sd.FileName);
                                    else
                                        MessageBox.Show("The report file does not exist. It may not have been created properly.", "Rapid-I 5.0");
                                }
                            }
                        }
                        break;

                    case "Print Report":
                        {
                            GlobalSettings.MainExcelInstance.UseExistingReport = false;
                            //GlobalSettings.MainExcelInstance.ToleranceFormat = GlobalSettings.XLDataTransferTolMode;
                            GlobalSettings.MainExcelInstance.MeasurementStringList = RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementListWithNT();
                            //GlobalSettings.MainExcelInstance.Reset_All();
                            //GlobalSettings.MainExcelInstance.DefaultReportSelection(!GlobalSettings.XLExportRowWise, !GlobalSettings.XLExportPortraitLayout);
                            //GlobalSettings.MainExcelInstance.InsertImageForDataTransfer = GlobalSettings.InsertImageForDataTransfer;
                            string reportFileName = GlobalSettings.MainExcelInstance.GeneratePrintReport(GlobalSettings.ReportPath);
                            System.Diagnostics.Process process = new System.Diagnostics.Process();
                            process.StartInfo.FileName = "excel.exe";
                            process.StartInfo.Arguments = "\"" + reportFileName + "\"";
                            process.Start();
                            //System.Printing.LocalPrintServer lps = new System.Printing.LocalPrintServer();
                            //lps.
                        }
                        break;
                    case "Home":
                        {
                            ((ToggleButton)RcadToolbar.GetContent().Get_Button("3D Rotate")).IsChecked = false;
                            if (RcadToolbar.GetContent().CheckIfButtonExists("3D Mode"))
                            {
                                Button DBtn = ((Button)RcadToolbar.GetContent().Get_Button("3D Mode"));
                                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("2D Mode", "RCAD Toolbar", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                                RcadScrollbar.AssignSingleBtnProp(DBtn, "2D Mode", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                            }
                        }
                        break;
                    case "Measurement":
                        if (!(bool)((RadioButton)sender).IsChecked)
                        {
                            ToggleButton MRBtn = (ToggleButton)RcadToolbar.GetContent().Get_Button("Linear Mode");
                            if ((bool)MRBtn.IsChecked)
                            {
                                MRBtn.IsChecked = false;
                                RWrapper.RW_MainInterface.MYINSTANCE().HandleRcadToolbar_Click("Linear Mode");
                            }
                            MRBtn = (ToggleButton)RcadToolbar.GetContent().Get_Button("PCD Mode");
                            if ((bool)MRBtn.IsChecked)
                            {
                                MRBtn.IsChecked = false;
                                RWrapper.RW_MainInterface.MYINSTANCE().HandleRcadToolbar_Click("PCD Mode");
                            }
                        }
                        break;
                    case "Zoom-In":
                        if (!(bool)((RadioButton)sender).IsChecked)
                        {
                            //RWrapper.RW_MainInterface.MYINSTANCE().OnRCadMouseWheel(240);
                        }
                        break;
                    case "Zoom-Out":
                        if (!(bool)((RadioButton)sender).IsChecked)
                        {
                            //RWrapper.RW_MainInterface.MYINSTANCE().OnRCadMouseWheel(-240);
                        }
                        break;
                    case "Linear Mode":
                        {
                            if (((ToggleButton)sender).IsChecked == true)
                            {
                                RadioButton MRBtn = (RadioButton)RcadToolbar.GetContent().Get_Button("Measurement");
                                if (!(bool)MRBtn.IsChecked)
                                {
                                    MRBtn.IsChecked = true;
                                    RWrapper.RW_MainInterface.MYINSTANCE().HandleRcadToolbar_Click("Measurement");
                                }
                                ToggleButton MTBtn = (ToggleButton)RcadToolbar.GetContent().Get_Button("PCD Mode");
                                if ((bool)MTBtn.IsChecked)
                                {
                                    MTBtn.IsChecked = false;
                                    RWrapper.RW_MainInterface.MYINSTANCE().HandleRcadToolbar_Click("PCD Mode");
                                }
                            }
                        }
                        break;
                    case "PCD Mode":
                        {
                            if (((ToggleButton)sender).IsChecked == true)
                            {
                                RadioButton MRBtn = (RadioButton)RcadToolbar.GetContent().Get_Button("Measurement");
                                if (!(bool)MRBtn.IsChecked)
                                {
                                    MRBtn.IsChecked = true;
                                    RWrapper.RW_MainInterface.MYINSTANCE().HandleRcadToolbar_Click("Measurement");
                                }
                                ToggleButton MTBtn = (ToggleButton)RcadToolbar.GetContent().Get_Button("Linear Mode");
                                if ((bool)MTBtn.IsChecked)
                                {
                                    MTBtn.IsChecked = false;
                                    RWrapper.RW_MainInterface.MYINSTANCE().HandleRcadToolbar_Click("Linear Mode");
                                }
                            }
                        }
                        break;
                    case "CCPM":
                        {
                            if (((ToggleButton)sender).IsChecked == true)
                            {
                                RadioButton MRBtn = (RadioButton)RcadToolbar.GetContent().Get_Button("Measurement");
                                if (!(bool)MRBtn.IsChecked)
                                {
                                    MRBtn.IsChecked = true;
                                    RWrapper.RW_MainInterface.MYINSTANCE().HandleRcadToolbar_Click("Measurement");
                                }
                                ToggleButton MTBtn = (ToggleButton)RcadToolbar.GetContent().Get_Button("Linear Mode");
                                if ((bool)MTBtn.IsChecked)
                                {
                                    MTBtn.IsChecked = false;
                                    RWrapper.RW_MainInterface.MYINSTANCE().HandleRcadToolbar_Click("Linear Mode");
                                }
                                MTBtn = (ToggleButton)RcadToolbar.GetContent().Get_Button("PCD Mode");
                                if ((bool)MTBtn.IsChecked)
                                {
                                    MTBtn.IsChecked = false;
                                    RWrapper.RW_MainInterface.MYINSTANCE().HandleRcadToolbar_Click("PCD Mode");
                                }
                            }
                        }
                        break;
                    case "2D Mode":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("3D Mode", "3D Mode", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        RcadScrollbar.AssignSingleBtnProp(Btn, "3D Mode", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        RWrapper.RW_MainInterface.MYINSTANCE().HandleRcadToolbar_Click("3D Mode");
                        break;
                    case "3D Mode":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("2D Mode", "RCAD Toolbar", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        RcadScrollbar.AssignSingleBtnProp(Btn, "2D Mode", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        RWrapper.RW_MainInterface.MYINSTANCE().HandleRcadToolbar_Click("3D Mode");
                        break;
                    case "Set Transparency Level":
                        TransparencyWindow.ShowDialog();
                        break;
                    case "Graphics Rotated Along UCS":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Graphics Not Rotated Along UCS", "Graphics Not Rotated Along UCS", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        RcadScrollbar.AssignSingleBtnProp(Btn, "Graphics Not Rotated Along UCS", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        break;
                    case "Graphics Not Rotated Along UCS":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Graphics Rotated Along UCS", "Graphics Rotated Along UCS", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        RcadScrollbar.AssignSingleBtnProp(Btn, "Graphics Rotated Along UCS", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        break;
                    case "Move With DRO":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Dont Move With DRO", "Dont Move With DRO", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        RcadScrollbar.AssignSingleBtnProp(Btn, "Dont Move With DRO", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        break;
                    case "Dont Move With DRO":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Move With DRO", "Move With DRO", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        RcadScrollbar.AssignSingleBtnProp(Btn, "Move With DRO", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        break;
                    case "Filled Surface":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Wired Surface", "Wired Surface", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        RcadScrollbar.AssignSingleBtnProp(Btn, "Wired Surface", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        break;
                    case "Wired Surface":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Transparent", "Transparent", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        RcadScrollbar.AssignSingleBtnProp(Btn, "Transparent", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        break;
                    case "Transparent":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Filled Surface", "Filled Surface", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        RcadScrollbar.AssignSingleBtnProp(Btn, "Filled Surface", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        break;
                    case "Auto Measurement":
                        {
                            List<double> PositionList = new List<double>();
                            System.Windows.Forms.OpenFileDialog sd = new System.Windows.Forms.OpenFileDialog();
                            sd.Filter = "Old Excel Files(*.xls)|*.xls|New Excel Files(*.xlsx)|*.xlsx";
                            sd.Title = "Open File";
                            sd.DefaultExt = ".xls";
                            if (sd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                            {
                                GlobalSettings.MainExcelInstance.AutoMeasurmentFileRead(sd.FileName, ref PositionList);
                                RWrapper.RW_MainInterface.MYINSTANCE().setCoordinateListForAutomeasurement(PositionList);
                            }
                        }
                        break;
                    case "Bounded Partial Shapes":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Localised Partial Shapes", "Localised Partial Shapes", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        RcadScrollbar.AssignSingleBtnProp(Btn, "Localised Partial Shapes", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        RWrapper.RW_MainInterface.MYINSTANCE().HandleRcadToolbar_Click("Localised Partial Shapes");
                        break;
                    case "Localised Partial Shapes":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Whole Shape", "Whole Shape", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        RcadScrollbar.AssignSingleBtnProp(Btn, "Whole Shape", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        RWrapper.RW_MainInterface.MYINSTANCE().HandleRcadToolbar_Click("Whole Shape");
                        break;
                    case "Whole Shape":
                        GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Bounded Partial Shapes", "Bounded Partial Shapes", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        RcadScrollbar.AssignSingleBtnProp(Btn, "Bounded Partial Shapes", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        RWrapper.RW_MainInterface.MYINSTANCE().HandleRcadToolbar_Click("Bounded Partial Shapes");
                        break;
                    case "Save Image":
                        {
                            RWrapper.RW_MainInterface.MYINSTANCE().ClearShapeAndMeasure_Selections();
                            System.Threading.Thread.Sleep(50);
                            System.Windows.Forms.SaveFileDialog sd = new System.Windows.Forms.SaveFileDialog();
                            //sd.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.Desktop);
                            sd.Filter = "(*.jpg)JPEG Files|*.jpg|(*.bmp)Bitmap Files|*.bmp";
                            sd.Title = "Save";
                            sd.DefaultExt = ".jpg";
                            if (sd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                            {
                                RWrapper.RW_MainInterface.MYINSTANCE().SaveRcadWindowImage(sd.FileName, true);
                            }
                        }
                        break;
                    case "Nudge Selected Shapes":
                    case "Rotate":
                        {
                            DxfNudgeRotateStepWin.Hide();
                            double StepValue = 0;
                            if (BtnName == "Nudge Selected Shapes")
                            {
                                if ((bool)((RadioButton)Btn).IsChecked)
                                {
                                    RWrapper.RW_MainInterface.MYINSTANCE().SetNudge_RotateForRCad(true, false);
                                    DxfNudgeRotateStepWin.Owner = RcadWin;
                                    DxfNudgeRotateStepWin.Show();
                                    DxfNudgeRotateStepWin.Activate();
                                    DxfNudgeRotateStepWin.txtUnitType.Text = "µm";
                                    if (double.TryParse(DxfNudgeRotateStepWin.ValTxtBx.Text, out StepValue))
                                        RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(StepValue, 0, 0);
                                }
                                else
                                {
                                    RWrapper.RW_MainInterface.MYINSTANCE().SetNudge_RotateForRCad(false, false);
                                }
                            }
                            else if (BtnName == "Rotate")
                            {
                                if ((bool)((RadioButton)Btn).IsChecked)
                                {
                                    RWrapper.RW_MainInterface.MYINSTANCE().SetNudge_RotateForRCad(false, true);
                                    DxfNudgeRotateStepWin.Owner = this; // RcadWin;
                                    DxfNudgeRotateStepWin.Show();
                                    DxfNudgeRotateStepWin.Activate();
                                    DxfNudgeRotateStepWin.txtUnitType.Text = "min";
                                    if (double.TryParse(DxfNudgeRotateStepWin.ValTxtBx.Text, out StepValue))
                                        RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(StepValue, 0, 0);
                                }
                                else
                                {
                                    RWrapper.RW_MainInterface.MYINSTANCE().SetNudge_RotateForRCad(false, false);
                                }
                            }
                        }
                        break;
                    case "Align":
                        RWrapper.RW_MainInterface.MYINSTANCE().SetAlignmentCorrectionValue(GlobalSettings.DxfAutoAlignOffset_XVals.ToArray(), GlobalSettings.DxfAutoAlignOffset_YVals.ToArray(), GlobalSettings.DxfAutoAlignDistVals.ToArray());
                        break;
                    case "TransformSelectedPtsOnly":
                        if ((bool)((ToggleButton)Btn).IsChecked)
                            TransformSelectedPtsOnly = true;
                        else
                            TransformSelectedPtsOnly = false;
                        break;
                    case "TransLateNRotateCldPts":
                        RWrapper.RW_MainInterface.MYINSTANCE().AutoAlignmentForSelectedentities(TransformSelectedPtsOnly, 0);
                        break;
                    case "TranslateCloudPts":
                        RWrapper.RW_MainInterface.MYINSTANCE().AutoAlignmentForSelectedentities(TransformSelectedPtsOnly, 1);
                        break;
                    case "RotateCloudPts":
                        RWrapper.RW_MainInterface.MYINSTANCE().AutoAlignmentForSelectedentities(TransformSelectedPtsOnly, 2);
                        break;
                    case "Surface Layering":
                        AssignShapePanelToShapeInfoWindow(7, "", "TriangulatedSurfaceLayering", true);
                        break;
                    case "Auto Shape and Measurement":
                        RepeatShapeAndMeasurementWin.MyInstance().Owner = GlobalSettings.MainWin;
                        RepeatShapeAndMeasurementWin.MyInstance().Left = GlobalSettings.MainWin.VideoLeft;
                        RepeatShapeAndMeasurementWin.MyInstance().Top = GlobalSettings.MainWin.VideoTop;
                        RepeatShapeAndMeasurementWin.MyInstance().Show();
                        break;
                    case "ShowAllRarelyBtnInRCAD":
                        RcadScrollbar.ShowHideRarelyUsedButtons();
                        break;
                    case "ZoomExtents":
                        RWrapper.RW_MainInterface.MYINSTANCE().SetRcadZoomToExtent((bool)((ToggleButton)Btn).IsChecked);
                        break;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV16", ex);
            }
        }
        void RCADDropDownItems_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (!InitialCheckAndClear(sender, e)) return;
                ContentControl Btn = (ContentControl)sender;
                ScrToolbar DxfScrollbar = RcadToolbar.GetContent();
                string BtnName = ((ButtonProperties)Btn.Tag).ButtonName;

                if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                {
                    if (!(bool)GlobalSettings.FeatureLicenseInfo[((ButtonProperties)Btn.Tag).FeatureID])
                    {
                        MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                        if (Btn.GetType() == typeof(ToggleButton))
                            ((ToggleButton)Btn).IsChecked = false;
                        if (Btn.GetType() == typeof(RadioButton))
                            ((RadioButton)Btn).IsChecked = false;
                        return;
                    }
                }
                switch (BtnName)
                {
                    case "Export CAD":
                        {
                            RWrapper.RW_MainInterface.MYINSTANCE().DXFExport(false);
                        }
                        break;
                    case "Export Cloud Point as Polyline":
                        {
                            RWrapper.RW_MainInterface.MYINSTANCE().DXFExport(true);
                        }
                        break;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV17aSrcDD", ex);
            }
        }
        void DxfScrollBarButtons_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (!InitialCheckAndClear(sender, e)) return;
                ContentControl Btn = (ContentControl)sender;
                ScrToolbar DxfScrollbar = DXFToolbar.GetContent();
                string BtnName = ((ButtonProperties)Btn.Tag).ButtonName;
                if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                {
                    if (!(bool)GlobalSettings.FeatureLicenseInfo[((ButtonProperties)Btn.Tag).FeatureID])
                    {
                        MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                        if (Btn.GetType() == typeof(ToggleButton))
                            ((ToggleButton)Btn).IsChecked = false;
                        if (Btn.GetType() == typeof(RadioButton))
                            ((RadioButton)Btn).IsChecked = false;
                        return;
                    }
                }
                RWrapper.RW_MainInterface.MYINSTANCE().HandleDXFToolbar_Click(BtnName);
                switch (BtnName)
                {
                    case "Maximize":
                        {
                            double[] TmpPosInfo = new double[4];
                            PrepareForDxfMaximise(ref TmpPosInfo);
                            if (GlobalSettings.IsButtonWithNames)
                            {
                                TmpPosInfo[1] += 25;
                                TmpPosInfo[3] -= 25;
                            }
                            ChangeRcadDxfWidthHeight_Positioning(ref DxfWin, ref DXFToolbar, TmpPosInfo[0],
                                 TmpPosInfo[1],
                                 TmpPosInfo[2],
                                 TmpPosInfo[3], 2, true);
                            SetOwnerofWindow(DxfWin);
                            GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Minimize", "Minimize", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                            DxfScrollbar.AssignSingleBtnProp(Btn, "Minimize", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                            //RWrapper.RW_MainInterface.MYINSTANCE().UpdateDxfGraphics();
                            UpdateOnSeparateThread();
                        }
                        break;
                    case "Minimize":
                        {
                            PrepareForDxfMinimise();
                            ChangeRcadDxfWidthHeight_Positioning(ref DxfWin, ref DXFToolbar, RcadDxfPositionings[0],
                                   RcadDxfPositionings[1], RcadDxfPositionings[2], RcadDxfPositionings[3], 2, false);
                            DxfWin.Owner = this;
                            GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Maximize", "Maximize", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                            DxfScrollbar.AssignSingleBtnProp(Btn, "Maximize", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                            //RWrapper.RW_MainInterface.MYINSTANCE().UpdateVideoGraphics();
                            //RWrapper.RW_MainInterface.MYINSTANCE().UpdateDxfGraphics();
                        }
                        break;
                    case "Nudge CAD":
                    case "Rotate":
                        {
                            DxfNudgeRotateStepWin.Hide();
                            double StepValue = 0;
                            if (BtnName == "Nudge CAD")
                            {
                                if ((bool)((RadioButton)Btn).IsChecked)
                                {
                                    RWrapper.RW_MainInterface.MYINSTANCE().SetNudge_RotateForDxf(true, false);
                                    if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.VisionHeadOnly)
                                        DxfNudgeRotateStepWin.Owner = DxfWin;
                                    else
                                        SetOwnerofWindow(DxfNudgeRotateStepWin);

                                    DxfNudgeRotateStepWin.Show();
                                    DxfNudgeRotateStepWin.Activate();
                                    DxfNudgeRotateStepWin.txtUnitType.Text = "µm";
                                    if (double.TryParse(DxfNudgeRotateStepWin.ValTxtBx.Text, out StepValue))
                                        RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(StepValue, 0, 0);
                                }
                                else
                                {
                                    RWrapper.RW_MainInterface.MYINSTANCE().SetNudge_RotateForDxf(false, false);
                                }
                            }
                            else if (BtnName == "Rotate")
                            {
                                if ((bool)((RadioButton)Btn).IsChecked)
                                {
                                    RWrapper.RW_MainInterface.MYINSTANCE().SetNudge_RotateForDxf(false, true);
                                    if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.VisionHeadOnly)
                                        DxfNudgeRotateStepWin.Owner = DxfWin;
                                    else
                                        SetOwnerofWindow(DxfNudgeRotateStepWin);

                                    //DxfNudgeRotateStepWin.Owner = DxfWin;
                                    DxfNudgeRotateStepWin.Show();
                                    DxfNudgeRotateStepWin.Activate();
                                    DxfNudgeRotateStepWin.txtUnitType.Text = "min";
                                    if (double.TryParse(DxfNudgeRotateStepWin.ValTxtBx.Text, out StepValue))
                                        RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(StepValue, 0, 0);
                                }
                                else
                                {
                                    RWrapper.RW_MainInterface.MYINSTANCE().SetNudge_RotateForDxf(false, false);
                                }
                            }
                        }
                        break;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV17", ex);
            }
        }
        void DxfDropDownItems_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (!InitialCheckAndClear(sender, e)) return;
                ContentControl Btn = (ContentControl)sender;
                ScrToolbar DxfScrollbar = DXFToolbar.GetContent();
                string BtnName = ((ButtonProperties)Btn.Tag).ButtonName;

                if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                {
                    if (!(bool)GlobalSettings.FeatureLicenseInfo[((ButtonProperties)Btn.Tag).FeatureID])
                    {
                        MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                        if (Btn.GetType() == typeof(ToggleButton))
                            ((ToggleButton)Btn).IsChecked = false;
                        if (Btn.GetType() == typeof(RadioButton))
                            ((RadioButton)Btn).IsChecked = false;
                        return;
                    }
                }
                switch (BtnName)
                {
                    case "Export CAD":
                        {
                            RWrapper.RW_MainInterface.MYINSTANCE().DXFExport(false);
                        }
                        break;
                    case "Export Cloud Point as Polyline":
                        {
                            RWrapper.RW_MainInterface.MYINSTANCE().DXFExport(true);
                        }
                        break;
                    case "ShowAllRarelyBtnInDXF":
                        DxfScrollbar.ShowHideRarelyUsedButtons();
                        break;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV17aSrcDD", ex);
            }
        }
        void ZeroingAngleButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                ContentControl Btn = (ContentControl)sender;
                string BtnName = ((ButtonProperties)Btn.Tag).ButtonName;
                if (BtnName == "Zeroing Angle Button")
                {
                    double[] DroValue = { 0, 0, 0, GlobalSettings.ZeroingAngle };
                    //RWrapper.RW_DRO.MYINSTANCE().ResetDroValue(ref DroValue);
                }
                else
                {
                    ButtonProperties BtnProp = (ButtonProperties)Btn.Tag;
                    double ZeroingAngle = 0.0;
                    Double.TryParse(BtnProp.ButtonName, out ZeroingAngle);
                    GlobalSettings.ZeroingAngle = ZeroingAngle;

                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MVZeroingAngle", ex);
            }
        }
        public void CircularInterpolationBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                CircularInterpolationWin CircularMovementWin = new CircularInterpolationWin();
                CircularMovementWin.Owner = this; // RcadWin;
                CircularMovementWin.Show();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:PointClusterBtn", ex);
            }
        }
        public void LinearInterpolationBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                LinearInterpolationWin LinearMovementWin = new LinearInterpolationWin();
                LinearMovementWin.Owner = this; // RcadWin;
                LinearMovementWin.Show();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:PointClusterBtn", ex);
            }
        }
        public void PathInterpolationBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                PathInterpolationWindow.Owner = this; // RcadWin;
                PathInterpolationWindow.Show();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:PointClusterBtn", ex);
            }
        }
        public void ReferancePointBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_LinearInterPolation.MYINSTANCE().TakePointForReference();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:ReferancePoint", ex);
            }
        }

        bool InitialCheckAndClear(object sender, RoutedEventArgs e, int TBarType = 0)
        {//CrosshairTBar = 1 implies draw toolbar, 2 implies crosshair toolbar, 0 implies any other toolbar
            try
            {
                ContentControl btn = (ContentControl)sender;
                if (!btn.IsEnabled)
                    return false;
                if (sender.GetType() == typeof(RadioButton))
                {
                    RadioButton RBtn = (RadioButton)sender;
                    ToolbarRBtn_HandleCheckUncheck(RBtn, e);
                    bool? IsChk = RBtn.IsChecked;

                    if ((bool)IsChk)
                    {
                        if (TBarType == 2)
                        {
                            CHairRBtnChecked = true;
                            CHairRBtnName = (RBtn.Tag as ButtonProperties).ButtonName;
                        }
                        else if (TBarType == 1)
                        {
                            CHairRBtnChecked = false;
                            CHairRBtnName = "";
                        }
                        ClearToolbarSelection(1, TBarType);
                    }
                    else
                    {
                        if (TBarType == 1 && CHairRBtnChecked)
                        {
                            //CHairRBtnChecked = false;
                            //Simulate_ToolbarBtnClick("Crosshair", CHairRBtnName, false);
                            CrosshairScrollBarButtons_Click(CrosshairScrollList.Get_Button(CHairRBtnName), null);
                            return false;
                        }
                        ClearToolbarSelection(1);
                        CHairRBtnChecked = false;
                    }
                    RBtn.IsChecked = IsChk;
                }
                return true;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV18", ex);
                return false;
            }
        }
        bool SaveCheckStatus(object sender, string ToolbarName)
        {
            try
            {
                ContentControl btn = (ContentControl)sender;
                if (!btn.IsEnabled)
                    return false;
                if (sender.GetType() == typeof(RadioButton))
                {
                    RadioButton RBtn = (RadioButton)sender;
                    ButtonProperties BtnProp = (RBtn.Tag as ButtonProperties);
                    if (BtnProp.RememberLastState)
                    {
                        string ButtonName = BtnProp.ButtonName;
                        if (GlobalSettings.SettingsReader.SaveCheckedStatusOfButton("Alignment", ToolbarName, ButtonName, (bool)RBtn.IsChecked))
                        {
                            BtnProp.LastCheckedState = (bool)RBtn.IsChecked;
                        }
                    }
                }
                else if (sender.GetType() == typeof(ToggleButton))
                {
                    ToggleButton TBtn = (ToggleButton)sender;
                    ButtonProperties BtnProp = (TBtn.Tag as ButtonProperties);
                    if (BtnProp.RememberLastState)
                    {
                        string ButtonName = BtnProp.ButtonName;
                        if (GlobalSettings.SettingsReader.SaveCheckedStatusOfButton("Alignment", ToolbarName, ButtonName, (bool)TBtn.IsChecked))
                        {
                            BtnProp.LastCheckedState = (bool)TBtn.IsChecked;
                        }
                    }
                }
                return true;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0587", ex);
                return false;
            }
        }
        void ToolbarRBtn_HandleCheckUncheck(RadioButton rb, RoutedEventArgs e)
        {
            try
            {
                if ((bool)rb.IsChecked)
                    rb.IsChecked = false;
                else
                    rb.IsChecked = true;

                if (e != null)
                    e.Handled = true;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV19", ex);
            }
        }

        void ClearToolbarSelection(int TypeOfBtn = 3, int TBarType = 0)
        {//CrosshairTBar = 1 implies draw toolbar, 2 implies crosshair toolbar, 0 implies any other toolbar
            try
            {
                ScrToolbar[] ToolBarsArray = { MeasureScrollList, DrawScrollList, DerivedShapeScrollList, CrosshairScrollList, EditScrollList, ToolsScrollList, UCSScrollList,
                                             RcadToolbar.GetContent(), DXFToolbar.GetContent() };
                if (TypeOfBtn == 0)
                    for (int i = 0; i < ToolBarsArray.Length; i++)
                        ToolBarsArray[i].ClearNormalBtnState();
                else if (TypeOfBtn == 1)
                    for (int i = 0; i < ToolBarsArray.Length; i++)
                    {
                        if (TBarType == 2 && ToolBarsArray[i] == DrawScrollList) continue;
                        ToolBarsArray[i].ClearRadioBtnSelection();
                    }
                else if (TypeOfBtn == 2)
                    for (int i = 0; i < ToolBarsArray.Length; i++)
                        ToolBarsArray[i].ClearToggBtnSelection();
                else if (TypeOfBtn == 3)
                    for (int i = 0; i < ToolBarsArray.Length; i++)
                        ToolBarsArray[i].ClearAllBtnSelection();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV20", ex);
            }

        }
        void ToolbarSetInitialChkStatus()
        {
            try
            {
                ScrToolbar[] ToolBarsArray = { MeasureScrollList, DrawScrollList, DerivedShapeScrollList, CrosshairScrollList, EditScrollList, ToolsScrollList, UCSScrollList,
                                             RcadToolbar.GetContent(), DXFToolbar.GetContent() };
                for (int i = 0; i < ToolBarsArray.Length; i++)
                {
                    ToolBarsArray[i].SetInitialChkStatus();
                    ToolBarsArray[i].SetLastChkStatus();
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV21", ex);
            }
        }
        void ClearAll_AllToolbars()
        {
            CHairRBtnChecked = false;
            ComboBox TmpCBx;
            GroupBox[] ToolsGrpBxList = { MeasureGrpBx, DrawGrpBx, DerivedShpGrpBx, CrossHairGrpBx, EditGrpBx, ToolsGrpBx, UCSGrpBx };
            for (int i = 0; i < ToolsGrpBxList.Length; i++)
            {
                TmpCBx = (ComboBox)ToolsGrpBxList[i].Template.FindName("SettingsComboBx", ToolsGrpBxList[i]);
                TmpCBx.Text = "";
            }
        }

        public void Simulate_ToolbarBtnClick(string Alignment, string ButtonName, bool status)
        {
            try
            {
                if (Alignment == "Undo" || Alignment == "Delete")
                {
                    DrawScrollList.ClearAllBtnSelection();
                    CrosshairScrollList.ClearAllBtnSelection();
                    return;
                }
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    ScrToolbar activeToolbar = null;
                    ButtonHandler_Pointer Handler_Func;
                    switch (Alignment)
                    {
                        case "Measure":
                            activeToolbar = MeasureScrollList;
                            Handler_Func = MeasureScrollBarButtons_Click;
                            break;
                        case "Draw":
                            activeToolbar = DrawScrollList;
                            Handler_Func = DrawScrollBarButtons_Click;
                            break;
                        case "DerivedShape":
                            activeToolbar = DerivedShapeScrollList;
                            Handler_Func = DerivedShapeScrollBarButtons_Click;
                            break;
                        case "Crosshair":
                            activeToolbar = CrosshairScrollList;
                            Handler_Func = CrosshairScrollBarButtons_Click;
                            break;
                        case "Tools":
                            if (ButtonName == "Login")
                            {
                                activeToolbar = ToolsScrollList;
                                Handler_Func = LoginBtn_Click;
                                break;
                            }
                            activeToolbar = ToolsScrollList;
                            Handler_Func = ToolsScrollBarButtons_Click;
                            break;
                        case "UCS":
                            activeToolbar = UCSScrollList;
                            Handler_Func = UCSScrollBarButtons_Click;
                            break;
                        case "RCAD Toolbar":
                            activeToolbar = RcadToolbar.GetContent();
                            Handler_Func = RcadScrollBarButtons_Click;
                            break;
                        case "DXF Toolbar":
                            activeToolbar = DXFToolbar.GetContent();
                            Handler_Func = DxfScrollBarButtons_Click;
                            break;
                        default:
                            activeToolbar = MeasureScrollList;
                            Handler_Func = MeasureScrollBarButtons_Click;
                            break;
                    }
                    ContentControl Btn = activeToolbar.Check_Togg_or_Radio_Btn(ButtonName, status);
                    if (Btn != null) Handler_Func(Btn, null);
                }
                else
                {
                    this.Dispatcher.Invoke(new RWrapper.RW_MainInterface.RaiseToolbarClickEventHandler(Simulate_ToolbarBtnClick), Alignment, ButtonName, status);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV22", ex);
            }
        }
        public void Simulate_ToolbarBtnCheck(string Alignment, string ButtonName, bool status)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    ClearToolbarSelection(1);
                    ScrToolbar activeToolbar = null;
                    switch (Alignment)
                    {
                        case "Measure":
                            activeToolbar = MeasureScrollList;
                            break;
                        case "Draw":
                            activeToolbar = DrawScrollList;
                            break;
                        case "DerivedShape":
                            activeToolbar = DerivedShapeScrollList;
                            break;
                        case "Crosshair":
                            activeToolbar = CrosshairScrollList;
                            break;
                        case "Tools":
                            activeToolbar = ToolsScrollList;
                            break;
                        case "UCS":
                            activeToolbar = UCSScrollList;
                            break;
                        case "RCAD Toolbar":
                            activeToolbar = RcadToolbar.GetContent();
                            break;
                        case "DXF Toolbar":
                            activeToolbar = DXFToolbar.GetContent();
                            break;
                    }
                    activeToolbar.Check_Togg_or_Radio_Btn(ButtonName, status);
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RaiseToolbarClickEventHandler(Simulate_ToolbarBtnCheck), Alignment, ButtonName, status);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV23", ex);
            }
        }

        #endregion

        //DateTime PPBuild_s1, PPBuild_s2, PPBuild_s3, PPBuild_s4;
        public void HandleLowerPanelsButtonsClick(object sender, RoutedEventArgs e)
        {
            try
            {
                ContentControl b = (ContentControl)sender;
                string[] tagStr;
                 ButtonProperties bpp;
                if (b.Tag.GetType() == typeof(ButtonProperties))
                    bpp = (ButtonProperties)b.Tag;
                else
                {
                    tagStr = (string[])b.Tag;
                    bpp = new ButtonProperties(tagStr[2], Convert.ToBoolean(tagStr[3]), Convert.ToBoolean(tagStr[4]), tagStr[0] );
                }
               
                switch (bpp.ToolBarName)// tagStr[0])
                {

                    case "Part Program Options":
                        if (b.Equals((ToggleButton)stOptions.Children[2]))
                            ((ToggleButton)stOptions.Children[2]).IsChecked = false;
                        RWrapper.RW_PartProgram.MYINSTANCE().Handle_Program_btnClicks(bpp.ButtonName); //tagStr[2]
                        if (bpp.ButtonName == "Part Program Home") //tagStr[2]
                            CNCPopUp.ClickGoTo();
                        else if (bpp.ButtonName == "Grid Run") //tagStr[2]
                        {
                            TxtGridRunCount.Text = "";
                            if (((ToggleButton)b).IsChecked == true)

                                TxtGridRunCount.Visibility = System.Windows.Visibility.Visible;


                            else
                                TxtGridRunCount.Visibility = System.Windows.Visibility.Collapsed;
                        }
                        break;
                    case "Partprogram":
                        if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                        {
                            if (!(bool)GlobalSettings.FeatureLicenseInfo[Convert.ToInt32(bpp.FeatureID)]) //tagStr[4]
                            {
                                MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                                if (b.GetType() == typeof(ToggleButton))
                                    ((ToggleButton)b).IsChecked = false;
                                if (b.GetType() == typeof(RadioButton))
                                    ((RadioButton)b).IsChecked = false;
                                return;
                            }
                        }
                        switch (bpp.ButtonName) //tagStr[2]
                        {
                            case "View Report":
                                if (PartProgramPopUp.lstReports.SelectedItem != null)
                                {
                                    if (File.Exists(PartProgramPopUp.lstReports.SelectedItem.ToString()))
                                    {
                                        ToggleButton TBtn = null;
                                        try
                                        {
                                            TBtn = stOptions.Children.OfType<ToggleButton>()
                                                .Where(T => ((string[])T.Tag)[2] == "Grid Run").First(); //(string[])  [2]
                                        }
                                        catch (Exception ex)
                                        {
                                            // MessageBox.Show(ex.Message + "    " + ex.StackTrace);
                                            RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0585", ex);
                                        }
                                        if (TBtn != null)
                                        {
                                            if ((bool)TBtn.IsChecked)
                                            {
                                                File.Copy(PartProgramPopUp.lstReports.SelectedItem.ToString(), Environment.GetEnvironmentVariable("TEMP") + "\\Current Report Copy.xls", true);
                                                System.Diagnostics.Process.Start(Environment.GetEnvironmentVariable("TEMP") + "\\Current Report Copy.xls");
                                            }
                                            else
                                            {
                                                System.Diagnostics.Process.Start(PartProgramPopUp.lstReports.SelectedItem.ToString());
                                            }
                                        }
                                    }
                                    else
                                        MessageBox.Show("The report file does not exist. It may have been deleted. Please create a new report and then click on view.", "Rapid-I 5.0");
                                }
                                break;
                            case "Edit Program":
                                if (!CheckLicenseForPP(RWrapper.RW_PartProgram.MYINSTANCE().GetFeatureLIst()))
                                {
                                    if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                                    {
                                        MessageBox.Show("Sorry! You do not have license for the features used in this Program.", "Rapid-I");
                                        return;
                                    }
                                }
                                if (PartProgramPopUp.SelectedProgIndex > -1)
                                {
                                    System.Data.DataRow drr = ((System.Data.DataRowView)PartProgramPopUp.dgvPrograms.Items[PartProgramPopUp.SelectedProgIndex]).Row;
                                    RWrapper.RW_PartProgram.MYINSTANCE().PartProgram_Edit(GlobalSettings.PartProgramPaths[PartProgramPopUp.SelectedProgIndex] + drr[0].ToString());
                                }
                                break;
                            case "Run":
                                if (PartProgramPopUp.SelectedProgIndex >= 0)
                                {
                                    if (!CheckLicenseForPP(RWrapper.RW_PartProgram.MYINSTANCE().GetFeatureLIst()))
                                    {
                                        if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                                        {
                                            MessageBox.Show("Sorry! You do not have license for the features used in this Program.", "Rapid-I");
                                            return;
                                        }
                                    }
                                    GlobalSettings.MainExcelInstance.ReportName = PartProgramPopUp.lstReports.SelectedItem.ToString();
                                    //PartProgramPopUp.dgvPrograms.IsEnabled = false;
                                    //b.Content = null;
                                    //ContentControl cc = St.Children.OfType<ContentControl>().Where(T =>
                                    //                     T.ToolTip.ToString() == "Run" || 
                                    //                     T.ToolTip.ToString() == "Pause" || 
                                    //                     T.ToolTip.ToString() == "Continue").First();
                                    //b.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Pause", "Pause", 32, 32);
                                    //b.ToolTip = "Pause";
                                    St.AssignSingleBtnProp(b, "Pause", "Pause", GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Pause", "Pause", 32, 32));
                                    //((string[])(b.Tag))[2] = "Pause";
                                    ((ButtonProperties)b.Tag).ButtonName = "Pause";
                                    //ButtonProperties bp = (ButtonProperties) b.Tag;
                                    //bp.ButtonName = "Pause";
                                    //b.Tag = bp;
                                    System.Threading.Thread TmpTh = new System.Threading.Thread(GlobalSettings.MainExcelInstance.OpenExcel);
                                    TmpTh.Start();
                                    EnableDisableControlsDuringProgramRun(false);
                                    ProgramRunning = true;
                                    ArrangeForPartProgramRun();

                                    if (GlobalSettings.SwitchOffSnapModeDuringPP)
                                    {
                                        if (SnapStateButton.ToolTip.ToString() != "Snap OFF")
                                        {
                                            SnapStateButton.ToolTip = "VSnap ON";
                                            SnapStateButton_Click(null, null);
                                        }
                                    }
                                    //Saving the home position values
                                    if (GlobalSettings.SaveHomePosition)
                                    {
                                        TextBox[] GotoBoxes = { CNCPopUp.txtgotodroX, CNCPopUp.txtgotodroY, CNCPopUp.txtgotodroZ };
                                        for (int ii = 0; ii < 3; ii++)
                                        {
                                            if (Math.Abs(RWrapper.RW_DRO.CurrentUCSDRO[ii] - double.Parse(GotoBoxes[ii].Text)) > 0.05)
                                                GotoBoxes[ii].Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[ii]);
                                        }
                                        try
                                        {
                                            if (Math.Abs(RWrapper.RW_DRO.CurrentUCSDRO[3] * (180 / Math.PI) - double.Parse(CNCPopUp.txtgotodroR.Text)) > 0.2)
                                                CNCPopUp.txtgotodroR.Text = ConvertAngleToDisplayMode(RWrapper.RW_DRO.CurrentUCSDRO[3]);
                                        }
                                        catch (Exception exx1)
                                        {
                                            RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0597", exx1);
                                        }
                                        //CNCPopUp.txtgotodroY.Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[1]);
                                        //CNCPopUp.txtgotodroZ.Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[2]);
                                        //CNCPopUp.txtgotodroR.Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[3] * (180 / Math.PI));
                                    }
                                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Normal_As_One_Shot ||
                                        GlobalSettings.RapidMachineType == GlobalSettings.MachineType.One_Shot ||
                                        GlobalSettings.RapidMachineType == GlobalSettings.MachineType.OnlineSystem)
                                    {
                                        RWrapper.RW_PartProgram.MYINSTANCE().PartProgram_Run();
                                        //Thread tt1 = new Thread(new ThreadStart(RWrapper.RW_PartProgram.MYINSTANCE().PartProgram_Run));
                                    }

                                    else if ( // &&
                                        GlobalSettings.RapidMachineType != GlobalSettings.MachineType.Magnifier_DSP)
                                    {
                                        if (GlobalSettings.RunAllPPinCNCModeByDefault && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.DSP &&
                                                    GlobalSettings.RapidMachineType != GlobalSettings.MachineType.Horizontal_DSP)
                                        {
                                            try
                                            {
                                                if (!RWrapper.RW_CNC.MYINSTANCE().CNCmode)//CNCRBtn.Content.ToString() == "Manual")
                                                {
                                                    //if (RWrapper.RW_MainInterface.MYINSTANCE().MachineCardFlag < 2) CNCRBtn.Content = "CNC";
                                                    RWrapper.RW_CNC.MYINSTANCE().Activate_CNCModeDRO();
                                                }
                                                else
                                                    RWrapper.RW_PartProgram.MYINSTANCE().PartProgram_Run();
                                            }
                                            catch (Exception exx)
                                            {
                                                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV77a", exx);
                                            }
                                        }
                                        else
                                            RWrapper.RW_PartProgram.MYINSTANCE().PartProgram_Run();
                                    }
                                }
                                else
                                {
                                    if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.OnlineSystem)
                                        MessageBox.Show("Sorry!! No program has been loaded. Please load a program and then run it.", "Error");
                                }
                                break;
                            case "Pause":
                                //b.Content = null;
                                //b.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Continue", "Continue", 32, 32);
                                //b.ToolTip = "Continue";
                                St.AssignSingleBtnProp(b, "Continue", "Continue", GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Continue", "Continue", 32, 32));
                                //((string[])(b.Tag))[2] = "Continue";
                                ((ButtonProperties)b.Tag).ButtonName = "Continue";
                                RWrapper.RW_PartProgram.MYINSTANCE().PartProgram_Pause();
                                EnableDisableControlsDuringProgramRun(true);
                                break;
                            case "Continue":
                                //b.Content = null;
                                //b.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Pause", "Pause", 32, 32);
                                //b.ToolTip = "Pause";
                                St.AssignSingleBtnProp(b, "Pause", "Pause", GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Pause", "Pause", 32, 32));
                                //((string[])(b.Tag))[2] = "Pause";
                                ((ButtonProperties)b.Tag).ButtonName = "Pause";
                                MagnificationList.IsEnabled = false;
                                RWrapper.RW_PartProgram.MYINSTANCE().PartProgram_Continue();
                                break;
                            case "Build":
                                //Generate a new default name for the program & report.
                                MaxCountOfProgram += 1;
                                //PPBuild_s1 = DateTime.Now;
                                //UpdateAllReportNames();
                                //PPBuild_s2 = DateTime.Now;
                                string newProgramName = "Component" + MaxCountOfProgram;
                                string newReportName = "Component" + MaxCountOfProgram + "_Report1";
                                string newReportStyle = "Component" + MaxCountOfProgram + "_ReportStyle1";
                                //Now show these names in the part program build window.
                                PartProgramBuildWin.SetProgramName(newProgramName);
                                PartProgramBuildWin.SetProgramPath(GlobalSettings.ProgramPath);
                                PartProgramBuildWin.SetReportName(newReportName);
                                PartProgramBuildWin.SetReportPath(GlobalSettings.ReportPath);
                                PartProgramBuildWin.SetReportStyle(newReportStyle);
                                PartProgramBuildWin.ResetAllReportCBx();
                                PartProgramBuildWin.Topmost = false;
                                //PPBuild_s3 = DateTime.Now;
                                SetOwnerofWindow(PartProgramBuildWin);
                                PartProgramBuildWin.Activate();
                                //PPBuild_s4 = DateTime.Now;
                                GlobalSettings.MainExcelInstance.UseExistingReport = false;
                                PartProgramBuildWin.AllExistingProgramCBx.SelectedIndex = -1;
                                PartProgramBuildWin.AllExistingProgramCBx.Text = "";
                                PartProgramBuildWin.ShowDialog();
                                //MessageBox.Show( String.Format("{0:F4}", ((PPBuild_s2 - PPBuild_s1).Ticks / 10000).ToString()) + "; " +
                                //     String.Format("{0:F4}", ((PPBuild_s3 - PPBuild_s2).Ticks / 10000).ToString()) + "; " +
                                //      String.Format("{0:F4}", ((PPBuild_s4 - PPBuild_s3).Ticks / 10000).ToString())
                                //      , "Rapid-I");
                                break;
                            //case "Load":
                            //    //Load the program now in Framework on video also
                            //    if (PartProgramPopUp.dgvPrograms.SelectedIndex >= 0)
                            //    {
                            //        System.Data.DataRow dr = ((System.Data.DataRowView)PartProgramPopUp.dgvPrograms.Items[PartProgramPopUp.dgvPrograms.SelectedIndex]).Row;
                            //        RWrapper.RW_PartProgram.MYINSTANCE().PartProgram_Load(PartProgramPaths[PartProgramPopUp.dgvPrograms.SelectedIndex] + dr[0].ToString());
                            //    }
                            //    break;
                            case "Home":

                                break;
                            case "Stop":
                                PartProgramPopUp.dgvPrograms.IsEnabled = true;
                                EnableDisableControlsDuringProgramRun(true);
                                ProgramRunning = false;
                                //CNCRBtn.Content = "Manual";
                                if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.OnlineSystem)
                                {
                                    RWrapper.RW_CNC.MYINSTANCE().Activate_ManualModeDRO();
                                    GlobalSettings.MainExcelInstance.CloseExcel();
                                }
                                RWrapper.RW_PartProgram.MYINSTANCE().PartProgram_Stop();
                                ContentControl c = St.Children.OfType<ContentControl>().Where(T =>
                                    T.ToolTip.ToString() == "Run" || T.ToolTip.ToString() == "Pause" || T.ToolTip.ToString() == "Continue").First();
                                //if (c.ToolTip.ToString() == "Pause" || c.ToolTip.ToString() == "Continue")
                                //c.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32);
                                //c.ToolTip = "Run";
                                St.AssignSingleBtnProp(c, "Run", "Run", GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32));
                                //((string[])(c.Tag))[2] = "Run";
                                ((ButtonProperties)c.Tag).ButtonName = "Run";
                                ArrangeForPartProgramStop();
                                //for (int i = 0; i < PartProgramPopUp.St.Children.Count; i++)
                                //{
                                //    ContentControl c = (ContentControl)PartProgramPopUp.St.Children[i];\
                                //    if (c.ToolTip.ToString() == "Run" || c.ToolTip.ToString() == "Pause" || c.ToolTip.ToString() == "Continue")
                                //    {
                                //        c.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 48, 48);
                                //        c.ToolTip = "Run";
                                //        break;
                                //    }
                                //}
                                break;
                            case "Reference Dot":
                            case "Reference Sphere":
                            case "Probe Position":
                            case "Pause Position":
                            case "Clear Ref Dot and Sphere":
                                RWrapper.RW_PartProgram.MYINSTANCE().Handle_Program_btnClicks(bpp.ButtonName); // tagStr[2]);
                                break;
                            case "Save Video Image":
                                RWrapper.RW_PartProgram.MYINSTANCE().Handle_Program_btnClicks(bpp.ButtonName); // tagStr[2]);
                                break;
                            case "Move Probe To Vision Position":
                                if (!RWrapper.RW_CNC.MYINSTANCE().getCNCMode())
                                {
                                    //if (RWrapper.RW_MainInterface.MYINSTANCE().MachineCardFlag < 2) CNCRBtn.Content = "CNC"; 
                                    RWrapper.RW_CNC.MYINSTANCE().Activate_CNCModeDRO(); 
                                }
                                RWrapper.RW_CNC.MYINSTANCE().MoveProbeToVisionPosition();
                                break;
                            case "1 Point":
                            case "2 Points":
                            case "1 Point 1 Line":
                            case "Reset Align":
                                RWrapper.RW_PartProgram.MYINSTANCE().PartProgram_EditAlign(bpp.ButtonName);// tagStr[2]);
                                break;
                            case "Sigma Mode":
                                RWrapper.RW_PartProgram.MYINSTANCE().PartProgram_EditAlign(bpp.ButtonName); // tagStr[2]);
                                break;
                            case "CircularInterpolationPartProgram":
                                {
                                    CircularInterpolationPartProgWin.MyInstance().Owner = GlobalSettings.MainWin;
                                    CircularInterpolationPartProgWin.MyInstance().Left = RcadLeft + 5;
                                    CircularInterpolationPartProgWin.MyInstance().Top = RcadTop + 5;
                                    CircularInterpolationPartProgWin.MyInstance().Show();
                                }
                                break;
                            default:
                                {
                                    RWrapper.RW_PartProgram.MYINSTANCE().Handle_Program_btnClicks(bpp.ButtonName); // tagStr[2]);
                                }
                                break;
                        }
                        break;
                    case "Thread":
                        {
                            switch (b.Content.ToString())
                            {
                                case "New":
                                    ThreadPopup.btnNew.IsEnabled = false;
                                    ThreadPopup.txtThreadCount.IsEnabled = true;
                                    //ThreadPopup.rdbIncludeMajMinDia.IsEnabled = false;
                                    ThreadPopup.btnGetResult.IsEnabled = true;
                                    ThreadPopup.TopSurfaceFlatChbk.IsEnabled = true;
                                    ThreadPopup.RootSurfaceFlatChbk.IsEnabled = true;
                                    RWrapper.RW_Thread.MYINSTANCE().NewClicked((bool)ThreadPopup.TopSurfaceFlatChbk.IsChecked, (bool)ThreadPopup.RootSurfaceFlatChbk.IsChecked);
                                    break;
                                case "Get Result":
                                    if (RWrapper.RW_Thread.MYINSTANCE().CalculateThreadMeasurement())
                                    {
                                        ThreadPopup.btnNew.IsEnabled = true;
                                        ThreadPopup.btnGetResult.IsEnabled = false;
                                        ThreadPopup.txtThreadCount.IsEnabled = false;
                                        ThreadPopup.TopSurfaceFlatChbk.IsEnabled = false;
                                        ThreadPopup.RootSurfaceFlatChbk.IsEnabled = false;
                                        //ThreadPopup.rdbIncludeMajMinDia.IsEnabled = true;
                                        ThreadPopup.dgvThreadProperties.SetValue(DataGrid.ItemsSourceProperty, RWrapper.RW_Thread.MYINSTANCE().ThreadMeasureTable.DefaultView);
                                    }
                                    break;
                            }
                        }
                        break;
                    case "PCD":
                        {
                            switch (b.Content.ToString())
                            {
                                case "New PCD":
                                    RWrapper.RW_PCD.MYINSTANCE().NewClicked();
                                    break;
                                case "Measure":
                                    RWrapper.RW_PCD.MYINSTANCE().PCD_MeasurementClicked();
                                    break;
                                case "Export":
                                    System.Windows.Forms.SaveFileDialog sd = new System.Windows.Forms.SaveFileDialog();
                                    sd.Filter = "(*.xls)Excel Files|*.xls";
                                    sd.Title = "Save as";
                                    sd.DefaultExt = ".xls";
                                    if (sd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                                    {
                                        //GlobalSettings.MainExcelInstance.MeasurementList = RWrapper.RW_PCD.MYINSTANCE().GetPCDMeasureList();
                                        GlobalSettings.MainExcelInstance.ReportName = sd.FileName;
                                        GlobalSettings.MainExcelInstance.Reset_All();
                                        GlobalSettings.MainExcelInstance.GenerateExcel(false, true);
                                    }
                                    break;
                            }
                        }
                        break;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV24", ex);
            }
        }

        #region handling the open gl windows Mouse realted events.

        void win_MouseEnterLeave(object sender, MouseEventArgs e)
        {
            try
            {
                //Now call the appropriate functions
                Window win = (Window)sender;
                if (win.IsMouseOver)
                {
                    if (sender.Equals(RcadWin))
                    {
                        if (e.MiddleButton == MouseButtonState.Pressed)
                            RWrapper.RW_MainInterface.MYINSTANCE().OnRCadMiddleMouseDown(e.GetPosition(RcadWin).X, e.GetPosition(RcadWin).Y);
                        RWrapper.RW_MainInterface.MYINSTANCE().OnRCadMouseEnter();
                    }
                    else if (sender.Equals(DxfWin))
                    {
                        if (e.MiddleButton == MouseButtonState.Pressed)
                            RWrapper.RW_MainInterface.MYINSTANCE().OnDXFMiddleMouseDown(e.GetPosition(DxfWin).X, e.GetPosition(DxfWin).Y);
                        RWrapper.RW_MainInterface.MYINSTANCE().OnDXFMouseEnter();
                    }
                    else if (sender.Equals(ThreadWin))
                    {
                        //LowerTabWin2.ShowWindow();
                        RWrapper.RW_Thread.MYINSTANCE().OnThreadMouseEnter();
                    }
                    else if (sender.Equals(OverlapImgWin))
                    {
                        RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinMouseEnter();
                    }
                }
                else
                {
                    if (sender.Equals(RcadWin))
                        RWrapper.RW_MainInterface.MYINSTANCE().OnRCadMouseLeave();
                    else if (sender.Equals(DxfWin))
                        RWrapper.RW_MainInterface.MYINSTANCE().OnDXFMouseLeave();
                    else if (sender.Equals(ThreadWin))
                    {
                        RWrapper.RW_Thread.MYINSTANCE().OnThreadMouseLeave();
                        //if (!ThreadPopup.IsMouseDirectlyOver)
                        //LowerTabWin2.HideWindow();
                    }
                    else if (sender.Equals(OverlapImgWin))
                    {
                        RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinMouseLeave();
                    }
                }

            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV25", ex);
            }
        }
        void OpenglWin_PreviewMouseWheel(object sender, MouseWheelEventArgs e)
        {
            try
            {
                //Get the position of the mouse a a point
                int mouseDelta = e.Delta;

                FrameworkMouseWheel(mouseDelta, sender as Window);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV26", ex);
            }
        }
        void OpenglWin_PreviewMouseDown(object sender, MouseButtonEventArgs e)
        {
            try
            {
                //Get the position of the mouse a a point
                Point mousePoint = e.GetPosition((IInputElement)sender);
                //Get the calling window refrence
                Window win = (Window)sender;
                if (win == RcadWin)
                    if (ProcessingTouch)
                        return;
                //Now check which button is pressed.
                if (e.ChangedButton == MouseButton.Left)
                    FrameWorkLeftMouseDown(mousePoint, win);
                else if (e.ChangedButton == MouseButton.Middle)
                    FrameWorkMiddleMouseDown(mousePoint, win);
                else if (e.ChangedButton == MouseButton.Right)
                    FrameWorkRightMouseDown(mousePoint, win);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV27", ex);
            }
        }
        void OpenglWin_PreviewMouseUp(object sender, MouseButtonEventArgs e)
        {
            try
            {
                //Get the position of the mouse a a point
                Point mousePoint = e.GetPosition((IInputElement)sender);
                //Get the calling window refrence
                Window win = (Window)sender;
                if (win == RcadWin)
                    if (ProcessingTouch)
                        return;
                //Now check which button is pressed.
                if (e.ChangedButton == MouseButton.Left)
                    FrameWorkLeftMouseUp(mousePoint, win);
                else if (e.ChangedButton == MouseButton.Middle)
                    FrameWorkMiddleMouseUp(mousePoint, win);
                else if (e.ChangedButton == MouseButton.Right)
                    FrameWorkRightMouseUp(mousePoint, win);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV28", ex);
            }
        }
        void OpenglWin_PreviewMouseMove(object sender, MouseEventArgs e)
        {
            try
            {
                //Get the position of the mouse a a point
                Point mousePoint = e.GetPosition((IInputElement)sender);
                //Get the calling window refrence
                Window win = (Window)sender;
                if (win == RcadWin)
                    if (ProcessingTouch)
                        return;
                FrameWorkMouseMove(mousePoint, win);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV29", ex);
            }
        }

        void OpenglWin_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            try
            {
                //Get the position of the mouse a a point
                Point mousePoint = e.GetPosition((IInputElement)sender);
                //Get the calling window refrence
                Window win = (Window)sender;
                if (win == RcadWin)
                    if (ProcessingTouch)
                        return;
                //Now check which button is pressed.
                if (e.ChangedButton == MouseButton.Left)
                    FrameWorkMouseDoubleClick(mousePoint, win);
                //else if (e.ChangedButton == MouseButton.Middle)
                //    FrameWorkMiddleMouseDown(mousePoint, win);
                //else if (e.ChangedButton == MouseButton.Right)
                //    FrameWorkRightMouseDown(mousePoint, win);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV27D", ex);
            }
        }

        //Touch related events
        void OpenglWin_ManipulationDelta(object sender, ManipulationDeltaEventArgs e)
        {
            try
            {
                //Get the position of the mouse a a point
                int mouseDelta = (int)(e.DeltaManipulation.Expansion.X);
                if (mouseDelta == 0) return;
                if (AllowZoom)
                {
                    //Now call the appropriate functions
                    if (sender.Equals(RcadWin))
                        RWrapper.RW_MainInterface.MYINSTANCE().OnRCadMouseWheel(mouseDelta);
                    else if (sender.Equals(DxfWin))
                        RWrapper.RW_MainInterface.MYINSTANCE().OnDXFMouseWheel(mouseDelta);
                    else if (sender.Equals(ThreadWin))
                        RWrapper.RW_Thread.MYINSTANCE().OnThreadMouseWheel(mouseDelta);
                    else if (sender.Equals(OverlapImgWin))
                        RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinMouseWheel(mouseDelta);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV30", ex);
            }
        }

        int Tch1 = -1, Tch2 = -1;
        Point TchPt1, TchPt2;
        bool AllowZoom = false, ProcessingTouch = false;
        double ZoomScale = 1, initialTchSeparation = 0;

        void MainView_TouchDown(object sender, TouchEventArgs e)
        {
            try
            {
                //Get the position of the mouse as a point
                Point mousePoint = e.GetTouchPoint((IInputElement)sender).Position;
                //Get the calling window refrence
                Window win = (Window)sender;
                ProcessingTouch = true;

                if (Tch1 == -1)
                {
                    Tch1 = e.TouchDevice.Id;
                    TchPt1 = mousePoint;
                }
                else if (Tch2 == -1)
                {
                    Tch2 = e.TouchDevice.Id;
                    TchPt2 = mousePoint;
                }

                if (Tch1 != -1 && Tch2 != -1)
                {
                    AllowZoom = true;
                    SetGestureCenterPt(mousePoint, win);
                }
                else
                {
                    FrameWorkLeftMouseDown(mousePoint, win);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV31", ex);
            }
        }
        void MainView_TouchUp(object sender, TouchEventArgs e)
        {
            try
            {
                //Get the position of the mouse as a point
                Point mousePoint = e.GetTouchPoint((IInputElement)sender).Position;
                //Get the calling window refrence
                Window win = (Window)sender;

                AllowZoom = false;
                if (Tch1 == e.TouchDevice.Id)
                    Tch1 = -1;
                else if (Tch2 == e.TouchDevice.Id)
                    Tch2 = -1;

                if (Tch1 == -1 && Tch2 == -1)
                {
                    ProcessingTouch = false;
                    FrameWorkLeftMouseUp(mousePoint, win);
                }
                else
                {
                    if (Tch1 > -1)
                        FrameWorkLeftMouseDown(TchPt1, win);
                    else
                        FrameWorkLeftMouseDown(TchPt2, win);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV32", ex);
            }
        }
        void MainView_TouchMove(object sender, TouchEventArgs e)
        {
            try
            {
                //Get the position of the mouse as a point
                Point mousePoint = e.GetTouchPoint((IInputElement)sender).Position;
                //Get the calling window refrence
                Window win = (Window)sender;

                if (Tch1 > -1 && Tch2 > -1)
                {
                    if (Tch1 == e.TouchDevice.Id)
                        TchPt1 = mousePoint;
                    else
                        TchPt2 = mousePoint;

                    Point GestureCenterPoint = new Point((TchPt1.X + TchPt2.X) / 2, (TchPt1.Y + TchPt2.Y) / 2);
                    FrameWorkMouseMove(GestureCenterPoint, win);

                    double TchSeparation = Math.Sqrt(Math.Pow(TchPt1.X - TchPt2.X, 2) + Math.Pow(TchPt1.Y - TchPt2.Y, 2));
                    ZoomScale = TchSeparation / initialTchSeparation;
                    if (ZoomScale > 1.025)
                    {
                        FrameworkMouseWheel(10, win);
                        initialTchSeparation = TchSeparation;
                    }
                    else if (ZoomScale < 0.975)
                    {
                        FrameworkMouseWheel(-10, win);
                        initialTchSeparation = TchSeparation;
                    }
                }
                else
                {
                    FrameWorkMouseMove(mousePoint, win);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV33", ex);
            }
        }
        void SetGestureCenterPt(Point MPt, Window win)
        {
            try
            {
                if (MPt == TchPt1)
                    FrameWorkLeftMouseUp(TchPt2, win);
                else
                    FrameWorkLeftMouseUp(TchPt1, win);
                //Get the position zoom gesture
                Point GestureCenterPoint = new Point((TchPt1.X + TchPt2.X) / 2, (TchPt1.Y + TchPt2.Y) / 2);
                initialTchSeparation = Math.Sqrt(Math.Pow(TchPt1.X - TchPt2.X, 2) + Math.Pow(TchPt1.Y - TchPt2.Y, 2));
                ZoomScale = 1;
                FrameWorkMouseMove(GestureCenterPoint, win);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV34", ex);
            }
        }

        void FrameworkMouseWheel(int Delta, Window win)
        {
            try
            {
                //Now call the appropriate functions
                if (win.Equals(RcadWin))
                    RWrapper.RW_MainInterface.MYINSTANCE().OnRCadMouseWheel(Delta);
                else if (win.Equals(DxfWin))
                    RWrapper.RW_MainInterface.MYINSTANCE().OnDXFMouseWheel(Delta);
                else if (win.Equals(ThreadWin))
                    RWrapper.RW_Thread.MYINSTANCE().OnThreadMouseWheel(Delta);
                else if (win.Equals(OverlapImgWin))
                    RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinMouseWheel(Delta);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV35", ex);
            }
        }
        void FrameWorkLeftMouseDown(Point MPt, Window win)
        {
            try
            {
                if (win.Equals(RcadWin))
                    RWrapper.RW_MainInterface.MYINSTANCE().OnRCadLeftMouseDown(MPt.X, MPt.Y);
                else if (win.Equals(DxfWin))
                {
                    RWrapper.RW_MainInterface.MYINSTANCE().OnDXFLeftMouseDown(MPt.X, MPt.Y);
                    if (SaoiPopUp != null)
                    {
                        SetCordinatesForSaoi(MPt.X, MPt.Y);
                    }
                }
                else if (win.Equals(ThreadWin))
                    RWrapper.RW_Thread.MYINSTANCE().OnThreadLeftMouseDown(MPt.X, MPt.Y);
                else if (win.Equals(OverlapImgWin))
                    RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinLeftMouseDown(MPt.X, MPt.Y);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV36", ex);
            }
        }
        void SetCordinatesForSaoi(double x, double y)
        {
            if (!SaoiPopUp.isBottomRightset)
            {
                SaoiPopUp.isBottomRightset = true;
                SaoiPopUp.txtTopX.Text = x.ToString();
                SaoiPopUp.txtTopY.Text = y.ToString();
                SaoiPopUp.BottomRightX = x;
                SaoiPopUp.BottomRightY = y;

            }
            if (!SaoiPopUp.isTopLeftSet)
            {
                SaoiPopUp.isTopLeftSet = true;
                SaoiPopUp.txtTopX.Text = x.ToString();
                SaoiPopUp.txtTopY.Text = y.ToString();
                SaoiPopUp.TopLeftX = x;
                SaoiPopUp.TopLeftY = y;
            }
        }
        void FrameWorkLeftMouseUp(Point MPt, Window win)
        {
            try
            {
                if (win.Equals(RcadWin))
                    RWrapper.RW_MainInterface.MYINSTANCE().OnRCadLeftMouseUp(MPt.X, MPt.Y);
                else if (win.Equals(DxfWin))
                    RWrapper.RW_MainInterface.MYINSTANCE().OnDXFLeftMouseUp(MPt.X, MPt.Y);
                else if (win.Equals(ThreadWin))
                    RWrapper.RW_Thread.MYINSTANCE().OnThreadLeftMouseUp(MPt.X, MPt.Y);
                else if (win.Equals(OverlapImgWin))
                    RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinLeftMouseUp(MPt.X, MPt.Y);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV37", ex);
            }
        }
        void FrameWorkRightMouseDown(Point MPt, Window win)
        {
            try
            {
                if (win.Equals(RcadWin))
                {
                    if (RcadToolbar.GetContent().CheckIfButtonExists("3D Mode"))
                    {
                        ToggleButton DBtn = ((ToggleButton)RcadToolbar.GetContent().Get_Button("3D Rotate"));
                        if ((bool)(DBtn.IsChecked))
                        {
                            DBtn.RaiseEvent(new RoutedEventArgs(ToggleButton.ClickEvent));
                            DBtn.IsChecked = false;
                        }
                    }
                    RWrapper.RW_MainInterface.MYINSTANCE().OnRCadRightMouseDown(MPt.X, MPt.Y);
                }
                else if (win.Equals(DxfWin))
                    RWrapper.RW_MainInterface.MYINSTANCE().OnDXFRightMouseDown(MPt.X, MPt.Y);
                else if (win.Equals(ThreadWin))
                    RWrapper.RW_Thread.MYINSTANCE().OnThreadRightMouseDown(MPt.X, MPt.Y);
                else if (win.Equals(OverlapImgWin))
                    RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinRightMouseDown(MPt.X, MPt.Y);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV38", ex);
            }
        }
        void FrameWorkRightMouseUp(Point MPt, Window win)
        {
            try
            {
                if (win.Equals(RcadWin))
                    RWrapper.RW_MainInterface.MYINSTANCE().OnRCadRightMouseUp(MPt.X, MPt.Y);
                else if (win.Equals(DxfWin))
                    RWrapper.RW_MainInterface.MYINSTANCE().OnDXFRightMouseUp(MPt.X, MPt.Y);
                else if (win.Equals(ThreadWin))
                    RWrapper.RW_Thread.MYINSTANCE().OnThreadRightMouseUp(MPt.X, MPt.Y);
                else if (win.Equals(OverlapImgWin))
                    RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinRightMouseUp(MPt.X, MPt.Y);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV39", ex);
            }
        }
        void FrameWorkMiddleMouseDown(Point MPt, Window win)
        {
            try
            {
                if (win.Equals(RcadWin))
                    RWrapper.RW_MainInterface.MYINSTANCE().OnRCadMiddleMouseDown(MPt.X, MPt.Y);
                else if (win.Equals(DxfWin))
                    RWrapper.RW_MainInterface.MYINSTANCE().OnDXFMiddleMouseDown(MPt.X, MPt.Y);
                else if (win.Equals(ThreadWin))
                    RWrapper.RW_Thread.MYINSTANCE().OnThreadMiddleMouseDown(MPt.X, MPt.Y);
                else if (win.Equals(OverlapImgWin))
                    RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinMiddleMouseDown(MPt.X, MPt.Y);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV40", ex);
            }
        }
        void FrameWorkMiddleMouseUp(Point MPt, Window win)
        {
            try
            {
                if (win.Equals(RcadWin))
                    RWrapper.RW_MainInterface.MYINSTANCE().OnRCadMiddleMouseUp(MPt.X, MPt.Y);
                else if (win.Equals(DxfWin))
                    RWrapper.RW_MainInterface.MYINSTANCE().OnDXFMiddleMouseUp(MPt.X, MPt.Y);
                else if (win.Equals(ThreadWin))
                    RWrapper.RW_Thread.MYINSTANCE().OnThreadMiddleMouseUp(MPt.X, MPt.Y);
                else if (win.Equals(OverlapImgWin))
                    RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinMiddleMouseUp(MPt.X, MPt.Y);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV41", ex);
            }
        }
        void FrameWorkMouseMove(Point MPt, Window win)
        {
            try
            {
                if (win.Equals(RcadWin))
                {
                    RWrapper.RW_MainInterface.MYINSTANCE().OnRCadMouseMove(MPt.X, MPt.Y);
                    //txt_MM_X.Text = String.Format("{0:F4}", RWrapper.RW_MainInterface.MYINSTANCE().RCadMousePos[0]);// MPt.X.ToString();
                    //txt_MM_Y.Text = String.Format("{0:F4}", RWrapper.RW_MainInterface.MYINSTANCE().RCadMousePos[1]);//MPt.Y.ToString();
                }
                else if (win.Equals(DxfWin))
                    RWrapper.RW_MainInterface.MYINSTANCE().OnDXFMouseMove(MPt.X, MPt.Y);
                else if (win.Equals(ThreadWin))
                    RWrapper.RW_Thread.MYINSTANCE().OnThreadMouseMove(MPt.X, MPt.Y);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV42", ex);
            }
        }
        void FrameWorkMouseDoubleClick(Point MPt, Window win)
        {
            try
            {
                if (win.Equals(RcadWin))
                    RWrapper.RW_MainInterface.MYINSTANCE().OnRCadDoubleLeftMouseDown(MPt.X, MPt.Y);
                //else if (win.Equals(DxfWin))
                //    RWrapper.RW_MainInterface.MYINSTANCE().OnDXFRightMouseDown(MPt.X, MPt.Y);
                //else if (win.Equals(ThreadWin))
                //    RWrapper.RW_Thread.MYINSTANCE().OnThreadRightMouseDown(MPt.X, MPt.Y);
                //else if (win.Equals(OverlapImgWin))
                //    RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinRightMouseDown(MPt.X, MPt.Y);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV38", ex);
            }
        }

        #endregion

        #region ContextMenuHandling
        void RightClickedOnEntity(int i, string Category)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    switch (i)
                    {
                        case 1://Shapes
                            Point currentMousePosition = Mouse.GetPosition(this);
                            VideoContextMenu.IsOpen = true;
                            break;
                        case 2://Measurements
                            Point currentMousePo = Mouse.GetPosition(this);
                            RcadContextMenu.IsOpen = true;
                            ChangeRcadContextMenuContents("RcadContextMenu", ref RcadContextMenu, Category);
                            break;
                        case 3://Surface Edge Algorithms
                            ChooseAlgoContextMenu.Visibility = System.Windows.Visibility.Visible;
                            ChooseAlgoContextMenu.IsOpen = true;
                            break;
                    }
                }
                else
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RightClickOnEntityEventHandler(RightClickedOnEntity), i, Category);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV43", ex);
            }
        }

        //This is to syncronize the position of VideoContextmenu, since it is shared by both Video Window & Shape List Panel
        private void MeasurementPanel_PreviewMouseRightButtonDown(object sender, RoutedEventArgs e)
        {
            try
            {
                Point currentMousePos = Mouse.GetPosition(this);
                //MeasurementsPopup.UpdateMeasurementSelection();
                ChangeRcadContextMenuContents("RcadContextMenu", ref RcadContextMenu, "RightClickOnMeasure");
                //VideoContextMenu.Margin = new Thickness(currentMousePos.X, 0, 0, 0);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV44", ex);
            }
        }
        //Handling the context menus items click events
        public void HandlingContextMenuItemClick(object sender, RoutedEventArgs e)
        {
            try
            {
                e.Handled = true;
                //Take an refrence to the menu item.
                MenuItem M = (MenuItem)e.Source;
                switch (M.Name)
                {
                    case "VideoContextMenu":
                        {
                            string[] SeletedStr = (string[])M.Tag;
                            RWrapper.RW_MainInterface.MYINSTANCE().HandleRightClick_OnEntities(SeletedStr[0], SeletedStr[1]);
                            switch (SeletedStr[1])
                            {
                                case "Rename Shape":
                                    ShapeParameterWin.RenameShape();
                                    break;
                                case "FG Points Visible":
                                    ShowHideFGpointsStatus = "FG Points Hidden";
                                    break;
                                case "FG Points Hidden":
                                    ShowHideFGpointsStatus = "FG Points Visible";
                                    break;
                                case "Shape Parameters":
                                    ShapesRBtn.IsChecked = true;
                                    if (ShapeParameterWin.ShapePointsDG.ItemsSource != null)
                                        ShapeParameterWin.ShapePointsDG.Columns[ShapeParameterWin.ShapePointsDG.Columns.Count - 1].Visibility = System.Windows.Visibility.Hidden;
                                    break;
                                case "Set For Offset Correction":
                                    if (GlobalSettings.NumberOfAxes == 4)
                                    {
                                        AssignShapePanelToShapeInfoWindow(8, "Select Coordinate to offset;X;Y;Z;R", "ShapeForOffsetCorrection", true);
                                    }
                                    else
                                    {
                                        AssignShapePanelToShapeInfoWindow(8, "Select Coordinate to offset;X;Y;Z", "ShapeForOffsetCorrection", true);
                                    }
                                    break;
                            }
                        }
                        break;
                    case "RcadContextMenu":
                        {
                            string[] SeletedStr = (string[])M.Tag;
                            RWrapper.RW_MainInterface.MYINSTANCE().HandleRightClick_OnEntities(SeletedStr[0], SeletedStr[1]);

                            if (SeletedStr[1] == "Deviation")
                            {
                                AssignShapePanelToShapeInfoWindow(3, "Center X;Center Y;Radius", "ArcDiviation");
                            }
                            else if (SeletedStr[1] == "Rename Measurement")
                                MeasurementsPopup.RenameMeasurement();
                        }
                        break;
                    case "ChooseAlgoContextMenu":
                        {
                            for (int i = 0; i < ChooseAlgoContextMenu.Items.Count; i++)
                            {
                                if (!sender.Equals(ChooseAlgoContextMenu.Items[i]))
                                    ((MenuItem)ChooseAlgoContextMenu.Items[i]).IsChecked = false;
                            }
                            string[] SeletedStr = (string[])M.Tag;
                            RWrapper.RW_MainInterface.MYINSTANCE().HandleRightClick_OnEntities(SeletedStr[0], SeletedStr[1]);
                        }
                        break;
                    case "PPContextMenu":
                        {
                            string[] SeletedStr = (string[])M.Tag;
                            if (SeletedStr[1] == "Rename Program")
                            {
                                if (PartProgramPopUp.RenamePartProgram())
                                {
                                    LoadProgramReportNames(false);
                                }
                            }
                            else if (SeletedStr[1] == "Delete Program")
                            {
                                int ProgramsDeleted = PartProgramPopUp.DeletePartProgram();
                                MaxCountOfProgram -= ProgramsDeleted;
                                if (ProgramsDeleted > 0)
                                {
                                    //MaxCountOfProgram -= 1;
                                    LoadProgramReportNames(false);
                                }
                            }
                            if (SeletedStr[1] == "Export Program")
                            {
                                if (PartProgramPopUp.ExportPartProgram())
                                {
                                    LoadProgramReportNames(false);
                                }
                            }
                            if (SeletedStr[1] == "Import Program")
                            {
                                if (PartProgramPopUp.ImportPartProgram())
                                {
                                    LoadProgramReportNames(false);
                                }
                            }
                        }
                        break;
                    case "TakeProbePoint":
                        {
                            string[] SeletedStr = (string[])M.Tag;
                            if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.DSP && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.Horizontal_DSP)
                            {
                                switch (SeletedStr[1])
                                {
                                    case "TakeProbePoint":
                                        RWrapper.RW_CNC.MYINSTANCE().SendDROToGetProbePoint(true);
                                        break;
                                    case "GotoProbePosition":
                                        RWrapper.RW_CNC.MYINSTANCE().SendDROToGetProbePoint(false);
                                        break;
                                }
                            }
                        }
                        break;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV45", ex);
            }
        }

        //Function to change the contents of the RCAD Context menu
        private void ChangeRcadContextMenuContents(string NameOfContextmenu, ref ContextMenu _contextMenu, string SelectStr)
        {
            try
            {
                List<Image> RcadContextMenuImages;
                RcadContextMenuImages = GlobalSettings.SettingsReader.UpdateBtnInfoList(SelectStr, 24, 24);
                List<int> RcadContextMenuType = GlobalSettings.SettingsReader.ButtontypeList;
                List<string> RcadContextMenuDisplayNames = GlobalSettings.SettingsReader.ButtonToolTipList;
                List<string> RcadContextMenuNames = GlobalSettings.SettingsReader.ButtonBaseNameList;
                _contextMenu.Items.Clear();
                List<ContentMenuObjects> CMObjList = new List<ContentMenuObjects>();
                for (int i = 0; i < RcadContextMenuImages.Count; i++)
                {

                    ContentMenuObjects CMObj = new ContentMenuObjects();
                    CMObj.ParentId = GlobalSettings.SettingsReader.ButtonParentId[i];
                    CMObj.Id = GlobalSettings.SettingsReader.ButtonId[i];
                    CMObj.Image = RcadContextMenuImages[i];
                    CMObj.ContextMenuname = NameOfContextmenu;
                    CMObj.ItemName = RcadContextMenuNames[i];
                    CMObj.HasClickEvent = GlobalSettings.SettingsReader.ButttonHasClick[i];
                    CMObj.ToolTip = RcadContextMenuDisplayNames[i];
                    CMObj.Alignment = SelectStr;
                    if (RcadContextMenuType[i] == 10)
                        CMObj.isSeperator = true;
                    CMObjList.Add(CMObj);
                }
                RapidiContextMenu RCM = new RapidiContextMenu(HandlingContextMenuItemClick);
                RCM.GetMenu(CMObjList, ref _contextMenu);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV46", ex);
            }
        }

        #endregion

        #region DRO
        //handling the DRO buttons click in Frontend
        void DroButton_Click(object sender, MouseEventArgs e)
        {
            try
            {
                if (e.LeftButton == MouseButtonState.Pressed)
                    RWrapper.RW_DRO.MYINSTANCE().Handle_DROButtonClick(((Button)sender).Content.ToString(), true);
                else
                    RWrapper.RW_DRO.MYINSTANCE().Handle_DROButtonClick(((Button)sender).Content.ToString(), false);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV47", ex);
            }
        }
        //Handling A axis button click(TIS)
        void DRObtnA_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                //if ((sender as Button).Content.ToString() == "V")
                //{
                //    //btnA.Content = "H";
                //    RWrapper.RW_CNC.MYINSTANCE().RotateAAxis(true);
                //}
                //else
                //{
                //    //btnA.Content = "V";
                //    RWrapper.RW_CNC.MYINSTANCE().RotateAAxis(false);
                //}
                RWrapper.RW_CNC.MYINSTANCE().Swivel_A_Axis();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV47aa", ex);
            }
        }
        //Axis Lock button clicks
        void AxisLockButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Button thisBtn =(Button)sender;
                string AxisName = (thisBtn).Name;
                AxisName = AxisName.Substring(AxisName.Length - 1, 1);
                if (thisBtn.Tag.ToString() == "Unlocked")
                {
                    RWrapper.RW_CNC.MYINSTANCE().Lock_Axis(AxisName);
                    thisBtn.Background = Brushes.Tomato;
                    thisBtn.Tag = "Locked";
                    thisBtn.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                }
                else
                {
                    RWrapper.RW_CNC.MYINSTANCE().Unlock_Axis(AxisName);
                    thisBtn.Background = Brushes.PaleGreen;
                    thisBtn.Tag = "Unlocked";
                    thisBtn.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;

                }

                //if (sender.Equals(btnLockX))
                //{
                //    if (btnLockX.Tag.ToString() == "Unlocked")
                //    {
                //        RWrapper.RW_CNC.MYINSTANCE().Lock_Axis("X");
                //        //btnLockX.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("AxisLocked", "AxisLocked", 35, 35);
                //        btnLockX.Background = Brushes.Tomato;
                //        btnLockX.Tag = "Locked";
                //        btnLockX.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                //    }
                //    else
                //    {
                //        RWrapper.RW_CNC.MYINSTANCE().Unlock_Axis("X");
                //        //btnLockX.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("AxisUnlocked", "AxisUnlocked", 35, 35);
                //        btnLockX.Background = Brushes.PaleGreen;
                //        btnLockX.Tag = "Unlocked";
                //        btnLockX.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                //    }
                //}
                //else if (sender.Equals(btnLockY))
                //{
                //    if (btnLockY.Tag.ToString() == "Unlocked")
                //    {
                //        RWrapper.RW_CNC.MYINSTANCE().Lock_Axis("Y");
                //        //btnLockY.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("AxisLocked", "AxisLocked", 35, 35);
                //        btnLockY.Background = Brushes.Tomato;
                //        btnLockY.Tag = "Locked";
                //        btnLockY.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                //    }
                //    else
                //    {
                //        RWrapper.RW_CNC.MYINSTANCE().Unlock_Axis("Y");
                //        //btnLockY.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("AxisUnlocked", "AxisUnlocked", 35, 35);
                //        btnLockY.Background = Brushes.Tomato;
                //        btnLockY.Tag = "Unlocked";
                //        btnLockY.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                //    }
                //}
                //else if (sender.Equals(btnLockZ))
                //{
                //    if (btnLockZ.Tag.ToString() == "Unlocked")
                //    {
                //        RWrapper.RW_CNC.MYINSTANCE().Lock_Axis("Z");
                //        //btnLockZ.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("AxisLocked", "AxisLocked", 35, 35);
                //        btnLockZ.Background = Brushes.Tomato;
                //        btnLockZ.Tag = "Locked";
                //        btnLockZ.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                //    }
                //    else
                //    {
                //        RWrapper.RW_CNC.MYINSTANCE().Unlock_Axis("Z");
                //        //btnLockZ.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("AxisUnlocked", "AxisUnlocked", 35, 35);
                //        btnLockZ.Background = Brushes.Tomato;
                //        btnLockZ.Tag = "Unlocked";
                //        btnLockZ.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                //    }
                //}
                //else if (sender.Equals(btnLockR))
                //{
                //    if (btnLockR.Tag.ToString() == "Unlocked")
                //    {
                //        RWrapper.RW_CNC.MYINSTANCE().Lock_Axis("R");
                //        btnLockR.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("AxisLocked", "AxisLocked", 35, 35);
                //        btnLockR.Tag = "Locked";
                //        btnLockR.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                //    }
                //    else
                //    {
                //        RWrapper.RW_CNC.MYINSTANCE().Unlock_Axis("R");
                //        btnLockR.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("AxisUnlocked", "AxisUnlocked", 35, 35);
                //        btnLockR.Tag = "Unlocked";
                //        btnLockR.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                //    }
                //}
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV47a", ex);
            }
        }
        //Function to handle axis lock event from framework
        void HandleAxisLock(int AxisIndx, bool LockAxis)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    string CurrBtnStatus = "Unlocked";
                    if (!LockAxis) CurrBtnStatus = "Locked";
                    switch (AxisIndx)
                    {
                        case 0:
                            if (btnLockX.Tag.ToString() == CurrBtnStatus)
                                AxisLockButton_Click(btnLockX, null);
                            break;
                        case 1:
                            if (btnLockY.Tag.ToString() == CurrBtnStatus)
                                AxisLockButton_Click(btnLockY, null);
                            break;
                        case 2:
                            if (btnLockZ.Tag.ToString() == CurrBtnStatus)
                                AxisLockButton_Click(btnLockZ, null);
                            break;
                        case 3:
                            if (btnLockR.Tag.ToString() == CurrBtnStatus)
                                AxisLockButton_Click(btnLockR, null);
                            break;
                    }
                }
                else
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_CNC.AxisLockEventHandler(HandleAxisLock), AxisIndx, LockAxis);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV48", ex);
            }
        }

        //bool UpdatingDRO; //int SkippedDROUpdateCt = 0;
        //Function to invoke for DRO value Change
        void HandleDroValueChanged()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    //UpdatingDRO = true;
                    Button[] DROBtns = { btnX, btnY, btnZ, btnR };
                    for (int i = 0; i < GlobalSettings.NumberOfAxes; i++)
                    {
                        if (i < GlobalSettings.DROIsLinearList.Count)
                        {
                            if (GlobalSettings.DROIsLinearList[i])
                                DROBtns[i].SetValue(AttachedDROProperties.DROTextProperty, String.Format("{0:F" + GlobalSettings.CurrentDroPrecision + "}", (RWrapper.RW_DRO.DROValue[i])));
                            else
                            {
                                DROBtns[i].SetValue(AttachedDROProperties.DROTextProperty, ConvertAngleToDisplayMode(RWrapper.RW_DRO.DROValue[i]));
                                //if (GlobalSettings.CurrentAngleMode == GlobalSettings.AngleMode.Decimal_Degree)
                                //    DROBtns[i].SetValue(AttachedDROProperties.DROTextProperty, String.Format("{0:F" + GlobalSettings.CurrentDroPrecision + "}", RWrapper.RW_DRO.DROValue[i] * 180 / Math.PI));
                                //else if (GlobalSettings.CurrentAngleMode == GlobalSettings.AngleMode.Radians)
                                //    DROBtns[i].SetValue(AttachedDROProperties.DROTextProperty, String.Format("{0:0.0000}", RWrapper.RW_DRO.DROValue[i]));
                                //else if (GlobalSettings.CurrentAngleMode == GlobalSettings.AngleMode.Degree_Minute_Second)
                                //{
                                //    double Val = RWrapper.RW_DRO.DROValue[i] * 180 / Math.PI;
                                //    string sign = Val < 0 ? "-" : "";
                                //    Val = Math.Abs(Val);
                                //    string RVal = sign + Math.Floor(Val).ToString() + '\u00B0';
                                //    Val = (Val - Math.Floor(Val)) * 60;
                                //    RVal += Math.Floor(Val).ToString("00") + '\'';
                                //    Val = (Val - Math.Floor(Val)) * 60;
                                //    RVal += Math.Floor(Val).ToString("00") + '"';
                                //    DROBtns[i].SetValue(AttachedDROProperties.DROTextProperty, RVal);
                                //}

                            }
                        }
                        else
                        {
                            DROBtns[i].SetValue(AttachedDROProperties.DROTextProperty, String.Format("{0:F" + GlobalSettings.CurrentDroPrecision + "}", (RWrapper.RW_DRO.DROValue[i])));
                        }
                    }
                    xCorr.Text = String.Format("{0:F4}", RWrapper.RW_DRO.MYINSTANCE().Corrections[0]);
                    yCorr.Text = String.Format("{0:F4}", RWrapper.RW_DRO.MYINSTANCE().Corrections[1]);
                    //UpdatingDRO = false;
                    RWrapper.RW_DRO.MYINSTANCE().UpdatingDROTextBox = false;
                }
                else
                {
                    //if (UpdatingDRO)
                    //{
                    //    SkippedDROUpdateCt++;
                    //    if (SkippedDROUpdateCt > 50)
                    //    {
                    //        SkippedDROUpdateCt = 0; UpdatingDRO = false;
                    //    }
                    //    else
                    //        return;
                    //}
                    this.Dispatcher.BeginInvoke(new System.Windows.Forms.MethodInvoker(HandleDroValueChanged));
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV48", ex);
                RWrapper.RW_DRO.MYINSTANCE().UpdatingDROTextBox = false;
            }
        }

        string ConvertAngleToDisplayMode(double CurrentAngle)
        {
            try
            {
                switch (GlobalSettings.CurrentAngleMode)
                {
                    case GlobalSettings.AngleMode.Decimal_Degree:
                        return String.Format("{0:F" + GlobalSettings.CurrentDroPrecision + "}", CurrentAngle * 180 / Math.PI);

                    case GlobalSettings.AngleMode.Radians:
                        return String.Format("{0:0.0000}", CurrentAngle);

                    case GlobalSettings.AngleMode.Degree_Minute_Second:
                        double Val = CurrentAngle * 180 / Math.PI;
                        string sign = Val < 0 ? "-" : "";
                        Val = Math.Abs(Val);
                        string RVal = sign + Math.Floor(Val).ToString() + '\u00B0';
                        Val = (Val - Math.Floor(Val)) * 60;
                        RVal += Math.Floor(Val).ToString("00") + '\'';
                        Val = (Val - Math.Floor(Val)) * 60;
                        RVal += Math.Floor(Val).ToString("00") + '"';
                        return RVal;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV48a", ex);
            }
            return String.Format("{0:F" + GlobalSettings.CurrentDroPrecision + "}", CurrentAngle);
        }

        void HandleSwivelFinishedEvent(String pos)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    btnA.SetValue(AttachedDROProperties.DROTextProperty, pos);
                    //GlobalSettings.A_AxisIsVertical = false;
                    //if (pos == "V") GlobalSettings.A_AxisIsVertical = true;
                }
                else
                    this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Normal,
                                        new RWrapper.RW_DRO.SwivelFinishedEventHandler(HandleSwivelFinishedEvent), pos);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV48b", ex);
            }
        }

        //Function to show or collapse Axis Lock buttons
        void AllowingAxisLock(bool Allow)
        {
            try
            {
                Thickness TmpMargin = new Thickness(4, 0, 4, 0);
                System.Windows.Visibility TmpVisibility = System.Windows.Visibility.Collapsed;
                if (Allow)
                {
                    TmpVisibility = System.Windows.Visibility.Visible;
                    TmpMargin = new Thickness(0, 0, 3, 0);
                }
                btnLockX.Visibility = TmpVisibility;
                btnLockY.Visibility = TmpVisibility;
                btnLockZ.Visibility = TmpVisibility;
                btnLockR.Visibility = TmpVisibility;
                XdroGrid.Margin = TmpMargin;
                YdroGrid.Margin = TmpMargin;
                ZdroGrid.Margin = TmpMargin;
                RdroGrid.Margin = TmpMargin;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV49", ex);
            }
        }

        void HandleDroTargetReached(int Val)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    if (Val == 8)
                    {
                        string DisplayMessage = "The machine seems to be stuck. Please move the DRO manually so that the machine can continue running. We regret this mechanical glitch!";
                        System.Windows.Forms.DialogResult Result = System.Windows.Forms.DialogResult.None;
                        Result = System.Windows.Forms.MessageBox.Show(DisplayMessage, "Rapid I", System.Windows.Forms.MessageBoxButtons.OK);
                        if (Result == System.Windows.Forms.DialogResult.OK)
                            RWrapper.RW_DRO.MYINSTANCE().ResetMachineStuckFlag();
                    }
                    else if (Val == 11)
                    {
                        if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.TIS)
                        {
                            if (RWrapper.TIS_Control.MYINSTANCE().RotaryScanisON)
                            {
                                TISControl.HandleRotaryScanUpdate(RWrapper.TIS_Control.MYINSTANCE().CurrentScanDoneCount);
                            }
                        }
                    }
                    else if (Val == 30)
                    {
                        //The controller is an online system. Let us connect properly. 
                        RWrapper.RW_DRO.MYINSTANCE().ConnectOnlineSystem(true);
                        RWrapper.RW_DRO.MYINSTANCE().Handle_DROTargetReached(Val);
                    }
                    else if (Val == 31)
                    {
                        //The controller is an online system. Let us connect properly. 
                        RWrapper.RW_DRO.MYINSTANCE().ConnectOnlineSystem(false);
                    }
                    else if (Val == 32)
                    {
                        //Online System we got command to run part program....
                        //ReadyToMovetoNextOnlineSlot = false;
                        if (!ProgramRunning)
                        {
                            //We have reached target and next component is ready for analysis. So let us run the Part Program on that and add an entry for that
                            //in our list. (We will remove when we eject the component into appropriate bin. 
                            GlobalSettings.OnlineSystemStates.Add(GlobalSettings.OnlineRunningState.InProcess);
                            ProgramRunning = true;
                            RWrapper.RW_PartProgram.MYINSTANCE().PartProgram_Run();
                            //HandleLowerPanelsButtonsClick(St.Children.OfType<Button>().Where(T => ((string[])T.Tag)[2] == "Run").First(), null);
                        }
                        else 
                            ReadyToRunNextOnlineComponent = true;
                    }
                    else if (Val == 33)
                    {
                        //We have reached target and loaded and ready to go for next step. We will set a variable that we can move to next step
                        //when previous program run completes, it will check for this flag and then give the next goto command.
                        //ReadyToRunNextOnlineComponent = false; 
                        if (!ProgramRunning)
                        {
                            //We have reached the target. We check for the status of the 3 slots.....
                            bool PassPrev = false;
                            bool RejectPrevToPrev = true; // false;

                            //Always consider that there are 3 drills in the table.... 
                            //if (GlobalSettings.OnlineSystemStates.cou)
                            //if (GlobalSettings.OnlineSystemStates.Count > 0 && GlobalSettings.OnlineSystemStates[1] == GlobalSettings.OnlineRunningState.Pass) PassPrev = true;
                            //if (GlobalSettings.OnlineSystemStates.Count > 2 && GlobalSettings.OnlineSystemStates[0] == GlobalSettings.OnlineRunningState.Fail) RejectPrevToPrev = true;
                            if (GlobalSettings.OnlineSystemStates[GlobalSettings.OnlineSystemStates.Count - 1] == GlobalSettings.OnlineRunningState.Pass) PassPrev = true;
                            RWrapper.RW_CNC.MYINSTANCE().MoveTable(PassPrev, RejectPrevToPrev, OnlineControlWindow.Speed);

                            if (GlobalSettings.OnlineSystemStates.Count > 1 && RejectPrevToPrev) GlobalSettings.OnlineSystemStates.RemoveAt(0);
                            if (PassPrev) GlobalSettings.OnlineSystemStates.RemoveAt(0);
                        }
                        else
                            ReadyToMovetoNextOnlineSlot = true;
                    }
                    else
                    {
                            RWrapper.RW_DRO.MYINSTANCE().Handle_DROTargetReached(Val);
                    }
                }
                else
                    this.Dispatcher.BeginInvoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_DRO.DROTargetReachedEventHandler(HandleDroTargetReached), Val);

            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV49", ex);
            }
        }
        //Handler to get user selection of Tprobe stylus during probe hit
        void MainView_TouchedProbePostion()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    if (GlobalSettings.CurrentProbeType == GlobalSettings.ProbeType.Generic_Probe)
                    {
                        if (GenericProb.Myinstance().IsVisible)
                        {
                            GenericProb.Myinstance().SendStylusData();
                        }
                        else
                        {
                            GenericProb.Myinstance().Show();
                        }
                    }
                    else
                    {
                        PrbStylusSelectionWin.ArrangeForCurrentProbeType();
                        if (PrbStylusSelectionWin.IsVisible)
                        {
                            PrbStylusSelectionWin.SendStylusData();
                        }
                        else
                        {
                            PrbStylusSelectionWin.Show();
                        }
                    }
                }
                else
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_DRO.TouchedProbeEventHandler(MainView_TouchedProbePostion));
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV48", ex);
            }
        }
        #endregion

        #region CNC

        void ChangeCNCControlsStatus(bool status)
        {
            try
            {
                GroupBox[] grps = { CNCPopUp.grpGoTo, CNCPopUp.grpMoveBy, CNCPopUp.grpFeedrates };
                for (int i = 0; i < grps.Length; i++)
                {
                    grps[i].IsEnabled = status;
                    if (status)
                        grps[i].Opacity = 1;
                    else
                        grps[i].Opacity = 0.5;
                }
                CNCPopUp.grpGoTo.IsEnabled = status;
                CNCPopUp.grpMoveBy.IsEnabled = status;
                CNCPopUp.grpFeedrates.IsEnabled = status;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV50", ex);
            }
        }
        //handling the CNC Dro reached event
        void HandleCNC_DROTargetReached()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    ChangeCNCControlsStatus(true);
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_CNC.CNCEventHandler(HandleCNC_DROTargetReached));
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV51", ex);
            }
        }

        void HandleCNC_GenerateCNCBtnClick(string CNCMode)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    if (CNCMode == "CNC")
                    {
                        //if (RWrapper.RW_MainInterface.MYINSTANCE().MachineCardFlag < 2) CNCRBtn.Content = "CNC";
                        RWrapper.RW_CNC.MYINSTANCE().Activate_CNCModeDRO();
                    }
                    else
                    {
                        //if (RWrapper.RW_MainInterface.MYINSTANCE().MachineCardFlag < 2) CNCRBtn.Content = "Manual";
                        RWrapper.RW_CNC.MYINSTANCE().Activate_ManualModeDRO();
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_CNC.CNCButtonClickEventHandler(HandleCNC_GenerateCNCBtnClick), CNCMode);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV52", ex);
            }
        }

        void Handle_CNCModeChanged(int Cnt)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    if (Cnt == 1)
                    {
                        CNCRBtn.Content = "CNC";
                        //If we are running part program, let us call the run part program step now that CNC is activated....
                        if (ProgramRunning)
                            RWrapper.RW_PartProgram.MYINSTANCE().PartProgram_Run();
                    }
                    else if (Cnt == 2)
                        CNCRBtn.Content = "Manual";
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_CNC.CNCModeChangeEventHandler(Handle_CNCModeChanged), Cnt);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message + "      " + ex.StackTrace);
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV52a", ex);
            }
        }
        #endregion

        #region DXF

        //Handling the DXF related events
        void HandleDxfUpdate(RWrapper.RW_Enum.RW_DXFPARAMETER_TYPE DxfEventType)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    switch (DxfEventType)
                    {
                        case RWrapper.RW_Enum.RW_DXFPARAMETER_TYPE.DXF_ADDED:
                            DXFAdded();
                            break;
                        case RWrapper.RW_Enum.RW_DXFPARAMETER_TYPE.DXF_REMOVED:
                            DXFRemoved();
                            break;
                        case RWrapper.RW_Enum.RW_DXFPARAMETER_TYPE.DXF_SELECTION_CHANGED:
                            DXFSelectionChanged();
                            break;
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_UCSParameter.DXFParamEventHandler(HandleDxfUpdate), DxfEventType);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV53", ex);
            }
        }
        //Adding a new Dxf
        void DXFAdded()
        {
            try
            {
                //Get the instance of the drop down
                ScrToolbar st = null;
                DropDownList dd = null;
                for (int i = 0; i < DXFToolbar.Children.Count; i++)
                {
                    if (DXFToolbar.Children[i].GetType() == typeof(ScrToolbar))
                    {
                        st = (ScrToolbar)DXFToolbar.Children[i];
                        break;
                    }
                }
                for (int i = 0; i < st.Children.Count; i++)
                {
                    if (st.Children[i].GetType() == typeof(DropDownList))
                    {
                        dd = (DropDownList)st.Children[i];
                        break;
                    }
                }
                //Get the item to be added
                CheckBox chk = new CheckBox();
                //Adding the entity name to the shape hash table for quick ref.
                DXFListHashTable.Add(RWrapper.RW_MainInterface.EntityID, chk);
                //Set the other UI properties
                chk.Content = RWrapper.RW_MainInterface.EntityName;
                chk.Tag = RWrapper.RW_MainInterface.EntityID;
                chk.IsChecked = true;
                //Handle the Checked/Unchecked events
                chk.Checked += new RoutedEventHandler(HandleDXFListButton_Selection);
                chk.Unchecked += new RoutedEventHandler(HandleDXFListButton_Selection);
                //Show the context menu to be shown to delete the Dxfs.
                ContextMenu con = new System.Windows.Controls.ContextMenu();
                MenuItem m = new MenuItem();
                m.Header = "Delete";
                m.Tag = chk.Tag;
                m.Click += new RoutedEventHandler(HandleDeleteDxfClick_Click);
                con.Items.Add(m);
                chk.ContextMenu = con;
                //Add the button to the panel
                dd.lstDropDownItems.Items.Add(chk);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV54", ex);
            }
        }
        //Deleting DXF
        void DXFRemoved()
        {
            try
            {
                //Get the instance of the drop down
                ScrToolbar st = null;
                DropDownList dd = null;
                for (int i = 0; i < DXFToolbar.Children.Count; i++)
                {
                    if (DXFToolbar.Children[i].GetType() == typeof(ScrToolbar))
                    {
                        st = (ScrToolbar)DXFToolbar.Children[i];
                        break;
                    }
                }
                for (int i = 0; i < st.Children.Count; i++)
                {
                    if (st.Children[i].GetType() == typeof(DropDownList))
                    {
                        dd = (DropDownList)st.Children[i];
                        break;
                    }
                }

                CheckBox chk = (CheckBox)DXFListHashTable[RWrapper.RW_MainInterface.EntityID];
                if (dd.lstDropDownItems.Items.Contains(chk))
                {
                    dd.lstDropDownItems.Items.Remove(chk);
                    DXFListHashTable.Remove((int)chk.Tag);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV55", ex);
            }
        }
        //selection change for shapes from framework.
        void DXFSelectionChanged()
        {
            try
            {
                //Get the instance of the drop down
                ScrToolbar st = null;
                DropDownList dd = null;
                for (int i = 0; i < DXFToolbar.Children.Count; i++)
                {
                    if (DXFToolbar.Children[i].GetType() == typeof(ScrToolbar))
                    {
                        st = (ScrToolbar)DXFToolbar.Children[i];
                        break;
                    }
                }
                for (int i = 0; i < st.Children.Count; i++)
                {
                    if (st.Children[i].GetType() == typeof(DropDownList))
                    {
                        dd = (DropDownList)st.Children[i];
                        break;
                    }
                }
                //deselect all the check box.
                foreach (CheckBox chk in dd.lstDropDownItems.Items)
                    chk.IsChecked = false;
                //Get the refrence to the dxf shape list from framewwork
                List<int> FrameworkShapeList = RWrapper.RW_MainInterface.EntityIDList;
                //Now get the related checkbox from Hash table and select them.
                for (int i = 0; i < FrameworkShapeList.Count; i++)
                    ((CheckBox)DXFListHashTable[FrameworkShapeList[i]]).IsChecked = true;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV56", ex);
            }
        }

        //This function handle the delete click of seleted DXF file fom dropdown SelectDxf of DXF toolbar
        void HandleDeleteDxfClick_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_UCSParameter.MYINSTANCE().DeleteDXF((int)((MenuItem)sender).Tag);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV57", ex);
            }
        }
        //Handling the DXF selection changed events.
        void HandleDXFListButton_Selection(object sender, RoutedEventArgs e)
        {
            try
            {
                CheckBox chk = (CheckBox)sender;
                RWrapper.RW_UCSParameter.MYINSTANCE().SelectDXF((int)chk.Tag);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV58", ex);
            }
        }
        //handling the dxf Step change for Nudge & rotate values.
        void DxfNudgeRotateStepValue_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                TextBox t = (TextBox)sender;
                double StepValue = 0;
                if (double.TryParse(t.Text, out StepValue))
                {
                    //RWrapper.RW_MainInterface.MYINSTANCE().SetDxfNudgevalue(StepValue);
                    RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(StepValue, 0, 0);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV59", ex);
            }
        }
        //To consider nudge rotate functions for part program
        void SaveAlignmentBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_MainInterface.MYINSTANCE().SetActionForNudgeRotate();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV59a", ex);
            }
        }

        #endregion

        #region UCS

        //UCS related event handling
        void HandleUCSUpdate(RWrapper.RW_Enum.RW_UCSPARAMETER_TYPE UCSEventType)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    switch (UCSEventType)
                    {
                        case RWrapper.RW_Enum.RW_UCSPARAMETER_TYPE.UCS_ADDED:
                            UCSAdded();
                            break;
                        case RWrapper.RW_Enum.RW_UCSPARAMETER_TYPE.UCS_REMOVED:
                            UCSRemoved();
                            break;
                        case RWrapper.RW_Enum.RW_UCSPARAMETER_TYPE.UCS_SELECTION_CHANGED:
                            UCSSelectionChanged();
                            break;
                        case RWrapper.RW_Enum.RW_UCSPARAMETER_TYPE.UCS_ROTATE_ANGLE:
                            UCSRotateAngle();
                            break;
                        case RWrapper.RW_Enum.RW_UCSPARAMETER_TYPE.CHANGE_DXF_LIST:
                            ChangeDXFShapeList();
                            break;
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_UCSParameter.UCSParamEventHandler(HandleUCSUpdate), UCSEventType);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV60", ex);
            }

        }
        //When new UCS is added
        void UCSAdded()
        {
            try
            {
                UCSList.lstDropDownItems.Items.Add(RWrapper.RW_MainInterface.EntityName);
                UCSHashTable.Add(RWrapper.RW_MainInterface.EntityName, RWrapper.RW_MainInterface.EntityID);
                UCSList.lstDropDownItems.SelectedItem = RWrapper.RW_MainInterface.EntityName;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV61", ex);
            }
        }
        //UCS deletion
        void UCSRemoved()
        {
            try
            {
                if (UCSList.lstDropDownItems.SelectedIndex == 0)
                    return;
                UCSHashTable.Remove(RWrapper.RW_MainInterface.EntityName);
                UCSList.lstDropDownItems.Items.Remove(RWrapper.RW_MainInterface.EntityName);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV62", ex); }
        }
        //Change in UCS selection
        void UCSSelectionChanged()
        {
            RcadToolbar.GetContent().ClearToggBtnSelection();
            RWrapper.RW_MainInterface.MYINSTANCE().HandleRcadToolbar_Click("Zoom-In");
            try
            {
                UCSList.lstDropDownItems.SelectedItem = RWrapper.RW_MainInterface.EntityName;
                //Change the color of the DRO for new UCS added
                if (UCSList.lstDropDownItems.SelectedIndex >= 0)
                {
                    int LastCount = 3;
                    if (UCSList.lstDropDownItems.SelectedIndex <= (UCSColorList.Length / 3) - 1)
                        LastCount = (UCSList.lstDropDownItems.SelectedIndex + 1) * 2 + UCSList.lstDropDownItems.SelectedIndex;
                    else
                    {
                        int SelectedIndex = UCSList.lstDropDownItems.SelectedIndex - ((UCSList.lstDropDownItems.SelectedIndex / (UCSColorList.Length / 3)) * (UCSColorList.Length / 3));
                        LastCount = (SelectedIndex + 1) * 2 + SelectedIndex;
                    }
                    SolidColorBrush TmpBG = new SolidColorBrush(), TmpFG = new SolidColorBrush();
                    TmpBG.Color = Color.FromArgb(255, UCSColorList[LastCount - 2], UCSColorList[LastCount - 1], UCSColorList[LastCount]);
                    TmpFG.Color = Color.FromArgb(255, (byte)(255 - UCSColorList[LastCount - 2]), (byte)(255 - UCSColorList[LastCount - 1]), (byte)(255 - UCSColorList[LastCount]));
                    btnX.SetValue(AttachedDROProperties.DROTxtBGProperty, TmpBG);
                    btnX.SetValue(AttachedDROProperties.DROTxtFGProperty, TmpFG);

                    btnY.SetValue(AttachedDROProperties.DROTxtBGProperty, btnX.GetValue(AttachedDROProperties.DROTxtBGProperty));
                    btnZ.SetValue(AttachedDROProperties.DROTxtBGProperty, btnX.GetValue(AttachedDROProperties.DROTxtBGProperty));
                    btnR.SetValue(AttachedDROProperties.DROTxtBGProperty, btnX.GetValue(AttachedDROProperties.DROTxtBGProperty));
                    //bdrR1.Background = (Brush)btnX.GetValue(AttachedDROText.DROTxtBGProperty);
                    btnY.SetValue(AttachedDROProperties.DROTxtFGProperty, btnX.GetValue(AttachedDROProperties.DROTxtFGProperty));
                    btnZ.SetValue(AttachedDROProperties.DROTxtFGProperty, btnX.GetValue(AttachedDROProperties.DROTxtFGProperty));
                    btnR.SetValue(AttachedDROProperties.DROTxtFGProperty, btnX.GetValue(AttachedDROProperties.DROTxtFGProperty));
                    //txtDroGrade.Foreground = (Brush)btnX.GetValue(AttachedDROText.DROTxtFGProperty);
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV63", ex); }
        }
        //Entering the angle of rotation
        void UCSRotateAngle()
        {
            try
            {
                //Show the window to make user enter the desired value of Angle 
                ShapeRenameWin info = new ShapeRenameWin();
                info.txtUpperHeading.Text = "Enter desired angle of Rotation(Deg)";
                info.ShowDialog();
                if (info.Result == true)
                {
                    double b = 0;
                    double.TryParse(info.txtNewName.Text, out b);
                    RWrapper.RW_MainInterface.MYINSTANCE().SetUCSRotateAngle((Math.PI / 180) * b);
                }
                //info.Close();
                info = null;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV64", ex); }
        }
        //Handling the dxf shapes list change when UCS is changed.
        void ChangeDXFShapeList()
        {
            try
            {
                //Get the instance of the drop down
                ScrToolbar st = null;
                DropDownList dd = null;
                for (int i = 0; i < DXFToolbar.Children.Count; i++)
                {
                    if (DXFToolbar.Children[i].GetType() == typeof(ScrToolbar))
                    {
                        st = (ScrToolbar)DXFToolbar.Children[i];
                        break;
                    }
                }
                for (int i = 0; i < st.Children.Count; i++)
                {
                    if (st.Children[i].GetType() == typeof(DropDownList))
                    {
                        dd = (DropDownList)st.Children[i];
                        break;
                    }
                }

                dd.lstDropDownItems.Items.Clear();
                DXFListHashTable.Clear();
                for (int i = 0; i < RWrapper.RW_MainInterface.EntityIDList.Count; i++)
                {
                    CheckBox chk = new CheckBox();
                    //Adding the entity name to the shape hash table for quick ref.
                    DXFListHashTable.Add(RWrapper.RW_MainInterface.EntityIDList[i], chk);
                    //Set the other UI properties
                    chk.Content = RWrapper.RW_MainInterface.EntityNameList[i];
                    chk.Tag = RWrapper.RW_MainInterface.EntityIDList[i];
                    chk.IsChecked = true;
                    //Handle the Checked/Unchecked events
                    chk.Checked += new RoutedEventHandler(HandleDXFListButton_Selection);
                    chk.Unchecked += new RoutedEventHandler(HandleDXFListButton_Selection);
                    //Show the context menu to be shown to delete the Dxfs.
                    ContextMenu con = new System.Windows.Controls.ContextMenu();
                    MenuItem m = new MenuItem();
                    m.Header = "Delete";
                    m.Tag = chk.Tag;
                    m.Click += new RoutedEventHandler(HandleDeleteDxfClick_Click);
                    con.Items.Add(m);
                    chk.ContextMenu = con;
                    //Add the button to the panel
                    dd.lstDropDownItems.Items.Add(chk);
                }

            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV65", ex); }
        }

        #endregion

        #region Shape Parameters
        //Handling Shapes parameters
        void Handle_CircleDiameter()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    AssignShapePanelToShapeInfoWindow(0, "Enter the Diameter of Circle", "CircleDiameter");
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_CircleDiameter));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV66", ex); }
        }
        void Handle_ParallelLineOffset()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    AssignShapePanelToShapeInfoWindow(2, "Enter the offset;Enter the length", "LineOffset");
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_ParallelLineOffset));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV67", ex); }
        }
        void Handle_PerpediclaurityLand(double value)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    AssignShapePanelToShapeInfoWindow(0, "Enter the Land", "PerpediclaurityLand", false, value.ToString());
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandlerWithDouble(Handle_PerpediclaurityLand));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV68", ex); }
        }
        void Handle_AngularityLand(double value)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    AssignShapePanelToShapeInfoWindow(1, "Deg;Land", "AngularityLand", false, " ;" + value.ToString());
                    ((InfoBox2)shapeWin.bdrContent.Child).stOthers.Visibility = System.Windows.Visibility.Hidden;
                    ((InfoBox2)shapeWin.bdrContent.Child).ChkBx.Visibility = System.Windows.Visibility.Hidden;
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandlerWithDouble(Handle_AngularityLand));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV69", ex); }
        }
        void Handle_ParallelismLand(double value)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    AssignShapePanelToShapeInfoWindow(0, "Enter the Land", "ParallelismLand", false, value.ToString());
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandlerWithDouble(Handle_ParallelismLand));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV70", ex); }
        }
        void Handle_DerivedLineLength()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    AssignShapePanelToShapeInfoWindow(0, "Enter the Length", "DerivedLineLength");
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_DerivedLineLength));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV71", ex); }
        }
        void Handle_EnterTruePosition()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    AssignShapePanelToShapeInfoWindow(1, "X;Y", "TruePosition");
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_EnterTruePosition));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV72", ex); }
        }
        void Handle_EnterTruePositionForPoint()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    AssignShapePanelToShapeInfoWindow(1, "X;Y", "TruePositionForPoint");
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_EnterTruePositionForPoint));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV72a", ex); }
        }
        void Handle_Enter3DTruePosition()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    AssignShapePanelToShapeInfoWindow(6, "X;Y;Radial;Diametric;;", "TruePosition3D");
                    ((InfoBox7)shapeWin.bdrContent.Child).RBtn1.IsChecked = true;
                    ((InfoBox7)shapeWin.bdrContent.Child).RBtn3.Visibility = System.Windows.Visibility.Hidden;
                    ((InfoBox7)shapeWin.bdrContent.Child).ChkValue.Visibility = System.Windows.Visibility.Hidden;
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_Enter3DTruePosition));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV72b", ex); }
        }
        void Handle_Enter3DTruePositionFor3DPoint()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    AssignShapePanelToShapeInfoWindow(6, "X;Y;Radial;Diametric;;", "TruePositionFor3DPoint");
                    ((InfoBox7)shapeWin.bdrContent.Child).RBtn1.IsChecked = true;
                    ((InfoBox7)shapeWin.bdrContent.Child).RBtn3.Visibility = System.Windows.Visibility.Hidden;
                    ((InfoBox7)shapeWin.bdrContent.Child).ChkValue.Visibility = System.Windows.Visibility.Hidden;
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_Enter3DTruePositionFor3DPoint));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV72b", ex); }
        }
        void Handle_OnePtAngleLine()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    AssignShapePanelToShapeInfoWindow(3, "Deg;Min;Length", "OnePtAngleLine");
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_OnePtAngleLine));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV73", ex); }
        }
        void Handle_OnePtOffsetLine()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    AssignShapePanelToShapeInfoWindow(3, "Distance;Offset;Length", "OnePtOffsetLine");
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_OnePtOffsetLine));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV74", ex); }
        }
        void Handle_RelativePointOffset()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    AssignShapePanelToShapeInfoWindow(3, "Offset X;Offset Y;Offset Z", "RelativePointOffset");
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_RelativePointOffset));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV75a", ex); }
        }
        void Handle_ParallelLine3DOffset()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    AssignShapePanelToShapeInfoWindow(2, "Offset; Length", "ParallelLine3DOffset");
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_ParallelLine3DOffset));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV75b", ex); }
        }
        void Handle_ParallelPlaneOffset()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    AssignShapePanelToShapeInfoWindow(0, "Enter Distance from chosen Plane", "ParallelPlaneOffset"); //Offset X;Offset Y;Offset Z
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_ParallelPlaneOffset));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV75b", ex); }
        }
        void Handle_PerpendicularPlaneOffset()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    AssignShapePanelToShapeInfoWindow(3, "Offset X;Offset Y;Offset Z", "PerpendicularPlaneOffset");
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_PerpendicularPlaneOffset));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV75b", ex); }
        }
        void Handle_Translate()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    AssignShapePanelToShapeInfoWindow(3, "Offset X;Offset Y;Offset Z", "ShapeTranslate");
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_Translate));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV75b", ex); }
        }
        void Handle_Rotate()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    AssignShapePanelToShapeInfoWindow(0, "Enter the Angle of Rotation", "ShapeRotate");
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_Rotate));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV75b", ex); }
        }
        void Handle_ShowOuterMostLinePosition()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    FilterLnTypeWin.ShowDialog();
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_ShowOuterMostLinePosition));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV75c", ex); }
        }
        void Handle_ShowDeviationWindow()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    AssignShapePanelToShapeInfoWindow(10, "Upper Cutoff;Tolerance;Deviation Interval;Linear (X);Linear (Y);Geometric;Closed Loop", "GenericDeviationTolerance", true);
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_ShowDeviationWindow));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV75c", ex); }
        }
        void Handle_DoubleClickedOnEntityRcad(int i)
        {
            try
            {
                if (i == 1)
                {
                    string[] SeletedStr = { "RightClickOnMeasure", "Rename Measurement" };
                    RWrapper.RW_MainInterface.MYINSTANCE().HandleRightClick_OnEntities(SeletedStr[0], SeletedStr[1]);
                    MeasurementsPopup.RenameMeasurement();
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV75DblclickRcad", ex); }
        }
        void Handle_TruePositionEvent(double x, double y)
        {
            try
            {
                if (shapeWin != null)
                {
                    if (shapeWin.bdrContent.Child.GetType() == typeof(InfoBox2))
                    {
                        InfoBox2 Inf2 = (InfoBox2)shapeWin.bdrContent.Child;
                        Inf2.txtValue.Text = x.ToString();
                        Inf2.txtValue2.Text = y.ToString();
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0582", ex);
                //throw;
            }
        }

        void Handle_ShowCloudPointMeasureWindow()
        {
            SetOwnerofWindow(CloudPointMeasurement.Myinstance());
            CloudPointMeasurement.Myinstance().Show();
        }
        //this function is called to hide the ShapeEntity value window when the drawing of shape is completed
        void Handle_HideEntityPanel()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    shapeWin.Hide();
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_HideEntityPanel));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV75", ex); }
        }

        void Handle_FixtureAlign_RefPt_Taken()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    //AssignShapePanelToShapeInfoWindow(1, "X;Y", "TruePosition");
                    if (FixtureCalibWin != null)
                    {
                        FixtureCalibWin.CalibProgressBdr2.Visibility = System.Windows.Visibility.Visible;
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_FixtureAlign_RefPt_Taken));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV72", ex); }
        }


        //handling the closing of the Shape window for entering values
        void shapeWin_WinClosing(object sender, RoutedEventArgs e)
        {
            try
            {
                shapeWin.Left = RcadLeft;
                shapeWin.Top = RcadTop;
                if (!shapeWin.Result) { RWrapper.RW_MainInterface.MYINSTANCE().CancelPressed(); return; }
                if (shapeWin.bdrContent.Child.GetType() == typeof(InfoBox))
                {
                    InfoBox info = (InfoBox)shapeWin.bdrContent.Child;
                    double dvalue = 0;
                    //if (info.txtValue.Text.Length == 0) { return; }
                    if (info.txtValue.Text.Length > 0)
                        dvalue = Convert.ToDouble(info.txtValue.Text);
                    else dvalue = 0;
                    switch (info.Name)
                    {
                        case "CircleDiameter":
                            //RWrapper.RW_MainInterface.MYINSTANCE().SetParallelineOffset(dvalue);
                            RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(dvalue, 0, 0);
                            break;
                        case "PerpediclaurityLand":
                            //RWrapper.RW_MainInterface.MYINSTANCE().SetLandAndAngle(Math.PI / 2, dvalue);
                            RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(Math.PI / 2, dvalue, 0);
                            break;
                        case "ParallelismLand":
                            //RWrapper.RW_MainInterface.MYINSTANCE().SetLandAndAngle(0, dvalue);
                            RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(0, dvalue, 0);
                            break;
                        case "DerivedLineLength":
                            RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(dvalue, 0, 0);
                            break;
                        case "SplineCount":
                            RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters((int)dvalue, 0, 0);
                            break;
                        case "CylinderDiameter":
                            RWrapper.RW_VBlockCallibration.MYINSTANCE().Create_AxisLine(dvalue);
                            break;
                        case "ShapeRotate":
                            RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(dvalue, 0, 0);
                            break;
                        case "SetCloudPtColourInterval":
                            RWrapper.RW_MainInterface.MYINSTANCE().SetColorThresholdValue(dvalue);
                            break;
                        case "ParallelPlaneOffset":
                            RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(0, dvalue, 0);
                            break;
                    }
                }
                else if (shapeWin.bdrContent.Child.GetType() == typeof(InfoBox2))
                {
                    InfoBox2 info = (InfoBox2)shapeWin.bdrContent.Child;
                    double dvalue1 = 0, dvalue2 = 0;
                    //if (info.txtValue.Text.Length == 0 || info.txtValue2.Text.Length == 0) { return; }
                    if (info.txtValue.Text.Length > 0)
                        dvalue1 = Convert.ToDouble(info.txtValue.Text);
                    else dvalue1 = 0;
                    if (info.txtValue2.Text.Length > 0)
                        dvalue2 = Convert.ToDouble(info.txtValue2.Text);
                    else dvalue2 = 0;
                    switch (info.Name)
                    {
                        case "AngularityLand":
                            RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(dvalue1, dvalue2, 0);
                            break;
                        case "TruePosition":
                            RWrapper.RW_MainInterface.MYINSTANCE().SetTruePos(dvalue1, dvalue2, info.AxisStatus, (bool)info.ChkBx.IsChecked, false);
                            break;
                        case "TruePositionForPoint":
                            RWrapper.RW_MainInterface.MYINSTANCE().SetTruePos(dvalue1, dvalue2, info.AxisStatus, (bool)info.ChkBx.IsChecked, true);
                            break;
                    }
                }
                else if (shapeWin.bdrContent.Child.GetType() == typeof(InfoBox3))
                {
                    InfoBox3 info = (InfoBox3)shapeWin.bdrContent.Child;
                    double dvalue1 = 0, dvalue2 = 0;
                    //if (info.txtValue.Text.Length == 0) { return; }
                    if (info.txtValue.Text.Length > 0)
                        dvalue1 = Convert.ToDouble(info.txtValue.Text);
                    else dvalue1 = 0;
                    if (info.txtValue2.Text.Length > 0)
                        dvalue2 = Convert.ToDouble(info.txtValue2.Text);
                    else dvalue2 = 0;

                    switch (info.Name)
                    {
                        case "LineOffset":
                            //RWrapper.RW_MainInterface.MYINSTANCE().SetParallelineOffset(dvalue);
                            RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(dvalue1, dvalue2, 0);
                            break;
                        case "PPParam_Deviation":
                            RWrapper.RW_PartProgram.MYINSTANCE().SetToleranceValueForDeviation(dvalue1, dvalue2);
                            break;
                        case "ParallelLine3DOffset":
                            RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(0, dvalue1, dvalue2);
                            break;
                    }
                }
                else if (shapeWin.bdrContent.Child.GetType() == typeof(InfoBox4))
                {
                    InfoBox4 info = (InfoBox4)shapeWin.bdrContent.Child;
                    double dvalue1 = 0, dvalue2 = 0, dvalue3 = 0, dans = 0;
                    //if (info.txtValue.Text.Length == 0 || info.txtValue2.Text.Length == 0) { return; }
                    if (info.txtValue.Text.Length > 0)
                        dvalue1 = Convert.ToDouble(info.txtValue.Text);
                    else dvalue1 = 0;
                    if (info.txtValue2.Text.Length > 0)
                        dvalue2 = Convert.ToDouble(info.txtValue2.Text);
                    else dvalue2 = 0;
                    if (info.txtValue3.Text.Length > 0)
                        dvalue3 = Convert.ToDouble(info.txtValue3.Text);
                    else dvalue3 = 0;
                    switch (info.Name)
                    {
                        case "OnePtAngleLine":
                            if (dvalue1 < 0)
                                dans = (dvalue1 - dvalue2 / 60) * Math.PI / 180;
                            else
                                dans = (dvalue1 + dvalue2 / 60) * Math.PI / 180;
                            //RWrapper.RW_MainInterface.MYINSTANCE().SetOnePointAngleLine(dans);
                            RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(dans, dvalue3, 0);
                            break;
                        case "OnePtOffsetLine":
                            //RWrapper.RW_MainInterface.MYINSTANCE().SetOnePointOffsetLine(dvalue1, dvalue2);
                            RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(dvalue1, dvalue2, dvalue3);
                            break;
                        case "ArcDiviation":
                            RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(dvalue1, dvalue2, dvalue3);
                            break;
                        case "RelativePointOffset":
                            RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(dvalue1, dvalue2, dvalue3);
                            break;
                        case "PPParam_PTOffset":
                            RWrapper.RW_PartProgram.MYINSTANCE().SetOffsetForRelativePoint(dvalue1, dvalue2, dvalue3);
                            break;
                        case "ParallelLine3DOffset":
                            RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(dvalue1, dvalue2, dvalue3);
                            break;
                        case "ParallelPlaneOffset":
                            RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(dvalue1, dvalue2, dvalue3);
                            break;
                        case "PerpendicularPlaneOffset":
                            RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(dvalue1, dvalue2, dvalue3);
                            break;
                        case "ShapeTranslate":
                            RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(dvalue1, dvalue2, dvalue3);
                            break;
                    }
                }
                else if (shapeWin.bdrContent.Child.GetType() == typeof(InfoBox5))
                {
                    InfoBox5 info = (InfoBox5)shapeWin.bdrContent.Child;
                    double dvalue1 = 0, dvalue2 = 0, dvalue3 = 0;
                    bool ChkStatus = false, CheckStatus2 = false, RBtnStatus2 = false;
                    int RBtnStatus = 2, RBtnStatus3;

                    if (info.txtValue.Text.Length > 0)
                        dvalue1 = Convert.ToDouble(info.txtValue.Text);
                    else dvalue1 = 0;
                    if (info.txtValue2.Text.Length > 0)
                        dvalue2 = Convert.ToDouble(info.txtValue2.Text);
                    else dvalue2 = 0;
                    if (info.txtValue3.Text.Length > 0)
                        dvalue3 = Convert.ToDouble(info.txtValue3.Text);
                    else dvalue3 = 0;
                    ChkStatus = (bool)info.ChkBx.IsChecked;
                    CheckStatus2 = (bool)info.ChkBx2.IsChecked;
                    RBtnStatus = ((bool)info.RBtn1.IsChecked ? 0 : (bool)info.RBtn2.IsChecked ? 1 : 2);
                    RBtnStatus2 = (bool)info.RBtn5.IsChecked;
                    RBtnStatus3 = ((bool)info.RBtn6.IsChecked ? 0 : 1);
                    switch (info.Name)
                    {
                        case "SheshaDeviationTolerance":
                            RWrapper.RW_MainInterface.MYINSTANCE().CloudPointDeviation_DxfShape(dvalue1, dvalue2, dvalue3, ChkStatus, RBtnStatus, CheckStatus2, RBtnStatus3, RBtnStatus2);
                            break;
                    }
                }
                else if (shapeWin.bdrContent.Child.GetType() == typeof(InfoBox6))
                {
                    InfoBox6 info = (InfoBox6)shapeWin.bdrContent.Child;
                    double dvalue1 = 0, dvalue2 = 0, dvalue3 = 0, dvalue4 = 0, dvalue5 = 0;
                    if (info.txtValue.Text.Length > 0)
                        dvalue1 = Convert.ToDouble(info.txtValue.Text);
                    else dvalue1 = 0;
                    if (info.txtValue2.Text.Length > 0)
                        dvalue2 = Convert.ToDouble(info.txtValue2.Text);
                    else dvalue2 = 0;
                    if (info.txtValue3.Text.Length > 0)
                        dvalue3 = Convert.ToDouble(info.txtValue3.Text);
                    else dvalue3 = 0;
                    if (info.txtValue4.Text.Length > 0)
                        dvalue4 = Convert.ToDouble(info.txtValue4.Text);
                    else dvalue4 = 0;
                    if (info.txtValue5.Text.Length > 0)
                        dvalue5 = Convert.ToDouble(info.txtValue5.Text);
                    else dvalue5 = 0;
                    switch (info.Name)
                    {
                        case "PPParam_Deviation&PTOffset":
                            RWrapper.RW_PartProgram.MYINSTANCE().SetToleranceValueForDeviation(dvalue1, dvalue2);
                            RWrapper.RW_PartProgram.MYINSTANCE().SetOffsetForRelativePoint(dvalue3, dvalue4, dvalue5);
                            break;
                    }
                }
                else if (shapeWin.bdrContent.Child.GetType() == typeof(InfoBox7))
                {
                    InfoBox7 info = (InfoBox7)shapeWin.bdrContent.Child;
                    double dvalue = 0, dvalue2 = 0;
                    bool ChkStatus = false;
                    int RBtnStatus = 2;

                    if (info.txtValue.Text.Length > 0)
                        dvalue = Convert.ToDouble(info.txtValue.Text);
                    else dvalue = 0;
                    if (info.txtValue2.Text.Length > 0)
                        dvalue2 = Convert.ToDouble(info.txtValue2.Text);
                    else dvalue2 = 0;
                    RBtnStatus = ((bool)info.RBtn1.IsChecked ? 0 : (bool)info.RBtn2.IsChecked ? 1 : 2);
                    ChkStatus = (bool)info.ChkValue.IsChecked;
                    switch (info.Name)
                    {
                        case "TruePosition3D":
                            RWrapper.RW_MainInterface.MYINSTANCE().Set3DTruePos(dvalue, dvalue2, RBtnStatus, false);
                            break;
                        case "TruePositionFor3DPoint":
                            RWrapper.RW_MainInterface.MYINSTANCE().Set3DTruePos(dvalue, dvalue2, RBtnStatus, true);
                            break;
                    }
                }
                else if (shapeWin.bdrContent.Child.GetType() == typeof(InfoBox8))
                {
                    InfoBox8 info = (InfoBox8)shapeWin.bdrContent.Child;
                    double dvalue = 0;
                    int PlnType = 0;

                    if (info.txtValue.Text.Length > 0)
                        dvalue = Convert.ToDouble(info.txtValue.Text);
                    else dvalue = 0;
                    PlnType = (bool)info.XYPlaneRBtn.IsChecked ? 2 : (bool)info.YZPlaneRBtn.IsChecked ? 0 : 1;
                    switch (info.Name)
                    {
                        case "TriangulatedSurfaceLayering":
                            RWrapper.RW_MainInterface.MYINSTANCE().CreateIntersectionPtsWithTriangle(dvalue, PlnType);
                            break;
                    }
                }
                else if (shapeWin.bdrContent.Child.GetType() == typeof(InfoBox9))
                {
                    InfoBox9 info = (InfoBox9)shapeWin.bdrContent.Child;
                    bool ChkStatus = false, ChkStatus2 = false, ChkStatus3 = false, ChkStatus4 = false;
                    ChkStatus = (bool)info.ChkBx.IsChecked;
                    ChkStatus2 = (bool)info.ChkBx2.IsChecked;
                    ChkStatus3 = (bool)info.ChkBx3.IsChecked;
                    ChkStatus4 = (bool)info.ChkBx4.IsChecked;
                    switch (info.Name)
                    {
                        case "ShapeForOffsetCorrection":
                            RWrapper.RW_MainInterface.MYINSTANCE().SetOffsetCorrectionflag(ChkStatus, ChkStatus2, ChkStatus3, ChkStatus4);
                            break;
                        case "CalibWinGoHomeAxisSelection":
                            RWrapper.RW_AutoCalibration.MYINSTANCE().Handle_HomeClicked(ChkStatus, ChkStatus2, ChkStatus3);
                            break;
                    }
                }
                else if (shapeWin.bdrContent.Child.GetType() == typeof(InfoBox10))
                {
                    InfoBox10 info = (InfoBox10)shapeWin.bdrContent.Child;
                    bool ChkStatus = false;
                    ChkStatus = (bool)info.RBtn.IsChecked;
                    switch (info.Name)
                    {
                        case "AreaShape":
                            RWrapper.RW_MainInterface.MYINSTANCE().CreateAreaShape(ChkStatus);
                            break;
                    }
                }
                else if (shapeWin.bdrContent.Child.GetType() == typeof(InfoBox11))
                {
                    InfoBox11 info = (InfoBox11)shapeWin.bdrContent.Child;
                    double dvalue = 0, dvalue2 = 0, dvalue3 = 0;
                    bool ChkStatus = false;
                    int RBtnStatus = 2;

                    if (info.txtValue.Text.Length > 0)
                        dvalue = Convert.ToDouble(info.txtValue.Text);
                    else dvalue = 0;
                    if (info.txtValue2.Text.Length > 0)
                        dvalue2 = Convert.ToDouble(info.txtValue2.Text);
                    else dvalue2 = 0;
                    if (info.txtValue3.Text.Length > 0)
                        dvalue3 = Convert.ToDouble(info.txtValue3.Text);
                    else dvalue3 = 0;
                    RBtnStatus = ((bool)info.RBtn1.IsChecked ? 0 : (bool)info.RBtn2.IsChecked ? 1 : 2);
                    ChkStatus = (bool)info.ChkValue.IsChecked;
                    switch (info.Name)
                    {
                        case "GenericDeviationTolerance":
                            RWrapper.RW_MainInterface.MYINSTANCE().CloudPointDeviation_DxfShape(dvalue, dvalue2, dvalue3, RBtnStatus, ChkStatus);
                            break;
                    }
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV76", ex); }
        }
        //This function assign the content to window which has to be shown during
        //process of drawing shpews based on which shape & which criteria is choosen by user.
        public void AssignShapePanelToShapeInfoWindow(int InfoBoxNo, string status, string name, bool DialogShow = false, string TextValue = "")
        {
            try
            {
                UIElement info;
                if (InfoBoxNo == 0)
                {
                    info = new InfoBox();
                    ((InfoBox)info).txtStatus.Text = status;
                    ((InfoBox)info).Name = name;
                    ((InfoBox)info).txtValue.Text = TextValue;
                }
                else if (InfoBoxNo == 1)
                {
                    TextValue = ";";
                    info = new InfoBox2();
                    ((InfoBox2)info).txtStatus.Text = status.Split(';')[0];
                    ((InfoBox2)info).txtStatus2.Text = status.Split(';')[1];
                    ((InfoBox2)info).Name = name;
                    ((InfoBox2)info).txtValue.Text = TextValue.Split(';')[0];
                    ((InfoBox2)info).txtValue2.Text = TextValue.Split(';')[1];
                }
                else if (InfoBoxNo == 2)
                {
                    info = new InfoBox3();
                    ((InfoBox3)info).txtStatus.Text = status.Split(';')[0];
                    ((InfoBox3)info).txtStatus2.Text = status.Split(';')[1];
                    ((InfoBox3)info).Name = name;
                }
                else if (InfoBoxNo == 3)
                {
                    info = new InfoBox4();
                    ((InfoBox4)info).txtStatus.Text = status.Split(';')[0];
                    ((InfoBox4)info).txtStatus2.Text = status.Split(';')[1];
                    ((InfoBox4)info).txtStatus3.Text = status.Split(';')[2];
                    ((InfoBox4)info).Name = name;
                }
                else if (InfoBoxNo == 4)
                {
                    info = new InfoBox5();
                    ((InfoBox5)info).txtStatus.Text = status.Split(';')[0];
                    ((InfoBox5)info).txtStatus2.Text = status.Split(';')[1];
                    ((InfoBox5)info).txtStatus3.Text = status.Split(';')[2];
                    ((InfoBox5)info).ChkBx.Content = status.Split(';')[3];
                    ((InfoBox5)info).RBtn1.Content = status.Split(';')[4];
                    ((InfoBox5)info).RBtn2.Content = status.Split(';')[5];
                    ((InfoBox5)info).RBtn3.Content = status.Split(';')[6];
                    ((InfoBox5)info).ChkBx2.Content = status.Split(';')[7];
                    ((InfoBox5)info).txtStatus4.Text = status.Split(';')[8];
                    ((InfoBox5)info).RBtn4.Content = status.Split(';')[9];
                    ((InfoBox5)info).RBtn5.Content = status.Split(';')[10];
                    ((InfoBox5)info).txtStatus5.Text = status.Split(';')[11];
                    ((InfoBox5)info).RBtn6.Content = status.Split(';')[12];
                    ((InfoBox5)info).RBtn7.Content = status.Split(';')[13];
                    ((InfoBox5)info).RBtn1.IsChecked = true;
                    ((InfoBox5)info).RBtn5.IsChecked = true;
                    ((InfoBox5)info).RBtn6.IsChecked = true;
                    ((InfoBox5)info).ChkBx2.IsChecked = true;
                    ((InfoBox5)info).Name = name;
                }
                else if (InfoBoxNo == 5)
                {
                    info = new InfoBox6();
                    ((InfoBox6)info).txtStatus.Text = status.Split(';')[0];
                    ((InfoBox6)info).txtStatus2.Text = status.Split(';')[1];
                    ((InfoBox6)info).txtStatus3.Text = status.Split(';')[2];
                    ((InfoBox6)info).txtStatus4.Text = status.Split(';')[3];
                    ((InfoBox6)info).txtStatus5.Text = status.Split(';')[4];
                    ((InfoBox6)info).Name = name;
                }
                else if (InfoBoxNo == 6)
                {
                    info = new InfoBox7();
                    ((InfoBox7)info).txtStatus.Text = status.Split(';')[0];
                    ((InfoBox7)info).txtStatus2.Text = status.Split(';')[1];
                    ((InfoBox7)info).RBtn1.Content = status.Split(';')[2];
                    ((InfoBox7)info).RBtn2.Content = status.Split(';')[3];
                    ((InfoBox7)info).RBtn3.Content = status.Split(';')[4];
                    ((InfoBox7)info).ChkValue.Content = status.Split(';')[5];
                    ((InfoBox7)info).Name = name;
                }
                else if (InfoBoxNo == 7)
                {
                    info = new InfoBox8();
                    ((InfoBox8)info).Name = name;
                }
                else if (InfoBoxNo == 8)
                {
                    info = new InfoBox9();
                    ((InfoBox9)info).Name = name;
                    ((InfoBox9)info).txtStatus.Text = status.Split(';')[0];
                    ((InfoBox9)info).ChkBx.Content = status.Split(';')[1];
                    ((InfoBox9)info).ChkBx2.Content = status.Split(';')[2];
                    ((InfoBox9)info).ChkBx3.Content = status.Split(';')[3];
                    if (GlobalSettings.NumberOfAxes == 4)
                    {
                        ((InfoBox9)info).ChkBx4.Visibility = System.Windows.Visibility.Visible;
                        ((InfoBox9)info).ChkBx4.Content = status.Split(';')[4];
                    }
                }
                else if (InfoBoxNo == 9)
                {
                    info = new InfoBox10();
                    ((InfoBox10)info).Name = name;
                    ((InfoBox10)info).txtStatus.Text = status.Split(';')[0];
                    ((InfoBox10)info).RBtn.Content = status.Split(';')[1];
                    ((InfoBox10)info).RBtn2.Content = status.Split(';')[2];
                }
                else
                {
                    info = new InfoBox11();
                    ((InfoBox11)info).txtStatus.Text = status.Split(';')[0];
                    ((InfoBox11)info).txtStatus2.Text = status.Split(';')[1];
                    ((InfoBox11)info).txtStatus3.Text = status.Split(';')[2];
                    ((InfoBox11)info).RBtn1.Content = status.Split(';')[3];
                    ((InfoBox11)info).RBtn2.Content = status.Split(';')[4];
                    ((InfoBox11)info).RBtn3.Content = status.Split(';')[5];
                    ((InfoBox11)info).ChkValue.Content = status.Split(';')[6];
                    ((InfoBox11)info).Name = name;
                }
                shapeWin.bdrContent.Child = info;
                if (DialogShow)
                {
                    //shapeWin.Left = double.NaN;
                    //shapeWin.Top = double.NaN;
                    //shapeWin.WindowStartupLocation = System.Windows.WindowStartupLocation.CenterScreen;
                    shapeWin.ShowDialog();
                }
                else
                {
                    //shapeWin.WindowStartupLocation = System.Windows.WindowStartupLocation.Manual;
                    //shapeWin.Left = RcadLeft;
                    //shapeWin.Top = RcadTop;
                    shapeWin.Show();
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV77", ex); }
        }
        #endregion

        #region Part Program
        //Checking the default directory of programs & getting the max count of the Programs made.

        delegate void Del_DataTable(System.Data.DataTable table);
        void CheckPartprogramDir_GetMaxCountOfProgramsMade()
        {
            try
            {
                if (!Directory.Exists(GlobalSettings.ProgramPath))
                    Directory.CreateDirectory(GlobalSettings.ProgramPath);
                //Check for the Report path
                if (!Directory.Exists(GlobalSettings.ReportPath))
                    Directory.CreateDirectory(GlobalSettings.ReportPath);
                //Check for the Template path
                if (!Directory.Exists(GlobalSettings.TemplatePath))
                    Directory.CreateDirectory(GlobalSettings.TemplatePath);
                //Check for the shape points csv path
                if (!Directory.Exists(GlobalSettings.ShapePointsPath))
                    Directory.CreateDirectory(GlobalSettings.ShapePointsPath);
                //Now get the max count of programs.
                string[] _ProgramNames = Directory.GetFileSystemEntries(GlobalSettings.ProgramPath, "*.rppx");
                int prev = 0;
                for (int i = 0; i < _ProgramNames.Length; i++)
                {
                    if (_ProgramNames[i].Contains("Component"))
                    {
                        string _prog = (_ProgramNames[i].Substring(_ProgramNames[i].LastIndexOf("t", StringComparison.InvariantCulture) + 1)).Split('.')[0];
                        if (int.TryParse(_prog, out prev))
                        {
                            if (prev > MaxCountOfProgram)
                                MaxCountOfProgram = prev;
                        }
                    }
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV78", ex); }
        }
        //It loads all the program names in the part program panel
        void LoadProgramReportNames(bool flag = true, string Filter = "")
        {
            GlobalSettings.LoadExcelInstance.Reset_All();
            GlobalSettings.PartProgramPaths.Clear();
            System.Data.DataTable FullProgNames = GlobalSettings.LoadExcelInstance.GetAllProgramNames("Program_Id ASC").ToTable();
            System.Threading.Thread t1 = new System.Threading.Thread(()=> this.LoadProgramNamesInSeparateThread(Filter, flag, FullProgNames));
            t1.Start();
        }

        void ResetAll_LoadForm()
        {
            GlobalSettings.LoadExcelInstance.Reset_All();
        }

        private void LoadProgramNamesInSeparateThread(string Filter1, bool flag1, DataTable Full_ProgramNames)
        {
            try
            {
                string[] programCols = { "Name", "User", "Program_Id" };
                System.Data.DataTable Modified_ProgramNames = new System.Data.DataTable();
                for (int i = 0; i < programCols.Length - 1; i++)
                    Modified_ProgramNames.Columns.Add(programCols[i], typeof(string));
                Modified_ProgramNames.Columns.Add(programCols[programCols.Length - 1], typeof(int));

                for (int i = 0; i < Full_ProgramNames.Rows.Count; i++)
                {
                    string programName = Full_ProgramNames.Rows[i][1].ToString();
                    string path = programName.Substring(0, programName.LastIndexOf("\\") + 1);
                    string OnlyProgramName = programName.Substring(programName.LastIndexOf("\\") + 1);
                    if (Filter1 != "")
                    {
                        if (OnlyProgramName.ToUpper().ToString().Contains(Filter1.ToUpper()))
                        {
                            GlobalSettings.PartProgramPaths.Add(path);
                            Modified_ProgramNames.Rows.Add(new object[] { OnlyProgramName, Full_ProgramNames.Rows[i][2], Full_ProgramNames.Rows[i]["Program_Id"] });
                        }
                    }
                    else
                    {
                        GlobalSettings.PartProgramPaths.Add(path);
                        Modified_ProgramNames.Rows.Add(new object[] { OnlyProgramName, Full_ProgramNames.Rows[i][2], Full_ProgramNames.Rows[i]["Program_Id"] });
                    }
                }
                this.Dispatcher.Invoke(new Del_DataTable(FinaliseProgramListing), Modified_ProgramNames);
                //System.Threading.Thread t1 = new System.Threading.Thread(new ThreadStart(UpdateAllReportNames));
                //t1.Start();
                UpdateAllReportNames();
                if (flag1)
                    this.Dispatcher.BeginInvoke(new AsyncSimpleDelegate(UpdatePartProgramPopUp), System.Windows.Threading.DispatcherPriority.Normal);

            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV79", ex); }

        }

        void FinaliseProgramListing(System.Data.DataTable Modified_ProgramNames)
        {
            PartProgramPopUp.dgvPrograms.ItemsSource = null;
            PartProgramPopUp.dgvPrograms.AutoGenerateColumns = true;
            PartProgramPopUp.dgvPrograms.ItemsSource = Modified_ProgramNames.DefaultView;
            PartProgramPopUp.dgvPrograms.Columns[2].Visibility = Visibility.Hidden;
            if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.OnlineSystem)
            {
                OnlineControlWindow.cbComponentsList.ItemsSource = Modified_ProgramNames.DefaultView;
                OnlineControlWindow.cbComponentsList.DisplayMemberPath = "Name";
                OnlineControlWindow.cbComponentsList.SelectionChanged += new SelectionChangedEventHandler(dgvPrograms_SelectionChanged);
            }
        }

        void UpdatePartProgramPopUp()
        {
            try
            {
                PartProgramPopUp.dgvPrograms.MouseDoubleClick += new MouseButtonEventHandler(dgvPrograms_MouseDoubleClick);
                //PartProgramPopUp.dgvPrograms.SelectionChanged += new SelectionChangedEventHandler(dgvPrograms_SelectionChanged);
                //PartProgramPopUp.dgvPrograms.SelectionChanged += new SelectionChangedEventHandler(dgvPrograms_SelectionChanged);
                PartProgramPopUp.dgvTitles.CellEditEnding += new EventHandler<DataGridCellEditEndingEventArgs>(dgvTitles_CellEditEnding);
                PartProgramPopUp.lstReportStyles.SelectionChanged += new SelectionChangedEventHandler(lstReportStyles_SelectionChanged);
                PartProgramPopUp.lstReports.SelectionChanged += new SelectionChangedEventHandler(lstReports_SelectionChanged);
                PartProgramPopUp.SearchTxtbx.TextChanged += new TextChangedEventHandler(SearchTxtbx_KeyDown);
                // PartProgramPopUp.SearchTxtbx.KeyDown+=new KeyEventHandler(SearchTxtbx_KeyDown);
                PartProgramPopUp.dgvPrograms.UpdateLayout();
                PartProgramPopUp.dgvPrograms.ScrollIntoView(PartProgramPopUp.dgvPrograms.Items[PartProgramPopUp.dgvPrograms.Items.Count - 1], PartProgramPopUp.dgvPrograms.Columns[0]);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV80a", ex);
            }
        }
        //This function is called whenever an measurement(full run of program) is finished.
        void Handle_MeasurementFinished()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    PartProgramPopUp.dgvPrograms.IsEnabled = true;
                    if (TxtGridRunCount.Text == "") EnableDisableControlsDuringProgramRun(true);
                    ProgramRunning = false;
                    //GlobalSettings.MainExcelInstance.MeasurementList = RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementList();
                    if (PartProgramPopUp.SelectedProgIndex == -1)
                        return;
                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.OnlineSystem)
                    {
                        OnlineControlWindow.TotalNumberDone++;
                        int State = MeasurementsPopup.SetPassFail();
                        if (State == 1)
                            OnlineControlWindow.PassNumber++;
                        else
                            OnlineControlWindow.FailNumber++;

                        //Change Status of current component according to measurement report.
                        if (GlobalSettings.OnlineSystemStates.Count > 0)
                            GlobalSettings.OnlineSystemStates[GlobalSettings.OnlineSystemStates.Count - 1] = (GlobalSettings.OnlineRunningState)State;

                        if (ReadyToRunNextOnlineComponent)
                        {
                            //We have reached target and next component is ready for analysis. So let us run the Part Program on that and add an entry for that
                            //in our list. (We will remove when we eject the component into appropriate bin. 
                            GlobalSettings.OnlineSystemStates.Add(GlobalSettings.OnlineRunningState.InProcess);
                            Thread tt1 = new Thread(new ThreadStart(RWrapper.RW_PartProgram.MYINSTANCE().PartProgram_Run));
                            ReadyToRunNextOnlineComponent = false;
                        }
                        else if (ReadyToMovetoNextOnlineSlot)
                        {
                            ReadyToMovetoNextOnlineSlot = false;
                            //We have reached the target. We check for the status of the 3 slots.....
                            bool PassPrev = false;
                            bool RejectPrevToPrev = false;
                            //Always consider that there are 3 drills in the table.... 
                            //if (GlobalSettings.OnlineSystemStates.cou)
                            //if (GlobalSettings.OnlineSystemStates.Count > 2 && GlobalSettings.OnlineSystemStates[1] == GlobalSettings.OnlineRunningState.Pass) PassPrev = true;
                            //if (GlobalSettings.OnlineSystemStates.Count > 2 && GlobalSettings.OnlineSystemStates[2] == GlobalSettings.OnlineRunningState.Fail) RejectPrevToPrev = true;
                            RejectPrevToPrev = true;
                            if (GlobalSettings.OnlineSystemStates[GlobalSettings.OnlineSystemStates.Count - 1] == GlobalSettings.OnlineRunningState.Pass)
                                PassPrev = true;
                            RWrapper.RW_CNC.MYINSTANCE().MoveTable(PassPrev, RejectPrevToPrev, OnlineControlWindow.Speed);

                            if (RejectPrevToPrev) GlobalSettings.OnlineSystemStates.RemoveAt(0);
                            if (PassPrev) GlobalSettings.OnlineSystemStates.RemoveAt(0);
                        }
                    }
                    else
                    {
                        GlobalSettings.MainExcelInstance.ToleranceFormat = GlobalSettings.XLDataTransferTolMode;
                        GlobalSettings.MainExcelInstance.MeasurementStringList = RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementListWithNT();
                        GlobalSettings.MainExcelInstance.ProgramName = GlobalSettings.PartProgramPaths[PartProgramPopUp.SelectedProgIndex] + ((System.Data.DataRowView)PartProgramPopUp.dgvPrograms.Items[PartProgramPopUp.SelectedProgIndex]).Row[0].ToString();
                        GlobalSettings.MainExcelInstance.ReportName = PartProgramPopUp.lstReports.SelectedItem.ToString();
                        GlobalSettings.MainExcelInstance.TemplateFolderName = GlobalSettings.TemplatePath;
                        string PicFile1 = System.IO.Path.GetTempPath() + "RRMExportPic1.jpg";
                        string PicFile2 = System.IO.Path.GetTempPath() + "RRMExportPic2.jpg";
                        if (GlobalSettings.XLExportImg1DuringPP == 1)
                        {
                            RWrapper.RW_MainInterface.MYINSTANCE().ClearShapeAndMeasure_Selections();
                            System.Threading.Thread.Sleep(50);
                            RWrapper.RW_MainInterface.MYINSTANCE().SaveRcadWindowImage(PicFile1, false);
                        }
                        else if (GlobalSettings.XLExportImg1DuringPP == 2)
                        {
                            RWrapper.RW_MainInterface.MYINSTANCE().SaveVideoWindowImage(PicFile1, false, false);
                        }
                        else if (GlobalSettings.XLExportImg1DuringPP == 3)
                        {
                            RWrapper.RW_MainInterface.MYINSTANCE().SaveVideoWindowImage(PicFile1, true, false);
                        }

                        //if (GlobalSettings.XLExportImg2DuringPP == 1)
                        //{
                        //    RWrapper.RW_MainInterface.MYINSTANCE().ClearShapeAndMeasure_Selections();
                        //    System.Threading.Thread.Sleep(50);
                        //    RWrapper.RW_MainInterface.MYINSTANCE().SaveRcadWindowImage(PicFile2, false);
                        //}
                        //else 
                        if (GlobalSettings.XLExportImg2DuringPP == 1)
                        {
                            RWrapper.RW_MainInterface.MYINSTANCE().SaveVideoWindowImage(PicFile2, false, false);
                        }
                        else if (GlobalSettings.XLExportImg2DuringPP == 2)
                        {
                            RWrapper.RW_MainInterface.MYINSTANCE().SaveVideoWindowImage(PicFile2, true, false);
                        }
                        GlobalSettings.MainExcelInstance.PicturePath = PicFile1;
                        GlobalSettings.MainExcelInstance.Picture2Path = PicFile2;
                        GlobalSettings.MainExcelInstance.Reset_All(true);

                        if (GlobalSettings.XLExportImg1DuringPP > 0)
                        {
                            if (GlobalSettings.XLExportImg2DuringPP > 0)
                            {
                                GlobalSettings.MainExcelInstance.InsertDoublePicture = true;
                                string[] pf = new string[2];
                                pf[0] = PicFile1; pf[1] = PicFile2;
                                GlobalSettings.MainExcelInstance.ImagePaths = pf;
                            }
                            else
                                GlobalSettings.MainExcelInstance.InsertImage = true;
                        }

                        while (GlobalSettings.MainExcelInstance.IsOpening)
                            continue;

                        int State = MeasurementsPopup.SetPassFail();
                        switch (State)
                        {
                            case 0:
                                GlobalSettings.MainExcelInstance.passStatus = rrm.Form1.PassFail.none;
                                break;
                            case 1:
                                GlobalSettings.MainExcelInstance.passStatus = rrm.Form1.PassFail.pass;
                                break;
                            case 2:
                                GlobalSettings.MainExcelInstance.passStatus = rrm.Form1.PassFail.fail;
                                break;
                        }

                        GlobalSettings.MainExcelInstance.GenerateExcel(true);
                        //GlobalSettings.MainExcelInstance.Update_Database();
                        if (GlobalSettings.XLExportImg1DuringPP != 0)
                            System.IO.File.Delete(PicFile1);
                        if (GlobalSettings.XLExportImg2DuringPP != 0)
                            System.IO.File.Delete(PicFile2);

                        string TmpImgFolderPath = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Image";
                        string[] _ProgramImgNames = Directory.GetFileSystemEntries(TmpImgFolderPath, "*.jpg");
                        if (_ProgramImgNames.Length > 0)
                        {
                            string PPRunImgFolderPath = GlobalSettings.ProgramPath.Substring(0, GlobalSettings.ProgramPath.LastIndexOf("\\")) + "\\Images";
                            string ProgramName = ((System.Data.DataRowView)PartProgramPopUp.dgvPrograms.Items[PartProgramPopUp.SelectedProgIndex]).Row[0].ToString();
                            ProgramName = ProgramName.Substring(0, ProgramName.LastIndexOf("."));
                            PPRunImgFolderPath = PPRunImgFolderPath + "\\" + ProgramName + "\\" +
                                "Style" + (PartProgramPopUp.lstReportStyles.SelectedIndex + 1) +
                                "_Report" + (PartProgramPopUp.lstReports.SelectedIndex + 1) +
                                "_Run" + GlobalSettings.MainExcelInstance.ComponentNo;
                            if (!Directory.Exists(PPRunImgFolderPath))
                                Directory.CreateDirectory(PPRunImgFolderPath);

                            for (int i = 0; i < _ProgramImgNames.Length; i++)
                            {
                                string TmpFileName = _ProgramImgNames[i].Substring(_ProgramImgNames[i].LastIndexOf("\\") + 1);
                                File.Copy(_ProgramImgNames[i], PPRunImgFolderPath + "\\" + TmpFileName, true);
                                GlobalSettings.SettingsReader.InsertExcelDocPPRun(ProgramName + ".rppx", TmpFileName.Substring(0, TmpFileName.LastIndexOf(".")), PPRunImgFolderPath.Substring(PPRunImgFolderPath.LastIndexOf("\\") + 1));
                                //File.Delete(_ProgramImgNames[i]);
                            }
                        }

                        if (TxtGridRunCount.Text == "" || TxtGridRunCount.Text == "Done")
                        {
                            for (int i = 0; i < St.Children.Count; i++)
                            {
                                if (St.Children[i].GetType() != typeof(System.Windows.Controls.Button)) continue;
                                string cBtnName;
                                ContentControl c = (ContentControl)St.Children[i];
                                if (c.Tag.GetType() == typeof(string[]))
                                {
                                    cBtnName = ((string[])c.Tag)[2];
                                    if (c.ToolTip.ToString() == "Run" || c.ToolTip.ToString() == "Pause" || c.ToolTip.ToString() == "Continue")
                                    {
                                        St.AssignSingleBtnProp(c, "Run", "Run", GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32));
                                        ((string[])(c.Tag))[2] = "Run";
                                        break;
                                    }
                                }
                                else
                                {
                                    cBtnName = ((ButtonProperties)c.Tag).ButtonName;
                                    switch (cBtnName)
                                    {
                                        case "Run":
                                        case "Pause":
                                        case "Continue":
                                            St.AssignSingleBtnProp(c, "Run", "Run", GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32));
                                            c.ToolTip = "Run";
                                            break;
                                    }
                                }
                            }
                        }
                        ArrangeForPartProgramStop();
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_PartProgram.PartprogramEventHandler(Handle_MeasurementFinished));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV81", ex); }
        }

        //DateTime st1, st2, st3, st4, st5, st6;

        //handling the options of Part-Program Build windowvi
        void HandlePartProgramBuildWin_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Button b = (Button)sender;
                switch (b.Tag.ToString())
                {
                    case "Design Template":
                        {
                            DesignTemplateClicked = true;
                            if (!IsProgramNamesValid(PartProgramBuildWin.GetProgramName, PartProgramBuildWin.GetReportName, PartProgramBuildWin.GetReportStyle)) return;
                            GlobalSettings.MainExcelInstance.ProgramName = PartProgramBuildWin.GetProgramName;
                            GlobalSettings.MainExcelInstance.ReportName = PartProgramBuildWin.GetReportName;
                            GlobalSettings.MainExcelInstance.ReportStyleName = PartProgramBuildWin.GetReportStyle;
                            GlobalSettings.MainExcelInstance.DefaultReportSelection(!GlobalSettings.XLExportRowWise, !GlobalSettings.XLExportPortraitLayout);
                            PartProgramBuildWin.txtProgramName.IsReadOnly = true;
                            PartProgramBuildWin.txtReportName.IsReadOnly = true;
                            //GlobalSettings.MainExcelInstance.Report
                            //GlobalSettings.MainExcelInstance.MeasurementList = RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementList();
                            
                            GlobalSettings.MainExcelInstance.ToleranceFormat = GlobalSettings.XLDataTransferTolMode;
                            GlobalSettings.MainExcelInstance.MeasurementStringList = RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementListWithNT();
                            string PicFile1 = System.IO.Path.GetTempPath() + "RRMExportPic1.jpg";
                            string PicFile2 = System.IO.Path.GetTempPath() + "RRMExportPic2.jpg";
                            if (GlobalSettings.XLExportImg1DuringPP == 1)
                            {
                                RWrapper.RW_MainInterface.MYINSTANCE().ClearShapeAndMeasure_Selections();
                                System.Threading.Thread.Sleep(50);
                                RWrapper.RW_MainInterface.MYINSTANCE().SaveRcadWindowImage(PicFile1, false);
                            }
                            else if (GlobalSettings.XLExportImg1DuringPP == 2)
                            {
                                RWrapper.RW_MainInterface.MYINSTANCE().SaveVideoWindowImage(PicFile1, false, false);
                            }
                            else if (GlobalSettings.XLExportImg1DuringPP == 3)
                            {
                                RWrapper.RW_MainInterface.MYINSTANCE().SaveVideoWindowImage(PicFile1, true, false);
                            }

                            if (GlobalSettings.XLExportImg2DuringPP == 1)
                            {
                                RWrapper.RW_MainInterface.MYINSTANCE().ClearShapeAndMeasure_Selections();
                                System.Threading.Thread.Sleep(50);
                                RWrapper.RW_MainInterface.MYINSTANCE().SaveRcadWindowImage(PicFile2, false);
                            }
                            else if (GlobalSettings.XLExportImg2DuringPP == 2)
                            {
                                RWrapper.RW_MainInterface.MYINSTANCE().SaveVideoWindowImage(PicFile2, false, false);
                            }
                            else if (GlobalSettings.XLExportImg2DuringPP == 3)
                            {
                                RWrapper.RW_MainInterface.MYINSTANCE().SaveVideoWindowImage(PicFile2, true, false);
                            }
                            GlobalSettings.MainExcelInstance.PicturePath = PicFile1;
                            GlobalSettings.MainExcelInstance.Picture2Path = PicFile2;
                            GlobalSettings.MainExcelInstance.Reset_All(false, true);
                            //GlobalSettings.MainExcelInstance.TopMost = true;
                            
                            GlobalSettings.MainExcelInstance.Show();
                            GlobalSettings.MainExcelInstance.BringToFront();
                            GlobalSettings.MainExcelInstance.Hide();
                            GlobalSettings.MainExcelInstance.Show();
 
                        }
                        break;
                    case "Build":
                        {
                            //st1 = DateTime.Now;
                            List<System.Collections.ArrayList> TmpAllMeasuremntLst;
                            TmpAllMeasuremntLst = RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementListWithNT();
                            //st2 = DateTime.Now;
                            bool PPFlag = false;
                            PPFlag = RWrapper.RW_PartProgram.MYINSTANCE().PartProgram_Build(PartProgramBuildWin.GetProgramName);
                            //MessageBox.Show("Finished Creating Part Program rppx file " + PPFlag.ToString());
                            //st3 = DateTime.Now;
                            if (!PPFlag)
                            {
                                PartProgramBuildWin.Hide();
                                if (!OtherWinToolbarRotated)
                                    RapidCADRBtn.IsChecked = true;
                                EnableDisableControlsDuringPPBuild(false);
                            }
                            else
                            {
                                if (!DesignTemplateClicked)
                                {
                                    string CsvFilePath = PartProgramBuildWin.GetReportName.Substring(0, PartProgramBuildWin.GetReportName.LastIndexOf(".") + 1);
                                    CsvFilePath += "csv";

                                    if (PartProgramBuildWin.AllExistingProgramCBx.SelectedIndex <0)
                                        if (!IsProgramNamesValid(PartProgramBuildWin.GetProgramName, PartProgramBuildWin.GetReportName, PartProgramBuildWin.GetReportStyle)) return;
                                    //MessageBox.Show("Starting Excel Works");
                                    //st4 = DateTime.Now;

                                    //GlobalSettings.MainExcelInstance.MeasurementList = RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementList();
                                    GlobalSettings.MainExcelInstance.ToleranceFormat = GlobalSettings.XLDataTransferTolMode;
                                    GlobalSettings.MainExcelInstance.MeasurementStringList = TmpAllMeasuremntLst; // RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementListWithNT();
                                    //MessageBox.Show("Set Tolerance format and Measurementstring");
                                    GlobalSettings.MainExcelInstance.ProgramName = PartProgramBuildWin.GetProgramName;
                                    GlobalSettings.MainExcelInstance.ReportName = PartProgramBuildWin.GetReportName;
                                    GlobalSettings.MainExcelInstance.ReportStyleName = PartProgramBuildWin.GetReportStyle;
                                    GlobalSettings.MainExcelInstance.AllProgramInSingleReport = GlobalSettings.AllProgramInSingleReport;
                                    //MessageBox.Show("Finished setting Program and report names");
                                    GlobalSettings.MainExcelInstance.Reset_All();
                                    //MessageBox.Show("Finished making settings for report");
                                    GlobalSettings.MainExcelInstance.CreateCsv = GlobalSettings.PartProgBuildGenerateCSV;
                                    GlobalSettings.MainExcelInstance.ReportNameCsv = CsvFilePath;
                                }


                                string PicFile1 = System.IO.Path.GetTempPath() + "RRMExportPic1.jpg";
                                string PicFile2 = System.IO.Path.GetTempPath() + "RRMExportPic2.jpg";
                                if (GlobalSettings.XLExportImg1DuringPP == 1)
                                {
                                    RWrapper.RW_MainInterface.MYINSTANCE().ClearShapeAndMeasure_Selections();
                                    System.Threading.Thread.Sleep(50);
                                    RWrapper.RW_MainInterface.MYINSTANCE().SaveRcadWindowImage(PicFile1, false);
                                }
                                else if (GlobalSettings.XLExportImg1DuringPP == 2)
                                {
                                    RWrapper.RW_MainInterface.MYINSTANCE().SaveVideoWindowImage(PicFile1, false, false);
                                }
                                else if (GlobalSettings.XLExportImg1DuringPP == 3)
                                {
                                    RWrapper.RW_MainInterface.MYINSTANCE().SaveVideoWindowImage(PicFile1, true, false);
                                }

                                if (GlobalSettings.XLExportImg2DuringPP == 1)
                                {
                                    RWrapper.RW_MainInterface.MYINSTANCE().SaveVideoWindowImage(PicFile2, false, false);
                                }
                                else if (GlobalSettings.XLExportImg2DuringPP == 2)
                                {
                                    RWrapper.RW_MainInterface.MYINSTANCE().SaveVideoWindowImage(PicFile2, true, false);
                                }
                                GlobalSettings.MainExcelInstance.PicturePath = PicFile1;
                                GlobalSettings.MainExcelInstance.Picture2Path = PicFile2;
                                GlobalSettings.MainExcelInstance.Reset_All(true);

                                if (GlobalSettings.XLExportImg1DuringPP > 0)
                                {
                                    if (GlobalSettings.XLExportImg2DuringPP > 0)
                                    {
                                        GlobalSettings.MainExcelInstance.InsertDoublePicture = true;
                                        string[] pf = new string[2];
                                        pf[0] = PicFile1; pf[1] = PicFile2;
                                        GlobalSettings.MainExcelInstance.ImagePaths = pf;
                                    }
                                    else
                                        GlobalSettings.MainExcelInstance.InsertImage = true;
                                }
                                int State = MeasurementsPopup.SetPassFail();
                                switch (State)
                                {
                                    case 0:
                                        GlobalSettings.MainExcelInstance.passStatus = rrm.Form1.PassFail.none;
                                        break;
                                    case 1:
                                        GlobalSettings.MainExcelInstance.passStatus = rrm.Form1.PassFail.pass;
                                        break;
                                    case 2:
                                        GlobalSettings.MainExcelInstance.passStatus = rrm.Form1.PassFail.fail;
                                        break;
                                }

                                GlobalSettings.MainExcelInstance.TemplateFolderName = GlobalSettings.TemplatePath;
                                GlobalSettings.MainExcelInstance.ShowAvging = GlobalSettings.ShowAveragingInExcel;
                                //MessageBox.Show("Finished setting images and other stuff");
                                GlobalSettings.MainExcelInstance.GenerateExcel();
                                    //st5 = DateTime.Now;
                                //MessageBox.Show("Finished making excel report");
                                GlobalSettings.MainExcelInstance.Update_Database();
                                GlobalSettings.MainExcelInstance.NewTolerance();
                                //st6 = DateTime.Now;
                                if (GlobalSettings.XLExportImg1DuringPP != 0)
                                    System.IO.File.Delete(PicFile1);
                                if (GlobalSettings.XLExportImg2DuringPP != 0)
                                    System.IO.File.Delete(PicFile2);

                                //for (int i = 0; i < GlobalSettings.MainExcelInstance.MeasurementStringList.Count; i++)
                                //{
                                //    if (GlobalSettings.MainExcelInstance.MeasurementStringList[i][0].ToString() != TmpAllMeasuremntLst[i][0].ToString())
                                //    {
                                //        RWrapper.RW_MeasureParameter.MYINSTANCE().RenameMeasurement(GlobalSettings.MainExcelInstance.MeasurementStringList[i][0].ToString(), (int)GlobalSettings.MainExcelInstance.MeasurementStringList[i][6]);
                                //    }
                                //    if ((double)GlobalSettings.MainExcelInstance.MeasurementStringList[i][3] != (double)TmpAllMeasuremntLst[i][3] ||
                                //        (double)GlobalSettings.MainExcelInstance.MeasurementStringList[i][4] != (double)TmpAllMeasuremntLst[i][4] ||
                                //        (double)GlobalSettings.MainExcelInstance.MeasurementStringList[i][5] != (double)TmpAllMeasuremntLst[i][5])
                                //    {
                                //        RWrapper.RW_MeasureParameter.MYINSTANCE().SetMeasureNominalTolerance((double)GlobalSettings.MainExcelInstance.MeasurementStringList[i][3],
                                //                                                                             (double)GlobalSettings.MainExcelInstance.MeasurementStringList[i][4],
                                //                                                                             (double)GlobalSettings.MainExcelInstance.MeasurementStringList[i][5],
                                //                                                                             (int)GlobalSettings.MainExcelInstance.MeasurementStringList[i][6]);
                                //    }
                                //}
                                //MessageBox.Show("Finished updating DB");

                                PartProgramBuildWin.Hide();
                                //System.Threading.Thread t1 = new System.Threading.Thread(new ThreadStart(ResetAll_LoadForm));
                                //t1.Start();
                                //ResetAll_LoadForm();
                                //GlobalSettings.LoadExcelInstance.Reset_All();
                                LoadProgramReportNames(false);
                                DesignTemplateClicked = false;
                                PartProgramBuildWin.txtProgramName.IsReadOnly = false;
                                PartProgramBuildWin.txtReportName.IsReadOnly = false;
                                //PartProgramPopUp.dgvPrograms.SelectedIndex = PartProgramPopUp.dgvPrograms.Items.Count - 1;
                                //PartProgramPopUp.dgvPrograms.UpdateLayout();
                                //PartProgramPopUp.dgvPrograms.ScrollIntoView(PartProgramPopUp.dgvPrograms.Items[PartProgramPopUp.dgvPrograms.Items.Count - 1], PartProgramPopUp.dgvPrograms.Columns[0]);
                                //MessageBox.Show((st2 - st1).Milliseconds.ToString() + ", " + (st3 - st2).Milliseconds.ToString() + ", " + (st4 - st3).Milliseconds.ToString() + ", " +
                                //    (st5 - st4).Milliseconds.ToString() + ", " + (st6 - st5).Milliseconds.ToString() + ", " + (DateTime.Now - st6).Milliseconds.ToString() + ", ", "Rapid-I");
                            }
                        }
                        break;
                    //case "Set Critical Steps":
                    //    {
                    //        PPBuildOptionsWin TmpOptionsWin = new PPBuildOptionsWin();
                    //        TmpOptionsWin.ShowDialog();
                    //    }
                    //    break;
                    case "Close":
                        {
                            MaxCountOfProgram -= 1;
                            DesignTemplateClicked = false;
                            PartProgramBuildWin.txtProgramName.IsReadOnly = false;
                            PartProgramBuildWin.txtReportName.IsReadOnly = false;
                            PartProgramBuildWin.Hide();
                            GlobalSettings.MainExcelInstance.UseExistingReport = false;
                            PartProgramBuildWin.AllExistingProgramCBx.SelectedIndex = -1;
                            PartProgramBuildWin.AllExistingProgramCBx.Text = "";
                            System.Threading.Thread t1 = new System.Threading.Thread(new ThreadStart(UpdateAllReportNames));
                            t1.Start();
                            //UpdateAllReportNames();
                        }
                        break;
                }

            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV82", ex); }
        }


        //void HandlePartProgramBuildWin_Click(object sender, RoutedEventArgs e)
        //{
        //    try
        //    {
        //        Button b = (Button)sender;
        //        switch (b.Tag.ToString())
        //        {
        //            case "Design Template":
        //                {
        //                    DesignTemplateClicked = true;
        //                    if (!IsProgramNamesValid(PartProgramBuildWin.GetProgramName, PartProgramBuildWin.GetReportName, PartProgramBuildWin.GetReportStyle)) return;
        //                    GlobalSettings.MainExcelInstance.ProgramName = PartProgramBuildWin.GetProgramName;
        //                    GlobalSettings.MainExcelInstance.ReportName = PartProgramBuildWin.GetReportName;
        //                    GlobalSettings.MainExcelInstance.ReportStyleName = PartProgramBuildWin.GetReportStyle;
        //                    PartProgramBuildWin.txtProgramName.IsReadOnly = true;
        //                    PartProgramBuildWin.txtReportName.IsReadOnly = true;
        //                    //GlobalSettings.MainExcelInstance.Report
        //                    //GlobalSettings.MainExcelInstance.MeasurementList = RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementList();
        //                    GlobalSettings.MainExcelInstance.MeasurementStringList = RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementListWithNT();
        //                    string PicFile = System.IO.Path.GetTempPath() + "RRMExportPic.jpg";
        //                    RWrapper.RW_MainInterface.MYINSTANCE().ClearShapeAndMeasure_Selections();
        //                    System.Threading.Thread.Sleep(50);
        //                    RWrapper.RW_MainInterface.MYINSTANCE().ZoomToExtentsRcadorDxf(1);
        //                    System.Threading.Thread.Sleep(50);
        //                    RWrapper.RW_MainInterface.MYINSTANCE().SaveRcadWindowImage(PicFile, false);
        //                    GlobalSettings.MainExcelInstance.PicturePath = PicFile;
        //                    GlobalSettings.MainExcelInstance.Reset_All();
        //                    //GlobalSettings.MainExcelInstance.TopMost = true;
        //                    GlobalSettings.MainExcelInstance.Show();
        //                    GlobalSettings.MainExcelInstance.BringToFront();
        //                }
        //                break;
        //            case "Build":
        //                {
        //                    List<System.Collections.ArrayList> TmpAllMeasuremntLst;
        //                    TmpAllMeasuremntLst = RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementListWithNT();
        //                    RWrapper.RW_PartProgram.MYINSTANCE().PartProgram_Build(PartProgramBuildWin.GetProgramName);
        //                    if (!DesignTemplateClicked)
        //                    {
        //                        if (!IsProgramNamesValid(PartProgramBuildWin.GetProgramName, PartProgramBuildWin.GetReportName, PartProgramBuildWin.GetReportStyle)) return;
        //                        //GlobalSettings.MainExcelInstance.MeasurementList = RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementList();
        //                        GlobalSettings.MainExcelInstance.MeasurementStringList = RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementListWithNT();
        //                        GlobalSettings.MainExcelInstance.ProgramName = PartProgramBuildWin.GetProgramName;
        //                        GlobalSettings.MainExcelInstance.ReportName = PartProgramBuildWin.GetReportName;
        //                        GlobalSettings.MainExcelInstance.ReportStyleName = PartProgramBuildWin.GetReportStyle;
        //                        GlobalSettings.MainExcelInstance.Reset_All();
        //                    }
        //                    GlobalSettings.MainExcelInstance.ShowPassFail = MeasurementsPopup.ShowPassFail();
        //                    GlobalSettings.MainExcelInstance.TemplateFolderName = GlobalSettings.TemplatePath;
        //                    GlobalSettings.MainExcelInstance.GenerateExcel();
        //                    GlobalSettings.MainExcelInstance.Update_Database();
        //                    GlobalSettings.MainExcelInstance.NewTolerance();
        //                    //for (int i = 0; i < GlobalSettings.MainExcelInstance.MeasurementStringList.Count; i++)
        //                    //{
        //                    //    if (GlobalSettings.MainExcelInstance.MeasurementStringList[i][0].ToString() != TmpAllMeasuremntLst[i][0].ToString())
        //                    //    {
        //                    //        RWrapper.RW_MeasureParameter.MYINSTANCE().RenameMeasurement(GlobalSettings.MainExcelInstance.MeasurementStringList[i][0].ToString(), (int)GlobalSettings.MainExcelInstance.MeasurementStringList[i][6]);
        //                    //    }
        //                    //    if ((double)GlobalSettings.MainExcelInstance.MeasurementStringList[i][3] != (double)TmpAllMeasuremntLst[i][3] ||
        //                    //        (double)GlobalSettings.MainExcelInstance.MeasurementStringList[i][4] != (double)TmpAllMeasuremntLst[i][4] ||
        //                    //        (double)GlobalSettings.MainExcelInstance.MeasurementStringList[i][5] != (double)TmpAllMeasuremntLst[i][5])
        //                    //    {
        //                    //        RWrapper.RW_MeasureParameter.MYINSTANCE().SetMeasureNominalTolerance((double)GlobalSettings.MainExcelInstance.MeasurementStringList[i][3],
        //                    //                                                                             (double)GlobalSettings.MainExcelInstance.MeasurementStringList[i][4],
        //                    //                                                                             (double)GlobalSettings.MainExcelInstance.MeasurementStringList[i][5],
        //                    //                                                                             (int)GlobalSettings.MainExcelInstance.MeasurementStringList[i][6]);
        //                    //    }
        //                    //}
        //                    GlobalSettings.LoadExcelInstance.Reset_All();
        //                    LoadProgramReportNames(false);
        //                    PartProgramBuildWin.Hide();
        //                    DesignTemplateClicked = false;
        //                    PartProgramBuildWin.txtProgramName.IsReadOnly = false;
        //                    PartProgramBuildWin.txtReportName.IsReadOnly = false;
        //                    //PartProgramPopUp.dgvPrograms.SelectedIndex = PartProgramPopUp.dgvPrograms.Items.Count - 1;
        //                    PartProgramPopUp.dgvPrograms.UpdateLayout();
        //                    PartProgramPopUp.dgvPrograms.ScrollIntoView(PartProgramPopUp.dgvPrograms.Items[PartProgramPopUp.dgvPrograms.Items.Count - 1], PartProgramPopUp.dgvPrograms.Columns[0]);

        //                }
        //                break;
        //            case "Close":
        //                {
        //                    MaxCountOfProgram -= 1;
        //                    DesignTemplateClicked = false;
        //                    PartProgramBuildWin.txtProgramName.IsReadOnly = false;
        //                    PartProgramBuildWin.txtReportName.IsReadOnly = false;
        //                    PartProgramBuildWin.Hide();
        //                }
        //                break;
        //        }

        //    }
        //    catch (Exception ex)
        //    { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0074", ex); }
        //}

        bool ProgramLoading = false, ProgramRunning = false;
        //Handling the selection of an particular program by usere

        void dgvPrograms_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            if (e.LeftButton != MouseButtonState.Pressed) return;
            Load_PartProgram();
        }

        public void Load_PartProgram()
        {
            try
            {
                if (PartProgramPopUp.dgvPrograms.SelectedIndex >= 0)
                {
                    //get the selected program name
                    System.Data.DataRow dr = ((System.Data.DataRowView)PartProgramPopUp.dgvPrograms.SelectedItem).Row;
                    //Get all report styles based on the program selected
                    System.Collections.ArrayList ReportStyles = GlobalSettings.LoadExcelInstance.GetAllreportStyles((int)dr["Program_Id"]);// GlobalSettings.PartProgramPaths[PartProgramPopUp.dgvPrograms.SelectedIndex] + dr[0].ToString());
                    //MessageBox.Show("load1");
                    PartProgramPopUp.SelectedProgIndex = PartProgramPopUp.dgvPrograms.SelectedIndex;
                    //MessageBox.Show("load2");
                    //PartProgramPopUp.lstReportStyles.Items.Clear();
                    PartProgramPopUp.lstReportStyles.ItemsSource = ReportStyles;
                    //for (int i = 0; i < ReportStyles.Count; i++)
                    //    PartProgramPopUp.lstReportStyles.Items.Add(ReportStyles[i]);
                    
                    //MessageBox.Show(PartProgramPopUp.lstReportStyles.Items.Count.ToString());
                    //Select the last made program
                    //lstReportStyles_SelectionChanged
                    PartProgramPopUp.lstReportStyles.UpdateLayout();
                    //MessageBox.Show("load2.1");

                    //PartProgramPopUp.lstReportStyles.SelectedItem = PartProgramPopUp.lstReportStyles.Items[PartProgramPopUp.lstReportStyles.Items.Count - 1];
                    //PartProgramPopUp.lstReportStyles.Items[PartProgramPopUp.lstReportStyles.Items.Count - 1]
                    PartProgramPopUp.lstReportStyles.ScrollIntoView(PartProgramPopUp.lstReportStyles.SelectedItem);
                    //MessageBox.Show("load3");

                    if (PartProgramPopUp.dgvPrograms.SelectedIndex >= 0)
                    {
                        ProgramLoading = true;
                        //Load the selected part-program.
                        //PartProgramPopUp.SelectedProgIndex = PartProgramPopUp.dgvPrograms.SelectedIndex;
                        System.Data.DataRow dr2 = ((System.Data.DataRowView)PartProgramPopUp.dgvPrograms.Items[PartProgramPopUp.SelectedProgIndex]).Row;
                        //GlobalSettings.MainExcelInstance.ReportName = PartProgramPopUp.lstReports.SelectedItem.ToString();
                        GlobalSettings.MainExcelInstance.ProgramName = GlobalSettings.PartProgramPaths[PartProgramPopUp.SelectedProgIndex] + dr2[0].ToString();
                        //MessageBox.Show("load4");
                        PartProgramPopUp.CurrentLoadedProgLbl.Visibility = System.Windows.Visibility.Visible;
                        GlobalSettings.PartProgramLoaded = true;
                        PartProgramPopUp.CurrentLoadedProgLbl.Text = "Current: " + dr2[0].ToString();
                        if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI && GlobalSettings.DelfiRotaryZeroingFloatingWindowView)
                        {
                            DelfiRotaryContainerWindow.MyInstance().tbb.Text = "Current Program: " + dr2[0].ToString();
                        }
                        //MessageBox.Show("load5");

                        RWrapper.RW_PartProgram.MYINSTANCE().PartProgram_Load(GlobalSettings.PartProgramPaths[PartProgramPopUp.SelectedProgIndex] + dr2[0].ToString());
                        TxtGridRunCount.Text = "";
                        ProgramLoading = false;
                        lightingPopup.btnPPLightingControl.Visibility = Visibility.Visible;
                        PartProgramPopUp.lstReportStyles.SelectedIndex = -1;
                        PartProgramPopUp.lstReportStyles.SelectedIndex = PartProgramPopUp.lstReportStyles.Items.Count - 1;
                    }
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV83", ex); }
        }

        void dgvPrograms_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                if (sender.GetType() == typeof(ComboBox))
                {
                    PartProgramPopUp.dgvPrograms.SelectedIndex = ((ComboBox)sender).SelectedIndex;
                    Load_PartProgram();
                }
                //if (e.AddedItems.Count > 0 && PartProgramPopUp.dgvPrograms.SelectedIndex >= 0)
                //{

                //    //get the selected program name
                //    System.Data.DataRow dr = ((System.Data.DataRowView)e.AddedItems[0]).Row;
                //    //((System.Data.DataRowView)PartProgramPopUp.dgvPrograms.Items[PartProgramPopUp.dgvPrograms.SelectedIndex]).Row.
                //    //Get all report styles based on the program selected
                //    System.Collections.ArrayList ReportStyles = GlobalSettings.LoadExcelInstance.GetAllreportStyles(GlobalSettings.PartProgramPaths[PartProgramPopUp.dgvPrograms.SelectedIndex] + dr[0].ToString());

                //    PartProgramPopUp.lstReportStyles.Items.Clear();
                //    for (int i = 0; i < ReportStyles.Count; i++)
                //        PartProgramPopUp.lstReportStyles.Items.Add(ReportStyles[i]);
                //    //Select the last made program
                //    PartProgramPopUp.lstReportStyles.SelectedItem = PartProgramPopUp.lstReportStyles.Items[PartProgramPopUp.lstReportStyles.Items.Count - 1];
                //    //PartProgramPopUp.lstReportStyles.Items[PartProgramPopUp.lstReportStyles.Items.Count - 1]
                //    PartProgramPopUp.lstReportStyles.ScrollIntoView(PartProgramPopUp.lstReportStyles.SelectedItem);

                //    if (PartProgramPopUp.dgvPrograms.SelectedIndex >= 0)
                //    {
                //        ProgramLoading = true;
                //        //Load the selected part-program.
                //        PartProgramPopUp.SelectedProgIndex = PartProgramPopUp.dgvPrograms.SelectedIndex;
                //        System.Data.DataRow dr2 = ((System.Data.DataRowView)PartProgramPopUp.dgvPrograms.Items[PartProgramPopUp.dgvPrograms.SelectedIndex]).Row;
                //        GlobalSettings.MainExcelInstance.ReportName = PartProgramPopUp.lstReports.SelectedItem.ToString();
                //        GlobalSettings.MainExcelInstance.ProgramName = GlobalSettings.PartProgramPaths[PartProgramPopUp.dgvPrograms.SelectedIndex] + dr2[0].ToString();
                //        RWrapper.RW_PartProgram.MYINSTANCE().PartProgram_Load(GlobalSettings.PartProgramPaths[PartProgramPopUp.dgvPrograms.SelectedIndex] + dr2[0].ToString());
                //        ProgramLoading = false;
                //    }
                //}
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV83", ex); }
        }


        //handling the change of the report styles
        void lstReportStyles_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                if (sender.Equals(PartProgramPopUp.lstReportStyles))
                {
                    if (PartProgramPopUp.lstReportStyles.SelectedItem != null)
                    {
                        System.Collections.ArrayList Reports = GlobalSettings.LoadExcelInstance.GetAllReportNames(PartProgramPopUp.lstReportStyles.SelectedItem.ToString());
                        //MessageBox.Show("rs1");
                        PartProgramPopUp.lstReports.ItemsSource = Reports;
                        //PartProgramPopUp.lstReports.Items.Clear();
                        //for (int i = 0; i < Reports.Count; i++)
                        //    PartProgramPopUp.lstReports.Items.Add(Reports[i]);
                        if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
                            CheckAndAddnewReportForToday();
                        PartProgramPopUp.lstReports.UpdateLayout();
                        PartProgramPopUp.lstReports.SelectedIndex = -1; // SelectedItem = PartProgramPopUp.lstReports.Items[PartProgramPopUp.lstReports.Items.Count - 1];
                        PartProgramPopUp.lstReports.SelectedIndex = 0; // SelectedItem = PartProgramPopUp.lstReports.Items[PartProgramPopUp.lstReports.Items.Count - 1];
                        //PartProgramPopUp.lstReports.ScrollIntoView(PartProgramPopUp.lstReports.SelectedItem);
                        //MessageBox.Show("rs2");


                        //Get all the title values.

                        System.Data.DataTable titletable = new System.Data.DataTable();
                        titletable.Columns.Add("Title Name", typeof(string));
                        titletable.Columns.Add("Title Value", typeof(string));

                        System.Collections.ArrayList TitleList = GlobalSettings.LoadExcelInstance.GetAllReportTitles(PartProgramPopUp.lstReportStyles.SelectedItem.ToString());
                        for (int i = 0; i < TitleList.Count; i++)
                            titletable.Rows.Add((object[])TitleList[i]);
                        //MessageBox.Show("rs3");

                        PartProgramPopUp.dgvTitles.ItemsSource = null;
                        PartProgramPopUp.dgvTitles.ItemsSource = titletable.DefaultView;
                    }
                }
                else if (sender.Equals(PartProgramPopUp.lstReports))
                {

                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV84", ex); }
        }

        void lstReports_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                if (PartProgramPopUp.lstReports.SelectedItem != null)
                {
                    GlobalSettings.MainExcelInstance.ReportName = PartProgramPopUp.lstReports.SelectedItem.ToString();
                    System.Data.DataRow dr2 = ((System.Data.DataRowView)PartProgramPopUp.dgvPrograms.Items[PartProgramPopUp.SelectedProgIndex]).Row;
                    //MessageBox.Show(dr2[0].ToString());
                    //GlobalSettings.MainExcelInstance.ProgramName = GlobalSettings.PartProgramPaths[PartProgramPopUp.SelectedProgIndex] + dr2[0].ToString();
                    if (GlobalSettings.MainExcelInstance.ReportName == null || GlobalSettings.MainExcelInstance.ReportName == "")
                    {
                        MessageBox.Show("Selected Report does not exist!");
                        return;
                    }
                    if (!GlobalSettings.MainExcelInstance.ReportData(PartProgramPopUp.lstReportStyles.SelectedIndex))
                    {
                        MessageBox.Show("Could not read report file properly.", "Warning", MessageBoxButton.OK, MessageBoxImage.Warning);
                        return;
                    }
                    GlobalSettings.MainExcelInstance.UseExistingReport = true;
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV85", ex); }
        }
        //handling the Title data grid view cell end editto get to know that whether title value has been changed or not.
        void dgvTitles_CellEditEnding(object sender, DataGridCellEditEndingEventArgs e)
        {
            try
            {
                TitleValuesChanged = GlobalSettings.LoadExcelInstance.CheckTitlesChange(PartProgramPopUp.lstReportStyles.SelectedItem.ToString(),
                    PartProgramPopUp.dgvTitles.SelectedCells[0].Item.ToString(), PartProgramPopUp.dgvTitles.SelectedIndex);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV86", ex); }
        }
        //handling the Searching of Partprogram based on text Written the the Textbox.
        private void SearchTxtbx_KeyDown(object sender, TextChangedEventArgs e)
        {
            TextBox Txtbx = (TextBox)sender;
            LoadProgramReportNames(false, Txtbx.Text.ToString());
            // TODO: Add event handler implementation here.
        }

        //handling the re- Run of part program (grid run)
        void Handle_RuntheProgramAgain()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    //ContentControl c = St.Children.Cast<ContentControl>().Where(T => T.ToolTip.ToString() == "Run").First();
                    //c.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Pause", "Pause", 32, 32);
                    //c.ToolTip = "Pause";
                    //((string[])(c.Tag))[2] = "Pause";

                    GlobalSettings.MainExcelInstance.ReportName = PartProgramPopUp.lstReports.SelectedItem.ToString();
                    System.Threading.Thread TmpTh = new System.Threading.Thread(GlobalSettings.MainExcelInstance.OpenExcel);
                    TmpTh.Start();
                    PartProgramPopUp.dgvPrograms.IsEnabled = false;
                    //EnableDisableControlsDuringProgramRun(false);
                    ProgramRunning = true;
                    ArrangeForPartProgramRun();
                    RWrapper.RW_PartProgram.MYINSTANCE().PartProgram_GridRun();
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_PartProgram.PartprogramEventHandler(Handle_RuntheProgramAgain));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV87", ex); }
        }

        void Handle_RunIsnotValid()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    ContentControl c = St.Children.Cast<ContentControl>().Where(T => T.ToolTip.ToString() == "Pause").First();
                    //c.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32);
                    //c.ToolTip = "Run";
                    //MessageBox.Show("Got a call to run the program before it was loaded!", "Rapid-I");
                    St.AssignSingleBtnProp(c, "Run", "Run", GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32));
                    ((string[])(c.Tag))[2] = "Run";
                    //for (int i = 0; i < PartProgramPopUp.St.Children.Count; i++)
                    //{
                    //    ContentControl c = (ContentControl)PartProgramPopUp.St.Children[i];
                    //    if (c.ToolTip.ToString() == "Pause")
                    //    {
                    //        c.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 48, 48);
                    //        c.ToolTip = "Run";
                    //        break;
                    //    }
                    //}
                    PartProgramPopUp.dgvPrograms.IsEnabled = true;
                    EnableDisableControlsDuringProgramRun(true);
                    ProgramRunning = false;
                    //if (File.Exists(PartProgramPopUp.lstReports.SelectedItem.ToString()))
                    //    System.Diagnostics.Process.Start(PartProgramPopUp.lstReports.SelectedItem.ToString());

                    ArrangeForPartProgramStop();
                    //MessageBox.Show("Run was not valid!", "Rapid I");
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_PartProgram.PartprogramEventHandler(Handle_RunIsnotValid));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV88", ex); }
        }
        void MainView_MoveImageEvent(int NoOfImgs)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    string TmpImgFolderPath = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Image";
                    string ProgramName = PartProgramBuildWin.GetProgramName;
                    ProgramName = ProgramName.Substring(ProgramName.LastIndexOf("\\") + 1);
                    ProgramName = ProgramName.Substring(0, ProgramName.LastIndexOf("."));
                    File.Copy(TmpImgFolderPath + "\\Image.txt", GlobalSettings.ProgramPath + "\\" + ProgramName + "_image.txt", true);
                    //File.Delete(TmpImgFolderPath + "\\Image.txt");

                    string[] _ProgramImgNames = Directory.GetFileSystemEntries(TmpImgFolderPath, "*.jpg");
                    if (_ProgramImgNames.Length >= NoOfImgs)
                    {
                        string PPRunImgFolderPath = GlobalSettings.ProgramPath.Substring(0, GlobalSettings.ProgramPath.LastIndexOf("\\")) + "\\Images";
                        PPRunImgFolderPath = PPRunImgFolderPath + "\\" + ProgramName + "\\" + "Style1_Report1_Run1";
                        if (!Directory.Exists(PPRunImgFolderPath))
                            Directory.CreateDirectory(PPRunImgFolderPath);

                        for (int i = 0; i < _ProgramImgNames.Length; i++)
                        {
                            if (i < NoOfImgs)
                            {
                                string TmpFileName = _ProgramImgNames[i].Substring(_ProgramImgNames[i].LastIndexOf("\\") + 1);
                                File.Copy(_ProgramImgNames[i], PPRunImgFolderPath + "\\" + TmpFileName, true);
                                GlobalSettings.SettingsReader.InsertExcelDocPPRun(ProgramName + ".rppx", TmpFileName.Substring(0, TmpFileName.LastIndexOf(".")), PPRunImgFolderPath.Substring(PPRunImgFolderPath.LastIndexOf("\\") + 1));
                            }
                            //File.Delete(_ProgramImgNames[i]);
                        }
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_PartProgram.PPImageEventHandler(MainView_MoveImageEvent), NoOfImgs);
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV88a", ex); }
        }
        void Handle_PartProgramBuild()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    if (!DesignTemplateClicked)
                    {
                        if (!IsProgramNamesValid(PartProgramBuildWin.GetProgramName, PartProgramBuildWin.GetReportName, PartProgramBuildWin.GetReportStyle)) return;
                        //GlobalSettings.MainExcelInstance.MeasurementList = RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementList();
                        GlobalSettings.MainExcelInstance.ToleranceFormat = GlobalSettings.XLDataTransferTolMode;
                        GlobalSettings.MainExcelInstance.MeasurementStringList = RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementListWithNT();
                        GlobalSettings.MainExcelInstance.ProgramName = PartProgramBuildWin.GetProgramName;
                        GlobalSettings.MainExcelInstance.ReportName = PartProgramBuildWin.GetReportName;
                        GlobalSettings.MainExcelInstance.ReportStyleName = PartProgramBuildWin.GetReportStyle;
                        GlobalSettings.MainExcelInstance.Reset_All();
                    }
                    int State = MeasurementsPopup.SetPassFail();
                    switch (State)
                    {
                        case 0:
                            GlobalSettings.MainExcelInstance.passStatus = rrm.Form1.PassFail.none;
                            break;
                        case 1:
                            GlobalSettings.MainExcelInstance.passStatus = rrm.Form1.PassFail.pass;
                            break;
                        case 2:
                            GlobalSettings.MainExcelInstance.passStatus = rrm.Form1.PassFail.fail;
                            break;
                    }
                    string PicFile = "", Pic2File = "";
                    PicFile = System.IO.Path.GetTempPath() + "RRMExportPic.jpg";
                    Pic2File = System.IO.Path.GetTempPath() + "RRMExportPic1.jpg";
                    GlobalSettings.MainExcelInstance.PicturePath = PicFile;
                    GlobalSettings.MainExcelInstance.Picture2Path = Pic2File;
                    GlobalSettings.MainExcelInstance.TemplateFolderName = GlobalSettings.TemplatePath;
                    GlobalSettings.MainExcelInstance.ShowAvging = GlobalSettings.ShowAveragingInExcel;
                    GlobalSettings.MainExcelInstance.InsertImageForDataTransfer = GlobalSettings.InsertImageForDataTransfer;
                    GlobalSettings.MainExcelInstance.GenerateExcel();
                    GlobalSettings.MainExcelInstance.Update_Database();
                    GlobalSettings.MainExcelInstance.NewTolerance();

                    GlobalSettings.LoadExcelInstance.Reset_All();
                    LoadProgramReportNames(false);
                    PartProgramBuildWin.Hide();
                    DesignTemplateClicked = false;
                    PartProgramBuildWin.txtProgramName.IsReadOnly = false;
                    PartProgramBuildWin.txtReportName.IsReadOnly = false;
                    ProgramRBtn.IsChecked = true;
                    EnableDisableControlsDuringPPBuild(true);
                    //PartProgramPopUp.dgvPrograms.SelectedIndex = PartProgramPopUp.dgvPrograms.Items.Count - 1;
                    PartProgramPopUp.dgvPrograms.UpdateLayout();
                    PartProgramPopUp.dgvPrograms.ScrollIntoView(PartProgramPopUp.dgvPrograms.Items[PartProgramPopUp.dgvPrograms.Items.Count - 1], PartProgramPopUp.dgvPrograms.Columns[0]);
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_PartProgram.PartprogramEventHandler(Handle_PartProgramBuild));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV89", ex); }
        }
        void MainView_PausePartProgram()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    ContentControl c = St.Children.Cast<ContentControl>().Where(T => T.ToolTip.ToString() == "Pause").First();
                    //c.Content = null;
                    //c.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Continue", "Continue", 32, 32);
                    //c.ToolTip = "Continue";
                    St.AssignSingleBtnProp(c, "Continue", "Continue", GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Continue", "Continue", 32, 32));
                    ((string[])(c.Tag))[2] = "Continue";
                    MagnificationList.IsEnabled = true;
                    EnableDisableControlsDuringProgramRun(false, true);
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_DRO.PausePartProgramEventHandler(MainView_PausePartProgram));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV89", ex); }
        }

        void MainView_CameraSoundEvent()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    GlobalSettings.SettingsReader.GetSoundPlayer("CameraClick").Play();
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_DRO.CameraClickSoundEventHandler(MainView_CameraSoundEvent));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV89a", ex); }
        }

        void Handle_MeasureUnitUpdate(string Unit)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    if (Unit == "mm")
                        mmInchesList.lstDropDownItems.SelectedItem = "mm";
                    else
                        mmInchesList.lstDropDownItems.SelectedItem = "In";
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_PartProgram.MeasureUnitEventHandler(Handle_MeasureUnitUpdate), Unit);
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV90", ex); }
        }

        void HandlePartProgramNewReportChoosen(object sender, RoutedEventArgs e)
        {
            try
            {
                //Get the selected program name & report style name
                string prog = GlobalSettings.PartProgramPaths[PartProgramPopUp.SelectedProgIndex] + 
                    ((System.Data.DataRowView)PartProgramPopUp.dgvPrograms.Items[PartProgramPopUp.SelectedProgIndex]).Row[0].ToString();
                string repoSty = PartProgramPopUp.lstReportStyles.SelectedItem.ToString();
                string rptName = GlobalSettings.ReportPath + "\\" + PartProgramPopUp.txtNewReport.Text + ".xls";
                if (GlobalSettings.MainExcelInstance.CheckNames(rptName, 3))
                {
                    MessageBox.Show("The entered Report Name already exists. Please change it.", "Rapid I");
                    return;
                }
                GlobalSettings.LoadExcelInstance.InsertNewReport(prog, repoSty, rptName, TitleValuesChanged);
                System.Collections.ArrayList Reports = GlobalSettings.LoadExcelInstance.GetAllReportNames(PartProgramPopUp.lstReportStyles.SelectedItem.ToString());
                PartProgramPopUp.lstReports.ItemsSource = null;
                PartProgramPopUp.lstReports.ItemsSource = Reports;
                //PartProgramPopUp.lstReports.Items.Add(rptName);
                int inddex = PartProgramPopUp.lstReports.Items.IndexOf(rptName);
                PartProgramPopUp.lstReports.SelectedIndex = inddex; // PartProgramPopUp.lstReports.Items.Count - 1; // SelectedItem =  rptName;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV91", ex); }
        }

        void HandlePartProgramNewReportStyle_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (PartProgramPopUp.SelectedProgIndex >= 0)
                {
                    GlobalSettings.MainExcelInstance.ProgramName = GlobalSettings.PartProgramPaths[PartProgramPopUp.SelectedProgIndex] + ((System.Data.DataRowView)PartProgramPopUp.dgvPrograms.Items[PartProgramPopUp.SelectedProgIndex]).Row[0].ToString();
                    GlobalSettings.MainExcelInstance.ReportName = PartProgramPopUp.lstReports.SelectedItem.ToString();
                    GlobalSettings.MainExcelInstance.CreateCsv = GlobalSettings.PartProgBuildGenerateCSV;
                    GlobalSettings.MainExcelInstance.CreateNewStyle();
                    //GlobalSettings.MainExcelInstance.TopMost = true;
                    GlobalSettings.MainExcelInstance.ShowDialog();

                    //GlobalSettings.MainExcelInstance.BringToFront();
                }
                else MessageBox.Show("Please load a program and then click on new Style", "Rapid I");
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV92", ex); }
        }

        void MainExcelInstance_NewStyleCreated()
        {
            try
            {
                GlobalSettings.LoadExcelInstance.UpdateNewStyle();
                PartProgramPopUp.lstReportStyles.Items.Add(GlobalSettings.MainExcelInstance.ReportStyleName);
                //Select the last made program
                PartProgramPopUp.lstReportStyles.SelectedItem = GlobalSettings.MainExcelInstance.ReportStyleName;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV93", ex); }
        }
        void UpdateAllReportNames()
        {
            try
            {
                GlobalSettings.AllExistingPartProgramNames = GlobalSettings.MainExcelInstance.GetAllEntries("ReportInformation", "R_ReportName", "Report_Id"); // .Clear();
                string tRptName;
                for (int i = 0; i < GlobalSettings.AllExistingPartProgramNames.Count; i++)
                {
                    tRptName = GlobalSettings.AllExistingPartProgramNames[i];
                    GlobalSettings.AllExistingPartProgramNames[i] = tRptName.Substring(tRptName.LastIndexOf('\\') + 1);
                }

                //for (int i = 0; i < PartProgramPopUp.dgvPrograms.Items.Count; i++)
                //{
                //    System.Data.DataRow dr = ((System.Data.DataRowView)PartProgramPopUp.dgvPrograms.Items[i]).Row;
                //    System.Collections.ArrayList ReportStyles = GlobalSettings.LoadExcelInstance.GetAllreportStyles(GlobalSettings.PartProgramPaths[i] + dr[0].ToString());
                //    for (int j = 0; j < ReportStyles.Count; j++)
                //    {
                //        System.Collections.ArrayList Reports = GlobalSettings.LoadExcelInstance.GetAllReportNames(ReportStyles[j].ToString());
                //        foreach (string RptName in Reports)
                //        {
                //            string TmpRptName = RptName.Substring(RptName.LastIndexOf('\\') + 1);
                //            if (!GlobalSettings.AllExistingPartProgramNames.Contains(TmpRptName))
                //                GlobalSettings.AllExistingPartProgramNames.Add(TmpRptName);
                //        }
                //    }
                //}
                
                this.Dispatcher.BeginInvoke(new AsyncSimpleDelegate(PartProgramBuildWin.ResetAllReportCBx), System.Windows.Threading.DispatcherPriority.Normal);
                //PartProgramBuildWin.ResetAllReportCBx(); // AllExistingProgramCBx.Items.Clear();
                //PartProgramBuildWin.AllExistingProgramCBx.SelectedIndex = -1;
                //PartProgramBuildWin.AllExistingProgramCBx.ItemsSource = GlobalSettings.AllExistingPartProgramNames;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV90", ex); }
        }
        void dgvProgramSteps_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                if (PartProgramPopUp.dgvProgramSteps.SelectedItem == null)
                {
                    RWrapper.RW_MainInterface.MYINSTANCE().setActionIdForPPEdit(0, false);
                }
                else
                {
                    int StpIndx = int.Parse(((System.Data.DataRowView)PartProgramPopUp.dgvProgramSteps.SelectedItem)[6].ToString());
                    RWrapper.RW_MainInterface.MYINSTANCE().setActionIdForPPEdit(StpIndx, true);
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV90", ex); }
        }
        void MainView_NudgeRotatePPevent()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    PPNudgeRotateWin PPNudRotWin = new PPNudgeRotateWin();
                    if (ProgramRunning)
                    {
                        PPToolbarGrid.IsEnabled = false;
                        ShapeParameterWin.IsEnabled = true;
                        ShapeParameterWin.ButtonsPanel.IsEnabled = false;
                        ShapeParameterWin.ShapePointsDG.IsEnabled = false;
                        VideoContextMenu.IsEnabled = false;
                        RcadContextMenu.IsEnabled = false;
                    }
                    else
                    {
                        PPNudRotWin.ContinuePPBtn.Visibility = System.Windows.Visibility.Hidden;
                    }

                    PPNudRotWin.Top = RcadTop;
                    PPNudRotWin.Left = RcadLeft + DxfWin.Width - PPNudRotWin.Width;
                    PPNudRotWin.Owner = RcadWin;
                    PPNudRotWin.Show();
                    PPNudRotWin.Activate();
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(MainView_NudgeRotatePPevent));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV90a", ex); }
        }
        void PP_ParamResetEvent(RWrapper.RW_Enum.PROGRAM_PARAMETER ParmType)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    switch (ParmType)
                    {
                        case RWrapper.RW_Enum.PROGRAM_PARAMETER.DEVIATION:
                            AssignShapePanelToShapeInfoWindow(2, "Enter Upper Tolerance;Enter Lower Tolerance", "PPParam_Deviation", true);
                            break;
                        case RWrapper.RW_Enum.PROGRAM_PARAMETER.RELATIVE_PTOFFSET:
                            AssignShapePanelToShapeInfoWindow(3, "X Offset;Y Offset;Z Offset", "PPParam_PTOffset", true);
                            break;
                        case RWrapper.RW_Enum.PROGRAM_PARAMETER.DEVIATION_RELATIVE_PT:
                            AssignShapePanelToShapeInfoWindow(5, "Enter Upper Tolerance;Enter Lower Tolerance;X Offset;Y Offset;Z Offset", "PPParam_Deviation&PTOffset", true);
                            break;
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_PartProgram.PP_ParameterResetEventHandler(PP_ParamResetEvent), ParmType);
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV90a", ex); }
        }
        public void OnContinueAfterPPNudgeRotate()
        {
            try
            {
                PPToolbarGrid.IsEnabled = true;
                ShapeParameterWin.IsEnabled = false;
                ShapeParameterWin.ButtonsPanel.IsEnabled = true;
                ShapeParameterWin.ShapePointsDG.IsEnabled = true;
                VideoContextMenu.IsEnabled = true;
                RcadContextMenu.IsEnabled = true;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV90a", ex); }
        }
        //To add new report for today
        void CheckAndAddnewReportForToday()
        {
            string RptName = PartProgramPopUp.lstReports.Items[PartProgramPopUp.lstReports.Items.Count - 1].ToString();
            string NowDate = DateTime.Now.Date.Day + "_" + DateTime.Now.Date.Month + "_" + DateTime.Now.Date.Year;
            bool AddNewRpt = false;
            if (RptName.Contains("_(") && RptName.Contains(")") && RptName.Contains("20"))
            {
                string RptDate = RptName.Substring(RptName.LastIndexOf("(") + 1);
                RptDate = RptDate.Substring(0, RptDate.LastIndexOf(")"));
                if (NowDate != RptDate)
                {
                    RptName = RptName.Substring(0, RptName.LastIndexOf("_("));
                    AddNewRpt = true;
                }
            }
            else
            {
                RptName = RptName.Substring(0, RptName.LastIndexOf("."));
                AddNewRpt = true;
            }

            if (AddNewRpt)
            {
                string NewRpt = RptName + "_(" + NowDate + ").xls";
                string prog = GlobalSettings.PartProgramPaths[PartProgramPopUp.SelectedProgIndex] + ((System.Data.DataRowView)PartProgramPopUp.dgvPrograms.Items[PartProgramPopUp.SelectedProgIndex]).Row[0].ToString();
                string repoSty = PartProgramPopUp.lstReportStyles.SelectedItem.ToString();
                GlobalSettings.LoadExcelInstance.InsertNewReport(prog, repoSty, NewRpt, TitleValuesChanged);
                //PartProgramPopUp.lstReports.Items.Add(PartProgramPopUp.txtNewReport.Text);
            }
        }
        #endregion

        #region Cursors changes

        int LastVideoCursor = 0, LastRcadCursor = 0, LastDxfCursor = 0;
        //Handling framwwork event for cursor change
        void MainView_RapidCursorChanged(RWrapper.RW_Enum.RW_WINDOWTYPE WinType, RWrapper.RW_Enum.RW_CURSORTYPE CurType)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    switch (WinType)
                    {
                        case RWrapper.RW_Enum.RW_WINDOWTYPE.VIDEO_WINDOW:
                            //VideoFormsWin.Cursor = (System.Windows.Forms.Cursor)GlobalSettings.FormsCursors[(int)CurType];
                            VideoHolder.VideoHost.SetCursor((System.Windows.Forms.Cursor)GlobalSettings.FormsCursors[(int)CurType]);
                            LastVideoCursor = (int)CurType;
                            break;
                        case RWrapper.RW_Enum.RW_WINDOWTYPE.RCAD_WINDOW:
                            RcadWin.Cursor = (Cursor)GlobalSettings.WindowCursors[(int)CurType];
                            LastRcadCursor = (int)CurType;
                            break;
                        case RWrapper.RW_Enum.RW_WINDOWTYPE.DXF_WINDOW:
                            DxfWin.Cursor = (Cursor)GlobalSettings.WindowCursors[(int)CurType];
                            LastDxfCursor = (int)CurType;
                            break;
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_UCSParameter.CursorTypeEventHandler(MainView_RapidCursorChanged), WinType, CurType);
                }
            }
            catch (Exception ex) { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV94", ex); }
        }
        //Handling show/hide of wait cur
        void MainView_WaitCursor(int ShowHide)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    if (ShowHide == 1)
                    {
                        this.Cursor = Cursors.Wait;
                        //VideoFormsWin.Cursor = System.Windows.Forms.Cursors.WaitCursor;
                        VideoHolder.Cursor = System.Windows.Input.Cursors.Wait;
                        RcadWin.Cursor = Cursors.Wait;
                        DxfWin.Cursor = Cursors.Wait;
                    }
                    else
                    {
                        this.Cursor = Cursors.Arrow;
                        //VideoFormsWin.Cursor = (System.Windows.Forms.Cursor)GlobalSettings.FormsCursors[LastVideoCursor];
                        VideoHolder.VideoHost.SetCursor((System.Windows.Forms.Cursor)GlobalSettings.FormsCursors[LastVideoCursor]);
                        RcadWin.Cursor = (Cursor)GlobalSettings.WindowCursors[LastRcadCursor];
                        DxfWin.Cursor = (Cursor)GlobalSettings.WindowCursors[LastDxfCursor];
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.ShowHideWaitCursrEventHandler(MainView_WaitCursor), ShowHide);
                }
            }
            catch (Exception ex) { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV94", ex); }
        }

        #endregion

        #region Clear all functions

        //Handling the command to clear everything.
        void HandleClearEverything(int ClearStatus)
        {
            //UpdatingDRO = false;
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    if (ClearStatus == 0)//Clear everything
                    {
                        ClearEverything(false);
                        PartProgramPopUp.dgvPrograms.UnselectAllCells();
                        PartProgramPopUp.CurrentLoadedProgLbl.Visibility = System.Windows.Visibility.Hidden;
                        GlobalSettings.PartProgramLoaded = false;
                        PartProgramPopUp.lstReportStyles.Items.Clear();
                        PartProgramPopUp.lstReports.Items.Clear();
                        PartProgramPopUp.SelectedProgIndex = -1;
                        lightingPopup.btnPPLightingControl.Visibility = Visibility.Hidden;
                        GlobalSettings.MainExcelInstance.UseExistingReport = false;
                        GlobalSettings.MainExcelInstance.ToleranceFormat = 0;
                    }
                    else if (ClearStatus == 1)//Part-Program Load is done by user.
                    {
                        ClearEverything(true);
                        GlobalSettings.MainExcelInstance.ToleranceFormat = 0;
                        //PartProgramPopUp.dgvPrograms.UnselectAllCells();
                    }
                    else if (ClearStatus == 2)//part-program load is cancelled by user
                    {
                        PartProgramPopUp.dgvPrograms.UnselectAllCells();
                        PartProgramPopUp.CurrentLoadedProgLbl.Visibility = System.Windows.Visibility.Hidden;
                        GlobalSettings.PartProgramLoaded = false;
                        PartProgramPopUp.lstReportStyles.Items.Clear();
                        PartProgramPopUp.lstReports.Items.Clear();
                        PartProgramPopUp.SelectedProgIndex = -1;
                        lightingPopup.btnPPLightingControl.Visibility = Visibility.Hidden;
                        GlobalSettings.MainExcelInstance.ToleranceFormat = 0;
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.ClearAllEventHandler(HandleClearEverything), ClearStatus);
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0082", ex); }
        }
        void ClearEverything(bool Clear_for_PP)
        {
            //UpdatingDRO = false;
            try
            {
                //if (GlobalSettings.CurrentUserLevel != GlobalSettings.UserPrevilageLevel.None)
                //{
                //    Button TmpLOBtn = (Button)ToolsScrollList.Get_Button("Logout");
                //    ToolsScrollBarButtons_Click(TmpLOBtn, null);
                //}
                ClearToolbarSelection();
                //ToggleButton TBtn = null;
                //try
                //{
                //    TBtn = St.Children.OfType<ToggleButton>()
                //        .Where(T => (((string[])T.Tag)[2]) == "Reference Sphere").First();
                //}
                //catch (Exception ex)
                //{
                //}
                //if (TBtn != null)
                //{
                //    TBtn.IsChecked = false;
                //}
                //MessageBox.Show("mv1");
                ShapeParameterWin.ClearAll();
                MeasurementsPopup.ClearAll();
                ActionsPopup.ClearAll();
                lightingPopup.ClearAll();
                DXFListHashTable.Clear();
                UCSList.lstDropDownItems.Items.Clear();
                //MessageBox.Show("mv2");
                if (GlobalSettings.InchModeAsDefault)
                    mmInchesList.lstDropDownItems.SelectedItem = "In";
                else
                    mmInchesList.lstDropDownItems.SelectedItem = "mm";
                UCSHashTable.Clear();
                PictureViewerWin.ClearAll();
                PathInterpolationWindow.ClearAll();
                this.Cursor = Cursors.Arrow;
                //VideoFormsWin.Cursor = System.Windows.Forms.Cursors.Arrow;
                RcadWin.Cursor = Cursors.Arrow;
                DxfWin.Cursor = Cursors.Arrow;
                //MessageBox.Show("mv3");

                GlobalSettings.CurrentAngleMode = GlobalSettings.AngleMode.Degree_Minute_Second;
                TransformSelectedPtsOnly = false;

                PartProgramPopUp.dgvProgramSteps.ItemsSource = null;

                //Get the instance of the drop down for the Select DXF buttton
                DXFToolbar.Children.OfType<ScrToolbar>().First().Children.OfType<DropDownList>()
                                        .First().lstDropDownItems.Items.Clear();
                //HIde if Dxf Nudge & rotat steps window is still visible
                DxfNudgeRotateStepWin.Hide();
                //Clear all focus values 
                FocusPopUp.ClearAll();
                //MessageBox.Show("mv4");

                TransparencyWindow.ClearAll();

                ToolbarSetInitialChkStatus();
                ClearAll_AllToolbars();

                IEnumerable<ToggleButton> PPOptions = stOptions.Children.OfType<ToggleButton>().Where(T => ((string[])T.Tag)[3] == "True");
                foreach (ToggleButton tog in PPOptions)
                {
                    tog.IsChecked = true;
                    string[] tagstr = (string[])tog.Tag;
                    RWrapper.RW_MainInterface.MYINSTANCE().HandleCheckedProperty_Load(tagstr[0], tagstr[2]);
                }
                //MessageBox.Show("mv5");

                try
                {
                    RapidDropDownControl Rdd = (RapidDropDownControl)ToolsScrollList.Children.OfType<RapidDropDownControl>().First();
                    if (Rdd != null)
                        ClearRDDMaterialControl(Rdd, null);
                }
                catch (Exception)
                {
                }

                //MessageBox.Show("mv6");

                if ((((ButtonProperties)RcadToolbar.btnMaxMin.Tag).ButtonName) == "Minimize")
                    RcadScrollBarButtons_Click(RcadToolbar.btnMaxMin, null);
                //UpperDropDownContentsButtons_Click(RcadToolbar.btnMaxMin, null);
                else if ((((ButtonProperties)DXFToolbar.btnMaxMin.Tag).ButtonName) == "Minimize")
                    DxfScrollBarButtons_Click(DXFToolbar.btnMaxMin, null);
                //UpperDropDownContentsButtons_Click(DXFToolbar.btnMaxMin, null);

                MenuItem M = (MenuItem)VideoContextMenu.Items.OfType<MenuItem>().Where
                                        (T => ((string[])T.Tag)[1] == "FG Points Visible" || ((string[])T.Tag)[1] == "FG Points Hidden").First();
                M.Icon = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("FG Points Visible", "FG Points Visible", 24, 24);
                M.Header = GlobalSettings.SettingsReader.ButtonToolTip;
                M.Tag = new string[] { "RightClickOnShape", "FG Points Visible" };

                ThreadPopup.dgvThreadProperties.ItemsSource = null;
                ThreadPopup.btnNew.IsEnabled = true;
                ThreadPopup.btnGetResult.IsEnabled = false;
                ThreadPopup.txtThreadCount.IsEnabled = false;
                //ThreadPopup.rdbIncludeMajMinDia.IsEnabled = true;

                FixedGraphicsPopup.ClearAll();
                //MessageBox.Show("mv7");

                PCDPopup.cbAdd.Items.Clear();
                PCDPopup.cbRemove.Items.Clear();

                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
                    if (RRecordPanel != null)
                        RRecordPanel.dgvValues.ItemsSource = null;

                if (!Clear_for_PP)
                {
                    TextBox[] cnctextbox = { CNCPopUp.txtgotodroX, CNCPopUp.txtgotodroY, CNCPopUp.txtgotodroZ, CNCPopUp.txtmmX, CNCPopUp.txtmmY, CNCPopUp.txtmmZ };
                    for (int i = 0; i < cnctextbox.Length; i++)
                        cnctextbox[i].Text = "";
                    cnctextbox = null;
                }
                //MessageBox.Show("mv8");

                if (GlobalSettings.SaveHomePosition)
                {
                    CNCPopUp.txtgotodroX.Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[0]);
                    CNCPopUp.txtgotodroY.Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[1]);
                    CNCPopUp.txtgotodroZ.Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[2]);
                    CNCPopUp.txtgotodroR.Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[3] * (180 / Math.PI));
                }
                if (ScanStarted)
                {
                    ScanStarted = false;
                    RWrapper.RW_MainInterface.MYINSTANCE().Start_StopRotationalScan(false);
                }
                //MessageBox.Show("mv9");
                RWrapper.RW_MainInterface.MYINSTANCE().SetContinuousScanMode(false);
                ScanOnOffBtn.Visibility = Visibility.Hidden;
                GlobalSettings.ZeroingAngle = 0;
                if (VBlockPopup != null)
                    VBlockPopup.ClearAll();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0083", ex); }
        }
        void HandleClearAllRadioButton()
        {
            //UpdatingDRO = false;
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    ClearToolbarSelection(1);
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(HandleClearAllRadioButton));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0084", ex); }
        }

        #endregion

        #region Rcad/Dxf maximise and minimise preparations

        //Function to handle the Rcad  & DXF Maximizing & Swapping
        private void ChangeRcadDxfWidthHeight_Positioning(ref OpenGlWin win, ref ToolbarGrid winToolbar, double left, double top,
                                               double width, double height, int windowNo, bool FirstStorePositions)
        {
            try
            {
                if (FirstStorePositions)
                {
                    RcadDxfPositionings[0] = win.Left;
                    RcadDxfPositionings[1] = win.Top;
                    RcadDxfPositionings[2] = win.Width;
                    RcadDxfPositionings[3] = win.Height;
                    VideoHolder.Visibility = Visibility.Hidden;
                    if (win == RcadWin)
                        DxfWin.Visibility = Visibility.Hidden;
                    else
                        RcadWin.Visibility = Visibility.Hidden;
                }
                else
                {
                    RcadWin.Visibility = Visibility.Visible;
                    //DxfWin.Visibility = Visibility.Visible;
                    VideoHolder.Visibility = Visibility.Visible;
                }
                win.Left = left;
                win.Top = top;
                win.Height = (int)height;
                win.Width = (int)width;
                //wwinToolbar.Width = win.Width;
                //RWrapper.RW_MainInterface.MYINSTANCE().MaximizeWindow(windowNo, (int)win.Width, (int)win.Height);
                RWrapper.RW_MainInterface.MYINSTANCE().ResizetheWindow(windowNo, (int)win.Width, (int)win.Height);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0113", ex); }
        }

        void PrepareForRcadMaximise(ref double[] WinPosArray)
        {
            try
            {   //Hide the HobChecker Control Window
                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HobChecker)
                {
                    //hobWindow.Visibility = System.Windows.Visibility.Hidden;
                    DxfWin.Visibility = System.Windows.Visibility.Hidden;
                }

                if (OtherWinToolbarRotated)
                {
                    ToolBoxHolder.Owner = null;
                    ToolBoxHolder.Owner = this; // RcadWin;
                    PPToolBoxHolder.Owner = null;
                    PPToolBoxHolder.Owner = this; // RcadWin;

                    PrepareForRcadDxfSmallScreenMaxmise(ref WinPosArray);
                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge)
                    {
                        ToolbarBorder_AboveVideo.Child = null;
                        ToolbarBorder_AboveVideo.Child = RcadToolbar;
                    }
                    else
                    {
                        RcadToolbarBorder.Child = null;
                        ToolbarBorder.Child = RcadToolbar;

                    }
                    DxfWin.Hide();
                }
                else
                {
                    //DxfWin.Hide();
                    PrepareForRcadDxfMaximise(ref WinPosArray);
                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge)
                    {
                        ProgramRBtn.IsChecked = false;
                        PicViewerRBtn.IsChecked = false;
                        ToolbarBorder_AboveVideo.Child = null;
                        ToolbarBorder.Child = RcadToolbar;
                    }
                    ProgramRBtn.IsEnabled = false;
                    PicViewerRBtn.IsEnabled = false;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:127", ex);
            }
        }
        void PrepareForDxfMaximise(ref double[] WinPosArray)
        {
            try
            {  
                if (OtherWinToolbarRotated)
                {
                    ToolBoxHolder.Owner = null;
                    ToolBoxHolder.Owner = DxfWin;
                    PPToolBoxHolder.Owner = null;
                    PPToolBoxHolder.Owner = DxfWin;

                    PrepareForRcadDxfSmallScreenMaxmise(ref WinPosArray);
                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge)
                        RcadToolbarBorder.Child = null;
                    else
                        DxfToolbarBorder.Child = null;
                    ToolbarBorder.Child = DXFToolbar;
                    //RcadWin.Hide();
                }
                else
                {
                    if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.ProbeOnly && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HeightGauge)
                        RapidCADRBtn.IsChecked = false;
                    ProgramRBtn.IsChecked = false;
                    PicViewerRBtn.IsChecked = false;
                    RapidCADRBtn.IsEnabled = false;
                    ProgramRBtn.IsEnabled = false;
                    PicViewerRBtn.IsEnabled = false;
                    //RcadWin.Hide();
                    DxfLowerToolbarBorder.Child = null;
                    ToolbarBorder.Child = DXFToolbar;
                    PrepareForRcadDxfMaximise(ref WinPosArray);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:128", ex);
            }
        }
        void PrepareForRcadDxfSmallScreenMaxmise(ref double[] WinPosArray)
        {
            try
            {
                if ((bool)ProgramRBtn.IsChecked) ProgramRBtn.IsChecked = false;
                if ((bool)PicViewerRBtn.IsChecked) PicViewerRBtn.IsChecked = false;
                if ((bool)ShpsNMeasurementsTBtn.IsChecked) ShpsNMeasurementsTBtn.IsChecked = false;
                ProgramRBtn.IsEnabled = false;
                PicViewerRBtn.IsEnabled = false;
                ShpsNMeasurementsTBtn.IsEnabled = false;
                Tools_Grid.Visibility = System.Windows.Visibility.Hidden;
                TxtHelperGrid.Visibility = System.Windows.Visibility.Hidden;
                //Video__Grid.Width = 808 + 360;
                //Video__Grid.Width = Videowidth + Tools_Grid.ActualWidth + 4; // 7;                                          //1 Written by Harphool
                //if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.ProbeOnly && 
                //    GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HeightGauge &&  
                //    GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HobChecker)
                    //VideoFormsWin.Hide();
                //Video__Grid.Background = new SolidColorBrush(Colors.Black);
                WinPosArray[0] = 34;   // 38                                                                          //2 Written by Harphool
                //WinPosArray[1] = VideoFormsWin.Top;
                WinPosArray[1] = VideoTop + 8; // 18;
                WinPosArray[2] = Videowidth + Tools_Grid.ActualWidth + 24;                                              //3 Written by Harphool
                WinPosArray[3] = Videoheight + TxtHelperGrid.Height - RcadToolbar.ActualHeight + 28;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:129", ex);
            }
        }
        void PrepareForRcadDxfMaximise(ref double[] WinPosArray)
        {
            try
            {
                Video__Grid.Visibility = System.Windows.Visibility.Hidden;
                //PPToolbarGrid.Visibility = System.Windows.Visibility.Hidden;
                //if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.ProbeOnly && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HeightGauge)
                //    VideoFormsWin.Hide();

                Point TmpPt1 = VideoDisplayGrpBx.PointToScreen(new Point(0, 0));
                Point TmpPt2 = RapidCADHolder.PointToScreen(new Point(0, 0));

                Grid.SetColumn(RapidCADGrid, 1);
                Grid.SetColumnSpan(RapidCADGrid, 2);
                Grid.SetRowSpan(RapidCADGrid, 2);
                RapidCADGrid.Margin = new Thickness(5, 8, 16, 16);
                Video__Grid.Children.Remove(DROPanel);
                Video__Grid.Children.Remove(DropDownPanel);
                if (Videoheight > GlobalSettings.VideoHeightThresholdForFrontEndLayout && !OtherWinToolbarRotated) //GlobalSettings.RapidCameraType != GlobalSettings.CameraType.TwoM
                    DropDownPanel.Margin = new Thickness(0, 0, 500, 0);
                else
                    DropDownPanel.Margin = new Thickness(0, 0, 686, 0);
                RapidCADGrid.Children.Add(DROPanel);
                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
                {
                    if (!GlobalSettings.DelfiRotaryZeroingFloatingWindowView)
                        RapidCADGrid.Children.Add(DropDownPanel);
                }
                else
                {
                    RapidCADGrid.Children.Add(DropDownPanel);
                }

                if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.One_Shot && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.ProbeOnly && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HeightGauge)
                {
                    LowerGrid2.Visibility = System.Windows.Visibility.Hidden;
                    LowerGrid3.Visibility = System.Windows.Visibility.Hidden;
                }
                LowerGrid4.Visibility = System.Windows.Visibility.Hidden;

                if (GlobalSettings.IsButtonWithNames)
                {
                    WinPosArray[1] = TmpPt1.Y + 46;//Earlier it was 67
                    if (this.ActualHeight < 1050)
                        WinPosArray[3] = this.ActualHeight - 16 - TmpPt1.Y - 4 - 50;
                    else
                        WinPosArray[3] = this.ActualHeight - 16 - TmpPt1.Y - 4 - 70;
                }
                else
                {
                    WinPosArray[1] = TmpPt1.Y + 46;
                    WinPosArray[3] = this.ActualHeight - 16 - TmpPt1.Y - 4 - 46;
                }
                WinPosArray[0] = TmpPt1.X + 4;
                WinPosArray[2] = TmpPt2.X + RapidCADHolder.ActualWidth - TmpPt1.X - 8;

                if (GlobalSettings.IsButtonWithNames)
                    WinPosArray[3] = WinPosArray[3] - 75;
                else
                    WinPosArray[3] = WinPosArray[3] - 53;
                //Newly added on 17-Aug-2013
                Grid.SetColumn(PPToolbarGrid, 1);
                Grid.SetColumnSpan(PPToolbarGrid, 2);
                Grid.SetRowSpan(PPToolbarGrid, 2);
                RememberPPToolbarGridPosition = PPToolbarGrid.Margin;
                PPToolbarGrid.VerticalAlignment = System.Windows.VerticalAlignment.Bottom;
                PPToolbarGrid.Margin = new System.Windows.Thickness(5, 0, 15, 20);
                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
                {
                    this.TxtHelperGrid.Visibility = System.Windows.Visibility.Visible;
                }

            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:130", ex);
            }
        }

        void PrepareForRcadMinimise()
        {
            try
            {   //Show the HobChecker Control Window
                //if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HobChecker)
                //{
                //    hobWindow.Visibility = System.Windows.Visibility.Visible;
                //    hobWindow.Left = VideoLeft; 
                //    hobWindow.Top = VideoTop + 24;
                //    DxfWin.Visibility = System.Windows.Visibility.Hidden;
                //}

                if (OtherWinToolbarRotated)
                {
                    ToolBoxHolder.Owner = null;
                    SetOwnerofWindow(ToolBoxHolder);
                    PPToolBoxHolder.Owner = null;
                    PPToolBoxHolder.Owner = DxfWin;

                    DxfWin.Show();
                    UpdateOnSeparateThread();
                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge)
                    {                                                                               //1 Written by Harphool
                        ToolbarBorder.Child = null;                                                 //2
                        ToolbarBorder_AboveVideo.Child = RcadToolbar;                               //3    
                    }                                                                               //4    
                    else                                                                            //5
                    {                                                                               //6
                        ToolbarBorder.Child = null;                                                 //7
                        RcadToolbarBorder.Child = RcadToolbar;                                      //8
                    }                                                                               //9
                    PrepareForRcadDxfSmallScreenMinimise();
                }
                else
                {
                    if ((bool)DXFTBtn.IsChecked)
                        DxfWin.Show();
                    UpdateOnSeparateThread();
                    PrepareForRcadDxfMinimise();
                    ProgramRBtn.IsEnabled = true;
                    PicViewerRBtn.IsEnabled = true;
                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge)
                    {
                        ToolbarBorder.Child = null;
                        ToolbarBorder_AboveVideo.Child = RcadToolbar;
                        ProgramRBtn.IsChecked = true;
                    }
                    ////Check which of the buttons is checked and highlight accordingly..
                    //ToggleButton[] Btns2 = { ShapesRBtn, MeasurementsRBtn, DXFTBtn, ActionsRBtn };
                    //int i = 0;
                    //for (i = 0; i < Btns2.Length; i++)
                    //{
                    //    if ((bool)Btns2[i].IsChecked) break;
                    //}
                    ////DXFTBtn.IsChecked = true;
                    //Btns2[i].IsChecked = true;
                    ////LowerGroupTab3TBtn_Checked(Btns2[i], null);

                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:131", ex);
            }
        }
        void PrepareForDxfMinimise()
        {
            try
            {   //Show the HobChecker Control Window
                //if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HobChecker)
                //{
                //    hobWindow.Visibility = System.Windows.Visibility.Visible;
                //    hobWindow.Left = VideoLeft; 
                //    hobWindow.Top = VideoTop;
                //}

                if (OtherWinToolbarRotated)
                {
                    ToolBoxHolder.Owner = null;
                    SetOwnerofWindow(ToolBoxHolder);
                    PPToolBoxHolder.Owner = null;
                    PPToolBoxHolder.Owner = DxfWin;

                    //RcadWin.Show();
                    UpdateOnSeparateThread();
                    ToolbarBorder.Child = null;
                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge)
                        RcadToolbarBorder.Child = DXFToolbar;
                    else
                        DxfToolbarBorder.Child = DXFToolbar;
                    PrepareForRcadDxfSmallScreenMinimise();
                }
                else
                {
                    //RcadWin.Show();
                    ToolbarBorder.Child = null;
                    DxfLowerToolbarBorder.Child = DXFToolbar;
                    PrepareForRcadDxfMinimise();
                    RapidCADRBtn.IsEnabled = true;
                    ProgramRBtn.IsEnabled = true;
                    PicViewerRBtn.IsEnabled = true;
                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge)
                        ProgramRBtn.IsChecked = true;
                    else
                        RapidCADRBtn.IsChecked = true;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:132", ex);
            }
        }
        void PrepareForRcadDxfSmallScreenMinimise()
        {
            try
            {
                ProgramRBtn.IsEnabled = true;
                PicViewerRBtn.IsEnabled = true;
                ShpsNMeasurementsTBtn.IsEnabled = true;
                Tools_Grid.Visibility = System.Windows.Visibility.Visible;
                TxtHelperGrid.Visibility = System.Windows.Visibility.Visible;
                //Video__Grid.Width = Videowidth + 4; // 16;                                                                    //1 Written by Harphhol
                //if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.ProbeOnly && 
                //    GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HeightGauge && 
                //    GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HobChecker)
                //    VideoFormsWin.Show();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:133", ex);
            }
        }
        void PrepareForRcadDxfMinimise()
        {
            try
            {
                Video__Grid.Visibility = System.Windows.Visibility.Visible;
                //PPToolbarGrid.Visibility = System.Windows.Visibility.Visible;
                //if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.ProbeOnly && 
                //    GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HeightGauge &&
                //    GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HobChecker)
                //    VideoFormsWin.Show();
                Grid.SetColumn(RapidCADGrid, 2);
                Grid.SetColumnSpan(RapidCADGrid, 1);
                Grid.SetRowSpan(RapidCADGrid, 1);
                RapidCADGrid.Margin = new Thickness(5, 8, 16, 250);
                RapidCADGrid.Children.Remove(DROPanel);
                RapidCADGrid.Children.Remove(DropDownPanel);
                DropDownPanel.Margin = new Thickness(0, 0, 5, 0);
                if (!Video__Grid.Children.Contains(DROPanel))
                    Video__Grid.Children.Add(DROPanel);
                if (!Video__Grid.Children.Contains(DropDownPanel))
                {
                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
                    {
                        if (!GlobalSettings.DelfiRotaryZeroingFloatingWindowView)
                            Video__Grid.Children.Add(DropDownPanel);
                    }
                    else
                    {
                        Video__Grid.Children.Add(DropDownPanel);
                    }
                }

                if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.One_Shot && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.ProbeOnly && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HeightGauge)
                {
                    LowerGrid2.Visibility = System.Windows.Visibility.Visible;
                    LowerGrid3.Visibility = System.Windows.Visibility.Visible;
                }
                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
                {
                    this.Video__Grid.RowDefinitions[0].Height = new GridLength(70);
                }
                LowerGrid4.Visibility = System.Windows.Visibility.Visible;

                Grid.SetColumn(PPToolbarGrid, 2);
                PPToolbarGrid.HorizontalAlignment = HorizontalAlignment.Right; PPToolbarGrid.VerticalAlignment = VerticalAlignment.Top;
                //PPToolbarGrid.Margin = new Thickness(0, RapidCADGrid.Margin.Top + RapidCADGrid.ActualHeight + 8, 16, 0); 
                Grid.SetColumnSpan(PPToolbarGrid, 1);
                Grid.SetRowSpan(PPToolbarGrid, 1);
                PPToolbarGrid.Margin = new System.Windows.Thickness(RememberPPToolbarGridPosition.Left, RememberPPToolbarGridPosition.Top, RememberPPToolbarGridPosition.Right, RememberPPToolbarGridPosition.Bottom);
                
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0134", ex);
            }

        }

        #endregion

        #region Messagebox and Status Messages

        bool MainView_ShowMsgBoxText(string Code, RWrapper.RW_Enum.RW_MSGBOXTYPE MBxType, RWrapper.RW_Enum.RW_MSGBOXICONTYPE MBxIcoType, bool Insert_CustomVal, string Custom_Val)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    string StatusMsgSplitter = "##$$";
                    string MsgStr = GlobalSettings.SettingsReader.Get_MsgBx_String(Code);
                    System.Windows.Forms.DialogResult Result = System.Windows.Forms.DialogResult.None;
                    if (Insert_CustomVal)
                    {
                        string[] Cust_Vals = Custom_Val.Split('@');
                        for (int i = 0; i < Cust_Vals.Length; i++)
                        {
                            int Indx = MsgStr.IndexOf(StatusMsgSplitter);
                            if (Indx == -1) break;
                            MsgStr = MsgStr.Substring(0, Indx) + Cust_Vals[i] + MsgStr.Substring(Indx + StatusMsgSplitter.Length);
                        }
                    }
                    switch (MBxType)
                    {
                        case RWrapper.RW_Enum.RW_MSGBOXTYPE.MSG_OK:
 
                                //Result = System.Windows.Forms.MessageBox.Show(MsgStr, "Rapid I", System.Windows.Forms.MessageBoxButtons.OK);
                            
                            Result = System.Windows.Forms.MessageBox.Show(MsgStr, "Rapid I", System.Windows.Forms.MessageBoxButtons.OK);

                          
                            break;
                        case RWrapper.RW_Enum.RW_MSGBOXTYPE.MSG_OK_CANCEL:
                            Result = System.Windows.Forms.MessageBox.Show(MsgStr, "Rapid I", System.Windows.Forms.MessageBoxButtons.OKCancel);
                            break;
                        case RWrapper.RW_Enum.RW_MSGBOXTYPE.MSG_YES_NO:
                            Result = System.Windows.Forms.MessageBox.Show(MsgStr, "Rapid I", System.Windows.Forms.MessageBoxButtons.YesNo);
                            break;
                        case RWrapper.RW_Enum.RW_MSGBOXTYPE.MSG_YES_NO_CANCEL:
                            Result = System.Windows.Forms.MessageBox.Show(MsgStr, "Rapid I", System.Windows.Forms.MessageBoxButtons.YesNoCancel);
                            break;
                    }
                    this.Focus();
                    switch (Result)
                    {
                        case System.Windows.Forms.DialogResult.Cancel:
                        case System.Windows.Forms.DialogResult.No:
                            return false;
                        case System.Windows.Forms.DialogResult.None:
                        case System.Windows.Forms.DialogResult.OK:
                        case System.Windows.Forms.DialogResult.Yes:
                            return true;
                    }
                    return true;
                }
                else
                {
                    return (bool)this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.MsgBoxEventHandler(MainView_ShowMsgBoxText), Code, MBxType, MBxIcoType, Insert_CustomVal, Custom_Val);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:105", ex);
                return true;
            }
        }
        void MainView_DisplayHelpTextMessage(string Code, bool Insert_CustomVal, string Custom_Val)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    string StatusMsgSplitter = "##$$";
                    if (GlobalSettings.SettingsReader == null) MessageBox.Show(Code);
                    string MsgStr = GlobalSettings.SettingsReader.Get_StatusMsg_String(Code);
                    //MessageBox.Show(Code);
                    if (Insert_CustomVal)
                    {
                        string[] Cust_Vals = Custom_Val.Split('@');
                        for (int i = 0; i < Cust_Vals.Length; i++)
                        {
                            int Indx = MsgStr.IndexOf(StatusMsgSplitter);
                            if (Indx == -1) break;
                            MsgStr = MsgStr.Substring(0, Indx) + Cust_Vals[i] + MsgStr.Substring(Indx + StatusMsgSplitter.Length);
                        }
                    }
                    TxtHelper.Text = MsgStr;
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.HelpTextStatusEventHandler(MainView_DisplayHelpTextMessage), Code, Insert_CustomVal, Custom_Val);
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Got Error Condition and message is ... " + ex.Message);
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:106", ex);
            }
        }

        void MainView_GridRunCountEvent(string Runcount)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    TxtGridRunCount.Visibility = System.Windows.Visibility.Visible;
                    TxtGridRunCount.Text = Runcount;
                    if (TxtGridRunCount.Text == "Done")
                    {
                        //for (int i = 0; i < St.Children.Count; i++)
                        //{
                        //    ContentControl c = (ContentControl)St.Children[i];
                        //    if (c.ToolTip.ToString() == "Run" || c.ToolTip.ToString() == "Pause" || c.ToolTip.ToString() == "Continue")
                        //    {
                        //        //c.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32);
                        //        //c.ToolTip = "Run";
                        //        St.AssignSingleBtnProp(c, "Run", "Run", GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32));
                        //        ((string[])(c.Tag))[2] = "Run";
                        //        break;
                        //    }
                        //}
                        for (int i = 0; i < St.Children.Count; i++)
                        {
                            if (St.Children[i].GetType() != typeof(Image))
                            {
                                string cBtnName;
                                ContentControl c = (ContentControl)St.Children[i];
                                if (c.Tag.GetType() == typeof(string[]))
                                {
                                    cBtnName = ((string[])c.Tag)[2];
                                    if (c.ToolTip.ToString() == "Run" || c.ToolTip.ToString() == "Pause" || c.ToolTip.ToString() == "Continue")
                                    {
                                        St.AssignSingleBtnProp(c, "Run", "Run", GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32));
                                        ((string[])(c.Tag))[2] = "Run";
                                        break;
                                    }
                                }
                                else
                                {
                                    cBtnName = ((ButtonProperties)c.Tag).ButtonName;
                                    switch (cBtnName)
                                    {
                                        case "Run":
                                        case "Pause":
                                        case "Continue":
                                            St.AssignSingleBtnProp(c, "Run", "Run", GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32));
                                            c.ToolTip = "Run";
                                            break;
                                    }
                                }
                            }
                        }
                        EnableDisableControlsDuringProgramRun(true);
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_PartProgram.PPRunCount(MainView_GridRunCountEvent), Runcount);
                    //RWrapper.RW_MainInterface.HelpTextStatusEventHandler(MainView_DisplayHelpTextMessage), Runcount);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:106", ex);
            }
        }
        #endregion

        #region Excel related events

        void MainView_MeasureDeviationEvent(List<List<double>> DataToTransfer, System.Collections.ArrayList OtherProps)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    System.Windows.Forms.SaveFileDialog sd = new System.Windows.Forms.SaveFileDialog();
                    //sd.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.Desktop);
                    sd.Filter = "(*.xls)Excel Files|*.xls";
                    sd.Title = "Save as";
                    sd.DefaultExt = ".xls";
                    if (sd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                    {
                        GlobalSettings.MainExcelInstance.DeviationReporting(sd.FileName, DataToTransfer, OtherProps);
                        if (File.Exists(sd.FileName))
                            System.Diagnostics.Process.Start(sd.FileName);
                        else
                            MessageBox.Show("The report file does not exist. It may not have been created properly.", "Rapid-I 5.0");
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.DeviationEventHandler(MainView_MeasureDeviationEvent), DataToTransfer, OtherProps);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV94", ex);
            }
        }

        #endregion

        void ShowFileInPicViewer(string FlName)
        {
            try
            {
                PictureViewerWin.ShowImageFile(FlName);
                if (ProgramLoading) return;
                if (PicViewerRBtn.Visibility == System.Windows.Visibility.Visible)
                    PicViewerRBtn.IsChecked = true;
            }
            catch (Exception ex) { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV95", ex); }
        }
        void VideoWin_PreviewMouseRightButtonUp(object sender, MouseButtonEventArgs e)
        {
            try
            {
                ChooseAlgoContextMenu.Visibility = System.Windows.Visibility.Hidden;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV96", ex); }
        }
        //handling the Rotary Panel datagrid view's selected Angle record delete click.
        void HandleRRecordPanel_RowDeleteClicked(object sender, RoutedEventArgs e)
        {
            try
            {
                //Check the login status.If admin allow to delete otherwise ask for login priviledges.
                if (UserLevelTxt.Text != "Admin")
                {
                    if (MessageBox.Show("Please login as Admin to delete the value.\nClick Ok to proceed further.", "Rapid-I 5.0",
                                                              MessageBoxButton.OKCancel, MessageBoxImage.Information) == MessageBoxResult.OK)
                    {
                        ShowSettingsWindowAfterLogin = false;
                        Simulate_ToolbarBtnClick("Tools", "Login", false);
                        if (UserLevelTxt.Text == "Admin") goto ProceedToDelete; else return;
                    }
                    else return;
                }
            ProceedToDelete:
                ShowSettingsWindowAfterLogin = true;
                RWrapper.RW_MeasureParameter.MYINSTANCE().DeleteSelectedMesurement();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV97", ex); }
        }
        void dgvValues_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            try
            {
                System.Collections.IList coll = RRecordPanel.dgvValues.SelectedItems;
                List<int> PointIdList = new List<int>();
                for (int i = 0; i < coll.Count; i++)
                {
                    System.Data.DataRowView dr = (System.Data.DataRowView)coll[i];
                    int a = 0;
                    if (int.TryParse(dr.Row[3].ToString(), out a))
                        PointIdList.Add(a);
                }
                RWrapper.RW_MeasureParameter.MYINSTANCE().SelectMeasurement(PointIdList.ToArray());
                PointIdList = null;

            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV98", ex);
            }

        }
        void dgvValues_AutoGeneratedColumns(object sender, EventArgs e)
        {
            try
            {
                if (RRecordPanel.dgvValues.Columns.Count >= 0)
                    RRecordPanel.dgvValues.Columns[RRecordPanel.dgvValues.Columns.Count - 1].Visibility = Visibility.Collapsed;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV99", ex); }
        }

        //*************************************************************

        //Handling the Upper DropDownsList  Selection Changed
        void UpperDropDwonList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                //if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || ) 
                ListBox ls = (ListBox)sender;
                Popup pp = null;
                //ls.SelectedItem
                ls.ScrollIntoView(ls.SelectedItem);
                if (ls.Name == "MagLevels")
                {
                    pp = (Popup)((Border)((Grid)((ls).Parent)).Parent).Parent;
                    RWrapper.RW_MainInterface.MYINSTANCE().SetMagnification(ls.SelectedItem.ToString());
                    MagnificationList.btnTg.Content = ls.SelectedItem.ToString();
                }
                else if (ls.Name == "MMInches")
                {
                    pp = (Popup)((Border)((ls).Parent)).Parent;
                    string str = ls.SelectedItem.ToString();
                    if (str == "mm")
                    {
                        if (GlobalSettings.is_startup_rapidi!=0)        
                            GlobalSettings.CurrentDroPrecision = GlobalSettings.CurrentDroPrecision_Ori;
                        GlobalSettings.is_startup_rapidi = 1;
                        GlobalSettings.CurrentUnitType = GlobalSettings.UnitType.mm;
                        RWrapper.RW_MeasureParameter.MYINSTANCE().SetMeasurementUnit(0);
                    }
                    else if (str == "In")
                    {
                        if (GlobalSettings.is_startup_rapidi != 0)        
                            GlobalSettings.CurrentDroPrecision = GlobalSettings.CurrentDroPrecision_Ori + 1;
                        GlobalSettings.is_startup_rapidi = 1;
                        GlobalSettings.CurrentUnitType = GlobalSettings.UnitType.Inch;
                        RWrapper.RW_MeasureParameter.MYINSTANCE().SetMeasurementUnit(1);
                    }
                }
                else if (ls.Name == "UCS")
                {
                    pp = (Popup)((Border)((ls).Parent)).Parent;
                    if (ls.SelectedIndex >= 0)
                    {
                        //if(ShouldCallSelectUcsInFramework)
                        int idd = (int)UCSHashTable[ls.SelectedItem.ToString()];
                        RWrapper.RW_UCSParameter.MYINSTANCE().SelectUCS((int)UCSHashTable[ls.SelectedItem.ToString()]);
                    }
                }
                if (pp != null)
                    pp.IsOpen = false;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0081", ex); }
        }

        //Handling measurement font size dialog box
        void HandleMeasurementFSizeDialog(int CurrFSize, string Tmp)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    FontWin.SetCurrentFontSize(CurrFSize);
                    Point currentMousePos = Mouse.GetPosition(this);
                    if (currentMousePos.X + FontWin.Width < this.Width)
                        FontWin.Left = currentMousePos.X;
                    else
                        FontWin.Left = currentMousePos.X - FontWin.Width;
                    if (currentMousePos.Y + FontWin.Height < this.Height)
                        FontWin.Top = currentMousePos.Y;
                    else
                        FontWin.Top = currentMousePos.Y - FontWin.Height;
                    FontWin.ShowDialog();
                    if (FontWin.Result == true)
                        RWrapper.RW_MeasureParameter.MYINSTANCE().Change_MeasurementFontSize((int)FontWin.FontSizeCBx.SelectedItem);
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RightClickOnEntityEventHandler(HandleMeasurementFSizeDialog), CurrFSize, Tmp);
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0084.1", ex); }
        }

        void MainView_ShowWindowForAutoFocus(bool ShowWin)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    if (ShowWin)
                    {
                        SetOwnerofWindow(AutoFocusScanWin.MyInstance());
                        AutoFocusScanWin.MyInstance().WinUsedForFocusScan = true;
                        AutoFocusScanWin.MyInstance().Show();
                    }
                    else
                    {
                        AutoFocusScanWin.MyInstance().NotifyClose = false;
                        AutoFocusScanWin.MyInstance().Close();
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.ShowAutoFocusWindow(MainView_ShowWindowForAutoFocus), ShowWin);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV85", ex);
            }
        }
        void MainView_ShowWindowForAutoContour(bool ShowWin)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    if (ShowWin)
                    {
                        SetOwnerofWindow(AutoFocusScanWin.MyInstance());
                        AutoFocusScanWin.MyInstance().WinUsedForFocusScan = false;
                        AutoFocusScanWin.MyInstance().Show();
                    }
                    else
                    {
                        AutoFocusScanWin.MyInstance().NotifyClose = false;
                        AutoFocusScanWin.MyInstance().Close();
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.ShowAutoFocusWindow(MainView_ShowWindowForAutoFocus), ShowWin);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV85.1", ex);
            }
        }
        void MainView_ShowRotateScanEvent(int Mode)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    ScanOnOffBtn.Visibility = Visibility.Visible;
                    if (!ScanStarted)
                    {
                        RWrapper.RW_MainInterface.MYINSTANCE().SetContinuousScanMode(!ScanStarted);
                    }
                    if (Mode == 1)
                    {
                        ScanStarted = true;
                        ScanOnOffBtn.Content = "Scan";
                        ScanOnOffBtn.ToolTip = "Scan";
                        ScanOnOffBtn.Tag = "Scan";
                        ModeIndicatorRectForScanOnOff.Fill = ScanOn;
                        EnableDisableControlsDuringScanMode(false);
                        RWrapper.RW_MainInterface.MYINSTANCE().Start_StopRotationalScan(true);
                    }
                    if (Mode == 3)
                    {
                        ScanStarted = false;
                        ScanOnOffBtn.Content = "Scan";
                        ScanOnOffBtn.ToolTip = "Scan";
                        ScanOnOffBtn.Tag = "Scan Stop";
                        ModeIndicatorRectForScanOnOff.Fill = ScanOff;
                        EnableDisableControlsDuringScanMode(false);
                        RWrapper.RW_MainInterface.MYINSTANCE().Start_StopRotationalScan(false);
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.ShowRotateScanEventHandler(MainView_ShowRotateScanEvent), Mode);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV85.2", ex);
            }
        }
        void MainView_ChangeZoomSelection(string Selection)
        {
            if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
            {
                try
                {
                    MagnificationList.lstDropDownItems.SelectedItem = Selection;
                }
                catch (Exception ex)
                {
                    RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MVChangeZS", ex);
                }
            }
            else
                this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                   new RWrapper.RW_PartProgram.ChangeZoom(MainView_ChangeZoomSelection), Selection);
        }

        //handling the text entered in Video window by selecting Text tool from Draw dropdown.
        void HandleEnterTextValue()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    ShapeRenameWin sh = new ShapeRenameWin();
                    sh.Owner = this;
                    sh.txtUpperHeading.Text = "Enter the desired text";
                    sh.ShowDialog();
                    if (sh.Result)
                    {
                        RWrapper.RW_ShapeParameter.MYINSTANCE().SetTextValue_TextShape(sh.txtNewName.Text);
                        sh.Hide();
                        sh.Close();
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(HandleEnterTextValue));
                }
            }
            catch (Exception ex) { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0086", ex); }
        }

        void NomTolWin_WinClosing(object sender, RoutedEventArgs e)
        {
            try
            {
                if (!GlobalSettings.NomTolWin.Result) { RWrapper.RW_MainInterface.MYINSTANCE().CancelPressed(); return; }
                RWrapper.RW_MeasureParameter.MYINSTANCE().SetMeasureNominalTolerance(GlobalSettings.NomTolWin.Nom, GlobalSettings.NomTolWin.UpTol, GlobalSettings.NomTolWin.LoTol);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0088.1", ex); }
        }
        void UserThreadEnteringWin_WinClosing(object sender, RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_MainInterface.MYINSTANCE().SetDerivedShapeParameters(UserThreadEnteringWin.NumOfThreads, 0, 0);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0088.2", ex); }
        }
        void MainView_EnterThreadCount()
        {
            try
            {
                UserThreadEnteringWin.ResetUpDown();
                UserThreadEnteringWin.ShowDialog();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0088.3", ex); }
        }

        //handling the change in the value of thread count.
        void txtThreadCount_ValueChanged(object sender, RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_Thread.MYINSTANCE().UpdateThreadCountChange((int)ThreadPopup.txtThreadCount.Value, (bool)ThreadPopup.TopSurfaceFlatChbk.IsChecked, (bool)ThreadPopup.RootSurfaceFlatChbk.IsChecked);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0092", ex); }
        }
        void MainView_ThreadMeasureSelectionChanged()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    ThreadPopup.dgvThreadProperties.ItemsSource = null;
                    ThreadPopup.dgvThreadProperties.SetValue(DataGrid.ItemsSourceProperty, RWrapper.RW_Thread.MYINSTANCE().ThreadMeasureTable.DefaultView);
                    ThreadPopup.txtThreadCount.Value = RWrapper.RW_Thread.MYINSTANCE().CurrentMeasureThCount;
                    //ThreadPopup.rdbIncludeMajMinDia.IsChecked = RWrapper.RW_Thread.MYINSTANCE().MajorMinorDiaMode;
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_Thread.ThreadEventHandler(MainView_ThreadMeasureSelectionChanged));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0093", ex); }
        }
        void MainView_PCDMeasureSelectionChanged()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    PCDPopup.cbAdd.Items.Clear();
                    PCDPopup.cbRemove.Items.Clear();
                    for (int i = 0; i < RWrapper.RW_PCD.MYINSTANCE().AddShapeMemberNameList.Count; i++)
                        PCDPopup.cbAdd.Items.Add(RWrapper.RW_PCD.MYINSTANCE().AddShapeMemberNameList[i]);
                    for (int i = 0; i < RWrapper.RW_PCD.MYINSTANCE().PCDMemberNameList.Count; i++)
                        PCDPopup.cbRemove.Items.Add(RWrapper.RW_PCD.MYINSTANCE().PCDMemberNameList[i]);
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_PCD.PCDEventHandler(MainView_PCDMeasureSelectionChanged));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0094", ex); }
        }
        void PCD_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                if (e.AddedItems.Count > 0)
                {
                    if (sender.Equals(PCDPopup.cbRemove))
                    {
                        if (PCDPopup.cbRemove.SelectedItem != null)
                        {
                            RWrapper.RW_PCD.MYINSTANCE().Circle_Remove(PCDPopup.cbRemove.SelectedItem.ToString());
                            PCDPopup.cbAdd.Items.Add(PCDPopup.cbRemove.SelectedItem);
                            PCDPopup.cbRemove.Items.Remove(PCDPopup.cbRemove.SelectedItem);
                        }
                    }
                    else
                    {
                        if (PCDPopup.cbAdd.SelectedItem != null)
                        {
                            RWrapper.RW_PCD.MYINSTANCE().Circle_Added(PCDPopup.cbAdd.SelectedItem.ToString());
                            PCDPopup.cbRemove.Items.Add(PCDPopup.cbAdd.SelectedItem);
                            PCDPopup.cbAdd.Items.Remove(PCDPopup.cbAdd.SelectedItem);
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0095", ex);
            }

        }
        void MainView_ResetButtonState()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    Button[] caliButtons = { CalibWin.btnHome, CalibWin.btnResetDRO, CalibWin.btnCalibrate, CalibWin.btnDone, CalibWin.btnClear };
                    for (int i = 0; i < caliButtons.Length; i++)
                        caliButtons[i].IsEnabled = true;
                    CalibWin.cbMachineType.IsEnabled = true; CalibWin.cbSlipGaugeWidth.IsEnabled = true;
                    CalibWin.btnRun.Content = "Run";
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_AutoCalibration.CalibrationEventHandler(MainView_ResetButtonState));
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:098", ex);
            }
        }
        void MainView_DisableButtonState()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    Button[] caliButtons = { CalibWin.btnHome, CalibWin.btnResetDRO, CalibWin.btnCalibrate, CalibWin.btnDone, CalibWin.btnClear };
                    for (int i = 0; i < caliButtons.Length; i++)
                        caliButtons[i].IsEnabled = false;
                    CalibWin.cbMachineType.IsEnabled = false; CalibWin.cbSlipGaugeWidth.IsEnabled = false;
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_AutoCalibration.CalibrationEventHandler(MainView_DisableButtonState));
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:099", ex);
            }
        }
        void MainView_GetSurfaceEDUserChoice(string SEDEntities)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    //SEDEntitiesWindow.ResetWindow();
                    //SEDEntitiesWindow.AddEntities(SEDEntities);
                    //SEDEntitiesWindow.Show();
                    if (GlobalSettings.CurrentEdgeDetectionAlgorithm == GlobalSettings.EdgeDetectionAlgorithmType.Type6)
                    {
                        SetOwnerofWindow(SurfaceFrameGrabWin.MyInstance());
                        //SurfaceFrameGrabWin.MyInstance().Show();
                        //RWrapper.RW_MainInterface.MYINSTANCE().SetUserChosenSurfaceED("1");
                    }
                    else if (GlobalSettings.CurrentEdgeDetectionAlgorithm == GlobalSettings.EdgeDetectionAlgorithmType.Type1)
                    {
                        RWrapper.RW_MainInterface.MYINSTANCE().SetUserChosenSurfaceED("0");
                    }
                    else if (GlobalSettings.CurrentEdgeDetectionAlgorithm == GlobalSettings.EdgeDetectionAlgorithmType.Type2)
                    {
                        RWrapper.RW_MainInterface.MYINSTANCE().SetUserChosenSurfaceED("1");
                    }
                    else if (GlobalSettings.CurrentEdgeDetectionAlgorithm == GlobalSettings.EdgeDetectionAlgorithmType.Type3)
                    {
                        RWrapper.RW_MainInterface.MYINSTANCE().SetUserChosenSurfaceED("2");
                    }
                    else if (GlobalSettings.CurrentEdgeDetectionAlgorithm == GlobalSettings.EdgeDetectionAlgorithmType.Type4)
                    {
                        RWrapper.RW_MainInterface.MYINSTANCE().SetUserChosenSurfaceED("3");
                    }
                    else if (GlobalSettings.CurrentEdgeDetectionAlgorithm == GlobalSettings.EdgeDetectionAlgorithmType.Type5)
                    {
                        RWrapper.RW_MainInterface.MYINSTANCE().SetUserChosenSurfaceED("4");
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.SurfaceEDEventHandler(MainView_GetSurfaceEDUserChoice));
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:100", ex);
            }
        }
        void MainView_LineArcParameters(double Tolerance, double Maxradius, double MinRadius, double Angle, double Noise, double MaxDistCutOff, bool ClosedLoop, bool ShpAsFasTrace, bool JoinAllPts)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    LineArcPropWindow = new LineArcPropertiesWin();
                    SetOwnerofWindow(LineArcPropWindow);
                    LineArcPropWindow.SetInitialValues(Tolerance, Noise, Maxradius, MinRadius, MaxDistCutOff, Angle, ClosedLoop, ShpAsFasTrace, JoinAllPts);
                    LineArcPropWindow.Left = VideoLeft + 5;
                    LineArcPropWindow.Top = VideoTop + 5;
                    LineArcPropWindow.Show();
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.LineArcParametersEventHandler(MainView_LineArcParameters), Tolerance, Maxradius, MinRadius, Angle, Noise, MaxDistCutOff, ClosedLoop, ShpAsFasTrace, JoinAllPts);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:101", ex);
            }
        }
        void MainView_BestFitSurfaceParam(double BFSurfacePtsFactor, bool BFSurfacePtsAroundXYPln)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    BFSurfacePropWin = new BestFitSurfacePropertiesWin();
                    SetOwnerofWindow(BFSurfacePropWin);
                    BFSurfacePropWin.SetInitialValues(BFSurfacePtsFactor, BFSurfacePtsAroundXYPln);
                    BFSurfacePropWin.Left = VideoLeft + 5;
                    BFSurfacePropWin.Top = VideoTop + 5;
                    BFSurfacePropWin.Show();
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.BestFitSurfaceEventHandler(MainView_BestFitSurfaceParam), BFSurfacePtsFactor, BFSurfacePtsAroundXYPln);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:101", ex);
            }
        }
        void btnHelp_Click(object sender, RoutedEventArgs e)
        {
            HelpWindow.GetObj().ShowMain();
            HelpWindow.GetObj().Show();
        }

        void ShapeParameterWin_ShowHideFG(string e)
        {
            try
            {
                ShowHideFGpointsStatus = e;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:103", ex);
            }
        }

        private void LoginBtn_Click(object sender, RoutedEventArgs e)
        {
            SettingsLoginWindow.txtInfo.Text = "";
        ShowLoginWindow:
            SettingsLoginWindow.txtPassword.Password = "";
            string EnteredUserName = SettingsLoginWindow.LoggedInUser;
            SettingsLoginWindow.UserTabletList.Children.Clear();

            foreach (string sr in SettingsLoginWindow.LoginDetailsNameColl)
            {
                RadioButton Rb = new RadioButton();
                Rb.Style = UserTabletBtnStyle;
                Rb.Height = Rb.Width = 120;
                Rb.Content = sr;
                Rb.Tag = sr;
                Rb.Click += new RoutedEventHandler(SettingsLoginWindow.HandleUserTabletClick);
                SettingsLoginWindow.UserTabletList.Children.Add(Rb);
            }
            SettingsLoginWindow.LoggedInUser = EnteredUserName;
            SettingsLoginWindow.ShowDialog();
            if (SettingsLoginWindow.InfoStatus)
            {
                string username = SettingsLoginWindow.LoggedInUser;
                string password = SettingsLoginWindow.txtPassword.Password;
                //System.Collections.Hashtable loginDetails = RWrapper.RW_DBSettings.MYINSTANCE().LoginDetailsCollection;
                if (SettingsLoginWindow.LoginDetailsNameColl.Contains(username))
                {
                    if (password == SettingsLoginWindow.getPassword(username))
                    {
                        //RWrapper.RW_MainInterface.MYINSTANCE().LoginSuccessFull(username);
                        // b.Content = "Logout";
                        LoginBtn.Content = username;
                        LoginBtn.Content = SettingsLoginWindow.getPrevilegeLevel(username);
                        UserNameTxt.Text = username;
                        UserLevelTxt.Text = SettingsLoginWindow.getPrevilegeLevel(username);
                        switch (SettingsLoginWindow.getPrevilegeLevel(username))
                        {
                            case "System":
                                LoginBtn.Foreground = VSnapOn;
                                GlobalSettings.CurrentUserLevel = GlobalSettings.UserPrevilageLevel.System;
                                //HelpWindow.GetObj().IsAdmminMode = true;
                                DelfiRotaryContainerWindow.MyInstance().SaveOffset.Visibility = System.Windows.Visibility.Visible;
                                break;
                            case "Admin":
                                LoginBtn.Foreground = VSnapOn;
                                if (username == "admin")
                                {
                                    GlobalSettings.CurrentUserLevel = GlobalSettings.UserPrevilageLevel.Admin;
                                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
                                        DelfiRotaryContainerWindow.MyInstance().SaveOffset.Visibility = System.Windows.Visibility.Visible;
                                }
                                else
                                    GlobalSettings.CurrentUserLevel = GlobalSettings.UserPrevilageLevel.AdminUser;
                                break;
                            case "User":
                                LoginBtn.Foreground = SnapOn;
                                GlobalSettings.CurrentUserLevel = GlobalSettings.UserPrevilageLevel.User;
                                break;
                        }

                        if (SettingsLoginWindow.getPrevilegeLevel(username) == "Admin" || SettingsLoginWindow.getPrevilegeLevel(username) == "System")
                        {
                            ToolsScrollList.AddNewButton("Fixture Calibration", "Fixture Calibration", GlobalSettings.ToolbarBtnStdWidth, GlobalSettings.ToolbarBtnStdHeight,
                                    GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight, new Thickness(2, 0, 2, 0));
                            //ToolsScrollList.AddNewButton("StarProbeCalibration", "StarProbeCalibration", GlobalSettings.ToolbarBtnStdWidth, GlobalSettings.ToolbarBtnStdHeight,
                            //       GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight, new Thickness(2, 0, 2, 0));
                            //ToolsScrollList.AddNewButton("ProbeInspection", "ProbeInspection", GlobalSettings.ToolbarBtnStdWidth, GlobalSettings.ToolbarBtnStdHeight,
                            //    GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight, new Thickness(2, 0, 2, 0));
                        }
                        //Check if system has logged in then show another button into the tools dropdown for the calibation window.
                        if (SettingsLoginWindow.getPrevilegeLevel(username) == "System")
                        {
                            ToolsScrollList.AddNewButton("Calibration", "Calibration", GlobalSettings.ToolbarBtnStdWidth, GlobalSettings.ToolbarBtnStdHeight,
                                GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight, new Thickness(2, 0, 2, 0));
                            ToolsScrollList.AddNewButton("FocusScan Calibration", "FocusScan Calibration", GlobalSettings.ToolbarBtnStdWidth, GlobalSettings.ToolbarBtnStdHeight,
  GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight, new Thickness(2, 0, 2, 0));
                  
								
                            ToolsScrollList.Get_Button("Flip Vertical").Visibility = Visibility.Visible;
                            ToolsScrollList.Get_Button("Flip Horizontal").Visibility = Visibility.Visible;
                        }
                        if (SettingsLoginWindow.getPrevilegeLevel(username) == "Admin" || SettingsLoginWindow.getPrevilegeLevel(username) == "System")
                        {
                            IsAdmin = true;
                        }
                        //GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Logout", "Logout", GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                        //ToolsScrollList.AssignSingleBtnProp(Btn, "Logout", GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);

                        ContentControl SetBtn = ToolsScrollList.AddNewButton("Settings", "Settings", GlobalSettings.ToolbarBtnStdWidth, GlobalSettings.ToolbarBtnStdHeight,
                                GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight, new Thickness(2, 0, 2, 0));
                        ToolsScrollBarButtons_Click(SetBtn, null);
                    }
                    else
                    {
                        SettingsLoginWindow.txtInfo.Text = "Entered 'Password' is wrong.Please try again.";
                        goto ShowLoginWindow;
                    }
                }
                else
                {
                    SettingsLoginWindow.txtInfo.Text = "Entered username does'nt exist.Please try again.";
                    goto ShowLoginWindow;
                }

            }
        }

        private void LogoutBtn_Click(object sender, RoutedEventArgs e)
        {
            if(UserNameTxt.Text != "Not Logged In")
            {
                GlobalSettings.CurrentUserLevel = GlobalSettings.UserPrevilageLevel.None;
                ToolsScrollList.Remove_Button_from_End();
                if (UserLevelTxt.Text == "System")
                {
                    ToolsScrollList.Remove_Button_from_End();
                    ToolsScrollList.Remove_Button_from_End();
                    HelpWindow.GetObj().IsAdmminMode = false;
                    DelfiRotaryContainerWindow.MyInstance().SaveOffset.Visibility = System.Windows.Visibility.Collapsed;
                }

                if (UserLevelTxt.Text == "System")
                {
                    ToolsScrollList.Remove_Button_from_End();
                    DelfiRotaryContainerWindow.MyInstance().SaveOffset.Visibility = System.Windows.Visibility.Collapsed;
                    ToolsScrollList.Get_Button("Flip Vertical").Visibility = Visibility.Collapsed;
                    ToolsScrollList.Get_Button("Flip Horizontal").Visibility = Visibility.Collapsed;
                }
                if (UserLevelTxt.Text == "Admin" || UserLevelTxt.Text == "System")
                {
                    IsAdmin = false;
                }
                LoginBtn.Content = "Login";
                UserNameTxt.Text = "Not Logged In";
            }
        }

        void FixtureNameCBx_SelectionChanged(object sender, RoutedEventArgs e)
        {
            try
            {

                if (GlobalSettings.FixtureNameCBx.SelectedIndex < 0) return;
                int Indx = GlobalSettings.FixtureNameCBx.SelectedIndex;
  
                RWrapper.RW_FixtureCalibration.MYINSTANCE().UpdateFixureDetails_Frontend(GlobalSettings.FixtureNameCBx.SelectedItem.ToString(),
                    Convert.ToInt16(GlobalSettings.FixRowNum[Indx]),
                    Convert.ToInt16(GlobalSettings.FixColNum[Indx]),

                    Convert.ToInt16(GlobalSettings.FixRowGap[Indx]),
                    Convert.ToInt16(GlobalSettings.FixColGap[Indx]));

            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:104", ex);
            }
        }

        void MainView_AutoDxfAlign()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    RWrapper.RW_MainInterface.MYINSTANCE().SetAlignmentCorrectionValue(GlobalSettings.DxfAutoAlignOffset_XVals.ToArray(), GlobalSettings.DxfAutoAlignOffset_YVals.ToArray(), GlobalSettings.DxfAutoAlignDistVals.ToArray());
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.RWrapperEventHandler(MainView_AutoDxfAlign));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV66", ex); }
        }

        bool CheckLicenseForPP(string FeatureStatus)
        {
            for (int i = 1; i < FeatureStatus.Length; i++)
            {
                if (FeatureStatus[i] == '1')
                    if (!(bool)GlobalSettings.FeatureLicenseInfo[i])
                        return false;
            }
            return true;
        }

        void TakeProbePoint()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {

                    Point currentMousePo = Mouse.GetPosition(this);
                    RcadPointClickContextMenu.IsOpen = true;
                    //ChangeRcadContextMenuContents("RcadContextMenu", ref RcadContextMenu, Category);    
                }
                else
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_CNC.TakeProbePointEventHandler(TakeProbePoint));
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV43", ex.Message.ToString(), "");
            }
        }
        #endregion

        #region UI behavioural functions

        void ReadValuesFromDatabase()
        {
            try
            {
                GlobalSettings.FixtureNameCBx = new ComboBox();
                GlobalSettings.FixRowNum = new List<string>();
                GlobalSettings.FixColNum = new List<string>();
                GlobalSettings.FixRowGap = new List<string>();
                GlobalSettings.FixColGap = new List<string>();

                CalibrationModule.DB Settings_DataB = new CalibrationModule.DB(RWrapper.RW_MainInterface.DBConnectionString);
                Settings_DataB.FillTable("CurrentMachine");
                Settings_DataB.FillTable("FixtureDetails");
                Settings_DataB.FillTable("UserSettings");
                Settings_DataB.FillTable("OneShotParams");
                string MachineNo = Settings_DataB.get_GetRecord("CurrentMachine", Settings_DataB.GetRowCount("CurrentMachine") - 1, "MachineNo").ToString();
                System.Data.DataRow[] FixtureRows = Settings_DataB.Select_ChildControls("FixtureDetails", "MachineNo", MachineNo);
                foreach (System.Data.DataRow D in FixtureRows)
                {
                    GlobalSettings.FixtureNameList.Add(D["FixtureName"].ToString());
                    GlobalSettings.FixtureNameCBx.Items.Add(D["FixtureName"].ToString());
                    GlobalSettings.FixRowNum.Add(D["No_of_Rows"].ToString());     
                    GlobalSettings.FixColNum.Add(D["No_of_Columns"].ToString());
                    GlobalSettings.FixRowGap.Add(D["RowGap"].ToString());
                    GlobalSettings.FixColGap.Add(D["ColumnGap"].ToString());
                }
                GlobalSettings.FixtureNameCBx.SelectedItem = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["DefaultFixtureName"].ToString();
                GlobalSettings.LocalisedCorrectionOn = Convert.ToBoolean(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["UseLocalisedCorrection"]);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:105", ex);
            }
        }
        //Handling groupbox settings combo box key down event
        void SettingsCB_KeyDown(object sender, KeyEventArgs e)
        {
            try
            {
                Panel TmpPanel;
                ButtonHandler_Pointer Handler_Func;
                int a;
                bool BtnFound = false;

                if (e.Key != Key.Enter) return;

                if (((ComboBox)sender).Tag as string == "Measure")
                {
                    TmpPanel = MeasureScrollList.MainPanel;
                    Handler_Func = MeasureScrollBarButtons_Click;
                }
                else if (((ComboBox)sender).Tag as string == "Draw")
                {
                    TmpPanel = DrawScrollList.MainPanel;
                    Handler_Func = DrawScrollBarButtons_Click;
                }
                else if (((ComboBox)sender).Tag as string == "DerivedShape")
                {
                    TmpPanel = DerivedShapeScrollList.MainPanel;
                    Handler_Func = DerivedShapeScrollBarButtons_Click;
                }
                else if (((ComboBox)sender).Tag as string == "Crosshair")
                {
                    TmpPanel = CrosshairScrollList.MainPanel;
                    Handler_Func = CrosshairScrollBarButtons_Click;
                }
                else if (((ComboBox)sender).Tag as string == "Edit")
                {
                    TmpPanel = EditScrollList.MainPanel;
                    Handler_Func = EditScrollBarButtons_Click;
                }
                else if (((ComboBox)sender).Tag as string == "Tools")
                {
                    TmpPanel = ToolsScrollList.MainPanel;
                    Handler_Func = ToolsScrollBarButtons_Click;
                }
                else if (((ComboBox)sender).Tag as string == "UCS")
                {
                    TmpPanel = UCSScrollList.MainPanel;
                    Handler_Func = UCSScrollBarButtons_Click;
                }
                else// if ((ComboBox)sender == ToolsSettingsCB)
                {
                    TmpPanel = ToolsScrollList.MainPanel;
                    Handler_Func = ToolsScrollBarButtons_Click;
                }

                for (a = 0; a < TmpPanel.Children.Count; a++)
                    if (TmpPanel.Children[a].GetType() != typeof(Image))
                        if (((Control)TmpPanel.Children[a]).ToolTip.ToString() == ((ComboBox)sender).Text.ToString())
                        {
                            BtnFound = true;
                            break;
                        }

                if (BtnFound)
                {
                    ((Control)TmpPanel.Children[a]).BringIntoView();
                    Handler_Func(TmpPanel.Children[a], null);
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:106", ex); }
        }
        //Function to handle the About button click
        private void btnAbout_Click(object sender, MouseButtonEventArgs e)
        {
            try
            {
                splash.btnClose.Visibility = System.Windows.Visibility.Visible;
                splash.ShowDialog();
                this.Focus();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:107", ex); }
        }

        //handling the check and uncheck of Rcad/Dxf radio buttons
        void OtherWindowsTBtn_Checked(object sender, RoutedEventArgs e)
        {
            try
            {
                if (OtherWinToolbarRotated)
                {
                    ButtonProperties st = RcadToolbar.btnMaxMin.Tag as ButtonProperties;
                    if (st.ButtonName == "Minimize")
                        RcadScrollBarButtons_Click(RcadToolbar.btnMaxMin, null);
                    ButtonProperties st1 = DXFToolbar.btnMaxMin.Tag as ButtonProperties;
                    if (st1.ButtonName == "Minimize")
                        DxfScrollBarButtons_Click(DXFToolbar.btnMaxMin, null);

                    TxtHelperGrid.Visibility = System.Windows.Visibility.Hidden;
                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge)
                    {
                        RcadWin.Hide();
                        RcadToolbar.Visibility = System.Windows.Visibility.Hidden;
                    }
                    else
                    {
                        VideoHolder.Visibility = Visibility.Hidden;
                       //VideoFormsWin.Hide();
                    }
                }
                else
                {
                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge)
                    {
                        ButtonProperties st = RcadToolbar.btnMaxMin.Tag as ButtonProperties;
                        if (st.ButtonName == "Minimize")
                            RcadScrollBarButtons_Click(RcadToolbar.btnMaxMin, null);
                    }
                    ButtonProperties st1 = DXFToolbar.btnMaxMin.Tag as ButtonProperties;
                    if (st1.ButtonName == "Minimize")
                        DxfScrollBarButtons_Click(DXFToolbar.btnMaxMin, null);
                }

                ToggleButton TBtn = (ToggleButton)sender;
                if (TBtn.Tag.ToString() == "RapidCAD")
                {
                    PicViewerRBtn.IsChecked = false;
                    ProgramRBtn.IsChecked = false;
                    if (OtherWinToolbarRotated)
                        ShpsNMeasurementsTBtn.IsChecked = false;
                    RcadWin.Show();
                    UpdateOnSeparateThread();
                    ToolbarBorder.Child = RcadToolbar;
                    //RWrapper.RW_MainInterface.MYINSTANCE().UpdateRCadGraphics();
                }
                else if (TBtn.Tag.ToString() == "Pic Viewer")
                {
                    RapidCADRBtn.IsChecked = false;
                    ProgramRBtn.IsChecked = false;
                    if (OtherWinToolbarRotated)
                        ShpsNMeasurementsTBtn.IsChecked = false;
                    PictureViewerWin.Show();
                }
                else if (TBtn.Tag.ToString() == "Program")
                {
                    RapidCADRBtn.IsChecked = false;
                    PicViewerRBtn.IsChecked = false;
                    if (OtherWinToolbarRotated)
                    {
                        ShpsNMeasurementsTBtn.IsChecked = false;

                    }
                    PartProgramPopUp.Visibility = System.Windows.Visibility.Visible;
                }
                else if (TBtn.Tag.ToString() == "Shapes, Measurements & Actions")
                {
                    RapidCADRBtn.IsChecked = false;
                    ProgramRBtn.IsChecked = false;
                    PicViewerRBtn.IsChecked = false;
                    ShapeParameterWin.Visibility = System.Windows.Visibility.Visible;
                    MeasurementsPopup.Visibility = System.Windows.Visibility.Visible;
                    ActionsPopup.Visibility = System.Windows.Visibility.Visible;
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:108", ex); }
        }

        void OtherWindowsTBtn_Unchecked(object sender, RoutedEventArgs e)
        {
            try
            {
                ToggleButton TBtn = (ToggleButton)sender;

                if (TBtn.Tag.ToString() == "RapidCAD")
                {
                    ButtonProperties st = RcadToolbar.btnMaxMin.Tag as ButtonProperties;
                    if (st.ButtonName == "Minimize")
                        RcadScrollBarButtons_Click(RcadToolbar.btnMaxMin, null);
                    RcadWin.Hide();
                    ToolbarBorder.Child = null;
                }
                else if (TBtn.Tag.ToString() == "DXF")
                {
                    ButtonProperties st = DXFToolbar.btnMaxMin.Tag as ButtonProperties;
                    if (st.ButtonName == "Minimize")
                        DxfScrollBarButtons_Click(DXFToolbar.btnMaxMin, null);
                    DxfWin.Hide();
                    ToolbarBorder.Child = null;
                }
                else if (TBtn.Tag.ToString() == "Program")
                {
                    PartProgramPopUp.Visibility = System.Windows.Visibility.Hidden;
                }
                else if (TBtn.Tag.ToString() == "Pic Viewer")
                {
                    PictureViewerWin.Hide();
                }
                else if (TBtn.Tag.ToString() == "Shapes, Measurements & Actions")
                {
                    ShapeParameterWin.Visibility = System.Windows.Visibility.Hidden;
                    MeasurementsPopup.Visibility = System.Windows.Visibility.Hidden;
                    ActionsPopup.Visibility = System.Windows.Visibility.Hidden;
                }

                if (ShpsNMeasurementsTBtn != null)
                {
                    if (!(bool)RapidCADRBtn.IsChecked && !(bool)ProgramRBtn.IsChecked && !(bool)PicViewerRBtn.IsChecked && !(bool)ShpsNMeasurementsTBtn.IsChecked)
                    {
                        TxtHelperGrid.Visibility = System.Windows.Visibility.Visible;
                        if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge)
                        {
                            RcadWin.Show();
                            RcadToolbar.Visibility = System.Windows.Visibility.Visible;
                        }
                        else
                        {
                            //VideoFormsWin.Show();
                            VideoHolder.Visibility = Visibility.Visible;
                            RWrapper.RW_MainInterface.MYINSTANCE().UpdateVideoGraphics();
                        }
                    }
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0109", ex); }
        }
        void OtherWindowsTBtn_PreviewMouseLeftButtonDown(object sender, MouseEventArgs e)
        {
            try
            {
                if ((bool)((ToggleButton)sender).IsChecked)
                    if (!OtherWinToolbarRotated)
                        e.Handled = true;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0110", ex); }
        }

        void LowerGroupTab1TBtn_Checked(object sender, RoutedEventArgs e)
        {
            try
            {
                ToggleButton TBtn = (ToggleButton)sender;
                if (LowerPanelsMinimized) LowerPanelWin1.ShowWindow();
                if (TBtn.Tag.ToString() == "Focus")
                {
                    ThreadRBtn.IsChecked = false;
                    FixedGraphicsRBtn.IsChecked = false;
                    if (LowerPanelsMinimized)
                        LowerPanelWin1.LowerPanelWinContainer.Content = FocusPopUp;
                    else
                        LowerPanelHolder1.Content = FocusPopUp;
                }
                else if (TBtn.Tag.ToString() == "Thread")
                {
                    FocusRBtn.IsChecked = false;
                    //PCDRBtn.IsChecked = false;
                    FixedGraphicsRBtn.IsChecked = false;
                    if (LowerPanelsMinimized)
                        LowerPanelWin1.LowerPanelWinContainer.Content = ThreadPopup;
                    else
                        LowerPanelHolder1.Content = ThreadPopup;
                    ThreadGLWinShowHide(true);
                    RWrapper.RW_Thread.MYINSTANCE().UpdateThreadGraphics();
                    //System.Threading.Thread.Sleep(2000);
                    //UpdateOnSeparateThread();
                }
                //else if (TBtn.Content.ToString() == "PCD")
                //{
                //    FocusRBtn.IsChecked = false;
                //    ThreadRBtn.IsChecked = false;
                //    DepthRBtn.IsChecked = false;
                //    FixedGraphicsRBtn.IsChecked = false;
                //    if (LowerPanelsMinimized)
                //        LowerPanelWin2.LowerPanelWinContainer.Content = PCDPopup;
                //    else
                //        LowerPanelHolder1.Content = PCDPopup;
                //}
                else if (TBtn.Tag.ToString() == "Fixed Graphics")
                {
                    FocusRBtn.IsChecked = false;
                    ThreadRBtn.IsChecked = false;
                    //PCDRBtn.IsChecked = false;
                    if (LowerPanelsMinimized)
                        LowerPanelWin1.LowerPanelWinContainer.Content = FixedGraphicsPopup;
                    else
                        LowerPanelHolder1.Content = FixedGraphicsPopup;
                }
                else if (TBtn.Tag.ToString() == "Lighting")
                {
                    ThreadRBtn.IsChecked = false;
                    FixedGraphicsRBtn.IsChecked = false;
                    //PCDRBtn.IsChecked = false;
                    LowerPanelHolder1.Content = lightingPopup;
                }
                else if (TBtn.Tag.ToString() == "Manual")
                {
                    ThreadRBtn.IsChecked = false;
                    FixedGraphicsRBtn.IsChecked = false;
                    //PCDRBtn.IsChecked = false;
                    if (LowerPanelsMinimized)
                        LowerPanelWin1.LowerPanelWinContainer.Content = CNCPopUp;
                    else
                        LowerPanelHolder1.Content = CNCPopUp;
                }

                //UpdateOnSeparateThread();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0111", ex); }
        }
        void LowerGroupTab1TBtn_Unchecked(object sender, RoutedEventArgs e)
        {
            try
            {
                ToggleButton TBtn = (ToggleButton)sender;
                if (TBtn.Tag.ToString() == "Thread")
                {
                    ThreadGLWinShowHide(false);
                }


                if (!(bool)FocusRBtn.IsChecked && !(bool)ThreadRBtn.IsChecked && !(bool)FixedGraphicsRBtn.IsChecked)
                {
                    LowerPanelWin1.HideWindow();
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0102.3", ex); }
        }

        void LowerGroupTab2TBtn_Checked(object sender, RoutedEventArgs e)
        {
            try
            {
                ToggleButton TBtn = (ToggleButton)sender;
                if (LowerPanelsMinimized) LowerPanelWin1.ShowWindow();
                if (TBtn.Tag.ToString() == "Thread")
                {
                    CNCRBtn.IsChecked = false;
                    if (LowerPanelsMinimized)
                        LowerPanelWin1.LowerPanelWinContainer.Content = ThreadPopup;
                    else
                        LowerPanelHolder1.Content = ThreadPopup;
                    ThreadGLWinShowHide(true);
                    RWrapper.RW_Thread.MYINSTANCE().UpdateThreadGraphics();
                }
                else if (TBtn.Tag.ToString() == "CNC")
                {
                    ThreadRBtn.IsChecked = false;
                    if (LowerPanelsMinimized)
                        LowerPanelWin1.LowerPanelWinContainer.Content = CNCPopUp;
                    else
                        LowerPanelHolder1.Content = CNCPopUp;
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0111", ex); }
        }
        void LowerGroupTab2TBtn_Unchecked(object sender, RoutedEventArgs e)
        {
            try
            {
                ToggleButton TBtn = (ToggleButton)sender;
                if (TBtn.Tag.ToString() == "Thread")
                {
                    ThreadGLWinShowHide(false);
                }


                if (!(bool)CNCRBtn.IsChecked && !(bool)ThreadRBtn.IsChecked)
                {
                    LowerPanelWin1.HideWindow();
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0102.3", ex); }
        }

        void LowerGroupTab3TBtn_Checked(object sender, RoutedEventArgs e)
        {
            try
            {
                ToggleButton TBtn = (ToggleButton)sender;
                if (TBtn.Tag.ToString() == "Shapes")
                {
                    MeasurementsRBtn.IsChecked = false;
                    DXFTBtn.IsChecked = false;
                    ActionsRBtn.IsChecked = false;
                    DxfWin.Hide();
                    DxfLowerToolbarBorder.Child = null;
                    LowerPanelHolder4.Content = ShapeParameterWin;
                    ShapeParameterWin.ScrollAppropriateShape_IntoView();
                    ShapeParameterWin.ScrollAppropriateShapePoint_IntoView();
                }
                else if (TBtn.Tag.ToString() == "Measurements")
                {
                    ShapesRBtn.IsChecked = false;
                    DXFTBtn.IsChecked = false;
                    ActionsRBtn.IsChecked = false;
                    DxfWin.Hide();
                    DxfLowerToolbarBorder.Child = null;
                    LowerPanelHolder4.Content = MeasurementsPopup;
                    MeasurementsPopup.ScrollAppropriateMeasure_IntoView();
                }
                else if (TBtn.Tag.ToString() == "DXF")
                {
                    ShapesRBtn.IsChecked = false;
                    MeasurementsRBtn.IsChecked = false;
                    ActionsRBtn.IsChecked = false;
                    LowerPanelHolder4.Content = null;
                    DxfWin.Show();
                    UpdateOnSeparateThread();
                    DxfLowerToolbarBorder.Child = DXFToolbar;
                    //RWrapper.RW_MainInterface.MYINSTANCE().UpdateDxfGraphics();
                }
                else if (TBtn.Tag.ToString() == "Actions")
                {
                    ShapesRBtn.IsChecked = false;
                    MeasurementsRBtn.IsChecked = false;
                    DXFTBtn.IsChecked = false;
                    DxfWin.Hide();
                    DxfLowerToolbarBorder.Child = null;
                    LowerPanelHolder4.Content = ActionsPopup;
                    //RWrapper.RW_MainInterface.MYINSTANCE().UpdateDxfGraphics();
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0102.2", ex); }
        }

        void LowerGroupTab4TBtn_Checked(object sender, RoutedEventArgs e)
        {
            try
            {
                ToggleButton TBtn = (ToggleButton)sender;
                if (TBtn.Tag.ToString() == "Lighting")
                {
                    LowerPanelWin2.ShowWindow();
                }
                else if (TBtn.Content.ToString() == "Manual" || TBtn.Content.ToString() == "CNC")
                {   if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HobChecker)
                        LowerPanelWin3.ShowWindow();
                   }
                if (TBtn.Content.ToString() == "SAOI")
                {
                    LowerPanelWin4.ShowWindow();
                }
                if (TBtn.Content.ToString() == "Holder")
                {
                    VBlockHolderWin.ShowWindow();
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0102.2", ex); }
        }
        void LowerGroupTab4TBtn_Unchecked(object sender, RoutedEventArgs e)
        {
            try
            {
                ToggleButton TBtn = (ToggleButton)sender;
                if (TBtn.Tag.ToString() == "Lighting")
                {
                    LowerPanelWin2.HideWindow();
                }
                if (TBtn.Content.ToString() == "SAOI")
                {
                    LowerPanelWin4.HideWindow();
                }
                else if (TBtn.Content.ToString() == "Manual" || TBtn.Content.ToString() == "CNC")
                {
                    LowerPanelWin3.HideWindow();
                }
                if (TBtn.Content.ToString() == "Holder")
                {
                    VBlockHolderWin.HideWindow();
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0102.3", ex); }
        }

        void LowerGroupTabTBtn_PreviewMouseLeftButtonDown(object sender, MouseEventArgs e)
        {
            try
            {
                //if (RWrapper.RW_MainInterface.MYINSTANCE().MachineCardFlag < 2)
               // {
                    if (sender.Equals(CNCRBtn))
                    {
                        if (!RWrapper.RW_CNC.MYINSTANCE().CNCmode) // CNCRBtn.Content.ToString() == "Manual")
                        {
                            //CNCRBtn.Content = "CNC";
                            RWrapper.RW_CNC.MYINSTANCE().Activate_CNCModeDRO();
                        }
                        else
                        {
                           // CNCRBtn.Content = "Manual";
                            RWrapper.RW_CNC.MYINSTANCE().Activate_ManualModeDRO();
                        }
                    }
               // }
                if ((bool)((ToggleButton)sender).IsChecked)
                    if (!LowerPanelsMinimized)
                        e.Handled = true;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0102.4", ex); }
        }
        void CNCRBtn_MouseLeftButtonDown(object sender, MouseEventArgs e)
        {
            try
            {
                if (sender.Equals(CNCRBtn))
                {
                    if (HolderRBtn.Tag.ToString() == "TagToggeled")
                    {
                        HolderRBtn.IsChecked = false;
                        LowerPanelHolder3.Content = CNCPopUp;
                        HolderRBtn.Tag = "VBlockHolder";
                    }
                    else
                    {
                        if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                        {
                            if (!(bool)GlobalSettings.FeatureLicenseInfo[6])
                            {
                                MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                                e.Handled = true;
                                return;
                            }
                        }
                        if (!RWrapper.RW_CNC.MYINSTANCE().CNCmode) //CNCRBtn.Content.ToString() == "Manual")
                        {
                            //if (RWrapper.RW_MainInterface.MYINSTANCE().MachineCardFlag < 2) CNCRBtn.Content = "CNC";
                            RWrapper.RW_CNC.MYINSTANCE().Activate_CNCModeDRO();
                        }
                        else
                        {
                            //if (RWrapper.RW_MainInterface.MYINSTANCE().MachineCardFlag < 2) CNCRBtn.Content = "Manual";
                            RWrapper.RW_CNC.MYINSTANCE().Activate_ManualModeDRO();
                        }
                    }
                }
                if ((bool)((ToggleButton)sender).IsChecked)
                    if (!LowerPanelsMinimized)
                        e.Handled = true;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0102.4", ex); }
        }
        void CNCRBtn_MouseRightButtonUp(object sender, MouseEventArgs e)
        {
            try
            {
                if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                {
                    if (!(bool)GlobalSettings.FeatureLicenseInfo[6])
                    {
                        MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                        e.Handled = true;
                        return;
                    }
                }
                if (!RWrapper.RW_CNC.MYINSTANCE().CNCmode) //CNCRBtn.Content.ToString() == "Manual")
                {
                   // if (RWrapper.RW_MainInterface.MYINSTANCE().MachineCardFlag < 2) CNCRBtn.Content = "CNC";
                    RWrapper.RW_CNC.MYINSTANCE().Activate_CNCModeDRO();
                }
                else
                {
                    //if (RWrapper.RW_MainInterface.MYINSTANCE().MachineCardFlag < 2) CNCRBtn.Content = "Manual";
                    RWrapper.RW_CNC.MYINSTANCE().Activate_ManualModeDRO();
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0011", ex); }
        }

        void HolderRBtn_PreviewMouseLeftButtonDown(object sender, MouseEventArgs e)
        {
            try
            {
                if (sender.Equals(HolderRBtn))
                {
                    CNCRBtn.IsChecked = false;
                    HolderRBtn.Tag = "TagToggeled";
                    LowerPanelHolder3.Content = VBlockPopup;
                }
                if ((bool)((ToggleButton)sender).IsChecked)
                    if (!LowerPanelsMinimized)
                        e.Handled = true;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0102.613", ex);
            }
        }

        private void ThreadGLWinShowHide(bool ShowMe)
        {
            try
            {
                if (ShowMe)
                {
                    if (!ThreadOpenGlWindowInitialized)
                    {

                        if (ThreadPopup.MainGrid.RowDefinitions[0].ActualHeight == 0.0)
                        {
                            if (LowerPanelsMinimized)
                                ThreadPopup.Measure(new Size(LowerPanelWin1.ActualWidth, LowerPanelWin1.ActualHeight));
                            else
                                ThreadPopup.Measure(new Size(LowerPanelHolder1.ActualWidth, LowerPanelHolder1.ActualHeight));
                            ThreadPopup.Arrange(new Rect(ThreadPopup.DesiredSize));
                        }
                        if (LowerPanelsMinimized)
                        {
                            InitializeWindows(ThreadWin, LowerPanelWin1.ActualHeight - 2, LowerPanelWin1.ActualWidth - ThreadPopup.ColumnDefinitions[1].ActualWidth - 4, this, true,
                            LowerPanelWin1.Left + 1, LowerPanelWin1.Top + 1, true);
                            ThreadWin.Owner = LowerPanelWin1;
                        }
                        else
                        {
                            Point TmpPos = LowerPanelHolder1.PointToScreen(new Point(0, 0));
                            InitializeWindows(ThreadWin, LowerPanelHolder1.ActualHeight - 2, LowerPanelHolder1.ActualWidth - ThreadPopup.ColumnDefinitions[1].ActualWidth - 4, this, true,
                                TmpPos.X + 1, TmpPos.Y + 1, true);
                            ThreadWin.Owner = this;
                        }
                        RWrapper.RW_MainInterface.MYINSTANCE().IntializeOpengl(ThreadWin.Handle, (int)ThreadWin.Width, (int)ThreadWin.Height, 4, 800, 600);
                        ThreadOpenGlWindowInitialized = true;
                    }
                    ThreadWin.Show();
                    RWrapper.RW_Thread.MYINSTANCE().UpdateThreadGraphics();
                }
                else
                    ThreadWin.Hide();

                //UpdateOnSeparateThread();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0109", ex); }
        }
        //Handling the key down events
        void MainView_PreviewKeyUp(object sender, KeyEventArgs e)
        {
            try
            {
                if (e.Key == Key.LeftCtrl || e.Key == Key.RightCtrl)
                    tbControlBtn.Visibility = Visibility.Hidden;
                if (e.Key == Key.LeftShift || e.Key == Key.RightShift)
                    tbShiftBtn.Visibility = Visibility.Hidden;
                if (e.Key == Key.System)
                    if (e.SystemKey == Key.LeftAlt || e.SystemKey == Key.RightAlt) tbAltBtn.Visibility = Visibility.Hidden;
                if (e.Key == Key.Escape || e.Key == Key.RightShift || e.Key == Key.LeftCtrl || e.Key == Key.RightAlt || e.Key == Key.LeftAlt || e.Key == Key.System || e.Key == Key.RightCtrl || e.Key == Key.LeftShift)
                { 
                    tbControlBtn.Visibility = Visibility.Hidden;
                    tbShiftBtn.Visibility = Visibility.Hidden;
                    tbAltBtn.Visibility = Visibility.Hidden;
                }

                RWrapper.RW_MainInterface.MYINSTANCE().Form_KeyUp(e);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0110", ex); }
        }
        //handling the KeyUp events
        void MainView_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            try
            {
                if ((e.KeyboardDevice.Modifiers & ModifierKeys.Control) == ModifierKeys.Control) tbControlBtn.Visibility = Visibility.Visible;
                if ((e.KeyboardDevice.Modifiers & ModifierKeys.Shift) == ModifierKeys.Shift) tbShiftBtn.Visibility = Visibility.Visible;
                if ((e.KeyboardDevice.Modifiers & ModifierKeys.Alt) == ModifierKeys.Alt) tbAltBtn.Visibility = Visibility.Visible;
                //else
                //{
                if (e.Key == Key.D || e.Key == Key.R || e.Key == Key.L)
                    tbControlBtn.Visibility = Visibility.Hidden;
                
               

                if (e.Key == Key.Delete)
                {
                    DrawScrollList.ClearAllBtnSelection();
                    CrosshairScrollList.ClearAllBtnSelection();
                }
                //}
                if (e.OriginalSource.GetType() == typeof(TextBox))
                    if (e.Key == Key.Delete || e.Key == Key.Enter)
                        return;
                //Insert  the DRO values in CNC goto textboxes
                if (e.Key == Key.Insert)
                {
                    if ((e.KeyboardDevice.Modifiers & ModifierKeys.Control) == ModifierKeys.Control)
                    {

                        CNCPopUp.txtgotodroX.Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[0]);
                        CNCPopUp.txtgotodroY.Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[1]);
                        CNCPopUp.txtgotodroZ.Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[2]);
                        CNCPopUp.txtgotodroR.Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[3] * (180 / Math.PI));
                    }
                }
                //Acivate/Inactivate CNC
                else if (e.Key == Key.F6)
                {
                    if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                    {
                        if (!(bool)GlobalSettings.FeatureLicenseInfo[6])
                        {
                            MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                            return;
                        }
                    }
                    if (!RWrapper.RW_CNC.MYINSTANCE().CNCmode) //CNCRBtn.Content.ToString() == "Manual")
                    {
                        //if (RWrapper.RW_MainInterface.MYINSTANCE().MachineCardFlag < 2) CNCRBtn.Content = "CNC";
                        RWrapper.RW_CNC.MYINSTANCE().Activate_CNCModeDRO();
                    }
                    else
                    {
                        //if (RWrapper.RW_MainInterface.MYINSTANCE().MachineCardFlag < 2) CNCRBtn.Content = "Manual";
                        RWrapper.RW_CNC.MYINSTANCE().Activate_ManualModeDRO();
                    }
                }
                //Build program
                else if (e.Key == Key.F2)
                    HandleLowerPanelsButtonsClick(St.Children.OfType<Button>().Where(T => ((ButtonProperties)(T.Tag)).ButtonName == "Build").First(), null);
                //Edit program
                else if (e.Key == Key.F3)
                    HandleLowerPanelsButtonsClick(St.Children.OfType<Button>().Where(T => ((ButtonProperties)(T.Tag)).ButtonName == "Edit").First(), null); //(string[])T.Tag)[2]
                //Run program
                else if (e.Key == Key.F5)
                {
                    if (!OtherWinToolbarRotated)
                        ProgramRBtn.IsChecked = true;
                    HandleLowerPanelsButtonsClick(St.Children.OfType<Button>().Where(T => ((ButtonProperties)(T.Tag)).ButtonName == "Run").First(), null);//((string[])T.Tag)[2] ==
                }
                else if (e.Key == Key.F7 && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.Rotary_Del_FI)
                {
                    //ExtendedDD.btnSnapMode.IsChecked = (bool)ExtendedDD.btnSnapMode.IsChecked ? false : true;
                    SnapStateButton_Click(null, null);
                }
                else if (e.Key == Key.F8 && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.Rotary_Del_FI)
                {
                    //if ((bool)ExtendedDD.btnSnapMode.IsChecked)
                    //{
                    //ExtendedDD.btnVSnapMode.IsChecked = (bool)ExtendedDD.btnVSnapMode.IsChecked ? false : true;
                    SnapStateButton_Click(null, null);
                    //}
                }
                else if (e.Key == Key.U && (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge))
                {
                    if ((e.KeyboardDevice.Modifiers & ModifierKeys.Control) == ModifierKeys.Control)
                    {
                        int AuxVal = 0;
                        if (AuxillaryLightIsON)
                        {
                            AuxillaryLightIsON = false;
                            AuxVal = 0;
                        }
                        else
                        {
                            AuxillaryLightIsON = true;
                            AuxVal = 64;
                        }
                        int[] tmpVals = { 0, 0, 0, 0, 0, AuxVal };
                        RWrapper.RW_CNC.MYINSTANCE().UpdateLight(tmpVals);
                    }
                }
                else if (e.Key == Key.H && GlobalSettings.RapidMachineType == GlobalSettings.MachineType.TIS)
                {
                    if ((e.KeyboardDevice.Modifiers & ModifierKeys.Control) == ModifierKeys.Control)
                    {
                        RWrapper.RW_CNC.MYINSTANCE().Swivel_A_Axis();
                    }
                }
                if (SettingsLoginWindow.IsVisible == true && e.Key == Key.Enter)
                    e.Handled = true;

                if (e.Key == Key.P)
                {
                    if ((e.KeyboardDevice.Modifiers & ModifierKeys.Control) == ModifierKeys.Control)
                    {
                        this.SaveScreen();
                    }
                }
                RWrapper.RW_MainInterface.MYINSTANCE().Form_KeyDown(e);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0111", ex); }
        }

        public void HandleCNCFeedRateSettingsChanged()
        {
            CNCPopUp.UpdateFeedrateSettings(false);
        }

        //Function to handle the Closing of the Main window.
        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            try
            {
                if (UnconditionalShutDown)
                {
                    Application.Current.Shutdown();
                    return;
                }
                if (System.Windows.Forms.MessageBox.Show("All the unsaved data will be lost.\nAre you sure you want to close Rapid-I ?",
                Application.Current.MainWindow.Title, System.Windows.Forms.MessageBoxButtons.YesNo,
                System.Windows.Forms.MessageBoxIcon.Question) == System.Windows.Forms.DialogResult.Yes)
                {

                    if (ConnectedToHardware)
                    {                    //Make default sataus of machine on closing.
                        RWrapper.RW_CNC.MYINSTANCE().SwitchOnOffLight(false);
                        //ContentControl c = PartProgramPopUp.St.Children.OfType<ContentControl>().Where(T => T.ToolTip.ToString() == "Stop").First();
                        //HandleLowerPanelsButtonsClick(c, null);
                        //Sathya told me to give an delay of 20 ms (some issues in calling light closing)23 may 2011
                        System.Threading.Thread.Sleep(20);
                        if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.One_Shot)
                        {
                            RWrapper.RW_CNC.MYINSTANCE().Activate_ManualModeDRO();
                            //Release axis lock if any
                            RWrapper.RW_CNC.MYINSTANCE().Unlock_Axis("X");
                            RWrapper.RW_CNC.MYINSTANCE().Unlock_Axis("Y");
                            RWrapper.RW_CNC.MYINSTANCE().Unlock_Axis("Z");
                        }
                    }
                    //free the unmanaged resources from framework
                    RWrapper.RW_MainInterface.CloseAll();
                    //Free the managed resources.
                    //Close all windows associated with main window.
                    //VideoFormsWin.Close();
                    Window[] win = { this.RcadWin, this.DxfWin };//, this.LowerPopUpWindow };
                    for (int i = 0; i < win.Length; i++)
                        win[i].Close();
                    //Clear the shapelist hash table.
                    ShapeListHashTable.Clear();
                    ShapeListHashTable = null;
                    //Clear all the context menus
                    ContextMenu[] cons = { VideoContextMenu, RcadContextMenu };
                    for (int i = 0; i < cons.Length; i++)
                    {
                        cons[i].Items.Clear();
                        cons[i] = null;
                    }
                    //Clear all the resources refrences
                    Application.Current.Shutdown();
                }
                else
                {
                    e.Cancel = true;
                }
            }
            catch (Exception ex) { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0116", ex); }
            //finally { //RWrapper.RW_CNC.ManualMode_DRO(); }
        }
        //Minimize/maximise changed
        void MainView_StateChanged(object sender, EventArgs e)
        {
            try
            {
                //Make an pointer array to all the windows displayed on the main window
                Window[] ownedwins = { RcadWin, DxfWin, shapeWin,
                                         PartProgramBuildWin,ThreadWin, LowerPanelWin1, LowerPanelWin2, LowerPanelWin3, LowerPanelWin4,
                                         DxfNudgeRotateStepWin,PictureViewerWin,CalibWin, ToolBoxHolder, PPToolBoxHolder}; //, hobWindow };

                switch (this.WindowState)
                {
                    //On minimize minimize them all.
                    case System.Windows.WindowState.Minimized:
                        {
                            Application.Current.MainWindow.WindowState = WindowState.Minimized;
                            ActiveWindows.Clear();
                            for (int i = 0; i < ownedwins.Length; i++)
                            {
                                if (ownedwins[i] != null)
                                {
                                    if (ownedwins[i].IsVisible)
                                        ActiveWindows.Add(ownedwins[i]);
                                }
                            }
                            foreach (Window win in ActiveWindows)
                            {
                                if (win != null)
                                {
                                    win.Visibility = System.Windows.Visibility.Hidden;
                                }
                            }
                        }
                        break;
                    //On maximize bring all owned windows to their normal state.
                    case System.Windows.WindowState.Normal:
                        {
                            foreach (Window win in ActiveWindows)
                            {
                                if (win != null)
                                {
                                    win.Visibility = System.Windows.Visibility.Visible;
                                }
                            }
                            UpdateOnSeparateThread();
                            //RWrapper.RW_MainInterface.MYINSTANCE().UpdateVideoGraphics();
                            //RWrapper.RW_MainInterface.MYINSTANCE().UpdateRCadGraphics();
                            //RWrapper.RW_MainInterface.MYINSTANCE().UpdateDxfGraphics();
                            RWrapper.RW_Thread.MYINSTANCE().UpdateThreadGraphics();
                        }
                        break;
                }
                //after that make the owned windows pointer as null.
                //ActiveWindows.Clear();
                //ActiveWindows = null;
                ownedwins = null;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0118", ex);
            }

        }
        //This function can initialize any window & do input binding for closing & can also handle the Windows mouse related events.
        //Especially used for Opengl windows.
        void InitializeWindows(Window win, double height, double width, Window owner, bool IsDoInputBinding, double left, double top, bool IsHandleMouseEvents)
        {
            try
            {
                win.Height = height;
                win.Width = width;
                //win.Owner = owner;
                win.Left = left;
                win.Top = top;
                if (IsDoInputBinding)
                    win.InputBindings.Add(new KeyBinding((ICommand)TryFindResource("DoNothingCommandRefrence"), Key.F4, ModifierKeys.Alt));
                if (IsHandleMouseEvents)
                {
                    win.MouseEnter += new MouseEventHandler(win_MouseEnterLeave);
                    win.MouseLeave += new MouseEventHandler(win_MouseEnterLeave);
                    win.PreviewMouseDown += new MouseButtonEventHandler(OpenglWin_PreviewMouseDown);
                    win.PreviewMouseUp += new MouseButtonEventHandler(OpenglWin_PreviewMouseUp);
                    win.PreviewMouseMove += new MouseEventHandler(OpenglWin_PreviewMouseMove);
                    win.PreviewMouseWheel += new MouseWheelEventHandler(OpenglWin_PreviewMouseWheel);
                    win.MouseDoubleClick += new MouseButtonEventHandler(OpenglWin_MouseDoubleClick);
                    if (win == RcadWin)
                    {
                        //win.IsManipulationEnabled = true;
                        //win.ManipulationDelta += new EventHandler<ManipulationDeltaEventArgs>(OpenglWin_ManipulationDelta);
                        win.TouchDown += new EventHandler<TouchEventArgs>(MainView_TouchDown);
                        win.TouchUp += new EventHandler<TouchEventArgs>(MainView_TouchUp);
                        win.TouchMove += new EventHandler<TouchEventArgs>(MainView_TouchMove);
                    }

                }
                win.PreviewKeyUp += new KeyEventHandler(MainView_PreviewKeyUp);
                win.PreviewKeyDown += new KeyEventHandler(MainView_PreviewKeyDown);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0119", ex); }
        }

        void MainView_ShowHideStatusWindow(string StatusMessage, bool showpanel, bool AllowCancel)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    if (StatusMessageWin == null)
                        StatusMessageWin = new MessageWin();
                    StatusMessageWin.txtMessage.Text = StatusMessage;
                    if (AllowCancel)
                        StatusMessageWin.ShowCancelButton();
                    else
                        StatusMessageWin.HideCancelButton();

                    if (showpanel) StatusMessageWin.ShowDialog();
                    else StatusMessageWin.Hide();
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MainInterface.WaitMessageEventHandler(MainView_ShowHideStatusWindow), StatusMessage, showpanel, AllowCancel);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0124", ex);
            }
        }
        public void SnapStateButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (SnapStateButton.ToolTip.ToString() == "Snap OFF")
                {
                    RWrapper.RW_MainInterface.MYINSTANCE().ToggleSnapeMode();
                    ModeIndicatorRect.Fill = SnapOn;
                    SnapStateButton.ToolTip = "Snap ON";
                }
                else if (SnapStateButton.ToolTip.ToString() == "Snap ON")
                {
                    RWrapper.RW_MainInterface.MYINSTANCE().ToggleVSnapeMode();
                    ModeIndicatorRect.Fill = VSnapOn;
                    SnapStateButton.ToolTip = "VSnap ON";
                }
                else if (SnapStateButton.ToolTip.ToString() == "VSnap ON")
                {
                    RWrapper.RW_MainInterface.MYINSTANCE().ToggleSnapeMode();
                    RWrapper.RW_MainInterface.MYINSTANCE().ToggleVSnapeMode();
                    ModeIndicatorRect.Fill = SnapOff;
                    SnapStateButton.ToolTip = "Snap OFF";
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0126", ex);
            }
        }

        void UpdateOnSeparateThread()
        {
            try
            {
                int period = 1000;
                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.VisionHeadOnly) period = 50;

                System.Windows.Threading.DispatcherTimer dt = new System.Windows.Threading.DispatcherTimer(TimeSpan.FromMilliseconds(period),
                System.Windows.Threading.DispatcherPriority.Loaded, new EventHandler(StartUpdate), this.Dispatcher);
              //  dt.Start();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0011", ex); }
        }
        int count = 0;
        void StartUpdate(object obj, EventArgs e)
        {
            try
            {
                //RWrapper.RW_MainInterface.MYINSTANCE().UpdateVideoGraphics();
                if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.VisionHeadOnly)
                {
                    RWrapper.RW_MainInterface.MYINSTANCE().UpdateRCadGraphics();
                    RWrapper.RW_MainInterface.MYINSTANCE().UpdateDxfGraphics();
                    System.Windows.Threading.DispatcherTimer dt = (System.Windows.Threading.DispatcherTimer)obj;
                        dt.Stop();
                }
                else
                {
                    this.Show(); count++;
                    if (count >= 5)
                    {
                        System.Windows.Threading.DispatcherTimer dt = (System.Windows.Threading.DispatcherTimer)obj;
                        dt.Stop();
                        count = 0;
                    }
                    RWrapper.RW_MainInterface.MYINSTANCE().UpdateVideoGraphics();
                }

                //RWrapper.RW_Thread.MYINSTANCE().UpdateThreadGraphics();
                //count += 1;
                //if (count >= 3)
                //{
                //    System.Windows.Threading.DispatcherTimer dt = (System.Windows.Threading.DispatcherTimer)obj;
                //    dt.Stop();
                //}
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0130", ex);
            }
        }

        #region Enable Disable controls Functions

        void EnableDisableControlsDuringPPBuild(bool Enable)
        {
            try
            {
                MeasureScrollList.MainPanel.IsEnabled = Enable;
                DrawScrollList.MainPanel.IsEnabled = Enable;
                DerivedShapeScrollList.MainPanel.IsEnabled = Enable;
                CrosshairScrollList.MainPanel.IsEnabled = Enable;
                EditScrollList.MainPanel.IsEnabled = Enable;
                ToolsScrollList.MainPanel.IsEnabled = Enable;
                UCSScrollList.MainPanel.IsEnabled = Enable;
                FocusPopUp.IsEnabled = Enable;
                ThreadPopup.IsEnabled = Enable;
                FixedGraphicsPopup.IsEnabled = Enable;
                lightingPopup.IsEnabled = Enable;
                if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.DSP &&
                    GlobalSettings.RapidMachineType != GlobalSettings.MachineType.Magnifier_DSP &&
                    GlobalSettings.RapidMachineType != GlobalSettings.MachineType.Horizontal_DSP &&
                    GlobalSettings.RapidMachineType != GlobalSettings.MachineType.AutoFocus)
                    CNCPopUp.IsEnabled = Enable;
                ShapeParameterWin.IsEnabled = Enable;
                MeasurementsPopup.IsEnabled = Enable;
                RcadToolbar.GetContent().IsEnabled = Enable;
                DXFToolbar.GetContent().IsEnabled = Enable;
                DROPanel.IsEnabled = Enable;
                DropDownPanel.IsEnabled = Enable;
                OtherWinPanel.IsEnabled = Enable;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0135", ex);
            }
        }
        void EnableDisableControlsDuringProgramRun(bool Enable, bool EnableCrosshair = false)
        {
            try
            {
                MeasureScrollList.MainPanel.IsEnabled = Enable;
                DrawScrollList.MainPanel.IsEnabled = Enable;
                DerivedShapeScrollList.MainPanel.IsEnabled = Enable;
                CrosshairScrollList.MainPanel.IsEnabled = Enable;
                EditScrollList.MainPanel.IsEnabled = Enable;
                ToolsScrollList.MainPanel.IsEnabled = Enable;
                UCSScrollList.MainPanel.IsEnabled = Enable;
                FocusPopUp.IsEnabled = Enable;
                ThreadPopup.IsEnabled = Enable;
                FixedGraphicsPopup.IsEnabled = Enable;
                //FDepthPopup.IsEnabled = Enable;
                lightingPopup.IsEnabled = Enable;
                if (EnableCrosshair)
                {
                    CrosshairScrollList.MainPanel.IsEnabled = EnableCrosshair;
                }
                if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.DSP &&
                    GlobalSettings.RapidMachineType != GlobalSettings.MachineType.Magnifier_DSP &&
                    GlobalSettings.RapidMachineType != GlobalSettings.MachineType.Horizontal_DSP &&
                    GlobalSettings.RapidMachineType != GlobalSettings.MachineType.AutoFocus)
                    CNCPopUp.IsEnabled = Enable;
                ShapeParameterWin.IsEnabled = Enable;
                MeasurementsPopup.IsEnabled = Enable;
                for (int i = 0; i < RcadToolbar.Children.Count; i++)
                    if (RcadToolbar.Children[i].GetType() == typeof(ScrToolbar))
                        ((ScrToolbar)RcadToolbar.Children[i]).MainPanel.IsEnabled = Enable;
                for (int i = 0; i < DXFToolbar.Children.Count; i++)
                    if (DXFToolbar.Children[i].GetType() == typeof(ScrToolbar))
                        ((ScrToolbar)DXFToolbar.Children[i]).MainPanel.IsEnabled = Enable;
                XdroGrid.IsEnabled = Enable;
                YdroGrid.IsEnabled = Enable;
                ZdroGrid.IsEnabled = Enable;
                RdroGrid.IsEnabled = Enable;
                btnA.IsEnabled = Enable;
                MagnificationList.IsEnabled = Enable;
                mmInchesList.IsEnabled = Enable;
                UCSList.IsEnabled = Enable;
                SnapStateButton.IsEnabled = Enable;
                PartProgramPopUp.ContainerGrid.IsEnabled = Enable;
                //PartProgramPopUp.btnNewReportStyle.IsEnabled = Enable;
                //PartProgramPopUp.btnNewReport.IsEnabled = Enable;
                //PartProgramPopUp.lstReportStyles.IsEnabled = Enable;
                //PartProgramPopUp.lstReports.IsEnabled = Enable;
                for (int i = 0; i < stOptions.Children.Count; i++)
                {
                    stOptions.Children[i].IsEnabled = Enable;
                }
                for (int i = 0; i < St.Children.Count; i++)
                {
                    if (St.Children[i].GetType() != typeof(Image))
                    {
                        string cBtnName;
                        ContentControl c = (ContentControl)St.Children[i];
                        if (c.Tag.GetType() == typeof(string[]))
                        {
                            cBtnName = ((string[])c.Tag)[2];
                            //if (cBtnName == "Run" || ((string[])c.Tag)[2] == "Pause" || ((string[])c.Tag)[2] == "Continue" || ((string[])c.Tag)[2] == "Stop")
                            //    continue;
                        }
                        else
                        {
                            cBtnName = ((ButtonProperties)c.Tag).ButtonName;
                        }
                        switch (cBtnName)
                        {
                            case "Run":
                            case "Pause":
                            case "Continue":
                            case "Stop":
                                continue;
                        }

                        c.IsEnabled = Enable;
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0136", ex);
            }
        }

        #endregion

        void CheckPermission(string str)
        {
            try
            {
                if (IsAdmin)
                {
                    RWrapper.RW_MainInterface.MYINSTANCE().HandleToolsToolbar_Click(str);
                }
                else
                {
                    SettingsLoginWindow.txtInfo.Text = "Please login as user with Admin Privilege";
                ShowLoginWindow:
                    SettingsLoginWindow.txtPassword.Password = "";
                    string EnteredUserName = SettingsLoginWindow.LoggedInUser;
                    SettingsLoginWindow.UserTabletList.Children.Clear();
                    foreach (string sr in SettingsLoginWindow.LoginDetailsNameColl)
                    {
                        RadioButton Rb = new RadioButton();
                        Rb.Style = UserTabletBtnStyle;
                        Rb.Height = Rb.Width = 120;
                        Rb.Content = sr;
                        Rb.Tag = sr;
                        Rb.Click += new RoutedEventHandler(SettingsLoginWindow.HandleUserTabletClick);
                        SettingsLoginWindow.UserTabletList.Children.Add(Rb);
                    }
                    SettingsLoginWindow.LoggedInUser = EnteredUserName;
                    SettingsLoginWindow.ShowDialog();
                    if (SettingsLoginWindow.InfoStatus)
                    {
                        string username = SettingsLoginWindow.LoggedInUser;
                        string password = SettingsLoginWindow.txtPassword.Password;
                        //System.Collections.Hashtable loginDetails = RWrapper.RW_DBSettings.MYINSTANCE().LoginDetailsCollection;
                        //System.Collections.Hashtable loginPreviliges = RWrapper.RW_DBSettings.MYINSTANCE().LoginPrivilage;
                        if (SettingsLoginWindow.LoginDetailsNameColl.Contains(username))
                        {
                            if (password == SettingsLoginWindow.getPassword(username))
                            {
                                //Check if system has logged in then show another button into the tools dropdown for the calibation window.
                                if (SettingsLoginWindow.getPrevilegeLevel(username) == "Admin")
                                {
                                    RWrapper.RW_MainInterface.MYINSTANCE().HandleToolsToolbar_Click(str);
                                }
                                else
                                {
                                    SettingsLoginWindow.txtInfo.Text = "Please login as user with Admin Privilege";
                                    goto ShowLoginWindow;
                                }
                            }
                            else
                            {
                                SettingsLoginWindow.txtInfo.Text = "Entered 'Password' is wrong.Please try again.";
                                goto ShowLoginWindow;
                            }
                        }
                        else
                        {
                            SettingsLoginWindow.txtInfo.Text = "Entered username does'nt exist.Please try again.";
                            goto ShowLoginWindow;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0137", ex);
            }
        }

        bool IsProgramNamesValid(string PrgName, string RptName, string RptStyle)
        {
            try
            {
                if (GlobalSettings.MainExcelInstance.CheckNames(PrgName, 1))
                {
                    MessageBox.Show("The entered Program Name already exists. Please change it.", "Rapid I");
                    return false;
                }
                if (GlobalSettings.MainExcelInstance.CheckNames(RptStyle, 2))
                {
                    MessageBox.Show("The entered Report Style Name already exists. Please change it.", "Rapid I");
                    return false;
                }
                if (GlobalSettings.MainExcelInstance.CheckNames(RptName, 3))
                {
                    MessageBox.Show("The entered Report Name already exists. Please change it.", "Rapid I");
                    return false;
                }
                return true;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0138", ex);
                return false;
            }
        }

        double ToolBoxWidth = 0;
        void ArrangeForPartProgramRun()
        {
            try
            {
                if (!OtherWinToolbarRotated && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.ProbeOnly && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HeightGauge)
                {
                    if (!(bool)RapidCADRBtn.IsChecked)
                        RapidCADRBtn.IsChecked = true;

                    ProgramRBtn.IsEnabled = false;
                    //DXFRBtn.IsEnabled = false
                    PicViewerRBtn.IsEnabled = false;

                    //RcadWin.Top = RcadTop - 57;
                    //RcadWin.Height += 60;
                    //RWrapper.RW_MainInterface.MYINSTANCE().ResizetheWindow(1, (int)RcadWin.Width, (int)RcadWin.Height);
                    //RcadWin.Show();
                }
                else if (OtherWinToolbarRotated)
                {
                    if ((bool)ProgramRBtn.IsChecked)
                        ProgramRBtn.IsChecked = false;
                    ProgramRBtn.IsEnabled = false;
                    SetOwnerofWindow(ToolBoxHolder);
                    ToolBoxWidth = ToolBoxHolder.Width;
                    ToolBoxHolder.Width = ToolBoxWidth + 150;
                    //VideoDisplayGrpBx.Content = null;
                    //ToolBoxHolder.LowerPanelWinContainer.Content = PartProgramPopUp;
                    //PartProgramPopUp.Visibility = System.Windows.Visibility.Visible;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:139", ex);
            }
        }

        void ArrangeForPartProgramStop()
        {
            try
            {
                if (!OtherWinToolbarRotated && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.ProbeOnly && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.HeightGauge)
                {
                    ProgramRBtn.IsEnabled = true;
                    //DXFRBtn.IsEnabled = true;
                    PicViewerRBtn.IsEnabled = true;

                    //RcadWin.Top = RcadTop;
                    //RcadWin.Height -= 60;

                    //RWrapper.RW_MainInterface.MYINSTANCE().ResizetheWindow(1, (int)RcadWin.Width, (int)RcadWin.Height);
                    //RcadWin.Hide();
                }
                else if (OtherWinToolbarRotated)
                {
                    ToolBoxHolder.Width = ToolBoxWidth;
                    ToolBoxHolder.LowerPanelWinContainer.Content = LeftPanelGrid;
                    //SetOwnerofWindow(ToolBoxHolder);
                    //ToolBoxHolder.Visibility = System.Windows.Visibility.Visible;
                    PartProgramPopUp.Visibility = System.Windows.Visibility.Hidden;
                    //VideoDisplayGrpBx.Content = PartProgramPopUp;
                    ProgramRBtn.IsEnabled = true;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:140", ex);
            }
        }

        void Form1_OnErrorHappen(string Msg, Exception ex)
        {
            RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("ExcelError:" + Msg, ex);
        }

        void VideoFormsWinSet(int w, int h)
        {
            //VideoFormsWin.SetWinSize((int)Math.Round(Videowidth), (int)Math.Round(Videoheight));
            VideoHolder.Width = w; VideoHolder.Height = h;
            //VideoFormsWin.SetWinPosition((int)Math.Round(VideoLeft), (int)Math.Round(VideoTop));
        }

        void DigitalZoomOn()
        {
            if (!GlobalSettings.DoubleImageEnabled_During_DigiZoom)
            {
                return;
            }
            //VideoFormsWin.SetImageViewerSize((int)(Videowidth * 2), (int)(Videoheight * 2));
            //VideoTestWin.AutoScrollPosition = new System.Drawing.Point((int)(Videowidth), (int)(Videoheight));
            RWrapper.RW_MainInterface.MYINSTANCE().ResizetheWindow(0, (int)(Videowidth * 2), (int)(Videoheight * 2));
            RWrapper.RW_MainInterface.MYINSTANCE().UpdateVideoGraphics();

        }
        void DigitalZoomOff()
        {
            if (!GlobalSettings.DoubleImageEnabled_During_DigiZoom) return;
            //VideoFormsWin.SetImageViewerSize((int)(Videowidth), (int)(Videoheight));
            RWrapper.RW_MainInterface.MYINSTANCE().ResizetheWindow(0, (int)(Videowidth), (int)(Videoheight));
            RWrapper.RW_MainInterface.MYINSTANCE().UpdateVideoGraphics();
        }

        #region Machine Control
        void ToolsCheckboxBtnClick(object sender, RoutedEventArgs e)
        {
            try
            {
                string BtnTag = ((RadioButton)sender).Tag.ToString();
                RapidDropDownControl RDD = (RapidDropDownControl)((VisualTreeHelper.GetParent((RadioButton)(sender))));
                if (BtnTag == "MMC")
                {
                    RWrapper.RW_MainInterface.MYINSTANCE().SetMMCStatus(true);
                }
                else if (BtnTag == "LMC")
                {
                    RWrapper.RW_MainInterface.MYINSTANCE().SetMMCStatus(false);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV57", ex);
            }
        }
        protected void HeaderButtonClick(object sender, EventArgs e)
        {
            try
            {
                RapidDropDownControl MCRCtrl = sender as RapidDropDownControl;
                if (!MCRCtrl.SwitchOnColor)
                {
                    MCRCtrl.SwitchOnColor = true;
                    RWrapper.RW_MainInterface.MYINSTANCE().SetMMCOnOffStatus(true);
                }
                else
                {
                    if (MCRCtrl.Header == "MC")
                        return;
                    //MCRCtrl.Header = "MC";
                    //MCRCtrl.SwitchOnColor = false;
                    //MCRCtrl.PopupContainer.IsOpen = false;
                    //RWrapper.RW_MainInterface.MYINSTANCE().SetMMCOnOffStatus(false);
                    //for (int i = 0; i < MCRCtrl.Scr.Items.Count; i++)
                    //{
                    //    if (MCRCtrl.Scr.Items[i].GetType() == typeof(RadioButton))
                    //    {
                    //        RadioButton Rbtn = (RadioButton)MCRCtrl.Scr.Items[i];
                    //        Rbtn.IsChecked = false;
                    //    }
                    //}
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0589", ex);
            }
        }
        protected void ClearRDDMaterialControl(object sender, EventArgs e)
        {
            try
            {
                RapidDropDownControl MCRCtrl = sender as RapidDropDownControl;
                MCRCtrl.Header = "MC";
                MCRCtrl.SwitchOnColor = false;
                MCRCtrl.PopupContainer.IsOpen = false;
                RWrapper.RW_MainInterface.MYINSTANCE().SetMMCOnOffStatus(false);
                for (int i = 0; i < MCRCtrl.Scr.Items.Count; i++)
                {
                    if (MCRCtrl.Scr.Items[i].GetType() == typeof(RadioButton))
                    {
                        RadioButton Rbtn = (RadioButton)MCRCtrl.Scr.Items[i];
                        Rbtn.IsChecked = false;
                    }
                }
            }
            catch (Exception)
            {
                throw;
            }
        }
        #endregion

        #region Generate MCS String
        string GenerateMCSString()
        {
            string returnablestring = "MCS | ";
            if (GlobalSettings.CurrentMSCType == GlobalSettings.MCSTYPE.IXZ)
            {
                returnablestring += "-XZ";
            }
            else if (GlobalSettings.CurrentMSCType == GlobalSettings.MCSTYPE.IYZ)
            {
                returnablestring += "-YZ";
            }
            else
                returnablestring += GlobalSettings.CurrentMSCType.ToString();
            return returnablestring;
        }
        #endregion

        #region Scan mode of/off
        void ScanOnOffBtn_Click(object sender, RoutedEventArgs e)
        {
            if (!ScanStarted)
            {
                ScanStarted = true;
                ScanOnOffBtn.Content = "Scan";
                //ScanOnOffBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32);
                ScanOnOffBtn.ToolTip = "Scan";
                ScanOnOffBtn.Tag = "Scan Start";
                ModeIndicatorRectForScanOnOff.Fill = ScanOn;
                EnableDisableControlsDuringScanMode(false);
                RWrapper.RW_MainInterface.MYINSTANCE().Start_StopRotationalScan(true);
            }
            else
            {
                ScanStarted = false;
                ScanOnOffBtn.Content = "Scan";
                //ScanOnOffBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Stop", "Part Program", 32, 32);
                ScanOnOffBtn.ToolTip = "Scan";
                ScanOnOffBtn.Tag = "Scan Stop";
                ModeIndicatorRectForScanOnOff.Fill = ScanOff;
                EnableDisableControlsDuringScanMode(true);
                RWrapper.RW_MainInterface.MYINSTANCE().Start_StopRotationalScan(false);
            }
        }
        void EnableDisableControlsDuringScanMode(bool Enable)
        {
            try
            {
                MeasureScrollList.MainPanel.IsEnabled = Enable;
                DrawScrollList.MainPanel.IsEnabled = Enable;
                DerivedShapeScrollList.MainPanel.IsEnabled = Enable;
                CrosshairScrollList.MainPanel.IsEnabled = Enable;
                EditScrollList.MainPanel.IsEnabled = Enable;
                ToolsScrollList.MainPanel.IsEnabled = Enable;
                UCSScrollList.MainPanel.IsEnabled = Enable;
                FocusPopUp.IsEnabled = Enable;
                ThreadPopup.IsEnabled = Enable;
                FixedGraphicsPopup.IsEnabled = Enable;
                lightingPopup.IsEnabled = Enable;
                if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.DSP &&
                    GlobalSettings.RapidMachineType != GlobalSettings.MachineType.Magnifier_DSP &&
                    GlobalSettings.RapidMachineType != GlobalSettings.MachineType.Horizontal_DSP &&
                    GlobalSettings.RapidMachineType != GlobalSettings.MachineType.AutoFocus)
                    CNCPopUp.IsEnabled = Enable;
                ShapeParameterWin.IsEnabled = Enable;
                MeasurementsPopup.IsEnabled = Enable;
                for (int i = 0; i < RcadToolbar.Children.Count; i++)
                    if (RcadToolbar.Children[i].GetType() == typeof(ScrToolbar))
                        ((ScrToolbar)RcadToolbar.Children[i]).MainPanel.IsEnabled = Enable;
                for (int i = 0; i < DXFToolbar.Children.Count; i++)
                    if (DXFToolbar.Children[i].GetType() == typeof(ScrToolbar))
                        ((ScrToolbar)DXFToolbar.Children[i]).MainPanel.IsEnabled = Enable;
                XdroGrid.IsEnabled = Enable;
                YdroGrid.IsEnabled = Enable;
                ZdroGrid.IsEnabled = Enable;
                RdroGrid.IsEnabled = Enable;
                btnA.IsEnabled = Enable;
                DropDownPanel.IsEnabled = Enable;
                PartProgramPopUp.ContainerGrid.IsEnabled = Enable;
                for (int i = 0; i < stOptions.Children.Count; i++)
                {
                    stOptions.Children[i].IsEnabled = Enable;
                }
                for (int i = 0; i < St.Children.Count; i++)
                {
                    if (St.Children[i].GetType() != typeof(Image))
                    {
                        string cBtnName;
                        ContentControl c = (ContentControl)St.Children[i];
                        if (c.Tag.GetType() == typeof(string[]))
                            cBtnName = ((string[])c.Tag)[2];
                        else
                            cBtnName = ((ButtonProperties)c.Tag).ButtonName;
                        switch (cBtnName)
                        {
                            case "Run":
                            case "Pause":
                            case "Continue":
                            case "Stop":
                                continue;
                        }

                        //ContentControl c = (ContentControl)St.Children[i];
                        //if (((string[])c.Tag)[2] == "Run" || ((string[])c.Tag)[2] == "Pause" || ((string[])c.Tag)[2] == "Continue" || ((string[])c.Tag)[2] == "Stop")
                        //    continue;
                        c.IsEnabled = Enable;
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0136", ex);
            }
        }
        #endregion

        #endregion

        #region SphereCaliberation event
        void MainView_PointsCountEvent(int achieved, int total)
        {
            try
            {
                TxtGridRunCount.Text = achieved.ToString() + "/" + total.ToString();
                ListBoxItem cbi = (ListBoxItem)StarProbeCalibWin.ProbeTypeSelectionCBx.SelectedItem;
                cbi.Background = new SolidColorBrush(Colors.PaleGreen);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0588", ex);
            }
        }

        void MainView_FinishedSphereUpdate()
        {
            try
            {
                MessageBox.Show("Finished Sphere Measurement!", "Rapid-I 5.0");
                StarProbeCalibWin.StopBtn_Click(null, null); 
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0590", ex);
            }
        }

        void MainView_ProbeOffsetEvent(double[] ProbeToVisionOffset, int mode)
        {
            if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
            {
                System.Data.DataRow[] Drow = GlobalSettings.SettingsReader.GetRowsAccordingToCurrentMachineNumber("ProbeSettings");
                Drow[0]["ProbeOffsetX"] = ProbeToVisionOffset[0];
                Drow[0]["ProbeOffsetY"] = ProbeToVisionOffset[1];
                Drow[0]["ProbeOffsetZ"] = ProbeToVisionOffset[2];
                GlobalSettings.SettingsReader.UpdateTable("ProbeSettings");
            }
            else
                this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                   new RWrapper.RW_SphereCalibration.ProbeOffsetEventHandler(MainView_ProbeOffsetEvent), ProbeToVisionOffset, mode);
        }
        #endregion

        private void BtnInsertImage_Click(object sender, RoutedEventArgs e)
        {
            //if (!GlobalSettings.VideoWindowInitialised)
            //{
                System.Windows.Forms.OpenFileDialog ofd = new System.Windows.Forms.OpenFileDialog();
                ofd.Filter = "BMP|*.bmp|GIF|*.gif|JPG|*.jpg;*.jpeg|PNG|*.png|TIFF|*.tif;*.tiff";
                ofd.ShowDialog();
                if (ofd.FileName != "")
                {
                    System.Drawing.Bitmap bmp = new System.Drawing.Bitmap(ofd.FileName);
                    System.Drawing.Rectangle bmpRect = new System.Drawing.Rectangle(0, 0, bmp.Width, bmp.Height);
                    System.Drawing.Imaging.BitmapData bmd = bmp.LockBits(bmpRect, System.Drawing.Imaging.ImageLockMode.ReadWrite, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
                    GlobalSettings.StaticImageinVideo = new byte[bmp.Width * bmp.Height * 4];
                    System.Runtime.InteropServices.Marshal.Copy(bmd.Scan0, GlobalSettings.StaticImageinVideo, 0, bmp.Width * bmp.Height * 4);
                    bmp.UnlockBits(bmd);
                    //VideoFormsWin.pictureBox1.Image = bmp;
                    RWrapper.RW_MainInterface.MYINSTANCE().GetInsertedImageBytes(true, GlobalSettings.StaticImageinVideo);
                }
            //}
        }

        private void SaveBinarisedImageClick(object sender, RoutedEventArgs e)
        {
            System.Drawing.Bitmap bmp = new System.Drawing.Bitmap(800, 600, System.Drawing.Imaging.PixelFormat.Format24bppRgb);
            System.Drawing.Rectangle bmpRect = new System.Drawing.Rectangle(0, 0, bmp.Width, bmp.Height);
            System.Drawing.Imaging.BitmapData bmd = bmp.LockBits(bmpRect, System.Drawing.Imaging.ImageLockMode.ReadWrite, System.Drawing.Imaging.PixelFormat.Format24bppRgb);
            byte[] tempImage = new byte[800 * 600 * 3];
            RWrapper.RW_MainInterface.MYINSTANCE().GetBinarizedImage(tempImage);
            System.Runtime.InteropServices.Marshal.Copy(tempImage, 0, bmd.Scan0, bmp.Width * bmp.Height * 3);
            bmp.UnlockBits(bmd);
            bmp.Save(Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Log\\UserLog\\binarisedImage.bmp");
            MessageBox.Show("Saved the Image to Userlog folder");

        }

        private void LowerPanelHolder2_MouseEntered(object sender, MouseEventArgs e)
        {
            if (GlobalSettings.Light_Zone_Nos[0] > 8)
            {
                LowerPanelWin2.ShowWindow();
            }
        }
        //System.Threading.Timer CameraTicker;

        bool InvalidRapidCam3Present, CamDevicePresent;
        private bool Program_RapidCam3(bool MsgBoxFlag)
        {
            try
            {
                //int CamType = 1;
                //bool result = false;
                //MessageBox.Show("Rc3_1");
                // 1. Check all the devices in the USB hub. See if we have a device to program...
                System.Management.ManagementObjectCollection collection;
                using (var searcher = new System.Management.ManagementObjectSearcher(@"Select * From Win32_USBHub"))
                    collection = searcher.Get();
                string tempstr;
                //MessageBox.Show("Rc3_2");
                string logfile = RWrapper.RW_MainInterface.MYINSTANCE().AppDataFolderPath + "\\Log\\UserLog\\USBList.txt";
                if (System.IO.File.Exists(logfile)) System.IO.File.Delete(logfile);
                StreamWriter sw = new StreamWriter(logfile);
                InvalidRapidCam3Present = false;
                CamDevicePresent = false;
                // Iterate through each device and see if we have a programmed device
                foreach (System.Management.ManagementObject device in collection)
                {
                    tempstr = (string)device.GetPropertyValue("DeviceID");
                    sw.WriteLine(tempstr);
                    
                    //lbUSBDeviceList.Items.Add(tempstr);
                    if (tempstr.Contains("VID_0000&PID_0002"))
                    {
                        InvalidRapidCam3Present = true; CamDevicePresent = true;
                    }
                    else if (tempstr.Contains("VID_04B4&PID_00F3"))
                    {
                        InvalidRapidCam3Present = false; CamDevicePresent = true; //CameraTicker.Dispose();
                    }
                    else if (tempstr.Contains("VID_04B4&PID_00F9") || tempstr.Contains("VID_04B4&PID_00FA"))
                    {
                        return true; //WE have a programmed RapidCam3 already. So do not do anything!!!
                    }
                    //invalidUSBDeviceID = tempstr;
                }
                sw.Close();
                bool ProgrammedSuccesfully = false;
                bool OperatedPinsProperly = false;
                //MessageBox.Show("Rc3_3");
                //MessageBox.Show(CamDevicePresent.ToString() + "  Invalid Device: " + InvalidRapidCam3Present.ToString());
                if (CamDevicePresent)
                {
                    if (InvalidRapidCam3Present)
                    {
                        MessageBoxResult mbr = MessageBoxResult.Cancel;
                        if (MsgBoxFlag)
                        {
                            mbr = MessageBox.Show("Please switch on the Camera with the green button in Rapid-I machine and then press OK", "Rapid-I 5.0", MessageBoxButton.OKCancel, MessageBoxImage.Information);
                            if (mbr == MessageBoxResult.Cancel)
                                return false;
                        }
                    }
                }
                //else
                //    return false;
                try
                {
                    if (!RWDRO_Instantiated) return false;
                    //MessageBox.Show("Rc3_4");

                    OperatedPinsProperly = RWrapper.RW_MainInterface.MYINSTANCE().OperateSpecialPins(2, true);
                    if (!OperatedPinsProperly) return false;
                    //MessageBox.Show("Rc3_5");
                    Thread.Sleep(2000);
                    OperatedPinsProperly = RWrapper.RW_MainInterface.MYINSTANCE().OperateSpecialPins(2, false);
                    if (!OperatedPinsProperly) return false;
                    //MessageBox.Show("Rc3_6");
                    Thread.Sleep(1000);
                }
                catch (Exception ex3)
                {
                    MessageBox.Show(ex3.Message);
                    RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0591a", ex3);
                    return false;
                }

                RCam3.CamProgrammer cP = new RCam3.CamProgrammer();
                switch (GlobalSettings.RapidCameraType)
                {
                    case GlobalSettings.CameraType.TenM:
                    case GlobalSettings.CameraType.FiveFourtyP:
                        ProgrammedSuccesfully = cP.Program_Cam(10);
                        break;
                    default:
                        ProgrammedSuccesfully = cP.Program_Cam(2);
                        break;
                }
                    //( == GlobalSettings.CameraType.TenM ||) 
                    //if (GlobalSettings.RapidCameraType == GlobalSettings.CameraType.TwoM_Double)
                //else 
                if (ProgrammedSuccesfully) System.Threading.Thread.Sleep(5000);

                //else
                //{

                //        //CameraTicker = new Timer(new TimerCallback(CameraTickEventHandler), null, 0, 2000);

                //    }
                //}

                //if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Horizontal)
                //    result = cP.Program_Cam(CamType, true, false);
                //else
                //    result =  cP.Program_Cam(CamType, true, false);

                //if (File.Exists(AppDomain.CurrentDomain.BaseDirectory + "\\CyUSB.dll"))
                //    File.Delete(AppDomain.CurrentDomain.BaseDirectory + "\\CyUSB.dll");

                //CyUSB.USBDeviceList usbDevices = new CyUSB.USBDeviceList(CyUSB.CyConst.DEVICES_CYUSB);
                //CyUSB.CyFX3Device fx;
                //foreach (CyUSB.USBDevice uDevice in usbDevices)
                //{
                //    CyUSB.CyUSBDevice chosenDevice = uDevice as CyUSB.CyUSBDevice;
                //    //if (chosenDevice.bSuperSpeed)
                //    //{                //USB_DEVICE_DESCRIPTOR descriptor = new USB_DEVICE_DESCRIPTOR();
                //    if (chosenDevice.FriendlyName.Contains("Cypress FX3"))
                //    {
                //        fx = chosenDevice as CyUSB.CyFX3Device;
                //        // check for bootloader first, if it is not running then prompt message to user.
                //        if (!fx.IsBootLoaderRunning())
                //        {
                //            MessageBox.Show("Please reset your device to download firmware", "Bootloader is not running");
                //            return false;
                //        }
                //        CyUSB.FX3_FWDWNLOAD_ERROR_CODE enmResult = CyUSB.FX3_FWDWNLOAD_ERROR_CODE.FAILED;
                //        enmResult = fx.DownloadFw(AppDomain.CurrentDomain.BaseDirectory + "\\rcam3.dll", CyUSB.FX3_FWDWNLOAD_MEDIA_TYPE.RAM);
                //        //fx.Reset();
                //        //fx.ReConnect();
                //        //fx.IsBootLoaderRunning();
                //        System.Threading.Thread.Sleep(5000);
                //        if (enmResult == CyUSB.FX3_FWDWNLOAD_ERROR_CODE.SUCCESS)
                //            return true;
                //        else
                //            return false;
                //        //break;
                //    }
                //}
                //chosenDevice.GetDeviceDescriptor(ref descriptor);
                //if (descriptor.iManufacturer == 4)
                //}
                return ProgrammedSuccesfully;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0591", ex);
            }
            return false;
        }
        private void CameraTickEventHandler(object obj) //(object sender, EventArgs e)
        {
            try
            {
                System.Management.ManagementObjectCollection collection;
                using (var searcher = new System.Management.ManagementObjectSearcher(@"Select * From Win32_USBHub"))
                    collection = searcher.Get();
                string tempstr;
                InvalidRapidCam3Present = false;
                CamDevicePresent = false;

                foreach (System.Management.ManagementObject device in collection)
                {
                    tempstr = (string)device.GetPropertyValue("DeviceID");
                    //lbUSBDeviceList.Items.Add(tempstr);
                    if (tempstr.Contains("VID_0000&PID_0002"))
                    {
                        InvalidRapidCam3Present = true; CamDevicePresent = true;
                    }
                    else if (tempstr.Contains("VID_04B4&PID_00F3"))
                    {
                        InvalidRapidCam3Present = false; CamDevicePresent = true; //CameraTicker.Dispose();
                    }
                        //invalidUSBDeviceID = tempstr;
                }
                if (CamDevicePresent)
                {
                    if (!InvalidRapidCam3Present)
                    {
                        //CameraTicker.Dispose();
                        this.Dispatcher.Invoke(new AsyncSimpleDelegate(Program_RapidCam3Again), System.Windows.Threading.DispatcherPriority.Normal);
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0591", ex);
            }

        }

        private void Program_RapidCam3Again()
        {
            //if (InvalidRapidCam3Present && CamDevicePresent)
            //{
            if (System.Environment.Is64BitOperatingSystem)
            {
                if (GlobalSettings.RapidCameraType == GlobalSettings.CameraType.TwoM) GlobalSettings.RapidCameraType = GlobalSettings.CameraType.TwoM_Double;
                Program_RapidCam3(false);
                RWrapper.RW_MainInterface.MYINSTANCE().HandleToolsToolbar_Click("Reset Video");
                GlobalSettings.RapidCameraType = GlobalSettings.CameraType.TwoM;
            }
            //}
        }


    }

    public class OldWindow : System.Windows.Forms.IWin32Window
    {
        IntPtr _handle;

        public OldWindow(IntPtr handle)
        {
            _handle = handle;
        }

        #region IWin32Window Members

        IntPtr System.Windows.Forms.IWin32Window.Handle
        {
            get { return _handle; }
        }

        #endregion
    }
}
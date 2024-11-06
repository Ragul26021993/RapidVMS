using System;
using System.IO;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;
using CalibrationModule;
using LicensingModuleDLL;
using System.Data;
using System.Xml;
using RapidI;


namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for SettingsWin.xaml
    /// </summary>
    public partial class SettingsWin : Window
    {
        DB Settings_DataB;
        string MachineNo;
        System.Data.DataRow[] CurrentMachineDataR, DROROWS, MagRows, FixtureRows, CamAxisRows;
        String[] TableNames;
        List<string> UpdateTableNames;
        bool NegletCBoxSelection = false;
        double UnitType_MulFactor;
        ObservableCollection<DxfAutoAlignPropEntity> DxfAlignProp_DGSourcing;
        int[] AxisNumberProperties = new int[24];
        bool[] AxisBooleanProperties = new bool[12];
        bool AllowCNC, AllowAutoZoom, ProbeMoveUpFlag, AllowInterpolation, SmoothCircularInterpolation, RouteProbePath, RetractProbePointMode;
        double ProbeRetractSpeed, ProbeRetractDistance, ProbeApproachSpeed, ProbeApproachDistance, ProbeSensitivity;
        Color RcadGradClrUp = new Color(), RcadGradClrDown = new Color();
        public bool UpdatingEEPROMValuesNow = false;
        DataTable TouchProbeApprRetractParams, ZoomCalibrationTable;
        private bool AddingItemsToAxisNumberCB;
        private ObservableCollection<UserDefinedProbType> _ProbeTypeColl;
        public MainView.AsyncSimpleDelegate FeedRateChangedEvent;

        public ObservableCollection<UserDefinedProbType> ProbeTypeColl
        {
            get
            {
                return _ProbeTypeColl;
            }
            set
            {
                _ProbeTypeColl = value;
            }
        }

        private ObservableCollection<RotaryMeasurementTypetails> _RotaryMeasurementColl;
        public ObservableCollection<RotaryMeasurementTypetails> RotaryMeasurementColl
        {
            get
            {
                return _RotaryMeasurementColl;
            }
            set
            {
                _RotaryMeasurementColl = value;
            }
        }

        #region Constructor and initialisation

        //Constructor
        public SettingsWin()
        {
            try
            {
                this.InitializeComponent();
                this.Window.Loaded += new RoutedEventHandler(Window_Loaded);
                CancelBtn.Click += new RoutedEventHandler(CancelBtn_Click);
                UpdateBtn.Click += new RoutedEventHandler(UpdateBtn_Click);
                ExportBtn.Click += new RoutedEventHandler(ExportBtn_Click);
                ImportBtn.Click += new RoutedEventHandler(ImportBtn_Click);
                Init();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW01", ex); }
        }
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            try
            {
                //=============================================================Colour Chooser Buttons=============================================================//
                int UprgbVal = Convert.ToInt32(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["RcadGradientBackColourTop"].ToString());
                int DownrgbVal = Convert.ToInt32(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["RcadGradientBackColourBottom"].ToString());
                Rectangle ModeIndicatorRectUp = (Rectangle)RcadBackGradTopColorBtn.Template.FindName("ModeIndicator", RcadBackGradTopColorBtn);
                Rectangle ModeIndicatorRectDown = (Rectangle)RcadBackGradBottomColorBtn.Template.FindName("ModeIndicator", RcadBackGradBottomColorBtn);
                RcadGradClrUp = InterpretColour(UprgbVal);
                RcadGradClrDown = InterpretColour(DownrgbVal);
                ModeIndicatorRectUp.Fill = new SolidColorBrush(RcadGradClrUp);
                ModeIndicatorRectDown.Fill = new SolidColorBrush(RcadGradClrDown);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW01a", ex); }
        }
        public void Init()
        {
            try
            {
                Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                Settings_DataB.FillTable("CurrentMachine");
                Settings_DataB.FillTable("Login");
                Settings_DataB.FillTable("LanguageTypeLookup");
                Settings_DataB.FillTable("MachineTypeLookup");
                Settings_DataB.FillTable("LicenseInfo");
                Settings_DataB.FillTable("FeatureLicenseInfo");
                Settings_DataB.FillTable("ProbeTouchSettings");
                Settings_DataB.FillTable("TIS_Settings");

                TableNames = new string[] {"UserSettings", "DXFSettings", "MeasurementSettings", "LightingSettings", "FixtureDetails",
                        "MachineDetails", "VisionSettings", "MagLevels", "MagnifierSettings", "DROSettings", "EEPROMSettings", "ProbeSettings",
                        "SurfaceFGSettings","CameraAxisValue", "OneShotParams"};
                Settings_DataB.FillTableRange(TableNames);
                //Get the current machine no and all the Dro Rows corresponding to that machine no;and also the mag levels Rows 
                MachineNo = GlobalSettings.MachineNo; // Settings_DataB.get_GetRecord("CurrentMachine", Settings_DataB.GetRowCount("CurrentMachine") - 1, "MachineNo").ToString();
                DROROWS = Settings_DataB.Select_ChildControls("DROSettings", "MachineNo", MachineNo);
                MagRows = Settings_DataB.Select_ChildControls("MagLevels", "MachineNo", MachineNo);
                FixtureRows = Settings_DataB.Select_ChildControls("FixtureDetails", "MachineNo", MachineNo);
                CamAxisRows = Settings_DataB.Select_ChildControls("CameraAxisValue", "MachineNo", MachineNo);

                if (GlobalSettings.CurrentUnitType == GlobalSettings.UnitType.mm)
                    UnitType_MulFactor = 1;
                else
                    UnitType_MulFactor = 25.4;

                MachineNumCBx.SelectionChanged += new SelectionChangedEventHandler(MachineNumCBx_SelectionChanged);
                UserNameCBx.SelectionChanged += new SelectionChangedEventHandler(UserNameCBx_SelectionChanged);
                MachineTypeCBx.SelectionChanged += new SelectionChangedEventHandler(MachineTypeCBx_SelectionChanged);
                DefaultMagLevelCBx.SelectionChanged += new SelectionChangedEventHandler(DefaultMagLevelCBx_SelectionChanged);
                FixtureNameCBx.SelectionChanged += new SelectionChangedEventHandler(FixtureNameCBx_SelectionChanged);
                UserNameDeleteBtn.Click += new RoutedEventHandler(UserNameDeleteBtn_Click);
                //MachineNumCBx.TextInput += new TextCompositionEventHandler(MachineNumCBx_TextInput);
                UserNameCBx.TextInput += new TextCompositionEventHandler(UserNameCBx_TextInput);
                UserNameSaveBtn.Click += new RoutedEventHandler(UserNameSaveBtn_Click);
                SubmitKeyBtn.Click += new RoutedEventHandler(SubmitLicenseKeyBtn_Click);
                FixtureSaveBtn.Click += new RoutedEventHandler(FixtureSaveBtn_Click);
                DxfAutoAlignSaveBtn.Click += new RoutedEventHandler(DxfAutoAlignSaveBtn_Click);
                RcadBackGradTopColorBtn.Click += new RoutedEventHandler(RcadBackGradTopColorBtn_Click);
                RcadBackGradBottomColorBtn.Click += new RoutedEventHandler(RcadBackGradBottomColorBtn_Click);
                ReadBinaryBtn.Click += new RoutedEventHandler(ReadBinaryBtn_Click);
                OneShotSettingsTab.Visibility = System.Windows.Visibility.Hidden;
                gbLightControl.Visibility = Visibility.Hidden;

                //Touch Probe Parameters 
                TouchProbeApprRetractParams = new DataTable();
                TouchProbeApprRetractParams.Columns.Add(new DataColumn("AxisName"));
                TouchProbeApprRetractParams.Columns.Add(new DataColumn("ApproachDist"));
                TouchProbeApprRetractParams.Columns.Add(new DataColumn("RetractDist"));
                TouchProbeApprRetractParams.Columns.Add(new DataColumn("ApproachSpeed"));
                TouchProbeApprRetractParams.Columns.Add(new DataColumn("RetractSpeed"));
                DataRow[] TempProbeSettings = Settings_DataB.Select_ChildControls("ProbeTouchSettings", "MachineNo", MachineNo);
                string[] axisnames = new string[] { "X", "Y", "Z", "R" };

                //if (TempProbeSettings.Length < GlobalSettings.NumberOfAxes)
                //{
                //    int entries = TempProbeSettings.Length;
                //    for (int jj = 0; jj < TempProbeSettings)
                //}

                for (int ii = 0; ii < GlobalSettings.NumberOfAxes; ii++)
                {
                    DataRow dr = TouchProbeApprRetractParams.NewRow();
                    if (TempProbeSettings.Length > ii)
                    {
                        dr["AxisName"] = TempProbeSettings[ii]["Axis_Name"];
                        dr["ApproachDist"] = TempProbeSettings[ii]["Approach_Dist"];
                        dr["RetractDist"] = TempProbeSettings[ii]["Retract_Dist"];
                        dr["ApproachSpeed"] = TempProbeSettings[ii]["Approach_Speed"];
                        dr["RetractSpeed"] = TempProbeSettings[ii]["Retract_Speed"];
                    }
                    else
                    {
                        dr["AxisName"] = axisnames[ii];
                        dr["ApproachDist"] = 1;
                        dr["RetractDist"] = 1;
                        dr["ApproachSpeed"] = 0.1;
                        dr["RetractSpeed"] = 5;
                    }
                    TouchProbeApprRetractParams.Rows.Add(dr);
                }
                dgTouchParams.AutoGenerateColumns = true;
                dgTouchParams.AutoGeneratedColumns += dgTouchParams_AutoGeneratedColumns;
                dgTouchParams.ItemsSource = TouchProbeApprRetractParams.DefaultView;
                dgTouchParams.UpdateLayout();
                RetractProbePointModeChkBx.Click += new RoutedEventHandler(OnCheckBoxClicked);
                //CleanDatabaseBtn.Click += new RoutedEventHandler(CleanDatabaseBtn_Click);
                EnableOneshotFeaturesChkBx.PreviewMouseDown += new MouseButtonEventHandler(EnableOneshotFeaturesChkBx_PreviewMouseDown);

                CheckBox[] MagHiddenChkBxs = { MagLevel3XHiddenChkBx, MagLevel5XHiddenChkBx, MagLevel11XHiddenChkBx, MagLevel22XHiddenChkBx, MagLevel34XHiddenChkBx,
                                         MagLevel54XHiddenChkBx, MagLevel67XHiddenChkBx, MagLevel134XHiddenChkBx, MagLevel335XHiddenChkBx};
                for (int i = 0; i < MagHiddenChkBxs.Length; i++)
                {
                    MagHiddenChkBxs[i].Checked += new RoutedEventHandler(MagHiddenChkBxs_Checked);
                    MagHiddenChkBxs[i].Unchecked += new RoutedEventHandler(MagHiddenChkBxs_Unchecked);
                }
                if (GlobalSettings.RapidCameraType == GlobalSettings.CameraType.TenM || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.One_Shot || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.OnlineSystem)
                {
                    OneShotSettingsTab.Visibility = System.Windows.Visibility.Visible;
                    TabOneShotParamsFill();
                }

                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.One_Shot || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.OnlineSystem)
                    gbLightControl.Visibility = Visibility.Visible;

                switch (GlobalSettings.CurrentUserLevel)
                {
                    case GlobalSettings.UserPrevilageLevel.None:
                    case GlobalSettings.UserPrevilageLevel.User:
                        initUser();
                        break;
                    case GlobalSettings.UserPrevilageLevel.AdminUser:
                        initAdminUser();
                        break;
                    case GlobalSettings.UserPrevilageLevel.Admin:
                        initAdmin();
                        break;
                    case GlobalSettings.UserPrevilageLevel.System:
                        initSystem();
                        break;
                }
                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
                {
                    RotaryMeasurementTab.Visibility = Visibility.Visible;
                    gb_DephiRotary.Visibility = Visibility.Visible;
                }
                else if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Horizontal)
                {
                    RotaryMeasurementTab.Visibility = Visibility.Visible;
                    if (GlobalSettings.RotaryAxisForMeasurement)
                        gb_RotaryMeasurementSettings.Visibility = Visibility.Visible;
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW02", ex); }
        }

        void dgTouchParams_AutoGeneratedColumns(object sender, EventArgs e)
        {

            //dgTouchParams.Columns[0].CellStyle.Setters.Add(new Setter(FontWeightProperty, FontWeights.Bold));

            //for (int i = 1; i < dgTouchParams.Columns.Count; i++)
            //{
            //    dgTouchParams.Columns[i].CellStyle.Setters.Add(new Setter(HorizontalContentAlignmentProperty, HorizontalAlignment.Right));
            //}

        }

        void initUser()
        {
            try
            {
                TabDisplayFill();
                TabPartprogramFill();
                TabRotaryMeasurementFill();

                AdvancedTab.Visibility = System.Windows.Visibility.Hidden;
                TouchProbeTab.Visibility = System.Windows.Visibility.Hidden;
                UserSettingsTab.Visibility = System.Windows.Visibility.Hidden;
                LicenseInfoTab.Visibility = System.Windows.Visibility.Hidden;
                MachineDetailsTab.Visibility = System.Windows.Visibility.Hidden;
                VisionCalibTab.Visibility = System.Windows.Visibility.Hidden;
                DROTab.Visibility = System.Windows.Visibility.Hidden;
                EEPROMSettingsTab.Visibility = System.Windows.Visibility.Hidden;
                SurfaceFGSettingsTab.Visibility = System.Windows.Visibility.Hidden;
                XML_ExportImport.Visibility = Visibility.Hidden;
                ExportBtn.Visibility = System.Windows.Visibility.Hidden;
                //FilterTab.Visibility = System.Windows.Visibility.Hidden;
                MultiZoomLevelTab.Visibility = System.Windows.Visibility.Hidden;

            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW03", ex); }
        }
        void initAdminUser()
        {
            try
            {
                TabDisplayFill();
                TabPartprogramFill();
                TabAdvancedFill();
                TabTouchProbeFill();
                TabRotaryMeasurementFill();

                UserSettingsTab.Visibility = System.Windows.Visibility.Hidden;
                LicenseInfoTab.Visibility = System.Windows.Visibility.Hidden;
                MachineDetailsTab.Visibility = System.Windows.Visibility.Hidden;
                VisionCalibTab.Visibility = System.Windows.Visibility.Hidden;
                DROTab.Visibility = System.Windows.Visibility.Hidden;
                EEPROMSettingsTab.Visibility = System.Windows.Visibility.Hidden;
                SurfaceFGSettingsTab.Visibility = System.Windows.Visibility.Hidden;
                XML_ExportImport.Visibility = Visibility.Hidden;

                //ExportBtn.Visibility = System.Windows.Visibility.Hidden;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW04", ex); }
        }
        void initAdmin()
        {
            try
            {
                TabDisplayFill();
                TabPartprogramFill();
                TabAdvancedFill();
                TabTouchProbeFill();
                TabUserSettingsFill();
                TabLicenseInfoFill();
                TabRotaryMeasurementFill();

                MachineDetailsTab.Visibility = System.Windows.Visibility.Hidden;
                VisionCalibTab.Visibility = System.Windows.Visibility.Hidden;
                DROTab.Visibility = System.Windows.Visibility.Hidden;
                EEPROMSettingsTab.Visibility = System.Windows.Visibility.Hidden;
                SurfaceFGSettingsTab.Visibility = System.Windows.Visibility.Hidden;
                XML_ExportImport.Visibility = Visibility.Hidden;

                //ExportBtn.Visibility = System.Windows.Visibility.Hidden;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW05", ex); }
        }
        void initSystem()
        {
            try
            {
                TabDisplayFill();
                TabPartprogramFill();
                TabAdvancedFill();
                TabTouchProbeFill();
                TabUserSettingsFill();
                TabLicenseInfoFill();
                TabMachineDetailsFill();
                TabVideoAndCameraSettingsFill();
                TabDROSettingsFill();
                TabEEPROMSettingsFill();
                TabSurfaceFGSettingsFill();
                TabCameraAxisValueFill();
                TabFilterationValueFill();
                //================================================Apply Some Condition============================================//                
                TabRotaryMeasurementFill();

            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW06", ex); }
        }

        #endregion

        #region Functions to fill each tab with values from database

        //Tab for display settings
        void TabDisplayFill()
        {
            try
            {
                //=============================================================Text Boxes=============================================================//
                TextBox[] PPDetailsTxtBxs = { SpeedXTxtBx, SpeedYTxtBx, SpeedZTxtBx, SpeedRTxtBx, MaxValTxtBx, MinValTxtBx, StepValTxtBx, MinPointsReqTxtBx, MinPixelCountTxtBx, AngleTolTxtBx, PercentTolOnRadiusTxtBx };
                String[] ControlNames = { "SpeedX", "SpeedY", "SpeedZ", "SpeedR", "MaximumValue", "MinimumValue", "StepValue", "MinimumPointsRequiredToDetermineShape", "MinimumPixelDifferenceBetweenShapes", "AngleToleranceForCriticalShape", "PercentToleranceOnRadiusForCriticalShape" };
                bool[] IncludeMultiplicationFactor = { true, true, true, true, true, true, true, false, false, false, false };
                for (int i = 0; i < PPDetailsTxtBxs.Length; i++)
                    FillTextBox(PPDetailsTxtBxs[i], "UserSettings", "MachineNo", MachineNo, ControlNames[i], true, IncludeMultiplicationFactor[i]);

                FillTextBox(txtRefDotDia, "VisionSettings", "MachineNo", MachineNo, "RefDotDia", true);

                //=============================================================Check Boxes=============================================================//
                CheckBox[] AxisPolarityChkBxs = { PolarityXChkBx, PolarityYChkBx, PolarityZChkBx, PolarityRChkBx };
                for (int i = 0; i < DROROWS.Length; i++)
                    AxisPolarityChkBxs[i].IsChecked = Convert.ToBoolean(DROROWS[i]["AxisPolarity"]);

                FillCheckBox(ShwNameTagInVidChkBx, "UserSettings", "MachineNo", MachineNo, "ShowNameTaginVideo");
                FillCheckBox(SaveCHairWithImgChkBx, "UserSettings", "MachineNo", MachineNo, "SaveCrossHairWithImage");
                FillCheckBox(InchModeChkBx, "UserSettings", "MachineNo", MachineNo, "DefaultInchMode");
                FillCheckBox(ShowIconNameChkBx, "UserSettings", "MachineNo", MachineNo, "IconsWithName");
                FillCheckBox(CldPtsContourColouringChkBx, "UserSettings", "MachineNo", MachineNo, "CloudPtsContourColouring");
                FillCheckBox(InvertRcadImgBackColourChkBx, "UserSettings", "MachineNo", MachineNo, "InvertRcadImgBackColour");
                FillCheckBox(SaveHomePositionChkBx, "UserSettings", "MachineNo", MachineNo, "SaveHomePosition");
                FillCheckBox(SetCriticalShapeChkBx, "UserSettings", "MachineNo", MachineNo, "SetCriticalShape");
                FillCheckBox(MaintainDxfShpContinuityChkBx, "DXFSettings", "MachineNo", MachineNo, "MaintainDxfShpContinuity");
                FillCheckBox(ExportAllShpsToSingleUCSChkBx, "DXFSettings", "MachineNo", MachineNo, "ExportAllShpsToSingleUCS");
                FillCheckBox(RedBlueShapesAsTolLimitChkBx, "DXFSettings", "MachineNo", MachineNo, "DXFRedBlueToleranceMode");
                FillCheckBox(ExportDxfShapesAtZZeroChkBx, "DXFSettings", "MachineNo", MachineNo, "ExportDxfShapesAtZZero");
                FillCheckBox(chkDXF_Float, "DXFSettings", "MachineNo", MachineNo, "DoNotFloatDXFwithMouseMove", true);

                //=============================================================Combo Boxes=============================================================//
                LanguageCBx.Items.Clear();
                int RowC = Settings_DataB.GetRowCount("LanguageTypeLookup");
                for (int i = 0; i < RowC; i++)
                    LanguageCBx.Items.Add(Settings_DataB.get_GetRecord("LanguageTypeLookup", i, "LanguageType"));
                LanguageCBx.Text = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["LanguageType"].ToString();

                DefaultMagLevelCBx.Items.Clear();
                foreach (System.Data.DataRow D in MagRows)
                {
                    if (!Convert.ToBoolean(D["Hidden"]))
                        DefaultMagLevelCBx.Items.Add(D["MagLevelValue"].ToString());
                }
                DefaultMagLevelCBx.Text = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["DefaultMagLevel"].ToString();

                DefaultImgFormatCBx.Items.Clear();
                DefaultImgFormatCBx.Items.Add("JPEG");
                DefaultImgFormatCBx.Items.Add("BMP");
                DefaultImgFormatCBx.Items.Add("TIFF");
                DefaultImgFormatCBx.Items.Add("GIF");
                DefaultImgFormatCBx.Text = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["DefaultImageFormat"].ToString();

                string[] TmpAr = { "Tahoma", "Times Roman 24", "Helvetica 18", "Calibri 9", "Stroke Roman", "Stroke Mono Roman" };
                for (int i = 0; i < TmpAr.Length; i++)
                    GraphicsTxtFamilyCBx.Items.Add(TmpAr[i]);
                GraphicsTxtFamilyCBx.SelectedIndex = GlobalSettings.FontSelectedIndx;
                GraphicsTxtFamilyCBx.SelectionChanged += new SelectionChangedEventHandler(GraphicsTxtFamilyCBx_SelectionChanged);

                MeasurementModeCBx.Text = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["MeasurementGradientMode"].ToString();
                MeasurementModeCBx.SelectionChanged += new SelectionChangedEventHandler(MeasurementModeCBx_SelectionChanged);

                int SelectedResolution = Convert.ToInt32(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["CameraResolution"].ToString());
                CamResolCBx.SelectedIndex = SelectedResolution;

                for (int i = 1; i <= 8; i++)
                    MesureNoOfDecCbx.Items.Add(i);
                MesureNoOfDecCbx.SelectedItem = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["MesureNoOfDec"];
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW07", ex); }
        }
        //Tab for part-program settings
        void TabPartprogramFill()
        {
            try
            {
                //=============================================================Text Boxes=============================================================//
                TextBox[] PPDetailsTxtBxs = { NoOfRowsTxtBx, NoOfColsTxtBx, RowGapTxtBx, ColGapTxtBx };
                String[] ControlNames = { "No_of_Rows", "No_of_Columns", "RowGap", "ColumnGap" };
                bool[] MulFactors = { false, false, true, true };
                ExcelRptPasswordTxtBx.IsEnabled = false; ExcelRptPasswordTxtBx.Text = "";

                for (int i = 0; i < PPDetailsTxtBxs.Length; i++)
                    FillTextBox(PPDetailsTxtBxs[i], "FixtureDetails", "MachineNo", MachineNo, ControlNames[i], true, MulFactors[i]);

                //=============================================================Check Boxes=============================================================//
                //FillCheckBox(EnableGridPrgChkBx, "FixtureDetails", "MachineNo", MachineNo, "EnableGrid");
                FillCheckBox(SwitchOffSnapModeDuringPPChkBx, "UserSettings", "MachineNo", MachineNo, "SwitchOffSnapModeDuringPP");
                FillCheckBox(RunAllPPinCNCModeByDefaultChkBx, "UserSettings", "MachineNo", MachineNo, "RunAllPPinCNCModeByDefault");
                FillCheckBox(UseOptimisationInPPChkBx, "UserSettings", "MachineNo", MachineNo, "UseOptimisedPath");
                FillCheckBox(RerunFailedMeasurementsChkBx, "UserSettings", "MachineNo", MachineNo, "RerunFailedMeasureStepsInPP");
                FillCheckBox(XLExportRowWiseChkBx, "UserSettings", "MachineNo", MachineNo, "XLExportRowWise");
                FillCheckBox(XLExportPortraitChkBx, "UserSettings", "MachineNo", MachineNo, "XLExportPortraitLayout");
                FillCheckBox(XLExportRCadImageChkBx, "UserSettings", "MachineNo", MachineNo, "XLExportRCadImage");
                FillCheckBox(ExportShpPtsToCSVChkBx, "UserSettings", "MachineNo", MachineNo, "ExportShpPtsToCSVDuringPP");
                FillCheckBox(ExportShpParamsToCSVChkBx, "UserSettings", "MachineNo", MachineNo, "ExportShpParamsToCSVDuringPP");
                FillCheckBox(HideShapeAndMeasurementChkBx, "UserSettings", "MachineNo", MachineNo, "HideShapesAndMeasurements");
                FillCheckBox(EnableCsvExportChkBx, "UserSettings", "MachineNo", MachineNo, "EnableCsvExport");
                FillCheckBox(TwoStepHomingChkBx, "MachineDetails", "MachineNo", MachineNo, "TwoStepHoming");
                FillCheckBox(ShowAveragingChkBx, "UserSettings", "MachineNo", MachineNo, "ShowAveraging");
                FillCheckBox(InsertImgChkBx, "UserSettings", "MachineNo", MachineNo, "InsertImage");
                FillCheckBox(PasswordProtectExcelRptChkBx, "UserSettings", "MachineNo", MachineNo, "ProtectXLwithPassword");
                FillCheckBox(MakePDFReport, "VisionSettings", "MachineNo", MachineNo, "MakePDFReport");
                FillCheckBox(PaginateReports, "VisionSettings", "MachineNo", MachineNo, "PaginateReports");
                FillCheckBox(MultiReportsInContinuousMode, "VisionSettings", "MachineNo", MachineNo, "MultiReportsInContinuousMode");
                FillCheckBox(cbAllProgramsInSingleReport, "VisionSettings", "MachineNo", MachineNo, "AllProgramInSingleReport");
                //FillCheckBox(cb_Align_by_CG, "VisionSettings", "MachineNo", MachineNo, "Align_by_CG");
                FillCheckBox(cbCNCAssisted_FG, "VisionSettings", "MachineNo", MachineNo, "CNCAssisted_FG");

                //=============================================================Radio Buttons=============================================================//
                string TolMode = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["DataTransferToleranceFormat"].ToString();
                TolMode = TolMode.Replace("\"", string.Empty).Trim();
                int ToleranceMode;
                AllowedRangeRBtn.IsChecked = false;
                NomPlsMnsTolRBtn.IsChecked = false;
                ShwLSLorUSLRBtn.IsChecked = false;
                if (int.TryParse(TolMode, out ToleranceMode))
                {
                    switch (ToleranceMode)
                    {
                        case 1:
                            AllowedRangeRBtn.IsChecked = true;
                            break;
                        case 2:
                            NomPlsMnsTolRBtn.IsChecked = true;
                            break;
                        case 3:
                            ShwLSLorUSLRBtn.IsChecked = true;
                            break;
                    }
                }
                else
                {
                    if (TolMode.Contains("AllowedRange"))
                        AllowedRangeRBtn.IsChecked = true;
                    else if (TolMode.Contains("NomPlsMinus"))
                        NomPlsMnsTolRBtn.IsChecked = true;
                    else if (TolMode.Contains("ShowLSLorUSL"))
                        ShwLSLorUSLRBtn.IsChecked = true;
                }
                //switch (TolMode)
                //{
                //    case "AllowedRange":
                //        AllowedRangeRBtn.IsChecked = true;
                //        break;
                //    case "NomPlsMinusTol":
                //        NomPlsMnsTolRBtn.IsChecked = true;
                //        break;
                //    case "ShowLSLorUSL":
                //        ShwLSLorUSLRBtn.IsChecked = true;
                //        break;
                //}

                //=============================================================Combo Boxes=============================================================//
                //ProgramDriveCBx.Items.Clear();
                //System.IO.DriveInfo[] DirectoryList = System.IO.DriveInfo.GetDrives();
                //foreach (System.IO.DriveInfo D in DirectoryList)
                //    if (D.DriveType == System.IO.DriveType.Fixed)
                //        ProgramDriveCBx.Items.Add(D.Name);
                String tempstr = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", this.MachineNo)[0]["SelectProgramDrive"].ToString();
                //tempstr = tempstr.Substring(0, tempstr.Length - 1);
                //if (ProgramDriveCBx.Items.Contains(tempstr))
                //    ProgramDriveCBx.Text = tempstr;
                //else
                //    ProgramDriveCBx.SelectedIndex = 0;
                tblock_ProgramLocation.Text = tempstr;
                tempstr = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", this.MachineNo)[0]["SelectReportPath"].ToString();
                tblock_ReportLocation.Text = tempstr;
                if (tblock_ReportLocation.Text != tblock_ProgramLocation.Text)
                    chkDecoupleReportPath.IsChecked = true;

                //XL Export Image 1 ComboBox
                string Img = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["XLExportImage1DuringPP"].ToString();
                Img = Img.Replace("\"", string.Empty).Trim();
                switch (Img)
                {
                    case "None":
                        TransferImg1ToXLDuringPPCBx.SelectedIndex = 0;
                        break;
                    case "RCad Image":
                        TransferImg1ToXLDuringPPCBx.SelectedIndex = 1;
                        break;
                    case "Video Image":
                        TransferImg1ToXLDuringPPCBx.SelectedIndex = 2;
                        break;
                    case "Video With Graphics":
                        TransferImg1ToXLDuringPPCBx.SelectedIndex = 3;
                        break;
                }
                //XL Export Image 2 ComboBox
                Img = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["XLExportImage2DuringPP"].ToString();
                Img = Img.Replace("\"", string.Empty).Trim();
                switch (Img)
                {
                    case "None":
                        TransferImg2ToXLDuringPPCBx.SelectedIndex = 0;
                        break;
                    case "RCad Image":
                        TransferImg2ToXLDuringPPCBx.SelectedIndex = 1;
                        break;
                    case "Video Image":
                        TransferImg2ToXLDuringPPCBx.SelectedIndex = 2;
                        break;
                    case "Video With Graphics":
                        TransferImg2ToXLDuringPPCBx.SelectedIndex = 3;
                        break;
                }

                FixtureNameCBx.Items.Clear();
                foreach (System.Data.DataRow D in FixtureRows)
                    FixtureNameCBx.Items.Add(D["FixtureName"].ToString());
                FixtureNameCBx.SelectedItem = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", this.MachineNo)[0]["DefaultFixtureName"].ToString();
                switch (GlobalSettings.CurrentUserLevel)
                {
                    case GlobalSettings.UserPrevilageLevel.Admin:
                    case GlobalSettings.UserPrevilageLevel.AdminUser:
                    case GlobalSettings.UserPrevilageLevel.System:
                        ExcelRptPasswordTxtBx.IsEnabled = true;
                        string ss = "";
                        for (int i = 0; i < GlobalSettings.XLPassword.Length; i++)
                            ss += "*";
                        ExcelRptPasswordTxtBx.Text = ss;
                        break;
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW08", ex); }
        }
        //Tab for advanced settings
        void TabAdvancedFill()
        {
            try
            {
                //=============================================================Text Boxes=============================================================//
                TextBox[] UserSettingsTxtBxs = { RepeatCntTxtBx, FocusActiveWidthTxtBx, RotaryAngDiffTxtBx,
                                                 MaxMinFocusRatioCutoffTxtBx, MaxMinZRatioCutoffTxtBx, SigmaActiveWdthRatioCutoffTxtBx,
                                                 NoiseFilterFactorTxtBx, GrainFactorTxtBx, ProfileScanCutoffPointTBx,
                                                 ProfileScanRectHorizontalBorderBufferTxtBx, ProfileScanRectVerticalBorderBufferTxtBx, NeighbourPtDistToleranceTxtBx, ProfileScanMoveToNextFrameFractionTxtBx,
                                                 PointsBundleForDiaProfile, BestFitPointsBundleForDia,PixelAveragingrTxtBx,NoOfImagesForAvgTxtBx,BestFitResolutionTxtBx,BestFitSampleSizeTxtBx, RotaryInterceptTxtBx};
                String[] TxtBxNames = { "RepeatCount", "DTol", "RotaryAngleDiff",
                                        "MaxMinFocusRatioCutoff", "MaxMinZRatioCutoff", "SigmaActiveWdthRatioCutoff",
                                        "NoiseFilterFactor", "GrainFactor", "ProfileScanCutoffPoint",
                                        "ProfileScanRectHorizontalBorderBuffer", "ProfileScanRectVerticalBorderBuffer", "ProfileScanNeighbourPtDistTolerance", "ProfileScanMoveToNextFrameFraction",
                                        "PointsBundleForDiaProfile", "BestFitPointsBundleForDia","PixelAveragingValue","ImageCountForAveraging","BestFitResolution","BestFitSampleSize", "RotaryInterceptValue"};
                bool[] MulFactors = { false, true, false,
                                      false, false, false,
                                      false, false, false,
                                      false, false, false, false,
                                      false, false , false,false,
                                      false,false, false};
                FractionisUpdating = true;

                for (int i = 0; i < UserSettingsTxtBxs.Length; i++)
                    FillTextBox(UserSettingsTxtBxs[i], "UserSettings", "MachineNo", MachineNo, TxtBxNames[i], true, MulFactors[i]);

                FractionisUpdating = false;

                FillTextBox(SurfaceStepLengthTxtBx, "DXFSettings", "MachineNo", MachineNo, "SurfaceStepLength", true, true);
                FillTextBox(SurfacePtClusterTxtBx, "DXFSettings", "MachineNo", MachineNo, "SurfacePtClustSize", true);
                FillTextBox(PtGapSurfaceBuildTxtBx, "DXFSettings", "MachineNo", MachineNo, "SurfacePtGap", true, true);
                FillTextBox(SurfaceBfitDistToleranceTxtBx, "DXFSettings", "MachineNo", MachineNo, "BestFitSurfaceTolerance", true);

                //=============================================================Check Boxes=============================================================//
                CheckBox[] UserSettingsChkBxs = { UseLineArcFitChkBx, CldPtsAsSurfaceChkBx, UseLocalCorrectChkBx, OutlierFilteringChkBx, ShwDeviationForLnsOnlyChkBx, ApplyFocusCorrectionChkBx, AutoAdjustLightingChkBx, EnableOneshotFeaturesChkBx, ArcBestFitTypeForDia, ApplyPixelAveragingChkBx, UseAverageImageChkBx, CloseOpenSurfaceChkBx };
                String[] ChkBxNames = { "UseLineArcFitting", "CloudPointsAsSurface", "UseLocalisedCorrection", "OutlierFilteringForLineOrArc", "ShwDeviationForLnsOnly", "ApplyFocusCorrection", "AutoAdjustLighting", "EnableOneshotFeatures", "ArcBestFitTypeForDia", "ApplyPixelAveraging", "UseAverageImage", "CloseOpenSurface" };

                for (int i = 0; i < UserSettingsChkBxs.Length; i++)
                    FillCheckBox(UserSettingsChkBxs[i], "UserSettings", "MachineNo", MachineNo, ChkBxNames[i]);

                FillCheckBox(AllPtsAsCldPtsChkBx, "DXFSettings", "MachineNo", MachineNo, "AllPointAsCloudPoints");
                FillCheckBox(OptimiseSurfaceBuildChkBx, "DXFSettings", "MachineNo", MachineNo, "OptimiseSurfaceBuild");
                FillCheckBox(UsePtDistUpperCutOffChkBx, "DXFSettings", "MachineNo", MachineNo, "UseSurfacebuildUpperCutoff");
                FillCheckBox(PtsAroundXYPlnChkBx, "DXFSettings", "MachineNo", MachineNo, "CloudPtsAroundXYPlane");

                //For linear mode check box
                DB GetValues = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                GetValues.FillTable("ToolStripButton");
                System.Data.DataRow TmpDataR = GetValues.Select_ChildControls("ToolStripButton", "ButtonName", "Linear Mode")[0];
                DefaultLinearModChkBx.IsChecked = Convert.ToBoolean(TmpDataR["CheckedProperty"]);
                GetValues.FinalizeClass();

                //=============================================================Radio Buttons=============================================================//
                string RefAxis = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["UCSBasePlnLnRefAxis"].ToString();
                switch (RefAxis)
                {
                    case "XAxis":
                        UCSBasePlnLnRefXAxisRBtn.IsChecked = true;
                        break;
                    case "YAxis":
                        UCSBasePlnLnRefYAxisRBtn.IsChecked = true;
                        break;
                }
                //=============================================================MCS Combobox=============================================================//
                int McsType = Convert.ToInt32(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["MCSType"]);
                switch (McsType)
                {
                    case 0:
                        MCSTypeCBx.SelectedIndex = 0;
                        break;
                    case 1:
                        MCSTypeCBx.SelectedIndex = 1;
                        break;
                    case 2:
                        MCSTypeCBx.SelectedIndex = 2;
                        break;
                    case 3:
                        MCSTypeCBx.SelectedIndex = 3;
                        break;
                    case 4:
                        MCSTypeCBx.SelectedIndex = 4;
                        break;
                }


                //=============================================================Dxf Auto Allignment Properties=============================================================//
                DxfAlignProp_DGSourcing = new ObservableCollection<DxfAutoAlignPropEntity>();
                DxfAutoAlignPropDG.ItemsSource = DxfAlignProp_DGSourcing;
                for (int i = 0; i < GlobalSettings.DxfAutoAlignDistVals.Count; i++)
                {
                    DxfAutoAlignPropEntity TmpEntity = new DxfAutoAlignPropEntity();
                    TmpEntity.DistStr = GlobalSettings.DxfAutoAlignDistVals[i].ToString();
                    TmpEntity.Offset_XStr = GlobalSettings.DxfAutoAlignOffset_XVals[i].ToString();
                    TmpEntity.Offset_YStr = GlobalSettings.DxfAutoAlignOffset_YVals[i].ToString();
                    DxfAlignProp_DGSourcing.Add(TmpEntity);
                }
                DxfAutoAlignPropDG.PreparingCellForEdit += new EventHandler<DataGridPreparingCellForEditEventArgs>(DxfAutoAlignPropDG_PreparingCellForEdit);
                DxfAutoAlignPropDG.CellEditEnding += new EventHandler<DataGridCellEditEndingEventArgs>(DxfAutoAlignPropDG_CellEditEnding);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW09", ex); }
        }
        //Tab for touch probe settings
        void TabTouchProbeFill()
        {
            try
            {
                //=============================================================Text Boxes=============================================================//
                TextBox[] UserSettingsTxtBxs = { ProbeOffsetValTxtBx, ProbeSpeedTxtBx, MasterProbeDiaTxtBx, ProbeDiaTxtBx, OffsetXTxtBx, OffsetYTxtBx, OffsetZTxtBx,
                                                   TProbeXLeftDiaTxtBx, TProbeXRightDiaTxtBx, TProbeXOffsetXTxtBx, TProbeXOffsetYTxtBx, TProbeXOffsetZTxtBx,
                                                   TProbeYFrontDiaTxtBx, TProbeYBackDiaTxtBx, TProbeYOffsetXTxtBx, TProbeYOffsetYTxtBx, TProbeYOffsetZTxtBx,
                                                   StarProbeLeftDiaTxtBx, StarProbeRightDiaTxtBx, StarProbeFrontDiaTxtBx, StarProbeBackDiaTxtBx,
                                                   StarProbeRightOffsetXTxtBx, StarProbeRightOffsetYTxtBx, StarProbeRightOffsetZTxtBx,
                                                   StarProbeBackOffsetXTxtBx, StarProbeBackOffsetYTxtBx, StarProbeBackOffsetZTxtBx,
                                                   StarProbeFrontOffsetXTxtBx, StarProbeFrontOffsetYTxtBx, StarProbeFrontOffsetZTxtBx,
                                                   ProbeSensitivityTxtBx,ProbeStraightOffsetXTxtBx,ProbeStraightOffsetYTxtBx,ProbeStraightOffsetZTxtBx,
                                                   TProbeYSOffsetXTxtBx,TProbeYSOffsetYTxtBx,TProbeYSOffsetZTxtBx,
                                                   TProbXSOffsetXTxtBx,TProbeXSOffsetYTxtBx,TProbeXSOffsetZTxtBx,TProbeXStraightDiaTxtBx,TProbeYStraightDiaTxtBx,
                                                   StarProbeStraightDiaTxtBx, txtGageOff_X, txtGageOff_Y, txtGageOff_Z};
                String[] TxtBxNames = { "ProbeOffsetValue", "ProbeSpeed", "MasterProbeDia", "ProbeDiameter", "ProbeOffsetX", "ProbeOffsetY", "ProbeOffsetZ",
                                          "TProbeAlongXLeftDia", "TProbeAlongXRightDia", "TProbeAlongXOffsetX", "TProbeAlongXOffsetY", "TProbeAlongXOffsetZ",
                                          "TProbeAlongYFrontDia", "TProbeAlongYBackDia", "TProbeAlongYOffsetX", "TProbeAlongYOffsetY", "TProbeAlongYOffsetZ",
                                          "StarProbeLeftDia", "StarProbeRightDia", "StarProbeFrontDia", "StarProbeBackDia",
                                          "StarProbeRightOffsetX", "StarProbeRightOffsetY", "StarProbeRightOffsetZ",
                                          "StarProbeBackOffsetX", "StarProbeBackOffsetY", "StarProbeBackOffsetZ",
                                          "StarProbeFrontOffsetX", "StarProbeFrontOffsetY", "StarProbeFrontOffsetZ",
                                          "ProbeSensitivity","StarProbeStrtOffsetX","StarProbeStrtOffsetY","StarProbeStrtOffsetZ",
                                          "TProbeStrtOffsetX","TProbeStrtOffsetY","TProbeStrtOffsetZ",
                                          "TProbeXStrtOffsetX","TProbeXStrtOffsetY","TProbeXStrtOffsetZ","TProbeAlongXStrtDia","TProbeAlongYStrtDia",
                                          "StarProbeStrtDia", "GageOffsetX", "GageOffsetY", "GageOffsetZ"};

                for (int i = 0; i < UserSettingsTxtBxs.Length; i++)
                    FillTextBox(UserSettingsTxtBxs[i], "ProbeSettings", "MachineNo", MachineNo, TxtBxNames[i], true, true);

                ProbeRetractDistance = Convert.ToDouble(Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["ProbeRevertDistance"]);
                FillTextBox(ProbeMoveBackValTxtBx, "EEPROMSettings", "MachineNo", MachineNo, "ProbeRevertDistance", true, true);
                ProbeRetractSpeed = Convert.ToDouble(Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["ProbeRevertSpeed"]);
                FillTextBox(ProbeRetractSpeedTxtBx, "EEPROMSettings", "MachineNo", MachineNo, "ProbeRevertSpeed", true, true);
                ProbeApproachDistance = Convert.ToDouble(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeOffsetValue"]);
                FillTextBox(ProbeOffsetValTxtBx, "ProbeSettings", "MachineNo", MachineNo, "ProbeOffsetValue", true, true);
                ProbeApproachSpeed = Convert.ToDouble(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeSpeed"]);
                FillTextBox(ProbeSpeedTxtBx, "ProbeSettings", "MachineNo", MachineNo, "ProbeSpeed", true, true);
                ProbeSensitivity = Convert.ToDouble(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeSensitivity"]);
                FillTextBox(ProbeSensitivityTxtBx, "ProbeSettings", "MachineNo", MachineNo, "ProbeSensitivity", true, true);
                ProbeMoveUpFlag = Convert.ToBoolean(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeMoveUp"]);


                //=============================================================Radio Buttons=============================================================//
                string ProbeType = Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeType"].ToString();
                switch (ProbeType)
                {
                    case "NormalProbe":
                        ProbeTypeCBx.SelectedIndex = 0;
                        break;
                    case "TProbeAlongX":
                        ProbeTypeCBx.SelectedIndex = 1;
                        break;
                    case "TProbeAlongY":
                        ProbeTypeCBx.SelectedIndex = 2;
                        break;
                    case "StarProbe":
                        ProbeTypeCBx.SelectedIndex = 3;
                        break;
                    case "Generic Probe":
                        ProbeTypeCBx.SelectedIndex = 4;
                        break;
                }

                //=============================================================Check Boxes=============================================================//
                FillCheckBox(ShowProbeInRcadChkBx, "ProbeSettings", "MachineNo", MachineNo, "ShowTouchProbeInRcad");
                FillCheckBox(UseOneTouchAlignmentChkBx, "ProbeSettings", "MachineNo", MachineNo, "UseProbeOneTouchAlignment");
                FillCheckBox(chkRetractProbeMethod, "ProbeSettings", "MachineNo", MachineNo, "ProbeMoveUp");
                FillCheckBox(UseGotoPointsforPobeHitPartProgChkBx, "ProbeSettings", "MachineNo", MachineNo, "UseGotoPointsforPobeHitPartProg");
                //=============================================================Data Grid===============================================================//
                string CurrentGenericProbeSelected = Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["CurrentGenericProbeType"].ToString();
                ProbeTypeColl = new ObservableCollection<UserDefinedProbType>();
                DataTable DetailsDt = GlobalSettings.SettingsReader.GetTable("ProbAssemblyTypeDetails");
                List<string> UniqueProbeTypeList = new List<string>();
                for (int i = 0; i < DetailsDt.Rows.Count; i++)
                {
                    string ProbetypeString = DetailsDt.Rows[i]["ProbType"].ToString();
                    if (ProbetypeString != "")
                    {
                        if (!UniqueProbeTypeList.Contains(ProbetypeString))
                        {
                            UniqueProbeTypeList.Add(ProbetypeString);
                        }
                        else
                            continue;
                    }
                }
                foreach (string Probetype in UniqueProbeTypeList)
                {
                    DataRow[] DetailsRow = DetailsDt.Select("ProbType = '" + Probetype + "'");
                    UserDefinedProbType probetype = new UserDefinedProbType();
                    probetype.ProbtypeName = Probetype;
                    probetype.ProbeDetails = new ObservableCollection<Probedetails>();
                    //ProbDetailsDG.ItemsSource = ProbeDetails;
                    if (DetailsRow.Length > 0)
                    {
                        foreach (DataRow DRows in DetailsRow)
                        {
                            Probedetails pd = new Probedetails();
                            pd.ProbtypeName = DRows["ProbType"].ToString();
                            pd.OffsetX = DRows["OffsetX"].ToString();
                            pd.OffsetY = DRows["OffsetY"].ToString();
                            pd.OffsetZ = DRows["OffsetZ"].ToString();
                            pd.Comment = DRows["Comment"].ToString();
                            pd.Orientation = DRows["Orientation"].ToString();
                            pd.ID = Convert.ToInt32(DRows["ID"].ToString());
                            pd.Radius = Convert.ToInt32(DRows["Radius"].ToString());
                            pd.MachineNo = DRows["MachineNo"].ToString();
                            probetype.ProbeDetails.Add(pd);
                        }
                    }
                    ProbeTypeColl.Add(probetype);
                }
                GenericProbGBx.DataContext = ProbeTypeColl;
                for (int i = 0; i < ProbeTypeColl.Count; i++)
                {
                    if (ProbeTypeColl[i].ProbtypeName == CurrentGenericProbeSelected)
                    {
                        ProbeTypeChbk.SelectedItem = ProbeTypeColl[i];
                        break;
                    }
                }
                RetractProbePointMode = (bool)Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["RetractProbePointMode"];
                RetractProbePointModeChkBx.IsChecked = (bool)RetractProbePointMode;
                OnCheckBoxClicked(RetractProbePointModeChkBx, null);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW10", ex); }
        }
        //Tab for Adding and deleting new users
        void TabUserSettingsFill()
        {
            try
            {
                //=============================================================Combo Boxes=============================================================//
                UserNameCBx.Items.Clear();
                System.Data.DataRow[] Logindetails = Settings_DataB.Select_ChildControls("Login", "Login_Priviledge", "User");
                foreach (System.Data.DataRow D in Logindetails)
                    UserNameCBx.Items.Add(D["Login_Name"].ToString());
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW11", ex); }
        }
        //Tab for showing feature wise license
        void TabLicenseInfoFill()
        {
            bool LicenseEntered = Convert.ToBoolean(Settings_DataB.get_GetRecord("LicenseInfo", 0, "LicenseEntered"));
            if (LicenseEntered)
            {
                InstallationKeyLbl.Visibility = System.Windows.Visibility.Hidden;
                InstallationKeyTxtBx.Visibility = System.Windows.Visibility.Hidden;
            }
            else
                InstallationKeyTxtBx.Text = Settings_DataB.get_GetRecord("LicenseInfo", 0, "InstallationKey").ToString();

            ObservableCollection<FeatureLicenseInfoEntity> FeatureLicenseDGSourcing = new ObservableCollection<FeatureLicenseInfoEntity>();
            for (int i = 0; i < Settings_DataB.GetTable("FeatureLicenseInfo").Rows.Count; i++)
            {
                FeatureLicenseInfoEntity FeatureEntity = new FeatureLicenseInfoEntity();
                FeatureEntity.FeatureName = Settings_DataB.GetTable("FeatureLicenseInfo").Rows[i]["FeatureName"].ToString();
                FeatureEntity.LicenseType = Settings_DataB.GetTable("FeatureLicenseInfo").Rows[i]["LicenseType"].ToString();

                if (FeatureEntity.LicenseType == "Limited")
                {
                    TimeSpan ValidDays = new TimeSpan(Convert.ToInt16(Settings_DataB.GetTable("FeatureLicenseInfo").Rows[i]["NoOfDaysValid"]), 0, 0, 0);
                    DateTime ExpiryDate = Convert.ToDateTime(Settings_DataB.GetTable("FeatureLicenseInfo").Rows[i]["DateEntered"]) + ValidDays;
                    int DaysRemaining;
                    if (ExpiryDate < DateTime.Now)
                        DaysRemaining = 0;
                    else
                        DaysRemaining = (ExpiryDate - DateTime.Now).Days;
                    FeatureEntity.DaysRemaining = DaysRemaining.ToString();
                }
                FeatureLicenseDGSourcing.Add(FeatureEntity);
            }
            FeatureLicenseInfoDG.ItemsSource = FeatureLicenseDGSourcing;
        }
        //Tab for machine details
        void TabMachineDetailsFill()
        {
            try
            {
                //=============================================================Text Boxes=============================================================//
                TextBox[] MachineDetailsTxtBxs = { CompanyNameTxtBx, LocationTxtBx, VersionNoTxtBx, DROStyleTxtBx, OneShotImgCompBoundryWidthTxtBx, OneShotImgBinarisingValTxtBx,
                                                     WriteToHardDiskPtsLimitTxtBx,FocusDelayTxtBx, txtBxDROJumpTolerance}; //,PixelWidthCorrectionParameterQTxtBx,PixelWidthCorrectionParameterLX1TxtBx,PixelWidthCorrectionParameterLYTxtBx,X0TxtBx,Y0TxtBx };
                String[] ControlNames = { "CompanyName", "Location", "StartVersionNo", "DROStyle", "OneShotImageBoundryWidth", "OneShotImageBinariseValue", "WriteToHardDiskPtsLimit", "DelayInFocus", "MesureNoOfDec" };//, "PixelWidthCorrectionParameterQ", "PixelWidthCorrectionParameterLX1", "PixelWidthCorrectionParameterLY",
                                                                                                                                                                                                                         // "PixelWidthX0","PixelWidthY0"};

                for (int i = 0; i < MachineDetailsTxtBxs.Length; i++)
                    FillTextBox(MachineDetailsTxtBxs[i], "MachineDetails", "MachineNo", MachineNo, ControlNames[i]);
                FillTextBox(FrameRateTxtBx, "MachineDetails", "MachineNo", MachineNo, "FrameRate", true);
                FillTextBox(AutoZoomDelayTxtBx, "MachineDetails", "MachineNo", MachineNo, "AccessoryLenses", true);
                FillTextBox(CommandSendDelayTxtBx, "MachineDetails", "MachineNo", MachineNo, "CommandSendDelay", true);
                FillTextBox(WaitTimeAfterTargetReachedTxtBx, "MachineDetails", "MachineNo", MachineNo, "DelayAfterTargetReached", true);

                //=============================================================Check Boxes=============================================================//
                CheckBox[] MachineDetailsChkBxs = { AuxillarLightChkBx, CoAxialLightChkBx, ProbePresentChkBx, DefaultFlexCHairChkBx, AutoZoomInOutChkBx, AllowAxisLockChkBx, DigiZoomOneShotChkBx,
                                               LockUIChkBx, Point5XLensChkBx, ThreeDSoftwareChkBx, HideGraphicsOnVidChkBx, HardnessModulePresentChkBx, MultiLevelMachineChkBx,
                                               EnableDoubleImageOption, EnablePartialImageAllignInOneShotChkBx, AllowFocusRawValuesChkBx, AllowStepFileImportChkBx, UseLookAheadChkBx , DemoChkBx,PixelWidthCorrectionChkBx,
                                                cbRotaryMeasurementPresent};
                String[] ChkBxNames = { "AuxiliaryLighting", "CoAxialLighting", "ProbePresent", "DefaultFlexibleCrosshair", "AutoZoomInOut", "AllowAxisLock", "UseDigitalZoominOneShot",
                                  "LockUIatStartup", "FiveXLensePresent", "ThreedSoftware", "HideGraphicsOnVideo", "HardnessModule", "MultiLevelZoomMachine",
                                  "DoubleImageEnabled", "OneShotPartialImageEnabled", "AllowFocusRawValues", "AllowStepFileImport", "UseLookAhead" , "DemoMode","PixelWidthCorrection", "RotaryAxisForMeasurement"};

                for (int i = 0; i < MachineDetailsChkBxs.Length; i++)
                    FillCheckBox(MachineDetailsChkBxs[i], "MachineDetails", "MachineNo", MachineNo, ChkBxNames[i]);

                //Update Homing at start...
                FillCheckBox(HomeMachineAtStartup, "VisionSettings", "MachineNo", MachineNo, "AutoHoming_at_Start");

                //Set the MultilevelZoom
                GlobalSettings.MultilevelZoomMahine = (bool)MultiLevelMachineChkBx.IsChecked;
                GlobalSettings.PixelWidthCorrectionEnable = (bool)PixelWidthCorrectionChkBx.IsChecked;

                //=============================================================Combo Boxes=============================================================//
                //Add all the avaliable machine nos. into item list of this combobox 
                int RowC = Settings_DataB.GetRowCount("MachineDetails");
                List<String> Source = new List<string>();
                for (int i = 0; i < RowC; i++)
                {
                    //if (!MachineNumCBx.Items.Contains(Settings_DataB.get_GetRecord("MachineDetails", i, "MachineNo")))
                    //MachineNumCBx.Items.Add(Settings_DataB.get_GetRecord("MachineDetails", i, "MachineNo"));
                    if (!Source.Contains(Settings_DataB.get_GetRecord("MachineDetails", i, "MachineNo").ToString()))
                        Source.Add(Settings_DataB.get_GetRecord("MachineDetails", i, "MachineNo").ToString());
                    Source.Sort();
                }
                for (int i = 0; i < Source.Count; i++)
                    MachineNumCBx.Items.Add(Source[i]);
                NegletCBoxSelection = true;
                MachineNumCBx.SelectedItem = MachineNo;
                NegletCBoxSelection = false;

                MachineTypeCBx.SelectionChanged -= MachineTypeCBx_SelectionChanged;
                MachineTypeCBx.Items.Clear();
                RowC = Settings_DataB.GetRowCount("MachineTypeLookup");
                for (int i = 0; i < RowC; i++)
                    MachineTypeCBx.Items.Add(Settings_DataB.get_GetRecord("MachineTypeLookup", i, "MachineType"));
                MachineTypeCBx.SelectedItem = Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["MachineType"];
                MachineTypeCBx.SelectionChanged += new SelectionChangedEventHandler(MachineTypeCBx_SelectionChanged);

                AddingItemsToAxisNumberCB = true;
                //NoOfAxisCBx.SelectionChanged -= NumberOfAxisCBx_SelectionChanged;
                NoOfAxisCBx.Items.Clear();
                NoOfAxisCBx.Items.Add("2");
                NoOfAxisCBx.Items.Add("3");
                NoOfAxisCBx.Items.Add("4");
                NoOfAxisCBx.Items.Add("5");
                NoOfAxisCBx.Items.Add("6");
                AddingItemsToAxisNumberCB = false;
                //NoOfAxisCBx.SelectionChanged += new SelectionChangedEventHandler(NumberOfAxisCBx_SelectionChanged);
                NoOfAxisCBx.SelectedItem = (Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["NoOfAxes"]).ToString();

                CamTypeCBx.Items.Clear();
                CamTypeCBx.Items.Add("2M");
                CamTypeCBx.Items.Add("3M");
                CamTypeCBx.Items.Add("540P");
                CamTypeCBx.Items.Add("720P");
                CamTypeCBx.Items.Add("360P");
                CamTypeCBx.Items.Add("5M");
                CamTypeCBx.Items.Add("10M");
                CamTypeCBx.Items.Add("2M2");
                CamTypeCBx.Items.Add("1080P");

                CamTypeCBx.SelectedItem = (Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["CameraType"]).ToString();

                OneShotImgCompTolUpDown.StepValue = 0.1;
                OneShotImgCompTolUpDown.Minimum = 0.1;
                OneShotImgCompTolUpDown.Maximum = 20;
                OneShotImgCompTolUpDown.Value = Convert.ToDouble((Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["OneShotImageTolerance"]).ToString());

                OneShotImgCompTargetTolUpDown.StepValue = 0.1;
                OneShotImgCompTargetTolUpDown.Minimum = 0.1;
                OneShotImgCompTargetTolUpDown.Maximum = 10;
                OneShotImgCompTargetTolUpDown.Value = Convert.ToDouble((Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["OneShotImageMatchTargetTolerance"]).ToString());

            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW12", ex); }
        }
        //Tab for magnifier settings
        void TabVideoAndCameraSettingsFill()
        {
            try
            {
                //=============================================================Text Boxes=============================================================//
                FillTextBox(LightValueTxtBx, "LightingSettings", "MachineNo", MachineNo, "LightValue", true);
                FillTextBox(txtProfileZone, "LightingSettings", "MachineNo", MachineNo, "Profile_Zone_No", true);
                FillTextBox(txt11XProfile, "LightingSettings", "MachineNo", MachineNo, "Profile11X_Zone_No", true);
                FillTextBox(txtAuxiliary, "LightingSettings", "MachineNo", MachineNo, "Aux_Zone_No", true);
                FillTextBox(txtCoAxial, "LightingSettings", "MachineNo", MachineNo, "CoAx_ZoneNo", true);
                FillTextBox(txtProfileLightAt11X, "LightingSettings", "MachineNo", MachineNo, "ProfileLightLevelFor11X", true);
                
                try
                {
                    FillTextBox(txtSurfaceZ1, "LightingSettings", "MachineNo", MachineNo, "Zone1_No", true);
                    FillTextBox(txtSurfaceZ2, "LightingSettings", "MachineNo", MachineNo, "Zone2_No", true);
                    FillTextBox(txtSurfaceZ3, "LightingSettings", "MachineNo", MachineNo, "Zone3_No", true);
                    FillTextBox(txtSurfaceZ4, "LightingSettings", "MachineNo", MachineNo, "Zone4_No", true);
                }
                catch (Exception exxx)
                {

                }
                //FillTextBox(AxisAngleTxtBx, "VisionSettings", "MachineNo", MachineNo, "CameraSkew", true, true);
                //FillTextBox(AxisAngleXZTxtBx, "VisionSettings", "MachineNo", MachineNo, "CameraSkewXZ", true, true);
                //FillTextBox(AxisAngleYZTxtBx, "VisionSettings", "MachineNo", MachineNo, "CameraSkewYZ", true, true);
                AxisAngleTxtBx.Text = Convert.ToDouble(Settings_DataB.Select_ChildControls("VisionSettings", "MachineNo", MachineNo)[0]["CameraSkew"]).ToString();
                AxisAngleXZTxtBx.Text = Convert.ToDouble(Settings_DataB.Select_ChildControls("VisionSettings", "MachineNo", MachineNo)[0]["CameraSkewXZ"]).ToString();
                AxisAngleYZTxtBx.Text = Convert.ToDouble(Settings_DataB.Select_ChildControls("VisionSettings", "MachineNo", MachineNo)[0]["CameraSkewYZ"]).ToString();

                FillTextBox(CamDistortionFactorTxtBx, "VisionSettings", "MachineNo", MachineNo, "CamDistortionFactor", true, true);
                FillTextBox(CamDistortionOffsetXTxtBx, "VisionSettings", "MachineNo", MachineNo, "CamDistortionOffsetX", true, true);
                FillTextBox(CamDistortionOffsetYTxtBx, "VisionSettings", "MachineNo", MachineNo, "CamDistortionOffsetY", true, true);

                //All the textBoxes
                TextBox[] VideoSettingsTxtBxs = { XOffsetVideoTxtBx, YOffsetVideoTxtBx,
                                               XPosStartRight, XPosStartLeft, YPosStartTop, YPosStartBottom,
                                               XMultiplierRight, XMultiplierLeft, YMultiplierTop, YMultiplierBottom };
                String[] TxtBxNames = { "Xoffset", "Yoffset",
                                  "XPositionStartRight", "XPositionStartLeft", "YPositionStartTop", "YPositionStartBottom",
                                  "XMultiplyFactorRight", "XMultiplyFactorLeft", "YMultiplyFactorTop", "YMultiplyFactorBottom"};

                for (int i = 0; i < VideoSettingsTxtBxs.Length; i++)
                    FillTextBox(VideoSettingsTxtBxs[i], "VisionSettings", "MachineNo", MachineNo, TxtBxNames[i], true);

                //=============================================================Text Boxes=============================================================//
                CamLenseTypeCBx.Items.Clear();
                CamLenseTypeCBx.Items.Add("1X");
                CamLenseTypeCBx.Items.Add("2X");
                CamLenseTypeCBx.Items.Add("5X");
                CamLenseTypeCBx.Text = Settings_DataB.Select_ChildControls("VisionSettings", "MachineNo", MachineNo)[0]["CamLenseType"].ToString();

                //================================================Mag Level Text Boxes and Check Boxes================================================//
                TextBox[] PixWidthTxtBxs = { MagLevel3XTxtBx, MagLevel5XTxtBx, MagLevel11XTxtBx, MagLevel22XTxtBx, MagLevel34XTxtBx,
                                         MagLevel54XTxtBx, MagLevel67XTxtBx, MagLevel134XTxtBx, MagLevel335XTxtBx};
                TextBox[] PixWidthYDirTxtBxs = { MagLevel3XTxtBx_YDir, MagLevel5XTxtBx_YDir, MagLevel11XTxtBx_YDir, MagLevel22XTxtBx_YDir, MagLevel34XTxtBx_YDir,
                                         MagLevel54XTxtBx_YDir, MagLevel67XTxtBx_YDir, MagLevel134XTxtBx_YDir, MagLevel335XTxtBx_YDir};
                CheckBox[] MagHiddenChkBxs = { MagLevel3XHiddenChkBx, MagLevel5XHiddenChkBx, MagLevel11XHiddenChkBx, MagLevel22XHiddenChkBx, MagLevel34XHiddenChkBx,
                                         MagLevel54XHiddenChkBx, MagLevel67XHiddenChkBx, MagLevel134XHiddenChkBx, MagLevel335XHiddenChkBx};
                CheckBox[] MagProLight11XChkBxs = { MagLevel3XProLight11XChkBx, MagLevel5XProLight11XChkBx, MagLevel11XProLight11XChkBx, MagLevel22XProLight11XChkBx, MagLevel34XProLight11XChkBx,
                                         MagLevel54XProLight11XChkBx, MagLevel67XProLight11XChkBx, MagLevel134XProLight11XChkBx, MagLevel335XProLight11XChkBx};
                TextBox[] DigitalZoomX ={ DigitalZoom3XTxtBx_XDir, DigitalZoomLevel5XTxtBx_XDir,  DigitalZoomLevel11XTxtBx_XDir,  DigitalZoomLevel22XTxtBx_XDir,  DigitalZoomLevel34XTxtBx_XDir,
                                          DigitalZoomLevel54XTxtBx_XDir,  DigitalZoomLevel67XTxtBx_XDir,  DigitalZoomLevel134XTxtBx_XDir,  DigitalZoomLevel335XTxtBx_XDir};
                TextBox[] DigitalZoomY ={ DigitalZoom3XTxtBx_YDir, DigitalZoomLevel5XTxtBx_YDir,  DigitalZoomLevel11XTxtBx_YDir,  DigitalZoomLevel22XTxtBx_YDir,  DigitalZoomLevel34XTxtBx_YDir,
                                          DigitalZoomLevel54XTxtBx_YDir,  DigitalZoomLevel67XTxtBx_YDir,  DigitalZoomLevel134XTxtBx_YDir,  DigitalZoomLevel335XTxtBx_YDir};
                //Pixel Width Values
                for (int i = 0; i < MagRows.Length; i++)
                {
                    PixWidthTxtBxs[i].Text = Convert.ToDouble(MagRows[i]["PixelWidth"]).ToString();
                    MagHiddenChkBxs[i].IsChecked = Convert.ToBoolean(MagRows[i]["Hidden"]);
                    MagProLight11XChkBxs[i].IsChecked = Convert.ToBoolean(MagRows[i]["ElvnXProfileLight"]);
                    PixWidthYDirTxtBxs[i].Text = Convert.ToDouble(MagRows[i]["PixelWidthY"]).ToString();
                    DigitalZoomX[i].Text = Convert.ToDouble(MagRows[i]["DigitalZoomPixel_X"]).ToString();
                    DigitalZoomY[i].Text = Convert.ToDouble(MagRows[i]["DigitalZoomPixel_Y"]).ToString();
                }

            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW13", ex); }
        }
        //Tab for DRO settings
        void TabDROSettingsFill()
        {
            try
            {
                //=============================================================Text Boxes=============================================================//
                double[] lashValue = new double[4], axisMultiplier = new double[4], countsPerunit = new double[4];
                bool[] movementType = new bool[4], movementDirection = new bool[4];
                //RW_DRO axis details...///
                for (int i = 0; i < DROROWS.Length; i++)
                {
                    //droLabels += Convert.ToString(DROROWS[i]["AxisName"]);
                    movementType[i] = Convert.ToBoolean(DROROWS[i]["AxisType"]);
                    countsPerunit[i] = Convert.ToDouble(DROROWS[i]["AxisCount"]);
                    axisMultiplier[i] = Convert.ToDouble(DROROWS[i]["AxisMultiplier"]);
                    lashValue[i] = Convert.ToDouble(DROROWS[i]["AxisLash"]);
                    //movementDirection[i] = Convert.ToBoolean(DROROWS[i]["AxisPolarity"]);
                }
                TextBox[] CPUTxtBxs = { XAxisCperUTxtBx, YAxisCperUTxtBx, ZAxisCperUTxtBx, RAxisCperUTxtBx };
                TextBox[] AxisMulTxtBxs = { XAxisMultiplierTxtBx, YAxisMultiplierTxtBx, ZAxisMultiplierTxtBx, RAxisMultiplierTxtBx };
                TextBox[] LashValTxtBxs = { XAxisLashTxtBx, YAxisLashTxtBx, ZAxisLashTxtBx, RAxisLashTxtBx };
                CheckBox[] AxisTypeChkBxs = { XAxisLinearChkBx, YAxisLinearChkBx, ZAxisLinearChkBx, RAxisLinearChkBx };

                for (int i = 0; i < movementType.Length; i++)
                {
                    CPUTxtBxs[i].Text = Convert.ToDouble(countsPerunit[i]).ToString();
                    AxisMulTxtBxs[i].Text = Convert.ToDouble(axisMultiplier[i]).ToString();
                    LashValTxtBxs[i].Text = Convert.ToDouble(lashValue[i]).ToString();
                    AxisTypeChkBxs[i].IsChecked = movementType[i];
                }

                FillTextBox(LimitHitCountTxtBx, "MachineDetails", "MachineNo", MachineNo, "LimitHitCount", true);
                FillTextBox(McDisconnectCountTxtBx, "MachineDetails", "MachineNo", MachineNo, "MachineDisconnectCount", true);

                //=============================================================Homing Settings=============================================================//
                HomePositiveXValCBx.Items.Clear();
                HomePositiveXValCBx.Items.Add("15");
                HomePositiveXValCBx.Items.Add("20");
                HomePositiveXValCBx.Items.Add("40");
                HomePositiveYValCBx.Items.Clear();
                HomePositiveYValCBx.Items.Add("15");
                HomePositiveYValCBx.Items.Add("20");
                HomeToPositiveXChkBx.IsChecked = Convert.ToBoolean(DROROWS[0]["AxisHomeToPositive"]);
                HomeToPositiveYChkBx.IsChecked = Convert.ToBoolean(DROROWS[1]["AxisHomeToPositive"]);
                if ((bool)HomePositiveXValCBx.IsEnabled)
                {
                    int ExtremityVal = Convert.ToInt32(DROROWS[0]["AxisPositiveHomeValue"]);
                    if (ExtremityVal == 200)
                        HomePositiveXValCBx.SelectedIndex = 1;
                    else if (ExtremityVal == 400)
                        HomePositiveXValCBx.SelectedIndex = 2;
                    else
                        HomePositiveXValCBx.SelectedIndex = 0;
                }
                if ((bool)HomePositiveYValCBx.IsEnabled)
                {
                    int ExtremityVal = Convert.ToInt32(DROROWS[1]["AxisPositiveHomeValue"]);
                    if (ExtremityVal == 200)
                        HomePositiveYValCBx.SelectedIndex = 1;
                    else
                        HomePositiveYValCBx.SelectedIndex = 0;
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW14", ex); }
        }
        //Tab for EEPROM settings
        public void TabEEPROMSettingsFill()
        {
            try
            {
                //=============================================================Text Boxes=============================================================//
                TextBox[] EEPROMSettingsTxtBxs = { X_AccuracyTxtBx, Y_AccuracyTxtBx, Z_AccuracyTxtBx, R_AccuracyTxtBx,
                                                 X_PitchTxtBx, Y_PitchTxtBx, Z_PitchTxtBx, R_PitchTxtBx,
                                                 X_AccelerationTxtBx, Y_AccelerationTxtBx, Z_AccelerationTxtBx, R_AccelerationTxtBx,
                                                 X_DecelerationTxtBx, Y_DecelerationTxtBx, Z_DecelerationTxtBx, R_DecelerationTxtBx,
                                                 X_JoyStkSpeedFactorTxtBx, Y_JoyStkSpeedFactorTxtBx, Z_JoyStkSpeedFactorTxtBx, R_JoyStkSpeedFactorTxtBx,
                                                 X_Resolution, Y_Resolution, Z_Resolution, R_Resolution};
                String[] ColNames = { "Accuracy_X", "Accuracy_Y", "Accuracy_Z", "Accuracy_R",
                                      "Pitch_X", "Pitch_Y", "Pitch_Z", "Pitch_R",
                                      "Acceleration_X", "Acceleration_Y", "Acceleration_Z", "Acceleration_R",
                                      "Deceleration_X", "Deceleration_Y", "Deceleration_Z", "Deceleration_R",
                                      "JoyStickSpeedFactor_X", "JoyStickSpeedFactor_Y", "JoyStickSpeedFactor_Z", "JoyStickSpeedFactor_R",
                                    "Resolution_X", "Resolution_Y", "Resolution_Z", "Resolution_R"};

                for (int i = 0; i < EEPROMSettingsTxtBxs.Length; i++)
                {
                    AxisNumberProperties[i] = Convert.ToInt32(Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0][ColNames[i]]);
                    EEPROMSettingsTxtBxs[i].Text = AxisNumberProperties[i].ToString();
                }

                //=============================================================Check Boxes and Radio Btns=============================================================//
                CheckBox[] EEPROMSettingsChkBxs = { X_ProbeActivatedChkBx, Y_ProbeActivatedChkBx, Z_ProbeActivatedChkBx, R_ProbeActivatedChkBx };
                RadioButton[] EEPROMSettingsScalePositiveRBtns = { X_ScaleDir_PositiveRBtn, Y_ScaleDir_PositiveRBtn, Z_ScaleDir_PositiveRBtn, R_ScaleDir_PositiveRBtn };
                RadioButton[] EEPROMSettingsScaleNegativeRBtns = { X_ScaleDir_NegativeRBtn, Y_ScaleDir_NegativeRBtn, Z_ScaleDir_NegativeRBtn, R_ScaleDir_NegativeRBtn };
                RadioButton[] EEPROMSettingsMotorPositiveRBtns = { X_MotorDir_PositiveRBtn, Y_MotorDir_PositiveRBtn, Z_MotorDir_PositiveRBtn, R_MotorDir_PositiveRBtn };
                RadioButton[] EEPROMSettingsMotorNegativeRBtns = { X_MotorDir_NegativeRBtn, Y_MotorDir_NegativeRBtn, Z_MotorDir_NegativeRBtn, R_MotorDir_NegativeRBtn };
                CheckBox[] EEPROM_AxisEnableBoxes = { X_AxisEnabledChkBx, Y_AxisEnabledChkBx, Z_AxisEnabledChkBx, R_AxisEnabledChkBx };

                string[] EEPROMSettingsProbeActivateColName = { "ProbeActivation_X", "ProbeActivation_Y", "ProbeActivation_Z", "ProbeActivation_R" };
                string[] EEPROMSettingsScaleDirColName = { "ScalePositiveDirection_X", "ScalePositiveDirection_Y", "ScalePositiveDirection_Z", "ScalePositiveDirection_R" };
                string[] EEPROMSettingsMotorDirColName = { "MotorPositiveDirection_X", "MotorPositiveDirection_Y", "MotorPositiveDirection_Z", "MotorPositiveDirection_R" };
                int axisenableValue = (int)Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["AxisEnable_Value"];
                Chk_DRO_24Bit.IsChecked = (bool)Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["DRO_24_Bit"];

                string binaryaxisvalue = Convert.ToString(axisenableValue, 2);
                for (int jj = 0; jj < 4 - binaryaxisvalue.Length; jj++)
                    binaryaxisvalue = "0" + binaryaxisvalue;
                char[] axisvalueArray = binaryaxisvalue.ToCharArray();

                for (int i = 0; i < EEPROMSettingsChkBxs.Length; i++)
                {
                    AxisBooleanProperties[3 * i] = (bool)Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0][EEPROMSettingsProbeActivateColName[i]];
                    AxisBooleanProperties[3 * i + 1] = (bool)Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0][EEPROMSettingsScaleDirColName[i]];
                    AxisBooleanProperties[3 * i + 2] = (bool)Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0][EEPROMSettingsMotorDirColName[i]];

                    EEPROMSettingsChkBxs[i].IsChecked = AxisBooleanProperties[3 * i];

                    EEPROM_AxisEnableBoxes[3-i].IsChecked = Convert.ToBoolean(int.Parse(axisvalueArray[i].ToString()));

                    if (AxisBooleanProperties[3 * i + 1])
                        EEPROMSettingsScalePositiveRBtns[i].IsChecked = true;
                    else
                        EEPROMSettingsScaleNegativeRBtns[i].IsChecked = true;

                    if (AxisBooleanProperties[3 * i + 2])
                        EEPROMSettingsMotorPositiveRBtns[i].IsChecked = true;
                    else
                        EEPROMSettingsMotorNegativeRBtns[i].IsChecked = true;
                }
                AllowCNC = (bool)Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["AllowCNC"];
                EnableCNCChkBx.IsChecked = AllowCNC;
                AllowAutoZoom = (bool)Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["AllowAutoZoom"];
                EnableAutoZoomChkBx.IsChecked = AllowAutoZoom;
                AllowInterpolation = (bool)Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["AllowInterpolation"];
                AllowInterpolationChkBx.IsChecked = AllowInterpolation;
                SmoothCircularInterpolation = (bool)Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["SmoothCircularInterpolation"];

                RetractProbePointMode = (bool)Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["RetractProbePointMode"];
                RetractProbePointModeChkBx.IsChecked = (bool)RetractProbePointMode;
                OnCheckBoxClicked(RetractProbePointModeChkBx, null);

                SetProbeRetractMode.IsChecked = (bool)Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["SetProbeRetractMode"];

                if (SmoothCircularInterpolation) SmoothCircularInterpolationRBtn.IsChecked = true; else PolygonCircularInterpolationRBtn.IsChecked = true;
                RouteProbePath = (bool)Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["RouteProbePath"];
                RouteProbePathChkBx.IsChecked = RouteProbePath;
                txt_Accel_Manual.Text = Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["Accel_Manual"].ToString();
                if (txt_Accel_Manual.Text == "") txt_Accel_Manual.Text = "0";
                txt_Accel_CNC.Text = Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["Accel_CNC"].ToString();
                if (txt_Accel_CNC.Text == "") txt_Accel_CNC.Text = "0";
                txt_JS_JogSpeed.Text = Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["JS_JogSpeed"].ToString();
                if (txt_JS_JogSpeed.Text == "") txt_JS_JogSpeed.Text = "0";
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW14a", ex); }
        }
        //Tab for Surface FG settings
        void TabSurfaceFGSettingsFill()
        {
            try
            {
                int AlgoType = Convert.ToInt32(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["SelectedAlgorithm"]);
                switch (AlgoType)
                {
                    case 0:
                        AlgoTypeCBx.SelectedIndex = 0;
                        break;
                    case 1:
                        AlgoTypeCBx.SelectedIndex = 1;
                        break;
                    case 2:
                        AlgoTypeCBx.SelectedIndex = 2;
                        break;
                    case 3:
                        AlgoTypeCBx.SelectedIndex = 3;
                        break;
                    case 4:
                        AlgoTypeCBx.SelectedIndex = 4;
                        break;
                    case 5:
                        AlgoTypeCBx.SelectedIndex = 5;
                        break;
                }
                //=============================================================Text Boxes=============================================================//
                TextBox[] SurfaceFGSettingsTxtBxs = { Type1_LookAheadValTxtBx, Type1_BlackTolTxtBx, Type1_FilterMaskTolTxtBx,Type1_WhiteTolTxtBx,Type1_FilterMaskTolB2WTxtBx,
                                                      Type2_LookAheadValTxtBx, Type2_CurrAbsThresholdTxtBx, Type2_CurrDiffThresholdTxtBx, Type2_LookaheadAbsThresholdTxtBx, Type2_LookaheadDiffThresholdTxtBx, Type2_MedianFilterBxSizeTxtBx, Type2_AvgFilterBxSizeTxtBx,
                                                      Type3_LookAheadValTxtBx, Type3_BlackTolTxtBx, Type3_FilterMaskTolTxtBx,Type3_WhiteTolTxtBx,Type3_FilterMaskTolB2WTxtBx,
                                                      Type4_LookAheadValTxtBx, Type4_TextureDiffThresholdTxtBx, Type4_AcrossEdgeIncrTxtBx, Type4_AlongEdgeIncrTxtBx, Type4_AcrossEdgeBxThicknessTxtBx, Type4_AlongEdgeBxThicknessTxtBx,
                                                      Type5_LocalMaximaExtentTxtBx, Type5_DiffThresholdTxtBx, Type5_FilterMaskSizeTxtBx ,
                                                      T6MAskFactor , T6BinaryFactorLower , T6BinaryFactorUpper , T6MaskingLevel , T6PixelThreshold , T6PixelNeighbour ,T6PixelScanRange,T6CutoffPixels,T6JumpThreshold };
                String[] ColNames = { "T1LookAheadValue", "T1BlackTolerance", "T1FilterMaskTolerance","T1WhiteTolerance","T1FilterMaskToleranceB2W",
                                      "T2LookAheadValue", "T2CurrAbsThreshold", "T2CurrDiffThreshold", "T2LookaheadAbsThreshold", "T2LookaheahDiffThreshold", "T2MedianFilterBoxSize", "T2AverageFilterBoxSize",
                                      "T3LookAheadValue", "T3BlackTolerance", "T3FilterMaskTolerance","T3WhiteTolerance","T3FilterMaskToleranceB2W",
                                      "T4LookAheadValue", "T4TextureDiffThreshold", "T4AcrossEdgeIncrement", "T4AlongEdgeIncrement", "T4AcrossEdgeBoxThickness", "T4AlongEdgeBoxThickness",
                                      "T5LocalMaximaExtent", "T5DiffThreshold", "T5FilterMaskSize" ,"T6MaskFactor",
                                      "T6LowerBinaryFactor","T6UpperBinaryFactor","T6MaskingLevel","T6PixelThreshold" , "T6PixelNeighbour" , "T6PixelScanRange" , "T6CutoffPixels" , "T6JumpThreshold" };

                for (int i = 0; i < SurfaceFGSettingsTxtBxs.Length; i++)
                    FillTextBox(SurfaceFGSettingsTxtBxs[i], "SurfaceFGSettings", "MachineNo", MachineNo, ColNames[i]);

                bool T6ApplySubAlgoVar = (bool)Settings_DataB.Select_ChildControls("SurfaceFGSettings", "MachineNo", MachineNo)[0]["T6ApplySubAlgo"];
                T6ApplySubAlgo.IsChecked = T6ApplySubAlgoVar;
                bool T6ApplyDirectionalScanVar = (bool)Settings_DataB.Select_ChildControls("SurfaceFGSettings", "MachineNo", MachineNo)[0]["T6ApplyDirectionalScan"];
                T6ApplyDirectionalScan.IsChecked = T6ApplyDirectionalScanVar;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW14b", ex); }
        }
        //Tab for Camera Axis value
        void TabCameraAxisValueFill()
        {
            if (GlobalSettings.MultilevelZoomMahine)
            {
                //MultiZoomLevelTab.Visibility = System.Windows.Visibility.Visible;
                ////================================================Camera Axis Value Text Boxes and Check Boxes================================================//
                //TextBox[] PixWidthTxtBxs = { MultiZoomLevel11XTxtBx, MultiZoomLevel15XTxtBx, MultiZoomLevel20XTxtBx, MultiZoomLevel24XTxtBx, MultiZoomLevel28XTxtBx,
                //                         MultiZoomLevel33XTxtBx, MultiZoomLevel37XTxtBx, MultiZoomLevel41XTxtBx, MultiZoomLevel46XTxtBx,MultiZoomLevel50XTxtBx,MultiZoomLevel54XTxtBx,MultiZoomLevel58XTxtBx,MultiZoomLevel63XTxtBx,MultiZoomLevel67XTxtBx};
                //TextBox[] AxisValueTxtBxs = { MultiZoomLevel11XTxtBx_AxisValue, MultiZoomLevel15XTxtBx_AxisValue, MultiZoomLevel20XTxtBx_AxisValue, MultiZoomLevel24XTxtBx_AxisValue, MultiZoomLevel28XTxtBx_AxisValue,MultiZoomLevel33XTxtBx_AxisValue,MultiZoomLevel37XTxtBx_AxisValue,
                //                         MultiZoomLevel141XTxtBx_AxisValue, MultiZoomLevel46XTxtBx_AxisValue, MultiZoomLevel50XTxtBx_AxisValue, MultiZoomLevel54XTxtBx_AxisValue,MultiZoomLevel58XTxtBx_AxisValue,MultiZoomLevel63XTxtBx_AxisValue,MultiZoomLevel67XTxtBx_AxisValue};
                //CheckBox[] CAVHiddenChkBxs = { MultiZoomLevel11XHiddenChkBx, MultiZoomLevel15XHiddenChkBx, MultiZoomLevel20XHiddenChkBx, MultiZoomLevel24XHiddenChkBx, MultiZoomLevel28XHiddenChkBx,MultiZoomLevel33XHiddenChkBx,MultiZoomLevel37XHiddenChkBx,MultiZoomLevel41XHiddenChkBx,
                //                         MultiZoomLevel46XHiddenChkBx, MultiZoomLevel50XHiddenChkBx, MultiZoomLevel54XHiddenChkBx, MultiZoomLevel58XHiddenChkBx,MultiZoomLevel63XHiddenChkBx,MultiZoomLevel67XHiddenChkBx};
                ////Pixel Width Values
                //for (int i = 0; i < CamAxisRows.Length; i++)
                //{
                //    PixWidthTxtBxs[i].Text = Convert.ToDouble(CamAxisRows[i]["PixelValue_R"]).ToString();
                //    CAVHiddenChkBxs[i].IsChecked = Convert.ToBoolean(CamAxisRows[i]["Hidden"]);
                //    AxisValueTxtBxs[i].Text = Convert.ToDouble(CamAxisRows[i]["AxisValue_R"]).ToString();
                //}
                try
                {
                    CamAxisRows = Settings_DataB.Select_ChildControls("CameraAxisValue", "MachineNo", MachineNo);
                    if (CamAxisRows.Length > 0)
                        ZoomCalibrationTable = CamAxisRows.CopyToDataTable();
                    else
                        ZoomCalibrationTable = Settings_DataB.GetTable("CameraAxisValue").Clone();
                }
                catch (Exception)
                {
                    ZoomCalibrationTable = Settings_DataB.GetTable("CameraAxisValue").Clone();
                }
                dgZoomCalibration.ItemsSource = ZoomCalibrationTable.DefaultView;
                dgZoomCalibration.UpdateLayout();
            }
            else
            {
                MultiZoomLevelTab.Visibility = System.Windows.Visibility.Hidden;
            }
        }
        //Tab FilterationValues
        void TabFilterationValueFill()
        {
            ApplyFilterationCkbx.IsChecked = Convert.ToBoolean(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["ApplyFilteration"]);
            FilterIterationFactorTxtBx.Text = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["FilterIterationFactor"].ToString();
            FilterFactorTxtBx.Text = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["FilterFactor"].ToString();
        }
        //Tab Rotary Measurement
        void TabRotaryMeasurementFill()
        {
            try
            {
                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
                {
                    RotaryMeasurementColl = new ObservableCollection<RotaryMeasurementTypetails>();
                    DataRow[] RotaryMeasurementDataRow = GlobalSettings.SettingsReader.GetRowsAccordingToSelectedMachineNumber("RotaryMeasurementTypeDetails", MachineNo);
                    for (int i = 0; i < RotaryMeasurementDataRow.Length; i++)
                    {
                        RotaryMeasurementTypetails RMTD = new RotaryMeasurementTypetails();
                        RMTD.ComponentName = RotaryMeasurementDataRow[i]["ComponentName"].ToString();
                        RMTD.ZeroingAngle = Convert.ToDouble(RotaryMeasurementDataRow[i]["ZeroingAngle"].ToString());
                        RMTD.ID = Convert.ToInt32(RotaryMeasurementDataRow[i]["ID"].ToString());
                        RotaryMeasurementColl.Add(RMTD);
                    }
                    RotaryDG.ItemsSource = RotaryMeasurementColl;
                    FillCheckBox(ViewChbk, "UserSettings", "MachineNo", MachineNo, "DelfiFloatingView");
                    //HobBCXTxtBx.Text = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["HobBCX"].ToString(); ;
                    //HobBCYTxtBx.Text = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["HobBCY"].ToString(); ;
                }
                else if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Horizontal)
                {
                    if (GlobalSettings.RotaryAxisForMeasurement)
                    {
                        FillTextBox(txtRotaryScanSpan, "TIS_Settings", "MachineNo", MachineNo, "RotaryScanSpan");
                        FillTextBox(txtRotaryScanSpeed, "TIS_Settings", "MachineNo", MachineNo, "RotaryScanSpeed");
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW14d", ex);
            }
        }
        //Fill up valuse in OneShot tab
        void TabOneShotParamsFill()
        {
            int i = 0;
            //Iterate through each of the controls in the wrap panel, and then find out which are the textboxes. 
            // in each of these textboxes, fill up our values. 
            foreach (object obj in WP_PixelWidth.Children)
            {
                if (obj.GetType() == typeof(TextBox))
                {
                    TextBox tb = (TextBox)obj;
                    tb.Text = Convert.ToString(GlobalSettings.OneShotPixelWidthCorrections[i]);
                    i++;
                }
            }
            foreach (object obj in WP_BD.Children)
            {
                if (obj.GetType() == typeof(TextBox))
                {
                    TextBox tb = (TextBox)obj;
                    tb.Text = Convert.ToString(GlobalSettings.OneShotPixelWidthCorrections[i]);
                    i++;
                }
            }
            FillTextBox(txtProfile_Slope, "OneShotParams", "MachineNo", MachineNo, "Profile_Slope", true);
            FillTextBox(txtProfile_BaseValue, "OneShotParams", "MachineNo", MachineNo, "Profile_BaseValue", true);
            FillTextBox(txtSurface_Slope, "OneShotParams", "MachineNo", MachineNo, "Surface_Slope", true);
            FillTextBox(txtSurface_BaseValue, "OneShotParams", "MachineNo", MachineNo, "Surface_BaseValue", true);

            PixelWidthCorrectionChkBx.IsChecked = GlobalSettings.PixelWidthCorrectionEnable;
            EnableBD_Corr.IsChecked = GlobalSettings.EnableBD;
        }


        //To fill textbox value
        void FillTextBox(TextBox TxtBx, string TableName, string ColName, object Parameter, string ColHeader, bool ConvertToDouble = false, bool Include_MulFactor = false)
        {
            int lineN = 0;
            try
            {
                if (Include_MulFactor)
                {
                    if (ConvertToDouble)
                    {
                        lineN = 1;
                        string value = Settings_DataB.Select_ChildControls(TableName, ColName, Parameter)[0][ColHeader].ToString();
                        int intValue = 0;
                        if (int.TryParse(value, out intValue))
                        {
                            lineN = 2;
                            TxtBx.Text = intValue.ToString();
                        }
                        else
                        {
                            lineN = 4;
                            TxtBx.Text = String.Format("{0:F" + GlobalSettings.CurrentDroPrecision + "}", double.Parse(value));
                        }
                    }
                    else
                    {
                        lineN = 6;
                        TxtBx.Text = Settings_DataB.Select_ChildControls(TableName, ColName, Parameter)[0][ColHeader].ToString();
                    }
                }
                else
                {
                    if (ConvertToDouble)
                    {
                        lineN = 8;
                        string value = Settings_DataB.Select_ChildControls(TableName, ColName, Parameter)[0][ColHeader].ToString();
                        int intValue = 0;
                        if (int.TryParse(value, out intValue))
                        {
                            lineN = 10;
                            TxtBx.Text = intValue.ToString();
                        }
                        else
                        {
                            lineN = 12;
                            TxtBx.Text = String.Format("{0:F" + GlobalSettings.CurrentDroPrecision + "}", double.Parse(value));
                        }
                    }
                    else
                    {
                        lineN = 14;
                        TxtBx.Text = Settings_DataB.Select_ChildControls(TableName, ColName, Parameter)[0][ColHeader].ToString();
                    }
                }
            }
            catch (Exception ex)
            {
                //MessageBox.Show(TableName + ", " + ColName + ", " + ColHeader + ", " + lineN);
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW15_" + lineN, ex);
            }
        }
        //To set checkbox value
        void FillCheckBox(CheckBox ChkBx, string TableName, string ColName, object Parameter, string ColHeader, bool Opposite = false)
        {
            try
            {
                bool tempValue = Convert.ToBoolean(Settings_DataB.Select_ChildControls(TableName, ColName, Parameter)[0][ColHeader]);
                ChkBx.IsChecked = tempValue;
                if (Opposite) ChkBx.IsChecked = !tempValue;

            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW16", ex); }
        }

        #endregion

        #region Functions to Update Database with front end values

        //Update button click handler
        void UpdateBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                UpdateAll();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW17", ex); }
        }
        //Function to call each tab update appropriately
        public void UpdateAll()
        {
            try
            {
                UpdateTableNames = new List<string> {"UserSettings", "DXFSettings", "MeasurementSettings", "LightingSettings",
                        "MachineDetails", "VisionSettings", "MagnifierSettings", "EEPROMSettings", "ProbeSettings", "SurfaceFGSettings", "OneShotParams", "TIS_Settings"};
                CurrentMachineDataR = new System.Data.DataRow[UpdateTableNames.Count];
                bool MachineNumExists = false;
                int RowCount;

                //Making new rows of each and every table(if required).
                if (GlobalSettings.CurrentUserLevel == GlobalSettings.UserPrevilageLevel.System)
                {
                    MachineNo = MachineNumCBx.Text;
                    if (Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo).Length > 0)//MachineNo is present
                    {
                        for (int RCount = 0; RCount < (UpdateTableNames.Count); RCount++)
                        {
                            if (Settings_DataB.Select_ChildControls(UpdateTableNames[RCount], "MachineNo", MachineNo).Length > 0)                               //1 Written by Harphool
                            {                                                                                                                                   //2
                                CurrentMachineDataR[RCount] = Settings_DataB.Select_ChildControls(UpdateTableNames[RCount], "MachineNo", MachineNo)[0];
                            }                                                                                                                                   //3
                            else                                                                                                                                //4
                            {                                                                                                                                   //5
                                CurrentMachineDataR[RCount] = Settings_DataB.GetTable(UpdateTableNames[RCount]).NewRow();                                       //6
                                CurrentMachineDataR[RCount]["MachineNo"] = MachineNo;                                                                           //7
                                Settings_DataB.GetTable(UpdateTableNames[RCount]).Rows.Add(CurrentMachineDataR[RCount]);                                        //8
                            }                                                                                                                                   //9
                        }
                        MachineNumExists = true;
                        GlobalSettings.MachineNo = MachineNo;
                    }
                    else
                    {
                        //Update the machine no in the Machine details & give refrence in CurrentMachineDataR to be of new rows.
                        for (int RCount = 0; RCount < (UpdateTableNames.Count); RCount++)
                        {
                            CurrentMachineDataR[RCount] = Settings_DataB.GetTable(UpdateTableNames[RCount]).NewRow();
                        }

                        //Initialise new rows for DRO settings
                        RowCount = DROROWS.Length;
                        //DROROWS = new System.Data.DataRow[RowCount];
                        for (int c = 0; c < RowCount; c++)
                            DROROWS[c] = Settings_DataB.GetTable("DROSettings").NewRow();

                        //Initialise new rows for Mag levels
                        RowCount = MagRows.Length;
                        string MagnificationLevels;
                        //MagRows = new System.Data.DataRow[RowCount];
                        for (int c = 0; c < RowCount; c++)
                        {
                            MagnificationLevels = MagRows[c]["MagLevelValue"].ToString();
                            MagRows[c] = Settings_DataB.GetTable("MagLevels").NewRow();
                            MagRows[c]["MagLevelValue"] = MagnificationLevels;
                        }

                        //Initialise new row for fixture details
                        System.Data.DataRow dr = Settings_DataB.DataS.Tables["FixtureDetails"].NewRow();
                        dr["MachineNo"] = MachineNo;
                        dr["FixtureName"] = "Default";
                        dr["No_of_Rows"] = "1";
                        dr["No_of_Columns"] = "1";
                        dr["RowGap"] = "0";
                        dr["ColumnGap"] = "0";
                        Settings_DataB.DataS.Tables["FixtureDetails"].Rows.Add(dr);
                    }
                }
                else
                {
                    for (int RCount = 0; RCount < CurrentMachineDataR.Length; RCount++)
                        CurrentMachineDataR[RCount] = Settings_DataB.Select_ChildControls(UpdateTableNames[RCount], "MachineNo", MachineNo)[0];
                    MachineNumExists = true;
                }

                try
                {
                    switch (GlobalSettings.CurrentUserLevel)
                    {
                        case GlobalSettings.UserPrevilageLevel.None:
                        case GlobalSettings.UserPrevilageLevel.User:
                            UpdateUser();
                            break;
                        case GlobalSettings.UserPrevilageLevel.AdminUser:
                            UpdateAdminUser();
                            break;
                        case GlobalSettings.UserPrevilageLevel.Admin:
                            UpdateAdmin();
                            break;
                        case GlobalSettings.UserPrevilageLevel.System:
                            if (UpdatingEEPROMValuesNow)
                            {
                                TabEEPROMSettingsUpdate();
                                //MessageBox.Show("EEPROM was updated Successfully!!", "Rapid-I");
                                EEpromUpdateResult.Text = "Updated Successfully";
                                return;
                            }
                            else
                                UpdateSystem();
                            break;
                    }
                    if (UpdateRemainingValues(MachineNumExists))
                        MessageBox.Show("Update is Successfully done!!", "Rapid-I");
                    else
                        MessageBox.Show("Update was not successful!! Please Check if the Settings file is writable.", "Rapid-I");
                    //if (!UpdatingEEPROMValuesNow) this.Close();
                    this.Close();
                }
                catch (Exception e)
                {
                    MessageBox.Show("Error: Values not updated properly!!" + "      " + e.Message + "    " + e.StackTrace, "Rapid-I");
                    this.Close();
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW18", ex); }
        }

        //Function to call appropriate update for user previlage level
        void UpdateUser()
        {
            try
            {
                TabDisplayUpdate();
                TabPartprogramUpdate();
                TabRotaryMeasurementUpdate();
                TabOneShotUpdate();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW19", ex); }
        }
        //Function to call appropriate update for admin user previlage level
        void UpdateAdminUser()
        {
            try
            {
                TabDisplayUpdate();
                TabPartprogramUpdate();
                TabAdvancedUpdate();
                TabTouchProbeUpdate();
                TabRotaryMeasurementUpdate();
                TabOneShotUpdate();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW20", ex); }
        }
        //Function to call appropriate update for Admin previlage level
        void UpdateAdmin()
        {
            try
            {
                TabDisplayUpdate();
                TabPartprogramUpdate();
                TabAdvancedUpdate();
                TabTouchProbeUpdate();
                TabUserSettingsUpdate();
                TabRotaryMeasurementUpdate();
                TabOneShotUpdate();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW21", ex); }
        }
        //Function to call appropriate update for System previlage level
        void UpdateSystem()
        {
            try
            {
                TabDisplayUpdate();
                TabPartprogramUpdate();
                TabAdvancedUpdate();
                TabTouchProbeUpdate();
                TabUserSettingsUpdate();
                TabMachineDetailsUpdate();
                TabVideoAndCameraSettingsUpdate();
                TabDROSettingsUpdate();
                TabSurfaceFGSettingsUpdate();
                TabCameraAxisValueUpdate();
                TabFilterationValueUpdate();
                TabRotaryMeasurementUpdate();
                TabOneShotUpdate();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW22", ex); }
        }
        //Function to get index of table name from array
        int UpdateTableIndex(string TName)
        {
            try
            {
                return UpdateTableNames.IndexOf(TName);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW23", ex);
                return 0;
            }
        }
        //Update display settings
        void TabDisplayUpdate()
        {
            try
            {
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["SpeedX"] = Convert.ToDouble(SpeedXTxtBx.Text) * UnitType_MulFactor;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["SpeedY"] = Convert.ToDouble(SpeedYTxtBx.Text) * UnitType_MulFactor;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["SpeedZ"] = Convert.ToDouble(SpeedZTxtBx.Text) * UnitType_MulFactor;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["SpeedR"] = Convert.ToDouble(SpeedRTxtBx.Text) * UnitType_MulFactor;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["MaximumValue"] = Convert.ToDouble(MaxValTxtBx.Text) * UnitType_MulFactor;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["MinimumValue"] = Convert.ToDouble(MinValTxtBx.Text) * UnitType_MulFactor;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["StepValue"] = Convert.ToDouble(StepValTxtBx.Text) * UnitType_MulFactor;
                GlobalSettings.SpeedMinVal = Convert.ToDouble(MinValTxtBx.Text) * UnitType_MulFactor;
                GlobalSettings.SpeedMaxVal = Convert.ToDouble(MaxValTxtBx.Text) * UnitType_MulFactor;
                GlobalSettings.SpeedStepVal = Convert.ToDouble(StepValTxtBx.Text) * UnitType_MulFactor;
                FeedRateChangedEvent.Invoke();

                CheckBox[] AxisPolarityChkBxs = { PolarityXChkBx, PolarityYChkBx, PolarityZChkBx, PolarityRChkBx };
                for (int i = 0; i < DROROWS.Length; i++)
                    DROROWS[i]["AxisPolarity"] = AxisPolarityChkBxs[i].IsChecked;

                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["ShowNameTaginVideo"] = ShwNameTagInVidChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["SaveCrossHairWithImage"] = SaveCHairWithImgChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["DefaultInchMode"] = InchModeChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["IconsWithName"] = ShowIconNameChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["CloudPtsContourColouring"] = CldPtsContourColouringChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["InvertRcadImgBackColour"] = InvertRcadImgBackColourChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["SaveHomePosition"] = SaveHomePositionChkBx.IsChecked;
                GlobalSettings.SaveHomePosition = (bool)SaveHomePositionChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["SetCriticalShape"] = SetCriticalShapeChkBx.IsChecked;
                GlobalSettings.SetCriticalShapes = (bool)SetCriticalShapeChkBx.IsChecked;

                CurrentMachineDataR[UpdateTableIndex("DXFSettings")]["MaintainDxfShpContinuity"] = MaintainDxfShpContinuityChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("DXFSettings")]["ExportAllShpsToSingleUCS"] = ExportAllShpsToSingleUCSChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("DXFSettings")]["DXFRedBlueToleranceMode"] = RedBlueShapesAsTolLimitChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("DXFSettings")]["ExportDxfShapesAtZZero"] = ExportDxfShapesAtZZeroChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("DXFSettings")]["DoNotFloatDXFwithMouseMove"] = !chkDXF_Float.IsChecked;

                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["LanguageType"] = LanguageCBx.Text;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["DefaultMagLevel"] = DefaultMagLevelCBx.Text;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["DefaultImageFormat"] = DefaultImgFormatCBx.Text;

                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["GraphicsFontType"] = GlobalSettings.FontSelectedIndx;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["MeasurementGradientMode"] = MeasurementModeCBx.Text.ToString();

                CurrentMachineDataR[UpdateTableIndex("MachineDetails")]["RcadGradientBackColourTop"] = InterpretValFromColour(RcadGradClrUp);
                CurrentMachineDataR[UpdateTableIndex("MachineDetails")]["RcadGradientBackColourBottom"] = InterpretValFromColour(RcadGradClrDown);

                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["MinimumPointsRequiredToDetermineShape"] = Convert.ToInt32(MinPointsReqTxtBx.Text);
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["MinimumPixelDifferenceBetweenShapes"] = Convert.ToInt32(MinPixelCountTxtBx.Text);
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["AngleToleranceForCriticalShape"] = Convert.ToInt32(AngleTolTxtBx.Text);
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["PercentToleranceOnRadiusForCriticalShape"] = Convert.ToInt32(PercentTolOnRadiusTxtBx.Text);
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["MesureNoOfDec"] = MesureNoOfDecCbx.Text;
                CurrentMachineDataR[UpdateTableIndex("VisionSettings")]["RefDotDia"] = txtRefDotDia.Text;

                GlobalSettings.AngleTolerance = Convert.ToInt32(AngleTolTxtBx.Text);
                GlobalSettings.PercentageTolOnRadius = Convert.ToInt32(PercentTolOnRadiusTxtBx.Text);
                RWrapper.RW_MainInterface.MYINSTANCE().SetCriticalShapeParameter(GlobalSettings.SetCriticalShapes, (GlobalSettings.AngleTolerance * Math.PI / 180), GlobalSettings.PercentageTolOnRadius);
                if (CamResolCBx.SelectedIndex > -1)
                {
                    //if (!(GlobalSettings.RapidMachineType == GlobalSettings.MachineType.One_Shot && GlobalSettings.RapidCameraType == GlobalSettings.CameraType.ThreeM))
                    //{
                    //    CurrentMachineDataR[UpdateTableIndex("UserSettings")]["CameraResolution"] = CamResolCBx.SelectedIndex;
                    //    string[] SelectedResolution = CamResolCBx.Text.ToString().Split('-');
                    //    RWrapper.RW_MainInterface.MYINSTANCE().UpdateCameraZoom(Convert.ToInt32(SelectedResolution[0]), Convert.ToInt32(SelectedResolution[1]));
                    //}
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW24", ex); }
        }
        //Update part-program settings
        void TabPartprogramUpdate()
        {
            try
            {
                ////All the textBoxes
                //TextBox[] PPDetailsTxtBxs = { NoOfRowsTxtBx, NoOfColsTxtBx, RowGapTxtBx, ColGapTxtBx };
                //String[] ControlNames = { "No_of_Rows", "No_of_Columns", "RowGap", "ColumnGap" };
                //bool[] MulFactors = { false, false, true, true };

                //for (int i = 0; i < PPDetailsTxtBxs.Length; i++)
                //    if (MulFactors[i])
                //        CurrentMachineDataR[UpdateTableIndex("FixtureDetails")][ControlNames[i]] = Convert.ToDouble(PPDetailsTxtBxs[i].Text) * UnitType_MulFactor;
                //    else
                //        CurrentMachineDataR[UpdateTableIndex("FixtureDetails")][ControlNames[i]] = PPDetailsTxtBxs[i].Text;

                //CurrentMachineDataR[UpdateTableIndex("FixtureDetails")]["EnableGrid"] = EnableGridPrgChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["SwitchOffSnapModeDuringPP"] = SwitchOffSnapModeDuringPPChkBx.IsChecked;
                GlobalSettings.SwitchOffSnapModeDuringPP = (bool)SwitchOffSnapModeDuringPPChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["RunAllPPinCNCModeByDefault"] = RunAllPPinCNCModeByDefaultChkBx.IsChecked;
                GlobalSettings.RunAllPPinCNCModeByDefault = (bool)RunAllPPinCNCModeByDefaultChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["UseOptimisedPath"] = UseOptimisationInPPChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["RerunFailedMeasureStepsInPP"] = RerunFailedMeasurementsChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["XLExportRowWise"] = GlobalSettings.XLExportRowWise = (bool)XLExportRowWiseChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["XLExportPortraitLayout"] = GlobalSettings.XLExportPortraitLayout = (bool)XLExportPortraitChkBx.IsChecked;
                GlobalSettings.MainExcelInstance.DefaultReportSelection(!GlobalSettings.XLExportRowWise, !GlobalSettings.XLExportPortraitLayout);
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["XLExportRCadImage"] = GlobalSettings.XLExportRCadImgWithData = (bool)XLExportRCadImageChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["ExportShpPtsToCSVDuringPP"] = ExportShpPtsToCSVChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["ExportShpParamsToCSVDuringPP"] = ExportShpParamsToCSVChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["HideShapesAndMeasurements"] = GlobalSettings.ShapeMeasurementHidden = (bool)HideShapeAndMeasurementChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["EnableCsvExport"] = GlobalSettings.PartProgBuildGenerateCSV = (bool)EnableCsvExportChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("MachineDetails")]["TwoStepHoming"] = (bool)TwoStepHomingChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["EnableCsvExport"] = GlobalSettings.PartProgBuildGenerateCSV = (bool)EnableCsvExportChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["ShowAveraging"] = GlobalSettings.ShowAveragingInExcel = (bool)ShowAveragingChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["InsertImage"] = GlobalSettings.InsertImageForDataTransfer = (bool)InsertImgChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["ProtectXLwithPassword"] = GlobalSettings.InsertImageForDataTransfer = (bool)PasswordProtectExcelRptChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("VisionSettings")]["MakePDFReport"] = GlobalSettings.MakePDFReport = (bool)MakePDFReport.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("VisionSettings")]["PaginateReports"] = GlobalSettings.PaginateReports = (bool)PaginateReports.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("VisionSettings")]["MultiReportsInContinuousMode"] = GlobalSettings.MultiReportsContinuousMode = (bool)MultiReportsInContinuousMode.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("VisionSettings")]["AllProgramInSingleReport"] = GlobalSettings.AllProgramInSingleReport = (bool)cbAllProgramsInSingleReport.IsChecked;
                //CurrentMachineDataR[UpdateTableIndex("VisionSettings")]["Align_by_CG"] = (bool)cb_Align_by_CG.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("VisionSettings")]["CNCAssisted_FG"] = (bool)cbCNCAssisted_FG.IsChecked;

                GlobalSettings.MainExcelInstance.IsDoPagination = GlobalSettings.PaginateReports;

                switch (GlobalSettings.CurrentUserLevel)
                {
                    case GlobalSettings.UserPrevilageLevel.Admin:
                    case GlobalSettings.UserPrevilageLevel.AdminUser:
                    case GlobalSettings.UserPrevilageLevel.System:
                        GlobalSettings.XLPassword = ExcelRptPasswordTxtBx.Text;
                        CurrentMachineDataR[UpdateTableIndex("UserSettings")]["XLPassword"] = GlobalSettings.XLPassword;
                        GlobalSettings.MainExcelInstance.Password = GlobalSettings.XLPassword;
                        break;
                }
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["SelectProgramDrive"] = tblock_ProgramLocation.Text;//ProgramDriveCBx.Text;
                GlobalSettings.ProgramPath = tblock_ProgramLocation.Text + "Programs";
                if (tblock_ReportLocation.Text == "")
                {
                    tblock_ReportLocation.Text = tblock_ProgramLocation.Text;
                    CurrentMachineDataR[UpdateTableIndex("UserSettings")]["SelectReportPath"] = tblock_ReportLocation.Text; // +"Reports";//ProgramDriveCBx.Text;                    
                }
                else
                    CurrentMachineDataR[UpdateTableIndex("UserSettings")]["SelectReportPath"] = tblock_ReportLocation.Text;//ProgramDriveCBx.Text;

                if (tblock_ReportLocation.Text == tblock_ProgramLocation.Text)
                    GlobalSettings.ReportPath = tblock_ProgramLocation.Text + "Reports";
                else
                    GlobalSettings.ReportPath = tblock_ReportLocation.Text.Substring(0, tblock_ReportLocation.Text.Length - 1);


                GlobalSettings.ProgramPath = tblock_ProgramLocation.Text + "Programs"; //+ Microsoft.VisualBasic.Strings.Chr(92)
                                                                                       //GlobalSettings.ReportPath = GlobalSettings.ReportPath; // +"Reports";
                GlobalSettings.TemplatePath = tblock_ProgramLocation.Text + "Templates";
                GlobalSettings.ShapePointsPath = tblock_ProgramLocation.Text + "Shape Points";
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


                GlobalSettings.XLExportImg1DuringPP = TransferImg1ToXLDuringPPCBx.SelectedIndex;
                if (GlobalSettings.XLExportImg1DuringPP > 0)
                    GlobalSettings.MainExcelInstance.InsertImage = true;

                switch (TransferImg1ToXLDuringPPCBx.SelectedIndex)
                {
                    case 0:
                        Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["XLExportImage1DuringPP"] = "None";
                        break;
                    case 1:
                        Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["XLExportImage1DuringPP"] = "RCad Image";
                        break;
                    case 2:
                        Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["XLExportImage1DuringPP"] = "Video Image";
                        break;
                    case 3:
                        Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["XLExportImage1DuringPP"] = "Video With Graphics";
                        break;
                }
                GlobalSettings.XLExportImg2DuringPP = TransferImg2ToXLDuringPPCBx.SelectedIndex;

                switch (TransferImg2ToXLDuringPPCBx.SelectedIndex)
                {
                    case 0:
                        Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["XLExportImage2DuringPP"] = "None";
                        break;
                    //case 1:
                    //    Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["XLExportImage2DuringPP"] = "RCad Image";
                    //    break;
                    case 1: //2:
                        Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["XLExportImage2DuringPP"] = "Video Image";
                        break;
                    case 2: //3:
                        Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["XLExportImage2DuringPP"] = "Video With Graphics";
                        break;
                }
                GlobalSettings.XLDataTransferTolMode = 0;
                if ((bool)AllowedRangeRBtn.IsChecked)
                {
                    CurrentMachineDataR[UpdateTableIndex("UserSettings")]["DataTransferToleranceFormat"] = 1; // "AllowedRange";
                    GlobalSettings.XLDataTransferTolMode = 1;
                }
                else if ((bool)NomPlsMnsTolRBtn.IsChecked)
                {
                    CurrentMachineDataR[UpdateTableIndex("UserSettings")]["DataTransferToleranceFormat"] = 2; // "NomPlsMinusTol";
                    GlobalSettings.XLDataTransferTolMode = 2;
                }
                else if ((bool)ShwLSLorUSLRBtn.IsChecked)
                {
                    CurrentMachineDataR[UpdateTableIndex("UserSettings")]["DataTransferToleranceFormat"] = 3; // "ShowLSLorUSL";
                    GlobalSettings.XLDataTransferTolMode = 3;
                }
                try
                {
                    GlobalSettings.MainExcelInstance.ToleranceFormat = GlobalSettings.XLDataTransferTolMode;
                }
                catch (Exception exx)
                {
                    RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SetW87", exx);
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW25", ex); }
        }
        //Update advanced settings
        void TabAdvancedUpdate()
        {
            try
            {
                //=============================================================Text Boxes=============================================================//
                TextBox[] UserSettingsTxtBxs = { RepeatCntTxtBx, FocusActiveWidthTxtBx, RotaryAngDiffTxtBx,
                                                 MaxMinFocusRatioCutoffTxtBx, MaxMinZRatioCutoffTxtBx, SigmaActiveWdthRatioCutoffTxtBx,
                                                 NoiseFilterFactorTxtBx, GrainFactorTxtBx, ProfileScanCutoffPointTBx,
                                                 ProfileScanRectHorizontalBorderBufferTxtBx, ProfileScanRectVerticalBorderBufferTxtBx, NeighbourPtDistToleranceTxtBx, ProfileScanMoveToNextFrameFractionTxtBx,
                                                 PointsBundleForDiaProfile, BestFitPointsBundleForDia,PixelAveragingrTxtBx,NoOfImagesForAvgTxtBx,BestFitResolutionTxtBx, BestFitSampleSizeTxtBx, RotaryInterceptTxtBx};
                String[] TxtBxNames = { "RepeatCount", "DTol", "RotaryAngleDiff",
                                        "MaxMinFocusRatioCutoff", "MaxMinZRatioCutoff", "SigmaActiveWdthRatioCutoff",
                                        "NoiseFilterFactor", "GrainFactor", "ProfileScanCutoffPoint",
                                        "ProfileScanRectHorizontalBorderBuffer", "ProfileScanRectVerticalBorderBuffer", "ProfileScanNeighbourPtDistTolerance", "ProfileScanMoveToNextFrameFraction",
                                        "PointsBundleForDiaProfile", "BestFitPointsBundleForDia","PixelAveragingValue","ImageCountForAveraging","BestFitResolution","BestFitSampleSize", "RotaryInterceptValue"};
                bool[] MulFactors = { false, true, true,
                                      false, false, false,
                                      false, false, false,
                                      false, false, false, false,
                                      false, false,false,false,
                                    false , false, true};

                for (int i = 0; i < UserSettingsTxtBxs.Length; i++)
                    if (MulFactors[i])
                        CurrentMachineDataR[UpdateTableIndex("UserSettings")][TxtBxNames[i]] = Convert.ToDouble(UserSettingsTxtBxs[i].Text) * UnitType_MulFactor;
                    else
                        CurrentMachineDataR[UpdateTableIndex("UserSettings")][TxtBxNames[i]] = UserSettingsTxtBxs[i].Text;

                CurrentMachineDataR[UpdateTableIndex("SurfaceFGSettings")]["T6PixelNeighbour"] = Convert.ToDouble(NeighbourPtDistToleranceTxtBx.Text);
                CurrentMachineDataR[UpdateTableIndex("DXFSettings")]["SurfaceStepLength"] = Convert.ToDouble(SurfaceStepLengthTxtBx.Text) * UnitType_MulFactor;
                CurrentMachineDataR[UpdateTableIndex("DXFSettings")]["SurfacePtClustSize"] = SurfacePtClusterTxtBx.Text;
                CurrentMachineDataR[UpdateTableIndex("DXFSettings")]["SurfacePtGap"] = Convert.ToDouble(PtGapSurfaceBuildTxtBx.Text) * UnitType_MulFactor;
                CurrentMachineDataR[UpdateTableIndex("DXFSettings")]["BestFitSurfaceTolerance"] = SurfaceBfitDistToleranceTxtBx.Text;

                //=============================================================Check Boxes=============================================================//
                CheckBox[] UserSettingsChkBxs = { UseLineArcFitChkBx, CldPtsAsSurfaceChkBx, UseLocalCorrectChkBx, OutlierFilteringChkBx, ShwDeviationForLnsOnlyChkBx, ApplyFocusCorrectionChkBx, AutoAdjustLightingChkBx, EnableOneshotFeaturesChkBx, ArcBestFitTypeForDia, ApplyPixelAveragingChkBx, UseAverageImageChkBx, CloseOpenSurfaceChkBx };
                String[] ChkBxNames = { "UseLineArcFitting", "CloudPointsAsSurface", "UseLocalisedCorrection", "OutlierFilteringForLineOrArc", "ShwDeviationForLnsOnly", "ApplyFocusCorrection", "AutoAdjustLighting", "EnableOneshotFeatures", "ArcBestFitTypeForDia", "ApplyPixelAveraging", "UseAverageImage", "CloseOpenSurface" };

                for (int i = 0; i < UserSettingsChkBxs.Length; i++)
                    CurrentMachineDataR[UpdateTableIndex("UserSettings")][ChkBxNames[i]] = UserSettingsChkBxs[i].IsChecked;
                GlobalSettings.LocalisedCorrectionOn = (bool)UseLocalCorrectChkBx.IsChecked;

                CurrentMachineDataR[UpdateTableIndex("DXFSettings")]["AllPointAsCloudPoints"] = AllPtsAsCldPtsChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("DXFSettings")]["OptimiseSurfaceBuild"] = OptimiseSurfaceBuildChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("DXFSettings")]["UseSurfacebuildUpperCutoff"] = UsePtDistUpperCutOffChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("DXFSettings")]["CloudPtsAroundXYPlane"] = PtsAroundXYPlnChkBx.IsChecked;

                GlobalSettings.InsertImageForDataTransfer = (bool)InsertImgChkBx.IsChecked;
                GlobalSettings.ShowAveragingInExcel = (bool)ShowAveragingChkBx.IsChecked;

                //=============================================================MCSTypeCBx Boxes=============================================================//

                switch (MCSTypeCBx.SelectedIndex)
                {
                    case 0:
                        CurrentMachineDataR[UpdateTableIndex("UserSettings")]["MCSType"] = 0;
                        break;
                    case 1:
                        CurrentMachineDataR[UpdateTableIndex("UserSettings")]["MCSType"] = 1;
                        break;
                    case 2:
                        CurrentMachineDataR[UpdateTableIndex("UserSettings")]["MCSType"] = 2;
                        break;
                    case 3:
                        CurrentMachineDataR[UpdateTableIndex("UserSettings")]["MCSType"] = 3;
                        break;
                    case 4:
                        CurrentMachineDataR[UpdateTableIndex("UserSettings")]["MCSType"] = 4;
                        break;
                }

                //=============================================================Radio Buttons=============================================================//
                if ((bool)UCSBasePlnLnRefXAxisRBtn.IsChecked)
                    CurrentMachineDataR[UpdateTableIndex("UserSettings")]["UCSBasePlnLnRefAxis"] = "XAxis";
                else if ((bool)UCSBasePlnLnRefYAxisRBtn.IsChecked)
                    CurrentMachineDataR[UpdateTableIndex("UserSettings")]["UCSBasePlnLnRefAxis"] = "YAxis";

                //For linear mode check box
                DB GetValues = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                GetValues.FillTable("ToolStripButton");
                System.Data.DataRow TmpDataR = GetValues.Select_ChildControls("ToolStripButton", "ButtonName", "Linear Mode")[0];
                TmpDataR["CheckedProperty"] = DefaultLinearModChkBx.IsChecked;
                GetValues.Update_Table("ToolStripButton");
                GetValues.FinalizeClass();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW26", ex); }
        }
        //Update touch probe settings
        void TabTouchProbeUpdate()
        {
            try
            {
                //All the textBoxes
                TextBox[] UserSettingsTxtBxs = {  MasterProbeDiaTxtBx, ProbeDiaTxtBx, OffsetXTxtBx, OffsetYTxtBx, OffsetZTxtBx,
                                                   TProbeXLeftDiaTxtBx, TProbeXRightDiaTxtBx, TProbeXOffsetXTxtBx, TProbeXOffsetYTxtBx, TProbeXOffsetZTxtBx,
                                                   TProbeYFrontDiaTxtBx, TProbeYBackDiaTxtBx, TProbeYOffsetXTxtBx, TProbeYOffsetYTxtBx, TProbeYOffsetZTxtBx,
                                                   StarProbeLeftDiaTxtBx, StarProbeRightDiaTxtBx, StarProbeFrontDiaTxtBx, StarProbeBackDiaTxtBx,
                                                   StarProbeRightOffsetXTxtBx, StarProbeRightOffsetYTxtBx, StarProbeRightOffsetZTxtBx,
                                                   StarProbeBackOffsetXTxtBx, StarProbeBackOffsetYTxtBx, StarProbeBackOffsetZTxtBx,
                                                   StarProbeFrontOffsetXTxtBx, StarProbeFrontOffsetYTxtBx, StarProbeFrontOffsetZTxtBx,
                                                   ProbeStraightOffsetXTxtBx,ProbeStraightOffsetYTxtBx,ProbeStraightOffsetZTxtBx,
                                                   TProbeYSOffsetXTxtBx,TProbeYSOffsetYTxtBx,TProbeYSOffsetZTxtBx,
                                                   TProbXSOffsetXTxtBx,TProbeXSOffsetYTxtBx,TProbeXSOffsetZTxtBx,TProbeXStraightDiaTxtBx,TProbeYStraightDiaTxtBx,
                                                   StarProbeStraightDiaTxtBx, txtGageOff_X, txtGageOff_Y,txtGageOff_Z};
                String[] TxtBxNames = {  "MasterProbeDia", "ProbeDiameter", "ProbeOffsetX", "ProbeOffsetY", "ProbeOffsetZ",
                                          "TProbeAlongXLeftDia", "TProbeAlongXRightDia", "TProbeAlongXOffsetX", "TProbeAlongXOffsetY", "TProbeAlongXOffsetZ",
                                          "TProbeAlongYFrontDia", "TProbeAlongYBackDia", "TProbeAlongYOffsetX", "TProbeAlongYOffsetY", "TProbeAlongYOffsetZ",
                                          "StarProbeLeftDia", "StarProbeRightDia", "StarProbeFrontDia", "StarProbeBackDia",
                                          "StarProbeRightOffsetX", "StarProbeRightOffsetY", "StarProbeRightOffsetZ",
                                          "StarProbeBackOffsetX", "StarProbeBackOffsetY", "StarProbeBackOffsetZ",
                                          "StarProbeFrontOffsetX", "StarProbeFrontOffsetY", "StarProbeFrontOffsetZ",
                                          "StarProbeStrtOffsetX","StarProbeStrtOffsetY","StarProbeStrtOffsetZ",
                                          "TProbeStrtOffsetX","TProbeStrtOffsetY","TProbeStrtOffsetZ",
                                          "TProbeXStrtOffsetX","TProbeXStrtOffsetY","TProbeXStrtOffsetZ","TProbeAlongXStrtDia","TProbeAlongYStrtDia",
                                          "StarProbeStrtDia", "GageOffsetX", "GageOffsetY", "GageOffsetZ"};


                for (int i = 0; i < UserSettingsTxtBxs.Length; i++)
                    CurrentMachineDataR[UpdateTableIndex("ProbeSettings")][TxtBxNames[i]] = Convert.ToDouble(UserSettingsTxtBxs[i].Text) * UnitType_MulFactor;
                CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")]["ProbeRevertDistance"] = Convert.ToDouble(ProbeMoveBackValTxtBx.Text) * UnitType_MulFactor;

                CurrentMachineDataR[UpdateTableIndex("ProbeSettings")]["ShowTouchProbeInRcad"] = ShowProbeInRcadChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("ProbeSettings")]["UseProbeOneTouchAlignment"] = UseOneTouchAlignmentChkBx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("ProbeSettings")]["ProbeMoveUp"] = chkRetractProbeMethod.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("ProbeSettings")]["UseGotoPointsforPobeHitPartProg"] = UseGotoPointsforPobeHitPartProgChkBx.IsChecked;

                if ((bool)RetractProbePointModeChkBx.IsChecked)
                {
                    CurrentMachineDataR[UpdateTableIndex("ProbeSettings")]["ProbeOffsetValue"] = Convert.ToDouble(ProbeOffsetValTxtBx.Text) * UnitType_MulFactor;
                    CurrentMachineDataR[UpdateTableIndex("ProbeSettings")]["ProbeSpeed"] = Convert.ToDouble(ProbeSpeedTxtBx.Text) * UnitType_MulFactor;
                    CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")]["ProbeRevertSpeed"] = Convert.ToDouble(ProbeRetractSpeedTxtBx.Text) * UnitType_MulFactor;
                    CurrentMachineDataR[UpdateTableIndex("ProbeSettings")]["ProbeSensitivity"] = Convert.ToDouble(ProbeSensitivityTxtBx.Text) * UnitType_MulFactor;
                }
                else
                {
                    CurrentMachineDataR[UpdateTableIndex("ProbeSettings")]["ProbeOffsetValue"] = Convert.ToDouble(ProbeOffsetValTxtBx.Text) * UnitType_MulFactor;
                    CurrentMachineDataR[UpdateTableIndex("ProbeSettings")]["ProbeSpeed"] = Convert.ToDouble(ProbeSpeedTxtBx.Text) * UnitType_MulFactor;
                    CurrentMachineDataR[UpdateTableIndex("ProbeSettings")]["ProbeOffsetValueOnwards"] = Convert.ToDouble(ProbeMoveBackValTxtBx.Text) * UnitType_MulFactor;
                    CurrentMachineDataR[UpdateTableIndex("ProbeSettings")]["ProbeSpeedOnwards"] = Convert.ToDouble(ProbeSpeedTxtBx.Text) * UnitType_MulFactor;
                    CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")]["ProbeRevertSpeedOnwards"] = Convert.ToDouble(ProbeRetractSpeedTxtBx.Text) * UnitType_MulFactor;
                    CurrentMachineDataR[UpdateTableIndex("ProbeSettings")]["ProbeSensitivityOnwards"] = Convert.ToDouble(ProbeSensitivityTxtBx.Text) * UnitType_MulFactor;
                }

                double TmpVal1, TmpVal2, TmpVal3, TmpVal4, TmpVal5;
                TmpVal1 = Convert.ToDouble(ProbeMoveBackValTxtBx.Text) * UnitType_MulFactor;
                TmpVal2 = Convert.ToDouble(ProbeRetractSpeedTxtBx.Text) * UnitType_MulFactor;
                TmpVal3 = Convert.ToDouble(ProbeOffsetValTxtBx.Text) * UnitType_MulFactor;
                TmpVal4 = Convert.ToDouble(ProbeSpeedTxtBx.Text) * UnitType_MulFactor;
                TmpVal5 = Convert.ToDouble(ProbeSensitivityTxtBx.Text) * UnitType_MulFactor;
                bool TmpFlag = Convert.ToBoolean(chkRetractProbeMethod.IsChecked);
                RetractProbePointModeChkBx.IsChecked = chkRetractProbeMethod.IsChecked;

                bool CurrentProbePointMode = Convert.ToBoolean(RetractProbePointModeChkBx.IsChecked);
                //if (ProbeRetractDistance != TmpVal1 || ProbeRetractSpeed != TmpVal2 || ProbeApproachDistance != TmpVal3 || ProbeApproachSpeed != TmpVal4 || ProbeSensitivity != TmpVal5 || TmpFlag != ProbeMoveUpFlag || RetractProbePointMode != CurrentProbePointMode)
                //for (int j = 0; j < 4; j++)
                //{
                int updatedAppRevertValues = 0;
                if (CurrentProbePointMode)
                {
                    RWrapper.RW_DBSettings.MYINSTANCE().UpdateProbeApproachGap_Speed(TmpVal3, TmpVal2);
                    updatedAppRevertValues = RWrapper.RW_DRO.MYINSTANCE().SetProbeRevertDistSpeed(0, TmpVal1, TmpVal2, TmpVal3, TmpVal4, TmpVal5, TmpFlag);
                }
                else
                {
                    RWrapper.RW_DBSettings.MYINSTANCE().UpdateProbeApproachGap_Speed(TmpVal3, TmpVal4);
                    updatedAppRevertValues = RWrapper.RW_DRO.MYINSTANCE().SetProbeRevertDistSpeed(0, TmpVal3, TmpVal2, TmpVal3, TmpVal4, TmpVal5, TmpFlag);
                }
                if (updatedAppRevertValues > 0)
                    MessageBox.Show("Kindly switch off and switch on the machine and restart software for updates to take effect", "Rapid-I 5.0");
                //}
                if (ProbeTypeChbk.Text != "")
                {
                    CurrentMachineDataR[UpdateTableIndex("ProbeSettings")]["CurrentGenericProbeType"] = ProbeTypeChbk.Text;
                }

                switch (ProbeTypeCBx.SelectedIndex)
                {
                    case 0:
                        CurrentMachineDataR[UpdateTableIndex("ProbeSettings")]["ProbeType"] = "NormalProbe";
                        GlobalSettings.CurrentProbeType = GlobalSettings.ProbeType.Normal_Probe;
                        break;
                    case 1:
                        CurrentMachineDataR[UpdateTableIndex("ProbeSettings")]["ProbeType"] = "TProbeAlongX";
                        GlobalSettings.CurrentProbeType = GlobalSettings.ProbeType.TProbe_AlongX;
                        break;
                    case 2:
                        CurrentMachineDataR[UpdateTableIndex("ProbeSettings")]["ProbeType"] = "TProbeAlongY";
                        GlobalSettings.CurrentProbeType = GlobalSettings.ProbeType.TProbe_AlongY;
                        break;
                    case 3:
                        CurrentMachineDataR[UpdateTableIndex("ProbeSettings")]["ProbeType"] = "StarProbe";
                        GlobalSettings.CurrentProbeType = GlobalSettings.ProbeType.Star_Probe;
                        break;
                    case 4:
                        CurrentMachineDataR[UpdateTableIndex("ProbeSettings")]["ProbeType"] = "Generic Probe";
                        GlobalSettings.CurrentProbeType = GlobalSettings.ProbeType.Generic_Probe;
                        break;
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW27", ex); }
        }
        //Update new users
        void TabUserSettingsUpdate()
        {
            try
            {                                                                                    //7
                Settings_DataB.Update_Table("Login");
            }
            catch (Exception Ex) { MessageBox.Show(Ex.Message); }                                    //8

        }
        //Update machine details
        void TabMachineDetailsUpdate()
        {
            try
            {
                //All the textBoxes
                TextBox[] MachineDetailsTxtBxs = { CompanyNameTxtBx, LocationTxtBx, VersionNoTxtBx, DROStyleTxtBx, FrameRateTxtBx, AutoZoomDelayTxtBx, CommandSendDelayTxtBx,
                                                     WaitTimeAfterTargetReachedTxtBx, OneShotImgCompBoundryWidthTxtBx, OneShotImgBinarisingValTxtBx, WriteToHardDiskPtsLimitTxtBx,
                                                 FocusDelayTxtBx, txtBxDROJumpTolerance};// ,PixelWidthCorrectionParameterQTxtBx , PixelWidthCorrectionParameterLX1TxtBx,PixelWidthCorrectionParameterLYTxtBx,X0TxtBx,Y0TxtBx};
                String[] ControlNames = { "CompanyName", "Location", "StartVersionNo", "DROStyle", "FrameRate", "AccessoryLenses", "CommandSendDelay",
                                            "DelayAfterTargetReached", "OneShotImageBoundryWidth", "OneShotImageBinariseValue", "WriteToHardDiskPtsLimit","DelayInFocus", "MesureNoOfDec"};// , "PixelWidthCorrectionParameterQ" , "PixelWidthCorrectionParameterLX1", "PixelWidthCorrectionParameterLY",
                                                                                                                                                                                           //"PixelWidthX0" , "PixelWidthY0"};

                for (int i = 0; i < MachineDetailsTxtBxs.Length; i++)
                    CurrentMachineDataR[UpdateTableIndex("MachineDetails")][ControlNames[i]] = MachineDetailsTxtBxs[i].Text;

                //All Check Boxes
                CheckBox[] MachineDetailsChkBxs = { AuxillarLightChkBx, CoAxialLightChkBx, ProbePresentChkBx, DefaultFlexCHairChkBx, AutoZoomInOutChkBx, AllowAxisLockChkBx, DigiZoomOneShotChkBx,
                                               LockUIChkBx, Point5XLensChkBx, ThreeDSoftwareChkBx, HideGraphicsOnVidChkBx, HardnessModulePresentChkBx, MultiLevelMachineChkBx,
                                               EnableDoubleImageOption, EnablePartialImageAllignInOneShotChkBx, AllowFocusRawValuesChkBx, AllowStepFileImportChkBx, UseLookAheadChkBx, DemoChkBx,PixelWidthCorrectionChkBx, cbRotaryMeasurementPresent};
                String[] ChkBxNames = { "AuxiliaryLighting", "CoAxialLighting", "ProbePresent", "DefaultFlexibleCrosshair", "AutoZoomInOut", "AllowAxisLock", "UseDigitalZoominOneShot",
                                  "LockUIatStartup", "FiveXLensePresent", "ThreedSoftware", "HideGraphicsOnVideo", "HardnessModule", "MultiLevelZoomMachine",
                                  "DoubleImageEnabled", "OneShotPartialImageEnabled", "AllowFocusRawValues", "AllowStepFileImport", "UseLookAhead","DemoMode" , "PixelWidthCorrection", "RotaryAxisForMeasurement"};

                for (int i = 0; i < MachineDetailsChkBxs.Length; i++)
                    CurrentMachineDataR[UpdateTableIndex("MachineDetails")][ChkBxNames[i]] = MachineDetailsChkBxs[i].IsChecked;

                GlobalSettings.DoubleImageEnabled_During_DigiZoom = (bool)EnableDoubleImageOption.IsChecked;
                GlobalSettings.MainWin.AllowAxisLock = (bool)AllowAxisLockChkBx.IsChecked;
                GlobalSettings.AllowFocusRawValues = (bool)AllowFocusRawValuesChkBx.IsChecked;
                GlobalSettings.MultilevelZoomMahine = (bool)MultiLevelMachineChkBx.IsChecked;
                //RWrapper.RW_MainInterface.MYINSTANCE().Do_Homing_At_Startup = (bool)HomeMachineAtStartup.IsChecked;

                //GlobalSettings.PixelWidthCorrectionParameterQ = Convert.ToDouble(PixelWidthCorrectionParameterQTxtBx.Text);
                //GlobalSettings.PixelWidthCorrectionParameterLX1 = Convert.ToDouble(PixelWidthCorrectionParameterLX1TxtBx.Text);
                //GlobalSettings.PixelWidthCorrectionParameterLY = Convert.ToDouble(PixelWidthCorrectionParameterLYTxtBx.Text);
                //GlobalSettings.PixelWidthX0 = Convert.ToDouble(X0TxtBx.Text);
                //GlobalSettings.PixelWidthY0 = Convert.ToDouble(Y0TxtBx.Text);
                //RWrapper.RW_MainInterface.MYINSTANCE().SetPixelWidthCorrectionStatusForOneShot(GlobalSettings.PixelWidthCorrectionEnable);
                //RWrapper.RW_MainInterface.MYINSTANCE().SetPixelWidthCorrectionConstants(GlobalSettings.PixelWidthCorrectionParameterQ, GlobalSettings.PixelWidthCorrectionParameterLX1, GlobalSettings.PixelWidthCorrectionParameterLY, GlobalSettings.PixelWidthX0, GlobalSettings.PixelWidthY0);

                CurrentMachineDataR[UpdateTableIndex("MachineDetails")]["MachineType"] = MachineTypeCBx.Text;
                CurrentMachineDataR[UpdateTableIndex("MachineDetails")]["NoOfAxes"] = NoOfAxisCBx.Text;
                CurrentMachineDataR[UpdateTableIndex("MachineDetails")]["CameraType"] = CamTypeCBx.Text;
                CurrentMachineDataR[UpdateTableIndex("MachineDetails")]["OneShotImageTolerance"] = OneShotImgCompTolUpDown.Value.ToString();
                CurrentMachineDataR[UpdateTableIndex("MachineDetails")]["OneShotImageMatchTargetTolerance"] = OneShotImgCompTargetTolUpDown.Value.ToString();

                CurrentMachineDataR[UpdateTableIndex("VisionSettings")]["AutoHoming_at_Start"] = HomeMachineAtStartup.IsChecked;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW29", ex); }
        }
        //Update Video And Camera settings
        void TabVideoAndCameraSettingsUpdate()
        {
            try
            {
                CurrentMachineDataR[UpdateTableIndex("LightingSettings")]["LightValue"] = LightValueTxtBx.Text;
                CurrentMachineDataR[UpdateTableIndex("LightingSettings")]["Profile_Zone_No"] = txtProfileZone.Text;
                CurrentMachineDataR[UpdateTableIndex("LightingSettings")]["Aux_Zone_No"] = txtAuxiliary.Text;
                CurrentMachineDataR[UpdateTableIndex("LightingSettings")]["Profile11X_Zone_No"] = txt11XProfile.Text;
                CurrentMachineDataR[UpdateTableIndex("LightingSettings")]["CoAx_ZoneNo"] = txtCoAxial.Text;
                CurrentMachineDataR[UpdateTableIndex("LightingSettings")]["ProfileLightLevelFor11X"] = txtProfileLightAt11X.Text;

                try
                {
                    CurrentMachineDataR[UpdateTableIndex("LightingSettings")]["Zone1_No"] = txtSurfaceZ1.Text;
                    CurrentMachineDataR[UpdateTableIndex("LightingSettings")]["Zone2_No"] = txtSurfaceZ2.Text;
                    CurrentMachineDataR[UpdateTableIndex("LightingSettings")]["Zone3_No"] = txtSurfaceZ3.Text;
                    CurrentMachineDataR[UpdateTableIndex("LightingSettings")]["Zone4_No"] = txtSurfaceZ4.Text;
                }
                catch (Exception exxx)
                { }

                CurrentMachineDataR[UpdateTableIndex("VisionSettings")]["CameraSkew"] = AxisAngleTxtBx.Text;
                CurrentMachineDataR[UpdateTableIndex("VisionSettings")]["CameraSkewXZ"] = AxisAngleXZTxtBx.Text;
                CurrentMachineDataR[UpdateTableIndex("VisionSettings")]["CameraSkewYZ"] = AxisAngleYZTxtBx.Text;
                CurrentMachineDataR[UpdateTableIndex("VisionSettings")]["CamDistortionFactor"] = CamDistortionFactorTxtBx.Text;
                CurrentMachineDataR[UpdateTableIndex("VisionSettings")]["CamDistortionOffsetX"] = CamDistortionOffsetXTxtBx.Text;
                CurrentMachineDataR[UpdateTableIndex("VisionSettings")]["CamDistortionOffsetY"] = CamDistortionOffsetYTxtBx.Text;

                CurrentMachineDataR[UpdateTableIndex("VisionSettings")]["CamLenseType"] = CamLenseTypeCBx.Text;

                //All the textBoxes
                TextBox[] VideoSettingsTxtBxs = { XOffsetVideoTxtBx, YOffsetVideoTxtBx,
                                               XPosStartRight, XPosStartLeft, YPosStartTop, YPosStartBottom,
                                               XMultiplierRight, XMultiplierLeft, YMultiplierTop, YMultiplierBottom };
                String[] TxtBxNames = { "Xoffset", "Yoffset",
                                  "XPositionStartRight", "XPositionStartLeft", "YPositionStartTop", "YPositionStartBottom",
                                  "XMultiplyFactorRight", "XMultiplyFactorLeft", "YMultiplyFactorTop", "YMultiplyFactorBottom"};

                for (int i = 0; i < VideoSettingsTxtBxs.Length; i++)
                    CurrentMachineDataR[UpdateTableIndex("VisionSettings")][TxtBxNames[i]] = VideoSettingsTxtBxs[i].Text;

                TextBox[] PixWidthTxtBxs = { MagLevel3XTxtBx, MagLevel5XTxtBx, MagLevel11XTxtBx, MagLevel22XTxtBx, MagLevel34XTxtBx,
                                         MagLevel54XTxtBx, MagLevel67XTxtBx, MagLevel134XTxtBx, MagLevel335XTxtBx};
                TextBox[] PixWidthYDirTxtBxs = { MagLevel3XTxtBx_YDir, MagLevel5XTxtBx_YDir, MagLevel11XTxtBx_YDir, MagLevel22XTxtBx_YDir, MagLevel34XTxtBx_YDir,
                                         MagLevel54XTxtBx_YDir, MagLevel67XTxtBx_YDir, MagLevel134XTxtBx_YDir, MagLevel335XTxtBx_YDir};
                CheckBox[] MagHiddenChkBxs = { MagLevel3XHiddenChkBx, MagLevel5XHiddenChkBx, MagLevel11XHiddenChkBx, MagLevel22XHiddenChkBx, MagLevel34XHiddenChkBx,
                                         MagLevel54XHiddenChkBx, MagLevel67XHiddenChkBx, MagLevel134XHiddenChkBx, MagLevel335XHiddenChkBx};
                CheckBox[] MagProLight11XChkBxs = { MagLevel3XProLight11XChkBx, MagLevel5XProLight11XChkBx, MagLevel11XProLight11XChkBx, MagLevel22XProLight11XChkBx, MagLevel34XProLight11XChkBx,
                                         MagLevel54XProLight11XChkBx, MagLevel67XProLight11XChkBx, MagLevel134XProLight11XChkBx, MagLevel335XProLight11XChkBx};
                TextBox[] DigitalZoomX ={ DigitalZoom3XTxtBx_XDir, DigitalZoomLevel5XTxtBx_XDir,  DigitalZoomLevel11XTxtBx_XDir,  DigitalZoomLevel22XTxtBx_XDir,  DigitalZoomLevel34XTxtBx_XDir,
                                          DigitalZoomLevel54XTxtBx_XDir,  DigitalZoomLevel67XTxtBx_XDir,  DigitalZoomLevel134XTxtBx_XDir,  DigitalZoomLevel335XTxtBx_XDir};
                TextBox[] DigitalZoomY ={ DigitalZoom3XTxtBx_YDir, DigitalZoomLevel5XTxtBx_YDir,  DigitalZoomLevel11XTxtBx_YDir,  DigitalZoomLevel22XTxtBx_YDir,  DigitalZoomLevel34XTxtBx_YDir,
                                          DigitalZoomLevel54XTxtBx_YDir,  DigitalZoomLevel67XTxtBx_YDir,  DigitalZoomLevel134XTxtBx_YDir,  DigitalZoomLevel335XTxtBx_YDir};

                if (CurrentMachineDataR[UpdateTableIndex("MachineDetails")]["MachineType"].ToString() == "One Shot")
                {
                    for (int i = 0; i < MagRows.Length; i++)
                    {
                        MagRows[i]["PixelWidth"] = PixWidthTxtBxs[i].Text;
                        MagRows[i]["Hidden"] = MagHiddenChkBxs[i].IsChecked;
                        MagRows[i]["ElvnXProfileLight"] = MagProLight11XChkBxs[i].IsChecked;
                        MagRows[i]["PixelWidthY"] = PixWidthYDirTxtBxs[i].Text;
                        MagRows[i]["DigitalZoomPixel_X"] = DigitalZoomX[i].Text;
                        MagRows[i]["DigitalZoomPixel_Y"] = DigitalZoomY[i].Text;
                    }
                }
                else
                {
                    for (int i = 0; i < MagRows.Length; i++)
                    {
                        MagRows[i]["PixelWidth"] = PixWidthTxtBxs[i].Text;
                        MagRows[i]["Hidden"] = MagHiddenChkBxs[i].IsChecked;
                        MagRows[i]["ElvnXProfileLight"] = MagProLight11XChkBxs[i].IsChecked;
                        MagRows[i]["PixelWidthY"] = PixWidthTxtBxs[i].Text;
                        MagRows[i]["DigitalZoomPixel_X"] = DigitalZoomX[i].Text;
                        MagRows[i]["DigitalZoomPixel_Y"] = DigitalZoomY[i].Text;
                    }
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW30", ex); }
        }
        //Update DRO settings
        void TabDROSettingsUpdate()
        {
            try
            {
                TextBox[] CPUTxtBxs = { XAxisCperUTxtBx, YAxisCperUTxtBx, ZAxisCperUTxtBx, RAxisCperUTxtBx };
                TextBox[] AxisMulTxtBxs = { XAxisMultiplierTxtBx, YAxisMultiplierTxtBx, ZAxisMultiplierTxtBx, RAxisMultiplierTxtBx };
                TextBox[] LashValTxtBxs = { XAxisLashTxtBx, YAxisLashTxtBx, ZAxisLashTxtBx, RAxisLashTxtBx };
                CheckBox[] AxisTypeChkBxs = { XAxisLinearChkBx, YAxisLinearChkBx, ZAxisLinearChkBx, RAxisLinearChkBx };

                for (int i = 0; i < DROROWS.Length; i++)
                {
                    DROROWS[i]["AxisCount"] = CPUTxtBxs[i].Text;
                    DROROWS[i]["AxisMultiplier"] = AxisMulTxtBxs[i].Text;
                    DROROWS[i]["AxisLash"] = LashValTxtBxs[i].Text;
                    DROROWS[i]["AxisType"] = AxisTypeChkBxs[i].IsChecked;
                }

                CurrentMachineDataR[UpdateTableIndex("MachineDetails")]["LimitHitCount"] = LimitHitCountTxtBx.Text;
                CurrentMachineDataR[UpdateTableIndex("MachineDetails")]["MachineDisconnectCount"] = McDisconnectCountTxtBx.Text;

                DROROWS[0]["AxisHomeToPositive"] = HomeToPositiveXChkBx.IsChecked;
                DROROWS[1]["AxisHomeToPositive"] = HomeToPositiveYChkBx.IsChecked;
                if ((bool)HomePositiveXValCBx.IsEnabled)
                {
                    if (HomePositiveXValCBx.SelectedIndex == 1)
                        DROROWS[0]["AxisPositiveHomeValue"] = 200;
                    else if (HomePositiveXValCBx.SelectedIndex == 2)
                        DROROWS[0]["AxisPositiveHomeValue"] = 400;
                    else
                        DROROWS[0]["AxisPositiveHomeValue"] = 150;
                }
                if ((bool)HomePositiveYValCBx.IsEnabled)
                {
                    if (HomePositiveYValCBx.SelectedIndex == 1)
                        DROROWS[1]["AxisPositiveHomeValue"] = 200;
                    else
                        DROROWS[1]["AxisPositiveHomeValue"] = 150;
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW31", ex); }
        }
        //Update EEPROM settings
        public void TabEEPROMSettingsUpdate()
        {
            if (!Directory.Exists(Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Log\\UserLog"))
                Directory.CreateDirectory(Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Log\\UserLog");

            string Logfile = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Log\\UserLog\\EPS.Log";
            System.IO.StreamWriter logger = new StreamWriter(Logfile);
            logger.WriteLine(DateTime.Now.ToShortDateString() + ", " + DateTime.Now.ToLongTimeString());

            EEpromUpdateResult.Text = "";
            try
            {
                //=============================================================Text Boxes=============================================================//
                TextBox[] AccuracyTxtBxs = { X_AccuracyTxtBx, Y_AccuracyTxtBx, Z_AccuracyTxtBx, R_AccuracyTxtBx };
                TextBox[] PitchTxtBxs = { X_PitchTxtBx, Y_PitchTxtBx, Z_PitchTxtBx, R_PitchTxtBx };
                TextBox[] AccelerationTxtBxs = { X_AccelerationTxtBx, Y_AccelerationTxtBx, Z_AccelerationTxtBx, R_AccelerationTxtBx };
                TextBox[] DecelerationTxtBxs = { X_DecelerationTxtBx, Y_DecelerationTxtBx, Z_DecelerationTxtBx, R_DecelerationTxtBx };
                TextBox[] JoyStickSpeedFactorTxtBxs = { X_JoyStkSpeedFactorTxtBx, Y_JoyStkSpeedFactorTxtBx, Z_JoyStkSpeedFactorTxtBx, R_JoyStkSpeedFactorTxtBx };
                TextBox[] Resolution_TxtBxs = { X_Resolution, Y_Resolution, Z_Resolution, R_Resolution };

                String[] AccuracyColNames = { "Accuracy_X", "Accuracy_Y", "Accuracy_Z", "Accuracy_R" };
                String[] PitchColNames = { "Pitch_X", "Pitch_Y", "Pitch_Z", "Pitch_R" };
                String[] AccelerationColNames = { "Acceleration_X", "Acceleration_Y", "Acceleration_Z", "Acceleration_R" };
                String[] DecelerationColNames = { "Deceleration_X", "Deceleration_Y", "Deceleration_Z", "Deceleration_R" };
                String[] JoyStickSpeedFactorColNames = { "JoyStickSpeedFactor_X", "JoyStickSpeedFactor_Y", "JoyStickSpeedFactor_Z", "JoyStickSpeedFactor_R" };
                String[] ResolutionColNames = { "Resolution_X", "Resolution_Y", "Resolution_Z", "Resolution_R" };

                int TmpVal;

                for (int i = 0; i < 4; i++)
                {
                    TmpVal = Convert.ToInt32(AccuracyTxtBxs[i].Text);
                    //if (TmpVal != AxisNumberProperties[0 + i])
                    //{
                    CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")][AccuracyColNames[i]] = TmpVal;
                    RWrapper.RW_DRO.MYINSTANCE().SetAccuracy(TmpVal, i, false);
                    //}
                    logger.WriteLine("Set Accuracy for Axis " + i + " = " + TmpVal);
                    System.Threading.Thread.Sleep(20);
                    //for (int kkjkj = 0; kkjkj < 10000000; kkjkj++)
                    //{
                    //}
                    TmpVal = Convert.ToInt32(PitchTxtBxs[i].Text);
                    //if (TmpVal != AxisNumberProperties[4 + i])
                    //{
                    CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")][PitchColNames[i]] = TmpVal;
                    RWrapper.RW_DRO.MYINSTANCE().SetPitch(TmpVal, i, false);
                    //}
                    logger.WriteLine("Set Pitch for Axis " + i + " = " + TmpVal);
                    System.Threading.Thread.Sleep(20);

                    TmpVal = Convert.ToInt32(AccelerationTxtBxs[i].Text);
                    //if (TmpVal != AxisNumberProperties[8 + i])
                    //{
                    CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")][AccelerationColNames[i]] = TmpVal;
                    RWrapper.RW_DRO.MYINSTANCE().SetAcceleration(TmpVal, i, false);
                    //}
                    logger.WriteLine("Set Acceleration for Axis " + i + " = " + TmpVal);
                    System.Threading.Thread.Sleep(20);

                    TmpVal = Convert.ToInt32(DecelerationTxtBxs[i].Text);
                    //if (TmpVal != AxisNumberProperties[12 + i])
                    //{
                    CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")][DecelerationColNames[i]] = TmpVal;
                    RWrapper.RW_DRO.MYINSTANCE().SetDeceleration(TmpVal, i, false);
                    //}
                    logger.WriteLine("Set Deceleration for Axis " + i + " = " + TmpVal);
                    System.Threading.Thread.Sleep(20);

                    TmpVal = Convert.ToInt32(JoyStickSpeedFactorTxtBxs[i].Text);
                    //if (TmpVal != AxisNumberProperties[16 + i])
                    //{
                    CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")][JoyStickSpeedFactorColNames[i]] = TmpVal;
                    RWrapper.RW_DRO.MYINSTANCE().SetJoysticSpeedFactor(TmpVal, i, false);
                    //}
                    logger.WriteLine("Set Joystick Speed for Axis " + i + " = " + TmpVal);
                    System.Threading.Thread.Sleep(20);

                    TmpVal = Convert.ToInt32(Resolution_TxtBxs[i].Text);
                    //if (TmpVal != AxisNumberProperties[20 + i])
                    //{
                    CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")][ResolutionColNames[i]] = TmpVal;
                    RWrapper.RW_DRO.MYINSTANCE().SetResolution(TmpVal, i, false);
                    //}
                    logger.WriteLine("Set Resolution for Axis " + i + " = " + TmpVal);
                    System.Threading.Thread.Sleep(20);
                }

                //=============================================================Check Boxes and Radio Btns=============================================================//
                CheckBox[] EEPROMSettingsChkBxs = { X_ProbeActivatedChkBx, Y_ProbeActivatedChkBx, Z_ProbeActivatedChkBx, R_ProbeActivatedChkBx };
                RadioButton[] EEPROMSettingsScalePositiveRBtns = { X_ScaleDir_PositiveRBtn, Y_ScaleDir_PositiveRBtn, Z_ScaleDir_PositiveRBtn, R_ScaleDir_PositiveRBtn };
                RadioButton[] EEPROMSettingsMotorPositiveRBtns = { X_MotorDir_PositiveRBtn, Y_MotorDir_PositiveRBtn, Z_MotorDir_PositiveRBtn, R_MotorDir_PositiveRBtn };

                string[] EEPROMSettingsProbeActivateColName = { "ProbeActivation_X", "ProbeActivation_Y", "ProbeActivation_Z", "ProbeActivation_R" };
                string[] EEPROMSettingsScaleDirColName = { "ScalePositiveDirection_X", "ScalePositiveDirection_Y", "ScalePositiveDirection_Z", "ScalePositiveDirection_R" };
                string[] EEPROMSettingsMotorDirColName = { "MotorPositiveDirection_X", "MotorPositiveDirection_Y", "MotorPositiveDirection_Z", "MotorPositiveDirection_R" };
                bool TmpBooleanVal, TmpBooleanVal1, TmpBooleanVal2;

                TmpBooleanVal = false;
                for (int i = 0; i < 4; i++)
                {
                    //if ((bool)EEPROMSettingsChkBxs[i].IsChecked != AxisBooleanProperties[2 * i])
                    //{
                    TmpBooleanVal = true;
                    CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")][EEPROMSettingsProbeActivateColName[i]] = (bool)EEPROMSettingsChkBxs[i].IsChecked;
                    //}
                }

                //if (TmpBooleanVal)
                //    RWrapper.RW_DRO.MYINSTANCE().Activate_DeactivateProbe((bool)X_ProbeActivatedChkBx.IsChecked, (bool)Y_ProbeActivatedChkBx.IsChecked, (bool)Z_ProbeActivatedChkBx.IsChecked, (bool)R_ProbeActivatedChkBx.IsChecked);

                for (int i = 0; i < 4; i++)
                {
                    TmpBooleanVal = (bool)EEPROMSettingsScalePositiveRBtns[i].IsChecked;
                    //if (TmpBooleanVal != AxisBooleanProperties[2 * i + 1])
                    //{
                    CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")][EEPROMSettingsScaleDirColName[i]] = TmpBooleanVal;
                    //if (RWrapper.RW_MainInterface.MYINSTANCE().MachineCardFlag < 2)
                    RWrapper.RW_DRO.MYINSTANCE().SetScaleDirection(TmpBooleanVal, i, false);
                    //}
                    logger.WriteLine("Set Scale Direction for Axis " + i + " = " + TmpBooleanVal.ToString());
                    System.Threading.Thread.Sleep(20);
                }

                for (int i = 0; i < 4; i++)
                {
                    TmpBooleanVal = (bool)EEPROMSettingsMotorPositiveRBtns[i].IsChecked;
                    //if (TmpBooleanVal != AxisBooleanProperties[2 * i + 1])
                    //{
                    CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")][EEPROMSettingsMotorDirColName[i]] = TmpBooleanVal;
                    RWrapper.RW_DRO.MYINSTANCE().SetMotorDirection(TmpBooleanVal, i, false);
                    //}
                    logger.WriteLine("Set Motor Direction for Axis " + i + " = " + TmpBooleanVal.ToString());
                    System.Threading.Thread.Sleep(20);
                }


                CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")]["SetProbeRetractMode"] = (bool)SetProbeRetractMode.IsChecked;
                RWrapper.RW_DRO.MYINSTANCE().SetProbeReractModeForManual((bool)SetProbeRetractMode.IsChecked);

                TmpBooleanVal = (bool)EnableCNCChkBx.IsChecked;
                TmpBooleanVal1 = (bool)EnableAutoZoomChkBx.IsChecked;
                TmpBooleanVal2 = (bool)RouteProbePathChkBx.IsChecked;
                RetractProbePointMode = (bool)RetractProbePointModeChkBx.IsChecked;

                CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")]["AllowCNC"] = TmpBooleanVal;
                CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")]["AllowAutoZoom"] = TmpBooleanVal1;
                CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")]["RouteProbePath"] = TmpBooleanVal2;
                CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")]["RetractProbePointMode"] = RetractProbePointMode;

                int AxisEnableVal = 0;
                if ((bool)X_AxisEnabledChkBx.IsChecked) AxisEnableVal += 1;
                if ((bool)Y_AxisEnabledChkBx.IsChecked) AxisEnableVal += 2;
                if ((bool)Z_AxisEnabledChkBx.IsChecked) AxisEnableVal += 4;
                if ((bool)R_AxisEnabledChkBx.IsChecked) AxisEnableVal += 8;
                CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")]["AxisEnable_Value"] = AxisEnableVal;
                CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")]["DRO_24_Bit"] = (bool)Chk_DRO_24Bit.IsChecked;
                
                RWrapper.RW_DRO.MYINSTANCE().Activate_Deactivate_Zoom_Cnc(TmpBooleanVal, TmpBooleanVal1, TmpBooleanVal2, RetractProbePointMode, false, AxisEnableVal, (bool)Chk_DRO_24Bit.IsChecked);
                logger.WriteLine("Set Zoom, CNC, 24BitDRO, AxisEnable");
                System.Threading.Thread.Sleep(20);

                RWrapper.RW_DRO.MYINSTANCE().Activate_DeactivateProbe((bool)X_ProbeActivatedChkBx.IsChecked, (bool)Y_ProbeActivatedChkBx.IsChecked, (bool)Z_ProbeActivatedChkBx.IsChecked, (bool)R_ProbeActivatedChkBx.IsChecked, false);
                logger.WriteLine("Probe status is set to " + ((bool)Y_ProbeActivatedChkBx.IsChecked).ToString());
                System.Threading.Thread.Sleep(20);

                TmpBooleanVal = (bool)AllowInterpolationChkBx.IsChecked;
                TmpBooleanVal1 = (bool)SmoothCircularInterpolationRBtn.IsChecked;
                //if (TmpBooleanVal != AllowInterpolation || TmpBooleanVal1 != SmoothCircularInterpolation)
                //{
                CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")]["AllowInterpolation"] = TmpBooleanVal;
                CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")]["SmoothCircularInterpolation"] = TmpBooleanVal1;
                RWrapper.RW_DRO.MYINSTANCE().SetInterPolationParameter(TmpBooleanVal, TmpBooleanVal1);
                logger.WriteLine("Interpolation Enable is " + TmpBooleanVal + ". Smooth Interpolation Enable is " + TmpBooleanVal1);
                System.Threading.Thread.Sleep(20);

                //}
                //Clean up existing entries in the dB....
                if (RWrapper.RW_MainInterface.MYINSTANCE().MachineCardFlag == 2)
                {
                    string[] AxisNames = new string[] { "X", "Y", "Z", "R" };
                    DataRow[] TempProbeSettings = Settings_DataB.Select_ChildControls("ProbeTouchSettings", "MachineNo", MachineNo);
                    Settings_DataB.DataA.AcceptChangesDuringUpdate = true;
                    //if (TempProbeSettings.Length == 4)
                    //{
                    for (int j = 0; j < TempProbeSettings.Length; j++)
                        //Settings_DataB.GetTable("ProbeTouchSettings").Rows.Remove(TempProbeSettings[j]);
                        TempProbeSettings[j].Delete();
                    Settings_DataB.Update_Table("ProbeTouchSettings");
                    //}
                    for (int ii = 0; ii < TouchProbeApprRetractParams.Rows.Count; ii++)
                    {
                        DataRow dr = null;
                        //for (int j = 0; j < TempProbeSettings.Length; j++)
                        //{
                        //    if (TempProbeSettings[j]["Axis_Name"] == TouchProbeApprRetractParams.Rows[ii])
                        //    {
                        //        dr = TempProbeSettings[j];
                        //        break;
                        //    }
                        //}
                        //bool NewEntry = false;
                        //if (dr == null)
                        //{
                        //    NewEntry = true;
                        dr = Settings_DataB.GetTable("ProbeTouchSettings").NewRow();
                        dr["MachineNo"] = MachineNo;
                        //}
                        dr["Axis_Name"] = TouchProbeApprRetractParams.Rows[ii]["AxisName"];
                        dr["Approach_Dist"] = TouchProbeApprRetractParams.Rows[ii]["ApproachDist"];
                        dr["Retract_Dist"] = TouchProbeApprRetractParams.Rows[ii]["RetractDist"];
                        dr["Approach_Speed"] = TouchProbeApprRetractParams.Rows[ii]["ApproachSpeed"];
                        dr["Retract_Speed"] = TouchProbeApprRetractParams.Rows[ii]["RetractSpeed"];
                        //if (NewEntry)
                        Settings_DataB.GetTable("ProbeTouchSettings").Rows.Add(dr);
                        RWrapper.RW_DRO.MYINSTANCE().SetProbeRevertDistSpeed(Array.IndexOf(AxisNames, dr["Axis_Name"]), (double)dr["Retract_Dist"], (double)dr["Retract_Speed"], (double)dr["Approach_Dist"], (double)dr["Approach_Speed"], 10, false);
                        logger.WriteLine("Touch Probe Approach/Retract Params set for Axis " + ii);
                        System.Threading.Thread.Sleep(20);
                    }
                    Settings_DataB.Update_Table("ProbeTouchSettings");
                    //Now Update the Speed Control Factors
                    int[] SpeedControlValues = new int[] { Convert.ToInt32(txt_Accel_Manual.Text), Convert.ToInt32(txt_Accel_CNC.Text), Convert.ToInt32(txt_JS_JogSpeed.Text) };
                    CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")]["Accel_Manual"] = SpeedControlValues[0];
                    CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")]["Accel_CNC"] = SpeedControlValues[1];
                    CurrentMachineDataR[UpdateTableIndex("EEPROMSettings")]["JS_JogSpeed"] = SpeedControlValues[2];
                    RWrapper.RW_DRO.MYINSTANCE().SetSpeedControls(SpeedControlValues);
                }
            }
            catch (Exception ex)
            {
                EEpromUpdateResult.Text = "Error in Updation";
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW31a", ex);
            }
            logger.Close();
        }
        //Update EEPROM settings
        void TabSurfaceFGSettingsUpdate()
        {
            try
            {
                //=============================================================Text Boxes=============================================================//
                TextBox[] SurfaceFGSettingsTxtBxs = { Type1_LookAheadValTxtBx, Type1_BlackTolTxtBx, Type1_FilterMaskTolTxtBx,Type1_WhiteTolTxtBx,Type1_FilterMaskTolB2WTxtBx,
                                                      Type2_LookAheadValTxtBx, Type2_CurrAbsThresholdTxtBx, Type2_CurrDiffThresholdTxtBx, Type2_LookaheadAbsThresholdTxtBx, Type2_LookaheadDiffThresholdTxtBx, Type2_MedianFilterBxSizeTxtBx, Type2_AvgFilterBxSizeTxtBx,
                                                      Type3_LookAheadValTxtBx, Type3_BlackTolTxtBx, Type3_FilterMaskTolTxtBx,Type3_WhiteTolTxtBx,Type3_FilterMaskTolB2WTxtBx,
                                                      Type4_LookAheadValTxtBx, Type4_TextureDiffThresholdTxtBx, Type4_AcrossEdgeIncrTxtBx, Type4_AlongEdgeIncrTxtBx, Type4_AcrossEdgeBxThicknessTxtBx, Type4_AlongEdgeBxThicknessTxtBx,
                                                      Type5_LocalMaximaExtentTxtBx, Type5_DiffThresholdTxtBx, Type5_FilterMaskSizeTxtBx ,
                                                      T6MAskFactor , T6BinaryFactorLower , T6BinaryFactorUpper , T6MaskingLevel , T6PixelThreshold ,  T6PixelScanRange,T6CutoffPixels,T6JumpThreshold};
                String[] ColNames = { "T1LookAheadValue", "T1BlackTolerance", "T1FilterMaskTolerance","T1WhiteTolerance","T1FilterMaskToleranceB2W",
                                      "T2LookAheadValue", "T2CurrAbsThreshold", "T2CurrDiffThreshold", "T2LookaheadAbsThreshold", "T2LookaheahDiffThreshold", "T2MedianFilterBoxSize", "T2AverageFilterBoxSize",
                                      "T3LookAheadValue", "T3BlackTolerance", "T3FilterMaskTolerance","T3WhiteTolerance","T3FilterMaskToleranceB2W",
                                      "T4LookAheadValue", "T4TextureDiffThreshold", "T4AcrossEdgeIncrement", "T4AlongEdgeIncrement", "T4AcrossEdgeBoxThickness", "T4AlongEdgeBoxThickness",
                                      "T5LocalMaximaExtent", "T5DiffThreshold", "T5FilterMaskSize" ,
                                      "T6MaskFactor","T6LowerBinaryFactor","T6UpperBinaryFactor","T6MaskingLevel","T6PixelThreshold" , "T6PixelScanRange" ,"T6CutoffPixels","T6JumpThreshold"};

                for (int i = 0; i < SurfaceFGSettingsTxtBxs.Length; i++)
                    CurrentMachineDataR[UpdateTableIndex("SurfaceFGSettings")][ColNames[i]] = SurfaceFGSettingsTxtBxs[i].Text;

                CheckBox[] MachineDetailsChkBxs = { T6ApplySubAlgo, T6ApplyDirectionalScan };
                String[] ChkBxNames = { "T6ApplySubAlgo", "T6ApplyDirectionalScan" };

                for (int i = 0; i < MachineDetailsChkBxs.Length; i++)
                    CurrentMachineDataR[UpdateTableIndex("SurfaceFGSettings")][ChkBxNames[i]] = MachineDetailsChkBxs[i].IsChecked;

                switch (AlgoTypeCBx.SelectedIndex)
                {
                    case 0:
                        CurrentMachineDataR[UpdateTableIndex("UserSettings")]["SelectedAlgorithm"] = 0;
                        GlobalSettings.CurrentEdgeDetectionAlgorithm = GlobalSettings.EdgeDetectionAlgorithmType.Type1;
                        break;
                    case 1:
                        CurrentMachineDataR[UpdateTableIndex("UserSettings")]["SelectedAlgorithm"] = 1;
                        GlobalSettings.CurrentEdgeDetectionAlgorithm = GlobalSettings.EdgeDetectionAlgorithmType.Type2;
                        break;
                    case 2:
                        CurrentMachineDataR[UpdateTableIndex("UserSettings")]["SelectedAlgorithm"] = 2;
                        GlobalSettings.CurrentEdgeDetectionAlgorithm = GlobalSettings.EdgeDetectionAlgorithmType.Type3;
                        break;
                    case 3:
                        CurrentMachineDataR[UpdateTableIndex("UserSettings")]["SelectedAlgorithm"] = 3;
                        GlobalSettings.CurrentEdgeDetectionAlgorithm = GlobalSettings.EdgeDetectionAlgorithmType.Type4;
                        break;
                    case 4:
                        CurrentMachineDataR[UpdateTableIndex("UserSettings")]["SelectedAlgorithm"] = 4;
                        GlobalSettings.CurrentEdgeDetectionAlgorithm = GlobalSettings.EdgeDetectionAlgorithmType.Type5;
                        break;
                    case 5:
                        CurrentMachineDataR[UpdateTableIndex("UserSettings")]["SelectedAlgorithm"] = 5;
                        GlobalSettings.CurrentEdgeDetectionAlgorithm = GlobalSettings.EdgeDetectionAlgorithmType.Type6;
                        break;
                }

            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW31b", ex); }
        }
        //Update Camera axis value 
        void TabCameraAxisValueUpdate()
        {
            try
            {
                if (GlobalSettings.MultilevelZoomMahine)
                {
                    ////================================================Camera Axis Value Text Boxes and Check Boxes================================================//
                    //TextBox[] PixWidthTxtBxs = { MultiZoomLevel11XTxtBx, MultiZoomLevel15XTxtBx, MultiZoomLevel20XTxtBx, MultiZoomLevel24XTxtBx, MultiZoomLevel28XTxtBx,
                    //                     MultiZoomLevel33XTxtBx, MultiZoomLevel37XTxtBx, MultiZoomLevel41XTxtBx, MultiZoomLevel46XTxtBx,MultiZoomLevel50XTxtBx,MultiZoomLevel54XTxtBx,MultiZoomLevel58XTxtBx,MultiZoomLevel63XTxtBx,MultiZoomLevel67XTxtBx};
                    //TextBox[] AxisValueTxtBxs = { MultiZoomLevel11XTxtBx_AxisValue, MultiZoomLevel15XTxtBx_AxisValue, MultiZoomLevel20XTxtBx_AxisValue, MultiZoomLevel24XTxtBx_AxisValue, MultiZoomLevel28XTxtBx_AxisValue,MultiZoomLevel33XTxtBx_AxisValue,MultiZoomLevel37XTxtBx_AxisValue,
                    //                     MultiZoomLevel141XTxtBx_AxisValue, MultiZoomLevel46XTxtBx_AxisValue, MultiZoomLevel50XTxtBx_AxisValue, MultiZoomLevel54XTxtBx_AxisValue,MultiZoomLevel58XTxtBx_AxisValue,MultiZoomLevel63XTxtBx_AxisValue,MultiZoomLevel67XTxtBx_AxisValue};
                    //CheckBox[] CAVHiddenChkBxs = { MultiZoomLevel11XHiddenChkBx, MultiZoomLevel15XHiddenChkBx, MultiZoomLevel20XHiddenChkBx, MultiZoomLevel24XHiddenChkBx, MultiZoomLevel28XHiddenChkBx,MultiZoomLevel33XHiddenChkBx,MultiZoomLevel37XHiddenChkBx,MultiZoomLevel41XHiddenChkBx,
                    //                     MultiZoomLevel46XHiddenChkBx, MultiZoomLevel50XHiddenChkBx, MultiZoomLevel54XHiddenChkBx, MultiZoomLevel58XHiddenChkBx,MultiZoomLevel63XHiddenChkBx,MultiZoomLevel67XHiddenChkBx};
                    ////Pixel Width Values               
                    //for (int i = 0; i < CamAxisRows.Length; i++)
                    //{
                    //    CamAxisRows[i]["PixelValue_R"] = PixWidthTxtBxs[i].Text;
                    //    CamAxisRows[i]["Hidden"] = CAVHiddenChkBxs[i].IsChecked;
                    //    CamAxisRows[i]["AxisValue_R"] = AxisValueTxtBxs[i].Text;
                    //}
                    DataTable CamAxisTable = Settings_DataB.GetTable("CameraAxisValue");
                    foreach (DataRow dr in ZoomCalibrationTable.Rows)
                    {
                        dr["MachineNo"] = MachineNo;
                        DataRow ndr;
                        CamAxisRows = CamAxisTable.Select("MachineNo = '" + MachineNo + "' AND AxisValue_R = " + Convert.ToDouble(dr[2]));
                        if (CamAxisRows.Length == 0)
                            //We need to add a new row here
                            ndr = CamAxisTable.NewRow();
                        else
                            ndr = CamAxisRows[0];
                        ndr["MachineNo"] = MachineNo;
                        ndr["AxisValue_R"] = dr[2];
                        ndr["PixelValue_R"] = dr[3];
                        ndr["R_ZoomLevel"] = dr[4];
                        ndr["Hidden"] = false;

                        if (CamAxisRows.Length == 0) CamAxisTable.Rows.Add(ndr);
                    }
                    Settings_DataB.Update_Table("CameraAxisValue");
                }
            }
            catch (Exception)
            {
            }
        }
        //Update Filteration Values
        void TabFilterationValueUpdate()
        {
            try
            {
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["ApplyFilteration"] = ApplyFilterationCkbx.IsChecked;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["FilterIterationFactor"] = FilterIterationFactorTxtBx.Text;
                CurrentMachineDataR[UpdateTableIndex("UserSettings")]["FilterFactor"] = FilterFactorTxtBx.Text;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETWFilterationValue", ex);
            }
        }
        //Tab Rotary Measurement
        void TabRotaryMeasurementUpdate()
        {
            try
            {
                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
                {

                    ObservableCollection<RotaryMeasurementTypetails> RMeasurementColl = RotaryMeasurementColl;
                    if (RMeasurementColl == null)
                        return;

                    DataTable Dt = GlobalSettings.SettingsReader.GetTable("RotaryMeasurementTypeDetails");
                    int MaxId = 0;
                    if (Dt.Rows.Count == 0)
                        MaxId = 0;
                    else
                        MaxId = Convert.ToInt32(Dt.Compute("MAX(ID)", ""));

                    DataRow[] DetailsRow = Dt.Select("MachineNo= '" + MachineNo + "'");

                    //To Update and Delete the Rows
                    int Rowcount = DetailsRow.Length;
                    for (int i = 0; i < Rowcount; i++)
                    {
                        bool RowDeleted = true;
                        for (int Pcount = 0; Pcount < RMeasurementColl.Count; Pcount++)
                        {
                            try
                            {
                                //iF rOWS exists then Update the Values
                                if (Convert.ToInt32(DetailsRow[i]["ID"]) == RMeasurementColl[Pcount].ID)
                                {
                                    DetailsRow[i]["ComponentName"] = RMeasurementColl[Pcount].ComponentName;
                                    DetailsRow[i]["ZeroingAngle"] = RMeasurementColl[Pcount].ZeroingAngle;
                                    RowDeleted = false;
                                    break;
                                }
                            }
                            catch (DeletedRowInaccessibleException)
                            {

                            }
                        }
                        if (RowDeleted)
                        {
                            DetailsRow[i].Delete();
                            GlobalSettings.SettingsReader.UpdateTable("RotaryMeasurementTypeDetails");
                            Dt = GlobalSettings.SettingsReader.GetTable("RotaryMeasurementTypeDetails");
                            DetailsRow = Dt.Select("MachineNo= '" + GlobalSettings.MachineNo + "'");
                            Rowcount--;
                            i--;
                        }
                    }
                    //To Add new rows
                    for (int i = 0; i < RMeasurementColl.Count; i++)
                    {
                        if (RMeasurementColl[i].ID == 0)
                        {
                            DataRow drow = Dt.NewRow();
                            drow["ComponentName"] = RMeasurementColl[i].ComponentName;
                            drow["ZeroingAngle"] = RMeasurementColl[i].ZeroingAngle;
                            drow["MachineNo"] = MachineNo;
                            RMeasurementColl[i].ID = ++MaxId;
                            Dt.Rows.Add(drow);
                        }
                    }
                    GlobalSettings.SettingsReader.UpdateTable("RotaryMeasurementTypeDetails");
                    CurrentMachineDataR[UpdateTableIndex("UserSettings")]["DelfiFloatingView"] = ViewChbk.IsChecked;
                    //GlobalSettings.ValueofBCX = Convert.ToDouble(HobBCXTxtBx.Text);
                    //GlobalSettings.ValueofBCY = Convert.ToDouble(HobBCYTxtBx.Text);
                    //RWrapper.RW_MainInterface.MYINSTANCE().GetvalueofBCXBCY(GlobalSettings.ValueofBCX, GlobalSettings.ValueofBCY);
                    //CurrentMachineDataR[UpdateTableIndex("UserSettings")]["HobBCX"] = HobBCXTxtBx.Text;
                    //CurrentMachineDataR[UpdateTableIndex("UserSettings")]["HobBCY"] = HobBCYTxtBx.Text;
                }
                else if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Horizontal)
                {
                    if (GlobalSettings.RotaryAxisForMeasurement)
                    {
                        CurrentMachineDataR[UpdateTableIndex("TIS_Settings")]["RotaryScanSpan"] = txtRotaryScanSpan.Text;
                        CurrentMachineDataR[UpdateTableIndex("TIS_Settings")]["RotaryScanSpeed"] = txtRotaryScanSpeed.Text;
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETWRotaryMeawurement", ex);
            }
        }
        //Tab OneShot Parameters
        void TabOneShotUpdate()
        {
            try
            {
                if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.One_Shot && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.Normal_As_One_Shot)
                    return;

                TextBox[] GP1TExtboxes = { PixelWidthCorrectionParameterQTxtBx, PW_LinearX, PW_LinearY, ZeroCTxtBx, X0TxtBx, Y0TxtBx, bd1, bd2, bd3, bd4, bd5, bd6 };

                for (int i = 0; i < GP1TExtboxes.Length; i++)
                {
                    CurrentMachineDataR[UpdateTableIndex("OneShotParams")][i + 3] = Convert.ToDouble(GP1TExtboxes[i].Text);
                    GlobalSettings.OneShotPixelWidthCorrections[i] = Convert.ToDouble(GP1TExtboxes[i].Text);
                }
                GlobalSettings.PixelWidthCorrectionEnable = (bool)PixelWidthCorrectionChkBx.IsChecked;
                GlobalSettings.EnableBD = (bool)EnableBD_Corr.IsChecked;

                CurrentMachineDataR[UpdateTableIndex("OneShotParams")][2] = GlobalSettings.PixelWidthCorrectionEnable;
                CurrentMachineDataR[UpdateTableIndex("OneShotParams")][15] = GlobalSettings.EnableBD;
                CurrentMachineDataR[UpdateTableIndex("OneShotParams")]["SingleComponentinOneShot"] = !((bool)chkMultipleComponentsperImage.IsChecked);
                CurrentMachineDataR[UpdateTableIndex("OneShotParams")]["Profile_Slope"] = Convert.ToDouble(txtProfile_Slope.Text);
                CurrentMachineDataR[UpdateTableIndex("OneShotParams")]["Profile_BaseValue"] = Convert.ToDouble(txtProfile_BaseValue.Text);
                CurrentMachineDataR[UpdateTableIndex("OneShotParams")]["Surface_Slope"] = Convert.ToDouble(txtSurface_Slope.Text);
                CurrentMachineDataR[UpdateTableIndex("OneShotParams")]["Surface_BaseValue"] = Convert.ToDouble(txtSurface_BaseValue.Text);

                RWrapper.RW_MainInterface.MYINSTANCE().SetPixelWidthCorrectionStatusForOneShot(GlobalSettings.PixelWidthCorrectionEnable);

                //RWrapper.RW_MainInterface.MYINSTANCE().SetPixelWidthCorrectionConstants(GlobalSettings.OneShotPixelWidthCorrections);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETOneShotParams", ex);
            }

        }
        //To update all other remaining values
        bool UpdateRemainingValues(bool MachineNumExists)
        {
            try
            {
                if (!MachineNumExists)
                {
                    CurrentMachineDataR[UpdateTableIndex("MachineDetails")]["CrosshairColorR"] = "255";
                    CurrentMachineDataR[UpdateTableIndex("MachineDetails")]["CrosshairColorG"] = "0";
                    CurrentMachineDataR[UpdateTableIndex("MachineDetails")]["CrosshairColorB"] = "0";
                    CurrentMachineDataR[UpdateTableIndex("MachineDetails")]["NoOfAxes"] = 3;
                    CurrentMachineDataR[UpdateTableIndex("MachineDetails")]["DateNTime"] = DateTime.Now;
                    //CurrentMachineDataR[UpdateTableIndex("MachineDetails")]["MesureNoOfDec"] = 4;
                    CurrentMachineDataR[UpdateTableIndex("MachineDetails")]["R_Upper_Correction"] = 15;
                    CurrentMachineDataR[UpdateTableIndex("MachineDetails")]["R_Lower_Correction"] = 10;
                    CurrentMachineDataR[UpdateTableIndex("UserSettings")]["DateNTime"] = DateTime.Now;
                    CurrentMachineDataR[UpdateTableIndex("UserSettings")]["DefaultFixtureName"] = "Default";
                    CurrentMachineDataR[UpdateTableIndex("UserSettings")]["IgnorePtPercent"] = 500;
                    CurrentMachineDataR[UpdateTableIndex("MeasurementSettings")]["MinFontSize"] = 8;
                    CurrentMachineDataR[UpdateTableIndex("MeasurementSettings")]["MaxFontSize"] = 16;
                    CurrentMachineDataR[UpdateTableIndex("MeasurementSettings")]["X_Shift_FocusCalib"] = 1;
                    CurrentMachineDataR[UpdateTableIndex("MeasurementSettings")]["Y_Shift_FocusCalib"] = 1;
                    CurrentMachineDataR[UpdateTableIndex("MeasurementSettings")]["Deviation1_FocusCalib"] = 0;
                    CurrentMachineDataR[UpdateTableIndex("MeasurementSettings")]["Deviation2_FocusCalib"] = 0;

                    for (int RAdd = 0; RAdd < CurrentMachineDataR.Length; RAdd++)
                    {
                        CurrentMachineDataR[RAdd]["MachineNo"] = MachineNo;
                        Settings_DataB.GetTable(UpdateTableNames[RAdd]).Rows.Add(CurrentMachineDataR[RAdd]);
                    }

                    string[] DRONames = { "X", "Y", "Z", "R" };
                    for (int Count = 0; Count < DROROWS.Length; Count++)
                    {
                        DROROWS[Count]["MachineNo"] = MachineNo;
                        DROROWS[Count]["AxisName"] = DRONames[Count];
                        DROROWS[Count]["AxisPositiveHomeValue"] = 0;
                        Settings_DataB.GetTable("DROSettings").Rows.Add(DROROWS[Count]);
                    }
                    for (int Count = 0; Count < MagRows.Length; Count++)
                    {
                        MagRows[Count]["MachineNo"] = MachineNo;
                        Settings_DataB.GetTable("MagLevels").Rows.Add(MagRows[Count]);
                    }
                }

                //Updating the database
                string FileName = RWrapper.RW_MainInterface.MYINSTANCE().AppDataFolderPath + "\\Database\\RapidSettings.mdb";
                FileInfo fi = new FileInfo(FileName);
                if (fi.IsReadOnly)
                    return false;

                //for (int Count = 0; Count < CurrentMachineDataR.Length; Count++)
                //Settings_DataB.Update_Table(UpdateTableNames[Count]);
                try
                {                                                                                    //1 Written by Harphool
                    Settings_DataB.Update_Table("DROSettings");
                }
                catch (Exception Ex) { MessageBox.Show(Ex.Message); }                                    //2

                try
                {                                                                                    //3
                    Settings_DataB.Update_Table("MagLevels");
                }
                catch (Exception Ex) { MessageBox.Show(Ex.Message); }                                    //4

                try
                {                                                                                    //5
                                                                                                     //if (Settings_DataB.GetTable("CurrentMachine").Rows[Settings_DataB.GetRowCount("CurrentMachine") - 1]["MachineNo"] != MachineNo)
                                                                                                     //{
                    System.Data.DataRow dr = Settings_DataB.DataS.Tables["CurrentMachine"].NewRow();
                    dr["MachineNo"] = MachineNo;
                    dr["Version_number"] = Settings_DataB.GetTable("CurrentMachine").Rows[Settings_DataB.GetRowCount("CurrentMachine") - 1]["Version_number"];
                    dr["Release_Date"] = Settings_DataB.GetTable("CurrentMachine").Rows[Settings_DataB.GetRowCount("CurrentMachine") - 1]["Release_Date"];
                    Settings_DataB.DataS.Tables["CurrentMachine"].Rows.Add(dr);
                    Settings_DataB.Update_Table("CurrentMachine");
                    //}

                }
                catch (Exception Ex) { MessageBox.Show(Ex.Message); }                                    //6

                //try{                                                                                    //7
                //    Settings_DataB.Update_Table("Login");
                //}catch (Exception Ex){ MessageBox.Show(Ex.Message);}                                    //8

                try
                {                                                                                    //9
                    Settings_DataB.Update_Table("FixtureDetails");
                }
                catch (Exception Ex) { MessageBox.Show(Ex.Message); }                                    //10

                try
                {                                                                                    //11
                    Settings_DataB.Update_Table("CurrentMachine");
                }
                catch (Exception Ex) { MessageBox.Show(Ex.Message); }                                   //12

                //if (!MachineNumExists)
                CleanDatabase();

                RWrapper.RW_DBSettings.MYINSTANCE().UpdateCalibrationSettings();
                return true;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW32", ex);
                return false;
            }
        }

        private void BtnUpdateEEPROM_Click(object sender, RoutedEventArgs e)
        {
            MessageBoxResult msgR = MessageBox.Show("Are you sure you want to update EEPROM values in the database and hardware with the current values?", "Rapid-I 5.0", MessageBoxButton.YesNo, MessageBoxImage.Question);
            if (msgR == MessageBoxResult.Yes)
            {
                UpdatingEEPROMValuesNow = true;
                UpdateAll();
                UpdatingEEPROMValuesNow = false;
                //TabEEPROMSettingsUpdate();
            }
        }

        #endregion

        #region Functions to export Database values to text file for data transportation

        void ExportBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                ExportAll();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:EX00", ex); }
        }
        //Function to Export entries related to current Selected Machine By Shweytank Mishra
        void ExportAll()
        {
            try
            {
                //TableNames = new string[] {"UserSettings", "DXFSettings", "MeasurementSettings", "LightingSettings", "FixtureDetails",
                //            "MachineDetails", "VisionSettings", "MagLevels", "MagnifierSettings", 
                //            "DROSettings", "EEPROMSettings","CalibrationSettings","FocusScanCalibration","CameraAxisValue","ProbeSettings"};
                List<string> TableNames = new List<string>();
                MachineNo = MachineNumCBx.Text;
                //Written on 28-oct2012            
                System.Data.OleDb.OleDbConnection dbcon = new System.Data.OleDb.OleDbConnection(RWrapper.RW_MainInterface.DBConnectionString);
                dbcon.Open();
                DataTable schematable = new DataTable();
                schematable = dbcon.GetOleDbSchemaTable(System.Data.OleDb.OleDbSchemaGuid.Tables, new object[] { null, null, null, "Table" });
                dbcon.Close();
                for (int i = 0; i < schematable.Rows.Count; i++)
                {
                    dbcon.Open();
                    DataTable CollumnTable = dbcon.GetOleDbSchemaTable(System.Data.OleDb.OleDbSchemaGuid.Columns, new object[] { null, null, schematable.Rows[i]["Table_Name"], null });
                    dbcon.Close();
                    for (int j = 0; j < CollumnTable.Rows.Count; j++)
                    {
                        if (CollumnTable.Rows[j]["Column_Name"].ToString() == "MachineNo")
                        {
                            TableNames.Add(schematable.Rows[i]["Table_Name"].ToString());
                            break;
                        }
                    }
                }
                //Written on 28-oct2012 

                Microsoft.Win32.SaveFileDialog dlg = new Microsoft.Win32.SaveFileDialog();
                dlg.FileName = "Document"; // Default file name
                dlg.DefaultExt = ".xml"; // Default file extension
                dlg.Filter = "Text documents (.XML)|*.xml"; // Filter files by extension 
                dlg.InitialDirectory = System.Environment.GetFolderPath(Environment.SpecialFolder.MyComputer);
                // Show save file dialog box
                Nullable<bool> result = dlg.ShowDialog();
                // Process save file dialog box results 
                if (result == true)
                {
                    // Save document 
                    string filename = dlg.FileName;
                    DataSet ds = new DataSet();
                    for (int i = 0; i < TableNames.Count; i++)
                        ds.Tables.Add(GetCurrentMachineNoRowsForTable(MachineNo, TableNames[i]));
                    string XmlRootName = "MachineNo" + MachineNo;
                    XmlDocument doc = new XmlDocument();
                    XmlNode root = doc.CreateElement(XmlRootName);
                    doc.AppendChild(root);
                    string StackTrace = "   at System.Data.DataTableCollection.BaseAdd(DataTable table)\r\n   at System.Data.DataTableCollection.Add(DataTable table)\r\n   at Rapid.Windows.SettingsWin.ExportAll() in E:\\Rapid-i\\Sources\\Build3600\\Rapid-i\\RapidI_MVVM\\Views\\Settings\\SettingsWin.xaml.cs:line 2947";

                    XmlNode version = doc.CreateXmlDeclaration("1.0", "utf-8", "yes");
                    doc.InsertBefore(version, root);
                    for (int i = 0; i < ds.Tables.Count; i++)
                    {
                        AddHeadings(root, ds.Tables[i].TableName.ToString(), ds.Tables[i]);
                    }
                    doc.Save(filename);
                    MessageBox.Show("Database values exported successfully");
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:EX01", ex);
                MessageBox.Show(ex + "Export : 1");
            }
        }
        //Written by Shweytank Mishra

        void ImportBtn_Click(object sender, RoutedEventArgs e)
        {
            XML_Exim thisXmlExim = new XML_Exim(XMLImportStatus);
            thisXmlExim.ImportXML();

        }

        private void AddHeadings(XmlNode Root, string TableName, DataTable dt)
        {
            try
            {
                XmlNode Table = Root.OwnerDocument.CreateElement("Table");
                XmlAttribute attrib;
                attrib = Root.OwnerDocument.CreateAttribute("Name");
                attrib.Value = TableName;
                Table.Attributes.Append(attrib);
                Root.AppendChild(Table);
                for (int j = 0; j < dt.Rows.Count; j++)
                {
                    XmlNode Row = Table.OwnerDocument.CreateElement("DataRow");
                    Table.AppendChild(Row);
                    for (int col = 1; col < dt.Rows[j].ItemArray.Length; col++)
                    {
                        XmlNode Collumns = Row.OwnerDocument.CreateElement(dt.Columns[col].ColumnName.ToString());
                        XmlAttribute CollumnDatatype;
                        CollumnDatatype = Row.OwnerDocument.CreateAttribute("DataType");
                        CollumnDatatype.Value = dt.Columns[col].DataType.ToString();
                        Collumns.Attributes.Append(CollumnDatatype);
                        Collumns.InnerText = dt.Rows[j][col].ToString();
                        Row.AppendChild(Collumns);
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:EX02", ex);
            }
        }
        //Written by Shweytank Mishra
        DataTable GetCurrentMachineNoRowsForTable(string MachineNumber, string TableName)
        {
            try
            {
                System.Data.OleDb.OleDbConnection con = new System.Data.OleDb.OleDbConnection();
                con.ConnectionString = RWrapper.RW_MainInterface.DBConnectionString;
                string query = "Select * from " + TableName + " where MachineNo ='" + MachineNumber + "'";
                con.Open();
                System.Data.OleDb.OleDbDataAdapter adapter = new System.Data.OleDb.OleDbDataAdapter(query, con);
                DataTable DT = new DataTable(TableName);
                adapter.Fill(DT);
                con.Close();
                return DT;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:EX03", ex);
                return null;
            }
        }

        #endregion

        #region Button and ComboBox event handlers

        void MachineNumCBx_SelectionChanged(object sender, SelectionChangedEventArgs arg)
        {
            try
            {
                if (NegletCBoxSelection) return;
                if (MachineNumCBx.SelectedIndex < 0)
                    return;
                MachineNo = MachineNumCBx.SelectedItem.ToString();
                DROROWS = Settings_DataB.Select_ChildControls("DROSettings", "MachineNo", MachineNo);
                MagRows = Settings_DataB.Select_ChildControls("MagLevels", "MachineNo", MachineNo);
                FixtureRows = Settings_DataB.Select_ChildControls("FixtureDetails", "MachineNo", MachineNo);

                System.Threading.Thread.Sleep(500);
                switch (GlobalSettings.CurrentUserLevel)
                {
                    case GlobalSettings.UserPrevilageLevel.None:
                    case GlobalSettings.UserPrevilageLevel.User:
                        initUser();
                        break;
                    case GlobalSettings.UserPrevilageLevel.AdminUser:
                        initAdminUser();
                        break;
                    case GlobalSettings.UserPrevilageLevel.Admin:
                        initAdmin();
                        break;
                    case GlobalSettings.UserPrevilageLevel.System:
                        initSystem();
                        break;
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW33", ex); }
        }

        void UserNameCBx_SelectionChanged(object sender, SelectionChangedEventArgs arg)
        {
            try
            {
                if (UserNameCBx.SelectedIndex < 0) return;
                string TmpStr = UserNameCBx.SelectedItem.ToString();
                FillCheckBox(AllowUserBuildPPChkBx, "Login", "Login_Name", TmpStr, "AllowPartProgramBuilding");
                FillCheckBox(AllowUserRunPPChkBx, "Login", "Login_Name", TmpStr, "AllowPartProgramRunning");
                FillCheckBox(chkAllowLogin, "Login", "Login_Name", TmpStr, "AllowUserLogin");
                FillCheckBox(chkLightingAccess, "Login", "Login_Name", TmpStr, "AllowLightingSettingsAccess");
                //if (Convert.ToString(Settings_DataB.Select_ChildControls("Login", "Login_Name", TmpStr)[0]["LoginPriviledge"]) == "Admin")
                //    chkAdminAccess.IsChecked = true;
                //else
                //    chkAdminAccess.IsChecked = false;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW34", ex); }
        }

        void MachineTypeCBx_SelectionChanged(object sender, SelectionChangedEventArgs arg)
        {
            try
            {
                if (MachineTypeCBx.SelectedItem == null) return;
                if (MachineTypeCBx.SelectedItem.ToString() == "One Shot")
                    ShowHidePixelWidthOfYDir(true);
                else
                    ShowHidePixelWidthOfYDir(false);
                EnableOneshotFeaturesChkBx.IsChecked = false;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW35", ex); }
        }

        void NumberOfAxisCBx_SelectionChanged(object sender, SelectionChangedEventArgs arg)
        {
            try
            {
                if (NoOfAxisCBx.SelectedItem == null || AddingItemsToAxisNumberCB) return;
                if (NoOfAxisCBx.SelectedItem.ToString() == "4")
                    RotaryMeasurementTab.Visibility = System.Windows.Visibility.Visible;
                else
                    RotaryMeasurementTab.Visibility = System.Windows.Visibility.Collapsed;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW35", ex); }
        }

        void DefaultMagLevelCBx_SelectionChanged(object sender, SelectionChangedEventArgs arg)
        {
            try
            {
                if (DefaultMagLevelCBx.SelectedItem == null) return;
                EnableDisableMagLevelHiddenChkBx(DefaultMagLevelCBx.SelectedItem.ToString());
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW36", ex); }
        }

        void FixtureNameCBx_SelectionChanged(object sender, SelectionChangedEventArgs arg)
        {
            try
            {
                if (FixtureNameCBx.SelectedItem == null) return;

                for (int i = 0; i < FixtureRows.Length; i++)
                    if (FixtureRows[i]["FixtureName"].ToString() == FixtureNameCBx.SelectedItem.ToString())
                    {
                        NoOfRowsTxtBx.Text = FixtureRows[i]["No_of_Rows"].ToString();
                        NoOfColsTxtBx.Text = FixtureRows[i]["No_of_Columns"].ToString();
                        RowGapTxtBx.Text = (Convert.ToDouble(FixtureRows[i]["RowGap"]) / UnitType_MulFactor).ToString();
                        ColGapTxtBx.Text = (Convert.ToDouble(FixtureRows[i]["ColumnGap"]) / UnitType_MulFactor).ToString();
                        break;
                    }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW37", ex); }
        }

        void GraphicsTxtFamilyCBx_SelectionChanged(object sender, SelectionChangedEventArgs arg)
        {
            try
            {
                if (GraphicsTxtFamilyCBx.SelectedIndex > -1)
                {
                    GlobalSettings.FontSelectedIndx = GraphicsTxtFamilyCBx.SelectedIndex;
                    RWrapper.RW_MainInterface.MYINSTANCE().SetOGLFontFamily(GraphicsTxtFamilyCBx.SelectedItem.ToString());
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW37", ex); }
        }
        void MeasurementModeCBx_SelectionChanged(object sender, SelectionChangedEventArgs arg)
        {
            try
            {
                if (MeasurementModeCBx.SelectedIndex > -1)
                {
                    //if (GlobalSettings.MeasurementColorMode == "Gradient")
                    //{
                    //    RWrapper.RW_MainInterface.MYINSTANCE().SetMeasurementColorGradient(true);
                    //}
                    //else
                    //    RWrapper.RW_MainInterface.MYINSTANCE().SetMeasurementColorGradient(false);
                    if (MeasurementModeCBx.SelectedIndex == 0)
                        RWrapper.RW_MainInterface.MYINSTANCE().SetMeasurementColorGradient(true);
                    else
                        RWrapper.RW_MainInterface.MYINSTANCE().SetMeasurementColorGradient(false);
                    GlobalSettings.MeasurementColorMode = MeasurementModeCBx.SelectedValue.ToString();
                    //GlobalSettings.FontSelectedIndx = GraphicsTxtFamilyCBx.SelectedIndex;
                    //RWrapper.RW_MainInterface.MYINSTANCE().SetOGLFontFamily(GraphicsTxtFamilyCBx.SelectedItem.ToString());
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW37", ex); }
        }
        void UserNameDeleteBtn_Click(object sender, RoutedEventArgs arg)
        {
            try
            {
                //Check whether item selected is present in item collection or not.
                if (UserNameCBx.Items.Contains(UserNameCBx.Text))
                {
                    System.Data.DataRow dr = Settings_DataB.Select_ChildControls("Login", "Login_Name", UserNameCBx.Text)[0];
                    dr.Delete();
                    UserNameCBx.Items.Remove(UserNameCBx.Text);
                    //UpdateUser();
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW38", ex); }
        }
        void MachineNumCBx_TextInput(object sender, RoutedEventArgs arg)
        {
            if (!MachineNumCBx.Items.Contains(MachineNumCBx.Text))
                CompanyNameTxtBx.Text = "Customised Technologies (P) Ltd";
        }
        void UserNameCBx_TextInput(object sender, RoutedEventArgs arg)
        {
        }
        void UserNameSaveBtn_Click(object sender, RoutedEventArgs arg)
        {
            try
            {
                if (!AddOrEditUsers())
                    MessageBox.Show("The User Name or Password was Incorrect! Please enter again.");
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW39", ex); }
        }
        void SubmitLicenseKeyBtn_Click(object sender, RoutedEventArgs arg)
        {
            try
            {
                String LogFilePath = RWrapper.RW_MainInterface.MYINSTANCE().AppDataFolderPath + "\\Database";
                LicensingDLL LicenseMod = new LicensingDLL(RWrapper.RW_MainInterface.DBConnectionString, LogFilePath);
                if (LicenseMod.CheckKey(LicenseKeyTxtBx.Text))
                {
                    System.Windows.Forms.MessageBox.Show("Update Successful!" + "\n" +
                                    "You have aquired additional license." + "\n" +
                                    "Please restart the software for the changes to take effect.", "Rapid-I");
                    this.Close();
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW39a", ex); }
        }
        void MagHiddenChkBxs_Checked(object sender, RoutedEventArgs arg)
        {
            try
            {
                CheckBox[] MagHiddenChkBxs = { MagLevel3XHiddenChkBx, MagLevel5XHiddenChkBx, MagLevel11XHiddenChkBx, MagLevel22XHiddenChkBx, MagLevel34XHiddenChkBx,
                                         MagLevel54XHiddenChkBx, MagLevel67XHiddenChkBx, MagLevel134XHiddenChkBx, MagLevel335XHiddenChkBx};
                List<string> MagNames = new List<string> { "3X", "5X", "11X", "22X", "34X", "54X", "67X", "134X", "335X" };

                for (int i = 0; i < MagHiddenChkBxs.Length; i++)
                    if (sender.Equals(MagHiddenChkBxs[i]))
                        if (DefaultMagLevelCBx.Items.Contains(MagNames[i]))
                            DefaultMagLevelCBx.Items.Remove(MagNames[i]);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW40", ex); }
        }
        void MagHiddenChkBxs_Unchecked(object sender, RoutedEventArgs arg)
        {
            try
            {
                CheckBox[] MagHiddenChkBxs = { MagLevel3XHiddenChkBx, MagLevel5XHiddenChkBx, MagLevel11XHiddenChkBx, MagLevel22XHiddenChkBx, MagLevel34XHiddenChkBx,
                                         MagLevel54XHiddenChkBx, MagLevel67XHiddenChkBx, MagLevel134XHiddenChkBx, MagLevel335XHiddenChkBx};
                List<string> MagNames = new List<string> { "3X", "5X", "11X", "22X", "34X", "54X", "67X", "134X", "335X" };

                for (int i = 0; i < MagHiddenChkBxs.Length; i++)
                    if (sender.Equals(MagHiddenChkBxs[i]))
                        if (!DefaultMagLevelCBx.Items.Contains(MagNames[i]))
                            DefaultMagLevelCBx.Items.Add(MagNames[i]);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW41", ex); }
        }
        void FixtureSaveBtn_Click(object sender, RoutedEventArgs arg)
        {
            try
            {
                if (FixtureNameCBx.Items.Contains(FixtureNameCBx.Text))
                {
                    for (int i = 0; i < FixtureRows.Length; i++)
                        if (FixtureRows[i]["FixtureName"].ToString() == FixtureNameCBx.Text)
                        {
                            FixtureRows[i]["No_of_Rows"] = NoOfRowsTxtBx.Text;
                            FixtureRows[i]["No_of_Columns"] = NoOfColsTxtBx.Text;
                            FixtureRows[i]["RowGap"] = Convert.ToDouble(RowGapTxtBx.Text) * UnitType_MulFactor;
                            FixtureRows[i]["ColumnGap"] = Convert.ToDouble(ColGapTxtBx.Text) * UnitType_MulFactor;
                            int Fixtindx = GlobalSettings.FixtureNameList.IndexOf(FixtureNameCBx.Text);
                            GlobalSettings.FixRowNum[Fixtindx] = NoOfRowsTxtBx.Text;
                            GlobalSettings.FixColNum[Fixtindx] = NoOfColsTxtBx.Text;
                            GlobalSettings.FixRowGap[Fixtindx] = (Convert.ToDouble(RowGapTxtBx.Text) * UnitType_MulFactor).ToString();
                            GlobalSettings.FixColGap[Fixtindx] = (Convert.ToDouble(ColGapTxtBx.Text) * UnitType_MulFactor).ToString();
                            break;
                        }
                }
                else
                {
                    System.Data.DataRow dr = Settings_DataB.DataS.Tables["FixtureDetails"].NewRow();
                    dr["MachineNo"] = MachineNo;
                    dr["FixtureName"] = FixtureNameCBx.Text;
                    dr["No_of_Rows"] = NoOfRowsTxtBx.Text;
                    dr["No_of_Columns"] = NoOfColsTxtBx.Text;
                    dr["RowGap"] = Convert.ToDouble(RowGapTxtBx.Text) * UnitType_MulFactor;
                    dr["ColumnGap"] = Convert.ToDouble(ColGapTxtBx.Text) * UnitType_MulFactor;
                    Settings_DataB.DataS.Tables["FixtureDetails"].Rows.Add(dr);
                    FixtureRows = Settings_DataB.Select_ChildControls("FixtureDetails", "MachineNo", MachineNo);
                    FixtureNameCBx.Items.Add(FixtureNameCBx.Text);

                    GlobalSettings.FixRowNum.Add(NoOfRowsTxtBx.Text);
                    GlobalSettings.FixColNum.Add(NoOfColsTxtBx.Text);
                    GlobalSettings.FixRowGap.Add((Convert.ToDouble(RowGapTxtBx.Text) * UnitType_MulFactor).ToString());
                    GlobalSettings.FixColGap.Add((Convert.ToDouble(ColGapTxtBx.Text) * UnitType_MulFactor).ToString());
                    GlobalSettings.FixtureNameCBx.Items.Add(FixtureNameCBx.Text);
                    GlobalSettings.FixtureNameList.Add(FixtureNameCBx.Text);
                    GlobalSettings.FixtureNameCBx.SelectedItem = FixtureNameCBx.Text;
                }
                Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["DefaultFixtureName"] = FixtureNameCBx.Text;
                string FileName = RWrapper.RW_MainInterface.MYINSTANCE().AppDataFolderPath + "\\Database\\RapidSettings.mdb";
                FileInfo fi = new FileInfo(FileName);
                if (fi.IsReadOnly)
                    MessageBox.Show("Could not save changes!! Please Check if the Settings file is writable.", "Rapid-I");

                Settings_DataB.Update_Table("FixtureDetails");
                Settings_DataB.Update_Table("UserSettings");
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW42", ex); }
        }
        void DxfAutoAlignSaveBtn_Click(object sender, RoutedEventArgs arg)
        {
            try
            {
                GlobalSettings.DxfAutoAlignDistVals.Clear();
                GlobalSettings.DxfAutoAlignOffset_XVals.Clear();
                GlobalSettings.DxfAutoAlignOffset_YVals.Clear();
                string FileName = RWrapper.RW_MainInterface.MYINSTANCE().AppDataFolderPath + "\\Database\\DxfAutoAlignVal.txt";
                StreamWriter FileWrt = new StreamWriter(FileName, false);

                for (int i = 0; i < DxfAlignProp_DGSourcing.Count; i++)
                {
                    DxfAutoAlignPropEntity TmpEntity = DxfAlignProp_DGSourcing[i] as DxfAutoAlignPropEntity;
                    FileWrt.WriteLine(TmpEntity.DistStr + "," + TmpEntity.Offset_XStr + "," + TmpEntity.Offset_YStr);
                    GlobalSettings.DxfAutoAlignDistVals.Add(TmpEntity.DistVal);
                    GlobalSettings.DxfAutoAlignOffset_XVals.Add(TmpEntity.Offset_XVal);
                    GlobalSettings.DxfAutoAlignOffset_YVals.Add(TmpEntity.Offset_YVal);
                }

                FileWrt.Close();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW42a", ex); }
        }
        void RcadBackGradTopColorBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                System.Windows.Forms.ColorDialog ColorDg = new System.Windows.Forms.ColorDialog();
                if (ColorDg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    Rectangle ModeIndicatorRectUp = (Rectangle)RcadBackGradTopColorBtn.Template.FindName("ModeIndicator", RcadBackGradTopColorBtn);
                    Rectangle ModeIndicatorRectDown = (Rectangle)RcadBackGradBottomColorBtn.Template.FindName("ModeIndicator", RcadBackGradBottomColorBtn);

                    RcadGradClrUp.R = ColorDg.Color.R;
                    RcadGradClrUp.G = ColorDg.Color.G;
                    RcadGradClrUp.B = ColorDg.Color.B;
                    RcadGradClrUp.A = ColorDg.Color.A;
                    ModeIndicatorRectUp.Fill = new SolidColorBrush(RcadGradClrUp);
                    RWrapper.RW_MainInterface.MYINSTANCE().SetOGLGradientBackGround(RcadGradClrUp.R, RcadGradClrUp.G, RcadGradClrUp.B, RcadGradClrDown.R, RcadGradClrDown.G, RcadGradClrDown.B);
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW42b", ex); }
        }
        void RcadBackGradBottomColorBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                System.Windows.Forms.ColorDialog ColorDg = new System.Windows.Forms.ColorDialog();
                if (ColorDg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    Rectangle ModeIndicatorRectUp = (Rectangle)RcadBackGradTopColorBtn.Template.FindName("ModeIndicator", RcadBackGradTopColorBtn);
                    Rectangle ModeIndicatorRectDown = (Rectangle)RcadBackGradBottomColorBtn.Template.FindName("ModeIndicator", RcadBackGradBottomColorBtn);

                    RcadGradClrDown.R = ColorDg.Color.R;
                    RcadGradClrDown.G = ColorDg.Color.G;
                    RcadGradClrDown.B = ColorDg.Color.B;
                    RcadGradClrDown.A = ColorDg.Color.A;
                    ModeIndicatorRectDown.Fill = new SolidColorBrush(RcadGradClrDown);
                    RWrapper.RW_MainInterface.MYINSTANCE().SetOGLGradientBackGround(RcadGradClrUp.R, RcadGradClrUp.G, RcadGradClrUp.B, RcadGradClrDown.R, RcadGradClrDown.G, RcadGradClrDown.B);
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW42c", ex); }
        }
        void EnableOneshotFeaturesChkBx_PreviewMouseDown(object sender, RoutedEventArgs e)
        {
            if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
            {
                if (!(bool)GlobalSettings.FeatureLicenseInfo[10])
                {
                    MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                    e.Handled = true;
                }
            }
        }

        protected void GenericProbOkBtn_Click(object Sender, RoutedEventArgs e)
        {
            try
            {
                ObservableCollection<UserDefinedProbType> ProbelistColl = ProbeTypeColl;
                if (ProbelistColl == null)
                    return;

                DataTable Dt = GlobalSettings.SettingsReader.GetTable("ProbAssemblyTypeDetails");
                int MaxId = Convert.ToInt32(Dt.Compute("MAX(ID)", ""));
                DataRow[] DetailsRow = Dt.Select("MachineNo= '" + GlobalSettings.MachineNo + "'");

                //To Update and Delete the Rows

                int Rowcount = DetailsRow.Length;
                for (int i = 0; i < Rowcount; i++)
                {
                    bool RowDeleted = true;
                    for (int Pcount = 0; Pcount < ProbelistColl.Count; Pcount++)
                    {
                        for (int j = 0; j < ProbelistColl[Pcount].ProbeDetails.Count; j++)
                        {
                            try
                            {
                                if (Convert.ToInt32(DetailsRow[i]["ID"]) == ProbelistColl[Pcount].ProbeDetails[j].ID)
                                {
                                    DetailsRow[i]["OffsetX"] = ProbelistColl[Pcount].ProbeDetails[j].OffsetX;
                                    DetailsRow[i]["OffsetY"] = ProbelistColl[Pcount].ProbeDetails[j].OffsetY;
                                    DetailsRow[i]["OffsetZ"] = ProbelistColl[Pcount].ProbeDetails[j].OffsetZ;
                                    DetailsRow[i]["Comment"] = ProbelistColl[Pcount].ProbeDetails[j].Comment;
                                    DetailsRow[i]["Orientation"] = ProbelistColl[Pcount].ProbeDetails[j].Orientation;
                                    DetailsRow[i]["Radius"] = ProbelistColl[Pcount].ProbeDetails[j].Radius;
                                    RowDeleted = false;
                                    break;
                                }
                            }
                            catch (DeletedRowInaccessibleException)
                            {

                            }
                        }
                    }
                    if (RowDeleted)
                    {
                        DetailsRow[i].Delete();
                        GlobalSettings.SettingsReader.UpdateTable("ProbAssemblyTypeDetails");
                        Dt = GlobalSettings.SettingsReader.GetTable("ProbAssemblyTypeDetails");
                        DetailsRow = Dt.Select("MachineNo= '" + GlobalSettings.MachineNo + "'");
                        Rowcount--;
                        i--;
                    }
                }
                //To Add new rows
                for (int Pcount = 0; Pcount < ProbelistColl.Count; Pcount++)
                {
                    for (int i = 0; i < ProbelistColl[Pcount].ProbeDetails.Count; i++)
                    {
                        if (ProbelistColl[Pcount].ProbeDetails[i].ID == 0)
                        {
                            DataRow drow = Dt.NewRow();
                            drow["ProbType"] = ProbelistColl[Pcount].ProbtypeName;
                            drow["OffsetX"] = ProbelistColl[Pcount].ProbeDetails[i].OffsetX;
                            drow["OffsetY"] = ProbelistColl[Pcount].ProbeDetails[i].OffsetY;
                            drow["OffsetZ"] = ProbelistColl[Pcount].ProbeDetails[i].OffsetZ;
                            drow["Comment"] = ProbelistColl[Pcount].ProbeDetails[i].Comment;
                            drow["Orientation"] = ProbelistColl[Pcount].ProbeDetails[i].Orientation;
                            drow["Radius"] = ProbelistColl[Pcount].ProbeDetails[i].Radius;
                            drow["IsValid"] = true;
                            drow["MachineNo"] = GlobalSettings.MachineNo;
                            ProbelistColl[Pcount].ProbeDetails[i].ID = ++MaxId;
                            Dt.Rows.Add(drow);
                        }
                    }
                }
                GlobalSettings.SettingsReader.UpdateTable("ProbAssemblyTypeDetails");
                MessageBox.Show("Values Updated");
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:GP03", ex);
            }
        }

        private void dgZoomCalibration_AutoGeneratedColumns(object sender, EventArgs e)
        {
            dgZoomCalibration.Columns[0].Visibility = Visibility.Hidden;
            dgZoomCalibration.Columns[1].Visibility = Visibility.Hidden;
            dgZoomCalibration.Columns[5].Visibility = Visibility.Hidden;
            dgZoomCalibration.Columns[2].Header = "Position (DRO)";
            dgZoomCalibration.Columns[3].Header = "Pixel Width (mm)";
            dgZoomCalibration.Columns[4].Header = "Mag Level (X)";
        }

        private void ToleranceFormatCheckUncheck_Handler(object sender, RoutedEventArgs e)
        {
            CheckBox[] TolCBs = { AllowedRangeRBtn, NomPlsMnsTolRBtn, ShwLSLorUSLRBtn };
            CheckBox thisChk = (CheckBox)sender;
            if ((bool)thisChk.IsChecked)
            {
                for (int i = 0; i < TolCBs.Length; i++)
                    if (TolCBs[i] != thisChk)
                        TolCBs[i].IsChecked = false;
            }
        }

        private void NoOfAxisCBx_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            int NoOfA = int.Parse(((ComboBox)sender).SelectedValue.ToString());
            if (NoOfA < 4)
                cbRotaryMeasurementPresent.Visibility = Visibility.Hidden;
            else
                cbRotaryMeasurementPresent.Visibility = Visibility.Visible;
        }

        private void BtnEEPromWriter_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (!EEpromUpdateResult.Text.Contains("uccess"))
                    return;
                MessageBoxResult mbr = MessageBox.Show("This will overwrite the current values in the EEPROM with currently loaded memory values. Are you sure you want to proceed?", "Rapid-I", MessageBoxButton.YesNo, MessageBoxImage.Warning);
                if (mbr == MessageBoxResult.No)
                    return;
                RWrapper.RW_DRO.MYINSTANCE().WriteToEEPROM();
                MessageBox.Show(this, "Please switch off and switch on the machine for the updates to take effect", "Rapid-I");
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:0591_x", ex);
                //throw;
            }
        }

        protected void AddBtn_Click(object Sender, RoutedEventArgs e)
        {
            try
            {
                string PTNAME = ProbeTypeChbk.Text;
                bool AlreadyExist = false;
                if (PTNAME == "")
                {
                    return;
                }
                foreach (UserDefinedProbType UDPT in ProbeTypeColl)
                {
                    if (PTNAME == UDPT.ProbtypeName)
                    {
                        AlreadyExist = true;
                        break;
                    }
                }
                if (AlreadyExist == true)
                {
                    MessageBox.Show("Probe Type  already exist ");
                }
                else
                {
                    DataTable Dt = GlobalSettings.SettingsReader.GetTable("ProbAssemblyTypeDetails");
                    int MaxId = 0;
                    if (Dt.Rows.Count != 0)
                    {
                        MaxId = Convert.ToInt32(Dt.Compute("MAX(ID)", ""));
                    }

                    DataRow drow = Dt.NewRow();
                    drow["ProbType"] = PTNAME;
                    drow["OffsetX"] = 0;
                    drow["OffsetY"] = 0;
                    drow["OffsetZ"] = 0;
                    drow["Comment"] = "None";
                    drow["IsValid"] = true;
                    drow["Orientation"] = "None";
                    drow["MachineNo"] = GlobalSettings.MachineNo;
                    drow["Radius"] = 2;
                    Dt.Rows.Add(drow);
                    GlobalSettings.SettingsReader.UpdateTable("ProbAssemblyTypeDetails");
                    Dt = GlobalSettings.SettingsReader.GetTable("ProbAssemblyTypeDetails");
                    UserDefinedProbType Ptype = new UserDefinedProbType();
                    Ptype.ProbtypeName = PTNAME;
                    Ptype.ProbeDetails = new ObservableCollection<Probedetails>();
                    Probedetails pd = new Probedetails();
                    pd.ProbtypeName = PTNAME;
                    pd.OffsetX = "0";
                    pd.OffsetY = "0";
                    pd.OffsetZ = "0";
                    pd.Comment = "None";
                    pd.Orientation = "None";
                    pd.ID = MaxId = Convert.ToInt32(Dt.Compute("MAX(ID)", ""));
                    Ptype.ProbeDetails.Add(pd);
                    ProbeTypeColl.Add(Ptype);
                    ProbeTypeChbk.SelectedItem = Ptype;
                    //ProbeType.SelectedIndex = PTypeColl.Count - 1;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:GP02", ex);
            }
        }

        bool PercentageisUpdating, FractionisUpdating;
        private void ProfileScanMoveToNextFrameFractionTxtBx_TextChanged(object sender, TextChangedEventArgs e)
        {

            try
            {//if (!FractionisUpdating) return;
                double fractionvalue = 0;
                if (double.TryParse(((TextBox)sender).Text, out fractionvalue))
                {
                    ProfileScanMovePercentageBox.Text = (100 * (1 - fractionvalue)).ToString();
                }
            }
            catch (Exception Ex)
            {
                
            }
        }

        private void chkRetractProbeMethod_Click(object sender, RoutedEventArgs e)
        {
            RetractProbePointModeChkBx.IsChecked = chkRetractProbeMethod.IsChecked;
        }

        private void ProfileScanMovePercentageBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                double percentagevalue = 0;
                FractionisUpdating = false;
                if (double.TryParse(((TextBox)sender).Text, out percentagevalue))
                {
                    ProfileScanMoveToNextFrameFractionTxtBx.Text = String.Format("{0:F" + GlobalSettings.CurrentDroPrecision + "}", (100 - percentagevalue) / 100);
                }
            }
            catch (Exception ex)
            { }
        }

        //code to convert binary encrypted data to text.....
        void ReadBinaryBtn_Click(object sender, RoutedEventArgs arg)
        {
            try
            {
                System.Windows.Forms.OpenFileDialog readLogFile = new System.Windows.Forms.OpenFileDialog();
                if (readLogFile.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    string filepath = readLogFile.FileName;
                    FileStream fs = new FileStream(filepath, FileMode.Open, FileAccess.Read);
                    BinaryReader filebinReader = new BinaryReader(fs, new UnicodeEncoding());
                    byte[] memoryData = new byte[fs.Length - fs.Position];
                    for (int i = 0; i < memoryData.Length; i++)
                    {
                        memoryData[i] = (byte)(255 - filebinReader.ReadByte());
                    }
                    System.Text.UnicodeEncoding ecode = new UnicodeEncoding();
                    char[] finalData = ecode.GetChars(memoryData);
                    fs.Close();

                    System.Windows.Forms.SaveFileDialog writeLogfile = new System.Windows.Forms.SaveFileDialog();
                    if (writeLogfile.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                    {
                        string saveFilePath = writeLogfile.FileName + ".txt";
                        StreamWriter Swriter = new StreamWriter(saveFilePath, false);
                        Swriter.Write(finalData);
                        Swriter.Close();
                    }
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW42b", ex); }
        }

        private void Btn_CrossHair_Offset_Click(object sender, RoutedEventArgs e)
        {
            Cross_HairAligner cha = new Cross_HairAligner();
            cha.Left = GlobalSettings.MainWin.MainLayout.ColumnDefinitions[0].ActualWidth + 8;
            cha.Top = 132; GlobalSettings.MainWin.SetOwnerofWindow(cha); cha.Show();
            Settings_DataB.FinalizeClass();
            this.Close();
        }


        #endregion

        #region CheckBox Selectionchange

        protected void OnCheckBoxClicked(object sender, RoutedEventArgs e)
        {
            try
            {
                CheckBox Selectedcheckbox = (CheckBox)sender;
                bool CheckedStatus = (bool)Selectedcheckbox.IsChecked;
                if (Selectedcheckbox == RetractProbePointModeChkBx)
                {
                    if (CheckedStatus)
                    {
                        ProbeApproachDistance = Convert.ToDouble(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeOffsetValue"]);
                        FillTextBox(ProbeOffsetValTxtBx, "ProbeSettings", "MachineNo", MachineNo, "ProbeOffsetValue", true, true);
                        ProbeApproachSpeed = Convert.ToDouble(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeSpeed"]);
                        FillTextBox(ProbeSpeedTxtBx, "ProbeSettings", "MachineNo", MachineNo, "ProbeSpeed", true, true);
                        ProbeRetractSpeed = Convert.ToDouble(Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["ProbeRevertSpeed"]);
                        FillTextBox(ProbeRetractSpeedTxtBx, "EEPROMSettings", "MachineNo", MachineNo, "ProbeRevertSpeed", true, true);
                        ProbeRetractDistance = Convert.ToDouble(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeOffsetValueOnwards"]);
                        FillTextBox(ProbeMoveBackValTxtBx, "ProbeSettings", "MachineNo", MachineNo, "ProbeOffsetValueOnwards", true, true);
                        ProbeSensitivity = Convert.ToDouble(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeSensitivity"]);
                        FillTextBox(ProbeSensitivityTxtBx, "ProbeSettings", "MachineNo", MachineNo, "ProbeSensitivity", true, true);
                    }
                    else
                    {
                        ProbeApproachDistance = Convert.ToDouble(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeOffsetValue"]);
                        FillTextBox(ProbeOffsetValTxtBx, "ProbeSettings", "MachineNo", MachineNo, "ProbeOffsetValue", true, true);
                        ProbeApproachSpeed = Convert.ToDouble(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeSpeedOnwards"]);
                        FillTextBox(ProbeSpeedTxtBx, "ProbeSettings", "MachineNo", MachineNo, "ProbeSpeedOnwards", true, true);
                        ProbeRetractSpeed = Convert.ToDouble(Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["ProbeRevertSpeedOnwards"]);
                        FillTextBox(ProbeRetractSpeedTxtBx, "EEPROMSettings", "MachineNo", MachineNo, "ProbeRevertSpeedOnwards", true, true);
                        ProbeRetractDistance = Convert.ToDouble(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeOffsetValueOnwards"]);
                        FillTextBox(ProbeMoveBackValTxtBx, "ProbeSettings", "MachineNo", MachineNo, "ProbeOffsetValueOnwards", true, true);
                        ProbeSensitivity = Convert.ToDouble(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeSensitivityOnwards"]);
                        FillTextBox(ProbeSensitivityTxtBx, "ProbeSettings", "MachineNo", MachineNo, "ProbeSensitivityOnwards", true, true);
                    }
                }
            }
            catch (Exception)
            {

                throw;
            }
        }
        #endregion

        bool AddOrEditUsers()
        {
            try
            {
                //Now check that username already presents or not
                if (UserNameCBx.Text != "" && UserNameCBx.Text.Trim() != "")
                {
                    if (PasswordTxtBx.Text == RetypePasswordTxtBx.Text)
                    {
                        System.Data.DataRow dr;
                        if (UserNameCBx.Items.Contains(UserNameCBx.Text))
                        {
                            dr = Settings_DataB.Select_ChildControls("Login", "Login_Name", UserNameCBx.Text)[0];
                        }
                        else
                        {
                            dr = Settings_DataB.DataS.Tables["Login"].NewRow();
                            dr["Login_Name"] = UserNameCBx.Text;
                            UserNameCBx.Items.Add(UserNameCBx.Text);
                        }
                        dr["Login_Password"] = PasswordTxtBx.Text;
                        dr["AllowPartProgramBuilding"] = AllowUserBuildPPChkBx.IsChecked;
                        dr["AllowPartProgramRunning"] = AllowUserRunPPChkBx.IsChecked;
                        dr["AllowUserLogin"] = chkAllowLogin.IsChecked;
                        dr["AllowLightingSettingsAccess"] = chkLightingAccess.IsChecked;
                        //if ((bool)chkAdminAccess.IsChecked)
                        //    dr["Login_Priviledge"] = "Admin";
                        //else
                        dr["Login_Priviledge"] = "User";
                        Settings_DataB.DataS.Tables["Login"].Rows.Add(dr);
                        return true;
                    }
                }
                return false;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW43", ex);
                return false;
            }
        }
        void ShowHidePixelWidthOfYDir(bool Visible)
        {
            try
            {
                TextBox[] YDirTxtBxs = { MagLevel3XTxtBx_YDir, MagLevel5XTxtBx_YDir, MagLevel11XTxtBx_YDir, MagLevel22XTxtBx_YDir, MagLevel34XTxtBx_YDir, MagLevel54XTxtBx_YDir,
                                   MagLevel67XTxtBx_YDir, MagLevel134XTxtBx_YDir, MagLevel335XTxtBx_YDir};
                System.Windows.Visibility CtrlVisibility = System.Windows.Visibility.Hidden;
                if (Visible)
                    CtrlVisibility = System.Windows.Visibility.Visible;

                for (int i = 0; i < YDirTxtBxs.Length; i++)
                    YDirTxtBxs[i].Visibility = CtrlVisibility;
                PixelWidthYDirLbl.Visibility = CtrlVisibility;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW44", ex); }
        }
        void EnableDisableMagLevelHiddenChkBx(string MagLevelStr)
        {
            try
            {
                CheckBox[] MagHideChkBxs = { MagLevel3XHiddenChkBx, MagLevel5XHiddenChkBx, MagLevel11XHiddenChkBx, MagLevel22XHiddenChkBx, MagLevel34XHiddenChkBx,
                                           MagLevel54XHiddenChkBx, MagLevel67XHiddenChkBx, MagLevel134XHiddenChkBx, MagLevel335XHiddenChkBx };
                List<string> MagNames = new List<string> { "3X", "5X", "11X", "22X", "34X", "54X", "67X", "134X", "335X" };

                for (int i = 0; i < MagHideChkBxs.Length; i++)
                    MagHideChkBxs[i].IsEnabled = true;
                MagHideChkBxs[MagNames.IndexOf(MagLevelStr)].IsEnabled = false;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW45", ex); }
        }
        void CancelBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Settings_DataB.FinalizeClass();
                this.Close();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW46", ex); }
        }
        void DxfAutoAlignPropDG_PreparingCellForEdit(object sender, DataGridPreparingCellForEditEventArgs e)
        {
            ((TextBox)e.EditingElement).SelectAll();
        }
        void DxfAutoAlignPropDG_CellEditEnding(object sender, DataGridCellEditEndingEventArgs e)
        {
            string EnteredStr = ((TextBox)e.EditingElement).Text;
            DxfAutoAlignPropEntity MEntity = e.Row.Item as DxfAutoAlignPropEntity;

            double EnteredVal;
            if (!double.TryParse(EnteredStr, out EnteredVal))
                ((TextBox)e.EditingElement).Text = "0";
        }

        //code to update database with default value if value is null
        void CleanDatabase()
        {
            try
            {
                for (int i = 0; i < CurrentMachineDataR.Length; i++)
                {
                    DataRow dtrow = GetDefaultRow(Settings_DataB.DataS, UpdateTableNames[i]);
                    for (int k = 0; k < CurrentMachineDataR[i].ItemArray.Length; k++)
                    {
                        if (CurrentMachineDataR[i].ItemArray[k].ToString() == "")
                        {
                            if (dtrow.ItemArray[k].ToString() != "")
                                CurrentMachineDataR[i][k] = dtrow.ItemArray[k];
                        }
                    }
                    try
                    {                                                                                                //1 Written by Harphool
                        Settings_DataB.Update_Table(UpdateTableNames[i]);
                    }
                    catch (Exception Ex)
                    { MessageBox.Show(Ex.Message); }                                              //2
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW42c", ex);
            }
        }
        DataRow GetDefaultRow(DataSet RapidSettingDataSet, string tableName)
        {
            DataRow dtrow = RapidSettingDataSet.Tables[tableName].NewRow();
            try
            {
                System.Collections.Hashtable Temp_ListString = new System.Collections.Hashtable();
                System.Collections.Hashtable Temp_DataType = new System.Collections.Hashtable();
                System.Data.OleDb.OleDbConnection dbconnection = new System.Data.OleDb.OleDbConnection(RWrapper.RW_MainInterface.DBConnectionString);
                dbconnection.Open();
                DataTable dtTable = dbconnection.GetOleDbSchemaTable(System.Data.OleDb.OleDbSchemaGuid.Columns, new object[] { null, null, tableName, null });
                dbconnection.Close();
                for (int i = 0; i < dtTable.Rows.Count; i++)
                {
                    Temp_DataType.Add(dtTable.Rows[i].ItemArray[3].ToString(), dtTable.Rows[i].ItemArray[11].ToString());
                    Temp_ListString.Add(dtTable.Rows[i].ItemArray[3].ToString(), dtTable.Rows[i].ItemArray[8].ToString());
                }
                //DataType...[7,DateTime].. [11,boolean]...
                for (int i = 0; i < RapidSettingDataSet.Tables[tableName].Columns.Count; i++)
                {
                    if (Temp_ListString[RapidSettingDataSet.Tables[tableName].Columns[i].ColumnName.ToString()].ToString() != "")
                    {
                        string DataStr = Temp_DataType[RapidSettingDataSet.Tables[tableName].Columns[i].ColumnName.ToString()].ToString();
                        if (DataStr == "11")
                        {
                            bool Result = false;
                            dtrow[i] = bool.TryParse(Temp_ListString[RapidSettingDataSet.Tables[tableName].Columns[i].ColumnName.ToString()].ToString(), out Result);
                        }
                        else
                            dtrow[i] = Temp_ListString[RapidSettingDataSet.Tables[tableName].Columns[i].ColumnName.ToString()];
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SETW43a", ex);
            }
            return dtrow;
        }

        Color InterpretColour(int rgbVal)
        {
            Color MyColor = new Color();
            int r_Mask = 255 << 16;
            int g_Mask = 255 << 8;
            int b_Mask = 255;
            MyColor.R = (byte)((rgbVal & r_Mask) >> 16);
            MyColor.G = (byte)((rgbVal & g_Mask) >> 8);
            MyColor.B = (byte)(rgbVal & b_Mask);
            MyColor.A = 255;
            //MyColor.ColorContext = new ColorContext(
            return MyColor;
        }
        int InterpretValFromColour(Color ClrVal)
        {
            return ((int)ClrVal.R << 16) + ((int)ClrVal.G << 8) + (ClrVal.B);
        }

        public object PixelWidthCorrectionParameterLXTxtBx { get; set; }

        private void Btn_ProgramLocator_Click(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.FolderBrowserDialog fbd = new System.Windows.Forms.FolderBrowserDialog();
            fbd.ShowDialog();
            TextBlock tb = tblock_ProgramLocation;
            Button btn = (Button)sender;
            if (btn.Name.Contains("Report"))
                tb = tblock_ReportLocation;
            tb.Text = fbd.SelectedPath;
            if (!tb.Text.EndsWith(Convert.ToString(Microsoft.VisualBasic.Strings.Chr(92))))
                tb.Text += Microsoft.VisualBasic.Strings.Chr(92);
            if (!(bool)chkDecoupleReportPath.IsChecked)
                tblock_ReportLocation.Text = tblock_ProgramLocation.Text;
        }

        private void EEpromUpdateResult_TextChanged(object sender, TextChangedEventArgs e)
        {
            TextBox tb = (TextBox)sender;
            string inputstring = tb.Text;
            if (inputstring.Contains("Successfully"))
                tb.Background = Brushes.PaleGreen;
            else if (inputstring.Contains("Error"))
                tb.Background = Brushes.Tomato;
            else
                tb.Background = Brushes.LightGray;
        }
    }


    public class DxfAutoAlignPropEntity
    {
        public string DistStr
        {
            get
            {
                return DistVal.ToString();
            }
            set
            {
                DistVal = double.Parse(value);
            }
        }
        public string Offset_XStr
        {
            get
            {
                return Offset_XVal.ToString();
            }
            set
            {
                Offset_XVal = double.Parse(value);
            }
        }
        public string Offset_YStr
        {
            get
            {
                return Offset_YVal.ToString();
            }
            set
            {
                Offset_YVal = double.Parse(value);
            }
        }
        public double DistVal, Offset_XVal, Offset_YVal;
    }

    public class FeatureLicenseInfoEntity
    {
        public string FeatureName
        {
            get
            {
                return _FeatureName;
            }
            set
            {
                _FeatureName = value;
            }
        }
        public string LicenseType
        {
            get
            {
                return _LicenseType;
            }
            set
            {
                _LicenseType = value;
            }
        }
        public string DaysRemaining
        {
            get
            {
                return _DaysRemaining;
            }
            set
            {
                _DaysRemaining = value;
            }
        }
        private string _FeatureName, _LicenseType, _DaysRemaining;
    }
}

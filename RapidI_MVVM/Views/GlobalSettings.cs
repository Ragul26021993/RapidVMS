using System;
using System.Windows;
using System.Windows.Input;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Rapid.Windows;
using Rapid.Panels;
using RapidI.Help;
using rrm;

namespace Rapid
{
    class GlobalSettings
    {
        public enum MachineType
        {
            FullCNC,
            AutoFocus,
            Horizontal,
            Rotary_Del_FI,
            Rotary_Bosch_FI,
            One_Shot,
            Normal_As_One_Shot,
            DSP,
            Magnifier_DSP,
            Horizontal_DSP,
            TIS,
            ProbeOnly,
            HeightGauge,
            ContourTracer,
            HobChecker,
            OnlineSystem,
            VisionHeadOnly
        };
        public static MachineType RapidMachineType;

        public enum CameraType
        {
            TwoM,
            ThreeM,
            TenM,
            FiveFourtyP,
            SevenTwentyP,
            ThreeSixtyP,
            TwoM_Double,
            TenEightyP
        };
        public static CameraType RapidCameraType;

        public enum UserPrevilageLevel
        {
            System,
            Admin,
            AdminUser,
            User,
            None
        };

        public enum OnlineRunningState
        {
            InProcess = 0,
            Pass = 1,
            Fail = 2
        };

        public static UserPrevilageLevel CurrentUserLevel;

        public enum UnitType
        {
            mm,
            Inch
        };
        public static UnitType CurrentUnitType;

        public enum AngleMode
        {
            Degree_Minute_Second,
            Decimal_Degree,
            Radians
        };
        public static AngleMode CurrentAngleMode;

        public enum ProbeType
        {
            Normal_Probe,
            TProbe_AlongX,
            TProbe_AlongY,
            Star_Probe,
            Generic_Probe
        };
        public static ProbeType CurrentProbeType;

        public static string CurrentGenericProbeType;

        //public enum DroPrecision
        //{
        //    SingleDigit,
        //    DoubleDigit,
        //    ThreeDigit,
        //    FourDigit,
        //    FiveDigit,              //vinod decimal precision
        //    SixDigit,
        //    SevenDigit,
        //    EightDigit
        //};
        public static int CurrentDroPrecision;
        public static int CurrentDroPrecision_Ori;

        public enum EdgeDetectionAlgorithmType
        {
            Type1,
            Type2,
            Type3,
            Type4,
            Type5,
            Type6
        };
        public static EdgeDetectionAlgorithmType CurrentEdgeDetectionAlgorithm = EdgeDetectionAlgorithmType.Type6;

        public enum MCSTYPE
        {
            XY,
            YZ,
            XZ,
            IYZ,
            IXZ
        };
        public static MCSTYPE CurrentMSCType = MCSTYPE.XY;

        public static MainView MainWin;
        public static CNCPanel CNCModule;
        public static SAOI SaoiModule;
        public static VBlockHolder HolderModule;
        public static DBReader SettingsReader;
        public static ToleranceEnteringWin NomTolWin;
        public static FGandCldPtSizeWin PtSizeWin;
        //public static PPLightControl PPLightControlWin; 

        public static Style ToggleButtonStyle;
        public static Style NormalButtonStyle;
        public static Style RadioButtonStyle;
        public static Style ScrBar_ToggleButtonStyle;
        public static Style ScrBar_NormalButtonStyle;
        public static Style ScrBar_RadioButtonStyle;
        public static double BtnIcoStdWidth = 32, BtnIcoStdHeight = 32, ToolbarBtnStdWidth = 40, ToolbarBtnStdHeight = 40;
        public static Form1 MainExcelInstance;
        public static LoadFrm LoadExcelInstance;

        public static List<string> PartProgramPaths = new List<string>();
        public static List<string> AllExistingPartProgramNames = new List<string>();

        public static System.Windows.Controls.ComboBox FixtureNameCBx;
        public static List<string> FixtureNameList = new List<string>();
        public static List<string> FixRowNum, FixColNum, FixRowGap, FixColGap;
        public static bool LocalisedCorrectionOn;
        public static bool DoubleImageEnabled_During_DigiZoom;
        public static bool AllowFocusRawValues;
        public static bool InchModeAsDefault;
        public static bool MultilevelZoomMahine;
        public static bool PixelWidthCorrectionEnable;
        public static bool CompletedReadingAllDatafromDB;

        public static double[] DROSpeeds = new double[4];
        public static List<double> DxfAutoAlignDistVals = new List<double>();
        public static List<double> DxfAutoAlignOffset_XVals = new List<double>();
        public static List<double> DxfAutoAlignOffset_YVals = new List<double>();
        public static double SpeedStepVal, SpeedMinVal, SpeedMaxVal;

        public static int NumberOfAxes { get; set; }
        public static int FontSelectedIndx = 0;

        public static double ProbebackDist, ProbeBackSpeed, PRetractDistance, PApproachDistance, PApproachSpeed, PSensitivityValue, ProbeBackSpeedOnwards, PApproachDistanceOnwards, PApproachSpeedOnwards, PSensitivityValueOnwards;
        public static bool RetractProbePt_Enabled, AllowInterpolation, SmoothCircularInterpolation, AllowCNC, AllowAutoZoom, RouteProbePath, RetractProbePointMode;
        public static bool ProbeActivation_X, ProbeActivation_Y, ProbeActivation_Z, ProbeActivation_R, SetProbeRetractMode;

        public static bool IsRAxisPresent { get; set; }
        public static bool AuxillaryLightPresent { get; set; }
        public static bool CoAxialLightPresent { get; set; }
        public static bool LockUIDuringStartup { get; set; }
        public static bool Is3DSoftware { get; set; }
        public static bool IsButtonWithNames { get; set; }

        public static bool XLExportRowWise { get; set; }
        public static bool XLExportPortraitLayout { get; set; }
        public static bool XLExportRCadImgWithData { get; set; }
        public static int XLDataTransferTolMode { get; set; }
        public static int XLExportImg1DuringPP { get; set; }
        public static int XLExportImg2DuringPP { get; set; }

        public static string CompanyName { get; set; }
        public static string CompanyLocation { get; set; }
        public static string SoftwareVersion { get; set; }
        public static string Language { get; set; }
        public static string Release_Date { get; set; }
        public static string MachineNo { get; set; }
        //public static string RapidCamType { get; set; }

        public static string ProgramPath { get; set; }
        public static string ReportPath { get; set; }
        public static string TemplatePath { get; set; }
        public static string ShapePointsPath { get; set; }
        public static string ImageGridSavePath { get; set; }

        public static List<string> MagList = new List<string>();
        public static List<bool> DROIsLinearList = new List<bool>();
        public static List<string> LightingConfigNameList = new List<string>();
        public static string DefaultMagLevel { get; set; }
        public static int[] Light_Zone_Nos;
        public static int[] TheLightArrayfromdatabase; //Created to fix the Lighting. Sebestian !

        public static string ExcelDocConnString = "Provider=Microsoft.Ace.OLEDB.12.0;Data Source=" + System.Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + @"\Rapid-I 5.0\Database\ExcelDoc.mdb;Persist Security Info=False;Jet OLEDB:Database Password=excelforctpl";
        public static Hashtable WindowCursors;
        public static Hashtable FormsCursors;

        public static Hashtable FeatureLicenseInfo;

        public static bool PartProgBuildGenerateCSV;
        public static bool SaveHomePosition = true;
        public static bool ShowAveragingInExcel = false;
        public static bool InsertImageForDataTransfer = false;
        public static string XLPassword = "";
        public static bool MakePDFReport = false;
        public static bool PaginateReports = false;
        public static bool MultiReportsContinuousMode = false;
        public static bool AllProgramInSingleReport = false;
        

        #region Runtime Values
        public static bool PartProgramLoaded = false;
        #endregion

        public static int LimitRowColumProduct = 10000;
        public static int is_startup_rapidi = 0;        //vinod used for first time not set value 4-5 precision in dro for msrmnt mode, first time it should take from db

        #region Critical Shape Related Parameters
        public static bool SetCriticalShapes = false;
        public static double AngleTolerance = 0;
        public static double PercentageTolOnRadius = 0;
        #endregion

        public static string MeasurementColorMode = "Gradient";
        public static bool SwitchOffSnapModeDuringPP = false;
        public static bool RunAllPPinCNCModeByDefault = false;
        public static string SaveWindowsParameterFolderPath = "";

        public static bool CloseOpenSurface = false;
        public static double ZeroingAngle = 0;
        public static bool DelfiRotaryZeroingFloatingWindowView = true;
        public static bool ShapeMeasurementHidden = true;
        public static double[] OneShotPixelWidthCorrections;
        public static bool EnableBD;

        //public static double ValueofBCX;
        //public static double ValueofBCY;

        //public static bool VideoWindowInitialised; //If Main Interface successfully initialises video via RapidCam and we have cameraconnected....
        public static byte[] StaticImageinVideo;

        public static int LightZonesForThisController;
        //public static bool Do_Homing_At_Startup = false;

        #region Pixel Width Correction
        //public static double PixelWidthCorrectionParameterQ;
        //public static double PixelWidthCorrectionParameterLX1;
        //public static double PixelWidthCorrectionParameterLY;
        //public static double PixelWidthX0;
        //public static double PixelWidthY0;
        #endregion

        public static int VideoHeightThresholdForFrontEndLayout;

        public static List<OnlineRunningState> OnlineSystemStates;

        public static int Ch_12_Value, Ch_34_Value;
        public static string DefaultSurfaceLightSettings, DefaultProfileLightSettings, RefDotLightSettings;

        public static bool RotaryAxisForMeasurement;
        public static bool CNC_Assisted_FG;
    }
}
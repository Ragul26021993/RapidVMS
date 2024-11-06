using System;
using System.Reflection;
using System.Drawing;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media.Imaging;
using System.Media;
using CalibrationModule;
using RWrapper;

namespace Rapid
{
    class DBReader
    {
        DB Settings_DataB;
        string MachineNo;

        public List<System.Windows.Controls.Image> ButtonImageList;
        public List<string> ButtonBaseNameList, ButtonToolTipList;
        public List<int> ButtontypeList, ButtonId, ButtonParentId, ButtonFeatureId;
        public List<bool> ButtonCheckedPropertyList, ButttonHasClick;
        public List<bool> ButtonRevertToInitialPropertyList, RerelyUsedButtonsList, LastCheckedState, RememberLastState;
        public Hashtable StringList;

        public System.Windows.Controls.Image ButtonImage;
        public string ButtonBaseName, ButtonToolTip;
        public int ButtonType;
        public bool ButtonCheckedProperty, ButtonRevertToInitialProperty;
        public int StageFinished;

        public DBReader()
        {
            init();
        }
        void init()
        {
            try
            {
                StageFinished = 0;
                ButtonImageList = new List<System.Windows.Controls.Image>();
                ButtonBaseNameList = new List<string>();
                ButtonToolTipList = new List<string>();
                ButtonCheckedPropertyList = new List<bool>();
                ButtonRevertToInitialPropertyList = new List<bool>();
                ButtontypeList = new List<int>();
                ButtonId = new List<int>();
                ButtonParentId = new List<int>();
                ButtonFeatureId = new List<int>();
                ButttonHasClick = new List<bool>();
                RerelyUsedButtonsList = new List<bool>();
                LastCheckedState = new List<bool>();
                RememberLastState = new List<bool>();
                GlobalSettings.OneShotPixelWidthCorrections = new double[12];

                StringList = new Hashtable();
                GlobalSettings.FeatureLicenseInfo = new Hashtable();

                GlobalSettings.CompletedReadingAllDatafromDB = false;
                StageFinished = 1;

                string ConnStr;
                if (System.Environment.Is64BitOperatingSystem)
                    ConnStr = "Provider=Microsoft.Ace.OLEDB.12.0;OLE DB Services = -1; Data Source=" + Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Database\\RapidSettings.mdb;Persist Security Info=False;Jet OLEDB:Database Password=cTpL_4577ri"; // ; OLEDB Services = -1";
                else
                    ConnStr = "Provider=Microsoft.Jet.OLEDB.4.0; Data Source=" + Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Database\\RapidSettings.mdb;Persist Security Info=False;Jet OLEDB:Database Password=cTpL_4577ri"; // ; OLEDB Services = -1";

                Settings_DataB = new DB(ConnStr);
                Settings_DataB.FillTable("CurrentMachine");
                Settings_DataB.FillTable("UserSettings");
                Settings_DataB.FillTable("MachineDetails");
                Settings_DataB.FillTable("MagLevels");
                Settings_DataB.FillTable("LightingSetting");
                Settings_DataB.FillTable("ProbeSettings");
                Settings_DataB.FillTable("EEPROMSettings");
                Settings_DataB.FillTable("FeatureLicenseInfo");
                Settings_DataB.FillTable("DROSettings");
                Settings_DataB.FillTable("OneShotParams");
                Settings_DataB.FillTable("ButtonDisplayDetails");
                Settings_DataB.FillTable("LightingSettings");
                Settings_DataB.FillTable("DefaultCamSettings");
                Settings_DataB.FillTable("SurfaceFGSettings");
                Settings_DataB.FillTable("VisionSettings");
                Settings_DataB.FillTable("ToolStripButton");

                StageFinished = 2;

                //int Ctt = Settings_DataB.GetRowCount("CurrentMachine");
                System.Data.DataTable currMachineTable = Settings_DataB.DataS.Tables["CurrentMachine"];
                System.Data.DataRow[] drrs = currMachineTable.Select("C_Id > 0", "C_ID ASC");
                MachineNo = drrs[drrs.Length - 1]["MachineNo"].ToString();
                //MachineNo = Settings_DataB.get_GetRecord("CurrentMachine", Settings_DataB.GetRowCount("CurrentMachine") - 1, "MachineNo").ToString();
                GlobalSettings.MachineNo = MachineNo;
                if (Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo).Length <= 0)
                {
                    MachineNo = "0";
                    GlobalSettings.MachineNo = "0";
                }
                StageFinished = 3;

                //Check if we have columns in our database. if not, add them. 
                this.Add_NewField("LightingSettings", "ProfileLightLevelFor11X", typeof(System.Int32), 1);
                StageFinished = 33;

                if (System.Environment.Is64BitOperatingSystem)
                    this.Add_NewField("DefaultCamSettings", "RefDotCamSettings", typeof(System.String), "18C800608000608060604C");
                else
                    this.Add_NewField("DefaultCamSettings", "RefDotCamSettings", typeof(System.String), "58FF008001004C");

                //if (!Settings_DataB.GetTable("LightingSettings").Columns.Contains("ProfileLightLevelFor11X"))
                //{
                //    Settings_DataB.AddColumnToTable("LightingSettings", "ProfileLightLevelFor11X", typeof(System.Int32));
                //    Settings_DataB.FillTable("LightingSettings");
                //    System.Data.DataTable thisTable = Settings_DataB.GetTable("LightingSettings");
                //    foreach (System.Data.DataRow row in thisTable.Rows)
                //        row["ProfileLightLevelFor11X"] = 1;

                //    //System.Data.DataRow[] rows = Settings_DataB.Select_ChildControls("LightingSettings", "MachineNo", MachineNo);
                //    //for (int ii = 0; ii < rows.Length; ii++)
                //    //    rows[ii]["ProfileLightLevelFor11X"] = 1;
                //    Settings_DataB.Update_Table("LightingSettings");
                //    Settings_DataB.FillTable("LightingSettings");
                //}
                GlobalSettings.DROSpeeds[0] = Convert.ToDouble(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["SpeedX"]);
                GlobalSettings.DROSpeeds[1] = Convert.ToDouble(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["SpeedY"]);
                GlobalSettings.DROSpeeds[2] = Convert.ToDouble(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["SpeedZ"]);
                GlobalSettings.DROSpeeds[3] = Convert.ToDouble(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["SpeedR"]);
                GlobalSettings.SpeedMaxVal = Convert.ToDouble(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["MaximumValue"]);
                GlobalSettings.SpeedMinVal = Convert.ToDouble(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["MinimumValue"]);
                GlobalSettings.SpeedStepVal = Convert.ToDouble(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["StepValue"]);
                GlobalSettings.DelfiRotaryZeroingFloatingWindowView = Convert.ToBoolean(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["DelfiFloatingView"]);
                GlobalSettings.InchModeAsDefault = Convert.ToBoolean(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["DefaultInchMode"]);
                GlobalSettings.PartProgBuildGenerateCSV = Convert.ToBoolean(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["EnableCsvExport"]);
                GlobalSettings.SaveHomePosition = Convert.ToBoolean(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["SaveHomePosition"]);
                GlobalSettings.SetCriticalShapes = Convert.ToBoolean(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["SetCriticalShape"]);
                GlobalSettings.AngleTolerance = Convert.ToDouble(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["AngleToleranceForCriticalShape"]);
                GlobalSettings.PercentageTolOnRadius = Convert.ToDouble(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["PercentToleranceOnRadiusForCriticalShape"]);
                GlobalSettings.FontSelectedIndx = Convert.ToInt32(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["GraphicsFontType"]);
                GlobalSettings.MeasurementColorMode = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["MeasurementGradientMode"].ToString();
                GlobalSettings.SwitchOffSnapModeDuringPP = Convert.ToBoolean(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["SwitchOffSnapModeDuringPP"]);
                GlobalSettings.RunAllPPinCNCModeByDefault = Convert.ToBoolean(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["RunAllPPinCNCModeByDefault"]);

                GlobalSettings.NumberOfAxes = Convert.ToInt16(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["NoOfAxes"]);
                GlobalSettings.AuxillaryLightPresent = Convert.ToBoolean(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["AuxiliaryLighting"]);
                GlobalSettings.CoAxialLightPresent = Convert.ToBoolean(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["CoAxialLighting"]);
                GlobalSettings.LockUIDuringStartup = Convert.ToBoolean(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["LockUIatStartup"]);
                GlobalSettings.Is3DSoftware = Convert.ToBoolean(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["ThreedSoftware"]);
                GlobalSettings.DoubleImageEnabled_During_DigiZoom = Convert.ToBoolean(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["DoubleImageEnabled"]);
                GlobalSettings.MainWin.AllowAxisLock = Convert.ToBoolean(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["AllowAxisLock"]);
                GlobalSettings.AllowFocusRawValues = Convert.ToBoolean(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["AllowFocusRawValues"]);
                GlobalSettings.RotaryAxisForMeasurement = Convert.ToBoolean(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["RotaryAxisForMeasurement"]);

                StageFinished = 4;

                //GlobalSettings.PixelWidthCorrectionEnable = Convert.ToBoolean(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["PixelWidthCorrection"]);
                //GlobalSettings.PixelWidthCorrectionParameterQ = Convert.ToDouble(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["PixelWidthCorrectionParameterQ"]);
                //GlobalSettings.PixelWidthCorrectionParameterLX1 = Convert.ToDouble(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["PixelWidthCorrectionParameterLX1"]);
                //GlobalSettings.PixelWidthCorrectionParameterLY = Convert.ToDouble(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["PixelWidthCorrectionParameterLY"]);
                //GlobalSettings.PixelWidthX0 = Convert.ToDouble(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["PixelWidthX0"]);
                //GlobalSettings.PixelWidthY0 = Convert.ToDouble(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["PixelWidthY0"]);
                System.Data.DataRow[] tempDataR;
                tempDataR = Settings_DataB.Select_ChildControls("OneShotParams", "MachineNo", MachineNo);
                if (tempDataR.Length > 0)
                {
                    for (int i = 0; i < 12; i++)
                    {
                        GlobalSettings.OneShotPixelWidthCorrections[i] = Convert.ToDouble(tempDataR[0][i + 3]);
                    }
                    GlobalSettings.PixelWidthCorrectionEnable = Convert.ToBoolean(Settings_DataB.Select_ChildControls("OneShotParams", "MachineNo", MachineNo)[0]["PixelWidthCorrection"]);
                    GlobalSettings.EnableBD = Convert.ToBoolean(Settings_DataB.Select_ChildControls("OneShotParams", "MachineNo", MachineNo)[0]["EnableBD"]);
                }
                GlobalSettings.ProbebackDist = Convert.ToDouble(Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["ProbeRevertDistance"]);
                GlobalSettings.ProbeBackSpeed = Convert.ToDouble(Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["ProbeRevertSpeed"]);
                GlobalSettings.ProbeBackSpeedOnwards = Convert.ToDouble(Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["ProbeRevertSpeedOnwards"]);
                GlobalSettings.AllowInterpolation = Convert.ToBoolean(Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["AllowInterpolation"]);
                GlobalSettings.SmoothCircularInterpolation = Convert.ToBoolean(Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["SmoothCircularInterpolation"]);

                GlobalSettings.AllowCNC = Convert.ToBoolean(Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["AllowCNC"]);
                GlobalSettings.AllowAutoZoom = Convert.ToBoolean(Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["AllowAutoZoom"]);
                GlobalSettings.RouteProbePath = Convert.ToBoolean(Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["RouteProbePath"]);
                GlobalSettings.ProbeActivation_X = Convert.ToBoolean(Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["ProbeActivation_X"]);
                GlobalSettings.ProbeActivation_Y = Convert.ToBoolean(Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["ProbeActivation_Y"]);
                GlobalSettings.ProbeActivation_Z = Convert.ToBoolean(Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["ProbeActivation_Z"]);
                GlobalSettings.ProbeActivation_R = Convert.ToBoolean(Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["ProbeActivation_R"]);
                GlobalSettings.RetractProbePointMode = Convert.ToBoolean(Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["RetractProbePointMode"]);
                GlobalSettings.SetProbeRetractMode = Convert.ToBoolean(Settings_DataB.Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]["SetProbeRetractMode"]);

                StageFinished = 5;

                GlobalSettings.PApproachDistance = Convert.ToDouble(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeOffsetValue"]);
                GlobalSettings.PApproachDistanceOnwards = Convert.ToDouble(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeOffsetValueOnwards"]);
                GlobalSettings.PApproachSpeed = Convert.ToDouble(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeSpeed"]);
                GlobalSettings.PApproachSpeedOnwards = Convert.ToDouble(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeSpeedOnwards"]);
                GlobalSettings.PRetractDistance = Convert.ToDouble(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeOffsetValueOnwards"]);
                GlobalSettings.PSensitivityValue = Convert.ToDouble(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeSensitivity"]);
                GlobalSettings.PSensitivityValueOnwards = Convert.ToDouble(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeSensitivityOnwards"]);
                GlobalSettings.RetractProbePt_Enabled = Convert.ToBoolean(Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeMoveUp"]);

                GlobalSettings.CompanyName = Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["CompanyName"].ToString();
                GlobalSettings.CompanyLocation = Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["Location"].ToString();
                //GlobalSettings.SoftwareVersion = Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["StartVersionNo"].ToString();

                GlobalSettings.SoftwareVersion = Settings_DataB.Select_ChildControls("CurrentMachine", "MachineNo", MachineNo)[0]["Version_number"].ToString();
                GlobalSettings.Release_Date = Settings_DataB.Select_ChildControls("CurrentMachine", "MachineNo", MachineNo)[0]["Release_Date"].ToString();
                GlobalSettings.Language = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["LanguageType"].ToString();

                GlobalSettings.XLExportRowWise = Convert.ToBoolean(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["XLExportRowWise"]);
                GlobalSettings.XLExportPortraitLayout = Convert.ToBoolean(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["XLExportPortraitLayout"]);
                GlobalSettings.XLExportRCadImgWithData = Convert.ToBoolean(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["XLExportRCadImage"]);
                GlobalSettings.ShapeMeasurementHidden = Convert.ToBoolean(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["HideShapesAndMeasurements"]);
                GlobalSettings.XLPassword = Convert.ToString(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["XLPassword"]);

                GlobalSettings.ShowAveragingInExcel = Convert.ToBoolean(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["ShowAveraging"]);
                GlobalSettings.InsertImageForDataTransfer = Convert.ToBoolean(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["InsertImage"]);
                GlobalSettings.CloseOpenSurface = Convert.ToBoolean(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["CloseOpenSurface"]);
                GlobalSettings.MultilevelZoomMahine = Convert.ToBoolean(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["MultiLevelZoomMachine"].ToString());
                GlobalSettings.MakePDFReport = Convert.ToBoolean(Settings_DataB.Select_ChildControls("VisionSettings", "MachineNo", MachineNo)[0]["MakePDFReport"]);
                GlobalSettings.PaginateReports = Convert.ToBoolean(Settings_DataB.Select_ChildControls("VisionSettings", "MachineNo", MachineNo)[0]["PaginateReports"]);
                GlobalSettings.MultiReportsContinuousMode = Convert.ToBoolean(Settings_DataB.Select_ChildControls("VisionSettings", "MachineNo", MachineNo)[0]["MultiReportsInContinuousMode"]);
                GlobalSettings.AllProgramInSingleReport = Convert.ToBoolean(Settings_DataB.Select_ChildControls("VisionSettings", "MachineNo", MachineNo)[0]["AllProgramInSingleReport"]);

                StageFinished = 6;

                string Img = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["XLExportImage1DuringPP"].ToString();
                Img = Img.Replace("\"", string.Empty).Trim();
                switch (Img)
                {
                    case "None":
                        GlobalSettings.XLExportImg1DuringPP = 0;
                        break;
                    case "RCad Image":
                        GlobalSettings.XLExportImg1DuringPP = 1;
                        break;
                    case "Video Image":
                        GlobalSettings.XLExportImg1DuringPP = 2;
                        break;
                    case "Video With Graphics":
                        GlobalSettings.XLExportImg1DuringPP = 3;
                        break;
                }
                Img = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["XLExportImage2DuringPP"].ToString();
                Img = Img.Replace("\"", string.Empty).Trim();
                switch (Img)
                {
                    case "None":
                        GlobalSettings.XLExportImg2DuringPP = 0;
                        break;
                    case "RCad Image":
                        GlobalSettings.XLExportImg2DuringPP = 1;
                        break;
                    case "Video Image":
                        GlobalSettings.XLExportImg2DuringPP = 2;
                        break;
                    case "Video With Graphics":
                        GlobalSettings.XLExportImg2DuringPP = 3;
                        break;
                }
                StageFinished = 7;

                for (int i = 0; i < Settings_DataB.GetTable("FeatureLicenseInfo").Rows.Count; i++)
                {
                    //GlobalSettings.FeatureLicenseInfo[i] = 
                    string LicenseType = Settings_DataB.GetTable("FeatureLicenseInfo").Rows[i]["LicenseType"].ToString();

                    if (LicenseType == "Limited")
                    {
                        TimeSpan ValidDays = new TimeSpan(Convert.ToInt16(Settings_DataB.GetTable("FeatureLicenseInfo").Rows[i]["NoOfDaysValid"]), 0, 0, 0);
                        DateTime ExpiryDate = Convert.ToDateTime(Settings_DataB.GetTable("FeatureLicenseInfo").Rows[i]["DateEntered"]) + ValidDays;
                        bool Active;
                        if (ExpiryDate < DateTime.Now)
                            Active = false;
                        else
                            Active = true;
                        GlobalSettings.FeatureLicenseInfo[i] = Active;
                    }
                    else if (LicenseType == "No")
                    {
                        GlobalSettings.FeatureLicenseInfo[i] = false;
                    }
                    else
                    {
                        GlobalSettings.FeatureLicenseInfo[i] = true;
                    }
                }
                StageFinished = 8;

                string TolMode = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["DataTransferToleranceFormat"].ToString();
                TolMode = TolMode.Replace("\"", string.Empty).Trim();
                int ToleranceMode;
                GlobalSettings.XLDataTransferTolMode = 0;
                if (int.TryParse(TolMode, out ToleranceMode))
                {
                    GlobalSettings.XLDataTransferTolMode = ToleranceMode;
                }
                else
                {
                    if (TolMode.Contains("AllowedRange"))
                        GlobalSettings.XLDataTransferTolMode = 1;
                    else if (TolMode.Contains("NomPlsMinus"))
                        GlobalSettings.XLDataTransferTolMode = 2;
                    else if (TolMode.Contains("ShowLSLorUSL"))
                        GlobalSettings.XLDataTransferTolMode = 3;
                }


                int MCSType = Convert.ToInt32(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["MCSType"].ToString());
                switch (MCSType)
                {
                    case 0:
                        GlobalSettings.CurrentMSCType = GlobalSettings.MCSTYPE.XY;
                        break;
                    case 1:
                        GlobalSettings.CurrentMSCType = GlobalSettings.MCSTYPE.YZ;
                        break;
                    case 2:
                        GlobalSettings.CurrentMSCType = GlobalSettings.MCSTYPE.XZ;
                        break;
                    case 3:
                        GlobalSettings.CurrentMSCType = GlobalSettings.MCSTYPE.IYZ;
                        break;
                    case 4:
                        GlobalSettings.CurrentMSCType = GlobalSettings.MCSTYPE.IXZ;
                        break;
                }

                int CurrentAlgo = Convert.ToInt32(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["SelectedAlgorithm"].ToString());
                switch (CurrentAlgo)
                {
                    case 0:
                        GlobalSettings.CurrentEdgeDetectionAlgorithm = GlobalSettings.EdgeDetectionAlgorithmType.Type1;
                        break;
                    case 1:
                        GlobalSettings.CurrentEdgeDetectionAlgorithm = GlobalSettings.EdgeDetectionAlgorithmType.Type2;
                        break;
                    case 2:
                        GlobalSettings.CurrentEdgeDetectionAlgorithm = GlobalSettings.EdgeDetectionAlgorithmType.Type3;
                        break;
                    case 3:
                        GlobalSettings.CurrentEdgeDetectionAlgorithm = GlobalSettings.EdgeDetectionAlgorithmType.Type4;
                        break;
                    case 4:
                        GlobalSettings.CurrentEdgeDetectionAlgorithm = GlobalSettings.EdgeDetectionAlgorithmType.Type5;
                        break;
                    case 5:
                        GlobalSettings.CurrentEdgeDetectionAlgorithm = GlobalSettings.EdgeDetectionAlgorithmType.Type6;
                        break;
                }

                StageFinished = 8;

                GlobalSettings.IsButtonWithNames = Convert.ToBoolean(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["IconsWithName"]);

                string PPDrive = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["SelectProgramDrive"].ToString();
                string PPReportLocation = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["SelectReportPath"].ToString();
                GlobalSettings.ProgramPath = PPDrive + "Programs";
                if (PPReportLocation == "" || PPReportLocation == PPDrive)
                    GlobalSettings.ReportPath = PPDrive + "Reports";
                else
                    GlobalSettings.ReportPath = PPReportLocation.Substring(0, PPReportLocation.Length - 1);

                GlobalSettings.TemplatePath = PPDrive + "Templates";
                GlobalSettings.ShapePointsPath = PPDrive + "Shape Points";

                //if(!Settings_DataB.GetTable("UserSettings").Columns.Contains("ImageGridSavePath"))
                //    this.Add_NewField("UserSettings", "ImageGridSavePath", typeof(System.String), PPDrive + "Images");

                //GlobalSettings.ImageGridSavePath = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["ImageGridSavePath"].ToString();
                //if (GlobalSettings.ImageGridSavePath == "")
                GlobalSettings.ImageGridSavePath = PPDrive + "Images";
                if (!System.IO.Directory.Exists(GlobalSettings.ImageGridSavePath))
                    System.IO.Directory.CreateDirectory(GlobalSettings.ImageGridSavePath);
                StageFinished = 9;

                bool RapidCam3Present = false;
                string CamType = Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["CameraType"].ToString();
                switch (CamType)
                {
                    case "2M":
                        GlobalSettings.RapidCameraType = GlobalSettings.CameraType.TwoM;
                        break;
                    case "3M":
                        GlobalSettings.RapidCameraType = GlobalSettings.CameraType.ThreeM;
                        break;
                    case "10M":
                        GlobalSettings.RapidCameraType = GlobalSettings.CameraType.TenM;
                        RapidCam3Present = true;
                        break;
                    case "540P":
                        GlobalSettings.RapidCameraType = GlobalSettings.CameraType.FiveFourtyP;
                        RapidCam3Present = true;
                        break;
                    case "720P":
                        GlobalSettings.RapidCameraType = GlobalSettings.CameraType.SevenTwentyP;
                        RapidCam3Present = true;
                        break;
                    case "360P":
                        GlobalSettings.RapidCameraType = GlobalSettings.CameraType.ThreeSixtyP;
                        RapidCam3Present = true;
                        break;
                    case "2M2":
                        GlobalSettings.RapidCameraType = GlobalSettings.CameraType.TwoM_Double;
                        RapidCam3Present = true;
                        break;
                    case "1080P":
                        GlobalSettings.RapidCameraType = GlobalSettings.CameraType.TenEightyP;
                        RapidCam3Present = true;
                        break;

                    default:
                        GlobalSettings.RapidCameraType = GlobalSettings.CameraType.TwoM;
                        break;
                }

                string McType = Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["MachineType"].ToString();
                switch (McType)
                {
                    case "FullCNC":
                        GlobalSettings.RapidMachineType = GlobalSettings.MachineType.FullCNC;
                        break;
                    case "AutoFocus":
                        GlobalSettings.RapidMachineType = GlobalSettings.MachineType.AutoFocus;
                        break;
                    case "Horizontal":
                        GlobalSettings.RapidMachineType = GlobalSettings.MachineType.Horizontal;
                        break;
                    case "Rotary Del_FI":
                        GlobalSettings.RapidMachineType = GlobalSettings.MachineType.Rotary_Del_FI;
                        break;
                    case "Rotary Bosch_FI":
                        GlobalSettings.RapidMachineType = GlobalSettings.MachineType.Rotary_Bosch_FI;
                        break;
                    case "One Shot":
                    case "One_Shot":
                        GlobalSettings.RapidMachineType = GlobalSettings.MachineType.One_Shot;
                        break;
                    case "Normal As One Shot":
                    case "Normal_As_One_Shot":
                        GlobalSettings.RapidMachineType = GlobalSettings.MachineType.Normal_As_One_Shot;
                        break;
                    case "DSP":
                        GlobalSettings.RapidMachineType = GlobalSettings.MachineType.DSP;
                        break;
                    case "Magnifier DSP":
                        GlobalSettings.RapidMachineType = GlobalSettings.MachineType.Magnifier_DSP;
                        break;
                    case "Horizontal DSP":
                        GlobalSettings.RapidMachineType = GlobalSettings.MachineType.Horizontal_DSP;
                        break;
                    case "TIS":
                        GlobalSettings.RapidMachineType = GlobalSettings.MachineType.TIS;
                        break;
                    case "ProbeOnly":
                        GlobalSettings.RapidMachineType = GlobalSettings.MachineType.ProbeOnly;
                        break;
                    case "HeightGauge":
                        GlobalSettings.RapidMachineType = GlobalSettings.MachineType.HeightGauge;
                        break;
                    case "Contour Tracer":
                        GlobalSettings.RapidMachineType = GlobalSettings.MachineType.ContourTracer;
                        break;
                    case "HobChecker":
                        GlobalSettings.RapidMachineType = GlobalSettings.MachineType.HobChecker;
                        break;
                    case "OnlineSystem":
                        GlobalSettings.RapidMachineType = GlobalSettings.MachineType.OnlineSystem;
                        break;
                    case "VisionHeadOnly":
                        GlobalSettings.RapidMachineType = GlobalSettings.MachineType.VisionHeadOnly;
                        break;
                }

                GlobalSettings.CurrentDroPrecision = Convert.ToInt32(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["MesureNoOfDec"]);
                GlobalSettings.CurrentDroPrecision_Ori = GlobalSettings.CurrentDroPrecision;

                string ProbeType = Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["ProbeType"].ToString();
                switch (ProbeType)
                {
                    case "NormalProbe":
                        GlobalSettings.CurrentProbeType = GlobalSettings.ProbeType.Normal_Probe;
                        break;
                    case "TProbeAlongX":
                        GlobalSettings.CurrentProbeType = GlobalSettings.ProbeType.TProbe_AlongX;
                        break;
                    case "TProbeAlongY":
                        GlobalSettings.CurrentProbeType = GlobalSettings.ProbeType.TProbe_AlongY;
                        break;
                    case "StarProbe":
                        GlobalSettings.CurrentProbeType = GlobalSettings.ProbeType.Star_Probe;
                        break;
                    case "Generic Probe":
                        GlobalSettings.CurrentProbeType = GlobalSettings.ProbeType.Generic_Probe;
                        break;
                }
                GlobalSettings.CurrentGenericProbeType = Settings_DataB.Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]["CurrentGenericProbeType"].ToString();

                StageFinished = 10;

                if (GlobalSettings.NumberOfAxes == 4 || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Bosch_FI || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
                    GlobalSettings.IsRAxisPresent = true;
                else
                    GlobalSettings.IsRAxisPresent = false;

                System.Data.DataRow[] MagRows = Settings_DataB.Select_ChildControls("MagLevels", "MachineNo", MachineNo);
                GlobalSettings.MagList.Clear();
                for (int i = 0; i < MagRows.Length; i++)
                    if (!Convert.ToBoolean(MagRows[i]["Hidden"]))
                        GlobalSettings.MagList.Add(MagRows[i]["MagLevelValue"].ToString());
                GlobalSettings.DefaultMagLevel = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["DefaultMagLevel"].ToString();

                System.Data.DataRow[] DRORows = Settings_DataB.Select_ChildControls("DROSettings", "MachineNo", MachineNo);
                for (int i = 0; i < DRORows.Length; i++)
                    GlobalSettings.DROIsLinearList.Add((bool)DRORows[i]["AxisType"]);

                int RowC = Settings_DataB.GetRowCount("LightingSetting");
                for (int i = 0; i < RowC; i++)
                    GlobalSettings.LightingConfigNameList.Add(Settings_DataB.get_GetRecord("LightingSetting", i, "LightName").ToString());

                GlobalSettings.LightZonesForThisController = Convert.ToInt32(Settings_DataB.Select_ChildControls("LightingSettings", "MachineNo", MachineNo)[0]["LightValue"]);
                //Created to take the new Values from the databse and thus use it to fix the Lighting of the Machine.
                //Created by: Sebestian

                GlobalSettings.TheLightArrayfromdatabase = new int[] { 0, 1, 2, 3, 4, 5, 6, 7 }; //Total Predefined Values 8
                //getting the zone1 
                GlobalSettings.TheLightArrayfromdatabase[0]= Convert.ToInt32(Settings_DataB.Select_ChildControls("LightingSettings", "MachineNo", MachineNo)[0]["Zone1_No"]);
                //getting the zone2
                GlobalSettings.TheLightArrayfromdatabase[1]= Convert.ToInt32(Settings_DataB.Select_ChildControls("LightingSettings", "MachineNo", MachineNo)[0]["Zone2_No"]);
                //getting the zone3
                GlobalSettings.TheLightArrayfromdatabase[2]= Convert.ToInt32(Settings_DataB.Select_ChildControls("LightingSettings", "MachineNo", MachineNo)[0]["Zone3_No"]);
                //getting the zone4
                GlobalSettings.TheLightArrayfromdatabase[3]= Convert.ToInt32(Settings_DataB.Select_ChildControls("LightingSettings", "MachineNo", MachineNo)[0]["Zone4_No"]);
                //Getting the aux
                GlobalSettings.TheLightArrayfromdatabase[4] = Convert.ToInt32(Settings_DataB.Select_ChildControls("LightingSettings", "MachineNo", MachineNo)[0]["Profile_Zone_No"]);
                //Getting the Profile
                GlobalSettings.TheLightArrayfromdatabase[5] = Convert.ToInt32(Settings_DataB.Select_ChildControls("LightingSettings", "MachineNo", MachineNo)[0]["Aux_Zone_No"]);
                //Coaxial Light
                GlobalSettings.TheLightArrayfromdatabase[6] = Convert.ToInt32(Settings_DataB.Select_ChildControls("LightingSettings", "MachineNo", MachineNo)[0]["CoAx_ZoneNo"]);
                // Profile Light which is 11 x or something.
                GlobalSettings.TheLightArrayfromdatabase[7] = Convert.ToInt32(Settings_DataB.Select_ChildControls("LightingSettings", "MachineNo", MachineNo)[0]["Profile11X_Zone_No"]);
                

                //Value packed.
                GlobalSettings.Light_Zone_Nos = new int[] { 4, 7, 5, 6 };
                try
                {
                    GlobalSettings.Light_Zone_Nos[0] = Convert.ToInt32(Settings_DataB.Select_ChildControls("LightingSettings", "MachineNo", MachineNo)[0]["Profile_Zone_No"]);
                    GlobalSettings.Light_Zone_Nos[1] = Convert.ToInt32(Settings_DataB.Select_ChildControls("LightingSettings", "MachineNo", MachineNo)[0]["Aux_Zone_No"]);
                    GlobalSettings.Light_Zone_Nos[2] = Convert.ToInt32(Settings_DataB.Select_ChildControls("LightingSettings", "MachineNo", MachineNo)[0]["CoAx_ZoneNo"]);
                    GlobalSettings.Light_Zone_Nos[3] = Convert.ToInt32(Settings_DataB.Select_ChildControls("LightingSettings", "MachineNo", MachineNo)[0]["Profile11X_Zone_No"]);
                }
                catch (Exception ex)
                {
                    RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError58", ex);
                }
                //System.Windows.Media.Color UpClr = InterpretColour(Convert.ToInt32(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["RcadGradientBackColourTop"].ToString()));
                //System.Windows.Media.Color DownClr = InterpretColour(Convert.ToInt32(Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["RcadGradientBackColourBottom"].ToString()));

                //RWrapper.RW_MainInterface.MYINSTANCE().SetOGLGradientBackGround(UpClr.R, UpClr.G, UpClr.B, DownClr.R, DownClr.G, DownClr.B);
                StageFinished = 11;

                ReadDxfAutoAlignmentVals();
                ReadCursors();
                ReadFormsCursors();

                FeatureLicenseUpdate();
                GlobalSettings.SaveWindowsParameterFolderPath = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Saved Parameters";
                GlobalSettings.Ch_12_Value = 136;
                GlobalSettings.Ch_34_Value = 136;
                if (Settings_DataB.Select_ChildControls("DefaultCamSettings", "MachineNo", MachineNo).Length > 0)
                {
                    GlobalSettings.DefaultSurfaceLightSettings = (Settings_DataB.Select_ChildControls("DefaultCamSettings", "MachineNo", MachineNo)[0]["SurfaceLightSettings"]).ToString();
                    GlobalSettings.DefaultProfileLightSettings = (Settings_DataB.Select_ChildControls("DefaultCamSettings", "MachineNo", MachineNo)[0]["ProfileLightSettings"]).ToString();
                    GlobalSettings.Ch_12_Value = Convert.ToInt32(Settings_DataB.Select_ChildControls("DefaultCamSettings", "MachineNo", MachineNo)[0]["GainSettings_12"]);
                    GlobalSettings.Ch_34_Value = Convert.ToInt32(Settings_DataB.Select_ChildControls("DefaultCamSettings", "MachineNo", MachineNo)[0]["GainSettings_34"]);
                }
                else
                {
                    //We do not have an entry for this machine number. Let us make a new entry
                    System.Data.DataTable defTab = Settings_DataB.GetTable("DefaultCamSettings");
                    System.Data.DataRow dr = defTab.NewRow();
                    System.Data.DataRow refrow;
                    if (RapidCam3Present)
                        refrow = Settings_DataB.Select_ChildControls("DefaultCamSettings", "MachineNo", "RC3")[0];
                    else
                        refrow = Settings_DataB.Select_ChildControls("DefaultCamSettings", "MachineNo", "RC0")[0];

                    GlobalSettings.DefaultSurfaceLightSettings = refrow["SurfaceLightSettings"].ToString();
                    GlobalSettings.DefaultProfileLightSettings = refrow["ProfileLightSettings"].ToString();
                    for (int i = 1; i < defTab.Columns.Count; i++)
                        dr[i] = refrow[i];
                    dr["MachineNo"] = MachineNo;
                    defTab.Rows.Add(dr);
                    Settings_DataB.Update_Table("DefaultCamSettings");
                }
                GlobalSettings.CNC_Assisted_FG = Convert.ToBoolean(Settings_DataB.Select_ChildControls("VisionSettings", "MachineNo", MachineNo)[0]["CNCAssisted_FG"]);

                StageFinished = 12;

                //Check for settings for cross-hair offsets for drawing in different graphics cards correctly.
                Settings_DataB.FillTable("CrossHairOffsets");
                if (Settings_DataB.Select_ChildControls("CrossHairOffsets", "MachineNo", MachineNo).Length == 0)
                {
                    //We do not have an entry. Let us make one, with default value zero (which the dB itself will do). We only need to make a new row.
                    System.Data.DataTable defTab = Settings_DataB.GetTable("CrossHairOffsets");
                    System.Data.DataRow dr = defTab.NewRow();
                    dr["MachineNo"] = MachineNo;
                    for (int ii = 0; ii < defTab.Columns.Count; ii++)
                        dr[ii] = 0;
                    dr["MachineNo"] = MachineNo;
                    defTab.Rows.Add(dr);
                    Settings_DataB.Update_Table("CrossHairOffsets");
                }
                //System.Data.DataTable ddtt = Settings_DataB.GetTable("ToolStripButton");
                //Settings_DataB.FinalizeClass();
                //Settings_DataB = null;
                GlobalSettings.CompletedReadingAllDatafromDB = true;
                StageFinished = 13;

            }
            catch (Exception ex)
            {
                //RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR01", ex);
            }
        }
        void ReadDxfAutoAlignmentVals()
        {
            try
            {
                //Read Dxf Auto Alignment values
                string FileName = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Database\\DxfAutoAlignVal.txt";
                if (File.Exists(FileName))
                {
                    StreamReader FileRdr = new StreamReader(FileName);
                    string ReadStr = FileRdr.ReadLine();
                    while (ReadStr != null)
                    {
                        if (ReadStr == "")
                        {
                            ReadStr = FileRdr.ReadLine();
                            continue;
                        }
                        string[] LineEntity = ReadStr.Split(',');
                        double[] LineEntityVals = new double[3] { 0, 0, 0 };
                        for (int j = 0; j < LineEntity.Length; j++)
                            double.TryParse(LineEntity[j], out LineEntityVals[j]);
                        GlobalSettings.DxfAutoAlignDistVals.Add(LineEntityVals[0]);
                        GlobalSettings.DxfAutoAlignOffset_XVals.Add(LineEntityVals[1]);
                        GlobalSettings.DxfAutoAlignOffset_YVals.Add(LineEntityVals[2]);
                        ReadStr = FileRdr.ReadLine();
                    }
                    FileRdr.Close();
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR01a", ex); }
        }
        void ReadCursors()
        {
            try
            {
                //Settings_DataB.FillTable("ToolStripButton");
                System.Data.DataRow[] CursorRows = Settings_DataB.Select_ChildControls("ToolStripButton", "Alignment", "VideoCursor");
                GlobalSettings.WindowCursors = new Hashtable();
                string[] CurNames = { "Cross", "ArcFG", "CircleFG", "AngRectFG", "RectFG", "Null", "Trimming", "PanModeHold", "PanModeRelease" };
                GlobalSettings.WindowCursors[0] = System.Windows.Input.Cursors.Arrow;
                for (int i = 0; i < CursorRows.Length; i++)
                {
                    for (int j = 0; j < CurNames.Length; j++)
                    {
                        if (CursorRows[i]["ButtonName"].ToString() == CurNames[j])
                        {
                            byte[] Source = (byte[])CursorRows[i]["ButtonIcon"];
                            System.IO.Stream IcoStream = new System.IO.MemoryStream(Source);
                            GlobalSettings.WindowCursors[j + 1] = new System.Windows.Input.Cursor(IcoStream);
                            break;
                        }
                    }
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR02", ex); }
        }
        void ReadFormsCursors()
        {
            try
            {
                GlobalSettings.FormsCursors = new Hashtable();
                GlobalSettings.FormsCursors[0] = System.Windows.Forms.Cursors.Arrow;
                //To create the cursor 
                //add the .ico or .cur file to project i.e, in the Sol Explorer
                //Go to the properties of that perticular file i,e by rightClicking on the perticular item in the solution explorer
                //Change the Build action Property to resource
                string[] cursorfilename = { "cross24.cur", "FG_Arc.cur", "FG_Circle.cur", "FG_Rect2.ico", "FGRect1.cur", "tran.cur", "Trimming.cur", "Hand-Grab-bed.ico", "Hand-Grab.ico" };

                System.Windows.Resources.StreamResourceInfo TmpResInfo;
                for (int i = 0; i < cursorfilename.Length; i++)
                {
                    //adds the cursor to the list
                    TmpResInfo = System.Windows.Application.GetResourceStream(new System.Uri("pack://application:,,,/Resources/Cursors/" + cursorfilename[i]));
                    GlobalSettings.FormsCursors[i + 1] = new System.Windows.Forms.Cursor(TmpResInfo.Stream);
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR02", ex); }
        }
        void FeatureLicenseUpdate()
        {
            if (GlobalSettings.CompanyName == "Customised Technologies (P) Ltd")
            {
                for (int i = 0; i < 11; i++)
                    GlobalSettings.FeatureLicenseInfo[i] = true;
                return;
            }
            //Hierarchical license checking
            if (!(bool)GlobalSettings.FeatureLicenseInfo[6])
            {
                //if CNC and Auto Focus feature is disabled, then..
                GlobalSettings.FeatureLicenseInfo[1] = false; //Disable Touch Probe feature
                GlobalSettings.FeatureLicenseInfo[3] = false; //Disable 2D Contour Scanning
                GlobalSettings.FeatureLicenseInfo[5] = false; //Disable 3D Scanning
                GlobalSettings.FeatureLicenseInfo[9] = false; //Disable Profile Scanning
            }
            if (!(bool)GlobalSettings.FeatureLicenseInfo[2])
            {
                //if Dxf feature is disabled, then..
                GlobalSettings.FeatureLicenseInfo[4] = false; //Disable Dxf Comparator feature
            }

            //Touch Probe Feature
            if ((bool)GlobalSettings.FeatureLicenseInfo[1])
            {
                Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["ProbePresent"] = true;
                Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["ThreedSoftware"] = true;
                GlobalSettings.Is3DSoftware = true;
            }
            else
            {
                Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["ProbePresent"] = false;
                if (!(bool)GlobalSettings.FeatureLicenseInfo[5])
                {
                    Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["ThreedSoftware"] = false;
                    GlobalSettings.Is3DSoftware = false;
                }
            }
            //3D Scanning Feature
            if (!(bool)GlobalSettings.FeatureLicenseInfo[5])
            {
                if (!(bool)GlobalSettings.FeatureLicenseInfo[1])
                {
                    Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["ThreedSoftware"] = false;
                    GlobalSettings.Is3DSoftware = false;
                }
            }
            //Graphics on video
            if ((bool)GlobalSettings.FeatureLicenseInfo[7])
            {
                Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["HideGraphicsOnVideo"] = false;
            }
            else
            {
                Settings_DataB.Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]["HideGraphicsOnVideo"] = true;
            }
            //Normal As One Shot Feature
            bool EnableOneshotFeatures = Convert.ToBoolean(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["EnableOneshotFeatures"]);
            if (!(bool)GlobalSettings.FeatureLicenseInfo[10])
            {
                Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["EnableOneshotFeatures"] = false;
                EnableOneshotFeatures = false;
            }
            if (EnableOneshotFeatures)
                GlobalSettings.RapidMachineType = GlobalSettings.MachineType.Normal_As_One_Shot;

            Settings_DataB.Update_Table("MachineDetails");
            Settings_DataB.Update_Table("UserSettings");
        }

        public List<System.Windows.Controls.Image> UpdateBtnInfoList(string Allignment, double IconWidth, double IconHeight)
        {
            //ButtonImageList.Clear();
            //return ButtonImageList;
            int ct = 0;
            try
            {
                //if (Settings_DataB == null)
                //    Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                System.Data.DataTable tsb = Settings_DataB.GetTable("ToolStripButton");
                //if (tsb != null)
                //    tsb.Clear();
                //Settings_DataB.FillTable("ToolStripButton");

                ButtonImageList.Clear();
                ButtonBaseNameList.Clear();
                ButtonToolTipList.Clear();
                ButtonCheckedPropertyList.Clear();
                ButtonRevertToInitialPropertyList.Clear();
                ButtontypeList.Clear();
                ButtonId.Clear();
                ButtonParentId.Clear();
                ButtonFeatureId.Clear();
                ButttonHasClick.Clear();
                RerelyUsedButtonsList.Clear();
                LastCheckedState.Clear();
                RememberLastState.Clear();

                System.Data.DataRow[] ToolStripRows = Settings_DataB.Select_ChildControls("ToolStripButton", "Alignment", Allignment);
                ToolStripRows = SortRowAccordingToFreq(ToolStripRows);
                string ToolTipColName = "ButtonToolTip" + GlobalSettings.Language;
                //Read the table of buttons to be displayed for current machineNo 
                Settings_DataB.FillTable("ButtonDisplayDetails");
                System.Data.DataTable ButtonDisplayTable = Settings_DataB.GetTable("ButtonDisplayDetails");//.Select("MachineNo = " + MachineNo);

                for (int i = 0; i < ToolStripRows.Length; i++)
                {
                    ct++;
                    if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                    {
                        bool ThreeDBtn = Convert.ToBoolean(ToolStripRows[i]["ThreeDButtonType"]);
                        bool DisableBtn = Convert.ToBoolean(ToolStripRows[i]["HideTool"]);
                        //Get the particular rows of this button and see if it has to be hidden. 
                        System.Data.DataRow[] tempDataRows = ButtonDisplayTable.Select("MachineNo = '" + MachineNo + "' and ButtonID = '" + ToolStripRows[i]["ButtonID"] + "'");
                        if (tempDataRows.Length > 0)
                        {
                            if (Convert.ToBoolean(tempDataRows[tempDataRows.Length - 1]["HideTool"]))
                            {
                                DisableBtn = true;
                            }
                        }

                        if (DisableBtn)
                            continue;

                        if (!GlobalSettings.Is3DSoftware)
                            if (ThreeDBtn) continue;
                    }
                    ButtonToolTipList.Add(ToolStripRows[i][ToolTipColName].ToString());
                    ButtontypeList.Add(Convert.ToInt16(ToolStripRows[i]["ButtonType"]));
                    ButtonCheckedPropertyList.Add(Convert.ToBoolean(ToolStripRows[i]["CheckedProperty"]));
                    ButtonRevertToInitialPropertyList.Add(!Convert.ToBoolean(ToolStripRows[i]["IgnoreOriginalStateDuringClearAll"]));
                    ButtonBaseNameList.Add(ToolStripRows[i]["ButtonName"].ToString());
                    ButtonId.Add(Convert.ToInt16(ToolStripRows[i]["ButtonId"]));
                    ButtonParentId.Add(Convert.ToInt16(ToolStripRows[i]["ButtonParentId"]));
                    ButttonHasClick.Add(Convert.ToBoolean(ToolStripRows[i]["HasClick"]));
                    ButtonFeatureId.Add(Convert.ToInt16(ToolStripRows[i]["FeatureID"]));
                    RerelyUsedButtonsList.Add(Convert.ToBoolean(ToolStripRows[i]["IsRarelyUsed"]));
                    LastCheckedState.Add(Convert.ToBoolean(ToolStripRows[i]["LastCheckedState"]));
                    RememberLastState.Add(Convert.ToBoolean(ToolStripRows[i]["RememberLastState"]));

                    byte[] Source = (byte[])ToolStripRows[i]["ButtonIcon"];

                    System.Windows.Controls.Image img = new System.Windows.Controls.Image();
                    System.Windows.Media.Imaging.BitmapImage bmp = new System.Windows.Media.Imaging.BitmapImage();
                    bmp.CacheOption = System.Windows.Media.Imaging.BitmapCacheOption.OnLoad;
                    bmp.BeginInit();
                    bmp.StreamSource = (new System.IO.MemoryStream(Source));
                    bmp.EndInit();
                    img.Source = bmp;
                    img.Width = IconWidth;
                    img.Height = IconHeight;
                    ButtonImageList.Add(img);
                }
                //Settings_DataB = null;
                return ButtonImageList;
            }
            catch (Exception ex)
            {
                ct++;
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR03", ex);
                return null;
            }
        }
        public System.Windows.Controls.Image UpdateSingleBtnInfo(string BtnName, string Allignment, double IconWidth, double IconHeight)
        {
            //return ButtonImage;
            try
            {
                //if (Settings_DataB == null)
                //    Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                //if (!Settings_DataB.DataS.Tables.Contains("ToolStripButton"))
                //Settings_DataB.FillTable("ToolStripButton");
                System.Data.DataRow[] AllBtnRows = Settings_DataB.Select_ChildControls("ToolStripButton", "Alignment", Allignment);
                System.Data.DataRow BtnRow;
                int indx = -1;
                for (int i = 0; i < AllBtnRows.Length; i++)
                    if (AllBtnRows[i]["ButtonName"].ToString() == BtnName)
                        indx = i;
                if (indx < 0) return ButtonImage;
                BtnRow = AllBtnRows[indx];
                string ToolTipColName = "ButtonToolTip" + GlobalSettings.Language;

                ButtonToolTip = BtnRow[ToolTipColName].ToString();
                ButtonType = Convert.ToInt16(BtnRow["ButtonType"]);
                ButtonCheckedProperty = Convert.ToBoolean(BtnRow["CheckedProperty"]);
                ButtonRevertToInitialProperty = !Convert.ToBoolean(BtnRow["IgnoreOriginalStateDuringClearAll"]);
                ButtonBaseName = BtnRow["ButtonName"].ToString();

                byte[] source = (byte[])BtnRow["ButtonIcon"];
            
                System.Windows.Controls.Image img = new System.Windows.Controls.Image();
                System.Windows.Media.Imaging.BitmapImage bmp = new System.Windows.Media.Imaging.BitmapImage();
                bmp.CacheOption = System.Windows.Media.Imaging.BitmapCacheOption.OnLoad;
                bmp.BeginInit();
                bmp.StreamSource = (new System.IO.MemoryStream(source));
                bmp.EndInit();
                img.Source = bmp;
                img.Width = IconWidth;
                img.Height = IconHeight;
                ButtonImage = img;

                //Settings_DataB = null;
                return ButtonImage;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR04", ex);
                return null;
            }
        }

        public SoundPlayer GetSoundPlayer(string AudioName)
        {
            try
            {
                //if (Settings_DataB == null)
                    //Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                //if (!Settings_DataB.DataS.Tables.Contains("SoundFiles"))
                    Settings_DataB.FillTable("SoundFiles");
                System.Data.DataRow AudioRow = Settings_DataB.Select_ChildControls("SoundFiles", "AudioName", AudioName)[0];

                byte[] source = (byte[])AudioRow["AudioData"];

                Stream rawStream = new MemoryStream(source);
                return new SoundPlayer(rawStream);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR04a", ex);
                return null;
            }
        }

        public void InsertExcelDocPPRun(string ProgramName, string ImageName, string RunName, bool Accepted = false, bool Rejected = false)
        {
            try
            {
                //if (Settings_DataB == null)
                    //Settings_DataB = new DB(GlobalSettings.ExcelDocConnString);
                //if (!Settings_DataB.DataS.Tables.Contains("PartProgramAndItsRun"))
                    Settings_DataB.FillTable("PartProgramAndItsRun");
                System.Data.DataRow PPImgrow = Settings_DataB.GetTable("PartProgramAndItsRun").NewRow();
                PPImgrow["PartProgramName"] = ProgramName;
                PPImgrow["ImageName"] = ImageName;
                PPImgrow["RunName"] = RunName;
                PPImgrow["Accepted"] = Accepted;
                PPImgrow["Rejected"] = Rejected;
                Settings_DataB.GetTable("PartProgramAndItsRun").Rows.Add(PPImgrow);
                Settings_DataB.Update_Table("PartProgramAndItsRun");
                //Settings_DataB = null;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR04a", ex);
            }
        }

        public void SaveContourTracerCalibValues(double[] NormalVector, double[] Offset, double ScalingCoeff, double RLevel)
        {
            try
            {
                //if (Settings_DataB == null)
                    //Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                //if (!Settings_DataB.DataS.Tables.Contains("ContourTracerCalibration"))
                    Settings_DataB.FillTable("ContourTracerCalibration");
                System.Data.DataRow PPImgrow = Settings_DataB.GetTable("PartProgramAndItsRun").NewRow();

                Settings_DataB.GetTable("PartProgramAndItsRun").Rows.Add(PPImgrow);
                Settings_DataB.Update_Table("PartProgramAndItsRun");
                //Settings_DataB = null;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR04b", ex);
            }
        }

        public void Update_Or_Save_LightSettings(string LightName, int[] LVals)
        {
            try
            {
                if (LightName == "")
                    return;

                string[] zones = { "zone1", "zone2", "zone3", "zone4", "zone5", "zone6", "zone7", "zone8" };
                //if (Settings_DataB == null)
                    //Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                //if (!Settings_DataB.DataS.Tables.Contains("LightingSetting"))
                    Settings_DataB.FillTable("LightingSetting");
                System.Data.DataRow LightRow;

                if (GlobalSettings.LightingConfigNameList.Contains(LightName))
                {
                    LightRow = Settings_DataB.Select_ChildControls("LightingSetting", "LightName", LightName)[0];
                }
                else
                {
                    LightRow = Settings_DataB.GetTable("LightingSetting").NewRow();
                    LightRow["LightName"] = LightName;
                    Settings_DataB.GetTable("LightingSetting").Rows.Add(LightRow);
                    GlobalSettings.LightingConfigNameList.Add(LightName);
                }

                for (int i = 0; i < LVals.Length; i++)
                    LightRow[zones[i]] = LVals[i];
                LightRow["zone7"] = 0;
                LightRow["zone8"] = 0;
                Settings_DataB.Update_Table("LightingSetting");
                //Settings_DataB = null;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR05", ex); }
        }
        public void Select_LightSettings(string LightName)
        {
            try
            {
                if (LightName == "" || !GlobalSettings.LightingConfigNameList.Contains(LightName))
                    return;

                System.Data.DataRow LightRow;
                //if (Settings_DataB == null)
                    //Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                //if (!Settings_DataB.DataS.Tables.Contains("LightingSetting"))
                    Settings_DataB.FillTable("LightingSetting");
                LightRow = Settings_DataB.Select_ChildControls("LightingSetting", "LightName", LightName)[0];
                int[] lightvalue = { Convert.ToInt16(LightRow["zone1"]), Convert.ToInt16(LightRow["zone2"]), Convert.ToInt16(LightRow["zone3"]), Convert.ToInt16(LightRow["zone4"]),
                                   Convert.ToInt16(LightRow["zone5"]), Convert.ToInt16(LightRow["zone6"]), Convert.ToInt16(LightRow["zone7"]), Convert.ToInt16(LightRow["zone8"]) };
                RWrapper.RW_CNC.MYINSTANCE().UpdateLight(lightvalue);
                //Settings_DataB = null;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR06", ex); }
        }
        public void Delete_LightSettings(string LightName)
        {
            try
            {
                if (LightName == "" || !GlobalSettings.LightingConfigNameList.Contains(LightName))
                    return;

                System.Data.DataTable LightTable;
                int RowIndex = -1;
                //if (Settings_DataB == null)
                    //Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                //if (!Settings_DataB.DataS.Tables.Contains("LightingSetting"))
                    Settings_DataB.FillTable("LightingSetting");
                LightTable = Settings_DataB.GetTable("LightingSetting");
                for (int i = 0; i < LightTable.Rows.Count; i++)
                {
                    if (LightTable.Rows[i]["LightName"].ToString() == LightName)
                    {
                        RowIndex = i;
                        break;
                    }
                }
                if (RowIndex < 0) return;
                //Settings_DataB.GetTable("LightingSetting").Rows.RemoveAt(RowIndex);
                Settings_DataB.GetTable("LightingSetting").Rows[RowIndex].Delete();
                GlobalSettings.LightingConfigNameList.Remove(LightName);
                Settings_DataB.Update_Table("LightingSetting");
                //Settings_DataB = null;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR07", ex); }
        }

        public string Get_MsgBx_String(string Code)
        {
            int ct = 0;
            try
            {
                //if (Settings_DataB == null)
                    //Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                ct++;
                //if (!Settings_DataB.DataS.Tables.Contains("MsgBoxStatus"))
                    Settings_DataB.FillTable("MsgBoxStatus");
                ct++;
                System.Data.DataRow[] MsgBxRows = Settings_DataB.Select_ChildControls("MsgBoxStatus", "MsgBoxID", Code);//[0];
                ct++;

                if (MsgBxRows == null) return "";
                string MsgColName = "Msg" + GlobalSettings.Language;
                ct++;
                return MsgBxRows[0][MsgColName].ToString();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR08 , ExpectedStatusCode:" + Code + "Caused by FrameWork Event ShowMsgBoxText" + ", " + ct.ToString(), ex);
                return "";
            }
        }
        public string Get_StatusMsg_String(string Code)
        {
            int ct = 0;
            try
            {
                
                //if (Settings_DataB == null)
                    Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                ct++;
                //if (!Settings_DataB.DataS.Tables.Contains("StatusMessage"))
                    Settings_DataB.FillTable("StatusMessage");
                ct++;
                System.Data.DataRow[] StatusMsgRows = Settings_DataB.Select_ChildControls("StatusMessage", "MsgID", Code);//[0];
                ct++;
                if (StatusMsgRows == null) return "";
                string MsgColName = "Msg" + GlobalSettings.Language;
                ct++;
                //System.Windows.MessageBox.Show(MsgColName);
                return StatusMsgRows[0][MsgColName].ToString();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR09 , ExpectedStatusCode:" + Code + " Caused by Framework event DisplayStatusMessage" + ", " + ct.ToString(), ex);
                return "";
            }
        }

        public Hashtable UpdateLanguageTextList(string Alignment)
        {
            try
            {
                //if (Settings_DataB == null)
                    //Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                //if (!Settings_DataB.DataS.Tables.Contains("LanguageText"))
                    Settings_DataB.FillTable("LanguageText");
                System.Data.DataRow[] LanguageTextRows = Settings_DataB.Select_ChildControls("LanguageText", "TextAlignment", Alignment);

                foreach (System.Data.DataRow TxtRow in LanguageTextRows)
                {
                    string TextColName = "Text" + GlobalSettings.Language;
                    StringList[TxtRow["TextName"].ToString()] = TxtRow[TextColName].ToString();
                }
                return StringList;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR10", ex);
                return null;
            }
        }
        public string GetLanguageText(string IdString, string Alignment)
        {
            try
            {
                //if (Settings_DataB == null)
                    //Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                //if (!Settings_DataB.DataS.Tables.Contains("LanguageText"))
                    Settings_DataB.FillTable("LanguageText");
                System.Data.DataRow[] LanguageTextRows = Settings_DataB.Select_ChildControls("LanguageText", "TextAlignment", Alignment);
                System.Data.DataRow LangRow;
                int indx = -1;
                for (int i = 0; i < LanguageTextRows.Length; i++)
                    if (LanguageTextRows[i]["TextName"].ToString() == IdString)
                        indx = i;
                if (indx < 0) return "";
                LangRow = LanguageTextRows[indx];
                string TextColName = "Text" + GlobalSettings.Language;

                return LangRow[TextColName].ToString();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR11", ex);
                return "";
            }
        }

        public int GetRotaryGradeSelection()
        {
            try
            {
                int RetVal;
                //if (Settings_DataB == null)
                    //Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                //if (!Settings_DataB.DataS.Tables.Contains("UserSettings"))
                    Settings_DataB.FillTable("UserSettings");
                RetVal = Convert.ToInt32(Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["RotaryGradeSelected"]);
                //Settings_DataB = null;
                return RetVal;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR12", ex);
                return 0;
            }
        }

        public void SetRotaryGradeSelection(int GradeIndex)
        {
            try
            {
                //if (Settings_DataB == null)
                    //Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                //if (!Settings_DataB.DataS.Tables.Contains("UserSettings"))
                    Settings_DataB.FillTable("UserSettings");
                Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]["RotaryGradeSelected"] = GradeIndex;
                Settings_DataB.Update_Table("UserSettings");
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR12a", ex);
            }
        }

        public int GetFileVersionDetails(List<string> FileName, List<string> FileVersion)
        {
            try
            {
                //if (Settings_DataB == null)
                    Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                //System.Windows.Forms.MessageBox.Show(RWrapper.RW_MainInterface.DBConnectionString);
                //if (!Settings_DataB.DataS.Tables.Contains("FileVersionRecord"))
                    Settings_DataB.FillTable("FileVersionRecord");
                int RowC = Settings_DataB.GetRowCount("FileVersionRecord");
                for (int i = 0; i < RowC; i++)
                {
                    FileName.Add(Settings_DataB.get_GetRecord("FileVersionRecord", i, "FileName").ToString());
                    FileVersion.Add(Settings_DataB.get_GetRecord("FileVersionRecord", i, "FileVersion").ToString());
                }
                return RowC;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR13", ex);
                return 0;
            }
        }

        public System.Data.DataRow[] SortRowAccordingToFreq(System.Data.DataRow[] BtnRows)
        {
            try
            {
                return BtnRows.OrderBy(item => int.Parse(item["Frequency"].ToString())).Reverse().ToArray<System.Data.DataRow>();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR14", ex);
                return null;
            }
        }

        private System.Windows.Media.Color InterpretColour(int rgbVal)
        {
            System.Windows.Media.Color MyColor = new System.Windows.Media.Color();
            int r_Mask = 255 << 16;
            int g_Mask = 255 << 8;
            int b_Mask = 255;
            MyColor.R = (byte)((rgbVal & r_Mask) >> 16);
            MyColor.G = (byte)((rgbVal & g_Mask) >> 8);
            MyColor.B = (byte)(rgbVal & b_Mask);
            MyColor.A = 255;
            return MyColor;
        }

        public System.Data.DataRow[] GetRowsAccordingToCurrentMachineNumber(string Tablename)
        {
            try
            {
                //if (Settings_DataB == null)
                    //Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                //if (!Settings_DataB.DataS.Tables.Contains(Tablename))
                    Settings_DataB.FillTable(Tablename);                
                return Settings_DataB.Select_ChildControls(Tablename, "MachineNo", MachineNo);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR15", ex);
                return null;
            }
        }

        public System.Data.DataRow[] GetRowsAccordingToSelectedMachineNumber(string Tablename , string MachineNumber)
        {
            try
            {
                //if (Settings_DataB == null)
                    Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                //if (!Settings_DataB.DataS.Tables.Contains(Tablename))
                    Settings_DataB.FillTable(Tablename);
                return Settings_DataB.Select_ChildControls(Tablename, "MachineNo", MachineNumber);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR15", ex);
                return null;
            }
        }

        public System.Data.DataRow[] GetRowsAccordingToFilter(string Tablename, string ColumnName, object Parameter)
        {
            try
            {
                //if (Settings_DataB == null)
                    //Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                //if (!Settings_DataB.DataS.Tables.Contains(Tablename))
                    Settings_DataB.FillTable(Tablename);
                return Settings_DataB.Select_ChildControls(Tablename, ColumnName, Parameter);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR15", ex);
                return null;
            }
        }

        public bool SaveCheckedStatusOfButton(string ColumnName, object Alignment , string ButtonName , bool CheckStatus)
        {
            try
            {
                System.Data.DataRow[] DR = GetRowsAccordingToFilter("ToolStripButton", ColumnName, Alignment);
                foreach (System.Data.DataRow row in DR)
                {
                    if (row["ButtonName"].ToString() == ButtonName)
                    {
                        row["LastCheckedState"] = CheckStatus;
                        break;
                    }
                }
                Settings_DataB.Update_Table("ToolStripButton");
                return true;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR_SCSOB01", ex);
                return false;
            }
        }

        public bool UpdateTable(string Tablename)
        {
            try
            {
                Settings_DataB.Update_Table(Tablename);
                return true;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR16", ex);
                return false;
            }
        }

        public System.Data.DataTable GetTable(string Tablename)
        {
            try
            {
                //if (Settings_DataB == null)
                    //Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                //if (!Settings_DataB.DataS.Tables.Contains(Tablename))
                    Settings_DataB.FillTable(Tablename);
                return Settings_DataB.GetTable(Tablename);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR17", ex);
                return null;
            }
        }

        public bool CheckButtonStatus(string Tablename, string ColumnName)
        {
            bool Itellstatus = false;
            try
            {
                //if (Settings_DataB == null)
                    Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
                //if (!Settings_DataB.DataS.Tables.Contains(Tablename))
                    Settings_DataB.FillTable(Tablename);
                System.Data.DataRow[] DRow = Settings_DataB.Select_ChildControls(Tablename, "MachineNo", MachineNo);
                Itellstatus = (bool)(DRow[0][ColumnName]);
                return Itellstatus;
            }
            catch (Exception)
            {
                return Itellstatus;
            }

        }

        private void Add_NewField(string TableName, string ColumnName, System.Type ColumnType, object DefaultValue = null)
        {
            try
            {
                if (!Settings_DataB.GetTable(TableName).Columns.Contains(ColumnName))
                {
                    Settings_DataB.AddColumnToTable(TableName, ColumnName, ColumnType); // typeof(System.Int32));
                    Settings_DataB.FillTable(TableName);
                    System.Data.DataTable thisTable = Settings_DataB.GetTable(TableName);
                    if (DefaultValue != null)
                    {
                        foreach (System.Data.DataRow row in thisTable.Rows)
                            row[ColumnName] = DefaultValue;
                        Settings_DataB.Update_Table(TableName);
                    }
                    Settings_DataB.FillTable(TableName);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DBR18", ex);
            }
        }
    }
}

//
//Wrapper Main Class: Intermediate for front end and framework...
//Header files from frame work and Other Classes from wrapper..//
#pragma once
//including assemblies.. required..
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Runtime::InteropServices;
using namespace System::Windows::Input;
using namespace System::Drawing;
using namespace System::Data;


#include "Engine\RCadapp.h"
#include "..\HardwareCommunication\Engine\HardwareCommunication.h"
#include "RW_DBSettings.h"
#include "RW_Enum.h"
#include "RW_DRO.h"
#include "RW_CNC.h"
#include "RW_SuperImposeImage.h"
#include "RW_PCD.h"
#include "RW_FocusDepth.h"
#include "RW_FixedGraph.h"
#include "RW_PartProgram.h"
#include "RapidDimListener.h"
#include "RapidShapeListener.h"
#include "RapidDxfListener.h"
#include "RapidUcsListener.h"
#include "RW_Thread.h"
#include "RW_LaserComm.h"
#include "RW_AutoCalibration.h"
#include "RW_ShapeParameter.h"
#include "RW_MeasureParameter.h"
#include "RW_UCSParameter.h"
#include "OglLighting.h"
#include "RW_DrawGraphicsText.h"
#include "vcclr.h"
#include <list>
#include "..\MAINDLL\Handlers_SmartMeasures\SM_LineArcFGHandler.h"
#include "ProbePathEntity.h"
#include "RW_FixtureCalibration.h"
#include "RW_VBlockCallibration.h"
#include "FixedTool\CalibrationText.h"
#include "RW_SphereCalibration.h"
#include "RW_CircularInterPolation.h"
#include "RW_LinearInterPolation.h"
#include "..\MAINDLL\Shapes\TriangularSurface.h"

#include "RapidActionListener.h"
#include "RW_ActionParameter.h"
#include "..\MAINDLL\Actions\AddProbePathAction.h"
#include "RW_ImageGridProgram.h"
#include "RW_TIS_Control.h"

namespace RWrapper 
{
	//Satic Byte of size hold the pixel(save graphics)
	//static BYTE pixels[3664 * 2748 * 4];

public ref struct CommandLogger
	{
		String^ CommandValue;
		String^ Comments;
		DateTime^ Timestamp;
	};

public ref class RW_MainInterface
{
public:
#pragma region Delegates to Handle events
	delegate void RWrapperEventHandler();
	delegate void RWrapperEventHandlerWithDouble(double i);
	delegate void ClearAllEventHandler(int i);
	delegate void SurfaceEDEventHandler(System::String^ SEDEntities);
	delegate void RightClickOnEntityEventHandler(int i, System::String^ Alignment);
	delegate void DoubleClickOnEntityEventHandler(int i);
	delegate void WaitMessageEventHandler(System::String^ StatusMessage, bool showpanel, bool AllowCancel);
	delegate void HelpTextStatusEventHandler(System::String^ text, bool Insert_TempStrFlag, System::String^ TempStr);
	delegate void RaiseToolbarClickEventHandler(System::String^ ToolbarName, System::String^ buttonName, bool Checked);
	delegate bool MsgBoxEventHandler(System::String^ MessageText, RWrapper::RW_Enum::RW_MSGBOXTYPE btnType, RWrapper::RW_Enum::RW_MSGBOXICONTYPE icon, bool Insert_TempStrFlag, System::String^ TempStr);
	delegate void LineArcParametersEventHandler(double tolerance, double max_radius, double min_radius, double min_angle_cutoff, double noise, double max_dist_betn_pts, bool closed_loop, bool shape_as_fast_trace, bool join_all_pts);
	delegate void ShowHideWaitCursrEventHandler(int i);
	delegate void BestFitSurfaceEventHandler(double i, bool value);
	delegate void DeviationEventHandler(System::Collections::Generic::List<System::Collections::Generic::List<System::Double>^>^ DeviationMeasureList,  System::Collections::ArrayList^ CompDetails_ArrayList);
	delegate void ProfileScanEndPtsEventHandler(int i);
	delegate void ShowAutoFocusWindow(bool open);
	delegate void ShowRotateScanEventHandler(int i);
	delegate void UpdateTruePositionWindowParam(double x, double y);
	delegate void UpdateCamSettingsHandler(System::String^ UpdatedSettings);

	int ProbeCount;
#pragma endregion
 private:
	
#pragma region Private Variables
	  System::Collections::Generic::List<RWrapper::RW_Probe_Path_Entity^>^ ProbePathList;
	 bool window1_State, window2_State;
	 System::Collections::ArrayList^ ProbeScanDetais_ArrayList;
	System::ComponentModel::Container ^components;

	static RW_MainInterface^ MainInInstance; //Main form Static Instance..//
	System::Windows::Forms::ColorDialog^ _colorDialog; //Comman color dialog for shapes, measurements, crosshair..
	//RWrapper::RW_CalibrationMod^ Settings_Instance; //Calibration Module Instance...
	System::String^ ModuleInitialisationLog_Path; //For initialisation Log, command send log path..
	System::String^ LastSelectedToolbar; //To Save Last selected toolbar type
	System::String^ LastSelectedButton; //To Save last selected tool
	System::Windows::Forms::Form^ HiddenPictureBox; // used for Digital Zoom..double sized Image..
	//Error Module Instance...//
	ErrHandlerApp::ErrHandlerApp^ ErrorHandlerObject;
	System::IntPtr^ VideoWindowHandle;
	double InitialDigitalZoomLevel;
	System::Collections::Generic::List<CommandLogger^>^ SentCommandLogCollection;
	System::Threading::Timer^ CommandsLogWriter;
	bool OK_To_Write_Logs;
#pragma endregion

#pragma region Private Functions
	 //Export points to files...//
	 void ExportPointToCAD();
	 void ExportPointToText();
	 void ExportPointToCsv();
	 void ExportParamToCsv();
	 void ReadPointFromText();
	 void ReadFocusValuesFromText();

	 void SaveCurrentClickedButton(System::String^ Alignment, System::String^ BtnStr);
	 void ClearLastSelectedButton();
	 void HandleRaiseToolBarClickevent(System::String^ LastAlignment, System::String^ LastBtnStr, System::String^ CurrentAlignment, System::String^ CurrentBtnStr);
	 void HandleRightClickOnShape(System::String^ Str);
	 void HandleRightClickOnMeasure(System::String^ Str);
	 void HandleRightClickOnCircle(System::String^ Str);
	 void HandleRightClickOnLine(System::String^ Str);
	 void HandleRightClickOnPerimeter(System::String^ Str);
	 void HandleRightClickOnPoint(System::String^ Str);
	 void HandleRightClickOnLine3D(System::String^ Str);
	 void HandleRightClickOnCylinder(System::String^ Str);
	 void HandleRightClickOnCone(System::String^ Str);
	 void HandleRightClickOnPlane(System::String^ Str);
	 void HandleRightClickOnSphere(System::String^ Str);
	 void SetSurfaceGrabType(System::String^ Str);
	 void HandleRightClickOnCircle3D(System::String^ Str);
	 void HandleRightClickOnCloudPoints(System::String^ Str);

	 void AllowPermissions(System::String^ FolderPath);

	 void CommandLogWriteTick(System::Object^ obj);
#pragma endregion

public:

#pragma region All Events ..Hanlded in Front end
	 //All the events... The name itself indicates its functionality...!!//
	
	 //Events to handle the derived shapes....
	 event RWrapperEventHandler^ HideEntityPanel;
	 event RWrapperEventHandler^ ParallelLineOffset;
	 event RWrapperEventHandler^ CircleDiameter;
	 event RWrapperEventHandler^ OnePtOffsetLine;
	 event RWrapperEventHandler^ OnePtAngleLine;
	 event RWrapperEventHandlerWithDouble^ ParallelismLand;
	 event RWrapperEventHandlerWithDouble^ PerpediclaurityLand;
	 event RWrapperEventHandlerWithDouble^ AngularityLand;
	 event RWrapperEventHandler^ EnterTruePosition;
	 event RWrapperEventHandler^ Enter3DTruePosition;
	 event RWrapperEventHandler^ Enter3DTruePositionPoint;
	 event RWrapperEventHandler^ EnterTruePositionForPoint;
	 event RWrapperEventHandler^ EnterTextValue;
	 event RWrapperEventHandler^ EnterDerivedLineLength;
	 event RWrapperEventHandler^ EnterThreadCount;
	 event RWrapperEventHandler^ AutoDxfAlign;
	 event RWrapperEventHandler^ ParallelPlaneOffset;
	 event RWrapperEventHandler^ PerpendicularPlaneOffset;
	 event RWrapperEventHandler^ ParallelLine3DOffset;
	 event RWrapperEventHandler^ PerpendicularLine3DOffset;
	 event RWrapperEventHandler^ RelativePointOffset;
	 event RWrapperEventHandler^ NudgeRotatePPevent;
	 event RWrapperEventHandler^ RotationAngleevent;
	 event RWrapperEventHandler^ Translationevent;
	 event RWrapperEventHandler^ ShowOuterMostLineEvent;
	 event RWrapperEventHandler^ ShowDeviationWindow;
	 event RWrapperEventHandler^ ShowCloudPointMeasureWindow;
	 event RWrapperEventHandler^ LineArcUcsCreatedEvent;
	 event RWrapperEventHandler^ FixtureAlign_RefPt_Taken;

	 event BestFitSurfaceEventHandler^ BestFitSurfaceParam;
	 event ShowAutoFocusWindow^ ShowWindowForAutoFocus;
	 event ShowAutoFocusWindow^ ShowWindowForAutoContour;
	 event LineArcParametersEventHandler^ LineArcParameters;

	 event RWrapperEventHandler^ ClearAllRadioButtonCheckedProperty;
	 event ShowHideWaitCursrEventHandler^ WaitCursor;
	 event ProfileScanEndPtsEventHandler^ ProfileScanEndPtEvent;
	 event SurfaceEDEventHandler^ GetSurfaceEDUserChoice;
	 //Events to Handle the Right On entity....//
	 event DoubleClickOnEntityEventHandler^ DoubleClickedOnEntity;
	 event RightClickOnEntityEventHandler^ RightClickedOnEntity;
	 event ClearAllEventHandler^ ClearEverything;
	 event RightClickOnEntityEventHandler^ ChangeMeasurementFontSize;

	 event RaiseToolbarClickEventHandler^ GenerateToolbarClickEvent;
	 event RaiseToolbarClickEventHandler^ GenerateToolbarBtnCheckEvent;

	 event HelpTextStatusEventHandler^ DisplayStatusMessage;

	 event WaitMessageEventHandler^ ShowHideStatusWindow;
	 event MsgBoxEventHandler^ ShowMsgBoxText;
	 event SurfaceEDEventHandler^ SavedImagePath;
	 event DeviationEventHandler^ MeasureDeviationEvent;
	 event ShowRotateScanEventHandler^ ShowRotateScanEvent;
	 event UpdateTruePositionWindowParam^ TruePositionEvent;

	 event UpdateCamSettingsHandler^ UpdateCamSettingsEvent;
#pragma endregion	

#pragma region All Public Variables..
	 
	 System::String^ WaitStatusMessage; //Status Message..
	 bool AllowCancelofStatusTask;
	 //Curretn Entity Name.. and Id To be displaye on front end
	 static System::String^ EntityName;
	 static int EntityID;
	 //Entity Id list.. whenever multiselection happens..
	 static System::Collections::Generic::List<System::Int32>^ EntityIDList;
	 static System::Collections::Generic::List<System::String^>^ EntityNameList;

	 BYTE* image_pixels;

	 //Database connection string..
	 static System::String^ DBConnectionString;
	 //Program Data folder path.. DB Path..
	 System::String^ AppDataFolderPath, ^LogFolderPath;

	 bool MachineConnectedStatus, MachineCNCStatus;
	 bool IsMultiLevelZoom; // , Force_Auto_FG;
	 bool AllowStepImport, UseLookAhead, TwoXLensFitted;
	 int MachineCardFlag;

	 int DealyinSec, Delay_AfterTargetReached; //Used for automated Zoom In out.. wait after sending command..
	 System::Windows::Forms::Timer^ StopWaitMessageTimer;
	 System::Windows::Forms::Timer^ StartWaitMessageTimer;
	 System::String^ LastSelectedMagLevel;
	 double DigitalZoomLevel;
	 bool Do_Homing_At_Startup;
	 cli::array<Double>^ RCadMousePos;
#pragma endregion

#pragma region Constructor and Initialise, Key Event Functions
	 //Constructor and destructor...//
	 RW_MainInterface();
	 ~RW_MainInterface();	
	 static void CloseAll();
	 //MainInstance.. Instance...//
	 static RW_MainInterface^ MYINSTANCE();
	 bool IntializeOpengl(System::IntPtr^ handle, int width, int height, int Windowno, int CamWidth, int CamHeight);
	 //Form Key board event Handling//
	 void Form_KeyUp(System::Windows::Input::KeyEventArgs^ e);
	 void Form_KeyDown(System::Windows::Input::KeyEventArgs^ e);
#pragma endregion

#pragma region Save Image, Get Status Message, Clear All functions
	 //Function to get the error/stack message according to the code..
	 System::String^ GetStatusMessage(System::String^ code, System::String^ table);

	  //Clear all...//
	 void ClearALL();	
	 void InternalClearAll(int id);

	 void SaveImageForOneShot(System::String^ FileName);
	 void SaveAllImageswithoutgraphics(int Cnt);
	 
	 void ZoomToExtentsRcadorDxf(int windowno);
	 void ClearShapeAndMeasure_Selections();
	 void SaveVideoWindowImage(System::String^ FilePath, bool WithGraphics, bool ShowOnPicViewer);
	 void SaveRcadWindowImage(System::String^ FilePath, bool ShowOnPicViewer);
	 void RGBArray2Image(BYTE* SourceImagePixel, int width, int height, System::String^ FilePath, bool IncludeAlpha, bool FlipY);	 	 
#pragma endregion

#pragma region Functions To Handle the Toolbar Click
	 void HandleMeasure_Click(System::String^ Str);
	 void HandleDrawToolbar_Click(System::String^ Str);
	 void HandleDerivedShapeToolbar_Click(System::String^ Str);
	 void HandleCrossHairToolbar_Click(System::String^ Str);
	 void HandleEditToolbar_Click(System::String^ Str);
	 void HandleToolsToolbar_Click(System::String^ Str);
	 void HandleRcadToolbar_Click(System::String^ Str);
	 void HandleDXFToolbar_Click(System::String^ Str);
	 void HandleShapeParameter_Click(System::String^ Str);
	 void HandleMeasureParameter_Click(System::String^ Str);
	 void HandleUCSProperties_Click(System::String^ Str);
	 void HandleVisionHeadToolBar_Click(System::String^ Str);

	 void HandleRightClick_OnEntities(System::String^ Alingment, System::String^ Str);
	 void HandleCheckedProperty_Load(System::String^ Alignment, System::String^ Str);
	 void HandleGenerateToolbarClick(System::String^ Alignment, System::String^ BtnName);
	 void HandleGenerateToolbarClick1(System::String^ Alignment, System::String^ BtnName, bool flag);
	 void GenerateEventToCheckCurrentSM();
	 void CallClearRadioButttons();
#pragma endregion

#pragma region Video Window Mouse Event Handling
	 void OnVideoLeftMouseDown(double x, double y); 
	 void OnVideoRightMouseDown(double x, double y); 
	 void OnVideoMiddleMouseDown(double x, double y); 
	 void OnVideoLeftMouseUp(double x, double y); 
	 void OnVideoRightMouseUp(double x, double y); 
	 void OnVideoMiddleMouseUp(double x, double y); 
	 void OnVideoMouseMove(double x, double y);
	 void OnVideoMouseWheel(int evalue);
	 void OnVideoMouseEnter();
	 void OnVideoMouseLeave();
#pragma endregion

#pragma region Rcad Window Mouse Event Handling
	 void OnRCadLeftMouseDown(double x, double y); 
	 void OnRCadDoubleLeftMouseDown(double x, double y); 
	 void OnRCadRightMouseDown(double x, double y); 
	 void OnRCadMiddleMouseDown(double x, double y); 
	 void OnRCadLeftMouseUp(double x, double y); 
	 void OnRCadRightMouseUp(double x, double y); 
	 void OnRCadMiddleMouseUp(double x, double y); 
	 void OnRCadMouseMove(double x, double y);
	 void OnRCadMouseWheel(int evalue);
	 void OnRCadMouseEnter();
	 void OnRCadMouseLeave();
#pragma endregion

#pragma region DXf window Mouse Event Handling
	 void OnDXFLeftMouseDown(double x, double y); 
	 void OnDXFRightMouseDown(double x, double y); 
	 void OnDXFMiddleMouseDown(double x, double y); 
	 void OnDXFLeftMouseUp(double x, double y); 
	 void OnDXFRightMouseUp(double x, double y); 
	 void OnDXFMiddleMouseUp(double x, double y); 
	 void OnDXFMouseMove(double x, double y);
	 void OnDXFMouseWheel(int evalue);
	 void OnDXFMouseEnter();
	 void OnDXFMouseLeave();
#pragma endregion
	 
#pragma region Partprogram window Mouse Event Handling
	 void OnPartprogramLeftMouseDown(double x, double y); 
	 void OnPartprogramRightMouseDown(double x, double y); 
	 void OnPartprogramMiddleMouseDown(double x, double y); 
	 void OnPartprogramLeftMouseUp(double x, double y); 
	 void OnPartprogramRightMouseUp(double x, double y); 
	 void OnPartprogramMiddleMouseUp(double x, double y); 
	 void OnPartprogramMouseMove(double x, double y);
	 void OnPartprogramMouseWheel(int evalue);
	 void OnPartprogramMouseEnter();
	 void OnPartprogramMouseLeave();
#pragma endregion
	

#pragma region Update Status Message
	 void Update_HelpStatusMessage(System::String^ Str, bool flag, System::String^ delimiter);
	 void ShowWaitStatusMessage(System::String^ SMessage, int startafter, bool ShowCancelButton);
	 void StartHideWaitStatusMessage(int hideafter);
	 void HideWaitMessagePanel(System::Object^ sender, System::EventArgs^ e);
	 void ShowWaitMessagePanel(System::Object^ sender, System::EventArgs^ e);
	 bool Update_MsgBoxStatus(System::String^ MessageText, RWrapper::RW_Enum::RW_MSGBOXTYPE btnType, RWrapper::RW_Enum::RW_MSGBOXICONTYPE icon, bool Insert_TempStrFlag, System::String^ TempStr);
#pragma endregion

	 void Update_FramGrab(System::String^ Alignment, System::String^ btnName, bool btnclick);
#pragma region LineArcFg functions
	 void GetLineArcDiffShapes(double tolerance, double max_radius, double min_radius, double min_angle_cutoff, double noise, double max_dist_betn_pts, bool closed_loop, bool shape_as_fast_trace, bool join_all_pts);
	 void AddLineArcDiffShapes();
	 void CancelLineArc();
#pragma endregion

#pragma region Maximise, Resize OGl windows, Snap Mode Toggle
	 void MaximizeWindow(int windowno, int width, int height);
	 void ResizetheWindow(int windowno, int width, int height);
	 void ToggleShowHideWindow(int windowno, bool Hidewindow);
	 void ToggleSnapeMode();
	 void ToggleVSnapeMode();
#pragma endregion


#pragma region Set Values from Front end
	 //DXF import and export actions..//
	 void DXFImport(bool KeepitFixed);

	 void DXFExport(bool CloudPointAsPolyLine);
	 void SetAlignmentCorrectionValue(cli::array<System::Double, 1>^ CorrectionlistX, cli::array<System::Double, 1>^ CorrectionlistY, cli::array<System::Double, 1>^ DistList);
	 void SetDerivedShapeParameters(double offset, double dist, double Llength);
	 void SetUCSRotateAngle(double angle);
	 void SetTruePos(double x, double y, int i, bool PolarCord, bool forPoint);
	 void Set3DTruePos(double x, double y, int i, bool forPoint);
	 void CancelPressed();
	 void MM_DropDown_clicked(int mode);
	 void SetMagnification(System::String^ Str);
	 void SetCameraToCompleteZoom_Exeload();
	 void RefreshDRO();
	 void SetProgramHomePostion();
	 bool IsPartprogramRunning();
	 int SetUserChosenSurfaceED(System::String^ Str);
	 void HighlightUserChosenSurfaceED(System::String^ Str);
	 void SetEntityTransparency(double Alpha_value);
	 void setCoordinateListForAutomeasurement(System::Collections::Generic::List<System::Double>^ listcoord);
	 void setCoordinate_ProbeMeasurement(System::Collections::Generic::List<RWrapper::RW_Probe_Entity^>^ ExcelList, System::Collections::ArrayList^ Comp_ArrayList);
	 void setActionIdForPPEdit(int ActionId, bool selected);
	 void SetNudge_RotateForRCad(bool IsNudge, bool IsRotate);
	 void SetNudge_RotateForDxf(bool IsNudge, bool IsRotate);
	 void AutoAlignmentForSelectedentities(bool SelectedPtsOnly, int trans_Rotate);
	 void SetOGLFontFamily(System::String^ Str); //for font family
	 void SetOGLGradientBackGround(double r1,double g1, double b1, double r2, double g2, double b2);
	 void SetFGandCloudPointSize(int PtSize);
	 void SetActionForNudgeRotate();
	 void SetProfileScanParam(int jump);
	 void PickProfileScanEndPoint();
	 void SetPLaceCalibrationDeatil(System::String^ MachineNo,System::String^ CmpName, System::String^ CalibName, System::String^ CalibDate);
	 void PLaceCalibrationTable();
	 void CreateIntersectionPtsWithTriangle(double interval, int Axisno);
	 void SetOffsetCorrectionflag(bool XOffset, bool YOffset, bool ZOffset, bool ROffset);
	 void SetProfileScanPause_StoppedFlag(bool StopFlag);
	 void ContinueProfileScan();
	 void ContinueProfileScan(int jump);
	 void SetProfileScanParam_MultiFeature(int jump);
	 void ContinueMultiFeatureProfileScan();
	 void CreateAreaShape(bool WhiteRegion);
	 void CompareCloudPtsWithStlSurface(double tolerance);
	 void SetColorThresholdValue(double tolerance);
	 void PointToCloudPointsMeasurement(double Angle, bool ArcBestFit, int NumberOfpoints);
	 void CloudPointComparator(double interval, bool ArcBestFit, int NumberOfPts);
	  void CloudPointComparator(int MeasureCount, bool ArcBestFit, int NumberOfPts);
	 void SetRGBChannelColor(int Channel_Number);
	 void CreateCloudPointsMeasurement(System::Collections::ArrayList^ Measure_ArrayList);
	 void SetSurfaceAloType(int type);
	 void Start_StopRotationalScan(bool StartScan);
	 void SetContinuousScanMode(bool ScanMode);
	 void SetRunoutFlute(int cnt, int RunOut_Type);
	 void SetRunOutFlute_Radius(int FluteCount, double AxialRadius);
	 double GetRunOutAxialRadius(System::Boolean% Flag);
	 void SetFilterStatus(bool flag);
	 void SetMMCStatus(bool flag);
	 void SetMMCOnOffStatus(bool flag);
	 void SetAlgo6Param(int type6_MaskFactor, int type6_Binary_LowerFactor, int type6_Binary_UpperFactor, int type6_PixelThreshold, int type6_MaskingLevel, int type6_ScanRange, int type6_NoiseFilterDistInPixel, int type6SubAlgo_NoiseFilterDistInPixel, int type6JumpThreshold, bool DoAverageFilter);
	 void SetSurfaceEDSettings(int channel);
	 void SetNogoMeasurement(bool flag);
	 void SetCriticalShapeParameter(bool CriticalShape, double AngleTolerance, double RadiusPercentage);
	 void SetMeasurementColorGradient(bool gradient);
	 void SetRcadZoomToExtent(bool flag);
	 void CreateSurfaceForClosedSurface(bool flag);
	 void UpdateCameraZoom(int CamWidth, int CamHeight);
	 void SetbestfitAlignmentParam(double Resolution, int SampleSize);
	 void CreateCamProfileShape(double MinRadius, double MaxRadius, double sector_angle, double min_tol, double max_tol, bool Tool_Type, int Pts_toBundle);			//vinod according to new fe.  14/02/2014
	 void GetCamProfileDistanceFromCenter(double angle, int PtsToBundle);
	 void GethobCheckerPersonaldata(int OrderNo, int SerialNo, int ToolID, System::String^ CustomerName, System::String^ Date, System::String^ Operator, int MachineNo, bool IsHobTypeBore, System::String^ AccuracyClass, double leadofGash, System::String^ Module);		//vinod 22/02/2014
	 void GetvalueofBCXBCY(double ValBCX, double ValBCY);		//vinod..for hob checker getc_axis_xy..
	 void SetShapeMeasureHideDuringProgramRun(bool Flag);
	 void SetPixelWidthCorrectionStatusForOneShot(bool Flag);
	 void SetPixelWidthCorrectionConstants(double* PixelWidthConstants); //double QuadraticTerm, double LinearTermX, double LinearTermY, double PWX0, double PWY0);//vinod........................
	 void SetCamProperty(int PropID, int Value);
	 int GetCamProperty(int PropID);
	 void GetRefDotParams(int index, cli::array<System::Double, 1>^ CentreVal);
	 void SetdefaultCamSettings(int SettingType, System::String^ NewCamSettings);

#pragma endregion

	 void ProbeGotoRequiredPosition_GetProbePts();
	 void Continue_PauseProbeScanning(bool continueflag);
	 void AbortProbeScanningProcess();
	 void ShowDeviation_ProbeScanPts();
	 void CloudPointDeviation_DxfShape(double UpperCutoff, double tolerance, double interval, int IntervalType, bool ClosedLoop);
	 void CloudPointDeviation_DxfShape(double Uppertolerance, double Lowertolerance, double interval, bool DeviationWithYellowShapes, int IntervalType, bool deviationType, int measureRefrenceAxis, bool BestfitArc);

#pragma region Set Dialog Results from front end: Color dialog
	 void Set_FixedCrosshairColor(int r_val, int g_val, int b_val);
	 void Set_FlexibleCrosshairColor(int r_val, int g_val, int b_val);
	 void Set_MeasurementColor(int r_val, int g_val, int b_val);
	 void Set_ShapeColor(int r_val, int g_val, int b_val);
#pragma endregion

#pragma region Set Dialog Results from front end: Save dialog, OpenDialog
	 void SaveDialog_DxfExport(System::String^ filename);
	 void OpenDialog_DxfImport(System::String^ filename);
#pragma endregion
	 
#pragma region Write User Log
	 void WriteUserLog(System::String^ FunctionName, System::String^ Action, System::String^ Position);
#pragma endregion

#pragma region Write Error Log
	 void WriteErrorLog(System::String^ ErrorCode, System::Exception^ ex);
	 void WriteErrorLog(System::String^ ErrorCode, System::String^ Errormsg, System::String^ ErrorTrace);
	 void WriteModuleInitilization(System::String^ Str);
	 void WriteCommandSendLog(System::String^ Str);
	 void WriteCommandSendLog(RWrapper::CommandLogger^ currentCommandLog);
#pragma endregion

#pragma region Raise events
	 void RaiseEvents(int i);
	 void RaiseEvents(int i, double val);
	 void RaiseSurfaceFrameGrabEvent();
	 void RaiseSurfaceFrameGrabEvent(System::String^ SFGEntityNames);
	 void RaiseSavedImageEvent(System::String^ FilePath);
	 void RaiseCamSettingsUpdated(System::String^ uCamSettings);
	 void LineArcEvent(double tolerance, double max_radius, double min_radius, double min_angle_cutoff, double noise, double max_dist_betn_pts, bool closed_loop, bool shape_as_fast_trace, bool join_all_pts);
	 void StartFocusInFrameGrab();
	 void StartFocusForRightClick();
	 void StartFocusInProfileScan(); 
	 void ShowHideWaitCursor(int i);
	 void GotProfileScanEndPts(int i);
	 void BestFitSurfaceWindow(double i, bool value);
	 void ShowWindowForAutoFocusRaise(bool open);
	 void ShowWindowForAutoContourRaise(bool open);
	 void ShowRotationScanWindowRaise(int i);
	 void BestFitWithDifferentParam(double i, bool value, bool again);
	 void LineArcUcsCreated();
	 void RaiseTruePositionEvent(double x, double y);
#pragma endregion 
	 
#pragma region Update OGl Window Grpahics
	 void UpdateVideoGraphics();
	 void UpdateRCadGraphics();
	 void UpdateDxfGraphics();
#pragma endregion

#pragma region Tool Inspection System Functions
	 void CaptureCAxis();
#pragma endregion
 
	 void GenerateToolBarClick(System::String^ ToolbarName,System::String^ buttonName, bool Checked);
	 void deletePointsLogFile();
	 void SetShapeForMeasure(int id);
	 void SetShapeRightClickForMeasure(int id);
	  void RightClickOnShapeInMeasureMode(Shape *hsp);
	  void GetInsertedImageBytes(bool InsertImage, cli::array<Byte, 1>^ SourceBytes);
	  void GetBinarizedImage(cli::array<Byte, 1>^ SourceBytes);
	  void ClassControl(ClassNames ClassName, bool Start);
	  int GetCurrentCameraType();
	  void Abort_HomePosition_Command();
	  bool OperateSpecialPins(int PinNumber, bool SwitchON);
	  void Set_Forced_Auto_FG(bool isAuto);
	  void CheckRunningGraphicsCard();
	  void CrossHairOffsets(cli::array<int>^ OffsetValue, bool GetValuesOnly);

private:	
#pragma region Private functions
	 void DataTransferToExcelForDeviation();
#pragma endregion
 };
}

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!

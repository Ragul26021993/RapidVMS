//Main Class in FrameWork..Interacts with the Wrapper...
//All Window Mouse events are Handled here..
//And Also almost all the button handling..
#pragma region Added Header Files
#pragma once

#include <list>
#include <map>
#include <vector>
#include <TCHAR.H>
#include <iomanip>
#include <string>
#include <fstream>
//Header file added for Mouse handle, Action class and UCS.
#include "..\MainDLL.h"
//Header file RCollection base.
#include "RCollectionBase.h"
//Header file for Listenable Item.
#include "ListenableItem.h"
#include "CollectionListener.h"
#include "..\RapidGraphics\Engine\RGraphicsDraw.h"
#include "..\GridModule\Engine\GridModule.h"
#include "..\Helper\General.h"
#include "UCS.h"

#define RCIT list<BaseItem*>::iterator
#define FGPOINTSIZE 3.0
#define PSC_ITER map<int,Shape*>::iterator
#define PC_ITER map<int,SinglePoint*>::iterator

#pragma endregion

#pragma region Macros and Objects Of Diff Modules

//Expose RBF class instance.
#define BESTFITOBJECT RCadApp::getInstance()->getBestfit()
//Expose RCadApp class instance.
#define MAINDllOBJECT RCadApp::getInstance()
//Exposed Edge detection
#define EDSCOBJECT RCadApp::getInstance()->getEdgeDetection()
//Expose Helper Object.
#define HELPEROBJECT RCadApp::getInstance()->getHelper()
//Expose Calibration Calc Module Instance
#define CALBCALCOBJECT RCadApp::getInstance()->getcbcInst()
//Expose Fixed Tool Handler..
#define FIXEDTOOLOBJECT RCadApp::getInstance()->getSpecialToolHandler()
//Expose Dxf Expose Object..
#define DXFEXPOSEOBJECT RCadApp::getInstance()->getDXFexobj()
//Expose Thread Calc Module Instance
#define THREADCALCOBJECT RCadApp::getInstance()->gettcfnInst()
//Expose Focus Calc Module Instance
#define FOCUSCALCOBJECT RCadApp::getInstance()->getffnInst()
//Expose Partprogram Calc Module Instance
#define PPCALCOBJECT RCadApp::getInstance()->getppnInst()
//expose calibration text
#define CALIBRATIONTEXT RCadApp::getInstance ()->getctInst()

#pragma endregion


#define SNAPDIST 7.0 //Shape snap dist... Circular Pixel gap.
#define RECTWIDTH 5.0 //Snap Point Rectangle width.

class FocusFunctions;
class FixedToolCollection;
class CalibrationText;
class EdgeDetectionSingleChannel;
class CalibrationCalculation;
class ThreadCalcFunctions;
class DXFExpose;
class Line;
class Plane;
class Shape;
class Sphere;
class MouseHandler;
class RAction;
class AddThreadMeasureAction;
class HelperClass;
class PartProgramFunctions;
class DimBase;
class Circle;

//Main Class...
class MAINDLL_API RCadApp: 
	public CollectionListener
{
private:

	class GenericProbeOffset
	{
		public:
		  double offsetX, offsetY, offsetZ, radius;
	};

#pragma region All Collections..
	RCollectionBase actionhistory; //collection of all actions executed by the user.
	RCollectionBase ucslist; //collection of UCS
	RC_ITER currentaction;	//pointer to current Action
	std::list<Shape*> CurrentSelectedShapeList; //Holds pointer to the selected shapes to create copy.

	map<int, Shape*> ShapesForTwoStepHoming; //Shape Pointers taken for Two Step Homing..
	map<int, Shape*> ShapesForBaseAlignment; // Shape Pointers taken for Base alignment
	map<int, Shape*> ShapesForIntersectionLine;
	map<int, Shape*> ShapesForIntersectionPoint;
	map<int, Shape*> ShapesForIntersectionPointFromLines;
	map<int, GenericProbeOffset*> GenericProbeOffsetCollection;

#pragma endregion

#pragma region Private Flags Used
	
	//Flags used for Graphics.
	bool VShape_Updated, VSelectedShapeUpdated;
	bool RShape_Updated, RSelectedShape_Updated;
	bool VMeasurement_Updated, VSelectedMeasurement_Updated;
	bool RMeasurement_Updated, RSelectedMeasurement_Updated;
	bool VDXFShape_updated, RDXFShape_updated, RDXFMeasure_updated;
	bool GridmoduleUpdated;
	bool PartprogramShape_Updated;

	bool ResetRcadRotation; //3D flags..
	
	bool undone, Shapeflag; //Holds whether Undo is done or not.
	bool ShapePasted;		
		
	bool SurfaceON, ProfileON, AxillaryON; //Light Status..
	bool CaptureCad, CaptureVideo;

	bool VideoGraphicsUpdating, RcadGraphicsUpdating, DxfGraphicsUpdating;
	bool VideoGraphicsUpdateQueue, RcadGraphicsUpdateQueue, DxfGraphicsUpdateQueue;

	bool UpdteVgp_UcsChange, UpdteCadgp_UcsChange;

	bool Xaxis_Highlighted, Yaxis_Highlighted;

	int Video_Cnt, Rcad_Cnt, Dxf_Cnt, Save_Cnt;
	double Alpha_Value;
	
	double RcadTransformationMatrix[16], CloudPtZExtent[2];
	bool ChangeRcadViewOrien;
	bool CreateBFSurfaceActions;

	bool Left_Mouse_Down, Right_Mouse_Down;				//vinod machine move on both click downward (left and right button in any order)
	
#pragma endregion

#pragma region Private Variables
	//Holds current Light indensity.
	std::string lightProperty;
	int UCSid; //UCSid of current UCS//
	std::string CurrentShapeName; //Current Shape Name..
	int currentwindow;//Holds current active window.
	HWND HiddenPictureHandle; //Hidden Picture box window handlde..

#pragma endregion

#pragma region Variables Pointer to Shape Dim etc
	Shape *nearestShape, *DXFnearestShape; //holds the nearest shape that is to be highlighted
	DimBase *nearestmeasure, *CurrentSelectedDim; //holds nearest measurement.
	Vector *lastsnap, *DXFlastsnap; //holds the last snap point, DXF dimension End point notification.
	Vector middlept, middlepttemp; // Used in Copy Paste...
	Vector currentDRO; //Holds current DRO position.
	Vector SnapPointModePoint; //last Snap Point...
	MouseHandler* currentHandler; //the current Handler object which is attached
	
	GridModule* MyGridModule; // GridModule///
	GRectangle* GridCellShape;
#pragma endregion

#pragma region Other Moudle Instances Pointers..	
	HelperClass* HelperInst;
	//Calibration calculation instance..
	CalibrationCalculation* cbcInstance;
	EdgeDetectionSingleChannel* Edgedetection;
	RBF* bestFit;
	FixedToolCollection* FixedToolHandler; // handler for the fixed graph
	DXFExpose* DxfexposeObj;
	//Thread Calc Instance..
	ThreadCalcFunctions* thcfInstance;
	//Focus calc Instance..
	FocusFunctions* ffinstance;
	//partprogrma
	PartProgramFunctions* ppinstance;
	CalibrationText* CalibTextIns;
#pragma endregion

#pragma region Graphics Thread And Draw Functions
	friend void VideoGraphics_Thread(void *anything); //Video Graphics...
	friend void RcadGraphics_Thread(void *anything); //Rcad Thread function
	friend void DxfGraphics_Thread(void *anything); //DXF graphcis.

	//OpenGL list building functions
	void buildVideoShapeList(RCollectionBase& ShapeCollection, double wupp);
	void buildRcadShapeList(RCollectionBase& ShapeCollection, double wupp);
	void buildVideoSelectedShapeList(RCollectionBase& ShapeCollection, double wupp);
	void buildRcadSelectedShapeList(RCollectionBase& ShapeCollection, double wupp);

	void buildVideoMeasurementList(RCollectionBase& DimCollection, double wupp);
	void buildRcadMeasurementList(RCollectionBase& DimCollection, double wupp);
	void buildVideoSelectedMeasurementList(RCollectionBase& DimCollection, double wupp);
	void buildRcadSelectedMeasurementList(RCollectionBase& DimCollection, double wupp);

	void buildVideoDXFList(list<Shape*> &ShapeCollection, list<DimBase*> &DimCollection, double wupp);
	void buildDxfDXFList(list<Shape*> &ShapeCollection, list<DimBase*> &DimCollection, double wupp);

	void buildVideoSnapPointNameList(double wupp);
	void buildRcadSnapPointNameList(double wupp);
	double getZCoordinateOfCurrentShape();

	//draw functions..
	void drawhlrect(double x, double y, double size, bool trans = true,double z = 0);
	void drawSnapPoint(bool flag); //Snap Point Text..
	void drawSnapPoint_FT(); //draw the snap points of highlighted shape during fastrace drawing..
	void DrawExtensionForImplicitSnap(Shape* s1, Shape* s2, Vector *v);//draw the extension line for snap points
	std::string HighlightedPt2String(double x, double y, double z); //Converting Double to string pointer.
	void SetAxisHighLighted(Shape* CShape);
	bool GetPointOnHighlightedShape(double x, double y);

	void ExtendShapes();
	bool RotateSelectedTextShape();
#pragma endregion
	
#pragma region Instance And Initialisation Functions
private:
	static RCadApp* _instance;//static Instance of this class.
	//Constructor and destructor...//
	RCadApp(std::string = ""); 
	~RCadApp();
	//Initialisation Functions..
	void InitialiseAllCallbacks();
	void InitialiseAllMutex();
	void InitialiseAllInstance();
	void InitialiseCurrenthandlerSettings();
	void InitialiseGraphicsUpdateFlags(bool flag = false);
	void InitialiseShapeHighlightedFlags();
	void InitialiseFrameGrabSettings();
	void ResetVideoShapeUpdateFlag();
	void DeleteRelatedMeasurement(ShapeWithList* Shp);
	RapidToggleProperty<bool> ClearGraphicsOnVideo;
public:
	//Expose Instance to UI application.
	static RCadApp* getInstance();
	static void CreateMainDllInstance(char* path);
	static void deleteInstance(); //delete the Instance of Main application.
	static void SetValuesOfBCXBCY(double x, double y);					//vinod..for hob checker getc_axis_xy..
	static void set_flag_for_measurement_mode(bool flag);				//vinod..if set then m/c not move on l+r click
	static void Set_IsDelphiCam(bool flag);								//vinod..dont draw cross on drawing,, it looks clumsy
	void CloseSoftware();
	void AddMCSduringLoad();
	void InitialiseOtherModules1();
	void InitialiseOtherModules2();
	bool Initialise_VideoWindow(HWND handle, HWND hiddenwindow, int width, int height, int camWidth = -1, int camHeight = -1, bool ConnecttoRapidCam = true); //
	void Initialise_RcadWindow(HWND handle, int width, int height);
	void Initialise_DxfWindow(HWND handle, int width, int height);
	void Initialise_OverlapImgWindow(HWND handle, int width, int height);
	void StartVideoThread();
	void StartRcadThread();
	void StartDxfThread();
	void SetHiddenWindowHandle(HWND handle);
	void SetImageSize_ED();
	void SetCurrentWindow(int n);
	void ResetCameraDistortionParameter(double distortionFactor, long Xoffset, long Yoffset);
#pragma endregion

public:
#pragma region Public Properties Used
	RapidProperty<bool> CameraConnected;
	RapidToggleProperty<bool> LinearModeFlag;
	RapidToggleProperty<bool> PCDMeasureModeFlag;
	RapidToggleProperty<bool> CCPMeasureModeFlag;
	RapidProperty<bool> IsProbePresent;

	RapidProperty<bool> EditmeasurementMode;
	RapidProperty<bool> SnapPointHighlighted;
	RapidProperty<bool> DrawSnapPointOnHighlightedShape;
	RapidProperty<bool> DXFSnapPointHighlighted;
	RapidProperty<bool> ShapeHighlighted;
	RapidProperty<bool> DXFShapeHighlighted;
	RapidProperty<bool> MeasurementHighlighted;
	RapidProperty<bool> GridCellHighlighted;
	RapidProperty<bool> MouseClickAndDragMode;
	RapidProperty<bool> MousedragwithShape;
	RapidProperty<bool> DigitalZoomMode;
	RapidProperty<bool> UseAvImage;

	RapidProperty<int>RepeatCount; //No. of times a framegrab should be repeated to get good results. Used in OneShot (27 Nov 2018) R A N

	RapidProperty<bool> FullWindowDigitalZoom;

	RapidProperty<bool> ApplyLineArcSeperationMode;
	RapidProperty<bool> OutlierFilteringForLineArc;
	RapidProperty<bool> DigitalZoomModeForOneShot;
	RapidProperty<bool> IsOldPPFormat;
	RapidProperty<bool> PPLoadSuccessful;
	RapidProperty<bool> DemoMode;
	RapidProperty<int> PixelOffset_X;
	RapidProperty<int> PixelOffset_Y;
	RapidProperty<int> SkipPixelWidth;
	RapidProperty<int> SkipPixelHeight;
	RapidProperty<int> NoOfFramesToAverage;

	RapidProperty<int> XPositionStart_Right;
	RapidProperty<int> XPositionStart_Left;
	RapidProperty<int> YPositionStart_Top;
	RapidProperty<int> YPositionStart_Bottom;
	RapidProperty<double> XMultiplyFactor_Right;
	RapidProperty<double> XMultiplyFactor_Left;
	RapidProperty<double> YMultiplyFactor_Top;
	RapidProperty<double> YMultiplyFactor_Bottom;
	RapidProperty<bool> UCSSelectedLineDirAlongX;
	RapidProperty<bool> UpdateGrafixFor3DSnapPoint;
	RapidProperty<bool> ChangeRcadViewOrienFor3D;
	RapidProperty<bool> ApplyMMCCorrection;
	RapidProperty<bool> ApplyFilterCorrection;
	RapidProperty<bool> OuterCorrection;
	RapidProperty<bool> RotateGrafix;
	RapidProperty<int> FundamentalShapes;
	RapidProperty<int> FilterCorrectionFactor;
	RapidProperty<int> FilterCorrectionIterationCnt;
	RapidProperty<int> SelectedAlgorithm;
	RapidProperty<bool> UseGotoPostionInAction;
	RapidProperty<bool> CNCAssisted_FG; //Default for FG is automatic run when CNC is on; set this to true if FG selection is to be manual. 
	RapidProperty<bool> Force_AutoFG_InAssistedMode; //Set this to true if you want the current FG to run automatically in CNC-Assisted mode. 
	RapidProperty<bool> ShiftUpperCrossHair;

	//RapidProperty<int> Channel_No;
	//RapidProperty<bool> DoAverageFilter;
	std::list<Shape*> CurrentCopiedShapeList; //Current pasted shape list.. To handle mouse move.
	std::map<int,double*> ProbePathPoints;
	int ImageAction_Count;

	BYTE* pixels; // [42000000]; //Made larger from 240 Lakhs
	double* pixelvalues; // [42000000]; //made larger. R.A.N 15 Mar2014 for new cam
	BYTE* pixelBytes; // [12000000];
	
	//Added on 2 March 2012...Narayanan Sir asked for settings for focus depth: box width
	//Generalised.. you can have anything!
	struct MAINDLL_API FrmaeGrabDefaultProperty
	{
	public:
		int FrameGrab_MinWidth, FrameGrab_MinHeight;
		int FrameGrab_MaxWidth, FrameGrab_MaxHeight;
		int FrameGrab_DefaultWidth, FrameGrab_DefaultHeight;
		int FrameGrab_CurrentWidth, FrameGrab_CurrentHeight;
		void Set_Width(int minwidth, int maxwidth, int dwidth)
		{
			FrameGrab_MinWidth = minwidth;
			FrameGrab_MaxWidth = maxwidth;
			FrameGrab_DefaultWidth = dwidth;
		}
		void Set_Height(int minheight, int maxheight, int dheight)
		{
			FrameGrab_MinHeight = minheight;
			FrameGrab_MaxHeight = maxheight;
			FrameGrab_DefaultHeight = dheight;
		}
		void ResetCurrentValuetoDefault()
		{
			FrameGrab_CurrentWidth = FrameGrab_DefaultWidth;
			FrameGrab_CurrentHeight = FrameGrab_DefaultHeight;
		}
	};
	FrmaeGrabDefaultProperty FrameGrab_AngularRect;
	FrmaeGrabDefaultProperty FrameGrab_Arc;
	FrmaeGrabDefaultProperty FrameGrab_Circular;
	FrmaeGrabDefaultProperty FrameGrab_FixedRectangle;
	FrmaeGrabDefaultProperty FrameGrab_ScanMode;
	FrmaeGrabDefaultProperty FrameGrab_FocusDepthMultiBox;

	RapidProperty<bool> FGDirection_AngularRect;
	RapidProperty<bool> FGDirection_Circular;
	RapidProperty<bool> FGDirection_Arc;
	RapidProperty<int> FGDirection_Rectangle;
	RapidProperty<int> FGDirection_FixedRectangle;
	RapidProperty<int> DMMMeasureTypeLinear;
	RapidProperty<int> SurfaceFgtype;
	//RapidProperty<bool> Align_by_CG;
	

	RapidProperty<bool> DefaultCrosshairMode;
	RapidProperty<bool> HideGraphicsOnVideoSettingsMode;
	RapidProperty<bool> SaveCrossHairWithImage;
	
	RapidProperty<bool> IsCNCMode;
	RapidProperty<bool> ShowHideCrossHair;
	RapidProperty<bool> ShapeHighlightedForImplicitSnap;
	RapidProperty<bool> MouseOnVideo;
	RapidProperty<bool> ProfileScanMode;
	
	RapidProperty<bool> MeasurementMode;
	RapidProperty<bool> RcadZoomExtents;
	RapidToggleProperty<bool> RcadGraphicsMoveWithDRO;

	RapidProperty<bool> DrawWiredSurface;
	RapidProperty<bool> RcadWindow3DMode;
	RapidProperty<bool> AllowTransparencyTo3DShapes;
	RapidProperty<bool> DrawCylinderdisks;
	RapidToggleProperty<bool> ThreeDRotationMode;

	RapidProperty<bool> TriangulateCpSurface;
	RapidProperty<bool> UseMultiThreads_Surfacebuild;
	RapidProperty<int> TriangleStepLength;
	RapidProperty<int> PointClusterSize;
	RapidProperty<double> CloudPtdistance;
	RapidProperty<bool> CreateSurfaceForClosedSurface;
	RapidProperty<bool> BestFitSurface_pts_around_xy_plane;
	RapidProperty<double> BestFitSurface_PtsFactor;
	

	RapidToggleProperty<bool> ShowImplicitIntersectionPoints;
	RapidToggleProperty<bool> ShowExplicitIntersectionPoints;
	RapidToggleProperty<bool> ShowMeasurementOnVideo;
	RapidToggleProperty<bool> ShowDXFShapesOnVideo;
	RapidToggleProperty<bool> ShowHideFGPoints;
	RapidToggleProperty<bool> SigmaModeFlag;
	RapidToggleProperty<bool> ScaleViewMode;
	RapidToggleProperty<bool> HatchedCrossHairMode;
	RapidProperty<bool> KeepGraphicsStraightflag;
	RapidProperty<bool> ShowNameTagFlag;
	RapidProperty<bool> UseGotoPointsforProbeHitPartProg;
	RapidToggleProperty<bool> HideGraphicsOnVideo;
	

	RapidProperty<double> X_AxisProbeRadiusLeft;
	RapidProperty<double> X_AxisProbeRadiusRight;
	RapidProperty<double> X_AxisProbeRadiusStrt;
	RapidProperty<double> X_AxisProbeArmLengthX;
	RapidProperty<double> X_AxisProbeArmLengthY;
	RapidProperty<double> X_AxisProbeArmLengthZ;


	RapidProperty<double> Y_AxisProbeRadiusTop;
	RapidProperty<double> Y_AxisProbeRadiusBottom;
	RapidProperty<double> Y_AxisProbeArmLengthX;
	RapidProperty<double> Y_AxisProbeArmLengthY;
	RapidProperty<double> Y_AxisProbeArmLengthZ;

	RapidProperty<double> StarProbeRightOffsetX;
	RapidProperty<double> StarProbeRightOffsetY;
	RapidProperty<double> StarProbeRightOffsetZ;
	RapidProperty<double> StarProbeBackOffsetX;
	RapidProperty<double> StarProbeBackOffsetY;
	RapidProperty<double> StarProbeBackOffsetZ;
	RapidProperty<double> StarProbeFrontOffsetX;
	RapidProperty<double> StarProbeFrontOffsetY;
	RapidProperty<double> StarProbeFrontOffsetZ;
	RapidProperty<double> StarStraightOffsetX;
	RapidProperty<double> StarStraightOffsetY;
	RapidProperty<double> StarStraightOffsetZ;
	
	RapidEnums::PROBETYPE CurrentProbeType;
	RapidEnums::STYLUSTYPE CurrentStylusType;
	RapidProperty<bool> StylusPolarity;

	RapidProperty<double> ProbeRadius;
	RapidProperty<double> MasterProbeRadius;
	RapidProperty<int> MeasurementNoOfDecimal;
	RapidProperty<int> MinimumPointsInEdge;
	RapidProperty<int> PixelNeighbour;
	RapidProperty<double> RotaryCalibrationAngleValue;
	RapidProperty<double> RotaryCalibrationIntercept;
	RapidProperty<double> RotaryFixtureOffset;
	RapidProperty<double> Rotary_YaxisOffset;
	RapidProperty<bool> SmartMeasureTypeMode;
	//Flag used to toggle handle action.
	RapidToggleProperty<bool> SetCrossOnVideo;
	RapidToggleProperty<bool> ShowProbeDraw;
	RapidProperty<bool> AutomaticTwoStepHoming;
	RapidProperty<int> ReferenceDatumNameCount_GDT;
	RapidProperty<int> VideoFrameRate;
	RapidProperty<int> AngleMeasurementMode; 
	RapidProperty<int> RunoutFluteCount;
	RapidProperty<int> RunOutType;
	RapidProperty<double> RunoutAxialRadius;
	RapidProperty<bool> MasteProbeCalibrationMode;
	RapidProperty<bool> PixelWidthCorrectionForOneShot;
	RapidProperty<double> PixelWidthCorrectionQuadraticTerm;
	RapidProperty<double> PixelWidthCorrectionLinearTermX;	
	RapidProperty<double> PixelWidthCorrectionLinearTermY;	
	RapidProperty<double> PW_C0_Rad;//R A..................
	RapidProperty<double> PixelWidthX0;//vinod..................
	RapidProperty<double> PixelWidthY0;//vinod...................

#pragma endregion

#pragma region  Public Variables
	bool CheckStlHighlightedPt, StlHighlightedPointFlag, CriticalShapeStatus, MeasurementcolorInGradient, TruePositionMode;
	bool HideShapeMeasurementDuringPP;
	double AngleTolerance, PercentageTolerance, BestFitAlignment_Resolution;
	Vector StlHighlightedPt, GotoPostionForAction;
	// thread maesure action..........
	AddThreadMeasureAction* PointerToShapeCollection;
	//lock the list iteration operation when mouse hilighting/building list
	HANDLE DxfMutex;
	HANDLE ShapeMutex, SelectedShapeMutex;
	HANDLE MeasureMutex, SelectedMeasureMutex;
	
	RAction *LastAdded_Action;

	std::string currentPath, DatabaseFolderPath, SurfaceCamSettings_default, ProfileCamSettings_default, RCam3_D_GainSettings, RefDotCamSettings;
	list<Shape*> CurrentFTShapeCollection;
	int Offset_Normal[8]; //Offset values for left, top, right and bottom portions of the cross-hair when it is straight
	int Offset_Skewed[8]; //Offset values for left, top, right and bottom portions of the cross-hair when we have UCS with an angle cross-hair


	Shape *ReferenceDotShape, *Vblock_CylinderAxisLine, *SecondReferenceDotShape, *ReferenceLineShape;
	int ToolAxisLineId, NumberOfReferenceShape, BestFitAlignment_SampleSize;
	//Boolean For Probe home mode....
	bool CurrentExtensionFlag, UseLocalisedCorrection, IsPartProgramPause;
	double ColorThreshold, ProfileScanMoveToNextFrameFactor;
	bool Derivedmodeflag, RcadBackGround_White, EnableTriangulationLighting, DiscreteColor; 
	bool SaveImageFlag, Continuous_ScanMode;
	bool ProbeEnableFlag;
	bool ProbeSmartMeasureflag;
	bool AutoProbeMeasureFlag;
	bool DigitalZoomIsOn;
	bool DoNotSamplePts;
	bool AddAction_PPEditMode, RuninCNCMode;
	int LastActionIdForPPEdit, Color_Channel, SamplePtsCutOff, BestFit_PointsToBundle, Dots_PointsMinCount, Dots_PixelCount;
	bool OneTouchProbeAlignment, ArcBestFitType_Dia;
	bool RepeatSGWidth, PlaceDetail,PlaceTable;
	RapidProperty<bool> ShowCoordinatePlaneFlag;
	RapidProperty<bool> ShowPictorialView;
	bool FloatDXFwithMouseMove;
	RapidProperty<double> RefDot_Diameter;

	RapidEnums::MCSTYPE CurrentMCSType;

	bool DXFMouseFlag, RCADMouseFlag, VideoMouseFlag;
	string LastSelectedMagLevel;
	//flag to show automated mode is on.....
	RapidToggleProperty<bool> AutomatedMeasureModeFlag;
	std::list<Shape*> Automated_ShapeTypeList;
	std::list<DimBase*> Automated_MeasurementTypeList;

	//Holds unit per pixel value of windows.
	double VideoWupp, CameraDistortionFactor;
	long CamXOffSet, CamYOffSet;
	//flag to hold nudge, rotate shapes.
	bool NudgeToolflag, NudgeThreadShapes, RotateDXFflag, NudgeShapesflag, DrawRectanle_ImageDelay;
	bool dontUpdateGraphcis;
	//Holds angle mode like degree, radian and decimal to display.
	
	//flag to hold control button is on or off.
	bool ControlOn,ShiftON, AltON;
	bool ActionSavingFlag;
	bool AddingUCSModeFlag;
	bool ThreadRunning;
	//int currCamWidth;	//R A N
	//int currCamHeight;	//R A N
	bool InsertStaticImageinVideo;
	BYTE staticImageBytes[42000000]; //Holding static BG image when camera not connected.
	BYTE BinarisedImage[42000000];

	//Video, Cad and Dxf graphics Instances...//
	RGraphicsWindow* Video_Graphics;
	RGraphicsWindow* Rcad_Graphics;
	RGraphicsWindow* Dxf_Graphics;
	
	//static variables to hold current camera width/height // convert this to maindll accessible variable for calculations
	int currCamWidth = 800, VideoDisplayWidth = 800;
	int currCamHeight = 600, VideoDisplayHeight = 600;
	int CurrentCameraType = 0;

	std::string PrevLightCommand;
	std::string PrevCameraSetting;

#pragma endregion

#pragma region Error handling
	//Set and raise error Happened..
	void SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname);
#pragma endregion

#pragma region GraphicsUpdate Functions
	//Calculating Center of window during DRO changes..
	void centerScreen(double x, double y,double z, bool updateVideo = true);
	//Calculating Center of window during DRO changes..
	void SetcenterScreen_Video(double x, double y,double z);
	//Calculating Center of window during DRO changes..
	void SetcenterScreen_Rcad(double x, double y,double z);
	//Calculating Center of window during DRO changes..
	void SetcenterScreen_Dxf(double x, double y,double z);
	//Update Center of the screen
	void centerScreen_Update(double x, double y,double z);
	//Update Mouse position for DRO change...
	void UpdateMousePosition(double x, double y, int window);
	//Function to get Current DRO.
	void setCurrentDRO(double x, double y, double z, double r);
	//Expose current DRO.
	Vector& getCurrentDRO();
	//Update Video graphics.
	void update_VideoGraphics(bool GiveDelay = false);
	//Update RWindow graphcis.
	void update_RcadGraphics(bool GiveDelay = false);
	//Update DXF graphics.
	void update_DxfGraphics(bool GiveDelay = false);
	//Wait Video graphics update..
	void Wait_VideoGraphicsUpdate();
	//Wait Rcad graphics update..
	void Wait_RcadGraphicsUpdate();
	//Wait Dxf graphics Update...
	void Wait_DxfGraphicsUpdate();
	//Wait Save Actions..
	void Wait_SaveActionsWorkSpace();
void SaveActions();
	//Update all
	void UpdateGraphicsOf3Window();
	//Set the Flags to Update all.
	void updateAll();
	//Set the Flags to Update all Shape, Measurement Graphics.
	void UpdateShapesChanged(bool updategraphics = true);
	//Set the Flags to Update Shape graphics.
	void Shape_Updated(bool updategraphics = true);
	//Set the Flags to Update Measure Graphics.
	void Measurement_updated(bool updategraphics = true);
	//Set the Flags to Update Selected Shape graphics.
	void SelectedShape_Updated(bool updategraphics = true);
	//Set the Flags to Update Selected Measure Graphics.
	void SelectedMeasurement_Updated(bool updategraphics = true);
	//Entity Update on Rcad window
	void Entity_UpdatedOnRcad(bool updategraphics = true);
	//Set the Flags to Update DXF Shape Graphics.
	void DXFShape_Updated(bool updategraphics = true);
	//Set the Flags to Update DXF Measure Graphics.
	void DXFMeasurement_Updated(bool updategraphics = true);
	//Set the Flags to Update DXF Shape Graphics.
	void DXFShape_Updated_Alignment(bool updategraphics = true);
	//Set the Flags to Update Partprogram Graphics.
	void PartProgramShape_updated(bool updategraphics = true);
	//Calculate and Redraw all the measurements..
	void UpdateAllMeasurements();
	//Clear Shape & measure Selections.
	void ClearShapeMeasureSelections();
	//Clear Shape Selections
	void ClearShapeSelections();
	//Clear Measure Selections
	void ClearMeasureSelections();
	//show or Hide DXF shapes on Video.
	void ShowHideDXFOnVideo();
	//Toggle surface..
	void ToggleWiredLoopMode(bool flag);
	//Toggle transparency..
	void ToggleTransparencyMode(bool flag);
	//Toggle cylinder disk draw..
	void ToggleCylinderDiskDrawMode();
	//update transparency level of the shape..
	void SetRapidShapeTransparency(double Alpha_val);
	//Update Shapes in Shape list..
	void RecalculateAndUpdateShapes(RapidEnums::SHAPETYPE ShType);
	//Show / Hide graphcis on Video.
	void hideGraphics_OnVideo(bool flag);
	//update video viewport...
	void UpdateVideoViewPortSettings();
	//Update Graphics for Part program.
	void updatePPGraphics(bool flag = false);
	//Render Video window graphics
	void Draw_VideoWindowGraphics();
	//Render Rcad Window graphics
	void Draw_RcadWindowGraphics();
	//Render Rcad Window graphics
	void Draw_DxfWindowGraphics();
	// Show Hide Coordinate Planes..
	void ShowHideCoordinatePlanes();
	//Updating the center of rotation for Rcad 3D rotation
	void UpdateCenterOfRotation();

	void SetRcadTransformationMatrix();

#pragma endregion

#pragma region Mouse Event Handling
	 
#pragma region Video Window Mouse Event Handling
	 void OnLeftMouseDown_Video(double x, double y); 
	 void OnRightMouseDown_Video(double x, double y); 
	 void OnMiddleMouseDown_Video(double x, double y); 
	 void OnLeftMouseUp_Video(double x, double y); 
	 void OnRightMouseUp_Video(double x, double y); 
	 void OnMiddleMouseUp_Video(double x, double y); 
	 void OnMouseMove_Video(double x, double y);
	 void OnMouseWheel_Video(int evalue);
	 void OnMouseEnter_Video();
	 void OnMouseLeave_Video();
#pragma endregion

#pragma region Rcad Window Mouse Event Handling
	 void OnLeftMouseDown_Rcad(double x, double y); 
	 void OnRightMouseDown_Rcad(double x, double y); 
	 void OnMiddleMouseDown_Rcad(double x, double y); 
	 void OnLeftMouseUp_Rcad(double x, double y); 
	 void OnRightMouseUp_Rcad(double x, double y); 
	 void OnMiddleMouseUp_Rcad(double x, double y); 
	 void OnMouseMove_Rcad(double x, double y);
	 void OnMouseWheel_Rcad(int evalue);
	 void OnMouseEnter_Rcad();
	 void OnMouseLeave_Rcad();
#pragma endregion

#pragma region DXf window Mouse Event Handling
	 void OnLeftMouseDown_Dxf(double x, double y); 
	 void OnRightMouseDown_Dxf(double x, double y); 
	 void OnMiddleMouseDown_Dxf(double x, double y); 
	 void OnLeftMouseUp_Dxf(double x, double y); 
	 void OnRightMouseUp_Dxf(double x, double y); 
	 void OnMiddleMouseUp_Dxf(double x, double y); 
	 void OnMouseMove_Dxf(double x, double y);
	 void OnMouseWheel_Dxf(int evalue);
	 void OnMouseEnter_Dxf();
	 void OnMouseLeave_Dxf();
#pragma endregion

#pragma endregion

	 Shape* AddShapeForRightClick(bool SelectShape = true);

#pragma region keyboard key press events
	//Handling Key up event.
	void keyUp(UINT nChar,UINT nRepCnt, UINT nFlags, int window=0);
	//Handling Key down event.
	void keyDown(UINT nChar,UINT nRepCnt, UINT nFlags, int window=0);
#pragma endregion
	
	void SetGenericProbeOffset(int id, double offsetx, double offsety, double offsetz, double radius);
	void GetGenericProbeOffset(int id, double *offsetx, double *offsety, double *offsetz);
	double GetGenericProbeRadius(int id);

#pragma region Instance of other Modules
	//Expose the Edge detection Instance
	EdgeDetectionSingleChannel* getEdgeDetection();
	//Expose Helper Class Inst..
	HelperClass* getHelper();
	//Expose Calibration Calc Instance..
	CalibrationCalculation* getcbcInst();
	//Expose Thread Calc Instance..
	ThreadCalcFunctions* gettcfnInst();
	//Expose Focus Calc Instance..
	FocusFunctions* getffnInst();
	//Expose partprogram Instance..
	PartProgramFunctions* getppnInst();
	//Expose specal tool handler.
	FixedToolCollection* getSpecialToolHandler();
	//Expose Dxf Expose class..
	DXFExpose* getDXFexobj();
	//Expose instance for Best fit dll.
	RBF* getBestfit();
	CalibrationText* getctInst();
#pragma endregion
		
#pragma region All Collections of Current UCs
	//Expose Action List
	RCollectionBase& getActionsHistoryList();
	//Expose all UCS List.
	RCollectionBase& getUCSList();
	//Expose all Shape list of current selected UCS.
	RCollectionBase& getShapesList();
	//Expose all Shape list of current UCS during pp to path.
	RCollectionBase& getPPShapesList_Path();
	//Expose all Measure list of current UCS during pp to path.
	RCollectionBase& getPPMeasuresList_Path();
	//Expose Dimension list for current UCS.
	RCollectionBase& getDimList();
	//Expose all DXF list loaded from file.
	RCollectionBase& getDXFList();
	//Expose all selected shape from current UCS.
	RCollectionBase& getSelectedShapesList();
	//Expose selected Dimension list for current UCS.
	RCollectionBase& getSelectedDimList();
	//Expose all DXF list loaded from file.
	RCollectionBase& getSelectedDXFList();
	//Handler for current Selected UCS.
	UCS& getCurrentUCS();
	//Get window handle by passing integer. 0 is Video handle, 1 is RWindow handle, 2 is DXF window handle.
	RWindow& getWindow(int windowno);
	//Get UCS by passing id as parameter.
	UCS* getUCS(int id);
#pragma endregion

#pragma region Handler And enumeration Types
	//Draw handler enums..
	RapidEnums::RAPIDHANDLERTYPE CURRENTHANDLE;
	RapidEnums::RAPIDHANDLERTYPE DUMMYHANDLE;
	RapidEnums::RAPIDHANDLERTYPE PREVIOUSHANDLE;
	RapidEnums::RAPIDDRAWHANDLERTYPE CURRENTRAPIDHANDLER;
	//Smart Measurement Enum type..
	RapidEnums::SMARTMEASUREMENTMODE SmartMeasureType;
	RapidEnums::SMARTMEASUREMENTMODE PreviousSMhandler;
	//Current Machine enum types..
	RapidEnums::RAPIDMACHINETYPE CurrentMahcineType;
	//current lense type...
	RapidEnums::RAPIDLENSETYPE CurrentLenseType;
	//All Graphics window Cursor enum type..
	RapidEnums::RWINDOW_CURSORTYPE VideoMpointType;
	//Projection enum type..
	RapidEnums::RAPIDPROJECTIONTYPE ProjectionType;
	RapidEnums::RAPIDPROJECTIONTYPE UCSProjectionType;
	//Measurement unit type..
	RapidEnums::RAPIDUNITS MeasureUnitType;
	//Set the current handler.
	void changeHandler(RapidEnums::RAPIDHANDLERTYPE HANDLER, bool CalledInternally = false);
	//Setting Mouse handler to current handler.
	void setHandler(MouseHandler* handler);
	//Add Handler of non toggle type..
	void SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE HANDLER);
	//Expose current handler.
	MouseHandler* getCurrentHandler();
	//Set the Current Smart Measurement handler
	void SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE SMhandler);
	//Set the internal handler for the smart measurement
	MouseHandler* SetCurrentSMFGHandler(RapidEnums::RAPIDHANDLERTYPE HANDLER);
	//Set Current Smartmeasure type..
	void SetMeasurementType(RapidEnums::SMARTMEASUREMENTMODE d);
	//Handle type of Measurement for Circle and Arc - Radius, Diameter, Arc length and center.
	bool SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE d, int type);
	//Function to set video window cursor type.
	void SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE Ctype);
	void SetRcadCursorType(RapidEnums::RWINDOW_CURSORTYPE Ctype);
	void SetDxfCursorType(RapidEnums::RWINDOW_CURSORTYPE Ctype);
	//Set the current measurment unit...
	void SetMeasurementUnit(RapidEnums::RAPIDUNITS d);
#pragma endregion

#pragma region Highlighted Shape, Measure, Point
	//Holds pointer of Highlighted shape.
	Shape* highlightedShape();
	//Get pointer of Highlighted point.
	Vector* highlightedPoint();
	//Get pointer of Highlighted measure.
	DimBase* highlightedmeasure();
	void SetSnapPointHighlighted(double x, double y, double z);
#pragma endregion

#pragma region Listening to Collections
	//Item added event..
	virtual void itemAdded(BaseItem* item, Listenable* sender);
	//Item removed event..
	virtual void itemRemoved(BaseItem* item, Listenable* sender);
	//Item Changed event..
	virtual void itemChanged(BaseItem* item, Listenable* sender);
	//Selection changed event..
	virtual void selectionChanged(Listenable* sender);
#pragma endregion

#pragma region UCS operations
	//Function to Add new UCS when ever Axis of DRO set to Zero.
	void AddNewUCS_DROclear();
	//Change one UCS to other UCS by passing ID.
	void changeUCS(int id, bool addaction = false);
	//Graphical shifting from MCS to UCS.
	void MCStoUCS(UCS& cucs, bool updategraphics = true);
	//Graphical shifting from UCS to MCS.
	void UCStoMCS(UCS& cucs, bool updategraphics = true);
	//Delete currently selected UCS.
	void deleteUCS();
	//Copy all shapes from one UCS to Other UCS.
	void CopyAllShapesUCS_UCS(UCS *Source, UCS *Destination);
	//Copy selected shapes.
	void CopySelectedShapeUCS_UCS(UCS *Source, UCS *Destination);
	//Transform shapes from collection according to transform matrix.
	void CopyShapes(RCollectionBase& Source, double* transform);
	//Copy Shapes from current UCS.
	void CopyShape();
	void CopyShapesToUCS4(RCollectionBase& Source, double* transformc, double* transform);
	//Past shapes to destination UCS.
	void PasteShape();
	//Fucntion to Handle Shift orgin of UCS center.
	void ShiftOrigin(double *transform);
	//Set UCS angle from UI.
	void SetUCSRotateAngle(double angle);//
	//Update graphics for UCS Change..
	void UpdateGraphics_UCSChange();
	//Set required flags for Ucs change graphics udpate..
	void SetGraphicsUpdateFlag_UCSChange();

	void RotateRcadFor3DMode();

	void TakeFrameGrabForSphere();
#pragma endregion

#pragma region Shape and Measurement Selection, Delete
	//Select shape from shape list by passing id.
	void selectShape(int id, bool multi);
	//Select multiple shapes..
	void selectShape(list<int> *PtIdList);
	//Delete selected shape.
	void deleteShape();
	void deleteReferenceShape(ShapeWithList* Shp);
	Shape* getHighestParentOfGroup(Shape *s);
	//Delete current drawing shape for Esc key
	void deleteForEscape();
	//Select All...
	bool SelectAll();
	//Deselct  all..
	void DeselectAll(bool flag = true);
	//Set the selected shape as reference shape..
	void SetShapeAsReference_Shape();
	//Set the selected shape for two step homing..
	void SetShapeForTwoStepHmoing(bool TwoStepHoming);
	void CreateReferenceSphere();
	//Master Cylinder to be stored in Hob Checker
	void CreateReferenceCylinder();

	void CreateRefernceLineShape();
	void SetShapeForReferenceDot(bool FirstReferenceDot = true);
	void SetShapeForUCSBasePlane(map<int, Shape*> BasePlaneShape);
	//Set shape(basically 2D line) for checking if its close to horizontal or not
	void SetShapeAsRotaryEntity();
	//Select Measurement from measure list by passing ID.
	void selectMeasurement(int id, bool multi, bool updateVideoGraphics = true);
	//Select multiple measurements..
	void selectMeasurement(list<int> *PtIdList);
	//Delete selected measurement.
	void deleteMeasurement();
	//Setting flag for Changing position for Measurement.
	void setEditmeasurment();
	//Changing measurement mode from normal <-> linear
	void ChangeMeasurementMode();
	//Select shape by drawing rectangle.
	void selectShapesWindow(Vector &p1, Vector &p2, bool multi = false);
	//Select Measurement by drawing rectangle.
	void selectMeasurewindow(Vector &p1, Vector &p2, bool multi = false);
	//Select Fg points of selected shape using rectangle.
	void selectFGPointsWithTransformation(Vector &p1, Vector &p2, std::list<int> *PtIdList, double *transMatrix);

	void selectFGPointsFromShapes(Vector &p1, Vector &p2, FramegrabBase* cAction);
	void selectFGPointsInCircularFrameFromShapes(double r1, double r2, double* center, double *endPoint1, double *endPoint2, FramegrabBase* cAction);
	void selectFGPointsInAngularRectFrameFromShapes(double* point1, double* point2, double width, FramegrabBase* cAction);

	//select Fg points.. from front end dgv
	void selectFGPoints(list<int> *PtIdList);
	//delete selected points list
	void DeletePointList_SelectedShape(list<int> *PtIdList);
	//delete selected fg points list
	void deleteFGPoints();
	//Removing all Points from selected shapes that are added to shapes.
	void ResetthePoints_selectedshape();
	//Change color for selected shape.
	void SetshapeColor(int r, int g, int b);
	//Change color for Selected measurement
	void SetMeasurementColor(int r, int g,int b);
	//Change draw mode type of partial shape.
	void ChangePartialShapeDrawMode(int type);
	//Show FG points
	void ShowFGPoints(bool flag);
	//Change Measurement Font Size...
	void SeMeasurementFontSize(int Fsize);
#pragma endregion
	
#pragma region Undo Redo Actions Mag Home And ClearALL
	//used to add an action
	void addAction(RAction* action);	
	//used to undo actions.
	bool Undo();
	//undo iterate..
	bool undoAgain();
	//undo fastrace..
	bool undoforFT(int cnt);
	//used to redo actions.
	bool Redo();	
	//Clear All the Graphcis and UCS. It is like software start up.
	void clearAll();
	//Clear all flags..
	void ClearAllModes();
	//reset all the entity index..
	void ResetAllEntityIndex();
	//Handling home button in window.
	void homeTheWindow(int windowno); //1 for CAD, 2 for UCS
	//Zoom to extents..
	void ZoomToWindowExtents(int wno);
	//Changing magnification for Windows.
	void SetVideoPixelWidth(double pixelwidth_x, double pixelwidth_y);
#pragma endregion

#pragma region Camera Related Fucntion
	//Calling Video format dialog box.
	void cameraFormatDialog();
	//Calling Vodeo source dialog box.
	void videoDialog();
	//flip video..
	void FlipVHorizontal();
	//Flip video
	void FlipVVertical();
	//Freeze the video..
	void FreezeON();
	//Resume the video...
	void ResumeThevideo();
	void UpdateCamera_ZoomLevel(int CamWidth, int CamHeight);
	//Switch on Digital zoom
	void DigitalZoomON(double PixelWidth_X, double PixelWidth_Y);
	//switch of digital zoom
	void DigitalZoomOff(double PixelWidth_X, double PixelWidth_Y);	
	//Connect to Camera. Updated Function that will connect for all situations - normal, digital zoom etc...
	void ConnectToCamera(HWND handle, int camWidth, int camHeight, double ZoomRatio, int DisplayW, int DisplayH, double FrameRate, bool ConnecttoRapidCam);
	//get camera settings..
	std::string getCamSettings();
	std::string getRCam3_DigitalGain();
	//Set camera settings..
	void SetCamProperty(std::string str);
	void SetRCam3_DigitalGain(std::string str);
	void SetCameraProp(int Property, int Value);
	int GetCameraProp(int Property);
	//Hex to decimal conversion
	int Hex_Decimal(std::string hexval);
	//Hex to decimal conversion..
	int Decimal_equivalent(char t);
#pragma endregion

#pragma region Light Properties
	//Set Profile light mode
	void SetProfileLightMode(bool flag);
	//Set surface light Mode..
	void SetSurfaceLightMode(bool flag);
	//Set auxillary light mode..
	void SetAxilaryLightMode(bool flag);
	void SetSurfaceLightFlag(bool flag);
	//get Current profile light mode
	bool getProfile();
	//get current surface light mode..
	bool getSurface();
	// get current auxillary light..
	bool getAxilary();
	//Set current light intensity form application.
	void setLightProperty(char* l);
	//Expose current light intensity. 
	char* getLightProperty();
#pragma endregion

#pragma region Save Image Functions
	//Save Video image with Graphics.
	void SaveImage_WithGraphics(BYTE* destination);
	//Save Video image without Graphics.
	void SaveImage_WithoutGraphics(BYTE* destination);
	//Save Image for one shot..
	void SaveIamge_OneShot(BYTE* destination, int bpp);
	//Getting Rwindow image by passing Width and Height.
	BYTE* getRWindowImage(int width, int height);
#pragma endregion

#pragma region Get Mouse Click Properties
	//Get Click done by current handler by UI for CAD alignment
	int GetClickDone();
	//Get Maximum Click done by current handler by UI for CAD alignment
	int GetMaxClicks();
	//Set Click done to Zero by current handler by UI for CAD alignment
	void Resetthe_Clicks();
	//Set Maximum Click to current handler by UI for CAD alignment
	void SetCLickDone(int click);
#pragma endregion

#pragma region PCD Functions
	//Current PCD Circle..
	Shape* CurrentPCDShape;
	//Add new PCd Circle..
	void AddNewPcdCircle(bool is3D=false);
	//Add circle to the selected PCD circle..
	void AddCircleToCurrentPCD(int id);
	//Remove circle from the selected PCD circle..
	void RemoveCircleFromCurrentPCD(int id);
#pragma endregion

#pragma region Property Set Functions
	//Set Crosshair color..
	void SetCrosshair_Color(double r, double g, double b);
	//Set flexible Crosshair Color..
	void SetFlexibleCrosshair_Color(double r, double g, double b);
	//Set Offset for PArallel lines by UI.
	void SetDerivedShapeParam(double offset, double dist, double Llength);
	//Set Tp parameters..
	void SetTruePostion(double x, double y, int enumtype, bool forpoint = false);
	//Set Tp parameters..
	void SetTruePostion3D(double x, double y, int enumtype, bool forPoint);
	//Set Current Text
	void SetTextForTextShape(char* Ctext);
	//Display snap point co-ordinate while mouse move.
	void SetAngleMode(int mode);
	//Move Selected shapes or DXF shapes according to Arrow key down.
	void SetShapeCollectionForNudge(bool isDXF);
	//set parameter for deviation.........
	void SetDeviationParameter(double Uppertolerance, double Lowertolerance, double interval, bool DeviationWithYellowShapes, int IntervalType, bool deviationType, int measureRefrenceAxis, bool BestfitArc);
	void SetAction_NudgeRotate();
	//Toggle Rcad Window 3d Mode..
	void Toggle3DMode();
	//Reset 3D Rotation..
	void Reset3DRotation();

	//Set the status code..
	void SetStatusCode(std::string s, bool Insert_TempStrFlag = false, std::string TempStr = "");
	//show messagebox text. .............................
	bool ShowMsgBoxString(std::string s, RapidEnums::MSGBOXTYPE btntype, RapidEnums::MSGBOXICONTYPE icon, bool Insert_TempStrFlag = false, std::string TempStr = "");
	//Fucntion to make Rwindow Graphics stright.
	void KeepGraphicsStright();
	//Expose current selected window.
	int getCurrentWindow();
	//Setting the Pan mode flag.
	void SetPanmode(int windwono);
	//UnSetting Pan mode
	void UnsetPanmode(int windowno);
	//Expose That Pan mode is selected or not.
	bool getPanMode(int windowno);
	//Changing line to infinite line and Viz.
	void changeline();
	//Set Rotation Angle Rcad Graphics..
	void SetRcadRotateAngle(double angle);
	//Set Rotation Angle Video Graphics..
	void SetVideoRotateAngle(double angle);
	//Set the user chosen surface ED entity..
	int SetSurfaceEDType(char* EntityName);
	//Highlight the user chosen surface ED entity..
	void HighlightSurfaceFGShape(char* EntityName);
	//set mouse down...
	void SetLmouseDownOfRcad();

	void SetAlgo6Param(int type6_MaskFactor, int type6_Binary_LowerFactor, int type6_Binary_UpperFactor, int type6_PixelThreshold, int type6_MaskingLevel, int type6_ScanRange, int type6_NoiseFilterDistInPixel, int type6SubAlgo_NoiseFilterDistInPixel, int type6JumpThreshold, bool AverageFilterON);
	void SetSurfaceEDSettings(int Channel_Number); // , bool DoAverageFilter);

	//Change Rcad View Orientations..
	void ChangeWindowViewOrientation(int WindowNo, RapidEnums::RAPIDPROJECTIONTYPE CviewType);
#pragma endregion

#pragma region Probe Point & Test Functions
	//Probe and Vision Offsets...
	double ProbeOffsetX, ProbeOffsetY, ProbeOffsetZ;
	//Add Probe point to the current selected shape..
	void AddProbePointToSelectedShape(double x, double y, double z, double r, int i, int side = -1);
	//Master sphere for calibration
	void AddPointsToAutoSphere(double *pnts, int cnt);
	//This is the master Cylinder to be constructed. 
	void AddPointsToAutoCylinder(double *pnts, int cnt);
	//Note down the probe path .. For partprogram..
	void NotedownProbePath(int side = 0);
	void NotedownProbePath(int side, double x, double y, double z);

	void NotedDownImagePickPoint();
	//Add points from text file to the seleceted shape..
	void AddTextPointToSelectedShape(double x, double y, double z, double r = 0.0);
	// Update shape after adding points to it..
	void UpdateAfterAddingPoints();
	//Focus scan testing using a file
	void AddNewPointWithFocus(std::list<double> *PtValues);
#pragma endregion

#pragma region Grid handling
	//Initialise Grid Module...
	void InitialiseGridModule(double LeftTopX, double LeftTopY, double GridWidth, double GridHeight, double CellWidth, double CellHeight);
	//Draw current grids..
	void buildGridList();
	//Increment the grid selection..
	void IncrementGridSelections();
	void ShowHideCurrentGrid();
	void GotoDefaultPosition();
	void ClearCurrentGrid();
	void setCurrentHighlitedCell(double x , double y);
#pragma endregion

#pragma region Line Arc Seperation and Perimeter Calculation
	//best fit surface generation
	void BestFitSurface();
	void BestFitSurfaceWithDiffParam(double i, bool value, bool again);

	//Add Line arc seperation
	void AddLineorArcShape_Points();
	//calculate area for One shot
	void CalculateAreaUsingOneShot();
	//Apply line are seperation for selected shape
	void ApplyLineArcSeperation_SelectedShape();
	//do id /od measurement..
	void DoIdorOdMeasurement(bool flag);
	
#pragma endregion

	void SetRunoutFluteCount(int cnt);
	//bool Calculate_CurrentFrameDifference(double* PP_CG_Params, int BoundaryWidth, int BinariseVal, bool SurfaceLightOn);


#pragma region Set Image for EdgeDetection
	//SetImage in Edge detection
	bool SetImageInEdgeDetection(int Channel_Number = 1, bool SetImageinBytes = false, int MaxTrialNumber = 2);
	bool SetImageInEdgeDetectionForDifferentChannel(int Channel_Number = 0);
	double GetAvgOfImage();
	//Set Zoomed image in Edge detection.
	bool SetZoomedImageInEdgeDetection();
#pragma endregion

#pragma region TIS Related

	bool UpdatingCAxis;
	double TIS_CAxis[6], LastRVal, BC_Axis[6], H_CamPosition[4];
	std::list<Vector>* PtsAroundCAxis;
	//int CAxis_PtsCount;
	bool AAxisHorizontal;
	bool R_Axis_ForMeasurement;

	//void Initialise_CAxis_Calc();
	//void Calculate_PtsAround_CAxis();
	void Finalise_CAxis_Calc();
	void GetC_Axis_XY(double ZPos, double *Ans);
	
#pragma endregion

#pragma region Other Functions
	//Adding point to selected shape by scan crosshair.
	void SelectScanPoint(int *position, RapidEnums::RAPIDFGACTIONTYPE Fgtype, bool runningprogram);
	//Live Scan Cros hair..
	bool LiveScanCrosshair(int *position);
	//Funciton to display DRO when mouse move.
	void ToolSelectedMousemove(double x, double y, double z);
	//Current shape properties,..//
	void CurrentShapeProperties(double x, double y, double z, double z1,double type);
	//Transform Multipication with points.
	Vector TransformMultiply(double *transform, double x, double y, double z = 1.0);
	Vector TransformMultiply_PlaneAlign(double *transform, double x, double y, double z);
	//Transform DRo values. For UCS..
	bool TransformDROValues(double x, double y, double z, double *Tvalues);
	//Find the Pointer of Vector from Snap point.
	Vector* getVectorPointer(Vector* pt, bool considerz = false);
	Vector* getVectorPointer_UCS(Vector* pt, UCS* Cucs, bool considerz = false);
	//Add Axis Shapes..
	void AddAxisShapes(UCS* currentUCS);
	void DrawCoordinatePlaneFor3DMode(UCS* currentUCS);
	void AddMarkingShape(UCS* currentUCS);
	//Add New shape..
	void AddNewShape(RapidEnums::SHAPETYPE Stype);
	//Handle Esc Key press
	void EscButtonHandling();
	//Shape Win cancel pressed..
	void HandleCancelPressed();
	//Write Module initialisation log
	void WriteModuleInitialization(std::string data);
	 void CallCalculateDiffShapes(double tolerance, double max_radius, double min_radius, double min_angle_cutoff, double noise, double max_dist_betn_pts, bool closed_loop, bool shape_as_fast_trace, bool join_all_pts);
	 void CallAddShapes();
	 void ClearLineArcShapes();
	 void AddNewPlaneForSelectedPoints();
	  void AddNewSplineForSelectedPoints();
	 void AddNewCircleForSelectedPoints();
	 void AddNewLineForSelectedPoints();
	 void deleteOtherShapes(int i);
	 void deletetextfilefromPointslog();
	 //function to get fgpoints from text file....................
	 void GetFgPoints_fromTextFile();
	 void SavePointsToHardDisc();
	 bool AddPauseAfterAction();
	 void MeasurementForHighlightedShape(int MeasureType, bool AutomateFixMeasure = false);
	 void HighlightShapeForMeasure(int id);
	 void CloudPointsMeasure(list<string> list);
	 void HighlightShapeRightClickForMeasure(int id);
	 void AddProbePathPoint(RAction *atn);
	 void WriteStringToFile(std::list<std::string> Liststr, std::string filepath, bool AppendFlag = true);
	 void ShowDeviationwithCloudPts(double UpperCutoff, double tolerance, double interval, int IntervalType, bool ClosedLoop, bool TwoD_Deviation);
	 void ComparePoints_CloudPts(double Angle, bool ArcBestFit, int NumberOfPts);
	 void CompareCloudPts(double interval, bool ArcBestFit, int NumberOfPts);
	 void CompareCloudPts(int MeasureCount, bool ArcBestFit, int NumberOfPts);
	 void StartRotationalScan(bool ScanStart);
	 void ContinuePauseRotationalScan(bool Pauseflag);
	 void DoOnePointAlignment(double x, double y, double z);
	 void CalculateCloudPtZlevelExtent();
	 void GotoLastLocalisedAction();
	 bool CreatePointsForDelphiComponent(double angle, double *idCenter, list<double> *firsthalfpnts, list<double> *scndhalfpnts, Circle *parentCir, Line* parentLine);
	 void setBaseRPropertyNonHighlighted(double x, double y, double z, bool Probeflag, FramegrabBase *baseaction, Shape *currentShape);
	 void GetBinarisedImage(); // double* targetArray);
#pragma endregion

#pragma region All Callbacks
	 //callback function to gotoNext cell for fixture Callibration..
	 void (CALLBACK *Fixture_GotoNextCellCallBack)();
	   //callback function to raise an event for take probe point..
	  void (CALLBACK *TakeProbePointCallback)(double* Pos, bool GotoPos);
	  // callback function to delete pointslogtext file
	 void (CALLBACK *deleteAlltextfilefromPointsLog) ();
	  // Callback for Framgrab changed........
	 void (CALLBACK *FramGrabUpdate) (char* Alignment, char* btnName, bool btnclick);
	//Callback for error happened.
	void (CALLBACK *MainDllErrorHappened) (char* RCaderrorcode, char* RCadFileName, char* RCadFunName);
	//To handle the btn check for smart measurement mode.
	void (CALLBACK *CheckSmartMeasurementButton)();
	//To show the panel for Derived shapes.. Parameter entry
	void (CALLBACK *DerivedShapeCallback)();
	void (CALLBACK *DerivedShapeCallbackWithPara)(double val);
	//To display the status code..
	void (CALLBACK *CurrentStatus)(char* scode, bool Insert_TempStrFlag, char* TempStr);
	//To display message box............
	bool (CALLBACK *CurrentMsgBoxStatus)(char* Text, int btntype, int icon, bool Insert_TempStrFlag, char* TempStr);
	//Handle the cursor changed event..
	void (CALLBACK *WindowCursorChanged)(int Wtype, int Ctype);
	//Show the context menu for surface Frame grab..
	void (CALLBACK *ContextMenuForSfg)();
	//Callback for SFG type select.
	void (CALLBACK *SEDUserChoice)(char* name);
	//Clear all Radio buttons..
	void (CALLBACK *ClearAllRadioButton)();
	//Clear all Radio buttons..
	void (CALLBACK *ClearAllRadioButtonOtherThrd)();
	//Update the mouse position on DRO text box..
	void (CALLBACK *UpdateMousePositionOnVideo)(double*);
	//Update the shape param of current shape..
	void (CALLBACK *UpdateShapeParamForMouseMove)(double*);
	//To move machine to required position.
	void (CALLBACK *MoveMachineToRequiredPos)(double x, double y, double z, bool activateCNC, RapidEnums::MACHINEGOTOTYPE);
	//Save camera image
	void (CALLBACK *SaveCameraImage)(std::string fname);
	void (CALLBACK *CamSettingsChanged)(std::string CamSettings);

	void (CALLBACK *RaiseDrawNormalString)(int fontsize, char* fonttype, char* fontstring, double PosX, double PosY, double PosZ, double angle);
	void (CALLBACK *InitaliseDefaultFontList)(RGraphicsFont* fontobject);
	
	void (CALLBACK *LineArcCallback)(double tolerance, double max_radius, double min_radius, double min_angle_cutoff, double noise, double max_dist_betn_pts, bool closed_loop, bool shape_as_fast_trace, bool join_all_pts);

	// Callback function to send ptId to front end for showfg Points..
	void (CALLBACK *ShowFgPtIdList)(std::list<int> *PtIdlist);
	void (CALLBACK *ShowHideWaitCursor)(int i);
	void (CALLBACK *AutoDxfAlign)();
	void (CALLBACK *DoFocusFG)();
	void (CALLBACK *DoFocusProfile)();
	void (CALLBACK *LineArcUcsCreated)();
	void (CALLBACK *UCSParamChanged)();
	void (CALLBACK *DoRightClickFocus)();
	void (CALLBACK *SetRightClickOnShape)(Shape *hsp);
	void (CALLBACK *DeviationCallBack)();
	void (CALLBACK *CloudPointsMeasureCallBack)();
	void (CALLBACK *CallDxfImport)();
	void (CALLBACK *PointsTakenProfileScan)(int i);
	void (CALLBACK *CreateNextProfileScan)();
	void (CALLBACK *GoToPreviousPosition)(RAction *prevAtn);
	void (CALLBACK *GoToNextPosition)(RAction *curAtn, bool redo);
	void (CALLBACK *ShowAutoFocusWindow)(bool open);
	void (CALLBACK *ShowAutoContourWindow)(bool open);
	void (CALLBACK *ShowRotationScanWindow)(int i);
	void (CALLBACK *VisionSpherCalibrationOver)(double x, double y, double z);
	void (CALLBACK *TruePositionOffset)(double x, double y);
	void (CALLBACK *ChangeCncMode)(int i);
	void (CALLBACK *CameraReConnected)();
	void (CALLBACK *FixtureRefPoint_Taken)();

#pragma endregion
	
	private:
		//function to Create new entity on clicking rightMousedown on Video.......Added By Rahul
	void CreateNewEntity_RightClickOnVideo();
	void AddPoints_Framgrab(PointCollection *ptcoll, FramegrabBase* currentAction, std::list<int> *AddedPtsId);
	void GetPointsId_AngRectangleFramgrab(PointCollection *ptcoll, double* point1, double* point2, double width, std::list<int> *PtIdList);
	void GetPointsId_CirAndArcFramgrab(PointCollection *ptcoll, double r1, double r2, double* center, double *endPoint1, double *endPoint2, std::list<int> *PtIdList);
	void ArrangeActionForPPEdit();
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!

//Class to handle Partprogram related functions.
//Partprogram read write..
//Partprogram run..and all the conditions.
#pragma once
#include "..\MainDLL.h"
#include <fstream>
#include <string>
#include <iostream>
#include "..\Helper\General.h"
#include "UCS.h"

class Shape;
class DimBase;
class RAction;

class MAINDLL_API PartProgramFunctions
{

private:
	RCollectionBase PPActionlist, PPShortestPathActionlist; //collection of actions to be execute by PartProgram
	RC_ITER ppCurrentaction, TmpPPActionPtr; //pointer to curret Action during PartProgram.
	struct ActionsPointCollection //To store the Action points during program.
	{
		Vector AP_PointDRO; //Current DRO.
		Vector AP_Points[3]; //FG end points
		int AP_Id;
	};
	map<int, ActionsPointCollection*> PPActionPositionColl; //original Fg end points and DRO of fg action Collections.
	
	map<int, std::string> shapename, crosshairName;//Partprogram Shape name & Crosshair type.
	BYTE* OneShotpixelarray;
	std::map<int, int> ActionIdListForNogoMeasurement;
	std::map<int, int> ActionIdListForCriticalShape;
	bool ImageActionpresent;
	double TwoStepRotationAngle;
	double TwoStepOrigin[2];
public:
	std::list<int> LastActionIdList_OffsetAction;
	RapidProperty<bool> ReferencePtAsHomePos; //Reference point is taken as Home Position.
	RapidProperty<bool> ProgramRunningForFirstTime; //Whether program is running for the first time after load.
	RapidProperty<bool> ReferenceDotIstakenAsHomePosition; //Reference dot is take as Home position.
	RapidProperty<bool> ReferenceDotIstakenAsSecondHomePos;
	RapidProperty<bool> ProgramMadeUsingReferenceDot; //Program made using reference point as homing.
	RapidProperty<bool> ProgramMadeUsingTwoReferenceDot;
	RapidProperty<bool> ReferenceLineAsRefAngle;
	RapidProperty<bool> ProgramMadeUsingRefLine;
	RapidProperty<bool> VblockAxisAsReference; //VblockAxis As rference line...
	RapidProperty<bool> ProgramMadeUsingVblockAxis; //Program made using VblockAxis As rference line...
	RapidProperty<bool> FirstShapeisSetAsReferenceShape; //First shape in the shape list set as reference shape.
	RapidProperty<bool> ProgramMadeUsingFirstShapeAsReference; //Program made using first shape as reference shape.
	RapidProperty<bool> SelectedShapesForTwoStepHoming; //Whether the shapes seleceted for two step homing..
	RapidProperty<bool> FinishedManualTwoStepAlignment; //To check whether manual two step homing is done or not during program run.
	RapidProperty<bool> ProgramMadeUsingTwoStepHoming; //Program made using two step homing.
	RapidProperty<bool> UpdateFistFramegrab; //Update the First frame graphics after program load.
	RapidProperty<bool> PartprogramLoaded; //Whether the program is loaded or not.
	RapidProperty<bool> IsPartProgramRunning; //whether the partprogram is running or not.
	RapidProperty<bool> DrawPartprogramArrow;  //Handle the draw of the partprogram path on Dxf window.
	RapidProperty<bool> FirstMousedown_Partprogram; //Whether the first action is done.. To shift the shapes.
	RapidProperty<bool> FirstMousedown_Partprogram_Manual; //Whether the first action is done.. To shift the shapes.
	RapidProperty<bool> StartingPointofPartprogram; //whether to display the starting point of the partprogram / not.
	RapidProperty<bool> ProgramAutoIncrementMode; //handle autoicrement mode for fastrace.
	RapidProperty<bool> ProgramDoneForIdorOdMeasurement; //If only id/od measurement is done then skip the image load, comparision etc.(Save the time!)
	RapidProperty<bool> PartprogramisLoading; //Partprogram is loading flag.
	RapidProperty<bool> PartprogramisLoadingEditMode; //Partprogram is load with edit.. i.e. adding actions and shapes to the main collection
	RapidProperty<bool> DoShortestPath; //Decides if shortest path for PP should be done during load
	RapidProperty<int> OneShotTolPer; //One shot image comparision tolerance.
	RapidProperty<int> OneShotTargetTolPer; //One shot image comparision match target tolerance.
	RapidProperty<int> OneShotImgBoundryWidth; //One shot image comparision boundry width to be neglected.
	RapidProperty<int> OneShotImgBinariseVal; //One shot image comparision pixel value used for binarising the image.
	RapidProperty<bool> PPDontUpdateSnapPts; //this should be set to true to delay update of snap points till flag is set to false
	RapidProperty<bool> PPAlignEditFlag; // this flag should be true for PP edit and false for PP build..........
	RapidProperty<bool> BasePlaneWithFrameGrab;
	RapidProperty<bool> ExportPointsToCsvInPP;
	RapidProperty<bool> ExportParamToCsvInPP;
	RapidProperty<bool> IntersectionPointMissed;
	RapidToggleProperty<bool> UseImageComparision;
	RapidToggleProperty<bool> PartProgramAlignmentIsDone;
	RapidProperty<int> NoOfFeatures; //Total number of features allowed in this version of software
	RapidProperty<int> ScaleFactor;

	bool FirstProbePointActionFlag, ActionTobeTransformed, SigmaModeInPP, ShiftProgram, RefreshImageForEdgeDetection;
	FramegrabBase* ppFirstgrab;//Holds current point picking action for part program.
	Vector startppPoint, endppPoint, InitilizePointPP; //Storing Starting position of ending position of drawing arrow.
	Vector checkArcClick, PreviousClick; //Getting Arc point of click to start next shape.
	Vector HomePosition, PPHomePosition, HomePosition_2nd, PPHomePosition_2nd; //Reference dot home position.
	double ReferenceLineAngle;

	void (CALLBACK *FinishedTwoStepHomingMode)(); //Callback to notify that two step manual homing procedure is finished..So that activate CNC and continue program run..
	void (CALLBACK *partProgramReached)(); //PartProgram Callback... Current Step is finished.
	void (CALLBACK *FinishedFirstPointActionIdentification)(); //callback to notify that first pointaction has been identified by LineArcFGHandler.
	void (CALLBACK *AbortPartProgram)(); // Callback to abort partprogram......
	void (CALLBACK *FinishedFirstPointActionForProbe)();
	void (CALLBACK *CreateRectangleExtentForImage)();
	Vector ImageRefShift, ImageObjShift; //One Shot image Shift
	double ImageRAngle; //Rotation angle..
	int TotalNoOfImages, CurrentProramCount; //Counter for multiple components..
	IMGLIST AllImageCollecton; //All image collection.. for multiple components.
	intPtsList AllPointsColl; //Points Collection while getting all the imges
	double MSystemOrigin[3];

	RC_ITER PPrerunActionList;

	Vector CurrentMachineDRO, MachineDro_ProbeTouch;
	Vector SavedCurrentMachineDRO;

	RapidEnums::RAPIDUNITS ProgramMeasureUnitType;
	RapidEnums::RAPIDMACHINETYPE ProgramMachineType;
	RapidProperty<int> ProgramAngleMeasurementMode; 

private:
	void InitialisePartprogramRelatedFlags(); //Initialise flags.
	void saveData(std::string filename, bool SaveForRecovery = false); //Save the partprogram data.
	void loadData(std::string filename); //Load the partprogram data.
	void EditData(std::string filename, bool WorkSpaceRecovery = false); //Edit program data..Add to main collections.
	bool checkToleranceLimitForNogoMeasurement(int MeasureId);
	bool checkToleranceLimitForCriticalShape(int ShapeId);
	void DeleteUnwantedShapes();
	void ShiftShape_ForReferencedot_PPEdit();
	void CheckProbePointAction();
	void getLastActionIdForOffsetCorrection();
	void StoreOriginalActionForNextRun(RCollectionBase* PPCollAction);
	void CopyShapesMeasurementFromPPList();
	void SetActionIdForCriticalShape(std::list<int> *AbortCheckActionIdList);
public:
	PartProgramFunctions(); //Constructor.
	~PartProgramFunctions(); //Destructor.

	void buildpp(std::string path, bool SaveForRecovery = false);//Function To build partprogram.
	void Editpp(std::string path, bool WorkSpaceRecovery = false);//Editprogram..
	void loadpp(std::string path);//Function to Load part program.
	std::string getFeatureList();
	void SetActionIdForNogoMeasurement();
	bool CreateProfileLineArcFrameGrabs();
	void ArrangeForShortestPath();//Partprogram actions arrange for shortest path..
	void ArrangeForShortestPath(std::list<int> *AbortCheckActionIdList);//Partprogram actions arrange for shortest path..
	void AssignFirstFGAction(bool AddPointActionFlag = true);
	bool SetFirstFrameGrabHandler(RapidEnums::RAPIDFGACTIONTYPE Cation);
	void CopyFirstActionProperties(FramegrabBase* FirstFg);
	void LoadFirstActionAfterProgramFinish();
	void CopyAllShapesfromPP();
	void WriteDebugInfo();
	void ChangePosition_FrameGrab(); //Change position of the first frame grab for DRO change..
	void buildPPlist(); //build program graphics list..
	void drawPartprogramArrow(double wupp); //draw the program path.. on dxf window.
	bool CheckWithinCurrentView(); //check whether the current frame is within video.. if yes no need to send CNC command.. Save time!
	bool CheckCurrentActionType_ProfileFG(); //Check whether current action type is profile light framegrab
	bool CheckCurrentFrameGrab_Fail(); //Check whether we can be get points with the current edge.. during program run
	void CalculateFocusRectangeBox_FG(); //Calculate enclosed rectangle for current FG.
	
	
	bool BeginPartProgram(); //Begin part program when Run button pressed.
	void ShiftShape_ForFirstMouseDown(Vector MousePos, bool Referenceflag); //Shift Shapes for first action
	void ShiftShape_ForFirstProbePoint(Vector difference, bool ActionOnly = false);
	void ShiftShape_ForProbeHoming(Vector MousePos); //Shift Shapes for first action
	void ShiftShape_ForFirstMouseDown_Manual(Vector MousePos); //Shift Shapes for first action

	void RotateActionForGridRun(double* RotationPt, double Theta);

	void TwoStepHoming_Alignment(); //Two step alignment ..Shape comparision function
	void TwoStep_Alignment(Shape* Csh_Orgn1, Shape* Csh_Orgn2, Shape* Csh_Mod1, Shape* Csh_Mod2); //Two step alignment ..
	void BasePlane_Alignment(Shape* Csh_Orgn1, Shape* Csh_Orgn2, Shape* Csh_Orgn3, Shape* Csh_Mod1, Shape* Csh_Mod2, Shape* Csh_Mod3); //Base Plane alignment ..

	void TwoPointHoming_Alignment(Shape* Csh_Original1, Shape* Csh_Original2, Shape* Csh_Modified1, Shape* Csh_Modified2);
	void OnePointOneLineHoming_Alignment(Shape* Csh_Original1, Shape* Csh_Original2, Shape* Csh_Modified1, Shape* Csh_Modified2);
	void TwoLineHoming_Alignment(Shape* Csh_Original1, Shape* Csh_Original2, Shape* Csh_Modified1, Shape* Csh_Modified2);

	void PlaneLinePoint_Alignment(Shape* Csh_Orgn1, Shape* Csh_Orgn2, Shape* Csh_Orgn3, Shape* Csh_Mod1, Shape* Csh_Mod2, Shape* Csh_Mod3);
	void ThreeShapes_Alignment(Shape** shapesOriginal,Shape** shapesModified);
	void PlanePointPoint_Alignment(Shape* Csh_Orgn1, Shape* Csh_Orgn2, Shape* Csh_Orgn3, Shape* Csh_Mod1, Shape* Csh_Mod2, Shape* Csh_Mod3);
	void PlaneLineLine_Alignment(Shape* Csh_Orgn1, Shape* Csh_Orgn2, Shape* Csh_Orgn3, Shape* Csh_Mod1, Shape* Csh_Mod2, Shape* Csh_Mod3);
	void BasePlane_AlignmentTransform(double* O_Plane, double* O_Line, double* O_Point, double* M_Plane, double* M_Line, double* M_Point);
	void CheckForFrameGrab();
	void SaveCurrentAction(RCollectionBase* PPColl);
	void ResetPP_EditAlign();
	void TrnasformProgram_TwoStepHoming(double *TransformMatrix, bool RotateAroundPt = false, bool ActionsOnly = false);
	void TransformProgram_BasePlaneAlignment(double *TransformMatrix);
	void TranslateUCSParameters(Vector& difference, UCS& CurrentUCS);
	void TransformUCSParameters(double *TransformMatrix, UCS& CurrentUCS);
	void DoHoming(bool UpdateGraphics = true); //Do homing for the action.. Used probe related program..

	void RunCurrentStep(); //Run the program step..
	void drawRunpp(); //set the current draw handler
	void drawThreadRunpp(); //set the current draw handler..

	RAction* GetActionfor_ActionId(int ActionId);
	void IncrementToCurrentAction();
	void IncrementStepForProbe();
	void IncrementAction(bool insideFramework = false); //Increment to program step action
	void IncrementActionThreadAction(); // Increment thread program action list..
	void DecrementAction();	//Decrement part program action.Used after pause and continue.
	void ChangeUCSAction_DuringProgram(int id);//Change the ucs during program run.. handle ucs change related updates..
	void ClearAfterProgramFinish();

	RCollectionBase& getPPActionlist(); //Exposting part program Action list.
	RCollectionBase& getPPArrangedActionlist(); //Exposting appropriate PP Action list according to shortest path flag
	void IncrementActionForBatchOfCommand(bool increment = true);
	void IncrementActionForSynchronisation();
	RAction* getCurrentAction(bool tempPtr = false); //current action
	RAction* getCurrentThreadAction(); //current thread action
	FramegrabBase* getFrameGrab(bool tempPtr = false); //current action framegrab..
	FramegrabBase* getFrameGrab_Edit1PtAlign(bool tempPtr = false);
	FramegrabBase* getFrameGrab_Edit2PtAlign(bool tempPtr = false);
	FramegrabBase* getThreadFrameGrab(); //current thread action framegrab
	Shape* getCurrentPPShape(); //current selected shape ..i.e. for the current action
	RapidEnums::ACTIONTYPE getCActionType(bool tempPtr = false); // current action type..
	char* getCurrentActionLightProperty(); //current action light property..
	char* getCurrentThreadActionLightProperty(); //current thread action light property
	double ProbeTouchPointDro[3];
	void SetProgramSteps(); //Set all program steps..
	map<int, std::string> getShapeName(); //Shape name list of all the shapes present in current program..
	map<int, std::string> getCrosshairType(); //croshair type list of current program..
	
	//One Shot Program handling functions..
	bool SetRefImage_OneShot(BYTE* soruce, int length, int bpp); //set Ref image..
	bool SetObjectImage_OneShot(); //Set Object image..Current image from camera
	bool RedefineOneShot_AboutCG(bool DoForBothImages, int* Params); //Redfine both image about CG
	bool CalculateImage_RotateAngle(); //Rotated angle..
	bool Calculate_BestMatch(); //Calculate best match..
	//bool Calculate_CurrentFrameDifference(BYTE* source, double* PP_CG_Params);

	void RotateFgGraphics_OneShot(); //Rotate graphics as per the angle got from rotation
	void RotateFgGraphics_AfterImageMatch();
	void ShiftShape_ForOneShot(Vector MousePos, Vector PixeShift); //Shift shapes 
	bool SetObjImage_OneShot(); //Set object image for multiple components.
	int GetAlltheImages(); //Get all the images from the camera..
	void SaveAllImages(BYTE* destination, int Cnt); // save all the images.. For testing..
	void ClearImageListMemory(); // Clear the image list after completing program run..

	void CalculateReferenceDotPosition();//Set Reference dot position X, Y 
	void CalculateSecondReferenceDotPosition();//Set Reference dot position X, Y 
	void SetReferenceDotPosition();//Set Reference dot position after focus.
	void SetSecondReferenceDotPosition();//Set Reference dot position after focus.
	void ResetReferenceDotPosition(double* DroValue);
	bool CalculateDelphiPPSuccessStatus();//To calculate success status of PP for Delphi Machine type

	void UpdateallSnapPtsForShapes(); //update all snap points for shapes (after PP run).
	//Clear all list, modes etc..
	void ClearAllPP(); 
	void CopyShapes_DuringProgram();
	void RotateShapes_DuringProgram();
	void ReflectShapes_DuringProgram();
	void TranslateShapes_DuringProgram();
	void GetActionListInFailedMeasure(std::list<int> *actionIdList);
	void SetCurrentAction(RAction* cAction);
	int ResetActionParameter();
	void SetDeviationTolerance(double Uppertolerance, double Lowertolerance);
	void TranslateProgramShapeMeasureAction(Vector& TransVector);
	void TransformProgramShapeMeasureAction(double *TransformMatrix);
};


//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!

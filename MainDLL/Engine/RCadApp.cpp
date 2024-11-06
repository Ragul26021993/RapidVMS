//Main Class in FrameWork..Interacts with the Wrapper...
//All Window Mouse events are Handled here..
//Graphics Threads, All Graphcis Update..
//All front end action will call a function here and continue.!
//Developed by Sathyanarayana.

#pragma once

#include "StdAfx.h"
#include "RCadApp.h"
#include <process.h>
#include<stdio.h>
#include<sstream>

#include "..\BestFit\RapidCamControl.h"
#include "..\FixedTool\FixedHeaders.h"
#include "..\Handlers\HandlerHeaders.h"
#include "..\Measurement\MeasurementHeaders.h"
#include "..\Shapes\ShapeHeaders.h"
#include "..\Actions\ActionHeaders.h"
#include "IsectPt.h"
#include "SnapPointCollection.h"
#include "BaseItem.h"
#include "..\DXF\DXFExpose.h"
#include "..\DXF\DXFCollection.h"
#include "FocusFunctions.h"
#include "..\Handlers\MouseHandler.h"
#include "..\FixedTool\CalibrationText.h"
#include "..\EdgeDetectionClassSingleChannel\Engine\EdgeDetectionClassSingleChannel.h"
#include "CalibrationCalculation.h"
#include "ThreadCalcFunctions.h"
#include "..\Helper\Helper.h"
#include "PartProgramFunctions.h"
#include "RepeatAutoMeasurement.h"
#include "ImageDelayCalibration.h"

static bool IsDelphiCam = false;				// vinod not draw cross when take a probepoint, so it will not mix with actual drawing
bool CreateTextureFlag = true;
static bool Is_measurement_mode_on=false;				//vinod if true machine should not move on left+right click functionality
//static double BCXValue = 0.0, BCYValue = 0.0;	
#define SHAPE_HIGHLIGHT_COLOR 100.0/255.0, 147.0/255.0, 237.0/255.0

//semaphores - the graphics thread doesn't loop unnecessarily they wait till next SetEvent..
HANDLE Video_Semaphore, Rcad_Semaphore, Dxf_Semaphore;
//Static byte array, used to pixels of the video..!!
//static BYTE pixels[42000000]; //Made larger from 240 Lakhs
//
//static double pixelvalues[42000000]; //made larger. R.A.N 15 Mar2014 for new cam
//static BYTE pixelBytes[12000000];

									 //static double pixelvalues_RBChannel[3200000];
static BYTE* screenshot = NULL;
HWND VideoGraphicsWindow;

//Handle Error callback from Graphcis dll.....
void GraphicsErrorHandler(char* ecode, char* fname, char* funcname)
{
	MAINDllOBJECT->SetAndRaiseErrorMessage(ecode, fname, funcname);
}

//Handle Error callback from RBF dll.....
void RBFErrorHandler(char* ecode, char* fname, char* funcname)
{
	MAINDllOBJECT->SetAndRaiseErrorMessage(ecode, fname, funcname);
}

//Handle Error callback from Graphcis dll.....
void RGraphicsDrawErrorHandler(char* ecode, char* fname, char* funcname)
{
	MAINDllOBJECT->SetAndRaiseErrorMessage(ecode, fname, funcname);
}

//Handle Error callback from Edge Detection Module......
void EdgeDetectionErrorHandler(char* ecode, char* fname, char* funcname)
{
	MAINDllOBJECT->SetAndRaiseErrorMessage(ecode, fname, funcname);
}

//Handling grid module error...
void GridModuleErrorHandler(char* ecode, char* fname, char* funcname)
{
	MAINDllOBJECT->SetAndRaiseErrorMessage(ecode, fname, funcname);
}

//Set and raise the Error happened..
void RCadApp::SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname)
{
	if(MainDllErrorHappened != NULL)
		MainDllErrorHappened((char*)ecode.c_str(), (char*)filename.c_str(), (char*)fctnname.c_str());
}

//Handling rapid cam error messages...
void RapidCamErrorHandler(char* ecode)
{
	MAINDllOBJECT->ShowMsgBoxString(ecode, RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
}

//Save Actions. Used to recover the Workspace, if software is crashed..
void RCadApp::SaveActions()
{
	try
	{
		MAINDllOBJECT->ActionSavingFlag = true;
		std::string Pppath = MAINDllOBJECT->currentPath + "\\ActionSave.rppx";
		PPCALCOBJECT->buildpp(Pppath.c_str(), true);
		MAINDllOBJECT->ActionSavingFlag = false;
		_endthread();
	}
	catch(...){ MAINDllOBJECT->ActionSavingFlag = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD0001", __FILE__, __FUNCSIG__); }
}


//Thread for the graphics on video 
void VideoGraphics_Thread(void *anything)
{
	try
	{
		//Create a semaphore for Video graphics thread
		Video_Semaphore = CreateEvent(NULL, TRUE, FALSE, _T("VideoGraphics"));
		//The graphics thread starts here
		while(MAINDllOBJECT->ThreadRunning)
		{
			Sleep(0);
			try
			{
				MAINDllOBJECT->Draw_VideoWindowGraphics();
				//WaitForSingleObject(Video_Semaphore, INFINITE); 
				//ResetEvent(Video_Semaphore);
				if(!MAINDllOBJECT->VideoGraphicsUpdateQueue)
				{
					WaitForSingleObject(Video_Semaphore, INFINITE); 
					ResetEvent(Video_Semaphore);
				}
				else
				{
					MAINDllOBJECT->VideoGraphicsUpdateQueue = false;
					MAINDllOBJECT->VideoGraphicsUpdating = true;
				}
			}
			catch(...)
			{ 
				MAINDllOBJECT->VideoMouseFlag = false; MAINDllOBJECT->CaptureVideo = false; MAINDllOBJECT->VideoGraphicsUpdating = false;
				MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1001", __FILE__, __FUNCSIG__); 
				_endthread();
			}
		}
		MAINDllOBJECT->VideoGraphicsUpdating = false;
		_endthread();
	}
	catch(...)
	{ 
		MAINDllOBJECT->VideoMouseFlag = false; MAINDllOBJECT->CaptureVideo = false; MAINDllOBJECT->VideoGraphicsUpdating = false;
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD2002", __FILE__, __FUNCSIG__); 
		_endthread();
	}
}

//Thread for graphics on rcad window
void RcadGraphics_Thread(void *anything)
{
	try
	{
		//Create a semaphore for Rcad graphics thread
		Rcad_Semaphore = CreateEvent(NULL, TRUE, FALSE, _T("RcadGraphics"));
		//The graphics thread starts here
		while(MAINDllOBJECT->ThreadRunning)
		{
			try
			{
				Sleep(0);
				MAINDllOBJECT->Draw_RcadWindowGraphics();
				//WaitForSingleObject(Rcad_Semaphore, INFINITE); 
				//ResetEvent(Rcad_Semaphore);
				if(!MAINDllOBJECT->RcadGraphicsUpdateQueue)
				{
					WaitForSingleObject(Rcad_Semaphore, INFINITE); 
					ResetEvent(Rcad_Semaphore);
				}
				else
				{
					MAINDllOBJECT->RcadGraphicsUpdateQueue = false;
					MAINDllOBJECT->RcadGraphicsUpdating = true;
				}
			}
			catch(...)
			{ 
				MAINDllOBJECT->RCADMouseFlag = false; MAINDllOBJECT->CaptureCad = false; MAINDllOBJECT->RcadGraphicsUpdating = false;
				MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1002", __FILE__, __FUNCSIG__); 
				_endthread();
			}
		}
		MAINDllOBJECT->RcadGraphicsUpdating = false;
		_endthread();
	}
	catch(...)
	{ 
		MAINDllOBJECT->RCADMouseFlag = false; MAINDllOBJECT->CaptureCad = false; MAINDllOBJECT->RcadGraphicsUpdating = false;
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1002", __FILE__, __FUNCSIG__); 
		_endthread();
	}
}

//Thread for the graphics on DXF
void DxfGraphics_Thread(void *anything)
{
	try
	{
		//Create a semaphore for Dxf graphics thread
		Dxf_Semaphore = CreateEvent(NULL, TRUE, FALSE, _T("DxfGraphics"));
		//The graphics thread starts here
		while(MAINDllOBJECT->ThreadRunning)
		{
			try
			{
				Sleep(0);
				MAINDllOBJECT->Draw_DxfWindowGraphics();
				//WaitForSingleObject(Dxf_Semaphore, INFINITE); 
				//ResetEvent(Dxf_Semaphore);
				if(!MAINDllOBJECT->DxfGraphicsUpdateQueue)
				{
					WaitForSingleObject(Dxf_Semaphore, INFINITE); 
					ResetEvent(Dxf_Semaphore);
				}
				else
				{
					MAINDllOBJECT->DxfGraphicsUpdateQueue = false;
					MAINDllOBJECT->DxfGraphicsUpdating = true;
				}
			}
			catch(...)
			{ 
				MAINDllOBJECT->DXFMouseFlag = false; MAINDllOBJECT->DxfGraphicsUpdating = false;
				MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1003", __FILE__, __FUNCSIG__); 
				_endthread();
			}
		}
		MAINDllOBJECT->DxfGraphicsUpdating = false;
		_endthread();
	}
	catch(...)
	{ 
		MAINDllOBJECT->DXFMouseFlag = false; MAINDllOBJECT->DxfGraphicsUpdating = false;
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1003", __FILE__, __FUNCSIG__); 
		_endthread();
	}
}

//Constructor..//
RCadApp::RCadApp(std::string path)
{
	try
	{
		this->currentPath = path;
		this->DatabaseFolderPath = "";
		InitialiseAllInstance();
		InitialiseAllCallbacks();
		InitialiseAllMutex();

		InitialiseFrameGrabSettings();
		InitialiseCurrenthandlerSettings();
		InitialiseGraphicsUpdateFlags();
		InitialiseShapeHighlightedFlags();

		this->ThreadRunning = true;
		this->CaptureCad = false;
		this->CaptureVideo = false;
		this->PartprogramShape_Updated = false;

		this->VideoGraphicsUpdating = false;
		this->RcadGraphicsUpdating = false;
		this->DxfGraphicsUpdating = false;

		this->VideoGraphicsUpdateQueue = false;
		this->RcadGraphicsUpdateQueue = false;
		this->DxfGraphicsUpdateQueue = false;
	
		this->Xaxis_Highlighted = false;
		this->Yaxis_Highlighted = false;
		this->MasteProbeCalibrationMode(false);
				
		this->ClearGraphicsOnVideo(false);
		this->HideGraphicsOnVideo(false);
		this->ScaleViewMode(false);
		this->DigitalZoomModeForOneShot(false);
		this->IsOldPPFormat(false);
		this->PPLoadSuccessful(true);
		this->ApplyLineArcSeperationMode(true);
		this->OutlierFilteringForLineArc(false);
		this->SmartMeasureTypeMode(false);
		this->MousedragwithShape(false);
		this->ShowExplicitIntersectionPoints(true);
		this->ShowImplicitIntersectionPoints(false);
		this->ShowMeasurementOnVideo(true);
		this->SigmaModeFlag(false);
		this->IsProbePresent(false); 
		this->DefaultCrosshairMode(false);
		this->ShowDXFShapesOnVideo(true);
		this->MouseClickAndDragMode(false);
		this->MeasurementMode(false);
		this->SetCrossOnVideo(false);
		this->CameraConnected(false);
		this->EditmeasurementMode(false);
		this->HatchedCrossHairMode(false);
		this->RcadZoomExtents(false);
		this->RcadGraphicsMoveWithDRO(false);
		this->StylusPolarity(false);
		this->TriangulateCpSurface(false);
		this->UseMultiThreads_Surfacebuild(false);
		this->TriangleStepLength(10);
		this->PointClusterSize(500);
		this->CloudPtdistance(0.4);
		this->CreateSurfaceForClosedSurface(false);
		this->UseAvImage(false);
		this->NoOfFramesToAverage(1);

		this->DrawWiredSurface(false);
		this->AllowTransparencyTo3DShapes(false);
		this->DrawCylinderdisks(true);

		this->ImageAction_Count = 0;
		this->IsCNCMode(false);
		this->ShowHideCrossHair(true);
		this->ShapeHighlightedForImplicitSnap(false);
		this->ShowHideFGPoints(false);
		this->HideGraphicsOnVideoSettingsMode(false);
		this->SaveCrossHairWithImage(false);
		this->ProfileScanMode(false);
		this->LinearModeFlag(false);
		this->PCDMeasureModeFlag(false);
		this->CCPMeasureModeFlag(false);
		this->DigitalZoomMode(false);
		this->FullWindowDigitalZoom(false);
		this->UCSSelectedLineDirAlongX(true);
		this->UpdateGrafixFor3DSnapPoint(false);
		this->MeasurementNoOfDecimal(4);
		this->MinimumPointsInEdge(200);
		this->PixelNeighbour(15);
		this->ReferenceDatumNameCount_GDT(0);
		this->VideoFrameRate(20);

		this->RotaryCalibrationAngleValue(0);
		this->RotaryCalibrationIntercept(0);
		this->Rotary_YaxisOffset(0);

		this->PixelOffset_X(0);
		this->PixelOffset_Y(0);

		this->XPositionStart_Right(0);
		this->XPositionStart_Left(0);
		this->YPositionStart_Top(0);
		this->YPositionStart_Bottom(0);
		this->XMultiplyFactor_Right(1);
		this->XMultiplyFactor_Left(1);
		this->YMultiplyFactor_Top(1);
		this->YMultiplyFactor_Bottom(1);
		this->RunoutAxialRadius(0);
		this->RunoutFluteCount(2);
		this->RunOutType(0);
		this->ChangeRcadViewOrienFor3D(false);
		this->PixelWidthCorrectionForOneShot(false);
		this->PixelWidthCorrectionQuadraticTerm(0);
		this->PixelWidthCorrectionLinearTermX(0);
		this->PixelWidthCorrectionLinearTermY(0);
		
		
		this->UseLocalisedCorrection = false;
		this->IsPartProgramPause = false;
		this->GridmoduleUpdated = false;
		this->TruePositionMode = false;
		this->Alpha_Value = 0.4;
		StlHighlightedPointFlag = false;
		CheckStlHighlightedPt = false;
		CurrentExtensionFlag = false;
		ActionSavingFlag = false;
		CriticalShapeStatus = false; MeasurementcolorInGradient = false;
		AngleTolerance = 0;
		PercentageTolerance = 0;
		BestFitAlignment_Resolution = 0.01;
		BestFitAlignment_SampleSize = 100;
		for (int i = 0; i < 7; i ++) TIS_CAxis[i] = 0.0;
		TIS_CAxis[5] = 1.0;
		HideShapeMeasurementDuringPP = false;
		this->Video_Graphics = NULL;
		this->Rcad_Graphics = NULL;
		this->Dxf_Graphics = NULL;
	
		CurrentPCDShape = NULL;
		ReferenceDotShape = NULL; ReferenceLineShape = NULL;
		Vblock_CylinderAxisLine = NULL;
		SecondReferenceDotShape = NULL;
		ToolAxisLineId = 99;
		NumberOfReferenceShape = 4;

		ShapePasted = false;
		ProfileON = false;
		SurfaceON = false;
		AxillaryON = false;

		DXFMouseFlag = false;
		RCADMouseFlag = false;
		VideoMouseFlag = false;
		UpdteVgp_UcsChange = false;
		UpdteCadgp_UcsChange = false;



		UCSid = -1;
		
		currentwindow = 0;
		ColorThreshold = 0;
		CameraDistortionFactor = 0;
		CamXOffSet = 0; CamYOffSet = 0;
		NudgeShapesflag = false;
		DrawRectanle_ImageDelay = false;
		AngleMeasurementMode(0);
		
		dontUpdateGraphcis = false;
		
		undone = false;
		Shapeflag = false;

		AddingUCSModeFlag = true;
	
		Derivedmodeflag = false;
		NudgeToolflag = false;
		NudgeThreadShapes = false;
		RotateDXFflag = false;
		SaveImageFlag = false; Continuous_ScanMode = false;
		ProbeEnableFlag = false;
		RcadBackGround_White = false;
		EnableTriangulationLighting = false;
		DiscreteColor = false;
		ProbeSmartMeasureflag = false;
		AutoProbeMeasureFlag = false;
		PointerToShapeCollection = NULL;
		DoNotSamplePts = false;
		AddAction_PPEditMode = false;
		LastActionIdForPPEdit = 0;
		Color_Channel = 1;
		SamplePtsCutOff = 50000; this->BestFit_PointsToBundle = 5;
		this->Dots_PointsMinCount = 4, this->Dots_PixelCount = 4;
		this->ArcBestFitType_Dia = false;
		ProfileScanMoveToNextFrameFactor = 0.3;
		ShowCoordinatePlaneFlag(false);
		ShowPictorialView(false);
		OneTouchProbeAlignment = false;
		SelectedAlgorithm(5);
		nearestShape = NULL;
		DXFnearestShape = NULL;
		nearestmeasure = NULL;
	
		CloudPtZExtent[0] = 0; CloudPtZExtent[1] = 0;
		ResetRcadRotation = false;
		this->ThreeDRotationMode(false);
		this->RcadWindow3DMode(false);
		ShowProbeDraw(true);
		ControlOn = false;
		ShiftON = false;
		this->ChangeRcadViewOrien = false;
		this->ApplyMMCCorrection(false);
		this->ApplyFilterCorrection(false);
		this->FilterCorrectionFactor(2);
		this->FilterCorrectionIterationCnt(1);
		this->RotateGrafix(true);
	   //by default automated flag is false........
		AutomatedMeasureModeFlag(false);
		DigitalZoomIsOn = false;
	
		Video_Cnt = 0;
		Rcad_Cnt = 0;
		Dxf_Cnt = 0;
				
		this->ucslist.addListener(this);
		this->actionhistory.addListener(this);
		SmartMeasureType = RapidEnums::SMARTMEASUREMENTMODE::DEFAULTTYPE;
		VideoMpointType = RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT;
		CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER;
		PREVIOUSHANDLE = RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER;
		MeasureUnitType = RapidEnums::RAPIDUNITS::MM;

		//TIS related
		this->UpdatingCAxis = false;
		this->PtsAroundCAxis = NULL;
		this->AAxisHorizontal = false;
		
		int j = currentPath.rfind("\\");
		currentPath = currentPath.erase(j);
		j = currentPath.rfind("\\");
		currentPath = currentPath.erase(j);
		this->CreateBFSurfaceActions = false;
		this->RepeatSGWidth = false;
		this->PlaceDetail = false;
		this->PlaceTable = false;
		this->FundamentalShapes(7);
		this->DemoMode(false);
		this->AutomaticTwoStepHoming(false);
		this->LastSelectedMagLevel = "";
		this->CurrentMCSType = RapidEnums::MCSTYPE::MCSXY;	
		this->UseGotoPostionInAction(false);
		this->UseGotoPointsforProbeHitPartProg(false);
		this->Left_Mouse_Down = false; this->Right_Mouse_Down = false;
		ZeroMemory(&this->Offset_Normal[0], sizeof(int) * 8);
		ZeroMemory(&this->Offset_Skewed[0], sizeof(int) * 8);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1004", __FILE__, __FUNCSIG__); }
}

//Destructor...  Clear all the UCS and close all the Handles...//
RCadApp::~RCadApp()
{
	try
	{
		ThreadRunning = false;
		ResetEvent(Video_Semaphore); ResetEvent(Rcad_Semaphore); ResetEvent(Dxf_Semaphore);
		CloseHandle(ShapeMutex); CloseHandle(SelectedShapeMutex);
		CloseHandle(MeasureMutex); CloseHandle(SelectedMeasureMutex);
		CloseHandle(DxfMutex); CloseHandle(Video_Semaphore); 
		CloseHandle(Rcad_Semaphore); CloseHandle(Dxf_Semaphore);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1004", __FILE__, __FUNCSIG__); }
}

//Static Instance..
RCadApp* RCadApp::_instance = 0;

//create this class Instance...//
void RCadApp::CreateMainDllInstance(char* path)
{
	try
	{
		std::string Strpath = (const char*)path;
		_instance = new RCadApp(Strpath);
		_instance->currentHandler = new DefaultHandler();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1005", __FILE__, __FUNCSIG__); }
}

//returns this class Instance....//
RCadApp* RCadApp::getInstance()
{
	return _instance;
}

void RCadApp::set_flag_for_measurement_mode(bool flag)
{
	Is_measurement_mode_on=flag;
}

void RCadApp::SetValuesOfBCXBCY(double x, double y)					//vinod..for hob checker getc_axis_xy..
{
	//BCXValue = x;
	//BCXValue = y;
}

//Delete the Instance .. Close event..!
void RCadApp::deleteInstance()
{
	try
	{
		//if(MAINDllOBJECT->CameraConnected())
			CameraClose();
		//delete the graphics objects..
		if(_instance->Video_Graphics != NULL) delete _instance->Video_Graphics;
		if(_instance->Rcad_Graphics != NULL)  delete _instance->Rcad_Graphics;
		if(_instance->Dxf_Graphics != NULL)  delete _instance->Dxf_Graphics;

		if(_instance->HelperInst != NULL) delete _instance->HelperInst; 
		if(_instance->DxfexposeObj != NULL) delete _instance->DxfexposeObj;
		if(_instance->bestFit != NULL) delete _instance->bestFit;
		if(_instance->cbcInstance != NULL) delete _instance->cbcInstance;
		if(_instance->thcfInstance != NULL) delete _instance->thcfInstance;
		if(_instance->ffinstance != NULL) delete _instance->ffinstance;
		if(_instance->FixedToolHandler != NULL) delete _instance->FixedToolHandler; 
		if(_instance->Edgedetection != NULL) delete _instance->Edgedetection;
		if(_instance->ppinstance != NULL) delete _instance->ppinstance;
		RGraphicsDraw::DeleteGraphicsDrawInstance();
		delete _instance->currentHandler;
		delete _instance;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1006", __FILE__, __FUNCSIG__); }
}

void RCadApp::CloseSoftware()
{
	try
	{
		ThreadRunning = false;
		update_VideoGraphics(true);
		update_RcadGraphics(true);
		update_DxfGraphics();
		Wait_VideoGraphicsUpdate();
		Wait_RcadGraphicsUpdate();
		Wait_DxfGraphicsUpdate();
	}
	catch(...)
	{
	}
}

void RCadApp::InitialiseAllInstance()
{
	try
	{
		this->Edgedetection = NULL;
		this->thcfInstance = NULL;
		this->FixedToolHandler = NULL;
		this->cbcInstance = NULL;
		this->ffinstance = NULL;
		this->HelperInst = NULL;
		this->DxfexposeObj = NULL;
		this->bestFit = NULL;
		this->ppinstance = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD2003", __FILE__, __FUNCSIG__); }
}

//Initiallise Callback pointers..
void RCadApp::InitialiseAllCallbacks()
{
	try
	{
		MainDllErrorHappened = NULL;
		CheckSmartMeasurementButton = NULL;
		DerivedShapeCallback = NULL;
		CurrentStatus = NULL;
		WindowCursorChanged = NULL;
		SEDUserChoice = NULL;
		ContextMenuForSfg = NULL;
		ClearAllRadioButton = NULL;
		UpdateMousePositionOnVideo = NULL;
		UpdateShapeParamForMouseMove = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1007", __FILE__, __FUNCSIG__); }
}

//Initialise required mutex..
void RCadApp::InitialiseAllMutex()
{
	try
	{
		ShapeMutex = CreateMutex(NULL, false, NULL); 
		SelectedShapeMutex = CreateMutex(NULL, false, NULL); 
		MeasureMutex = CreateMutex(NULL, false, NULL);
		SelectedMeasureMutex = CreateMutex(NULL, false, NULL);
		DxfMutex = CreateMutex(NULL, false, NULL);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1008", __FILE__, __FUNCSIG__); }
}

//Initialise flags..
void RCadApp::InitialiseCurrenthandlerSettings()
{
	try
	{		
		this->FGDirection_AngularRect(false);
		this->FGDirection_Circular(false);
		this->FGDirection_Arc(false);
		this->FGDirection_Rectangle(0);
		this->FGDirection_FixedRectangle(0);
		this->DMMMeasureTypeLinear(0);
		this->SurfaceFgtype(0);
		FrameGrab_AngularRect.ResetCurrentValuetoDefault();
		FrameGrab_Arc.ResetCurrentValuetoDefault();
		FrameGrab_Circular.ResetCurrentValuetoDefault();
		FrameGrab_FixedRectangle.ResetCurrentValuetoDefault();
		FrameGrab_ScanMode.ResetCurrentValuetoDefault();
		FrameGrab_FocusDepthMultiBox.ResetCurrentValuetoDefault();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1009", __FILE__, __FUNCSIG__); }
}

//Initialise flags..
void RCadApp::InitialiseGraphicsUpdateFlags(bool flag)
{
	try
	{
		this->VShape_Updated = flag;
		this->VSelectedShapeUpdated = flag;
		this->RShape_Updated = flag;
		this->RSelectedShape_Updated = flag;
		this->VMeasurement_Updated = flag;
		this->VSelectedMeasurement_Updated = flag;
		this->RMeasurement_Updated = flag;
		this->RSelectedMeasurement_Updated = flag;
		this->VDXFShape_updated = flag;
		this->RDXFShape_updated = flag;
		this->RDXFMeasure_updated = flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1010", __FILE__, __FUNCSIG__); }
}

//Initialise flags..
void RCadApp::InitialiseShapeHighlightedFlags()
{
	try
	{
		this->ShapeHighlighted(false);
		this->SnapPointHighlighted(false);
		this->DrawSnapPointOnHighlightedShape(false);
		this->DXFSnapPointHighlighted(false);
		this->DXFShapeHighlighted(false);
		this->GridCellHighlighted(false);
		this->MeasurementHighlighted(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1011", __FILE__, __FUNCSIG__); }
}

void RCadApp::InitialiseFrameGrabSettings()
{
	try
	{
		FrameGrab_AngularRect.Set_Width(20, 140, 50);
		FrameGrab_AngularRect.Set_Height(20, 140, 50);

		FrameGrab_Arc.Set_Width(20, 100, 50);
		FrameGrab_Arc.Set_Height(20, 100, 50);

		FrameGrab_Circular.Set_Width(20, 100, 50);
		FrameGrab_Circular.Set_Height(20, 100, 50);

		FrameGrab_FixedRectangle.Set_Width(40, 780, 400);
		FrameGrab_FixedRectangle.Set_Height(30, 580, 300);

		FrameGrab_ScanMode.Set_Width(20, 120, 40);
		FrameGrab_ScanMode.Set_Height(20, 120, 40);

		FrameGrab_FocusDepthMultiBox.Set_Width(20, 100, 40);
		FrameGrab_FocusDepthMultiBox.Set_Height(20, 100, 40);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1012", __FILE__, __FUNCSIG__); }
}

//Initialise Other Modules..
void RCadApp::InitialiseOtherModules1()
{
	try
	{
		RGraphicsDraw::CreateGraphicsDrawInstance();
		GRAFIX->RGraphicsDrawError = &RGraphicsDrawErrorHandler;

		this->bestFit = new RBF();
		this->bestFit->RBFError = &RBFErrorHandler;

		this->HelperInst = new HelperClass();
		this->cbcInstance = new CalibrationCalculation();
		this->FixedToolHandler = new FixedToolCollection();
		this->DxfexposeObj = new DXFExpose();
		this->thcfInstance = new ThreadCalcFunctions();
		this->ffinstance = new FocusFunctions();
		this->ppinstance = new PartProgramFunctions();
		this->CalibTextIns = new CalibrationText();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1013", __FILE__, __FUNCSIG__); }
}

//Initialise Other Modules..
void RCadApp::InitialiseOtherModules2()
{
	try
	{
		this->ppinstance->ppFirstgrab = new FramegrabBase();
		RMATH2DOBJECT->LoadIdentityMatrix(&RcadTransformationMatrix[0], 4);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD10014", __FILE__, __FUNCSIG__); }
}

//Reset Video Graphics update flags..
void RCadApp::ResetVideoShapeUpdateFlag()
{
	try
	{
		this->VSelectedMeasurement_Updated = false;
		this->VMeasurement_Updated = false;
		this->VSelectedShapeUpdated = false;
		this->VShape_Updated = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1015", __FILE__, __FUNCSIG__); }
}

//Add new Ucs during load..
void RCadApp::AddMCSduringLoad()
{
	try
	{
		UCS* NewUcs = new UCS(_T("MCS"), false);
		ProjectionType = RapidEnums::RAPIDPROJECTIONTYPE::XY;
		UCSProjectionType = RapidEnums::RAPIDPROJECTIONTYPE::XY;
		AddingUCSModeFlag = true;
		/*MAINDllOBJECT->AddAxisShapes(NewUcs);
		MAINDllOBJECT->DrawCoordinatePlaneFor3DMode(NewUcs);
		MAINDllOBJECT->AddMarkingShape(NewUcs);*/
		NewUcs->SetUCSProjectionType((int)UCSProjectionType);
		getUCSList().addItem(NewUcs);
		NewUcs->selected(true);
		AddingUCSModeFlag = false;	
		MAINDllOBJECT->AddAxisShapes(NewUcs);
		//MAINDllOBJECT->DrawCoordinatePlaneFor3DMode(NewUcs);
		MAINDllOBJECT->AddMarkingShape(NewUcs);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1016", __FILE__, __FUNCSIG__); }
}

//Initialise Video window..
bool RCadApp::Initialise_VideoWindow(HWND handle, HWND hiddenwindow, int width, int height, int camWidth, int camHeight, bool ConnecttoRapidCam) //
{
	//bool GotCameraAndVideoWindow = false;	
	try
	{
		double DigitalZoomRatio = 1.0;
		//RapidCamError = &RapidCamErrorHandler;

		//if (Initialise(handle, 1, 102)) MAINDllOBJECT->CurrentCameraType = CurrentCamera();
		//if (MAINDllOBJECT->CurrentCameraType == 3)
		//	DigitalZoomRatio = 4.0;
		VideoGraphicsWindow = hiddenwindow;

		//this->ConnectToCamera(handle, camWidth, camHeight, DigitalZoomRatio, width, height, 20, ConnecttoRapidCam);
		//camWidth and camHeight are new params that need to be passed if front end video window size (width and height)
		//will be different from (smaller than) camera frame video size. 
		if (camWidth == -1) camWidth = width;
		if (camHeight == -1) camHeight = height;

		//to be used subsequently during readpixels for graphics update on video
		currCamWidth = camWidth;
		currCamHeight = camHeight;
		HelperInst->CamSizeRatio = (double)camWidth / (double)width;

		pixelvalues = new double[currCamWidth * currCamHeight * 3]; // *sizeof(double)];
		pixelBytes = new BYTE[currCamWidth * currCamHeight];
		pixels = new BYTE[currCamWidth * currCamHeight * 4];

		memset(pixels, 102, currCamWidth * currCamHeight * 4);
		memset(pixelvalues, 0, currCamWidth * currCamHeight * 3 * sizeof(double));
		memset(pixelBytes, 0, currCamWidth * currCamHeight);

		//if((MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT) && MAINDllOBJECT->DigitalZoomModeForOneShot())
		RapidCamError = &RapidCamErrorHandler;
		//Video window initialize
		if(MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::HOBCHECKER && ConnecttoRapidCam)
		{
			bool succeeded = Initialise(handle, 1, 102);
			if (succeeded)
			{
				MAINDllOBJECT->CurrentCameraType = CurrentCamera();
				if ((camWidth == width) && (camHeight == height))
				{
					succeeded = SetVideoWindow(camWidth, camHeight, MAINDllOBJECT->VideoFrameRate(), true, this->CameraDistortionFactor, this->CamXOffSet, this->CamYOffSet);
				}
				else //We have to compress a larger video stream to show in a smaller area. So we call SetVideoWindowEx
				{
					bool CompressFrame = false;
					if (camWidth > width && !DigitalZoomIsOn) CompressFrame = true;
					succeeded = SetVideoWindowEx(camWidth, camHeight, 0, 0, width, height, MAINDllOBJECT->VideoFrameRate(), true, this->CameraDistortionFactor, this->CamXOffSet, this->CamYOffSet, CompressFrame);
				}
			}
			if (succeeded)
			{
				if(CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL || CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP)
					FlipVertical();
				Preview();
				//Set_ImBufferMode(true);
				CameraConnected(true);
			}
			else
				CameraConnected(false);
		}
		else 
			CameraConnected(false);

		if (CurrentCamera() == 3) //HelperInst->CamSizeRatio != 1 &&
			this->Edgedetection = new EdgeDetectionSingleChannel(camWidth, camHeight, 4, 15);
		else
			this->Edgedetection = new EdgeDetectionSingleChannel(width, height, 3, 10);

		this->Edgedetection->EdgeDetectionError = &EdgeDetectionErrorHandler;
		this->Edgedetection->CamSizeRatio = (double)camWidth / (double)width;

		RWindow& VideoWin = getWindow(0);
		//below window being initialized with front end video size
		HWND chosenWindow;

		if (CameraConnected())
		{
			chosenWindow = VideoGraphicsWindow;
		}
		else
			chosenWindow = handle;

		VideoWin.init(chosenWindow, getCurrentUCS().getWindow(0).getUppX(), getCurrentUCS().getWindow(0).getUppY(), 1.0, width, height); //0.94
		VideoWin.WindowNo = 0;
		//Graphics Initialization....////
		Video_Graphics = new RGraphicsWindow();
		Video_Graphics->GraphicsDLLError = &GraphicsErrorHandler;
		Video_Graphics->addWindow(chosenWindow, VideoWin.getWinDim().x, VideoWin.getWinDim().y, 0.4f, 0.4f, 0.4f);
		//Video_Graphics->addWindow(VideoWin.getHandle(), width, height, 0.4f, 0.4f, 0.4f); // VideoWin.getWinDim().x, VideoWin.getWinDim().y  currCamWidth, currCamHeight,
		//GRAFIX->CreateNewFont(hiddenwindow, "Tahoma");
		GRAFIX->CreateNewFont(VideoWin.getHandle(), "Tahoma");
		//}
		//else
		//{
		//	Video_Graphics->addWindow(VideoWin.getHandle(), VideoWin.getWinDim().x, VideoWin.getWinDim().y, 0.4f, 0.4f, 0.4f);
		//	GRAFIX->CreateNewFont(VideoWin.getHandle(), "Tahoma");
		//}
		VideoWupp = VideoWin.getUppX();//Video Window pixel width....
		VideoWin.WindowPanned(true);
		//Draw_VideoWindowGraphics();
		//_beginthread(&VideoGraphics_Thread, 0, NULL);
		//update_VideoGraphics(false);
		if (CameraConnected() && CurrentCamera() == 3)
		{
			SetSurfaceLightMode(true);
			this->SetRCam3_DigitalGain(MAINDllOBJECT->RCam3_D_GainSettings);
		}

		return CameraConnected();
	}
	catch (std::exception &ex)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1017", __FILE__, ex.what()); //__FUNCSIG__
		return CameraConnected();
	}
}


//Initialise Rcad window..
void RCadApp::Initialise_RcadWindow(HWND handle, int width, int height)
{
	try
	{
		RWindow& RcadWin = getWindow(1);
		RcadWin.init(handle, 0.007, 0.007, 0.94, width, height);
		RcadWin.WindowNo = 1;
		//Graphics Initialization....//
		Rcad_Graphics = new RGraphicsWindow(); 
		Rcad_Graphics->GraphicsDLLError = &GraphicsErrorHandler;
		//Add the rcad window...
		Rcad_Graphics->addWindow(RcadWin.getHandle(), RcadWin.getWinDim().x, RcadWin.getWinDim().y, 0.0f, 0.0f, 0.0f);
		GRAFIX->CreateNewFont(RcadWin.getHandle(), "Tahoma");
		RcadWin.WindowPanned(true);
		//Draw_RcadWindowGraphics();
		//_beginthread(&RcadGraphics_Thread, 0, NULL);
		//update_RcadGraphics(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1018", __FILE__, __FUNCSIG__); }
}

//Initialise Dxf window..
void RCadApp::Initialise_DxfWindow(HWND handle, int width, int height)
{
	try
	{
		RWindow& DxfWin = getWindow(2);
		DxfWin.init(handle, 0.01, 0.01, 0.94, width, height);	
		DxfWin.WindowNo = 2;
		//Graphics Initialization....//
		Dxf_Graphics = new RGraphicsWindow(); 
		Dxf_Graphics->GraphicsDLLError = &GraphicsErrorHandler;
		//Add the rcad window...
		Dxf_Graphics->addWindow(DxfWin.getHandle(), DxfWin.getWinDim().x, DxfWin.getWinDim().y, 0.0f, 0.0f, 0.0f);
		GRAFIX->CreateNewFont(DxfWin.getHandle(), "Tahoma");
		DxfWin.WindowPanned(true);
		//Draw_DxfWindowGraphics();
		//_beginthread(&DxfGraphics_Thread, 0, NULL);
		//update_DxfGraphics(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1019", __FILE__, __FUNCSIG__); }
}

void RCadApp::StartVideoThread()
{
	_beginthread(&VideoGraphics_Thread, 0, NULL);
}

void RCadApp::StartRcadThread()
{
	_beginthread(&RcadGraphics_Thread, 0, NULL);
}

void RCadApp::StartDxfThread()
{
	_beginthread(&DxfGraphics_Thread, 0, NULL);
}

//Set the hidden window handle..
void RCadApp::SetHiddenWindowHandle(HWND handle)
{
	try
	{
		HiddenPictureHandle = handle;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1020", __FILE__, __FUNCSIG__); }
}

//Set EDSC image size..
void RCadApp::SetImageSize_ED()
{
	try
	{
		int wwidth = getWindow(0).getWinDim().x, wheight = getWindow(0).getWinDim().y;
		if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
		{
			if(MAINDllOBJECT->DigitalZoomModeForOneShot())
				EDSCOBJECT->ResetImageSize(wwidth * 2, wheight * 2);
			else
				EDSCOBJECT->ResetImageSize(currCamWidth, currCamHeight);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1021", __FILE__, __FUNCSIG__); }
}

//Set Current Avtive window number
void RCadApp::SetCurrentWindow(int n)
{
	currentwindow = n;
}

void RCadApp::ResetCameraDistortionParameter(double distortionFactor, long Xoffset, long Yoffset)
{
	try
	{
		if(MAINDllOBJECT->CameraConnected())
			SetRadialImageCorrectionParams(distortionFactor, Xoffset, Yoffset);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1022", __FILE__, __FUNCSIG__); }
}

//Calculates the centerscreen for DRO Change..//
void RCadApp::centerScreen(double x, double y, double z, bool updateVideo)
{	
	try
	{
		//for change in DRO call mousemove... to update the current handler drawing...!
		UpdateMousePosition((int)getWindow(currentwindow).getLastWinMouse().x, (int)getWindow(currentwindow).getLastWinMouse().y, currentwindow);
		//Set the current DRO position..//
		getCurrentUCS().SetCurrentUCS_DRO(&MAINDllOBJECT->getCurrentDRO());
		if(!RcadGraphicsMoveWithDRO())
			getWindow(1).DontMoveWithDro(1);
		//Set the window centers..//
		if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
		{
			if (updateVideo)
				getWindow(0).centerCam(x - getCurrentUCS().UCSPoint.getX(), y - getCurrentUCS().UCSPoint.getY());
			//getWindow(1).centerCam(x - getWindow(1).getpt().x - getWindow(1).getHomediff().x, y - getWindow(1).getpt().y - getWindow(1).getHomediff().y); //- getCurrentUCS().UCSPoint.getX() //  - getCurrentUCS().UCSPoint.getY() /////// , 		
			//getWindow(2).centerCam(x, y); // -getWindow(2).getpt().x - getWindow(2).getHomediff().x, y - getWindow(2).getpt().y - getWindow(2).getHomediff().y);
			//if (updateVideo)
			//	getWindow(0).centerCam(x, y);
			//getWindow(1).centerCam(x - getWindow(1).getpt().x - getWindow(1).getHomediff().x, y - getWindow(1).getpt().y - getWindow(1).getHomediff().y);
			//getWindow(2).centerCam(x - getWindow(2).getpt().x - getWindow(2).getHomediff().x, y - getWindow(2).getpt().y - getWindow(2).getHomediff().y);
		}
		else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
		{
			if (updateVideo)
				getWindow(0).centerCam(y - getCurrentUCS().UCSPoint.getY(), z - getCurrentUCS().UCSPoint.getZ());
			//getWindow(1).centerCam(x - getCurrentUCS().UCSPoint.getY() - getWindow(1).getpt().x - getWindow(1).getHomediff().x, 
						//		   y - getCurrentUCS().UCSPoint.getZ() - getWindow(1).getpt().y - getWindow(1).getHomediff().y);
			getWindow(2).centerCam(x - getWindow(2).getpt().y - getWindow(2).getHomediff().y, 
								   y - getWindow(2).getpt().z - getWindow(2).getHomediff().z);
		}
		else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
		{
			if (updateVideo)
				getWindow(0).centerCam(x - getCurrentUCS().UCSPoint.getX(), z - getCurrentUCS().UCSPoint.getZ());
			//getWindow(1).centerCam(x - getCurrentUCS().UCSPoint.getX() - getWindow(1).getpt().x - getWindow(1).getHomediff().x, 
						//		   y - getCurrentUCS().UCSPoint.getZ() - getWindow(1).getpt().y - getWindow(1).getHomediff().y);
			getWindow(2).centerCam(x - getWindow(2).getpt().x - getWindow(2).getHomediff().x, 
								   y - getWindow(2).getpt().z - getWindow(2).getHomediff().z);
		}
		else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
		{
			if (updateVideo)
				getWindow(0).centerCam(y - getCurrentUCS().UCSPoint.getY(), getCurrentUCS().UCSPoint.getZ() - z);
			//getWindow(1).centerCam(x - getCurrentUCS().UCSPoint.getY() - getWindow(1).getpt().x - getWindow(1).getHomediff().x,
			//						-1 * (y - getCurrentUCS().UCSPoint.getZ() - getWindow(1).getpt().y - getWindow(1).getHomediff().y));
			getWindow(2).centerCam(x - getWindow(2).getpt().y - getWindow(2).getHomediff().y,
									-1 * (y - getWindow(2).getpt().z - getWindow(2).getHomediff().z));
		}
		else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
		{
			if (updateVideo)
				getWindow(0).centerCam(x - getCurrentUCS().UCSPoint.getX(), getCurrentUCS().UCSPoint.getZ()- z);
			//getWindow(1).centerCam(x - getCurrentUCS().UCSPoint.getX() - getWindow(1).getpt().x - getWindow(1).getHomediff().x,
			//						-1 * (y - getCurrentUCS().UCSPoint.getZ() - getWindow(1).getpt().y - getWindow(1).getHomediff().y));
			getWindow(2).centerCam(x - getWindow(2).getpt().x - getWindow(2).getHomediff().x,
									-1 * (y - getWindow(2).getpt().z - getWindow(2).getHomediff().z));
		}

		getWindow(0).WindowPanned(true);
		getWindow(1).WindowPanned(true);
		getWindow(2).WindowPanned(true);
		update_VideoGraphics();
		update_DxfGraphics();
		update_RcadGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1023", __FILE__, __FUNCSIG__); }
}

//Calculating Center of window during DRO changes..
void RCadApp::SetcenterScreen_Video(double x, double y,double z)
{
	try
	{
		getWindow(0).centerCam(x - getCurrentUCS().UCSPoint.getX(), y - getCurrentUCS().UCSPoint.getY());
		getWindow(0).WindowPanned(true);
		update_VideoGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1024", __FILE__, __FUNCSIG__); }
}

//Calculating Center of window during DRO changes..
void RCadApp::SetcenterScreen_Rcad(double x, double y,double z)
{
	try
	{
		//if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
		//	getWindow(1).centerCam(x - getCurrentUCS().UCSPoint.getX() - getWindow(1).getpt().x - getWindow(1).getHomediff().x, y - getCurrentUCS().UCSPoint.getY() - getWindow(1).getpt().y - getWindow(1).getHomediff().y);
		//else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
		//	getWindow(1).centerCam(x - getCurrentUCS().UCSPoint.getY() - getWindow(1).getpt().x - getWindow(1).getHomediff().x, y - getCurrentUCS().UCSPoint.getZ() - getWindow(1).getpt().y - getWindow(1).getHomediff().y);
		//else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
		//    getWindow(1).centerCam(x - getCurrentUCS().UCSPoint.getX() - getWindow(1).getpt().x - getWindow(1).getHomediff().x, y - getCurrentUCS().UCSPoint.getZ() - getWindow(1).getpt().y - getWindow(1).getHomediff().y);
		//else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
		//	getWindow(1).centerCam(x - getCurrentUCS().UCSPoint.getY() - getWindow(1).getpt().x - getWindow(1).getHomediff().x, 
		//							-1 * (y - getCurrentUCS().UCSPoint.getZ() - getWindow(1).getpt().y - getWindow(1).getHomediff().y));
		//else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
		//	getWindow(1).centerCam(x - getCurrentUCS().UCSPoint.getX() - getWindow(1).getpt().x - getWindow(1).getHomediff().x, 
		//							-1 * (y - getCurrentUCS().UCSPoint.getZ() - getWindow(1).getpt().y - getWindow(1).getHomediff().y));

		getWindow(1).WindowPanned(true);
		update_RcadGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1025", __FILE__, __FUNCSIG__); }
}

//Calculating Center of window during DRO changes..
void RCadApp::SetcenterScreen_Dxf(double x, double y,double z)
{
	try
	{
		//getWindow(2).centerCam(x - getWindow(2).getpt().x - getWindow(2).getHomediff().x, y - getWindow(2).getpt().y - getWindow(2).getHomediff().y);
		getWindow(2).WindowPanned(true);
		update_DxfGraphics(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1026", __FILE__, __FUNCSIG__); }
}

//Calculates the centerscreen for pan, Zoom/
void RCadApp::centerScreen_Update(double x, double y, double z)
{	
	try
	{
		////Set the window centers..//
		//getWindow(1).centerCam(x - getCurrentUCS().UCSPoint.getX() - getWindow(1).getpt().x - getWindow(1).getHomediff().x, y - getCurrentUCS().UCSPoint.getY() - getWindow(1).getpt().y - getWindow(1).getHomediff().y);
		getWindow(2).centerCam(x - getWindow(2).getpt().x - getWindow(2).getHomediff().x, y - getWindow(2).getpt().y - getWindow(2).getHomediff().y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1027", __FILE__, __FUNCSIG__); }
}

//Update the Mouse position for DRO Change..
void RCadApp::UpdateMousePosition(double x, double y, int window)
{
	try
	{
		if(window == 0 || window == 1 || window == 2)
		{
			currentwindow = window;
			getWindow(window).mouseMove(x, y);
			double CurrentMousePosX = getWindow(window).getPointer().x, CurrentMousePosY = getWindow(window).getPointer().y;
			currentHandler->mouseMove_x(CurrentMousePosX, CurrentMousePosY);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1028", __FILE__, __FUNCSIG__); }
}

//Set current DRO..//
void RCadApp::setCurrentDRO(double x, double y, double z, double r)
{
	try{ currentDRO.set(x, y, z, r); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1029", __FILE__, __FUNCSIG__); }
}

//Current DRO...//
Vector& RCadApp::getCurrentDRO()
{
	return currentDRO;
}

//Update Video Graphics..
void RCadApp::update_VideoGraphics(bool GiveDelay)
{
	try
	{
		if(dontUpdateGraphcis || VideoGraphicsUpdating) 
		{
			CaptureVideo = false;
			if(!VideoGraphicsUpdateQueue && VideoGraphicsUpdating)
				VideoGraphicsUpdateQueue = true;
			return;
		}
		//If any one of the other window is maximised then skip video graphics rendering.!!
		if(getWindow(1).maxed() || getWindow(2).maxed())
		{
			if(!UpdteVgp_UcsChange && !CaptureVideo)
			{
				ResetVideoShapeUpdateFlag();
				return;
			}
		}
		SetEvent(Video_Semaphore); 
		if(GiveDelay) Sleep(5);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1030", __FILE__, __FUNCSIG__); }
}

//Update Rcad Graphics...
void RCadApp::update_RcadGraphics(bool GiveDelay)
{
	try
	{
		if(dontUpdateGraphcis || RcadGraphicsUpdating) 
		{
			CaptureCad = false;
			if(!RcadGraphicsUpdateQueue && RcadGraphicsUpdating)
				RcadGraphicsUpdateQueue = true;
			return;
		}
		if(getWindow(2).maxed())
		{
			if(!UpdteCadgp_UcsChange && !CaptureCad)
			{
				RShape_Updated = false;
				RSelectedShape_Updated = false;
				RSelectedMeasurement_Updated = false;
				RMeasurement_Updated = false;
				return;
			}
		}
		SetEvent(Rcad_Semaphore); 
		if(GiveDelay) Sleep(5);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1031", __FILE__, __FUNCSIG__); }
}

//Update Dxf Graphics..
void RCadApp::update_DxfGraphics(bool GiveDelay)
{
	try
	{
		if(dontUpdateGraphcis || DxfGraphicsUpdating)
		{
			if(DxfGraphicsUpdating)
				DxfGraphicsUpdateQueue = true;
			return;
		}
		if(getWindow(1).maxed())
			return;
		SetEvent(Dxf_Semaphore); 
		if(GiveDelay) Sleep(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1032", __FILE__, __FUNCSIG__); }
}

//Wait Video graphics update..
void RCadApp::Wait_VideoGraphicsUpdate()
{
	Video_Cnt = 0;
	//while(VideoGraphicsUpdating)
	//{
	//	Video_Cnt++;
	//}
}

//Wait Rcad graphics update..
void RCadApp::Wait_RcadGraphicsUpdate()
{
	Rcad_Cnt = 0;
	//while(RcadGraphicsUpdating)
	//{

	//	Rcad_Cnt++;
	//}
}

//Wait Dxf graphics Update...
void RCadApp::Wait_DxfGraphicsUpdate()
{
	Dxf_Cnt = 0;
	//while(DxfGraphicsUpdating)
	//{
	//	Dxf_Cnt++;
	//}
}

//Wait for Save Actions
void RCadApp::Wait_SaveActionsWorkSpace()
{
	Save_Cnt = 0;
	//while(ActionSavingFlag)
	//{
	//	Save_Cnt++;
	//}
}

//Update Graphics of video, Rcad, Dxf window...
void RCadApp::UpdateGraphicsOf3Window()
{
	try
	{
		update_VideoGraphics(true);
		update_RcadGraphics(true);
		update_DxfGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1033", __FILE__, __FUNCSIG__); }
}

// Set Flags to Update all the graphics..//
void RCadApp::updateAll()
{
	try
	{
		getWindow(0).WindowPanned(true);
		getWindow(1).WindowPanned(true);
		getWindow(2).WindowPanned(true);
		UpdateShapesChanged();
		DXFEXPOSEOBJECT->UpdateForUCS();
		DXFShape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1034", __FILE__, __FUNCSIG__); }
}

// Set Flags to Update the Shapes and Measurement graphics
void RCadApp::UpdateShapesChanged(bool updategraphics)
{
	try
	{
		if(!VMeasurement_Updated)
			this->VMeasurement_Updated = true;
		if(!RMeasurement_Updated)
			this->RMeasurement_Updated = true;
		if(!VShape_Updated)
			this->VShape_Updated = true;
		if(!RShape_Updated)
			this->RShape_Updated = true;
		if(updategraphics)
		{
			update_VideoGraphics(true);
			update_RcadGraphics(true);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1035", __FILE__, __FUNCSIG__); }
}

//Set Flags to Update the Shape list..//
void RCadApp::Shape_Updated(bool updategraphics)
{
	try
	{
		if(!VShape_Updated)
			this->VShape_Updated = true;
		if(!RShape_Updated)
			this->RShape_Updated = true;
		if(updategraphics)
		{
			update_VideoGraphics(true);
			update_RcadGraphics(true);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1036", __FILE__, __FUNCSIG__); }
}

//Set Flags to Update the Measurement list..//
void RCadApp::Measurement_updated(bool updategraphics)
{
	try
	{
		if(!VMeasurement_Updated)
			this->VMeasurement_Updated = true;
		if(!RMeasurement_Updated)
			this->RMeasurement_Updated = true;
		if(updategraphics)
		{
			update_VideoGraphics(true);
			update_RcadGraphics(true);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1037", __FILE__, __FUNCSIG__); }
}

//Set Flags to Update Selected Shape list..//
void RCadApp::SelectedShape_Updated(bool updategraphics)
{
	try
	{
		if(!VSelectedShapeUpdated)
			this->VSelectedShapeUpdated = true;
		if(!RSelectedShape_Updated)
			this->RSelectedShape_Updated = true;
		if(updategraphics)
		{
			update_VideoGraphics(true);
			update_RcadGraphics(true);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1038", __FILE__, __FUNCSIG__); }
}

//Set Flags to Update Selected Measurement list..//
void RCadApp::SelectedMeasurement_Updated(bool updategraphics)
{
	try
	{
		if(!VSelectedMeasurement_Updated)
			this->VSelectedMeasurement_Updated = true;
		if(!RSelectedMeasurement_Updated)
			this->RSelectedMeasurement_Updated = true;
		if(updategraphics)
		{
			update_VideoGraphics(true);
			update_RcadGraphics(true);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1039", __FILE__, __FUNCSIG__); }
}

void RCadApp::Entity_UpdatedOnRcad(bool updategraphics)
{
	try
	{
		if(!RMeasurement_Updated)
			this->RMeasurement_Updated = true;
		if(!RShape_Updated)
			this->RShape_Updated = true;
		if(updategraphics)
			update_RcadGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1040", __FILE__, __FUNCSIG__); }
}

//Set Flags to Update DXF Shape list..//
void RCadApp::DXFShape_Updated(bool updategraphics)
{
	try
	{
		this->VDXFShape_updated = true;
		this->RDXFShape_updated = true;
		this->RDXFMeasure_updated = true;
		if(updategraphics)
		{
			update_VideoGraphics(true);
			update_DxfGraphics();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1041", __FILE__, __FUNCSIG__); }
}

//Set Flags to Update DXF Measurement list..//
void RCadApp::DXFMeasurement_Updated(bool updategraphics)
{
	try
	{
		this->RDXFMeasure_updated = true;
		if(updategraphics)
			update_DxfGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1042", __FILE__, __FUNCSIG__); }
}

//Set Flags to Update DXF Shape list..//
void RCadApp::DXFShape_Updated_Alignment(bool updategraphics)
{
	try
	{
		this->VShape_Updated = true;
		this->RShape_Updated = true;
		this->RDXFShape_updated = true;
		if(updategraphics)
		{
			update_VideoGraphics();
			update_RcadGraphics();
			update_DxfGraphics();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1043", __FILE__, __FUNCSIG__); }
}

void RCadApp::PartProgramShape_updated(bool updategraphics)
{
	try
	{
		if(!this->PartprogramShape_Updated)
			this->PartprogramShape_Updated = true;
		if(updategraphics)
			update_DxfGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1044", __FILE__, __FUNCSIG__); }
}

//Calculate and Redraw all the measurements..
void RCadApp::UpdateAllMeasurements()
{
	try
	{
		dontUpdateGraphcis = true;
		for(RC_ITER UCSitem = getUCSList().getList().begin(); UCSitem != getUCSList().getList().end(); UCSitem++)
		{
			UCS* Cucs = (UCS*)(*UCSitem).second;
			for(RC_ITER Dimitem = Cucs->getDimensions().getList().begin(); Dimitem != Cucs->getDimensions().getList().end(); Dimitem++)
			{
				DimBase* dim = (DimBase*)(*Dimitem).second;
				dim->UpdateMeasurement();
			}
		}
		dontUpdateGraphcis = false;
		Measurement_updated();
	}
	catch(...){ dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1045", __FILE__, __FUNCSIG__); }
}

void RCadApp::ClearShapeMeasureSelections()
{
	try
	{
		Wait_RcadGraphicsUpdate();
		Wait_VideoGraphicsUpdate();
		getDimList().clearSelection();
		getShapesList().clearSelection();
		UpdateShapesChanged();
		Sleep(10);
		Wait_RcadGraphicsUpdate();
		Wait_VideoGraphicsUpdate();
	}
	catch(...){ dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1046", __FILE__, __FUNCSIG__); }
}

//Clear Shape Selections
void RCadApp::ClearShapeSelections()
{
	try
	{
		Wait_RcadGraphicsUpdate();
		Wait_VideoGraphicsUpdate();
		getShapesList().clearSelection();
		Shape_Updated();
		Sleep(10);
		Wait_RcadGraphicsUpdate();
		Wait_VideoGraphicsUpdate();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1047", __FILE__, __FUNCSIG__); }
}

//Clear Measure Selections
void RCadApp::ClearMeasureSelections()
{
	try
	{
		Wait_RcadGraphicsUpdate();
		Wait_VideoGraphicsUpdate();
		getDimList().clearSelection();
		Measurement_updated();
		Sleep(10);
		Wait_RcadGraphicsUpdate();
		Wait_VideoGraphicsUpdate();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1048", __FILE__, __FUNCSIG__); }
}

//hide the dxf on video...//
void RCadApp::ShowHideDXFOnVideo()
{
	try
	{
		ShowDXFShapesOnVideo.Toggle();
		update_VideoGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1049", __FILE__, __FUNCSIG__); }
}

//hide the dxf on video...//
void RCadApp::ToggleWiredLoopMode(bool flag)
{
	try
	{
		DrawWiredSurface(flag);
		Wait_VideoGraphicsUpdate();
		Wait_RcadGraphicsUpdate();
		UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1050", __FILE__, __FUNCSIG__); }
}

//Toggle transparency..
void RCadApp::ToggleTransparencyMode(bool flag)
{
	try
	{
		AllowTransparencyTo3DShapes(flag);
		Wait_VideoGraphicsUpdate();
		Wait_RcadGraphicsUpdate();
		UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1051", __FILE__, __FUNCSIG__); }
}

//Toggle cylinder disk draw..
void RCadApp::ToggleCylinderDiskDrawMode()
{
	try
	{
		DrawCylinderdisks(!DrawCylinderdisks());
		Wait_VideoGraphicsUpdate();
		Wait_RcadGraphicsUpdate();
		UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1052", __FILE__, __FUNCSIG__); }
}

//update transparency level of the shape..
void RCadApp::SetRapidShapeTransparency(double Alpha_val)
{
	try
	{
		this->Alpha_Value = Alpha_val;
		Wait_VideoGraphicsUpdate();
		Wait_RcadGraphicsUpdate();
		UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1053", __FILE__, __FUNCSIG__); }
}

//Recalculate parameters and update shapes
void RCadApp::RecalculateAndUpdateShapes(RapidEnums::SHAPETYPE ShType)
{
	try
	{
		Wait_VideoGraphicsUpdate();
		Wait_RcadGraphicsUpdate();
		dontUpdateGraphcis = true;
		RCollectionBase& ShapeCollection = getShapesList();
		if(ShType == RapidEnums::SHAPETYPE::DEFAULTSHAPE)
		{
			for(RC_ITER ShItem = ShapeCollection.getList().begin(); ShItem != ShapeCollection.getList().end(); ShItem++)
				((ShapeWithList*)(*ShItem).second)->UpdateShape(false);
		}
		else
		{
			for(RC_ITER ShItem = ShapeCollection.getList().begin(); ShItem != ShapeCollection.getList().end(); ShItem++)
			{
				ShapeWithList* CShape = (ShapeWithList*)(*ShItem).second;
				if(CShape->ShapeType == ShType)
					CShape->UpdateShape(false);
			}
		}
		dontUpdateGraphcis = false;
		UpdateShapesChanged();
	}
	catch(...){ dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1054", __FILE__, __FUNCSIG__); }
}

//hide graphics flag...//
void RCadApp::hideGraphics_OnVideo(bool flag)
{
	try
	{
		this->ClearGraphicsOnVideo(flag);
		this->HideGraphicsOnVideo(flag);
		if(!flag)
			Shape_Updated();
		else
			update_VideoGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1055", __FILE__, __FUNCSIG__); }
}

//udpate
void RCadApp::UpdateVideoViewPortSettings()
{
	try
	{
		getWindow(0).WindowPanned(true);
		update_VideoGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1056", __FILE__, __FUNCSIG__); }
}

//Update the partprogram graphics....///
void RCadApp::updatePPGraphics(bool flag)
{
	try
	{
		if(flag)
		{
			if(!this->PartprogramShape_Updated)
				this->PartprogramShape_Updated = true;
		}
		UpdateShapesChanged();
		update_DxfGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1057", __FILE__, __FUNCSIG__); }
}

//Render Video window graphics
void RCadApp::Draw_VideoWindowGraphics()
{
	try
	{
		if (Video_Graphics == NULL) return;

		VideoGraphicsUpdating = true;
		RWindow& VideoWin = getWindow(0);
		double VideoUpp = VideoWin.getUppX();
		//Select the current window...
		Video_Graphics->SelectWindow();
		//If the Window View Is changed.. Set the Window ViewPort..
		//GRAFIX->DrawVideoTextureBackGround();
		if(VideoWin.WindowPanned())
		{
			//To keep Fixed Graphcis at position.
			getSpecialToolHandler()->SetWindowCenter(VideoWin.getCenter().x, VideoWin.getCenter().y);
			//Keeping Framegrab at fixed position during Part program run.
			if(PPCALCOBJECT->UpdateFistFramegrab())
				PPCALCOBJECT->ChangePosition_FrameGrab();
			
			double ext[4];
			VideoWin.getExtents(ext);
			Video_Graphics->setView(ext[0], ext[1], ext[2], ext[3], VideoWin.getUppX(), VideoWin.getCenter().x, VideoWin.getCenter().y, CameraConnected());
			GRAFIX->SetWindowSettings(VideoWin.getUppX(), VideoWin.getCenter().x, VideoWin.getCenter().y, 0);
			if(VideoWin.WindowResized())
			{
				Video_Graphics->resize((int)VideoWin.getWinDim().x, (int)VideoWin.getWinDim().y);
				VideoWin.WindowResized(false);
			}
			VideoWin.WindowPanned(false);
		}
		if(this->HideGraphicsOnVideo()) //Hide all graphics one Video..
		{
			//this is done to clear the graph that was set previously inside cam dll
			if(CameraConnected() && this->ClearGraphicsOnVideo())
			{
				this->ClearGraphicsOnVideo(false);
				memset(pixels, 102, this->currCamWidth * this->currCamHeight * 4); // 24000000);
				SetGraph32(&pixels[0]);
			}
			ResetVideoShapeUpdateFlag();
			//Video_Graphics->render();
			CaptureVideo = false;
			VideoMouseFlag = false;
			VideoGraphicsUpdating = false;
			if (MAINDllOBJECT->getCurrentUCS().UCSangle() == 0)
				GRAFIX->drawCrossHair(HatchedCrossHairMode(), !ScaleViewMode(), &MAINDllOBJECT->Offset_Normal[0], ShiftUpperCrossHair());
			else
				GRAFIX->drawCrossHair(HatchedCrossHairMode(), !ScaleViewMode(), &MAINDllOBJECT->Offset_Skewed[0], ShiftUpperCrossHair());

			if(CameraConnected())
			{
				if(!(PPCALCOBJECT->IsPartProgramRunning() && MAINDllOBJECT->Derivedmodeflag))
				{
					if(DigitalZoomMode())
						Video_Graphics->readPixelsDigitalZoom(&pixels[0]);
					else
						Video_Graphics->readPixels(&pixels[0], currCamWidth / HelperInst->CamSizeRatio, currCamHeight / HelperInst->CamSizeRatio);
					SetGraph32(&pixels[0]);
				}
			}
			Video_Graphics->render(!CameraConnected());
			return;
		}								
		if(HideGraphicsOnVideoSettingsMode())
		{
			Video_Graphics->translateMatrix(0.0, 0.0, VideoUpp);
			GRAFIX->SetColor_Double(1, 0, 1);
			if(CURRENTRAPIDHANDLER == RapidEnums::RAPIDDRAWHANDLERTYPE::RAPIDFRAMEGRAB_HANDLER)
				currentHandler->draw(0, VideoUpp);
			Video_Graphics->translateMatrix(0.0, 0.0, -VideoUpp);
			//Set crosshair color and draw it...//
			if(ShowHideCrossHair() && (!CaptureVideo || SaveCrossHairWithImage()))
			{
				if(CALBCALCOBJECT->CalibrationModeFlag())
				{
					if(CALBCALCOBJECT->XWidthMeasurementMode())
						GRAFIX->drawYCrossHair(HatchedCrossHairMode(), 3);
					else if(CALBCALCOBJECT->YWidthMeasurementMode())
						GRAFIX->drawXCrossHair(HatchedCrossHairMode(), 3);
					else if (CALBCALCOBJECT->AngleWidthMeasurementMode())
					{
						//GRAFIX->drawAngleCrossHair(HatchedCrossHairMode(), M_PI_4 * 3, 3/*, CALBCALCOBJECT->Length(), CALBCALCOBJECT->Width()*/);
						if (MAINDllOBJECT->getCurrentUCS().UCSangle() == 0)
							GRAFIX->drawCrossHair(HatchedCrossHairMode(), !ScaleViewMode(), &MAINDllOBJECT->Offset_Normal[0], ShiftUpperCrossHair());
						else
							GRAFIX->drawCrossHair(HatchedCrossHairMode(), !ScaleViewMode(), &MAINDllOBJECT->Offset_Skewed[0], ShiftUpperCrossHair());
					}
				}
				else
				{
					//GRAFIX->drawCrossHair(HatchedCrossHairMode(), !ScaleViewMode(), &MAINDllOBJECT->Offset_Normal[0], ShiftUpperCrossHair());
					//if(ScaleViewMode()) //Draw scaleview.....
					//	GRAFIX->drawScaleViewMode(VideoUpp, VideoWin.getWinDim().x, VideoWin.getWinDim().y, MeasureUnitType);
					if (MAINDllOBJECT->getCurrentUCS().UCSangle() == 0)
						GRAFIX->drawCrossHair(HatchedCrossHairMode(), !ScaleViewMode(), &MAINDllOBJECT->Offset_Normal[0], ShiftUpperCrossHair());
					else
						GRAFIX->drawCrossHair(HatchedCrossHairMode(), !ScaleViewMode(), &MAINDllOBJECT->Offset_Skewed[0], ShiftUpperCrossHair());
				}
			}
			ResetVideoShapeUpdateFlag();
		}
		else
		{
			//Draw the current shape...
//			Video_Graphics->translateMatrix(240*VideoUpp, -60*VideoUpp, 0);
			Video_Graphics->translateMatrix(0.0, 0.0, VideoUpp);
			GRAFIX->SetColor_Double(1, 0, 1);
			if(currentHandler != NULL)
				currentHandler->draw(0, VideoUpp);
			Video_Graphics->translateMatrix(0.0, 0.0, -VideoUpp);

			if(VShape_Updated)
			{
				WaitForSingleObject(ShapeMutex, INFINITE);
				try
				{
					RCollectionBase& TempShapeColl = getShapesList();
					buildVideoShapeList(TempShapeColl, VideoUpp);
					ReleaseMutex(ShapeMutex);
				}
				catch(...)
				{
					ReleaseMutex(ShapeMutex);
					throw "videodrawexcp";
				}
				VSelectedShapeUpdated = true;
			}
			if(VSelectedShapeUpdated)
			{
				WaitForSingleObject(SelectedShapeMutex, INFINITE);
				try
				{
					RCollectionBase& TempShapeColl = getSelectedShapesList();
					buildVideoSelectedShapeList(TempShapeColl, VideoUpp);
					ReleaseMutex(SelectedShapeMutex);
				}
				catch(...)
				{
					ReleaseMutex(SelectedShapeMutex);
					throw "videodrawexcp";
				}
				VSelectedShapeUpdated = false;
				VShape_Updated = false;
			}
			if(VMeasurement_Updated)
			{
				WaitForSingleObject(MeasureMutex, INFINITE);
				try
				{
					RCollectionBase& TempDimColl = getDimList();
					buildVideoMeasurementList(TempDimColl, VideoUpp);
					ReleaseMutex(MeasureMutex);
				}
				catch(...)
				{
					ReleaseMutex(MeasureMutex);
					throw "videodrawexcp";
				}
				VSelectedMeasurement_Updated = true;
			}
			if(VSelectedMeasurement_Updated)
			{
				WaitForSingleObject(SelectedMeasureMutex, INFINITE);
				try
				{
					RCollectionBase& TempDimColl = getSelectedDimList();
					buildVideoSelectedMeasurementList(TempDimColl, VideoUpp);
					ReleaseMutex(SelectedMeasureMutex);
				}
				catch(...)
				{
					ReleaseMutex(SelectedMeasureMutex);
					throw "videodrawexcp";
				}
				VSelectedMeasurement_Updated = false;
				VMeasurement_Updated = false;
			}
			if(VDXFShape_updated)
			{
				WaitForSingleObject(DxfMutex, INFINITE);
				try
				{
					list<Shape*> &TempShapeColl = DXFEXPOSEOBJECT->CurrentDXFShapeList;
					list<DimBase*> &TempDimColl = DXFEXPOSEOBJECT->CurrentDXFMeasureList;
					buildVideoDXFList(TempShapeColl, TempDimColl, VideoUpp);
					ReleaseMutex(DxfMutex);
				}
				catch(...)
				{
					ReleaseMutex(DxfMutex);
					throw "videodrawexcp";
				}
				VDXFShape_updated = false;
			}

			//Show the measurements on video..//
			if(ShowMeasurementOnVideo())
			{
				GRAFIX->CalldedrawGlList(3);
				GRAFIX->CalldedrawGlList(11);
			}
			//Show the DXF shapes on Video..//
			if(ShowDXFShapesOnVideo())
				GRAFIX->CalldedrawGlList(5);
			GRAFIX->CalldedrawGlList(1);
			GRAFIX->CalldedrawGlList(9);					
			//Draw the extension line for current implicit snap point...//
			if(CurrentExtensionFlag)
				DrawExtensionForImplicitSnap(getCurrentUCS().getIPManager().getParent1(), getCurrentUCS().getIPManager().getParent2(), highlightedPoint());
			//Rotate the graphics..//
			if(Video_Graphics->getRotate())
				Video_Graphics->RotateGL();

			//if(getShapesList().count() > 2)
			//{
			//	/*double leftx = getWindow(0).getPointer().x, lefty = getWindow(0).getPointer().y;
			//	RaiseDrawNormalString(60, "Tahoma", "Test Rapid-I", leftx, lefty, 0, 0);
			//	GRAFIX->RenderTextureInCube();*/
			//	double leftx = getWindow(0).getPointer().x, lefty = getWindow(0).getPointer().y;
			//	if(CreateTextureFlag)
			//	{
			//		
			//		RaiseDrawNormalString(20, "Tahoma", "Test Rapid-I", leftx, lefty, 0, VideoUpp);
			//		CreateTextureFlag = false;
			//	}
			//	else
			//		GRAFIX->RenderTextureInCube(leftx, lefty, 0, VideoUpp);
			//}
	


			//Draw fixed graphs....
			FixedToolHandler->drawAll();
				
			//Set crosshair color and draw it...//
			if(ShowHideCrossHair() && (!CaptureVideo || SaveCrossHairWithImage()))
			{
				if(CALBCALCOBJECT->CalibrationModeFlag())
				{
					if(CALBCALCOBJECT->OneShotMode())
					{
						GRAFIX->drawLineFragment(HatchedCrossHairMode(), CALBCALCOBJECT->LineX1(), CALBCALCOBJECT->LineY1(), CALBCALCOBJECT->LineX2(), CALBCALCOBJECT->LineY2(), 3);
					}
					else
					{
						if(CALBCALCOBJECT->XWidthMeasurementMode())
							GRAFIX->drawYCrossHair(HatchedCrossHairMode(), 3);
						else if(CALBCALCOBJECT->YWidthMeasurementMode())
							GRAFIX->drawXCrossHair(HatchedCrossHairMode(), 3);
						else if(CALBCALCOBJECT->AngleWidthMeasurementMode())
							GRAFIX->drawAngleCrossHair(HatchedCrossHairMode(), M_PI_4 * 3, 3/*, CALBCALCOBJECT->Length(), CALBCALCOBJECT->Width()*/);
					}
				}
				else
				{
					//GRAFIX->drawCrossHair(HatchedCrossHairMode(), !ScaleViewMode(), &MAINDllOBJECT->Offset_Normal[0], ShiftUpperCrossHair());
					if (MAINDllOBJECT->getCurrentUCS().UCSangle() == 0)
						GRAFIX->drawCrossHair(HatchedCrossHairMode(), !ScaleViewMode(), &MAINDllOBJECT->Offset_Normal[0], ShiftUpperCrossHair());
					else
						GRAFIX->drawCrossHair(HatchedCrossHairMode(), !ScaleViewMode(), &MAINDllOBJECT->Offset_Skewed[0], ShiftUpperCrossHair());

					if(ScaleViewMode()) //Draw scaleview.....
						GRAFIX->drawScaleViewMode(VideoUpp, VideoWin.getWinDim().x, VideoWin.getWinDim().y, MeasureUnitType);
				}
			}
			//draw the Current snap point/ Shape name..//
			buildVideoSnapPointNameList(VideoUpp);
			GRAFIX->CalldedrawGlList(14);		

			GRAFIX->SetColor_Double(1, 0, 1);																//pink color

			Video_Graphics->translateMatrix(0.0, 0.0, VideoUpp);
			//Draw focus rectangles...//
			FOCUSCALCOBJECT->drawFocusRectangels();
			if(DrawRectanle_ImageDelay)
				IMAGEDELAYOBJECT->DrawRectangle();
			Video_Graphics->translateMatrix(0.0, 0.0, -VideoUpp);
//			Video_Graphics->translateMatrix(-240*VideoUpp, 60*VideoUpp, 0);
		}
		//Set the camera graph as the graphics pixels
		if(CameraConnected())
		{
			if(!(PPCALCOBJECT->IsPartProgramRunning() && MAINDllOBJECT->Derivedmodeflag))
			{
				if(DigitalZoomMode())
					Video_Graphics->readPixelsDigitalZoom(&pixels[0]);
				else
					Video_Graphics->readPixels((void*)pixels, currCamWidth /  HelperInst->CamSizeRatio, currCamHeight / HelperInst->CamSizeRatio); // 
				
				SetGraph32(&pixels[0]);
				//SetGraph24(pixels);
			}
		}
		else
		{
			if (MAINDllOBJECT->InsertStaticImageinVideo)
			{
				if (DigitalZoomMode())
					Video_Graphics->readPixelsDigitalZoom(&pixels[0]);
				else
					Video_Graphics->readPixels(&pixels[0], currCamWidth / HelperInst->CamSizeRatio, currCamHeight / HelperInst->CamSizeRatio); // 
				//for (int i = 0; i < )
			}
			//else
		}
				Video_Graphics->render(!CameraConnected());
		//Flag for mouse move....
		CaptureVideo = false;
		VideoMouseFlag = false;
		UpdteVgp_UcsChange = false;
		VideoGraphicsUpdating = false;
	}
	catch(...)
	{
		CaptureVideo = false; VideoMouseFlag = false;
		UpdteVgp_UcsChange = false; VideoGraphicsUpdating = false; 
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1058", __FILE__, __FUNCSIG__); 
	}
}

//Render Rcad Window graphics
void RCadApp::Draw_RcadWindowGraphics()
{ 
	try
	{		
		RcadGraphicsUpdating = true;
		RWindow& RcadWin = getWindow(1);
		double RcadUpp = RcadWin.getUppX();
		//Select the Rcad window...//
		Rcad_Graphics->SelectWindow();
		//change background to gradient
		//CALIBRATIONTEXT->drawCalibrationText(MAINDllOBJECT->getWindow(1).getPointer().x - MAINDllOBJECT->getWindow(1).getCenter().x ,MAINDllOBJECT->getWindow(1).getPointer().y - MAINDllOBJECT->getWindow(1).getCenter().y, (char*)"vijay\nkanth", MAINDllOBJECT->getWindow(1).getUppX() * 20 );
		
		if(RcadWindow3DMode())
		{
			GRAFIX->DrawBackGround(true);
		}
		if(MAINDllOBJECT->ShowPictorialView() && !MAINDllOBJECT->TriangulateCpSurface())
		{
			if(abs(CloudPtZExtent[0] - CloudPtZExtent[1]) > 0.01)
			{
				double LeftTop[2] = {RcadWin.getCam().x, RcadWin.getCam().y};
				GRAFIX->DrawColorIndicator(CloudPtZExtent, LeftTop, MAINDllOBJECT->ColorThreshold, RcadUpp);
			}
		}
		if(PlaceDetail)
			CALIBRATIONTEXT->drawCalibrationText(RcadWin.getPointer().x ,RcadWin.getPointer().y, MAINDllOBJECT->getWindow(1).getUppX() * 20 );
		if(PlaceTable)
			CALIBRATIONTEXT->drawCalibrationMeasurement(RcadWin.getPointer().x  ,RcadWin.getPointer().y , RcadWin.getUppX() * 20, RcadWin.getUppY() );
		
		if(RcadWin.WindowPanned())
		{
			double ext[4];
			RcadWin.getExtents(ext);
			Rcad_Graphics->setView( ext[0], ext[1], ext[2], ext[3], RcadWin.getUppX(), RcadWin.getCenter().x, RcadWin.getCenter().y);
			GRAFIX->SetWindowSettings(RcadWin.getUppX(), RcadWin.getCenter().x, RcadWin.getCenter().y, 1);
			RcadWin.WindowPanned(false);
			if(RcadWin.WindowResized())
			{
				Rcad_Graphics->resize((int)RcadWin.getWinDim().x, (int)RcadWin.getWinDim().y);
				RcadWin.WindowResized(false);
			}
		}
		if(RShape_Updated)
		{
			WaitForSingleObject(ShapeMutex, INFINITE);
			try
			{
				RCollectionBase& TempShapeColl = getShapesList();
				buildRcadShapeList(TempShapeColl, RcadUpp);
				ReleaseMutex(ShapeMutex);
			}
			catch(...)
			{
				ReleaseMutex(ShapeMutex);
				throw "rcaddrawexcp";
			}
			RSelectedShape_Updated = true;
		}
		if(RSelectedShape_Updated)
		{
			WaitForSingleObject(SelectedShapeMutex, INFINITE);
			try
			{
				RCollectionBase& TempShapeColl = getSelectedShapesList();
				buildRcadSelectedShapeList(TempShapeColl, RcadUpp);
				ReleaseMutex(SelectedShapeMutex);
			}
			catch(...)
			{
				ReleaseMutex(SelectedShapeMutex);
				throw "rcaddrawexcp";
			}
			RShape_Updated = false;
			RSelectedShape_Updated = false;
		}
		if(RMeasurement_Updated)
		{
			WaitForSingleObject(MeasureMutex, INFINITE);
			try
			{
				RCollectionBase& TempDimColl = getDimList();
				buildRcadMeasurementList(TempDimColl, RcadUpp);
				ReleaseMutex(MeasureMutex);
			}
			catch(...)
			{
				ReleaseMutex(MeasureMutex);
				throw "rcaddrawexcp";
			}
			RSelectedMeasurement_Updated = true;	
		}
		if(RSelectedMeasurement_Updated)
		{
			WaitForSingleObject(SelectedMeasureMutex, INFINITE);
			try
			{
				RCollectionBase& TempDimColl = getSelectedDimList();
				buildRcadSelectedMeasurementList(TempDimColl, RcadUpp);
				ReleaseMutex(SelectedMeasureMutex);
			}
			catch(...)
			{
				ReleaseMutex(SelectedMeasureMutex);
				throw "rcaddrawexcp";
			}
			RSelectedMeasurement_Updated = false;
			RMeasurement_Updated = false;
		}

		//if(getShapesList().count() > 2)
		//{
		//	/*double leftx = getWindow(1).getPointer().x, lefty = getWindow(1).getPointer().y;
		//	RaiseDrawNormalString(14, "Tahoma", "Test Rapid-I", leftx, lefty, 0, 0);
		//	GRAFIX->RenderTextureInCube();*/
		//	if(CreateTextureFlag)
		//	{
		//		double leftx = getWindow(1).getPointer().x, lefty = getWindow(1).getPointer().y;
		//		RaiseDrawNormalString(14, "Tahoma", "Test Rapid-I", leftx, lefty, 0, 0);
		//		CreateTextureFlag = false;
		//	}
		//	else
		//		GRAFIX->RenderTextureInCube();
		//}


		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(!RcadWindow3DMode())
			{
				Rcad_Graphics->translateMatrix(0.0, 0.0, -4);
				RWindow& VideoWin = getWindow(0);
				//double leftx = getWindow(1).getPointer().x, lefty = getWindow(1).getPointer().y;
				double leftx = VideoWin.getCam().x, lefty = VideoWin.getCam().y;
				double rightx = leftx + VideoWin.getViewDim().x, righty = lefty - VideoWin.getViewDim().y;
				GRAFIX->SetColor_Double_Alpha(0.9, 0.9, 0.9, 0.5);
				GRAFIX->EnableTransparency();
				GRAFIX->drawRectangle(leftx, lefty, rightx, righty, true);
				GRAFIX->DisableTransparency();
				Rcad_Graphics->translateMatrix(0.0, 0.0, 4);
			}
		}
	
		//Shape list and measurement list..//
		//Rcad_Graphics->translateMatrix(0.0, 0.0, -RcadUpp);
		GRAFIX->CalldedrawGlList(4);
		GRAFIX->CalldedrawGlList(2);
		GRAFIX->CalldedrawGlList(10);
		GRAFIX->CalldedrawGlList(12);
		//Rcad_Graphics->translateMatrix(0.0, 0.0, RcadUpp);
		//snap point and shape name
		buildRcadSnapPointNameList(RcadUpp);
		GRAFIX->CalldedrawGlList(15);

		//Set the graphics color..//
		GRAFIX->SetColor_Double(1, 0, 1);
		Rcad_Graphics->translateMatrix(0.0, 0.0, RcadUpp);
		if(currentHandler != NULL)
			currentHandler->draw(1, RcadUpp);
		Rcad_Graphics->translateMatrix(0.0, 0.0, -RcadUpp);
		//Draw the extension line for implicit snap point...//
		if(CurrentExtensionFlag)
			DrawExtensionForImplicitSnap(getCurrentUCS().getIPManager().getParent1(), getCurrentUCS().getIPManager().getParent2(), highlightedPoint());
		
		//draw parptorgram arrow and path
		PPCALCOBJECT->drawPartprogramArrow(RcadUpp);

		//If rotate mode then rotate the grpahics..//
		if(Rcad_Graphics->getRotate())
			Rcad_Graphics->RotateGL();
				
		GRAFIX->SetColor_Double(1, 0, 0);
		if(!CaptureCad || SaveCrossHairWithImage())
		{
			if(RcadWindow3DMode())
			{
				GRAFIX->drawAxes_3DWithArrow(0, 0, 0, RcadWin.getUppX());
				//GRAFIX->drawAxes_3DWithArrow(RcadWin.getCenter().x, RcadWin.getCenter().y, 0, RcadWin.getUppX());
				if(IsProbePresent() && ShowProbeDraw())
				{
					GRAFIX->drawProbe_Z((getCurrentDRO().getX() + ProbeOffsetX), (getCurrentDRO().getY() + ProbeOffsetY), (getCurrentDRO().getZ() + ProbeOffsetZ));
					for(map<int, double*>::iterator It = ProbePathPoints.begin(); It != ProbePathPoints.end(); It++)
					{
						GRAFIX->EnableTransparency();
						GRAFIX->EnableLighting();
						GRAFIX->drawSphere_Probe(It->second[0], It->second[1], It->second[2], 5 * getWindow(1).getUppX());
						GRAFIX->DisableLighting();
						GRAFIX->DisableTransparency();
					}		
				}
			}
			else
				GRAFIX->drawAxes(RcadUpp, Xaxis_Highlighted, Yaxis_Highlighted);
		}
		
		GRAFIX->SetColor_Double(1, 0, 1);														//pink color

		//Reset to normal view..(after 3D rotation)
		if(ResetRcadRotation)
		{
			ResetRcadRotation = false;
			GRAFIX->LoadGraphicsIdentityMatrix();
			RcadWin.ResetAll();
		}
		//3D rotation...//
		if(RcadWindow3DMode())
		{
			if(ThreeDRotationMode() && !RcadWin.PanMode && !ControlOn)
			{
				if(MouseClickAndDragMode())
				{
					RcadWin.RotateGraphics(Rcad_Graphics);
				}
			}
		}

		if(ChangeRcadViewOrien)
		{
			ChangeRcadViewOrien = false;
			Rcad_Graphics->RotateGrWindow(&RcadTransformationMatrix[0]);
		}
		//Save the Rcad window image...//
		if(CaptureCad) 
		{
			screenshot = (BYTE*)malloc(sizeof(BYTE) * RcadWin.getWinDim().x * RcadWin.getWinDim().y * 4);
			Rcad_Graphics->readPixels(screenshot);
			//UCS cUCS = MAINDllOBJECT->getCurrentUCS();
			//screenshot = (BYTE*)malloc(sizeof(BYTE) * cUCS.getWindow(0).getWinDim().x * cUCS.getWindow(0).getWinDim().y * 4);//    .getWinDim().x * RcadWin.getWinDim().y * 4);
			//Video_Graphics->readPixels(screenshot);
			CaptureCad = false;
		}
		//Render the graphics...//
		Rcad_Graphics->render();
		RCADMouseFlag = false;
		UpdteCadgp_UcsChange = false;
		RcadGraphicsUpdating = false;
		
	}
	
	catch(...)
	{
		RCADMouseFlag = false; CaptureCad = false; RcadGraphicsUpdating = false;
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1059", __FILE__, __FUNCSIG__); 
	}
}

//Render Dxf Window graphics
void RCadApp::Draw_DxfWindowGraphics()
{
	try
	{
		DxfGraphicsUpdating = true;
		RWindow& DxfWin = getWindow(2);
		double DxfUpp = DxfWin.getUppX();
		//Select the dxf window..//
		Dxf_Graphics->SelectWindow();
		//update the viewport...//
		if(DxfWin.WindowPanned())
		{
			double ext[4];
			DxfWin.getExtents(ext);
			Dxf_Graphics->setView(ext[0], ext[1], ext[2], ext[3], DxfUpp, DxfWin.getCenter().x, DxfWin.getCenter().y);
			GRAFIX->SetWindowSettings(DxfWin.getUppX(), DxfWin.getCenter().x, DxfWin.getCenter().y, 2);
			DxfWin.WindowPanned(false);
			if(DxfWin.WindowResized())
			{
				Dxf_Graphics->resize((int)DxfWin.getWinDim().x, (int)DxfWin.getWinDim().y);
				DxfWin.WindowResized(false);
			}			
		}
		if(RDXFMeasure_updated || RDXFShape_updated)
		{
			WaitForSingleObject(DxfMutex, INFINITE);
			try
			{
				list<Shape*> &TempShapeColl = DXFEXPOSEOBJECT->CurrentDXFShapeList;
				list<DimBase*> &TempDimColl = DXFEXPOSEOBJECT->CurrentDXFMeasureList;
				buildDxfDXFList(TempShapeColl, TempDimColl, DxfUpp);
				ReleaseMutex(DxfMutex);
			}
			catch(...)
			{
				ReleaseMutex(DxfMutex);
				throw "dxfdrawexcp";
			}
			RDXFMeasure_updated = false;
			RDXFShape_updated = false;
		}
		if(GridmoduleUpdated)
		{
			buildGridList();
			GRAFIX->CalldedrawGlList(8);
			/*GridmoduleUpdated = false;*/
		}
		/*if(PartprogramShape_Updated)
		{
			PPCALCOBJECT->buildPPlist();
			PartprogramShape_Updated = false;
		}*/
		
		Dxf_Graphics->translateMatrix(0.0, 0.0, -400);
		GRAFIX->drawFixedAxisBackground(getCurrentUCS().getWindow(0).getViewDim().x, getCurrentUCS().getWindow(0).getViewDim().y, getCurrentUCS().GetCurrentUCS_DROpostn().getX(), getCurrentUCS().GetCurrentUCS_DROpostn().getY());
		Dxf_Graphics->translateMatrix(0.0, 0.0, 400);
		//Dxf_Graphics->translateMatrix(0.0, 0.0, -DxfUpp);
		//render the graphics list...//
		GRAFIX->CalldedrawGlList(6);
		
		
		//Program path and List..
		//GRAFIX->CalldedrawGlList(7);
		
		//Dxf_Graphics->translateMatrix(0.0, 0.0, DxfUpp);
		
		//draw parptorgram arrow and path
		//PPCALCOBJECT->drawPartprogramArrow(DxfUpp);
		
		//Draw the axis..//		
		//Dxf_Graphics->translateMatrix(0.0, 0.0, DxfUpp);
		//GRAFIX->drawFixedAxis(-DxfWin.getpt().x, -DxfWin.getpt().y, DxfUpp);
		GRAFIX->drawFixedAxis(getCurrentUCS().GetCurrentUCS_DROpostn().getX(), getCurrentUCS().GetCurrentUCS_DROpostn().getY(), DxfUpp); // -DxfWin.getpt().x, -DxfWin.getpt().y, DxfUpp);
		
		//Dxf_Graphics->translateMatrix(0.0, 0.0, -DxfUpp);

		GRAFIX->SetColor_Double(1, 0, 0);
		//Dxf_Graphics->translateMatrix(0.0, 0.0, DxfUpp);
		if(CURRENTRAPIDHANDLER == RapidEnums::RAPIDDRAWHANDLERTYPE::RAPIDFRAMEGRAB_HANDLER || CURRENTRAPIDHANDLER == RapidEnums::RAPIDDRAWHANDLERTYPE::DXFZOOMINOUT_HANDLER || CURRENTRAPIDHANDLER == RapidEnums::RAPIDDRAWHANDLERTYPE::CADALIGNMENT_HANDLER)
			currentHandler->draw(2, DxfUpp);
		//Dxf_Graphics->translateMatrix(0.0, 0.0, -DxfUpp);	
		GRAFIX->SetColor_Double(1, 0, 0);
		
		//Draw the dxf snap point..//
		if(DXFSnapPointHighlighted())
		{
			GRAFIX->SetColor_Double(0, 0, 1);
			drawhlrect(DXFlastsnap->getX(), DXFlastsnap->getY(), DxfUpp * RECTWIDTH);
		}
		//Highlight the dxf shape...///
		/*if(DXFShapeHighlighted())
		{
			GRAFIX->SetColor_Double(100.0/255.0, 147.0/255.0, 237.0/255.0);
			Dxf_Graphics->translateMatrix(0.0, 0.0, DxfUpp);	
			DXFnearestShape->draw(2, Dxfupp);
			Dxf_Graphics->translateMatrix(0.0, 0.0, -DxfUpp);	
			GRAFIX->SetColor_Double(1, 1, 1);
		}*/
		/*if(GridCellHighlighted() && !GridCellShape->selected())
		{
			GRAFIX->SetColor_Double(100.0/255.0, 147.0/255.0, 237.0/255.0);
			GRAFIX->drawRectangle(*GridCellShape->GetLeftTop(),*(GridCellShape->GetLeftTop() + 1), *GridCellShape->GetBottomRight(), *(GridCellShape->GetBottomRight() + 1));
			GRAFIX->SetColor_Double(1, 1, 1);
		}*/
		//render the graphics...//
		Dxf_Graphics->render();
		DXFMouseFlag = false;
		DxfGraphicsUpdating = false;
	}
	catch(...)
	{
		DXFMouseFlag = false; DxfGraphicsUpdating = false;
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1060", __FILE__, __FUNCSIG__); 
	}
}

void RCadApp::ShowHideCoordinatePlanes()
{
	try
	{
		if(ShowCoordinatePlaneFlag())
		{
			MAINDllOBJECT->getCurrentUCS().XY_CoordinatePlane->IsValid(true);
			MAINDllOBJECT->getCurrentUCS().YZ_CoordinatePlane->IsValid(true);
			MAINDllOBJECT->getCurrentUCS().ZX_CoordinatePlane->IsValid(true);
		}
		else
		{
			MAINDllOBJECT->getCurrentUCS().XY_CoordinatePlane->IsValid(false);
			MAINDllOBJECT->getCurrentUCS().YZ_CoordinatePlane->IsValid(false);
			MAINDllOBJECT->getCurrentUCS().ZX_CoordinatePlane->IsValid(false);
		}
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1061", __FILE__, __FUNCSIG__); 
	}
}

//Updating the center of rotation for Rcad 3D rotation
void RCadApp::UpdateCenterOfRotation()
{
	try
	{
		double AllShpCenter[3] = {0};
		int cnt = 0;
		RWindow& RcadWin = getWindow(1);
		for(RC_ITER bi = getShapesList().getList().begin(); bi != getShapesList().getList().end(); bi++)
		{
			Shape* CShape = (Shape*)(*bi).second;
			if(CShape->Normalshape())
			{
				cnt++;
				double TmpShpCenter[3] = {0};
				CShape->GetShapeCenter(TmpShpCenter);
				AllShpCenter[0] += TmpShpCenter[0];
				AllShpCenter[1] += TmpShpCenter[1];
				AllShpCenter[2] += TmpShpCenter[2];
			}
		}
		if(cnt == 0) cnt = 1; //To avoid Divide by zero error
		RcadWin.SetCenterOfRotation(AllShpCenter[0] / cnt, AllShpCenter[1] / cnt, AllShpCenter[2] / cnt);
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1062", __FILE__, __FUNCSIG__); 
	}
}

void RCadApp::SetRcadTransformationMatrix()
{
	MAINDllOBJECT->getWindow(1).SetTransFormationMatrix(RcadTransformationMatrix);
	ChangeRcadViewOrien = true;
	Wait_RcadGraphicsUpdate();
	Wait_VideoGraphicsUpdate();
	update_RcadGraphics();
	Sleep(10);
	Wait_RcadGraphicsUpdate();
	UpdateShapesChanged();
	Sleep(40);
	Wait_RcadGraphicsUpdate();
	Wait_VideoGraphicsUpdate();
	Sleep(20);
	Wait_RcadGraphicsUpdate();
}
//Redraw all shapes on Video...//
void RCadApp::buildVideoShapeList(RCollectionBase& ShapeCollection, double wupp)
{
	try
	{
		GRAFIX->DeleteGlList(1);
		GRAFIX->CreateNewGlList(1);		
		for(RC_ITER bi = ShapeCollection.getList().begin(); bi != ShapeCollection.getList().end(); bi++)
		{
			Shape* CShape = (Shape*)(*bi).second;
			if(this->HideShapeMeasurementDuringPP && PPCALCOBJECT->IsPartProgramRunning() && !CShape->ShapeAsfasttrace())
				continue;
			if(!CShape->Normalshape()) //&& !CShape->IsUsedForReferenceDot())
			{
				if(ShowHideCrossHair())
				{
					if(CShape->HighlightedForMeasurement())
					{
						GRAFIX->SetColor_Double(0, 1, 0);
						CShape->drawShape(0, wupp);
					}
					else if(CShape->HighlightedFormouse())
					{
						GRAFIX->SetColor_Double(SHAPE_HIGHLIGHT_COLOR);
						CShape->drawShape(0, wupp);
					}	
				}
				continue;
			}
			if(CShape->ShapeAs3DShape() && CShape->ShapeType != RapidEnums::SHAPETYPE::CLOUDPOINTS && CShape->ShapeType != RapidEnums::SHAPETYPE::SPLINE)continue;
			if(MAINDllOBJECT->TriangulateCpSurface() && CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS) continue;
			if(CShape->selected()) continue;
			if(CShape->ShapeColor.r == 1 && CShape->ShapeColor.g == 1 && CShape->ShapeColor.b == 1)
				GRAFIX->SetColor_Double(1, 0, 0);
			else
				GRAFIX->SetColor_Double(CShape->ShapeColor.r, CShape->ShapeColor.g, CShape->ShapeColor.b);
			CShape->ShowFGpointsflag(false);
			if(CShape->HighlightedForMeasurement())
				GRAFIX->SetColor_Double(0, 1, 0);
			else if(CShape->HighlightedForPCDHighLight())
				GRAFIX->SetColor_Double(0, 1, 0.5);
			else if(CShape->HighlightedFormouse())
				GRAFIX->SetColor_Double(SHAPE_HIGHLIGHT_COLOR);
			else if(CShape->ShapeAsReferenceShape())
				GRAFIX->SetColor_Double(0, 1, 1);
			else if(CShape->ShapeUsedForTwoStepHoming())
				GRAFIX->SetColor_Double(1, 1, 0);
			else if(CShape->ShapeAsRotaryEntity())
				GRAFIX->SetColor_Double(1, 0.5, 0);
			CShape->drawShape(0, wupp);
		}
		/*if(PointerToShapeCollection != NULL)
		{
			RCollectionBase& ShColl = PointerToShapeCollection->ShapeCollection_Actions;
			if(ShColl.getList().size() > 0)
			{
				for(RC_ITER bi = ShColl.getList().begin(); bi != ShColl.getList().end(); bi++)
				{
					Shape* CShape = (Shape*)(*bi).second;
					if(CShape->selected())
						GRAFIX->SetColor_Double(1, 0, 1);
					else
						GRAFIX->SetColor_Double(1, 0, 0);
					CShape->drawShape(0, wupp);
				}
			}
		}*/
		GRAFIX->EndNewGlList();
	}
	catch(...)
	{ 
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1063", __FILE__, __FUNCSIG__);
	}
}

//Redraw all shapes on Rcad...//
void RCadApp::buildRcadShapeList(RCollectionBase& ShapeCollection, double wupp)
{
	try
	{
		GRAFIX->DeleteGlList(2);
		GRAFIX->CreateNewGlList(2);
		Xaxis_Highlighted = false; Yaxis_Highlighted = false;
		for(RC_ITER bi = ShapeCollection.getList().begin(); bi != ShapeCollection.getList().end(); bi++)
		{
			Shape* CShape = (Shape*)(*bi).second;
			CShape->drawGDntRefernce();
			if(!CShape->Normalshape())
			{
				if(CShape->HighlightedForMeasurement())
				{
					GRAFIX->SetColor_Double(0, 1, 0);
					CShape->drawShape(1, wupp);
					SetAxisHighLighted(CShape);
				}
				else if(CShape->HighlightedFormouse())
				{
					GRAFIX->SetColor_Double(SHAPE_HIGHLIGHT_COLOR);
					CShape->drawShape(1, wupp);
					SetAxisHighLighted(CShape);
				}	
				else if(CShape->ShapeType == RapidEnums::SHAPETYPE::PLANE) //|| CShape->IsUsedForReferenceDot())
				{
					GRAFIX->SetColor_Double_Alpha(CShape->ShapeColor.r, CShape->ShapeColor.g, CShape->ShapeColor.b, Alpha_Value);
					CShape->drawShape(1, wupp);
					SetAxisHighLighted(CShape);
				}
				continue;
			}
			if(CShape->selected()) continue;
			CShape->ShowFGpointsflag(false);
			if(CShape->UseLightingProperties())
			{
				if(this->HideShapeMeasurementDuringPP && PPCALCOBJECT->IsPartProgramRunning())
				{
					GRAFIX->SetColor_Double_Alpha(0.5, 0.5, 0.5, Alpha_Value);
				}
				else
				{
					GRAFIX->SetColor_Double_Alpha(CShape->ShapeColor.r, CShape->ShapeColor.g, CShape->ShapeColor.b, Alpha_Value);
				}
				if(CShape->HighlightedForMeasurement())
					GRAFIX->SetColor_Double_Alpha(0, 1, 0, Alpha_Value);
				else if(CShape->HighlightedFormouse())
					GRAFIX->SetColor_Double_Alpha(SHAPE_HIGHLIGHT_COLOR, Alpha_Value);
				else if(CShape->ShapeAsReferenceShape())
					GRAFIX->SetColor_Double_Alpha(0, 1, 1, Alpha_Value);
				else if(CShape->ShapeUsedForTwoStepHoming())
					GRAFIX->SetColor_Double_Alpha(1, 1, 0, Alpha_Value);
				else if(CShape->ShapeAsRotaryEntity())
					GRAFIX->SetColor_Double_Alpha(1, 1, 0, Alpha_Value);
					
				if(CShape->ShapeType != RapidEnums::SHAPETYPE::CLOUDPOINTS && CShape->ShapeType != RapidEnums::SHAPETYPE::PLANE && CShape->ShapeType != RapidEnums::SHAPETYPE::TRIANGULARSURFACE)
				{
					GRAFIX->EnableLighting();
					CShape->drawShape(1, wupp);
					GRAFIX->DisableLighting();
				}
				else
					CShape->drawShape(1, wupp);
			}
			else
			{
				if(this->HideShapeMeasurementDuringPP && PPCALCOBJECT->IsPartProgramRunning())
				{
					GRAFIX->SetColor_Double(0.5, 0.5, 0.5);
				}
				else
				{
					GRAFIX->SetColor_Double(CShape->ShapeColor.r, CShape->ShapeColor.g, CShape->ShapeColor.b);
				}
				if(CShape->HighlightedForMeasurement())
					GRAFIX->SetColor_Double(0, 1, 0);
				else if(CShape->HighlightedForPCDHighLight())
					GRAFIX->SetColor_Double(0, 1, 0.5);
				else if(CShape->HighlightedFormouse())
					GRAFIX->SetColor_Double(SHAPE_HIGHLIGHT_COLOR);
				else if(CShape->ShapeAsReferenceShape())
					GRAFIX->SetColor_Double(0, 1, 1);
				else if(CShape->ShapeUsedForTwoStepHoming())
					GRAFIX->SetColor_Double(1, 1, 0);
				CShape->drawShape(1, wupp);
			}
		}
		/*if(PointerToShapeCollection != NULL)
		{
			RCollectionBase& ShColl = PointerToShapeCollection->ShapeCollection_Actions;
			if(ShColl.getList().size() > 0)
			{
				for(RC_ITER bi = ShColl.getList().begin(); bi != ShColl.getList().end(); bi++)
				{
					Shape* CShape = (Shape*)(*bi).second;
					if(CShape->selected())
						GRAFIX->SetColor_Double(1, 0, 1);
					else
						GRAFIX->SetColor_Double(1, 0, 0);
					CShape->drawShape(0, wupp);
				}
			}
		}*/
		GRAFIX->EndNewGlList();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1064", __FILE__, __FUNCSIG__); }
}

void RCadApp::Set_IsDelphiCam(bool flag)		//vinod  if delphicam process is on going never draw a cross mark when probe take a point,it looks clumsy for more measurements like 1 degree so set a flag for this					
{
	IsDelphiCam=flag;
}

//Draw all selected shapes on Video
void RCadApp::buildVideoSelectedShapeList(RCollectionBase& ShapeCollection, double wupp)
{
	try
	{
		GRAFIX->DeleteGlList(9);
		GRAFIX->CreateNewGlList(9);
		for(RC_ITER bi = ShapeCollection.getList().begin(); bi != ShapeCollection.getList().end(); bi++)
		{
			Shape* CShape = (Shape*)(*bi).second;
			if(CShape->ShapeAs3DShape() && CShape->ShapeType != RapidEnums::SHAPETYPE::CLOUDPOINTS && CShape->ShapeType != RapidEnums::SHAPETYPE::SPLINE)continue;
			if(MAINDllOBJECT->TriangulateCpSurface() && CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS) continue;
			CShape->ShowFGpointsflag(ShowHideFGPoints());
			GRAFIX->SetColor_Double(1, 0, 1);
			CShape->drawShape(0, wupp);
			if(!ShowHideFGPoints() && !HELPEROBJECT->RotationScanFlag)
			{
				GRAFIX->SetColor_Double(1, 0, 0);
				if(!IsDelphiCam)										//vinod  if delphicam process is on going never draw a cross mark when probe take a point,it looks clumsy for more measurements like 1 degree
				CShape->drawSelectedPointCrossMark(0, wupp);
			}
		}
		GRAFIX->EndNewGlList();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1065", __FILE__, __FUNCSIG__); }
}

//Draw all selected shapes on Rcad
void RCadApp::buildRcadSelectedShapeList(RCollectionBase& ShapeCollection, double wupp)
{
	try
	{
		GRAFIX->DeleteGlList(10);
		GRAFIX->CreateNewGlList(10);
		for(RC_ITER bi = ShapeCollection.getList().begin(); bi != ShapeCollection.getList().end(); bi++)
		{
			Shape* CShape = (Shape*)(*bi).second;
			CShape->ShowFGpointsflag(ShowHideFGPoints());
			if(CShape->UseLightingProperties())
			{
				if(CShape->HighlightedForMeasurement())
					GRAFIX->SetColor_Double_Alpha(0, 1, 0, Alpha_Value);
				else if(CShape->HighlightedFormouse())
					GRAFIX->SetColor_Double_Alpha(SHAPE_HIGHLIGHT_COLOR, Alpha_Value);
				else
					GRAFIX->SetColor_Double_Alpha(1, 0, 1, Alpha_Value);
				if(CShape->ShapeType != RapidEnums::SHAPETYPE::CLOUDPOINTS && CShape->ShapeType != RapidEnums::SHAPETYPE::PLANE && CShape->ShapeType != RapidEnums::SHAPETYPE::TRIANGULARSURFACE)
				{
					GRAFIX->EnableLighting();
					CShape->drawShape(1, wupp);
					GRAFIX->DisableLighting();
				}
				else
					CShape->drawShape(1, wupp);
				if(!ShowHideFGPoints())
				{
					GRAFIX->SetColor_Double(1, 0, 0);
					CShape->drawSelectedPointCrossMark(1, wupp);
				}
			}
			else
			{
				GRAFIX->SetColor_Double(1, 0, 1);						//pink colour code 
				CShape->drawShape(1, wupp);
				if(!ShowHideFGPoints())
				{
					GRAFIX->SetColor_Double(1, 0, 0);			// for a red cross mark 
					if(!IsDelphiCam)										//vinod   if delphicam process is on going never draw a cross mark when probe take a point,it looks clumsy for more measurements like 1 degree
					CShape->drawSelectedPointCrossMark(1, wupp);
				}
			}
		}
		GRAFIX->EndNewGlList();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1066", __FILE__, __FUNCSIG__); }
}

//Redraw the measurements on video.//
void RCadApp::buildVideoMeasurementList(RCollectionBase& DimCollection, double wupp)
{
	try
	{
		GRAFIX->DeleteGlList(3);
		GRAFIX->CreateNewGlList(3);
		if(this->HideShapeMeasurementDuringPP && PPCALCOBJECT->IsPartProgramRunning())
		{
			GRAFIX->EndNewGlList();
			return;
		}
		for(RC_ITER bi = DimCollection.getList().begin(); bi != DimCollection.getList().end(); bi++)
		{
			DimBase* dim = (DimBase*)(*bi).second;
			if(dim->selected())continue;
			if(!dim->IsValid())continue;
			if(dim->ChildMeasurementType())continue;
			if(dim->MeasureAs3DMeasurement())continue;
			if(dim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_THREAD || dim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH) continue;
			if(dim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE)
			{
				DimPointCoordinate* CdimPt = (DimPointCoordinate*)dim;
				if(CdimPt->DimPointX->selected() || CdimPt->DimPointY->selected() || CdimPt->DimPointZ->selected())continue;
			}
			RMATH2DOBJECT->MeasurementDirection = dim->MeasurementDirection();
			//if(dim->HighlightedForMouseMove())
				//GRAFIX->SetColor_Double(0, 1, 1);
			//else
			GRAFIX->SetMeasurement_Color(dim->MeasureColor.r, dim->MeasureColor.g, dim->MeasureColor.b);
			dim->drawMeasurement(0, wupp);
		}
		GRAFIX->EndNewGlList();

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1067", __FILE__, __FUNCSIG__); }
}

//Redraw the measurements on Rcad.//
void RCadApp::buildRcadMeasurementList(RCollectionBase& DimCollection, double wupp)
{
	try
	{
		GRAFIX->DeleteGlList(4);
		GRAFIX->CreateNewGlList(4);
		if(this->HideShapeMeasurementDuringPP && PPCALCOBJECT->IsPartProgramRunning())
		{
			GRAFIX->EndNewGlList();
			return;
		}
		for(RC_ITER bi = DimCollection.getList().begin(); bi != DimCollection.getList().end(); bi++)
		{
			DimBase* dim = (DimBase*)(*bi).second;
			if(dim->selected())continue;
			if(!dim->IsValid())continue;
			if(dim->ChildMeasurementType())continue;
			if(dim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_THREAD || dim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH) continue;
			if(dim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE)
			{
				DimPointCoordinate* CdimPt = (DimPointCoordinate*)dim;
				if(CdimPt->DimPointX->selected() || CdimPt->DimPointY->selected() || CdimPt->DimPointZ->selected())continue;
			}
			RMATH2DOBJECT->MeasurementDirection = dim->MeasurementDirection();
			//if(dim->HighlightedForMouseMove())
				//GRAFIX->SetColor_Double(0, 1, 1);
			//else
			GRAFIX->SetMeasurement_Color(dim->MeasureColor.r, dim->MeasureColor.g, dim->MeasureColor.b);
			dim->drawMeasurement(1, wupp);
		}
		if(PointerToShapeCollection != NULL)
		{
			RCollectionBase& MeasureColl = PointerToShapeCollection->ThrdMeasureCollection;
			if(MeasureColl.getList().size() > 0)
			{
				for(RC_ITER bi = MeasureColl.getList().begin(); bi != MeasureColl.getList().end(); bi++)
				{
					DimBase* dim = (DimBase*)(*bi).second;
					GRAFIX->SetMeasurement_Color(0, 1, 0);
					dim->drawMeasurement(1, wupp);
				}
			}
		}
		GRAFIX->EndNewGlList();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1068", __FILE__, __FUNCSIG__); }
}

//Redraw selected measurements on video..
void RCadApp::buildVideoSelectedMeasurementList(RCollectionBase& DimCollection, double wupp)
{
	try
	{
		//GRAFIX->translateMatrix(0.0, 0.0, 2.0);
		GRAFIX->DeleteGlList(11);
		GRAFIX->CreateNewGlList(11);
		for(RC_ITER bi = DimCollection.getList().begin(); bi != DimCollection.getList().end(); bi++)
		{
			DimBase* dim = (DimBase*)(*bi).second;
			if(dim->MeasureAs3DMeasurement())continue;
			if(!dim->IsValid())continue;
			if(dim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_THREAD || dim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH) continue;
			GRAFIX->SetMeasurement_Color(1, 0.5, 0.8);
			RMATH2DOBJECT->MeasurementDirection = dim->MeasurementDirection();
			if(dim->ChildMeasurementType() && dim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE)
			{
				DimChildMeasurement* CdimPt = (DimChildMeasurement*)dim;
				if((DimBase*)getDimList().ItemExists(CdimPt->ParentMeasureId()))
				{
					DimBase* Cdim = (DimBase*)getDimList().getList()[CdimPt->ParentMeasureId()];
					Cdim->drawMeasurement(1, wupp);
				}
			}
			else
				dim->drawMeasurement(0, wupp);
		}
		GRAFIX->EndNewGlList();
		//GRAFIX->translateMatrix(0.0, 0.0, -2.0);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1069", __FILE__, __FUNCSIG__); }
}

//Redraw selected measurements on Rcad..
void RCadApp::buildRcadSelectedMeasurementList(RCollectionBase& DimCollection, double wupp)
{
	try
	{
		//GRAFIX->translateMatrix(0.0, 0.0, 2.0);
		GRAFIX->DeleteGlList(12);
		GRAFIX->CreateNewGlList(12);
		for(RC_ITER bi = DimCollection.getList().begin(); bi != DimCollection.getList().end(); bi++)
		{
			DimBase* dim = (DimBase*)(*bi).second;
			if(!dim->IsValid())continue;
			if(dim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_THREAD || dim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH) continue;
			GRAFIX->SetMeasurement_Color(1, 0.5, 0.8);
			RMATH2DOBJECT->MeasurementDirection = dim->MeasurementDirection();
			if(dim->ChildMeasurementType() && dim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE)
			{
				DimChildMeasurement* CdimPt = (DimChildMeasurement*)dim;
				if((DimBase*)getDimList().ItemExists(CdimPt->ParentMeasureId()))
				{
					DimBase* Cdim = (DimBase*)getDimList().getList()[CdimPt->ParentMeasureId()];
					Cdim->drawMeasurement(1, wupp);
				}
			}
			else
				dim->drawMeasurement(1, wupp);
		}
		GRAFIX->EndNewGlList();
		//GRAFIX->translateMatrix(0.0, 0.0, -2.0);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1070", __FILE__, __FUNCSIG__); }
}

//Redraw all shapes in DXF shapeslist on video...//
void RCadApp::buildVideoDXFList(list<Shape*> &ShapeCollection, list<DimBase*> &DimCollection, double wupp)
{
	try
	{
		GRAFIX->DeleteGlList(5);
		GRAFIX->CreateNewGlList(5);
		if(ShapeCollection.size() > 0)
		{
			for each(Shape* Cshape in ShapeCollection)
			{
				if(Cshape->ShapeAs3DShape() && Cshape->ShapeType != RapidEnums::SHAPETYPE::SPLINE)continue;
				if(Cshape->HighlightedFormouse())
					GRAFIX->SetColor_Double(100.0/255.0, 147.0/255.0, 237.0/255.0);
				else
				{
					if(Cshape->ShapeColor.r == 1 && Cshape->ShapeColor.g == 1 && Cshape->ShapeColor.b == 1)
						GRAFIX->SetColor_Double(1, 0, 0);
					else
						GRAFIX->SetColor_Double(Cshape->ShapeColor.r, Cshape->ShapeColor.g, Cshape->ShapeColor.b);
				}
				Cshape->drawShape(0, wupp);
			}
		}
		/*if(DimCollection.size() > 0)
		{
			for each(DimBase* Cdim in DimCollection)
			{
				GRAFIX->SetMeasurement_Color(0, 1, 0);
				Cdim->draw(0, wupp);
			}
		}*/
		GRAFIX->EndNewGlList();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1071", __FILE__, __FUNCSIG__); }
}

//Redraw all shapes in DXF shapeslist on dxf...//
void RCadApp::buildDxfDXFList(list<Shape*> &ShapeCollection, list<DimBase*> &DimCollection, double wupp)
{
	try
	{
		GRAFIX->DeleteGlList(6);
		GRAFIX->CreateNewGlList(6);
		if(ShapeCollection.size() > 0)
		{
			for each(Shape* Cshape in ShapeCollection)
			{
				if(Cshape->HighlightedForMeasurement())
					GRAFIX->SetColor_Double(0, 1, 0);
				else if(Cshape->HighlightedFormouse())
					GRAFIX->SetColor_Double(100.0/255.0, 147.0/255.0, 237.0/255.0);
				else
					GRAFIX->SetColor_Double(Cshape->ShapeColor.r, Cshape->ShapeColor.g, Cshape->ShapeColor.b);
				Cshape->drawShape(2, wupp);
			}
		}
		if(DimCollection.size() > 0)
		{
			for each(DimBase* Cdim in DimCollection)
			{
				GRAFIX->SetMeasurement_Color(0, 1, 0);
				Cdim->drawMeasurement(2, wupp);
			}
		}
		GRAFIX->EndNewGlList();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1072", __FILE__, __FUNCSIG__); }
}

//draw snap point / name on video
void RCadApp::buildVideoSnapPointNameList(double wupp)
{
	try
	{
		GRAFIX->DeleteGlList(14);
		GRAFIX->CreateNewGlList(14);
		//draw the snap point//
		if(SnapPointHighlighted())
		{
			GRAFIX->SetColor_Double(0, 0, 1);
			drawhlrect(lastsnap->getX(), lastsnap->getY(), wupp * RECTWIDTH);
			if(ShowMeasurementOnVideo() && !ShapePasted)
				drawSnapPoint(true);
		}
		else if(ShapeHighlighted() && !ShapeHighlightedForImplicitSnap())
		{
			if(!nearestShape->ShapeAs3DShape())
			{
				if(nearestShape->Normalshape())
				{
					if(ShowNameTagFlag())
					{
						GRAFIX->SetColor_Double(1, 1, 1);
						GRAFIX->drawStringEdited3D(getWindow(currentwindow).getPointer().x + 30 * wupp, getWindow(currentwindow).getPointer().y - 20 * wupp, 0, 0, (char*)CurrentShapeName.c_str(), 20 * wupp);
					}
				}
				else
				{
					if(ShowHideCrossHair())
					{
						GRAFIX->SetColor_Double(1, 1, 1);
						GRAFIX->drawStringEdited3D(getWindow(currentwindow).getPointer().x + 30 * wupp, getWindow(currentwindow).getPointer().y - 20 * wupp, 0, 0, (char*)CurrentShapeName.c_str(), 20 * wupp);
					}
				}
				GRAFIX->SetColor_Double(0, 1, 0);
				drawSnapPoint_FT();
			}
		}
		//Highlight the current shape snap...//
		if(ShowExplicitIntersectionPoints() && DrawSnapPointOnHighlightedShape())
		{
			GRAFIX->SetColor_Double(1, 0, 1);
			drawhlrect(SnapPointModePoint.getX(), SnapPointModePoint.getY(), wupp * RECTWIDTH, false);
		}
		GRAFIX->EndNewGlList();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1073", __FILE__, __FUNCSIG__); }
}

//draw snap point / shape name on rcad
void RCadApp::buildRcadSnapPointNameList(double wupp)
{
	try
	{
		GRAFIX->DeleteGlList(15);
		GRAFIX->CreateNewGlList(15);
		//Draw the snap point ..(rectangle..)
		if(StlHighlightedPointFlag)
		{
			GRAFIX->SetColor_Double(0, 0, 1);
			drawhlrect(StlHighlightedPt.getX(), StlHighlightedPt.getY(), wupp * RECTWIDTH, true, StlHighlightedPt.getZ());
		}
		if (SnapPointHighlighted())
		{
			GRAFIX->SetColor_Double(0, 0, 1);
			drawhlrect(lastsnap->getX(), lastsnap->getY(), wupp * RECTWIDTH, true, lastsnap->getZ());
			drawSnapPoint(false);
		}
		//Highlight the current SnapPointHighlighted shape...//
		else if(ShapeHighlighted() && !ShapeHighlightedForImplicitSnap())
		{
			//Draw the shape name..//
			GRAFIX->SetColor_Double(1, 1, 1);
			if(ShowNameTagFlag())
			{		
				double zValue = 0;
				if(nearestShape->ShapeAs3DShape())
				{
					zValue = getZCoordinateOfCurrentShape();
					GRAFIX->drawStringEdited3D(getWindow(currentwindow).getPointer(true).x + 30 * wupp, getWindow(currentwindow).getPointer(true).y, zValue, (char*)CurrentShapeName.c_str(), 20 * wupp);
				}
				else
					GRAFIX->drawStringEdited3D(getWindow(currentwindow).getPointer().x + 30 * wupp, getWindow(currentwindow).getPointer().y - 20 * wupp, zValue, 0, (char*)CurrentShapeName.c_str(), 20 * wupp);
			}				
		}
		GRAFIX->EndNewGlList();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1074", __FILE__, __FUNCSIG__); }
}

//get highest value of Z for current shape...
double RCadApp::getZCoordinateOfCurrentShape()
{
	try
	{
		double Zvalue = 0;
		switch(nearestShape->ShapeType)
		{
		case RapidEnums::SHAPETYPE::SPHERE:
			Zvalue = abs(((Sphere*)nearestShape)->getCenter()->getZ()) + ((Sphere*)nearestShape)->Radius() + 1;
			break;
		case RapidEnums::SHAPETYPE::CYLINDER:
			{
				if(abs(((Cylinder*)nearestShape)->getCenter1()->getZ()) + ((Cylinder*)nearestShape)->Radius1() > abs(((Cylinder*)nearestShape)->getCenter2()->getZ()) + ((Cylinder*)nearestShape)->Radius2())
					Zvalue = abs(((Cylinder*)nearestShape)->getCenter1()->getZ()) + ((Cylinder*)nearestShape)->Radius1() + 1;
				else
					Zvalue = abs(((Cylinder*)nearestShape)->getCenter2()->getZ()) + ((Cylinder*)nearestShape)->Radius2() + 1;
			}
			break;
		case RapidEnums::SHAPETYPE::CONE:
			{
				if(abs(((Cone*)nearestShape)->getCenter1()->getZ()) + ((Cone*)nearestShape)->Radius1() > abs(((Cone*)nearestShape)->getCenter2()->getZ()) + ((Cone*)nearestShape)->Radius2())
					Zvalue = abs(((Cone*)nearestShape)->getCenter1()->getZ()) + ((Cone*)nearestShape)->Radius1() + 1;
				else
					Zvalue = abs(((Cone*)nearestShape)->getCenter2()->getZ()) + ((Cone*)nearestShape)->Radius2() + 1;
			}
			break;
		}
		return Zvalue;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1075", __FILE__, __FUNCSIG__); }
}

//draw the snap rectangle...//
void RCadApp::drawhlrect(double x, double y, double size, bool trans, double z)
{
	try
	{
		if((MAINDllOBJECT->getCurrentUCS().UCSMode() == 4) && (MAINDllOBJECT->getCurrentWindow() == 0) && trans)
		{
			double TransformM[16] = {0}, temp1[4], temp2[4] = {x, y, 0, 1}; 
			int Order1[2] = {4, 4}, Order2[2] = {4, 1};
			RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 4, 1, TransformM);
			RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1, temp2, Order2, temp1);
			x = temp1[0]; y = temp1[1];
		}
		//glLineWidth(5);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA,GL_DST_ALPHA);
		if(!MAINDllOBJECT->RcadWindow3DMode())
		GRAFIX->translateMatrix(0, 0, 0.1);
		GRAFIX->drawPoint(x, y, z, 10);
		//GRAFIX->drawRectangle(x - size, y - size, x + size, y + size);
		if(!MAINDllOBJECT->RcadWindow3DMode())
		GRAFIX->translateMatrix(0, 0, -0.1);
		//glDisable(GL_BLEND);
		//GRAFIX->drawRectangle(x - size, y - size, x + size, y + size, true);
		//glLineWidth(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0012", __FILE__, __FUNCSIG__); }
}

//draw the snap point...//
void RCadApp::drawSnapPoint(bool flag)
{
	try
	{
		if(MousedragwithShape() || MouseClickAndDragMode())return;
		GRAFIX->SetColor_Double(1, 1, 1);
		GRAFIX->translateMatrix(0.0, 0.0, 2.0);
		std::string displaypoint;
		Vector TempPoint = *lastsnap;
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
		{
			TempPoint.set(TempPoint.getX() / 25.4, TempPoint.getY() / 25.4, TempPoint.getZ() / 25.4);
		}
		if(getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
			displaypoint = HighlightedPt2String(TempPoint.getX(), TempPoint.getY(), TempPoint.getZ());
		else if(getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
			displaypoint = HighlightedPt2String(TempPoint.getZ(), TempPoint.getX(), TempPoint.getY());
		else if(getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
			displaypoint = HighlightedPt2String(TempPoint.getX(), TempPoint.getZ(), TempPoint.getY());
		else if (getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
			displaypoint = HighlightedPt2String(-TempPoint.getZ(), TempPoint.getX(), TempPoint.getY());
		else if (getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
			displaypoint = HighlightedPt2String(TempPoint.getX(), -TempPoint.getZ(), TempPoint.getY());

		if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 4 && MAINDllOBJECT->getCurrentWindow() == 0)
		{
			double TransformM[16] = {0}, temp1[4], temp2[4] = {lastsnap->getX(), lastsnap->getY(), 0, 1}; 
			int Order1[2] = {4, 4}, Order2[2] = {4, 1};
			RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 4, 1, TransformM);
			RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1, temp2, Order2, temp1);
			if(flag)
				GRAFIX->drawStringEdited3D(temp1[0], temp1[1] + 10 * getWindow(0).getUppX(), 0, 0, (char*)displaypoint.c_str(), 20 * getWindow(0).getUppX());
			else
				GRAFIX->drawStringEdited3D(temp1[0], temp1[1] + 10 * getWindow(1).getUppX(), 0, 0, (char*)displaypoint.c_str(), 20 * getWindow(1).getUppX());
			GRAFIX->translateMatrix(0.0, 0.0, -2.0);
		}
		else
		{
			if(flag)
				GRAFIX->drawStringEdited3D(lastsnap->getX(), lastsnap->getY() + 10 * getWindow(0).getUppX(), 0, 0, (char*)displaypoint.c_str(), 20 * getWindow(0).getUppX());
			else
				GRAFIX->drawStringEdited3D(lastsnap->getX(), lastsnap->getY() + 10 * getWindow(1).getUppX(), 0, 0, (char*)displaypoint.c_str(), 20 * getWindow(1).getUppX());
			GRAFIX->translateMatrix(0.0, 0.0, -2.0);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1076", __FILE__, __FUNCSIG__); }
}

//snap Point Rect on highlighted shape..
bool RCadApp::GetPointOnHighlightedShape(double x, double y)
{
	try
	{
		bool flag = false;
		double angle, intercept, center[4], point[3], mp[2] = {x, y};
		Shape* CShape = highlightedShape();
		if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 4 && MAINDllOBJECT->getCurrentWindow() == 0 && CShape->Normalshape())
		{
			double TransformM[16] = {0};
			int Order1[2] = {4, 4}, Order2[2] = {4, 1};
			RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 4, 1, TransformM);
			switch(CShape->ShapeType)
			{
				case RapidEnums::SHAPETYPE::LINE:
				case RapidEnums::SHAPETYPE::XLINE:
				case RapidEnums::SHAPETYPE::XRAY:
					{
					flag = true;
					double tempSlope, tempIntercept, temp1[4] = {((Line*)CShape)->getPoint1()->getX(), ((Line*)CShape)->getPoint1()->getY(), ((Line*)CShape)->getPoint1()->getZ(), 1},  temp2[4], temp3[4] = {((Line*)CShape)->getPoint2()->getX(), ((Line*)CShape)->getPoint2()->getY(), ((Line*)CShape)->getPoint2()->getZ(), 1},  temp4[4]; 
					RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1, temp1, Order2, temp2);
					RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1, temp3, Order2, temp4);
					RMATH2DOBJECT->TwoPointLine(temp2, temp4, &tempSlope, &tempIntercept);
					RMATH2DOBJECT->Point_PerpenIntrsctn_Line(tempSlope, tempIntercept, &mp[0], &point[0]);
					break;
					}
				case RapidEnums::SHAPETYPE::CIRCLE:
				case RapidEnums::SHAPETYPE::ARC:
					{
					flag = true;
					double temp1[4] = {((Line*)CShape)->getPoint1()->getX(), ((Line*)CShape)->getPoint1()->getY(), ((Line*)CShape)->getPoint1()->getZ(), 1};
					((Circle*)CShape)->getCenter()->FillDoublePointer(&temp1[0]);
					RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1, temp1, Order2, center);
					RMATH2DOBJECT->Point_onCircle(&center[0], ((Circle*)CShape)->Radius(), &mp[0], &point[0]);
					break;
					}
				default:
					point[0] = mp[0]; point[1] = mp[1];
					break;
			}
		}
		else
		{
			switch(CShape->ShapeType)
			{
				case RapidEnums::SHAPETYPE::LINE:
				case RapidEnums::SHAPETYPE::XLINE:
				case RapidEnums::SHAPETYPE::XRAY:
					flag = true;
					angle = ((Line*)CShape)->Angle(); intercept = ((Line*)CShape)->Intercept();
					RMATH2DOBJECT->Point_PerpenIntrsctn_Line(angle, intercept, &mp[0], &point[0]);
					break;
				case RapidEnums::SHAPETYPE::CIRCLE:
				case RapidEnums::SHAPETYPE::ARC:
					flag = true;
					((Circle*)CShape)->getCenter()->FillDoublePointer(&center[0]);
					RMATH2DOBJECT->Point_onCircle(&center[0], ((Circle*)CShape)->Radius(), &mp[0], &point[0]);
					break;
				default:
					point[0] = mp[0]; point[1] = mp[1];
					break;
			}
		}
		point[2] =  getCurrentDRO().getZ();
		SnapPointModePoint.set(point[0], point[1], point[2]);
		return flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1077", __FILE__, __FUNCSIG__); return false; }
}

//draw snap points of Fastrace during FT draw..
void RCadApp::drawSnapPoint_FT()
{
	try
	{
		Shape* Cshape = highlightedShape();
		if(Cshape != NULL && Cshape->ShapeForCurrentFastrace())
		{
			if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 4 && MAINDllOBJECT->getCurrentWindow() == 0)
			{
				double TransformM[16] = {0};
				int Order1[2] = {4, 4}, Order2[2] = {4, 1};
				RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 4, 1, TransformM);
				switch(Cshape->ShapeType)
				{
				case RapidEnums::SHAPETYPE::LINE:
					{
					double tempend1[4], temp1[4]={((Line*)Cshape)->getPoint1()->getX(), ((Line*)Cshape)->getPoint1()->getY(), 0, 1}, tempend2[4], temp2[4] = {((Line*)Cshape)->getPoint2()->getX(), ((Line*)Cshape)->getPoint2()->getY(), 0, 1};
					RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1, temp1, Order2, tempend1);
					RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1, temp2, Order2, tempend2);
					GRAFIX->drawPoint(tempend1[0], tempend1[1], 0, 10);
					GRAFIX->drawPoint(tempend2[0], tempend2[0], 0, 10);
					break;
					}
				case RapidEnums::SHAPETYPE::ARC:
					{
					double tempend1[4], temp1[4]={((Circle*)Cshape)->getendpoint1()->getX(), ((Circle*)Cshape)->getendpoint1()->getY(), 0, 1}, tempend2[4], temp2[4] = {((Circle*)Cshape)->getendpoint2()->getX(), ((Circle*)Cshape)->getendpoint2()->getY(), 0, 1};
					double tempmid[4], temp3[4] = {((Circle*)Cshape)->getMidPoint()->getX(), ((Circle*)Cshape)->getMidPoint()->getY(), 0, 1};
					RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1, temp1, Order2, tempend1);
					RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1, temp2, Order2, tempend2);
					RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1, temp3, Order2, tempmid);
					GRAFIX->drawPoint(tempend1[0], tempend1[1], 0, 10);
					GRAFIX->drawPoint(tempend2[0], tempend2[0], 0, 10);
					GRAFIX->drawPoint(tempmid[0], tempmid[1], 0, 10);
					break;
					}
				}
			}
			else
			{
				switch(Cshape->ShapeType)
				{
				case RapidEnums::SHAPETYPE::LINE:
					GRAFIX->drawPoint(((Line*)Cshape)->getPoint1()->getX(), ((Line*)Cshape)->getPoint1()->getY(), 0, 10);
					GRAFIX->drawPoint(((Line*)Cshape)->getPoint2()->getX(), ((Line*)Cshape)->getPoint2()->getY(), 0, 10);
					break;
				case RapidEnums::SHAPETYPE::ARC:
					GRAFIX->drawPoint(((Circle*)Cshape)->getendpoint1()->getX(), ((Circle*)Cshape)->getendpoint1()->getY(), 0, 10);
					GRAFIX->drawPoint(((Circle*)Cshape)->getendpoint2()->getX(), ((Circle*)Cshape)->getendpoint2()->getY(), 0, 10);
					GRAFIX->drawPoint(((Circle*)Cshape)->getMidPoint()->getX(), ((Circle*)Cshape)->getMidPoint()->getY(), 0, 10);
					break;
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1078", __FILE__, __FUNCSIG__); }
}

//Functions to calculate the parameters for extension lines of shape intersection with the axes//
void RCadApp::DrawExtensionForImplicitSnap(Shape* s1, Shape* s2, Vector *v)
{
	try
	{
		Shape* temp;
		int count = 0;
		double p[2] = {v->getX(), v->getY()}, ans[6];
		////************************Extension line of the shape intersections with Axis**********************//
		if(s2 == NULL)
		{
			if(s1->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				double c1[2] = {((Line*)s1)->getPoint1()->getX(), ((Line*)s1)->getPoint1()->getY()}, c2[2] = {((Line*)s1)->getPoint2()->getX(), ((Line*)s1)->getPoint2()->getY()};
				count = RMATH2DOBJECT->Lineto_finiteline(&c1[0], &c2[0],&p[0],&ans[0]);
				GRAFIX->drawLineStipple(ans[0],ans[1],p[0],p[1]);
			}
			else if(s1->ShapeType == RapidEnums::SHAPETYPE::XRAY)
				GRAFIX->drawLineStipple(((Line*)s1)->getPoint1()->getX(),((Line*)s1)->getPoint1()->getY(),p[0],p[1]);
			else if(s1->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				double p1[2] = {((Circle*)s1)->getCenter()->getX(),((Circle*)s1)->getCenter()->getY()}, angles1[2] = {((Circle*)s1)->Startangle(),((Circle*)s1)->Sweepangle()};
				count = RMATH2DOBJECT->Infinitelineto_Arc(&p1[0],((Circle*)s1)->Radius(),&angles1[0],&p[0],&ans[0]);
				GRAFIX->drawArcStipple(((Circle*)s1)->getCenter()->getX(), ((Circle*)s1)->getCenter()->getY(), ((Circle*)s1)->Radius(), ans[0], ans[1]);	
			}
		}
		////************************Functions to find the extension line of the shape intersections**********************//
		////************************Extension for the same shape intersection points************************************//
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::LINE))
		{
			double p1[2] = {((Line*)s1)->getPoint1()->getX(), ((Line*)s1)->getPoint1()->getY()}, p2[2] = {((Line*)s1)->getPoint2()->getX(), ((Line*)s1)->getPoint2()->getY()};
			double p3[2] = {((Line*)s2)->getPoint1()->getX(), ((Line*)s2)->getPoint1()->getY()}, p4[2] = {((Line*)s2)->getPoint2()->getX(), ((Line*)s2)->getPoint2()->getY()};
			count = RMATH2DOBJECT->Finitelineto_Finiteline(&p1[0],&p2[0],&p3[0],&p4[0],&p[0],&ans[0]);
			if(count == 1)
				GRAFIX->drawLineStipple(ans[0],ans[1],p[0],p[1]);
			if(count == 2)
			{
				GRAFIX->drawLineStipple(ans[0],ans[1],p[0],p[1]);
				GRAFIX->drawLineStipple(ans[2],ans[3],p[0],p[1]);
			}	
		}
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::ARC, RapidEnums::SHAPETYPE::ARC))
		{
			double p1[2] = {((Circle*)s1)->getCenter()->getX(),((Circle*)s1)->getCenter()->getY()}, p2[2] = {((Circle*)s2)->getCenter()->getX(),((Circle*)s2)->getCenter()->getY()};
			double angles1[2] = {((Circle*)s1)->Startangle(),((Circle*)s1)->Sweepangle()}, angles2[2] = {((Circle*)s2)->Startangle(),((Circle*)s2)->Sweepangle()};
			count = RMATH2DOBJECT->Arcto_Arc(&p1[0],((Circle*)s1)->Radius(),&p2[0],((Circle*)s2)->Radius(),&angles1[0],&angles2[0],&p[0],&ans[0]);
			if(count == 1)
				GRAFIX->drawArcStipple(ans[0], ans[1], ans[2], ans[3], ans[4]);
			if(count == 2)
			{
				GRAFIX->drawArcStipple(((Circle*)s1)->getCenter()->getX(), ((Circle*)s1)->getCenter()->getY(), ((Circle*)s1)->Radius(), ans[0], ans[1]);
				GRAFIX->drawArcStipple(((Circle*)s2)->getCenter()->getX(), ((Circle*)s2)->getCenter()->getY(), ((Circle*)s2)->Radius(), ans[2], ans[3]);
			}
		}
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XRAY, RapidEnums::SHAPETYPE::XRAY))
		{
			double a1[2] = {((Line*)s1)->getPoint1()->getX(),((Line*)s1)->getPoint1()->getY()}, a2[2] = {((Line*)s2)->getPoint1()->getX(),((Line*)s2)->getPoint1()->getY()};
			count = RMATH2DOBJECT->rayto_ray(&a1[0], ((Line*)s1)->Angle(), &a2[0], ((Line*)s2)->Angle(),&p[0], &ans[0]);
			if(count == 1)
				GRAFIX->drawLineStipple(ans[0],ans[1],p[0],p[1]);
			if(count == 2)
			{
				GRAFIX->drawLineStipple(ans[0],ans[1],p[0],p[1]);
				GRAFIX->drawLineStipple(ans[2],ans[3],p[0],p[1]);
			}
		}
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::LINE))
		{
			if(s1->ShapeType == RapidEnums::SHAPETYPE::XLINE)
			{
				temp = s1; 
				s1 = s2; 
				s2 = temp;
			}
			double	c1[2] = {((Line*)s1)->getPoint1()->getX(),((Line*)s1)->getPoint1()->getY()}, c2[2] = {((Line*)s1)->getPoint2()->getX(), ((Line*)s1)->getPoint2()->getY()};
			count = RMATH2DOBJECT->Lineto_finiteline(&c1[0], &c2[0],&p[0],&ans[0]);
			GRAFIX->drawLineStipple(ans[0],ans[1],p[0],p[1]);
		}
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::XRAY))
		{
			if(s1->ShapeType == RapidEnums::SHAPETYPE::XLINE)
			{
				temp = s1;
				s1 = s2;
				s2 = temp;
			}
			GRAFIX->drawLineStipple(((Line*)s1)->getPoint1()->getX(),((Line*)s1)->getPoint1()->getY(),p[0],p[1]);
		}
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XLINE, RapidEnums::SHAPETYPE::ARC))
		{
			if(s1->ShapeType == RapidEnums::SHAPETYPE::XLINE)
			{
				temp = s1;
				s1 = s2;
				s2 = temp;
			}
			double p1[2] = {((Circle*)s1)->getCenter()->getX(),((Circle*)s1)->getCenter()->getY()}, angles1[2] = {((Circle*)s1)->Startangle(),((Circle*)s1)->Sweepangle()};
			count = RMATH2DOBJECT->Infinitelineto_Arc(&p1[0],((Circle*)s1)->Radius(),&angles1[0],&p[0],&ans[0]);
			GRAFIX->drawArcStipple(((Circle*)s1)->getCenter()->getX(), ((Circle*)s1)->getCenter()->getY(), ((Circle*)s1)->Radius(), ans[0], ans[1]);
		}
		////*********************************LINE WITH OTHER DIFFERENT SHAPES*************************************//
		////****************************************EXCEPT XLINE*************************************************//
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::LINE, RapidEnums::SHAPETYPE::XRAY))
		{
			if(s1->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				temp = s1;
				s1 = s2;
				s2 = temp;
			}
			double a1[2] = {((Line*)s2)->getPoint1()->getX(),((Line*)s2)->getPoint1()->getY()}, b1[2] = {((Line*)s2)->getPoint2()->getX(),((Line*)s2)->getPoint2()->getY()},
				a2[2] = {((Line*)s1)->getPoint1()->getX(),((Line*)s1)->getPoint1()->getY()};
			count = RMATH2DOBJECT->Finitelineto_ray(&a1[0], &b1[0], &a2[0],((Line*)s1)->Angle(), &p[0], &ans[0]);
			if(count == 1)
				GRAFIX->drawLineStipple(ans[0],ans[1],p[0],p[1]);
			if(count == 2)
			{
				GRAFIX->drawLineStipple(ans[0],ans[1],p[0],p[1]);
				GRAFIX->drawLineStipple(a2[0],a2[1],p[0],p[1]);
			}
		}
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CIRCLE, RapidEnums::SHAPETYPE::LINE))
		{
			if (s1->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				temp = s1;
				s1 = s2;
				s2 = temp;
			}
			double a1[2] = {((Line*)s2)->getPoint1()->getX(),((Line*)s2)->getPoint1()->getY()}, b1[2] = {((Line*)s2)->getPoint2()->getX(),((Line*)s2)->getPoint2()->getY()},
				 a2[2] = {((Circle*)s1)->getCenter()->getX(),((Circle*)s1)->getCenter()->getY()};
			count = RMATH2DOBJECT->Finitelineto_Circle(&a1[0], &b1[0], &a2[0], ((Circle*)s1)->Radius(),&p[0], &ans[0]);
			GRAFIX->drawLineStipple(ans[0],ans[1],p[0],p[1]);
		}
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::ARC, RapidEnums::SHAPETYPE::LINE))
		{
			if (s1->ShapeType == RapidEnums::SHAPETYPE::LINE)
			{
				temp = s1;
				s1 = s2;
				s2 = temp;
			}
			double a1[2] = {((Line*)s2)->getPoint1()->getX(),((Line*)s2)->getPoint1()->getY()},b1[2] = {((Line*)s2)->getPoint2()->getX(),((Line*)s2)->getPoint2()->getY()};
			double a2[2] = {((Circle*)s1)->getCenter()->getX(),((Circle*)s1)->getCenter()->getY()}, angles[2] = {((Circle*)s1)->Startangle(),((Circle*)s1)->Sweepangle()};
			count = RMATH2DOBJECT->Finitelineto_arc(&a1[0], &b1[0], &a2[0],((Circle*)s1)->Radius(), &angles[0],&p[0],&ans[0]);
			if(count == 1)
				GRAFIX->drawLineStipple(ans[0],ans[1],p[0],p[1]);
			if(count == 2)
			{
				GRAFIX->drawLineStipple(ans[0],ans[1],p[0],p[1]);
				GRAFIX->drawArcStipple(((Circle*)s1)->getCenter()->getX(), ((Circle*)s1)->getCenter()->getY(), ((Circle*)s1)->Radius(), ans[2], ans[3]);
			}
		}
		////*********************************XRAY WITH DIFFERENT SHAPES**********************************//
		////*********************************EXCEPT XLINE AND LINE***************************************//
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XRAY, RapidEnums::SHAPETYPE::CIRCLE))
		{
			if(s1->ShapeType == RapidEnums::SHAPETYPE::XRAY)
			{
				temp = s1;
				s1 = s2;
				s2 = temp;
			}
			GRAFIX->drawLineStipple(((Line*)s2)->getPoint1()->getX(),((Line*)s2)->getPoint1()->getY(),p[0],p[1]);
		}
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::XRAY, RapidEnums::SHAPETYPE::ARC))
		{
			if(s1->ShapeType == RapidEnums::SHAPETYPE::XRAY)
			{
				temp = s1;
				s1 = s2;
				s2 = temp;
			}
			double e1[2] = {((Line*)s2)->getPoint1()->getX(),((Line*)s2)->getPoint1()->getY()}, a1[2] = {((Circle*)s1)->getCenter()->getX(),((Circle*)s1)->getCenter()->getY()};
			double angles[2] = {((Circle*)s1)->Startangle(),((Circle*)s1)->Sweepangle()};
			count = RMATH2DOBJECT->Rayto_Arc(&e1[0], ((Line*)s2)->Angle(), &a1[0], ((Circle*)s1)->Radius(), &angles[0], &p[0], &ans[0]);
			if(count == 1)
				GRAFIX->drawLineStipple(ans[0],ans[1],p[0],p[1]);
			if(count == 2)
			{
				GRAFIX->drawLineStipple(ans[0],ans[1],p[0],p[1]);
				GRAFIX->drawArcStipple(((Circle*)s1)->getCenter()->getX(),((Circle*)s1)->getCenter()->getY(), ((Circle*)s1)->Radius(), ans[2], ans[3]);
			}
		}
		////*****************************************CIRCLE TO ARC INTERSECTION*****************************************//
		else if(compareboth<int>(s1->ShapeType, s2->ShapeType, RapidEnums::SHAPETYPE::CIRCLE, RapidEnums::SHAPETYPE::ARC))
		{
			if(s1->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
			{
				temp = s1;
				s1 = s2;
				s2 = temp;
			}
			double a1[2] = {((Circle*)s1)->getCenter()->getX(),((Circle*)s1)->getCenter()->getY()}, angles[2] = {((Circle*)s1)->Startangle(),((Circle*)s1)->Sweepangle()};
			count = RMATH2DOBJECT->Circleto_arc(&a1[0],((Circle*)s1)->Radius(),&angles[0],&p[0],&ans[0]);
			GRAFIX->drawArcStipple(a1[0], a1[1], ((Circle*)s1)->Radius(), ans[0], ans[1]);	
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("HELP0014", __FILE__, __FUNCSIG__); }
}

//double to string conversion..//
std::string RCadApp::HighlightedPt2String(double x, double y, double z)
{
	try
	{
		int NoofDec = MAINDllOBJECT->MeasurementNoOfDecimal();
		char highlightPoint[60]; 
		RMATH2DOBJECT->Double2String(x, NoofDec, highlightPoint); 
		std::string PointX = highlightPoint;
		RMATH2DOBJECT->Double2String(y, NoofDec, highlightPoint); 
		std::string PointY = highlightPoint;
		RMATH2DOBJECT->Double2String(z, NoofDec, highlightPoint); 
		std::string PointZ = highlightPoint;
		std::string ConcatedStr = "(" +  PointX + ", " + PointY + ", " + PointZ + ")";
		strcpy(highlightPoint, ConcatedStr.c_str());  //Copy the string to Destination//
		return highlightPoint;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1079", __FILE__, __FUNCSIG__); return "0.0000";}
}

//Set Flag for highlighted axis..
void RCadApp::SetAxisHighLighted(Shape* CShape)
{
	try
	{
		std::string Str = (const char*)CShape->getModifiedName();
		RapidEnums::RAPIDPROJECTIONTYPE Utype = RapidEnums::RAPIDPROJECTIONTYPE(getCurrentUCS().UCSProjectionType());
		if(Utype == RapidEnums::RAPIDPROJECTIONTYPE::XY)
		{
			if(Str == "X Axis")
				Xaxis_Highlighted = true;
			if(Str == "Y Axis")
				Yaxis_Highlighted = true;
		}
		else if (Utype == RapidEnums::RAPIDPROJECTIONTYPE::YZ || Utype == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
		{
			if(Str == "Y Axis")
				Xaxis_Highlighted = true;
			if(Str == "Z Axis")
				Yaxis_Highlighted = true;
		}
		else if (Utype == RapidEnums::RAPIDPROJECTIONTYPE::XZ || Utype == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
		{
			if(Str == "X Axis")
				Xaxis_Highlighted = true;
			if(Str == "Z Axis")
				Yaxis_Highlighted = true;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1080", __FILE__, __FUNCSIG__); }
}

bool ExtendingShapes;
//Extend shapes for Shift Drag..
void RCadApp::ExtendShapes()
{
	try
	{
		if (ExtendingShapes) return;
		ExtendingShapes = true;
		Shape* CShape = (Shape*)getShapesList().selectedItem();
		if(CShape ==  NULL) return;
		//MessageBox(NULL, CShape->getOriginalName(), L"Rapid-I", MB_TOPMOST);
		double point[2] = { 0,0 };
		double mp[2] = {getWindow(getCurrentWindow()).getPointer().x, getWindow(getCurrentWindow()).getPointer().y};
		double angle = 0, intercept = 0, center[2];
		if(getCurrentWindow() == 0 || getCurrentWindow() == 1)
		{
			switch(CShape->ShapeType)
			{
			case RapidEnums::SHAPETYPE::LINE:
				if(lastsnap == ((Line*)CShape)->getPoint1() || lastsnap == ((Line*)CShape)->getPoint2())
				{
					angle = ((Line*)CShape)->Angle();
					intercept = ((Line*)CShape)->Intercept();
					RMATH2DOBJECT->Point_PerpenIntrsctn_Line(angle, intercept, &mp[0], &point[0]);
					if(lastsnap == ((Line*)CShape)->getPoint1())
						((Line*)CShape)->setPoint1(Vector(point[0], point[1], ((Line*)CShape)->getPoint1()->getZ()));
					else
						((Line*)CShape)->setPoint2(Vector(point[0], point[1], ((Line*)CShape)->getPoint2()->getZ()));
					((Line*)CShape)->calculateAttributes();
					this->VSelectedShapeUpdated = true;
					this->RSelectedShape_Updated = true;
					CShape->notifyAll(ITEMSTATUS::DATACHANGED, CShape);	
				}
				break;
			case RapidEnums::SHAPETYPE::XRAY:
				if(lastsnap == ((Line*)CShape)->getPoint1())
				{
					angle = ((Line*)CShape)->Angle();
					intercept = ((Line*)CShape)->Intercept();
					RMATH2DOBJECT->Point_PerpenIntrsctn_Line(angle, intercept, &mp[0], &point[0]);
					((Line*)CShape)->setPoint1(Vector(point[0], point[1], ((Line*)CShape)->getPoint1()->getZ()));
					this->VSelectedShapeUpdated = true;
					this->RSelectedShape_Updated = true;
					CShape->notifyAll(ITEMSTATUS::DATACHANGED, CShape);	
				}
				break;
			case RapidEnums::SHAPETYPE::ARC:
				if(lastsnap == ((Circle*)CShape)->getendpoint1() || lastsnap == ((Circle*)CShape)->getendpoint2())
				{
					((Circle*)CShape)->getCenter()->FillDoublePointer(&center[0]);
					RMATH2DOBJECT->Point_onCircle(&center[0], ((Circle*)CShape)->Radius(), &mp[0], &point[0]);
					if(lastsnap == ((Circle*)CShape)->getendpoint1())
						((Circle*)CShape)->setendpoint1(Vector(point[0], point[1], ((Circle*)CShape)->getendpoint1()->getZ()));
					else
						((Circle*)CShape)->setendpoint2(Vector(point[0], point[1], ((Circle*)CShape)->getendpoint2()->getZ()));
					((Circle*)CShape)->calculateAttributes();
					this->VSelectedShapeUpdated = true;
					this->RSelectedShape_Updated = true;
					CShape->notifyAll(ITEMSTATUS::DATACHANGED, CShape);	
				}
				break;
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1081", __FILE__, __FUNCSIG__); }
	ExtendingShapes = false;
}

//Rotate Selected text shapes..
bool RCadApp::RotateSelectedTextShape()
{
	try
	{
		Shape* Cshape = (Shape*)getShapesList().selectedItem();
		if(Cshape == NULL) return false;
		if(getCurrentWindow() == 0)
		{
			double mp[2] = {getWindow(getCurrentWindow()).getPointer().x, getWindow(getCurrentWindow()).getPointer().y};
			if(Cshape->ShapeType == RapidEnums::SHAPETYPE::RTEXT)
			{
				((RText*)Cshape)->RotateText(mp[0], mp[1]);
				this->VSelectedShapeUpdated = true;
				this->RSelectedShape_Updated = true;
				Cshape->notifyAll(ITEMSTATUS::DATACHANGED, Cshape);	
				return true;
			}
		}
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1082", __FILE__, __FUNCSIG__); return false;}
}

void RCadApp::CreateNewEntity_RightClickOnVideo()
{
	try
	{
		switch(SmartMeasureType)
		{
		case RapidEnums::SMARTMEASUREMENTMODE::DIAMEASUREMENT:
			setHandler(new DefaultHandler());
			setHandler(new SM_DiameterMeasureHandler());
			break;

		case RapidEnums::SMARTMEASUREMENTMODE::RADIUSMEASUREMENT:
			setHandler(new DefaultHandler());
			setHandler(new SM_RadiusMeasureHandler());
			break;

		case RapidEnums::SMARTMEASUREMENTMODE::ANGLEMEASUREWITH_XAXIS:
			setHandler(new DefaultHandler());
			setHandler(new SM_AngleMeasureWithXAxis());
			break;

		case RapidEnums::SMARTMEASUREMENTMODE::ANGLEMEASUREWITH_YAXIS:
			setHandler(new DefaultHandler());
			setHandler(new SM_AngleMeasureWithYAxis());
			break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1083", __FILE__, __FUNCSIG__);}
}

//Video Left Mouse Down..
void RCadApp::OnLeftMouseDown_Video(double x, double y)
{
	try
	{
		Wait_VideoGraphicsUpdate();
		Wait_RcadGraphicsUpdate();
		currentwindow = 0;
		/*if(HELPEROBJECT->StartProfileScan)
		{
			HELPEROBJECT->StartProfileScan = false;
			HELPEROBJECT->ProfileScan();
			return;
		}*/
		this->MouseClickAndDragMode(true);
		if(ShapePasted)
		{
			for each(Shape* CSh in CurrentCopiedShapeList)
				CSh->UpdateEnclosedRectangle();
			CurrentCopiedShapeList.clear();
			ShapePasted = false;
		}
		if(ControlOn)
		{
			if(ShiftON)
			{
				if(highlightedShape() != NULL && highlightedShape()->Normalshape())
				{
					selectShape(highlightedShape()->getId(), true);
					return;
				}
			}
			else
			{
				if(highlightedShape() != NULL && highlightedShape()->Normalshape())
				{
					selectShape(highlightedShape()->getId(), false);
					return;
				}
			}
		}
		if(SnapPointHighlighted())
			MousedragwithShape(true);
		if(ShiftON && MousedragwithShape())
		{
			Shape* sh = getCurrentUCS().getIPManager().getParent1();
			Shape* sh1 = getCurrentUCS().getIPManager().getParent2();
			if(sh1 == NULL)
			{
				if(sh->ShapeAsfasttrace())
					MousedragwithShape(false);
				else
				{
					selectShape(sh->getId(), false);
					return;
				}
			}
		}
		if(MousedragwithShape())
		{
			Shape* sh = getCurrentUCS().getIPManager().getParent1();
			if(sh->ShapeType == RapidEnums::SHAPETYPE::RTEXT)
			{
				selectShape(sh->getId(), false);
				return;
			}
		}
		getWindow(0).mouseDown(x, y);
		if(HELPEROBJECT->PickEndPoint_ProfileScan)
		{
			if(highlightedPoint() != NULL)
			{
				HELPEROBJECT->PickEndPoint_ProfileScan = false;
				HELPEROBJECT->ProfileScanStart_Pt.X = highlightedPoint()->getX();
				HELPEROBJECT->ProfileScanStart_Pt.Y = highlightedPoint()->getY();
				HELPEROBJECT->ProfileLastPtVector = highlightedPoint();
				HELPEROBJECT->profileScanEndPtPassed = true;
				MAINDllOBJECT->PointsTakenProfileScan(0);
				return;
			}
		}
		if(HELPEROBJECT->profile_scan_running == false && HELPEROBJECT->profile_scan_ended == false && HELPEROBJECT->profile_scan_failed == true)
		{
			if((HELPEROBJECT->skip_cont_constraint)&&(HELPEROBJECT->profile_scan_stopped == false))
			{
				HELPEROBJECT->profileScanPtsCnt += 1;
				if(HELPEROBJECT->profileScanPtsCnt == 1)
				{
					if(CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::PROFILESCANHANDLER)
						MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PROFILESCANHANDLER);
					currentHandler->LmouseDown_x(getWindow(0).getPointer().x, getWindow(0).getPointer().y, 0, 0, false, false);
					return;
				}
				else if(HELPEROBJECT->profileScanPtsCnt == 2)
				{
					currentHandler->LmouseDown_x(getWindow(0).getPointer().x, getWindow(0).getPointer().y, 0, 0, false, false);
					return;
				}
			}
			else
			{
				HELPEROBJECT->ProfileScan();
				return;
			}
		}
		if(PPCALCOBJECT->FirstMousedown_Partprogram_Manual() && PPCALCOBJECT->IsPartProgramRunning())
			PPCALCOBJECT->ShiftShape_ForFirstMouseDown_Manual(Vector(getWindow(0).getPointer().x, getWindow(0).getPointer().y, getCurrentDRO().getZ()));
		if(EditmeasurementMode())
		{
			EditmeasurementMode(false);
			return;
		}
		if(ControlOn)
		{
			if(getSelectedDimList().getList().size() == 1)
			{
				CurrentSelectedDim = (DimBase*)getDimList().selectedItem();
				if(CurrentSelectedDim->ChildMeasurementType() && CurrentSelectedDim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE)
				{
					DimChildMeasurement* CdimPt = (DimChildMeasurement*)CurrentSelectedDim;
					if(getDimList().ItemExists(CdimPt->ParentMeasureId()))
					{
						CurrentSelectedDim = (DimBase*)getDimList().getList()[CdimPt->ParentMeasureId()];
					}
				}
				EditmeasurementMode(true);
			}
			return;
		}	
		
		currentHandler->L_mouseDownPixel(x, y);
		currentHandler->PreventDoublePt = true;
		if (MeasurementMode()) currentHandler->PreventDoublePt = false;

		if(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CONTOURSCAN_FGHANDLER)
			currentHandler->LmouseDown_x(getWindow(0).getPointer().x, getWindow(0).getPointer().y);
		else if(highlightedShape() != NULL && ShowExplicitIntersectionPoints() && (VideoMpointType == RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS || VideoMpointType == RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL) && 
			(CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::CONTOURSCAN_FGHANDLER && CURRENTRAPIDHANDLER != RapidEnums::RAPIDDRAWHANDLERTYPE::RAPIDFRAMEGRAB_HANDLER))
			currentHandler->LmouseDown_x(SnapPointModePoint.getX(), SnapPointModePoint.getY());
		else if(this->SnapPointHighlighted())
			currentHandler->LmouseDown_x(lastsnap->getX(), lastsnap->getY());
		else 
			currentHandler->LmouseDown_x(getWindow(0).getPointer().x, getWindow(0).getPointer().y, 0, 0, false, false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1084", __FILE__, __FUNCSIG__); }
}

//Video Right Mouse down..
void RCadApp::OnRightMouseDown_Video(double x, double y)
{
	try
	{
		Wait_VideoGraphicsUpdate();
		Wait_RcadGraphicsUpdate();
		currentwindow = 0;
	
		if(THREADCALCOBJECT->ThreadMeasurementMode() || THREADCALCOBJECT->ThreadMeasurementModeStarted())
		{
			if(currentHandler->getClicksDone() == 0)
			{
				THREADCALCOBJECT->CurrentSelectedThreadAction->IncrementShapePointer();
				THREADCALCOBJECT->ThreadShape_Updated();
				SetStatusCode("RCadApp01");
			}
			else
				currentHandler->RmouseDown(getWindow(0).getPointer().x, getWindow(0).getPointer().y);
		}
		else
		{
			if(EditmeasurementMode())
			{
				DimBase* Cdim = (DimBase*)getDimList().selectedItem();
				Cdim->ChangeMeasurementType();
				SelectedMeasurement_Updated();
				return;
			}
			if(SmartMeasureType == RapidEnums::SMARTMEASUREMENTMODE::DEFAULTTYPE && (CURRENTRAPIDHANDLER == RapidEnums::RAPIDDRAWHANDLERTYPE::RAPIDFRAMEGRAB_HANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::PROBE_HANDLER))	
			{
				if(currentHandler->getClicksDone() == 0)
				{
					AddShapeForRightClick();
				}
				else
					 currentHandler->RmouseDown(getWindow(0).getPointer().x, getWindow(0).getPointer().y);
				return;
			}
			else if(SmartMeasureType != RapidEnums::SMARTMEASUREMENTMODE::DEFAULTTYPE && CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::PROBE_HANDLER)	
			{
				if(currentHandler->getClicksDone() == 0)
					CreateNewEntity_RightClickOnVideo();
				else
					currentHandler->RmouseDown(getWindow(0).getPointer().x, getWindow(0).getPointer().y);
				return;
			}
			if(this->SnapPointHighlighted())
				currentHandler->RmouseDown(lastsnap->getX(), lastsnap->getY());
			else
				currentHandler->RmouseDown(getWindow(0).getPointer().x, getWindow(0).getPointer().y);
			MAINDllOBJECT->update_VideoGraphics(true);
			MAINDllOBJECT->update_RcadGraphics();
		}

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1085", __FILE__, __FUNCSIG__); }
}

//Video Middle mouse down..
void RCadApp::OnMiddleMouseDown_Video(double x, double y)
{
	try
	{
		currentwindow = 0;
		if(MAINDllOBJECT->GetClickDone() == 0)
		{
			if(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB)
			{
				std::string Alignment = "Crosshair";
				std::string BtnName = "FrameGrab Arc";
				FramGrabUpdate((char*)Alignment.c_str(),(char*)BtnName.c_str(), true);	
			}		
			else if(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB)
			{
				std::string Alignment = "Crosshair";
				std::string BtnName = "FrameGrab Circle";
				FramGrabUpdate((char*)Alignment.c_str(),(char*)BtnName.c_str(), true);	
			}						
			else if(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CIRCLE_FRAMEGRAB)
			{
				std::string Alignment = "Crosshair";
				std::string BtnName = "FrameGrab Rectangle";
				FramGrabUpdate((char*)Alignment.c_str(),(char*)BtnName.c_str(), true);	
			}											
			else if(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB)
			{
				std::string Alignment = "Crosshair";
				std::string BtnName = "FrameGrab Fixed Rectangle";
				FramGrabUpdate((char*)Alignment.c_str(),(char*)BtnName.c_str(), true);	
			}				
			else if(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::FIXEDRECT_FRAMEGRAB)
			{
				std::string Alignment = "Crosshair";
				std::string BtnName = "FrameGrab AngularRect";
				FramGrabUpdate((char*)Alignment.c_str(),(char*)BtnName.c_str(), true);	
			}				
		}
		currentHandler->MmouseDown(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1086", __FILE__, __FUNCSIG__); }
}

//Video Left Mouse up..
void RCadApp::OnLeftMouseUp_Video(double x, double y)
{
	try
	{
		currentwindow = 0;
		this->MouseClickAndDragMode(false);
		//if(MousedragwithShape() && ShiftON)
		//{
		//	BaseItem* Cshape = getShapesList().selectedItem();
		//	Cshape->notifyAll(ITEMSTATUS::DATACHANGED, Cshape);
		//}
		this->MousedragwithShape(false);
		if(EditmeasurementMode())
		{
			EditmeasurementMode(false);
			return;
		}
		currentHandler->LmouseUp(getWindow(0).getPointer().x, getWindow(0).getPointer().y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1087", __FILE__, __FUNCSIG__); }
}

//Video Right mouse up..
void RCadApp::OnRightMouseUp_Video(double x, double y)
{
	try
	{
		currentwindow = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1088", __FILE__, __FUNCSIG__); }
}

//Video Middle Mouse up..
void RCadApp::OnMiddleMouseUp_Video(double x, double y)
{
	try
	{
		this->MouseClickAndDragMode(false);
		currentwindow = 0;
		currentHandler->MmouseUp(x,y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1089", __FILE__, __FUNCSIG__); }
}

//Video Mouse move..
void RCadApp::OnMouseMove_Video(double x, double y)
{
	try
	{
		currentwindow = 0;
		getWindow(0).mouseMove(x, y);
		double CurrentMousePosX = getWindow(0).getPointer().x;
		double CurrentMousePosY = getWindow(0).getPointer().y;
		if(MousedragwithShape() && ShiftON)
		{
			ExtendShapes();
			update_VideoGraphics();
			update_RcadGraphics();
			return;
		}
		if(MousedragwithShape())
		{
			if(RotateSelectedTextShape())
			{
				update_VideoGraphics();
				update_RcadGraphics();
				return;
			}
		}
		if(ShapePasted)
		{
			VSelectedShapeUpdated = true;
			RSelectedShape_Updated = true;
			Vector Cmpt(CurrentMousePosX - middlepttemp.getX(), CurrentMousePosY - middlepttemp.getY(), 0);
			HELPEROBJECT->moveShapeCollection(CurrentCopiedShapeList, Cmpt, true);
			middlepttemp.set(CurrentMousePosX, CurrentMousePosY, 0);
		}

		this->DrawSnapPointOnHighlightedShape(false);
		this->SnapPointHighlighted(false); 
		this->ShapeHighlighted(false); 
		this->MeasurementHighlighted(false); 
		this->DXFShapeHighlighted(false);
		this->ShapeHighlightedForImplicitSnap(false);
		if(EditmeasurementMode())
		{
			double SelectionLine[6];
			getWindow(1).CalculateSelectionLine(&SelectionLine[0]);
			CurrentSelectedDim->SetSelectionLine(&SelectionLine[0]);
			CurrentSelectedDim->SetMousePosition(CurrentMousePosX, CurrentMousePosY, 0);
			RSelectedMeasurement_Updated = true;
			VSelectedMeasurement_Updated = true;
		}
		Vector* vx = NULL;
		if(MouseClickAndDragMode() && (CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::FASTRACE_HANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::FASTRACE_HANDLER_ENDPT))
		{
			if(CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER)
				currentHandler->mouseMove_x(CurrentMousePosX, CurrentMousePosY, 0, false, false);
		}
		else
		{
			if(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::DMM_HANDLER && !ShowExplicitIntersectionPoints())
			{
				currentHandler->mouseMove_x(CurrentMousePosX, CurrentMousePosY, 0, false, false);
			}
			else
			{
				//if(ShowExplicitIntersectionPoints())
				{
					if(getCurrentUCS().UCSMode() == 4)
						vx = this->getCurrentUCS().getIPManager().getNearestSnapPointUCS(getShapesList(), &getCurrentUCS().transform[0], CurrentMousePosX, CurrentMousePosY, getWindow(0).getUppX() * SNAPDIST, ShapePasted);
					else
						vx = this->getCurrentUCS().getIPManager().getNearestSnapPoint(getShapesList(), CurrentMousePosX, CurrentMousePosY, getWindow(0).getUppX() * SNAPDIST, ShapePasted);
					if(nearestShape != NULL && vx == NULL)
					{
						if(getCurrentUCS().UCSMode() == 4)
							vx = this->getCurrentUCS().getIPManager().getNearestIntersectionPointOnShapeUCS((ShapeWithList*)nearestShape, &getCurrentUCS().transform[0], CurrentMousePosX, CurrentMousePosY, getWindow(0).getUppX() * SNAPDIST, ShapePasted);
						else
							vx = this->getCurrentUCS().getIPManager().getNearestIntersectionPointOnShape((ShapeWithList*)nearestShape, CurrentMousePosX, CurrentMousePosY, getWindow(0).getUppX() * SNAPDIST, ShapePasted);
					}
					if(vx != NULL)
					{
						this->SnapPointHighlighted(true);
						lastsnap = vx;
						currentHandler->mouseMove_x(vx->getX(), vx->getY());
					}
					else
					{
						this->CurrentExtensionFlag = false;
						Shape* nshape = NULL; 
						DimBase* nmeasure = NULL;
						if(nearestShape != NULL)
						{
							nearestShape->HighlightedFormouse(false);
							if(nearestShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
							{
								if(((Circle*)nearestShape)->CircleType == RapidEnums::CIRCLETYPE::PCDCIRCLE)
								{
									for each(Shape* Csh in ((PCDCircle*)nearestShape)->PCDParentCollection)
										Csh->HighlightedForPCDHighLight(false);
								}
							}
							VShape_Updated = true; RShape_Updated = true;
						}
						if(DXFnearestShape != NULL)
						{
							this->DXFnearestShape->HighlightedFormouse(false);
							this->DXFnearestShape = NULL;
							this->VDXFShape_updated = true;
						}
						/*if(nearestmeasure != NULL)
						{
							nearestmeasure->HighlightedForMouseMove(false);
							VMeasurement_Updated = true;
							RMeasurement_Updated = true;
						}*/
						nearestShape = NULL; nearestmeasure = NULL;
						DXFnearestShape = NULL;
						nshape = HELPEROBJECT->getNearestShapeOnVideo(getShapesList(), CurrentMousePosX, CurrentMousePosY, SNAPDIST * getWindow(0).getUppX(), true);
						if(nshape == NULL) // DXF Shape highlight during DMM!!!
						{
							nshape = HELPEROBJECT->getNearestShape(DXFEXPOSEOBJECT->CurrentDXFShapeList, CurrentMousePosX, CurrentMousePosY, SNAPDIST * getWindow(0).getUppX());
							if(nshape != NULL)
							{
								this->DXFnearestShape = nshape;
								this->DXFnearestShape->HighlightedFormouse(true);
								this->VDXFShape_updated = true;
								this->DXFShapeHighlighted(true);
								nshape = NULL;
							}
						}
						if(currentHandler->CurrentdrawHandler != NULL)
						{
							if(getDimList().count() > 0 && !PPCALCOBJECT->IsPartProgramRunning() && !EditmeasurementMode() && currentHandler->getClicksDone() == 0 && currentHandler->CurrentdrawHandler->getClicksDone() == 0)
								nmeasure = HELPEROBJECT->getNearestmeasure(getDimList(), CurrentMousePosX, CurrentMousePosY, 0);
						}
						else if(getDimList().count() > 0 && !PPCALCOBJECT->IsPartProgramRunning() && !EditmeasurementMode() && currentHandler->getClicksDone() == 0)
							nmeasure = HELPEROBJECT->getNearestmeasure(getDimList(), CurrentMousePosX, CurrentMousePosY, 0);
						if(ShowHideFGPoints() && nshape != NULL)
						{
							if(nshape->selected())
								nshape = NULL;
						}
						if(nshape != NULL)
						{
							nearestShape = nshape;
							if(!ShapeHighlightedForImplicitSnap())
							{
								nearestShape->HighlightedFormouse(true);
								if(nearestShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
								{
									if(((Circle*)nearestShape)->CircleType == RapidEnums::CIRCLETYPE::PCDCIRCLE)
									{
										for each(Shape* Csh in ((PCDCircle*)nearestShape)->PCDParentCollection)
											Csh->HighlightedForPCDHighLight(true);
									}
								}
								CurrentShapeName = (const char*)nearestShape->getModifiedName();
								VShape_Updated = true; RShape_Updated = true;
								this->ShapeHighlighted(true);
							}
						}
						if(nmeasure != NULL)
						{
							nearestmeasure = nmeasure;
							//nearestmeasure->HighlightedForMouseMove(true);
							if(!nearestmeasure->selected())
								selectMeasurement(nearestmeasure->getId(), false, false);
							VMeasurement_Updated = true;
							RMeasurement_Updated = true;
							this->MeasurementHighlighted(true);
						}
						if(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CONTOURSCAN_FGHANDLER || CURRENTRAPIDHANDLER == RapidEnums::RAPIDDRAWHANDLERTYPE::RAPIDFRAMEGRAB_HANDLER)
						{
							currentHandler->mouseMove_x(CurrentMousePosX, CurrentMousePosY, 0, false);
						}
						else if(CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER)
						{
							if(highlightedShape() != NULL && ShowExplicitIntersectionPoints() && (VideoMpointType == RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS || VideoMpointType == RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL))
							{
								this->DrawSnapPointOnHighlightedShape(GetPointOnHighlightedShape(CurrentMousePosX, CurrentMousePosY));
								currentHandler->mouseMove_x(SnapPointModePoint.getX(), SnapPointModePoint.getY());
							}
							else
								currentHandler->mouseMove_x(CurrentMousePosX, CurrentMousePosY, 0, false, false);
						}
					}
				}
				//else
					//currentHandler->mouseMove_x(CurrentMousePosX, CurrentMousePosY);
			}
		}
		update_VideoGraphics();
		update_RcadGraphics();
	}
	catch(...)
	{
		VideoMouseFlag = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1090", __FILE__, __FUNCSIG__); 
	}
}

//Video mouse scroll..
void RCadApp::OnMouseWheel_Video(int evalue)
{
	try
	{
		currentwindow = 0;
		currentHandler->MouseScroll(evalue>0? true:false);
		update_VideoGraphics();
		update_RcadGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1091", __FILE__, __FUNCSIG__); }
}

//Video Mouse enter...
void RCadApp::OnMouseEnter_Video()
{
	try
	{
		MouseOnVideo(true);
		currentwindow = 0;	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1092", __FILE__, __FUNCSIG__); }
}

//Video mouse leave..
void RCadApp::OnMouseLeave_Video()
{
	try
	{
		MouseOnVideo(false);
		update_VideoGraphics();
		update_RcadGraphics();
		double data[3] = {0};
		getWindow(0).ResetMouseLastPosition();
		UpdateMousePositionOnVideo(&data[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1093", __FILE__, __FUNCSIG__); }
}

//Rcad left mouse down..
void RCadApp::OnLeftMouseDown_Rcad(double x, double y)
{
	try
	{
		Left_Mouse_Down = true;
		Wait_VideoGraphicsUpdate();
		Wait_RcadGraphicsUpdate();
		currentwindow = 1;
		getWindow(1).mouseDown(x, y);
		this->MouseClickAndDragMode(true);	
		if(ShapeHighlighted())
		{
			if(Right_Mouse_Down&&(!Is_measurement_mode_on))
			{
					double Xvalue = MAINDllOBJECT->getWindow(1).getCam().x + x * MAINDllOBJECT->getWindow(1).getUppX(), Yvalue = MAINDllOBJECT->getWindow(1).getCam().y - y * MAINDllOBJECT->getWindow(1).getUppY();
					double Position[3] = {Xvalue, Yvalue, MAINDllOBJECT->getCurrentDRO().getZ()};
					TakeProbePointCallback(Position, true);
				return;
			}
		}

		if(RepeatAutoMeasurement::CheckRepeatAutoMeasureInstance())
		{
			if(AUTOMEASUREOBJECT->TakeReferencePointFlag)
			{
				if(highlightedPoint() != NULL)
				{
					AUTOMEASUREOBJECT->pointOfRotation[0] = highlightedPoint()->getX();
					AUTOMEASUREOBJECT->pointOfRotation[1] = highlightedPoint()->getY();
					AUTOMEASUREOBJECT->pointOfRotation[2] = highlightedPoint()->getZ();
					AUTOMEASUREOBJECT->PtOfRotationFlag = true;
					AUTOMEASUREOBJECT->TakeReferencePointFlag = false;	
					AUTOMEASUREOBJECT->ResetParam_CircularRepeatMeasure(2);
				}
				return;
			}
		}
		if(this->MeasurementMode() && this->TruePositionMode)
		{
			if(this->highlightedPoint())
			{
				MAINDllOBJECT->TruePositionOffset(this->highlightedPoint()->getX(), this->highlightedPoint()->getY());
				this->TruePositionMode = false;
				return;
			}
		}
		if(HELPEROBJECT->SnapPtForFixture)
		{
			if(highlightedPoint() != NULL)
			{
				HELPEROBJECT->SnapPt_Pointer = highlightedPoint();
				HELPEROBJECT->SnapPtForFixture = false;
				MAINDllOBJECT->FixtureRefPoint_Taken();
				MAINDllOBJECT->Fixture_GotoNextCellCallBack();
			}
			else
			{
				//message...
			}
			return;
		}
		if(RcadWindow3DMode())
		{
			double SelectionLine[6];
			getWindow(1).CalculateSelectionLine(&SelectionLine[0]);
			getWindow(1).camz = HELPEROBJECT->GetZlevelOfRcadWindow(getShapesList(), &SelectionLine[0]);
		}
		if(ShapePasted)
		{
			for each(Shape* CSh in CurrentCopiedShapeList)
				CSh->UpdateEnclosedRectangle();
			CurrentCopiedShapeList.clear();
			ShapePasted = false;
		}
		if(ControlOn)
		{
			if(ShiftON)
			{
				if(highlightedShape() != NULL && highlightedShape()->Normalshape())
				{
					selectShape(highlightedShape()->getId(), true);
					return;
				}
			}
			else if(AltON)
			{
				double GotoPos[3] = {getWindow(1).getPointer().x, getWindow(1).getPointer().y, getCurrentDRO().getZ()}; 
				if(highlightedPoint() != NULL)
				{
					Vector* TempPt = highlightedPoint();
					GotoPos[0] = TempPt->getX(); GotoPos[1] = TempPt->getY(); GotoPos[2] = TempPt->getZ();
				}
				MAINDllOBJECT->MoveMachineToRequiredPos(GotoPos[0], GotoPos[1], GotoPos[2], true, RapidEnums::MACHINEGOTOTYPE::RCADWINDOWGOTO);
				return;
			}
			else
			{
				if(highlightedShape() != NULL && highlightedShape()->Normalshape())
				{
					selectShape(highlightedShape()->getId(), false);
					return;
				}
			}
		}
		if(SnapPointHighlighted())
			MousedragwithShape(true);
		if(ShiftON && MousedragwithShape())
		{
			Shape* sh = getCurrentUCS().getIPManager().getParent1();
			Shape* sh1 = getCurrentUCS().getIPManager().getParent2();
			if(sh1 == NULL)
			{
				if(sh->ShapeAsfasttrace())
					MousedragwithShape(false);
				else
				{
					selectShape(sh->getId(), false);
					return;
				}
			}
		}
		if(HELPEROBJECT->PickEndPoint_ProfileScan)
		{
			if(highlightedPoint() != NULL)
			{
				HELPEROBJECT->PickEndPoint_ProfileScan = false;
				HELPEROBJECT->ProfileScanStart_Pt.X = highlightedPoint()->getX();
				HELPEROBJECT->ProfileScanStart_Pt.Y = highlightedPoint()->getY();
				HELPEROBJECT->ProfileLastPtVector = highlightedPoint();
				HELPEROBJECT->profileScanEndPtPassed = true;
				MAINDllOBJECT->PointsTakenProfileScan(0);
				return;
			}
		}
		if(PPCALCOBJECT->FirstMousedown_Partprogram_Manual() && PPCALCOBJECT->IsPartProgramRunning())
			PPCALCOBJECT->ShiftShape_ForFirstMouseDown_Manual(Vector(getWindow(1).getPointer().x, getWindow(1).getPointer().y, getCurrentDRO().getZ()));
		currentwindow = 1;
		if(EditmeasurementMode())
		{
			EditmeasurementMode(false);
			return;
		}
		if(ControlOn)
		{
			if(getSelectedDimList().getList().size() == 1)
			{
				CurrentSelectedDim = (DimBase*)getDimList().selectedItem();
				if(CurrentSelectedDim->ChildMeasurementType() && CurrentSelectedDim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE)
				{
					DimChildMeasurement* CdimPt = (DimChildMeasurement*)CurrentSelectedDim;
					CurrentSelectedDim = (DimBase*)getDimList().getList()[CdimPt->ParentMeasureId()];
				}
				EditmeasurementMode(true);
			}
			return;
		}
		if(getWindow(1).PanMode)
		{
			getWindow(1).togglePan();
			if(getWindow(1).GetpanMode())
				SetRcadCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_PANHOLD);
			else
				SetRcadCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
		}
		if(!MeasurementMode() && ShapeHighlighted() && (VideoMpointType == RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS || VideoMpointType == RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL || Derivedmodeflag))
		{
			SnapPoint* Snp;
			switch(nearestShape->ShapeType)
			{				
				case RapidEnums::SHAPETYPE::CONE:
				case RapidEnums::SHAPETYPE::CIRCLE:
				case RapidEnums::SHAPETYPE::ARC:
				case RapidEnums::SHAPETYPE::CIRCLE3D:
					{
						Shape* SelectedShape = NULL;
						SelectedShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
						if((SelectedShape != NULL) && (nearestShape->getId() == SelectedShape->getId()) && !currentHandler->AlignmentModeFlag)
							break;
						Snp = ((ShapeWithList*)nearestShape)->SnapPointList->getList()[3];
						getCurrentUCS().getIPManager().SetParents(Snp->getParent(), NULL);
						this->SnapPointHighlighted(true);
						lastsnap = Snp->getPt();
						currentHandler->LmouseDown_x(Snp->getPt()->getX(), Snp->getPt()->getY(),Snp->getPt()->getZ(), 0, true);
						this->SnapPointHighlighted(false);
						return;
					}
					break;
				case RapidEnums::SHAPETYPE::SPHERE:
					{
						Shape* SelectedShape = NULL;
						SelectedShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
						if((SelectedShape != NULL) && (nearestShape->getId() == SelectedShape->getId()) && !currentHandler->AlignmentModeFlag)
							break;
						Snp = ((ShapeWithList*)nearestShape)->SnapPointList->getList()[3];
						getCurrentUCS().getIPManager().SetParents(Snp->getParent(), NULL);
						this->SnapPointHighlighted(true);
						lastsnap = Snp->getPt();
						currentHandler->LmouseDown_x(Snp->getPt()->getX(), Snp->getPt()->getY(),Snp->getPt()->getZ(), 0, true);
						this->SnapPointHighlighted(false);
					}
					break;
				case RapidEnums::SHAPETYPE::INTERSECTIONSHAPE:
					{
						Shape* SelectedShape = NULL;
						SelectedShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
						if((SelectedShape != NULL) && (nearestShape->getId() == SelectedShape->getId()) && !currentHandler->AlignmentModeFlag)
							break;
						Snp = ((ShapeWithList*)nearestShape)->SnapPointList->getList()[3];
						getCurrentUCS().getIPManager().SetParents(Snp->getParent(), NULL);
						this->SnapPointHighlighted(true);
						lastsnap = Snp->getPt();
						currentHandler->LmouseDown_x(Snp->getPt()->getX(), Snp->getPt()->getY(),Snp->getPt()->getZ(), 0, true);
						this->SnapPointHighlighted(false);
					}
					break;
			}
		}
		if(!MeasurementMode() && highlightedShape() != NULL && (highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::PLANE) && ShowImplicitIntersectionPoints())
		{
			Shape* Cshape = highlightedShape();
			if(ShapesForIntersectionLine.empty())
			{
			   ShapesForIntersectionLine[Cshape->getId()] = Cshape;
			}
			else
			{
				bool newIntersection = true;
				list<BaseItem*> childFirstShape = (ShapesForIntersectionLine.begin())->second->getChild(), childSecondShape =  Cshape->getChild();
				if(!(childSecondShape.empty() || childFirstShape.empty()))
				{
					for each(BaseItem* chs1 in childSecondShape)
					{
						for each(BaseItem* chs2 in childFirstShape)
						{
							if(chs1->getId() == chs2->getId())
							{
								newIntersection = false;
								break;
							}
						}
						if(!newIntersection) break;
					}
				}
				if(newIntersection)
				{
					Shape *PShape = ShapesForIntersectionLine.begin()->second;
					double planeParam1[4] = {0}, planeParam2[4] = {0};
					((Plane*)Cshape)->getParameters(planeParam1);
					((Plane*)PShape)->getParameters(planeParam2);
					if(!(RMATH3DOBJECT->Checking_Parallel_Plane_to_Plane(planeParam1, planeParam2)))
					{
						Line* IntersectionLine =	new Line(_T("L3D"), RapidEnums::SHAPETYPE::LINE3D);
						IntersectionLine->LineType = RapidEnums::LINETYPE::INTERSECTIONLINE3D;
						PShape->addChild(IntersectionLine);
						Cshape->addChild(IntersectionLine);
						IntersectionLine->addParent(PShape);
						IntersectionLine->addParent(Cshape);
						IntersectionLine->UpdateBestFit();
						AddShapeAction::addShape(IntersectionLine);	
						UpdateShapesChanged(true);
					}
				}
				ShapesForIntersectionLine.clear();
				ShapesForIntersectionPoint.clear();
			}
		}
		else if(!ShapesForIntersectionLine.empty())
		{
			ShapesForIntersectionLine.clear();
		}

		if(!MeasurementMode() && highlightedShape() != NULL && 
			((highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE3D || 
				highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::XLINE3D ||
				highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::CONE ||
				highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::CYLINDER) ||
			(highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::PLANE)) && 
			ShowImplicitIntersectionPoints())
		{
			Shape* Cshape = highlightedShape();
			if(ShapesForIntersectionPoint.empty())
			{
				ShapesForIntersectionPoint[Cshape->getId()] = Cshape;
			}
			else if(Cshape->ShapeType != ShapesForIntersectionPoint.begin()->second->ShapeType)
			{
				bool newIntersection = true;
				Shape *PShape = ShapesForIntersectionPoint.begin()->second;
				list<BaseItem*> childFirstShape = PShape->getChild(), childSecondShape = Cshape->getChild();
				if(!(childSecondShape.empty() || childFirstShape.empty()))
				{
					for each(BaseItem* chs1 in childSecondShape)
					{
						for each(BaseItem* chs2 in childFirstShape)
						{
							if(chs1->getId() == chs2->getId())
							{
							newIntersection = false;
							break;
							}
						}
						if(!newIntersection) break;
					}
				}
				if(newIntersection)
				{
					if(Cshape->ShapeType != RapidEnums::SHAPETYPE::LINE3D && Cshape->ShapeType != RapidEnums::SHAPETYPE::XLINE3D)
					{
						Shape* temp = Cshape;
						Cshape = PShape;
						PShape = temp;
					}
					double lineParam[6] = {0}, planeParam[4] = {0};

					if (Cshape->ShapeType == RapidEnums::SHAPETYPE::CYLINDER || Cshape->ShapeType == RapidEnums::SHAPETYPE::CONE)
						((Cylinder*)Cshape)->getAxisLine(lineParam);
					else if(Cshape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || Cshape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D)
						((Circle*)Cshape)->getParameters(lineParam);
					else
						((Line*)Cshape)->getParameters(lineParam);

					((Plane*)PShape)->getParameters(planeParam);
					if(!(RMATH3DOBJECT->Checking_Parallel_Line_to_Plane(lineParam, planeParam)))
					{
						RPoint* intersectionPoint =	new RPoint(_T("P3D"), RapidEnums::SHAPETYPE::RPOINT3D);
						intersectionPoint->PointType = RPoint::RPOINTTYPE::INTERSECTIONPOINT3D;
						PShape->addChild(intersectionPoint);
						Cshape->addChild(intersectionPoint);
						intersectionPoint->addParent(PShape);
						intersectionPoint->addParent(Cshape);
						intersectionPoint->UpdateBestFit();
						AddShapeAction::addShape(intersectionPoint);	
						UpdateShapesChanged(true);
					}
				}
				ShapesForIntersectionPoint.clear();
				ShapesForIntersectionLine.clear();
			}
		}
		else if(!ShapesForIntersectionPoint.empty())
		{
			ShapesForIntersectionPoint.clear();
		}

		if(!MeasurementMode() && highlightedShape() != NULL && (highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE) && ShowImplicitIntersectionPoints())
		{
			Shape* Cshape = highlightedShape();
			if(ShapesForIntersectionPointFromLines.empty())
			{
			   ShapesForIntersectionPointFromLines[Cshape->getId()] = Cshape;
			}
			else if(Cshape->getId() !=  ShapesForIntersectionPointFromLines.begin()->second->getId())
			{
			   bool newIntersection = true;
			   Shape *PShape = ShapesForIntersectionPointFromLines.begin()->second;
			   list<BaseItem*> childFirstShape = PShape->getChild(), childSecondShape = Cshape->getChild();
			   if(!(childSecondShape.empty() || childFirstShape.empty()))
			   {
				   for each(BaseItem* chs1 in childSecondShape)
				   {
					 for each(BaseItem* chs2 in childFirstShape)
					 {
						 if(chs1->getId() == chs2->getId())
						 {
						   newIntersection = false;
						   break;
						 }
					 }
					 if(!newIntersection) break;
				   }
			   }
			   if(newIntersection)
			   {
				   double lineParam1[6] = {0}, lineParam2[6] = {0};
				   ((Line*)Cshape)->getParameters(lineParam1);
				   ((Line*)PShape)->getParameters(lineParam2);
				   if(!(RMATH3DOBJECT->Checking_Parallel_Line_to_Line(lineParam1, lineParam2)))
				   {
					   RPoint* intersectionPoint =	new RPoint(_T("P3D"), RapidEnums::SHAPETYPE::RPOINT3D);
					   intersectionPoint->PointType = RPoint::RPOINTTYPE::INTERSECTIONPOINT3D;
					   PShape->addChild(intersectionPoint);
					   Cshape->addChild(intersectionPoint);
					   intersectionPoint->addParent(PShape);
					   intersectionPoint->addParent(Cshape);
					   intersectionPoint->UpdateBestFit();
					   AddShapeAction::addShape(intersectionPoint);	
					   UpdateShapesChanged(true);
				   }
			   }
			   ShapesForIntersectionPointFromLines.clear();
			}
		}
		else if(!ShapesForIntersectionPointFromLines.empty())
		{
			ShapesForIntersectionPointFromLines.clear();
		}

		if(	currentHandler->AlignmentModeFlag || 
			CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::TWOARC_HANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::RCADZOOMOUT ||
			CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::TWOLINE_HANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::PERPENDICULAR_LINE3D || 
			CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::SHAPEPROJECTIONHANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::REFLECTIONHANDLER || 
			CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::TRANSLATIONHANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::TRANSLATIONHANDLERPOINTTOPOINT || 
			CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ROTATIONHANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::SELECTION_HANDLER || 
			CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::RCADZOOMIN || 
			CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::NEARESTPOINT || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::NEARESTPOINT3D || 
			CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::FARTHESTPOINT3D || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_1PT1LINE || 
			CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_2LINE || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::EDITCAD_1PT1LINE_HANDLER || 
			CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::EDITCAD_2LINEHANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB || 
			CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::MOVE_HANDLER || 
			CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::FARTHESTPOINT || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CIRCLE_FRAMEGRAB || 
			CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_1PT1LINE3D || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::INTERSECTIONSHAPEHANDLER|| 
			MeasurementMode() || SnapPointHighlighted() || Derivedmodeflag || VideoMpointType == RapidEnums::RWINDOW_CURSORTYPE::CURSOR_TRIM || SmartMeasureTypeMode())
		{
			if(SnapPointHighlighted() && CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::TRIMMING_HANDLER)
			{
				Shape *selShp = NULL;
				selShp = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
				if(selShp == NULL)
				{
					currentHandler->LmouseDown_x(lastsnap->getX(), lastsnap->getY(), lastsnap->getZ(), 0, true);
				}
				else if(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::SPHERE_HANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CLOUDPOINTS_HANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CONE_HANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::RELATIVEPOINT_HANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CIRCLE3D_HANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ELLIPSE3D_HANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CYLINDER_HANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CONE_HANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::PLANE_HANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CIRCLE3P_HANDLER)
				{
					bool callHandler = true;
					if(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1() != NULL)
					{
						if(selShp->getId() == MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1()->getId())
						  callHandler = false;
					}
					if(callHandler && MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2() != NULL)
					{
						if(selShp->getId() == MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2()->getId())
						  callHandler = false;
					}
				   if(callHandler)
					   currentHandler->LmouseDown_x(lastsnap->getX(), lastsnap->getY(), lastsnap->getZ(), 0, true);
				}
				else
				{
					currentHandler->LmouseDown_x(lastsnap->getX(), lastsnap->getY(), lastsnap->getZ(), 0, true);
				}
			}
			else if(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::SELECTION_HANDLER)
				currentHandler->LmouseDown_x(getWindow(1).getPointer(true).x, getWindow(1).getPointer(true).y);
			else
			{
				if(highlightedShape() != NULL)
				{
					this->DrawSnapPointOnHighlightedShape(GetPointOnHighlightedShape(getWindow(1).getPointer().x, getWindow(1).getPointer().y));
					currentHandler->PreventDoublePt = true;

					if (MeasurementMode())
					{
						currentHandler->PreventDoublePt = false;
						currentHandler->LmouseDown_x(SnapPointModePoint.getX(), SnapPointModePoint.getY(), SnapPointModePoint.getZ(), 0, true);
					}
					else
						currentHandler->LmouseDown_x(SnapPointModePoint.getX(), SnapPointModePoint.getY(), SnapPointModePoint.getZ(), 0, true);
				}
				else
					currentHandler->LmouseDown_x(getWindow(1).getPointer().x, getWindow(1).getPointer().y);
			}
			MousedragwithShape(false);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1094", __FILE__, __FUNCSIG__); }
}

//Rcad Right mouse down..
void RCadApp::OnRightMouseDown_Rcad(double x, double y)
{
	try
	{
		Right_Mouse_Down = true;
		Wait_VideoGraphicsUpdate();
		Wait_RcadGraphicsUpdate();
		dontUpdateGraphcis = true;
		currentwindow = 1;
		getWindow(1).mouseDown(x, y);
		this->MouseClickAndDragMode(true);	
		if(ShapeHighlighted())
		{
			if(Left_Mouse_Down&&(!Is_measurement_mode_on))
			{
					double Xvalue = MAINDllOBJECT->getWindow(1).getCam().x + x * MAINDllOBJECT->getWindow(1).getUppX(), Yvalue = MAINDllOBJECT->getWindow(1).getCam().y - y * MAINDllOBJECT->getWindow(1).getUppY();
					double Position[3] = {Xvalue, Yvalue, MAINDllOBJECT->getCurrentDRO().getZ()};
					TakeProbePointCallback(Position, true);
				return;
			}
		}
		if(EditmeasurementMode())
		{
			DimBase* Cdim = (DimBase*)getDimList().selectedItem();
			dontUpdateGraphcis = false;
			Cdim->ChangeMeasurementType();
			SelectedMeasurement_Updated();
			return;
		}
		if(!MeasurementMode())
		{
			if(highlightedPoint() != NULL)
			{
				//double Position[3] = {highlightedPoint()->getX(), highlightedPoint()->getY(), highlightedPoint()->getZ()};
				double Position[3] = {highlightedPoint()->getX(), highlightedPoint()->getY(), MAINDllOBJECT->getCurrentDRO().getZ()};
				TakeProbePointCallback(Position, false);
				return;
			}
		}
		if(!MeasurementMode() && ShapeHighlighted() && (VideoMpointType == RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS || VideoMpointType == RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL))
		{
			SnapPoint* Snp;
			switch(nearestShape->ShapeType)
			{
				case RapidEnums::SHAPETYPE::CIRCLE:
				case RapidEnums::SHAPETYPE::ARC:
					Snp = ((ShapeWithList*)nearestShape)->SnapPointList->getList()[3];
					getCurrentUCS().getIPManager().SetParents(Snp->getParent(), NULL);
					this->SnapPointHighlighted(true);
					lastsnap = Snp->getPt();
					currentHandler->RmouseDown(Snp->getPt()->getX(), Snp->getPt()->getY());
					dontUpdateGraphcis = false;
					this->SnapPointHighlighted(false);
					return;
					break;
			}
		}
		if(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::SELECTION_HANDLER || MeasurementMode() || SnapPointHighlighted() || Derivedmodeflag || VideoMpointType == RapidEnums::RWINDOW_CURSORTYPE::CURSOR_TRIM)
		{
			if(SnapPointHighlighted() && CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::TRIMMING_HANDLER)
				currentHandler->RmouseDown(lastsnap->getX(), lastsnap->getY());
			else
				currentHandler->RmouseDown(getWindow(1).getPointer().x, getWindow(1).getPointer().y);
		}
		if(!MeasurementMode() && SmartMeasureType == RapidEnums::SMARTMEASUREMENTMODE::DEFAULTTYPE && (CURRENTRAPIDHANDLER == RapidEnums::RAPIDDRAWHANDLERTYPE::RAPIDFRAMEGRAB_HANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::PROBE_HANDLER))	
		{
			if(currentHandler->getClicksDone() == 0)
			{
				AddShapeForRightClick();
			}
		}
		/*if(this->SnapPointHighlighted())
			currentHandler->RmouseDown(lastsnap->getX(), lastsnap->getY());
		else
			currentHandler->RmouseDown(getWindow(1).getPointer().x, getWindow(1).getPointer().y);*/
		dontUpdateGraphcis = false;
		MAINDllOBJECT->update_VideoGraphics();
		MAINDllOBJECT->update_RcadGraphics();
	}
	catch(...){ dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1095", __FILE__, __FUNCSIG__); }
}

//RCad Middle mosue down..
void RCadApp::OnMiddleMouseDown_Rcad(double x, double y)
{
	try
	{
		Wait_VideoGraphicsUpdate();
		Wait_RcadGraphicsUpdate();
		currentwindow = 1;
		getWindow(1).togglePan();
		SetRcadCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_PANHOLD);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1096", __FILE__, __FUNCSIG__); }
}

//Rcad Left mouse up..
void RCadApp::OnLeftMouseUp_Rcad(double x, double y)
{
	try
	{
		Left_Mouse_Down = false;
		currentwindow = 1;
		this->MouseClickAndDragMode(false);
		//if(MousedragwithShape() && ShiftON)
		//{
		//	BaseItem* Cshape = getShapesList().selectedItem();
		//	Cshape->notifyAll(ITEMSTATUS::DATACHANGED, Cshape);
		//}
		if(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::RCADZOOMIN || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::RCADZOOMOUT)
			getWindow(1).mouseDown(x,y);
		this->MousedragwithShape(false);
		if(getWindow(1).PanMode && getWindow(1).GetpanMode())
		{
			getWindow(1).togglePan();
			SetRcadCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_PANRELEASE);
		}
		if(EditmeasurementMode())
		{
			EditmeasurementMode(false);
			return;
		}
		currentHandler->LmouseUp(getWindow(1).getPointer().x, getWindow(1).getPointer().y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1097", __FILE__, __FUNCSIG__); }
}

//RCad right mouse up...
void RCadApp::OnRightMouseUp_Rcad(double x, double y)
{
	try
	{
		Right_Mouse_Down = false;
		currentwindow = 1;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1098", __FILE__, __FUNCSIG__); }
}

//Rcad Middle mouse up..
void RCadApp::OnMiddleMouseUp_Rcad(double x, double y)
{
	try
	{
		currentwindow = 1;
		this->MouseClickAndDragMode(false);
		if(getWindow(1).GetpanMode())
		{
			getWindow(1).togglePan();
			if(getWindow(1).PanMode)
				SetRcadCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_PANRELEASE);
			else if(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::TRIMMING_HANDLER)
				SetRcadCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_TRIM);
			else
				SetRcadCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1099", __FILE__, __FUNCSIG__); }
}

//bool MouseIsMoving_RCad;
//RCad mouse move..
void RCadApp::OnMouseMove_Rcad(double x, double y)
{
	//if (MouseIsMoving_RCad) return;
	//MouseIsMoving_RCad = true;
	try
	{
		currentwindow = 1;
		getWindow(1).mouseMove(x, y);
		double CurrentMousePosX = getWindow(1).getPointer().x;
		double CurrentMousePosY = getWindow(1).getPointer().y;
		if(MousedragwithShape() && ShiftON)
		{
			if (!lastsnap)
			{ }
			else
			{
				ExtendShapes();
				update_VideoGraphics();
				update_RcadGraphics();
				return;
			}
		}
		if(ShapePasted)
		{
			VSelectedShapeUpdated = true;
			RSelectedShape_Updated = true;
			Vector Cmpt(CurrentMousePosX - middlepttemp.getX(), CurrentMousePosY - middlepttemp.getY(), 0);
			HELPEROBJECT->moveShapeCollection(CurrentCopiedShapeList, Cmpt, true);
			middlepttemp.set(CurrentMousePosX, CurrentMousePosY, 0);
		}
		if(getWindow(1).GetpanMode())
		{
			update_RcadGraphics();
		}
		else
		{
			this->SnapPointHighlighted(false); 
			this->ShapeHighlighted(false); 
			this->MeasurementHighlighted(false); 
			this->DXFShapeHighlighted(false);
			this->ShapeHighlightedForImplicitSnap(false);
			if(EditmeasurementMode())
			{
				double SelectionLine[6];
				getWindow(1).CalculateSelectionLine(&SelectionLine[0]);
				CurrentSelectedDim->SetSelectionLine(&SelectionLine[0]);
				CurrentSelectedDim->SetMousePosition(CurrentMousePosX, CurrentMousePosY, 0);
				RSelectedMeasurement_Updated = true;
				VSelectedMeasurement_Updated = true;
			}
			//if(ShowExplicitIntersectionPoints())
			{
				Vector* vx = NULL;
				if(RcadWindow3DMode())
				{   
					//if(ThreeDRotationMode() && !getWindow(1).PanMode && !ControlOn) // Added By Rahul for Zoom to extent with Rotation..
					//{
					//	if(MouseClickAndDragMode())
					//	{
					//		if(MAINDllOBJECT->RcadZoomExtents())
					//			getWindow(1).ZoomToExtents(1);
					//	}
					//}
					double SelectionLine[6];
					getWindow(1).CalculateSelectionLine(&SelectionLine[0]);
					vx = this->getCurrentUCS().getIPManager().getNearestSnapPoint(getShapesList(), &SelectionLine[0], getWindow(1).getUppX() * SNAPDIST, ShapePasted);
					if(nearestShape != NULL && vx == NULL)
						vx = this->getCurrentUCS().getIPManager().getNearestIntersectionPointOnShape((ShapeWithList*)nearestShape, &SelectionLine[0], getWindow(1).getUppX() * SNAPDIST,ShapePasted);
				}
				else
				{
					vx = this->getCurrentUCS().getIPManager().getNearestSnapPoint(getShapesList(), CurrentMousePosX, CurrentMousePosY, getWindow(1).getUppX() * SNAPDIST, ShapePasted);
					if(nearestShape != NULL && vx == NULL)
						vx = this->getCurrentUCS().getIPManager().getNearestIntersectionPointOnShape((ShapeWithList*)nearestShape, CurrentMousePosX, CurrentMousePosY, getWindow(1).getUppX() * SNAPDIST,ShapePasted);
				}
				if(vx!=NULL)
				{
					this->SnapPointHighlighted(true);
					lastsnap = vx;
					currentHandler->mouseMove_x(vx->getX(), vx->getY(), vx->getZ(),true);
				}
				else
				{
					this->CurrentExtensionFlag = false;
					Shape* nshape = NULL; 
					DimBase* nmeasure = NULL;
					if(nearestShape != NULL)
					{
						nearestShape->HighlightedFormouse(false);
						if(nearestShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
						{
							if(((Circle*)nearestShape)->CircleType == RapidEnums::CIRCLETYPE::PCDCIRCLE)
							{
								for each(Shape* Csh in ((PCDCircle*)nearestShape)->PCDParentCollection)
									Csh->HighlightedForPCDHighLight(false);
							}
						}
						VShape_Updated = true; RShape_Updated = true;
					}
					/*if(nearestmeasure != NULL)
					{
						nearestmeasure->HighlightedForMouseMove(false);
						VMeasurement_Updated = true;
						RMeasurement_Updated = true;
					}*/
					nearestShape = NULL; nearestmeasure = NULL;
					DXFnearestShape = NULL;
					if(RcadWindow3DMode())
					{
						double SelectionLine[6];
						getWindow(1).CalculateSelectionLine(&SelectionLine[0]);
						nshape = HELPEROBJECT->getNearestShape(getShapesList(), &SelectionLine[0], SNAPDIST * getWindow(1).getUppX(), true);
					}
					else
						nshape = HELPEROBJECT->getNearestShape(getShapesList(), CurrentMousePosX, CurrentMousePosY, SNAPDIST * getWindow(1).getUppX(), true);
					if(getDimList().count() > 0 && !PPCALCOBJECT->IsPartProgramRunning() && !EditmeasurementMode() && currentHandler->getClicksDone() == 0)
						nmeasure = HELPEROBJECT->getNearestmeasure(getDimList(), CurrentMousePosX, CurrentMousePosY, 1);
				
					if(ShowHideFGPoints() && nshape != NULL)
					{
						if(nshape->selected())
							nshape = NULL;
					}
					if(nshape != NULL)
					{
						nearestShape = nshape;
						if(!ShapeHighlightedForImplicitSnap())
						{
							nearestShape->HighlightedFormouse(true);
							if(nearestShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
							{
								if(((Circle*)nearestShape)->CircleType == RapidEnums::CIRCLETYPE::PCDCIRCLE)
								{
									for each(Shape* Csh in ((PCDCircle*)nearestShape)->PCDParentCollection)
										Csh->HighlightedForPCDHighLight(true);
								}
							}
							CurrentShapeName = (const char*)nearestShape->getModifiedName();
							VShape_Updated = true; RShape_Updated = true;
							this->ShapeHighlighted(true);
						}
					}
					else if(this->UpdateGrafixFor3DSnapPoint())
					{
					   this->UpdateGrafixFor3DSnapPoint(false);
					   RShape_Updated = true;
					}
					if(nmeasure != NULL)
					{
						nearestmeasure = nmeasure;
						//nearestmeasure->HighlightedForMouseMove(true);
						if(!nearestmeasure->selected())
							selectMeasurement(nearestmeasure->getId(), false, false);
						VMeasurement_Updated = true;
						RMeasurement_Updated = true;
						this->MeasurementHighlighted(true);
					}
					bool Clicked = false;
					if(!MeasurementMode() && ShapeHighlighted() && !(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::RCADZOOMIN || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::RCADZOOMOUT) && (VideoMpointType == RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS || VideoMpointType == RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL || Derivedmodeflag))
					{
						SnapPoint* Snp = NULL;
						switch(nearestShape->ShapeType)
						{
							case RapidEnums::SHAPETYPE::CIRCLE:
							case RapidEnums::SHAPETYPE::CIRCLE3D:
							case RapidEnums::SHAPETYPE::ARC:
								Snp = ((ShapeWithList*)nearestShape)->SnapPointList->getList()[3];
								getCurrentUCS().getIPManager().SetParents(Snp->getParent(), NULL);
								currentHandler->mouseMove_x(Snp->getPt()->getX(), Snp->getPt()->getY(),Snp->getPt()->getZ(), true);
								Clicked = true;
								break;
						}
					}
					if(CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER && !Clicked)
					{
						if(SnapPointHighlighted() && CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::TRIMMING_HANDLER)
							currentHandler->mouseMove_x(lastsnap->getX(), lastsnap->getY());
						else if(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::SELECTION_HANDLER)
							currentHandler->mouseMove_x(getWindow(1).getPointer(true).x, getWindow(1).getPointer(true).y);
						else
						{
							if(highlightedShape() != NULL)
							{
								this->DrawSnapPointOnHighlightedShape(GetPointOnHighlightedShape(CurrentMousePosX, CurrentMousePosY));
								currentHandler->mouseMove_x(SnapPointModePoint.getX(), SnapPointModePoint.getY());
							}
							else
								currentHandler->mouseMove_x(CurrentMousePosX, CurrentMousePosY);
							/*this->DrawSnapPointOnHighlightedShape(GetPointOnHighlightedShape(CurrentMousePosX, CurrentMousePosY));
							currentHandler->mouseMove_x(CurrentMousePosX, CurrentMousePosY);	*/
						}
					}
				}
			}
			//else
				//currentHandler->mouseMove_x(CurrentMousePosX, CurrentMousePosY);
			update_VideoGraphics();
			update_RcadGraphics();
		}
	}
	catch(...){ RCADMouseFlag = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1101", __FILE__, __FUNCSIG__); }
	//MouseIsMoving_RCad = false;
}

//Rcad mouse scroll..
void RCadApp::OnMouseWheel_Rcad(int evalue)
{
	try
	{
		if((getCurrentHandler()->CurrentWindow == 1) && getCurrentHandler()->getClicksDone() != 0)
		{
			currentHandler->MouseScroll(evalue > 0 ? true : false);
			update_VideoGraphics();
			update_RcadGraphics();
		}
		else
		{
			Wait_RcadGraphicsUpdate();
			currentwindow = 1;
			getWindow(1).changeZoom(evalue > 0 ? false : true);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1102", __FILE__, __FUNCSIG__); }
}

//RCad Mouse enter..
void RCadApp::OnMouseEnter_Rcad()
{
	try
	{
		currentwindow = 1;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1103", __FILE__, __FUNCSIG__); }
}

//Rcad mouse leave..
void RCadApp::OnMouseLeave_Rcad()
{
	try
	{
		Left_Mouse_Down=false;
		Right_Mouse_Down=false;	
		if(getWindow(1).GetpanMode())
		{
			getWindow(1).togglePan();
			if(getWindow(1).PanMode)
				SetRcadCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_PANRELEASE);
			else if(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::TRIMMING_HANDLER)
				SetRcadCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_TRIM);
			else
				SetRcadCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1104", __FILE__, __FUNCSIG__); }
}

//Dxf left mouse down..
void RCadApp::OnLeftMouseDown_Dxf(double x, double y)
{
	try
	{
		currentwindow = 2;
		getWindow(2).mouseDown(x, y);
		setCurrentHighlitedCell(getWindow(2).getPointer().x,getWindow(2).getPointer().y);
		if(getWindow(2).PanMode)
		{
			getWindow(2).togglePan();
			if(getWindow(2).GetpanMode())
				SetDxfCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_PANHOLD);
			else
				SetDxfCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
		}
		if(ControlOn && DXFSnapPointHighlighted())
		{
			double p1[2] = {DXFlastsnap->getX(), DXFlastsnap->getY()},
				p2[2] = {getCurrentDRO().getX(), getCurrentDRO().getY()}, transform[9];
			RMATH2DOBJECT->TransformM_OnePtAlign(&p1[0], &p2[0], &transform[0]);
			DXFEXPOSEOBJECT->CADalignment(&transform[0]);
		}
		else if(Derivedmodeflag || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::DXFAREA_HANDLER || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::MOVE_HANDLER ||
			CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::DXFZOOMIN || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::DXFZOOMOUT || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_1PT
			 || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_1LINE3D || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_1PT1LINE || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_1PT1LINE3D ||
			 CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_2LINE || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_2PT)
		{
			if(DXFSnapPointHighlighted())
				currentHandler->LmouseDown_x(DXFlastsnap->getX(), DXFlastsnap->getY());
			else
				currentHandler->LmouseDown_x(getWindow(2).getPointer().x, getWindow(2).getPointer().y);
		}
		else if (CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::MEASURE_HANDLER)
		{
			DimensionHandler* thisHandler = (DimensionHandler*)currentHandler;
			//if (thisHandler->CurrentMeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_PROFILE)
			//{
			//	thisHandler->
			//}

		}
		if(GridCellHighlighted() && ControlOn)
		{
			GridCellShape->selected(!GridCellShape->selected());
			MyGridModule->AddRemoveToselectedItems(GridCellShape, getCurrentDRO().getX(), getCurrentDRO().getY(), GridCellShape->selected());
			GridmoduleUpdated = true;
			ControlOn=false;
			update_DxfGraphics();
		}
		else if(GridCellHighlighted())
		{
			if(GridCellShape==NULL) return;
			if(GridCellShape->selected()) return;
			MyGridModule->ClearSelection();
			GridCellShape->selected(true);
			GridCellShape->IncrementVcount();
			MyGridModule->SetSelectedRect(GridCellShape);
			MyGridModule->SetCurrentIterator(GridCellShape);
			GridmoduleUpdated = true;
			update_DxfGraphics();
			Sleep(200);
			double GotoPos[3] = {*MyGridModule->getCurrentSelecteCell()->GetCenter(), *(MyGridModule->getCurrentSelecteCell()->GetCenter() + 1), currentDRO.getZ()};
			MoveMachineToRequiredPos(GotoPos[0], GotoPos[1], GotoPos[2], true, RapidEnums::MACHINEGOTOTYPE::SAOICELLGOTO);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1105", __FILE__, __FUNCSIG__); }
}

//Dxf right mouse down..
void RCadApp::OnRightMouseDown_Dxf(double x, double y)
{
	try
	{
		currentwindow = 2;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1106", __FILE__, __FUNCSIG__); }
}

//Dxf right mouse down..
void RCadApp::OnMiddleMouseDown_Dxf(double x, double y)
{
	try
	{
		currentwindow = 2;
		getWindow(2).togglePan();
		SetDxfCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_PANRELEASE);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1107", __FILE__, __FUNCSIG__); }
}

//Dxf left mouse up..
void RCadApp::OnLeftMouseUp_Dxf(double x, double y)
{
	try
	{
		currentwindow = 2;
		if(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::DXFZOOMIN || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::DXFZOOMOUT)
			getWindow(2).mouseDown(x,y);
		if(getWindow(2).PanMode && getWindow(2).GetpanMode())
		{
			getWindow(2).togglePan();
			SetDxfCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_PANRELEASE);
		}
		currentHandler->LmouseUp(getWindow(2).getPointer().x, getWindow(2).getPointer().y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1108", __FILE__, __FUNCSIG__); }
}

//Dxf right mouse up..
void RCadApp::OnRightMouseUp_Dxf(double x, double y)
{
	try
	{
		currentwindow = 2;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1109", __FILE__, __FUNCSIG__); }
}

//Dxf middle mouse up..
void RCadApp::OnMiddleMouseUp_Dxf(double x, double y)
{
	try
	{
		currentwindow = 2;
		if(getWindow(2).GetpanMode())
		{
			getWindow(2).togglePan();
			if(getWindow(2).PanMode)
				SetDxfCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_PANRELEASE);
			else
				SetDxfCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1110", __FILE__, __FUNCSIG__); }
}

//Dxf mouse move..
void RCadApp::OnMouseMove_Dxf(double x, double y)
{
	try
	{
		currentwindow = 2;
		getWindow(2).mouseMove(x, y);
		double CurrentMousePosX = getWindow(2).getPointer().x;
		double CurrentMousePosY = getWindow(2).getPointer().y;
		if(getWindow(2).GetpanMode())
		{
			update_DxfGraphics(false);
		}
		else
		{
			this->SnapPointHighlighted(false); 
			this->ShapeHighlighted(false); 
			this->MeasurementHighlighted(false); 
			this->DXFShapeHighlighted(false);
			this->ShapeHighlightedForImplicitSnap(false);
			this->DXFlastsnap = NULL; 
			
			//GridCellHighlighted = false;
			//GRectangle* nshape1 = NULL;
			//nshape1 = MyGridModule->getCurrentHighlightedCell(CurrentMousePosX, CurrentMousePosY);
			//if(nshape1!=NULL)
			//{
				//GridCellHighlighted = true;
				//GridCellShape = nshape1;
			//}

			this->DXFSnapPointHighlighted(false); this->DXFShapeHighlighted(false);
			if(DXFnearestShape != NULL)
			{
				this->DXFnearestShape->HighlightedFormouse(false);
				this->DXFnearestShape = NULL;
				this->RDXFShape_updated = true;
			}
			Vector* vx = NULL;
			if(DXFEXPOSEOBJECT->CurrentDXFShapeList.size() > 0)
				vx = this->getCurrentUCS().getIPManager().getNearestSnapPoint(DXFEXPOSEOBJECT->CurrentDXFShapeList, CurrentMousePosX, CurrentMousePosY, getWindow(2).getUppX() * SNAPDIST);
			if(vx != NULL)
			{
				this->DXFSnapPointHighlighted(true);
				DXFlastsnap = vx;
				currentHandler->mouseMove_x(vx->getX(), vx->getY());
			}
			else
			{			
				Shape* nshape;
				nshape = HELPEROBJECT->getNearestShape(DXFEXPOSEOBJECT->CurrentDXFShapeList, CurrentMousePosX, CurrentMousePosY, SNAPDIST * getWindow(2).getUppX());
				if(nshape != NULL)
				{
					this->DXFnearestShape = nshape;
					this->DXFnearestShape->HighlightedFormouse(true);
					this->RDXFShape_updated = true;
					this->DXFShapeHighlighted(true);
				}
				if(Derivedmodeflag || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_2LINE || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_2PT || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_1PT || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_1PT1LINE
					&& DXFShapeHighlighted() && !(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::DXFZOOMIN || CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::DXFZOOMOUT) && (VideoMpointType == RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS || VideoMpointType == RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL))
				{
					if(DXFnearestShape != NULL)
					{
						SnapPoint* Snp;
						switch(DXFnearestShape->ShapeType)
						{
							case RapidEnums::SHAPETYPE::CIRCLE:
							case RapidEnums::SHAPETYPE::ARC:
								this->DXFSnapPointHighlighted(true);
								DXFlastsnap = ((Circle*)DXFnearestShape)->getCenter();
								currentHandler->mouseMove_x(DXFlastsnap->getX(), DXFlastsnap->getY());
								update_DxfGraphics();
								return;
								break;
						}
					}
				}
				if(CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER)
					currentHandler->mouseMove_x(CurrentMousePosX, CurrentMousePosY);
			}
			update_DxfGraphics(false);
		}
	}
	catch(...)
	{
		DXFMouseFlag = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1111", __FILE__, __FUNCSIG__);
	}
}

//Dxf mouse scroll..
void RCadApp::OnMouseWheel_Dxf(int evalue)
{
	try
	{
		Wait_DxfGraphicsUpdate();
		currentwindow = 2;
		getWindow(2).changeZoom(evalue>0?false:true);
		//update_DxfGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1112", __FILE__, __FUNCSIG__); }
}

//Dxf mouse enter.,
void RCadApp::OnMouseEnter_Dxf()
{
	try
	{
		currentwindow = 2;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1113", __FILE__, __FUNCSIG__); }
}

//Dxf mouse leave..
void RCadApp::OnMouseLeave_Dxf()
{
	try
	{
		if(getWindow(2).GetpanMode())
		{
			getWindow(2).togglePan();
			if(getWindow(2).PanMode)
				SetDxfCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_PANRELEASE);
			else
				SetDxfCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1114", __FILE__, __FUNCSIG__); }
}

Shape* RCadApp::AddShapeForRightClick(bool SelectShape)
{
	try
	{		//if(getShapesList().getList().size() < FundamentalShapes()) return NULL;
		RC_ITER i = getShapesList().getList().end();
		i--;
		Shape* sh = dynamic_cast<Shape*>((*i).second);
		/*if(static_cast<Line*>((*i).second))
		{
			bool checkflg = true;
		}
		if(static_cast<Circle*>((*i).second))
		{
			bool checkflg = true;
		}*/
		Shape* Nsh = NULL;
		switch (sh->ShapeType)
		{

		case RapidEnums::SHAPETYPE::XRAY:
			Nsh = new Line(_T("R"), RapidEnums::SHAPETYPE::XRAY);
			((Line*)Nsh)->LineType = RapidEnums::LINETYPE::RAYLINE;
			AddShapeAction::addShape(Nsh, SelectShape);
			break;

		case RapidEnums::SHAPETYPE::XLINE:
			Nsh = new Line(_T("XL"), RapidEnums::SHAPETYPE::XLINE);
			((Line*)Nsh)->LineType = RapidEnums::LINETYPE::INFINITELINE;
			AddShapeAction::addShape(Nsh, SelectShape);
			break;

		case RapidEnums::SHAPETYPE::LINE:
			Nsh = new Line(_T("L"), RapidEnums::SHAPETYPE::LINE);
			((Line*)Nsh)->LineType = RapidEnums::LINETYPE::FINITELINE;
			AddShapeAction::addShape(Nsh, SelectShape);
			break;

		case RapidEnums::SHAPETYPE::CIRCLE:
			Nsh = new Circle(_T("C"), RapidEnums::SHAPETYPE::CIRCLE);
			((Circle*)Nsh)->CircleType = ((Circle*)sh)->CircleType;
			AddShapeAction::addShape(Nsh, SelectShape);
			break;
		case RapidEnums::SHAPETYPE::ARC:
			Nsh = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
			((Circle*)Nsh)->CircleType = ((Circle*)sh)->CircleType;
			AddShapeAction::addShape(Nsh, SelectShape);
			break;
		case RapidEnums::SHAPETYPE::RPOINT:
			Nsh = new RPoint(_T("P"), RapidEnums::SHAPETYPE::RPOINT);
			((RPoint*)Nsh)->PointType = ((RPoint*)sh)->PointType;
			AddShapeAction::addShape(Nsh, SelectShape);
			break;
		case RapidEnums::SHAPETYPE::RPOINT3D:
			Nsh = new RPoint(_T("P3D"), RapidEnums::SHAPETYPE::RPOINT3D);
			((RPoint*)Nsh)->PointType = ((RPoint*)sh)->PointType;
			AddShapeAction::addShape(Nsh, SelectShape);
			break;
		case RapidEnums::SHAPETYPE::LINE3D:
			Nsh = new Line(_T("L3D"), RapidEnums::SHAPETYPE::LINE3D);
			((Line*)Nsh)->LineType = RapidEnums::LINETYPE::FINITELINE3D;
			AddShapeAction::addShape(Nsh, SelectShape);
			break;
		case RapidEnums::SHAPETYPE::PLANE:
			Nsh = new Plane();
			AddShapeAction::addShape(Nsh, SelectShape);
			break;
		case RapidEnums::SHAPETYPE::SPHERE:
			Nsh = new Sphere();
			AddShapeAction::addShape(Nsh, SelectShape);
			break;
		case RapidEnums::SHAPETYPE::CYLINDER:
			Nsh = new Cylinder();
			AddShapeAction::addShape(Nsh, SelectShape);
			break;
			//Added by Shweytank Mishra
		case RapidEnums::SHAPETYPE::CIRCLE3D:
			Nsh = new Circle(_T("C"), RapidEnums::SHAPETYPE::CIRCLE3D);
			AddShapeAction::addShape(Nsh, SelectShape);
			break;
		case RapidEnums::SHAPETYPE::ELLIPSE3D:
			Nsh = new Conics3D(_T("EL"), RapidEnums::SHAPETYPE::ELLIPSE3D);
			AddShapeAction::addShape(Nsh, SelectShape);
			break;
		case RapidEnums::SHAPETYPE::PARABOLA3D:
			Nsh = new Conics3D(_T("PB"), RapidEnums::SHAPETYPE::PARABOLA3D);
			AddShapeAction::addShape(Nsh, SelectShape);
			break;
		case RapidEnums::SHAPETYPE::HYPERBOLA3D:
			Nsh = new Conics3D(_T("HB"), RapidEnums::SHAPETYPE::HYPERBOLA3D);
			AddShapeAction::addShape(Nsh, SelectShape);
			break;
		case RapidEnums::SHAPETYPE::CLOUDPOINTS:
			Nsh = new CloudPoints();
			AddShapeAction::addShape(Nsh, SelectShape);
			break;
		case RapidEnums::SHAPETYPE::SPLINE:
			Nsh = new Spline();
			AddShapeAction::addShape(Nsh, SelectShape);
			break;
		}
		return Nsh;
	}
	catch (...)
	{
		 MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1115", __FILE__, __FUNCSIG__); 
	}
}

//Handle the keydown event handling...//
void RCadApp::keyDown(UINT nChar,UINT nRepCnt, UINT nFlags, int window)
{
	try
	{
		if(nChar == 131089)
			ControlOn = true;
		else if(nChar == 65552)
			ShiftON = true;
		if(currentHandler != NULL && !(dynamic_cast<DefaultHandler*>(currentHandler)))
			currentHandler->keyDown(nChar, nRepCnt, nFlags);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1116", __FILE__, __FUNCSIG__); }
}

//Handle the key upevent...//
void RCadApp::keyUp(UINT nChar,UINT nRepCnt, UINT nFlags, int window)
{
	try
	{
		 if(currentHandler != NULL && !(dynamic_cast<DefaultHandler*>(currentHandler)))
			currentHandler->keyUp(nChar, nRepCnt, nFlags);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1117", __FILE__, __FUNCSIG__); }
}

void RCadApp::SetGenericProbeOffset(int id, double offsetx, double offsety, double offsetz, double radius)
{
	try
	{
		if(GenericProbeOffsetCollection.find(id) == GenericProbeOffsetCollection.end())
		{
		   GenericProbeOffsetCollection[id] = new GenericProbeOffset();
		}
		GenericProbeOffsetCollection[id]->offsetX = offsetx;
		GenericProbeOffsetCollection[id]->offsetY = offsety;
		GenericProbeOffsetCollection[id]->offsetZ = offsetz;
		GenericProbeOffsetCollection[id]->radius = radius;
	}
	catch(...)
	{
	   MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1118", __FILE__, __FUNCSIG__);
	}
}

double RCadApp::GetGenericProbeRadius(int id)
{
	try
	{
		if(GenericProbeOffsetCollection.find(id) == GenericProbeOffsetCollection.end())
		{
			return 0;
		}
		else
		{
			return GenericProbeOffsetCollection[id]->radius;
		}
	}
	catch(...)
	{
	   MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1119", __FILE__, __FUNCSIG__);
	}
}

void RCadApp::GetGenericProbeOffset(int id, double *offsetx, double *offsety, double *offsetz)
{
	try
	{
		if(GenericProbeOffsetCollection.find(id) == GenericProbeOffsetCollection.end())
		{
			*offsetx = 0;
			*offsety = 0;
			*offsetz = 0;
		}
		else
		{
			*offsetx = GenericProbeOffsetCollection[id]->offsetX;
			*offsety = GenericProbeOffsetCollection[id]->offsetY;
			*offsetz = GenericProbeOffsetCollection[id]->offsetZ;
		}
	}
	catch(...)
	{
	   MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1120", __FILE__, __FUNCSIG__);
	}
}

void RCadApp::RotateRcadFor3DMode()
{
	try
	{
		ChangeRcadViewOrien = true;
		double rotationmatrix1[9] = {0}, rotationmatrix2[9] = {0}, axisdir1[3] = {0, 0, 1};
		RMATH3DOBJECT->RotationAround3DAxisThroughOrigin(-M_PI / 6, axisdir1, rotationmatrix1);
		RMATH3DOBJECT->RotationAround3DAxisThroughOrigin(-M_PI / 6, rotationmatrix1, rotationmatrix2);
		RcadTransformationMatrix[0] = rotationmatrix2[0]; RcadTransformationMatrix[1] = rotationmatrix2[1]; RcadTransformationMatrix[2] = rotationmatrix2[2]; RcadTransformationMatrix[3] = 0;
		RcadTransformationMatrix[4] = rotationmatrix2[3]; RcadTransformationMatrix[5] = rotationmatrix2[4]; RcadTransformationMatrix[6] = rotationmatrix2[5]; RcadTransformationMatrix[7] = 0;
		RcadTransformationMatrix[8] = rotationmatrix2[6]; RcadTransformationMatrix[9] = rotationmatrix2[7]; RcadTransformationMatrix[10] = rotationmatrix2[8]; RcadTransformationMatrix[11] = 0;
		RcadTransformationMatrix[12] = 0; RcadTransformationMatrix[13] = 0; RcadTransformationMatrix[14] = 0; RcadTransformationMatrix[15] = 1;
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->update_RcadGraphics();
		Sleep(10);
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		MAINDllOBJECT->UpdateShapesChanged();
		Sleep(40);
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		Sleep(20);
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		/*getWindow(1).SetMousemoveFor3Dmode();
		getWindow(1).RotateGraphics(Rcad_Graphics);*/
		getWindow(1).ZoomToExtents(1);
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1121", __FILE__, __FUNCSIG__);
	}
}

void RCadApp::TakeFrameGrabForSphere()
{
	if(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::SPHEREFRAMEGRABHANDLER)
	{
	  ((SphereFrameGrabHandler*)getCurrentHandler())->get_FramGrabParameter();
	}
}

//Edge detection instance..
EdgeDetectionSingleChannel* RCadApp::getEdgeDetection()
{
	return this->Edgedetection;
}

//Helper class instance..
HelperClass* RCadApp::getHelper()
{
	return HelperInst;
}

//Calibration calc instance..
CalibrationCalculation* RCadApp::getcbcInst()
{
	return cbcInstance;
}

//Thread calc instance..
ThreadCalcFunctions* RCadApp::gettcfnInst()
{
	return thcfInstance;
}

//Expose Focus Calc Instance..
FocusFunctions* RCadApp::getffnInst()
{
	return ffinstance;
}

//Expose partprogram Instance..
PartProgramFunctions* RCadApp::getppnInst()
{
	return ppinstance;
}

//Expose specal tool handler.
FixedToolCollection* RCadApp::getSpecialToolHandler()
{
	return FixedToolHandler;
}

//Dxf expose class instance..
DXFExpose* RCadApp::getDXFexobj()
{
	return DxfexposeObj;
}

//Best fit Instance...//
RBF* RCadApp::getBestfit()
{
	return bestFit;
}

CalibrationText* RCadApp::getctInst()
{
	return CalibTextIns;
}

//get the actionhistory collection....//
RCollectionBase& RCadApp::getActionsHistoryList()
{
	return actionhistory;
}

//Get the ucs collection list...//
RCollectionBase& RCadApp::getUCSList()
{
	return this->ucslist;
}

//get the collection of shapes in Current UCS...//
RCollectionBase& RCadApp::getShapesList()
{
	return getCurrentUCS().getShapes();
}

//Expose all Shape list of current UCS during pp to path.
RCollectionBase& RCadApp::getPPShapesList_Path()
{
	return getCurrentUCS().getPPShapes_Path();
}

RCollectionBase& RCadApp::getPPMeasuresList_Path()
{
	return getCurrentUCS().getPPMeasures_Path();
}

//Get the collection measurements in Currrent UCS...//
RCollectionBase& RCadApp::getDimList()
{
	return getCurrentUCS().getDimensions();
} 

//Get all the dxf list..//
RCollectionBase& RCadApp::getDXFList()
{
	return getCurrentUCS().getDxfCollecion();
}

//Get the collection of the selected shape list..//
RCollectionBase& RCadApp::getSelectedShapesList()
{
	return getCurrentUCS().getShapes().getSelected();
}

//Collection of seleceted measurement list.../
RCollectionBase& RCadApp::getSelectedDimList()
{
	return getCurrentUCS().getDimensions().getSelected();
}

//Get Selected dxf list..//
RCollectionBase& RCadApp::getSelectedDXFList()
{
	return getCurrentUCS().getDxfCollecion().getSelected();
}

//Returns the Current Selected UCS..//
UCS& RCadApp::getCurrentUCS()
{
	return (*((UCS*)(this->getUCSList().selectedItem())) );
}

//Get the window according to the number...
RWindow& RCadApp::getWindow(int windowno = 0)
{
	return getCurrentUCS().getWindow(windowno);
}

//get the UCS instance for the given id.....//
UCS* RCadApp::getUCS(int id)
{
	return (UCS*)getUCSList().getList()[id];
}

//Change the current handler...//
void RCadApp::changeHandler(RapidEnums::RAPIDHANDLERTYPE HANDLER, bool CalledInternally)
{
	try
	{
		if(SmartMeasureType != RapidEnums::SMARTMEASUREMENTMODE::DEFAULTTYPE)
		{
			if(currentHandler->CurrentdrawHandler != NULL)
			{
				if(currentHandler->CurrentdrawHandler->getClicksDone() != 0)
				{
					MAINDllOBJECT->SetStatusCode("RCadApp02");
					CheckSmartMeasurementButton();
					return;
				}
			}
			if(!MAINDllOBJECT->ProbeSmartMeasureflag)
			{
				if(currentHandler->FinishedCurrentDrawing)
				{
					SmartMeasureTypeMode(false);
					SetMeasurementType(RapidEnums::SMARTMEASUREMENTMODE::DEFAULTTYPE);
					goto cntul;
					return;

				}
				else
				{
					if(SmartMeasureType == RapidEnums::SMARTMEASUREMENTMODE::RECANGLEMEASUREMENT)
					{
						CheckSmartMeasurementButton();
						MAINDllOBJECT->SetStatusCode("RCadApp03");
						return;
					}
					if(HANDLER == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR || HANDLER == RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB || 
									HANDLER == RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB || HANDLER == RapidEnums::RAPIDHANDLERTYPE::CIRCLE_FRAMEGRAB || HANDLER == RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB ||
									HANDLER == RapidEnums::RAPIDHANDLERTYPE::FIXEDRECT_FRAMEGRAB || HANDLER == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARC_FRAMEGRAB || HANDLER == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARECT_FRAMEGRAB
									|| HANDLER == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_CIRCLE_FRAMEGRAB || HANDLER == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_RECT_FRAMEGRAB
									 || HANDLER == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_FIXEDRECT_FRAMEGRAB)
					{
						this->currentHandler->ChangeHanlder_Type(HANDLER);
						CheckSmartMeasurementButton();
						return;
					}
					else
					{
						MAINDllOBJECT->SetStatusCode("RCadApp02");
						CheckSmartMeasurementButton();
						return;
					}
				}
			}
						
		}
cntul:
		this->MeasurementMode(false);
		SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
		SetRcadCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
		CURRENTRAPIDHANDLER = RapidEnums::RAPIDDRAWHANDLERTYPE::RAPIDDEFAULT_HANLDER;
		Derivedmodeflag = false;
		if(HANDLER != PREVIOUSHANDLE)
			SetCrossOnVideo(false);
		CURRENTHANDLE = HANDLER;
		if(SetCrossOnVideo())
		{
			SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
			setHandler(new DefaultHandler());
			SetCrossOnVideo(false);
			return;
		}
		SetCrossOnVideo(true);
		switch(HANDLER)
		{
			case RapidEnums::RAPIDHANDLERTYPE::AUTOFOCUSSCANHANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new AutoFocusScanHandler());
				break;
			case RapidEnums::RAPIDHANDLERTYPE::AUTOCONTOURSCANHANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new AutoContourScanHandler());
				break;
			case RapidEnums::RAPIDHANDLERTYPE::POINT_HANDLER:
				if(DefaultCrosshairMode())
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				else
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new PointHandler(true));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::POINT_HANDLER3D:
				if(DefaultCrosshairMode())
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				else
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new PointHandler(false));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::TRIMMING_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_TRIM);
				SetRcadCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_TRIM);
				setHandler(new TrimmingHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::XLINE_HANDLER:
				if(DefaultCrosshairMode())
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				else
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new LineHandlers(RapidEnums::LINETYPE::INFINITELINE));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::XRAY_HANDLER:
				if(DefaultCrosshairMode())
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				else
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new LineHandlers(RapidEnums::LINETYPE::RAYLINE));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER:
				if(DefaultCrosshairMode())
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				else
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new LineHandlers(RapidEnums::LINETYPE::FINITELINE));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::SPLINE_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new SplineHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::TWOARC_HANDLER:
				if(DefaultCrosshairMode())
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				else
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new GroupShapeHandler(RapidEnums::SHAPETYPE::TWOARC));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::TWOLINE_HANDLER:
				if(DefaultCrosshairMode())
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				else
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new GroupShapeHandler(RapidEnums::SHAPETYPE::TWOLINE));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::LINE3D_HANDLER:
				if(DefaultCrosshairMode())
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				else
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new LineHandlers(RapidEnums::LINETYPE::FINITELINE3D));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CIRCLE3P_HANDLER:
				if(DefaultCrosshairMode())
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				else
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new CircleHandlers(RapidEnums::CIRCLETYPE::TRHEEPTCIRCLE));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CIRCLE1P_HANDLER:
				if(DefaultCrosshairMode())
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				else
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new CircleHandlers(RapidEnums::CIRCLETYPE::CIRCLE_WITHCENTER));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PERIMETER_HANDLER:
				if(DefaultCrosshairMode())
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				else
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new PerimeterHandler());			
				break;

			case RapidEnums::RAPIDHANDLERTYPE::FOCUSONRIGHTCLICKHANDLER:
				if(DefaultCrosshairMode())
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				else
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new FocusOnRightClickHandler());			
				break;
			
			case RapidEnums::RAPIDHANDLERTYPE::FOCUS_DEPTHTWOBOXHANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new FocusDepthMultiBoxHandler(true));
				FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::TWOBOXDEPTH_MODE;
				break;

			case RapidEnums::RAPIDHANDLERTYPE::FOCUS_DEPTHFOURBOXHANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new FocusDepthMultiBoxHandler(false));
				FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::FOURBOXDEPTH_MODE;
				break;

			case RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER:
				if(DefaultCrosshairMode())
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				else
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new ArcHandler(RapidEnums::CIRCLETYPE::ARC_MIDDLE));
				break;
			
			case RapidEnums::RAPIDHANDLERTYPE::SELECTION_HANDLER:
				if(DefaultCrosshairMode())
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				else
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new SelectionHandler());
				break;
			case RapidEnums::RAPIDHANDLERTYPE::CONTOURSCAN_FGHANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new ContourScan_FG());
				break;
			case RapidEnums::RAPIDHANDLERTYPE::IMAGEANALYSIS_FGHANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new ImageAnalysisFrameGrabeHander());
				break;
			case RapidEnums::RAPIDHANDLERTYPE::RCADZOOMIN:
				CURRENTRAPIDHANDLER = RapidEnums::RAPIDDRAWHANDLERTYPE::RCADZOOMINOUT_HANDLER;
				//SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				//Derivedmodeflag = true;
				setHandler(new ZoomInOutHandler(true));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::RCADZOOMOUT:
				CURRENTRAPIDHANDLER = RapidEnums::RAPIDDRAWHANDLERTYPE::RCADZOOMINOUT_HANDLER;
				//SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				//Derivedmodeflag = true;
				setHandler(new ZoomInOutHandler(false));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::DXFZOOMIN:
				CURRENTRAPIDHANDLER = RapidEnums::RAPIDDRAWHANDLERTYPE::DXFZOOMINOUT_HANDLER;
				//SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				//Derivedmodeflag = true;
				setHandler(new ZoomInOutHandler(true));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::DXFZOOMOUT:
				CURRENTRAPIDHANDLER = RapidEnums::RAPIDDRAWHANDLERTYPE::DXFZOOMINOUT_HANDLER;
				//SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				//Derivedmodeflag = true;
				setHandler(new ZoomInOutHandler(false));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::MOVE_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_PANRELEASE);
				setHandler(new MoveShapeHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::MEASURE_HANDLER:
				MeasurementMode(true);
				setHandler(new DimensionHandler());
				CurrentPCDShape = NULL;
				DeselectAll(false);
				break;

			case RapidEnums::RAPIDHANDLERTYPE::ROTARYMEASURE_HANDLER:
				MeasurementMode(true);
				setHandler(new RotaryDimensionHandler());
				DeselectAll(false);
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CLOUDPOINTMEASUREMENTHANDLER:
				MeasurementMode(true);
				setHandler(new CloudPointMeasurementsHandler());
			break;

			case RapidEnums::RAPIDHANDLERTYPE::DMM_HANDLER:
				MeasurementMode(true);
				if(DefaultCrosshairMode())
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				else
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new DMMHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::FASTRACE_HANDLER:
				if(DefaultCrosshairMode())
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				else
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new FTHandler(false));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::FASTRACE_HANDLER_ENDPT:
				if(DefaultCrosshairMode())
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				else
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new FTHandler(true));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new FlexibleCrossHairHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::SCAN_CROSSHAIR:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new ScanModeHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::LIVESCAN_CROSSHAIR:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new LiveScanModeHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::SELECTPOINT_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new SelectPointHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CIRCLE_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CIRCLEFG);
				setHandler(new FrameGrabCircle());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ANGRECTFG);
				setHandler(new FrameGrabAngularRect());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::FIXEDRECT_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
				setHandler(new FrameGrabFixedRectangle());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ARCFG);
				setHandler(new FrameGrabArc());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
				setHandler(new FrameGrabRectangle(false));
				break;
			
			case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_CIRCLE_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CIRCLEFG);
				setHandler(new RotationScanHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARC_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ARCFG);
				setHandler(new RotationScanHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARECT_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
				setHandler(new RotationScanHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_RECT_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
				setHandler(new RotationScanHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_FIXEDRECT_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
				setHandler(new RotationScanHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB_ALLEDGE:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
				setHandler(new FrameGrabRectangle(true));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::MOUSECLICK_GRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				setHandler(new FrameGrabEdgeMouseClick());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::DXFAREA_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new DXFAreaHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CAD_1PT_EDITALIGN_HANDLER:
				DXFEXPOSEOBJECT->editDXF();
				setHandler(new EditCad1PtHandler());
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CAD_2PT_EDITALIGN_HANDLER:
				DXFEXPOSEOBJECT->editDXF();
				setHandler(new EditCad2PtHandler());
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				break;			

			case RapidEnums::RAPIDHANDLERTYPE::EDITCAD_2LINEHANDLER:
				DXFEXPOSEOBJECT->editDXF();
				setHandler(new EditCad2LineHandler());
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				break;

			case RapidEnums::RAPIDHANDLERTYPE::EDITCAD_1PT1LINE_HANDLER:
				DXFEXPOSEOBJECT->editDXF();
				setHandler(new EditCad1Pt1LineHandler());
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_1PT:
				setHandler(new Cad1PtHandler());
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				DeselectAll(false);
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PROGRAM_1PT_ALIGNMENT:
				setHandler(new Program1PtHandler());
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PROGRAM_1LN_1PTALIGNMENT:
				setHandler(new Program1Line1PtHandler());
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::PROGRAM_2PT_ALIGNMENT:
				setHandler(new Program2PtHandler());
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_2PT:
				 setHandler(new Cad2PtHandler());
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				DeselectAll(false);
				break;

			case RapidEnums::RAPIDHANDLERTYPE::COPYSHAPE_2PTALIGN:
				setHandler(new CopyShape2PtAlign());
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_1PT1LINE:
				setHandler(new Cad1Pt1LineHandler());
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				DeselectAll(false);
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_1PT1LINE3D:
				setHandler(new Cad1Pt1LineHandler3D());
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				DeselectAll(false);
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_3PT:
				setHandler(new Cad3PtHandler());
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				DeselectAll(false);
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_2LINE:
				setHandler(new Cad2LineHandler());
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				DeselectAll(false);
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_1LINE3D:
				setHandler(new Cad1LineHandler3D());
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				DeselectAll(false);
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PPALIGNMENT_1PT:
				setHandler(new PP1PtHandler());
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				break;
				
			case RapidEnums::RAPIDHANDLERTYPE::PPALIGNMENT_2PT:
				setHandler(new PP2PtHandler());
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PPALIGNMENT_1PT1LINE:
				setHandler(new PP1Pt1LineHandler());
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				break;
				
			case RapidEnums::RAPIDHANDLERTYPE::MIDPOINT:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new MidPointHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE:
				Derivedmodeflag = true;
				setHandler(new LineParallel2Line());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE_FG:
				Derivedmodeflag = true;
				setHandler(new LineParallel2Line(true));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PERPENDICULAR_LINE:
				Derivedmodeflag = true;
				setHandler(new  LinePerpendicular2Line());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE3D:
				Derivedmodeflag = true;
				setHandler(new LineParallelToLine3D());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PERPENDICULAR_LINE3D:
				Derivedmodeflag = true;
				setHandler(new  LinePerpendicularToLine3D());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PARALLEL_PLANE:
				Derivedmodeflag = true;
				setHandler(new PlaneParallel2Plane());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PERPENDICULARPLANE_ONLINE:
				Derivedmodeflag = true;
				setHandler(new PlanePerpendicular2LineHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PERPENDICULAR_PLANE:
				Derivedmodeflag = true;
				setHandler(new PlanePerpendicular2Plane());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::TANGENT_ON_CIRCLE:
				Derivedmodeflag = true;
				setHandler(new LineTangent2Circle());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PARALLEL_TANGENT:
				Derivedmodeflag = true;
				setHandler(new LineParallelTangent());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PERPENDICULAR_TANGENT:
				Derivedmodeflag = true;
				setHandler(new LinePerpendicularTangent());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::TANGENT_POINT_CIRCLE:
				Derivedmodeflag = true;
				setHandler(new LineTangentThruPoint());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::TANGENT_ON_2CIRCLE:
				Derivedmodeflag = true;
				setHandler(new LineTangentOn2Circles());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PARALLEL_TANGENT_2CIRCLE:
				Derivedmodeflag = true;
				setHandler(new LineParallelTangent2Circle());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::ANGLE_BISECTOR:
				Derivedmodeflag = true;
				setHandler(new LineAngleBisector());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PARALLELARC:
				Derivedmodeflag = true;
				setHandler(new ParallelArc());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::TANGENTIAL_CIRCLE:
				Derivedmodeflag = true;
				setHandler(new CircleTangent2TwoLine());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CIRCLETANTOCIRCLE:
				Derivedmodeflag = true;
				setHandler(new CircleTangent2Circle());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::LINEARCLINE_HANDLER:
				Derivedmodeflag = true;
				setHandler(new LineArcLineHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::ARC_TANTO_ARC_HANDLER:
				Derivedmodeflag = true;
				setHandler(new ArcArcHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::ARC_TANTO_LINE_HANDLER:
				Derivedmodeflag = true;
				setHandler(new ArcTanToLineHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CIRCLETANTO2CIRCLE:
				Derivedmodeflag = true;
				setHandler(new CircleTangent2TwoCircle1());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CIRCLETANTO2CIRCLE1:
				Derivedmodeflag = true;
				setHandler(new CircleTangent2TwoCircle2());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PLANEFROMTWOLINEHANDLER:
				Derivedmodeflag = true;
				setHandler(new PlaneThrough2Lines3D());
				break;
			case RapidEnums::RAPIDHANDLERTYPE::CIRCLETANTOLINECIRCLE:
				Derivedmodeflag = true;
				setHandler(new CircleTangent2LineCircle());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CIRCLE_TANTOTRIANGLEIN:
				Derivedmodeflag = true;
				setHandler(new CircleInsideATraingle(true));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CIRCLE_TANTOTRIANGLEOUT:
				Derivedmodeflag = true;
				setHandler(new CircleInsideATraingle(false));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::ONE_POINTANGLE_LINE:
				Derivedmodeflag = true;
				setHandler(new LineOnePointAngle());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::ONE_POINT_OFFSET_LINE:
				Derivedmodeflag = true;
				setHandler(new LineOnePointOffsetDist());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::POINTONCIRCLE:
				Derivedmodeflag = true;
				setHandler(new PointOnCircle());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::POINTONLINE:
				Derivedmodeflag = true;
				setHandler(new PointOnLine());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::FILLET_HANDLER:
				Derivedmodeflag = true;
				setHandler(new FilletHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PCD_HANDLER:
				Derivedmodeflag = true;
				setHandler(new PCDHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PCD_HANDLER_3D:
				Derivedmodeflag = true;
				setHandler(new PCDHandler3D());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::LINEPER2LINEONPLN_HANDLER:
				Derivedmodeflag = true;
				setHandler(new  LinePerpendicular2LineOnPlane());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PROBE_HANDLER:
				MAINDllOBJECT->ProbeSmartMeasureflag = false;
				setHandler(new ProbeHandler(true));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::REFLECTIONHANDLER:
				setHandler(new ReflectionHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::SPHEREFRAMEGRABHANDLER:
				setHandler(new SphereFrameGrabHandler());
				break;

		   case RapidEnums::RAPIDHANDLERTYPE::INTERSECTIONSHAPEHANDLER:
				setHandler(new IntersectionShapeHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::ROTATIONHANDLER:
				setHandler(new RotateShapeHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::TRANSLATIONHANDLER:
				setHandler(new TranslateShapeHandler(false));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::TWOPOINTALIGNMENTHANDLER:
				Derivedmodeflag = true;
				setHandler(new TwoPointAlignmentHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::TRANSLATIONHANDLERPOINTTOPOINT:
				setHandler(new TranslateShapeHandler(true));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::SHAPEPROJECTIONHANDLER:
				setHandler(new ProjectionHandler());				
				break;

			case RapidEnums::RAPIDHANDLERTYPE::SELECTED_TRANSFORM_HANDLER:
				setHandler(new AutoAlignHandler(true, 0));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::SELECTED_TRANSLATE_HANDLER:
				setHandler(new AutoAlignHandler(true, 1));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::SELECTED_ROTATE_HANDLER:
				setHandler(new AutoAlignHandler(true, 2));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::TRANSFORM_HANDLER:
				setHandler(new AutoAlignHandler(false, 0));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::TRANSLATE_HANDLER:
				setHandler(new AutoAlignHandler(false, 1));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::ROTATE_HANDLER:
				setHandler(new AutoAlignHandler(false, 2));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::DEVIATION_HANDLER:
				setHandler(new DeviationHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::DXF_DEVIATION_HANDLER:
				setHandler(new DXFDeviationHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CLOUDCOMPARISION_HANDLER:
				setHandler(new CloudComparisionHandler());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PROFILESCANHANDLER:
				setHandler(new ProfileScanHandler());
				CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PROFILESCANHANDLER;
				break;

			case RapidEnums::RAPIDHANDLERTYPE::RELATIVEPOINT_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::RELATIVEPOINT));				
				break;
			case RapidEnums::RAPIDHANDLERTYPE::CAMPROFILE_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::CAMPROFILE));				
				break;
			case RapidEnums::RAPIDHANDLERTYPE::PLANE_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::PLANE));				
				break;
			case RapidEnums::RAPIDHANDLERTYPE::CYLINDER_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::CYLINDER));				
				break;
			case RapidEnums::RAPIDHANDLERTYPE::CONE_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::CONE));				
				break;
			case RapidEnums::RAPIDHANDLERTYPE::TORUS_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::TORUS));				
				break;
			case RapidEnums::RAPIDHANDLERTYPE::CIRCLE3D_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::CIRCLE3D));				
				break;
			case RapidEnums::RAPIDHANDLERTYPE::ELLIPSE3D_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::ELLIPSE3D));				
				break;
			case RapidEnums::RAPIDHANDLERTYPE::PARABOLA3D_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::PARABOLA3D));				
				break;
			case RapidEnums::RAPIDHANDLERTYPE::HYPERBOLA3D_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::HYPERBOLA3D));				
				break;
			case RapidEnums::RAPIDHANDLERTYPE::BLACK_AREA_HANDLER:		
				AddShapeAction::addShape(new AreaShape(false));		
				break;
			case RapidEnums::RAPIDHANDLERTYPE::WHITE_AREA_HANDLER:
				AddShapeAction::addShape(new AreaShape(true));
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CIRCLEINVOLUTE_HANDLER:
				if(DefaultCrosshairMode())
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				else
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new CircleInvoluteHandler());
			break;

			case RapidEnums::RAPIDHANDLERTYPE::POINTFROMSHAPEHANDLER:
				{					
					setHandler(new PointFromShapeHandler());
					break;
				}

			case RapidEnums::RAPIDHANDLERTYPE::SPHERE_HANDLER:
				if(MasteProbeCalibrationMode())
				{
					if(getShapesList().count() != 3)
					{
						ShowMsgBoxString("RCadSetOtherhandlers01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
						return;
					}
					else
					{
						RC_ITER item = getShapesList().getList().begin();
						item++; item++;
						Shape* ThShape = (Shape*)((*item).second);
						if(ThShape->ShapeType == RapidEnums::SHAPETYPE::PLANE)
						{
							Shape* Csh = new Sphere();
							Csh->addParent(ThShape);
							ThShape->addChild(Csh);
							SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
							setHandler(new DummyHandler(Csh));						
						}
						else
						{
							ShowMsgBoxString("RCadSetOtherhandlers01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
							return;
						}
					}
				}
				else
				{
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
					setHandler(new DummyHandler(RapidEnums::SHAPETYPE::SPHERE));				
				}
				break;

			case RapidEnums::RAPIDHANDLERTYPE::DOTCOUNTS_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::DOTSCOUNT));				
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CLOUDPOINTS_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::CLOUDPOINTS));				
				break;

			case RapidEnums::RAPIDHANDLERTYPE::POLYLINECLOUDPOINTS:
				{
					Shape *csh = new CloudPoints();
					((CloudPoints*)csh)->CloudPointsForLine = true;
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
					setHandler(new DummyHandler(csh));					
				}
				break;
			default:
				setHandler(new DefaultHandler());
				CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER;
				break;
		}
		PREVIOUSHANDLE = HANDLER;
		if(CalledInternally)
			ClearAllRadioButton();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1122", __FILE__, __FUNCSIG__); }
}

//Set the current handler..//
void RCadApp::setHandler(MouseHandler* handler)
{
	try
	{
		dontUpdateGraphcis = true;
		Wait_VideoGraphicsUpdate();
		Wait_RcadGraphicsUpdate();
		Wait_DxfGraphicsUpdate();
		if(this->currentHandler != NULL)
		{
			delete this->currentHandler;
			this->currentHandler = NULL;
		}
		if(!PPCALCOBJECT->IsPartProgramRunning())
			FOCUSCALCOBJECT->ClrearAllFocus(false);
		this->currentHandler = handler;
		dontUpdateGraphcis = false;
	}
	catch(...)
	{
		dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1123", __FILE__, __FUNCSIG__);
	}
}

//Set other handlers...not of toggle type..
void RCadApp::SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE HANDLER)
{
	try
	{
		if(SmartMeasureType != RapidEnums::SMARTMEASUREMENTMODE::DEFAULTTYPE)
		{
			if(currentHandler->CurrentdrawHandler != NULL)
			{
				if(currentHandler->CurrentdrawHandler->getClicksDone() != 0)
				{
					MAINDllOBJECT->SetStatusCode("RCadApp04");
					CheckSmartMeasurementButton();
					return;
				}
			}
			if(currentHandler->FinishedCurrentDrawing)
			{
				SmartMeasureTypeMode(false);
				SetMeasurementType(RapidEnums::SMARTMEASUREMENTMODE::DEFAULTTYPE);
			}
			else
			{
				MAINDllOBJECT->SetStatusCode("RCadApp04");
				CheckSmartMeasurementButton();
				return;
			}
		}
		setHandler(new DefaultHandler());
		this->MeasurementMode(false);
		this->Derivedmodeflag = false;
		this->SetCrossOnVideo(false);

		SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
		CURRENTHANDLE = HANDLER;
		switch(HANDLER)
		{
			case RapidEnums::RAPIDHANDLERTYPE::RELATIVEPOINT_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::RELATIVEPOINT));		
				/*AddShapeAction::addShape(new RPoint(_T("RP"), RapidEnums::SHAPETYPE::RELATIVEPOINT));*/
				break;
			case RapidEnums::RAPIDHANDLERTYPE::PLANE_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::PLANE));
				/*AddShapeAction::addShape(new Plane());*/
				break;
			case RapidEnums::RAPIDHANDLERTYPE::CYLINDER_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::CYLINDER));
				/*AddShapeAction::addShape(new Cylinder());*/
				break;
			case RapidEnums::RAPIDHANDLERTYPE::CONE_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::CONE));
				/*AddShapeAction::addShape(new Cone());*/
				break;
			case RapidEnums::RAPIDHANDLERTYPE::TORUS_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::TORUS));
				//AddShapeAction::addShape(new Torus());
				break;
			case RapidEnums::RAPIDHANDLERTYPE::CIRCLE3D_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::CIRCLE3D));
				//AddShapeAction::addShape(new Circle(_T("C"), RapidEnums::SHAPETYPE::CIRCLE3D));
				break;
			case RapidEnums::RAPIDHANDLERTYPE::ELLIPSE3D_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::ELLIPSE3D));
				//AddShapeAction::addShape(new Conics3D(_T("EL"), RapidEnums::SHAPETYPE::ELLIPSE3D));
				break;
			case RapidEnums::RAPIDHANDLERTYPE::PARABOLA3D_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::PARABOLA3D));
				//AddShapeAction::addShape(new Conics3D(_T("PB"), RapidEnums::SHAPETYPE::PARABOLA3D));
				break;
			case RapidEnums::RAPIDHANDLERTYPE::HYPERBOLA3D_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::HYPERBOLA3D));
				//AddShapeAction::addShape(new Conics3D(_T("HB"), RapidEnums::SHAPETYPE::HYPERBOLA3D));
				break;
			case RapidEnums::RAPIDHANDLERTYPE::SPHERE_HANDLER:
				if(MasteProbeCalibrationMode())
				{
					if(getShapesList().count() != 3)
					{
						ShowMsgBoxString("RCadSetOtherhandlers01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
						return;
					}
					else
					{
						RC_ITER item = getShapesList().getList().begin();
						item++; item++;
						Shape* ThShape = (Shape*)((*item).second);
						if(ThShape->ShapeType == RapidEnums::SHAPETYPE::PLANE)
						{
							Shape* Csh = new Sphere();
							Csh->addParent(ThShape);
							ThShape->addChild(Csh);
							SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
							setHandler(new DummyHandler(Csh));
						/*AddShapeAction::addShape(Csh);*/
						}
						else
						{
							ShowMsgBoxString("RCadSetOtherhandlers01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
							return;
						}
					}
				}
				else
				{
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
					setHandler(new DummyHandler(RapidEnums::SHAPETYPE::SPHERE));
				/*AddShapeAction::addShape(new Sphere());*/
				}
				break;
			case RapidEnums::RAPIDHANDLERTYPE::DOTCOUNTS_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::DOTSCOUNT));				
				break;
			case RapidEnums::RAPIDHANDLERTYPE::CLOUDPOINTS_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new DummyHandler(RapidEnums::SHAPETYPE::CLOUDPOINTS));
				/*AddShapeAction::addShape(new CloudPoints());*/
				break;
			case RapidEnums::RAPIDHANDLERTYPE::POLYLINECLOUDPOINTS:
				{
					Shape *csh = new CloudPoints();
					((CloudPoints*)csh)->CloudPointsForLine = true;
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
					setHandler(new DummyHandler(csh));
					/*AddShapeAction::addShape(csh);*/
				}
				break;
			case RapidEnums::RAPIDHANDLERTYPE::RTEXT_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new RTextHandler());
				break;
			case RapidEnums::RAPIDHANDLERTYPE::IDMEASURE_HANDLER:
				setHandler(new FrameGrabIdAndOd(RapidEnums::RAPIDFGACTIONTYPE::IDMEASUREMENT));
				break;
			case RapidEnums::RAPIDHANDLERTYPE::ODMEASURE_HANDLER:
				setHandler(new FrameGrabIdAndOd(RapidEnums::RAPIDFGACTIONTYPE::ODMEASUREMENT));
				break;
			case RapidEnums::RAPIDHANDLERTYPE::DETECTALLEDGED_HANDLER:
				setHandler(new FrameGrabIdAndOd(RapidEnums::RAPIDFGACTIONTYPE::DETECT_ALLEDGES));
				break;
			case RapidEnums::RAPIDHANDLERTYPE::NEARESTPOINT:
				setHandler(new NearestPointHandler(true));
				break;
			case RapidEnums::RAPIDHANDLERTYPE::FARTHESTPOINT:
				setHandler(new NearestPointHandler(false));
				break;
			case RapidEnums::RAPIDHANDLERTYPE::NEARESTPOINT3D:
				setHandler(new NearestPointHandler(true, false));
				break;
			case RapidEnums::RAPIDHANDLERTYPE::FARTHESTPOINT3D:
				setHandler(new NearestPointHandler(false, false));
				break;
			case RapidEnums::RAPIDHANDLERTYPE::PCDMEASURE_HANDLER:
				MeasurementMode(true);
				setHandler(new PCDMeasureHandler());
				break;
			case RapidEnums::RAPIDHANDLERTYPE::UCS_XY:
				Derivedmodeflag = true;
				DeselectAll(false);
				setHandler(new UCSPointHandle());
				break;
			case RapidEnums::RAPIDHANDLERTYPE::UCS_YZ:
				Derivedmodeflag = true;
				DeselectAll(false);
				setHandler(new UCSPointHandle());
				break;
			case RapidEnums::RAPIDHANDLERTYPE::UCS_XZ:
				Derivedmodeflag = true;
				DeselectAll(false);
				setHandler(new UCSPointHandle());
				break;
			case RapidEnums::RAPIDHANDLERTYPE::UCS_POINT:
				Derivedmodeflag = true;
				DeselectAll(false);
				setHandler(new UCSPointHandle());
				break;
			case RapidEnums::RAPIDHANDLERTYPE::UCS_LINE:
				Derivedmodeflag = true;
				DeselectAll(false);
				setHandler(new UCSLineHandle());
				break;
			case RapidEnums::RAPIDHANDLERTYPE::UCS_PLANE:
				Derivedmodeflag = true;
				DeselectAll(false);
				setHandler(new UCSPlaneHandle());
				break;
			case RapidEnums::RAPIDHANDLERTYPE::UCS_SHIFT:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				DeselectAll(false);
				setHandler(new UCSPointHandle());
				break;

			case RapidEnums::RAPIDHANDLERTYPE::FOCUS_DEPTHTWOBOXHANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new FocusDepthMultiBoxHandler(true));
				FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::TWOBOXDEPTH_MODE;
				break;

			case RapidEnums::RAPIDHANDLERTYPE::FOCUS_DEPTHFOURBOXHANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				setHandler(new FocusDepthMultiBoxHandler(false));
				FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::FOURBOXDEPTH_MODE;
				break;

			case RapidEnums::RAPIDHANDLERTYPE::THREAD_LINEARCHANDLER:
				Derivedmodeflag = true;
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				setHandler(new ThreadLineArcHandler());
				break;
		}

		ClearAllRadioButton();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1124", __FILE__, __FUNCSIG__); }
}

//Returns the current active handler...//
MouseHandler* RCadApp::getCurrentHandler()
{
	return this->currentHandler;
}

//Set smart measurement handler..
void RCadApp::SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE SMhandler)
{
	try
	{
		this->MeasurementMode(false);
		MAINDllOBJECT->PREVIOUSHANDLE = RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER;
		if(SMhandler != PreviousSMhandler)
			SmartMeasureTypeMode(true);
		else
		{
			if(SmartMeasureTypeMode())
			{
				SmartMeasureTypeMode(false);
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				setHandler(new DefaultHandler());
				SetMeasurementType(RapidEnums::SMARTMEASUREMENTMODE::DEFAULTTYPE);
				return;
			}
			else
				SmartMeasureTypeMode(true);
		}
		CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::SMARTMEASUREMODE_HANDLER;
		SetMeasurementType(SMhandler);
		SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
		switch(SMhandler)
		{
			case RapidEnums::SMARTMEASUREMENTMODE::ANGLEMEASUREMENT:
				setHandler(new SM_AngleMeasureHandler());
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::SPLINEMEASUREMENT:
				setHandler(new SM_SplineMeasurementHandler());
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::ANGLEMEASUREWITH_XAXIS:
				setHandler(new SM_AngleMeasureWithXAxis());
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::ANGLEMEASUREWITH_YAXIS:
				setHandler(new SM_AngleMeasureWithYAxis());
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::DIAMEASUREMENT:
				setHandler(new SM_DiameterMeasureHandler());
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::RADIUSMEASUREMENT:
				setHandler(new SM_RadiusMeasureHandler());
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::STEPMEASUREMENT_3LINES:
				setHandler(new SM_SL1MeasureHandler());
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::POINTANGLE:
				setHandler(new SM_PointAngleMeasurementHandler());
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::STEPMEASUREMENT_3LINESWITHAXIS:
				setHandler(new SM_SL1MeasureHandler(true));
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::STEPMEASUREMENT_3LINES1:
				setHandler(new SM_SL2MeasureHandler());
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::STEPMEASUREMENT_4LINES:
				setHandler(new SM_SL3MeasureHandler());
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::STEPMEASUREMENT_4LINESWITHAXIS:
				setHandler(new SM_SL3MeasureHandler(true));
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::PARALLELRUNOUT:
			case RapidEnums::SMARTMEASUREMENTMODE::INTERSECTIONRUNOUT:
				setHandler(new SM_RunoutMeasurementHandler(RunoutFluteCount(), MAINDllOBJECT->RunOutType()));
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::RECANGLEMEASUREMENT:
				setHandler(new SM_RakeAngleMeasureHandler());
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::CHAMFERMEASUREMENT_LINE:
				setHandler(new SM_Chamfer_LineMeasureHandler());
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::CHAMFERMEASUREMENT_ARC:
				setHandler(new SM_Chamfer_ArcMeasureHandler(true, true));
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::GROOVEMEASUREMENTS_ONEFRAME:
				setHandler(new SM_AutoGrooveMeasurementHandler());
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::GROOVEMEASUREMENT:
				setHandler(new SM_GrooveMeasurement());
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::ARCARCWIDTH:
				setHandler(new SM_Arc_ArcMeasureWidth());
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::POINTARCWIDTH:
				setHandler(new SM_Arc_PointMeasureWidth());
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::LINEARCWIDTH:
				setHandler(new SM_Line_ArcMeasureWidth());
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::POINTLINEWIDTH:
				setHandler(new SM_Point_LineWidthMeasureHandler());
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::LINELINEWIDTH:
				setHandler(new SM_Line_LineWidthMeasureHandler());
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::LINEARCFG:
				setHandler(new SM_LineArcFGHandler());
				Shape* CSh = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
				if(CSh->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
				{
				   ((SM_LineArcFGHandler*)currentHandler)->AddpointCollFromCloudPnt();
				}
				break;
		}
		PreviousSMhandler = SMhandler;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1125", __FILE__, __FUNCSIG__); }
}

//Set internal smart measurement handler..
MouseHandler* RCadApp::SetCurrentSMFGHandler(RapidEnums::RAPIDHANDLERTYPE HANDLER)
{
	try
	{
		MouseHandler* Chandler = NULL;
		switch(HANDLER)
		{
			case RapidEnums::RAPIDHANDLERTYPE::POINT_HANDLER:
				if(DefaultCrosshairMode())
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
				else
					SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				Chandler = new PointHandler(true);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				Chandler = new LineHandlers();
				break;
			case RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				Chandler = new ArcHandler();
				break;
			case RapidEnums::RAPIDHANDLERTYPE::CIRCLE3P_HANDLER:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				Chandler = new CircleHandlers();
				break;
			case RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				Chandler = new LineParallel2Line();
				break;
			case RapidEnums::RAPIDHANDLERTYPE::TANGENTIAL_CIRCLE:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				Chandler = new CircleTangent2TwoLine();
				break;
			case RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
				Chandler = new FlexibleCrossHairHandler();
				break;
			case RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ARCFG);
				if(MAINDllOBJECT->Continuous_ScanMode)
					Chandler = new RotationScanHandler(RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARC_FRAMEGRAB);
				else
					Chandler = new FrameGrabArc();
				break;
			case RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ANGRECTFG);
				if(MAINDllOBJECT->Continuous_ScanMode)
					Chandler = new RotationScanHandler(RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARECT_FRAMEGRAB);
				else
					Chandler = new FrameGrabAngularRect();
				break;
			case RapidEnums::RAPIDHANDLERTYPE::CIRCLE_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CIRCLEFG);
				if(MAINDllOBJECT->Continuous_ScanMode)
					Chandler = new RotationScanHandler(RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_CIRCLE_FRAMEGRAB);
				else
					Chandler = new FrameGrabCircle();
				break;
			case RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
				if(MAINDllOBJECT->Continuous_ScanMode)
					Chandler = new RotationScanHandler(RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_RECT_FRAMEGRAB);
				else
					Chandler = new FrameGrabRectangle(false);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB_ALLEDGE:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
				Chandler = new FrameGrabRectangle(true);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::FIXEDRECT_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
				if(MAINDllOBJECT->Continuous_ScanMode)
					Chandler = new RotationScanHandler(RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_FIXEDRECT_FRAMEGRAB);
				else
					Chandler = new FrameGrabFixedRectangle();
				break;
			case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARC_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ARCFG);
				Chandler = new RotationScanHandler(HANDLER);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARECT_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ANGRECTFG);
				Chandler = new RotationScanHandler(HANDLER);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_CIRCLE_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CIRCLEFG);
				Chandler = new RotationScanHandler(HANDLER);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_RECT_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
				Chandler = new RotationScanHandler(HANDLER);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_FIXEDRECT_FRAMEGRAB:
				SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_RECTFG);
				Chandler = new RotationScanHandler(HANDLER);
				break;
		}
		return Chandler;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1126", __FILE__, __FUNCSIG__); return NULL;}
}

//Set Smart Measurement Type..
void RCadApp::SetMeasurementType(RapidEnums::SMARTMEASUREMENTMODE d)
{
	try{ SmartMeasureType = d; }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1127", __FILE__, __FUNCSIG__); }
}

//Set the circle measurement.. ..//types..
bool RCadApp::SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE d, int type)
{
	try
	{
		if(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::MEASURE_HANDLER && currentHandler != NULL)
		{
			bool flag = ((DimensionHandler*)currentHandler)->SetCurrentMeasurementTypeForRightClick(d, type);
			return flag;
		}
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1128", __FILE__, __FUNCSIG__); return false; }
}

//Set the Mouse Cursor type... to be displayed on video.....//
void RCadApp::SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE Ctype)
{
	try
	{
		VideoMpointType = Ctype;
		WindowCursorChanged(0, (int)VideoMpointType);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1129", __FILE__, __FUNCSIG__); }
}

void RCadApp::SetRcadCursorType(RapidEnums::RWINDOW_CURSORTYPE Ctype)
{
	try{ WindowCursorChanged(1, (int)Ctype); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1130", __FILE__, __FUNCSIG__); }
}

void RCadApp::SetDxfCursorType(RapidEnums::RWINDOW_CURSORTYPE Ctype)
{
	try{ WindowCursorChanged(2, (int)Ctype); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1131", __FILE__, __FUNCSIG__); }
}

//Measurement Unit type..//
void RCadApp::SetMeasurementUnit(RapidEnums::RAPIDUNITS d)
{
	try
	{
		dontUpdateGraphcis = true;
		MeasureUnitType = d;
		for(RC_ITER UCSitem = getUCSList().getList().begin(); UCSitem != getUCSList().getList().end(); UCSitem++)
		{
			UCS* Cucs = (UCS*)(*UCSitem).second;
			for(RC_ITER Dimitem = Cucs->getDimensions().getList().begin(); Dimitem != Cucs->getDimensions().getList().end(); Dimitem++)
			{
				DimBase* dim = (DimBase*)(*Dimitem).second;
				dim->UpdateMeasurement();
			}
		}
		DXFEXPOSEOBJECT->CurrentDXFMeasureList.clear();
		for(RC_ITER Citem = getDXFList().getList().begin(); Citem != getDXFList().getList().end(); Citem++)
		{
			DXFCollection* CDxfitem = ((DXFCollection*)(*Citem).second);
			for(RC_ITER Cdxfdim = CDxfitem->getMeasureCollection().getList().begin(); Cdxfdim != CDxfitem->getMeasureCollection().getList().end(); Cdxfdim++)
			{
				DimBase* dim = (DimBase*)(*Cdxfdim).second;
				dim->UpdateMeasurement();
				if(CDxfitem->selected())
					DXFEXPOSEOBJECT->CurrentDXFMeasureList.push_back(dim);
			}
		}
		dontUpdateGraphcis = false;
		DXFMeasurement_Updated();
		Measurement_updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1132", __FILE__, __FUNCSIG__); dontUpdateGraphcis = false; }
}

//Current Highlighted shape...
Shape* RCadApp::highlightedShape()
{
	try
	{
		if (ShapeHighlighted() && !ShapeHighlightedForImplicitSnap()) {			
			return this->nearestShape;
		}
		else if(DXFShapeHighlighted()) return this->DXFnearestShape;
		else return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1133", __FILE__, __FUNCSIG__); return NULL; }
}

//Current highlighted Dim....//
DimBase* RCadApp::highlightedmeasure()
{
	try
	{
		if(MeasurementHighlighted()) return this->nearestmeasure;
		return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1134", __FILE__, __FUNCSIG__); return NULL; }
}

//Current Highlighted point.. Snap point...//
Vector* RCadApp::highlightedPoint()
{
	try
	{
		if(this->SnapPointHighlighted()) return this->lastsnap;
		else if(this->DXFSnapPointHighlighted()) return this->DXFlastsnap;
		else return NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1135", __FILE__, __FUNCSIG__); return NULL; }
}

void RCadApp::SetSnapPointHighlighted(double x, double y, double z)
{
	try
	{
		Vector temp(x, y, z);
		Vector* Vx = MAINDllOBJECT->getVectorPointer(&temp);
		if(Vx != NULL)
		{
			this->lastsnap = Vx;
			this->SnapPointHighlighted(true);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1136", __FILE__, __FUNCSIG__); }
}

//Item added event...// Call update grpahics and notify to the listeners.../
void RCadApp::itemAdded(BaseItem* item, Listenable* sender)
{
	try
	{
		this->VideoMouseFlag = true;
		this->RCADMouseFlag = true;
		if(static_cast<RCollectionBase*>(sender) == &this->getCurrentUCS().getShapes())
		{
			Shape* CShape = (Shape*)item;
			if(AutomatedMeasureModeFlag())
				Automated_ShapeTypeList.push_back(CShape);
		}
		else if(static_cast<RCollectionBase*>(sender) == &this->getCurrentUCS().getDimensions())
		{
			DimBase* Cdim = (DimBase*)item;
			if(AutomatedMeasureModeFlag())
				Automated_MeasurementTypeList.push_back(Cdim);
			if(Cdim->MeasureAsGDnTMeasurement())
				RSelectedShape_Updated = true;
		}
		else if (static_cast<RCollectionBase*>(sender) == &this->ucslist)
		{
			UCS* ucs = (UCS*)(item);
			ucs->getShapes().addListener(this);
			ucs->getDimensions().addListener(this);
			ucs->getDxfCollecion().addListener(this);
		}
		else if(static_cast<RCollectionBase*>(sender) == &this->actionhistory)
		{

		}
		this->RCADMouseFlag = false;
		this->VideoMouseFlag = false;
	}
	catch(...){ this->RCADMouseFlag = false; this->VideoMouseFlag = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1137", __FILE__, __FUNCSIG__);}
}

//Item removed event....// Call update graphics and notify to listeners..//
void RCadApp::itemRemoved(BaseItem* item, Listenable* sender)
{
	try
	{
		if(static_cast<RCollectionBase*>(sender) == &this->getCurrentUCS().getShapes())
		{
			Shape* CShape = (Shape*)item;
			if(AutomatedMeasureModeFlag())
			{
				bool removeShape = false;
				for each(Shape* Shp in Automated_ShapeTypeList)
				{
					if(CShape->getId() == Shp->getId())
					{
						removeShape = true;
						break;
					}
				}
				if(removeShape)
					Automated_ShapeTypeList.remove(CShape);
			}
		}
		else if(static_cast<RCollectionBase*>(sender) == &this->getCurrentUCS().getDimensions())
		{
			DimBase* Cdim = (DimBase*)item;
			if(AutomatedMeasureModeFlag())
			{
				bool removeMeasure = false;
				for each(DimBase* measure in Automated_MeasurementTypeList)
				{
					if(measure->getId() == Cdim->getId())
					{
						removeMeasure = true;
						break;
					}
				}
				if(removeMeasure)
					Automated_MeasurementTypeList.remove(Cdim);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1138", __FILE__, __FUNCSIG__); }
}

//Item changed event...// Call update and notify itemchanged event...//
void RCadApp::itemChanged(BaseItem* item, Listenable* sender)
{
	try
	{
		if(static_cast<RCollectionBase*>(sender) == &this->getCurrentUCS().getShapes())
		{
			if(((Shape*)item)->getMchild().size() > 0 )
				UpdateShapesChanged();
			else
				UpdateShapesChanged();
		}
		else if(static_cast<RCollectionBase*>(sender) == &this->getCurrentUCS().getDimensions())
		{
			Measurement_updated();
		}
		else
		{
			update_VideoGraphics();
			update_RcadGraphics();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1139", __FILE__, __FUNCSIG__); }
}

//Selection Changed event..
void RCadApp::selectionChanged(Listenable* sender)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1140", __FILE__, __FUNCSIG__); }
}

//Add the new UCS for DRO label button click...//
void RCadApp::AddNewUCS_DROclear()
{
	try
	{
		AddingUCSModeFlag = true;
		UCStoMCS(getCurrentUCS());
		getUCSList().getList()[0]->selected(true);
		UCS* nucs = new UCS(_T("UCS"));
		UCS& cucs = MAINDllOBJECT->getCurrentUCS();
		nucs->UCSMode(0);
		/*nucs->UCSPoint = getCurrentDRO();*/
		nucs->getWindow(0).init(cucs.getWindow(0));
		nucs->getWindow(1).init(cucs.getWindow(1));
		nucs->getWindow(2).init(cucs.getWindow(2));
		AddUCSAction::addUCS(nucs);
		updateAll();
		nucs->SetUCSProjectionType((int)UCSProjectionType);
		AddingUCSModeFlag = false;
		changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
	}
	catch(...){ AddingUCSModeFlag = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1141", __FILE__, __FUNCSIG__); }
}

//Handles the changes ucs action..//
void RCadApp::changeUCS(int id,  bool addaction)
{
	try
	{
		if(id == getCurrentUCS().getId()) return;
		UCStoMCS(getCurrentUCS(), false);
		UpdateGraphics_UCSChange();

		if (PPCALCOBJECT->IsPartProgramRunning()) Sleep(500);

		if(addaction)
			ChangeUCSAction::changeUCS(id);
		else
			getUCSList().getList()[id]->selected(true);
		if (id != 0)
		{
			MCStoUCS(getCurrentUCS(), false);
		}
		UpdateGraphics_UCSChange();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1142", __FILE__, __FUNCSIG__); }
}

//Change MCS to UCS...
void RCadApp::MCStoUCS(UCS& cucs, bool updategraphics)
{
	int LineNumber = 0;
	try
	{
		if(cucs.UCSMode() == 1)
		{
			LineNumber++;
			double transformM[9] = {0};
			if(cucs.UCSProjectionType() == 1)  
			{
					transformM[1] = 1;   transformM[5] = 1;   transformM[6] = 1; 
			}
			else if(cucs.UCSProjectionType() == 0)
			{
					transformM[0] = 1;   transformM[4] = 1;   transformM[8] = 1; 
			}
			else if(cucs.UCSProjectionType() == 2) 
			{
					transformM[0] = 1;   transformM[5] = 1;   transformM[7] = 1; 
			}

			Vector difference(-cucs.CenterPt->getX(), -cucs.CenterPt->getY(), -cucs.CenterPt->getZ());
			LineNumber++;
			for(RC_ITER item = cucs.getShapes().getList().begin();item != cucs.getShapes().getList().end();item++)
			{
				Shape* CShape = (Shape*)(*item).second;
				if(CShape->getMCSParentShape() != NULL)
				{
					Shape* ParentShape = (Shape*)CShape->getMCSParentShape();
					CShape->CopyShapeParameters(ParentShape);
					if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
					{
						((CloudPoints*)CShape)->PointsListOriginal->deleteAll();
						((CloudPoints*)CShape)->PointsList->deleteAll();
						((CloudPoints*)CShape)->CopyPoints(ParentShape);
					}
					CShape->Translate(difference);
					CShape->Transform(transformM);
					CShape->notifyAll(ITEMSTATUS::DATACHANGED, CShape);
				}
			}
			//cucs.UCSPoint.set(cucs.CenterPt->getX(), cucs.CenterPt->getY(), cucs.CenterPt->getZ());
		}
		else if(cucs.UCSMode() == 2)
		{
			LineNumber += 10;
			double UcsAngle = 0, Intercept = 0;
			if(cucs.UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
			{
				switch(cucs.AxisLine->ShapeType)
				{
					case RapidEnums::SHAPETYPE::XLINE:
					case RapidEnums::SHAPETYPE::XRAY:
					case RapidEnums::SHAPETYPE::LINE:
						UcsAngle = ((Line*)cucs.AxisLine)->Angle();
						Intercept = ((Line*)cucs.AxisLine)->Intercept();
						RMATH2DOBJECT->Angle_FirstFourthQuad(&UcsAngle);
						break;
				}
			}
			else
			{
				UcsAngle = cucs.UCSangle();
				Intercept = cucs.UCSIntercept();
			}
			LineNumber++;
			cucs.SetParameters(UcsAngle, cucs.CenterPt);
			cucs.UCSangle(UcsAngle);
			cucs.UCSIntercept(Intercept);
			//if(cucs.CenterPt != NULL)
			//     cucs.UCSPoint.set(cucs.CenterPt->getX(), cucs.CenterPt->getY(), cucs.CenterPt->getZ());
			//double p1[3] = {0}, p2[2] = {cucs.UCSPoint.getX(), cucs.UCSPoint.getY()};
			//double transformM[9] = {0};
			//RMATH2DOBJECT->TransformM_PointLineAlign(&p2[0], UcsAngle, Intercept, &p1[0], 0, 0, &transformM[0]);
			//transformM[2] = 0; transformM[5] = 0; 
			//RMATH2DOBJECT->TransformM_RotateAboutPoint(&p1[0], -UcsAngle, &cucs.transform[0]);
			//double transformMParent[9] = {0};
			//if(cucs.ParentUcsId() != 0)
			//{
			//	UCS *parentUcs = MAINDllOBJECT->getUCS(cucs.ParentUcsId());
			//	if(parentUcs->UCSProjectionType() == 1)  
			//	{
			//			transformMParent[1] = 1;   transformMParent[5] = 1;   transformMParent[6] = 1; 
			//	}
			//	else if(parentUcs->UCSProjectionType() == 0)
			//	{
			//			transformMParent[0] = 1;   transformMParent[4] = 1;   transformMParent[8] = 1; 
			//	}
			//	else if(parentUcs->UCSProjectionType() == 2) 
			//	{
			//			transformMParent[0] = 1;   transformMParent[5] = 1;   transformMParent[7] = 1; 
			//	}
			//	p1[0] = parentUcs->UCSPoint.getX();
			//	p1[1] = parentUcs->UCSPoint.getY();	
			//	p1[2] = parentUcs->UCSPoint.getZ();
			//}
			//for(RC_ITER item = cucs.getShapes().getList().begin();item != cucs.getShapes().getList().end();item++)
			//{
			//	Shape* CShape = (Shape*)(*item).second;
			//	if(CShape->getMCSParentShape() != NULL)
			//	{
			//		Shape* ParentShape = (Shape*)CShape->getMCSParentShape();
		 //   	    CShape->CopyShapeParameters(ParentShape);
			//		if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
			//		{
			//			((CloudPoints*)CShape)->PointsListOriginal->deleteAll();
			//			((CloudPoints*)CShape)->PointsList->deleteAll();
			//			((CloudPoints*)CShape)->CopyPoints(ParentShape);
			//		}
			//		//if(cucs.ParentUcsId() != 0 && (ParentShape->UcsId() == 0))
			//	 //   {
			//		//	//Vector tmpv = Vector(-p1[0], -p1[1], -p1[2]);
			//		// //   CShape->Translate(tmpv);
			//		// //   CShape->Transform(transformMParent);
			//		//}
			//		//Vector tmpv = Vector(-p2[0], - p2[1], - cucs.UCSPoint.getZ());
			//		//CShape->Translate(tmpv);
			//		//CShape->Transform(&transformM[0]);
			//		CShape->Transform(cucs.transform);
			//		CShape->notifyAll(ITEMSTATUS::DATACHANGED, CShape);
			//	}
			//}
			if(!cucs.KeepGraphicsStraightMode())
				SetRcadRotateAngle(cucs.UCSangle());
			LineNumber++;
			SetVideoRotateAngle(cucs.UCSangle());
			LineNumber++;
		}
		else if(cucs.UCSMode() == 3)
		{
			LineNumber += 20;
			double p1[2] = {0, 0}, UcsAngle = cucs.UCSangle();
			double transformM[9] = {0};
			RMATH2DOBJECT->TransformM_RotateAboutPoint(&p1[0], UcsAngle, &transformM[0]);
			RMATH2DOBJECT->TransformM_RotateAboutPoint(&p1[0], -UcsAngle, &cucs.transform[0]);
			LineNumber++;
			for(RC_ITER item = cucs.getShapes().getList().begin();item != cucs.getShapes().getList().end();item++)
			{
				Shape* CShape = (Shape*)(*item).second;
				if(CShape->getMCSParentShape() != NULL)
				{
					Shape* ParentShape = (Shape*)CShape->getMCSParentShape();
					CShape->CopyShapeParameters(ParentShape);
					CShape->Transform(&transformM[0]);
					CShape->notifyAll(ITEMSTATUS::DATACHANGED, CShape);
				}
			}
			LineNumber++;
			if(!cucs.KeepGraphicsStraightMode())
				SetRcadRotateAngle(cucs.UCSangle());
			SetVideoRotateAngle(cucs.UCSangle());
			LineNumber++;
		}
		else if(cucs.UCSMode() == 4)
		{
			LineNumber += 30;
			list<list<double>> ShapesParam;
			list<list<double>>* ShpParmPntr = &ShapesParam;
			for(map<int, Shape*>::iterator It1 = cucs.ShapesCol.begin(); It1 != cucs.ShapesCol.end(); It1++)
				HELPEROBJECT->getShapeDimension(It1->second, ShpParmPntr);
			LineNumber++;
			for(map<int, Vector*>::iterator It2 = cucs.PointsCol.begin(); It2 != cucs.PointsCol.end(); It2++)
			{
				It2->second = MAINDllOBJECT->getVectorPointer_UCS(It2->second, MAINDllOBJECT->getUCS(0), true);
				HELPEROBJECT->getPointParam(It2->second, ShpParmPntr);
			}
			LineNumber++;
			double TransformM[16] = {0}, InversOldTransM[16] = {0}, DiffTransM[16] = {0}, PCenter[3];
			if(!HELPEROBJECT->UCSBasePlaneTransformM(&ShapesParam, TransformM)) return;
			RMATH2DOBJECT->OperateMatrix4X4(&(cucs.transform[0]), 4, 1, InversOldTransM);
			LineNumber++;
			int Order1[2] = {4,4}, Order2[2]= {4,4};
			RMATH2DOBJECT->MultiplyMatrix1(TransformM, Order1,InversOldTransM, Order2, DiffTransM);
			for(int i = 0; i < 16; i++) {cucs.transform[i] = TransformM[i];}
			for(RC_ITER item = cucs.getShapes().getList().begin();item != cucs.getShapes().getList().end(); item++)
			{
				Shape* CShape = (Shape*)(*item).second;
				if(CShape->getMCSParentShape() != NULL)
				{
					Shape* ParentShape = (Shape*)CShape->getMCSParentShape();
					CShape->CopyShapeParameters(ParentShape);
					if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
					{
						((CloudPoints*)CShape)->PointsListOriginal->deleteAll();
						((CloudPoints*)CShape)->PointsList->deleteAll();
						((CloudPoints*)CShape)->CopyPoints(ParentShape);
					}
					CShape->AlignToBasePlane(TransformM);
					CShape->notifyAll(ITEMSTATUS::DATACHANGED, CShape);
				}
				else
					((ShapeWithList*)CShape)->TransformShapePoints(DiffTransM);
			}
			LineNumber++;
		}
		UpdteVgp_UcsChange = true;
		UpdteCadgp_UcsChange = true;
		LineNumber++;
		if(updategraphics)
		{
			update_VideoGraphics();
			update_RcadGraphics();
			Sleep(30);
			Wait_VideoGraphicsUpdate();
			Wait_RcadGraphicsUpdate();
			update_RcadGraphics();
			Sleep(30);
			Wait_RcadGraphicsUpdate();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1143" + std::to_string(LineNumber), __FILE__, __FUNCSIG__); }
}

//Change UCS to MCS....
void RCadApp::UCStoMCS(UCS& cucs, bool updategraphics)
{
	try
	{
		if(cucs.UCSMode() == 2 || cucs.UCSMode() == 3)
		{
			if(!cucs.KeepGraphicsStraightMode())
				SetRcadRotateAngle(-1 * cucs.UCSangle());
			SetVideoRotateAngle(-1 * cucs.UCSangle());
		}
		//SetVideoRotateAngle(0);
		//Video_Graphics->UndoAllRotations();
		//Rcad_Graphics->UndoAllRotations();
		UpdteVgp_UcsChange = true;
		UpdteCadgp_UcsChange = true;
		if(updategraphics)
		{
			update_VideoGraphics();
			update_RcadGraphics();
			Sleep(30);
			Wait_VideoGraphicsUpdate();
			Wait_RcadGraphicsUpdate();
			update_RcadGraphics();
			Sleep(30);
			Wait_RcadGraphicsUpdate();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1144", __FILE__, __FUNCSIG__); }
}

//Delete the UCS according to the ID...
void RCadApp::deleteUCS()
{
	try
	{
		if(getCurrentUCS().getId() == 0) return; //you cannot delete the MCS!
		DeleteUCSAction *dua = new DeleteUCSAction();
		addAction(dua);	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1145", __FILE__, __FUNCSIG__); }
}

//Copy all the shapes from one ucs to another..//
void RCadApp::CopyAllShapesUCS_UCS(UCS *Source, UCS *Destination)
{
	try
	{
		double p1[2] = {Source->UCSPoint.getX(),Source->UCSPoint.getY()}, p2[2] = {Destination->UCSPoint.getX(),Destination->UCSPoint.getY()};
		double SourceAngle = Source->UCSangle(), DstAngle = Destination->UCSangle(), transformMatrix[9];
		double SourceIntercept = Source->UCSIntercept(), DstIntercept = Destination->UCSIntercept();
		double RotatedAngle, ShiftedOrigin[2] = { 0 };
		RMATH2DOBJECT->TransformM_PointLineAlign(&p2[0], DstAngle, DstIntercept, &p1[0], SourceAngle, SourceIntercept, &transformMatrix[0], &RotatedAngle, ShiftedOrigin);
		CopyShapes(Source->getShapes(), &transformMatrix[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1146", __FILE__, __FUNCSIG__); }
}

//Copy selected shapes from ucs to another ucs..//
void RCadApp::CopySelectedShapeUCS_UCS(UCS *Source, UCS *Destination)
{
	try
	{   
		if (Destination->UCSMode() == 4)
		{
			double TransformMatrix[16]={0};
			CopyShapesToUCS4(Source->getShapes().getSelected(), &(Source->transform[0]),  &(Destination->transform[0]));
		} 
		else
		{
			double p1[2] = {Source->UCSPoint.getX(),Source->UCSPoint.getY()}, p2[2] = {Destination->UCSPoint.getX(),Destination->UCSPoint.getY()};
			double SourceAngle = Source->UCSangle(), DstAngle = Destination->UCSangle(), transformMatrix[9];
			double SourceIntercept = Source->UCSIntercept(), DstIntercept = Destination->UCSIntercept();
			double RotatedAngle, ShiftedOrigin[2] = { 0 };
			RMATH2DOBJECT->TransformM_PointLineAlign(&p2[0], DstAngle, DstIntercept, &p1[0], SourceAngle, SourceIntercept, &transformMatrix[0], &RotatedAngle, ShiftedOrigin);
			CopyShapes(Source->getShapes().getSelected(), &transformMatrix[0]);		
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1147", __FILE__, __FUNCSIG__); }
}

//Copy the selected shapes...//
void RCadApp::CopyShapes(RCollectionBase& Source, double* transform)
{
	try
	{
		int CucsId = getCurrentUCS().getId();
		for(RC_ITER item = Source.getList().begin();item != Source.getList().end();item++)
		{
			Shape* ParentShape = (Shape*)(*item).second;
			Shape* Cshape = ParentShape->Clone(0);
			Cshape->UcsId(CucsId);
			Cshape->Transform(transform);
			Cshape->adducsCopyRelation(ParentShape);
			AddShapeAction::addShape(Cshape);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1148", __FILE__, __FUNCSIG__); }
}

//Copy the selected shapes...//
void RCadApp::CopyShape()
{
	try
	{
		CurrentSelectedShapeList.clear();
		UCSid = getCurrentUCS().getId();
		for(RC_ITER i = getCurrentUCS().getShapes().getSelected().getList().begin(); i != getCurrentUCS().getShapes().getSelected().getList().end(); i++)
		{
			Shape* sh = (Shape*)(*i).second;
			CurrentSelectedShapeList.push_back(sh);
		}
		if(CurrentSelectedShapeList.size() > 0)
		{
			double lefttop[2], rightbottom[2];
			HELPEROBJECT->GetSelectedShapeExtents(CurrentSelectedShapeList, &lefttop[0], &rightbottom[0]);
			middlept.set((lefttop[0] + rightbottom[0])/2, (lefttop[1] + rightbottom[1])/2, MAINDllOBJECT->getCurrentDRO().getZ());
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1149", __FILE__, __FUNCSIG__); }
}

void RCadApp::CopyShapesToUCS4(RCollectionBase& Source, double* transformc, double* transformd)
{
	try
	{
		for(RC_ITER item = Source.getList().begin();item != Source.getList().end();item++)
		{
			Shape* ParentShape = (Shape*)(*item).second;
			Shape* Cshape = ParentShape->Clone(0);
			Cshape->UpdateEnclosedRectangle();
			Cshape->AlignToBasePlane(transformd);
			Cshape->adducsCopyRelation(ParentShape);
			AddShapeAction::addShape(Cshape);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1150", __FILE__, __FUNCSIG__); }
}
//Paste the copied collection...//
void RCadApp::PasteShape()
{
	try
	{
		if(UCSid == -1) return;
		if(UCSid == getCurrentUCS().getId())
		{
			changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
			if(CurrentSelectedShapeList.size() > 0)
				middlepttemp.set(middlept.getX(), middlept.getY(), MAINDllOBJECT->getCurrentDRO().getZ());
			ShapePasted = true;
		}
		CopyShapesAction::setCopiedShape(CurrentSelectedShapeList, UCSid, getCurrentUCS().getId());
		Shape_Updated();
		UCSid = -1;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1151", __FILE__, __FUNCSIG__); }
}

//Shift Origin..
void RCadApp::ShiftOrigin(double *transform)
{
	try
	{
		for(RC_ITER item = getCurrentUCS().getShapes().getList().begin(); item !=  getCurrentUCS().getShapes().getList().end(); item++)
			((Shape*)(*item).second)->Transform(transform);
		updateAll();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1152", __FILE__, __FUNCSIG__); }
}

//Set the UCS rotation angle..//
void RCadApp::SetUCSRotateAngle(double angle)
{
	try
	{
		RMATH2DOBJECT->Angle_FirstFourthQuad(&angle);
		AddingUCSModeFlag = true;
		//New UCS..
		UCS* nucs = new UCS(_T("UCS"));
		UCS& cucs = MAINDllOBJECT->getCurrentUCS();
		//Set ucs mode..//
		nucs->UCSMode(3);
		//Set the rotation angle...//
		nucs->UCSangle(angle);
		double p1[3] = {0.0,0.0,0.0};
		double p[3] = {0.0,0.0,0.0};
		//Calculate the Back transformation matrix.....//
		double CenterPt[2] = {0}, UcsAngle = nucs->UCSangle();
		RMATH2DOBJECT->TransformM_RotateAboutPoint(&CenterPt[0], -UcsAngle, &nucs->transform[0]);
		//Add windows..//
		nucs->getWindow(0).init(cucs.getWindow(0));
		nucs->getWindow(1).init(cucs.getWindow(1));
		nucs->getWindow(2).init(cucs.getWindow(2));
		//Set the projection type..//
		nucs->SetUCSProjectionType(cucs.UCSProjectionType());
		AddUCSAction::addUCS(nucs);
		SetRcadRotateAngle(angle);
		SetVideoRotateAngle(angle);
		centerScreen(getCurrentDRO().getX(), getCurrentDRO().getY(), getCurrentDRO().getZ());
		//Update all graphics...//
		updateAll();
		AddingUCSModeFlag = false;
		SetStatusCode("Finished");
		changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
	}
	catch(...){ dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1153", __FILE__, __FUNCSIG__); }
}

//Update graphics for UCS Change..
void RCadApp::UpdateGraphics_UCSChange()
{
	try
	{
		Wait_VideoGraphicsUpdate();
		Wait_RcadGraphicsUpdate();
		Wait_DxfGraphicsUpdate();
		SetGraphicsUpdateFlag_UCSChange();
		Sleep(30);
		Wait_VideoGraphicsUpdate();
		Wait_RcadGraphicsUpdate();
		Wait_DxfGraphicsUpdate();
		SetGraphicsUpdateFlag_UCSChange();
		Sleep(30);
		Wait_VideoGraphicsUpdate();
		Wait_RcadGraphicsUpdate();
		Wait_DxfGraphicsUpdate();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1154", __FILE__, __FUNCSIG__); }
}

//Set required flags for Ucs change graphics udpate..
void RCadApp::SetGraphicsUpdateFlag_UCSChange()
{
	try
	{
		getWindow(0).WindowPanned(true);
		getWindow(1).WindowPanned(true);
		getWindow(2).WindowPanned(true);
		UpdateShapesChanged(false);
		DXFEXPOSEOBJECT->UpdateForUCS();
		DXFShape_Updated(false);
		update_VideoGraphics();
		update_RcadGraphics();
		update_DxfGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1155", __FILE__, __FUNCSIG__); }
}

//Select shape as per id...//
void RCadApp::selectShape(int id, bool multi) 
{
	try
	{
		if(PPCALCOBJECT->PartprogramisLoading()) return;
		dontUpdateGraphcis = true;
		CurrentPCDShape = NULL;
		getShapesList().multiselect(multi);
		if(!multi)
		{
			Shape* CShape = (Shape*)getShapesList().getList()[id];
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE||CShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D)
			{
				if(((Circle*)CShape)->CircleType == RapidEnums::CIRCLETYPE::PCDCIRCLE)
				{
					CurrentPCDShape = CShape;
					if(!PCDMeasureModeFlag() && !PPCALCOBJECT->IsPartProgramRunning())
					{
						if(!(dynamic_cast<PCDHandler*>(currentHandler)))
						{
							setHandler(new PCDHandler(false));
							Derivedmodeflag = true;
							CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PCD_HANDLER;
							PREVIOUSHANDLE = RapidEnums::RAPIDHANDLERTYPE::PCD_HANDLER;
						}
					}
					//Update Pcd Graphics Window..
				}
			}
			else
				CurrentPCDShape = NULL;
			getShapesList().clearSelection(false);
			getShapesList().getList()[id]->selected(true);
			getSelectedShapesList().addItem(getShapesList().getList()[id], false);
			getShapesList().SetSeelectedItem();
			getShapesList().notifySelection();
		}
		else
		{
			if(getShapesList().getList()[id]->selected())
				getShapesList().getList()[id]->selected(false);
			else
				getShapesList().getList()[id]->selected(true);
		}
		if(!PPCALCOBJECT->IsPartProgramRunning())
		{
			Shape* sh = (Shape*)getShapesList().getList()[id];
			if(((ShapeWithList*)sh)->PointsList->Pointscount() > 1)
			{
				char temp[10];
				itoa(((ShapeWithList*)sh)->PointsList->Pointscount(), &temp[0], 10);
				std::string Tempstr = (const char*)temp;
				std::string Tempstr1 = (const char*)sh->getModifiedName();
				MAINDllOBJECT->SetStatusCode("RCadAppselectShape01", true, Tempstr1 +"@"+ Tempstr);
			}
		}
		if(MeasurementMode() && CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ROTARYMEASURE_HANDLER)
		{
			RC_ITER it = getSelectedShapesList().getList().begin();
			ShapeHighlighted(true);
			nearestShape = ((Shape*)(*it).second);
			((MouseHandler*)currentHandler)->LmouseDown_x(0, 0);
			ShapeHighlighted(false);
		}
		getShapesList().multiselect(false);
		dontUpdateGraphcis = false;
		Shape_Updated();
	}
	catch(...){ dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1156", __FILE__, __FUNCSIG__); }
}

//Select Multiple shapes..
void RCadApp::selectShape(list<int> *PtIdList) 
{
	try
	{
		if(PPCALCOBJECT->PartprogramisLoading()) return;
		if(PtIdList->size() == 1)
		{
			selectShape((*PtIdList->begin()), false);
		}
		else
		{
			dontUpdateGraphcis = true;
			getShapesList().clearSelection(true);
			BaseItem* Citem = NULL;
			for each(int Shid in *PtIdList)
			{
				Citem = getShapesList().getList()[Shid];
				Citem->selected(true, false);
				getSelectedShapesList().addItem(Citem, false);
			}
			getShapesList().SetSeelectedItem();
			getShapesList().notifySelection();
			dontUpdateGraphcis = false;
			Shape_Updated();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1157", __FILE__, __FUNCSIG__); }
}

//Delete the selected Shape...//
void RCadApp::deleteShape()
{
	try 
	{
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
		for(RC_ITER itr = MAINDllOBJECT->getShapesList().getList().begin(); itr != MAINDllOBJECT->getShapesList().getList().end(); itr++)
		{
			Shape* Cshape = (Shape*)((*itr).second);
			if(Cshape->ShapeForThreadMeasurement())
				Cshape->selected(false);
		}
		if(MAINDllOBJECT->getSelectedShapesList().count() == 0) return;   //no need to do anything when nothing is selected
		if(ShowHideFGPoints())
		{
			deleteFGPoints();
		}
		else
		{
			if(ReferenceDotShape != NULL)
				ReferenceDotShape->selected(false);
			if(this->Vblock_CylinderAxisLine != NULL)
				this->Vblock_CylinderAxisLine->selected(false);
			if(SecondReferenceDotShape != NULL)
				SecondReferenceDotShape->selected(false);
			if(this->ReferenceLineShape != NULL)
				this->ReferenceLineShape->selected(false);
			
			DeleteShapesAction* dsa = new DeleteShapesAction();
			RC_ITER i = getShapesList().getList().end(); i--;
			Shape* sh = dynamic_cast<Shape*>((*i).second);
			if (sh->getGroupParent().size() > 0)
			{
				Shape *psh = getHighestParentOfGroup(sh);
				psh->selected(true);
			}
			addAction(dsa);
			if(getShapesList().getList().size() > 1) // FundamentalShapes())
			{
				RC_ITER i = getShapesList().getList().end();
				i--;
				Shape* sh = dynamic_cast<Shape*>((*i).second);
				sh->selected(true);
			}
			UpdateShapesChanged();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1158", __FILE__, __FUNCSIG__); }
}

void RCadApp::DeleteRelatedMeasurement(ShapeWithList* Shp)
{
	try
	{
		while(Shp->getMchild().size() != 0)
		{
			list<BaseItem*>::iterator Measureiterator = Shp->getMchild().begin();
			DimBase* Cdim = (DimBase*)(*Measureiterator);
			for(list<BaseItem*>::iterator shapeiterator = Cdim->getParents().begin();shapeiterator != Cdim->getParents().end(); shapeiterator++)
			{
				((Shape*)(*shapeiterator))->RemoveMChild(Cdim);
			}
			MAINDllOBJECT->getDimList().removeItem((BaseItem*)Cdim, true);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1159", __FILE__, __FUNCSIG__); }
}

void RCadApp::deleteReferenceShape(ShapeWithList* Shp)
{
	try 
	{
		for each(BaseItem* act in Shp->PointAtionList)
		{
			MAINDllOBJECT->getActionsHistoryList().removeItem(act, true);
		}
		Shp->PointAtionList.clear();
		if(Shp->CurrentShapeAddAction != NULL) 
		{
			MAINDllOBJECT->getActionsHistoryList().removeItem(Shp->CurrentShapeAddAction, true);
			Shp->CurrentShapeAddAction = NULL;
		}
		for(list<BaseItem*>::iterator shapeiterator = Shp->getParents().begin();shapeiterator != Shp->getParents().end(); shapeiterator++)
		{
			((Shape*)(*shapeiterator))->RemoveChild(Shp);
		}
		if(Shp->getMchild().size() > 0)
		{
			DeleteRelatedMeasurement(Shp);
		}
		if(Shp->getChild().size() > 0)
		{
			for(list<BaseItem*>::iterator shapeiterator = Shp->getChild().begin(); shapeiterator != Shp->getChild().end(); shapeiterator++)
			{
				ShapeWithList* Cshape = (ShapeWithList*)(*shapeiterator);
				deleteReferenceShape(Cshape);
			}
		}
		if(Shp->getGroupChilds().size() > 0)
		{
			for(list<BaseItem*>::iterator shapeiterator = Shp->getGroupChilds().begin();shapeiterator != Shp->getGroupChilds().end(); shapeiterator++)
			{
				ShapeWithList* Cshape = (ShapeWithList*)(*shapeiterator);
				deleteReferenceShape(Cshape);
			}
		}
		MAINDllOBJECT->getShapesList().removeItem((BaseItem*)Shp, true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD0003", __FILE__, __FUNCSIG__); }
}

Shape* RCadApp::getHighestParentOfGroup(Shape *s)
 {  
	 s->selected(false);
	 Shape* psh= (Shape *)(*(s->getGroupParent().begin()));
	 if(psh->getGroupParent().size() > 0)
	 {
		psh = getHighestParentOfGroup(psh);
	 }
	 return psh;
 }

//delete current drawing shape..
void RCadApp::deleteForEscape()
{
	try
	{
		RCollectionBase& selshapes = MAINDllOBJECT->getSelectedShapesList();
		RCollectionBase& rshapes = MAINDllOBJECT->getShapesList();
		RCollectionBase& rActions = MAINDllOBJECT->getActionsHistoryList();
		//RCollectionBase* delshapes = new RCollectionBase(false);
		while(selshapes.count() != 0)
		{
			RC_ITER i = selshapes.getList().begin();
			((Shape*)((*i).second))->IsValid(false);
			for each(BaseItem* act in ((ShapeWithList*)((*i).second))->PointAtionList)
				//rActions.removeItem(act, true);
				rActions.notifyRemove(act);
			//delshapes->addItem((Shape*)((*i).second), false);
			//ShapeWithList *csh = (ShapeWithList*)(*i).second;
			if (((ShapeWithList*)((*i).second))->CurrentShapeAddAction != NULL)
				MAINDllOBJECT->getActionsHistoryList().notifyRemove(((ShapeWithList*)((*i).second))->CurrentShapeAddAction);

			for (list<BaseItem*>::iterator shapeiterator = ((Shape*)((*i).second))->getParents().begin(); shapeiterator != ((Shape*)((*i).second))->getParents().end(); shapeiterator++)
			{
				((Shape*)(*shapeiterator))->RemoveChild(((Shape*)((*i).second)));
			}

			rshapes.removeItem(((*i).second), true);
		}
		//for (RC_ITER It = delshapes->getList().begin(); It != delshapes->getList().end(); It++)
		//{
		//	ShapeWithList *csh = (ShapeWithList*)(It->second);
		//	//for each(BaseItem* act in csh->PointAtionList)
		//	//	MAINDllOBJECT->getActionsHistoryList().notifyRemove(act);
		//	if (csh->CurrentShapeAddAction != NULL)
		//		MAINDllOBJECT->getActionsHistoryList().notifyRemove(csh->CurrentShapeAddAction);
		//}

		rshapes.clearSelection(true);
	/*	RCollectionBase& selshapes = MAINDllOBJECT->getSelectedShapesList();
		RCollectionBase& rshapes = MAINDllOBJECT->getShapesList();
		while(selshapes.count() != 0)
		{
			RC_ITER i = selshapes.getList().begin();
			((Shape*)((*i).second))->IsValid(false);
			for each(BaseItem* act in ((ShapeWithList*)((*i).second))->PointAtionList)
				((AddPointAction*)act)->deactivateAction();
			rshapes.removeItem(((*i).second));
		}*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1160", __FILE__, __FUNCSIG__); }
}

//Select all shapes..
bool RCadApp::SelectAll()
{
	try
	{
		if(getSelectedShapesList().count() == (getShapesList().count() - 2)) return false;
		Shape* s;
		for (RC_ITER i= getShapesList().getList().begin(); i != getShapesList().getList().end(); i++)
		{
			s = (Shape*)(*i).second;
			if(!s->Normalshape()) continue;
			if(!s->selected())
			{
				s->selected(true, false);
				getSelectedShapesList().addItem(getShapesList().getList()[s->getId()], false);
			}
		}
		getShapesList().notifySelection();
		Shape_Updated();
		char temp[10];
		itoa(getSelectedShapesList().getList().size(), &temp[0], 10);
		std::string Tempstr = (const char*)temp;
		MAINDllOBJECT->SetStatusCode("RCadAppSelectAll01", true, Tempstr);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1161", __FILE__, __FUNCSIG__); return false;}
}

//Deselect all shapes..
void RCadApp::DeselectAll(bool flag)
{
	try
	{
		getShapesList().clearSelection();
		Shape_Updated();
		if(flag)
			MAINDllOBJECT->SetStatusCode("Cleared Selection");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1162", __FILE__, __FUNCSIG__); }
}

//Set the selected shape as reference shape..
void RCadApp::SetShapeAsReference_Shape()
{
	try
	{
		if(getSelectedShapesList().count() == 1)
		{
			Shape* CShape = (Shape*)getShapesList().selectedItem();
			RC_ITER Citem = getShapesList().getList().begin();
			Citem++; Citem++;
			if((*Citem).second->getId() == CShape->getId())
			{
				if(!PPCALCOBJECT->ReferencePtAsHomePos())
				{
					ShowMsgBoxString("RCadSetShapeAsReference_Shape01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
					return;
				}
				else
				{
					if(!PPCALCOBJECT->ReferenceDotIstakenAsHomePosition())
					{
						ShowMsgBoxString("RCadSetShapeAsReference_Shape02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
						return;
					}
				}
				if(!CShape->ShapeAsfasttrace())
					PPCALCOBJECT->FirstShapeisSetAsReferenceShape(!CShape->ShapeAsReferenceShape());
			}
			if(CShape->ShapeAsfasttrace()) return;
			CShape->ShapeAsReferenceShape(!CShape->ShapeAsReferenceShape());
			for each(RAction* CAction in ((ShapeWithList*)CShape)->PointAtionList)
			{
				if(CAction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
				{
					FramegrabBase* Cfg = ((AddPointAction*)CAction)->getFramegrab();
					Cfg->ReferenceAction(CShape->ShapeAsReferenceShape());
				}
			}
			SelectedShape_Updated();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1163", __FILE__, __FUNCSIG__); }
}

//Set the selected shpae for two step homing,..
void RCadApp::SetShapeForTwoStepHmoing(bool TwoStepHoming)
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning()) return;
		/*if(PPCALCOBJECT->ReferenceDotIstakenAsHomePosition())
		{
			ShowMsgBoxString("RCadSetShapeForTwoStepHmoing01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			SetStatusCode("RCadApp06");
			return;
		}*/
		if(getSelectedShapesList().count() != 1)
		{
			SetStatusCode("RCadApp07");
			return;
		}
		Shape* CShape = (Shape*)getShapesList().selectedItem();
		if(TwoStepHoming)
		{
			if((int)ShapesForBaseAlignment.size() > 0)
			{
				SetStatusCode("RCadApp08");
				return;
			}
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::LINE || CShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || CShape->ShapeType == RapidEnums::SHAPETYPE::ARC ||
				CShape->ShapeType == RapidEnums::SHAPETYPE::RPOINT)
			{
				if(CShape->ShapeAsfasttrace()) 
				{
					SetStatusCode("RCadApp09");
					return;
				}

				CShape->ShapeUsedForTwoStepHoming(!CShape->ShapeUsedForTwoStepHoming());
				int ShapeCount = 0;
				for(RC_ITER Sitem = getShapesList().getList().begin(); Sitem != getShapesList().getList().end(); Sitem++)
				{
					Shape* Csh = (Shape*)(*Sitem).second;
					if(Csh->ShapeUsedForTwoStepHoming())ShapeCount++;
				}
				if(ShapeCount > 2)
				{
					CShape->ShapeUsedForTwoStepHoming(false);
					SetStatusCode("RCadApp10");
					ShowMsgBoxString("RCadSetShapeForTwoStepHmoing02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
					return;
				}
				std::string Tempstr = (const char*)CShape->getModifiedName();
				if(CShape->ShapeUsedForTwoStepHoming())
				{
					ShapesForTwoStepHoming[CShape->getId()] = CShape;
					SetStatusCode("RCadAppSetShapeForTwoStepHmoing01", true, Tempstr);
				}
				else
				{
					ShapesForTwoStepHoming.erase(CShape->getId());
					SetStatusCode("RCadAppSetShapeForTwoStepHmoing02", true, Tempstr);
				}
				if(ShapeCount == 2)
					PPCALCOBJECT->SelectedShapesForTwoStepHoming(true);
				for each(RAction* CAction in ((ShapeWithList*)CShape)->PointAtionList)
				{
					if(CAction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
					{
						FramegrabBase* Cfg = ((AddPointAction*)CAction)->getFramegrab();
						Cfg->ActionForTwoStepHoming(CShape->ShapeUsedForTwoStepHoming());
					}
				}
			}
			else
			{
				SetStatusCode("RCadApp11");
			}
		}
		else
		{
			if((int)ShapesForTwoStepHoming.size() > 0)
			{
				SetStatusCode("RCadApp12");
				return;
			}
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::PLANE || CShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || 
				CShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D || CShape->ShapeType == RapidEnums::SHAPETYPE::CYLINDER || 
				CShape->ShapeType == RapidEnums::SHAPETYPE::CONE || CShape->ShapeType == RapidEnums::SHAPETYPE::LINE || 
				CShape->ShapeType == RapidEnums::SHAPETYPE::LINE3D || 
				CShape->ShapeType == RapidEnums::SHAPETYPE::RPOINT || CShape->ShapeType == RapidEnums::SHAPETYPE::RPOINT3D)
			{
				if(CShape->ShapeAsfasttrace()) 
				{
					SetStatusCode("RCadApp13");
					return;
				}

				CShape->ShapeUsedForTwoStepHoming(!CShape->ShapeUsedForTwoStepHoming());
				int ShapeCount = 0;
				for(RC_ITER Sitem = getShapesList().getList().begin(); Sitem != getShapesList().getList().end(); Sitem++)
				{
					Shape* Csh = (Shape*)(*Sitem).second;
					if(Csh->ShapeUsedForTwoStepHoming()) ShapeCount++;
				}
				if(ShapeCount > 3)
				{
					CShape->ShapeUsedForTwoStepHoming(false);
					SetStatusCode("RCadApp10");
					ShowMsgBoxString("RCadSetShapeForTwoStepHmoing03", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
					return;
				}
				std::string Tempstr = (const char*)CShape->getModifiedName();
				if(CShape->ShapeUsedForTwoStepHoming())
				{
					ShapesForBaseAlignment[CShape->getId()] = CShape;
					
					SetStatusCode("RCadAppSetShapeForTwoStepHmoing03",true, Tempstr);
				}
				else
				{
					ShapesForBaseAlignment.erase(CShape->getId());
					
					SetStatusCode("RCadAppSetShapeForTwoStepHmoing03",true, Tempstr);
				}
				if(ShapeCount == 3)
				{
					PPCALCOBJECT->SelectedShapesForTwoStepHoming(true);
					SetShapeForUCSBasePlane(ShapesForBaseAlignment);
				}
				for each(RAction* CAction in ((ShapeWithList*)CShape)->PointAtionList)
				{
					if(CAction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
					{
						FramegrabBase* Cfg = ((AddPointAction*)CAction)->getFramegrab();
						Cfg->ActionForTwoStepHoming(CShape->ShapeUsedForTwoStepHoming());
					}
				}
			}
			else
			{
				SetStatusCode("RCadApp14");
			}
		}
		SelectedShape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1164", __FILE__, __FUNCSIG__); }
}

void RCadApp::CreateReferenceSphere()
{
	try
	{
		MAINDllOBJECT->ReferenceDotShape = new Sphere();
		MAINDllOBJECT->ReferenceDotShape->DoNotSaveInPP = true;
		MAINDllOBJECT->ReferenceDotShape->setId(MAINDllOBJECT->ToolAxisLineId - 1);
		MAINDllOBJECT->getCurrentUCS().getShapes().addItem(MAINDllOBJECT->ReferenceDotShape);
		SetShapeForReferenceDot();
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1165", __FILE__, __FUNCSIG__); }
}

void RCadApp::CreateRefernceLineShape()
{
	try
	{
		MAINDllOBJECT->ReferenceLineShape = new Line();
		MAINDllOBJECT->ReferenceLineShape->setOriginalName(RMATH2DOBJECT->StringToWString("Refline"));
		MAINDllOBJECT->ReferenceLineShape->DoNotSaveInPP = true;
		MAINDllOBJECT->ReferenceLineShape->setId(MAINDllOBJECT->ToolAxisLineId - 3);
		MAINDllOBJECT->getCurrentUCS().getShapes().addItem(MAINDllOBJECT->ReferenceLineShape);
		for each(BaseItem* act in ((ShapeWithList*)(ReferenceLineShape))->PointAtionList)
		{
			actionhistory.removeItem(act, true, true);
		}
		((ShapeWithList*)(ReferenceLineShape))->PointAtionList.clear();
		PPCALCOBJECT->ReferenceLineAsRefAngle(true);
		ReferenceLineShape->Normalshape(true);
		ReferenceLineShape->selected(true);
		if(((ShapeWithList*)ReferenceLineShape)->CurrentShapeAddAction != NULL)
				getActionsHistoryList().removeItem(((ShapeWithList*)ReferenceLineShape)->CurrentShapeAddAction, true, true);
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1166", __FILE__, __FUNCSIG__); }
}

void RCadApp::CreateReferenceCylinder()
{
	try
	{	if (MAINDllOBJECT->ReferenceDotShape == NULL)
		{
			MAINDllOBJECT->ReferenceDotShape = new Cylinder();
			MAINDllOBJECT->ReferenceDotShape->setId(MAINDllOBJECT->ToolAxisLineId - 2);
			MAINDllOBJECT->getCurrentUCS().getShapes().addItem(MAINDllOBJECT->ReferenceDotShape);
		}

		MAINDllOBJECT->ReferenceDotShape->DoNotSaveInPP = true;
		//int taID = MAINDllOBJECT->ToolAxisLineId - 1;
		//if (taID < 0) taID = 0;
		//MAINDllOBJECT->ReferenceDotShape->setId(taID);
		SetShapeForReferenceDot(true);
		MAINDllOBJECT->selectShape(MAINDllOBJECT->ToolAxisLineId - 2, false);
		MAINDllOBJECT->ResetthePoints_selectedshape();
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1168", __FILE__, __FUNCSIG__); }
}

void RCadApp::SetShapeForReferenceDot(bool FirstReferenceDot)
{
	if(FirstReferenceDot)
	{
		if(ReferenceDotShape == NULL) return;
		if(ReferenceDotShape->ShapeType == RapidEnums::SHAPETYPE::SPHERE || ReferenceDotShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D  
			|| ReferenceDotShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || ReferenceDotShape->ShapeType == RapidEnums::SHAPETYPE::RPOINT3D 
			|| ReferenceDotShape->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
		{
			for each(BaseItem* act in ((ShapeWithList*)(ReferenceDotShape))->PointAtionList)
			{
				actionhistory.removeItem(act, true, true);
			}
			((ShapeWithList*)(ReferenceDotShape))->PointAtionList.clear();
			PPCALCOBJECT->ReferenceDotIstakenAsHomePosition(true);
			ReferenceDotShape->Normalshape(true);
			ReferenceDotShape->UpdateHomePostionFromReferenceDotShape();
			ReferenceDotShape->selected(true);
			if(((ShapeWithList*)ReferenceDotShape)->CurrentShapeAddAction != NULL)
				 getActionsHistoryList().removeItem(((ShapeWithList*)ReferenceDotShape)->CurrentShapeAddAction, true, true);
		}
		else
		{
		   PPCALCOBJECT->ReferenceDotIstakenAsHomePosition(false);
		   ReferenceDotShape = NULL;
		}
	}
	else
	{
		if(ReferenceDotShape == NULL) return;
		if(SecondReferenceDotShape == NULL) return;
		if(SecondReferenceDotShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D || SecondReferenceDotShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
		{
			for each(BaseItem* act in ((ShapeWithList*)(SecondReferenceDotShape))->PointAtionList)
			{
				actionhistory.removeItem(act, true, true);
			}
			((ShapeWithList*)(SecondReferenceDotShape))->PointAtionList.clear();
			PPCALCOBJECT->ReferenceDotIstakenAsSecondHomePos(true);
			SecondReferenceDotShape->Normalshape(true);
			SecondReferenceDotShape->UpdateHomePostionFromReferenceDotShape(false);
			SecondReferenceDotShape->selected(true);
			if(((ShapeWithList*)SecondReferenceDotShape)->CurrentShapeAddAction != NULL)
				 getActionsHistoryList().removeItem(((ShapeWithList*)SecondReferenceDotShape)->CurrentShapeAddAction, true, true);
		}
		else
		{
		   PPCALCOBJECT->ReferenceDotIstakenAsSecondHomePos(false);
		   SecondReferenceDotShape = NULL;
		}
	}
}

//Select the measurement as per id...//
void RCadApp::selectMeasurement(int id, bool multi, bool updateVideoGraphics)
{
	try
	{
		if(PPCALCOBJECT->PartprogramisLoading()) return;
		dontUpdateGraphcis = true;
		getDimList().multiselect(multi);
		DimBase* Cdim = (DimBase*)getDimList().getList()[id];
		if(!multi)
		{
			if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_THREAD)
			{
				DimThread* CthrdDim;
				if(Cdim->ChildMeasurementType())
					CthrdDim = (DimThread*)getDimList().getList()[((DimChildMeasurement*)Cdim)->ParentMeasureId()];
				else
					CthrdDim = (DimThread*)Cdim;
				THREADCALCOBJECT->CurrentSelectedThreadAction = CthrdDim->PointerToAction;
				THREADCALCOBJECT->ThreadShape_Updated();
			}
			else
				THREADCALCOBJECT->CurrentSelectedThreadAction = NULL;
			if(Cdim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_DEPTH)
				FOCUSCALCOBJECT->CurrentDepthMeasure = Cdim;
			else
				FOCUSCALCOBJECT->CurrentDepthMeasure = NULL;
			getDimList().clearSelection(false);
			getDimList().getList()[id]->selected(true);
		}
		else
		{
			if(getDimList().getList()[id]->selected())
				getDimList().getList()[id]->selected(false);
			else
				getDimList().getList()[id]->selected(true);
		}
		getDimList().multiselect(false);
		dontUpdateGraphcis = false;
		if(updateVideoGraphics) Measurement_updated();
	}
	catch(...){ dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1169", __FILE__, __FUNCSIG__); }
}

void RCadApp::SetShapeForUCSBasePlane(map<int, Shape*> ShapesForBasePlane)
{
	try
	{
		if(CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::UCS_PLANE)
		{
			MAINDllOBJECT->changeUCS(0);
			MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::UCS_PLANE);
		}
		((UCSPlaneHandle*)currentHandler)->AddShape(ShapesForBasePlane);
	}
	catch(...){ dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1170", __FILE__, __FUNCSIG__); } //called 0101 before
}

//Set shape(basically 2D line) for checking if its close to horizontal or not
void RCadApp::SetShapeAsRotaryEntity()
{
	try
	{
		if(getShapesList().getSelected().count() == 1)
		{
			Shape* CShape = (Shape*)getShapesList().selectedItem();
			CShape->ShapeAsRotaryEntity(!CShape->ShapeAsRotaryEntity());
			SelectedShape_Updated();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1171", __FILE__, __FUNCSIG__); }
}

//Select multiple measurements..
void RCadApp::selectMeasurement(list<int> *PtIdList)
{
	try
	{
		if(PPCALCOBJECT->PartprogramisLoading()) return;
		if(PtIdList->size() == 1)
		{
			selectMeasurement((*PtIdList->begin()), false);
		}
		else
		{
			dontUpdateGraphcis = true;
			getDimList().clearSelection(true);
			for each(int Shid in *PtIdList)
			{
				BaseItem* Citem = getDimList().getList()[Shid];
				Citem->selected(true, false);
				getSelectedDimList().addItem(Citem, false);
				if(((DimBase*)Citem)->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE)
				{
				}
				/*if(((DimBase*)Citem)->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE)
				{
					DimPointCoordinate* dimPt = (DimPointCoordinate*)Citem;
					BaseItem* Citem1 = getDimList().getList()[dimPt->DimPointX->getId()];
					Citem1->selected(true, false);
					getSelectedDimList().addItem(Citem1, false);
					BaseItem* Citem2 = getDimList().getList()[dimPt->DimPointY->getId()];
					Citem2->selected(true, false);
					getSelectedDimList().addItem(Citem2, false);
					BaseItem* Citem3 = getDimList().getList()[dimPt->DimPointZ->getId()];
					Citem3->selected(true, false);
					getSelectedDimList().addItem(Citem3, false);
				}
				else
				{
					Citem->selected(true, false);
					getSelectedDimList().addItem(Citem, false);
				}*/		
			}
			getDimList().SetSeelectedItem();
			getDimList().notifySelection();
			dontUpdateGraphcis = false;
			Measurement_updated();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1172", __FILE__, __FUNCSIG__); }
}

//delete measurement..//
void RCadApp::deleteMeasurement() 
{
	try
	{
		if(MAINDllOBJECT->getSelectedDimList().count() == 0) return;
		DeleteMeasureAction* dma = new DeleteMeasureAction();
		addAction(dma);
		int cnt = getDimList().getList().size();
		if(cnt > 0)
		{
			RC_ITER i = getDimList().getList().end();
			i--;
			DimBase* sh = dynamic_cast<DimBase*>((*i).second);
			sh->selected(true);
		}
		Measurement_updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1173", __FILE__, __FUNCSIG__); }
}

//Edit measurement mode..//
void RCadApp::setEditmeasurment()
{
	try
	{
		if(getSelectedDimList().getList().size() == 1)
		{
			CurrentSelectedDim = (DimBase*)getDimList().selectedItem();
			if(CurrentSelectedDim->ChildMeasurementType() && CurrentSelectedDim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE)
			{
				DimChildMeasurement* CdimPt = (DimChildMeasurement*)CurrentSelectedDim;
				CurrentSelectedDim = (DimBase*)getDimList().getList()[CdimPt->ParentMeasureId()];
			}
			EditmeasurementMode(true);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1174", __FILE__, __FUNCSIG__); }
}

//Change the measurement mode.. linear <=> normal
void RCadApp::ChangeMeasurementMode()
{
	try
	{
		DimBase* Cdim = (DimBase*)getDimList().selectedItem();
		Cdim->LinearMeasurementMode(!Cdim->LinearMeasurementMode());
		Cdim->UpdateMeasurement();
		SelectedMeasurement_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1175", __FILE__, __FUNCSIG__); }
}

//Select shapes inside a rectangle....//
void RCadApp::selectShapesWindow(Vector &p1, Vector &p2, bool multi)
{
	try
	{
		Shape *s = NULL, *s1 = NULL;
		for (RC_ITER i= getShapesList().getList().begin(); i != getShapesList().getList().end(); i++)
		{
			s = (Shape*)(*i).second;
			if(!s->Normalshape()) continue;
			if(!s->selected())
			{
				if(s->Shape_IsInWindow(p1, p2))
				{
					s->selected(true, false);
					getSelectedShapesList().addItem(getShapesList().getList()[s->getId()], false);
					s1 = s;
				}
			}
		}
		if(s1 != NULL)
			getShapesList().notifySelection();
		Shape_Updated();
		char temp[10];
		itoa(getSelectedShapesList().getList().size(), &temp[0], 10);
		std::string Tempstr = (const char*)temp;
		MAINDllOBJECT->SetStatusCode("RCadAppSelectAll01", true, Tempstr);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1176", __FILE__, __FUNCSIG__); }
}

//Select measurements inside a rectangle..//
void RCadApp::selectMeasurewindow(Vector &p1, Vector &p2, bool multi)
{
	try
	{
		DimBase *dim = NULL, *dim1 = NULL;
		for (RC_ITER i= getDimList().getList().begin(); i != getDimList().getList().end(); i++)
		{
			dim = (DimBase*)(*i).second;
			if(!dim->selected())
			{
				if(dim->Measure_IsInWindow(p1, p2))
				{
					dim->selected(true, false);
					getSelectedDimList().addItem(getDimList().getList()[dim->getId()], false);
					dim1 = dim;
				}
			}
		}
		if(dim1 != NULL)
			getDimList().notifySelection();
		Measurement_updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1177", __FILE__, __FUNCSIG__); }
}

void RCadApp::selectFGPointsWithTransformation(Vector &p1, Vector &p2, std::list<int> *PtIdList, double *transMatrix)
{
	try
	{
		int s1[2] = {4,4}, s2[2] = {4,1};
		double points13[8] = {p1.getX(), p1.getY(), 0, 1, p2.getX(), p2.getY(), 0, 1}, endp13[8] = {0};
	
		for(RC_ITER ShItem = getShapesList().getList().begin(); ShItem != getShapesList().getList().end(); ShItem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)(*ShItem).second;
			if(!CShape->Normalshape())continue;
			else if(!ShowHideFGPoints() && CShape->ShapeType != RapidEnums::SHAPETYPE::CLOUDPOINTS) continue;
	 
			for(PC_ITER Item = CShape->PointsList->getList().begin(); Item != CShape->PointsList->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				double cPoint[4] = {Spt->X, Spt->Y, Spt->Z, 1}, rPnt[4] = {0};
				RMATH3DOBJECT->MultiplyMatrix1(transMatrix, s1, cPoint, s2, rPnt);
				if(SinglePoint(rPnt[0], rPnt[1], rPnt[2]).Point_IsInWindow(points13[0], points13[5], points13[4], points13[1]))
				{
					if(!Spt->IsSelected)
					{
						PtIdList->push_back(Spt->PtID);
						Spt->IsSelected = true;
						CShape->PointsList->SelectedPointsCnt++;
					}
				}
			}
			char temp[10];
			itoa(CShape->PointsList->SelectedPointsCnt, &temp[0], 10);
			std::string Tempstr = (const char*)temp;
			SetStatusCode("RCadAppselectFGPoints01", true, Tempstr);
		}
	}
	catch(...)
	{ 
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1178", __FILE__, __FUNCSIG__); 
	}
}

//Select fg points
void RCadApp::selectFGPointsFromShapes(Vector &p1, Vector &p2, FramegrabBase* cAction)
{
	try
	{
		list<int> PtIdList;
		for(RC_ITER ShItem = getShapesList().getList().begin(); ShItem != getShapesList().getList().end(); ShItem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)(ShItem->second);
			if(CShape->ShapeType != RapidEnums::SHAPETYPE::CLOUDPOINTS)continue;	
			for(PC_ITER Item = CShape->PointsList->getList().begin(); Item != CShape->PointsList->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				if(Spt->Point_IsInWindow(p1.getX(), p2.getY(), p2.getX(), p1.getY()))
				{
					 cAction->AllPointsList.Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z, Spt->R, Spt->Pdir));
					 PtIdList.push_back(Spt->PtID);
				}
			}
	/*		for each(int Ptid in  PtIdList)
			{
				for each(BaseItem* item in CShape->PointAtionList)
				{
					AddPointAction* CAction = (AddPointAction*)item;
					CAction->getFramegrab()->PointActionIdList.remove(Ptid);
				}
			}
			list<BaseItem*> :: iterator Paction = CShape->PointAtionList.begin();
			while(Paction != CShape->PointAtionList.end())
			{
				AddPointAction* CAction = (AddPointAction*)(*Paction);
				int Cnt = CAction->getFramegrab()->PointActionIdList.size();
				if(Cnt == 0)
				{
					getActionsHistoryList().removeItem((*Paction), true, true);
					CShape->PointAtionList.remove((*Paction));
					Paction = CShape->PointAtionList.begin();
				}
				else
					Paction++;
			}
			CShape->PointsList->SelectedPointsCnt = 0;*/
		/*	if(!MAINDllOBJECT->DemoMode())
				CShape->Delete_Points(&PtIdList);*/
			Sleep(20);
		}						
		update_RcadGraphics(true);
		update_VideoGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1179", __FILE__, __FUNCSIG__); }
}

void RCadApp::selectFGPointsInCircularFrameFromShapes(double r1, double r2, double* center, double *endPoint1, double *endPoint2, FramegrabBase* cAction)
{
	try
	{
		list<int> PtIdList;
		double max = 0, min = 0;
		if(r1 > r2)
		{
			max = r1; 
			min = r2;
		}
		else
		{
			max = r2;
			min = r1;
		}
		for(RC_ITER ShItem = getShapesList().getList().begin(); ShItem != getShapesList().getList().end(); ShItem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)(ShItem->second);
			if(CShape->ShapeType != RapidEnums::SHAPETYPE::CLOUDPOINTS)continue;	
			for(PC_ITER Item = CShape->PointsList->getList().begin(); Item != CShape->PointsList->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				double tmp[2] = {Spt->X, Spt->Y};
				double dist = RMATH2DOBJECT->Pt2Pt_distance(tmp, center);
				if(min <= dist && max >= dist)
				{
					if(*endPoint2 != 0)
					{
	/*				   double ang, ang1, ang2 = 0; 
					   ang1 = RMATH2DOBJECT->ray_angle(endPoint1, center);
					   ang2 = RMATH2DOBJECT->ray_angle(endPoint2, center);*/
					   double ang = RMATH2DOBJECT->ray_angle(tmp, center);
					   if(ang > *endPoint1)
					   {
						 if(ang > *endPoint2) continue;
					   }
				/*	   else if(ang < *endPoint2) continue;*/
					}
					cAction->AllPointsList.Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z, Spt->R, Spt->Pdir));
					PtIdList.push_back(Spt->PtID);
				}
			}
		/*	for each(int Ptid in  PtIdList)
			{
				for each(BaseItem* item in CShape->PointAtionList)
				{
					AddPointAction* CAction = (AddPointAction*)item;
					CAction->getFramegrab()->PointActionIdList.remove(Ptid);
				}
			}
			list<BaseItem*> :: iterator Paction = CShape->PointAtionList.begin();
			while(Paction != CShape->PointAtionList.end())
			{
				AddPointAction* CAction = (AddPointAction*)(*Paction);
				int Cnt = CAction->getFramegrab()->PointActionIdList.size();
				if(Cnt == 0)
				{
					getActionsHistoryList().removeItem((*Paction), true, true);
					CShape->PointAtionList.remove((*Paction));
					Paction = CShape->PointAtionList.begin();
				}
				else
					Paction++;
			}
			CShape->PointsList->SelectedPointsCnt = 0;*/
		/*	if(!MAINDllOBJECT->DemoMode())
				CShape->Delete_Points(&PtIdList);*/
			Sleep(20);
		}						
		update_RcadGraphics(true);
		update_VideoGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1180", __FILE__, __FUNCSIG__); }
}

void RCadApp::selectFGPointsInAngularRectFrameFromShapes(double* point1, double* point2, double width, FramegrabBase* cAction)
{
	try
	{
		list<int> PtIdList;
		double height = RMATH2DOBJECT->Pt2Pt_distance(point1, point2);
		double SinTh = (point2[0] - point1[0]) / sqrt(pow((point2[0] - point1[0]),2) + pow((point2[1] - point1[1]),2));
		double CosTh = (point2[1] - point1[1]) / sqrt(pow((point2[0] - point1[0]),2) + pow((point2[1] - point1[1]),2));
		for(RC_ITER ShItem = getShapesList().getList().begin(); ShItem != getShapesList().getList().end(); ShItem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)(ShItem->second);
			if(CShape->ShapeType != RapidEnums::SHAPETYPE::CLOUDPOINTS)continue;
			for(PC_ITER Item = CShape->PointsList->getList().begin(); Item != CShape->PointsList->getList().end(); Item++)
			{
				SinglePoint* Spt = (*Item).second;
				double XTransformed = (Spt->X - point1[0]) * CosTh - (Spt->Y - point1[1]) * SinTh;
				double YTransformed = (Spt->X - point1[0]) * SinTh + (Spt->Y - point1[1]) * CosTh;
				if(XTransformed <= width && XTransformed >= -width)
				{
					if(YTransformed <= height && YTransformed >= 0)
					{
						cAction->AllPointsList.Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z, Spt->R, Spt->Pdir));		
						PtIdList.push_back(Spt->PtID);
					}
				}
			}		
			//for each(int Ptid in  PtIdList)
			//{
			//	for each(BaseItem* item in CShape->PointAtionList)
			//	{
			//		AddPointAction* CAction = (AddPointAction*)item;
			//		CAction->getFramegrab()->PointActionIdList.remove(Ptid);
			//	}
			//}
			//list<BaseItem*> :: iterator Paction = CShape->PointAtionList.begin();
			//while(Paction != CShape->PointAtionList.end())
			//{
			//	AddPointAction* CAction = (AddPointAction*)(*Paction);
			//	int Cnt = CAction->getFramegrab()->PointActionIdList.size();
			//	if(Cnt == 0)
			//	{
			//		getActionsHistoryList().removeItem((*Paction), true, true);
			//		CShape->PointAtionList.remove((*Paction));
			//		Paction = CShape->PointAtionList.begin();
			//	}
			//	else
			//		Paction++;
			//}
			//CShape->PointsList->SelectedPointsCnt = 0;
	/*		if(!MAINDllOBJECT->DemoMode())
				CShape->Delete_Points(&PtIdList);*/
			Sleep(20);
		}						
		update_RcadGraphics(true);
		update_VideoGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1181", __FILE__, __FUNCSIG__);} // old code RCAD00121
}

void RCadApp::selectFGPoints(list<int> *PtIdList)
{
	try
	{
		ShapeWithList *Cshape = (ShapeWithList*)getShapesList().selectedItem();
		if(getSelectedShapesList().count() == 1)
		{
			for(PC_ITER Item = Cshape->PointsList->getList().begin(); Item != Cshape->PointsList->getList().end(); Item++)
				(*Item).second->IsSelected = false;
			Cshape->PointsList->SelectedPointsCnt = 0;
			for each(int Ptid in *PtIdList)
			{
				Cshape->PointsList->getList()[Ptid]->IsSelected = true;
				Cshape->PointsList->SelectedPointsCnt++;
			}
			Shape_Updated();
		}
		else
			SetStatusCode("RCadApp15");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1182", __FILE__, __FUNCSIG__); }
}

//delete selected FG points
void RCadApp::deleteFGPoints()
{
	try
	{
		list<int> PtIdList;
		ShapeWithList *Cshape = (ShapeWithList*)getShapesList().selectedItem();
		if(Cshape->PointsList->SelectedPointsCnt == 0) return;
		if(getSelectedShapesList().count() == 1)
		{
			if(Cshape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE && ((Circle*)Cshape)->CircleType == RapidEnums::CIRCLETYPE::PCDCIRCLE)
			{
				Circle* Ccirc = (Circle*)Cshape;
				((PCDCircle*)Ccirc)->RemoveSelectedShapes();
			}
			else
			{
				
				for(PC_ITER Item = Cshape->PointsList->getList().begin(); Item != Cshape->PointsList->getList().end(); Item++)
				{
					SinglePoint* Spt = (*Item).second;
					if(Spt->IsSelected)
						PtIdList.push_back(Spt->PtID);
				}
				
				for each(int Ptid in  PtIdList)
				{
					for each(BaseItem* item in Cshape->PointAtionList)
					{
						AddPointAction* CAction = (AddPointAction*)item;
						CAction->getFramegrab()->PointActionIdList.remove(Ptid);
					}
				}
				list<BaseItem*> :: iterator Paction = Cshape->PointAtionList.begin();
				while(Paction != Cshape->PointAtionList.end())
				{
					AddPointAction* CAction = (AddPointAction*)(*Paction);
					int Cnt = CAction->getFramegrab()->PointActionIdList.size();
					if(Cnt == 0)
					{
						getActionsHistoryList().removeItem((*Paction), true, true);
						Cshape->PointAtionList.remove((*Paction));
						Paction = Cshape->PointAtionList.begin();
					}
					else
						Paction++;
				}
				Cshape->PointsList->SelectedPointsCnt = 0;
				Cshape->Delete_Points(&PtIdList);
				Sleep(20);
				update_RcadGraphics(true);
				update_VideoGraphics();
			}
		}
		else
			SetStatusCode("RCadApp16");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1183", __FILE__, __FUNCSIG__); }
}

//delete points from selected shape list
void RCadApp::DeletePointList_SelectedShape(list<int> *PtIdList)
{
	try
	{
		ShapeWithList* sh = (ShapeWithList*)getShapesList().selectedItem();
		if(sh != NULL)
		{
			sh->Delete_Points(PtIdList);
			for each(int Ptid in  *PtIdList)
			{
				for each(BaseItem* item in sh->PointAtionList)
				{
					AddPointAction* CAction = (AddPointAction*)item;
					CAction->getFramegrab()->PointActionIdList.remove(Ptid);
				}
			}
			list<BaseItem*> :: iterator Paction = sh->PointAtionList.begin();
			while(Paction != sh->PointAtionList.end())
			{
				AddPointAction* CAction = (AddPointAction*)(*Paction);
				int Cnt = CAction->getFramegrab()->PointActionIdList.size();
				if(Cnt == 0)
				{
					getActionsHistoryList().removeItem((*Paction), true, true);
					sh->PointAtionList.remove((*Paction));
					Paction = sh->PointAtionList.begin();
				}
				else
					Paction++;
			}
			Shape_Updated();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1184", __FILE__, __FUNCSIG__); }
}

//Reset the point to selected shape....//
void RCadApp::ResetthePoints_selectedshape()
{
	try
	{
		ShapeWithList* Cshape = (ShapeWithList*)getShapesList().selectedItem();
		if(Cshape != NULL)
		{
			for each(BaseItem* act in Cshape->PointAtionList)
				getActionsHistoryList().removeItem(act, true, true);
			Cshape->ResetCurrentParameters();
			currentHandler->setClicksDone(0);
			Shape_Updated();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1185", __FILE__, __FUNCSIG__); }
}

//Set the color for selected shapes..//
void RCadApp::SetshapeColor(int r, int g, int b)
{
	try
	{
		for(RC_ITER item = getSelectedShapesList().getList().begin(); item != getSelectedShapesList().getList().end(); item++)
		{
			Shape* Sh = (Shape*)(*item).second;
			Sh->selected(false, false);
			Sh->ShapeColor.set((double)r/255.0, (double)g/255.0, (double)b/255.0);	
		}
		DeselectAll();
		Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1186", __FILE__, __FUNCSIG__); }
}

//Set the color for selected measurements..//
void RCadApp::SetMeasurementColor(int r, int g, int b)
{
	try
	{
		for(RC_ITER item = getSelectedDimList().getList().begin(); item != getSelectedDimList().getList().end(); item++)
		{
			DimBase* dim = (DimBase*)(*item).second;
			dim->selected(false, false);
			dim->MeasureColor.set((double)r/255.0, (double)g/255.0, (double)b/255.0);
		}
		getSelectedDimList().clearAll();
		getDimList().notifySelection();
		Measurement_updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1187", __FILE__, __FUNCSIG__); }
}

//Change draw mode type of partial shape.
void RCadApp::ChangePartialShapeDrawMode(int type)
{
	try
	{
		for(RC_ITER item = getSelectedShapesList().getList().begin(); item != getSelectedShapesList().getList().end(); item++)
		{
			Shape* Sh = (Shape*)(*item).second;
			Sh->PartialShapeDrawMode(type);
		}
		Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1188", __FILE__, __FUNCSIG__); }
}

//Show FG points mode...//
void RCadApp::ShowFGPoints(bool flag)
{
	try
	{
		ShowHideFGPoints(flag);
		Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1189", __FILE__, __FUNCSIG__); }
}

//Change Measurement Font Size...
void RCadApp::SeMeasurementFontSize(int Fsize)
{
	try
	{
		for(RC_ITER item = getSelectedDimList().getList().begin(); item != getSelectedDimList().getList().end(); item++)
		{
			DimBase* dim = (DimBase*)(*item).second;
			dim->RcadFont_ScaleFactor(Fsize);
			dim->VideoFont_ScaleFactor(Fsize);
			dim->UpdateMeasurement();
		}
		Measurement_updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1190", __FILE__, __FUNCSIG__); }
}

//Add to the action history...//
void RCadApp::addAction(RAction *action)
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			std::wofstream cfile;
			std::string Pppath = currentPath + "\\Program_Action.txt";
			cfile.open(Pppath.c_str(),std::ios_base::app);
			cfile << endl;
			cfile << (int)action->CurrentActionType << endl;
			cfile << endl;
			cfile.close();
		}
		if (undone)
		{
			RC_ITER i = currentaction;	//used later for removing container items
			//coz inside the while loop only the actual action objects are deleted

			while(currentaction != getActionsHistoryList().getList().end())
			{
				//take care of delete all the actions beyond the 
				//current point including the current one
				delete (*currentaction).second;	//Here notice that I'm deleting only the object, not the container item.
				currentaction++;
			}

			//now remove the pointer items from the container RCollectionBase <map>
			getActionsHistoryList().getList().erase(i, getActionsHistoryList().getList().end());
		}
		LastAdded_Action = action;
		//add the action to the list and execute it.
		if(action->execute())
		{
			//if action is the first point action in the action history list, then save camera image.
			if(CameraConnected())
			{
				bool firstpt_action = true;
				if(action->CurrentActionType != RapidEnums::ACTIONTYPE::ADDPOINTACTION)
					firstpt_action = false;

				if(firstpt_action)
				{
					map <int, BaseItem*> mp_base = getActionsHistoryList().getList();
					for(RC_ITER ActItem = mp_base.begin(); ActItem != mp_base.end(); ActItem++)
					{
						if(((RAction*)((*ActItem).second))->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
						{
							firstpt_action = false;
							break;
						}
					}
				}
				if(firstpt_action)
				{
					std::string ImFilePath = currentPath + "\\FirstPointActionImage.jpg";
					SaveCameraImage(ImFilePath);
				}
			}
			getActionsHistoryList().addItem(action);

			if(PPCALCOBJECT->PPAlignEditFlag())
			{
				if(AddAction_PPEditMode)
					ArrangeActionForPPEdit();
			}
			if(this->RcadZoomExtents())
			{
				if(action->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
					getWindow(1).ZoomToExtents(1);
			}
			if(!ActionSavingFlag)
			{
				if(!PPCALCOBJECT->PartprogramisLoadingEditMode() && !PPCALCOBJECT->PartprogramisLoading() && !PPCALCOBJECT->IsPartProgramRunning())
				{
					//if(action->ActionStatus())
					//	_beginthread(&SaveActions, 0, NULL);
				}
			}
			undone = false;
		}
		else
			delete action;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1191", __FILE__, __FUNCSIG__); }
}

//Undo the last step..//
bool RCadApp::Undo()
{
	try
	{
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
		if(getActionsHistoryList().count() == 0) 
			return false;
		if(undone == false)
		{
			DUMMYHANDLE = CURRENTHANDLE;
			currentaction = getActionsHistoryList().getList().end();
		}
		if(currentaction == getActionsHistoryList().getList().begin())
			return false;
		else
		{
			bool currentUndone = false;
			currentaction--;
			Shape* sh;
			int i = 0;
			RAction* atn = static_cast<RAction*>((*currentaction).second);
			if(atn->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				switch(((AddPointAction*)atn)->getFramegrab()->FGtype)
				{
				case RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE:
					CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER;
					MeasurementMode(false);
					sh = (Shape*)(((AddPointAction*)atn)->getShape());
					if(Shapeflag && !sh->ShapeAsfasttrace())
					{
						Shapeflag = false;
						return false;
					}
					else
						Shapeflag = true;
					switch(sh->ShapeType)
					{
					case RapidEnums::SHAPETYPE::LINE:
						delete currentHandler;
						if(sh->ShapeAsfasttrace())
						{
							currentHandler =  new FTHandler(((Line*)sh)->FastraceType_Mid(), (ShapeWithList*)sh, true);
							currentHandler->setClicksValue(0, ((Line*)sh)->getPoint1()->getX(),((Line*)sh)->getPoint1()->getY(), ((Line*)sh)->getPoint1()->getZ());
							((FTHandler*)currentHandler)->SettheClickvalue(0, ((Line*)sh)->getPoint1()->getX(),((Line*)sh)->getPoint1()->getY(), ((Line*)sh)->getPoint1()->getZ());
						}
						else
						{
							currentHandler =  new LineHandlers(RapidEnums::LINETYPE::FINITELINE, (ShapeWithList*)sh, false);
							currentHandler->setClicksValue(0, ((Line*)sh)->getPoint1()->getX(),((Line*)sh)->getPoint1()->getY(), ((Line*)sh)->getPoint1()->getZ());
						}
						break;
					case RapidEnums::SHAPETYPE::CIRCLE:
						delete currentHandler;
						currentHandler =  new CircleHandlers(((Circle*)sh)->CircleType, (Circle*)sh, false);
						for(PC_ITER Item = ((ShapeWithList*)sh)->PointsList->getList().begin(); Item != ((ShapeWithList*)sh)->PointsList->getList().end(); Item++)
						{
							SinglePoint* Spt = (*Item).second;
							currentHandler->setClicksValue(i, Spt->X, Spt->Y, Spt->Z);
							i++;
						}
						break;

					case RapidEnums::SHAPETYPE::TWOARC:
						delete currentHandler;
						currentHandler =  new GroupShapeHandler((TwoArc*)sh, false);
						for(PC_ITER Item = ((ShapeWithList*)sh)->PointsList->getList().begin(); Item != ((ShapeWithList*)sh)->PointsList->getList().end(); Item++)
						{
							SinglePoint* Spt = (*Item).second;
							currentHandler->setClicksValue(i, Spt->X, Spt->Y, Spt->Z);
							i++;
						}
						break;

					case RapidEnums::SHAPETYPE::TWOLINE:
						delete currentHandler;
						currentHandler =  new GroupShapeHandler((TwoLine*)sh, false);
						for(PC_ITER Item = ((ShapeWithList*)sh)->PointsList->getList().begin(); Item != ((ShapeWithList*)sh)->PointsList->getList().end(); Item++)
						{
							SinglePoint* Spt = (*Item).second;
							currentHandler->setClicksValue(i, Spt->X, Spt->Y, Spt->Z);
							i++;
						}
						break;

					case RapidEnums::SHAPETYPE::ARC:
						delete currentHandler;
						if(sh->ShapeAsfasttrace())
						{
							if(((Circle*)sh)->CircleType == RapidEnums::CIRCLETYPE::ARC_ENDPOINT)
								currentHandler =  new FTHandler(false, (ShapeWithList*)sh, true);
							else
								currentHandler =  new FTHandler(true, (ShapeWithList*)sh, true);
							for(PC_ITER Item = ((ShapeWithList*)sh)->PointsList->getList().begin(); Item != ((ShapeWithList*)sh)->PointsList->getList().end(); Item++)
							{
								SinglePoint* Spt = (*Item).second;
								currentHandler->setClicksValue(i, Spt->X, Spt->Y, Spt->Z);
								((FTHandler*)currentHandler)->SettheClickvalue(i, Spt->X, Spt->Y, Spt->Z);
								i++;
							}
						}
						else
						{
							currentHandler =  new ArcHandler(((Circle*)sh)->CircleType, (Circle*)sh, false);
							for(PC_ITER Item = ((ShapeWithList*)sh)->PointsList->getList().begin(); Item != ((ShapeWithList*)sh)->PointsList->getList().end(); Item++)
							{
								SinglePoint* Spt = (*Item).second;
								currentHandler->setClicksValue(i, Spt->X, Spt->Y, Spt->Z);
								i++;
							}
						}
						break;
					case RapidEnums::SHAPETYPE::XRAY:
						delete currentHandler;
						currentHandler =  new LineHandlers(RapidEnums::LINETYPE::RAYLINE, (ShapeWithList*)sh, false);
						currentHandler->setClicksValue(0, ((Line*)sh)->getPoint1()->getX(),((Line*)sh)->getPoint1()->getY(), ((Line*)sh)->getPoint1()->getZ());
						break;
					case RapidEnums::SHAPETYPE::XLINE:
						delete currentHandler;
						currentHandler =  new LineHandlers(RapidEnums::LINETYPE::INFINITELINE, (ShapeWithList*)sh, false);
						currentHandler->setClicksValue(0, ((Line*)sh)->getPoint1()->getX(),((Line*)sh)->getPoint1()->getY(), ((Line*)sh)->getPoint1()->getZ());
						break;
					case RapidEnums::SHAPETYPE::SPLINE:
						delete currentHandler;
						currentHandler =  new SplineHandler((ShapeWithList*)sh, true);
						break;
					case RapidEnums::SHAPETYPE::CIRCLEINVOLUTE:
						delete currentHandler;
						currentHandler =  new GroupShapeHandler((CircleInvolute*)sh, false);
						for(PC_ITER Item = ((ShapeWithList*)sh)->PointsList->getList().begin(); Item != ((ShapeWithList*)sh)->PointsList->getList().end(); Item++)
						{
							SinglePoint* Spt = (*Item).second;
							currentHandler->setClicksValue(i, Spt->X, Spt->Y, Spt->Z);
							i++;
						}
						break;
					}
					atn->undo();
					undone = true;
					currentUndone = true;
					if(((ShapeWithList*)sh)->PointsList->Pointscount() == 0)
						currentHandler->setClicksDone(0);
					else if(((ShapeWithList*)sh)->PointsList->Pointscount() == 1)
						currentHandler->setClicksDone(1);
					else if(((ShapeWithList*)sh)->PointsList->Pointscount() == 2)
						currentHandler->setClicksDone(2);
					currentHandler->mouseMove();
					update_VideoGraphics();
					update_RcadGraphics();
					break;
				case RapidEnums::RAPIDFGACTIONTYPE::FOCUS_SCAN:
				case RapidEnums::RAPIDFGACTIONTYPE::FOCUS_CONTOURSCAN:
				case RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB:
				case RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB:
				case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB:
				case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES:
				case RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE:
				case RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR:
				case RapidEnums::RAPIDFGACTIONTYPE::CONTOURSCAN_FG:
				case RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT:
					atn->undo();
					undone = true;
					currentUndone = true;
					break;
				}
			}
			else if(atn->CurrentActionType == RapidEnums::ACTIONTYPE::LOCALISEDCORRECTION)
			{
				atn->undo();
				undone = true;
				currentUndone = true;
				if(atn->CurrentActionType == RapidEnums::ACTIONTYPE::LOCALISEDCORRECTION)
					GoToPreviousPosition(atn);
				else
					GoToPreviousPosition(NULL);
			}
			else if(atn->CurrentActionType == RapidEnums::ACTIONTYPE::PERIMETERACTION)
			{
				switch(((PerimeterAction*)atn)->getFramegrab()->FGtype)
				{
				case RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE:
					CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER;
					MeasurementMode(false);
					sh = (Shape*)(((PerimeterAction*)atn)->getShape());		
					switch(sh->ShapeType)
					{
					case RapidEnums::SHAPETYPE::LINE:
						{
							delete currentHandler;
							Shape* ParentCshp = (Shape*)(*sh->getGroupParent().begin());
							currentHandler =  new PerimeterHandler(ParentCshp, sh->getId(), true);
							currentHandler->setClicksValue(0, ((Line*)sh)->getPoint1()->getX(),((Line*)sh)->getPoint1()->getY(), ((Line*)sh)->getPoint1()->getZ());
							((PerimeterHandler*)currentHandler)->SettheClickvalue(0, ((Line*)sh)->getPoint1()->getX(),((Line*)sh)->getPoint1()->getY(), ((Line*)sh)->getPoint1()->getZ());
						}
						break;
					case RapidEnums::SHAPETYPE::ARC:
						{
							delete currentHandler;
							Shape* ParentCshp = (Shape*)(*sh->getGroupParent().begin());
							currentHandler =  new PerimeterHandler(ParentCshp, sh->getId(), true);
							for(PC_ITER Item = ((ShapeWithList*)sh)->PointsList->getList().begin(); Item != ((ShapeWithList*)sh)->PointsList->getList().end(); Item++)
							{
								SinglePoint* Spt = (*Item).second;
								currentHandler->setClicksValue(i, Spt->X, Spt->Y, Spt->Z);
								((PerimeterHandler*)currentHandler)->SettheClickvalue(i, Spt->X, Spt->Y, Spt->Z);
								i++;
							}
						}
						break;
					}
					atn->undo();
					undone = true;
					currentUndone = true;
					if(((ShapeWithList*)sh)->PointsList->Pointscount() == 0)
						currentHandler->setClicksDone(0);
					else if(((ShapeWithList*)sh)->PointsList->Pointscount() == 1)
						currentHandler->setClicksDone(1);
					else if(((ShapeWithList*)sh)->PointsList->Pointscount() == 2)
						currentHandler->setClicksDone(2);
					currentHandler->mouseMove();
					update_VideoGraphics();
					update_RcadGraphics();
					break;
				}
			}
			else if(atn->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT_ALIGNACTION || atn->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION 
				|| atn->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_1PT1LN_ALIGNACTION || atn->CurrentActionType == RapidEnums::ACTIONTYPE::CAD_2LN_ALIGNACTION)
			{
				ControlOn = false;
				atn->undo();
				undone = true;
				currentUndone = true;
			}
			else
			{
				atn->undo();
				undone = true;
				currentUndone = true;
			}
			if(currentUndone)
				 getActionsHistoryList().notifyRemove(atn);
			UpdateShapesChanged();
			DXFShape_Updated();
			if(atn->CurrentActionType == RapidEnums::ACTIONTYPE::ADDSHAPEACTION)
			{
				delete currentHandler;
				currentHandler = new DefaultHandler();
				if(Shapeflag)
				{
					undoAgain();
					undoAgain();
				}
			}
		}
		Sleep(100);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1192", __FILE__, __FUNCSIG__); return false; }
}

//undo iterate step.. for shape added..
bool RCadApp::undoAgain()
{
	try
	{
		if(undone == false)
			currentaction = getActionsHistoryList().getList().end();
		if(currentaction == getActionsHistoryList().getList().begin())
			return false;
		else
		{
			currentaction--;
			Shape* sh;
			int i = 0;
			RAction* atn = static_cast<RAction*>((*currentaction).second);
			bool currentUndone = false;
			if(atn->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				sh = (Shape*)(((AddPointAction*)atn)->getShape());
				if(Shapeflag && !sh->ShapeAsfasttrace())
				{
					Shapeflag = false;
					return false;
				}
				Shapeflag = sh->ShapeAsfasttrace();
				switch(sh->ShapeType)
				{
				case RapidEnums::SHAPETYPE::LINE:
					delete currentHandler;
					if(sh->ShapeAsfasttrace())
					{
						currentHandler =  new FTHandler(((Line*)sh)->FastraceType_Mid(), (ShapeWithList*)sh, true);
						currentHandler->setClicksValue(0, ((Line*)sh)->getPoint1()->getX(),((Line*)sh)->getPoint1()->getY(), ((Line*)sh)->getPoint1()->getZ());
						((FTHandler*)currentHandler)->SettheClickvalue(0, ((Line*)sh)->getPoint1()->getX(),((Line*)sh)->getPoint1()->getY(), ((Line*)sh)->getPoint1()->getZ());
					}
					else
					{
						currentHandler =  new LineHandlers(RapidEnums::LINETYPE::FINITELINE, (ShapeWithList*)sh, false);
						currentHandler->setClicksValue(0, ((Line*)sh)->getPoint1()->getX(),((Line*)sh)->getPoint1()->getY(), ((Line*)sh)->getPoint1()->getZ());
					}
					break;

				case RapidEnums::SHAPETYPE::TWOARC:
						delete currentHandler;
						currentHandler =  new GroupShapeHandler((TwoArc*)sh, false);
						for(PC_ITER Item = ((ShapeWithList*)sh)->PointsList->getList().begin(); Item != ((ShapeWithList*)sh)->PointsList->getList().end(); Item++)
						{
							SinglePoint* Spt = (*Item).second;
							currentHandler->setClicksValue(i, Spt->X, Spt->Y, Spt->Z);
							i++;
						}
						break;

				 case RapidEnums::SHAPETYPE::TWOLINE:
						delete currentHandler;
						currentHandler =  new GroupShapeHandler((TwoLine*)sh, false);
						for(PC_ITER Item = ((ShapeWithList*)sh)->PointsList->getList().begin(); Item != ((ShapeWithList*)sh)->PointsList->getList().end(); Item++)
						{
							SinglePoint* Spt = (*Item).second;
							currentHandler->setClicksValue(i, Spt->X, Spt->Y, Spt->Z);
							i++;
						}
						break;

				case RapidEnums::SHAPETYPE::CIRCLE:
					delete currentHandler;
					currentHandler =  new CircleHandlers(((Circle*)sh)->CircleType, (Circle*)sh, false);
					for(PC_ITER Item = ((ShapeWithList*)sh)->PointsList->getList().begin(); Item != ((ShapeWithList*)sh)->PointsList->getList().end(); Item++)
					{
						SinglePoint* Spt = (*Item).second;
						currentHandler->setClicksValue(i, Spt->X, Spt->Y, Spt->Z);
						i++;
					}
					break;
				case RapidEnums::SHAPETYPE::ARC:
					delete currentHandler;
					if(sh->ShapeAsfasttrace())
					{
						if(((Circle*)sh)->CircleType == RapidEnums::CIRCLETYPE::ARC_ENDPOINT)
							currentHandler =  new FTHandler(false, (ShapeWithList*)sh, true);
						else
							currentHandler =  new FTHandler(true, (ShapeWithList*)sh, true);
						for(PC_ITER Item = ((ShapeWithList*)sh)->PointsList->getList().begin(); Item != ((ShapeWithList*)sh)->PointsList->getList().end(); Item++)
						{
							SinglePoint* Spt = (*Item).second;
							currentHandler->setClicksValue(i, Spt->X, Spt->Y, Spt->Z);
							((FTHandler*)currentHandler)->SettheClickvalue(i, Spt->X, Spt->Y, Spt->Z);
							i++;
						}
					}
					else
					{
						currentHandler =  new ArcHandler(((Circle*)sh)->CircleType, (Circle*)sh, false);
						for(PC_ITER Item = ((ShapeWithList*)sh)->PointsList->getList().begin(); Item != ((ShapeWithList*)sh)->PointsList->getList().end(); Item++)
						{
							SinglePoint* Spt = (*Item).second;
							currentHandler->setClicksValue(i, Spt->X, Spt->Y, Spt->Z);
							i++;
						}
					}
					break;
				case RapidEnums::SHAPETYPE::XRAY:
					delete currentHandler;
					currentHandler =  new LineHandlers(RapidEnums::LINETYPE::RAYLINE, (ShapeWithList*)sh, false);
					currentHandler->setClicksValue(0, ((Line*)sh)->getPoint1()->getX(),((Line*)sh)->getPoint1()->getY(), ((Line*)sh)->getPoint1()->getZ());
					break;
				case RapidEnums::SHAPETYPE::XLINE:
					delete currentHandler;
					currentHandler =  new LineHandlers(RapidEnums::LINETYPE::INFINITELINE, (ShapeWithList*)sh, false);
					currentHandler->setClicksValue(0, ((Line*)sh)->getPoint1()->getX(),((Line*)sh)->getPoint1()->getY(), ((Line*)sh)->getPoint1()->getZ());
					break;
				}
				atn->undo();
				undone = true;
				currentUndone = true;
				if(((ShapeWithList*)sh)->PointsList->Pointscount() == 0)
					currentHandler->setClicksDone(0);
				else if(((ShapeWithList*)sh)->PointsList->Pointscount() == 1)
					currentHandler->setClicksDone(1);
				else
					currentHandler->setClicksDone(2);
			}
			else
			{
				atn->undo();
				undone = true;
				currentUndone = true;
			}
			if(currentUndone)
				 getActionsHistoryList().notifyRemove(atn);
			UpdateShapesChanged();
			DXFShape_Updated();
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1193", __FILE__, __FUNCSIG__); } return false;
}

//undo for Fastrace..
bool RCadApp::undoforFT(int cnt)
{
	try
	{
		for(int i = 0; i < cnt; i++)
		{
			currentaction--;
			RAction* atn = static_cast<RAction*>((*currentaction).second);
			atn->undo();	
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1194", __FILE__, __FUNCSIG__); }
}

//Redo the last undo...
bool RCadApp::Redo()
{
	try
	{
		if(undone == false) return false;
		int currentclickdone;
		bool redone = false;
		if(CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER)
			currentclickdone = currentHandler->getClicksDone();
		(static_cast<RAction*>((*currentaction).second))->redo();
		redone = true;
		Shape* sh;
		int i = 0;
		RAction* atn = static_cast<RAction*>((*currentaction).second);
		if(atn->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
		{
			if(((AddPointAction*)atn)->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE)
			{
				CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER;
				sh = (Shape*)(((AddPointAction*)atn)->getShape());
				switch(sh->ShapeType)
				{
				case RapidEnums::SHAPETYPE::LINE:
					delete currentHandler;
					if(sh->ShapeAsfasttrace())
					{
						int cnt = ((ShapeWithList*)sh)->PointsList->Pointscount();
						if(cnt == 2)
						{
							currentHandler =  new FTHandler(((Line*)sh)->FastraceType_Mid());
							currentHandler->setClicksValue(0, ((Line*)sh)->getPoint2()->getX(),((Line*)sh)->getPoint2()->getY(), ((Line*)sh)->getPoint2()->getZ());
							if(currentclickdone == 1)
							{
								((FTHandler*)currentHandler)->SettheClickvalue(0, ((Line*)sh)->getPoint2()->getX(),((Line*)sh)->getPoint2()->getY(), ((Line*)sh)->getPoint2()->getZ());
								currentHandler->setClicksDone(1);
							}
						}
						else if(cnt == 1)
						{
							currentHandler =  new FTHandler(((Line*)sh)->FastraceType_Mid(), (ShapeWithList*)sh, true);
							currentHandler->setClicksValue(0, ((Line*)sh)->getPoint1()->getX(),((Line*)sh)->getPoint1()->getY(), ((Line*)sh)->getPoint1()->getZ());
							((FTHandler*)currentHandler)->SettheClickvalue(0, ((Line*)sh)->getPoint1()->getX(),((Line*)sh)->getPoint1()->getY(), ((Line*)sh)->getPoint1()->getZ());
							currentHandler->setClicksDone(1);
						}
					}
					else
					{
						if(((ShapeWithList*)sh)->PointsList->Pointscount() == 1)
						{
							currentHandler =  new LineHandlers(RapidEnums::LINETYPE::FINITELINE, (ShapeWithList*)sh, false);
							currentHandler->setClicksValue(0, ((Line*)sh)->getPoint1()->getX(),((Line*)sh)->getPoint1()->getY(), ((Line*)sh)->getPoint1()->getZ());
							currentHandler->setClicksDone(1);
							currentHandler->mouseMove_x(getWindow(0).getPointer().x, getWindow(0).getPointer().y, 0, false, false);
						}
						else
						{
							SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
							currentHandler =  new DefaultHandler();
						}
					}
					break;

				  case RapidEnums::SHAPETYPE::TWOARC:
						delete currentHandler;
						currentHandler =  new GroupShapeHandler((TwoArc*)sh, false);
						for(PC_ITER Item = ((ShapeWithList*)sh)->PointsList->getList().begin(); Item != ((ShapeWithList*)sh)->PointsList->getList().end(); Item++)
						{
							SinglePoint* Spt = (*Item).second;
							currentHandler->setClicksValue(i, Spt->X, Spt->Y, Spt->Z);
							i++;
						}
						break;

				  case RapidEnums::SHAPETYPE::TWOLINE:
						delete currentHandler;
						currentHandler =  new GroupShapeHandler((TwoLine*)sh, false);
						for(PC_ITER Item = ((ShapeWithList*)sh)->PointsList->getList().begin(); Item != ((ShapeWithList*)sh)->PointsList->getList().end(); Item++)
						{
							SinglePoint* Spt = (*Item).second;
							currentHandler->setClicksValue(i, Spt->X, Spt->Y, Spt->Z);
							i++;
						}
						break;

				case RapidEnums::SHAPETYPE::CIRCLE:
					delete currentHandler;
					currentHandler =  new CircleHandlers(((Circle*)sh)->CircleType, (Circle*)sh, false);
					for(PC_ITER Item = ((ShapeWithList*)sh)->PointsList->getList().begin(); Item != ((ShapeWithList*)sh)->PointsList->getList().end(); Item++)
					{
						SinglePoint* Spt = (*Item).second;
						currentHandler->setClicksValue(i, Spt->X, Spt->Y, Spt->Z);
						i++;
					}
					currentHandler->setClicksDone(((ShapeWithList*)sh)->PointsList->Pointscount());
					currentHandler->mouseMove_x(getWindow(0).getPointer().x, getWindow(0).getPointer().y, 0, false, false);
					break;
				case RapidEnums::SHAPETYPE::ARC:
					delete currentHandler;
					if(sh->ShapeAsfasttrace())
					{
						int cnt = ((ShapeWithList*)sh)->PointsList->Pointscount();
						if(cnt == 3)
						{
							if(((Circle*)sh)->CircleType == RapidEnums::CIRCLETYPE::ARC_ENDPOINT)
								currentHandler =  new FTHandler(false);
							else
								currentHandler =  new FTHandler(true);
							currentHandler->setClicksValue(0, ((Line*)sh)->getPoint2()->getX(),((Line*)sh)->getPoint2()->getY(), ((Line*)sh)->getPoint2()->getZ());
							((FTHandler*)currentHandler)->SettheClickvalue(0, ((Line*)sh)->getPoint2()->getX(),((Line*)sh)->getPoint2()->getY(), ((Line*)sh)->getPoint2()->getZ());
							currentHandler->setClicksDone(1);
						}
						else
						{
							if(((Circle*)sh)->CircleType == RapidEnums::CIRCLETYPE::ARC_ENDPOINT)
								currentHandler =  new FTHandler(false, (ShapeWithList*)sh, true);
							else
								currentHandler =  new FTHandler(true, (ShapeWithList*)sh, true);
							for(PC_ITER Item = ((ShapeWithList*)sh)->PointsList->getList().begin(); Item != ((ShapeWithList*)sh)->PointsList->getList().end(); Item++)
							{
								SinglePoint* Spt = (*Item).second;
								currentHandler->setClicksValue(i, Spt->X, Spt->Y, Spt->Z);
								((FTHandler*)currentHandler)->SettheClickvalue(i, Spt->X, Spt->Y, Spt->Z);
								i++;
							}
							currentHandler->setClicksDone(((ShapeWithList*)sh)->PointsList->Pointscount());
						}
					}
					else if(sh->ShapeForPerimeter())
					{
						int cnt = ((ShapeWithList*)sh)->PointsList->Pointscount();
						if(cnt == 3)
						{
							Shape* ParentCshp = (Shape*)(*sh->getGroupParent().begin());
							currentHandler =  new PerimeterHandler(ParentCshp, sh->getId(), false);
							currentHandler->setClicksValue(0, ((Line*)sh)->getPoint2()->getX(),((Line*)sh)->getPoint2()->getY(), ((Line*)sh)->getPoint2()->getZ());
							((PerimeterHandler*)currentHandler)->SettheClickvalue(0, ((Line*)sh)->getPoint2()->getX(),((Line*)sh)->getPoint2()->getY(), ((Line*)sh)->getPoint2()->getZ());
							currentHandler->setClicksDone(1);
						}
						else
						{
							Shape* ParentCshp = (Shape*)(*sh->getGroupParent().begin());
							currentHandler =  new PerimeterHandler(ParentCshp, sh->getId(), false);
							for(PC_ITER Item = ((ShapeWithList*)sh)->PointsList->getList().begin(); Item != ((ShapeWithList*)sh)->PointsList->getList().end(); Item++)
							{
								SinglePoint* Spt = (*Item).second;
								currentHandler->setClicksValue(i, Spt->X, Spt->Y, Spt->Z);
								((PerimeterHandler*)currentHandler)->SettheClickvalue(i, Spt->X, Spt->Y, Spt->Z);
								i++;
							}
							currentHandler->setClicksDone(((ShapeWithList*)sh)->PointsList->Pointscount());
						}
					}
					else
					{
						currentHandler =  new ArcHandler(((Circle*)sh)->CircleType, (Circle*)sh, false);
						for(PC_ITER Item = ((ShapeWithList*)sh)->PointsList->getList().begin(); Item != ((ShapeWithList*)sh)->PointsList->getList().end(); Item++)
						{
							SinglePoint* Spt = (*Item).second;
							currentHandler->setClicksValue(i, Spt->X, Spt->Y, Spt->Z);
							i++;
						}
					}
					currentHandler->mouseMove_x(getWindow(0).getPointer().x, getWindow(0).getPointer().y, 0, false, false);
					break;
				case RapidEnums::SHAPETYPE::XRAY:
					delete currentHandler;
					currentHandler =  new LineHandlers(RapidEnums::LINETYPE::RAYLINE, (ShapeWithList*)sh, false);
					currentHandler->setClicksValue(0, ((Line*)sh)->getPoint1()->getX(),((Line*)sh)->getPoint1()->getY(), ((Line*)sh)->getPoint1()->getZ());
					currentHandler->setClicksDone(((ShapeWithList*)sh)->PointsList->Pointscount());
					currentHandler->mouseMove_x(getWindow(0).getPointer().x, getWindow(0).getPointer().y, 0, false, false);
					break;
				case RapidEnums::SHAPETYPE::XLINE:
					delete currentHandler;
					currentHandler =  new LineHandlers(RapidEnums::LINETYPE::INFINITELINE, (ShapeWithList*)sh, false);
					currentHandler->setClicksValue(0, ((Line*)sh)->getPoint1()->getX(),((Line*)sh)->getPoint1()->getY(), ((Line*)sh)->getPoint1()->getZ());
					currentHandler->setClicksDone(((ShapeWithList*)sh)->PointsList->Pointscount());
					currentHandler->mouseMove_x(getWindow(0).getPointer().x, getWindow(0).getPointer().y, 0, false, false);
					break;
				case RapidEnums::SHAPETYPE::SPLINE:
					delete currentHandler;
					currentHandler =  new SplineHandler((ShapeWithList*)sh, false);
					break;
				}
			}
			else
			{
				switch(((AddPointAction*)atn)->getFramegrab()->FGtype)
				{
				   case RapidEnums::RAPIDFGACTIONTYPE::FOCUS_SCAN:
				   case RapidEnums::RAPIDFGACTIONTYPE::FOCUS_CONTOURSCAN:
				   case RapidEnums::RAPIDFGACTIONTYPE::CONTOURSCAN_FG:
				   atn->redo();
				   redone = true;
				}
			 }
		}
		else if(atn->CurrentActionType == RapidEnums::ACTIONTYPE::LOCALISEDCORRECTION)
		{
			GoToNextPosition(atn, true);
		}
		else if(atn->CurrentActionType == RapidEnums::ACTIONTYPE::PERIMETERACTION)
		{
			CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER;
			sh = (Shape*)(((PerimeterAction*)atn)->getShape());
			switch(sh->ShapeType)
			{
			case RapidEnums::SHAPETYPE::LINE:
				{
					delete currentHandler;
					int cnt = ((ShapeWithList*)sh)->PointsList->Pointscount();
					if(cnt == 2)
					{
						Shape* ParentCshp = (Shape*)(*sh->getGroupParent().begin());
						currentHandler =  new PerimeterHandler(ParentCshp, sh->getId(), false);
						currentHandler->setClicksValue(0, ((Line*)sh)->getPoint2()->getX(),((Line*)sh)->getPoint2()->getY(), ((Line*)sh)->getPoint2()->getZ());
						if(currentclickdone == 1)
						{
							((PerimeterHandler*)currentHandler)->SettheClickvalue(0, ((Line*)sh)->getPoint2()->getX(),((Line*)sh)->getPoint2()->getY(), ((Line*)sh)->getPoint2()->getZ());
							currentHandler->setClicksDone(1);
						}
					}
					else if(cnt == 1)
					{
						Shape* ParentCshp = (Shape*)(*sh->getGroupParent().begin());
						currentHandler =  new PerimeterHandler(ParentCshp, sh->getId(), false);
						currentHandler->setClicksValue(0, ((Line*)sh)->getPoint1()->getX(),((Line*)sh)->getPoint1()->getY(), ((Line*)sh)->getPoint1()->getZ());
						((PerimeterHandler*)currentHandler)->SettheClickvalue(0, ((Line*)sh)->getPoint1()->getX(),((Line*)sh)->getPoint1()->getY(), ((Line*)sh)->getPoint1()->getZ());
						currentHandler->setClicksDone(1);
					}
				}
				break;
			case RapidEnums::SHAPETYPE::ARC:
				{
					delete currentHandler;	
					int cnt = ((ShapeWithList*)sh)->PointsList->Pointscount();
					if(cnt == 3)
					{
						Shape* ParentCshp = (Shape*)(*sh->getGroupParent().begin());
						currentHandler =  new PerimeterHandler(ParentCshp, sh->getId(), false);
						currentHandler->setClicksValue(0, ((Line*)sh)->getPoint2()->getX(),((Line*)sh)->getPoint2()->getY(), ((Line*)sh)->getPoint2()->getZ());
						((PerimeterHandler*)currentHandler)->SettheClickvalue(0, ((Line*)sh)->getPoint2()->getX(),((Line*)sh)->getPoint2()->getY(), ((Line*)sh)->getPoint2()->getZ());
						currentHandler->setClicksDone(1);
					}
					else
					{
						Shape* ParentCshp = (Shape*)(*sh->getGroupParent().begin());
						currentHandler =  new PerimeterHandler(ParentCshp, sh->getId(), false);
						for(PC_ITER Item = ((ShapeWithList*)sh)->PointsList->getList().begin(); Item != ((ShapeWithList*)sh)->PointsList->getList().end(); Item++)
						{
							SinglePoint* Spt = (*Item).second;
							currentHandler->setClicksValue(i, Spt->X, Spt->Y, Spt->Z);
							((PerimeterHandler*)currentHandler)->SettheClickvalue(i, Spt->X, Spt->Y, Spt->Z);
							i++;
						}
						currentHandler->setClicksDone(((ShapeWithList*)sh)->PointsList->Pointscount());
					}
					}
				break;
			}
			ControlOn = false;
		}
		currentaction++;
		if(currentaction != getActionsHistoryList().getList().end())
		{
			RAction* atn1 = static_cast<RAction*>((*currentaction).second);
			if(atn1->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				sh = (Shape*)(((AddPointAction*)atn1)->getShape());
				if(sh->ShapeAsfasttrace())
				{
					if((sh->ShapeType == RapidEnums::SHAPETYPE::ARC && (((ShapeWithList*)sh)->PointsList->Pointscount() == 3 || ((ShapeWithList*)sh)->PointsList->Pointscount() == 0)) || sh->ShapeType == RapidEnums::SHAPETYPE::LINE)
					{
						Redo();
						(static_cast<RAction*>((*currentaction).second))->redo();
						redone = true;
						currentaction++;
					}
				}
			}
		}
		else
		{
			if(DUMMYHANDLE == RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER)
			{
				delete currentHandler;
				currentHandler = new DefaultHandler();
			}
			else if(currentHandler->getClicksDone() == 0)
				currentHandler->setClicksDone(0);
		}
		if(redone)
		  getActionsHistoryList().notifyAdd(atn);
		UpdateShapesChanged();
		DXFShape_Updated();
		if (currentaction == getActionsHistoryList().getList().end()) // this means we have no more actions
		{
			currentaction--;
			undone = false;
			return false;
		}
		Sleep(50);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1195", __FILE__, __FUNCSIG__); return false; }
}

//Clear all.. to start fresh....
void RCadApp::clearAll() 
{
	try
	{
		Wait_SaveActionsWorkSpace();
		Wait_VideoGraphicsUpdate();
		Wait_RcadGraphicsUpdate();
		Wait_DxfGraphicsUpdate();
		this->TruePositionMode = false;
		ActionSavingFlag = false;
		dontUpdateGraphcis = true;
		//if(getCurrentUCS().UCSangle() != 0.0)
		//{
		//	if(!getCurrentUCS().KeepGraphicsStraightMode())
		//		KeepGraphicsStright();
		//	SetVideoRotateAngle(-1 * getCurrentUCS().UCSangle());
		//}
		SetVideoRotateAngle(0);
		UCS *MCSPrevious = (UCS*)getUCSList().getList()[0];
		RWindow TempWindow[3];
		for(int i = 0; i < 3; i++)
			TempWindow[i].init(MCSPrevious->getWindow(i));
		if(ReferenceDotShape != NULL)
		{
			getShapesList().removeItem(ReferenceDotShape);
			if(ReferenceDotShape->selected())
				getSelectedShapesList().removeItem(ReferenceDotShape);
		}
		if(this->Vblock_CylinderAxisLine != NULL)
		{
			getShapesList().removeItem(Vblock_CylinderAxisLine);
			if(Vblock_CylinderAxisLine->selected())
				getSelectedShapesList().removeItem(Vblock_CylinderAxisLine);
		}
		if(SecondReferenceDotShape != NULL)
		{
			getShapesList().removeItem(SecondReferenceDotShape);
			if(SecondReferenceDotShape->selected())
				getSelectedShapesList().removeItem(SecondReferenceDotShape);
		}
		if(this->ReferenceLineShape != NULL)
		{
			getShapesList().removeItem(ReferenceLineShape);
			if(ReferenceLineShape->selected())
				getSelectedShapesList().removeItem(ReferenceLineShape);
		}
		for(RC_ITER Item_ucs = getUCSList().getList().begin(); Item_ucs != getUCSList().getList().end(); Item_ucs++)
		{
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			Cucs->clearAll();
			delete Cucs;
			//if(Cucs->getId() != 0) delete Cucs;
		}
		ucslist.clearAll();
		actionhistory.deleteAll();
		while (ProbePathPoints.size() != 0)
		{
				map<int, double*>::iterator i = ProbePathPoints.begin();
				double* bi = (*i).second;
				ProbePathPoints.erase(i->first);
				delete [] bi;		
		}		
		currentaction = actionhistory.getList().end();
		
		DXFEXPOSEOBJECT->CurrentDXFShapeList.clear();
		DXFEXPOSEOBJECT->CurrentDXFMeasureList.clear();

		ShapesForTwoStepHoming.clear();
		ShapesForBaseAlignment.clear();
			
	//	FixedToolHandler->clearFixedTool();
		SmartMeasureType = RapidEnums::SMARTMEASUREMENTMODE::DEFAULTTYPE;

		ResetAllEntityIndex();
		
		PreviousSMhandler = RapidEnums::SMARTMEASUREMENTMODE::DEFAULTTYPE;

		SetMeasurementType(RapidEnums::SMARTMEASUREMENTMODE::DEFAULTTYPE);
		SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
		SetRcadCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
		SetDxfCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
		
		AddMCSduringLoad();
		for(int i = 0; i < 3; i++)
			getCurrentUCS().getWindow(i).init(TempWindow[i]);

		setHandler(new DefaultHandler());

		THREADCALCOBJECT->ClearAllThread();
		FOCUSCALCOBJECT->ClrearAllFocus();
		PPCALCOBJECT->ClearAllPP();
		HELPEROBJECT->ClearAllFlags();

		VMeasurement_Updated = true;
		VShape_Updated = true;
		VDXFShape_updated = true;
		VSelectedShapeUpdated = true;
		RMeasurement_Updated = true;
		RShape_Updated = true;
		RDXFShape_updated = true;
		RDXFMeasure_updated = true;
		RSelectedShape_Updated = true;
		VSelectedMeasurement_Updated = true;
		RSelectedMeasurement_Updated = true;
		PartprogramShape_Updated = true;
		this->ImageAction_Count = 0;
		DXFMouseFlag = false;
		RCADMouseFlag = false;
		VideoMouseFlag = false;
		dontUpdateGraphcis = false;
		AutoProbeMeasureFlag = false;
		PointerToShapeCollection = NULL;
		DoNotSamplePts = false;
		AddAction_PPEditMode = false;
		Color_Channel = 1;
		LastActionIdForPPEdit = 0;
		ShowCoordinatePlaneFlag(false);
		SigmaModeFlag(false);
		MousedragwithShape(false);
		MAINDllOBJECT->Automated_ShapeTypeList.clear();
		MAINDllOBJECT->Automated_MeasurementTypeList.clear();
		if(ReferenceDotShape != NULL)
		{
		   ((ShapeWithList*)(ReferenceDotShape))->PointAtionList.clear();
		   AddShapeAction::addShape(ReferenceDotShape);
		   getActionsHistoryList().removeItem(((ShapeWithList*)ReferenceDotShape)->CurrentShapeAddAction, true, true);
		   Shape_Updated();
		}
		if(this->Vblock_CylinderAxisLine != NULL)
		{
		   ((ShapeWithList*)(Vblock_CylinderAxisLine))->PointAtionList.clear();
		   AddShapeAction::addShape(Vblock_CylinderAxisLine);
		   getActionsHistoryList().removeItem(((ShapeWithList*)Vblock_CylinderAxisLine)->CurrentShapeAddAction, true, true);
		   Shape_Updated();
		}
		if(SecondReferenceDotShape != NULL)
		{
		   ((ShapeWithList*)(SecondReferenceDotShape))->PointAtionList.clear();
		   AddShapeAction::addShape(SecondReferenceDotShape);
		   getActionsHistoryList().removeItem(((ShapeWithList*)SecondReferenceDotShape)->CurrentShapeAddAction, true, true);
		   Shape_Updated();
		}
		if(this->ReferenceLineShape != NULL)
		{
		   ((ShapeWithList*)(ReferenceLineShape))->PointAtionList.clear();
		   AddShapeAction::addShape(ReferenceLineShape);
		   getActionsHistoryList().removeItem(((ShapeWithList*)ReferenceLineShape)->CurrentShapeAddAction, true, true);
		   Shape_Updated();
		}
		update_VideoGraphics(true);
		update_RcadGraphics();
		update_DxfGraphics();
		Wait_VideoGraphicsUpdate();
		Wait_RcadGraphicsUpdate();
		Wait_DxfGraphicsUpdate();
	}
	catch(...)
	{ 
		DXFMouseFlag = false; RCADMouseFlag = false;
		VideoMouseFlag = false; dontUpdateGraphcis = false;
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1196", __FILE__, __FUNCSIG__); 
	}
}

//Clear all flags..
void RCadApp::ClearAllModes()
{
	try
	{
		InitialiseCurrenthandlerSettings();

	
		this->HatchedCrossHairMode(false);
		
		
		//this->ShowDXFShapesOnVideo(true);
		this->SetCrossOnVideo(false);

		this->ReferenceDatumNameCount_GDT(0);
	
		IsPartProgramPause = false;
		Derivedmodeflag = false;
		NudgeToolflag = false;
		NudgeThreadShapes = false;
		RotateDXFflag = false;
		ProbeEnableFlag = false;
		ColorThreshold = 0;
		DiscreteColor = false;
		ProbeSmartMeasureflag = false;
		EnableTriangulationLighting = false;
		this->VideoGraphicsUpdating = false;
		this->RcadGraphicsUpdating = false;
		this->DxfGraphicsUpdating = false;

		this->VideoGraphicsUpdateQueue = false;
		this->RcadGraphicsUpdateQueue = false;
		this->DxfGraphicsUpdateQueue = false;

		this->Xaxis_Highlighted = false;
		this->Yaxis_Highlighted = false;
	
		this->ClearGraphicsOnVideo(false);
		HideGraphicsOnVideo(false);
		CaptureCad = false;
		CaptureVideo = false;
		ScaleViewMode(false);
		PartprogramShape_Updated = false;

		this->Alpha_Value = 0.4;
		StlHighlightedPointFlag = false;
		CheckStlHighlightedPt = false;
		CurrentExtensionFlag = false;

		ShowHideFGPoints(false);
	
		AddingUCSModeFlag = false;
		NudgeShapesflag = false;
		DrawRectanle_ImageDelay = false;
		
		//AngleMeasurementMode(0);
		undone = false;
	
		ShapeHighlightedForImplicitSnap(false);
		this->RcadGraphicsMoveWithDRO(false);

		this->DrawWiredSurface(false);
		this->AllowTransparencyTo3DShapes(true);
	
		PREVIOUSHANDLE = RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER;
		
		this->DrawSnapPointOnHighlightedShape(false);
		this->ShapeHighlighted(false);
		this->SnapPointHighlighted(false);
		this->DXFSnapPointHighlighted(false);
		this->DXFShapeHighlighted(false);
		this->MeasurementHighlighted(false);
		//this->ShowMeasurementOnVideo(true);
		nearestShape = NULL;
		DXFnearestShape = NULL;
		nearestmeasure = NULL;
		CurrentPCDShape = NULL;
		CloudPtZExtent[0] = 0; CloudPtZExtent[1] = 0;
		ResetRcadRotation = true;
		ThreeDRotationMode(false);
		RcadWindow3DMode(false);

		ControlOn = false;
		ShiftON = false;

		this->MeasurementMode(false);
		this->ProfileScanMode(false);
		this->LinearModeFlag(false);
		this->PCDMeasureModeFlag(false);
		this->CCPMeasureModeFlag(false);
		this->MouseClickAndDragMode(false);
		//this->DigitalZoomMode(false);

		getWindow(0).WindowPanned(true);
		getWindow(1).WindowPanned(true);
		getWindow(2).WindowPanned(true);
	}
	catch(...)
	{ 
		DXFMouseFlag = false; RCADMouseFlag = false;
		VideoMouseFlag = false; dontUpdateGraphcis = false;
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1197", __FILE__, __FUNCSIG__); 
	}
}

//Reset the shape, measurment , action index
void RCadApp::ResetAllEntityIndex()
{
	try
	{
		//Reset Base Index..
		BaseItem::reset();
		FramegrabBase::reset();
		IsectPt::reset();
		UCS::reset();
		
		//Reset the Shape indexes..
		Circle::reset();
		CloudPoints::reset();
		Cylinder::reset();
		DepthShape::reset();
		Line::reset();
		Perimeter::reset();
		Plane::reset();
		Pline::reset();
		RPoint::reset();
		RText::reset();
		Sphere::reset();
		PCDCircle::reset();
	
		//Reset Measurement indexes..
		DimBase::reset();
		DimAngularity::reset();
		DimArcAngle::reset();
		DimCircularity::reset();
		DimDepth::reset();
		DimDiameter2D::reset();
		DimDigitalMicrometer::reset();
		DimLine::reset();
		DimOrdinate::reset();
		DimPointCoordinate::reset();
		DimRadius2D::reset();
		DimLine::reset();
		DimThread::reset();
		DimTruePosition::reset();
		DimPerimeter_Diameter::reset();

		DimRotaryAngleMeasure::reset();
		DimConcentricity::reset();
		DimCylindricity::reset();
		DimFlatness::reset();
		DimSphericity::reset();
		DimLine3DAngularity::reset();
		DimPlaneAngularity::reset();
		DimCylinderAngularity::reset();
		DimLine3D::reset();
		DimPCDAngle::reset();
		DimPCDOffset::reset();
		DimSplineMeasurement::reset();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1198", __FILE__, __FUNCSIG__);}
}

//Set the home position.. starting (default) width, height, pixelwidth etc...
void RCadApp::homeTheWindow(int windowno)
{
	try
	{
		if(windowno == 2)
		{
			getWindow(windowno).homeIt(windowno);
			update_DxfGraphics();
			Sleep(30);
			Wait_DxfGraphicsUpdate();
		}
		else if(windowno == 1)
		{
			getWindow(windowno).homeIt(windowno);
			update_RcadGraphics();
			Sleep(20);
			Wait_RcadGraphicsUpdate();
			UCS& cucs = getCurrentUCS();
			if(cucs.UCSMode() == 2 || cucs.UCSMode() == 3)
			{
				//if(!cucs.KeepGraphicsStraightMode())
				//	SetRcadRotateAngle(cucs.UCSangle());
				update_RcadGraphics();
				Sleep(20);
				Wait_RcadGraphicsUpdate();
				update_RcadGraphics();
				Sleep(20);
				Wait_RcadGraphicsUpdate();
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1199", __FILE__, __FUNCSIG__);}
}

void RCadApp::ZoomToWindowExtents(int wno)
{
	try
	{
		if(wno > 0 && wno < 3)
		{
			getWindow(wno).ZoomToExtents(wno);
			update_RcadGraphics();
			update_DxfGraphics();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1200", __FILE__, __FUNCSIG__);}
}

//Change the Video window pixelwidth...//
void RCadApp::SetVideoPixelWidth(double pixelwidth_x, double pixelwidth_y)
{
	try
	{
		for(RC_ITER Citem = getUCSList().getList().begin(); Citem != getUCSList().getList().end(); Citem++)
		{
			UCS* CUcs = (UCS*)(*Citem).second;
			CUcs->getWindow(0).changeZoom_Video(pixelwidth_x, pixelwidth_y);
		}
		UpdateShapesChanged();
		update_DxfGraphics();
		if(currentHandler != NULL)
		{
			if(dynamic_cast<FrameGrabHandler*>(currentHandler))
				currentHandler->EscapebuttonPress();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1201", __FILE__, __FUNCSIG__);}
}

//Camera properties window..//
void RCadApp::cameraFormatDialog()
{
	try
	{
		if (CurrentCamera() == 3)
		{
			CameraClose();
			bool succeeded = Initialise(NULL, 1, 102);
			if (succeeded)
			{
				MAINDllOBJECT->CurrentCameraType = CurrentCamera();
				if (HELPEROBJECT->CamSizeRatio == 1)
				{
					succeeded = SetVideoWindow(currCamWidth, currCamHeight, MAINDllOBJECT->VideoFrameRate(), true, this->CameraDistortionFactor, this->CamXOffSet, this->CamYOffSet);
				}
				else //We have to compress a larger video stream to show in a smaller area. So we call SetVideoWindowEx
				{
					succeeded = SetVideoWindowEx(currCamWidth, currCamHeight, 0, 0, currCamWidth / HELPEROBJECT->CamSizeRatio, currCamHeight / HELPEROBJECT->CamSizeRatio,
						MAINDllOBJECT->VideoFrameRate(), true, this->CameraDistortionFactor, this->CamXOffSet, this->CamYOffSet, true);
				}
				if (succeeded)
				{
					//if (CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL || CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP)
					//	FlipVertical();
					Preview();
					//Set_ImBufferMode(true);
					CameraConnected(true);
					this->SetRCam3_DigitalGain(this->RCam3_D_GainSettings);
					MAINDllOBJECT->CameraReConnected(); // Reactivate lighting
					Sleep(100);
				}
				else
					CameraConnected(false);
			}
			else
				CameraConnected(false);
		}
		else
			GetCameraFormatDialog();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1202", __FILE__, __FUNCSIG__);}
}

//Video properties window...//
void RCadApp::videoDialog()
{
	try{ GetVideoDialog();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1203", __FILE__, __FUNCSIG__);}
}

//Video properties.. Flip horizontal...
void RCadApp::FlipVHorizontal()
{
	try{ FlipHorizontal();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1204", __FILE__, __FUNCSIG__);}
}

//Flip vertical.. the video..//
void RCadApp::FlipVVertical()
{
	try{ FlipVertical();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1205", __FILE__, __FUNCSIG__);}
}

//Freeze the camera preview..//
void RCadApp::FreezeON()
{
	try{ StopPreview();	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1206", __FILE__, __FUNCSIG__);}
}

//Resume the video...//
void RCadApp::ResumeThevideo()
{
	try{ Preview();	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1207", __FILE__, __FUNCSIG__);}
}

void RCadApp::UpdateCamera_ZoomLevel(int CamWidth, int CamHeight)
{
	try
	{
		if(!CameraConnected()) return;
		dontUpdateGraphcis = true;
		CameraClose();
		if(Initialise(getWindow(0).getHandle(), 1, 102))
		{
			int width = getWindow(0).getWinDim().x, height = getWindow(0).getWinDim().y;
			bool succeeded = false;
			if ((CamWidth == width) && (CamHeight == height))
				succeeded = SetVideoWindow(CamWidth, CamHeight, MAINDllOBJECT->VideoFrameRate(), true, this->CameraDistortionFactor, this->CamXOffSet, this->CamYOffSet);
			else
				if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT) 
					succeeded = SetVideoWindowEx(CamWidth, CamHeight, 0, 0, width, height, MAINDllOBJECT->VideoFrameRate(), true, this->CameraDistortionFactor, this->CamXOffSet, this->CamYOffSet, true);
				else
					succeeded = SetVideoWindowEx(CamWidth, CamHeight, (CamWidth - width)/2, (CamHeight - height)/2, width, height, MAINDllOBJECT->VideoFrameRate(), true, this->CameraDistortionFactor, this->CamXOffSet, this->CamYOffSet, false);
			if (succeeded)
			{
				currCamWidth = CamWidth;
				currCamHeight = CamHeight;

				if((MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT) && MAINDllOBJECT->DigitalZoomModeForOneShot())
					EDSCOBJECT->ResetImageSize(CamWidth * 2, CamHeight * 2);
					
				else
					EDSCOBJECT->ResetImageSize(CamWidth, CamHeight);

				if(CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL || CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP)
					FlipVertical();
				Preview();
				memset(pixels, 102, this->currCamWidth * this->currCamHeight * 4); // 24000000);
				Set_ImBufferMode(true);
				CameraConnected(true);
			}
		}
		else 
		{
			CameraConnected(false);
		}
		dontUpdateGraphcis = false;
		getWindow(0).WindowPanned(true);
		update_VideoGraphics();
	}
	catch(...){ dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1208", __FILE__, __FUNCSIG__);} // old RCAD0001a"
}

//Set the window view for digital mode..//
void RCadApp::DigitalZoomON(double PixelWidth_X, double PixelWidth_Y)
{
	try
	{
		if(!CameraConnected()) return;
		if(DigitalZoomModeForOneShot())
		{
			SetStatusCode("RCadApp17");
			return;
		}
		dontUpdateGraphcis = true;
		CameraClose();
		if(this->FullWindowDigitalZoom())
		{
			if(Initialise(getWindow(0).getHandle(), 1, 102))
			{
				int wwidth = getWindow(0).getWinDim().x, wheight = getWindow(0).getWinDim().y;
				int frate = 10;
				if(wwidth == 2048) frate = 3;
				SetVideoWindow(wwidth, wheight, frate, false, MAINDllOBJECT->CameraDistortionFactor, MAINDllOBJECT->CamXOffSet, MAINDllOBJECT->CamYOffSet);
				if(CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL || CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP)
					FlipVertical();
				Preview();
				memset(pixels, 102, this->currCamWidth * this->currCamHeight * 4); // 24000000);
				Set_ImBufferMode(true);
				if(PixelWidth_X != 0 && PixelWidth_Y != 0)
					SetVideoPixelWidth(PixelWidth_X, PixelWidth_Y);
				else
					SetVideoPixelWidth(getWindow(0).getUppX() / 2, getWindow(0).getUppX() / 2);
				
				SetProfileLightMode(ProfileON);

				//int camNo = CurrentCamera();
				//if(CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT)
				//{	
				//	if (camNo == 2)
				//		SetCamProperty("05050080010002");
				//	else
				//		SetCamProperty("58FF0080010000");
				//}
				//else
				//{	
				//	if (camNo == 2)
				//		SetCamProperty("05050580010002");
				//	else
				//		SetCamProperty("58FF008001004C");
				//}
			}
		}
		else
		{
			if(Initialise(getWindow(0).getHandle(), 1, 102))
			{
				int wwidth = getWindow(0).getWinDim().x, wheight = getWindow(0).getWinDim().y;
				int frate = 5;
				//This code has to change from 3M specific camera to any camera code - R A N 05 Apr 2014
				if(wwidth == 1024) frate = 3;
				int TempW = 2 * wwidth, TempH = wheight * 2;
				this->Edgedetection->ResetImageSize(TempW, TempH);
				bool CompressFrame = false;
				if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT) CompressFrame = true;

				//if(SetVideoWindowEx(TempW,  TempH, wwidth/2,  wheight/2, wwidth, wheight, frate, false, MAINDllOBJECT->CameraDistortionFactor, MAINDllOBJECT->CamXOffSet, MAINDllOBJECT->CamYOffSet))
				if(SetVideoWindowEx(TempW,  TempH, 0,  0, wwidth, wheight, frate, false, MAINDllOBJECT->CameraDistortionFactor, MAINDllOBJECT->CamXOffSet, MAINDllOBJECT->CamYOffSet, CompressFrame))
				{	HelperInst->CamSizeRatio = (double) TempW / (double) wwidth;
					if(CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL || CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP)
						FlipVertical();
					Preview();
					Set_ImBufferMode(true);
					this->DigitalZoomMode(true);
					//if(PixelWidth_X != 0 && PixelWidth_Y != 0)
						SetVideoPixelWidth(PixelWidth_X, PixelWidth_Y);
					/*else																				//vinod. it always goes in else.. never in if.. bad coding before.. now ok.. 
						SetVideoPixelWidth(getWindow(0).getUppX() / 2, getWindow(0).getUppX() / 2);*/
				}
			}
		}
		DigitalZoomIsOn = true;
		dontUpdateGraphcis = false;
		getWindow(0).WindowPanned(true);
		update_VideoGraphics();
	}
	catch(...){ dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1209", __FILE__, __FUNCSIG__);}
}

//Set the window view for normal view..//
void RCadApp::DigitalZoomOff(double PixelWidth_X, double PixelWidth_Y)
{
	try
	{
		if(!CameraConnected()) return;
		if(DigitalZoomModeForOneShot())
		{
			SetStatusCode("RCadApp17");
			return;
		}
		dontUpdateGraphcis = true;
		//CameraClose();
		//MAINDllOBJECT->ConnectToCamera();
		//if(FullWindowDigitalZoom())
		//{
		//	if(Initialise(getWindow(0).getHandle(), 1, 102))
		//	{
		//		if(SetVideoWindow(getWindow(0).getWinDim().x, getWindow(0).getWinDim().y, VideoFrameRate(), true, MAINDllOBJECT->CameraDistortionFactor, MAINDllOBJECT->CamXOffSet, MAINDllOBJECT->CamYOffSet))
		//		{
		//			if(CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL || CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP)
		//				FlipVertical();
		//			Preview();
		//			Set_ImBufferMode(true);
		//			SetVideoPixelWidth(PixelWidth_X, PixelWidth_Y);
		//		}
		//	}
		//}
		//else
		//{
		//	if(Initialise(getWindow(0).getHandle(), 1, 102))
		//	{
		//		if(SetVideoWindow(getWindow(0).getWinDim().x, getWindow(0).getWinDim().y, VideoFrameRate(), false, MAINDllOBJECT->CameraDistortionFactor, MAINDllOBJECT->CamXOffSet, MAINDllOBJECT->CamYOffSet))
		//		{
		//			if(CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL || CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP)
		//				FlipVertical();
		//			Preview();
		//			Set_ImBufferMode(true);
		//			this->DigitalZoomMode(false);
		//			SetVideoPixelWidth(PixelWidth_X, PixelWidth_Y);
		//		}
		//	}
		//}
		DigitalZoomIsOn = false;
		dontUpdateGraphcis = false;
		update_VideoGraphics();
	}
	catch(...){ dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1210", __FILE__, __FUNCSIG__);}
}

void RCadApp::ConnectToCamera(HWND handle, int camWidth, int camHeight, double ZoomRatio, int DisplayW, int DisplayH, double FrameRate, bool ConnecttoRapidCam)
{
	try 
	{
		//if (camWidth == -1) camWidth = width;
		//if (camHeight == -1) camHeight = height;
		//if (CameraConnected()) CameraClose();
		////to be used subsequently during readpixels for graphics update on video
		//currCamWidth = camWidth; VideoDisplayWidth = DisplayW;
		//currCamHeight = camHeight; VideoDisplayHeight = DisplayH;
		////if (ZoomRatio == 1.0 && currCamWidth != VideoDisplayWidth)
		////	HelperInst->CamSizeRatio = (double)currCamWidth / (double)VideoDisplayWidth;
		////else
		//HelperInst->CamSizeRatio = ZoomRatio; // (double)camWidth / (double)width;
		//double DisplayFraction = 1.0;
		//if (this->CurrentCameraType == 3)
		//	DisplayFraction = ZoomRatio / 4;
		//else if (currCamWidth != DisplayW)
		//	DisplayFraction = ZoomRatio / 2;

		////Video window initialize
		//if (MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::HOBCHECKER && ConnecttoRapidCam)
		//{
		//	bool succeeded = Initialise(handle, 1, 102);
		//	if (succeeded)
		//	{
		//		if (ZoomRatio == 1.0 && camWidth == DisplayW && camHeight == DisplayH) //Straight forward - no video compression or resizing required.
		//		{
		//			succeeded = SetVideoWindow(camWidth, camHeight, MAINDllOBJECT->VideoFrameRate(), true, this->CameraDistortionFactor, this->CamXOffSet, this->CamYOffSet);
		//		}
		//		else //We have to compress a larger video stream to show in a smaller area. So we call SetVideoWindowEx or we may need digital Zoom
		//		{
		//			bool CompressFrame = false;
		//			if (FullWindowDigitalZoom())
		//			{
		//				CompressFrame = true;
		//				succeeded = SetVideoWindowEx2(camWidth, camHeight, 0, 0, DisplayW, DisplayH, DisplayFraction, MAINDllOBJECT->VideoFrameRate(), true, this->CameraDistortionFactor, this->CamXOffSet, this->CamYOffSet);
		//			}
		//			else
		//			{
		//				int SrcX = camWidth / 2 - ZoomRatio * ((double)DisplayW / 2.0);
		//				int SrcY = camHeight / 2 - ZoomRatio * ((double)DisplayH / 2.0);
		//				succeeded = SetVideoWindowEx2(camWidth, camHeight, SrcX, SrcY, DisplayW, DisplayH, DisplayFraction, MAINDllOBJECT->VideoFrameRate(), true, this->CameraDistortionFactor, this->CamXOffSet, this->CamYOffSet);
		//			}
		//		}
		//	}
		//	if (succeeded)
		//	{
		//		if (CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL || CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP)
		//			FlipVertical();
		//		Preview();
		//		//Set_ImBufferMode(true);
		//		CameraConnected(true);
		//	}
		//	else
		//		CameraConnected(false);
		//}
		//else
		//	CameraConnected(false);
		////Store the current camera we are connecting to...
		//if (CameraConnected())
		//{
		//	this->CurrentCameraType = CurrentCamera();
		//	//SetVideoPixelWidth(getWindow(0).getUppX() * DisplayFraction, getWindow(0).getUppX() * DisplayFraction);
		//}

	}
	catch (...)
	{
		dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1211", __FILE__, __FUNCSIG__);
	}

}


//Get the Camera settings...
std::string RCadApp::getCamSettings()
{
	try
	{
		std::string str = "";
		int tt;
		if (!CameraConnected()) return str;
		int currCam = CurrentCamera();
		for(int i = 0; i < 11; i++)
		{
			bool flag = true;
			tt = GetCameraProperty(i);
			switch(i)
			{
				case 0:
					if(CurrentCamera() == 0)
						tt += 128;
					else if (CurrentCamera() == 1)
						tt += 10;
					break;
				case 10:
					if (CurrentCamera() == 1)
						tt += 11;
					else if (CurrentCamera() == 0)
						if (tt < 4) tt = 76;					
					break;
				case 5:
				case 9:
					if(currCam > 0 && currCam < 3)
						flag = false;
					break;
				case 4:
					if (CurrentCamera() == 1)
						tt = 32;
					else if (CurrentCamera() == 3)
					{
						if (tt < 1) tt = 1;
					}
					else
						flag = false;
					break;
				case 7:
					if (CurrentCamera() == 1)
						tt = 100;
					else if (CurrentCamera() == 3)
						flag = true;
					else
						flag = false;
					break;
				case 6:
				case 8:
					if (CurrentCamera() != 3)
						flag = false;
					break;
			}
			if(flag)
			{
				std::string temp = "";
				_itoa(tt, (char*)temp.c_str(), 16);
				std::string temp1 = (const char*)temp.c_str();
				if(temp1.size() < 2) temp1 = "0" + temp1;
				str = str + temp1;
			}
		}
		return str;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1212", __FILE__, __FUNCSIG__); return "";}
}

std::string RCadApp::getRCam3_DigitalGain()
{
	try
	{
		std::string str = "";
		int tt;
		if (!CameraConnected()) return str;
		int currCam = CurrentCamera();
		for (int i = 14; i < 18; i++)
		{
			tt = GetCameraProperty(i);
			std::string temp = "";
			_itoa(tt, (char*)temp.c_str(), 16);
			std::string temp1 = (const char*)temp.c_str();
			if (temp1.size() < 2) temp1 = "0" + temp1;
			str = str + temp1;
		}
		return str;
	}
	catch (...) { MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1213", __FILE__, __FUNCSIG__); return ""; }
}

//Sets the camera property....//
void RCadApp::SetCamProperty(std::string str)
{
	MAINDllOBJECT->SetAndRaiseErrorMessage(str + "Demo", __FILE__, __FUNCSIG__);
	try
	{
		if (!CameraConnected()) return;
		std::string st;
		int j = 0, tt;
		for(int i = 0; i < 11; i++)
		{
			bool flag = true;
			st = str.substr(2 * j, 2);
			tt = Hex_Decimal(st.c_str());
			switch(i)
			{
				case 0:
					if(CurrentCamera() == 0)
						tt -= 128;
					else if (CurrentCamera() == 1)
						tt -= 10;
					break;
				case 10:
					if(CurrentCamera() == 1)
						tt -= 11;
					else if(CurrentCamera() == 0)
						if (tt < 4) tt = 76;
					break;
				case 5:
				case 9:
					if(MAINDllOBJECT->CurrentCameraType > 0 && MAINDllOBJECT->CurrentCameraType < 3)
						flag = false;
					break;
				case 4:
					if(CurrentCamera() == 1)
						tt = 32;
					else if (CurrentCamera() == 3)
					{
						if (tt < 1) tt = 1;
					}
					else
						flag = false;
					break;
				case 7:
					if(CurrentCamera() == 1)
						tt = 100;
					else if (CurrentCamera() == 3)
						flag = true;
					else
						flag = false;
					break;
				case 6:
				case 8:
					if (CurrentCamera() != 3)
						flag = false;
					break;
			}
			if(flag)
			{
				SetCameraProperty(i, tt);
				j++;
			}
		}
		//Raise the event that the camera settings have changed so that front end can update itself...
		MAINDllOBJECT->CamSettingsChanged(str);
	}
	catch(std::exception &ex)
	{ 
		std::string tempString = __FUNCSIG__;
		tempString.append(ex.what());
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1214", __FILE__, tempString);
	}
}

void RCadApp::SetRCam3_DigitalGain(std::string str)
{
	if (!CameraConnected()) return;
	std::string st;
	int j = 0, tt;
	st = str.substr(0, 1);

	SetCameraProperty(12, std::stoi(str.substr(0, 1)));
	SetCameraProperty(13, std::stoi(str.substr(1, 1)));

	for (int i = 1; i < 5; i++)
	{
		bool flag = true;
		st = str.substr(2 * i, 2);
		tt = Hex_Decimal(st.c_str());
		SetCameraProperty(13 + i, tt);
	}
}

void RCadApp::SetCameraProp(int Property, int Value)
{
	if (MAINDllOBJECT->CameraConnected())
		SetCameraProperty(Property, Value);
}

int RCadApp::GetCameraProp(int Property)
{
	int propValue = GetCameraProperty(Property);
	return propValue;
}


//Converting Hexadecimal to Decimal Value........
int RCadApp::Hex_Decimal(std::string hexval)
{
	try
	{
		int dec_val = 0;
		int pw = 0;
		for(int j = 0; j < (int)hexval.length(); j++)
		{
			int dec_equ = Decimal_equivalent(hexval.at(j)); 
			if(dec_equ < 0)continue;
			pw = hexval.length() - j - 1;
			dec_val += dec_equ * pow(16.0, pw);
		}
		return dec_val;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1215", __FILE__, __FUNCSIG__); return -1;}
}

//Hex to decimal..
int RCadApp::Decimal_equivalent(char t)
{
	try
	{
		if (t == 'a' || t == 'A')
			return 10;
		else if (t == 'b' || t == 'B')
			return 11;
		else if (t == 'c' || t == 'C')
			return 12;
		else if (t == 'd' || t == 'D')
			return 13;
		else if (t == 'e' || t == 'E')
			return 14;
		else if (t == 'f' || t == 'F')
			return 15;
		else
			return atoi(&t);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1216", __FILE__, __FUNCSIG__); return -1;}
}
/*
//Set profile light mode..
void RCadApp::SetProfileLightMode(bool flag)
{
	try
	{
		ProfileON = flag;
		if(CameraConnected())
		{
			int camNo = CurrentCamera();
			if (flag)
			{
				if(CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT)
				{	
					if (camNo == 2)
						SetCamProperty("05050080010000");
					else if (camNo == 3)
						SetCamProperty("50800001000080");
					else
						SetCamProperty("58FF0080010002");
				}
				else
				{	
					if (camNo == 2)
						SetCamProperty("2D0A000A180000"); //05050580010002");
					else if (camNo == 3)
						SetCamProperty("80001101FF0080");
					else
						SetCamProperty("58FF008001004C");
				}
			}
			else
			{
				if(CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT)
				{
					if(ProfileON)
						SetCamProperty("58FF0080010002");
					else if(SurfaceON)
						SetCamProperty("8080808001004C");
					//Set_Exposure(76);
				}
				else
				{
					if (camNo == 2)
					{
						if (ProfileON)SetCamProperty("2D0A000A180000");
						else if (SurfaceON)SetCamProperty("2D05050A180000");
					}
					else if (camNo == 3)
					{
						if (ProfileON)SetCamProperty("50800001000080");
						else if (SurfaceON)SetCamProperty("80001101FF0080");
					}
					else
					{
						if (ProfileON)SetCamProperty("58FF008001004C");
						else if (SurfaceON)SetCamProperty("8080808001004C");
					}
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD0001", __FILE__, __FUNCSIG__);}
}

//Set Surface light mode..
void RCadApp::SetSurfaceLightMode(bool flag)
{
	try
	{
		SurfaceON = flag;
		if(CameraConnected())
		{
			if (CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT)
			{
				if (CurrentCamera() == 3)
				{
					if (ProfileON)SetCamProperty("50800001000080");
					else if (SurfaceON)SetCamProperty("80001101FF0080");
				}
				else
				{
					if (ProfileON)SetCamProperty("58FF008001004C");
					else if (SurfaceON)SetCamProperty("80808080010002");
				}
			}
			else
			{
				int camNo = CurrentCamera();
				if (camNo == 2)
				{
					if (ProfileON)SetCamProperty("2D0A000A180000");
					else if (SurfaceON)SetCamProperty("2D05050A180000");

				}
				else if (camNo == 3)
				{
					if (ProfileON)SetCamProperty("50800001000080");
					else if (SurfaceON)SetCamProperty("80001101FF0080");
				}
				else
				{
					if (ProfileON)SetCamProperty("58FF008001004C");
					else if (SurfaceON)SetCamProperty("8080808001004C");
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD0001", __FILE__, __FUNCSIG__);}
}
*/

//Set profile light mode..
void RCadApp::SetProfileLightMode(bool flag)
{
	try
	{
		ProfileON = flag;
		if (CameraConnected())
		{
			int camNo = CurrentCamera();
			if (flag)
			{
				SetCamProperty(MAINDllOBJECT->ProfileCamSettings_default);
			}
			else
			{

				if (ProfileON)SetCamProperty(MAINDllOBJECT->ProfileCamSettings_default);
				else if (SurfaceON)SetCamProperty(MAINDllOBJECT->SurfaceCamSettings_default);

			}
		}
	}
	catch (...) { MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1217", __FILE__, __FUNCSIG__); }
}

//Set Surface light mode..
void RCadApp::SetSurfaceLightMode(bool flag)
{
	try
	{
		SurfaceON = flag;
		/*if (flag)
			MessageBox(NULL, L"True", L"Rapid-I", MB_TOPMOST);
		else
			MessageBox(NULL, L"False", L"Rapid-I", MB_TOPMOST);*/

		if (CameraConnected())
		{
			if (ProfileON)SetCamProperty(MAINDllOBJECT->ProfileCamSettings_default);
			else if (SurfaceON)SetCamProperty(MAINDllOBJECT->SurfaceCamSettings_default);

		}
	}
	catch (...) { MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1218", __FILE__, __FUNCSIG__); }
}


//set auxillary light mode..
void RCadApp::SetAxilaryLightMode(bool flag)
{
	try{AxillaryON = flag;}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1219", __FILE__, __FUNCSIG__);}
}

//get profile light mode..
bool RCadApp::getProfile()
{
	return ProfileON;
}

//get surface light mode..
bool RCadApp::getSurface()
{
	return SurfaceON;
}

//get auxillary light mode
bool RCadApp::getAxilary()
{
	return AxillaryON;
}

void RCadApp::SetSurfaceLightFlag(bool flag)
{
	SurfaceON = flag;
}

//Set the light proeprty...//
void RCadApp::setLightProperty(char* l)
{
	try{lightProperty = (const char*)l;}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1220", __FILE__, __FUNCSIG__);}
}

//Get the light property..//
char* RCadApp::getLightProperty()
{
	return (char*)lightProperty.c_str();
}

//Save Video Imamge with graphics......//
void RCadApp::SaveImage_WithGraphics(BYTE* destination)
{
	try
	{
		CaptureVideo = true;
		Wait_VideoGraphicsUpdate();
		Sleep(10);
		update_VideoGraphics();
		Sleep(10);
		int tempCount = 0;
		while(CaptureVideo) {tempCount++;}
		Sleep(20);
		dontUpdateGraphcis = true;
		Sleep(50);
		GetImage(destination, true); //!DigitalZoomMode());	
		dontUpdateGraphcis = false;
		update_VideoGraphics();
		/*char chars[n + 1];
		memcpy(chars, destination, n);
		chars[n] = '\0';*/
	}
	catch(...){ dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1221", __FILE__, __FUNCSIG__); }
}

//Save video image without graphics
void RCadApp::SaveImage_WithoutGraphics(BYTE* destination)
{
	try
	{
		if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT)
			GetImageNow_byte(destination, true); //!DigitalZoomMode());
		else
			GetImageNow_byte(destination, !DigitalZoomMode());
		//true); //
		/*double VWidth = MAINDllOBJECT->getWindow(0).getWinDim().x;
		double VHeight = MAINDllOBJECT->getWindow(0).getWinDim().y;
		if(MAINDllOBJECT->DigitalZoomModeForOneShot())
		{
			VWidth *= 2;
			VHeight *= 2;
		}
		double *tmp =  EDSCOBJECT->GetImage();
		for(int i = 0; i < VWidth * VHeight; i++)
		{
			destination[3 * i] = BYTE(tmp[i]);
			destination[3 * i + 1] = BYTE(tmp[i]);
			destination[3 * i + 2] = BYTE(tmp[i]);
		}*/

		/*for(int i = 0; i < 800 * 600; i++)
		{
			destination[3 * i] = BYTE(pixelvalues[i]);
			destination[3 * i + 1] = BYTE(pixelvalues[i]);
			destination[3 * i + 2] = BYTE(pixelvalues[i]);
		}*/

		//int length = getWindow(0).getWinDim().x * getWindow(0).getWinDim().y;
		//double* pixelarray = (double*)malloc(sizeof(double) *  length);
		//bool flag = GetImageNow_double_1bpp(1, pixelarray, !DigitalZoomMode());
		/*BYTE* pixelarray = (BYTE*)malloc(sizeof(BYTE) *  length);
		bool flag = GetImageNow_byte_1bpp(1, pixelarray, !DigitalZoomMode());
		for(int i = 0; i < length; i++)
		{
			destination[3 * i] = (BYTE)pixelarray[i];
			destination[3 * i + 1] = (BYTE)pixelarray[i];
			destination[3 * i + 2] = (BYTE)pixelarray[i];
		}
		free(pixelarray);*/
	}
	catch(...){dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1222", __FILE__, __FUNCSIG__); }
}

//Save image for one shot..
void RCadApp::SaveIamge_OneShot(BYTE* destination, int bpp)
{
	try
	{
		/*double tempcount = getWindow(0).getWinDim().x * getWindow(0).getWinDim().y;
		GetImageNow_byte_1bpp(1, &pixels[0]);
		for(int i = 0; i <tempcount; i++)
		{
			destination[3 * i] = pixels[i];
			destination[3 * i + 1] = pixels[i];
			destination[3 * i + 2] = pixels[i];
		}*/
		bool flag;
		if (bpp == 3)
			flag = GetImageNow_byte(destination, true); // !DigitalZoomMode()); CHanged by RA for AptinaCam
		else
			flag = GetImageNow_byte_1bpp(1, destination, true);
	}
	catch(...){dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1223", __FILE__, __FUNCSIG__); }
}

//Returns the byte array of pixels.. To save it as image...//
BYTE* RCadApp::getRWindowImage(int width, int height)
{
	try
	{
		CaptureCad = true;
		Wait_RcadGraphicsUpdate();
		Sleep(10);
		update_RcadGraphics();
		Wait_RcadGraphicsUpdate();
		Sleep(10);
		int tempCount = 0;
		while(CaptureCad) {tempCount++;}
		Sleep(20);
		//return screenshot;
		int TempWidth = width * 4;
		BYTE* destination = (BYTE*)calloc(TempWidth * height, sizeof(BYTE));
		int Count = 0;
		//if (!screenshot) 
		//	screenshot = (BYTE*)malloc(sizeof(BYTE) * getWindow(1).getWinDim().x * getWindow(1).getWinDim().y * 4);

		for(int i = 0; i < height; i++)
		{
			for(int j = 0; j < width; j++)
			{
				int Pos = TempWidth * i + 4 * j;
				if(this->RcadBackGround_White)
				{
					if (destination[Pos] == 0 && destination[Pos + 1] == 0 && destination[Pos + 2] == 0)
					{	destination[Pos] = 255 - screenshot[Pos]; //pixels[Pos]; //
						destination[Pos + 1] = 255 - screenshot[Pos + 1]; //[Pos + 1];
						destination[Pos + 2] = 255 - screenshot[Pos + 2]; //screenshot[Pos + 2];				
					}
				}
				else
				{
					destination[Pos] = screenshot[Pos]; //screenshot[Pos];
					destination[Pos + 1] = screenshot[Pos + 1]; // screenshot[Pos + 1];
					destination[Pos + 2] = screenshot[Pos + 2]; // screenshot[Pos + 2];
				}
				destination[Pos + 3] = screenshot[Pos + 3]; //screenshot[Pos + 3];
				if(destination[Pos] == 255 && destination[Pos + 1] == 0 && destination[Pos + 2] == 255)
				{
					destination[Pos] = 0;
					destination[Pos + 1] = 255;
					destination[Pos + 2] = 0;
				}
			}
		}
		update_RcadGraphics();
		Wait_RcadGraphicsUpdate();
		Sleep(20);
		free(screenshot);
		return destination;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1224", __FILE__, __FUNCSIG__); return NULL;}
}

//Returns the clickdone for current shape..//
int RCadApp::GetClickDone()
{
	return getCurrentHandler()->getClicksDone();
}

//returns the maximum clicks for current shape..//
int RCadApp::GetMaxClicks()
{
	return getCurrentHandler()->getMaxClicks();
}

//Resets the clickdone to zero..//
void RCadApp::Resetthe_Clicks()
{
	getCurrentHandler()->resetClicks();
}

//Set the click done for current entity...//
void RCadApp::SetCLickDone(int click)
{
	getCurrentHandler()->setClicksDone(click);
}

//Add the PCD circle action...
void RCadApp::AddNewPcdCircle(bool is3D)
{
	try
	{
		Shape* NewPcdSh;
		if(is3D)NewPcdSh = new PCDCircle(_T("PCD"),true);
		else NewPcdSh = new PCDCircle(_T("PCD"));
		CurrentPCDShape = NewPcdSh;
		AddShapeAction::addShape(NewPcdSh);
		/*for(RC_ITER i = getShapesList().getList().begin(); i != getShapesList().getList().end(); i++)
		{
			Shape* CShape = (Shape*)(*i).second;
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || CShape->ShapeType == RapidEnums::SHAPETYPE::ARC)
				((PCDCircle*)CurrentPCDShape)->PCDAllShapeCollection.push_back(CShape);
		}*/
		
		//changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1225", __FILE__, __FUNCSIG__); }
}

//Add circle to the selected PCD circle..
void RCadApp::AddCircleToCurrentPCD(int id)
{
	try
	{
		if(CurrentPCDShape == NULL) return;
		((PCDCircle*)CurrentPCDShape)->AddParentShapes(getShapesList().getList()[id]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1226", __FILE__, __FUNCSIG__); }
}

//Remove circle from the selected PCD circle..
void RCadApp::RemoveCircleFromCurrentPCD(int id)
{
	try
	{
		if(CurrentPCDShape == NULL) return;
		((PCDCircle*)CurrentPCDShape)->RemoveParentShapes(getShapesList().getList()[id]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1227", __FILE__, __FUNCSIG__); }
}

//Set crosshair color..
void RCadApp::SetCrosshair_Color(double r, double g, double b)
{
	try
	{
		GRAFIX->SetCrossHair_Color(r, g, b);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1228", __FILE__, __FUNCSIG__); }
}

//Set flexible crosshair color..
void RCadApp::SetFlexibleCrosshair_Color(double r, double g, double b)
{
	try
	{
		GRAFIX->SetFlexbleCrossHair_Color(r, g, b);
		update_VideoGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1229", __FILE__, __FUNCSIG__); }
}

//values from front end .. for derived shapes...//
void RCadApp::SetDerivedShapeParam(double offset, double dist, double Llength)
{
	try
	{
		if(!((Shape*)getSelectedShapesList().getList().empty()) && ((Shape*)getSelectedShapesList().getList().begin()->second)->ShapeType == RapidEnums::SHAPETYPE::RELATIVEPOINT)
		{
			((RPoint*)getSelectedShapesList().getList().begin()->second)->SetOffsets(offset, dist, Llength);
			return;
		}
		double data[3] = {offset, dist, Llength};
		if(currentHandler != NULL)
			currentHandler->SetAnyData(&data[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1230", __FILE__, __FUNCSIG__); }
}

//Set tp parameters..
void RCadApp::SetTruePostion(double x, double y, int enumtype, bool forpoint)
{
	try
	{
		((DimensionHandler*)getCurrentHandler())->SetTrueCenter(x, y, enumtype, forpoint);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1231", __FILE__, __FUNCSIG__); }
}

//Set tp parameters..
void RCadApp::SetTruePostion3D(double x, double y, int enumtype, bool forPoint)
{
	try{((DimensionHandler*)getCurrentHandler())->SetTrueCenter3D(x, y, enumtype, forPoint);}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1232", __FILE__, __FUNCSIG__); }
}

//Set current Text
void RCadApp::SetTextForTextShape(char* Ctext)
{
	try{((RTextHandler*)currentHandler)->SetCurrentText(Ctext);}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1233", __FILE__, __FUNCSIG__); }
}

//Set the angle mode... 0-deg min sec, 1-decimal deg, 2-radians
void RCadApp::SetAngleMode(int mode)
{
	try
	{
		this->AngleMeasurementMode(mode);
		UpdateAllMeasurements();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1234", __FILE__, __FUNCSIG__); }
}

//Nudge the dxf / normal shapes... set the nudge step ..
void RCadApp::SetShapeCollectionForNudge(bool isDXF)
{
	try
	{
		if(isDXF)
			((MoveShapeHandler*)currentHandler)->setRCollectionBase(DXFEXPOSEOBJECT->CurrentDXFShapeList);
		else
		{
			std::list<Shape*> temp;
			for(RC_ITER item = getSelectedShapesList().getList().begin();	item != getSelectedShapesList().getList().end(); item++)
				temp.push_back((Shape*)((*item).second));
			((MoveShapeHandler*)currentHandler)->setRCollectionBase(temp);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1235", __FILE__, __FUNCSIG__); }
}

void RCadApp::SetDeviationParameter(double Uppertolerance, double Lowertolerance, double interval, bool DeviationWithYellowShapes, int IntervalType, bool deviationType, int measureRefrenceAxis, bool BestfitArc)
{
	try
	{
		double data[8] = {Uppertolerance, Lowertolerance, interval, 0.0, IntervalType, int(deviationType), measureRefrenceAxis, int(BestfitArc)};
		if(DeviationWithYellowShapes)
			data[3] = 1;
		if(currentHandler != NULL)
			currentHandler->SetAnyData(&data[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1236", __FILE__, __FUNCSIG__); }
}

void RCadApp::SetAction_NudgeRotate()
{
	try
	{
		MoveShapesAction::SetActionStatus();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1237", __FILE__, __FUNCSIG__); }
}

//Toggle Rcad Window 3d Mode..
void RCadApp::Toggle3DMode()
{
	try
	{
		RcadWindow3DMode(!RcadWindow3DMode());
		if(!RcadWindow3DMode())
			Reset3DRotation();
		else
			update_RcadGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1238", __FILE__, __FUNCSIG__); }
}

//Reset 3D Rotation..
void RCadApp::Reset3DRotation()
{
	try
	{
		MAINDllOBJECT->getWindow(1).getTransformMatrixfor3Drotate(RcadTransformationMatrix, 1);
		RcadWindow3DMode(false);
		ResetRcadRotation = true;
		update_RcadGraphics();
		Sleep(50);
		if(!getCurrentUCS().KeepGraphicsStraightMode())
			SetRcadRotateAngle(getCurrentUCS().UCSangle());
		MAINDllOBJECT->ZoomToWindowExtents(1);
		UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1239", __FILE__, __FUNCSIG__); }
}

//Set the Status code...//
void RCadApp::SetStatusCode(std::string s, bool Insert_TempStrFlag, std::string TempStr)
{
	try{ CurrentStatus((char*)s.c_str(), Insert_TempStrFlag, (char*)TempStr.c_str()); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1248", __FILE__, __FUNCSIG__); }
}

//show messagebox text...................
bool RCadApp::ShowMsgBoxString(std::string s, RapidEnums::MSGBOXTYPE btntype, RapidEnums::MSGBOXICONTYPE icon, bool Insert_TempStrFlag, std::string TempStr)
{
	try
	{
		return CurrentMsgBoxStatus((char*)s.c_str(), int(btntype), int(icon), Insert_TempStrFlag, (char*)TempStr.c_str());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1240", __FILE__, __FUNCSIG__); }
}

//Keep graphics straight flag...//
void RCadApp::KeepGraphicsStright()
{
	try
	{
		/*for(RC_ITER Citem = getUCSList().getList().begin(); Citem != getUCSList().getList().end(); Citem++)
		{
			UCS* CUcs = (UCS*)(*Citem).second;
			CUcs->KeepGraphicsStraightMode(!CUcs->KeepGraphicsStraightMode());
		}*/
		getCurrentUCS().KeepGraphicsStraightMode(!getCurrentUCS().KeepGraphicsStraightMode());
		if(getCurrentUCS().KeepGraphicsStraightMode())
			SetRcadRotateAngle(-1 * getCurrentUCS().UCSangle());
		else
			SetRcadRotateAngle(getCurrentUCS().UCSangle());
		UpdteCadgp_UcsChange = true;
		update_RcadGraphics();
		Sleep(30);
		Wait_RcadGraphicsUpdate();
		UpdateAllMeasurements();
		Sleep(30);
		update_RcadGraphics();
		Sleep(30);
		Wait_RcadGraphicsUpdate();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1241", __FILE__, __FUNCSIG__); }
}

//Returns the current window..//
int RCadApp::getCurrentWindow()
{
	return currentwindow;
}

//Pan mode flag..//
void RCadApp::SetPanmode(int windowno)
{
	try
	{
		getWindow(windowno).PanMode = !getWindow(windowno).PanMode;
		switch(windowno)
		{
		case 1:
			if (getWindow(windowno).PanMode)
			{
				SetRcadCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_PANRELEASE);
			}		
			else
			{
				if(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::TRIMMING_HANDLER)
					SetRcadCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_TRIM);
				else
					SetRcadCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
			}
			break;
		case 2:
			if(getWindow(windowno).PanMode)
				SetDxfCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_PANRELEASE);
			else
				SetDxfCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
			break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1242", __FILE__, __FUNCSIG__); }
}

void RCadApp::UnsetPanmode(int windowno)
{
	getWindow(windowno).PanMode = false;
	SetRcadCursorType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
}

// returns the pan mode flag...//
bool RCadApp::getPanMode(int windowno)
{
	return getWindow(windowno).PanMode;
}

//Change the selected shape for space bar action
void RCadApp::changeline()
{
	try
	{
		if(EditmeasurementMode())
		{
			DimBase* Cdim = (DimBase*)getDimList().selectedItem();
			Cdim->MeasurementDirection(!Cdim->MeasurementDirection());
			SelectedMeasurement_Updated();
		}
		else if(!MeasurementMode())
		{
			dontUpdateGraphcis = true;
			for (RC_ITER item = getSelectedShapesList().getList().begin(); item!= getSelectedShapesList().getList().end(); item++)
			{
				ShapeWithList* Cshape = (ShapeWithList*)((*item).second);
				if(Cshape->ShapeAsfasttrace()) continue;
				switch(Cshape->ShapeType)
				{
				case RapidEnums::SHAPETYPE::LINE:
					Cshape->ShapeType = RapidEnums::SHAPETYPE::XLINE;
					for(int i = 0; i < 3; i++)
						(*Cshape->SnapPointList->getList()[i]).IsValid = false;
					for each(DimBase* dim in Cshape->getMchild())
					{
						if(dim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_LINEWIDTH)
							dim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINESLOPE;
					}
					Cshape->notifyAll(ITEMSTATUS::DATACHANGED, Cshape);	
					break;
				case RapidEnums::SHAPETYPE::XLINE:
					if(((Line*)Cshape)->LineType == RapidEnums::LINETYPE::RAYLINE)
					{
						Cshape->ShapeType = RapidEnums::SHAPETYPE::XRAY;
						(*Cshape->SnapPointList->getList()[0]).IsValid = true;
					}
					else
					{
						Cshape->ShapeType = RapidEnums::SHAPETYPE::LINE;
						for(int i = 0; i < 3; i++)
							(*Cshape->SnapPointList->getList()[i]).IsValid = true;
						for each(DimBase* dim in Cshape->getMchild())
						{
							if(dim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_LINESLOPE)
								dim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINEWIDTH;
						}
					}
					Cshape->notifyAll(ITEMSTATUS::DATACHANGED, Cshape);	
					break;
				case RapidEnums::SHAPETYPE::LINE3D:
					Cshape->ShapeType = RapidEnums::SHAPETYPE::XLINE3D;
					for(int i = 0; i < 3; i++)
						(*Cshape->SnapPointList->getList()[i]).IsValid = false;
					for each(DimBase* dim in Cshape->getMchild())
					{
						if(dim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_LINEWIDTH)
							dim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINESLOPE;
					}
					Cshape->notifyAll(ITEMSTATUS::DATACHANGED, Cshape);	
					break;
				case RapidEnums::SHAPETYPE::XLINE3D:
					Cshape->ShapeType = RapidEnums::SHAPETYPE::LINE3D;
					for(int i = 0; i < 3; i++)
						(*Cshape->SnapPointList->getList()[i]).IsValid = true;
					for each(DimBase* dim in Cshape->getMchild())
					{
						if(dim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_LINESLOPE)
							dim->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINEWIDTH;
					}
					Cshape->notifyAll(ITEMSTATUS::DATACHANGED, Cshape);	
					break;
				case RapidEnums::SHAPETYPE::XRAY:
					Cshape->ShapeType = RapidEnums::SHAPETYPE::XLINE;
					(*Cshape->SnapPointList->getList()[0]).IsValid = false;
					Cshape->notifyAll(ITEMSTATUS::DATACHANGED, Cshape);	
					break;
				case RapidEnums::SHAPETYPE::CIRCLE:
					if(((Circle*)Cshape)->CircleType == RapidEnums::CIRCLETYPE::PARALLELARCCIR || ((Circle*)Cshape)->CircleType == RapidEnums::CIRCLETYPE::ARC_ENDPOINT ||
						((Circle*)Cshape)->CircleType == RapidEnums::CIRCLETYPE::ARC_MIDDLE)
					{
						Cshape->ShapeType = RapidEnums::SHAPETYPE::ARC;
						for(int i = 0; i < 3; i++)
							(*Cshape->SnapPointList->getList()[i]).IsValid = true;

					}
					Cshape->notifyAll(ITEMSTATUS::DATACHANGED, Cshape);	
					break;
				case RapidEnums::SHAPETYPE::ARC:
					Cshape->ShapeType = RapidEnums::SHAPETYPE::CIRCLE;
					for(int i = 0; i < 3; i++)
						(*Cshape->SnapPointList->getList()[i]).IsValid = false;
					Cshape->notifyAll(ITEMSTATUS::DATACHANGED, Cshape);	
					break;
				}
			}
			dontUpdateGraphcis = false;
			UpdateShapesChanged();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1243", __FILE__, __FUNCSIG__); }
}

//Set Rotation Angle Rcad Graphics..
void RCadApp::SetRcadRotateAngle(double angle)
{
	try
	{
			Rcad_Graphics->RotateGL(angle);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1244", __FILE__, __FUNCSIG__); }
}

//Set Rotation Angle Video Graphics..
void RCadApp::SetVideoRotateAngle(double angle)
{
	try
	{
		if(Video_Graphics != NULL)	Video_Graphics->RotateGL(angle);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1245", __FILE__, __FUNCSIG__); }
}

//Set the user chosen surface ED entity..
int RCadApp::SetSurfaceEDType(char* EntityName)
{
	try
	{
		if (currentHandler != NULL)
			return ((FrameGrabHandler*)currentHandler)->SelectSurfaceGrabbedShape(EntityName);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1246", __FILE__, __FUNCSIG__); }
}

//Highlight the user chosen surface ED entity..
void RCadApp::HighlightSurfaceFGShape(char* EntityName)
{
	try
	{
		if(currentHandler != NULL)
			((FrameGrabHandler*)currentHandler)->HighlightFGShape(EntityName);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1247", __FILE__, __FUNCSIG__); }
}

//Highlight the user chosen surface ED entity..
void RCadApp::SetLmouseDownOfRcad()
{
	this->currentHandler->LmouseDown_x(0, 0);
}

void RCadApp::SetAlgo6Param(int type6_MaskFactor, int type6_Binary_LowerFactor, int type6_Binary_UpperFactor, int type6_PixelThreshold, int type6_MaskingLevel, int type6_ScanRange, int type6_NoiseFilterDistInPixel, int type6SubAlgo_NoiseFilterDistInPixel, int type6JumpThreshold, bool AverageFilterON)
{
	EDSCOBJECT->Type6_MaskFactor = type6_MaskFactor;
	EDSCOBJECT->Type6_Binary_LowerFactor = type6_Binary_LowerFactor;
	EDSCOBJECT->Type6_Binary_UpperFactor = type6_Binary_UpperFactor;
	EDSCOBJECT->Type6_PixelThreshold = type6_PixelThreshold;
	EDSCOBJECT->Type6_MaskingLevel = type6_MaskingLevel;
	EDSCOBJECT->Type6_ScanRange = type6_MaskingLevel;
	EDSCOBJECT->Type6_NoiseFilterDistInPixel = type6_NoiseFilterDistInPixel;
	EDSCOBJECT->Type6SubAlgo_NoiseFilterDistInPixel = type6SubAlgo_NoiseFilterDistInPixel;
	EDSCOBJECT->Type6JumpThreshold = type6JumpThreshold;
	EDSCOBJECT->DoAverageFiltering = AverageFilterON;
	if(currentHandler != NULL)
		((FrameGrabHandler*)currentHandler)->RedoAlgo6ForSurfaceFG();
}

void RCadApp::SetSurfaceEDSettings(int Channel_Number)
{
	if (currentHandler != NULL)
	{
		//MAINDllOBJECT->Channel_No(Channel_Number);
		//MAINDllOBJECT->DoAverageFilter(DoAverageFilter);
		FrameGrabHandler* thisFG = (FrameGrabHandler*)currentHandler;
		thisFG->baseaction->ChannelNo = Channel_Number;
		thisFG->RedoAlgo6ForSurfaceFG();
	}
}

void RCadApp::ChangeWindowViewOrientation(int WindowNo, RapidEnums::RAPIDPROJECTIONTYPE CviewType)
{
	try
	{
		if(CviewType == RapidEnums::RAPIDPROJECTIONTYPE::XY)
		{
			RcadTransformationMatrix[0] = 1; RcadTransformationMatrix[1] = 0; RcadTransformationMatrix[2] = 0; RcadTransformationMatrix[3] = 0;
			RcadTransformationMatrix[4] = 0; RcadTransformationMatrix[5] = 1; RcadTransformationMatrix[6] = 0; RcadTransformationMatrix[7] = 0;
			RcadTransformationMatrix[8] = 0; RcadTransformationMatrix[9] = 0; RcadTransformationMatrix[10] = 1; RcadTransformationMatrix[11] = 0;
			RcadTransformationMatrix[12] = 0; RcadTransformationMatrix[13] = 0; RcadTransformationMatrix[14] = 0; RcadTransformationMatrix[15] = 1;
		}
		else if(CviewType == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
		{
			RcadTransformationMatrix[0] = 0; RcadTransformationMatrix[1] = 0; RcadTransformationMatrix[2] = 1; RcadTransformationMatrix[3] = 0;
			RcadTransformationMatrix[4] = 1; RcadTransformationMatrix[5] = 0; RcadTransformationMatrix[6] = 0; RcadTransformationMatrix[7] = 0;
			RcadTransformationMatrix[8] = 0; RcadTransformationMatrix[9] = 1; RcadTransformationMatrix[10] = 0; RcadTransformationMatrix[11] = 0;
			RcadTransformationMatrix[12] = 0; RcadTransformationMatrix[13] = 0; RcadTransformationMatrix[14] = 0; RcadTransformationMatrix[15] = 1;
		}
		else if(CviewType == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
		{
			RcadTransformationMatrix[0] = 1; RcadTransformationMatrix[1] = 0; RcadTransformationMatrix[2] = 0; RcadTransformationMatrix[3] = 0;
			RcadTransformationMatrix[4] = 0; RcadTransformationMatrix[5] = 0; RcadTransformationMatrix[6] = 1; RcadTransformationMatrix[7] = 0;
			RcadTransformationMatrix[8] = 0; RcadTransformationMatrix[9] = 1; RcadTransformationMatrix[10] = 0; RcadTransformationMatrix[11] = 0;
			RcadTransformationMatrix[12] = 0; RcadTransformationMatrix[13] = 0; RcadTransformationMatrix[14] = 0; RcadTransformationMatrix[15] = 1;
		}
		else if (CviewType == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
		{
			RcadTransformationMatrix[0] = 1; RcadTransformationMatrix[1] = 0; RcadTransformationMatrix[2] = 0; RcadTransformationMatrix[3] = 0;
			RcadTransformationMatrix[4] = 0; RcadTransformationMatrix[5] = 0; RcadTransformationMatrix[6] = 1; RcadTransformationMatrix[7] = 0;
			RcadTransformationMatrix[8] = 0; RcadTransformationMatrix[9] = 1; RcadTransformationMatrix[10] = 0; RcadTransformationMatrix[11] = 0;
			RcadTransformationMatrix[12] = 0; RcadTransformationMatrix[13] = 0; RcadTransformationMatrix[14] = 0; RcadTransformationMatrix[15] = 1;
		}
		else if (CviewType == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
		{
			RcadTransformationMatrix[0] = 0; RcadTransformationMatrix[1] = 0; RcadTransformationMatrix[2] = 1; RcadTransformationMatrix[3] = 0;
			RcadTransformationMatrix[4] = 1; RcadTransformationMatrix[5] = 0; RcadTransformationMatrix[6] = 0; RcadTransformationMatrix[7] = 0;
			RcadTransformationMatrix[8] = 0; RcadTransformationMatrix[9] = 1; RcadTransformationMatrix[10] = 0; RcadTransformationMatrix[11] = 0;
			RcadTransformationMatrix[12] = 0; RcadTransformationMatrix[13] = 0; RcadTransformationMatrix[14] = 0; RcadTransformationMatrix[15] = 1;
		}

		getWindow(WindowNo).SetTransFormationMatrix(&RcadTransformationMatrix[0]);
		ChangeRcadViewOrien = true;
		Wait_RcadGraphicsUpdate();
		Wait_VideoGraphicsUpdate();
		update_RcadGraphics();
		Sleep(10);
		Wait_RcadGraphicsUpdate();
		UpdateShapesChanged();
		Sleep(40);
		Wait_RcadGraphicsUpdate();
		Wait_VideoGraphicsUpdate();
		Sleep(20);
		Wait_RcadGraphicsUpdate();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1249", __FILE__, __FUNCSIG__); }
}

//Add probe point to selected shape...
void RCadApp::AddProbePointToSelectedShape(double x, double y, double z, double r, int i, int side)
{

	try
	{
		//Left = 0, Right = 1, back = 2, front = 3; straight = 4;
		if(OneTouchProbeAlignment && PPCALCOBJECT->IsPartProgramRunning())
		{
			PPCALCOBJECT->ProbeTouchPointDro[0] = x;
			PPCALCOBJECT->ProbeTouchPointDro[1] = y; 
			PPCALCOBJECT->ProbeTouchPointDro[2] = z;
			PPCALCOBJECT->ProbeTouchPointDro[3] = r;
		}
		switch(CurrentProbeType)
		{
			case RapidEnums::PROBETYPE::X_AXISTPROBE:
			{
			   if(side == 1)
			   {
				 x += X_AxisProbeArmLengthX();
				 y += X_AxisProbeArmLengthY();
				 z += X_AxisProbeArmLengthZ();
			   }
			   else if(side == 4)
			   {
				 x += StarStraightOffsetX();
				 y += StarStraightOffsetY();
				 z += StarStraightOffsetZ();
			   }
			   break;
			}
			case RapidEnums::PROBETYPE::Y_AXISTPROBE:
			{
			   if(side == 2)
			   {
				 x += Y_AxisProbeArmLengthX();
				 y += Y_AxisProbeArmLengthY();
				 z += Y_AxisProbeArmLengthZ();
			   }
			   else if(side == 4)
			   {
				 x += StarStraightOffsetX();
				 y += StarStraightOffsetY();
				 z += StarStraightOffsetZ();
			   }
			   break;
			}
			case RapidEnums::PROBETYPE::STARPROBE:
			{
				if(side == 1)
				{
					x += StarProbeRightOffsetX();
					y += StarProbeRightOffsetY();
					z += StarProbeRightOffsetZ();
				}
				if(side == 2)
				{
					x += StarProbeBackOffsetX();
					y += StarProbeBackOffsetY();
					z += StarProbeBackOffsetZ();
				}
				if(side == 3)
				{
					x += StarProbeFrontOffsetX();
					y += StarProbeFrontOffsetY();
					z += StarProbeFrontOffsetZ();
				}
			   else if(side == 4)
			   {
				 x += StarStraightOffsetX();
				 y += StarStraightOffsetY();
				 z += StarStraightOffsetZ();
			   }
				break;
			}
			case RapidEnums::PROBETYPE::GENERICPROBE:
			{
				double offsetx = 0, offsety = 0, offsetz = 0;
				GetGenericProbeOffset(side, &offsetx, &offsety, &offsetz);
				x += offsetx;
				y += offsety;
				z += offsetz;
				break;
			}
		}

		currentwindow = 0;
		ShapeWithList* CShape = (ShapeWithList*)getShapesList().selectedItem();
		if(MAINDllOBJECT->CurrentMCSType == RapidEnums::MCSTYPE::MCSYZ)
		{
			double pnt[3] = {x, y, z};
			x = pnt[2];
			y = pnt[0];
			z = pnt[1];
		}
		else if(MAINDllOBJECT->CurrentMCSType == RapidEnums::MCSTYPE::MCSNEGYZ)
		{
			double pnt[3] = {x, y, z};
			x = pnt[2];
			y = -pnt[0];
			z = pnt[1];
		}
		else if(MAINDllOBJECT->CurrentMCSType == RapidEnums::MCSTYPE::MCSXZ)
		{
			double pnt[3] = {x, y, z};
			x = pnt[0];
			y = pnt[2];
			z = pnt[1];
		}
		else if(MAINDllOBJECT->CurrentMCSType == RapidEnums::MCSTYPE::MCSNEGXZ)
		{
			double pnt[3] = {x, y, z};
			x = -pnt[0];
			y = pnt[2];
			z = pnt[1];
		}

		if(CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::PROBE_HANDLER && 
			(CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::MIDPOINT || 
			SmartMeasureType == RapidEnums::SMARTMEASUREMENTMODE::DIAMEASUREMENT ||
			SmartMeasureType == RapidEnums::SMARTMEASUREMENTMODE::ANGLEMEASUREWITH_XAXIS ||  
			SmartMeasureType == RapidEnums::SMARTMEASUREMENTMODE::ANGLEMEASUREWITH_YAXIS ||  
			SmartMeasureType == RapidEnums::SMARTMEASUREMENTMODE::RADIUSMEASUREMENT))
		{
			currentHandler->CurrentShape = CShape;
			if(CShape != NULL)
			{
				if( MAINDllOBJECT->getCurrentUCS().UCSMode() == 0)
				{
					x -= MAINDllOBJECT->getCurrentUCS().currentDroX();
					y -= MAINDllOBJECT->getCurrentUCS().currentDroY();
					z -= MAINDllOBJECT->getCurrentUCS().currentDroZ();
				}			
				ProbeEnableFlag = true;
				ProbeSmartMeasureflag = true;
				dontUpdateGraphcis = true;
				double currentPoints[3] = {x + ProbeOffsetX, y + ProbeOffsetY, z + ProbeOffsetZ};
				if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
					currentHandler->setBaseRProperty(currentPoints[0], currentPoints[1], currentPoints[2], true);
				else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
					currentHandler->setBaseRProperty(currentPoints[1], currentPoints[2], currentPoints[0] , true);
				else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
					currentHandler->setBaseRProperty(currentPoints[0], currentPoints[2], currentPoints[1], true);
				else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
					currentHandler->setBaseRProperty(currentPoints[1], -currentPoints[2], currentPoints[0], true);
				else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
					currentHandler->setBaseRProperty(currentPoints[0], -currentPoints[2], currentPoints[1], true);
				dontUpdateGraphcis = false;
			
				FramegrabBase* CbaseAction = currentHandler->baseaction;
				if(PPCALCOBJECT->IsPartProgramRunning())
				{
					(CbaseAction->AllPointsList.getList()[0])->Pdir = i;
					(CbaseAction->AllPointsList.getList()[0])->PLR = side;
					CbaseAction->points[0].set(x + ProbeOffsetX, y + ProbeOffsetY, z + ProbeOffsetZ, r, i);	
					CbaseAction->ActionFinished(true);
					CShape->ManagePoint(CbaseAction, ShapeWithList::ADD_POINT);
				}
				else
				{
					CbaseAction->ProbeDirection = RapidEnums::PROBEDIRECTION(i); // Probe Direction....!
					CbaseAction->ProbePosition = RapidEnums::PROBEPOSITION(side);
					(CbaseAction->AllPointsList.getList()[0])->PLR = side;
					(CbaseAction->AllPointsList.getList()[0])->Pdir = i;
					CbaseAction->points[0].set(x + ProbeOffsetX, y + ProbeOffsetY, z + ProbeOffsetZ, r, i);	
					AddPointAction::pointAction(CShape, CbaseAction);
				}
				currentHandler->LmouseDown_x(currentPoints[0], currentPoints[1], currentPoints[2]);
			}
		}
		else
		{
			if(currentHandler == NULL)
			{
				changeHandler(RapidEnums::RAPIDHANDLERTYPE::PROBE_HANDLER);
				ClearAllRadioButtonOtherThrd();
			}
			else if(!(dynamic_cast<ProbeHandler*>(currentHandler)))
			{
				changeHandler(RapidEnums::RAPIDHANDLERTYPE::PROBE_HANDLER);
				ClearAllRadioButtonOtherThrd();
			}
			currentHandler->CurrentShape = CShape;
			if(CShape != NULL)
			{
				double currentPoints[4] = {x + ProbeOffsetX, y + ProbeOffsetY, z + ProbeOffsetZ, r};
				UCS *CUCS = &(MAINDllOBJECT->getCurrentUCS());
				if(CUCS->UCSMode() == 0 || CUCS->UCSMode() == 1)
				{
					currentPoints[0] -= (CUCS->currentDroX() + CUCS->UCSPoint.getX());
					currentPoints[1] -= (CUCS->currentDroY() + CUCS->UCSPoint.getY());
					currentPoints[2] -= (CUCS->currentDroZ() + CUCS->UCSPoint.getZ());
					currentPoints[3] -= (CUCS->currentDroR() + CUCS->UCSPoint.getR());
				}
				dontUpdateGraphcis = true;
				{
					currentHandler->raxis_valueSet(currentPoints[3]);
					if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
						currentHandler->setBaseRProperty(currentPoints[0], currentPoints[1], currentPoints[2], true);
					else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
						currentHandler->setBaseRProperty(currentPoints[1], currentPoints[2], currentPoints[0] , true);
					else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
						currentHandler->setBaseRProperty(currentPoints[0], currentPoints[2], currentPoints[1], true);
					else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
						currentHandler->setBaseRProperty(currentPoints[1], -currentPoints[2], currentPoints[0], true);
					else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
						currentHandler->setBaseRProperty(currentPoints[0], -currentPoints[2], currentPoints[1], true);
					if (UseGotoPostionInAction())
					{
						this->UseGotoPostionInAction(false);
						currentHandler->baseaction->PointDRO.set(GotoPostionForAction.getX(), GotoPostionForAction.getY(), GotoPostionForAction.getZ());
					}
				}
				dontUpdateGraphcis = false;
				FramegrabBase* CbaseAction = currentHandler->baseaction;
				if(PPCALCOBJECT->IsPartProgramRunning())
				{
					(CbaseAction->AllPointsList.getList()[0])->Pdir = i;
					(CbaseAction->AllPointsList.getList()[0])->PLR = side;
					CbaseAction->points[0].set(currentPoints[0], currentPoints[1], currentPoints[2], r, i);	
					CbaseAction->ActionFinished(true);
					CShape->ManagePoint(CbaseAction, ShapeWithList::ADD_POINT);
				}
				else
				{
					CbaseAction->ProbeDirection = RapidEnums::PROBEDIRECTION(i); // Probe Direction....!
					CbaseAction->ProbePosition = RapidEnums::PROBEPOSITION(side);
					(CbaseAction->AllPointsList.getList()[0])->PLR = side;
					(CbaseAction->AllPointsList.getList()[0])->Pdir = i;
					CbaseAction->points[0].set(currentPoints[0], currentPoints[1], currentPoints[2], r, i);	
					//if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HOBCHECKER
					//	!RWrapper::RW_DelphiCamModule::MYINSTANCE()->Getting_BC_Axis &&
					//		RWrapper::RW_DelphiCamModule::MYINSTANCE()->GotAxis)
					//{
					//	double tempP[4] = {0};
					//	memcpy(tempP, currentPoints, 4 * sizeof(double));
					//	RWrapper::RW ::RotatePoint_Around_C_Axis(&ProbeCoordinate[0]);
					//}
					
					AddPointAction::pointAction(CShape, CbaseAction);
				}
			}
			Shape_Updated();
			MAINDllOBJECT->SetStatusCode("PROBE0002");
		}
	}
	catch(...)
	{
		dontUpdateGraphcis = false;
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1250", __FILE__, __FUNCSIG__);
	}
}

void RCadApp::AddPointsToAutoSphere(double *pnts, int cnt)
{
	try
	{
		Sphere *autoSphere = new Sphere();
		PointCollection ptcol;
		for ( int i = 0; i < cnt; i++)
		{
			ptcol.Addpoint(new SinglePoint(pnts[3 * i], pnts[3 * i + 1], pnts[3 * i + 2]));
		}
		AddShapeAction::addShape(autoSphere);
		autoSphere->AddPoints(&ptcol);
		MAINDllOBJECT->ZoomToWindowExtents(1);
	}
	catch(...)
	{
		 MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1251", __FILE__, __FUNCSIG__); 
	}
}

void RCadApp::AddPointsToAutoCylinder(double *pnts, int cnt)
{
	try
	{
		Cylinder *autoCylinder = new Cylinder();
		PointCollection ptcol;
		for ( int i = 0; i < cnt; i++)
		{
			ptcol.Addpoint(new SinglePoint(pnts[3 * i], pnts[3 * i + 1], pnts[3 * i + 2]));
		}
		AddShapeAction::addShape(autoCylinder);
		autoCylinder->AddPoints(&ptcol);
		MAINDllOBJECT->ZoomToWindowExtents(1);
	}
	catch(...)
	{
		 MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1252", __FILE__, __FUNCSIG__); 
	}
}

//Note down the Probe Path.. For Partprogram.
void RCadApp::NotedownProbePath(int side)
{
	try
	{
		FramegrabBase* baseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT);
		baseaction->PointDRO.set(PPCALCOBJECT->CurrentMachineDRO/*MAINDllOBJECT->getCurrentDRO()*/);
		if(MAINDllOBJECT->CameraConnected())
		{
			baseaction->camProperty = getCamSettings();
			baseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
		}
		baseaction->lightProperty = MAINDllOBJECT->getLightProperty();
		baseaction->ProfileON = MAINDllOBJECT->getProfile();
		baseaction->SurfaceON = MAINDllOBJECT->getSurface();
		baseaction->ProbePosition =  RapidEnums::PROBEPOSITION(side);
		AddProbePathAction::AddProbePath(baseaction);
		SetStatusCode("RCadApp18");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1253", __FILE__, __FUNCSIG__); }
}

void RCadApp::NotedownProbePath(int side, double x, double y, double z)
{
	try
	{
		if (side < 0 || side > 5) side = 0;

		FramegrabBase* baseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT);
		//The point comes in the current DRO. Convert to MCS and then add to baseaction!
		baseaction->PointDRO.set(x + MAINDllOBJECT->getCurrentUCS().UCSPoint.getX(), y + MAINDllOBJECT->getCurrentUCS().UCSPoint.getY(), z + MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ());
		if (MAINDllOBJECT->CameraConnected())
		{
			baseaction->camProperty = getCamSettings();
			baseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
		}
		baseaction->lightProperty = MAINDllOBJECT->getLightProperty();
		baseaction->ProfileON = MAINDllOBJECT->getProfile();
		baseaction->SurfaceON = MAINDllOBJECT->getSurface();
		baseaction->ProbePosition = RapidEnums::PROBEPOSITION(side);
		AddProbePathAction::AddProbePath(baseaction);
		//SetStatusCode("RCadApp18");
	}
	catch (...) { MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1253-1", __FILE__, __FUNCSIG__); }
}


void RCadApp::NotedDownImagePickPoint()
{
	try
	{
		if(MAINDllOBJECT->CameraConnected())
		{
			FramegrabBase* baseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT);
			baseaction->PointDRO.set(PPCALCOBJECT->CurrentMachineDRO);
			baseaction->camProperty = getCamSettings();
			baseaction->lightProperty = MAINDllOBJECT->getLightProperty();
			baseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
			baseaction->ProfileON = MAINDllOBJECT->getProfile();
			baseaction->SurfaceON = MAINDllOBJECT->getSurface();
			AddImageAction::AddImage_DroValue(baseaction);
		}
		else
		{
			SetStatusCode("RCadApp24");
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1254", __FILE__, __FUNCSIG__); }
}

//Add point from text file..
void RCadApp::AddTextPointToSelectedShape(double x, double y, double z, double r)
{
	try
	{
		ShapeWithList* Cshape = (ShapeWithList*)getShapesList().selectedItem();
		if(Cshape != NULL)
		{
			Cshape->PointsList->Addpoint(new SinglePoint(x, y, z, r));
			Cshape->PointsListOriginal->Addpoint(new SinglePoint(x, y, z, r));
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1255", __FILE__, __FUNCSIG__); }
}

//Update shape after adding all Points..
void RCadApp::UpdateAfterAddingPoints()
{
	try
	{
		ShapeWithList* Cshape = (ShapeWithList*)getShapesList().selectedItem();
		if(Cshape != NULL)
		{
			Cshape->UpdateShape();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1256", __FILE__, __FUNCSIG__); }
}

//Testing Focus depth/ scan from a file..
void RCadApp::AddNewPointWithFocus(std::list<double> *PtValues)
{
	try
	{
		int Cnt = PtValues->size() / 2;
		int tempcnt = 0;
		double* Points = (double*)calloc(Cnt * 2, sizeof(double));
		for each(double x in *PtValues)
			Points[tempcnt++] = x;
		double Zvaluetest = 0;
		BESTFITOBJECT->CalculateFocus(Points, Cnt, 0.4, &Zvaluetest);
		double temp = 0.5;

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1257", __FILE__, __FUNCSIG__); }
}

//Initialise grid Module..Shewytank Mishra Version
void RCadApp::InitialiseGridModule(double LeftTopX, double LeftTopY, double GridWidth, double GridHeight, double CellWidth, double CellHeight)
{
	try
	{
		MyGridModule = new GridModule("Grid", true);
		MyGridModule->GridModuleError = &GridModuleErrorHandler;
		MyGridModule->SetLeftTop(LeftTopX,LeftTopY);
		MyGridModule->SetWidth(GridWidth);
		MyGridModule->SetHeight(GridHeight);
		MyGridModule->SetCellWidth(CellWidth);
		MyGridModule->SetCellHeight(CellHeight);
		MyGridModule->CalculateRectangles();
		GridmoduleUpdated = true;
		update_DxfGraphics();
		Sleep(200);
		//MyGridModule->WritetoDXF((char*)Dxffilename.c_str());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1258", __FILE__, __FUNCSIG__); }
}

void RCadApp::ShowHideCurrentGrid()
{
	if(GridmoduleUpdated)
	{
		GridmoduleUpdated=false;
		update_DxfGraphics();
	}
	else
	{
		GridmoduleUpdated=true;
		update_DxfGraphics();
	}
}

//written by .... Shweytank Mishra
void RCadApp::ClearCurrentGrid()
{
	if(MyGridModule != NULL)
	{
		for(GM_ITER item = MyGridModule->getItemList().begin(); item!=MyGridModule->getItemList().end(); item++)
		{
			GRectangle* Gsh = ((*item).second);
			delete Gsh;
		}
		delete MyGridModule;
		MyGridModule = NULL;
	}
		
		GridmoduleUpdated = true;
		update_DxfGraphics();
}

//Set the Current Cell Highlighted
void RCadApp::setCurrentHighlitedCell(double x , double y)
{
	if(MyGridModule != NULL && GridmoduleUpdated)
	{
		if(MyGridModule->getCurrentSelecteCell() == NULL)
		{
		GridCellShape=MyGridModule->getCurrentHighlightedCell(x,y);
		}
		else 
		{
			GridCellShape = MyGridModule->getCurrentHighlightedCell(x,y);
		}
	}	
}
//Draw grids..
void RCadApp::buildGridList()
{
	try
	{
		GRAFIX->DeleteGlList(8);
		GRAFIX->CreateNewGlList(8);
		if(MyGridModule == NULL)
		{
			return;
		}
		for(GM_ITER item = MyGridModule->getItemList().begin(); item != MyGridModule->getItemList().end(); item++)
		{
			GRectangle* Gsh = ((*item).second);
			if(Gsh->selected())
			{
				GRAFIX->SetColor_Double(1, 0, 0);
				GRAFIX->drawRectangle(*Gsh->GetLeftTop(),*(Gsh->GetLeftTop() + 1), *Gsh->GetBottomRight(), *(Gsh->GetBottomRight() + 1), true);
			}
			else
			{
				if(Gsh->GetVisitCount()>0)
				{
					int i=Gsh->GetVisitCount();
					GRAFIX->SetColor_Double(1 - (1/i),1 - (1/i),0);
					GRAFIX->drawRectangle(*Gsh->GetLeftTop(),*(Gsh->GetLeftTop() + 1), *Gsh->GetBottomRight(), *(Gsh->GetBottomRight() + 1),true);
				}
				else
				{
				GRAFIX->SetColor_Double(1, 0, 1);
				GRAFIX->drawRectangle(*Gsh->GetLeftTop(),*(Gsh->GetLeftTop() + 1), *Gsh->GetBottomRight(), *(Gsh->GetBottomRight() + 1));
				}
			}
		}
		if(MyGridModule->ShowColumnRowHeaders())
		{
			for(GM_ITER item = MyGridModule->getRCItemList().begin(); item != MyGridModule->getRCItemList().end(); item++)
			{
				GRectangle* Gsh = ((*item).second);
				//GRAFIX->drawRectangle(*Gsh->GetLeftTop(),*(Gsh->GetLeftTop() + 1), *Gsh->GetBottomRight(), *(Gsh->GetBottomRight() + 1));
				//DGRAFIX->drawString(*Gsh->GetCenter(), *(Gsh->GetCenter() + 1), (char*)Gsh->GetName().c_str());
			}
		}
		GRAFIX->SetColor_Double(0, 0, 1);
		if(MyGridModule->getCurrentSelecteCell() != NULL)
		GRAFIX->drawRectangle(*MyGridModule->getCurrentSelecteCell()->GetLeftTop(),*(MyGridModule->getCurrentSelecteCell()->GetLeftTop() + 1), *MyGridModule->getCurrentSelecteCell()->GetBottomRight(), *(MyGridModule->getCurrentSelecteCell()->GetBottomRight() + 1), false);
		GRAFIX->EndNewGlList();

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1259", __FILE__, __FUNCSIG__); }
}

//Increment the grid cell selection
void RCadApp::IncrementGridSelections()
{
	try
	{
		if(MyGridModule->getCurrentSelecteCell() == NULL && MyGridModule->getSelectedItemList().size() == 0) return;
		if(MyGridModule->getSelectedItemList().size() > 0)
		{
			MyGridModule->IncrementaccordingSorted();
			//MyGridModule->IncrementSelectedSelection();
			GridmoduleUpdated = true;
			update_DxfGraphics();
			Sleep(200);
			double GotoPos[3] = {*MyGridModule->getCurrentSelecteCell()->GetCenter(), *(MyGridModule->getCurrentSelecteCell()->GetCenter() + 1), currentDRO.getZ()};
			MoveMachineToRequiredPos(GotoPos[0], GotoPos[1], GotoPos[2], true, RapidEnums::MACHINEGOTOTYPE::SAOICELLGOTO);
		}
		else
		{
			MyGridModule->IncrementSelection();
			GridmoduleUpdated = true;
			update_DxfGraphics();
			Sleep(200);
			double GotoPos[3] = {*MyGridModule->getCurrentSelecteCell()->GetCenter(), *(MyGridModule->getCurrentSelecteCell()->GetCenter() + 1), currentDRO.getZ()};
			MoveMachineToRequiredPos(GotoPos[0], GotoPos[1], GotoPos[2], true, RapidEnums::MACHINEGOTOTYPE::SAOICELLGOTO);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1260", __FILE__, __FUNCSIG__); }
}

void RCadApp::GotoDefaultPosition()
{
	if(MyGridModule == NULL) return;
	else
	{
		GM_ITER item = MyGridModule->getItemList().begin();
		GRectangle* Gsh = ((*item).second);
		setCurrentHighlitedCell(*Gsh->GetLeftTop(),*(Gsh->GetLeftTop() + 1));
		update_DxfGraphics();
		MyGridModule->ClearSelection();
		GridCellShape->selected(true);
		GridCellShape->IncrementVcount();
		MyGridModule->SetSelectedRect(GridCellShape);
		MyGridModule->SetCurrentIterator(GridCellShape);
		GridmoduleUpdated = true;
		update_DxfGraphics();
		Sleep(200);
		double GotoPos[3] = {*MyGridModule->getCurrentSelecteCell()->GetCenter(), *(MyGridModule->getCurrentSelecteCell()->GetCenter() + 1), currentDRO.getZ()};
		MoveMachineToRequiredPos(GotoPos[0], GotoPos[1], GotoPos[2], true, RapidEnums::MACHINEGOTOTYPE::SAOICELLGOTO);
	}
}

//End Grid ....Shweytank Mishra

void RCadApp::BestFitSurfaceWithDiffParam(double i, bool value, bool again)
{
	BestFitSurface_PtsFactor(i);
	BestFitSurface_pts_around_xy_plane(value);
	if (again == false)
		CreateBFSurfaceActions = true;
	else
		CreateBFSurfaceActions = false;
	BestFitSurface();
	CreateBFSurfaceActions = false;
}

//best fit surface generation
void RCadApp::BestFitSurface()
{
	try
	{
		Wait_VideoGraphicsUpdate();
		Wait_RcadGraphicsUpdate();
		dontUpdateGraphcis = true;
		RCollectionBase& Sshapes = getShapesList();
		
		ShapeWithList* CShape = (ShapeWithList*)getShapesList().selectedItem();
		if(CShape->ShapeType != RapidEnums::SHAPETYPE::CLOUDPOINTS) return;
		int cnt = ((CloudPoints*) CShape)->TempShapeCollection.size();
		for (int j = 0; j < cnt; j++)
			Sshapes.removeItem(((CloudPoints*) CShape)->TempShapeCollection[j]);

		((CloudPoints*) CShape)->TempShapeCollection.clear();
		Shape* newShape;
		Surface_3DList surface_list;
		bool shapedoneusingprobe = false;
		int ptscount = CShape->PointsList->Pointscount();
		int step = 1;
		if (ptscount > 1000)
			step = ptscount/1000;
		double precision = -1;
		//allow this to be modified through settings from min of 1 pixel width to max of 200 pixel widths.
		precision = BestFitSurface_PtsFactor(); //default will be 0.2 mm which is approx 75 pixel widths
		//allow this to be modified through settings.  It will be false if points are spaced out in 3-d space and true if they are all around xy plane.
		bool pts_around_xy_plane = BestFitSurface_pts_around_xy_plane(); // true;
		if(CShape->PointAtionList.size() > 0)
		{
			AddPointAction* Caction = (AddPointAction*)(*(CShape->PointAtionList.begin()));
			if(Caction->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT)
				shapedoneusingprobe = true;
		}
		if(ptscount > 2)
		{
			BESTFITOBJECT->BestFitSurface(CShape->pts, ptscount, precision, &surface_list, step, pts_around_xy_plane);
			int i = 0;
			for (Surface_3DList::iterator surface_iter = surface_list.begin(); surface_iter != surface_list.end(); surface_iter++)
			{
				PointCollection PtColl;
				for(Pts_3DList::iterator pts_iter = surface_iter->pts_3d_list.begin(); pts_iter != surface_iter->pts_3d_list.end(); pts_iter++)
					PtColl.Addpoint(new SinglePoint(pts_iter->x, pts_iter->y, pts_iter->z));
				surface_iter->pts_3d_list.clear();
				if(surface_iter->shape == Shapes_3D::plane)
					newShape = new Plane();
				else if(surface_iter->shape == Shapes_3D::cylinder)
					newShape = new Cylinder();
				else if(surface_iter->shape == Shapes_3D::cone)
					newShape = new Cone();
				else if(surface_iter->shape == Shapes_3D::sphere)
					newShape = new Sphere();
				if(surface_iter->shape != Shapes_3D::nullshape)
				{
					newShape->PartialShape(true);
					AddShapeAction::addShape(newShape, false);
					newShape->ShapeDoneUsingProbe(shapedoneusingprobe);
					((ShapeWithList*)newShape)->AddPoints(&PtColl);
					//if CreateBFSurfaceActions is true:				
					//iterate through list of all action of the CShape Cloudpoints shape.
					//if current action type ADDPOINTACTION then check current fgtype.
					//Iterate through the Point collection and check which points belong to current framgrab.
					//create a new framgrab of same type using copy constructor and add points which we got in previous step to newly created framgrab.
					//create point action for this framgrab.
					//if no framegrab exists for some points then create base action for focus scan with default light setting.

					if (CreateBFSurfaceActions)
					{
						((ShapeWithList*)newShape)->PointsList->deleteAll();
						((ShapeWithList*)newShape)->PointsListOriginal->deleteAll();
						std::list<int> AddedPtsIdList;
						for each(RAction* CAction in CShape->PointAtionList)
						{
							if(CAction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
							{
								AddPointAction* ptAction = (AddPointAction*)(CAction);
								if(ptAction->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB || ptAction->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB || ptAction->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB
									|| ptAction->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE || ptAction->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB || ptAction->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES
									|| ptAction->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_CONTOURSCAN || ptAction->getFramegrab()->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_SCAN || RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT || RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE ||
									RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR)
								{
									FramegrabBase* Cbaseaction = new FramegrabBase(ptAction->getFramegrab());
									std::list<int> AddedPtsId;
									AddPoints_Framgrab(&PtColl, Cbaseaction, &AddedPtsId);
									if(Cbaseaction->AllPointsList.Pointscount() > 0)
										AddPointAction::pointAction(((ShapeWithList*)newShape), Cbaseaction);
									for each(int ptId in AddedPtsId)
										AddedPtsIdList.push_back(ptId);
								}
							}					
						 }
						//Add points Id of those points which does not have any framgrabtype given in above function........
						std::list<int> WithoutActionPtsIdList;
						for(PC_ITER Item = PtColl.getList().begin(); Item != PtColl.getList().end(); Item++)
						{
							SinglePoint* Spt = (*Item).second;
							bool IdPresent = false;
							for each(int PtId in AddedPtsIdList)
							{
								if(PtId == Spt->PtID)
								{
									IdPresent = true;
									break;
								}
							}
							if(!IdPresent)
								WithoutActionPtsIdList.push_back(Spt->PtID);
						}

						//points which does not have any action or imported dxf points.........
						for each(int Id in WithoutActionPtsIdList)
						{
							for(PC_ITER Item = PtColl.getList().begin(); Item != PtColl.getList().end(); Item++)
							{
								SinglePoint* Spt = (*Item).second;
								if(Id == Spt->PtID)
								{
									FramegrabBase *Cbaseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::FOCUS_SCAN);
									Vector FocusStartPosition;
									FocusStartPosition.set(Spt->X, Spt->Y, Spt->Z);
									Cbaseaction->PointDRO = FocusStartPosition;
									Cbaseaction->ProfileON = false;
									Cbaseaction->SurfaceON = true;
									Cbaseaction->myPosition[0].set(40, 40);
									Cbaseaction->myPosition[1].set(1 , 1, 0);
									Cbaseaction->myPosition[2].set(0.4 , 0.2, 2);
									Cbaseaction->AllPointsList.Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z, Spt->R, Spt->Pdir));
									//default light property..................
									Cbaseaction->camProperty = MAINDllOBJECT->getCamSettings();
									std::string defaultLightSetting = "0000000082828282";
									Cbaseaction->lightProperty = defaultLightSetting.c_str();
									Cbaseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
									AddPointAction::pointAction(((ShapeWithList*)newShape), Cbaseaction);
								}
							}
						}
					}
					((CloudPoints*)CShape)->TempShapeCollection[i] = newShape;
					i++;
				}
			}
			surface_list.clear();
		}
		else if (ptscount == 2)
		{
			newShape = new Line(false, RapidEnums::SHAPETYPE::LINE3D);
			AddShapeAction::addShape(newShape, false);
			newShape->ShapeDoneUsingProbe(shapedoneusingprobe);
			((ShapeWithList*)newShape)->AddPoints(CShape->PointsList);
			((CloudPoints*) CShape)->TempShapeCollection[0] = newShape;
		}
		dontUpdateGraphcis = false;
		Shape_Updated();
	}
	catch(...) { dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1261", __FILE__, __FUNCSIG__); }
}

//Add line arc seperation
void RCadApp::AddLineorArcShape_Points()
{
	try
	{
		ShapeWithList* CShape = (ShapeWithList*)getShapesList().selectedItem();
		if(CShape->ShapeType != RapidEnums::SHAPETYPE::CLOUDPOINTS) return;
		Shape* newShape;
				
		list<LineArc> LineArcColl;
		list<Pt> AllPointsCollection;
		map<int, Pt> AllPointsColl;
		map<int, Pt> AllPointsTempCollection;
		Pt toppoint; bool flag = true;
		int Cnt = 0;
		for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
		{
			SinglePoint* Spt = (*SptItem).second;
			Pt newpt;
			newpt.x = Spt->X; newpt.y = Spt->Y;
			newpt.pt_index = Cnt;
			if(flag)
			{ 
				flag = false;
				toppoint.x = newpt.x; toppoint.y = newpt.y; 
				toppoint.pt_index = newpt.pt_index;
			}
			else
			{
				if(newpt.y > toppoint.y)
				{
					toppoint.x = newpt.x; toppoint.y = newpt.y; 
					toppoint.pt_index = newpt.pt_index;
				}
			}
			AllPointsColl[Cnt] = newpt;
			Cnt++;
		}
		Cnt = 0;
		Pt FisrtPt;
		FisrtPt.x = toppoint.x; FisrtPt.y = toppoint.y;
		FisrtPt.pt_index = 0;
		//toppoint.pt_index = 0;
		int TotalCnt = AllPointsColl.size();
		AllPointsTempCollection[0] = FisrtPt;
		AllPointsCollection.push_back(FisrtPt);
		AllPointsColl.erase(toppoint.pt_index);
		
		for(int i = 0; i < TotalCnt - 1; i++)
		{
			Pt tempPoint = AllPointsTempCollection[Cnt];
			Pt CurrentPt;
			double dist = 0, mindist = 0; flag = true;
			int C_index;
			for(map<int, Pt>::iterator Pitem = AllPointsColl.begin(); Pitem != AllPointsColl.end(); Pitem++)
			{
				Pt Cpt = (*Pitem).second;
				double tempdist = 0;
				tempdist += pow((tempPoint.x - Cpt.x), 2);  
				tempdist += pow((tempPoint.y - Cpt.y), 2);  
				dist = sqrt(tempdist);
				if(flag)
				{ 
					mindist = dist; flag = false;
					CurrentPt.x = Cpt.x; CurrentPt.y = Cpt.y;
					C_index = Cpt.pt_index;
				}
				else
				{
					if(mindist > dist)
					{
						mindist = dist;
						CurrentPt.x = Cpt.x; CurrentPt.y = Cpt.y;
						C_index = Cpt.pt_index;
					}
				}
			}
			Cnt++;
			CurrentPt.pt_index = Cnt;
			AllPointsTempCollection[Cnt] = CurrentPt;
			AllPointsCollection.push_back(CurrentPt);
			AllPointsColl.erase(C_index);
		}
		double tolerance = getWindow(0).getUppX();
		BESTFITOBJECT->fnLinesAndArcs(&AllPointsCollection, &LineArcColl, tolerance);
		map<int, Shape*> CurrentShapeLst;
		Shape* LastShape = NULL;
		for (list<LineArc>::iterator linearc_iter = LineArcColl.begin(); linearc_iter != LineArcColl.end(); linearc_iter++)
		{
			PointCollection PtColl;
			int start_index = linearc_iter->start_pt.pt_index;
			int end_index = linearc_iter->end_pt.pt_index;
			if(abs(linearc_iter->slope) < 0.02) linearc_iter->slope = 0;
			Pt tempPoint = AllPointsTempCollection[start_index];
			PtColl.Addpoint(new SinglePoint(tempPoint.x, tempPoint.y, MAINDllOBJECT->getCurrentDRO().getZ()));
			tempPoint = AllPointsTempCollection[end_index];
			PtColl.Addpoint(new SinglePoint(tempPoint.x, tempPoint.y, MAINDllOBJECT->getCurrentDRO().getZ()));
			if(linearc_iter->slope == 0) // Add new Line..
			{
				newShape = new Line();
			}
			else
			{
				newShape = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
				((Circle*)newShape)->CircleType = RapidEnums::CIRCLETYPE::ARC_MIDDLE;
				tempPoint = AllPointsTempCollection[(int)(start_index + end_index)/2];
				PtColl.Addpoint(new SinglePoint(tempPoint.x, tempPoint.y, MAINDllOBJECT->getCurrentDRO().getZ()));
			}
			linearc_iter->mid_pt.pt_index = newShape->getId();
			CurrentShapeLst[linearc_iter->mid_pt.pt_index] = newShape;
			newShape->ShapeAsfasttrace(true);
			AddShapeAction::addShape(newShape, false);
			if(LastShape != NULL)
			{
				newShape->addFgNearShapes(LastShape);
				LastShape->addFgNearShapes(newShape);
			}
			((ShapeWithList*)newShape)->AddPoints(&PtColl);
			LastShape = newShape;
		}

		LastShape = NULL;
		for (list<LineArc>::iterator linearc_iter = LineArcColl.begin(); linearc_iter != LineArcColl.end(); linearc_iter++)
		{
			PointCollection PtColl;
			int start_index = linearc_iter->start_pt.pt_index;
			int end_index = linearc_iter->end_pt.pt_index;
			if(abs(linearc_iter->slope) < 0.02) linearc_iter->slope = 0;
			for(int i = start_index; i <= end_index; i++)
			{
				Pt tempPoint = AllPointsTempCollection[i];
				PtColl.Addpoint(new SinglePoint(tempPoint.x, tempPoint.y, MAINDllOBJECT->getCurrentDRO().getZ()));
			}
			LastShape = CurrentShapeLst[linearc_iter->mid_pt.pt_index];
			((ShapeWithList*)LastShape)->AddPoints(&PtColl);
		}
		Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1262", __FILE__, __FUNCSIG__); }
}

//Calculate area for one shot
void RCadApp::CalculateAreaUsingOneShot()
{
	try
	{
		
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y, upp = MAINDllOBJECT->getWindow(0).getUppX();
		MAINDllOBJECT->SetImageInEdgeDetection();
		int RectFGArray[4] = {0, 0, MAINDllOBJECT->getWindow(0).getWinDim().x, MAINDllOBJECT->getWindow(0).getWinDim().y};
		int noofpts = EDSCOBJECT->DetectEdgeStraightRect(&RectFGArray[0], EdgeDetectionSingleChannel::ScanDirection::Rightwards);
		if (noofpts < 400) //If less than 200 try in other direction
			noofpts = EDSCOBJECT->DetectEdgeStraightRect(&RectFGArray[0], EdgeDetectionSingleChannel::ScanDirection::Downwards);

		PointCollection CPointsList;

		for (int n = 0; n < noofpts; n++)
				CPointsList.Addpoint(new SinglePoint(cx + (EDSCOBJECT->DetectedPointsList[n * 2] * upp), cy - (EDSCOBJECT->DetectedPointsList[n * 2 + 1] * upp), 0.0));
		/*Shape* Cshh = new Line();
		AddShapeAction::addShape(Cshh);
		((ShapeWithList*)Cshh)->AddPoints(&CPointsList);
		return;*/

		list<LineArc> LineArcColl;
		list<Pt> AllPointsCollection;
		map<int, Pt> AllPointsColl;
		map<int, Pt> AllPointsTempCollection;
		Pt toppoint; bool flag = true;
		int Cnt = 0;
		for(PC_ITER SptItem = CPointsList.getList().begin(); SptItem != CPointsList.getList().end(); SptItem++)
		{
			SinglePoint* Spt = (*SptItem).second;
			Pt newpt;
			newpt.x = Spt->X; newpt.y = Spt->Y;
			newpt.pt_index = Cnt;
			if(flag)
			{ 
				flag = false;
				toppoint.x = newpt.x; toppoint.y = newpt.y; 
				toppoint.pt_index = newpt.pt_index;
			}
			else
			{
				if(newpt.y > toppoint.y)
				{
					toppoint.x = newpt.x; toppoint.y = newpt.y; 
					toppoint.pt_index = newpt.pt_index;
				}
			}
			AllPointsColl[Cnt] = newpt;
			Cnt++;
		}
		Cnt = 0;
		Pt FisrtPt;
		FisrtPt.x = toppoint.x; FisrtPt.y = toppoint.y;
		FisrtPt.pt_index = 0;
		//toppoint.pt_index = 0;
		int TotalCnt = AllPointsColl.size();
		AllPointsTempCollection[0] = FisrtPt;
		AllPointsCollection.push_back(FisrtPt);
		AllPointsColl.erase(toppoint.pt_index);
		
		for(int i = 0; i < TotalCnt - 1; i++)
		{
			Pt tempPoint = AllPointsTempCollection[Cnt];
			Pt CurrentPt;
			double dist = 0, mindist = 0; flag = true;
			int C_index;
			for(map<int, Pt>::iterator Pitem = AllPointsColl.begin(); Pitem != AllPointsColl.end(); Pitem++)
			{
				Pt Cpt = (*Pitem).second;
				double tempdist = 0;
				tempdist += pow((tempPoint.x - Cpt.x), 2);  
				tempdist += pow((tempPoint.y - Cpt.y), 2);  
				dist = sqrt(tempdist);
				if(flag)
				{ 
					mindist = dist; flag = false;
					CurrentPt.x = Cpt.x; CurrentPt.y = Cpt.y;
					C_index = Cpt.pt_index;
				}
				else
				{
					if(mindist > dist)
					{
						mindist = dist;
						CurrentPt.x = Cpt.x; CurrentPt.y = Cpt.y;
						C_index = Cpt.pt_index;
					}
				}
			}
			Cnt++;
			CurrentPt.pt_index = Cnt;
			AllPointsTempCollection[Cnt] = CurrentPt;
			AllPointsCollection.push_back(CurrentPt);
			AllPointsColl.erase(C_index);
		}

		list<Shape*> CurrentShapeLst;
		int PointsCount = (int)AllPointsTempCollection.size();
		for(int i = 0; i < PointsCount; i++)
		{
			PointCollection PtColl;
			Shape* nshape = new Line(false);
			int Stindex = i, endindex = i + 12;
			if(endindex >= PointsCount)
				endindex = 0;
			Pt tempPoint1 = AllPointsTempCollection[Stindex];
			Pt tempPoint2 = AllPointsTempCollection[endindex];
			PtColl.Addpoint(new SinglePoint(tempPoint1.x, tempPoint1.y, MAINDllOBJECT->getCurrentDRO().getZ()));
			PtColl.Addpoint(new SinglePoint(tempPoint2.x, tempPoint2.y, MAINDllOBJECT->getCurrentDRO().getZ()));
			((ShapeWithList*)nshape)->AddPoints(&PtColl);
			CurrentShapeLst.push_back(nshape);
			i += 11;
		}

		Shape* Pshape = new Perimeter();
		((Perimeter*)Pshape)->TotalPerimeter(0);
		((Perimeter*)Pshape)->Diameter(0);
		AddShapeAction::addShape(Pshape);

		for each (Shape* CSh in CurrentShapeLst)
			((Perimeter*)Pshape)->AddShape(CSh);
		((ShapeWithList*)Pshape)->UpdateBestFit();
		Shape_Updated();


		//double tolerance = 0.02; // getWindow(0).getUppX() / 10000;
		//fnLinesAndArcs(&AllPointsCollection, &LineArcColl, tolerance);
		//map<int, Shape*> CurrentShapeLst;
		//Shape* newShape = NULL;
		//Shape* LastShape = NULL;
		//list<LineArc>::iterator Temp = LineArcColl.begin();
		//list<LineArc>::iterator Temp1 = LineArcColl.end();
		//Temp1--;
		//int FistShIndex = (*Temp).start_pt.pt_index;
		//int LastShIndex = (*Temp1).start_pt.pt_index;
		//int ShapeCount = LineArcColl.size();
		//int TempShapeCount = 0;
		//for each(LineArc Cla in LineArcColl)
		//{
		//	PointCollection PtColl;
		//	int start_index = Cla.start_pt.pt_index;
		//	int end_index = Cla.end_pt.pt_index;
		//	if(abs(Cla.slope) < 0.02) Cla.slope = 0;
		//	Pt tempPoint = AllPointsTempCollection[start_index];
		//	PtColl.Addpoint(new SinglePoint(tempPoint.x, tempPoint.y));
		//	tempPoint = AllPointsTempCollection[end_index];
		//	if(TempShapeCount == ShapeCount - 1)
		//		tempPoint = AllPointsTempCollection[FistShIndex];
		//	PtColl.Addpoint(new SinglePoint(tempPoint.x, tempPoint.y));
		//	if(Cla.slope == 0) // Add new Line..
		//	{
		//		newShape = new Line(false);
		//	}
		//	else
		//	{
		//		newShape = new Circle(false, RapidEnums::SHAPETYPE::ARC);
		//		((Circle*)newShape)->CircleType = RapidEnums::CIRCLETYPE::ARC_MIDDLE;
		//		tempPoint = AllPointsTempCollection[(int)(start_index + end_index)/2];
		//		PtColl.Addpoint(new SinglePoint(tempPoint.x, tempPoint.y));
		//	}
		//	Cla.mid_pt.pt_index = newShape->getId();
		//	CurrentShapeLst[Cla.mid_pt.pt_index] = newShape;
		//	newShape->ShapeAsfasttrace(true);
		//	newShape->ShapeForPerimeter(true);
		//	//AddShapeAction::addShape(newShape);
		//	if(LastShape != NULL)
		//	{
		//		newShape->addFgNearShapes(LastShape);
		//		LastShape->addFgNearShapes(newShape);
		//	}
		//	if(TempShapeCount == ShapeCount - 1)
		//	{
		//		Shape* Csh = (*CurrentShapeLst.begin()).second;
		//		Csh->addFgNearShapes(newShape);
		//		newShape->addFgNearShapes(Csh);
		//	}
		//	((ShapeWithList*)newShape)->AddPoints(&PtColl);
		//	LastShape = newShape;
		//	TempShapeCount++;
		//}

		//LastShape = NULL;
		//for each(LineArc Cla in LineArcColl)
		//{
		//	PointCollection PtColl;
		//	int start_index = Cla.start_pt.pt_index;
		//	int end_index = Cla.end_pt.pt_index;
		//	if(abs(Cla.slope) < 0.02) Cla.slope = 0;
		//	for(int i = start_index; i <= end_index; i++)
		//	{
		//		Pt tempPoint = AllPointsTempCollection[i];
		//		PtColl.Addpoint(new SinglePoint(tempPoint.x, tempPoint.y));
		//	}
		//	LastShape = CurrentShapeLst[Cla.mid_pt.pt_index];
		//	((ShapeWithList*)LastShape)->AddPoints(&PtColl);
		//}

		//Shape* Pshape = new Perimeter();
		//((Perimeter*)Pshape)->TotalPerimeter(0);
		//((Perimeter*)Pshape)->Diameter(0);
		//AddShapeAction::addShape(Pshape);

		//for(map<int, Shape*>::iterator ShItem = CurrentShapeLst.begin(); ShItem != CurrentShapeLst.end(); ShItem++)
		//{
		//	Shape* CSh = (*ShItem).second;
		//	((Perimeter*)Pshape)->AddShape(CSh);
		//}
		//((ShapeWithList*)Pshape)->UpdateBestFit();
		//Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1263", __FILE__, __FUNCSIG__); }
}

//Apply line arc seperation
void RCadApp::ApplyLineArcSeperation_SelectedShape()
{ 
	try
	{
		//if(ApplyLineArcSeperationMode()) return;
		ShapeWithList* CShape = (ShapeWithList*)getShapesList().selectedItem();
		if(CShape == NULL)
		{
			SetStatusCode("RCadApp19");
			return;
		}
		if(getSelectedShapesList().count() > 1)
		{
			//If selected shapes are greater than one return..
			SetStatusCode("RCadApp20");
			return;
		}
		if(!(CShape->ShapeType == RapidEnums::SHAPETYPE::LINE || CShape->ShapeType == RapidEnums::SHAPETYPE::ARC || CShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE))
		{
			//If selected shape is 3D then return..
			SetStatusCode("RCadApp21");
			return;
		}
		CShape->DoLineArcSeperation();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1264", __FILE__, __FUNCSIG__); }
}

void RCadApp::DoIdorOdMeasurement(bool flag)
{
	try
	{
		//SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::DETECTALLEDGED_HANDLER);
		//((FrameGrabIdAndOd*)currentHandler)->SetAddPointAction();
		if(flag)
			SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::ODMEASURE_HANDLER);
		else
			SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::IDMEASURE_HANDLER);
		((FrameGrabIdAndOd*)currentHandler)->SetAddPointAction();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1265", __FILE__, __FUNCSIG__); }
}

//Set Image in Edge detection
bool RCadApp::SetImageInEdgeDetection(int Channel_Number, bool SetImageinBytes, int MaxTrialNumber)
{
	try
	{
		bool Tempflag = false;
		//if (!CameraConnected())
		//{
		//	for (int i = 0; i < 800 * 600; i++)
		//		pixelvalues[i] = staticImageBytes[i * 4 + Channel_Number];
		//	EDSCOBJECT->SetImage(&pixelvalues[0], false);
		//	return true;
		//}
		memset(pixelvalues, 0, currCamWidth * currCamHeight * 3 * sizeof(double));
		memset(pixelBytes, 0, currCamWidth * currCamHeight);
		for (int j = 0; j < MaxTrialNumber; j++)
		{
			if (!this->UseAvImage())
			{
				//if (SetImageinBytes)
				//	Tempflag = GetImageNow_byte_1bpp(Channel_Number, &pixelBytes[0], !MAINDllOBJECT->DigitalZoomMode());
				//else
					Tempflag = GetImageNow_double_1bpp(Channel_Number, &pixelvalues[0], !MAINDllOBJECT->DigitalZoomMode());
					//Tempflag = GetImageNow_ByteDouble_1bpp(Channel_Number, &pixelvalues[0], &pixelBytes[0], !MAINDllOBJECT->DigitalZoomMode());
			}
			else
			{
				Tempflag = GetAverageImage_double_1bpp(Channel_Number, &pixelvalues[0], !MAINDllOBJECT->DigitalZoomMode(), NoOfFramesToAverage()); //???  where is defined  ???
			}
			if (Tempflag) break;
			Sleep(100);
		}
		//if(!Tempflag)
		//{
		//	Sleep(100);
		//	if (!UseAvImage())
		//	{
		//		if (SetImageinBytes)
		//			Tempflag = GetImageNow_byte_1bpp(Channel_Number, &pixelBytes[0], !MAINDllOBJECT->DigitalZoomMode());
		//		else
		//			Tempflag = GetImageNow_double_1bpp(Channel_Number, &pixelvalues[0], !MAINDllOBJECT->DigitalZoomMode());
		//	}
		//	else
		//	{
		//		Tempflag = GetAverageImage_double_1bpp(Channel_Number, &pixelvalues[0], !MAINDllOBJECT->DigitalZoomMode(), NoOfFramesToAverage());
		//	}
		//}
		//if (SetImageinBytes)
			//EDSCOBJECT->SetImage(&pixelBytes[0]);
			//EDSCOBJECT->SetImage(&pixelBytes[0], false, CurrentCamera());
		//else
		EDSCOBJECT->SetImage(&pixelvalues[0], this->UseAvImage(), CurrentCamera());
		return Tempflag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1266", __FILE__, __FUNCSIG__); return false;}
}

bool RCadApp::SetImageInEdgeDetectionForDifferentChannel(int Channel_Number)
{
	try
	{
		/*bool Tempflag = GetImageNow_double_1bpp(Channel_Number, &pixelvalues_RBChannel[0], !DigitalZoomMode());
		if(!Tempflag)
		{
			Sleep(100);
			Tempflag = GetImageNow_double_1bpp(Channel_Number, &pixelvalues_RBChannel[0], !DigitalZoomMode());
		}
		EDSCOBJECT->SetImageforDifferentChannel(&pixelvalues_RBChannel[0]);
		return Tempflag;*/
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1267", __FILE__, __FUNCSIG__); return false;}
}

double RCadApp::GetAvgOfImage()
{
	return EDSCOBJECT->GetAvgOfImage();
}

//Set Zoomed Image in Edge detection,.. Double the size..
bool RCadApp::SetZoomedImageInEdgeDetection()
{
	try
	{
		if(this->DigitalZoomModeForOneShot())
		{
			dontUpdateGraphcis = true;
			CameraClose();

			bool TempCheckFlag = true;
			if(Initialise(HiddenPictureHandle, 1, 102))
			{
				int wwidth = getWindow(0).getWinDim().x, wheight = getWindow(0).getWinDim().y;
				int frate = 5;
				if(wwidth == 1024) frate = 6;
				int TempW = 2 * wwidth, TempH = wheight * 2;
				SetVideoWindow(TempW,  TempH, frate, false, MAINDllOBJECT->CameraDistortionFactor, MAINDllOBJECT->CamXOffSet, MAINDllOBJECT->CamYOffSet);
				if(CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL || CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP)
					FlipVertical();
				Preview();
				Set_ImBufferMode(true);
				SetProfileLightMode(ProfileON);
				//if(CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT)
				//	SetCamProperty("58FF0080010000");
				//else
				//	SetCamProperty("58FF008001004C");
				Sleep(2000);
				TempCheckFlag = GetImageNow_double_1bpp(1, &pixelvalues[0], true);
				if(!TempCheckFlag)
				{
					Sleep(2000);
					TempCheckFlag = GetImageNow_double_1bpp(1, &pixelvalues[0], true);
				}
			}
			EDSCOBJECT->SetImage(&pixelvalues[0], false, CurrentCamera());
			dontUpdateGraphcis = false;
			CameraClose();
			if(Initialise(getWindow(0).getHandle(), 1, 102))
			{
				SetVideoWindow(getWindow(0).getWinDim().x, getWindow(0).getWinDim().y, VideoFrameRate(), false, MAINDllOBJECT->CameraDistortionFactor, MAINDllOBJECT->CamXOffSet, MAINDllOBJECT->CamYOffSet);
				if(CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL || CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP)
					FlipVertical();
				Preview();
				Set_ImBufferMode(true);
				SetProfileLightMode(ProfileON);
				//if(CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT)
				//	SetCamProperty("58FF0080010000");
				//else
				//	SetCamProperty("58FF008001004C");
			}
			update_VideoGraphics();
			return TempCheckFlag;
		}
		else
		{
			bool Tempflag = GetImageNow_double_1bpp(1, &pixelvalues[0], !DigitalZoomMode());
			if(!Tempflag)
			{
				Sleep(100);
				Tempflag = GetImageNow_double_1bpp(1, &pixelvalues[0], !DigitalZoomMode());
			}
			EDSCOBJECT->SetImage(&pixelvalues[0], false, CurrentCamera());
			return Tempflag;
		}
		return false;
	}
	catch(...){ dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1268", __FILE__, __FUNCSIG__); return false;}
}


void RCadApp::Finalise_CAxis_Calc()
{	
	//Finalise C axis for hob checker
	if (CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HOBCHECKER)
	{	
		double CylParam[7] = {0}; 
		CylParam[5] = 1;
		Cylinder *Cy = (Cylinder*)MAINDllOBJECT->getShapesList().selectedItem(); //(Cylinder*)MAINDllOBJECT->ReferenceDotShape;
		Cy->getParameters(CylParam);
		
		//Calculate the point at Z = 0; use this as our BC centre, always. 
		double rr = CylParam[2]/CylParam[5];			
		TIS_CAxis[0] = rr * CylParam[3] + CylParam[0];
		TIS_CAxis[1] = rr * CylParam[4] + CylParam[1];
		TIS_CAxis[2] = 0;
		
		//Copy angle and radius values to TIS_CAxis
		//Also see if direction cosine N is -1. if so, multiply each angle by -1!
		double signchanger = 1;
		if (CylParam[5] < 0) signchanger = -1;

		for (int i = 3; i < 6; i ++)
			TIS_CAxis[i] = signchanger * CylParam[i];
		//Write the Diameter of the cylinder; not the radius!
		TIS_CAxis[6] = 2 * CylParam[6];
		//Keep the copy of this cylinder since TIS_CAxis will be used for RunoutCorrection
		//for (int i = 0; i < 6; i ++)
		//	BC_Axis[i] = TIS_CAxis[i]; 
		return;
	}
	//Calculate_PtsAround_CAxis();

	//PtsAroundCAxis->clear();
	//delete PtsAroundCAxis;
	//PtsAroundCAxis = NULL;
	//CAxis_PtsCount = 0;
	//LastRVal = 0;
}

//Select Scan mode Point..
void RCadApp::SelectScanPoint(int *position, RapidEnums::RAPIDFGACTIONTYPE Fgtype, bool runningprogram)
{
	int tempBoundaryBuffer = EDSCOBJECT->BoundryBuffer;
	try
	{
		ShapeWithList* sh = (ShapeWithList*)getShapesList().selectedItem();
		EDSCOBJECT->BoundryBuffer = 4;
		if (sh != NULL)
		{
			double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y, upp = MAINDllOBJECT->getWindow(0).getUppX() / HELPEROBJECT->CamSizeRatio;
			double ans[2]; SinglePoint scanpt;
			bool Ptselected = false;
			SetImageInEdgeDetection();
			if (EDSCOBJECT->GetEdgePoint(position, EdgeDetectionSingleChannel::ScanDirection::Rightwards, true, &ans[0]))
			{
				scanpt.X = cx + ans[0] * upp;
				scanpt.Y = cy - ans[1] * upp;
				scanpt.Z = getCurrentUCS().GetCurrentUCS_DROpostn().getZ();
				Ptselected = true;
			}
			else if (EDSCOBJECT->GetEdgePoint(position, EdgeDetectionSingleChannel::ScanDirection::Downwards, true, &ans[0]))
			{
				scanpt.X = cx + ans[0] * upp;
				scanpt.Y = cy - ans[1] * upp;
				scanpt.Z = getCurrentUCS().GetCurrentUCS_DROpostn().getZ();
				Ptselected = true;
			}
			if (Ptselected)
			{
				FramegrabBase* baseaction;
				if (runningprogram)
					baseaction = PPCALCOBJECT->getFrameGrab();
				else
					baseaction = new FramegrabBase(Fgtype);
				baseaction->myPosition[0].set(position[0], position[1]);
				baseaction->points[0].set(cx + (position[0] + position[2] / 2) * upp, cy - (position[1] + position[2] / 2) * upp);
				baseaction->FGWidth = position[2];
				baseaction->PointDRO.set(MAINDllOBJECT->getCurrentDRO());
				if (CameraConnected())
				{
					baseaction->camProperty = getCamSettings();
					baseaction->lightProperty = getLightProperty();
					baseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
				}
				baseaction->ProfileON = getProfile();
				baseaction->SurfaceON = getSurface();
				baseaction->AllPointsList.Addpoint(new SinglePoint(scanpt.X, scanpt.Y, scanpt.Z));
				if (runningprogram)
					sh->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
				else
					AddPointAction::pointAction(sh, baseaction);
				SetStatusCode("RCadApp22");
			}
			else
				SetStatusCode("RCadApp23");
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1269", __FILE__, __FUNCSIG__); }
	EDSCOBJECT->BoundryBuffer = tempBoundaryBuffer;
}

//Add point using live scan cross hair..
bool RCadApp::LiveScanCrosshair(int *position)
{
	try
	{
		double ans[2];	
		bool flag = false;
		SetImageInEdgeDetection();
		if(EDSCOBJECT->GetEdgePoint(position, EdgeDetectionSingleChannel::ScanDirection::Rightwards, true, &ans[0]))
			flag = true;
		else if(EDSCOBJECT->GetEdgePoint(position, EdgeDetectionSingleChannel::ScanDirection::Downwards, true, &ans[0]))
			flag = true;
		return flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1270", __FILE__, __FUNCSIG__); return false;}
}

//display the mouse position on DRO.. for mouse move on video if any drawing tool is selected..//
void RCadApp::ToolSelectedMousemove(double x, double y, double z)
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning() && MAINDllOBJECT->IsCNCMode())return;
		double data[3] = {0};
		if (AAxisHorizontal)
		{
			if (getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
			{
				data[0] = x - getWindow(0).getCenter().x;
				data[1] = y - getWindow(0).getCenter().y;
				data[2] = z;
			}
			else if (getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
			{
				data[0] = x; // -getWindow(0).getCenter().x;
				data[1] = y - getWindow(0).getCenter().x;
				data[2] = z - getWindow(0).getCenter().y;
			}
			else if (getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
			{
				data[0] = x - getWindow(0).getCenter().x;
				data[1] = y; // -getWindow(0).getCenter().y;
				data[2] = z - getWindow(0).getCenter().y;
			}
			else if (getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
			{
				data[0] = x; // -getWindow(0).getCenter().x;
				data[1] = y - getWindow(0).getCenter().x;
				data[2] = getWindow(0).getCenter().y - z;
			}
			else if (getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
			{
				data[0] = x - getWindow(0).getCenter().x;
				data[1] = y;
				data[2] = getWindow(0).getCenter().y - z;
			}
		}
		else
		{
			if (getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
			{
				data[0] = x - getWindow(0).getCenter().x;
				data[1] = y - getWindow(0).getCenter().y;
			}
			else if (getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
			{
				data[1] = x - getWindow(0).getCenter().x;
				data[2] = y - getWindow(0).getCenter().y;
			}
			else if (getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
			{
				data[0] = x - getWindow(0).getCenter().x;
				data[2] = y - getWindow(0).getCenter().y;
			}
			else if (getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
			{
				data[1] = x - getWindow(0).getCenter().x;
				data[2] = getWindow(0).getCenter().y - y;
			}
			else if (getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
			{
				data[0] = x - getWindow(0).getCenter().x;
				data[2] = getWindow(0).getCenter().y - y;
			}
		}
		UpdateMousePositionOnVideo(&data[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1271", __FILE__, __FUNCSIG__); }
}

//To display the current shape parameters in the shape parameter textbox..
void RCadApp::CurrentShapeProperties(double x, double y, double z, double z1, double type)
{
	try
	{
		return;
		if(PPCALCOBJECT->IsPartProgramRunning() && MAINDllOBJECT->IsCNCMode())return;
		double data[5] = {x, y, z, z1, type};
		UpdateShapeParamForMouseMove(&data[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1272", __FILE__, __FUNCSIG__); }
}

//Transforamtion calcualtion.. Multiply with the transformation matrix...!!
Vector RCadApp::TransformMultiply(double *transform, double x, double y, double z)
{
	try
	{
		int s1[2] = {3, 3}, s2[2] = {3, 1};
		double c[3], p[3] = {x, y, z};
		RMATH2DOBJECT->MultiplyMatrix1(transform, &s1[0], &p[0], &s2[0], &c[0]);
		return(Vector(c[0], c[1], c[2]));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1273", __FILE__, __FUNCSIG__); return Vector(0,0,0);}
}

Vector RCadApp::TransformMultiply_PlaneAlign(double *transform, double x, double y, double z)
{
	try
	{
		int s1[2] = {4, 4}, s2[2] = {4, 1};
		double c[4], p[4] = {x, y, z, 1};
		RMATH2DOBJECT->MultiplyMatrix1(transform, &s1[0], &p[0], &s2[0], &c[0]);
		return(Vector(c[0], c[1], c[2]));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1274", __FILE__, __FUNCSIG__); return Vector(0,0,0);}
}

//Transform DRO vlaues for UCS..
bool RCadApp::TransformDROValues(double x, double y, double z, double *Tvalues)
{
	try
	{
		int s1[2] = {3, 3}, s2[2] = {3, 1};
		double p[3] = {x, y, 1};
		if (!(getCurrentUCS().UCSMode() == 4))
		{
			RMATH2DOBJECT->MultiplyMatrix1(&getCurrentUCS().transform[0], &s1[0], &p[0], &s2[0], Tvalues);
			//Vector v = getCurrentUCS().UCSPoint(); // getCurrentUCS().UCSPoint();
			//Tvalues[2] = z - getCurrentUCS().UCSPoint.getZ();
			//Tvalues[3] -= getCurrentUCS().UCSPoint().getR();
		}
		else
		{
			Tvalues[0] = x;
			Tvalues[1] = y;
			Tvalues[2] = z;
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1275", __FILE__, __FUNCSIG__); return true; }
}

//returns vector pointer for snap point.. Used in partprogram...
Vector* RCadApp::getVectorPointer(Vector* pt, bool considerz)
{
	return this->getCurrentUCS().getIPManager().getNearestPoint(pt->getX(), pt->getY(), 0.0001, pt->getZ(), considerz);
}

Vector* RCadApp::getVectorPointer_UCS(Vector* pt, UCS* Cucs, bool considerz)
{
	return Cucs->getIPManager().getNearestPoint(pt->getX(), pt->getY(), 0.0001, pt->getZ(), considerz);
}

//Add Axis Shapes..
void RCadApp::AddAxisShapes(UCS* currentUCS)
{
	try
	{
		Shape* sh;
		sh = new Line(_T("Y Axis"), RapidEnums::SHAPETYPE::XLINE);
		((Line*)sh)->setPoint1(Vector(0, 999999, 0));
		((Line*)sh)->setPoint2(Vector(0, -999999, 0));
		((Line*)sh)->setLineParameters(M_PI_2, 0);
		((Line*)sh)->setLineParameters(0, 1, 0); 
		sh->Normalshape(false);
		sh->setModifiedName("Y Axis");
		((Line*)sh)->IsValid(true);
		sh->UcsId(currentUCS->getId());
		currentUCS->getShapes().addItem(sh, false);
		Line::decrement();
		currentUCS->Y_AxisLine = ((Line*)sh);

		sh = new Line(_T("X Axis"), RapidEnums::SHAPETYPE::XLINE);
		((Line*)sh)->setPoint1(Vector(999999, 0, 0));
		((Line*)sh)->setPoint2(Vector(-999999, 0, 0));
		((Line*)sh)->setLineParameters(0, 0);
		((Line*)sh)->setLineParameters(1, 0, 0);
		sh->Normalshape(false);
		sh->setModifiedName("X Axis");
		((Line*)sh)->IsValid(true);
		sh->UcsId(currentUCS->getId());
		currentUCS->getShapes().addItem(sh, false);
		currentUCS->getIPManager().itemChanged(sh, (Listenable*)sh);
		Line::decrement();
		currentUCS->X_AxisLine = ((Line*)sh);

		sh = new Line(_T("Z Axis"), RapidEnums::SHAPETYPE::LINE3D);
		((Line*)sh)->setPoint1(Vector(0, 0, 999999));
		((Line*)sh)->setPoint2(Vector(0, 0, -999999));
		((Line*)sh)->setLineParameters(0, 0);
		((Line*)sh)->setLineParameters(0, 0, 1);
		sh->Normalshape(false);
		sh->setModifiedName("Z Axis");
		((Line*)sh)->IsValid(true);
		sh->UcsId(currentUCS->getId());
		currentUCS->getShapes().addItem(sh, false);
		currentUCS->getIPManager().itemChanged(sh, (Listenable*)sh);
		Line::decrement();
		currentUCS->Z_AxisLine = ((Line*)sh);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1276", __FILE__, __FUNCSIG__); }
}

void RCadApp::DrawCoordinatePlaneFor3DMode(UCS* currentUCS)
{
	try
	{
		Plane* CSh = new Plane();
		PointCollection PtColl;
		PtColl.Addpoint(new SinglePoint(0, 0, 0));
		PtColl.Addpoint(new SinglePoint(0, 200, 0));
		PtColl.Addpoint(new SinglePoint(200, 200, 0));
		PtColl.Addpoint(new SinglePoint(200, 0, 0));
		CSh->AddPoints(&PtColl);
		CSh->setModifiedName("XY Plane");
		CSh->Normalshape(false);
		CSh->ShapeColor.set(0, 0, 1);
		CSh->IsValid(false);
		CSh->UcsId(currentUCS->getId());
		currentUCS->getShapes().addItem(CSh, false);
		currentUCS->getIPManager().itemChanged(CSh, (Listenable*)CSh);
		Plane::decrement();
		currentUCS->XY_CoordinatePlane = CSh;

		CSh = NULL;
		CSh = new Plane();
		PtColl.deleteAll();
		PtColl.Addpoint(new SinglePoint(0, 0, 0));
		PtColl.Addpoint(new SinglePoint(0, 0, 200));
		PtColl.Addpoint(new SinglePoint(0, 200, 200));
		PtColl.Addpoint(new SinglePoint(0, 200, 0));
		CSh->AddPoints(&PtColl);
		CSh->setModifiedName("YZ Plane");
		CSh->Normalshape(false);
		CSh->IsValid(false);
		CSh->ShapeColor.set(1, 0, 0);
		CSh->UcsId(currentUCS->getId());
		currentUCS->getShapes().addItem(CSh, false);
		currentUCS->getIPManager().itemChanged(CSh, (Listenable*)CSh);
		Plane::decrement();
		currentUCS->YZ_CoordinatePlane = CSh;

		CSh = NULL;
		CSh = new Plane();
		PtColl.deleteAll();
		PtColl.Addpoint(new SinglePoint(0, 0, 0));
		PtColl.Addpoint(new SinglePoint(200, 0, 0));
		PtColl.Addpoint(new SinglePoint(200, 0, 200));
		PtColl.Addpoint(new SinglePoint(0, 0, 200));
		CSh->AddPoints(&PtColl);
		CSh->setModifiedName("ZX Plane");
		CSh->Normalshape(false);
		CSh->IsValid(false);
		CSh->ShapeColor.set(0, 1, 0);
		CSh->UcsId(currentUCS->getId());
		currentUCS->getShapes().addItem(CSh, false);
		currentUCS->getIPManager().itemChanged(CSh, (Listenable*)CSh);
		Plane::decrement();
		currentUCS->ZX_CoordinatePlane = CSh;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1278", __FILE__, __FUNCSIG__); }
}

void RCadApp::AddMarkingShape(UCS* currentUCS)
{
	try
	{
		currentUCS->MarkingShape = new Line(_T("M_Shape"), RapidEnums::SHAPETYPE::XLINE);
		((Line*)currentUCS->MarkingShape)->setPoint1(Vector(999999, 0, 0));
		((Line*)currentUCS->MarkingShape)->setPoint2(Vector(-999999, 0, 0));
		((Line*)currentUCS->MarkingShape)->setLineParameters(0, 0);
		((Line*)currentUCS->MarkingShape)->setLineParameters(1, 0, 0);
		currentUCS->MarkingShape->setId(ToolAxisLineId + 1, true);
		currentUCS->MarkingShape->Normalshape(false);
		((Line*)currentUCS->MarkingShape)->IsValid(false);
		currentUCS->MarkingShape->UcsId(currentUCS->getId());
		currentUCS->getShapes().addItem(currentUCS->MarkingShape, false);
		currentUCS->getIPManager().itemChanged(currentUCS->MarkingShape, (Listenable*)currentUCS->MarkingShape);
		Line::decrement();
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1279", __FILE__, __FUNCSIG__);
	}
}

//Add new shapes..
void RCadApp::AddNewShape(RapidEnums::SHAPETYPE Stype)
{
	try
	{
		switch(Stype)
		{
			case RapidEnums::SHAPETYPE::LINE:
				AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
				break;
			case RapidEnums::SHAPETYPE::CIRCLE:
				AddShapeAction::addShape(new Circle(_T("C"), RapidEnums::SHAPETYPE::CIRCLE));
				break;
			case RapidEnums::SHAPETYPE::ARC:
				AddShapeAction::addShape(new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC));
				break;
			case RapidEnums::SHAPETYPE::RPOINT:
				AddShapeAction::addShape(new RPoint());
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1280", __FILE__, __FUNCSIG__); }
}

//Handle Esc key pressed..
void RCadApp::EscButtonHandling()
{
	try
	{
		currentHandler->EscapebuttonPress();
		currentHandler->resetClicks();
		update_VideoGraphics();
		update_RcadGraphics();
		update_DxfGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1281", __FILE__, __FUNCSIG__); }
}

//Handle shape win cancel pressed..
void RCadApp::HandleCancelPressed()
{
	try
	{
		switch(CURRENTHANDLE)
		{
			case RapidEnums::RAPIDHANDLERTYPE::MEASURE_HANDLER:
			case RapidEnums::RAPIDHANDLERTYPE::CIRCLETANTO2CIRCLE:
			case RapidEnums::RAPIDHANDLERTYPE::CIRCLETANTO2CIRCLE1:
			case RapidEnums::RAPIDHANDLERTYPE::ONE_POINTANGLE_LINE:
			case RapidEnums::RAPIDHANDLERTYPE::ONE_POINT_OFFSET_LINE:
			case RapidEnums::RAPIDHANDLERTYPE::TANGENTIAL_CIRCLE:
			case RapidEnums::RAPIDHANDLERTYPE::CIRCLE_FRAMEGRAB:
			case RapidEnums::RAPIDHANDLERTYPE::FIXEDRECT_FRAMEGRAB:
			case RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB:
			case RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB:
			case RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB:
			case RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB_ALLEDGE:
			case RapidEnums::RAPIDHANDLERTYPE::PLANEFROMTWOLINEHANDLER:
				currentHandler->EscapebuttonPress();
				currentHandler->resetClicks();
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1282", __FILE__, __FUNCSIG__); }
}
 
//Write initialisation log
void RCadApp::WriteModuleInitialization(std::string data)
{
	try
	{
		std::wofstream cfile;
		cfile.open(currentPath.c_str(),std::ios_base::app);
		cfile << data.c_str() << endl;
		cfile.close();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1283", __FILE__, __FUNCSIG__); }
}

 void RCadApp::CallCalculateDiffShapes(double tolerance, double max_radius, double min_radius, double min_angle_cutoff, double noise, double max_dist_betn_pts, bool closed_loop, bool shape_as_fast_trace, bool join_all_pts)
 {
	 try
	 {
		((LineArcAction *)HELPEROBJECT->ProfileScanLineArcAction)->deleteAll();
		 update_VideoGraphics();
		((SM_LineArcFGHandler*)(MAINDllOBJECT->currentHandler))->CalculateDiffShapes(tolerance, max_radius, min_radius, min_angle_cutoff, noise, max_dist_betn_pts, closed_loop, shape_as_fast_trace, join_all_pts);
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("NRCAD0001", __FILE__, __FUNCSIG__); }
 }

 void RCadApp::CallAddShapes()
 {
	 try
	 {
		 update_VideoGraphics();
		((SM_LineArcFGHandler*)(MAINDllOBJECT->currentHandler))->AddFinalShapes();
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("NRCAD0002", __FILE__, __FUNCSIG__); }
 }

 void RCadApp::ClearLineArcShapes()
 { 
	 try
	 {
		((SM_LineArcFGHandler*)(MAINDllOBJECT->currentHandler))->ClearLineArc();
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("NRCAD0003", __FILE__, __FUNCSIG__); }
 }

 void RCadApp::AddNewPlaneForSelectedPoints()
 {
	 ShapeWithList* CShape = (ShapeWithList*)getShapesList().selectedItem();
	 if(CShape == NULL)  return;
	 if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
	 {
		 ShapeWithList* CSh = new Plane();
		 PointCollection PtColl;
		 for(PC_ITER item = CShape->PointsList->getList().begin(); item != CShape->PointsList->getList().end(); item++)
		 {
			 SinglePoint* Spt = item->second;
			 if(Spt->IsSelected)
				PtColl.Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z));
		 }
		 CSh->AddPoints(&PtColl);
		 AddShapeAction::addShape(CSh, true);	
		 UpdateShapesChanged();
	 }
 }

 void RCadApp::AddNewSplineForSelectedPoints()
 {
	 ShapeWithList* CShape = (ShapeWithList*)getShapesList().selectedItem();
	 if(CShape == NULL)  return;
	 if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
	 {
		 ShapeWithList* CSh = new Spline(_T("S"));
		 ((Spline*)CSh)->ClosedSpline = false;
		 ((Spline*)CSh)->framgrabShape = true;
		 PointCollection PtColl;
		 for(PC_ITER item = CShape->PointsList->getList().begin(); item != CShape->PointsList->getList().end(); item++)
		 {
			 SinglePoint* Spt = item->second;
			 if(Spt->IsSelected)
				PtColl.Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z));
		 }
		 CSh->AddPoints(&PtColl);
		 AddShapeAction::addShape(CSh, true);	
		 UpdateShapesChanged();
	 }
 }

 void RCadApp::AddNewCircleForSelectedPoints()
 {
	 ShapeWithList* CShape = (ShapeWithList*)getShapesList().selectedItem();
	 if(CShape == NULL)  return;
	 if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
	 {
		 ShapeWithList* CSh = new Circle(_T("C3D"),RapidEnums::SHAPETYPE::CIRCLE3D);
		 ((Circle*)CSh)->CircleType = RapidEnums::CIRCLETYPE::CIRCLEFROMCLOUDPNTS;
		 PointCollection PtColl;
		 for(PC_ITER item = CShape->PointsList->getList().begin(); item != CShape->PointsList->getList().end(); item++)
		 {
			 SinglePoint* Spt = item->second;
			 PtColl.Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z));
		 }
		 CSh->AddPoints(&PtColl);
		 CSh->addParent(CShape);
		 CShape->addChild(CSh);
		 AddShapeAction::addShape(CSh, true);	
		 UpdateShapesChanged();
	 }
 }

void RCadApp::AddNewLineForSelectedPoints()
{
	 ShapeWithList* CShape = (ShapeWithList*)getShapesList().selectedItem();
	 if(CShape == NULL)  return;
	 if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
	 {
		 ShapeWithList* CSh = new Line(_T("L"), RapidEnums::SHAPETYPE::LINE);
		 ((Line*)CSh)->LineType = RapidEnums::LINETYPE::LINEFROMCLOUDPNTS;
		 PointCollection PtColl;
		 for(PC_ITER item = CShape->PointsList->getList().begin(); item != CShape->PointsList->getList().end(); item++)
		 {
			SinglePoint* Spt = item->second;
			PtColl.Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z));
		 }
		 CSh->AddPoints(&PtColl);
		 CSh->addParent(CShape);
		 CShape->addChild(CSh);
		 AddShapeAction::addShape(CSh, true);	
		 UpdateShapesChanged();
	 }
}

void RCadApp::deleteOtherShapes(int i)
 {
	 try
	 {
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		MAINDllOBJECT->dontUpdateGraphcis = true;
		map<int, Shape*> tempshapecollection;
		int i = 0;
		for(RC_ITER Item_shape = getCurrentUCS().getShapes().getList().begin(); Item_shape != getCurrentUCS().getShapes().getList().end(); Item_shape++)
		{
			Shape* Cshape = (Shape*)(*Item_shape).second;
			if(Cshape->Normalshape() && Cshape->getId() != i)
			{
				tempshapecollection[i] = Cshape;
				i++;
			}	
		}
		for (int i = 0; i < tempshapecollection.size(); i++)
		{
			Shape* Cshape = tempshapecollection[i];
			getCurrentUCS().getShapes().getList().erase(Cshape->getId());
			delete Cshape;
		}
		MAINDllOBJECT->dontUpdateGraphcis = false;
		update_VideoGraphics(true);
		update_RcadGraphics(true);
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
	 }
	 catch(...)
	 {
		 MAINDllOBJECT->dontUpdateGraphcis = false;
		 MAINDllOBJECT->SetAndRaiseErrorMessage("NRCAD0004", __FILE__, __FUNCSIG__); 
	 }
 }

void RCadApp::deletetextfilefromPointslog()
{
	try
	{
		deleteAlltextfilefromPointsLog();
	}
	 catch(...)
	 {
		
		 MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1284", __FILE__, __FUNCSIG__); //RCAD0013
	 }
 }

//function to get fgpoints from text file....................
void RCadApp::GetFgPoints_fromTextFile()
{
	try
	{
		ShapeWithList* myShape = (ShapeWithList*)(MAINDllOBJECT->getShapesList().selectedItem());
		myShape->Stepcount = 0;
		myShape->SkipPtscount = 0;
		int ShapeId = myShape->getId();
		//ShapeWithList* myShape = (ShapeWithList*)(MAINDllOBJECT->getShapesList().getList()[ShapeId]);
		std::string filePath, filename;
		double savedPts[3] = {0};
		std::ifstream PointCollectionReader;
		char test[50];
		_itoa(ShapeId, test, 10);
		filename = (const char*)(test);
		filePath = MAINDllOBJECT->currentPath + "\\PointsLog\\" + filename +".txt";
		PointCollectionReader.open(filePath);
		if(!PointCollectionReader)
			return;
		PointCollection PointsTempCollection;
		while(!PointCollectionReader.eof())
		{
			PointCollectionReader>>savedPts[0];
			PointCollectionReader>>savedPts[1];
			PointCollectionReader>>savedPts[2];
			PointsTempCollection.Addpoint(new SinglePoint(savedPts[0], savedPts[1], savedPts[2]));
		}				
		PointCollectionReader.close();
		myShape->AddPoints(&PointsTempCollection);
		std::remove(filePath.c_str());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1285", __FILE__, __FUNCSIG__); } //RCAD0015
}

void RCadApp::SavePointsToHardDisc()
{
	try
	{
		ShapeWithList* myShape = (ShapeWithList*)(MAINDllOBJECT->getShapesList().selectedItem());
		//if u want to change pts count value(20000) than u have to change in shapewithlist also GetDownSampledPoints....//
		while(myShape->PointsList->Pointscount() > MAINDllOBJECT->SamplePtsCutOff)
		{
			myShape->GetDownSampledPoints();
		}	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1286", __FILE__, __FUNCSIG__); }//RCAD0016
}

bool RCadApp::AddPauseAfterAction()
 {
	try
	{
		RAction *Caction = NULL;
		if(MAINDllOBJECT->getActionsHistoryList().count() == 0) 
			return false;
		RC_ITER ppCAction = MAINDllOBJECT->getActionsHistoryList().getList().end();
		if(ppCAction == MAINDllOBJECT->getActionsHistoryList().getList().begin())
			return false;
		else
			ppCAction--;
		Caction = (RAction*)(*ppCAction).second;
		if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			((AddPointAction*)Caction)->PausableAction = true;
		else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			((AddProbePathAction*)Caction)->PausableAction = true;
		else if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			((AddThreadMeasureAction*)Caction)->PausableAction = true;
		else
		   return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1287", __FILE__, __FUNCSIG__); }//RCAD0016a
}

void RCadApp::MeasurementForHighlightedShape(int MeasureType, bool AutomateFixMeasure)
{
	try
	{
		if(MeasurementMode() && CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::MEASURE_HANDLER)
		{
			((DimensionHandler*)getCurrentHandler())->MeasurementForHighlighted_Shape(MeasureType, AutomateFixMeasure);
		}
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1288", __FILE__, __FUNCSIG__);//RCAD0017b
	}
}

void RCadApp::HighlightShapeForMeasure(int id)
{
	try
	{
		if(MeasurementMode() && CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::MEASURE_HANDLER)
		{
			((DimensionHandler*)getCurrentHandler())->HighlightShapeForMeasurement(id);
		}
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1289", __FILE__, __FUNCSIG__);//RCAD0016b
	}
}

void RCadApp::CloudPointsMeasure(list<string> measureList)
{
	try
	{
		if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CLOUDPOINTMEASUREMENTHANDLER)
		{
			((CloudPointMeasurementsHandler*)MAINDllOBJECT->getCurrentHandler())->AddMeasurement(measureList);
		}
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1290", __FILE__, __FUNCSIG__);//RCAD0016b
	}
}

void RCadApp::HighlightShapeRightClickForMeasure(int id)
{
	try
	{
		if(MeasurementMode() && CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::MEASURE_HANDLER)
		{
			((DimensionHandler*)getCurrentHandler())->HighlightShapeRightClickForMeasurement(id);
		}
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1291", __FILE__, __FUNCSIG__);//RCAD0016b
	}
}

void RCadApp::AddProbePathPoint(RAction *atn)
{
	if(ProbePathPoints.find(atn->getId()) == ProbePathPoints.end())
	{
		double *droPnt = new double[3];
		FramegrabBase* fg = ((AddProbePathAction*)atn)->getFramegrab();
		droPnt[0] = fg->PointDRO.getX();
		droPnt[1] = fg->PointDRO.getY();
		droPnt[2] = fg->PointDRO.getZ();
		ProbePathPoints[atn->getId()] = droPnt;
	}
}

void RCadApp::AddPoints_Framgrab(PointCollection *ptcoll, FramegrabBase* currentAction, std::list<int> *AddedPtsId)
{
	try
	{
		std::list<int> PtsIdList;
		double wupp = MAINDllOBJECT->getWindow(0).getUppX();
		switch(currentAction->FGtype)
		{
		case RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB:
			{
				double point1[2] = {0}, point2[2] = {0};
				if (currentAction->points[0].getY() < currentAction->points[1].getY())
				{
					point1[0] = currentAction->points[0].getX(); point1[1] = currentAction->points[0].getY();
					point2[0] = currentAction->points[1].getX(); point2[1] = currentAction->points[1].getY();
				}
				else
				{
					point1[0] = currentAction->points[1].getX(); point1[1] = currentAction->points[1].getY();
					point2[0] = currentAction->points[0].getX(); point2[1] = currentAction->points[0].getY();
				}
				GetPointsId_AngRectangleFramgrab(ptcoll, point1, point2, currentAction->FGWidth * wupp, &PtsIdList);
			}
			break;
		case RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB:
			{
				double point1[2] = {currentAction->points[0].getX(), currentAction->points[0].getY()}, point2[2] = {currentAction->points[1].getX(), currentAction->points[1].getY()},
				point3[2] = {currentAction->points[2].getX(), currentAction->points[2].getY()};
				double cen[2] = {0}, radius = 0, radius1 = 0, radius2 = 0, startangle = 0, sweepangle = 0, ep1[4] = {0}, ep2[4] = {0};
				RMATH2DOBJECT->Arc_3Pt(&point1[0], &point2[0], &point3[0], &cen[0], &radius, &startangle, &sweepangle);
				radius1 = radius + currentAction->FGWidth * wupp + wupp;
				radius2 = radius - currentAction->FGWidth * wupp - wupp;
				RMATH2DOBJECT->ArcFrameGrabEndPts(&cen[0], radius1, radius2, startangle, sweepangle, &ep1[0], &ep2[0]);
				GetPointsId_CirAndArcFramgrab(ptcoll, radius1, radius2, cen, ep1, ep2, &PtsIdList);
			}
			break;
		case RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB: 
			{
				double radius = 0, radius1 = 0, radius2 = 0, cen[2] = {0};
				double Points2[2] = {(currentAction->points[0].getX() + currentAction->points[1].getX())/2 , (currentAction->points[0].getY() + currentAction->points[1].getY())/2 };
				radius = RMATH2DOBJECT->Pt2Pt_distance(currentAction->points[0].getX(),currentAction->points[0].getY(), Points2[0], Points2[1]);
				radius1 = radius - currentAction->FGWidth * wupp - wupp;
				radius2 = radius + currentAction->FGWidth * wupp + wupp;
				GetPointsId_CirAndArcFramgrab(ptcoll, radius1, radius2, Points2, NULL, NULL, &PtsIdList);
			}
			break;
		case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB: 			
		case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES: 			
		case RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE: 
			{
				for(PC_ITER Item = ptcoll->getList().begin(); Item != ptcoll->getList().end(); Item++)
				{
					SinglePoint* Spt = (*Item).second;
					double minX = currentAction->points[0].getX(), minY = currentAction->points[0].getY(), maxX = currentAction->points[1].getX(), maxY = currentAction->points[1].getY();
					if(currentAction->points[0].getX() > currentAction->points[1].getX())
					{
						minX = currentAction->points[1].getX(); maxX = currentAction->points[0].getX();
					}
					if(currentAction->points[0].getY() > currentAction->points[1].getY())
					{
						minY = currentAction->points[1].getY(); maxY = currentAction->points[0].getY();
					}
					if(Spt->Point_IsInWindow(minX, minY, maxX, maxY))
						PtsIdList.push_back(Spt->PtID);
				}
			}
			break;
		case RapidEnums::RAPIDFGACTIONTYPE::FOCUS_CONTOURSCAN:
		case RapidEnums::RAPIDFGACTIONTYPE::FOCUS_SCAN:
			{
				double noOfrows = currentAction->myPosition[1].getX(), noOfcolumns = currentAction->myPosition[1].getY(), cellwidth = currentAction->myPosition[0].getX(), cellheight = currentAction->myPosition[0].getY();
				Vector UpperCorner;
				Vector LowerCorner;
				UpperCorner.set(currentAction->PointDRO.getX() - noOfcolumns * cellwidth * wupp / 2, currentAction->PointDRO.getY() + noOfrows * cellheight * wupp / 2);
				LowerCorner.set(currentAction->PointDRO.getX() + noOfcolumns * cellwidth * wupp / 2, currentAction->PointDRO.getY() - noOfrows * cellheight * wupp / 2);				
				for(PC_ITER Item = ptcoll->getList().begin(); Item != ptcoll->getList().end(); Item++)
				{
					SinglePoint* Spt = (*Item).second;
					double minX = UpperCorner.getX(), minY = UpperCorner.getY(), maxX = LowerCorner.getX(), maxY = LowerCorner.getY();
					if(UpperCorner.getX() > LowerCorner.getX())
					{
						minX = LowerCorner.getX(); maxX = UpperCorner.getX();
					}
					if(UpperCorner.getY() > LowerCorner.getY())
					{
						minY = LowerCorner.getY(); maxY = UpperCorner.getY();
					}
					if(Spt->Point_IsInWindow(minX, minY, maxX, maxY))
						PtsIdList.push_back(Spt->PtID);
				}
			}
			break;
		case RapidEnums::RAPIDFGACTIONTYPE::PROBE_POINT:
		case RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR:
		case RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE:
			{
				for(PC_ITER Item = ptcoll->getList().begin(); Item != ptcoll->getList().end(); Item++)
				{
					SinglePoint* Spt = (*Item).second;
					if((abs(currentAction->points[0].getX() - Spt->X) < 0.000001 && abs(currentAction->points[0].getY() - Spt->Y) < 0.000001 && abs(currentAction->points[0].getZ() - Spt->Z) < 0.000001))
					{
						PtsIdList.push_back(Spt->PtID);
						break;
					}
				}
			}
			break;
		}
		for(PC_ITER Item = ptcoll->getList().begin(); Item != ptcoll->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			for each(int PtId in PtsIdList)
			{
				if(PtId == Spt->PtID)
				{
					AddedPtsId->push_back(PtId);
					currentAction->AllPointsList.Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z, Spt->R, Spt->Pdir));
					break;
				}
			}
		}
		currentAction->noofptstaken_build = currentAction->AllPointsList.Pointscount();
		currentAction->noofpts = currentAction->AllPointsList.Pointscount();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1292", __FILE__, __FUNCSIG__); }//RCAD0017
}

void RCadApp::GetPointsId_AngRectangleFramgrab(PointCollection *ptcoll, double* point1, double* point2, double width, std::list<int> *PtIdList)
{
	try
	{
		double height = RMATH2DOBJECT->Pt2Pt_distance(point1, point2);	
		double SinTh = (point2[0] - point1[0]) / sqrt(pow((point2[0] - point1[0]),2) + pow((point2[1] - point1[1]),2));
		double CosTh = (point2[1] - point1[1]) / sqrt(pow((point2[0] - point1[0]),2) + pow((point2[1] - point1[1]),2));
		for(PC_ITER Item = ptcoll->getList().begin(); Item != ptcoll->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			double XTransformed = (Spt->X - point1[0]) * CosTh - (Spt->Y - point1[1]) * SinTh;
			double YTransformed = (Spt->X - point1[0]) * SinTh + (Spt->Y - point1[1]) * CosTh;
			if(XTransformed <= width && XTransformed >= -width)
			{
				if(YTransformed <= height && YTransformed >= 0)
				{
					PtIdList->push_back(Spt->PtID);
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1293", __FILE__, __FUNCSIG__); }//RCAD0018
}

void RCadApp::GetPointsId_CirAndArcFramgrab(PointCollection *ptcoll, double r1, double r2, double* center, double *endPoint1, double *endPoint2, std::list<int> *PtIdList)
{
	try
	{
		double max = 0, min = 0;
		if(r1 > r2)	
		{
			max = r1;  min = r2;
		}
		else
		{
			max = r2;  min = r1;
		}	
		double ang1 = 0, ang2 = 0;
		ang1 = RMATH2DOBJECT->ray_angle(endPoint1, center);
		ang2 = RMATH2DOBJECT->ray_angle(endPoint2, center);
		for(PC_ITER Item = ptcoll->getList().begin(); Item != ptcoll->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			double tmp[2] = {Spt->X, Spt->Y};
			double dist = RMATH2DOBJECT->Pt2Pt_distance(tmp, center);
			if(min <= dist && max >= dist)
			{
				if(endPoint1 != NULL)
				{
					double ang, intcpt, intcpt1, intcpt2; 				
					ang = RMATH2DOBJECT->ray_angle(tmp, center);
					if(ang - ang1 > 0.001)
					{
						if(ang - ang2 > 0.001) continue;
					}
					else if(ang2 - ang > 0.001) continue;
				}
				PtIdList->push_back(Spt->PtID);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1294", __FILE__, __FUNCSIG__); }//RCAD0019
}

//insert action in main action history list....
void RCadApp::ArrangeActionForPPEdit()
{
	try
	{
		RCollectionBase TempActionList;
		RC_ITER itemAction = MAINDllOBJECT->getActionsHistoryList().getList().end();
		itemAction--;
		RAction* Lastaction = (RAction*)(*itemAction).second;
		int LastActionId = Lastaction->getId();
		
		for(RC_ITER ppCAction = MAINDllOBJECT->getActionsHistoryList().getList().begin(); ppCAction != MAINDllOBJECT->getActionsHistoryList().getList().end(); ppCAction++)
		{
			RAction* Caction = (RAction*)(*ppCAction).second;
			if(Caction->getId() <= LastActionIdForPPEdit)
				TempActionList.addItem(Caction);
			else if(LastActionId > Caction->getId())
			{
				Caction->setId(Caction->getId() + 1);
				TempActionList.addItem(Caction);
			}
		}
		Lastaction->setId(LastActionIdForPPEdit + 1);
		TempActionList.addItem(Lastaction);
		MAINDllOBJECT->getActionsHistoryList().clear();
		for(RC_ITER ppCAction = TempActionList.getList().begin(); ppCAction != TempActionList.getList().end(); ppCAction++)
		{
			RAction* Caction = (RAction*)(*ppCAction).second;
			MAINDllOBJECT->getActionsHistoryList().addItem(Caction);
		}
		LastActionIdForPPEdit += 1;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1295", __FILE__, __FUNCSIG__); }//RCAD0020
}

void RCadApp::WriteStringToFile(std::list<std::string> Liststr, std::string filepath, bool AppendFlag)
{
	try
	{
		std::wofstream TextWriter;
		if(AppendFlag)
			TextWriter.open(filepath, std::ios_base::app);
		else
			TextWriter.open(filepath);
		for(std::list<std::string>::iterator itr = Liststr.begin(); itr != Liststr.end(); itr++)
			TextWriter << (*itr).c_str() << endl;						
		TextWriter.close();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1296", __FILE__, __FUNCSIG__); }//RCAD0021
}

void RCadApp::ShowDeviationwithCloudPts(double UpperCutoff, double tolerance, double interval, int IntervalType, bool ClosedLoop, bool TwoD_Deviation)
{
	try
	{
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DXF_DEVIATION_HANDLER);
		((DXFDeviationHandler*)MAINDllOBJECT->getCurrentHandler())->DXFDeviationData(UpperCutoff, tolerance, interval, IntervalType, ClosedLoop, true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1297", __FILE__, __FUNCSIG__); }//RCAD0022
}

 void RCadApp::ComparePoints_CloudPts(double Angle, bool ArcBestFit, int NumberOfPts)
 {
	 try
	 {
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CLOUDCOMPARISION_HANDLER);
		((CloudComparisionHandler*)MAINDllOBJECT->getCurrentHandler())->Point_CloudptComparatorParm(Angle, ArcBestFit, NumberOfPts);
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1298", __FILE__, __FUNCSIG__); }//RCAD0022a
 }

void RCadApp::CompareCloudPts(double interval, bool ArcBestFit, int NumberOfPts)
{
	try
	{
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CLOUDCOMPARISION_HANDLER);
		((CloudComparisionHandler*)MAINDllOBJECT->getCurrentHandler())->CloudptComparatorParm(interval, ArcBestFit, NumberOfPts);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1299", __FILE__, __FUNCSIG__); }//RCAD0022a
}

void RCadApp::CompareCloudPts(int MeasureCount, bool ArcBestFit, int NumberOfPts)
{
	try
	{
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CLOUDCOMPARISION_HANDLER);
		((CloudComparisionHandler*)MAINDllOBJECT->getCurrentHandler())->CloudptComparatorParm(MeasureCount, ArcBestFit, NumberOfPts);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1300", __FILE__, __FUNCSIG__); }//RCAD0022a
}

void RCadApp::StartRotationalScan(bool ScanStart)
{
	try
	{
		if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_CIRCLE_FRAMEGRAB || MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARC_FRAMEGRAB ||
			MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_RECT_FRAMEGRAB || MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARECT_FRAMEGRAB 
			|| MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_FIXEDRECT_FRAMEGRAB)
		{
			((RotationScanHandler*)MAINDllOBJECT->getCurrentHandler())->StartRotationalScanning(ScanStart);
		}
		else if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::SMARTMEASUREMODE_HANDLER)
		{
			((RotationScanHandler*)MAINDllOBJECT->getCurrentHandler()->CurrentdrawHandler)->StartRotationalScanning(ScanStart);
		}
		else if (MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::SELECTPOINT_HANDLER)
		{
			//If we have the USB Gage running in Height Gauge mode, let us start scanning continuously in a separate thread....
			((SelectPointHandler*)MAINDllOBJECT->getCurrentHandler())->StartContinuousScanning(ScanStart);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1301", __FILE__, __FUNCSIG__); }//RCAD0022b
}

void RCadApp::ContinuePauseRotationalScan(bool Pauseflag)
{
	try
	{
		if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_CIRCLE_FRAMEGRAB || MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARC_FRAMEGRAB ||
			MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_RECT_FRAMEGRAB || MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARECT_FRAMEGRAB 
			|| MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_FIXEDRECT_FRAMEGRAB)
		{
			((RotationScanHandler*)MAINDllOBJECT->getCurrentHandler())->CotninuePauseRotationalScanning(Pauseflag);
		}
		else if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::SMARTMEASUREMODE_HANDLER)
		{
			((RotationScanHandler*)MAINDllOBJECT->getCurrentHandler()->CurrentdrawHandler)->CotninuePauseRotationalScanning(Pauseflag);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1302", __FILE__, __FUNCSIG__); }//RCAD0022b
}

void RCadApp::DoOnePointAlignment(double x, double y, double z)
{
	try
	{
		if(this->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CAD_1PT_EDITALIGN_HANDLER)
		{
			((EditCad1PtHandler*)currentHandler)->SelectPoints_HeightGauge(x, y, z);
		}
		else if(this->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::CAD_2PT_EDITALIGN_HANDLER)
		{
			((EditCad2PtHandler*)currentHandler)->SelectPoints_HeightGauge(x, y, z);
		}
		/*else
		{
			ShapeWithList* CShape = ((ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem());
			if(CShape != NULL)
			{
				PointCollection PtColl;
				PtColl.Addpoint(new SinglePoint(x, y, z));
				CShape->AddPoints(&PtColl);
			}
		}*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1303", __FILE__, __FUNCSIG__); }//RCAD0023
}

void RCadApp::CalculateCloudPtZlevelExtent()
{
	try
	{
		CloudPtZExtent[0] = 0; CloudPtZExtent[1] = 0;
		if(MAINDllOBJECT->ShowPictorialView() && !MAINDllOBJECT->TriangulateCpSurface())
		{	
			bool firstTime = true;
			for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
			{
				ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
				if(CShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
				{
					if(CShape->PointsList->Pointscount() < 1) continue;
					if(firstTime)
					{
						firstTime = false;
						PC_ITER SptItem = CShape->PointsList->getList().begin();
						SinglePoint* Spt = (*SptItem).second;						
						this->CloudPtZExtent[0] = Spt->Z;
						this->CloudPtZExtent[1] = Spt->Z;
					}
					for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
					{
						SinglePoint* Spt = (*SptItem).second;
						if(this->CloudPtZExtent[0] > Spt->Z)
						{
							this->CloudPtZExtent[0] = Spt->Z;
						}
						if(this->CloudPtZExtent[1] < Spt->Z)
						{
							this->CloudPtZExtent[1] = Spt->Z;
						}
					}
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCAD1304", __FILE__, __FUNCSIG__); }//RCAD0024
}

void RCadApp::GotoLastLocalisedAction()
{
	 map<int,BaseItem*>::iterator tmpAction = getActionsHistoryList().getList().end();
	 RAction* atn = NULL;
	 bool found = false;
	 while(tmpAction != getActionsHistoryList().getList().begin())
	 {
	   tmpAction--;
	   atn = static_cast<RAction*>((*currentaction).second);
	   if(atn->CurrentActionType == RapidEnums::ACTIONTYPE::LOCALISEDCORRECTION)
	   {
		  found = true;
		  break;
	   }
	 }
	 if(found)
	 {
		GoToNextPosition(atn, false);
	 }
}

bool RCadApp::CreatePointsForDelphiComponent(double angle, double *idParam, list<double> *firsthalfpnts, list<double> *scndhalfpnts, Circle *parentCir, Line* parentln)
 {
	 try
	 {
		angle = angle * M_PI / 180;
		Circle *parentcircle = parentCir;
		Line *parentLine = parentln, *CurrentShape = NULL;
	 
		double pnt2[3] = {0}, axisdir[3] = {0, 0, 1}, interceptPoint[4] = {0}, lastPoint[4] = {0};	
		double origin[3] = {0, 0, currentDRO.getZ()}, slope = parentLine->Angle(), intercept = parentLine->Intercept(), radius = parentcircle->Radius(), LineLength = parentLine->Length();
		if(RMATH2DOBJECT->Line_circleintersection(slope, intercept, idParam, idParam[3], interceptPoint) < 2) return false;
		parentcircle->getCenter()->FillDoublePointer(origin);

		parentLine->getPoint1()->FillDoublePointer(pnt2);

		double dist1 =  RMATH2DOBJECT->Pt2Pt_distance(pnt2, interceptPoint);
		double dist2 =  RMATH2DOBJECT->Pt2Pt_distance(pnt2, &interceptPoint[2]);
		if(dist1 > dist2)
		{
			double tmp[2] = {interceptPoint[0], interceptPoint[1]};
			interceptPoint[0] = interceptPoint[2];
			interceptPoint[1] = interceptPoint[3];
			interceptPoint[2] = tmp[0];
			interceptPoint[3] = tmp[1];
		}

		firsthalfpnts->push_back(idParam[0]);
		firsthalfpnts->push_back(idParam[1]);
		firsthalfpnts->push_back(idParam[2]);
		firsthalfpnts->push_back(interceptPoint[0]);
		firsthalfpnts->push_back(interceptPoint[1]);
		firsthalfpnts->push_back(idParam[2]);
		scndhalfpnts->push_back(idParam[0]);
		scndhalfpnts->push_back(idParam[1]);
		scndhalfpnts->push_back(idParam[2]);
		scndhalfpnts->push_back(interceptPoint[2]);
		scndhalfpnts->push_back(interceptPoint[3]);
		scndhalfpnts->push_back(idParam[2]);

		for(int i = 0; i < 2; i++){lastPoint[i] = interceptPoint[i];}
	
		for(double ang = angle; ang <= M_PI - angle; ang = ang + angle)
		{
			CurrentShape = new Line();
			((Line*)CurrentShape)->LineType = RapidEnums::LINETYPE::ONEPT_OFFSETLINE;
			((Line*)CurrentShape)->Length(LineLength);
			((Line*)CurrentShape)->DefinedLength(false);
			((Line*)CurrentShape)->LinePosition(0);
			((Line*)CurrentShape)->Offset(ang);

			FramegrabBase *baseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE);
			baseaction->CurrentWindowNo = 1;;
			setBaseRPropertyNonHighlighted(origin[0], origin[1], origin[2], false, baseaction, CurrentShape);
			parentLine->addChild(CurrentShape);
			CurrentShape->addParent(parentLine);
			AddShapeAction::addShape(CurrentShape);
			AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
			intercept = RMATH2DOBJECT->InterceptOfline(slope + ang, origin);
			
			if(RMATH2DOBJECT->Line_circleintersection(slope + ang, intercept, idParam, idParam[3], interceptPoint) < 2) return false;
			if(RMATH2DOBJECT->Pt2Pt_distance(interceptPoint, lastPoint) < RMATH2DOBJECT->Pt2Pt_distance(&interceptPoint[2], lastPoint))
			{
				for(int i = 0; i < 4; i++) {lastPoint[i] = interceptPoint[i];}
			}
			else
			{
				for(int i = 0; i < 2; i++)
				{
					lastPoint[i] = interceptPoint[i + 2];
					lastPoint[i + 2] = interceptPoint[i];
				}
			} 
			firsthalfpnts->push_back(idParam[0]);
			firsthalfpnts->push_back(idParam[1]);
			firsthalfpnts->push_back(idParam[2]);
			scndhalfpnts->push_back(idParam[0]);
			scndhalfpnts->push_back(idParam[1]);
			scndhalfpnts->push_back(idParam[2]);
			firsthalfpnts->push_back(lastPoint[0]);
			firsthalfpnts->push_back(lastPoint[1]);
			firsthalfpnts->push_back(idParam[2]);
			scndhalfpnts->push_back(lastPoint[2]);
			scndhalfpnts->push_back(lastPoint[3]);
			scndhalfpnts->push_back(idParam[2]);
		}
		return true;
	 }
	 catch(...)
	 {
	   return false;
	 }
 }

void RCadApp::setBaseRPropertyNonHighlighted(double x, double y, double z, bool Probeflag, FramegrabBase *baseaction, Shape *CurrentShape)
{
	try
	{
		Vector tmp, *tmp2 = NULL;
		tmp.set(x,y,z);
		tmp2 = MAINDllOBJECT->getVectorPointer(&tmp);
		if(CurrentShape != NULL && tmp2 != NULL)
		{
			baseaction->Pointer_SnapPt = tmp2;
			baseaction->DerivedShape = true;
			bool PresentFlag1 = false;
			if(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1() != NULL)
			{
				for each(Shape* Csh in CurrentShape->getParents())
				{
					if(Csh->getId() == MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1()->getId())
					{
						PresentFlag1 = true;
						break;
					}
				}
				if(!PresentFlag1)CurrentShape->addParent(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1());
				PresentFlag1 = false;
				for each(Shape* Csh in MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1()->getChild())
				{
					if(Csh->getId() == CurrentShape->getId())
					{
						PresentFlag1 = true;
						break;
					}
				}
				if(!PresentFlag1)MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1()->addChild(CurrentShape);
			}
				
			PresentFlag1 = false;
			if(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2() != NULL)
			{
				for each(Shape* Csh in CurrentShape->getParents())
				{
					if(Csh->getId() == MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2()->getId())
					{
						PresentFlag1 = true;
						break;
					}
				}
				if(!PresentFlag1)CurrentShape->addParent(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2());
				PresentFlag1 = false;
				for each(Shape* Csh in MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2()->getChild())
				{
					if(Csh->getId() == CurrentShape->getId())
					{
						PresentFlag1 = true;
						break;
					}
				}
				if(!PresentFlag1)MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2()->addChild(CurrentShape);
			}
		}
		if(MAINDllOBJECT->CameraConnected())
		{		
			baseaction->camProperty = MAINDllOBJECT->getCamSettings();
			baseaction->lightProperty = MAINDllOBJECT->getLightProperty();
			baseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
		}
		if(Probeflag)
			baseaction->PointDRO.set(PPCALCOBJECT->MachineDro_ProbeTouch);
		else
			baseaction->PointDRO.set(PPCALCOBJECT->CurrentMachineDRO);
		baseaction->DontCheckProjectionType(true);
		baseaction->ProfileON = MAINDllOBJECT->getProfile();
		baseaction->SurfaceON = MAINDllOBJECT->getSurface();
		// Getting DRO position of Mouse Click........
		baseaction->points[0].set(x, y, z);
		//baseaction->allPoints.push_back(&baseaction->points[0]);
		baseaction->AllPointsList.Addpoint(new SinglePoint(baseaction->points[0].getX(), baseaction->points[0].getY(), baseaction->points[0].getZ(), baseaction->points[0].getR(), baseaction->points[0].getP()));
		// Getting Pixel position of Mouse Click.....
		baseaction->myPosition[0].set(MAINDllOBJECT->getWindow(0).getLastWinMouse().x, MAINDllOBJECT->getWindow(0).getLastWinMouse().y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MH0008", __FILE__, __FUNCSIG__); }
}

void RCadApp::GetBinarisedImage()
{
	EDSCOBJECT->ConvertTo3bpp(&MAINDllOBJECT->BinarisedImage[0]);
}

//bool RCadApp::Calculate_CurrentFrameDifference(double* PP_CG_Params, int BoundaryWidth, int BinariseVal, bool SurfaceLightOn)
//{
//	try
//	{
//		bool flag = BESTFITOBJECT->get_CGParams(pixelBytes, currCamWidth, currCamHeight, true, PP_CG_Params, BoundaryWidth, BinariseVal, 1, SurfaceLightOn);
//		if (flag) return true;
//	}
//	catch (...)
//	{
//		MAINDllOBJECT->SetAndRaiseErrorMessage("PPFUNC0044a", __FILE__, __FUNCSIG__); return false;
//	}
//}

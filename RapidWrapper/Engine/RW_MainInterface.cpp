#include "stdafx.h"
#include "RW_MainInterface.h"
#include "..\MAINDLL\DXF\DXFExpose.h"
#include "..\MAINDLL\Engine\FocusFunctions.h"
#include "..\MAINDLL\Actions\AddPointAction.h"
#include "..\MAINDLL\FixedTool\FixedToolCollection.h"
#include "..\MAINDLL\Handlers_Measurement\DimensionHandler.h"
#include "..\MAINDLL\FixedTool\CalibrationText.h"
#include "..\MAINDll\Engine\CalibrationCalculation.h"
#include "..\MAINDll\Engine\ThreadCalcFunctions.h"
#include "..\MAINDll\Helper\Helper.h"
#include "..\MAINDll\Engine\PartProgramFunctions.h"
#include "..\MAINDLL\Engine\OverlapImgWin.h"
#include "RW_RepeatAutoMeasure.h"
#include "RW_AutoProfileScanMeasure.h"
#include "..\MAINDLL\Shapes\Line.h"
#include "..\MAINDLL\Shapes\RPoint.h"
#include "..\MAINDLL\Shapes\Circle.h"
#include "..\MAINDLL\Shapes\CamProfile.h"
#include "RW_CT_SphereCallibration.h"
#include "RW_RepeatShapeMeasure.h"
#include "RW_DelphiCamModule.h"
#include "RW_ImageDelayCalibration.h"
#include "RW_LBlock.h"
//#include "RW_TIS_Control.h"

using namespace System;
using namespace System::Diagnostics;
using namespace Microsoft::Win32;
using namespace System::Management;

bool Is_measurement_mode =false;		// vinod for machine not move on left and right click 
int CCCCCt;

void CALLBACK FrameWorkError(char* errorCode, char* filename, char* functname)
{
	try
	{			
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog(gcnew String(errorCode), gcnew String(filename), gcnew String(functname));
	}
	catch(Exception^ ex){ }
}

void CALLBACK TruepositionCallBack(double x, double y)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "TruepositionCallBack", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->RaiseTruePositionEvent(x, y);
}

void CALLBACK DxfImportCallBack()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "DxfImportCallBack", __FUNCSIG__);
	try
	{			
		RWrapper::RW_MainInterface::MYINSTANCE()->HandleDXFToolbar_Click("Import CAD");
	}
	catch(Exception^ ex){ }
}

void CALLBACK NextProfileScanCallBack()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "NextProfileScanCallBack", __FUNCSIG__);
	RWrapper::RW_AutoProfileScanMeasure::MYINSTANCE()->NextprofileScan();
}

void CALLBACK CallDeviationWindow()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "CallDeviationWindow", __FUNCSIG__);
	try
	{			
		RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(23);
	}
	catch(Exception^ ex){ }
}

void CALLBACK CallCloudPointsMeasureWindow()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "CallCloudPointsMeasureWindow", __FUNCSIG__);
	try
	{			
		RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(24);
	}
	catch(Exception^ ex){ }
}

void CALLBACK FramGrabChanged(char* Alignment, char* btnName, bool btnclick)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "FramGrabChanged", __FUNCSIG__);
	try
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->Update_FramGrab(gcnew System::String(Alignment), gcnew System::String(btnName), btnclick);
	}
	catch(Exception^ ex){ }
}

void CALLBACK CurrentStatusDisplay(char* Str, bool Insert_TempStrFlag, char* TempStr)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "CurrentStatusDisplay", __FUNCSIG__);
	try
	{
		/*switch(MsgType)
		{
			case 0:
				RWrapper::RW_MainInterface::MYINSTANCE()->WaitStatusMessage = RWrapper::RW_MainInterface::MYINSTANCE()->GetStatusMessage(gcnew System::String(Str), " StatusMessage ");
				break
			case 1:
				RWrapper::RW_MainInterface::MYINSTANCE()->WaitStatusMessage = gcnew System::String(Str);
				break;
		}*/
		RWrapper::RW_MainInterface::MYINSTANCE()->Update_HelpStatusMessage(gcnew System::String(Str), Insert_TempStrFlag, gcnew System::String(TempStr));		
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0001", ex);
	}
}

void CALLBACK SurfaceFramegrabMclick()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "SurfaceFramegrabMclick", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->RaiseSurfaceFrameGrabEvent();
}

bool CALLBACK CurrentMsgBoxStatusDisplay(char* Text, int btntype, int icon, bool Insert_TempStrFlag, char* TempStr)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "CurrentMsgBoxStatusDisplay", __FUNCSIG__);
	try
	{
		return RWrapper::RW_MainInterface::MYINSTANCE()->Update_MsgBoxStatus(gcnew System::String(Text), RWrapper::RW_Enum::RW_MSGBOXTYPE(btntype), RWrapper::RW_Enum::RW_MSGBOXICONTYPE(icon), Insert_TempStrFlag, gcnew System::String(TempStr));
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0001", ex);
	}
}

void CALLBACK SurfaceEDUserChoice(char* SFGEntityNames)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "SurfaceEDUserChoice", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->RaiseSurfaceFrameGrabEvent(gcnew System::String(SFGEntityNames));
}

void CALLBACK ClearAllRadiaButtonChecked()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "ClearAllRadiaButtonChecked", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->CallClearRadioButttons();
}

void CALLBACK ClearAllRadiaButtonCheckedBackground()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "ClearAllRadiaButtonCheckedBackground", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->CallClearRadioButttons();
}

void CALLBACK CheckCurrentSmartMeasurement()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "CheckCurrentSmartMeasurement", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->GenerateEventToCheckCurrentSM();
}

void CALLBACK UpdateShapeParamMouseMove(double* ShapeParam)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "UpdateShapeParamMouseMove", __FUNCSIG__);
	RWrapper::RW_ShapeParameter::MYINSTANCE()->SetShapeValues(ShapeParam);
}

void CALLBACK ShowWindowForAutoFocusCall(bool open)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "ShowWindowForAutoFocusCall", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->ShowWindowForAutoFocusRaise(open);
}

void CALLBACK ShowWindowForAutoContourCall(bool open)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "ShowWindowForAutoContourCall", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->ShowWindowForAutoContourRaise(open);
}

void CALLBACK ShowRotationScanWindowCallback(int i)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "ShowRotationScanWindowCallback", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->ShowRotationScanWindowRaise(i);
}

void CALLBACK LineArcUcsCreatedCallback()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "LineArcUcsCreatedCallback", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->LineArcUcsCreated();
}

void CALLBACK HandleUCSParamChanged()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "HandleUCSParamChanged", __FUNCSIG__);
	RWrapper::RW_DRO::MYINSTANCE()->UpdateCenterScreen();
}

void CALLBACK MachineGoToCommand(double x, double y, double z, bool ActivateCNCMode, RapidEnums::MACHINEGOTOTYPE Ctype)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "MachineGoToCommand", __FUNCSIG__);
	try
	{
		TargetReachedCallback CurrentTrType = TargetReachedCallback::MACHINE_GOTOCOMMAND;
		if(ActivateCNCMode)
		{
			if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
		}
		if(Ctype == RapidEnums::MACHINEGOTOTYPE::PROFILESCANGOTO)
			CurrentTrType = TargetReachedCallback::PROFILESCAN_MODE;
		else if(Ctype == RapidEnums::MACHINEGOTOTYPE::RCADWINDOWGOTO)
			CurrentTrType = TargetReachedCallback::MACHINE_GOTORCADWINDOWCLICK;
		else if(Ctype == RapidEnums::MACHINEGOTOTYPE::SAOICELLGOTO)
			CurrentTrType = TargetReachedCallback::MACHINE_GOTOSAOICELLCLICK;
		else if(Ctype == RapidEnums::MACHINEGOTOTYPE::AUTOMEASUREMODEGOTO)
			CurrentTrType = TargetReachedCallback::MACHINE_GOTOAUTOMEASUREMODE;
		else if(Ctype == RapidEnums::MACHINEGOTOTYPE::GRIDIMAGE)
			CurrentTrType = TargetReachedCallback::GRIDIMAGE_SUPERIMPOSE;
		else if(Ctype == RapidEnums::MACHINEGOTOTYPE::REPEATAUTOMEASUREGOTO_AUTOFOCUS)
			CurrentTrType = TargetReachedCallback::GOTO_REPEATAUTOMEASURE;
		else if(Ctype == RapidEnums::MACHINEGOTOTYPE::REPEATAUTOMEASUREGOTO)
			CurrentTrType = TargetReachedCallback::REPEATAUTOMEASURE_AUTOFOCUSPOSITIONREACHED;
		else if(Ctype == RapidEnums::MACHINEGOTOTYPE::DONOTHING)
			CurrentTrType = TargetReachedCallback::DONOTHING;
		else if(Ctype == RapidEnums::MACHINEGOTOTYPE::SPHERVISIONCALIBRATION)
			CurrentTrType = TargetReachedCallback::SPHEREVISIONPROBECALIBRATION;

		double target[4] = {x, y, z, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], CurrentTrType);
	}
	catch(Exception^ ex){ }
}

void CALLBACK ShowLineArcPanel(double tolerance, double max_radius, double min_radius, double min_angle_cutoff, double noise, double max_dist_betn_pts, bool closed_loop, bool shape_as_fast_trace, bool join_all_pts)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "ShowLineArcPanel", __FUNCSIG__);
	try
	{
			/*if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::LINEARCFG_HANDLER)*/
		RWrapper::RW_MainInterface::MYINSTANCE()->LineArcEvent(tolerance, max_radius, min_radius, min_angle_cutoff, noise, max_dist_betn_pts, closed_loop, shape_as_fast_trace, join_all_pts);
	
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0004", ex);
	}
}

void CALLBACK ShowrequiredPanel()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "ShowrequiredPanel", __FUNCSIG__);
	try
	{
		switch(MAINDllOBJECT->CURRENTHANDLE)
		{
			case RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE:
				if(MAINDllOBJECT->GetClickDone() == 1)
					RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(1);
				else
					RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(0);
				break;

			case RapidEnums::RAPIDHANDLERTYPE::PERPENDICULAR_LINE:
			case RapidEnums::RAPIDHANDLERTYPE::PARALLEL_TANGENT:
			case RapidEnums::RAPIDHANDLERTYPE::PERPENDICULAR_TANGENT:
			case RapidEnums::RAPIDHANDLERTYPE::TANGENT_POINT_CIRCLE:
			case RapidEnums::RAPIDHANDLERTYPE::TANGENT_ON_CIRCLE:
			case RapidEnums::RAPIDHANDLERTYPE::ANGLE_BISECTOR:
			case RapidEnums::RAPIDHANDLERTYPE::TANGENT_ON_2CIRCLE:
				if(MAINDllOBJECT->GetClickDone() == 0)
					RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(0);
				else
					RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(10);
				break;

			case RapidEnums::RAPIDHANDLERTYPE::CIRCLETANTO2CIRCLE:
			case RapidEnums::RAPIDHANDLERTYPE::CIRCLETANTO2CIRCLE1:
			case RapidEnums::RAPIDHANDLERTYPE::CIRCLETANTOCIRCLE:
			case RapidEnums::RAPIDHANDLERTYPE::TANGENTIAL_CIRCLE:
			case RapidEnums::RAPIDHANDLERTYPE::CIRCLETANTOLINECIRCLE:
				if(MAINDllOBJECT->GetClickDone() == (MAINDllOBJECT->GetMaxClicks() - 1))
					RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(2);
				else 
					RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(0);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::THREAD_LINEARCHANDLER:
				RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(11);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::SMARTMEASUREMODE_HANDLER:
				if(MAINDllOBJECT->SmartMeasureType == RapidEnums::SMARTMEASUREMENTMODE::RECANGLEMEASUREMENT)
					RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(2);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::PARALLEL_TANGENT_2CIRCLE:
				if(MAINDllOBJECT->GetClickDone() == (MAINDllOBJECT->GetMaxClicks() - 1))
					RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(1);
				else 
					RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(0);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::CIRCLE1P_HANDLER:
			case RapidEnums::RAPIDHANDLERTYPE::CIRCLE3P_HANDLER:
				if(MAINDllOBJECT->GetClickDone() == 0)
					RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(0);
				else
					RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(2);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::FILLET_HANDLER:
				if(MAINDllOBJECT->GetClickDone() == 0)
					RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(0);
				else
					RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(2);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::ONE_POINT_OFFSET_LINE:
				RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(3);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::ONE_POINTANGLE_LINE:
				RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(4);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::PARALLEL_PLANE:
				RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(13);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::PERPENDICULAR_PLANE:
				RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(14);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::PERPENDICULAR_LINE3D:
				RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(15);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE3D:
				RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(16);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::PARALLELARC:
				RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(2);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::RELATIVEPOINT_HANDLER:
				RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(17);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::MOVE_HANDLER:
				RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(18);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::ROTATIONHANDLER:
				RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(20);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::TRANSLATIONHANDLER:
				RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(21);
				break;
			case RapidEnums::RAPIDHANDLERTYPE::MEASURE_HANDLER:
				switch(((DimensionHandler*)MAINDllOBJECT->getCurrentHandler())->CurrentMeasurementType)
				{
					case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R:
						RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(9);
						break;
					case RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_R:
						RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(26);
						break;
					case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_R:
						RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(25);
						break;
					case RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_R:
						RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(27);
						break;
					case RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEDEVIATION_MEASURE:
						RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(0);
						break;
				}
		}
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0004", ex);
	}
}

void CALLBACK ShowrequiredPanelWithPara(double val)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "ShowrequiredPanelWithPara", __FUNCSIG__);
	try
	{
		switch(MAINDllOBJECT->CURRENTHANDLE)
		{
			case RapidEnums::RAPIDHANDLERTYPE::MEASURE_HANDLER:
			switch(((DimensionHandler*)MAINDllOBJECT->getCurrentHandler())->CurrentMeasurementType)
			{
				case RapidEnums::MEASUREMENTTYPE::DIM_LINEPARALLELISM:
				case RapidEnums::MEASUREMENTTYPE::DIM_PLANEPARALLELISM:
				case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPARALLELISM:
				case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPARALLELISM:
				case RapidEnums::MEASUREMENTTYPE::DIM_CONEPARALLELISM:
					RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(6, val);
					break;
				case RapidEnums::MEASUREMENTTYPE::DIM_LINEPERPENDICULARITY:
				case RapidEnums::MEASUREMENTTYPE::DIM_PLANEPERPENDICULARITY:
				case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPERPENDICULARITY:
				case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPERPENDICULARITY:
				case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERCOAXIALITY:
				case RapidEnums::MEASUREMENTTYPE::DIM_CONEPERPENDICULARITY:
				case RapidEnums::MEASUREMENTTYPE::DIM_CONECOAXIALITY:
					RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(7, val);
					break;
				case RapidEnums::MEASUREMENTTYPE::DIM_LINEANGULARITY:
				case RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERANGULARITY:
				case RapidEnums::MEASUREMENTTYPE::DIM_PLANEANGULARITY:
				case RapidEnums::MEASUREMENTTYPE::DIM_LINE3DANGULARITY:
				case RapidEnums::MEASUREMENTTYPE::DIM_CONEANGULARITY:
					RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(8, val);
					break;
			}
		}
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0004", ex);
	}
}

void CALLBACK AutoDxfAlignCall()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "AutoDxfAlignCall", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(12);
}

void CALLBACK DoFocusFrameGrab()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "DoFocusFrameGrab", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->StartFocusInFrameGrab();
}

void CALLBACK DoFocusRightClick()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "DoFocusRightClick", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->StartFocusForRightClick();
}

void CALLBACK DoFocusProfileScan()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "DoFocusProfileScan", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->StartFocusInProfileScan();
}

void CALLBACK UpdateReferenceSphere()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "UpdateReferenceSphere", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(19);
}

void CALLBACK SaveCameraImg(std::string fname)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "SaveCameraImg", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->SaveVideoWindowImage(gcnew System::String((char*)fname.c_str()), false, false);
}

void CALLBACK ShowWaitCursor(int i)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "ShowWaitCursor", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->ShowHideWaitCursor(i);
}

void CALLBACK profileScanEndPtsCallBack(int i)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "profileScanEndPtsCallBack", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->GotProfileScanEndPts(i);
}

void CALLBACK deletefileFromPtsLog()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "deletefileFromPtsLog", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->deletePointsLogFile();
}

void CALLBACK SetRightclickOnShape(Shape *hsp)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "SetRightclickOnShape", __FUNCSIG__);
	RWrapper::RW_MainInterface::MYINSTANCE()->RightClickOnShapeInMeasureMode(hsp);
}

void CALLBACK CameraSettingsChanged(std::string UpdatedCamSetting)
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "CameraSettingsChanged", __FUNCSIG__);
	System::String^ UcamSettings = gcnew System::String(UpdatedCamSetting.c_str());
	RWrapper::RW_MainInterface::MYINSTANCE()->RaiseCamSettingsUpdated(UcamSettings);
}

void CALLBACK FixtureRefPointTaken()
{
	//MAINDllOBJECT->SetAndRaiseErrorMessage("DEBUG", "FixtureRefPointTaken", __FUNCSIG__);
	try
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(28);
	}
	catch (Exception^ ex) {}
}


RWrapper::RW_MainInterface::RW_MainInterface()
{
	try
	{
		//MessageBox::Show("Starting Main INterface");

		this->MainInInstance = this;
		//this->AllowPermissions(System::Windows::Forms::Application::StartupPath);
		this->MachineConnectedStatus = false;
		this->IsMultiLevelZoom = false;
		this->AllowStepImport = false;
		this->MachineCardFlag = 0;
		this->MachineCNCStatus = false;
		this->UseLookAhead = false;
		this->DealyinSec = 9;
		this->Delay_AfterTargetReached = 50;
		this->LastSelectedMagLevel = "";
		AppDataFolderPath = Environment::GetFolderPath(Environment::SpecialFolder::CommonApplicationData);
		//AppDataFolderPath = System::Windows::Forms::Application::StartupPath;
		AppDataFolderPath = AppDataFolderPath + "\\Rapid-I 5.0";
		if (System::Environment::Is64BitOperatingSystem)
			this->DBConnectionString = "Provider=Microsoft.Ace.OLEDB.12.0; OLE DB Services = -1; Data Source=" + AppDataFolderPath + "\\Database\\RapidSettings.mdb;Persist Security Info=False;Jet OLEDB:Database Password=cTpL_4577ri";
		else
			this->DBConnectionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + AppDataFolderPath + "\\Database\\RapidSettings.mdb;Persist Security Info=False;Jet OLEDB:Database Password=cTpL_4577ri";

		//this->DBConnectionString = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=" + AppDataFolderPath + "\\Database\\RapidSettings.accdb"; //Persist Security Info=False;Jet OLEDB:Database Password=cTpL_4577ri";
		LogFolderPath = AppDataFolderPath + "\\Log";
		this->ErrorHandlerObject = gcnew ErrHandlerApp::ErrHandlerApp(LogFolderPath);
		this->ErrorHandlerObject->createFolder();
		System::IO::DirectoryInfo^ info = gcnew System::IO::DirectoryInfo(LogFolderPath);			
		info->Attributes = System::IO::FileAttributes::Hidden;
		//info->Attributes = System::IO::FileAttributes::ReadOnly;
		this->SentCommandLogCollection = gcnew System::Collections::Generic::List<CommandLogger^>();
		System::Threading::TimerCallback^ tcb = gcnew System::Threading::TimerCallback(RW_MainInterface::MYINSTANCE(), &RW_MainInterface::CommandLogWriteTick);
		//Instantiate the COmmands Log writing timer and start it...
		System::Object^ stt;
		this->CommandsLogWriter = gcnew System::Threading::Timer(tcb, stt, 0, 60000);

		this->ModuleInitialisationLog_Path = System::String::Concat(LogFolderPath, "\\ModuleInit", "\\InitializationLog ", Microsoft::VisualBasic::DateAndTime::Now.ToString("dd-MMM-yyyy"), ".txt");
		if (System::Environment::Is64BitOperatingSystem)
			image_pixels = new BYTE[3664 * 2748 * 4];
		else
			image_pixels = new BYTE[1600 * 1200 * 4];

		WriteModuleInitilization("Time and Date:  " + Microsoft::VisualBasic::DateAndTime::Now.ToString("dd-MMM-yyyy h:mm tt"));
		WriteModuleInitilization("");
		WriteModuleInitilization("Wrapper Initialisation Started");
		
		WriteModuleInitilization("Main FrameWork Initialization");

		RCadMousePos = gcnew cli::array<Double>(2);

		RCadApp::CreateMainDllInstance((char*)Marshal::StringToHGlobalAnsi(ModuleInitialisationLog_Path).ToPointer());	
		MAINDllOBJECT->InitialiseOtherModules1();
		MAINDllOBJECT->AddMCSduringLoad();
		MAINDllOBJECT->InitialiseOtherModules2();
		MAINDllOBJECT->deleteAlltextfilefromPointsLog = &deletefileFromPtsLog;
		MAINDllOBJECT->CurrentStatus = &CurrentStatusDisplay;
		MAINDllOBJECT->CurrentMsgBoxStatus = &CurrentMsgBoxStatusDisplay;
		MAINDllOBJECT->FramGrabUpdate = &FramGrabChanged;
		MAINDllOBJECT->MainDllErrorHappened = &FrameWorkError;
		MAINDllOBJECT->ShowAutoFocusWindow = &ShowWindowForAutoFocusCall;
		MAINDllOBJECT->ShowAutoContourWindow = &ShowWindowForAutoContourCall;
		MAINDllOBJECT->ShowRotationScanWindow = &ShowRotationScanWindowCallback;
		CollectionListener *ActionListner = new RapidActionListener();
		MAINDllOBJECT->getShapesList().addListener(new RapidShapeListener());
		MAINDllOBJECT->getUCSList().addListener(new RapidUcsListener());
		MAINDllOBJECT->getDimList().addListener(new RapidDimListener());
		MAINDllOBJECT->getDXFList().addListener(new RapidDxfListener());
		MAINDllOBJECT->getActionsHistoryList().addListener(ActionListner);
		PPCALCOBJECT->getPPActionlist().addListener(ActionListner);

		//gcnew RWrapper::TIS_Control();
		gcnew RWrapper::RW_DBSettings();		
		gcnew RWrapper::RW_FocusDepth();
		gcnew RWrapper::RW_CNC();	
		gcnew RWrapper::RW_PartProgram();
		gcnew RWrapper::RW_PCD();
		gcnew RWrapper::RW_FixedGraph();
		gcnew RWrapper::RW_Thread();
		gcnew RWrapper::RW_ShapeParameter();
		gcnew RWrapper::RW_MeasureParameter();
		gcnew RWrapper::RW_UCSParameter();
		gcnew RWrapper::RW_DrawGraphicsText();
		//gcnew RWrapper::RW_FixtureCalibration();
		gcnew RWrapper::RW_VBlockCallibration();
		gcnew RWrapper::RW_SphereCalibration();
		gcnew RWrapper::RW_ActionParameter();
		gcnew RWrapper::RW_ImageGridProgram();
		gcnew RWrapper::RW_CircularInterPolation();
		gcnew RWrapper::RW_LinearInterPolation();
		gcnew RWrapper::RW_DelphiCamModule();
		//gcnew RWrapper::TIS_Control();
		gcnew RWrapper::RW_AutoCalibration();
		//RWrapper::OglLighting^ TestInst = gcnew RWrapper::OglLighting();
		//TestInst->Show();
		gcnew RWrapper::RW_SuperImposeImage();

		this->ProbeCount = 0;
		ProbeScanDetais_ArrayList = gcnew System::Collections::ArrayList;
		this->_colorDialog = gcnew System::Windows::Forms::ColorDialog();
		RWrapper::RW_MainInterface::EntityIDList = gcnew  System::Collections::Generic::List<System::Int32>; 
		RWrapper::RW_MainInterface::EntityNameList = gcnew  System::Collections::Generic::List<System::String^>; 

		MAINDllOBJECT->DatabaseFolderPath = (char*)Marshal::StringToHGlobalAnsi(AppDataFolderPath).ToPointer();
		MAINDllOBJECT->DerivedShapeCallback = &ShowrequiredPanel;
		MAINDllOBJECT->DerivedShapeCallbackWithPara = &ShowrequiredPanelWithPara;
		MAINDllOBJECT->ContextMenuForSfg = &SurfaceFramegrabMclick;
		MAINDllOBJECT->SEDUserChoice = &SurfaceEDUserChoice;
		MAINDllOBJECT->ClearAllRadioButton = &ClearAllRadiaButtonChecked;
		MAINDllOBJECT->ClearAllRadioButtonOtherThrd = &ClearAllRadiaButtonCheckedBackground;
		MAINDllOBJECT->CheckSmartMeasurementButton = &CheckCurrentSmartMeasurement;
		MAINDllOBJECT->UpdateShapeParamForMouseMove = &UpdateShapeParamMouseMove;
		MAINDllOBJECT->MoveMachineToRequiredPos = &MachineGoToCommand;
		MAINDllOBJECT->SaveCameraImage = &SaveCameraImg;
		MAINDllOBJECT->LineArcCallback = &ShowLineArcPanel;
		MAINDllOBJECT->ShowHideWaitCursor = &ShowWaitCursor;
		MAINDllOBJECT->PointsTakenProfileScan = &profileScanEndPtsCallBack;
		MAINDllOBJECT->AutoDxfAlign = &AutoDxfAlignCall;
		MAINDllOBJECT->DoFocusFG = &DoFocusFrameGrab;
		MAINDllOBJECT->DoRightClickFocus = &DoFocusRightClick;
		MAINDllOBJECT->SetRightClickOnShape = &SetRightclickOnShape;
		MAINDllOBJECT->CallDxfImport = &DxfImportCallBack;
		MAINDllOBJECT->CreateNextProfileScan = &NextProfileScanCallBack;
		MAINDllOBJECT->DeviationCallBack = &CallDeviationWindow;
		MAINDllOBJECT->CloudPointsMeasureCallBack = &CallCloudPointsMeasureWindow;
		MAINDllOBJECT->DoFocusProfile = &DoFocusProfileScan;
		MAINDllOBJECT->LineArcUcsCreated = &LineArcUcsCreatedCallback;
		MAINDllOBJECT->UCSParamChanged = &HandleUCSParamChanged;
		MAINDllOBJECT->TruePositionOffset = &TruepositionCallBack;
		MAINDllOBJECT->CamSettingsChanged = &CameraSettingsChanged;
		MAINDllOBJECT->FixtureRefPoint_Taken = &FixtureRefPointTaken;

		MAINDllOBJECT->AddingUCSModeFlag = true;
		this->StopWaitMessageTimer = gcnew System::Windows::Forms::Timer();
		this->StopWaitMessageTimer->Enabled = false;
		this->StopWaitMessageTimer->Interval = 1000;
		this->StopWaitMessageTimer->Tick += gcnew EventHandler(this, &RWrapper::RW_MainInterface::HideWaitMessagePanel);
		this->StartWaitMessageTimer = gcnew System::Windows::Forms::Timer();
		this->StartWaitMessageTimer->Enabled = false;
		this->StartWaitMessageTimer->Interval = 1000;
		this->StartWaitMessageTimer->Tick += gcnew EventHandler(this, &RWrapper::RW_MainInterface::ShowWaitMessagePanel);
		this->TwoXLensFitted = false;
		CCCCCt = 2;

		LastSelectedToolbar = ""; LastSelectedButton = "";
		WriteModuleInitilization("Wrapper Initialisation Finished");
		WriteModuleInitilization("");
		WriteModuleInitilization("");
		this->window1_State = false; 
		this->window2_State = false;
		//Check if we have NVidia graphics card. If we do, then we shift the cross hair line by one pixel because That driver has a problem
		//LoadReferecneImage_OneShot("D:\\TestFile1.jpg");
		//RWrapper::RW_DrawGraphicsText::MYINSTANCE()->Draw_NormalString(40, "Tahoma", "Test Rapid-I", 0, 0, 0, 0);

	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0005", ex);
	}
}

RWrapper::RW_MainInterface::~RW_MainInterface()
{
	try
	{
		RCadApp::deleteInstance();
		System::GC::Collect();
		exit(0);
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0006", ex);
	}
}

void RWrapper::RW_MainInterface::CloseAll()
{
	try
	{
		 DROInstance->ColseSoftware();
		 MAINDllOBJECT->CloseSoftware();
		 Sleep(10);
		 DigitalReadOut::DeleteInstance();
		 System::IO::FileInfo^ ActionSaveFileInfo = gcnew System::IO::FileInfo(System::String::Concat(RWrapper::RW_MainInterface::MYINSTANCE()->LogFolderPath,"\\ActionSave.rppx"));
		 if(ActionSaveFileInfo->Exists)
			ActionSaveFileInfo->Delete();
		 System::IO::FileInfo^ FisrtImageInfo = gcnew System::IO::FileInfo(System::String::Concat(RWrapper::RW_MainInterface::MYINSTANCE()->LogFolderPath,"\\FirstPointActionImage.jpg"));
		 if(FisrtImageInfo->Exists)
			FisrtImageInfo->Delete();
		  RWrapper::RW_MainInterface::MYINSTANCE()->deletePointsLogFile();
		  RW_RepeatAutoMeasure::Close_AutoMeasureWindow();
		  RW_RepeatShapeMeasure::Close_RepeatShapeMeasureWindow();
		 delete MainInInstance;
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0007", ex);
	}
}

RWrapper::RW_MainInterface^RWrapper::RW_MainInterface::MYINSTANCE()
{
	return MainInInstance;
}

void RWrapper::RW_MainInterface::AllowPermissions(System::String^ FolderPath)
 {
	try
	{
		System::IO::DirectoryInfo^ info = gcnew System::IO::DirectoryInfo("C:\\Program Files\\Rapid-I 5.0");
		System::Security::AccessControl::DirectorySecurity^ ds = info->GetAccessControl();
		ds->AddAccessRule(gcnew System::Security::AccessControl::FileSystemAccessRule("Users", System::Security::AccessControl::FileSystemRights::FullControl, System::Security::AccessControl::InheritanceFlags::ContainerInherit, System::Security::AccessControl::PropagationFlags::None, System::Security::AccessControl::AccessControlType::Allow));
		ds->AddAccessRule(gcnew System::Security::AccessControl::FileSystemAccessRule("Users", System::Security::AccessControl::FileSystemRights::FullControl, System::Security::AccessControl::InheritanceFlags::ObjectInherit, System::Security::AccessControl::PropagationFlags::None, System::Security::AccessControl::AccessControlType::Allow));
		info->SetAccessControl(ds);
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0003b", ex);
	}

 }

void RWrapper::RW_MainInterface::RightClickOnShapeInMeasureMode(Shape *hsp)
{
	try
	{
		//if(hsp->IsUsedForReferenceDot()) return;
		if(hsp->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
			RightClickedOnEntity::raise(2, "RightClickOnCircle");
		else if(hsp->ShapeType == RapidEnums::SHAPETYPE::PERIMETER)
			RightClickedOnEntity::raise(2, "RightClickOnPerimeter");
		else if(hsp->ShapeType == RapidEnums::SHAPETYPE::ARC)
			RightClickedOnEntity::raise(2, "RightClickOnArc");
		else if(hsp->ShapeType == RapidEnums::SHAPETYPE::LINE)
			RightClickedOnEntity::raise(2, "RightClickOnLine");
		else if(hsp->ShapeType == RapidEnums::SHAPETYPE::LINE3D)
			RightClickedOnEntity::raise(2, "RightClickOnLine3D");
		else if(hsp->ShapeType == RapidEnums::SHAPETYPE::XLINE3D)
			RightClickedOnEntity::raise(2, "RightClickOnLine3D");
		else if(hsp->ShapeType == RapidEnums::SHAPETYPE::PLANE)
			RightClickedOnEntity::raise(2, "RightClickOnPlane");
		else if(hsp->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
			RightClickedOnEntity::raise(2, "RightClickOnCylinder");
		else if(hsp->ShapeType == RapidEnums::SHAPETYPE::SPHERE)
			RightClickedOnEntity::raise(2, "RightClickOnSphere");	
		else if(hsp->ShapeType == RapidEnums::SHAPETYPE::CONE)
			RightClickedOnEntity::raise(2, "RightClickOnCone");	
		else if(hsp->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D)
			RightClickedOnEntity::raise(2, "RightClickOnCircle3D");
		else if(hsp->ShapeType == RapidEnums::SHAPETYPE::ARC3D)
			RightClickedOnEntity::raise(2, "RightClickOnArc3D");
		else if(hsp->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
			RightClickedOnEntity::raise(2, "RightClickOnCloudPoints");
	}
	catch(Exception^ ex)
	{
		 RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0003c", ex);
	}
}

bool RWrapper::RW_MainInterface::IntializeOpengl(System::IntPtr^ handle, int width, int height, int Windowno, int CamWidth, int CamHeight)
 {
	 bool InitialisedCorrectly = false;
	try
	{
		if(Windowno == 0)
		{
			if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::PROBE_ONLY ||
				MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HEIGHT_GAUGE) // || 
				//MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HOBCHECKER ) 
				return false;
			System::Windows::Forms::Form^ HiddenPictureBox_Video = gcnew System::Windows::Forms::Form;
			HiddenPictureBox_Video->Width = width;
			HiddenPictureBox_Video->Height = height;
			//HiddenPictureBox_Video->TopMost = true;
			HiddenPictureBox_Video->AutoScale = false;
			HiddenPictureBox_Video->AutoSize = false;
			HiddenPictureBox_Video->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			//HiddenPictureBox_Video->Show();
			//MAINDllOBJECT->Initialise_VideoWindow((HWND)handle->ToPointer(), width, height);
			this->VideoWindowHandle = handle;
			InitialisedCorrectly = MAINDllOBJECT->Initialise_VideoWindow((HWND)handle->ToPointer(), (HWND)HiddenPictureBox_Video->Handle.ToPointer(), width, height, CamWidth, CamHeight); //
			RWrapper::RW_DBSettings::MYINSTANCE()->UpdateSurfaceFGSettings();
			this->DigitalZoomLevel = HELPEROBJECT->CamSizeRatio;
			this->InitialDigitalZoomLevel = this->DigitalZoomLevel;
			RW_DBSettings::MYINSTANCE()->UpdateVideoPixelWidth("D" + this->DigitalZoomLevel.ToString());
			//System::Windows::Forms::Form^ HiddenPictureBox = gcnew System::Windows::Forms::Form;
			//HiddenPictureBox->Width = width * 2;
			//HiddenPictureBox->Height = height * 2;
			////HiddenPictureBox->TopMost = true;
			//HiddenPictureBox->AutoScale = false;
			//HiddenPictureBox->AutoSize = false;
			//HiddenPictureBox->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			////HiddenPictureBox->Show();
			//MAINDllOBJECT->SetHiddenWindowHandle((HWND)HiddenPictureBox->Handle.ToPointer());
			//MAINDllOBJECT->Draw_VideoWindowGraphics();

			CheckRunningGraphicsCard();
			MAINDllOBJECT->StartVideoThread();
		}
		else if(Windowno == 1)
		{
			MAINDllOBJECT->Initialise_RcadWindow((HWND)handle->ToPointer(), width, height);
			MAINDllOBJECT->Draw_RcadWindowGraphics();
			MAINDllOBJECT->StartRcadThread();
			InitialisedCorrectly = true;
		}
		else if(Windowno == 2)
		{
			MAINDllOBJECT->Initialise_DxfWindow((HWND)handle->ToPointer(), width, height);
			MAINDllOBJECT->Draw_DxfWindowGraphics();
			MAINDllOBJECT->StartDxfThread();
			InitialisedCorrectly = true;
		}
		else if(Windowno == 3)
		{
			OVERLAPIMAGEWINOBJECT->InitialiseOverlapImgOGlWindow((HWND)handle->ToPointer(), width, height);
			InitialisedCorrectly = true;
		}
		else if(Windowno == 4)
		{
			THREADCALCOBJECT->InitialiseThreadOGlWindow((HWND)handle->ToPointer(), width, height);
			InitialisedCorrectly = true;
		}
		return InitialisedCorrectly;
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0008", ex);
		return false;
	}
}

void RWrapper::RW_MainInterface::DXFExport(bool CloudPointAsPolyLine)
{
	try
	{
		System::Windows::Forms::SaveFileDialog^ saveFileDialog1 = gcnew System::Windows::Forms::SaveFileDialog();
		saveFileDialog1->Filter = "dxf files (*.dxf)|*.dxf|IGES files (*.igs)|*.igs|STL files (*.stl)|*.stl";	//filters the visible files..
		saveFileDialog1->FilterIndex = 1 ;
		if(saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			System::String^ FilePath = saveFileDialog1->FileName;
			if(FilePath->Contains(".dxf"))
				DXFEXPOSEOBJECT->ExportDXFFile((char*)Marshal::StringToHGlobalAnsi(FilePath).ToPointer(), CloudPointAsPolyLine);
			else if(FilePath->Contains(".igs"))
				DXFEXPOSEOBJECT->ExportIGESFile((char*)Marshal::StringToHGlobalAnsi(FilePath).ToPointer());
			else if(FilePath->Contains(".stl"))
				DXFEXPOSEOBJECT->ExportSTLFile((char*)Marshal::StringToHGlobalAnsi(FilePath).ToPointer());
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0032", ex);
	}
}

void RWrapper::RW_MainInterface::DXFImport(bool KeepitFixed)
{
	try
	{
		System::Windows::Forms::OpenFileDialog^ openFileDialog1 = gcnew System::Windows::Forms::OpenFileDialog();
		if(this->AllowStepImport)
			openFileDialog1->Filter = "dxf files (*.dxf)|*.dxf|Drill files (*.drl)|*.drl|IGES files (*.igs)|*.igs|STL files (*.stl)|*.stl|STEP files (*.stp)|*.stp";//filters the visible files..
		else
			openFileDialog1->Filter = "dxf files (*.dxf)|*.dxf|Drill files (*.drl)|*.drl|IGES files (*.igs)|*.igs|STL files (*.stl)|*.stl";
		openFileDialog1->FilterIndex = 1;
		if(openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			System::String^ FilePath = openFileDialog1->FileName;
			int Lastindex = FilePath->LastIndexOf('.'), Startindex = FilePath->LastIndexOf('\\');
			System::String^ dfxFileName = FilePath->Substring(Startindex + 1, Lastindex - Startindex - 1);
			if (FilePath->Contains(".dxf"))
			{
				DXFEXPOSEOBJECT->ImportDXFFile((char*)Marshal::StringToHGlobalAnsi(FilePath).ToPointer(), (char*)Marshal::StringToHGlobalAnsi(dfxFileName).ToPointer(), dfxFileName->Length);
				if (KeepitFixed)
				{
					//if we are importing this dxf for a digital graticule
					//RCollectionBase cDXF = MAINDllOBJECT->getCurrentUCS().getDxfCollecion();
					bool currentClearState = RWrapper::RW_FixedGraph::MYINSTANCE()->DontClear;
					RWrapper::RW_FixedGraph::MYINSTANCE()->DontClear = true;
					for each(Shape* item in DXFEXPOSEOBJECT->CurrentDXFShapeList)
					{
						ShapeWithList *TmpShape = (ShapeWithList*)item;
						if (TmpShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
						{
							//We got a line. So lets add a fixed Line.
							RWrapper::RW_FixedGraph::MYINSTANCE()->AddFixedShape(0);
							//double LineParams[6] = { 0 };
							Line* LL = (Line*)TmpShape;
							//LL->(&LineParams[0]);
							if ((LL->getPoint1()->getX() == 0 && LL->getPoint1()->getY() == 0) || (LL->getPoint2()->getX() == 0 && LL->getPoint2()->getY() == 0))
								RWrapper::RW_FixedGraph::MYINSTANCE()->LineParameterChanged(false, true, false, false, LL->Angle() * 180 / M_PI, LL->Intercept());
							else if (LL->Intercept() == 0)
								RWrapper::RW_FixedGraph::MYINSTANCE()->LineParameterChanged(true, false, false, false, LL->Angle() * 180 / M_PI, LL->Intercept());
							else if (LL->Angle() == M_PI_2)
								RWrapper::RW_FixedGraph::MYINSTANCE()->LineParameterChanged(false, false, true, false, LL->Angle() * 180 / M_PI, LL->Intercept());
							else
								RWrapper::RW_FixedGraph::MYINSTANCE()->LineParameterChanged(false, false, false, true, LL->Angle() * 180 / M_PI, LL->Intercept());
						}
						else if (TmpShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
						{
							RWrapper::RW_FixedGraph::MYINSTANCE()->AddFixedShape(1);
							double CircleParams[8] = { 0 };
							Circle* CC = (Circle*)TmpShape;
							CC->getParameters(&CircleParams[0]);
							RWrapper::RW_FixedGraph::MYINSTANCE()->CircleParameterChanged(CircleParams[0], CircleParams[1], CircleParams[6], true);
						}
						else
							continue;
						//Make the current added shape "permanent" with Clear All

					}
					//We have loaded all fixed graphics stuff. So now let us clear the dxfcollection!
					//MAINDllOBJECT->getCurrentUCS().getDxfCollecion().clearAll();
					RWrapper::RW_FixedGraph::MYINSTANCE()->DontClear = currentClearState;
					DXFEXPOSEOBJECT->CurrentDXFShapeList.clear();
					DXFEXPOSEOBJECT->CurrentDXFMeasureList.clear();
					MAINDllOBJECT->dontUpdateGraphcis = false;
					MAINDllOBJECT->update_DxfGraphics();
				}
			}
			else if(FilePath->Contains(".drl"))
				DXFEXPOSEOBJECT->ImportDRLFile((char*)Marshal::StringToHGlobalAnsi(FilePath).ToPointer(), (char*)Marshal::StringToHGlobalAnsi(dfxFileName).ToPointer(), dfxFileName->Length);
			else if(FilePath->Contains(".igs"))
				DXFEXPOSEOBJECT->ImportIGESFile((char*)Marshal::StringToHGlobalAnsi(FilePath).ToPointer(), (char*)Marshal::StringToHGlobalAnsi(dfxFileName).ToPointer(), dfxFileName->Length);
			else if(FilePath->Contains(".stl"))
				DXFEXPOSEOBJECT->ImportSTLFile((char*)Marshal::StringToHGlobalAnsi(FilePath).ToPointer(), (char*)Marshal::StringToHGlobalAnsi(dfxFileName).ToPointer(), dfxFileName->Length);
			else if(FilePath->Contains(".stp"))
			{
				System::String^ ProgFolder = Environment::GetFolderPath(Environment::SpecialFolder::ProgramFiles) + "\\Rapid-I 5.0";
				System::String^ ProgDataFolder = Environment::GetFolderPath(Environment::SpecialFolder::CommonApplicationData) + "\\Rapid-I 5.0";
				System::String^ OutputFile = ProgDataFolder + "\\" + "out.txt ";
				System::String^ ErrorFile =  ProgDataFolder + "\\" + "error.txt ";
				System::String^ JOutputFile = "\"" + ProgDataFolder + "\"" + "\\" + "out.txt ";
				System::String^ JErrorFile =  "\"" + ProgDataFolder + "\"" + "\\" + "error.txt ";
				System::String^ ClassFile = "-classpath" + " " + "\"" + ProgFolder + "\""  + " " +"STP.Main";
				System::String^ JRE_REGISTRY_KEY = "HKEY_LOCAL_MACHINE\\SOFTWARE\\JavaSoft\\Java Runtime Environment";
				System::String^ jreVersion  = (System::String^)Registry::GetValue(JRE_REGISTRY_KEY, "CurrentVersion",nullptr);
				if(jreVersion !=  nullptr)
				{
					System::String^ keyName = System::IO::Path::Combine(JRE_REGISTRY_KEY, jreVersion);
					System::String^ jvmDir  =  (System::String^)Registry::GetValue(keyName, "RuntimeLib", nullptr);
					System::String^ dirpath = jvmDir->Substring(0, jvmDir->Length - 15);
					ProcessStartInfo ^pInfo;
					Process ^javaP;
					pInfo = gcnew ProcessStartInfo();
					pInfo->CreateNoWindow = true;
					pInfo->FileName = "java.exe";
					System::String^ Str = ClassFile + " " + FilePath + " " + JOutputFile + " " + JErrorFile ;
					pInfo->Arguments = Str;
					pInfo->UseShellExecute = false;
					pInfo->WorkingDirectory =dirpath;
					javaP = Process::Start(pInfo);
					javaP->Process::WaitForExit();
					javaP->Process::Close();
					System::IO::StreamReader^ tst = gcnew System::IO::StreamReader(ErrorFile);
					System::String^ ErrCheck = tst->ReadLine();
					if(ErrCheck == "SUCCESS")
						DXFEXPOSEOBJECT->ImportSTEPFile((char*)Marshal::StringToHGlobalAnsi(OutputFile).ToPointer(), (char*)Marshal::StringToHGlobalAnsi(dfxFileName).ToPointer(), dfxFileName->Length);	
					System::IO::File::Delete(OutputFile);
				}
				else
					MAINDllOBJECT->ShowMsgBoxString("RW_MainInteface_JRENotFound", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);		
			}
			else if (FilePath->Contains(".nc"))
				DXFEXPOSEOBJECT->ImportNCFile((char*)Marshal::StringToHGlobalAnsi(FilePath).ToPointer(), (char*)Marshal::StringToHGlobalAnsi(dfxFileName).ToPointer(), dfxFileName->Length);
		}
		if(HELPEROBJECT->MultiFeatureScan)
			DXFEXPOSEOBJECT->editDXF();
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0033", ex);
	}

}

void RWrapper::RW_MainInterface::ExportPointToCAD()
{
	try
	{
		System::Windows::Forms::SaveFileDialog^ saveFileDialog1 = gcnew System::Windows::Forms::SaveFileDialog();
		saveFileDialog1->Filter = "dxf files (*.dxf)|*.dxf|All files (*.*)|*.*";
		saveFileDialog1->FilterIndex = 1;
		if(saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			DXFEXPOSEOBJECT->ExportShapePointsToDXF((char*)Marshal::StringToHGlobalAnsi(saveFileDialog1->FileName).ToPointer(), true);
	}
	catch(Exception^ ex){ WriteErrorLog("WRMAIN0032", ex); }
}

void RWrapper::RW_MainInterface::ExportPointToText()
{
	try
	{
		System::Windows::Forms::SaveFileDialog^ saveFileDialog1 = gcnew System::Windows::Forms::SaveFileDialog();
		saveFileDialog1->Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*";
		saveFileDialog1->FilterIndex = 1;
		if(saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			DXFEXPOSEOBJECT->ExportShapePointsToText((char*)Marshal::StringToHGlobalAnsi(saveFileDialog1->FileName).ToPointer(), true);
	}
	catch(Exception^ ex){ WriteErrorLog("WRMAIN0034", ex); }
}

void RWrapper::RW_MainInterface::ExportPointToCsv()
{
	try
	{
		System::Windows::Forms::SaveFileDialog^ saveFileDialog1 = gcnew System::Windows::Forms::SaveFileDialog();
		saveFileDialog1->Filter = "csv files (*.csv)|*.csv|All files (*.*)|*.*";	//filters the visible files..
		saveFileDialog1->FilterIndex = 1 ;
		if(saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			DXFEXPOSEOBJECT->ExportShapePointsToCsv((char*)Marshal::StringToHGlobalAnsi(saveFileDialog1->FileName).ToPointer(), true);
	}
	catch(Exception^ ex){ WriteErrorLog("WRMAIN0034", ex); }
}

void RWrapper::RW_MainInterface::ExportParamToCsv()
{
	try
	{
		System::Windows::Forms::SaveFileDialog^ saveFileDialog1 = gcnew System::Windows::Forms::SaveFileDialog();
		saveFileDialog1->Filter = "csv files (*.csv)|*.csv|All files (*.*)|*.*";	//filters the visible files..
		saveFileDialog1->FilterIndex = 1 ;
		if(saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			DXFEXPOSEOBJECT->ExportShapeParametersToCsv((char*)Marshal::StringToHGlobalAnsi(saveFileDialog1->FileName).ToPointer(), false);
	}
	catch(Exception^ ex){ WriteErrorLog("WRMAIN0034", ex); }
}

void RWrapper::RW_MainInterface::ReadPointFromText()
{
	try
	{
		System::Windows::Forms::OpenFileDialog^ OpnFileDialog = gcnew System::Windows::Forms::OpenFileDialog();
		OpnFileDialog->Filter = "Text files (*.txt)|*.txt|All files (*.*)|*.*" ;
		OpnFileDialog->FilterIndex = 1 ;
		if(OpnFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			System::IO::StreamReader^ tst = gcnew System::IO::StreamReader(OpnFileDialog->FileName);
			double x = 0, y = 0, z = 0;
			System::Collections::Generic::List<Double>^ PointValues = gcnew System::Collections::Generic::List<Double>();
			while(!tst->EndOfStream)
			{
				System::String^ Str = tst->ReadLine();
				/*x = System::Convert::ToDouble(Str);
				Str = tst->ReadLine();
				y = System::Convert::ToDouble(Str);
				Str = tst->ReadLine();
				z = System::Convert::ToDouble(Str);*/
				cli::array<System::String^>^ StrSub = Str->Split(',');
				//x = System::Convert::ToDouble(StrSub[0]);
				//y = System::Convert::ToDouble(StrSub[1]);
				//z = System::Convert::ToDouble(StrSub[2]);
				PointValues->Add(System::Convert::ToDouble(StrSub[0]));
				PointValues->Add(System::Convert::ToDouble(StrSub[1]));
				//MAINDllOBJECT->AddTextPointToSelectedShape(x, y, z);
			}
			double* Points = (double*)malloc(PointValues->Count * sizeof(double));
			for (int i = 0; i < PointValues->Count; i++)
				Points[i] = PointValues[i];
			double ans = FOCUSCALCOBJECT->CalculateFocus_DefinedPts(Points, PointValues->Count / 2);
			MessageBox::Show(ans.ToString());
			MAINDllOBJECT->UpdateAfterAddingPoints();
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0029aa", ex);
	}
}

void RWrapper::RW_MainInterface::ReadFocusValuesFromText()
{
	try
	{
		System::Windows::Forms::OpenFileDialog^ OpnFileDialog = gcnew System::Windows::Forms::OpenFileDialog();
		OpnFileDialog->Filter = "Text files (*.txt)|*.txt|All files (*.*)|*.*" ;
		OpnFileDialog->FilterIndex = 1 ;
		if(OpnFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			std::list<double> ZfocusValues;
			System::IO::StreamReader^ tst = gcnew System::IO::StreamReader(OpnFileDialog->FileName);
			double x = 0;
			while(!tst->EndOfStream)
			{
				System::String^ Str = tst->ReadLine();
				x = System::Convert::ToDouble(Str);
				ZfocusValues.push_back(x);
			}
			MAINDllOBJECT->AddNewPointWithFocus(&ZfocusValues);
		}
	}
	catch(Exception^ ex)
	{
	}
}

void RWrapper::RW_MainInterface::SaveImageForOneShot(System::String^ FileName)
{
	try
	{
		if (!MAINDllOBJECT->CameraConnected())return;
		double VWidth = MAINDllOBJECT->getWindow(0).getWinDim().x * HELPEROBJECT->CamSizeRatio;
		double VHeight = MAINDllOBJECT->getWindow(0).getWinDim().y * HELPEROBJECT->CamSizeRatio;

		if (FileName->EndsWith("bmp") || FileName->EndsWith("jpg"))
		{
			MAINDllOBJECT->SaveIamge_OneShot(this->image_pixels, 3);
			cli::array<BYTE> ^MemPtr = gcnew cli::array<BYTE>(VWidth * VHeight * 3);
			//Cast from LPVOID to char *
			char *lkr = (char *)(LockResource(this->image_pixels)); // &RWrapper::pixels[0]));
			//Copy from unmanaged memory to managed array
			Marshal::Copy((IntPtr)lkr,MemPtr, 0, VWidth * VHeight * 3);
			System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(VWidth, VHeight);
			System::Drawing::Imaging::BitmapData^ imagedata = bmp->LockBits(Drawing::Rectangle(0, 0, VWidth, VHeight), Imaging::ImageLockMode::ReadWrite, Imaging::PixelFormat::Format24bppRgb);
			Marshal::Copy(MemPtr, 0, imagedata->Scan0, VWidth * VHeight * 3);
			bmp->UnlockBits(imagedata);
			if (FileName->EndsWith("bmp"))
				bmp->Save(FileName, System::Drawing::Imaging::ImageFormat::Bmp);
			else
				bmp->Save(FileName, System::Drawing::Imaging::ImageFormat::Jpeg);
		}
		else if(FileName->EndsWith("bin"))
		{
			MAINDllOBJECT->SaveIamge_OneShot(this->image_pixels, 1);  //(&RWrapper::pixels[0], 1);
			System::IO::FileStream^ osFile = gcnew System::IO::FileStream(FileName, System::IO::FileMode::Create);
			//System::IO::BinaryWriter^ oneS_RefFile = gcnew System::IO::BinaryWriter(osFile);
			System::IO::StreamWriter^ oneS_RefFile = gcnew System::IO::StreamWriter(osFile);
			int length = VWidth * VHeight;
			//cli::array<BYTE> ^Input = gcnew cli::array<BYTE>(VWidth * VHeight);
			//System::Runtime::InteropServices::Marshal::Copy(IntPtr((void *)RWrapper::pixels[0]), Input, 0, Input->Length);
			cli::array<BYTE> ^MemPtr = gcnew cli::array<BYTE>(VWidth * VHeight * 3);
			System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(VWidth, VHeight);
			System::Drawing::Imaging::BitmapData^ imagedata = bmp->LockBits(Drawing::Rectangle(0, 0, VWidth, VHeight), Imaging::ImageLockMode::ReadWrite, Imaging::PixelFormat::Format24bppRgb);

			for (int i = 0; i < length; i++)
			{
				oneS_RefFile->Write(this->image_pixels); // RWrapper::pixels[i]);
				MemPtr[3 * i] = this->image_pixels[i];
				MemPtr[3 * i + 1] = this->image_pixels[i];
				MemPtr[3 * i + 2] = this->image_pixels[i];
			}
			Marshal::Copy(MemPtr, 0, imagedata->Scan0, VWidth * VHeight * 3);
			bmp->UnlockBits(imagedata);
			bmp->Save(FileName + ".jpg", System::Drawing::Imaging::ImageFormat::Jpeg);
			//	oneS_RefFile->Write(RWrapper::pixels[i] && ",");
			osFile->Close();
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0029a", ex);
	}
}

void RWrapper::RW_MainInterface::SaveAllImageswithoutgraphics(int Cnt)
{
	try
	{	
		if(!MAINDllOBJECT->CameraConnected())return;
		System::Windows::Forms::SaveFileDialog^ saveFileDialog1;
		saveFileDialog1 = gcnew System::Windows::Forms::SaveFileDialog();
		saveFileDialog1->Filter = "JPEG Files (*.jpg)| *.jpg | Bmp Files (*.bmp)| *.bmp" ;//| Gif Files (*.gif) | *.gif | Tiff Files (*.tiff)| *.tiff" ;	//filters the visible files..
		saveFileDialog1->FilterIndex = 1;
		if(saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			System::String^ filePath = saveFileDialog1->FileName;
			PPCALCOBJECT->SaveAllImages(this->image_pixels, Cnt); // &RWrapper::pixels[0], Cnt);
			double VWidth = MAINDllOBJECT->getWindow(0).getWinDim().x;
			double VHeight = MAINDllOBJECT->getWindow(0).getWinDim().y;
			RGBArray2Image(this->image_pixels, VWidth, VHeight, filePath, false, false);//&RWrapper::pixels[0]
		}  	
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0029", ex);
	}
}


void RWrapper::RW_MainInterface::ZoomToExtentsRcadorDxf(int windowno)
{
	MAINDllOBJECT->getWindow(1).ZoomToExtents(windowno);
}

void RWrapper::RW_MainInterface::ClearShapeAndMeasure_Selections()
{
	 MAINDllOBJECT->ClearShapeMeasureSelections();
}

void RWrapper::RW_MainInterface::SaveVideoWindowImage(System::String^ FilePath, bool WithGraphics, bool ShowOnPicViewer)
{
	try
	{
		if(!MAINDllOBJECT->CameraConnected())return;
		MAINDllOBJECT->SaveImageFlag = true;
		double VWidth = MAINDllOBJECT->getWindow(0).getWinDim().x;
		double VHeight = MAINDllOBJECT->getWindow(0).getWinDim().y;
		//ZeroMemory(this->pixels,  RWrapper::pixels, 40274688);
		bool IncludeAlphainImage = false, flipImageVertical = false;
		if (WithGraphics)
		{
			MAINDllOBJECT->SaveImage_WithGraphics(this->image_pixels); // &RWrapper::pixels[0]);
			IncludeAlphainImage = true; flipImageVertical = true;
		}
		else
		{
			MAINDllOBJECT->SaveImage_WithoutGraphics(this->image_pixels); // &RWrapper::pixels[0]);
			VWidth *= HELPEROBJECT->CamSizeRatio;
			VHeight *= HELPEROBJECT->CamSizeRatio;
		}
		RGBArray2Image(this->image_pixels, VWidth, VHeight, FilePath, IncludeAlphainImage, flipImageVertical);//&RWrapper::pixels[0]
		if(ShowOnPicViewer)
			SavedImagePath::raise(FilePath);
		MAINDllOBJECT->SaveImageFlag = false;
	}
	catch(Exception^ ex){MAINDllOBJECT->SaveImageFlag = false; WriteErrorLog("WRMAIN0028", ex); }
}

void RWrapper::RW_MainInterface::SaveRcadWindowImage(System::String^ FilePath, bool ShowOnPicViewer)
{
	try
	{
		MAINDllOBJECT->SaveImageFlag = true;
		double RWidth = MAINDllOBJECT->getWindow(1).getWinDim().x;
		double RHeight = MAINDllOBJECT->getWindow(1).getWinDim().y;

		BYTE* RcadImage = MAINDllOBJECT->getRWindowImage(RWidth, RHeight);
		RGBArray2Image(RcadImage, RWidth, RHeight, FilePath, true, true);
		free(RcadImage);
		if(ShowOnPicViewer)
			SavedImagePath::raise(FilePath);
		MAINDllOBJECT->SaveImageFlag = false;
	}
	catch(Exception^ ex){MAINDllOBJECT->SaveImageFlag = false; WriteErrorLog("WRMAIN0030c", ex); }
}

void RWrapper::RW_MainInterface::RGBArray2Image(BYTE* SourceImagePixel, int width, int height, System::String^ FilePath, bool IncludeAlpha,  bool FlipY)
{
	try
	{
		int ByteLen = 3;
		System::Drawing::Imaging::PixelFormat PixFramt = System::Drawing::Imaging::PixelFormat::Format24bppRgb;
		if(IncludeAlpha)
		{
			ByteLen = 4;
			PixFramt = Imaging::PixelFormat::Format32bppArgb;
		}
		cli::array<BYTE> ^MemPtr = gcnew cli::array<BYTE>(width * height * ByteLen);
		//Cast from LPVOID to char *
		char* lkr = (char*)(LockResource(SourceImagePixel));
		//Copy from unmanaged memory to managed array
		Marshal::Copy((IntPtr)lkr, MemPtr, 0, width * height * ByteLen);
		System::Drawing::Bitmap^ bmp = gcnew System::Drawing::Bitmap(width, height);
		System::Drawing::Imaging::BitmapData^ imagedata = bmp->LockBits(Drawing::Rectangle(0, 0, width, height), Imaging::ImageLockMode::ReadWrite, PixFramt);
		Marshal::Copy(MemPtr, 0, imagedata->Scan0, width * height * ByteLen);
		bmp->UnlockBits(imagedata);
		int index = FilePath->IndexOf('.');
		System::String^ substr = FilePath->Substring(index + 1, 3);
		if(FlipY)
			bmp->RotateFlip(System::Drawing::RotateFlipType::RotateNoneFlipY);

		if(substr == "jpg")
			bmp->Save(FilePath, System::Drawing::Imaging::ImageFormat::Jpeg);
		else if(substr == "bmp")
			bmp->Save(FilePath, System::Drawing::Imaging::ImageFormat::Bmp);
		else if(substr == "gif")
			bmp->Save(FilePath, System::Drawing::Imaging::ImageFormat::Gif);
		else 
			bmp->Save(FilePath, System::Drawing::Imaging::ImageFormat::Jpeg);
	}
	catch(Exception^ ex){ WriteErrorLog("WRMAIN0030", ex); }
}

void RWrapper::RW_MainInterface::Form_KeyUp(System::Windows::Input::KeyEventArgs^ e)
{
	try
	{	
		if(PPCALCOBJECT->IsPartProgramRunning())return;
		if((e->KeyboardDevice->Modifiers & ModifierKeys::None)  == ModifierKeys::None)
		{
			MAINDllOBJECT->ControlOn = false;
			MAINDllOBJECT->ShiftON = false;
			MAINDllOBJECT->AltON = false;
			MAINDllOBJECT->keyUp(Convert::ToInt32(e->Key),0,0,0);
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0009", ex);
	}
}

void RWrapper::RW_MainInterface::Form_KeyDown(System::Windows::Input::KeyEventArgs^ e)
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning() && (!MAINDllOBJECT->RotateDXFflag) && (!MAINDllOBJECT->NudgeToolflag))	
		{
			if(e->Key == Key::Escape)
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
			if(e->Key != Key::F5)
				 MAINDllOBJECT->SetStatusCode("RW_PartProgram02");
			return;
		}
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		if((e->KeyboardDevice->Modifiers & ModifierKeys::Control)  == ModifierKeys::Control)
			MAINDllOBJECT->ControlOn = true;
		else if((e->KeyboardDevice->Modifiers & ModifierKeys::Shift)  == ModifierKeys::Shift)
			MAINDllOBJECT->ShiftON = true;
		else if((e->KeyboardDevice->Modifiers & ModifierKeys::Alt)  == ModifierKeys::Alt)
			MAINDllOBJECT->AltON = true;
		else if((e->KeyboardDevice->Modifiers & ModifierKeys::None)  == ModifierKeys::None)
		{
			MAINDllOBJECT->ShiftON = false;
			MAINDllOBJECT->ControlOn = false;
			MAINDllOBJECT->AltON = false;
		}
		if((e->KeyboardDevice->Modifiers & ModifierKeys::Control)  == ModifierKeys::Control && (e->KeyboardDevice->Modifiers & ModifierKeys::Shift)  == ModifierKeys::Shift)
		{
			MAINDllOBJECT->ShiftON = true;
			MAINDllOBJECT->ControlOn = true;
		}
		if((e->KeyboardDevice->Modifiers & ModifierKeys::Control)  == ModifierKeys::Control && (e->KeyboardDevice->Modifiers & ModifierKeys::Alt)  == ModifierKeys::Alt)
		{
		/*	WriteUserLog("Form_KeyDown", "Key Pressed", "Control + Alt + " + e->Key);*/
			MAINDllOBJECT->ControlOn = true;
			MAINDllOBJECT->AltON = true;
			switch(e->Key)
			{
			case Key::C:
				break;
			case Key::S:
				//SaveImage_WithGraphics();
				break;
			case Key::X:
				MAINDllOBJECT->deleteMeasurement();
				break;
			case Key::V:
				break;
			case Key::P:
				break;
			case Key::M:
				break;
			case Key::L:
				break;
			case Key::O:
				break;
			case Key::R:
				break;
			case Key::D:
				break;
			case Key::Z:
				MAINDllOBJECT->Redo();
				break;
			case Key::Q:
				break;
			}
		}
		else if(MAINDllOBJECT->ControlOn)
		{
			/*WriteUserLog("Form_KeyDown", "Key Pressed", "Control + " + e->Key);*/
			switch(e->Key)
			{
			case Key::F1:
				//DROInstance->ToManual();
				//RcadControls::MYINSTANCE()->mainRCad->WindowState = System::Windows::Forms::FormWindowState::Minimized;
				break;
			case Key::A:
				if(!MAINDllOBJECT->SelectAll())
					MAINDllOBJECT->DeselectAll(false);
				break;
			case Key::F:
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::FASTRACE_HANDLER);
				break;
			case Key::C:
				MAINDllOBJECT->CopyShape();
				break;
			case Key::S:
				//SaveImage_WithoutGraphics();
				break;
			case Key::X:
				GenerateToolbarClickEvent::raise("Delete", "X", true);
				MAINDllOBJECT->deleteShape();
				MAINDllOBJECT->ControlOn = false;
				break;
			case Key::V:
				MAINDllOBJECT->PasteShape();
				break;
			case Key::P:
				break;
			case Key::M:
				if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::MEASURE_HANDLER)
					GenerateToolbarClickEvent::raise("RCAD Toolbar", "Measurement", false);
				else
					GenerateToolbarClickEvent::raise("RCAD Toolbar", "Measurement", true);
				break;
			case Key::L:
				GenerateToolbarClickEvent::raise("RCAD Toolbar", "Linear Mode", !MAINDllOBJECT->LinearModeFlag());
				MAINDllOBJECT->ControlOn = false;
				break;
			case Key::O:
				break;
			case Key::R:
				GenerateToolbarClickEvent::raise("RCAD Toolbar", "Data Transfer to Excel", true);
				MAINDllOBJECT->ControlOn = false;
				break;
			case Key::D:
				GenerateToolbarClickEvent::raise("Tools", "Login", true);
				MAINDllOBJECT->ControlOn = false;
				break;
			case Key::Z:
				GenerateToolbarClickEvent::raise("Undo", "Z", true);
				MAINDllOBJECT->Undo();
				MAINDllOBJECT->ControlOn = false;
				break;
			case Key::Q:
				//RWrapper::RcadAnnotation1::MYINSTANCE()->createBitmap();
				break;
			case Key::D0:
				break;
			case Key::D1:
				MAINDllOBJECT->AddNewShape(RapidEnums::SHAPETYPE::RPOINT);
				break;
			case Key::D2:
				MAINDllOBJECT->AddNewShape(RapidEnums::SHAPETYPE::LINE);
				break;
			case Key::D3:
				MAINDllOBJECT->AddNewShape(RapidEnums::SHAPETYPE::CIRCLE);
				break;
			case Key::D4:
				MAINDllOBJECT->AddNewShape(RapidEnums::SHAPETYPE::ARC);
				break;
			case Key::D5:
				break;
			case Key::D6:
				break;
			case Key::D7:
				break;
			case Key::D8:
				break;
			case Key::D9:
				break;
			case Key::NumPad0:
				break;
			case Key::NumPad1:
				MAINDllOBJECT->AddNewShape(RapidEnums::SHAPETYPE::RPOINT);
				break;
			case Key::NumPad2:
				MAINDllOBJECT->AddNewShape(RapidEnums::SHAPETYPE::LINE);
				break;
			case Key::NumPad3:
				MAINDllOBJECT->AddNewShape(RapidEnums::SHAPETYPE::CIRCLE);
				break;
			case Key::NumPad4:
				MAINDllOBJECT->AddNewShape(RapidEnums::SHAPETYPE::ARC);
				break;
			case Key::NumPad5:
				break;
			case Key::NumPad6:
				break;
			case Key::NumPad7:
				break;
			case Key::NumPad8:
				break;
			case Key::NumPad9:
				break;
			case Key::Insert:
				RWrapper::RW_PartProgram::MYINSTANCE()->SaveCurrentDROValue();
				break;
			}
			//MAINDllOBJECT->ControlOn = false;
		}
		else if(MAINDllOBJECT->ShiftON )
		{
			/*WriteUserLog("Form_KeyDown", "Key Pressed", "Shift + " + e->Key);*/
			switch(e->Key)
			{
			case Key::Escape:
				 ClearALL();
				break;
			case Key::C:
				break;
			case Key::S:
				break;
			case Key::X:
				break;
			case Key::V:
				break;
			case Key::P:
				break;
			case Key::M:
				break;
			case Key::L:
				break;
			case Key::O:
				break;
			case Key::R:
				break;
			case Key::D:
				break;
			case Key::D0:
				break;
			case Key::D1:
				if(LastSelectedButton != "Flexible")
					GenerateToolbarClickEvent::raise("Crosshair", "Flexible", true);
				else
					GenerateToolbarClickEvent::raise("Crosshair", "Flexible", false);
				break;
			case Key::D2:
				if(LastSelectedButton != "Scan Mode")
					GenerateToolbarClickEvent::raise("Crosshair", "Scan Mode", true);
				else
					GenerateToolbarClickEvent::raise("Crosshair", "Scan Mode", false);
				break;
			case Key::D3:
				if(LastSelectedButton != "FrameGrab Rectangle")
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab Rectangle", true);
				else
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab Rectangle", false);
				break;
			case Key::D4:
				if(LastSelectedButton != "FrameGrab Circle")
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab Circle", true);
				else
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab Circle", false);
				break;
			case Key::D5:
				if(LastSelectedButton != "FrameGrab Arc")
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab Arc", true);
				else
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab Arc", false);
				break;
			case Key::D6:
				if(LastSelectedButton != "FrameGrab AngularRect")
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab AngularRect", true);
				else
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab AngularRect", false);
				break;
			case Key::D7:
				if(LastSelectedButton != "FrameGrab Fixed Rectangle")
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab Fixed Rectangle", true);
				else
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab Fixed Rectangle", false);
				break;
			case Key::D8:
				break;
			case Key::D9:
				break;
			case Key::NumPad0:
				break;
			case Key::NumPad1:
				if(LastSelectedButton != "Flexible")
					GenerateToolbarClickEvent::raise("Crosshair", "Flexible", true);
				else
					GenerateToolbarClickEvent::raise("Crosshair", "Flexible", false);
				break;
			case Key::NumPad2:
				if(LastSelectedButton != "Scan Mode")
					GenerateToolbarClickEvent::raise("Crosshair", "Scan Mode", true);
				else
					GenerateToolbarClickEvent::raise("Crosshair", "Scan Mode", false);
				break;
			case Key::NumPad3:
				if(LastSelectedButton != "FrameGrab Rectangle")
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab Rectangle", true);
				else
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab Rectangle", false);
				break;
			case Key::NumPad4:
				if(LastSelectedButton != "FrameGrab Circle")
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab Circle", true);
				else
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab Circle", false);
				break;
			case Key::NumPad5:
				if(LastSelectedButton != "FrameGrab Arc")
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab Arc", true);
				else
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab Arc", false);
				break;
			case Key::NumPad6:
				if(LastSelectedButton != "FrameGrab AngularRect")
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab AngularRect", true);
				else
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab AngularRect", false);
				break;
			case Key::NumPad7:
				if(LastSelectedButton != "FrameGrab Fixed Rectangle")
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab Fixed Rectangle", true);
				else
					GenerateToolbarClickEvent::raise("Crosshair", "FrameGrab Fixed Rectangle", false);
				break;
			case Key::NumPad8:
				break;
			case Key::NumPad9:
				break;
			}
		}
		else
		{
			/*WriteUserLog("Form_KeyDown", "Key Pressed", e->Key);*/
			switch(e->Key)
			{
			case Key::F1:		
				/*USER32DLLSTUFF::MYPOINT _myPt;
				USER32DLLSTUFF::GetCursorPos(& _myPt);
				RcadControls::MYINSTANCE()->ToolbarPanelHorizontal->Location = Point(_myPt.x,_myPt.y);
				RcadControls::MYINSTANCE()->ToolbarPanelHorizontal->Show();
				RcadControls::MYINSTANCE()->ToolbarPanelHorizontal->BringToFront();*/
				break;
			case Key::F2:
				break;
			case Key::F3:
				break;
			case Key::F4:
				break;
			case Key::F5:
				//RW_PartProgram::MYINSTANCE()->HandlePartProgramButtonActions("Run");
				break;
			case Key::F6:
				//RW_CNC::MYINSTANCE()->ActivateDeactivateCNC();
				break;
			case Key::F7:
				//MAINDllOBJECT->Showintersectionpts();
				break;
			case Key::F8:
				//MAINDllOBJECT->ToggleExtendedsnaps();
				break;
			case Key::F9:
				//DXFExport();
				DXFImport(false);
				break;
			case Key::F10:
		/*	case Key::System:*/
				//DXFImport();
				DXFEXPOSEOBJECT->editDXF();
				break;
			case Key::F11:
				//MAINDllOBJECT->editDXF();
				DXFExport(false);
				break;
			case Key::F12:
				RWrapper::RW_PartProgram::MYINSTANCE()->Handle_Program_btnClicks("Probe Position");
				break;
			case Key::Escape:
				//RW_CNC::MYINSTANCE()->MYINSTANCE()->CallManual_Click(true);
				MAINDllOBJECT->EscButtonHandling();
				RWrapper::RW_MainInterface::MYINSTANCE()->RaiseEvents(0);
				break;
			case Key::Space:
				MAINDllOBJECT->changeline();
				//MAINDllOBJECT->ProfileScan_PointEdgeDetect(MAINDllOBJECT->Last_MiddleX, MAINDllOBJECT->Last_MiddleY);
				break;
			case Key::Delete:
				//MAINDllOBJECT->DeleteFocusDatum();
				MAINDllOBJECT->deleteShape();
				break;
				
			}
		}
		MAINDllOBJECT->keyDown(Convert::ToInt32(e->Key),0,0,0);		
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0010", ex);
	}
}
 
System::String^ RWrapper::RW_MainInterface::GetStatusMessage(System::String^ code, System::String^ table)
 {
	 try
	 {
		System::String^ message = "";
		System::String^ dataBasePath = System::String::Concat(RWrapper::RW_MainInterface::DBConnectionString);
		System::Data::OleDb::OleDbConnection^ con = gcnew System::Data::OleDb::OleDbConnection(dataBasePath);
		con->Open();
		System::Data::OleDb::OleDbCommand^ command = gcnew  System::Data::OleDb::OleDbCommand("Select MsgName From" + table + "where MsgId='" + code + "'", con);
		if(System::Convert::ToString(command->ExecuteScalar()) != "")
			message = System::Convert::ToString(command->ExecuteScalar());
		con->Close();
		return message;
	 }
	 catch(Exception^ ex)
	 {
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0011", ex);
	 }
 }

void RWrapper::RW_MainInterface::ClearALL()
{
	if(MAINDllOBJECT->ShowMsgBoxString("RW_MainInterfaceClearALL01", RapidEnums::MSGBOXTYPE::MSG_YES_NO, RapidEnums::MSGBOXICONTYPE::MSG_QUESTION))
		InternalClearAll(0);
}

void RWrapper::RW_MainInterface::InternalClearAll(int id)
{
	try
	{
		if(id == 2)
		{
			ClearEverything::raise(id);
		}
		else
		{
			ProbeCount = 0;
			ProbeScanDetais_ArrayList->Clear();
			//System::Windows::MessageBox::Show("rwm1");
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
			MAINDllOBJECT->changeUCS(0);
			//System::Windows::MessageBox::Show("rwm2");
			ClearLastSelectedButton();
			RWrapper::RW_FocusDepth::MYINSTANCE()->AutoFocusMode = false;
			RWrapper::RW_PartProgram::MYINSTANCE()->ReturnTohomePosition = false;
			RWrapper::RW_FixedGraph::MYINSTANCE()->ClearFixedGraph(true);
			//System::Windows::MessageBox::Show("rwm3");
			MAINDllOBJECT->ClearAllModes();
			MAINDllOBJECT->AngleMeasurementMode(0);
			RWrapper::RW_PCD::MYINSTANCE()->ClearLastPCDValues();
			//System::Windows::MessageBox::Show("rwm4");
			ClearEverything::raise(id);
			RWrapper::RW_PCD::MYINSTANCE()->ResetAllPCD();
			MAINDllOBJECT->clearAll();
			CALBCALCOBJECT->ClearAllCalibrationCalcualtions();
			//System::Windows::MessageBox::Show("rwm5");
			RefreshDRO();
			//this->DigitalZoomLevel = this->InitialDigitalZoomLevel;
			RW_DBSettings::MYINSTANCE()->UpdateVideoPixelWidth("D" + this->DigitalZoomLevel.ToString());
			//System::Windows::MessageBox::Show("rwm6");
			ClearEverything::raise(3);
			MAINDllOBJECT->OnMouseLeave_Video();
			RWrapper::RW_CNC::MYINSTANCE()->UpdateLightStatusAfterClear();
			RWrapper::RW_MainInterface::MYINSTANCE()->deletePointsLogFile();
			RWrapper::RW_DelphiCamModule::MYINSTANCE()->ClearAll();
			if(RWrapper::RW_ImageDelayCalibration::Check_ImageDelay_Instance())
				RWrapper::RW_ImageDelayCalibration::MYINSTANCE()->ClearAll();
			if(RWrapper::RW_FixtureCalibration::Check_CallibrationInstance())
				RWrapper::RW_FixtureCalibration::MYINSTANCE()->ClearAll();					
			RWrapper::RW_CircularInterPolation::MYINSTANCE()->ClearAll();
			RWrapper::RW_LinearInterPolation::MYINSTANCE()->ClearAll();
			if(RWrapper::RW_RepeatAutoMeasure::Check_RepeatAutoMeasure_Instance())
				RWrapper::RW_RepeatAutoMeasure::MYINSTANCE()->ClearAll();
			if(RWrapper::RW_AutoProfileScanMeasure::Check_AutoProfileScan_Instance())
				RWrapper::RW_AutoProfileScanMeasure::MYINSTANCE()->ClearAll();
			if(RWrapper::RW_CT_SphereCallibration::Check_CallibrationInstance())
				RWrapper::RW_CT_SphereCallibration::MYINSTANCE()->ClearAll();
			if(RWrapper::RW_RepeatShapeMeasure::Check_RepeatShapeMeasure_Instance())
				RWrapper::RW_RepeatShapeMeasure::MYINSTANCE()->ClearAll();
			if(id == 0)
				MAINDllOBJECT->SetStatusCode("RW_MainInterface01");
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0027", ex);
	}
}

void RWrapper::RW_MainInterface::HandleMeasure_Click(System::String^ Str)
{
	try
	{
		PPCALCOBJECT->UpdateFistFramegrab(false);
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		WriteUserLog("HandleMeasure_Click", "MeasureButtonClick","Selected " + Str);
		if(PPCALCOBJECT->IsPartProgramRunning())	
		{
			MAINDllOBJECT->SetStatusCode("RW_PartProgram02");
			return;
		}
		if(Str == "Width")
		{
		  /*int imageCount = MAINDllOBJECT->GetAlltheImages();
			for(int i = 1; i <= imageCount; i++)
			{
				SaveAllImageswithoutgraphics(i);
			}*/
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DMM_HANDLER);
		   
		}
		else if(Str == "Perimeter")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PERIMETER_HANDLER);
		else if(Str == "FocusOnRightClick")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::FOCUSONRIGHTCLICKHANDLER);
		else if(Str == "AutoThread")
			MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::THREAD_LINEARCHANDLER);
		else if(Str == "OD Measure")
		{
			MAINDllOBJECT->SetStatusCode("HandleMeasure_Click01");
			MAINDllOBJECT->DoIdorOdMeasurement(true);
		}
		else if(Str == "ID Measure")
		{
			MAINDllOBJECT->SetStatusCode("HandleMeasure_Click02");
			MAINDllOBJECT->DoIdorOdMeasurement(false);
		}
		else if(Str == "Depth_TwoBox")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::FOCUS_DEPTHTWOBOXHANDLER);
		else if(Str == "Depth_FourBox")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::FOCUS_DEPTHFOURBOXHANDLER);
		else if(Str == "Angle")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::ANGLEMEASUREMENT);
		else if(Str == "SplineMeasure")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::SPLINEMEASUREMENT);
		else if(Str == "Angle_X_Axis")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::ANGLEMEASUREWITH_XAXIS);
		else if(Str == "Angle_Y_Axis")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::ANGLEMEASUREWITH_YAXIS);
		else if(Str == "Diameter")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::DIAMEASUREMENT);
		else if(Str == "Radius")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::RADIUSMEASUREMENT);
		else if(Str == "StepLength1")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::STEPMEASUREMENT_3LINES);
		else if(Str == "StepLength2")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::STEPMEASUREMENT_3LINES1);
		else if(Str == "StepLength3")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::STEPMEASUREMENT_4LINES);
		else if(Str == "Rake Angle")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::RECANGLEMEASUREMENT);
		else if(Str == "Chamfer_Line")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::CHAMFERMEASUREMENT_LINE);
		else if(Str == "Parallel Runout")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::PARALLELRUNOUT);
		else if(Str == "Intersection Runout")
		{
			if(MAINDllOBJECT->Vblock_CylinderAxisLine != NULL) 
				MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::INTERSECTIONRUNOUT);
		}
		else if(Str == "StepLengthWithAxis1")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::STEPMEASUREMENT_3LINESWITHAXIS);
		else if(Str == "StepLengthWithAxis2")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::STEPMEASUREMENT_4LINESWITHAXIS);
		else if(Str == "Chamfer_Arc")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::CHAMFERMEASUREMENT_ARC);
		else if(Str == "AutoGrooveMeasurement")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::GROOVEMEASUREMENTS_ONEFRAME);
		else if(Str == "GrooveMeasurement")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::GROOVEMEASUREMENT);
		else if(Str == "LineArcFrameGrab")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::LINEARCFG);
		else if(Str == "Point Angle")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::POINTANGLE);
		else if(Str == "ArcArcWidth")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::ARCARCWIDTH);
		else if(Str == "ArcPointWidth")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::POINTARCWIDTH);
		else if(Str == "LineArcWidth")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::LINEARCWIDTH);
		else if(Str == "LinePointWidth")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::POINTLINEWIDTH);
		else if(Str == "LineLineWidth")
			MAINDllOBJECT->SetSmartMeasurementHandler(RapidEnums::SMARTMEASUREMENTMODE::LINELINEWIDTH);
	
		if(MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER)
			SaveCurrentClickedButton("Measure", Str);
		else
			ClearLastSelectedButton();
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0014", ex);
	}

}

void RWrapper::RW_MainInterface::HandleDrawToolbar_Click(System::String^ Str)
{
	try
	{
		PPCALCOBJECT->UpdateFistFramegrab(false);
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		WriteUserLog("HandleDrawToolbar_Click", "DrawButtonClick","Selected " + Str);
		if(PPCALCOBJECT->IsPartProgramRunning())	
		{
			MAINDllOBJECT->SetStatusCode("RW_PartProgram02");
			return;
		}
		if(Str == "FasTrace Middle Point")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::FASTRACE_HANDLER);
		else if(Str == "FasTrace End Point")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::FASTRACE_HANDLER_ENDPT);
		else if(Str == "Circle")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CIRCLE3P_HANDLER);
		else if(Str == "Circle With Center")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CIRCLE1P_HANDLER);
		else if(Str == "Line")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER);
		else if(Str == "LineOnPlane")
		{
			Shape* Shp = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			if(Shp != NULL)
			{
				if(Shp->ShapeType == RapidEnums::SHAPETYPE::PLANE)
				{
					MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER);
					Shape* LnShp = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
					((Line*)LnShp)->SetProbeCorrectionShape(Shp);
				}
			}			
		}
		else if(Str == "Line3D")
		{
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::LINE3D_HANDLER);
		}
		else if(Str == "Arc")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER);
		else if(Str == "Ray")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::XRAY_HANDLER);
		else if(Str == "Infinite Line")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::XLINE_HANDLER);
		else if(Str == "Point")
		{
			//MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CIRCLEINVOLUTE_HANDLER);
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::POINT_HANDLER);
		}
		else if(Str == "Point3D")
		{
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::POINT_HANDLER3D);
		}
		else if(Str == "Relative Point")
		{
			/*MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::RELATIVEPOINT_HANDLER);*/
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::RELATIVEPOINT_HANDLER);
		}
		else if(Str == "Cloud Points")
		{
			//MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DOTCOUNTS_HANDLER);
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CLOUDPOINTS_HANDLER);
			MAINDllOBJECT->SetStatusCode("CloudPoints01");
		}
		else if(Str == "DotCounts")
		{
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DOTCOUNTS_HANDLER);
		}
		else if(Str == "PolyLine")
		{
			/*MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::POLYLINECLOUDPOINTS);*/
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::POLYLINECLOUDPOINTS);
			MAINDllOBJECT->SetStatusCode("CloudPoints01");
		}
		/*else if(Str == "Nearest Point")
			MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::NEARESTPOINT);
		else if(Str == "Farthest Point")
			MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::FARTHESTPOINT);*/
	/*	else if(Str == "Point on Line")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::POINTONLINE);
		else if(Str == "Point on Circle")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::POINTONCIRCLE);*/
		else if(Str == "Plane")
		{
			/*MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::PLANE_HANDLER);	*/
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PLANE_HANDLER);	
			MAINDllOBJECT->SetStatusCode("Plane01");
		}
		else if(Str == "Cylinder")
		{
			/*MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::CYLINDER_HANDLER);*/
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CYLINDER_HANDLER);
			MAINDllOBJECT->SetStatusCode("Cylinder01");
		}
		else if(Str == "Cone")
		{
		/*	MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::CONE_HANDLER);*/
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CONE_HANDLER);
			MAINDllOBJECT->SetStatusCode("Cone01");
		}		
		else if(Str == "Circle3D")
		{
			/*MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::CIRCLE3D_HANDLER);*/		
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CIRCLE3D_HANDLER);
			MAINDllOBJECT->SetStatusCode("Circle3D01");
		}		
		else if(Str == "Ellipse3D")
		{
			/*MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::ELLIPSE3D_HANDLER);*/
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ELLIPSE3D_HANDLER);
			MAINDllOBJECT->SetStatusCode("Ellipse3D01");
		}
		else if(Str == "Parabola3D")
		{
			/*MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::PARABOLA3D_HANDLER);*/
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PARABOLA3D_HANDLER);
			MAINDllOBJECT->SetStatusCode("Parabola3D01");
		}
		else if(Str == "Hyperbola3D")
		{
			/*MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::HYPERBOLA3D_HANDLER);*/
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::HYPERBOLA3D_HANDLER);
			MAINDllOBJECT->SetStatusCode("Hyperbola3D01");
		}
		else if(Str == "Sphere")
		{
		/*	MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::SPHERE_HANDLER);*/
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::SPHERE_HANDLER);
			MAINDllOBJECT->SetStatusCode("Sphere01");
		}
		else if(Str == "Torus")
		{
			/*MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::TORUS_HANDLER);*/
			MAINDllOBJECT->SetStatusCode("Torus01");
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::TORUS_HANDLER);
		}
		else if(Str == "TwoLine")
		{
			MAINDllOBJECT->SetStatusCode("TwoLine01");
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::TWOLINE_HANDLER);
		}
		else if(Str == "TwoArc")
		{
			MAINDllOBJECT->SetStatusCode("TwoArc01");
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::TWOARC_HANDLER);
		}
		else if(Str=="CircleInvolute")
		{
			MAINDllOBJECT->SetStatusCode("CircleInvolute01");
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CIRCLEINVOLUTE_HANDLER);
		}
		else if(Str == "Text")
		{
			MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::RTEXT_HANDLER);
			if(MAINDllOBJECT->SmartMeasureType == RapidEnums::SMARTMEASUREMENTMODE::DEFAULTTYPE)
				RWrapper::RW_MainInterface::EnterTextValue::raise();
		}
		else if(Str == "Seperate Line Arcs")
			MAINDllOBJECT->AddLineorArcShape_Points();
		else if(Str == "Spline")
		{
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::SPLINE_HANDLER);
		}
		else if(Str == "BestFit Surface")
		{
			MAINDllOBJECT->BestFitSurfaceWithDiffParam(0.5, true, true);
			BestFitSurfaceWindow(0.5, true);
		}
		MAINDllOBJECT->UpdateGraphicsOf3Window();
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		MAINDllOBJECT->Wait_DxfGraphicsUpdate();
		if(MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER)
			SaveCurrentClickedButton("Draw", Str);
		else if(Str == "Sphere" || Str == "Cylinder" || Str == "Plane" || Str == "Cone" || Str == "Cloud Points" ||
			Str == "Circle3D" || Str == "Ellipse3D" || Str == "Parabola3D" || Str == "Hyperbola3D")
			SaveCurrentClickedButton("Draw", Str);
		else
			ClearLastSelectedButton();
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0012", ex);
	}
}

void RWrapper::RW_MainInterface::HandleDerivedShapeToolbar_Click(System::String^ Str)
{
	try
	{
		PPCALCOBJECT->UpdateFistFramegrab(false);
		WriteUserLog("HandleDerivedShapeToolbar_Click", "DerivedShape ButtonClick","Selected " + Str);
		MAINDllOBJECT->ClearShapeSelections();	
		if (Str == "Mid Point")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::MIDPOINT);
		else if (Str == "Arc Tangential to End Lines")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::LINEARCLINE_HANDLER);
		else if (Str == "Parallel Line")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE);
		else if (Str == "Parallel Line FG")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE_FG);
		else if (Str == "Perpendicular Line")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PERPENDICULAR_LINE);
		else if (Str == "Parallel Tangent")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PARALLEL_TANGENT);
		else if (Str == "Perpendicular Tangent")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PERPENDICULAR_TANGENT);
		else if (Str == "Tangent")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::TANGENT_ON_CIRCLE);
		else if (Str == "Angle Bisector")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ANGLE_BISECTOR);
		else if (Str == "Tangent to 2 Circles")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::TANGENT_ON_2CIRCLE);
		else if (Str == "Parallel Tangent to 2 Circles")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PARALLEL_TANGENT_2CIRCLE);
		else if (Str == "Tangent through a Point")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::TANGENT_POINT_CIRCLE);
		else if (Str == "Line 1 Point + Angle")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ONE_POINTANGLE_LINE);
		else if (Str == "Line 1 Point + Offset")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ONE_POINT_OFFSET_LINE);
		else if (Str == "Circle Tangent to Circle")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CIRCLETANTOCIRCLE);
		else if (Str == "Circle Tangent to 2 Circles")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CIRCLETANTO2CIRCLE);
		else if (Str == "Plane Through 2 Line")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PLANEFROMTWOLINEHANDLER);
		else if (Str == "Circle Tangent to 2 Circles1")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CIRCLETANTO2CIRCLE1);
		else if (Str == "CircleTangentToLineCircle")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CIRCLETANTOLINECIRCLE);
		else if (Str == "Circle Inside Triangle")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CIRCLE_TANTOTRIANGLEIN);
		else if (Str == "Circle Outside Triangle")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CIRCLE_TANTOTRIANGLEOUT);
		else if (Str == "Parallel Arc")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PARALLELARC);
		else if (Str == "Circle Tangent to 2 Lines")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::TANGENTIAL_CIRCLE);
		else if (Str == "Nearest Point")
			MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::NEARESTPOINT);
		else if (Str == "Farthest Point")
			MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::FARTHESTPOINT);
		else if (Str == "Nearest Point3D")
			MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::NEARESTPOINT3D);
		else if (Str == "Farthest Point3D")
			MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::FARTHESTPOINT3D);
		else if (Str == "Arcs Tangential to Each other")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ARC_TANTO_ARC_HANDLER);
		else if (Str == "Arc Tangential to Line")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ARC_TANTO_LINE_HANDLER);
		else if (Str == "Point on Line")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::POINTONLINE);
		else if (Str == "Point on Circle")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::POINTONCIRCLE);
		else if (Str == "Fillet")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::FILLET_HANDLER);
		else if (Str == "PCD")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PCD_HANDLER);
		else if (Str == "3DPCD")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PCD_HANDLER_3D);
		else if (Str == "Parallel Plane")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PARALLEL_PLANE);
		else if (Str == "Perpendicular Plane")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PERPENDICULAR_PLANE);
		else if (Str == "Perpendicular Plane on Line")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PERPENDICULARPLANE_ONLINE);
		else if (Str == "Parallel Line3D")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE3D);
		else if (Str == "Perpendicular Line3D")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PERPENDICULAR_LINE3D);
		else if (Str == "Shapes Rotation")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ROTATIONHANDLER);
		else if (Str == "Shapes Reflection")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::REFLECTIONHANDLER);
		else if (Str == "Shapes Translation")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::TRANSLATIONHANDLER);
		else if (Str == "TwoPoint Alignment")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::TWOPOINTALIGNMENTHANDLER);
		else if (Str == "PointToPoint Translation")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::TRANSLATIONHANDLERPOINTTOPOINT);
		else if (Str == "Shapes Projection")			
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::SHAPEPROJECTIONHANDLER);
		else if(Str == "Perpendicular Line On Plane")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::LINEPER2LINEONPLN_HANDLER);
		else if(Str == "Intersection Shape")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::INTERSECTIONSHAPEHANDLER);
		else if(Str ==  "Sphere Vision Calibration")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::SPHEREFRAMEGRABHANDLER);

		if(MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER)
			SaveCurrentClickedButton("Draw", Str);
		else
			ClearLastSelectedButton();
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0012", ex);
	}
}

void RWrapper::RW_MainInterface::HandleCrossHairToolbar_Click(System::String^ Str)
{
	try
	{
		PPCALCOBJECT->UpdateFistFramegrab(false);
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		WriteUserLog("HandleCrossHairToolbar_Click", "DerivedShape ButtonClick","Selected " + Str);
		if(PPCALCOBJECT->IsPartProgramRunning())	
		{
			 MAINDllOBJECT->SetStatusCode("RW_PartProgram02");
			return;
		}
		if(Str == "Hatched")
		{
			MAINDllOBJECT->SetStatusCode("Hatched01");
			MAINDllOBJECT->HatchedCrossHairMode.Toggle();
		}
		else if(Str == "Scale View")
		{
			MAINDllOBJECT->SetStatusCode("ScaleView01");
			MAINDllOBJECT->ScaleViewMode.Toggle();
		}
		else if(Str == "Cross-Hair Visible")
		{
			MAINDllOBJECT->SetStatusCode("CrossHairVisible01");
			MAINDllOBJECT->ShowHideCrossHair(false);
		}
		else if(Str == "Cross-Hair Hidden")
			MAINDllOBJECT->ShowHideCrossHair(true);	
		else if(Str == "Crosshair Color")
		{
			MAINDllOBJECT->SetStatusCode("CrosshairColor01");
			if(this->_colorDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				System::Drawing::Color _colorRgb = this->_colorDialog->Color;
				MAINDllOBJECT->SetCrosshair_Color(_colorRgb.R/255.0, _colorRgb.G/255.0, _colorRgb.B/255.0);
				CalibrationModule::DB ^GetValues = gcnew CalibrationModule::DB(RWrapper::RW_MainInterface::DBConnectionString);
				cli::array<System::String^>^ TableNames = {"MachineDetails", "CurrentMachine"};
				GetValues->FillTableRange(TableNames);
				System::String^ MachineNo = System::Convert::ToString(GetValues->GetRecord["CurrentMachine",GetValues->GetRowCount("CurrentMachine")-1,"MachineNo"]);
				System::Data::DataRow^ DataR =  GetValues->Select_ChildControls("MachineDetails","MachineNo", MachineNo)[0];
				DataR->default["CrosshairColorR"] = System::Convert::ToString(_colorRgb.R);
				DataR->default["CrosshairColorG"] = System::Convert::ToString(_colorRgb.G);
				DataR->default["CrosshairColorB"] = System::Convert::ToString(_colorRgb.B);
				GetValues->Update_Table("MachineDetails");
				GetValues->FinalizeClass();
			}
		}
		else if(Str == "Flexible Crosshair Color")
		{
			MAINDllOBJECT->SetStatusCode("FlexibleCrosshairColor01");
			if(this->_colorDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				System::Drawing::Color _colorRgb = this->_colorDialog->Color;
				MAINDllOBJECT->SetFlexibleCrosshair_Color(_colorRgb.R/255.0, _colorRgb.G/255.0, _colorRgb.B/255.0);
			}
		}
		else if(Str == "Profile Scan Line Arc")
			HELPEROBJECT->AddLineArcAction_ProfileScan();
		else
		{
			if(Str == "Flexible")
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR);
			else if(Str == "Scan Mode")
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::SCAN_CROSSHAIR);
			else if(Str == "Flexible Scan")
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::LIVESCAN_CROSSHAIR);
			else if (Str == "Current Position")
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::SELECTPOINT_HANDLER);
			else if(Str == "FrameGrab Circle")
			{
				if(MAINDllOBJECT->Continuous_ScanMode)
					MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_CIRCLE_FRAMEGRAB);
				else
					MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CIRCLE_FRAMEGRAB);
			}
			else if(Str == "FrameGrab Arc")
			{
				if(MAINDllOBJECT->Continuous_ScanMode)
					MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARC_FRAMEGRAB);
				else
					MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB);
			}
			else if(Str == "FrameGrab AngularRect")
			{
				if(MAINDllOBJECT->Continuous_ScanMode)
					MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARECT_FRAMEGRAB);
				else
					MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB);
			}
			else if(Str == "FrameGrab Rectangle")
			{
				if(MAINDllOBJECT->Continuous_ScanMode)
					MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_RECT_FRAMEGRAB);
				else
					MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB);
			}
			else if(Str == "FrameGrab All Edge")
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB_ALLEDGE);
			else if(Str == "FrameGrab Fixed Rectangle")
			{
				if(MAINDllOBJECT->Continuous_ScanMode)
					MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_FIXEDRECT_FRAMEGRAB);
				else
					MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::FIXEDRECT_FRAMEGRAB);
			}
			else if(Str == "Grab For MouseClick")
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::MOUSECLICK_GRAB);
			else if(Str == "Focus Scan")
				FOCUSCALCOBJECT->SetFocuScanMode();
			else if(Str == "Contour Scan")
			{
				MAINDllOBJECT->SetStatusCode("ContourScan01");
				FOCUSCALCOBJECT->SetContourScanMode();
			}
			else if(Str == "ContourScanFG")
				 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CONTOURSCAN_FGHANDLER);
			else if(Str == "AFS Triangulation")
				 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::AUTOFOCUSSCANHANDLER);	
			else if(Str == "ACS PointsSelection")
				 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::AUTOCONTOURSCANHANDLER);
			else if(Str=="Add Points From Shapes")				
				 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::POINTFROMSHAPEHANDLER);		
			if(MAINDllOBJECT->getSelectedShapesList().count() == 0)
				MAINDllOBJECT->SetStatusCode("RW_MainInterface02");
		}
		MAINDllOBJECT->UpdateGraphicsOf3Window();
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		MAINDllOBJECT->Wait_DxfGraphicsUpdate();
		if(MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER || (FOCUSCALCOBJECT->FocusScanMode() || FOCUSCALCOBJECT->ContourScanMode()))
			SaveCurrentClickedButton("Crosshair", Str);
		else
			ClearLastSelectedButton();
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0013", ex);
	}
}

void RWrapper::RW_MainInterface::HandleEditToolbar_Click(System::String^ Str)
{
	try
	{
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		WriteUserLog("HandleEditToolbar_Click", "EditToolbar ButtonClick","Selected " + Str);
		if(PPCALCOBJECT->IsPartProgramRunning())	
		{
			 MAINDllOBJECT->SetStatusCode("RW_PartProgram02");
			return;
		}
		if(Str == "Multiselect")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::SELECTION_HANDLER);
		else if(Str == "Trim")
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::TRIMMING_HANDLER);
		else if(Str == "Undo")
		{
			 MAINDllOBJECT->SetStatusCode("HandleEditToolbar_Click01");
			MAINDllOBJECT->Undo();
		}
		else if(Str == "Redo")
		{
			 MAINDllOBJECT->SetStatusCode("HandleEditToolbar_Click02");
			MAINDllOBJECT->Redo();
		}
		else if(Str == "Clear All")
		{
			 MAINDllOBJECT->SetStatusCode("HandleEditToolbar_Click03");
			 ClearALL();
		}
		else if(Str == "Copy")
		{
			 MAINDllOBJECT->SetStatusCode("HandleEditToolbar_Click04");
			MAINDllOBJECT->CopyShape();
		}
		else if(Str == "Paste")
		{
			MAINDllOBJECT->SetStatusCode("HandleEditToolbar_Click05");
			MAINDllOBJECT->PasteShape();
		}
		else if(Str == "Export Points To Text")
			ExportPointToText();
		else if(Str == "Export Points To Csv")
			ExportPointToCsv();
		else if(Str == "Export Points To CAD")
			ExportPointToCAD();	
		
		if(MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER)
			SaveCurrentClickedButton("Edit", Str);
		else
			ClearLastSelectedButton();
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0013", ex);
	}
}

void RWrapper::RW_MainInterface::HandleToolsToolbar_Click(System::String^ Str)
{
	try
	{
		 MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		 MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		 WriteUserLog("HandleToolsToolbar_Click", "ToolsToolbar ButtonClick","Selected " + Str);
		 if(Str == "Video Source")
		 {
			 MAINDllOBJECT->SetStatusCode("HandleToolsToolbar_Click01");
			 MAINDllOBJECT->videoDialog();
		 }
		 else if(Str == "Reset Video")
		 {
			 MAINDllOBJECT->SetStatusCode("HandleToolsToolbar_Click02");
			 MAINDllOBJECT->cameraFormatDialog();
		 }
		 else if(Str == "Flip Horizontal")
		 {
			 //MessageBox::Show("Calling Flip Horizontal");
			 MAINDllOBJECT->FlipVHorizontal();
		 }
		 else if(Str == "Flip Vertical")
		 {
			 MAINDllOBJECT->FlipVVertical();
		 }
		 else if (Str->Contains("Operate Pins")) 
		 {
			 CCCCCt++;
			 switch (CCCCCt)
			 {
			 //case 1: DROInstance->ControlPins(1, true); break;
			 //case 2: DROInstance->ControlPins(1, false); break;
			 case 3: DROInstance->ControlPins(2, true); break;
			 case 4: DROInstance->ControlPins(2, false); break;

			 default:
				 break;
			 }
			 if (CCCCCt > 4) CCCCCt = 2;

		 }
		 else if(Str->Contains("Digital Zoom"))
		 {			
			 //MAINDllOBJECT->SetStatusCode("HandleToolsToolbar_Click05");
			 //if (MAINDllOBJECT->CurrentCameraType == 3)
			 //{
				// this->DigitalZoomLevel /= 2.0;  //System::Math::Round(this->DigitalZoomLevel / 2.0, 1);
				// if (this->DigitalZoomLevel < 1.0) this->DigitalZoomLevel = 4.0;
			 //}
			 //else if(MAINDllOBJECT->CurrentCameraType == 0)
			 //{
				// if (MAINDllOBJECT->FullWindowDigitalZoom())
				// {
				//	 if (MAINDllOBJECT->currCamWidth == 800)
				//	 {
				//		 MAINDllOBJECT->currCamWidth = 1600; MAINDllOBJECT->currCamHeight = 1200;
				//		 this->DigitalZoomLevel = 2.0;
				//	 }
				//	 else
				//	 {
				//		 MAINDllOBJECT->currCamWidth = 1600; MAINDllOBJECT->currCamHeight = 1200;
				//		 this->DigitalZoomLevel = 1.0;
				//	 }
				// }
				// else
				// {
				//	 if (MAINDllOBJECT->currCamWidth == 800)
				//	 {
				//		 MAINDllOBJECT->currCamWidth = 800; MAINDllOBJECT->currCamHeight = 600;
				//		 this->DigitalZoomLevel = 1.0;
				//	 }
				// }
			 //}
				// 
			 //MAINDllOBJECT->ConnectToCamera( (HWND)this->VideoWindowHandle->ToPointer(), MAINDllOBJECT->currCamWidth, MAINDllOBJECT->currCamHeight, this->DigitalZoomLevel,
				//									MAINDllOBJECT->VideoDisplayWidth, MAINDllOBJECT->VideoDisplayHeight, 20, true );
			 //RW_DBSettings::MYINSTANCE()->UpdateVideoPixelWidth("D" + this->DigitalZoomLevel.ToString());
			
			 
			 //MAINDllOBJECT->DigitalZoomON();
		 }
		 //else if(Str == "Digital Zoom OFF")
		 //{
			//
			// MAINDllOBJECT->DigitalZoomOff();
		 //}
		 else if(Str == "Freeze")
		 {
			 MAINDllOBJECT->SetStatusCode("HandleToolsToolbar_Click03");
			 MAINDllOBJECT->FreezeON();
		 }
		 else if(Str == "Resume")
		 {
			 MAINDllOBJECT->ResumeThevideo();
		 }
		 /* else if(Str == "Save Frame With Graphics")
			SaveImage_WithGraphics();
		 else if(Str == "Save Frame")
			SaveImage_WithoutGraphics();	*/
		 else if(Str == "Sigma Mode")
		 {
			 MAINDllOBJECT->SetStatusCode("HandleToolsToolbar_Click08");
			MAINDllOBJECT->SigmaModeFlag.Toggle();
		 }
		 else if(Str == "Measurement in Video Visible")
		 {
			 MAINDllOBJECT->SetStatusCode("HandleToolsToolbar_Click04");
			MAINDllOBJECT->ShowMeasurementOnVideo(false);
		 }
		 else if(Str == "Measurement in Video Hidden")
		 {
			
			 MAINDllOBJECT->ShowMeasurementOnVideo(true);
		 }
		 else if(Str == "Graphics On Video Visible")
		 {
			 MAINDllOBJECT->SetStatusCode("HandleToolsToolbar_Click06");
			MAINDllOBJECT->hideGraphics_OnVideo(true);
		 }
		 else if(Str == "Graphics On Video Hidden")
		 {
			
			 MAINDllOBJECT->hideGraphics_OnVideo(false);
		 }
		 else if(Str == "Deg Min Sec")
		 {
			 MAINDllOBJECT->SetStatusCode("HandleToolsToolbar_Click07");
			RWrapper::RW_MeasureParameter::MYINSTANCE()->SetAngleMeasureMode(0);
		 }
		 else if(Str == "Decimal Deg")
		 {
			MAINDllOBJECT->SetStatusCode("HandleToolsToolbar_Click13");
			RWrapper::RW_MeasureParameter::MYINSTANCE()->SetAngleMeasureMode(1);
		 }
		 else if(Str == "Radian")
		 {
			 MAINDllOBJECT->SetStatusCode("HandleToolsToolbar_Click12");
			RWrapper::RW_MeasureParameter::MYINSTANCE()->SetAngleMeasureMode(2);
		 }
		/* else if(Str == "FocusScan Calibration")
		 {
			 
			RWrapper::RW_FocusDepth::MYINSTANCE()->DoFocusCalibration();
		 }*/
		 else if(Str == "Reset DRO")
		 {
			 MAINDllOBJECT->SetStatusCode("HandleToolsToolbar_Click09");
			 if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ROTARY_DEL_FI)
				 DROInstance->ResetDROReadings();
			 else if(RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag < 3)
				 DROInstance->ToReset();
		 }
		 else if(Str == "Home Position")
		 {
			  MAINDllOBJECT->SetStatusCode("HandleToolsToolbar_Click11");
			 if(RWrapper::RW_MainInterface::MYINSTANCE()->MachineConnectedStatus)
			 {
				if(MAINDllOBJECT->ShowMsgBoxString("RW_MainInterfaceHandleToolsToolbar_Click01", RapidEnums::MSGBOXTYPE::MSG_YES_NO, RapidEnums::MSGBOXICONTYPE::MSG_QUESTION))
					RWrapper::RW_AutoCalibration::MYINSTANCE()->Handle_HomeClicked(true, true, false);
			 }
		 }
		 else if(Str == "Reconnect USB")
		 {
			  MAINDllOBJECT->SetStatusCode("HandleToolsToolbar_Click10");
			 DROInstance->ConnectToHardware(true);
		 }
		 else if(Str == "Auto Focus")
			 RWrapper::RW_FocusDepth::MYINSTANCE()->AutoFocusMode = !RWrapper::RW_FocusDepth::MYINSTANCE()->AutoFocusMode;
		 if (MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER)
			 SaveCurrentClickedButton("Tools", Str);
		 else if (Str == "TwoXLens")
			 this->TwoXLensFitted = !this->TwoXLensFitted;
		 else
			ClearLastSelectedButton();
		 if(Str == "Deg Min Sec" || Str == "Decimal Deg" || Str == "Radian")
			 RefreshDRO();
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0014a", ex);
	}
}

void RWrapper::RW_MainInterface::HandleRcadToolbar_Click(System::String^ Str)
{
	try
	{
		 MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		 MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		 WriteUserLog("HandleRcadToolbar_Click", "RcadToolbar ButtonClick","Selected " + Str);
		 if(Str == "Home")
		 {
			 MAINDllOBJECT->SetStatusCode("HandleRcadToolbar_Click10");
			MAINDllOBJECT->Reset3DRotation();
			MAINDllOBJECT->homeTheWindow(1);
		 }
		 /*else if(Str == "Export CAD")
			DXFExport();*/
		 else if(Str == "3D Mode")
		 {
			MAINDllOBJECT->SetStatusCode("HandleRcadToolbar_Click07");
			MAINDllOBJECT->Toggle3DMode();
			if(MAINDllOBJECT->RcadWindow3DMode())
			{
				if(MAINDllOBJECT->RotateGrafix())
				   MAINDllOBJECT->RotateRcadFor3DMode();
				else
				   MAINDllOBJECT->SetRcadTransformationMatrix();
				MAINDllOBJECT->RotateGrafix(false);
			}
			MAINDllOBJECT->ProjectionType = RapidEnums::RAPIDPROJECTIONTYPE::XY;
			/*MAINDllOBJECT->ChangeRcadViewOrientions(RapidEnums::RAPIDPROJECTIONTYPE::XY);*/
		 }
		 else if(Str == "TransferDeviationDataToExcel")
			DataTransferToExcelForDeviation();
		 else if(Str == "ShowCoordinatePlanes")
		 {
			 MAINDllOBJECT->ShowCoordinatePlaneFlag(!MAINDllOBJECT->ShowCoordinatePlaneFlag());
			 MAINDllOBJECT->ShowHideCoordinatePlanes();
		 }
		 else if(Str == "3D Rotate")
		 {
			 MAINDllOBJECT->SetStatusCode("HandleRcadToolbar_Click08");
			 MAINDllOBJECT->ThreeDRotationMode.Toggle();
			 if( MAINDllOBJECT->ThreeDRotationMode() && !MAINDllOBJECT->RcadWindow3DMode())
				GenerateToolbarClickEvent::raise("RCAD Toolbar", "2D Mode", true);
		 }
		 else if(Str == "EnableLight")
		 {
			MAINDllOBJECT->EnableTriangulationLighting = !MAINDllOBJECT->EnableTriangulationLighting;
			MAINDllOBJECT->UpdateShapesChanged();
		 }
		 else if(Str == "DiscreteColorBand")
		 {
			 MAINDllOBJECT->DiscreteColor = !MAINDllOBJECT->DiscreteColor;
			 MAINDllOBJECT->UpdateShapesChanged();
		 }
		 else if(Str == "Filled Surface")
		 {
			 MAINDllOBJECT->SetStatusCode("HandleRcadToolbar_Click09");
			MAINDllOBJECT->ToggleWiredLoopMode(true);
		 }
		 else if(Str == "Wired Surface")
		 {
			MAINDllOBJECT->SetStatusCode("HandleRcadToolbar_Click16");
			MAINDllOBJECT->ToggleWiredLoopMode(false);
			MAINDllOBJECT->ToggleTransparencyMode(true);
		 }
		 else if(Str == "Transparent")
		 {
			MAINDllOBJECT->ToggleTransparencyMode(false);
		 }
		  else if(Str == "Draw Cylinder Disk")
			MAINDllOBJECT->ToggleCylinderDiskDrawMode();
		 else if(Str == "Dont Move With DRO")
			 MAINDllOBJECT->RcadGraphicsMoveWithDRO(true);
		 else if(Str == "Move With DRO")
		 {
			 MAINDllOBJECT->SetStatusCode("HandleRcadToolbar_Click17");
			 MAINDllOBJECT->RcadGraphicsMoveWithDRO(false);
		 }
		 else if(Str == "Pan")
		 {
			MAINDllOBJECT->SetStatusCode("HandleRcadToolbar_Click02");
			MAINDllOBJECT->SetPanmode(1);
		 }
		 else if (Str == "Zoom-In")
		 {
			 /*		 if(MAINDllOBJECT->getWindow(1).PanMode)
						 GenerateToolbarClickEvent::raise("RCAD Toolbar", "Pan", true);*/
			 //MAINDllOBJECT->OnMouseWheel_Video(1);
			 MAINDllOBJECT->UnsetPanmode(1);
			 OnRCadMouseWheel(480);
		 }

		 else if (Str == "Zoom-Out")
		 {
			 /*		 if(MAINDllOBJECT->getWindow(1).PanMode)
						 GenerateToolbarClickEvent::raise("RCAD Toolbar", "Pan", true);*/
			 //MAINDllOBJECT->OnMouseWheel_Video(1);
			 OnRCadMouseWheel(-480);
		 }
		 else if(Str == "Hide Probe")
		 {
			 MAINDllOBJECT->ShowProbeDraw.Toggle();
			 MAINDllOBJECT->update_RcadGraphics();
		 }
		 else if(Str == "Zoom To Extents")
		 {
			MAINDllOBJECT->SetStatusCode("HandleRcadToolbar_Click05");
			MAINDllOBJECT->ZoomToWindowExtents(1);
		 }
		 else if(Str == "Graphics Rotated Along UCS" || Str == "Graphics Not Rotated Along UCS")
		 {
			 MAINDllOBJECT->SetStatusCode("HandleRcadToolbar_Click06");
			 MAINDllOBJECT->KeepGraphicsStright();	
		 }
		 else if(Str == "XY View")
		 {
			  MAINDllOBJECT->RotateGrafix(true);
			  MAINDllOBJECT->ProjectionType = RapidEnums::RAPIDPROJECTIONTYPE::XY;
			  MAINDllOBJECT->SetStatusCode("HandleRcadToolbar_Click13");
			  MAINDllOBJECT->ChangeWindowViewOrientation(1, RapidEnums::RAPIDPROJECTIONTYPE::XY);
			  MAINDllOBJECT->ZoomToWindowExtents(1);  
		 }
		 else if(Str == "YZ View")
		 {
			  MAINDllOBJECT->RotateGrafix(true);
			 MAINDllOBJECT->ProjectionType = RapidEnums::RAPIDPROJECTIONTYPE::YZ;
			 MAINDllOBJECT->SetStatusCode("HandleRcadToolbar_Click14");
			 MAINDllOBJECT->ChangeWindowViewOrientation(1, RapidEnums::RAPIDPROJECTIONTYPE::YZ);
			 MAINDllOBJECT->ZoomToWindowExtents(1);			
		 }
		 else if(Str == "XZ View")
		 {
			 MAINDllOBJECT->RotateGrafix(true);
			 MAINDllOBJECT->ProjectionType = RapidEnums::RAPIDPROJECTIONTYPE::XZ;
			 MAINDllOBJECT->SetStatusCode("HandleRcadToolbar_Click15");
			 MAINDllOBJECT->ChangeWindowViewOrientation(1, RapidEnums::RAPIDPROJECTIONTYPE::XZ);
			 MAINDllOBJECT->ZoomToWindowExtents(1);			
		 }
		 else if(Str == "Bounded Partial Shapes")
			MAINDllOBJECT->ChangePartialShapeDrawMode(0);
		 else if(Str == "Localised Partial Shapes")
			MAINDllOBJECT->ChangePartialShapeDrawMode(1);
		 else if(Str == "Whole Shape")
			MAINDllOBJECT->ChangePartialShapeDrawMode(2);

		 else
		 {
			 if(PPCALCOBJECT->IsPartProgramRunning())	
			 {
				 MAINDllOBJECT->SetStatusCode("RW_PartProgram02");
				return;
			 }
			 if(Str == "Measurement Color")
			 {
				if(this->_colorDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
				{
					System::Drawing::Color _colorRgb = this->_colorDialog->Color;
					MAINDllOBJECT->SetMeasurementColor(_colorRgb.R,_colorRgb.G,_colorRgb.B);
				}
			 }
			 else if(Str == "Roughness Measurement")// 
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CLOUDPOINTMEASUREMENTHANDLER);

			 else if(Str == "Measurement")
			 {
				 if(Is_measurement_mode)
					 Is_measurement_mode =false;
				 else
					 Is_measurement_mode=true;
				 RCadApp::set_flag_for_measurement_mode(Is_measurement_mode);
				 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::MEASURE_HANDLER);
			 }
			 else if(Str == "Automated Mode")
				 MAINDllOBJECT->AutomatedMeasureModeFlag.Toggle();
			 else if (Str == "Linear Mode")
			 {
				 MAINDllOBJECT->SetStatusCode("HandleRcadToolbar_Click01");
				 MAINDllOBJECT->LinearModeFlag.Toggle();
			 }
			 else if(Str == "PCD Mode")
				MAINDllOBJECT->PCDMeasureModeFlag.Toggle();
			 else if(Str == "RotaryMeasure")
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ROTARYMEASURE_HANDLER);
			 else if(Str == "Zoom-In")
			 {
		/*		 if(MAINDllOBJECT->getWindow(1).PanMode)
					GenerateToolbarClickEvent::raise("RCAD Toolbar", "Pan", true);*/
				 MAINDllOBJECT->SetStatusCode("HandleRcadToolbar_Click03");
				 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::RCADZOOMIN);
			 }
			 else if(Str == "Zoom-Out")
			 {
		/*		 if(MAINDllOBJECT->getWindow(1).PanMode)
					GenerateToolbarClicEvent::raise("RCAD Toolbar", "Pan", true);*/
				 MAINDllOBJECT->SetStatusCode("HandleRcadToolbar_Click04");
				 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::RCADZOOMOUT);
			 }
			 else if(Str == "Delete Measurement")
			 {
				  MAINDllOBJECT->SetStatusCode("HandleRcadToolbar_Click11");
				 MAINDllOBJECT->deleteMeasurement();
			 }
		 }
		 if(MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER)
			SaveCurrentClickedButton("RCAD Toolbar", Str);
		 else
			ClearLastSelectedButton();
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0023", ex);
	}
}

void RWrapper::RW_MainInterface::HandleDXFToolbar_Click(System::String^ Str)
{
	try
	{
		MAINDllOBJECT->Wait_DxfGraphicsUpdate();
		WriteUserLog("HandleDXFToolbar_Click", "DXFToolbar ButtonClick","Selected " + Str);
		if(PPCALCOBJECT->IsPartProgramRunning())	
		{
			MAINDllOBJECT->SetStatusCode("RW_PartProgram02");
			return;
		}
		if(Str == "Edit CAD")
		{
			MAINDllOBJECT->SetStatusCode("HandleDXFToolbar_Click04");
			DXFEXPOSEOBJECT->editDXF();
		}
		else if(Str == "Home")
		{
			MAINDllOBJECT->SetStatusCode("HandleDXFToolbar_Click10");
			MAINDllOBJECT->homeTheWindow(2);
		}
		else if(Str == "1 Point EditAlign")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CAD_1PT_EDITALIGN_HANDLER);
		else if(Str == "2 Point EditAlign")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CAD_2PT_EDITALIGN_HANDLER);
		else if(Str == "2 Line EditAlign")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::EDITCAD_2LINEHANDLER);
		else if(Str == "1Point 1Line EditAlign")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::EDITCAD_1PT1LINE_HANDLER);
		else if(Str == "1 Point")
		{
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_1PT);//ReadFocusValuesFromText();
		}
		else if(Str == "2 Points")
		{
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_2PT);
		}
		else if(Str == "CopyShape2PtAlign")
		{
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::COPYSHAPE_2PTALIGN);
		}
		else if(Str == "1 Point 1 Line")
		{
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_1PT1LINE);
		}
		else if(Str == "1 Point 1 Line 3D")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_1PT1LINE3D);
		else if(Str == "2 Lines")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_2LINE);
		else if(Str == "1 Line 3D")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CADALIGNMENT_1LINE3D);
		else if(Str == "Zoom-In")
		{
			MAINDllOBJECT->SetStatusCode("HandleDXFToolbar_Click02");
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DXFZOOMIN);
		}
		else if(Str == "Zoom-Out")
		{
			MAINDllOBJECT->SetStatusCode("HandleDXFToolbar_Click03");
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DXFZOOMOUT);
		}
		/*else if(Str == "Export CAD")
		{
			MAINDllOBJECT->SetStatusCode("HandleDXFToolbar_Click05");
			DXFExport();
		}*/
		else if(Str == "Import CAD")
		{
			 MAINDllOBJECT->SetStatusCode("HandleDXFToolbar_Click01");
			DXFImport(false);
		}
		else if(Str == "Zoom To Extents")
		{
			MAINDllOBJECT->ZoomToWindowExtents(2);
		}
		else if(Str == "Pan")
		{
			MAINDllOBJECT->SetStatusCode("HandleDXFToolbar_Click09");
			MAINDllOBJECT->SetPanmode(2);
		}
		else if(Str == "Align")
			DXFEXPOSEOBJECT->AlginTheDXF();
		else if(Str == "Reset Align")
		{
			MAINDllOBJECT->SetStatusCode("HandleDXFToolbar_Click06");
			DXFEXPOSEOBJECT->ResetDxfAlignment();
		}
		else if(Str == "Area")
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DXFAREA_HANDLER);
		if(MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER)
			SaveCurrentClickedButton("DXF Toolbar", Str);
		else
			ClearLastSelectedButton();
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0024", ex);
	}
}

void RWrapper::RW_MainInterface::HandleShapeParameter_Click(System::String^ Str)
{
	try
	{
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		WriteUserLog("HandleShapeParameter_Click", "ShapeParameter ButtonClick","Selected " + Str);
		if(PPCALCOBJECT->IsPartProgramRunning())	
		{
			 MAINDllOBJECT->SetStatusCode("RW_PartProgram02");
			return;
		}
		if(Str == "Copy")
			MAINDllOBJECT->CopyShape();
		else if(Str == "Paste")
			MAINDllOBJECT->PasteShape();
		else if(Str == "FG Points Visible")
			MAINDllOBJECT->ShowFGPoints(false);
		else if(Str == "FG Points Hidden")
			MAINDllOBJECT->ShowFGPoints(true);
		else if(Str == "Reset Points")
		{
			MAINDllOBJECT->ResetthePoints_selectedshape();
			RWrapper::RW_ShapeParameter::MYINSTANCE()->ResetShapePointsTable();
			RWrapper::RW_ShapeParameter::MYINSTANCE()->RaiseShapeChangedEvents(RWrapper::RW_Enum::RW_SHAPEPARAMTYPE::SHAPE_POINTS_UPDATE);
		}
		else if(Str == "Shape Color")
		{
			if(this->_colorDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				System::Drawing::Color _colorRgb = this->_colorDialog->Color;
				MAINDllOBJECT->SetshapeColor(_colorRgb.R,_colorRgb.G,_colorRgb.B);
			}
		}
		else if(Str == "Select Point")
		{
			ReadPointFromText();
			//MAINDllOBJECT->SelectPointforShapes();//ReadPointFromText();
		}
		else if(Str == "Export Points To Text")
			ExportPointToText();
		else if(Str == "Export Points To Csv")
			ExportPointToCsv();
		else if(Str == "Export Points To CAD")
			ExportPointToCAD();	
		else if(Str == "Export Shape Param To Csv")
			ExportParamToCsv();
		else if(Str == "Select All Shapes")
			MAINDllOBJECT->SelectAll();
		else if(Str == "Deselect All Shapes")
			MAINDllOBJECT->DeselectAll();
		else if(Str == "AddPlane")
			MAINDllOBJECT->AddNewPlaneForSelectedPoints();
		else if(Str == "AddSpline")
			MAINDllOBJECT->AddNewSplineForSelectedPoints();
		else if(Str == "AddCircle")
			MAINDllOBJECT->AddNewCircleForSelectedPoints();
		else if(Str == "AddLine")
			MAINDllOBJECT->AddNewLineForSelectedPoints();
		/*else if(Str == "Show Deviation")
			DXFEXPOSEOBJECT->ShowDeviationwithCloudPts();*/
		else if(Str == "As Reference Shape")
			MAINDllOBJECT->SetShapeAsReference_Shape();
		else if(Str == "Set For Two Step Homing")
			MAINDllOBJECT->SetShapeForTwoStepHmoing(true);
		else if(Str == "Set For Base Plane Alignment")
			MAINDllOBJECT->SetShapeForTwoStepHmoing(false);
		else if(Str == "Apply Line Arc Seperation")
			MAINDllOBJECT->ApplyLineArcSeperation_SelectedShape();		
		else if(Str == "Delete Shape")
			 MAINDllOBJECT->deleteShape();
		else if(Str == "Merge Shapes As Line")
			HELPEROBJECT->MergeShapesOfLineArcAction(true);
		else if(Str == "Merge Shapes As Arc")
			HELPEROBJECT->MergeShapesOfLineArcAction(false);
		else if(Str == "Chamfer Line-Line")
			HELPEROBJECT->Chamfer_Line_Arc(true);	
		else if(Str == "Chamfer Line-Arc")
			HELPEROBJECT->Chamfer_Line_Arc(false);
		else if(Str == "Hide Shape")
		{
			Shape *tmpShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			if(tmpShape != NULL)
			{
				tmpShape->IsHidden(!tmpShape->IsHidden());
				MAINDllOBJECT->UpdateShapesChanged(true);	
				RWrapper::RW_ShapeParameter::MYINSTANCE()->SetShapeParamForFrontend(tmpShape);
				RWrapper::RW_ShapeParameter::MYINSTANCE()->RaiseShapeChangedEvents(RWrapper::RW_Enum::RW_SHAPEPARAMTYPE::SHAPE_HIDE_UNHIDE);
			}
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0025", ex);
	}
}

void RWrapper::RW_MainInterface::HandleMeasureParameter_Click(System::String^ Str)
{
	try
	{
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		WriteUserLog("HandleMeasureParameter_Click", "MeasureParameter ButtonClick","Selected " + Str);
		 if(Str == "Delete Measurement")
			 MAINDllOBJECT->deleteMeasurement();
		 else if(Str == "Measurement Color")
		 {
			if(this->_colorDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				System::Drawing::Color _colorRgb=this->_colorDialog->Color;
				MAINDllOBJECT->SetMeasurementColor(_colorRgb.R,_colorRgb.G,_colorRgb.B);
			}
		 }
		else if(Str == "Edit Measurement")
			MAINDllOBJECT->setEditmeasurment();
		 else if(Str == "SetPPAbortCheck")
		{
			if(MAINDllOBJECT->getSelectedDimList().count() == 1)
			{
				 DimBase* Cdim = (DimBase*)MAINDllOBJECT->getDimList().selectedItem();
				 Cdim->MeasurementUsedForPPAbort(true);
			}
		}
		else if(Str == "Change Measure Mode")
			MAINDllOBJECT->ChangeMeasurementMode();
		else if(Str == "Measure Font Size")
		{
			if(MAINDllOBJECT->getSelectedDimList().count() == 1)
			{
				 DimBase* Cdim = (DimBase*)MAINDllOBJECT->getDimList().selectedItem();
				 int Cfsize = Cdim->RcadFont_ScaleFactor();
				 ChangeMeasurementFontSize::raise(Cfsize, "");
			 }
			 else if(MAINDllOBJECT->getSelectedDimList().count() > 1)
			 {
				 ChangeMeasurementFontSize::raise(20, "");
			 }
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0020", ex);
	}
}

void RWrapper::RW_MainInterface::HandleUCSProperties_Click(System::String^ Str)
{
	try
	{
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		WriteUserLog("HandleUCSProperties_Click", "UCSProperties ButtonClick", "Selected " + Str);
		if(PPCALCOBJECT->IsPartProgramRunning())	
		{
			 MAINDllOBJECT->SetStatusCode("RW_PartProgram02");
			return;
		}
		 if(Str == "Shift Origin")
			 MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::UCS_SHIFT);
		 else if(Str == "Delete UCS")
		 {	
			 MAINDllOBJECT->SetStatusCode("HandleUCSProperties_Click04");
			 MAINDllOBJECT->deleteUCS();
			 MAINDllOBJECT->updateAll();
		 }
		 else if(Str == "Point Align")
		 {
			 if(MAINDllOBJECT->getCurrentUCS().UCSMode() != 1)
				 MAINDllOBJECT->changeUCS(0);
			 MAINDllOBJECT->UCSProjectionType = RapidEnums::RAPIDPROJECTIONTYPE::XY;
			 MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::UCS_POINT);	
		 }
		 else if(Str == "Line Align")
		 {
			 if(MAINDllOBJECT->getCurrentUCS().UCSMode() != 1)
				 MAINDllOBJECT->changeUCS(0);
			 MAINDllOBJECT->UCSProjectionType = RapidEnums::RAPIDPROJECTIONTYPE::XY;
			 MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::UCS_LINE);
			 
		 }
		 else if(Str == "UCS BasePlane")
		 {
			 MAINDllOBJECT->changeUCS(0);
			 MAINDllOBJECT->UCSProjectionType = RapidEnums::RAPIDPROJECTIONTYPE::XY;
			 MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::UCS_PLANE);
		 }
		 else if(Str == "Rotate Axis")
		 {
			 MAINDllOBJECT->changeUCS(0);
			 RWrapper::RW_UCSParameter::MYINSTANCE()->RaiseUCSChangedEvents(RWrapper::RW_Enum::RW_UCSPARAMETER_TYPE::UCS_ROTATE_ANGLE);
		 }
		 else if(Str == "XY")
		 {
			 MAINDllOBJECT->changeUCS(0);
			 MAINDllOBJECT->UCSProjectionType = RapidEnums::RAPIDPROJECTIONTYPE::XY;
			 MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::UCS_XY);	
		 }
		 else if(Str == "YZ")
		 {
			 MAINDllOBJECT->changeUCS(0);
			 MAINDllOBJECT->UCSProjectionType = RapidEnums::RAPIDPROJECTIONTYPE::YZ;
			 MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::UCS_YZ);	
		 }
		 else if(Str == "XZ")
		 {
			 MAINDllOBJECT->changeUCS(0);
			 MAINDllOBJECT->UCSProjectionType = RapidEnums::RAPIDPROJECTIONTYPE::XZ;			 
			 MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::UCS_XZ);	
		 }
		 else if (Str == "YZ-")
		 {
			 //Check out if we need to make a new RapidHandler for the negative types
			 MAINDllOBJECT->changeUCS(0);
			 MAINDllOBJECT->UCSProjectionType = RapidEnums::RAPIDPROJECTIONTYPE::YZm;
			 MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::UCS_YZ);
		 }
		 else if (Str == "XZ-")
		 {
			 MAINDllOBJECT->changeUCS(0);
			 MAINDllOBJECT->UCSProjectionType = RapidEnums::RAPIDPROJECTIONTYPE::XZm;
			 MAINDllOBJECT->SetOtherhandlers(RapidEnums::RAPIDHANDLERTYPE::UCS_XZ);
		 }
		 RWrapper::RW_DRO::MYINSTANCE()->UpdateDRO_textbox();
		 RWrapper::RW_DRO::MYINSTANCE()->UpdateCenterScreen();
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0018", ex);
	}
}

void RWrapper::RW_MainInterface::HandleVisionHeadToolBar_Click(System::String^ Str)
{
	try 
	{
		if (Str == "Measurement" || Str == "Linear Mode" || Str == "PCD Mode")
			RW_MainInterface::MYINSTANCE()->HandleRcadToolbar_Click(Str);
		else
			RW_MainInterface::MYINSTANCE()->HandleDXFToolbar_Click(Str);
	}
	catch (Exception^ ex)
	{
		WriteErrorLog("WRMAIN0023a", ex);
	}

}

void RWrapper::RW_MainInterface::HandleRightClick_OnEntities(System::String^ Alingment, System::String^ Str)
{
	try
	{
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->Wait_RcadGraphicsUpdate();
		WriteUserLog("HandleRightClick_OnEntities", "RightClick On Entities", Str);
		if(PPCALCOBJECT->IsPartProgramRunning())	
		{
			 MAINDllOBJECT->SetStatusCode("RW_PartProgram02");
			return;
		}
		if(Alingment == "RightClickOnShape")// || Alingment == "VideoContextMenu") //// || Alingment == "VideoContextMenu" With the exe given on 12.. for hide/showFG points..
			HandleRightClickOnShape(Str);
		else if(Alingment == "RightClickOnMeasure")
			HandleRightClickOnMeasure(Str);
		else if(Alingment == "RightClickOnCircle")
			HandleRightClickOnCircle(Str);
		else if(Alingment == "RightClickOnArc")
			HandleRightClickOnCircle(Str);
		else if(Alingment == "RightClickOnLine")
			HandleRightClickOnLine(Str);
		else if(Alingment == "RightClickOnPoint")
			HandleRightClickOnPoint(Str);
		else if(Alingment == "RightClickOnPerimeter")
			HandleRightClickOnPerimeter(Str);
		else if(Alingment == "RightClickOnLine3D")
			HandleRightClickOnLine3D(Str);
		else if(Alingment == "RightClickOnPlane")
			HandleRightClickOnPlane(Str);
		else if(Alingment == "RightClickOnCylinder")
			HandleRightClickOnCylinder(Str);
		else if(Alingment == "RightClickOnCone")
			HandleRightClickOnCone(Str);
		else if(Alingment == "RightClickOnSphere")
			HandleRightClickOnSphere(Str);
		else if(Alingment == "SurfaceFrameGrab")
			SetSurfaceGrabType(Str);
		else if(Alingment == "RightClickOnCircle3D")
			HandleRightClickOnCircle3D(Str);
		else if(Alingment == "RightClickOnArc3D")
			HandleRightClickOnCircle3D(Str);
		else if(Alingment == "RightClickOnCloudPoints")
			HandleRightClickOnCloudPoints(Str);
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0013", ex);
	}
}

void RWrapper::RW_MainInterface::HandleRightClickOnShape(System::String^ Str)
{
	try
	 {
		WriteUserLog("HandleRightClickOnShape", "RightClick On Shape", "Selected " + Str);
		if(Str == "FG Points Visible")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnShape01");
			MAINDllOBJECT->ShowFGPoints(false);
		}
		else if(Str == "FG Points Hidden")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnShape02");
			MAINDllOBJECT->ShowFGPoints(true);
		}
		else if(Str == "Delete Shape")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnShape04");
			MAINDllOBJECT->deleteShape();
		}
		else if(Str == "Shape Color")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnShape05");
			if(this->_colorDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				System::Drawing::Color _colorRgb=this->_colorDialog->Color;
				MAINDllOBJECT->SetshapeColor(_colorRgb.R,_colorRgb.G,_colorRgb.B);
			}
		 }
		else if(Str == "Select All Shapes")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnShape06");
			MAINDllOBJECT->SelectAll();
		 }
		else if(Str == "Deselect All Shapes")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnShape07");
			MAINDllOBJECT->DeselectAll();
		}
		else if(Str == "As Reference Shape")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnShape08");
			MAINDllOBJECT->SetShapeAsReference_Shape();
		}
		else if(Str == "Set For Two Step Homing")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnShape09");
			MAINDllOBJECT->SetShapeForTwoStepHmoing(true);
		}
		else if(Str == "Set For Base Plane Alignment")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnShape10");
			MAINDllOBJECT->SetShapeForTwoStepHmoing(false);
		}
		else if(Str == "Set As Rotary Entity")
			MAINDllOBJECT->SetShapeAsRotaryEntity();
		else if(Str == "Apply Line Arc Seperation")
			MAINDllOBJECT->ApplyLineArcSeperation_SelectedShape();	
		else if(Str == "Save Points To Hard disc")
		{
			MAINDllOBJECT->DoNotSamplePts = false;
			MAINDllOBJECT->SavePointsToHardDisc();
		}
		else if(Str == "Read Points From Hard disc")
		{
			MAINDllOBJECT->DoNotSamplePts = true;
			MAINDllOBJECT->GetFgPoints_fromTextFile();
			MAINDllOBJECT->DoNotSamplePts = false;
		}
		/*else if(Str == "Set For Reference Dot")
		{
			if(MAINDllOBJECT->ReferenceDotShape != NULL) return;
			MAINDllOBJECT->ReferenceDotShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			MAINDllOBJECT->SetShapeForReferenceDot();
		}*/
		else if(Str == "Apply ProbeCorrection")
		{
			Shape *tmpShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			tmpShape->ProbeCorrection(true);
			((ShapeWithList*)tmpShape)->UpdateShape();
		}
		else if(Str == "Remove ProbeCorrection")
		{
			Shape *tmpShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			tmpShape->ProbeCorrection(false);
			((ShapeWithList*)tmpShape)->UpdateShape();
		}
		else if(Str == "Get Total Length")
			MAINDllOBJECT->MeasurementForHighlightedShape(0);
		else if(Str == "Get Diameter")
			MAINDllOBJECT->MeasurementForHighlightedShape(1);
		else if(Str == "Show Color Pattern")
		{
			for(RC_ITER Item_shape = MAINDllOBJECT->getShapesList().getList().begin(); Item_shape != MAINDllOBJECT->getShapesList().getList().end(); Item_shape++)
			{
				Shape* Cshape = (Shape*)(*Item_shape).second;
				if(!Cshape->Normalshape()) continue;
				if(Cshape->selected() && (Cshape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS))
				{
					((CloudPoints*)Cshape)->UpdatePointsColorWRTDeviation();
				}
			}
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0019", ex);
	}
}

void RWrapper::RW_MainInterface::HandleRightClickOnMeasure(System::String^ Str)
{
	try
	 {
		 WriteUserLog("HandleRightClickOnMeasure", "RightClick On Measure", "Selected " + Str);
		 if(Str == "Delete Measurement")
		 {
			 MAINDllOBJECT->SetStatusCode("HandleRightClickOnMeasure01");
			 MAINDllOBJECT->deleteMeasurement();
		 }
		 else if(Str == "Measurement Color")
		 {
			 MAINDllOBJECT->SetStatusCode("HandleRightClickOnMeasure02");
			if(this->_colorDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			{
				System::Drawing::Color _colorRgb = this->_colorDialog->Color;
				MAINDllOBJECT->SetMeasurementColor(_colorRgb.R,_colorRgb.G,_colorRgb.B);
			}
		 }
		else if(Str == "Edit Measurement")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnMeasure03");
			MAINDllOBJECT->setEditmeasurment();
		 }
		else if(Str == "Change Measure Mode")
			MAINDllOBJECT->ChangeMeasurementMode();
		else if(Str == "Measure Font Size")
		{
			if(MAINDllOBJECT->getSelectedDimList().count() == 1)
			{
				 DimBase* Cdim = (DimBase*)MAINDllOBJECT->getDimList().selectedItem();
				 int Cfsize = Cdim->RcadFont_ScaleFactor();
				 ChangeMeasurementFontSize::raise(Cfsize, "");
			 }
			 else if(MAINDllOBJECT->getSelectedDimList().count() > 1)
			 {
				 ChangeMeasurementFontSize::raise(20, "");
			 }
		}
		else if(Str == "SetPPAbortCheck")
		{
			if(MAINDllOBJECT->getSelectedDimList().count() == 1)
			{
				 DimBase* Cdim = (DimBase*)MAINDllOBJECT->getDimList().selectedItem();
				 Cdim->MeasurementUsedForPPAbort(true);
			}
		}
		else if(Str == "ResetPPAbortCheck")
		{
			if(MAINDllOBJECT->getSelectedDimList().count() == 1)
			{
				 DimBase* Cdim = (DimBase*)MAINDllOBJECT->getDimList().selectedItem();
				 Cdim->MeasurementUsedForPPAbort(false);
			}
		}
	 }
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0020", ex);
	}
}

void RWrapper::RW_MainInterface::HandleRightClickOnPoint(System::String^ Str)
{
	try
	{
		WriteUserLog("HandleRightClickOnPoint", "RightClick On Point", "Selected " + Str);
		if(Str == "Center")
		{
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE, 0))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "OrdinateX")
		{
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEX, 0))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "OrdinateY")
		{
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEY, 0))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "OrdinateZ")
		{
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEZ, 0))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "True Position")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle01");
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITIONPOINT_R, 0);
		}
		else if(Str == "True Position For 3D Point")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle01");
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITIONPOINT_R, 0);
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0022", ex);
	}

}

void RWrapper::RW_MainInterface::HandleRightClickOnLine(System::String^ Str)
{
	try
	{
		WriteUserLog("HandleRightClickOnLine", "RightClick On Line", "Selected " + Str);
		if(Str == "Straightness")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnLine01");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESS, 1))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Filtered Straightness")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnLine01");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_LINESTRAIGHTNESSFILTERED, 1))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Parallelism")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnLine03");
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_LINEPARALLELISM, 1);
		}
		else if(Str == "Perpendicularity")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnLine04");
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_LINEPERPENDICULARITY, 1);
		}
		else if(Str == "Angularity")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnLine02");
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_LINEANGULARITY, 1);
		}
		else if (Str == "GDnT_Profile")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnLine05");
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_PROFILE, 1);
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0022", ex);
	}
}

void RWrapper::RW_MainInterface::HandleRightClickOnPerimeter(System::String^ Str)
{
	try
	{
		WriteUserLog("HandleRightClickOnPerimeter", "RightClick On Perimeter", "Selected " + Str);
		if(Str == "Diameter")
		{
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_DIAMETER, 7))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Perimeter")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnPerimeter01");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_PERIMETER, 7))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Area")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnPerimeter02");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_PERIMETER_AREA, 7))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0022", ex);
	}
}

void RWrapper::RW_MainInterface::HandleRightClickOnCircle(System::String^ Str)
{
	try
	{
		WriteUserLog("HandleRightClickOnCircle", "RightClick On Circle", "Selected " + Str);
		if(Str == "Diameter")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle06");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_DIAMETER2D, 2))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Radius")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle07");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_RADIUS2D, 2))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Spline Count")
		{
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_SPLINECOUNT, 2))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Spline MIC")
		{
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_SPLINECIRCLEMIC, 2))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Spline MCC")
		{
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_SPLINECIRCLEMCC, 2))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Circularity")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle10");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_CIRCULARITY, 2))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Concentricity")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle11");
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_CONCENTRICITY, 2);
		}
		else if(Str == "Arc Angle")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle08");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_ARCANGLE, 2))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Arc Length")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle09");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_ARCLENGTH, 2))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Min Inscribed Dia")
		{
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEMIC, 2))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Max Circum Dia")
		{
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEMCC, 2))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "True Position")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle01");
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R, 2);
		}
		else if(Str == "Center")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle02");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE, 2))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "OrdinateX")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle03");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEX, 2))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "OrdinateY")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle04");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEY, 2))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "OrdinateZ")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle05");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEZ, 2))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Deviation")
		{
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_CIRCLEDEVIATION_MEASURE, 2);
				//MAINDllOBJECT->SetLmouseDownOfRcad();
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0021", ex);
	}
}

void RWrapper::RW_MainInterface::HandleRightClickOnCircle3D(System::String^ Str)
{
	try
	{
		WriteUserLog("HandleRightClickOnCircle3D", "RightClick On Circle3D", "Selected " + Str);
		if(Str == "Diameter")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle06");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_DIAMETER3D, 9))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Radius")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle07");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_RADIUS3D, 9))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Center")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle02");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE, 9))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "OrdinateX")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle03");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEX, 9))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "OrdinateY")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle04");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEY, 9))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "OrdinateZ")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle05");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEZ, 9))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "True Position")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle01");
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_TRUEPOSITION_R, 9);
		}
		else if(Str == "3D True Position")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle01");
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_3DTRUEPOSITION_R, 9);
		}
		
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0021", ex);
	}
}

void RWrapper::RW_MainInterface::HandleRightClickOnCloudPoints(System::String^ Str)
{
	try
	{
		WriteUserLog("HandleRightClickOnCloudPoints", "RightClick On CloudPoints", "Selected " + Str);
		if(Str == "Sq")
		{
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SQ, 10))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Sa")
		{
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SA, 10))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Sp")
		{
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SP, 10))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Sv")
		{
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SV, 10))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Sz")
		{
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SZ, 10))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Ssk")
		{
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SSK, 10))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Sku")
		{
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_SURFACEROUGHNESS_SKU, 10))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Get Total Length")
			MAINDllOBJECT->MeasurementForHighlightedShape(0);
		else if(Str == "Get Diameter")
			MAINDllOBJECT->MeasurementForHighlightedShape(1);
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0021b", ex);
	}
}

void RWrapper::RW_MainInterface::HandleRightClickOnLine3D(System::String^ Str)
{
	try
	{
		WriteUserLog("HandleRightClickOnLine3D", "RightClick On Line3D", "Selected " + Str);
		if(Str == "Straightness")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnLine01");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_LINE3DSTRAIGHTNESS, 3))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Filtered Straightness")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnLine01");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_LINE3DSTRAIGHTNESSFILTERED, 3))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Parallelism")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnLine03");
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPARALLELISM, 3);
		}
		else if(Str == "Perpendicularity")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnLine04");
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_LINE3DPERPENDICULARITY, 3);
		}
		else if(Str == "Angularity")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnLine02");
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_LINE3DANGULARITY, 3);
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0022", ex);
	}
}

void RWrapper::RW_MainInterface::HandleRightClickOnPlane(System::String^ Str)
{
	try
	{
		WriteUserLog("HandleRightClickOnPlane", "RightClick On Plane", "Selected " + Str);
		if(Str == "Flatness")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnPlane01");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_PLANEFLATNESS, 4))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Filtered Flatness")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnPlane01");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_PLANEFLATNESSFILTERED, 4))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Parallelism")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnPlane03");
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_PLANEPARALLELISM, 4);
		}
		else if(Str == "Perpendicularity")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnPlane04");
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_PLANEPERPENDICULARITY, 4);
		}
		else if(Str == "Angularity")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnPlane02");
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_PLANEANGULARITY, 4);
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0022", ex);
	}

}

void RWrapper::RW_MainInterface::HandleRightClickOnCylinder(System::String^ Str)
{
	try
	{
		WriteUserLog("HandleRightClickOnCylinder", "RightClick On Cylinder", "Selected " + Str);
		if(Str == "Cylindricity")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCylinder01");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITY, 5))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Filtered Cylindricity")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCylinder01");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_CYLINDRICITYFILTERED, 5))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Min Inscribed Dia")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCylinder02");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMIC, 5))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Max Circum Dia")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCylinder03");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERMCC, 5))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Perpendicularity")
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPERPENDICULARITY, 5);
		else if(Str == "Angularity")
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERANGULARITY, 5);
		else if(Str == "Coaxiality")
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERCOAXIALITY, 5);
		else if(Str == "Parallelism")
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_CYLINDERPARALLELISM, 5);
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0022", ex);
	}

}

void RWrapper::RW_MainInterface::HandleRightClickOnCone(System::String^ Str)
{
	try
	{
		WriteUserLog("HandleRightClickOnCone", "RightClick On Cone", "Selected " + Str);
		if(Str == "Conicality")
		{
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_CONICALITY, 8))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Perpendicularity")
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_CONEPERPENDICULARITY, 8);
		else if(Str == "Angularity")
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_CONEANGULARITY, 8);
		else if(Str == "Coaxiality")
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_CONECOAXIALITY, 8);
		else if(Str == "Parallelism")
			MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_CONEPARALLELISM, 8);
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0022", ex);
	}

}

void RWrapper::RW_MainInterface::HandleRightClickOnSphere(System::String^ Str)
{
	try
	{
		WriteUserLog("HandleRightClickOnSphere", "RightClick On Sphere", "Selected " + Str);
		if(Str == "Sphericity")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnSphere01");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_SPHERICITY, 6))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Min Inscribed Dia")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCylinder02");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_SPHEREMIC, 6))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Max Circum Dia")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCylinder03");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_SPHEREMCC, 6))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "Diameter")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle06");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_DIAMETER3D, 6))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
			else if(Str == "Center")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle02");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE, 6))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "OrdinateX")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle03");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEX, 6))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "OrdinateY")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle04");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEY, 6))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
		else if(Str == "OrdinateZ")
		{
			MAINDllOBJECT->SetStatusCode("HandleRightClickOnCircle05");
			if(MAINDllOBJECT->SetMeasure_RightClick(RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEZ, 6))
				MAINDllOBJECT->SetLmouseDownOfRcad();
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0022", ex);
	}

}

void RWrapper::RW_MainInterface::SetSurfaceGrabType(System::String^ Str)
 {
	 if(Str == "Type 1")
		MAINDllOBJECT->SurfaceFgtype(0);
	 else if(Str == "Type 2")
		MAINDllOBJECT->SurfaceFgtype(1);
	 else if(Str == "Type 3")
		MAINDllOBJECT->SurfaceFgtype(2);
	 else if(Str == "Type 4")
		MAINDllOBJECT->SurfaceFgtype(3);
	 else if(Str == "Type 5")
		MAINDllOBJECT->SurfaceFgtype(4);
 }
 
void RWrapper::RW_MainInterface::HandleCheckedProperty_Load(System::String^ Alignment, System::String^ Str)
{
	try
	{
		if(Alignment == "Measure")
		{
			HandleMeasure_Click(Str);
		}
		else if(Alignment == "Draw")
		{
			HandleDrawToolbar_Click(Str);
		}
		else if(Alignment == "Crosshair")
		{
			HandleCrossHairToolbar_Click(Str);
		}
		else if(Alignment == "Edit")
		{
			HandleEditToolbar_Click(Str);
		}
		else if(Alignment == "Tools")
		{
			HandleToolsToolbar_Click(Str);
		}
		else if(Alignment == "RCAD Toolbar")
		{
			HandleRcadToolbar_Click(Str);
		}
		else if(Alignment == "DXF Toolbar")
		{
			HandleDXFToolbar_Click(Str);
		}
		else if(Alignment == "PartProgram" || Alignment == "Part Program Options")
		{
			RWrapper::RW_PartProgram::MYINSTANCE()->Handle_Program_btnClicks(Str);
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0026", ex);
	}
}

void RWrapper::RW_MainInterface::HandleRaiseToolBarClickevent(System::String^ LastAlignment, System::String^ LastBtnStr, System::String^ CurrentAlignment, System::String^ CurrentBtnStr)
{
	try
	{
		if(MAINDllOBJECT->SmartMeasureType != RapidEnums::SMARTMEASUREMENTMODE::DEFAULTTYPE) return;
		if(CurrentAlignment == "Measure")
		{
		}
		else if(CurrentAlignment == "Draw")
		{
			if(CurrentBtnStr == "Circle" || CurrentBtnStr == "Circle With Center" || CurrentBtnStr == "Line" ||
				CurrentBtnStr == "Arc" || CurrentBtnStr == "Ray" || CurrentBtnStr == "Infinite Line" || CurrentBtnStr == "Line3D" ||
				CurrentBtnStr == "Plane" || CurrentBtnStr == "Sphere" || CurrentBtnStr == "Cylinder" || CurrentBtnStr == "Cone" ||
				CurrentBtnStr == "Cloud Points" || CurrentBtnStr == "Circle3D" || CurrentBtnStr == "Ellipse3D" || CurrentBtnStr == "Parabola3D" || CurrentBtnStr == "Hyperbola3D")
			{
				if(LastAlignment == "Crosshair")
				{
					if(LastBtnStr == "Flexible" || LastBtnStr == "Scan Mode" || LastBtnStr == "Flexible Scan" || LastBtnStr == "Current Position" ||
						LastBtnStr == "FrameGrab Circle" || LastBtnStr == "FrameGrab Arc" || LastBtnStr == "FrameGrab AngularRect" || LastBtnStr == "FrameGrab Rectangle" || 
						LastBtnStr == "FrameGrab Fixed Rectangle" || LastBtnStr == "FrameGrab All Edge" || LastBtnStr == "Grab For MouseClick" || LastBtnStr == "Focus Scan" || LastBtnStr == "Contour Scan")
					{
						GenerateToolbarClickEvent::raise(LastAlignment, LastBtnStr, true);
					}
				}
			}
		}
		else if(CurrentAlignment == "Crosshair")
		{
		}
		else if(CurrentAlignment == "Edit")
		{
		}
		else if(CurrentAlignment == "Tools")
		{
		}
		else if(CurrentAlignment == "RCAD Toolbar")
		{
		}
		else if(CurrentAlignment == "DXF Toolbar")
		{
		}
		else if(CurrentAlignment == "PartProgram" || CurrentAlignment == "Part Program Options")
		{
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0027", ex);
	}
}

void RWrapper::RW_MainInterface::HandleGenerateToolbarClick(System::String^ Alignment, System::String^ BtnName)
{
	try
	{
		GenerateToolbarClickEvent::raise(Alignment, BtnName, true);
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0027a", ex);
	}
}

void RWrapper::RW_MainInterface::HandleGenerateToolbarClick1(System::String^ Alignment, System::String^ BtnName, bool flag)
{
	try
	{
		GenerateToolbarClickEvent::raise(Alignment, BtnName, flag);
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0027a", ex);
	}
}

void RWrapper::RW_MainInterface::GenerateEventToCheckCurrentSM()
{
	try
	{
		switch(MAINDllOBJECT->SmartMeasureType)
		{
			case RapidEnums::SMARTMEASUREMENTMODE::ANGLEMEASUREMENT:
				GenerateToolbarBtnCheckEvent::raise("Measure", "Angle", true);
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::SPLINEMEASUREMENT:
				GenerateToolbarBtnCheckEvent::raise("Measure", "SplineMeasure", true);
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::ANGLEMEASUREWITH_XAXIS:
				GenerateToolbarBtnCheckEvent::raise("Measure", "Angle_X_Axis", true);
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::ANGLEMEASUREWITH_YAXIS:
				GenerateToolbarBtnCheckEvent::raise("Measure", "Angle_Y_Axis", true);
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::RECANGLEMEASUREMENT:
				GenerateToolbarBtnCheckEvent::raise("Measure", "Rake Angle", true);
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::RADIUSMEASUREMENT:
				GenerateToolbarBtnCheckEvent::raise("Measure", "Radius", true);
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::DIAMEASUREMENT:
				GenerateToolbarBtnCheckEvent::raise("Measure", "Diameter", true);
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::CHAMFERMEASUREMENT_ARC:
				GenerateToolbarBtnCheckEvent::raise("Measure", "Chamfer_Arc", true);
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::CHAMFERMEASUREMENT_LINE:
				GenerateToolbarBtnCheckEvent::raise("Measure", "Chamfer_Line", true);
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::STEPMEASUREMENT_3LINES:
				GenerateToolbarBtnCheckEvent::raise("Measure", "StepLength1", true);
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::STEPMEASUREMENT_4LINES:
				GenerateToolbarBtnCheckEvent::raise("Measure", "StepLength3", true);
				break;
			case RapidEnums::SMARTMEASUREMENTMODE::POINTANGLE:
				GenerateToolbarBtnCheckEvent::raise("Measure", "Point Angle", true);
				break;

		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0027a", ex);
	}
}

void RWrapper::RW_MainInterface::CallClearRadioButttons()
{
	ClearAllRadioButtonCheckedProperty::raise();
}

void RWrapper::RW_MainInterface::SaveCurrentClickedButton(System::String^ Alignment, System::String^ BtnStr)
{
	try
	{
		System::String^ TempLastSelectedToolbar = LastSelectedToolbar;
		System::String^ TempLastSelectedButton = LastSelectedButton;
		LastSelectedToolbar = Alignment;
		LastSelectedButton = BtnStr;
		//HandleRaiseToolBarClickevent(TempLastSelectedToolbar, TempLastSelectedButton, Alignment, BtnStr);
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0027", ex);
	}
}

void RWrapper::RW_MainInterface::ClearLastSelectedButton()
{
	LastSelectedToolbar = "";
	LastSelectedButton = "";
}


void RWrapper::RW_MainInterface::OnVideoLeftMouseDown(double x, double y)
{
	try
	{
		RW_FixedGraph::MYINSTANCE()->currentFixedGraph = FixedGraphBox::DEFALUT;
		if(RW_FixedGraph::MYINSTANCE()->NudgeFixedGraphics)
		{
			RW_FixedGraph::MYINSTANCE()->NudgeFixedGraphics = false;
			RW_FixedGraph::MYINSTANCE()->ClearCurrentSelectedShape();
		}
		else
			MAINDllOBJECT->OnLeftMouseDown_Video(x, y);
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0035", ex);
	}
}

void RWrapper::RW_MainInterface::OnVideoRightMouseDown(double x, double y)
{
	try
	{
		MAINDllOBJECT->OnRightMouseDown_Video(x, y);
		if(MAINDllOBJECT->GetClickDone() == 0 && MAINDllOBJECT->SmartMeasureType == RapidEnums::SMARTMEASUREMENTMODE::DEFAULTTYPE)
		{
			if(MAINDllOBJECT->getSelectedShapesList().count() > 0)
			{
				if(MAINDllOBJECT->highlightedShape() != NULL && MAINDllOBJECT->highlightedShape()->selected())
					RightClickedOnEntity::raise(1, "RightClickOnShape");
			}
			/*if(MAINDllOBJECT->highlightedShape() != NULL)
			{
				if(!MAINDllOBJECT->highlightedShape()->selected())
					MAINDllOBJECT->selectShape(MAINDllOBJECT->highlightedShape()->getId(), false);
				RightClickedOnShape::raise();
			}*/
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0036", ex);
	}
}

void RWrapper::RW_MainInterface::OnVideoMiddleMouseDown(double x, double y)
{
	MAINDllOBJECT->OnMiddleMouseDown_Video(x, y);
}

void RWrapper::RW_MainInterface::OnVideoLeftMouseUp(double x, double y)
{
	MAINDllOBJECT->OnLeftMouseUp_Video(x, y);
}

void RWrapper::RW_MainInterface::OnVideoRightMouseUp(double x, double y)
{
	MAINDllOBJECT->OnRightMouseUp_Video(x, y);
}

void RWrapper::RW_MainInterface::OnVideoMiddleMouseUp(double x, double y)
{
	MAINDllOBJECT->OnMiddleMouseUp_Video(x, y);
}

void RWrapper::RW_MainInterface::OnVideoMouseMove(double x, double y)
{
	try
	{
		if(MAINDllOBJECT->SaveImageFlag) return;
		if(RW_FixedGraph::MYINSTANCE()->currentFixedGraph != FixedGraphBox::DEFALUT && RW_FixedGraph::MYINSTANCE()->NudgeFixedGraphics)
		{
			RW_FixedGraph::MYINSTANCE()->NudgeFixedShape(x, y);
		}
		else
		{
			//if(MAINDllOBJECT->VideoMouseFlag) return; //Wait untill the 1st mouse move completes //Modified on 11/04/2012 Sathya
			MAINDllOBJECT->VideoMouseFlag = true;
			MAINDllOBJECT->OnMouseMove_Video(x, y);		
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0037", ex);
	}
}

void RWrapper::RW_MainInterface::OnVideoMouseWheel(int evalue)

{
	try
	{
		if(RW_FixedGraph::MYINSTANCE()->currentFixedGraph != FixedGraphBox::DEFALUT)
			FIXEDTOOLOBJECT->ChangeFixedTextSize(evalue > 0? true:false);
		else
			MAINDllOBJECT->OnMouseWheel_Video(evalue);
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0037", ex);
	}
}

void RWrapper::RW_MainInterface::OnVideoMouseEnter()
{
	MAINDllOBJECT->OnMouseEnter_Video();
}

void RWrapper::RW_MainInterface::OnVideoMouseLeave()
{
	MAINDllOBJECT->OnMouseLeave_Video();
}

void RWrapper::RW_MainInterface::OnRCadLeftMouseDown(double x, double y)
{	
	MAINDllOBJECT->OnLeftMouseDown_Rcad(x, y);
	//MAINDllOBJECT->UpdateCamera_ZoomLevel(2,2);
	
	//if(MAINDllOBJECT->PlaceTable)
	//CALIBRATIONTEXT->mousedownTable = true;
	//if(MAINDllOBJECT->PlaceDetail)
	//CALIBRATIONTEXT->mousedownDetail = true;
}

void RWrapper::RW_MainInterface::OnRCadDoubleLeftMouseDown(double x, double y)
{
	try
	{
		 if(MAINDllOBJECT->highlightedmeasure() != NULL)
			DoubleClickedOnEntity::raise(1);
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0038a", ex);
	}
}

void RWrapper::RW_MainInterface::OnRCadRightMouseDown(double x, double y)
{
	try
	{
		MAINDllOBJECT->OnRightMouseDown_Rcad(x, y);
		if(MAINDllOBJECT->highlightedmeasure() != NULL)
			RightClickedOnEntity::raise(2, "RightClickOnMeasure");
		else if((MAINDllOBJECT->highlightedShape() != NULL || MAINDllOBJECT->highlightedPoint() != NULL) && MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::MEASURE_HANDLER)
		{
			if(MAINDllOBJECT->highlightedShape() != NULL)
			{
				RightClickOnShapeInMeasureMode(MAINDllOBJECT->highlightedShape());
			}
			else
				RightClickedOnEntity::raise(2, "RightClickOnPoint");
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0038", ex);
	}
}

void RWrapper::RW_MainInterface::OnRCadMiddleMouseDown(double x, double y)
{
	MAINDllOBJECT->OnMiddleMouseDown_Rcad(x, y);
}

void RWrapper::RW_MainInterface::OnRCadLeftMouseUp(double x, double y)
{
	MAINDllOBJECT->OnLeftMouseUp_Rcad(x, y);
}

void RWrapper::RW_MainInterface::OnRCadRightMouseUp(double x, double y)
{
	MAINDllOBJECT->OnRightMouseUp_Rcad(x, y);
}

void RWrapper::RW_MainInterface::OnRCadMiddleMouseUp(double x, double y)
{
	MAINDllOBJECT->OnMiddleMouseUp_Rcad(x, y);
}

void RWrapper::RW_MainInterface::OnRCadMouseMove(double x, double y)
{
	try
	{
		if (MAINDllOBJECT->SaveImageFlag) return;
		if(MAINDllOBJECT->RCADMouseFlag) return;
		MAINDllOBJECT->RCADMouseFlag = true;
		MAINDllOBJECT->OnMouseMove_Rcad(x, y);
		RW_MainInterface::MYINSTANCE()->RCadMousePos[0] = MAINDllOBJECT->getCurrentUCS().getWindow(1).camx;
		RW_MainInterface::MYINSTANCE()->RCadMousePos[1] = MAINDllOBJECT->getCurrentUCS().getWindow(1).camy;
	}
	catch (Exception^ ex)
	{
		WriteErrorLog("WRMAIN0050", ex);
	}

}

void RWrapper::RW_MainInterface::OnRCadMouseWheel(int evalue)
{
	MAINDllOBJECT->OnMouseWheel_Rcad(evalue);
}

void RWrapper::RW_MainInterface::OnRCadMouseEnter()
{
	MAINDllOBJECT->OnMouseEnter_Rcad();
}

void RWrapper::RW_MainInterface::OnRCadMouseLeave()
{
	MAINDllOBJECT->OnMouseLeave_Rcad();
}




void RWrapper::RW_MainInterface::OnDXFLeftMouseDown(double x, double y)
{
	MAINDllOBJECT->OnLeftMouseDown_Dxf(x, y);
}

void RWrapper::RW_MainInterface::OnDXFRightMouseDown(double x, double y)
{
	MAINDllOBJECT->OnRightMouseDown_Dxf(x, y);
}

void RWrapper::RW_MainInterface::OnDXFMiddleMouseDown(double x, double y)
{
	MAINDllOBJECT->OnMiddleMouseDown_Dxf(x, y);
}

void RWrapper::RW_MainInterface::OnDXFLeftMouseUp(double x, double y)
{
	MAINDllOBJECT->OnLeftMouseUp_Dxf(x, y);
}

void RWrapper::RW_MainInterface::OnDXFRightMouseUp(double x, double y)
{
	MAINDllOBJECT->OnRightMouseUp_Dxf(x, y);
}

void RWrapper::RW_MainInterface::OnDXFMiddleMouseUp(double x, double y)
{
	MAINDllOBJECT->OnMiddleMouseUp_Dxf(x, y);
}

void RWrapper::RW_MainInterface::OnDXFMouseMove(double x, double y)
{
	//if(MAINDllOBJECT->DXFMouseFlag) return;
	MAINDllOBJECT->DXFMouseFlag = true;
	MAINDllOBJECT->OnMouseMove_Dxf(x, y);
}

void RWrapper::RW_MainInterface::OnDXFMouseWheel(int evalue)
{
	MAINDllOBJECT->OnMouseWheel_Dxf(evalue);
}

void RWrapper::RW_MainInterface::OnDXFMouseEnter()
{
	MAINDllOBJECT->OnMouseEnter_Dxf();
}

void RWrapper::RW_MainInterface::OnDXFMouseLeave()
{
	MAINDllOBJECT->OnMouseLeave_Dxf();
}



void RWrapper::RW_MainInterface::OnPartprogramLeftMouseDown(double x, double y)
{
	//MAINDllOBJECT->OnLeftMouseDown_Partprogram(x, y);
}

void RWrapper::RW_MainInterface::OnPartprogramRightMouseDown(double x, double y)
{
	//MAINDllOBJECT->OnRightMouseDown_Partprogram(x, y);
}

void RWrapper::RW_MainInterface::OnPartprogramMiddleMouseDown(double x, double y)
{
	//MAINDllOBJECT->OnMiddleMouseDown_Partprogram(x, y);
}

void RWrapper::RW_MainInterface::OnPartprogramLeftMouseUp(double x, double y)
{
	//MAINDllOBJECT->OnLeftMouseUp_Partprogram(x, y);
}

void RWrapper::RW_MainInterface::OnPartprogramRightMouseUp(double x, double y)
{
	//MAINDllOBJECT->OnRightMouseUp_Partprogram(x, y);
}

void RWrapper::RW_MainInterface::OnPartprogramMiddleMouseUp(double x, double y)
{
	//MAINDllOBJECT->OnMiddleMouseUp_Partprogram(x, y);
}

void RWrapper::RW_MainInterface::OnPartprogramMouseMove(double x, double y)
{
	//MAINDllOBJECT->OnMouseMove_Partprogram(x, y);
}

void RWrapper::RW_MainInterface::OnPartprogramMouseWheel(int evalue)
{
	//MAINDllOBJECT->OnMouseWheel_Partprogram(evalue);
}

void RWrapper::RW_MainInterface::OnPartprogramMouseEnter()
{
	//MAINDllOBJECT->OnMouseEnter_Partprogram();
}

void RWrapper::RW_MainInterface::OnPartprogramMouseLeave()
{
	//MAINDllOBJECT->OnMouseLeave_Partprogram();
}

void RWrapper::RW_MainInterface::Update_FramGrab(System::String^ Alignment, System::String^ btnName, bool btnclick)
 {
	 try
	 {
		 GenerateToolbarClickEvent::raise(Alignment, btnName, btnclick);
	 }
	 catch(Exception^ ex)
	 {
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0001a", ex);
	 }
}


void RWrapper::RW_MainInterface::Update_HelpStatusMessage(System::String^ Str, bool Insert_TempStrFlag, System::String^ TempStr)
{
	try
	{
		DisplayStatusMessage::raise(Str, Insert_TempStrFlag, TempStr);
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0001a", ex);
	}
}

void RWrapper::RW_MainInterface::ShowWaitStatusMessage(System::String^ SMessage, int startafter, bool ShowCancelButton)
{
	try
	{
		WaitStatusMessage = SMessage; AllowCancelofStatusTask = ShowCancelButton;
		StartWaitMessageTimer->Interval = startafter;
		StartWaitMessageTimer->Enabled = true;
	}
	catch(Exception^ ex)
	{
	}
}

void RWrapper::RW_MainInterface::StartHideWaitStatusMessage(int hideafter)
{
	try
	{
		StopWaitMessageTimer->Interval = hideafter;
		StopWaitMessageTimer->Enabled = true;
	}
	catch(Exception^ ex)
	{
	}
}

void RWrapper::RW_MainInterface::HideWaitMessagePanel(System::Object^ sender, System::EventArgs^ e)
{
	try
	{
		StopWaitMessageTimer->Enabled = false;
		WaitStatusMessage = "";
		ShowHideStatusWindow::raise(RWrapper::RW_MainInterface::MYINSTANCE()->WaitStatusMessage, false, false);
	}
	catch(Exception^ ex)
	{
	}
}


bool RWrapper::RW_MainInterface::Update_MsgBoxStatus(System::String^ MessageText, RWrapper::RW_Enum::RW_MSGBOXTYPE btnType, RWrapper::RW_Enum::RW_MSGBOXICONTYPE icon, bool Insert_TempStrFlag, System::String^ TempStr)
{
	try
	{
		return ShowMsgBoxText::raise(MessageText, btnType, icon, Insert_TempStrFlag, TempStr);
	}
	catch(Exception^ ex)
	{
	}
}

void RWrapper::RW_MainInterface::ShowWaitMessagePanel(System::Object^ sender, System::EventArgs^ e)
{
	try
	{
		StartWaitMessageTimer->Enabled = false;
		ShowHideStatusWindow::raise(RWrapper::RW_MainInterface::MYINSTANCE()->WaitStatusMessage, true, AllowCancelofStatusTask);
	}
	catch(Exception^ ex)
	{
	}
}


 void RWrapper::RW_MainInterface::GetLineArcDiffShapes(double tolerance, double max_radius, double min_radius, double min_angle_cutoff, double noise, double max_dist_betn_pts, bool closed_loop, bool shape_as_fast_trace, bool join_all_pts)
 {
	 try
	{
	   MAINDllOBJECT->CallCalculateDiffShapes(tolerance / 1000, max_radius, min_radius, min_angle_cutoff, noise / 1000, max_dist_betn_pts, closed_loop, shape_as_fast_trace, join_all_pts);
	}
	 catch(Exception^ ex){}
 }
 void RWrapper::RW_MainInterface::AddLineArcDiffShapes()
 {
	 try
	{
	  MAINDllOBJECT->CallAddShapes();
	}
	 catch(Exception^ ex){}
 }

 void RWrapper::RW_MainInterface::CancelLineArc()
  {
	try
	{
	   MAINDllOBJECT->ClearLineArcShapes();
	}
	catch(Exception^ ex){}
 }



void RWrapper::RW_MainInterface::MaximizeWindow(int windowno, int width, int height)
 {
	 try
	 {
		 WriteUserLog("MaximizeWindow", "Maximize Window", "WindowNo " +  windowno.ToString());
		 if(windowno == 1)
			 window1_State = !window1_State; 
		 else if(windowno == 2)
			 window2_State = !window2_State;
		 for(RC_ITER item = MAINDllOBJECT->getUCSList().getList().begin(); item != MAINDllOBJECT->getUCSList().getList().end(); item++)
		 {
			UCS* Cucs = (UCS*)((*item).second);
			Cucs->getWindow(windowno).ResizeWindow(width, height);
			if(windowno == 1)
				Cucs->getWindow(1).maxed(window1_State);
			else if(windowno == 2)
				Cucs->getWindow(2).maxed(window2_State);
		 }
		 MAINDllOBJECT->ZoomToWindowExtents(windowno);
	 }
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0041", ex);
	}
 }

void RWrapper::RW_MainInterface::ResizetheWindow(int windowno, int width, int height)
 {
	 try
	 {
		 WriteUserLog("ResizetheWindow", "Change window size","Window no." + windowno.ToString());
		 for(RC_ITER item = MAINDllOBJECT->getUCSList().getList().begin(); item != MAINDllOBJECT->getUCSList().getList().end(); item++)
		 {
			UCS* Cucs = (UCS*)((*item).second);
			Cucs->getWindow(windowno).ResizeWindow(width, height, false);
		 }
		 MAINDllOBJECT->ZoomToWindowExtents(windowno);
	 }
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0042", ex);
	}
 }

void RWrapper::RW_MainInterface::ToggleShowHideWindow(int windowno, bool Hidewindow)
{
	try
	{
		 WriteUserLog("ToggleShowHideWindow", "Toggle between windows","Window no." + windowno.ToString() + "Hide" + Hidewindow);
		 for(RC_ITER item = MAINDllOBJECT->getUCSList().getList().begin(); item != MAINDllOBJECT->getUCSList().getList().end(); item++)
		 {
			UCS* Cucs = (UCS*)((*item).second);
			Cucs->getWindow(windowno).WindowHidden(Hidewindow);
		 }
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0042", ex);
	}
}

void RWrapper::RW_MainInterface::ToggleSnapeMode()
{
	bool dddd = MAINDllOBJECT->ShowExplicitIntersectionPoints();
	MAINDllOBJECT->ShowExplicitIntersectionPoints.Toggle();
	dddd = MAINDllOBJECT->ShowExplicitIntersectionPoints();
}

void RWrapper::RW_MainInterface::ToggleVSnapeMode()
{
	bool dddd = MAINDllOBJECT->ShowImplicitIntersectionPoints();
	MAINDllOBJECT->ShowImplicitIntersectionPoints.Toggle();
	dddd = MAINDllOBJECT->ShowImplicitIntersectionPoints();
}

void RWrapper::RW_MainInterface::SetAlignmentCorrectionValue(cli::array<System::Double, 1>^ CorrectionlistX, cli::array<System::Double, 1>^ CorrectionlistY, cli::array<System::Double, 1>^ DistList)
{
	try
	{
		 std::list<double> LCorrectionX;
		 std::list<double> LCorrectionY;
		 std::list<double> distCorrection;
		 for each(System::Double^ Cvalue in CorrectionlistX)
		 {
			  double temp = (double)Cvalue;
			  LCorrectionX.push_back(temp);
		 }
		 for each(System::Double^ Cvalue in CorrectionlistY)
		 {
			  double temp = (double)Cvalue;
			  LCorrectionY.push_back(temp);
		 }
		  for each(System::Double^ Cvalue in DistList)
		 {
			  double temp = (double)Cvalue;
			  distCorrection.push_back(temp);
		 }
		DXFEXPOSEOBJECT->AutoAlignmentForDxf(&LCorrectionX, &LCorrectionY, &distCorrection);
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0043", ex);
	}
}

void RWrapper::RW_MainInterface::SetDerivedShapeParameters(double offset, double dist, double Llength)
{
	if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
	{
		offset = offset * 25.4; dist = dist * 25.4;
		Llength = Llength * 25.4;
	}
	MAINDllOBJECT->SetDerivedShapeParam(offset, dist, Llength);
}

void RWrapper::RW_MainInterface::SetUCSRotateAngle(double angle)
{
	MAINDllOBJECT->SetUCSRotateAngle(angle);
}

void RWrapper::RW_MainInterface::SetTruePos(double x, double y, int i, bool PolarCord, bool forPoint)
{
	MAINDllOBJECT->TruePositionMode = false;
	if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
	{
		x = x * 25.4;
		if(!PolarCord)
			y = y * 25.4;
	}
	if(PolarCord)
	{
		double X = x * System::Math::Cos(y * System::Math::PI / 180);
		double Y = x * System::Math::Sin(y * System::Math::PI / 180);
		MAINDllOBJECT->SetTruePostion(X, Y, i, forPoint);
	}
	else
		MAINDllOBJECT->SetTruePostion(x, y, i, forPoint);
}

void RWrapper::RW_MainInterface::Set3DTruePos(double x, double y, int i, bool forPoint)
{
	MAINDllOBJECT->TruePositionMode = false;
	if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
	{
		x = x * 25.4;
		y = y * 25.4;
	}
	MAINDllOBJECT->SetTruePostion3D(x, y, i, forPoint);
}

void RWrapper::RW_MainInterface::CancelPressed()
{
	MAINDllOBJECT->HandleCancelPressed();
}

void RWrapper::RW_MainInterface::MM_DropDown_clicked(int mode)
{
	MAINDllOBJECT->SetMeasurementUnit(RapidEnums::RAPIDUNITS(mode));
	RW_FixedGraph::MYINSTANCE()->ClearCurrentSelectedShape();
}

void RWrapper::RW_MainInterface::SetMagnification(System::String^ Str)
{
	try
	{
		if ((MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::PROBE_ONLY || MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HOBCHECKER))
			return;

		WriteUserLog("SetMagnification", "Magnification change","Selected " + Str);
		if (!IsMultiLevelZoom)
		{
			RWrapper::RW_DBSettings::MYINSTANCE()->UpdateVideoPixelWidth(Str);
			RW_DRO::MYINSTANCE()->UpdateCenterScreen();
		}
		if(MachineConnectedStatus && RWrapper::RW_DBSettings::MYINSTANCE()->AutoZoomInOutMode)
		{
			if (!PPCALCOBJECT->IsPartProgramRunning())
				RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::AUTO_ZOOMINOUTMODE);

			if(IsMultiLevelZoom)
			{
				RWrapper::RW_DBSettings::MYINSTANCE()->UpdateCameraZoomLevel(Str);
				StartHideWaitStatusMessage(DealyinSec * 1000);
				ShowWaitStatusMessage("Please wait... Camera Zooming To " + Str, 10, false);
				DROInstance->ResetLookingSymbol();
			}
			else
			{
				if((Str == "11X" && LastSelectedMagLevel == "67X") || (Str == "22X" && LastSelectedMagLevel == "134X"))
					DROInstance->sendMachineCommand_One((char*)Marshal::StringToHGlobalAnsi("F19").ToPointer(), true);
				else if ((Str == "67X" && LastSelectedMagLevel == "11X") || (Str == "134X" && LastSelectedMagLevel == "22X"))
				//else if(Str == "67X")
					DROInstance->sendMachineCommand_One((char*)Marshal::StringToHGlobalAnsi("F29").ToPointer(), true);
				if(Str == "11X" || Str == "67X" || Str == "22X" || Str == "134X")
				{
					StartHideWaitStatusMessage(DealyinSec * 1000); //Hide wait after delay seconds..
					ShowWaitStatusMessage("Please wait... Camera Zooming To " + Str, 10, false);
					DROInstance->ResetLookingSymbol();
				}
			}
		}
		LastSelectedMagLevel = Str;
		MAINDllOBJECT->LastSelectedMagLevel = (char*)Marshal::StringToHGlobalAnsi(Str).ToPointer();
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0048", ex);
	}
}

void RWrapper::RW_MainInterface::SetCameraToCompleteZoom_Exeload()
{
	try
	{
		if(MachineConnectedStatus && IsMultiLevelZoom)
		{
			double target[4] = { RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], 
									RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], -290 };
			double feedrate[4] = {5, 5, 5, 5};
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
			Sleep(30);
			DROInstance->ResetLookingSymbol();
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::AUTO_ZOOMINOUTDURINGEXELOAD);
			StartHideWaitStatusMessage(DealyinSec * 1000);
			ShowWaitStatusMessage("Please wait... Camera Zooming Out ", 10, false);
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0048", ex);
	}
}

void RWrapper::RW_MainInterface::RefreshDRO()
{
	 try
	 {
		 RWrapper::RW_DRO::MYINSTANCE()->UpdateDRO_textbox();
		 RWrapper::RW_DRO::MYINSTANCE()->UpdateCenterScreen();
	 }
	 catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0047", ex);
	}
}

void RWrapper::RW_MainInterface::SetProgramHomePostion()
{
	try
	{
		PPCALCOBJECT->HomePosition.set(MAINDllOBJECT->getCurrentDRO().getX(),MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ());
	}
	catch(Exception^ ex)
	{
	}
}

bool RWrapper::RW_MainInterface::IsPartprogramRunning()
{
	 bool flag = PPCALCOBJECT->IsPartProgramRunning();
	 return flag;
} 

int RWrapper::RW_MainInterface::SetUserChosenSurfaceED(System::String^ Str)
{
	return MAINDllOBJECT->SetSurfaceEDType((char*)Marshal::StringToHGlobalAnsi(Str).ToPointer());
}

void RWrapper::RW_MainInterface::HighlightUserChosenSurfaceED(System::String^ Str)
{
	MAINDllOBJECT->HighlightSurfaceFGShape((char*)Marshal::StringToHGlobalAnsi(Str).ToPointer());
}

void RWrapper::RW_MainInterface::SetEntityTransparency(double Alpha_value)
{
	MAINDllOBJECT->SetRapidShapeTransparency(Alpha_value);
}

void RWrapper::RW_MainInterface::setCoordinateListForAutomeasurement(System::Collections::Generic::List<System::Double>^ listcoord)
{
	 /*std::list<double> coordinateList;
	 for each(System::Double^ Cvalue in listcoord)
	 {
		  double temp = (double)Cvalue;
		  coordinateList.push_back(temp);
	 }
	 HELPEROBJECT->AutoMeasurementFor_IdenticalShapes(&coordinateList);*/
}

 //void RWrapper::RW_MainInterface::setCoordinate_ProbeMeasurement(System::Collections::Generic::List<System::Double>^ CoordList, System::Collections::Generic::List<System::String^>^ NameList)
 //{
	// try
	// {
	//	 std::list<std::string> nameListType;
	//	 std::list<double> coordinateList;
	//	 for each(System::Double^ Cvalue in CoordList)
	//	 {
	//		 double temp = (double)Cvalue;
	//         coordinateList.push_back(temp);
	//	 }
	//	 for each(System::String^ Cname in NameList)
	//	 {
	//		 std::string temp;
	//		 const char* tempchar = (const char*)(Marshal::StringToHGlobalAnsi(Cname)).ToPointer();
	//		 temp = tempchar;
	//		 nameListType.push_back(temp);
	//	 }
	// }
	// catch(Exception^ ex)
	// {
	//	 WriteErrorLog("WRMAIN0021", ex);
	// }
 //}

  void RWrapper::RW_MainInterface::setCoordinate_ProbeMeasurement(System::Collections::Generic::List<RWrapper::RW_Probe_Entity^>^ ExcelList, System::Collections::ArrayList^ Comp_ArrayList)
{
	try
	{
		ProbeScanDetais_ArrayList->Clear();
		ProbeScanDetais_ArrayList = Comp_ArrayList;
		double Feed_X = RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], Feed_Y = RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], Feed_Z = RWrapper::RW_DRO::MYINSTANCE()->feedRate[2];
		double Retaract_Dist = 2, Retract_Speed = 1, Approach_Dist = MAINDllOBJECT->ProbeRadius() + 2, Approach_Speed = 1;
		ProbePathList = gcnew  System::Collections::Generic::List<RWrapper::RW_Probe_Path_Entity^>();
		RWrapper::RW_Probe_Path_Entity^ ProbePath_LstEntity;
		double CurrentDro[3] = {MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ()};
		
		ProbePath_LstEntity = gcnew RW_Probe_Path_Entity();
		ProbePath_LstEntity->Pt_X =  CurrentDro[0];
		ProbePath_LstEntity->Pt_Y =  CurrentDro[1];
		ProbePath_LstEntity->Pt_Z =  CurrentDro[2];
		ProbePath_LstEntity->Feed_X = RWrapper::RW_DRO::MYINSTANCE()->feedRate[0];
		ProbePath_LstEntity->Feed_Y = RWrapper::RW_DRO::MYINSTANCE()->feedRate[1];
		ProbePath_LstEntity->Feed_Z = RWrapper::RW_DRO::MYINSTANCE()->feedRate[2];
		ProbePath_LstEntity->PathPt = true;	 
		ProbePathList->Add(ProbePath_LstEntity);

		for each(RWrapper::RW_Probe_Entity^ LstEntity in ExcelList)
		{
			std::string PtType;
			const char* tempchar = (const char*)(Marshal::StringToHGlobalAnsi(LstEntity->PtType)).ToPointer();
			PtType = tempchar;
			if(PtType == "P" || PtType == "p")
			{		
				ProbePath_LstEntity = gcnew RW_Probe_Path_Entity();
				ProbePath_LstEntity->Pt_X = LstEntity->Val1 + CurrentDro[0];
				ProbePath_LstEntity->Pt_Y = LstEntity->Val2 + CurrentDro[1];
				ProbePath_LstEntity->Pt_Z = LstEntity->Val3 + CurrentDro[2];
				ProbePath_LstEntity->Feed_X = RWrapper::RW_DRO::MYINSTANCE()->feedRate[0];
				ProbePath_LstEntity->Feed_Y = RWrapper::RW_DRO::MYINSTANCE()->feedRate[1];
				ProbePath_LstEntity->Feed_Z = RWrapper::RW_DRO::MYINSTANCE()->feedRate[2];
				ProbePath_LstEntity->PathPt = true;
				ProbePathList->Add(ProbePath_LstEntity);
			}
			else if(PtType == "T" || PtType == "t")
			{
				RWrapper::RW_Probe_Path_Entity^ StartPt;
				StartPt = ProbePathList[ProbePathList->Count-1];
				double ApproachPt[6];
				double StPoint[3] = {StartPt->Pt_X, StartPt->Pt_Y, StartPt->Pt_Z};
				double endPt[3] = {LstEntity->Val1 + CurrentDro[0], LstEntity->Val2 + CurrentDro[1], LstEntity->Val3 + CurrentDro[2]};
				HELPEROBJECT->getApproachDistPoint(StPoint, endPt, Approach_Dist, ApproachPt);

				ProbePath_LstEntity = gcnew RW_Probe_Path_Entity();
				ProbePath_LstEntity->Pt_X = ApproachPt[0];
				ProbePath_LstEntity->Pt_Y = ApproachPt[1];
				ProbePath_LstEntity->Pt_Z = ApproachPt[2];
				ProbePath_LstEntity->Feed_X = RWrapper::RW_DRO::MYINSTANCE()->feedRate[0];
				ProbePath_LstEntity->Feed_Y = RWrapper::RW_DRO::MYINSTANCE()->feedRate[1];
				ProbePath_LstEntity->Feed_Z = RWrapper::RW_DRO::MYINSTANCE()->feedRate[2];
				ProbePath_LstEntity->PathPt = true;
				ProbePathList->Add(ProbePath_LstEntity);

				ProbePath_LstEntity = gcnew RW_Probe_Path_Entity();
				ProbePath_LstEntity->Pt_X = ApproachPt[3];
				ProbePath_LstEntity->Pt_Y = ApproachPt[4];
				ProbePath_LstEntity->Pt_Z = ApproachPt[5];
				ProbePath_LstEntity->Feed_X = RWrapper::RW_DRO::MYINSTANCE()->feedRate[0];
				ProbePath_LstEntity->Feed_Y = RWrapper::RW_DRO::MYINSTANCE()->feedRate[1];
				ProbePath_LstEntity->Feed_Z = RWrapper::RW_DRO::MYINSTANCE()->feedRate[2];
				ProbePath_LstEntity->PathPt = false;
				ProbePathList->Add(ProbePath_LstEntity);			
			}
			/*else if(PtType == "R" || PtType == "r")
			{
				Retaract_Dist = LstEntity->Val1;
				Retract_Speed = LstEntity->Val2;
			}
			else if(PtType == "A" || PtType == "a")
			{
				Approach_Dist = LstEntity->Val1;
				Approach_Speed = LstEntity->Val2;
			}*/
		 }
		ProbeGotoRequiredPosition_GetProbePts();
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0021", ex);
	}
}

void RWrapper::RW_MainInterface::setActionIdForPPEdit(int ActionId, bool selected)
 {
	 try
	 {
		 if(selected)
			 MAINDllOBJECT->AddAction_PPEditMode = true;
		 else
			 MAINDllOBJECT->AddAction_PPEditMode = false;
		 MAINDllOBJECT->LastActionIdForPPEdit = ActionId;
	 }
	 catch(Exception^ ex)
	 {
		 WriteErrorLog("WRMAIN0021a", ex);
	 }
}

 void RWrapper::RW_MainInterface::SetNudge_RotateForRCad(bool IsNudge, bool IsRotate)
{
	 try
	 {
		 MAINDllOBJECT->SetStatusCode("HandleDXFToolbar_Click07");
		 MAINDllOBJECT->RotateDXFflag = false;
		 MAINDllOBJECT->NudgeToolflag = false;
		 if(IsNudge)
		 {
			 MAINDllOBJECT->NudgeToolflag = true;
			 MAINDllOBJECT->SetCrossOnVideo(false);
			 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::MOVE_HANDLER);
			 MAINDllOBJECT->SetShapeCollectionForNudge(false);
		 }
		 else if(IsRotate)
		 {
			 MAINDllOBJECT->RotateDXFflag = true;
			 MAINDllOBJECT->SetCrossOnVideo(false);
			 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::MOVE_HANDLER);
			 MAINDllOBJECT->SetShapeCollectionForNudge(false);
		 }
		 else
			 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
	 }
	 catch(Exception^ ex)
	 {
		 WriteErrorLog("WRMAIN0021b", ex);
	 }
}

 void RWrapper::RW_MainInterface::SetNudge_RotateForDxf(bool IsNudge, bool IsRotate)
 {
	 try
	 {
		 MAINDllOBJECT->SetStatusCode("HandleDXFToolbar_Click07");
		 MAINDllOBJECT->RotateDXFflag = false;
		 MAINDllOBJECT->NudgeToolflag = false;
		 if(IsNudge)
		 {
			 MAINDllOBJECT->NudgeToolflag = true;
			 MAINDllOBJECT->SetCrossOnVideo(false);
			 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::MOVE_HANDLER);
			 MAINDllOBJECT->SetShapeCollectionForNudge(true);
		 }
		 else if(IsRotate)
		 {
			 MAINDllOBJECT->RotateDXFflag = true;
			 MAINDllOBJECT->SetCrossOnVideo(false);
			 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::MOVE_HANDLER);
			 MAINDllOBJECT->SetShapeCollectionForNudge(true);
		 }
		 else
			 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
	 }
	 catch(Exception^ ex)
	 {
		 WriteErrorLog("WRMAIN0021c", ex);
	 }
}

void RWrapper::RW_MainInterface::AutoAlignmentForSelectedentities(bool SelectedShapesOnly, int trans_Rotate)
 {
	 try
	 {
		 if(SelectedShapesOnly)
		 {
			 if(trans_Rotate == 0)
				 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::SELECTED_TRANSFORM_HANDLER);
			 else if(trans_Rotate == 1)
				 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::SELECTED_TRANSLATE_HANDLER);
			  else if(trans_Rotate == 2)
				 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::SELECTED_ROTATE_HANDLER);
		 }
		 else
		 {
			 if(trans_Rotate == 0)
				 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::TRANSFORM_HANDLER);
			 else if(trans_Rotate == 1)
				 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::TRANSLATE_HANDLER);
			 else if(trans_Rotate == 2)
				 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ROTATE_HANDLER);
		 }
	 }
	 catch(Exception^ ex)
	 {
		 WriteErrorLog("WRMAIN0021c", ex);
	 }
}

void RWrapper::RW_MainInterface::SetOGLFontFamily(System::String^ Str)
{
	 try
	 {
		 GRAFIX->SetFontFamily((char*)Marshal::StringToHGlobalAnsi(Str).ToPointer());
		 MAINDllOBJECT->UpdateShapesChanged(true);
	 }
	 catch(Exception^ ex)
	 {
		 WriteErrorLog("WRMAIN0021d", ex);
	 }
}

void RWrapper::RW_MainInterface::SetOGLGradientBackGround(double r1,double g1, double b1, double r2, double g2, double b2)
{
	 try
	 {
		 GRAFIX->SetBackGroundGradient(r1, g1, b1, r2, g2, b2);
		 MAINDllOBJECT->UpdateShapesChanged(true);
	 }
	 catch(Exception^ ex)
	 {
		 WriteErrorLog("WRMAIN0021e", ex);
	 }
}

void RWrapper::RW_MainInterface::SetPLaceCalibrationDeatil(System::String^ Machine,System::String^ CmpName, System::String^ CalibName, System::String^ CalibDate)
{
	try
	{
		CALIBRATIONTEXT->MachineNo = (char*)Marshal::StringToHGlobalAnsi(Machine).ToPointer();
		CALIBRATIONTEXT->CmpnyName = (char*)Marshal::StringToHGlobalAnsi(CmpName).ToPointer();
		CALIBRATIONTEXT->CalrName = (char*)Marshal::StringToHGlobalAnsi(CalibName).ToPointer();
		CALIBRATIONTEXT->Date = (char*)Marshal::StringToHGlobalAnsi(CalibDate).ToPointer();
		MAINDllOBJECT->PlaceDetail = true;
		MAINDllOBJECT->UpdateShapesChanged(true);
	}
	catch(Exception^ ex)
	{
		 WriteErrorLog("WRMAIN0021f", ex);	
	}
}
void RWrapper::RW_MainInterface::PLaceCalibrationTable()
{
	try
	{
		MAINDllOBJECT->PlaceTable = true;
		 MAINDllOBJECT->UpdateShapesChanged(true);
	}
	catch(Exception^ ex)
	{
		 WriteErrorLog("WRMAIN0021g", ex);
	}
}

void RWrapper::RW_MainInterface::CreateIntersectionPtsWithTriangle(double interval, int Axisno)
{
	try
	{
		for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
		{
			Shape* CShape = (Shape*)((*Shpitem).second);
			if(CShape->ShapeType == RapidEnums::SHAPETYPE::TRIANGULARSURFACE)
				((TriangularSurface*)CShape)->CreateIntersectionPts_Triangle(interval, Axisno);
		}
	}
	catch(Exception^ ex)
	{
		 WriteErrorLog("WRMAIN0021h", ex);
	}
}

void RWrapper::RW_MainInterface::SetOffsetCorrectionflag(bool XOffset, bool YOffset, bool ZOffset, bool ROffset)
{
	try
	{
		Shape *tmpShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		tmpShape->XOffsetCorrection(XOffset);
		tmpShape->YOffsetCorrection(YOffset);
		tmpShape->ZOffsetCorrection(ZOffset);
		if(tmpShape->ShapeType == RapidEnums::SHAPETYPE::RPOINT || tmpShape->ShapeType == RapidEnums::SHAPETYPE::RPOINT3D)
			tmpShape->ROffsetCorrection(ROffset);
	}
	catch(Exception^ ex)
	{
		 WriteErrorLog("WRMAIN0021h", ex);
	}
}

void RWrapper::RW_MainInterface::SetProfileScanPause_StoppedFlag(bool StopFlag)
{	
	if(StopFlag)
		HELPEROBJECT->profile_scan_stopped = true;
	else
		HELPEROBJECT->profile_scan_paused = true;
}

void RWrapper::RW_MainInterface::ContinueProfileScan()
{
	HELPEROBJECT->ProfileScanContinue();
}

void RWrapper::RW_MainInterface::ContinueProfileScan(int jump)
{
	HELPEROBJECT->ProfileScanJumpParam = jump;
	HELPEROBJECT->ProfileScanContinue();
}

void RWrapper::RW_MainInterface::SetFGandCloudPointSize(int PtSize)
{
	 try
	 {
		 GRAFIX->SetFGandCloudPtSize(PtSize);
		 MAINDllOBJECT->UpdateShapesChanged(true);
	 }
	 catch(Exception^ ex)
	 {
		 WriteErrorLog("WRMAIN0021f", ex);
	 }
}

void RWrapper::RW_MainInterface::SetActionForNudgeRotate()
{
	 try
	 {
		 MAINDllOBJECT->SetAction_NudgeRotate();
	 }
	 catch(Exception^ ex)
	 {
		 WriteErrorLog("WRMAIN0021e", ex);
	 }
}

void RWrapper::RW_MainInterface::SetProfileScanParam(int jump)
{
	 try
	 {	
		 HELPEROBJECT->ProfileScanJumpParam = jump;
		 HELPEROBJECT->AddtheProfileScanShape();
	 }
	 catch(Exception^ ex)
	 {
		 WriteErrorLog("WRMAIN0021f", ex);
	 }
}

 void RWrapper::RW_MainInterface::PickProfileScanEndPoint()
 {
	  HELPEROBJECT->PickEndPoint_ProfileScan = true;
 }

void RWrapper::RW_MainInterface::SetProfileScanParam_MultiFeature(int jump)
{
	 try
	 {
		 HELPEROBJECT->MultiFeatureScan = true;
		 HELPEROBJECT->ProfileScanJumpParam = jump;
		 HELPEROBJECT->AddtheProfileScanShape();
	 }
	 catch(Exception^ ex)
	 {
		 WriteErrorLog("WRMAIN0021f", ex);
	 }
}

void RWrapper::RW_MainInterface::ContinueMultiFeatureProfileScan()
{
	 try
	 {
		 MAINDllOBJECT->SetAction_NudgeRotate();
		 MAINDllOBJECT->selectShape(&HELPEROBJECT->MultiFeatureShapeList);
		 RaiseEvents(23);
	 }
	 catch(Exception^ ex)
	 {
		 WriteErrorLog("WRMAIN0021f", ex);
	 }
}

void RWrapper::RW_MainInterface::CreateAreaShape(bool WhiteRegion)
{
	 try
	 {
		 if(WhiteRegion)
			 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::WHITE_AREA_HANDLER);
		 else
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::BLACK_AREA_HANDLER);
		 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB);
	 }
	 catch(Exception^ ex)
	 {
		 WriteErrorLog("WRMAIN0021f", ex);
	 }
}

void RWrapper::RW_MainInterface::CompareCloudPtsWithStlSurface(double tolerance)
{
	try
	{
		for(RC_ITER item = MAINDllOBJECT->getShapesList().getList().begin(); item != MAINDllOBJECT->getShapesList().getList().end(); item++)
		{
			Shape* currentShape = (Shape*)((*item).second);
			if(currentShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
				((CloudPoints*)currentShape)->CompareWithStlSurface(tolerance);
		}
	}
	 catch(Exception^ ex){ WriteErrorLog("WRMAIN0021f", ex); }
}

 void RWrapper::RW_MainInterface::SetColorThresholdValue(double tolerance)
{
	try
	{
		MAINDllOBJECT->ColorThreshold = tolerance;
		MAINDllOBJECT->UpdateShapesChanged();
	}
	 catch(Exception^ ex){ WriteErrorLog("WRMAIN0021g", ex); }
}

void RWrapper::RW_MainInterface::CloudPointComparator(double interval, bool ArcBestFit, int NumberOfPts)
{
	try
	{
		 MAINDllOBJECT->CompareCloudPts(interval, ArcBestFit, NumberOfPts);	
	}
	 catch(Exception^ ex){ WriteErrorLog("WRMAIN0021h", ex); }
}

void RWrapper::RW_MainInterface::PointToCloudPointsMeasurement(double Angle, bool ArcBestFit, int NumberOfpoints)
{
	try
	{
		 MAINDllOBJECT->ComparePoints_CloudPts(Angle, ArcBestFit, NumberOfpoints);	
	}
	catch(Exception^ ex)
	{
		 WriteErrorLog("WRMAIN0021h", ex);
	}
}

void RWrapper::RW_MainInterface::CloudPointComparator(int MeasureCount, bool ArcBestFit, int NumberOfPts)
{
	try
	{
		 MAINDllOBJECT->CompareCloudPts(MeasureCount, ArcBestFit, NumberOfPts);	
	}
	 catch(Exception^ ex){ WriteErrorLog("WRMAIN0021h", ex); }
}

void RWrapper::RW_MainInterface::SetRGBChannelColor(int Channel_Number)
 {
	 MAINDllOBJECT->Color_Channel = Channel_Number;
 }

void RWrapper::RW_MainInterface::CreateCloudPointsMeasurement(System::Collections::ArrayList^ Measure_ArrayList)
 {
		int cnt = Measure_ArrayList->Count;
		std::list<std::string> measureList;
		for(int i = 0; i < cnt; i++)
		{
			System::String^ Str = System::Convert::ToString(Measure_ArrayList[i]);
			measureList.push_back((char*)Marshal::StringToHGlobalAnsi(Str).ToPointer());
		}
		MAINDllOBJECT->CloudPointsMeasure(measureList);
 } 

void RWrapper::RW_MainInterface::SetSurfaceAloType(int type)
{
	HELPEROBJECT->SurfaceAlgoType = type;
}

 void RWrapper::RW_MainInterface::Start_StopRotationalScan(bool StartScan)
 {
	 if (StartScan)
	 {
		 //If we have a rotary axis, let us start rotating the axis to get the highest point 
		 //RapidEnums::RAPIDMACHINETYPE::

	 }
	  MAINDllOBJECT->StartRotationalScan(StartScan);
	  //RWrapper::RW_DRO::MYINSTANCE()->HeightGauge_ScanningModeON = StartScan;
 }

 void RWrapper::RW_MainInterface::SetContinuousScanMode(bool ScanMode)
 {
	  MAINDllOBJECT->Continuous_ScanMode = ScanMode;
 }

 void RWrapper::RW_MainInterface::SetRunoutFlute(int cnt, int RunOut_Type)
 {
	 MAINDllOBJECT->RunoutFluteCount(cnt);
	 MAINDllOBJECT->RunOutType(RunOut_Type);
 }

 void RWrapper::RW_MainInterface::SetRunOutFlute_Radius(int FluteCount, double AxialRadius)
{
	MAINDllOBJECT->RunoutFluteCount(FluteCount);
	MAINDllOBJECT->RunoutAxialRadius(AxialRadius);
	 MAINDllOBJECT->RunOutType(0);
}

  double RWrapper::RW_MainInterface::GetRunOutAxialRadius(System::Boolean% CheckForToolAxis)
  {
	  try
	  {
		  double AxialRadius = 0.0;
		  CheckForToolAxis = false;
		  if(MAINDllOBJECT->Vblock_CylinderAxisLine == NULL) return AxialRadius;
		  AxialRadius = RMATH2DOBJECT->Pt2Line_Dist(RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], ((Line*)MAINDllOBJECT->Vblock_CylinderAxisLine)->Angle(), ((Line*)MAINDllOBJECT->Vblock_CylinderAxisLine)->Intercept());
		  CheckForToolAxis = true;
		  return AxialRadius;
	}
	 catch(Exception^ ex){return 0.0; WriteErrorLog("WRMAIN0021a", ex);}
  }

  void RWrapper::RW_MainInterface::SetFilterStatus(bool flag)
  {
	  MAINDllOBJECT->ApplyFilterCorrection(flag);
  }

  void RWrapper::RW_MainInterface::SetMMCStatus(bool flag)
  {
	  MAINDllOBJECT->OuterCorrection(flag);
  }

	void RWrapper::RW_MainInterface::SetMMCOnOffStatus(bool flag)
  {
	  MAINDllOBJECT->ApplyMMCCorrection(flag);
  }

 void RWrapper::RW_MainInterface::SetAlgo6Param(int type6_MaskFactor, int type6_Binary_LowerFactor, int type6_Binary_UpperFactor, int type6_PixelThreshold, int type6_MaskingLevel, int type6_ScanRange, int type6_NoiseFilterDistInPixel, int type6SubAlgo_NoiseFilterDistInPixel, int type6JumpThreshold, bool DoAverageFilter)
   {
		try
		{
			MAINDllOBJECT->SetAlgo6Param(type6_MaskFactor, type6_Binary_LowerFactor, type6_Binary_UpperFactor, type6_PixelThreshold, type6_MaskingLevel, 
				type6_ScanRange, type6_NoiseFilterDistInPixel, type6SubAlgo_NoiseFilterDistInPixel, type6JumpThreshold, DoAverageFilter);
		}
		catch(Exception^ ex)
		{
			WriteErrorLog("WRMAIN0021a", ex);
		}
   }

 void RWrapper::RW_MainInterface::SetSurfaceEDSettings(int channel)
 {
	 MAINDllOBJECT->SetSurfaceEDSettings(channel);
 }

 void RWrapper::RW_MainInterface::SetNogoMeasurement(bool flag)
{
	try
	{
		for(RC_ITER MeasureItr = MAINDllOBJECT->getDimList().getList().begin(); MeasureItr != MAINDllOBJECT->getDimList().getList().end(); MeasureItr++)
		{
			DimBase* Cdim = (DimBase*)(*MeasureItr).second;
			Cdim->MeasurementUsedForPPAbort(flag);	
		}
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0021b", ex);
	}
}

 void RWrapper::RW_MainInterface::SetCriticalShapeParameter(bool CriticalShape, double angleTolerance, double RadiusPercentage)
 {
	 try
	 {
		 MAINDllOBJECT->CriticalShapeStatus = CriticalShape;
		 MAINDllOBJECT->AngleTolerance = angleTolerance;
		 MAINDllOBJECT->PercentageTolerance = RadiusPercentage;
		 for(RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin(); Item_ucs != MAINDllOBJECT->getUCSList().getList().end(); Item_ucs++)
		 {
			UCS* Cucs = (UCS*)(*Item_ucs).second;
			for(RC_ITER Item_Shape = Cucs->getShapes().getList().begin(); Item_Shape != Cucs->getShapes().getList().end(); Item_Shape++)
			{
				Shape* Cshape = (Shape*)(*Item_Shape).second;
				if(Cshape->Normalshape())
				{
					if(Cshape->ShapeType == RapidEnums::SHAPETYPE::LINE || Cshape->ShapeType == RapidEnums::SHAPETYPE::XRAY || Cshape->ShapeType == RapidEnums::SHAPETYPE::XLINE)
					{
						((Line*)Cshape)->SetAsCriticalShape(CriticalShape);
					}
					else if(Cshape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || Cshape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE3D || Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC3D || Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC)
					{
						((Circle*)Cshape)->SetAsCriticalShape(CriticalShape);
					}
				}
			}
		 }
	 }
	 catch(Exception^ ex)
	 {
		 WriteErrorLog("WRMAIN0021c", ex);
	 }
 }

 void RWrapper::RW_MainInterface::SetMeasurementColorGradient(bool gradient)
 {
	 MAINDllOBJECT->MeasurementcolorInGradient = gradient;
	 if (MAINDllOBJECT->getDimList().getList().size() > 0)
	 {
		 for (RC_ITER CdimItem = MAINDllOBJECT->getDimList().getList().begin(); CdimItem != MAINDllOBJECT->getDimList().getList().end(); CdimItem++)
		 {
			 DimBase* Cdim = (DimBase*)(*CdimItem).second;
			 Cdim->UpdateMeasurementColor();
		 }
	 }
 }

  void RWrapper::RW_MainInterface::SetRcadZoomToExtent(bool flag)
  {
	  MAINDllOBJECT->RcadZoomExtents(flag);
  }

  void RWrapper::RW_MainInterface::CreateSurfaceForClosedSurface(bool flag)
  {
	  try
	  {
		  MAINDllOBJECT->CreateSurfaceForClosedSurface(flag);
		  for(RC_ITER item = MAINDllOBJECT->getShapesList().getList().begin(); item != MAINDllOBJECT->getShapesList().getList().end(); item++)
		  {
			  Shape* currentShape = (Shape*)((*item).second);
			  if(currentShape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
				  currentShape->UpdateBestFit();
		  }
	  }
	  catch(Exception^ ex)
	  {
		  WriteErrorLog("WRMAIN0020a", ex);
	  }
  }

  void RWrapper::RW_MainInterface::UpdateCameraZoom(int CamWidth, int CamHeight)
  {
	  try
	  {
		  MAINDllOBJECT->UpdateCamera_ZoomLevel(CamWidth, CamHeight);
	  }
	  catch(Exception^ ex)
	  {
		  WriteErrorLog("WRMAIN0021a", ex);
	  }
}

void RWrapper::RW_MainInterface::SetbestfitAlignmentParam(double Resolution, int SampleSize)
{
	  try
	  {
		  MAINDllOBJECT->BestFitAlignment_Resolution = Resolution;
		  MAINDllOBJECT->BestFitAlignment_SampleSize = SampleSize;
	  }
	  catch(Exception^ ex)
	  {
		  WriteErrorLog("WRMAIN0021a", ex);
	  }
}

//void RWrapper::RW_MainInterface::CreateCamProfileShape(double MinRadius, double MaxRadius, double sector_angle, double min_tol, double max_tol, bool Tool_Type)			//vinod according to new fe.  14/02/2014
//{
//	  try
//	  {
//		 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
//		 Shape *OuterCircle, *RayLine;
//		 OuterCircle = NULL;
//		 RayLine = NULL;
//		for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
//		{
//			ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
//			if(!CShape->Normalshape()) continue;
//			if(!CShape->selected()) continue;
//			if(CShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)				//we need a circle and line
//			{				
//				OuterCircle = CShape;
//			}
//			else if(CShape->ShapeType == RapidEnums::SHAPETYPE::XRAY)
//			{				
//				RayLine = CShape;
//			}
//		}
//		if(OuterCircle != NULL && RayLine != NULL)
//		{
//			double circleCenter[3] = {0};
//			Line *New_Line = (Line*)RayLine;
//			double _slope = New_Line->Angle(), _intercept = New_Line->Intercept(), _radius;
//			Circle* New_Circle = (Circle*)OuterCircle;
//			New_Circle->getCenter()->FillDoublePointer(circleCenter, 3);
//			_radius = New_Circle->Radius();
//			CamProfile::Set_DeiphiCam_Angle(sector_angle, min_tol, max_tol, Tool_Type);			//sent centre value to draw respective nominal shape vinod 23/01/2014
//			CamProfile::Set_DeiphiCam_Circle_Centre(circleCenter[0], circleCenter[1], circleCenter[2], _slope, _intercept);			//sent centre value to draw respective nominal shape vinod 23/01/2014
//			RWrapper::RW_DelphiCamModule::Fill__Circle_Centre(circleCenter[0], circleCenter[1], circleCenter[2], _slope, _intercept, _radius, sector_angle);		//vinod sent value to delphicam
//			DXFEXPOSEOBJECT->Cam_profile_Circle_Centre(circleCenter[0], circleCenter[1], circleCenter[2],Tool_Type);			//11/02/2014 vinod.. for excel report we need cam profile circle center in DXFExpose.cpp
//			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CAMPROFILE_HANDLER);
//			 Shape* CurrentShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
//			 ((CamProfile*)MAINDllOBJECT->getCurrentHandler())->SetMinRadius(MinRadius);
//		}
//	  }
//	  catch(Exception^ ex)
//	  {
//		  WriteErrorLog("WRMAIN0021b", ex);
//	  }
//}

void RWrapper::RW_MainInterface::CreateCamProfileShape(double MinRadius, double MaxRadius, double sector_angle, double min_tol, double max_tol, bool Tool_Type, int Pts_toBundle)			//vinod according to new fe.  14/02/2014
{
	try
	{
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
		Shape *OuterCircle, *RayLine, *RPoint3dForZposition;
		OuterCircle = NULL;
		RayLine = NULL;
		RPoint3dForZposition = NULL;
		for (RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
		{
			ShapeWithList* CShape = (ShapeWithList*)((*Shpitem).second);
			if (!CShape->Normalshape()) continue;
			if (!CShape->selected()) continue;
			if (CShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)				//we need a circle and ray
			{
				OuterCircle = CShape;
			}
			else if (CShape->ShapeType == RapidEnums::SHAPETYPE::XRAY)
			{
				RayLine = CShape;
			}
			else if (CShape->ShapeType == RapidEnums::SHAPETYPE::RPOINT3D)
			{
				RPoint3dForZposition = CShape;						           //added by vinod jakhar on 11/04/2014 as per new requirement in measurement for z_position
			}
		}
		if (OuterCircle != NULL && RayLine != NULL && RPoint3dForZposition != NULL)
		{
			double circleCenter[3] = { 0 };
			Line *New_Line = (Line*)RayLine;
			double _slope = New_Line->Angle(), _intercept = New_Line->Intercept(), _radius;
			Circle *New_Circle = (Circle*)OuterCircle;
			New_Circle->getCenter()->FillDoublePointer(circleCenter, 3);
			_radius = New_Circle->Radius();
			RPoint *Point3DForZPosition = (RPoint*)RPoint3dForZposition;
			System::String^ DTVARDate = Microsoft::VisualBasic::DateAndTime::Now.ToString("dd-MMM-yyyy");
			System::String^ DTVARTime = Microsoft::VisualBasic::DateAndTime::Now.ToString("hh-mm-tt");
			CamProfile::Set_DeiphiCam_parameters(sector_angle, min_tol, max_tol, Tool_Type, Pts_toBundle, New_Line->getPoint1()->getX(), New_Line->getPoint1()->getY(), New_Line->getPoint2()->getX(), New_Line->getPoint2()->getY(), MinRadius, MaxRadius, (char*)Marshal::StringToHGlobalAnsi(DTVARDate).ToPointer(), (char*)Marshal::StringToHGlobalAnsi(DTVARTime).ToPointer());			//sent centre value to draw respective nominal shape vinod 23/01/2014
			CamProfile::Set_DeiphiCam_Circle_Centre(circleCenter[0], circleCenter[1], circleCenter[2], _slope, _intercept, _radius);			//sent centre value to draw respective nominal shape vinod 23/01/2014
			RWrapper::RW_DelphiCamModule::Fill__Circle_Centre(circleCenter[0], circleCenter[1], circleCenter[2], _slope, _intercept, _radius, sector_angle, (Point3DForZPosition->getPosition()->getZ()));		//vinod sent value to delphicam
			DXFEXPOSEOBJECT->Cam_profile_Circle_Centre(circleCenter[0], circleCenter[1], circleCenter[2], Tool_Type, (Point3DForZPosition->getPosition()->getZ()), MinRadius, MaxRadius);			//11/02/2014 vinod.. for excel report we need cam profile circle center in DXFExpose.cpp RW_MeasureParameter
			RWrapper::RW_MeasureParameter::MYINSTANCE()->ZPositionof3DpointforTVSCircleCen(Point3DForZPosition->getPosition()->getZ(), circleCenter[0], circleCenter[1], circleCenter[2], MinRadius, MaxRadius);//we have to send there Zposition to export in excel, all calculation is done in
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CAMPROFILE_HANDLER);
			Shape* CurrentShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			((CamProfile*)MAINDllOBJECT->getCurrentHandler())->SetMinRadius(MinRadius);
		}
	}
	catch (Exception^ ex)
	{
		WriteErrorLog("WRMAIN0021b", ex);
	}
}


void RWrapper::RW_MainInterface::GetCamProfileDistanceFromCenter(double angle, int PtsToBundle)
{
	  try
	  {
		 DXFEXPOSEOBJECT->WriteCamProfileDist_CsvFile(angle, PtsToBundle);
	  }
	  catch(Exception^ ex)
	  {
		  WriteErrorLog("WRMAIN0021c", ex);
	  }
}

//hob personaldata export 07/03/2014
void RWrapper::RW_MainInterface::GethobCheckerPersonaldata(int OrderNo, int SerialNo, int ToolID, System::String^ CustomerName, System::String^ Date, System::String^ Operator, int MachineNo, bool IsHobTypeBore, System::String^ AccuracyClass, double leadofGash, System::String^ Module)
{
	  try
	  {
		 DXFEXPOSEOBJECT->GethobCheckerPersonaldatain_CsvFile(OrderNo, SerialNo, ToolID, (char*)Marshal::StringToHGlobalAnsi(CustomerName).ToPointer(), (char*)Marshal::StringToHGlobalAnsi(Date).ToPointer(), (char*)Marshal::StringToHGlobalAnsi(Operator).ToPointer(),  MachineNo,  IsHobTypeBore, (char*)Marshal::StringToHGlobalAnsi(AccuracyClass).ToPointer(),  leadofGash, (char*)Marshal::StringToHGlobalAnsi(Module).ToPointer());
	  }
	  catch(Exception^ ex)
	  {
		  WriteErrorLog("WRMAIN0021c", ex);
	  }
}

void RWrapper::RW_MainInterface::GetvalueofBCXBCY(double ValBCX, double ValBCY)
{
	try
	{
		//RCadApp::SetValuesOfBCXBCY(ValBCX, ValBCY);			//vinod..for hob checker getc_axis_xy..
	}
	catch(Exception^ ex)
	{
		WriteErrorLog("WRMAIN0021ca", ex);
	}
}

void RWrapper::RW_MainInterface::SetShapeMeasureHideDuringProgramRun(bool Flag)
{
	  try
	  {
		 MAINDllOBJECT->HideShapeMeasurementDuringPP = Flag;
	  }
	  catch(Exception^ ex)
	  {
		  WriteErrorLog("WRMAIN0021c", ex);
	  }
}

void RWrapper::RW_MainInterface::SetPixelWidthCorrectionStatusForOneShot(bool Flag)
{
	MAINDllOBJECT->PixelWidthCorrectionForOneShot(Flag);
	RWrapper::RW_DBSettings::MYINSTANCE()->UpdateSurfaceFGSettings();
}

void RWrapper::RW_MainInterface::SetPixelWidthCorrectionConstants(double* PixelWidthConstants)
{
	MAINDllOBJECT->PixelWidthCorrectionQuadraticTerm(PixelWidthConstants[0]);
	MAINDllOBJECT->PixelWidthCorrectionLinearTermX(PixelWidthConstants[1]);
	MAINDllOBJECT->PixelWidthCorrectionLinearTermY(PixelWidthConstants[2]);
	MAINDllOBJECT->PixelWidthX0(PixelWidthConstants[3]);
	MAINDllOBJECT->PixelWidthY0(PixelWidthConstants[4]);
}

 void RWrapper::RW_MainInterface::ProbeGotoRequiredPosition_GetProbePts()
 {
	 try
	 {	
		 RWrapper::RW_Probe_Path_Entity^ ProbePath_LstEntity;
		if((ProbePathList->Count > 0) && (ProbeCount <= ProbePathList->Count-1))
		{
			RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::MACHINE_PROBE_PATHPOINT);
			MAINDllOBJECT->AutoProbeMeasureFlag = true;
			ProbePath_LstEntity = ProbePathList[ProbeCount];
			double target[4] = {ProbePath_LstEntity->Pt_X, ProbePath_LstEntity->Pt_Y, ProbePath_LstEntity->Pt_Z, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
			double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1],RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
			
			if(!ProbePath_LstEntity->PathPt)
			{
				feedrate[0] = 1;
				feedrate[1] = 1; 
				feedrate[2] = 1; 
				feedrate[3] = 1; 
			}

			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::MACHINE_PROBE_PATHPOINT);								
		}	
		else
		{
			if((ProbePathList->Count > 5))
			{
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
				if(MAINDllOBJECT->ShowMsgBoxString("RW_MainInterface_AutoProbe01", RapidEnums::MSGBOXTYPE::MSG_YES_NO, RapidEnums::MSGBOXICONTYPE::MSG_QUESTION))
				{
					bool flag = false;
					flag = HELPEROBJECT->ShowGraphicalRepresentaionforDeviation(double(System::Convert::ToDouble(ProbeScanDetais_ArrayList[0])), double(System::Convert::ToDouble(ProbeScanDetais_ArrayList[1])), bool(System::Convert::ToBoolean(ProbeScanDetais_ArrayList[2])));
					if(flag)
					{
						ProbePathList->Clear();
					}
				}
			}
		}
	 }
	 catch(Exception^ ex)
	{
		MAINDllOBJECT->AutoProbeMeasureFlag = false; WriteErrorLog("WRMAIN0021", ex);
	}
}

 void RWrapper::RW_MainInterface::Continue_PauseProbeScanning(bool continueflag)
 {
	 try
	 {
		 if(continueflag)
			 ProbeGotoRequiredPosition_GetProbePts();
		 else
			 RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
	 }
	 catch(Exception^ ex)
	 {
		WriteErrorLog("WRMAIN0022b", ex);
	 }
}

void RWrapper::RW_MainInterface::AbortProbeScanningProcess()
{
	 try
	 {
		 RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
		 this->ProbePathList->Clear();
		 this->ProbeCount = 0;
		 MAINDllOBJECT->AutoProbeMeasureFlag = false;
		 ProbeScanDetais_ArrayList->Clear();
	 }
	 catch(Exception^ ex)
	 {
		WriteErrorLog("WRMAIN0022b", ex);
	 }
}

void RWrapper::RW_MainInterface::ShowDeviation_ProbeScanPts()
 {
	 try
	 {
		bool flag = false;
		flag = HELPEROBJECT->ShowGraphicalRepresentaionforDeviation(double(System::Convert::ToDouble(ProbeScanDetais_ArrayList[0])), double(System::Convert::ToDouble(ProbeScanDetais_ArrayList[1])), bool(System::Convert::ToBoolean(ProbeScanDetais_ArrayList[2])));
		if(flag)
		{
			ProbePathList->Clear();
		}
	 }
	 catch(Exception^ ex)
	 {
		WriteErrorLog("WRMAIN0022c", ex);
	 }
}

void RWrapper::RW_MainInterface::CloudPointDeviation_DxfShape(double UpperCutoff, double tolerance, double interval, int IntervalType, bool ClosedLoop)
{
	 try
	 {
		 MAINDllOBJECT->ShowDeviationwithCloudPts(UpperCutoff, tolerance, interval, IntervalType, ClosedLoop, true);	
		//DXFEXPOSEOBJECT->ShowDeviationwithCloudPts(tolerance, interval,IntervalType, ClosedLoop);
	 }
	 catch(Exception^ ex)
	 {
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0022d", ex);
	 }
}

void RWrapper::RW_MainInterface::CloudPointDeviation_DxfShape(double Uppertolerance, double Lowertolerance, double interval, bool DeviationWithYellowShapes, int IntervalType, bool deviationType, int measureRefrenceAxis, bool BestfitArc)
{
	 try
	 {
		 MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEVIATION_HANDLER);
		 MAINDllOBJECT->SetDeviationParameter(Uppertolerance, Lowertolerance, interval, DeviationWithYellowShapes, IntervalType, deviationType, measureRefrenceAxis, BestfitArc);
	 }
	 catch(Exception^ ex)
	 {
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0022e", ex);
	 }
}

void RWrapper::RW_MainInterface::Set_FixedCrosshairColor(int r_val, int g_val, int b_val)
{
	MAINDllOBJECT->SetCrosshair_Color(r_val/255.0, r_val/255.0, r_val/255.0);
}

void RWrapper::RW_MainInterface::Set_FlexibleCrosshairColor(int r_val, int g_val, int b_val)
{
	MAINDllOBJECT->SetFlexibleCrosshair_Color(r_val/255.0, r_val/255.0, r_val/255.0);
}

void RWrapper::RW_MainInterface::Set_MeasurementColor(int r_val, int g_val, int b_val)
{
	MAINDllOBJECT->SetMeasurementColor(r_val/255.0, r_val/255.0, r_val/255.0);
}

void RWrapper::RW_MainInterface::Set_ShapeColor(int r_val, int g_val, int b_val)
{
	MAINDllOBJECT->SetshapeColor(r_val/255.0, r_val/255.0, r_val/255.0);
}

void RWrapper::RW_MainInterface::SaveDialog_DxfExport(System::String^ filename)
{
}

void RWrapper::RW_MainInterface::OpenDialog_DxfImport(System::String^ filename)
{
}

void RWrapper::RW_MainInterface::WriteUserLog(System::String^ FunctionName, System::String^ Action, System::String^ Position)
{
	try
	{
		FunctionName = FunctionName + ", " + DateTime::Now.ToShortDateString() + ", " + DateTime::Now.ToString("HH:mm:ss.fff");
		this->ErrorHandlerObject->WriteUserLogFile(FunctionName, Action, Position);
	}
	catch (Exception^ ex){}
}

void RWrapper::RW_MainInterface::WriteErrorLog(System::String^ ErrorCode, System::Exception^ ex)
{
	try
	{
		ErrorCode = ErrorCode + ", " + DateTime::Now.ToShortDateString() + ", " + DateTime::Now.ToString("HH:mm:ss.fff");
		this->ErrorHandlerObject->ReportError(ErrorCode, ex->Message, ex->StackTrace, false);
	}
	catch (Exception^ ex){}
}

void RWrapper::RW_MainInterface::WriteCommandSendLog(System::String^ Str)
{
	try
	{
		Str = Str + ", " + DateTime::Now.ToShortDateString() + ", " + DateTime::Now.ToString("HH:mm:ss.fff");
		this->ErrorHandlerObject->Err_WriteCommandSendLog(Str);
	}
	catch (Exception^ ex){}
}

void RWrapper::RW_MainInterface::WriteCommandSendLog(RWrapper::CommandLogger^ currentCommandLog)
{
	try
	{
		if (OK_To_Write_Logs)
		{
			OK_To_Write_Logs = false;
			Sleep(10);
		}
		this->SentCommandLogCollection->Add(currentCommandLog);
		OK_To_Write_Logs = true;
	}
	catch (Exception^ ex)
	{

	}
}

void RWrapper::RW_MainInterface::CommandLogWriteTick(System::Object^ obj)
{
	try
	{
		if (this->SentCommandLogCollection->Count > 0)
		{
			int i = 0;
			String^ Str;
			while (this->SentCommandLogCollection->Count > 0 && OK_To_Write_Logs)
			{
				Str = this->SentCommandLogCollection[i]->CommandValue + ", " + this->SentCommandLogCollection[i]->Comments;
				Str = Str + ", "  + this->SentCommandLogCollection[i]->Timestamp->ToShortDateString() + ", " + this->SentCommandLogCollection[i]->Timestamp->ToString("HH:mm:ss.fff");
				this->ErrorHandlerObject->Err_WriteCommandSendLog(Str);
				this->SentCommandLogCollection->RemoveAt(0);
			}
		}
	}
	catch(Exception^ ex)
	{ }
}

void RWrapper::RW_MainInterface::WriteErrorLog(System::String^ ErrorCode, System::String^ Errormsg, System::String^ ErrorTrace)
{
	try
	{
		ErrorCode = ErrorCode + ", " + DateTime::Now.ToShortDateString() + ", " + DateTime::Now.ToString("HH:mm:ss.fff");
		this->ErrorHandlerObject->ReportError(ErrorCode, Errormsg, ErrorTrace, false);
	}
	catch (Exception^ ex){}
}

void RWrapper::RW_MainInterface::WriteModuleInitilization(System::String^ Str)
{
	try
	{
		Str = Str + ", " + DateTime::Now.ToShortDateString() + ", " + DateTime::Now.ToString("HH:mm:ss.fff");
		this->ErrorHandlerObject->Err_WriteModuleInitialisation(Str);
	}
	catch (Exception^ ex){}
}

void RWrapper::RW_MainInterface::LineArcEvent(double tolerance, double max_radius, double min_radius, double min_angle_cutoff, double noise, double max_dist_betn_pts, bool closed_loop, bool shape_as_fast_trace, bool join_all_pts)
{
   LineArcParameters::raise(tolerance, max_radius, min_radius, min_angle_cutoff, noise, max_dist_betn_pts, closed_loop, shape_as_fast_trace, join_all_pts);
}

void RWrapper::RW_MainInterface::StartFocusInFrameGrab()
{
	try
	{
		FOCUSCALCOBJECT->FocusStartPosition.set(RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2]);
		FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::CONTOURFRAMEGRAB;
		if((MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::DSP) || (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP)) return;
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1); //cnc mode
		double feedrate[4] = {5, 5, 5, 5}, target[4] = {0};
		for(int i = 0; i < 4; i++)
		{
			if(i == RWrapper::RW_FocusDepth::MYINSTANCE()->GetSelectedAxis())
				target[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i] -  RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpan; // Changed sign on 2 april 2011... As per narayanan sir requirement
			else
				target[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		}
		if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
		   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::FOCUSDEPTH_SPAN);
	}
	catch(Exception^ ex)
	{ 
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0045b", ex);
	}
}

void RWrapper::RW_MainInterface::StartFocusForRightClick()
{
	try
	{
		double feedrate[4] = {0}, target[4] = {0};
		for(int i = 0; i < 4; i++)
		{
			feedrate[i] = RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpeed;
			if(i == RWrapper::RW_FocusDepth::MYINSTANCE()->GetSelectedAxis())
				target[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i] - RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpan;
			else
				target[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		}
		//FOCUSCALCOBJECT->ClearLastFocusValues();

		if(MAINDllOBJECT->getSurface())
		{
			FOCUSCALCOBJECT->FocusStartPosition.set(target[0], target[1], target[2], target[3]); // RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2]);
			FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::SCANFORRIGHTCLICK;
			if((MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::DSP) || (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP))
			{
				RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::FOCUSCHECK_SPAN);
				RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
			}
			else
			{ 
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1); //cnc mode
				if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
				   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
				DROInstance->notFocusing = false;
				RWrapper::RW_FocusDepth::MYINSTANCE()->Focusflag = true;
				RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::FOCUSCHECK_SPAN);
			}
		}
		else if(MAINDllOBJECT->getProfile())
		{
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1); //cnc mode
			RWrapper::RW_DRO::MYINSTANCE()->AutoFocusMode_Flag = true;
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::PROFILEAUTOFOCUSSTART);
		}
	}
	catch(Exception^ ex)
	{ 
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0045a", ex);
	}
}

void RWrapper::RW_MainInterface::StartFocusInProfileScan()
{
	try
	{
		FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::PROFILESCAN_FOCUS;
			RWrapper::RW_FocusDepth::MYINSTANCE()->CurrentFocusValue = "";
		RWrapper::RW_FocusDepth::MYINSTANCE()->RaiseFocusEvents(0);
		FOCUSCALCOBJECT->setRowColumn(1, 1, 0, 600, 400);
		FOCUSCALCOBJECT->CalculateRectangle();
		if((MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::DSP) && (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP)) return;
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1); //cnc mode
		double feedrate[4] = {5, 5, 5, 5}, target[4] = {0};
		for(int i = 0; i < 4; i++)
		{
			if(i == 2)
				target[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i] - 1; // Changed sign on 2 april 2011... As per narayanan sir requirement
			else
				target[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		}
		RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpan = 1;
		if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
		   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::FOCUSDEPTH_SPAN);
	}
	catch(Exception^ ex)
	{ 
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0045b", ex);
	}
}

void RWrapper::RW_MainInterface::BestFitSurfaceWindow(double i, bool value)
{
   BestFitSurfaceParam::raise(i, value);
}

void RWrapper::RW_MainInterface::ShowWindowForAutoFocusRaise(bool open)
{
   ShowWindowForAutoFocus::raise(open);
}

void RWrapper::RW_MainInterface::ShowWindowForAutoContourRaise(bool open)
{
   ShowWindowForAutoContour::raise(open);
}

void RWrapper::RW_MainInterface::ShowRotationScanWindowRaise(int i)
{
	//System::Windows::Forms::MessageBox::Show("Got Scan Event Raised", "Rapid-I");
	if (PPCALCOBJECT->IsPartProgramRunning())
	{
		if (i == 1)
		{

			RW_PartProgram::MYINSTANCE()->CalculateAndMovetoPosition(1); //Move in R axis to scan automatically!
			RW_MainInterface::MYINSTANCE()->SetContinuousScanMode(true); //Make Scanning mode for Pause/Continue work properly.
			RW_MainInterface::MYINSTANCE()->Start_StopRotationalScan(true); //Start the scan....
			//Remember Label is now Rotary_Axis_Scan, which when target is reached, will switch this off. 
			//RotationScanHandler.cpp will call its own StopScanning() method that will call ProgramReached and program should continue...
		}
	}
	else
		ShowRotateScanEvent::raise(i);
}

void RWrapper::RW_MainInterface::LineArcUcsCreated()
{
   LineArcUcsCreatedEvent::raise();
   RWrapper::RW_DRO::MYINSTANCE()->Handle_DROCallback(DROInstance->DROCoordinateValue);
}

void RWrapper::RW_MainInterface::RaiseTruePositionEvent(double x, double y)
{
	TruePositionEvent::raise(x, y);
}

void RWrapper::RW_MainInterface::BestFitWithDifferentParam(double i, bool value, bool again)
 {
	 MAINDllOBJECT->BestFitSurfaceWithDiffParam(i, value, again);
 }

 void RWrapper::RW_MainInterface::ShowHideWaitCursor(int i)
 {
	WaitCursor::raise(i);
 }

 void RWrapper::RW_MainInterface::GotProfileScanEndPts(int i)
 {
	ProfileScanEndPtEvent::raise(i);
 }
 
void RWrapper::RW_MainInterface::RaiseEvents(int Cnt)
 {
	 try
	 {
		 if (Cnt == 0)
			 HideEntityPanel::raise();
		 else if (Cnt == 1)
			 ParallelLineOffset::raise();
		 else if (Cnt == 2)
			 CircleDiameter::raise();
		 else if (Cnt == 3)
			 OnePtOffsetLine::raise();
		 else if (Cnt == 4)
			 OnePtAngleLine::raise();
		 else if (Cnt == 9)
			 EnterTruePosition::raise();
		 else if (Cnt == 10)
			 EnterDerivedLineLength::raise();
		 else if (Cnt == 11)
			 EnterThreadCount::raise();
		 else if (Cnt == 12)
			 AutoDxfAlign::raise();
		 else if (Cnt == 13)
			 ParallelPlaneOffset::raise();
		 else if (Cnt == 14)
			 PerpendicularPlaneOffset::raise();
		 else if (Cnt == 15)
			 PerpendicularLine3DOffset::raise();
		 else if (Cnt == 16)
			 ParallelLine3DOffset::raise();
		 else if (Cnt == 17)
			 RelativePointOffset::raise();
		 else if (Cnt == 18)
			 NudgeRotatePPevent::raise();
		 else if (Cnt == 20)
			 RotationAngleevent::raise();
		 else if (Cnt == 21)
			 Translationevent::raise();
		 else if (Cnt == 22)
			 ShowOuterMostLineEvent::raise();
		 else if (Cnt == 23)
			 ShowDeviationWindow::raise();
		 else if (Cnt == 24)
			 ShowCloudPointMeasureWindow::raise();
		 else if (Cnt == 25)
			 Enter3DTruePosition::raise();
		 else if (Cnt == 26)
			 EnterTruePositionForPoint::raise();
		 else if (Cnt == 27)
			 Enter3DTruePositionPoint::raise();
		 else if (Cnt == 28)
			 FixtureAlign_RefPt_Taken::raise();
	 }
	 catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0045", ex);
	}
 }

void RWrapper::RW_MainInterface::RaiseEvents(int Cnt, double val)
{
	try
	{
		 if(Cnt == 6)
			 ParallelismLand::raise(val);
		 else if(Cnt == 7)
			 PerpediclaurityLand::raise(val);
		 else if(Cnt == 8)
			 AngularityLand::raise(val);
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0045b", ex);
	}
}

void RWrapper::RW_MainInterface::RaiseSurfaceFrameGrabEvent()
{
	RightClickedOnEntity::raise(3, "SurfaceFrameGrab");
}

void RWrapper::RW_MainInterface::RaiseSurfaceFrameGrabEvent(System::String^ SFGEntityNames)
{
	GetSurfaceEDUserChoice::raise(SFGEntityNames);
}

void RWrapper::RW_MainInterface::RaiseSavedImageEvent(System::String^ FilePath)
{
	SavedImagePath::raise(FilePath);
}

void RWrapper::RW_MainInterface::RaiseCamSettingsUpdated(System::String^ uCamSettings)
{
	UpdateCamSettingsEvent::raise(uCamSettings);
}

void RWrapper::RW_MainInterface::UpdateVideoGraphics()
{
	//return;
	MAINDllOBJECT->Wait_VideoGraphicsUpdate();
	MAINDllOBJECT->update_VideoGraphics();
	MAINDllOBJECT->Wait_VideoGraphicsUpdate();
}

void RWrapper::RW_MainInterface::UpdateRCadGraphics()
{
	MAINDllOBJECT->Wait_RcadGraphicsUpdate();
	MAINDllOBJECT->update_RcadGraphics();
	MAINDllOBJECT->Wait_RcadGraphicsUpdate();
}

void RWrapper::RW_MainInterface::UpdateDxfGraphics()
{
	MAINDllOBJECT->Wait_DxfGraphicsUpdate();
	MAINDllOBJECT->update_DxfGraphics();
	MAINDllOBJECT->Wait_DxfGraphicsUpdate();
}

void RWrapper::RW_MainInterface::CaptureCAxis()
{
	try
	{
		//MAINDllOBJECT->UpdatingCAxis = true;
		//MAINDllOBJECT->Initialise_CAxis_Calc();
		//RWrapper::RW_DRO::MYINSTANCE()->RotaryStartPosition = gcnew array < double > {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		//RWrapper::RW_DRO::MYINSTANCE()->RotationalScanCount = 0;
		//RWrapper::RW_CNC::MYINSTANCE()->RotateCAxis360();
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0045a", ex);
	}
}

void RWrapper::RW_MainInterface::GenerateToolBarClick(System::String^ ToolbarName, System::String^ buttonName, bool Checked)
{
	GenerateToolbarClickEvent::raise(ToolbarName, buttonName, Checked);
}

void RWrapper::RW_MainInterface::deletePointsLogFile()
{
	try
	{
		std::string folderPath = MAINDllOBJECT->currentPath + "\\PointsLog";
		System::String^ pathstring = gcnew System::String(folderPath.c_str());
		System::IO::DirectoryInfo^ PointsLogInfo = gcnew System::IO::DirectoryInfo(pathstring);
		if(PointsLogInfo->Exists)
		{
			 cli::array<String^>^dirs = System::IO::Directory::GetFiles(pathstring);
			 for each(System::String^ filename in dirs)
			 {
				System::IO::File::Delete(filename);
			 }
		}	
	}
	 catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0046", ex);
	}
 }

void RWrapper::RW_MainInterface::SetShapeForMeasure(int id)
{
   MAINDllOBJECT->HighlightShapeForMeasure(id);
}

void RWrapper::RW_MainInterface::SetShapeRightClickForMeasure(int id)
{
	MAINDllOBJECT->HighlightShapeRightClickForMeasure(id);
}

void RWrapper::RW_MainInterface::DataTransferToExcelForDeviation()
{
	try
	{
		if(HELPEROBJECT->ProbeMeasurelist->size() > 0)
		{
			System::Collections::Generic::List<System::Double>^ updatedMeasureList;
			System::Collections::Generic::List<System::Collections::Generic::List<System::Double>^>^ DeviationMeasureList = gcnew System::Collections::Generic::List<System::Collections::Generic::List<System::Double>^>;
			for each(std::list<double> *measureParam in *HELPEROBJECT->ProbeMeasurelist)
			{
				updatedMeasureList = gcnew System::Collections::Generic::List<System::Double>;
				for each(double Cvalue in *measureParam)
				{
					 System::Double temp =  System::Double(Cvalue);
					 updatedMeasureList->Add(temp);
				}
				DeviationMeasureList->Add(updatedMeasureList);
			}
			System::Collections::ArrayList^ CompDetails_ArrayList = gcnew System::Collections::ArrayList;
			CompDetails_ArrayList->AddRange(ProbeScanDetais_ArrayList);
			CompDetails_ArrayList->RemoveAt(2);
			MeasureDeviationEvent::raise(DeviationMeasureList, CompDetails_ArrayList);
		}
	}
	 catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0047", ex);
	}
 }

void RWrapper::RW_MainInterface::SetCamProperty(int PropID, int Value)
{
	//Access RapidCam directly and set the property. 
	MAINDllOBJECT->SetCameraProp(PropID, Value);
}

int RWrapper::RW_MainInterface::GetCamProperty(int PropID)
{
	return MAINDllOBJECT->GetCameraProp(PropID);
}

void RWrapper::RW_MainInterface:: GetRefDotParams(int index, cli::array<System::Double, 1>^ CentreVal)
{
	//Copy Parameter values of current Reference Dot
	if (index == 0)
	{
		Circle* CC = (Circle*) MAINDllOBJECT->ReferenceDotShape;
		CentreVal[0] = CC->getCenter()->getX();
		CentreVal[1] = CC->getCenter()->getY();
		CentreVal[2] = CC->getCenter()->getZ();
		CentreVal[3] = CC->getCenter()->getR();
	}

}

void RWrapper::RW_MainInterface::GetInsertedImageBytes(bool InsertImage, cli::array<Byte, 1>^ SourceBytes)
{
	MAINDllOBJECT->InsertStaticImageinVideo = InsertImage;
	if (!InsertImage)
		memset(MAINDllOBJECT->staticImageBytes, 102, 800 * 600 * 4);
	else
	{
		int imagesize = 800 * 600 * 4;
		for (int i = 0; i < imagesize; i++)
			MAINDllOBJECT->staticImageBytes[i] = SourceBytes[i];
//		memcpy(MAINDllOBJECT->staticImageBytes, SourceBytes, 800 * 600 * 4);
	}
}

void RWrapper::RW_MainInterface::GetBinarizedImage(cli::array<Byte, 1>^ SourceBytes)
{
	int totalSize = 800 * 600 * 3;
	MAINDllOBJECT->GetBinarisedImage(); 
	for (int i = 0; i < totalSize; i++)
		SourceBytes[i] = MAINDllOBJECT->BinarisedImage[i];
}

void RWrapper::RW_MainInterface::ClassControl(ClassNames ClassName, bool Start)
{
		switch (ClassName)
		{
		case ClassNames::L_Block:
			if (Start)
				gcnew RWrapper::RW_LBlock();
			else
				RWrapper::RW_LBlock::MYINSTANCE()->~RW_LBlock();
			break;
		case ClassNames::AutoCalibration :
			if (Start)
				gcnew RWrapper::RW_AutoCalibration();
			else
				RWrapper::RW_AutoCalibration::MYINSTANCE()->~RW_AutoCalibration();
			break;
		case ClassNames::DelphiCam :
			if (Start)
				gcnew RWrapper::RW_DelphiCamModule();
			else
				RWrapper::RW_DelphiCamModule::MYINSTANCE()->~RW_DelphiCamModule();
			break;
		case ClassNames::SphereCalibration:
			if (Start)
				gcnew RWrapper::RW_SphereCalibration();
			else
				RWrapper::RW_SphereCalibration::MYINSTANCE()->~RW_SphereCalibration();
			break;
		case ClassNames::Fixture_Calib:
			if (Start)
				gcnew RWrapper::RW_FixtureCalibration();
			else
				RWrapper::RW_FixtureCalibration::MYINSTANCE()->~RW_FixtureCalibration();
			break;
		case ClassNames::ImageGridProgram:
			if (Start)
				gcnew RWrapper::RW_ImageGridProgram();
			else
				RWrapper::RW_ImageGridProgram::MYINSTANCE()->~RW_ImageGridProgram();
			break;
		case ClassNames::SuperImposeImage:
			if (Start)
				gcnew RWrapper::RW_SuperImposeImage();
			else
				RWrapper::RW_SuperImposeImage::MYINSTANCE()->~RW_SuperImposeImage();
			break;
		case ClassNames::V_Block_Calib:
			if (Start)
				gcnew RWrapper::RW_VBlockCallibration();
			else
				RWrapper::RW_VBlockCallibration::MYINSTANCE()->~RW_VBlockCallibration();
			break;
		default:
			break;
		}
}

int RWrapper::RW_MainInterface::GetCurrentCameraType()
{
	return MAINDllOBJECT->CurrentCameraType;
}

void RWrapper::RW_MainInterface::Abort_HomePosition_Command()
{
	RWrapper::RW_CNC::MYINSTANCE()->Activate_ManualModeDRO();
	this->StartHideWaitStatusMessage(1);
}

void RWrapper::RW_MainInterface::SetdefaultCamSettings(int SettingType, System::String^ NewCamSettings)
{
	switch (SettingType)
	{
	case 1:
		MAINDllOBJECT->SurfaceCamSettings_default = (const char*)(Marshal::StringToHGlobalAnsi(NewCamSettings)).ToPointer();
		break;
	case 0:
		MAINDllOBJECT->ProfileCamSettings_default = (const char*)(Marshal::StringToHGlobalAnsi(NewCamSettings)).ToPointer();
		break;

	case 2:
		MAINDllOBJECT->RefDotCamSettings = (const char*)(Marshal::StringToHGlobalAnsi(NewCamSettings)).ToPointer();
		break;
	default:
		break;
	}
}

bool RWrapper::RW_MainInterface::OperateSpecialPins(int PinNumber, bool SwitchON)
{
	try
	{
		DROInstance->ControlPins(PinNumber, SwitchON); return true;
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRMAIN0101", ex);
		return false;
	}
}

void RWrapper::RW_MainInterface::Set_Forced_Auto_FG(bool isAuto)
{
	MAINDllOBJECT->Force_AutoFG_InAssistedMode(isAuto);
}

void RWrapper::RW_MainInterface::CheckRunningGraphicsCard()
{
	ManagementObjectSearcher^ mos = gcnew ManagementObjectSearcher("SELECT * FROM Win32_VideoController");
	ManagementObjectCollection^ mosColl = mos->Get();
	//for(int i = 0; i < mosColl->Count; i++)
	for each(ManagementObject^ mobj in mosColl)
	{
		PropertyData^ Description = mobj->Properties["Description"];
		PropertyData^ currentBitsPerPixel = mobj->Properties["CurrentBitsPerPixel"];
		System::String^ graphicsCardName = Description->Value->ToString();
		//System::Windows::MessageBox::Show(graphicsCardName);

		if (graphicsCardName != "") //Convert::ToInt32(currentBitsPerPixel->Value) > 0 && 
		{
			if (graphicsCardName->Contains("NVIDIA"))
			{
				if (MAINDllOBJECT->CameraConnected())
				{
					MAINDllOBJECT->ShiftUpperCrossHair(true);
				}
				else 
					MAINDllOBJECT->ShiftUpperCrossHair(false);
				break;
			}
		}
	}
}

void RWrapper::RW_MainInterface::CrossHairOffsets(cli::array<int>^ OffsetValue, bool GetValuesOnly)
{
	try
	{
		if (GetValuesOnly)
		{
			if (MAINDllOBJECT->getCurrentUCS().UCSangle() == 0)
			{
				for (int i = 0; i < 8; i++)
					OffsetValue[i] = MAINDllOBJECT->Offset_Normal[i];
			}
			else
			{//memcpy(MAINDllOBJECT->Offset_Skewed, OffsetValue, 8 * sizeof(int));
				for (int i = 0; i < 8; i++)
					OffsetValue[i] = MAINDllOBJECT->Offset_Skewed[i];
			}
		}
		else
		{
			if (MAINDllOBJECT->getCurrentUCS().UCSangle() == 0)
			{
				//memcpy(MAINDllOBJECT->Offset_Normal, &OffsetValue[0], 8 * sizeof(int));
				for (int i = 0; i < 8; i++)
					MAINDllOBJECT->Offset_Normal[i] = OffsetValue[i];
			}
			else
			{//memcpy(MAINDllOBJECT->Offset_Skewed, OffsetValue, 8 * sizeof(int));
				for (int i = 0; i < 8; i++)
					MAINDllOBJECT->Offset_Skewed[i] = OffsetValue[i];
			}
			RW_DBSettings::MYINSTANCE()->UpdateCrosshairOffsets();
		}
		MAINDllOBJECT->update_VideoGraphics(false);
	}
	catch (Exception^ ex)
	{

	}
}



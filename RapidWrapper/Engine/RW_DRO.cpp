#include "Stdafx.h"
#include "RW_MainInterface.h"
#include "..\MAINDLL\Actions\AddImageAction.h"
#include "..\MAINDLL\Engine\FocusFunctions.h"
#include "..\MAINDll\Engine\CalibrationCalculation.h"
#include "..\MAINDll\Helper\Helper.h"
#include "..\MAINDll\Shapes\Circle.h"
#include "..\MAINDLL\Engine\PartProgramFunctions.h"
#include "..\MAINDLL\Actions\PerimeterAction.h"
#include "..\MAINDLL\Actions\CircularInterPolationAction.h"
#include "..\MAINDLL\Actions\LinearInterPolationAction.h"
#include "..\MAINDLL\Actions\AreaShapeAction.h"
#include "..\MAINDLL\Engine\SuperImposeImage.h"
#include "..\MAINDLL\Engine\RepeatAutoMeasurement.h"
#include "..\..\MainDLL\Handlers_FrameGrab\SelectPointHandler.h"
#include "RW_AutoProfileScanMeasure.h"
#include "RW_CT_SphereCallibration.h"
#include "RW_DelphiCamModule.h"
#include "RW_LBlock.h"	
#include "..\MAINDLL\Engine\RepeatShapeMeasure.h"
#include "RW_ImageDelayCalibration.h"
#include <iostream>
#include <fstream>
#include "CorrectionZone.h"

//#using "E:\\CTPL\\Sources\\Build3600\\Builds\\x64\\Debug\\CalibrationModule.dll"

//#using "D:\\\Rapid-i\\git\\rapid-i\\Builds\\x64\\Debug\\CalibrationModule.dll"
//#using "E:\\SourceLatest\\SourceLatest\\Builds\\x64\\Debug\\CalibrationModule.dll"

#pragma warning( disable : 4267 )  

double CurrentProbePosition[4] = {0};
int currentDir = 0;
int NumberofAxes = 0;
static int ScaleDirectionCallsCount, MotorDirectionCallsCount;
bool HandlingDROChangedValue;
bool FirstDROValueReceived;
int RotaryScanDelayCount = 0;
int AAxisPositionEventRaised = 0;

//Handle Hardware communications errors..
void CALLBACK DRO_ErrorHandle(char* code, char* fname, char* funName)
{
	try
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog(gcnew String(code), gcnew String(fname), gcnew String(funName));
	}
	catch(Exception^ ex){ }
}

//Handle the target reached,machine connected, disconnected callback..//
void CALLBACK DRO_TargetReached(int index)
{
	try
	{
		RWrapper::RW_DRO::MYINSTANCE()->Raise_DROTargetReached(index);
		RWrapper::RW_DRO::MYINSTANCE()->CurrentDROCallBackFlag = index;
		//RWrapper::RW_DRO::MYINSTANCE()->Handle_DROTargetReached(index);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0001", ex);	}
}

//Update the Lighting button graphics..//
void CALLBACK DRO_UpdateLightingGraphics(char* lightValue)
{
	try{ RWrapper::RW_CNC::MYINSTANCE()->LightCallbackFromDRO(lightValue); }
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0006", ex); }
}

//Handle the Partprogram callback..//Called from framework after finishing the current action
void CALLBACK PartProgramTargetReached()
{
	try
	{
		//if (RWrapper::RW_PartProgram::MYINSTANCE()->SameCommand == 4)
		//	RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_ACTION);
		//else
		MAINDllOBJECT->CNCAssisted_FG(RWrapper::RW_PartProgram::MYINSTANCE()->CNC_Assisted_Mode);
		RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_INCREMENT);
		RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0002", ex);	}
}

//Update the video mouse move on video..
void CALLBACK UpdateMouseMove(double* MousePointer)
{	
	RWrapper::RW_DRO::MYINSTANCE()->DisplayVideoMouseMove(MousePointer);
}

void CALLBACK DRO_CommandSendLog(char* dataString)
{
	RWrapper::CommandLogger^ thisCommandLog = gcnew RWrapper::CommandLogger();
	thisCommandLog->CommandValue = gcnew String(dataString);
	thisCommandLog->Timestamp = DateTime::Now;
	//RWrapper::RW_MainInterface::MYINSTANCE()->WriteCommandSendLog(gcnew String(dataString));
	RWrapper::RW_MainInterface::MYINSTANCE()->WriteCommandSendLog(thisCommandLog);
}

bool CALLBACK PartProgramRunStatus()
{
	bool CycleRunning = false;
	if (RWrapper::RW_DelphiCamModule::MYINSTANCE()->DoingCalibration ||
		RWrapper::RW_DelphiCamModule::MYINSTANCE()->DoingHobMeasurement ||
		RWrapper::RW_SphereCalibration::MYINSTANCE()->DoingCalibration ||
		PPCALCOBJECT->IsPartProgramRunning())
	{
		CycleRunning = true;
	}
	return CycleRunning;
}

bool CALLBACK MessageBoxStatus(char* Msgcode, bool Insert_TempStrFlag, char* TempStr)
{
	return RWrapper::RW_MainInterface::MYINSTANCE()->Update_MsgBoxStatus(gcnew System::String(Msgcode), RWrapper::RW_Enum::RW_MSGBOXTYPE::MSG_OK, RWrapper::RW_Enum::RW_MSGBOXICONTYPE::MSG_INFORMATION, Insert_TempStrFlag, gcnew System::String(TempStr));
}

void CALLBACK Version_NumberCallBack(double* Version_Number, int type)
{
	try
	{
		switch(type)
		{
		case 0: // version number...
			{
				for(int i = 0; i < 5; i++)
				{
					RWrapper::RW_DRO::MYINSTANCE()->HardwareVersionNumber[i] = Version_Number[i];
				}
				/*RWrapper::RW_MainInterface::MYINSTANCE()->WriteCommandSendLog("Card Version Number :");
				for(int i = 0; i < 5; i++)
				{
					char testStr[5] = {"\0"};
					RMATH2DOBJECT->Double2String(Version_Number[i] / 100, 2, testStr); 
					RWrapper::RW_MainInterface::MYINSTANCE()->WriteCommandSendLog(gcnew String(testStr));
				}*/
			}
			break;
		case 1: // Reset ReferenceDot Shape...
			{
				double DroValue[4] = {0};
				for(int i = 0; i < 4; i++)
					DroValue[i] = Version_Number[i];
				PPCALCOBJECT->ResetReferenceDotPosition(DroValue);
			}
			break;
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0002a", ex);	}
}

void CALLBACK ChangeCNC(int  i)
{
	RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(i);
}

RWrapper::RW_DRO::RW_DRO()
{
	try
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->MachineConnectedStatus = true;
		this->AutoFocusMode_Flag = false;
		this->DontTakeDroValues = false;
		this->AutoCalibrationMode = false;
		this->LocalisedCalibrationDone = false;
		this->DontUpdateLocalisedCalibration = false;
		this->HomingisDone = false;
		this->HomingMovementMode = false;
		RW_DRO::DROInst = this;
		MAINDllOBJECT->AddingUCSModeFlag = false;
		this->TakeProbePointFlag = false;
		this->AxisAngle = 0;
		this->AxisAngleXZ = 0;
		this->AxisAngleYZ = 0;
		this->RAxisValue = "";
		this->RAxisGradeValue = "";
		feedRate = (double*)calloc(sizeof(double),  5);
		this->GageAlignmentAxis = 2;
		ScaleDirectionCallsCount = 0;
		//Scale Data EEPROM value stores both motor and scale directions

		this->ReDoScanCount = 0;
		this->RetractProbePointMethod = false;

		NumberofAxes = RWrapper::RW_DBSettings::MYINSTANCE()->NoofMachineAxis;
		if (NumberofAxes < 4) NumberofAxes = 4;
		this->AxisProperty = (bool*)calloc(sizeof(bool), NumberofAxes);
		this->DROCoordinate = (double*)calloc(sizeof(double), NumberofAxes);
		this->OriginalDROCoordinate = (double*)calloc(sizeof(double), NumberofAxes);
		this->ProbeCoordinate = (double*)calloc(sizeof(double), NumberofAxes);
		this->returnPosition = (double*)calloc(sizeof(double), NumberofAxes);
		this->MousePosOnVideo = gcnew cli::array<double>(NumberofAxes);
		this->Last_DROValue = gcnew cli::array<double>(NumberofAxes);
		for(int i = 0; i < 4; i++)
			Last_DROValue[i] = -1000;
		this->HardwareVersionNumber = gcnew cli::array<double>(5);
		for(int i = 0; i < 5; i++)
			this->HardwareVersionNumber[i] = 10;
		this->DROValue = gcnew cli::array<double>(NumberofAxes);
		this->ContourScan_Callibration = false;
		this->Contour_ScalingCoefficient = 0;
		this->Contour_Reference_Rlevel = 0;
		this->Contour_Offset = gcnew cli::array<double>(3);
		this->Contour_AxisOfRotation = gcnew cli::array<double>(3);
		this->CurrentUCSDRO = gcnew cli::array<double>(NumberofAxes);
		this->CurrentUCSDRO_IntUse = gcnew cli::array<double>(NumberofAxes);
		this->AxisLock = (bool*)calloc(sizeof(bool), NumberofAxes);
		this->Corrections = gcnew cli::array<System::Double>(4);
		this->ScaleData = gcnew cli::array<int>(4); 

		for (int i = 0; i < NumberofAxes; i++)
		{
			this->MousePosOnVideo[i] = 0.0;
			this->DROValue[i] = 0.0;
			this->AxisLock[i] = false;
			ScaleDirectionString += "0"; 
			MotorDirectionString += "0";
			JoystickSpeedFactorString += "1";
			Corrections[i] = 0;
			ScaleData[i] = 0x0F80;
		}
		//this->ProbeParams = gcnew System::Collections::Generic::List<ProbeParameters^>();
		GetLocalisedCalibrationDetails();
		GetFocusCalibrationDetails();
		
		MAINDllOBJECT->UpdateMousePositionOnVideo = &UpdateMouseMove;
		PPCALCOBJECT->partProgramReached = &PartProgramTargetReached;
		FOCUSCALCOBJECT->ChangeCncMode = &ChangeCNC;
		MAINDllOBJECT->ChangeCncMode = &ChangeCNC;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0008", ex);	}
}

void RWrapper::RW_DRO::Handle_DRO_AxisValueChanged()
{
	try
	{
		if(RWrapper::RW_DRO::MYINSTANCE()->DontTakeDroValues) return;
		for(int i = 0; i < 4; i++)
			RWrapper::RW_DRO::MYINSTANCE()->OriginalDROCoordinate[i] = DROInstance->DROCoordinateValue[i];
		//System::Threading::ThreadStart^ threadDelegate = gcnew System::Threading::ThreadStart(RWrapper::RW_DRO::MYINSTANCE(), &RWrapper::RW_DRO::Handle_DROCallback);
		//System::Threading::Thread^ t1 = gcnew System::Threading::Thread(threadDelegate);
		////if (!HandlingDROChangedValue)
		//t1->Start();
		RWrapper::RW_DRO::MYINSTANCE()->Handle_DROCallback(DROInstance->DROCoordinateValue);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0031", ex);	}
}


void RWrapper::RW_DRO::Handle_DRO_ProbeHit()
{
	try
	{
		PPCALCOBJECT->MachineDro_ProbeTouch.set(DROInstance->ProbeCoordinate[0], DROInstance->ProbeCoordinate[1], DROInstance->ProbeCoordinate[2], DROInstance->ProbeCoordinate[3]);
		//PPCALCOBJECT->MachineDro_ProbeTouch.set(RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]);
		double currentDro[4] = {0};
		for(int i = 0; i < 4; i++)
			currentDro[i] = DROInstance->ProbeCoordinate[i];
			
		if(RWrapper::RW_SphereCalibration::MYINSTANCE()->DoingCalibration)
		{
			if(RWrapper::RW_SphereCalibration::MYINSTANCE()->TakeProbeHit)
			{
				RWrapper::RW_SphereCalibration::MYINSTANCE()->ProbeHitHandle(DROInstance->Probe_Hit_Direction, currentDro);
				RWrapper::RW_SphereCalibration::MYINSTANCE()->TakeProbeHit = false;
				RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = false;
				if (HandleTarget == TargetReachedCallback::SPHERECALIBRATIONMANUALPOINT)
					RWrapper::RW_SphereCalibration::MYINSTANCE()->SendDrotoNextposition();
			}
			return;
		}
		//if (RWrapper::RW_DelphiCamModule::MYINSTANCE()->ManualPoint)
		//{
		//	for (int i = 0; i < 4; i++)
		//		ProbeCoordinate[i] = currentDro[i];
		//	RWrapper::RW_DelphiCamModule::MYINSTANCE()->Get_Cylinder_Y_Position();
		//	return;
		//}

		if(!RWrapper::RW_DelphiCamModule::MYINSTANCE()->DoingCalibration)
			RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_PROBEGOBACK);
		
		if(MAINDllOBJECT->AutoProbeMeasureFlag)
			 RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PROBE_MEASURE_GOBACK);		
		
		if(RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 0)
		{
			if(!DROInstance->ProbeHitRaiseFlag)
			{
				MAINDllOBJECT->SetStatusCode("RW_DRO01");
				return;
			}
		}
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
			if(PPCALCOBJECT->ProgramMadeUsingTwoStepHoming() && !PPCALCOBJECT->FinishedManualTwoStepAlignment())
			{
				RWrapper::RW_DRO::MYINSTANCE()->HandleProbeHitCallback((int)fg->ProbeDirection, currentDro);
				RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = false;
			}
			else if(PPCALCOBJECT->FirstProbePointActionFlag && !PPCALCOBJECT->ActionTobeTransformed)
			{
				RWrapper::RW_DRO::MYINSTANCE()->HandleProbeHitCallback((int)fg->ProbeDirection, currentDro);
				RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = false;
			}
			else
			{
				if(RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 0)
				{
					if(!RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag)
					return;
				}			
				RWrapper::RW_DRO::MYINSTANCE()->HandleProbeHitCallback((int)fg->ProbeDirection, currentDro);
				RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = false;
			}
		}
		else
		{
			RWrapper::RW_DRO::MYINSTANCE()->HandleProbeHitCallback(DROInstance->Probe_Hit_Direction, currentDro);
		}
		
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0033", ex);	}
}

bool RWrapper::RW_DRO::ConnectTo_RapidIhardware()
{
	bool ConnectedToHardware = false;
	try
	{
		System::String^ SfilePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath;
		System::String^ DROCommnadSendPath = SfilePath + "\\Log\\CommandSend\\CommandSent " + Microsoft::VisualBasic::DateAndTime::Now.ToString("dd-MMM-yyyy") + ".txt";
		if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ROTARY_DEL_FI)
		{
			System::String^ SetFileName = SfilePath + "\\Database\\PortSettings.txt";
			gcnew RWrapper::RW_LaserComm(SetFileName);
			RWrapper::RW_LaserComm::MYINSTANCE()->InitialisePortComm();
		}
							
		System::String^ portname = GetCommPortName(); //"COM3"
		int length = portname->Length + 1;
		
		/*if(portname == "")
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->MachineConnectedStatus = false;
			System::Windows::Forms::MessageBox::Show("Please Install Rapid-I USB Driver", "Rapid-I");
		}*/
		if(DigitalReadOut::CreateInstance((char*)Marshal::StringToHGlobalAnsi(DROCommnadSendPath).ToPointer(), (char*)Marshal::StringToHGlobalAnsi(portname).ToPointer(), length))
		{
			DROInstance->DROErrorCallback = &DRO_ErrorHandle;
			DROInstance->DROTargetReachedCallback = &DRO_TargetReached;
			DROInstance->DROUpdateLightIntensity = &DRO_UpdateLightingGraphics;
			DROInstance->CommandSendLog = &DRO_CommandSendLog;
			DROInstance->CheckPartProgramRunningStatus = &PartProgramRunStatus;
			DROInstance->ShowMessageBox = &MessageBoxStatus;
			DROInstance->WriteVersionNumber = &Version_NumberCallBack;
			DROInstance->setNumberofAxis(4);
			GetDROSettingsValues(true);
			DROInstance->ResetMachineStartFalg();
			if (RWrapper::RW_DBSettings::MYINSTANCE()->CurrentMachineNumber->StartsWith("CS")) // Added this on 17 Aug2016 for Online System. DRO will go into error and endless loop for online systems if not initialised as itself. 
				DROInstance->SetMachineCardFlag(3);
			ConnectedToHardware = DROInstance->ConnectToHardware();
			if (MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::VISIONHEADONLY)
			{
				MAINDllOBJECT->SetProfileLightMode(true);
				MAINDllOBJECT->SetSurfaceLightMode(false);
			}
			//if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
			//	RW_DRO::MYINSTANCE()->ConnectOnlineSystem(false);
			//DROInstance->beginThread();

		}
		//RWrapper::RW_DRO::MYINSTANCE()->UpdateDRO_textbox();
		//RWrapper::RW_DRO::MYINSTANCE()->UpdateCenterScreen();

	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0008", ex);	}
	return ConnectedToHardware;
}

void RWrapper::RW_DRO::GetDROSettingsValues(bool updateFeedRate)
{
	CalibrationModule::DB ^GetValues = gcnew CalibrationModule::DB(RWrapper::RW_MainInterface::DBConnectionString);
	try
	{
		cli::array<System::String^>^ TableNames = { "VisionSettings", "UserSettings", "DROSettings", "MachineDetails", "EEPROMSettings", "LocalisedCorrections", "LC_Params", "LC_Details", "LC_Multiplier_Details", "LC_Peaks", "TIS_Settings", "OneShotParams", "ProbeSettings" };
		GetValues->FillTableRange(TableNames);
		System::String^ MachineNo = RWrapper::RW_DBSettings::MYINSTANCE()->CurrentMachineNumber;
		AxisAngle = System::Convert::ToDouble(GetValues->Select_ChildControls("VisionSettings", "MachineNo", MachineNo)[0]->default["CameraSkew"]);
		AxisAngleXZ = System::Convert::ToDouble(GetValues->Select_ChildControls("VisionSettings", "MachineNo", MachineNo)[0]->default["CameraSkewXZ"]);
		AxisAngleYZ = System::Convert::ToDouble(GetValues->Select_ChildControls("VisionSettings", "MachineNo", MachineNo)[0]->default["CameraSkewYZ"]);
		if (updateFeedRate)
		{
			feedRate[0] = RWrapper::RW_DBSettings::MYINSTANCE()->SpeedX;//System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["SpeedX"]);
			feedRate[1] = RWrapper::RW_DBSettings::MYINSTANCE()->SpeedY;//System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["SpeedY"]);
			feedRate[2] = RWrapper::RW_DBSettings::MYINSTANCE()->SpeedZ;//System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["SpeedZ"]);
			feedRate[3] = RWrapper::RW_DBSettings::MYINSTANCE()->SpeedR;//System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["SpeedR"]);
		}

		int DroCycleCount = System::Convert::ToInt32(GetValues->Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]->default["LimitHitCount"]);
		DROInstance->SetDroCycleCount(DroCycleCount);
		int CommunicationLostCount = System::Convert::ToInt32(GetValues->Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]->default["MachineDisconnectCount"]);
		DROInstance->SetMachineDisconnectCount(CommunicationLostCount);

		int writedelay = System::Convert::ToInt32(GetValues->Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]->default["CommandSendDelay"]);
		DROInstance->Axis_Accuracy[0] = System::Convert::ToDouble(GetValues->Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]->default["Accuracy_X"]) / 1000;
		DROInstance->Axis_Accuracy[1] = System::Convert::ToDouble(GetValues->Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]->default["Accuracy_Y"]) / 1000;
		DROInstance->Axis_Accuracy[2] = System::Convert::ToDouble(GetValues->Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]->default["Accuracy_Z"]) / 1000;
		DROInstance->Axis_Accuracy[3] = System::Convert::ToDouble(GetValues->Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]->default["Accuracy_R"]) / 1000;

		cli::array<System::Data::DataRow^>^ DROROWS = GetValues->Select_ChildControls("DROSettings", "MachineNo", MachineNo);
		//System::String^ droLabels = "";
		double lashValue[4], axisMultiplier[4], countsPerunit[4];
		bool movementType[4], movementDirection[4], MoveOppToVision[4];
		//RW_DRO axis details...///
		for (int i = 0; i < DROROWS->Length; i++)
		{
			//droLabels += System::Convert::ToString(DROROWS[i]->default["AxisName"]);
			movementType[i] = System::Convert::ToBoolean(DROROWS[i]->default["AxisType"]);
			AxisProperty[i] = movementType[i];
			countsPerunit[i] = System::Convert::ToDouble(DROROWS[i]->default["AxisCount"]);
			axisMultiplier[i] = System::Convert::ToDouble(DROROWS[i]->default["AxisMultiplier"]);
			lashValue[i] = System::Convert::ToDouble(DROROWS[i]->default["AxisLash"]);
			movementDirection[i] = System::Convert::ToBoolean(DROROWS[i]->default["AxisPolarity"]);
			MoveOppToVision[i] = System::Convert::ToBoolean(DROROWS[i]->default["MoveOppositeToVision"]);
		}

		XAxisHomeToPositive = System::Convert::ToBoolean(DROROWS[0]->default["AxisHomeToPositive"]);
		YAxisHomeToPositive = System::Convert::ToBoolean(DROROWS[1]->default["AxisHomeToPositive"]);
		XAxisPositiveHomeValue = System::Convert::ToInt32(DROROWS[0]->default["AxisPositiveHomeValue"]);
		YAxisPositiveHomeValue = System::Convert::ToInt32(DROROWS[1]->default["AxisPositiveHomeValue"]);
		if (XAxisPositiveHomeValue == 0)
			XAxisPositiveHomeValue = 150;
		if (YAxisPositiveHomeValue == 0)
			YAxisPositiveHomeValue = 150;
		GetValues->FinalizeClass();
		DROInstance->setDelayBetweenCommands(writedelay);
		DROInstance->setDROProperties(&movementType[0], &movementDirection[0], &lashValue[0], &axisMultiplier[0], &countsPerunit[0], &MoveOppToVision[0]);
		DROInstance->setNumberofMachineAxis(RWrapper::RW_DBSettings::MYINSTANCE()->NoofMachineAxis);
		DROInstance->DROJumpThreshold = System::Convert::ToDouble(GetValues->Select_ChildControls("MachineDetails", "MachineNo", MachineNo)[0]->default["MesureNoOfDec"]);
		
		if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT)
		{
			try
			{			//Update Lighting control values 
						//System::String^ tableN = "OneShotParams";
						//GetValues->FillTable(tableN);
				double ProfileSlope = System::Convert::ToDouble(GetValues->Select_ChildControls("OneShotParams", "MachineNo", MachineNo)[0]->default["Profile_Slope"]);
				double ProfileIntercept = System::Convert::ToDouble(GetValues->Select_ChildControls("OneShotParams", "MachineNo", MachineNo)[0]->default["Profile_BaseValue"]);
				double SurfaceSlope = System::Convert::ToDouble(GetValues->Select_ChildControls("OneShotParams", "MachineNo", MachineNo)[0]->default["Surface_Slope"]);
				double SurfaceBaseValue = System::Convert::ToDouble(GetValues->Select_ChildControls("OneShotParams", "MachineNo", MachineNo)[0]->default["Surface_BaseValue"]);
				DROInstance->SetOneShotLightParams(ProfileSlope, ProfileIntercept, SurfaceSlope, SurfaceBaseValue);
			}
			catch (Exception^ exxx)
			{
			}
		}

		
		RWrapper::RW_MainInterface::MYINSTANCE()->Do_Homing_At_Startup = System::Convert::ToBoolean(GetValues->Select_ChildControls("VisionSettings", "MachineNo", MachineNo)[0]->default["AutoHoming_at_Start"]);

		FirstDROValueReceived = true;
		//Read and instantiate Localised Corrections for C-Axis...
		//if (RWrapper::RW_LCParams::MYINSTANCE != NULL) RWrapper::RW_LCParams::MYINSTANCE()->~RW_LCParams();
		cli::array<System::Data::DataRow^> ^thisMachineCorrections = GetValues->Select_ChildControls("LocalisedCorrections", "MachineNo", MachineNo);
		//	
		DataTable^ CorrectionTable = GetValues->GetTable("LocalisedCorrections");

		//If we have any entries,
		if (thisMachineCorrections->Length > 0)
		{
			this->CorrectionLookup = gcnew cli::array<RW_LCParams^>(2);
			for (int i = 0; i < 2; i++)
			{
				this->CorrectionLookup[i] = gcnew RWrapper::RW_LCParams(i);
				//Now filter out and get entries for this axis - 
				//System::Collections::Generic::List<System::Data::DataRow^> ^CorrParamsForThisAxis = gcnew System::Collections::Generic::List<System::Data::DataRow^>();
				cli::array<System::Data::DataRow ^> ^ThisAxisParams = CorrectionTable->Select("MachineNo = '" + MachineNo + "' AND AxisName = " + i, "Position ASC");
				this->CorrectionLookup[i]->SetCorrectionParams(ThisAxisParams);
			}
		}

		//Make the main Hastable lists for storing the Zone correction objects...
		//this->LC_CorrectionZoneCollection = gcnew System::Collections::Generic::List<System::Collections::Hashtable^>();
		this->LC_CorrectionZoneCollection = gcnew System::Collections::Hashtable();

		cli::array<System::Data::DataRow^> ^LC_Param_Entries = GetValues->Select_ChildControls("LC_Params", "MachineNo", MachineNo);
		if (LC_Param_Entries->Length > 0)
		{

			//We have localised corrections to be done... So let us get the Details and instantiate and calculate the parameters...
			//First get the table of values - 
			DataTable^ LC_Details = GetValues->GetTable("LC_Details");
			/*	We will always have a square matrix!!
				Because once we have a decision to correct X axis when Y moves, we HAVE to correct
				Y-axis when X moves!
				Linear multiplier corrections are done as corrections in X when X moves, and Y when Y moves.
				Hence we have 4 entries when we do 2-axis corrections, and 9 entries when we do 3-axis corrections
			*/
			this->gridSize = gcnew cli::array<double>(LC_Param_Entries->Length);
			this->Range = gcnew cli::array<double>(LC_Param_Entries->Length);
			double AA;
			//cli::array<bool>^ DoCompleteCorrections = gcnew cli::array<bool>(LC_Param_Entries->Length);

			for (int i = 0; i < LC_Param_Entries->Length; i++)
			{
				gridSize[i] = Convert::ToDouble(LC_Param_Entries[i]["GridSize"]);
				Range[i] = Convert::ToDouble(LC_Param_Entries[i]["Range"]);
				//DoCompleteCorrections[i] = Convert::ToBoolean(LC_Param_Entries[i]["CompleteGridCorrection"]);
			}
			int X_CellCount = Range[0] / gridSize[0];
			int Y_CellCount = Range[1] / gridSize[1];
			//Now let us get all the entries for this machine no.
			cli::array<System::Data::DataRow ^> ^LC_Details_Current = LC_Details->Select("MachineNo = '" + MachineNo + "'"); //, "PrincipalPosition ASC");

			if (LC_Details_Current->Length > 0)
			{
				System::Collections::Generic::List<RWrapper::RegionalDeviations^>^ RD_Coll = gcnew System::Collections::Generic::List<RegionalDeviations^>();
				//instantiate a correctionzone and load values for those 
				for (int i = 0; i < LC_Details_Current->Length; i++)
				{
					String^ thisIndex = LC_Details_Current[i]["Pos_X"]->ToString() + ":" + LC_Details_Current[i]["Pos_Y"]->ToString();
					//DeviationValues[col] = Convert::ToDouble(LC_Details_Current[col]["Deviation"]);
					CorrectionZone^ thisZone;
					if (this->LC_CorrectionZoneCollection->Contains(thisIndex))
					{
						thisZone = (CorrectionZone^)this->LC_CorrectionZoneCollection[thisIndex];
					}
					else
					{
						//We do not have an instance. Lets make one and add it to the hashtable....
						thisZone = gcnew CorrectionZone(i, LC_Param_Entries->Length);
						thisZone->Index = thisIndex;
						this->LC_CorrectionZoneCollection->Add(thisIndex, thisZone);
					}
					////Import axis angle to this zone
					//AA = Convert::ToDouble(LC_Details_Current[i]["AA_XY"]);
					////if (thisZone->AxisAngles[0] == 0.0 && AA != 0.0)
					//	thisZone->AxisAngles[0] = AA;
					//AA = Convert::ToDouble(LC_Details_Current[i]["AA_XZ"]);
					////if (thisZone->AxisAngles[1] == 0.0 && AA != 0.0)
					//	thisZone->AxisAngles[1] = AA;
					//AA = Convert::ToDouble(LC_Details_Current[i]["AA_XZ"]);
					////if (thisZone->AxisAngles[2] == 0.0 && AA != 0.0)
					//	thisZone->AxisAngles[2] = AA;
					//thisZone->Deviations[0] = Convert::ToDouble(LC_Details_Current[i]["D_XX"]);
					//thisZone->Deviations[1] = Convert::ToDouble(LC_Details_Current[i]["D_XY"]);
					//thisZone->Deviations[2] = Convert::ToDouble(LC_Details_Current[i]["D_YX"]);
					//thisZone->Deviations[3] = Convert::ToDouble(LC_Details_Current[i]["D_YY"]);
					RegionalDeviations^ thisDev = gcnew RegionalDeviations;
					thisDev->index = thisIndex;
					thisDev->pos_x = Convert::ToInt32(LC_Details_Current[i]["Pos_X"]);
					thisDev->pos_y = Convert::ToInt32(LC_Details_Current[i]["Pos_Y"]);
					thisDev->d_xx = Convert::ToDouble(LC_Details_Current[i]["D_XX"]);
					thisDev->d_xy = Convert::ToDouble(LC_Details_Current[i]["D_XY"]);
					thisDev->d_yx = Convert::ToDouble(LC_Details_Current[i]["D_YX"]);
					thisDev->d_yy = Convert::ToDouble(LC_Details_Current[i]["D_YY"]);
					RD_Coll->Add(thisDev);
				}

				//Instantiate correctionZone's for the rest of the cells...
				for (int j = 0; j < Y_CellCount; j++)
				{
					//if (!DoCompleteCorrections[0] && j > 0) break;
					for (int i = 0; i < X_CellCount; i++)
					{
						//if (!DoCompleteCorrections[1] && i > 0) break;
						String^ thisIndex = i.ToString() + ":" + j.ToString();
						CorrectionZone^ thisZone;
						if (this->LC_CorrectionZoneCollection->Contains(thisIndex))
						{
							thisZone = (CorrectionZone^)this->LC_CorrectionZoneCollection[thisIndex];
						}
						else
						{
							//We do not have an instance. Lets make one and add it to the hashtable....
							thisZone = gcnew CorrectionZone(i, LC_Param_Entries->Length);
							thisZone->Index = thisIndex;
							this->LC_CorrectionZoneCollection->Add(thisIndex, thisZone);
							//Since we did not have an entry for this, we have to instantiate the error struct for this.
							RegionalDeviations^ thisDev = gcnew RegionalDeviations;
							thisDev->index = thisIndex;
							thisDev->pos_x = i; thisDev->pos_y = j;
							//if (i > 0 && j > 0)
							//{
							for (int k = 0; k < RD_Coll->Count; k++)
							{
								if (j > 0 && RD_Coll[k]->pos_x == i && RD_Coll[k]->pos_y == j - 1)
								{
									thisDev->d_xx = RD_Coll[k]->d_xx;
									thisDev->d_yx = RD_Coll[k]->d_yx;
								}
								if (i > 0 && RD_Coll[k]->pos_x == i - 1 && RD_Coll[k]->pos_y == j)
								{
									thisDev->d_xy = RD_Coll[k]->d_xy;
									thisDev->d_yy = RD_Coll[k]->d_yy;
								}
							}
							//}
							RD_Coll->Add(thisDev);
						}
					}
				}


				for each(System::Collections::DictionaryEntry Entry in this->LC_CorrectionZoneCollection)
				{
					CorrectionZone^ thisZone = (CorrectionZone^)Entry.Value;
					cli::array<System::String^, 1> ^splitter = { ":" };
					cli::array<System::String^> ^Indices = thisZone->Index->Split(splitter, 2, System::StringSplitOptions::None);
					cli::array<double>^ currentDev = { 0,0,0,0 };
					cli::array<double>^ prevDev = { 0,0,0,0 };

					int Row = int::Parse(Indices[0]);
					int Col = int::Parse(Indices[1]);
					System::String^ PrevIndex;
					//System::String^ CurrentIndex = Row.ToString() + ":" + Col.ToString();// , PrevIndex_Y;
					//Get all deviations of the current cell
					for (int i = 0; i < RD_Coll->Count; i++)
					{
						if (RD_Coll[i]->index == thisZone->Index)
						{
							currentDev = RD_Coll[i]->ToDoubleArray();
							break;
						}
					}
					if (Row > 0)
					{
						//PrevIndex = (Row - 1).ToString() + ":" + Col.ToString();
						//FIrst we get the deviations because of movement in X
						for (int i = 0; i < RD_Coll->Count; i++)
						{
							if (RD_Coll[i]->pos_y == Col && RD_Coll[i]->pos_x == Row - 1)
							{
								//if (RD_Coll[i]->index == PrevIndex)
								//{
								prevDev[0] += RD_Coll[i]->d_xx; //Correction in X because of movement in X
								prevDev[2] += RD_Coll[i]->d_yx; //Correction in Y because of movement in X 
								//break;
							//}
							}
						}
					}
					//Now get deviations because of movement in Y
					if (Col > 0)
					{
						//PrevIndex = Row.ToString() + ":" + (Col - 1).ToString();
						for (int i = 0; i < RD_Coll->Count; i++)
						{
							//if (RD_Coll[i]->index == PrevIndex)
							if (RD_Coll[i]->pos_x == Row && RD_Coll[i]->pos_y == Col - 1)
							{
								prevDev[1] += RD_Coll[i]->d_xy;
								prevDev[3] += RD_Coll[i]->d_yy;
								//break;
							}
						}
					}
					//if (thisZone->Index == "1:0")
					thisZone->CalculateCountCorrectionParameters(this->gridSize, currentDev, prevDev);
				}
			}
		}

		this->LC_MultiplierList = gcnew System::Collections::Generic::List<LC_Multiplier^>();
		cli::array<System::Data::DataRow^> ^LC_Multiplier_Entries = GetValues->Select_ChildControls("LC_Multiplier_Details", "MachineNo", MachineNo);
		if (LC_Multiplier_Entries->Length > 0)
		{
			//cli::array<double>^ Positions = gcnew cli::array<double>(6);
			//cli::array<double>^ MultiplierCorrections = gcnew cli::array<double>(2);
			System::Data::DataTable^ LCpTable = GetValues->GetTable("LC_Peaks");
			for (int i = 0; i < LC_Multiplier_Entries->Length; i++)
			{
				cli::array<System::Data::DataRow^> ^LC_Multiplier_Peaks = LCpTable->Select("LC_Multiplier_Details = " + Convert::ToInt32(LC_Multiplier_Entries[i]["LC_Multiplier_Details"]), "Pos ASC"); //GetValues->Select_ChildControls("LC_Peaks", "LC_Multiplier_Details", LC_Multiplier_Entries[i]["LC_Multiplier_Details"]);
				LC_Multiplier^ MultiplierInstance = gcnew LC_Multiplier(2);

				//Positions[0] = ;
				RWrapper::LC_Deviations^ DevStart = gcnew RWrapper::LC_Deviations;
				DevStart->Position = Convert::ToDouble(LC_Multiplier_Entries[i]["Start_X"]);
				System::Collections::Generic::List<RWrapper::LC_Deviations^> ^Dev_X = (System::Collections::Generic::List<RWrapper::LC_Deviations^>^) MultiplierInstance->Deviations[0];
				System::Collections::Generic::List<RWrapper::LC_Deviations^> ^Dev_Y = (System::Collections::Generic::List<RWrapper::LC_Deviations^>^) MultiplierInstance->Deviations[1];
				Dev_X->Add(DevStart);
				DevStart = gcnew RWrapper::LC_Deviations;
				DevStart->Position = Convert::ToDouble(LC_Multiplier_Entries[i]["Start_Y"]);
				Dev_Y->Add(DevStart);

				if (LC_Multiplier_Peaks->Length > 0)
				{
					for (int j = 0; j < LC_Multiplier_Peaks->Length; j++)
					{
						RWrapper::LC_Deviations^ thisDev = gcnew RWrapper::LC_Deviations;
						thisDev->Position = Convert::ToDouble(LC_Multiplier_Peaks[j]["Pos"]);
						thisDev->Deviation = Convert::ToDouble(LC_Multiplier_Peaks[j]["Dev"]);
						if (Convert::ToInt32(LC_Multiplier_Peaks[j]["AxisNumber"]) == 0)
							Dev_X->Add(thisDev);
						else
							Dev_Y->Add(thisDev);
					}
				}
				else
				{
					RWrapper::LC_Deviations^ DevPeak = gcnew RWrapper::LC_Deviations;
					DevPeak->Position = Convert::ToDouble(LC_Multiplier_Entries[i]["Peak_X"]);
					DevPeak->Deviation = Convert::ToDouble(LC_Multiplier_Entries[i]["Multiplier_X"]);
					Dev_X->Add(DevPeak);
					DevPeak = gcnew RWrapper::LC_Deviations;
					DevPeak->Position = Convert::ToDouble(LC_Multiplier_Entries[i]["Peak_Y"]);
					DevPeak->Deviation = Convert::ToDouble(LC_Multiplier_Entries[i]["Multiplier_Y"]);
					Dev_Y->Add(DevPeak);
				}
				//Positions[1] = Convert::ToDouble(LC_Multiplier_Entries[i]["Peak_X"]);
				//Positions[2] = Convert::ToDouble(LC_Multiplier_Entries[i]["End_X"]);
				RWrapper::LC_Deviations^ DevEnd = gcnew RWrapper::LC_Deviations;
				DevEnd->Position = Convert::ToDouble(LC_Multiplier_Entries[i]["End_X"]);
				Dev_X->Add(DevEnd);
				DevEnd = gcnew RWrapper::LC_Deviations;
				DevEnd->Position = Convert::ToDouble(LC_Multiplier_Entries[i]["End_Y"]);
				Dev_Y->Add(DevEnd);
				//Positions[4] = Convert::ToDouble(LC_Multiplier_Entries[i]["Peak_Y"]);
				//Positions[5] = Convert::ToDouble(LC_Multiplier_Entries[i]["End_Y"]);
				//MultiplierCorrections[0] = Convert::ToDouble(LC_Multiplier_Entries[i]["Multiplier_X"]);
				//MultiplierCorrections[1] = Convert::ToDouble(LC_Multiplier_Entries[i]["Multiplier_Y"]);
				MultiplierInstance->CalculateMultiplierCorrectionParameters(); // Positions, MultiplierCorrections);
				this->LC_MultiplierList->Add(MultiplierInstance);
			}
		}


		//for (int i = 0; i < LC_Param_Entries->Length; i++)
		//{
		//	//System::Collections::Hashtable^ thisAxisZoneCollection = gcnew System::Collections::Hashtable();
		//	//Get GridSize depending on the current axis...
		//	int RowCount = 0; //Number of Rows in the movement axis we have to have based on grid size and range in that axis
		//	int ColumnCount = 0; // Convert::ToDouble(LC_Param_Entries[i]["Range"]) / Convert::ToDouble(LC_Param_Entries[i]["GridSize"]);
		//	int currentGSIndex = 0;
		//	//if (i < LC_Param_Entries->Length - 1)
		//	//	currentGSIndex = i + 1;

		//	//Iterate for corrections for each of the axes....
		//	for (int j = 0; j < LC_Param_Entries->Length; j++)
		//	{
		//		currentGSIndex = j + 1;
		//		if (currentGSIndex == LC_Param_Entries->Length) currentGSIndex = 0;
		//		//Number of Rows in the movement axis, not principal axis. This holds the value we have to select from the Table
		//		RowCount = Range[currentGSIndex] / gridSize[currentGSIndex];
		//		ColumnCount = Range[j] / gridSize[j];
		//		//Now we iterate through each "Row" of movement axis and generate a set of columns in the principal axis
		//		for (int row = 0; row < RowCount; row++)
		//		{
		//			//Check for Axial correction.... 
		//			cli::array<System::Data::DataRow ^> ^LC_Details_Current = LC_Details->Select("MachineNo = '" + MachineNo +
		//				"' AND Principal_Axis = " + i +
		//				" AND Movement_Axis = " + j +
		//				" AND MovementPosition = " + (row + 1), "PrincipalPosition ASC");

		//			//if (LC_Details_Current->Length != ColumnCount)
		//			//{
		//			//	//We probably have orthogonal correction. Lets check
		//			//	LC_Details_Current = LC_Details->Select("MachineNo = '" + MachineNo +
		//			//		"' AND Principal_Axis = " + i +
		//			//		" AND Movement_Axis = " + j +
		//			//		" AND PrincipalPosition = " + (row + 1), "MovementPosition ASC");
		//			//}
		//			//	//if we dont have enough entries, we continue ....got the entries and their values... 
		//			//if (LC_Details_Current->Length != ColumnCount) continue;
		//			
		//			cli::array<double>^ DeviationValues = gcnew cli::array<double>(ColumnCount);
		//			for (int col = 0; col < LC_Details_Current->Length; col++)
		//			{						//Now let us see if we have a CorrectionZone instance for this. IF not we make one.
		//				String^ thisIndex = col.ToString() + ":" + row.ToString();
		//				DeviationValues[col] = Convert::ToDouble(LC_Details_Current[col]["Deviation"]);
		//				CorrectionZone^ thisZone;
		//				if (this->LC_CorrectionZoneCollection->Contains(thisIndex))
		//				{
		//					thisZone = (CorrectionZone^)this->LC_CorrectionZoneCollection[thisIndex];
		//				}
		//				else
		//				{
		//					//We do not have an instance. Lets make one and add it to the hashtable....
		//					thisZone = gcnew CorrectionZone(i, LC_Param_Entries->Length);
		//					thisZone->Index = thisIndex;
		//					this->LC_CorrectionZoneCollection->Add(thisIndex, thisZone);
		//				}
		//				//Import axis angle to this zone
		//				AA = Convert::ToDouble(LC_Details_Current[col]["AA_XY"]);
		//				if (thisZone->AxisAngles[0] == 0.0 && AA != 0.0)
		//					thisZone->AxisAngles[0] = AA;
		//				AA = Convert::ToDouble(LC_Details_Current[col]["AA_XZ"]);
		//				if (thisZone->AxisAngles[1] == 0.0 && AA != 0.0)
		//					thisZone->AxisAngles[1] = AA;
		//				AA = Convert::ToDouble(LC_Details_Current[col]["AA_XZ"]);
		//				if (thisZone->AxisAngles[2] == 0.0 && AA != 0.0)
		//					thisZone->AxisAngles[2] = AA;

		//				double prevDev = 0;
		//				if (col > 0) prevDev = Convert::ToDouble(LC_Details_Current[col - 1]["Deviation"]);
		//				thisZone->CalculateCorrectionParameters(gridSize[i], j,
		//					Convert::ToDouble(LC_Details_Current[col]["Deviation"]), prevDev,
		//					Convert::ToDouble(LC_Details_Current[col]["PrincipalPosition"]) * gridSize[j], i);
		//			}
		//		}
		//	}
			//this->LC_CorrectionZoneCollection->Add(thisAxisZoneCollection);
		//}


		//		int count = 0;
		//		//DataTable^ C_AxisTable = GetValues->GetTable("LocalisedCorrections")->Clone(); //gcnew DataTable
		//		////Let us read the data from a csv file until we complete calibration...
		//		//System::IO::StreamReader^ csvreader = gcnew System::IO::StreamReader(RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath + "\\Database\\LCValues.csv");
		//		ArrayList^ TempValuesFromTable = gcnew ArrayList;
		//		//while (!csvreader->EndOfStream)
		//		//{
		//		//	cli::array<System::String^>^ thisReading = csvreader->ReadLine()->Split(',');
		//		//	cli::array<double>^ CurrentReading = gcnew array < double > {Convert::ToDouble(thisReading[3]), Convert::ToDouble(thisReading[4])};
		//		//	TempValuesFromTable->Add(CurrentReading);
		//		//	//DataRow ^dr = C_AxisTable->NewRow();
		//		//	//for (int j = 0; j < C_AxisTable->Columns->Count; j++)
		//		//	//	dr[j] = thisReading[j];
		//		//	//C_AxisTable->Rows->Add(dr);
		//		//	count++;
		//		//}
		//		//csvreader->Close();
		//		cli::array<double, 2> ^ValuesFromTable = gcnew cli::array<double, 2>(count, 2);
		//		//for (int i = 0; i < count; i++)
		//		//{
		//		//	cli::array<double>^ CurrentReading = (cli::array<double>^)TempValuesFromTable[i];
		//		//	ValuesFromTable[i, 0] = CurrentReading[0];
		//		//	ValuesFromTable[i, 1] = CurrentReading[1];
		//		//}
		//		//Read from dB and import.... Permanent solution....
		//		for (int i = 0; i < thisMachineCorrections->Length; i++)
		//		{
		//			if (System::Convert::ToInt32(thisMachineCorrections[i]["AxisName"]) == 3)
		//			{
		//				DataRow ^dr = C_AxisTable->NewRow();
		//				for (int j = 0; j < C_AxisTable->Columns->Count; j++)
		//					dr[j] = thisMachineCorrections[i][j];

		//				C_AxisTable->Rows->Add(dr);
		//				count++;
		//			}
		//		}
		//		//cli::array<double, 2> ^ValuesFromTable = gcnew cli::array<double, 2>(count, 2);
		//		//thisMachineCorrections = C_AxisTable->Select("MachineNo = '" + MachineNo + "'", "Position ASC");
		//		//for (int i = 0; i < thisMachineCorrections->Length; i++)
		//		//{
		//		//	ValuesFromTable[i, 0] = System::Convert::ToDouble(thisMachineCorrections[i]["Position"]);
		//		//	ValuesFromTable[i, 1] = System::Convert::ToDouble(thisMachineCorrections[i]["CorrectionValue"]);
		//		//}
		//		RWrapper::RW_LCParams::MYINSTANCE()->IsRotaryAxis = true;
		//		RWrapper::RW_LCParams::MYINSTANCE()->SetCorrectionParams(ValuesFromTable);
		//	}
		//}
	//}
	//System::Windows::MessageBox::Show(this->LC_CorrectionZoneCollection->Count.ToString());

		RW_CNC::MYINSTANCE()->RotaryScanSpan = System::Convert::ToDouble(GetValues->Select_ChildControls("TIS_Settings", "MachineNo", MachineNo)[0]->default["RotaryScanSpan"]) * M_PI / 180;
		RW_CNC::MYINSTANCE()->RotaryScanSpeed = System::Convert::ToDouble(GetValues->Select_ChildControls("TIS_Settings", "MachineNo", MachineNo)[0]->default["RotaryScanSpeed"]);
		//Double rsspan = RW_CNC::MYINSTANCE()->RotaryScanSpan;
		RWrapper::RW_DRO::MYINSTANCE()->ApproachDistance = System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["ProbeOffsetValueOnwards"]);
		RWrapper::RW_DRO::MYINSTANCE()->ApproachSpeed = System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["ProbeSpeedOnwards"]);
		RWrapper::RW_DRO::MYINSTANCE()->RevertDistance = System::Convert::ToDouble(GetValues->Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]->default["ProbeRevertDistance"]);
		RWrapper::RW_DRO::MYINSTANCE()->RevertSpeed = System::Convert::ToDouble(GetValues->Select_ChildControls("EEPROMSettings", "MachineNo", MachineNo)[0]->default["ProbeRevertSpeed"]);
		RWrapper::RW_DRO::MYINSTANCE()->ProbeSensitivity = System::Convert::ToDouble(GetValues->Select_ChildControls("ProbeSettings", "MachineNo", MachineNo)[0]->default["ProbeSensitivity"]);


	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0008a", ex);
	}
	GetValues->FinalizeClass();
}

void RWrapper::RW_DRO::GetLocalisedCalibrationDetails()
{
	CalibrationModule::DB ^GetValues = gcnew CalibrationModule::DB(RWrapper::RW_MainInterface::DBConnectionString);
	try
	{
		cli::array<System::String^>^ TableNames = {"CalibrationSettings", "UserSettings"};
		GetValues->FillTableRange(TableNames);	
		System::String^ MachineNo = RWrapper::RW_DBSettings::MYINSTANCE()->CurrentMachineNumber;
		cli::array<System::Data::DataRow^>^ cbsrows = GetValues->Select_ChildControls("CalibrationSettings", "MachineNo", MachineNo);
		MAINDllOBJECT->UseLocalisedCorrection = System::Convert::ToDouble(GetValues->Select_ChildControls("UserSettings", "MachineNo", MachineNo)[0]->default["UseLocalisedCorrection"]);
		double slipgaugeLength = 0, Nextcellgap = 0, correction_X, correction_Y;
		int rcount = 0, ccount = 0;
		System::String^ CMachineType = "";
		if(cbsrows->Length == 0) // Calibration Not Done..
		{
			this->LocalisedCalibrationDone = false;
		}
		else
		{
			std::list<double> ErrorCorrectionValues;
			for(int i = 0; i < cbsrows->Length; i++)
			{
				CMachineType = System::Convert::ToString(cbsrows[i]->default["MachineType_Size"]);
				slipgaugeLength = System::Convert::ToDouble(cbsrows[i]->default["SlipGaugeLength"]);
				Nextcellgap = System::Convert::ToDouble(cbsrows[i]->default["NextCellGap"]);
				rcount = System::Convert::ToInt32(cbsrows[i]->default["RowCount"]);
				ccount = System::Convert::ToInt32(cbsrows[i]->default["ColumnCount"]);
				correction_X = System::Convert::ToDouble(cbsrows[i]->default["Correction_X"]);
				correction_Y = System::Convert::ToDouble(cbsrows[i]->default["Correction_Y"]);
				ErrorCorrectionValues.push_back(correction_X);
				ErrorCorrectionValues.push_back(correction_Y);
			}
			this->LocalisedCalibrationDone = true;
			CALBCALCOBJECT->SetCalibrationCorrectionValues(&ErrorCorrectionValues, slipgaugeLength, rcount, ccount);
		}
		if(!LocalisedCalibrationDone)
			MAINDllOBJECT->UseLocalisedCorrection = false;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0009", ex);	}
	GetValues->FinalizeClass();
}

void RWrapper::RW_DRO::GetContourScanCallibrationDetails()
{
	try
	{
		
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0009a", ex);	}
}

void RWrapper::RW_DRO::GetFocusCalibrationDetails()
{
	CalibrationModule::DB ^GetValues = gcnew CalibrationModule::DB(RWrapper::RW_MainInterface::DBConnectionString);
	try
	{
		GetValues->FillTable("FocusScanCalibration");
		GetValues->FillTable("MeasurementSettings");
		cli::array<System::Data::DataRow^>^ cbsrows = GetValues->Select_ChildControls("FocusScanCalibration", "MachineNo", RWrapper::RW_DBSettings::MYINSTANCE()->CurrentMachineNumber);
		cli::array<System::Data::DataRow^>^ mesrows = GetValues->Select_ChildControls("MeasurementSettings", "MachineNo", RWrapper::RW_DBSettings::MYINSTANCE()->CurrentMachineNumber);
		double CellleftX = 0, CellleftY = 0, Cellwidth = 0, Cellheight = 0, ErrorCorrection;
		double CalibrateError[4] = {0};
		
		if(cbsrows->Length == 0) // Focus Calibration Not Done..
		{
			FOCUSCALCOBJECT->FocusCalibrationIsDone(false);
			FOCUSCALCOBJECT->ApplyFocusCorrection(false);
		}
		else
		{
			CalibrateError[0] = System::Convert::ToDouble(mesrows[0]->default["X_Shift_FocusCalib"]);
			CalibrateError[1] = System::Convert::ToDouble(mesrows[0]->default["Y_Shift_FocusCalib"]);
			CalibrateError[2] = System::Convert::ToDouble(mesrows[0]->default["Deviation1_FocusCalib"]);
			CalibrateError[3] = System::Convert::ToDouble(mesrows[0]->default["Deviation2_FocusCalib"]);
			cli::array<int>^ id;
			cli::array<double>^ error;
			id = gcnew cli::array<int>(cbsrows->Length);
			error = gcnew cli::array<double>(cbsrows->Length);
			std::list<double> ErrorCorrectionValues;
			for(int i = 0; i < cbsrows->Length; i++)
			{
				id[i] = System::Convert::ToInt32(cbsrows[i]->default["FSC_ID"]);
			/*	CellleftX = System::Convert::ToDouble(cbsrows[i]->default["CellLeftX"]);
				CellleftY = System::Convert::ToDouble(cbsrows[i]->default["CellLeftY"]);
				Cellwidth = System::Convert::ToDouble(cbsrows[i]->default["CellWidth"]);
				Cellheight = System::Convert::ToDouble(cbsrows[i]->default["CellHeight"]);*/
				error[i] = System::Convert::ToDouble(cbsrows[i]->default["Correction_Z"]);
				//ErrorCorrection = System::Convert::ToDouble(cbsrows[i]->default["Correction_Z"]);
				//ErrorCorrectionValues.push_back(ErrorCorrection);				
			}		
			for(int i = 0; i < cbsrows->Length ; i++)
			{
				for(int j = 0; j < cbsrows->Length - 1 ; j++ )
				{
					if(id[j]>id[j+1])
						{
							int temp1 = id[j + 1];
							double temp2 = error[j + 1];
							id[j+1] = id[j];
							id[j] = temp1;
							error[j + 1] = error[j];
							error[j] = temp2;
						}
				}
			}
			for (int i = 0; i < cbsrows->Length; i++)
			{
				double temp = error[i];
				ErrorCorrectionValues.push_back(temp);
			}
				
			FOCUSCALCOBJECT->FocusCalibrationIsDone(true);
			FOCUSCALCOBJECT->SetFocusCalibrationValues(&ErrorCorrectionValues, CalibrateError, Cellwidth, Cellheight);
			//CALBCALCOBJECT->SetCalibrationCorrectionValues(&ErrorCorrectionValues, slipgaugeLength, rcount, ccount);
		}
		GetValues->FinalizeClass();
	}
	catch(Exception^ ex){
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0010", ex);	
		GetValues->FinalizeClass();
	}
}

System::String^ RWrapper::RW_DRO::GetCommPortName()
{
	try
	{
		Microsoft::VisualBasic::Devices::Computer^ MyComputer = gcnew Microsoft::VisualBasic::Devices::Computer();
		Microsoft::Win32::RegistryKey^ i = MyComputer->Registry->LocalMachine->OpenSubKey("System\\CurrentControlSet\\Enum\\USB\\");
		System::String^ MPortName = "";
		for each(System::String^ s in i->GetSubKeyNames())
		{
			int r = s->IndexOf("10C4");
			int tem = s->IndexOf("EA60");
			int r1 = s->IndexOf("&Mi_");
			int tem1 = s->IndexOf("_ea");
		
			if((r != -1 && tem != -1) || (r1 != -1 && tem1 != -1))
			{
				try
				{
					i = MyComputer->Registry->LocalMachine->OpenSubKey("System\\CurrentControlSet\\Enum\\USB\\" + s + "\\");
					i = i->OpenSubKey((i->GetSubKeyNames())[0] + "\\Device Parameters");
					System::String^ Pname = i->GetValue("PortName")->ToString();
					if(Pname != "") MPortName = Pname;
				}
				catch(Exception^ ex){}
			}
		}
		return MPortName;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0012", ex); return ""; }
}

RWrapper::RW_DRO::~RW_DRO()
{
}

RWrapper::RW_DRO^ RWrapper::RW_DRO::MYINSTANCE()
{
	return RW_DRO::DROInst;
}

System::String^ RWrapper::RW_DRO::radianToDegree(double _value)
{
	try
	{
		if(_value > 1000) return "";
		bool Negativeflag = false;
		if(_value < 0)
			Negativeflag = true;
		_value = System::Math::Abs(_value*(180/System::Math::PI));
		int _degree = Convert::ToInt32(System::Math::Abs(_value));
		double _decimalValue = _value - _degree;
		if(_decimalValue < 0)
		{
			_degree -= 1;
			_decimalValue += 1;
		}
		double _decimalValue1 = System::Math::Abs(60*(_decimalValue));
		int _min = Convert::ToInt32(_decimalValue1);

		double _decimalValue2 = _decimalValue1 - _min;
		if(_decimalValue2 < 0)
		{
			_min -= 1;
			_decimalValue2 += 1;
		}
		int _sec = System::Math::Abs(60 *_decimalValue2);
		double _t = _degree + (double)(_min/60) + (double)(_sec/3600);
		System::String^ _rvalue;
		System::String^ MinValue = System::Convert::ToString(_min);
		System::String^ SecValue = System::Convert::ToString(_sec);
		if(_min < 10)MinValue = "0" + MinValue;
		if(_sec < 10)SecValue = "0" + SecValue;
		if(Negativeflag)
			_rvalue = System::String::Concat("-",System::Convert::ToString(_degree), System::Convert::ToChar(176), " ", MinValue, System::Convert::ToChar(39), " ", SecValue, System::Convert::ToChar(34));
		else
			_rvalue = System::String::Concat(System::Convert::ToString(_degree), System::Convert::ToChar(176), " ", MinValue, System::Convert::ToChar(39), " ", SecValue, System::Convert::ToChar(34));
		return _rvalue;
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0011", ex);	return "";
	}
}

void RWrapper::RW_DRO::AxisAngleCorrections()
{
	try
	{
		//double DroValues[3] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2]};
		//double CDroValues[3] = {0};
		//BESTFITOBJECT->CompensateErrors(&DroValues[0], AxisAngle, &CDroValues[0], AxisAngleXZ, AxisAngleYZ);
		//RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0] = CDroValues[0];
		//RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] = CDroValues[1];
		//RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] = CDroValues[2];
		//RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] = CDroValues[3];
		//RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] = RWrapper::RW_LCParams::MYINSTANCE()->GetCorrectedPosition(RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]);
		//Correct for errors in straightness and encoder values
		//cli::array<double> ^CurrentDRO = { RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] };
		//if (this->CorrectionLookup) //Check if we have any correction entries for this machine. If not, we do not need to modify anything here....
		//{
		//	for (int i = 0; i < 2; i++)
		//	{
		//		CurrentDRO = this->CorrectionLookup[i]->GetCorrectedPosition(CurrentDRO);
		//	}
		//}
		cli::array<double>^ AxisAngles = { AxisAngle, AxisAngleXZ, AxisAngleYZ };
		//Do global Axis Angle corrections first before doing localised corrections...
		//this->DROCoordinate[0] += System::Math::Sin(AxisAngles[0]) * this->DROCoordinate[1] + sin(AxisAngles[1]) * this->DROCoordinate[2];
		//this->DROCoordinate[1] = System::Math::Cos(AxisAngles[0]) * this->DROCoordinate[1] + sin(AxisAngles[2]) * this->DROCoordinate[2];

		//Trial on 26 Jun 2016
		this->DROCoordinate[1] += System::Math::Sin(AxisAngles[0]) * this->DROCoordinate[0] + sin(AxisAngles[2]) * this->DROCoordinate[2];
		this->DROCoordinate[0] = System::Math::Cos(AxisAngles[0]) * this->DROCoordinate[0] + sin(AxisAngles[1]) * this->DROCoordinate[2];

		this->DROCoordinate[2] = System::Math::Cos(AxisAngles[1]) * System::Math::Cos(AxisAngles[2]) * this->DROCoordinate[2];
		for (int kk = 0; kk < 4; kk++)
			this->Corrections[kk] = 0;

		if (this->LC_CorrectionZoneCollection->Count > 0)
		{
			//CorrectionZone^ sampleCorrectionZone = (CorrectionZone^)this->LC_CorrectionZoneCollection["0:0"];
			cli::array<int>^ GridPosition = { 0,0,0,0 };
			for (int j = 0; j < 2; j++) //sampleCorrectionZone->DependencyRank
			{
				GridPosition[j] = this->DROCoordinate[j] / this->gridSize[j];
				if (GridPosition[j] < 0) GridPosition[j] = 0;
				if (this->DROCoordinate[j] > this->Range[j]) GridPosition[j] = (this->Range[j] - 1) / this->gridSize[j];
			}

			//for (int i = 0; i < this->LC_CorrectionZoneCollection->Count; i++)
			//{
				String^ thisIndex;
				//for (int j = 0; j < 2; j++) //this->LC_CorrectionZoneCollection->Count
				//{
				//	thisIndex += GridPosition[j].ToString();
				//}
				thisIndex = GridPosition[0].ToString() + ":" + GridPosition[1].ToString();
				//System::Collections::Hashtable^ currentHashTable = this->LC_CorrectionZoneCollection[i];
				if (this->LC_CorrectionZoneCollection->Contains(thisIndex))
					Corrections = ((CorrectionZone^)this->LC_CorrectionZoneCollection[thisIndex])->GetCorrectionValue(this->DROCoordinate, this->gridSize, GridPosition);
				//if (double::IsNaN(Corrections[i])) Corrections[i] = 0;
			//}
				//System::IO::StreamWriter^ sw = gcnew System::IO::StreamWriter(RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath + "\\Log\\ErrorLog\\LogFile\\Corrections.log", true);
				//sw->WriteLine(Corrections[0].ToString() + ", " + Corrections[1].ToString() + ", " + Corrections[2].ToString() + ", " + Corrections[3].ToString() + ", ");
				//sw->Close();
		}
		for (int i = 0; i < this->LC_MultiplierList->Count; i++)
			this->LC_MultiplierList[i]->GetCorrectionValue(this->DROCoordinate, Corrections);
		
		for (int j = 0; j < 4; j++)
			RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[j] -= Corrections[j];
	}
	catch (Exception^ ex) 
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0013", ex);
	}
}

void RWrapper::RW_DRO::SetReturnPosition()
{
	for(int i = 0; i < 4; i++)
		returnPosition[i] = DROCoordinate[i];
}


void RWrapper::RW_DRO::Handle_DROCallback(double* currentDro)
{
	try
	{
		if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM) return;

		for (int i = 0; i < 4; i++)
			DROCoordinate[i] = currentDro[i]; //DROInstance->DROCoordinateValue[i]; //
		AxisAngleCorrections();
		if(!DontUpdateLocalisedCalibration)
		{
			if(!AutoCalibrationMode && !HomingMovementMode) // If not then do the corrections..
			{
				if(LocalisedCalibrationDone && MAINDllOBJECT->UseLocalisedCorrection && HomingisDone)
				{
					double DroValueX = DROCoordinate[0];
					double DroValueY = DROCoordinate[1];
					double ActDroValueX = DroValueX, ActDroValueY = DroValueY;
					BESTFITOBJECT->correctXYCoord2(&DroValueX, &DroValueY);
					DROCoordinate[0] = DroValueX;
					DROCoordinate[1] = DroValueY;
				}
			}
		}
		if((MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HEIGHT_GAUGE || MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HEIGHTGAUGE_VISION)) // && RWrapper::RW_CircularInterPolation::MYINSTANCE()->AddPointsFlag)
		{		
			int MultiplyFactor = 1;
			if(MAINDllOBJECT->StylusPolarity())
				MultiplyFactor = -1;
			if(MAINDllOBJECT->CurrentStylusType == RapidEnums::STYLUSTYPE::ALONG_ZAXIS)
			{
				double CurrentZ = DROCoordinate[2] + DROCoordinate[3] * MultiplyFactor;
				DROCoordinate[2] = CurrentZ;
				if(RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMax < CurrentZ || RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMin > CurrentZ
					|| System::Math::Abs(this->Last_DROValue[0] - DROCoordinate[0]) > 0.01 || System::Math::Abs(this->Last_DROValue[1] - DROCoordinate[1]) > 0.01)
				{				
					if(RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMax < CurrentZ || RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMax == 0)
						RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMax = CurrentZ;
					if(RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMin > CurrentZ || RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMin == 0)
						RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMin = CurrentZ;
					this->Last_DROValue[0] = DROCoordinate[0]; this->Last_DROValue[1] = DROCoordinate[1]; this->Last_DROValue[2] = DROCoordinate[2]; this->Last_DROValue[3] = DROCoordinate[3];
					RWrapper::RW_CircularInterPolation::MYINSTANCE()->AddPointToSelectedCloudPoints(DROCoordinate[0], DROCoordinate[1], CurrentZ);				
				}
				else
				{	//If USB gage is connected, check that it is loaded. Zero Positioning is always done on the assumption that when DRO starts up, Gauge is unloaded....
					if (DROInstance->GageConnected && MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::SELECTPOINT_HANDLER)
					{
						if (abs(DROCoordinate[3]) > 0.0002)
							((SelectPointHandler*)MAINDllOBJECT->getCurrentHandler())->ProbeisLoaded = true;
						else
							((SelectPointHandler*)MAINDllOBJECT->getCurrentHandler())->ProbeisLoaded = false;
					}
				}
				//else if (HeightGauge_ScanningModeON)
				//{
				//	//ShapeWithList* CShape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
				//	//MAINDllOBJECT->getCurrentHandler()->LmaxmouseDown(); 
				//}
			}
			else if(MAINDllOBJECT->CurrentStylusType == RapidEnums::STYLUSTYPE::ALONG_YAXIS)
			{
				double CurrentY = DROCoordinate[1] + DROCoordinate[3] * MultiplyFactor;
				if(RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMax < CurrentY || RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMin > CurrentY
					|| System::Math::Abs(this->Last_DROValue[0] - DROCoordinate[0]) > 0.01 || System::Math::Abs(this->Last_DROValue[2] - DROCoordinate[2]) > 0.01)
				{
					if(RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMax < CurrentY || RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMax == 0)
						RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMax = CurrentY;
					if(RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMin > CurrentY || RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMin == 0)
						RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMin = CurrentY;
					this->Last_DROValue[0] = DROCoordinate[0]; this->Last_DROValue[1] = DROCoordinate[1]; this->Last_DROValue[2] = DROCoordinate[2]; this->Last_DROValue[3] = DROCoordinate[3];
					RWrapper::RW_CircularInterPolation::MYINSTANCE()->AddPointToSelectedCloudPoints(DROCoordinate[0], CurrentY, DROCoordinate[2]);
				}
			}
			else if(MAINDllOBJECT->CurrentStylusType == RapidEnums::STYLUSTYPE::ALONG_XAXIS)
			{
				double CurrentX = DROCoordinate[0] + DROCoordinate[3] * MultiplyFactor;
				if(RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMax < CurrentX || RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMin > CurrentX
					|| System::Math::Abs(this->Last_DROValue[1] - DROCoordinate[1]) > 0.01 || System::Math::Abs(this->Last_DROValue[2] - DROCoordinate[2]) > 0.01)
				{
					if(RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMax < CurrentX || RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMax == 0)
						RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMax = CurrentX;
					if(RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMin > CurrentX || RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMin == 0)
						RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMin = CurrentX;
					this->Last_DROValue[0] = DROCoordinate[0]; this->Last_DROValue[1] = DROCoordinate[1]; this->Last_DROValue[2] = DROCoordinate[2]; this->Last_DROValue[3] = DROCoordinate[3];
					RWrapper::RW_CircularInterPolation::MYINSTANCE()->AddPointToSelectedCloudPoints(CurrentX, DROCoordinate[1], DROCoordinate[2]);
				}
			}
		}
		else if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::TOUCHTYPE_CONTOURTRACER)
		{
			//correct dro coordinate....
			double TmpDroCordinate[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
			if(Contour_ScalingCoefficient != 0 && !ContourScan_Callibration)
			{
				double angle = (TmpDroCordinate[3] - Contour_Reference_Rlevel) / Contour_ScalingCoefficient, Contour_TransMatrix[9] = {0};
				Contour_TransMatrix[0] = cos(angle); Contour_TransMatrix[1] = 0; Contour_TransMatrix[2] = -sin(angle);
				Contour_TransMatrix[0] = Contour_AxisOfRotation[0]; Contour_TransMatrix[1] = Contour_AxisOfRotation[1]; Contour_TransMatrix[2] = Contour_AxisOfRotation[2];
				Contour_TransMatrix[0] = sin(angle); Contour_TransMatrix[1] = 0; Contour_TransMatrix[2] = cos(angle);
				Vector TmpValue = MAINDllOBJECT->TransformMultiply(Contour_TransMatrix, Contour_Offset[0], Contour_Offset[1], Contour_Offset[2]);
				TmpDroCordinate[0] = TmpDroCordinate[0] + TmpValue.getX() - Contour_Offset[0];
				TmpDroCordinate[1] = TmpDroCordinate[1] + TmpValue.getY() - Contour_Offset[1];
				TmpDroCordinate[2] = TmpDroCordinate[2] + TmpValue.getZ() - Contour_Offset[2];
			}

			if(RWrapper::RW_CircularInterPolation::MYINSTANCE()->AddPointsFlag)
			{
				if(RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMax < TmpDroCordinate[2] || RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMin > TmpDroCordinate[2]
					|| System::Math::Abs(this->Last_DROValue[0] - TmpDroCordinate[0]) > 0.01 || System::Math::Abs(this->Last_DROValue[1] - TmpDroCordinate[1]) > 0.01)
				{				
					if(RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMax < TmpDroCordinate[2] || RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMax == 0)
						RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMax = TmpDroCordinate[2];
					if(RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMin > TmpDroCordinate[2] || RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMin == 0)
						RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMin = TmpDroCordinate[2];
					this->Last_DROValue[0] = TmpDroCordinate[0]; this->Last_DROValue[1] = TmpDroCordinate[1]; this->Last_DROValue[2] = TmpDroCordinate[2]; this->Last_DROValue[3] = TmpDroCordinate[3];
					RWrapper::RW_CircularInterPolation::MYINSTANCE()->AddPointToSelectedCloudPoints(TmpDroCordinate[0], TmpDroCordinate[1], TmpDroCordinate[2]);				
				}
			}
		}
		//if (HandlingDROChangedValue) return;
		//HandlingDROChangedValue = true;
		RWrapper::RW_DRO::MYINSTANCE()->UpdateDRO_textbox();
		RWrapper::RW_DRO::MYINSTANCE()->UpdateCenterScreen();
		if (FirstDROValueReceived)
			RWrapper::RW_DBSettings::MYINSTANCE()->SetCameraToCompleteZoom_Exeload();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0004", ex);	}
	//HandlingDROChangedValue = false;
	FirstDROValueReceived = false;
}

void RWrapper::RW_DRO::UpdateDRO_textbox()
{
	try
	{
		double dx[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		MAINDllOBJECT->TransformDROValues(RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], &dx[0]);
		dx[2] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ();
		dx[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getR();
		if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::TOUCHTYPE_CONTOURTRACER || DROInstance->GageConnected)
		{
			//correct dro coordinate....
			if(RWrapper::RW_DRO::MYINSTANCE()->Contour_ScalingCoefficient != 0 && !RWrapper::RW_DRO::MYINSTANCE()->ContourScan_Callibration)
			{
				double angle = (dx[3] - RWrapper::RW_DRO::MYINSTANCE()->Contour_Reference_Rlevel) / RWrapper::RW_DRO::MYINSTANCE()->Contour_ScalingCoefficient, Contour_TransMatrix[9] = {0};
				Contour_TransMatrix[0] = cos(angle); Contour_TransMatrix[1] = 0; Contour_TransMatrix[2] = -sin(angle);
				Contour_TransMatrix[0] = RWrapper::RW_DRO::MYINSTANCE()->Contour_AxisOfRotation[0]; Contour_TransMatrix[1] = RWrapper::RW_DRO::MYINSTANCE()->Contour_AxisOfRotation[1]; Contour_TransMatrix[2] = RWrapper::RW_DRO::MYINSTANCE()->Contour_AxisOfRotation[2];
				Contour_TransMatrix[0] = sin(angle); Contour_TransMatrix[1] = 0; Contour_TransMatrix[2] = cos(angle);
				Vector TmpValue = MAINDllOBJECT->TransformMultiply(Contour_TransMatrix, RWrapper::RW_DRO::MYINSTANCE()->Contour_Offset[0], RWrapper::RW_DRO::MYINSTANCE()->Contour_Offset[1], RWrapper::RW_DRO::MYINSTANCE()->Contour_Offset[2]);
				dx[0] = dx[0] + TmpValue.getX() - RWrapper::RW_DRO::MYINSTANCE()->Contour_Offset[0];
				dx[1] = dx[1] + TmpValue.getY() - RWrapper::RW_DRO::MYINSTANCE()->Contour_Offset[1];
				dx[2] = dx[2] + TmpValue.getZ() - RWrapper::RW_DRO::MYINSTANCE()->Contour_Offset[2];
			}
			dx[0] = dx[0] - MAINDllOBJECT->getCurrentUCS().currentDroX() - MAINDllOBJECT->getCurrentUCS().UCSPoint.getX();
			dx[1] = dx[1] - MAINDllOBJECT->getCurrentUCS().currentDroY() - MAINDllOBJECT->getCurrentUCS().UCSPoint.getY();
			dx[2] = dx[2] - MAINDllOBJECT->getCurrentUCS().currentDroZ() - MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ();
			dx[3] = dx[3] - MAINDllOBJECT->getCurrentUCS().currentDroR();
			//dx[RWrapper::RW_DRO::MYINSTANCE()->GageAlignmentAxis] -= DROInstance->GagePosition;

			for(int i = 0; i < 4; i++)
			{
				if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
				{
					DROValue[i] = (dx[i] + MousePosOnVideo[i])/25.4;
					RWrapper::RW_DRO::MYINSTANCE()->CurrentUCSDRO[i] = dx[i]/25.4;
				}
				else
				{
					DROValue[i] = dx[i] + MousePosOnVideo[i];
					RWrapper::RW_DRO::MYINSTANCE()->CurrentUCSDRO[i] = dx[i];
				}
			}
		}
		else
		{	
			if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HOBCHECKER) 
				for (int i = 0; i < 4; i ++) MousePosOnVideo[i] = 0;

			//dx[0] = dx[0] - MAINDllOBJECT->getCurrentUCS().currentDroX() - MAINDllOBJECT->getCurrentUCS().UCSPoint.getX();
			//dx[1] = dx[1] - MAINDllOBJECT->getCurrentUCS().currentDroY() - MAINDllOBJECT->getCurrentUCS().UCSPoint.getY();
			//dx[2] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] - MAINDllOBJECT->getCurrentUCS().currentDroZ() - MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ();
			//dx[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] - MAINDllOBJECT->getCurrentUCS().currentDroR();
			for(int i = 0; i < 4; i++)
			{
				if(RWrapper::RW_DRO::MYINSTANCE()->AxisProperty[i])
				{
					if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
					{
						DROValue[i] = (dx[i] + MousePosOnVideo[i])/25.4;
						RWrapper::RW_DRO::MYINSTANCE()->CurrentUCSDRO[i] = dx[i]/25.4;
					}
					else
					{	
						DROValue[i] = dx[i] + MousePosOnVideo[i];
						RWrapper::RW_DRO::MYINSTANCE()->CurrentUCSDRO[i] = dx[i];
					}
				}
				else
				{
					DROValue[i] = dx[i];
					RWrapper::RW_DRO::MYINSTANCE()->CurrentUCSDRO[i] = dx[i];
					double AngleDec = System::Math::Abs(DROValue[i]*(180/System::Math::PI));
					if(AngleDec < RWrapper::RW_DBSettings::MYINSTANCE()->FirstGradeValue || AngleDec > RWrapper::RW_DBSettings::MYINSTANCE()->LastGradevalue)
						RAxisGradeValue = "";
					else
					{
						int GradeIndex = -1;
						for each(System::Collections::DictionaryEntry GradeValue in RWrapper::RW_DBSettings::MYINSTANCE()->GradeValueCollection)
						{
							GradeIndex = System::Convert::ToInt32(GradeValue.Key);
							if(AngleDec >= System::Convert::ToDouble(GradeValue.Value))
							{
								if(AngleDec <= System::Convert::ToDouble(RWrapper::RW_DBSettings::MYINSTANCE()->GradeValueCollection[GradeIndex]))
								{
									break;
								}
							}
						}
						if(GradeIndex >= 0)
							RAxisGradeValue = System::Convert::ToString(RWrapper::RW_DBSettings::MYINSTANCE()->GradeLevelCollection[GradeIndex]);
						/*double AngleDiff = AngleDec - RWrapper::RW_DBSettings::MYINSTANCE()->FirstGradeValue;
						double AngleIndex = AngleDiff/RWrapper::RW_DBSettings::MYINSTANCE()->GradeStep;
						int AngleIndexInt = (int)AngleIndex;
						double AngleIndexDiff = AngleIndex - AngleIndexInt;
						if(AngleIndexDiff > 0) AngleIndexInt += 1;
						RAxisGradeValue = System::Convert::ToString(RWrapper::RW_DBSettings::MYINSTANCE()->GradeLevelCollection[AngleIndexInt]);*/
					}
					if(MAINDllOBJECT->AngleMeasurementMode() == 0)
						RAxisValue = RWrapper::RW_DRO::MYINSTANCE()->radianToDegree(dx[i]);
					else if(MAINDllOBJECT->AngleMeasurementMode() == 1)
					{
						if(dx[i] < 0) AngleDec = -AngleDec;
						RAxisValue =  Microsoft::VisualBasic::Strings::Format(AngleDec, "0.0000");
					}
					else
						RAxisValue = Microsoft::VisualBasic::Strings::Format(DROValue[i], "0.0000");
				}
				RWrapper::RW_DRO::MYINSTANCE()->CurrentUCSDRO_IntUse[i] = dx[i];
			}
		}
		//MAINDllOBJECT->setCurrentDRO(dx[0] + MAINDllOBJECT->getCurrentUCS().UCSPoint.getX(), dx[1] + MAINDllOBJECT->getCurrentUCS().UCSPoint.getY(), dx[2] + MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ(), dx[3]);
		MAINDllOBJECT->setCurrentDRO(dx[0], dx[1], dx[2], dx[3]);
		//MAINDllOBJECT->setCurrentDRO(RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]);
		MAINDllOBJECT->getCurrentUCS().SetCurrentUCS_DRO(&MAINDllOBJECT->getCurrentDRO());
		PPCALCOBJECT->CurrentMachineDRO.set(RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]);
		if (!RWrapper::RW_DRO::MYINSTANCE()->UpdatingDROTextBox)
		{
			RWrapper::RW_DRO::MYINSTANCE()->UpdatingDROTextBox = true;
			RWrapper::RW_DRO::MYINSTANCE()->DROChanged::raise();
		}
	}
	catch(Exception^ ex)
	{ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0014", ex);	}
}

void RWrapper::RW_DRO::UpdateCenterScreen()
{
	try
	{
		if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::PROBE_ONLY || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HEIGHT_GAUGE ||
			//MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HEIGHTGAUGE_VISION ||
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HOBCHECKER) return;
		//Get the RW_DRO values for the centerscreen..//
		double dx[4] = { 0 }, dxx[4] = { 0 };
		if (MAINDllOBJECT->getCurrentUCS().UCSMode() == 2 || MAINDllOBJECT->getCurrentUCS().UCSMode() == 3)
			MAINDllOBJECT->TransformDROValues(RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], &dxx[0]);

		dx[0] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getX(); // -MAINDllOBJECT->getCurrentUCS().currentDroX();
		dx[1] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getY(); //- MAINDllOBJECT->getCurrentUCS().currentDroY();
		dx[2] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ(); // - MAINDllOBJECT->getCurrentUCS().currentDroZ();
		dx[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]; // - MAINDllOBJECT->getCurrentUCS().currentDroR();
		//dx[0] = MAINDllOBJECT->getCurrentUCS().currentDroX() - MAINDllOBJECT->getCurrentUCS().UCSPoint.getX();
		//dx[1] = MAINDllOBJECT->getCurrentUCS().currentDroY() - MAINDllOBJECT->getCurrentUCS().UCSPoint.getY();
		//dx[2] = MAINDllOBJECT->getCurrentUCS().currentDroZ() - MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ();
		//dx[3] = MAINDllOBJECT->getCurrentUCS().currentDroR();

		if(!RWrapper::RW_FocusDepth::MYINSTANCE()->Focusflag && !RWrapper::RW_DRO::MYINSTANCE()->AutoFocusMode_Flag)
		{ 
			if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY) // ||
				//MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ ||
				//MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ ||
				//MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm ||
				//MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
			{
				MAINDllOBJECT->centerScreen(dx[0], dx[1], dx[2], true);
				if (MAINDllOBJECT->getCurrentUCS().UCSMode() == 2 || MAINDllOBJECT->getCurrentUCS().UCSMode() == 3)
				{	
					MAINDllOBJECT->getWindow(0).centerCam(dx[0], dx[1]);// -MAINDllOBJECT->getCurrentUCS().UCSPoint.getX(), dx[1] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getY());
					MAINDllOBJECT->centerScreen(dxx[0], dxx[1], dxx[2], false); // -MAINDllOBJECT->getCurrentUCS().UCSPoint.getX(), dxx[1] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getY(), dxx[2] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ(), false);
				}
			}
				//According to the Projection type pass the X, Y, Z values to the centerscreen//
			//if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
			//	MAINDllOBJECT->centerScreen(dxx[0], dxx[1], dxx[2], false); // -MAINDllOBJECT->getCurrentUCS().UCSPoint.getX(), dxx[1] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getY(), dxx[2] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ(), false);
			//else 
			if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
			{
				MAINDllOBJECT->getWindow(0).centerCam(dx[0] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getX(), dx[1] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getY());
				MAINDllOBJECT->centerScreen(dxx[0], dxx[2], dxx[1], false);
			}
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
			{
				MAINDllOBJECT->getWindow(0).centerCam(dx[0] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getX(), dx[1] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getY()); 
				MAINDllOBJECT->centerScreen(dxx[1], dxx[2], dxx[0], false);
			}
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
			{
				MAINDllOBJECT->getWindow(0).centerCam(dx[0] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getX(), dx[1] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getY());
				MAINDllOBJECT->centerScreen(dxx[0], -dxx[2], dxx[1], false);
			}
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
			{
				MAINDllOBJECT->getWindow(0).centerCam(dx[0] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getX(), dx[1] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getY());
				MAINDllOBJECT->centerScreen(dxx[1], -dxx[2], dxx[0], false);
			}
		}
		else
		{	// Call focus function..//
			//if(RWrapper::RW_FocusDepth::MYINSTANCE()->Focusflag)
			if ((MAINDllOBJECT->getProfile() && !MAINDllOBJECT->getSurface()) || 
				(FOCUSCALCOBJECT->CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::HOMING_MODE || 
					FOCUSCALCOBJECT->CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::REFERENCEDOT_SECOND)) // RW_FocusDepth::MYINSTANCE()->)
				FOCUSCALCOBJECT->GetCurrentFocusValue_Profile(RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[RW_FocusDepth::MYINSTANCE()->GetSelectedAxis()]);
			else
				FOCUSCALCOBJECT->GetCurrentFocusValue_Normal(RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[RW_FocusDepth::MYINSTANCE()->GetSelectedAxis()]);
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0015", ex);	}
}

void RWrapper::RW_DRO::DisplayVideoMouseMove(double* mousepos)
{
	try
	{	
		if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HOBCHECKER || MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::VISIONHEADONLY) return;
		for(int i = 0; i < 3; i++)
			this->MousePosOnVideo[i] = mousepos[i];
		RWrapper::RW_DRO::MYINSTANCE()->UpdateDRO_textbox();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0016", ex);	}
}

void RWrapper::RW_DRO::HandleProbeHitCallback(int direction, double* currentDro)
{
	try
	{
		//Check if this is a touch point when machine was on a go to position and got a wrong point.... We shouldnt take and do anything with it....
		//if (this->IsSpuriousTouchPoint)
		//{
		//	this->IsSpuriousTouchPoint = false;
		//	return;
		//}
		//Set the Probe Position...
		for(int i = 0; i < 4; i++)
			ProbeCoordinate[i] = currentDro[i];
		double DroValues[4] = {ProbeCoordinate[0], ProbeCoordinate[1], ProbeCoordinate[2],ProbeCoordinate[3]};
		double CDroValues[4] = {0};
		////Compensate errors because of movement
		//BESTFITOBJECT->CompensateErrors(&DroValues[0], AxisAngle, &CDroValues[0], AxisAngleXZ, AxisAngleYZ);
		////Localised Calibration corrections
		//if(!DontUpdateLocalisedCalibration)
		//{
		//	if(!AutoCalibrationMode && !HomingMovementMode) // If not then do the corrections..
		//	{
		//		if(LocalisedCalibrationDone && MAINDllOBJECT->UseLocalisedCorrection && HomingisDone)
		//		{
		//			double DroValueX = CDroValues[0];
		//			double DroValueY = CDroValues[1];
		//			BESTFITOBJECT->correctXYCoord2(&DroValueX, &DroValueY);
		//			CDroValues[0] = DroValueX;
		//			CDroValues[1] = DroValueY;
		//		}
		//	}
		//}
		//for (int ii = 0; ii < 4; ii ++) ProbeCoordinate[ii] = CDroValues[ii];
		//ProbeCoordinate[3] = currentDro[3];

		//ProbeCoordinate[1] = CDroValues[1];
		//ProbeCoordinate[2] = CDroValues[2];
		MAINDllOBJECT->SetStatusCode("RW_DRO02");
		//Check Probe Type and take point
		switch(MAINDllOBJECT->CurrentProbeType)
		{
			case RapidEnums::PROBETYPE::SIMPLE_PROBE:
			{
				if(PPCALCOBJECT->IsPartProgramRunning())
				{
					if(RWrapper::RW_MainInterface::MYINSTANCE()->UseLookAhead)
					{
						FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
						if(fg == NULL)
						{
							fg->PartprogrmActionPassStatus(false);
							return;
						}
						if(fg->ActionForTwoStepHoming() || RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
						{
							fg->PartprogrmActionPassStatus(true);
							MAINDllOBJECT->AddProbePointToSelectedShape(ProbeCoordinate[0], ProbeCoordinate[1], ProbeCoordinate[2], ProbeCoordinate[3], DROInstance->Probe_Hit_Direction);
						}
					}
					else
					{
						if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::ADDPOINTACTION) 
						{
							FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
							if(fg == NULL)
							{
								fg->PartprogrmActionPassStatus(false);
								return;
							}
							if(fg->ActionForTwoStepHoming() || RWrapper::RW_CNC::MYINSTANCE()->getCNCMode() || 
								MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::DSP || 
								MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP)
							{
								direction = (int)fg->ProbeDirection;
								fg->PartprogrmActionPassStatus(true);

								MAINDllOBJECT->AddProbePointToSelectedShape(ProbeCoordinate[0], ProbeCoordinate[1], ProbeCoordinate[2], ProbeCoordinate[3], direction);
								if (!RW_CNC::MYINSTANCE()->CNCmode)
									Handle_PartProgramActionIncrement();
							}
						}
					}
				}
				else if(!PPCALCOBJECT->IsPartProgramRunning())
				{
					if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HOBCHECKER) // && 
					{
						ofstream pointsFile;
						System::String^ SfilePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath;
						SfilePath = SfilePath + "\\Log\\AxisLog\\SpacingPts.csv";
						pin_ptr<const wchar_t> wch = PtrToStringChars(SfilePath);
						pointsFile.open(wch, ios::app);
						pointsFile << ProbeCoordinate[0] << "," << ProbeCoordinate[1] << "," << ProbeCoordinate[2] << "," << ProbeCoordinate[3] << ","; // << tempC_AxisValue << ",";

						if (!RWrapper::RW_DelphiCamModule::MYINSTANCE()->Getting_BC_Axis &&
							RWrapper::RW_DelphiCamModule::MYINSTANCE()->GotAxis)
						{
							//We now rotate the point around the axis to display the points around the centre for each tip
							double temp[4] = { 0 };
							double tempC_AxisValue = ProbeCoordinate[3]; // RWrapper::RW_LCParams::MYINSTANCE()->GetCorrectedPosition(ProbeCoordinate[3]);
							ProbeCoordinate[3] = tempC_AxisValue;
							if (RWrapper::RW_DelphiCamModule::MYINSTANCE()->DelphiCamType == 4)// && RWrapper::RW_DelphiCamModule::MYINSTANCE()->HobParameters[13] != 0)
							{
								double tt[6] = { 0 };
								RMATH3DOBJECT->RotatePoint_Around_C_Axis(ProbeCoordinate, tt, MAINDllOBJECT->TIS_CAxis, temp);
							}
							else
								RMATH3DOBJECT->RotatePoint_Around_C_Axis(ProbeCoordinate, MAINDllOBJECT->BC_Axis, MAINDllOBJECT->TIS_CAxis, temp);
							//RMATH3DOBJECT->Correct_BC_Tilt(ProbeCoordinate, MAINDllOBJECT->TIS_CAxis);
							for (int i = 0; i < 4; i++) ProbeCoordinate[i] = temp[i];
							pointsFile << ProbeCoordinate[0] << "," << ProbeCoordinate[1];

							MAINDllOBJECT->AddProbePointToSelectedShape(ProbeCoordinate[0], ProbeCoordinate[1], ProbeCoordinate[2], ProbeCoordinate[3], direction);
						}
						else if (RW_DelphiCamModule::MYINSTANCE()->ManualPoint)
							RW_DelphiCamModule::MYINSTANCE()->ManualPoint = false;
						//else
							MAINDllOBJECT->AddProbePointToSelectedShape(ProbeCoordinate[0], ProbeCoordinate[1], ProbeCoordinate[2], ProbeCoordinate[3], direction);
						pointsFile << endl;
						pointsFile.close();
					}
					else
						MAINDllOBJECT->AddProbePointToSelectedShape(ProbeCoordinate[0], ProbeCoordinate[1], ProbeCoordinate[2], ProbeCoordinate[3], direction);

					//if (RWrapper::RW_LBlock::MYINSTANCE()->FirstStepRunning)
					//	RWrapper::RW_LBlock::MYINSTANCE()->Run_LBlock();
					//else if (RWrapper::RW_LBlock::MYINSTANCE()->CycleStarted)
					//{
					//	if (RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag) 
					//		RWrapper::RW_LBlock::MYINSTANCE()->HandleCompletedCurrentStep();
					//}
					//RWrapper::RW_DelphiCamModule::MYINSTANCE()->WaitingForTouchPt = false;
					//RWrapper::RW_DelphiCamModule::MYINSTANCE()->SendDrotoNextposition();
				}
			break;
			}
			case RapidEnums::PROBETYPE::X_AXISTPROBE:
			case RapidEnums::PROBETYPE::Y_AXISTPROBE:
			case RapidEnums::PROBETYPE::STARPROBE:
			case RapidEnums::PROBETYPE::GENERICPROBE:
			{
				for(int i = 0; i < 4; i++) CurrentProbePosition[i] = ProbeCoordinate[i];
				currentDir = direction;
				if(PPCALCOBJECT->IsPartProgramRunning())
				{								
					if(RWrapper::RW_MainInterface::MYINSTANCE()->UseLookAhead)
					{
						FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
						if(fg == NULL)
						{
							fg->PartprogrmActionPassStatus(false);
							return;
						}
						if(fg->ActionForTwoStepHoming() || RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
						{
							currentDir = DROInstance->Probe_Hit_Direction;
							SelectProbePointSide = true;
							GetProbePositionInTouch((int)fg->ProbePosition);
							fg->PartprogrmActionPassStatus(true);
						}
					}
					else
					{
						if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
						{
							FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
							//fg->lightProperty = 
							if(fg == NULL)
							{
								fg->PartprogrmActionPassStatus(false);
								return;
							}
							if(fg->ActionForTwoStepHoming() || RWrapper::RW_CNC::MYINSTANCE()->getCNCMode() || MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::DSP || MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP)
							{
								currentDir = (int)fg->ProbeDirection;
								SelectProbePointSide = true;
								GetProbePositionInTouch((int)fg->ProbePosition);
								fg->PartprogrmActionPassStatus(true);
							}
						}
					}
				}
				else if(!MAINDllOBJECT->getSelectedShapesList().getList().empty())
				{
					SelectProbePointSide = true;
					TouchedProbePostion::raise(); 
				}
				break;
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0016a", ex);	}
}

void RWrapper::RW_DRO::GetProbePositionInTouch(int currentSide)
{
	if(SelectProbePointSide)
		MAINDllOBJECT->AddProbePointToSelectedShape(CurrentProbePosition[0], CurrentProbePosition[1], CurrentProbePosition[2], CurrentProbePosition[3], currentDir, currentSide);
	else
	{
		MAINDllOBJECT->NotedownProbePath(currentSide);
		Console::Beep(600, 500);
	}
}

void RWrapper::RW_DRO::RaiseCameraClickSoundEvent()
{
	CameraSoundEvent::raise();
}

void RWrapper::RW_DRO::RaiseTouchedProbePostion()
{
	TouchedProbePostion::raise(); 
}

void RWrapper::RW_DRO::DoAutoFocusDuringProgramRun()
{
	try
	{
		bool CheckProfileFgType = PPCALCOBJECT->CheckCurrentActionType_ProfileFG();
		if(CheckProfileFgType)
		{
			bool CheckFGFail = PPCALCOBJECT->CheckCurrentFrameGrab_Fail();
			if(CheckFGFail)
			{
				//PPCALCOBJECT->CalculateFocusRectangeBox_FG();
				int RectFGArray[4] = {100,100, MAINDllOBJECT->currCamWidth - 200 , MAINDllOBJECT->currCamHeight - 200 };
				FOCUSCALCOBJECT->SetAutoFocusRectangle(&RectFGArray[0]);
				double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
				double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
				FOCUSCALCOBJECT->ClearLastFocusValues();
				//Calcualte the target postion:  current DRO + span;
				target[2] = target[2] - 0.5;
			   RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::PARTPROGRAM_AUTOFOCUSSPAN);
			}
			else
				PPCALCOBJECT->RunCurrentStep();
		}
		else
			PPCALCOBJECT->RunCurrentStep();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}


void RWrapper::RW_DRO::Handle_DROButtonClick(System::String^ Str, bool LeftClick)
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())	
		{
			 MAINDllOBJECT->SetStatusCode("RW_PartProgram02");
			return;
		}
		if(LeftClick)
		{
			if(Str == "X") //If current UCS == MCS add new UCS else if Shape count is > 0 then Add new UCS..//
			{
				int ct = MAINDllOBJECT->getShapesList().count();
				int ct2 = MAINDllOBJECT->FundamentalShapes();
				if (MAINDllOBJECT->getShapesList().count() > MAINDllOBJECT->FundamentalShapes() || (MAINDllOBJECT->getCurrentUCS().getId() == 0))
				{
					MAINDllOBJECT->AddNewUCS_DROclear();
					MAINDllOBJECT->getCurrentUCS().SetParameters(0, new Vector(DROCoordinate[0], 0, 0)); //.currentDroX(DROCoordinate[0]);
				}
				else
				{
					MAINDllOBJECT->getCurrentUCS().SetParameters(0, new Vector(DROCoordinate[0], MAINDllOBJECT->getCurrentUCS().UCSPoint.getY(), MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ(),
																				MAINDllOBJECT->getCurrentUCS().UCSPoint.getR())); //.currentDroX(DROCoordinate[0]);
				}
			}
			else if(Str == "Y")
			{
				int iddd = MAINDllOBJECT->getCurrentUCS().getId();
				if (MAINDllOBJECT->getShapesList().count() > MAINDllOBJECT->FundamentalShapes() || (MAINDllOBJECT->getCurrentUCS().getId() == 0))
				{
					MAINDllOBJECT->AddNewUCS_DROclear();
					//MAINDllOBJECT->getCurrentUCS().currentDroY(DROCoordinate[1]);
					MAINDllOBJECT->getCurrentUCS().SetParameters(0, new Vector(0, DROCoordinate[1], 0, 0)); //.currentDroX(DROCoordinate[0]);
				}
				else
				{
					MAINDllOBJECT->getCurrentUCS().SetParameters(0, new Vector(MAINDllOBJECT->getCurrentUCS().UCSPoint.getX(), DROCoordinate[1], MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ(),
						MAINDllOBJECT->getCurrentUCS().UCSPoint.getR())); //.currentDroX(DROCoordinate[0]);
				}

			}
			else if(Str == "Z")
			{
				if (MAINDllOBJECT->getShapesList().count() > MAINDllOBJECT->FundamentalShapes() || (MAINDllOBJECT->getCurrentUCS().getId() == 0))
				{
					MAINDllOBJECT->AddNewUCS_DROclear();
					//MAINDllOBJECT->getCurrentUCS().currentDroZ(DROCoordinate[2]);
					MAINDllOBJECT->getCurrentUCS().SetParameters(0, new Vector(0, 0, DROCoordinate[2], 0)); //.currentDroX(DROCoordinate[0]);
				}
				else
				{
					MAINDllOBJECT->getCurrentUCS().SetParameters(0, new Vector(MAINDllOBJECT->getCurrentUCS().UCSPoint.getX(), MAINDllOBJECT->getCurrentUCS().UCSPoint.getY(), DROCoordinate[2],
						MAINDllOBJECT->getCurrentUCS().UCSPoint.getR())); //.currentDroX(DROCoordinate[0]);
				}
			}
			else if(Str == "R")
			{
				if (MAINDllOBJECT->getShapesList().count() > MAINDllOBJECT->FundamentalShapes() || (MAINDllOBJECT->getCurrentUCS().getId() == 0))
				{
					MAINDllOBJECT->AddNewUCS_DROclear();
					//MAINDllOBJECT->getCurrentUCS().currentDroR(DROCoordinate[3]);
					MAINDllOBJECT->getCurrentUCS().SetParameters(0, new Vector(0, 0, 0, DROCoordinate[3], 0)); //.currentDroX(DROCoordinate[0]);
				}
				else
				{
					MAINDllOBJECT->getCurrentUCS().SetParameters(0, new Vector(MAINDllOBJECT->getCurrentUCS().UCSPoint.getX(), MAINDllOBJECT->getCurrentUCS().UCSPoint.getY(), 
																	MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ(), DROCoordinate[3] )); //.currentDroX(DROCoordinate[0]);
				}
			}
			UCS *ucs = &(MAINDllOBJECT->getCurrentUCS());
			ucs->getWindow(1).homeIt(1);
			RWrapper::RW_DRO::MYINSTANCE()->UpdateDRO_textbox();
			RWrapper::RW_DRO::MYINSTANCE()->UpdateCenterScreen();
		}
		else
		{
			UCS* CurrentUCS = &MAINDllOBJECT->getCurrentUCS();
			if(Str == "X") //If current UCS == MCS add new UCS else if Shape count is > 0 then Add new UCS..//
			{
				//if(MAINDllOBJECT->getShapesList().count() > 2 || MAINDllOBJECT->getCurrentUCS().getId() == 0)
				MAINDllOBJECT->AddNewUCS_DROclear();
				MAINDllOBJECT->getCurrentUCS().currentDroX(DROCoordinate[0]);
				MAINDllOBJECT->getCurrentUCS().currentDroY(CurrentUCS->currentDroY());
				MAINDllOBJECT->getCurrentUCS().currentDroZ(CurrentUCS->currentDroZ());
				MAINDllOBJECT->getCurrentUCS().currentDroR(CurrentUCS->currentDroR());
				MAINDllOBJECT->getCurrentUCS().UCSPoint.set(CurrentUCS->UCSPoint.getX(), CurrentUCS->UCSPoint.getY(), CurrentUCS->UCSPoint.getZ());
			}
			else if(Str == "Y")
			{
				//if(MAINDllOBJECT->getShapesList().count() > 2 || MAINDllOBJECT->getCurrentUCS().getId() == 0)
				MAINDllOBJECT->AddNewUCS_DROclear();
				MAINDllOBJECT->getCurrentUCS().currentDroY(DROCoordinate[1]);
				MAINDllOBJECT->getCurrentUCS().currentDroX(CurrentUCS->currentDroX());
				MAINDllOBJECT->getCurrentUCS().currentDroZ(CurrentUCS->currentDroZ());
				MAINDllOBJECT->getCurrentUCS().currentDroR(CurrentUCS->currentDroR());
				MAINDllOBJECT->getCurrentUCS().UCSPoint.set(CurrentUCS->UCSPoint.getX(), CurrentUCS->UCSPoint.getY(), CurrentUCS->UCSPoint.getZ());
			}
			else if(Str == "Z")
			{
				//if(MAINDllOBJECT->getShapesList().count() > 2 || MAINDllOBJECT->getCurrentUCS().getId() == 0)
				MAINDllOBJECT->AddNewUCS_DROclear();
				MAINDllOBJECT->getCurrentUCS().currentDroZ(DROCoordinate[2]);
				MAINDllOBJECT->getCurrentUCS().currentDroY(CurrentUCS->currentDroY());
				MAINDllOBJECT->getCurrentUCS().currentDroX(CurrentUCS->currentDroX());
				MAINDllOBJECT->getCurrentUCS().currentDroR(CurrentUCS->currentDroR());
				MAINDllOBJECT->getCurrentUCS().UCSPoint.set(CurrentUCS->UCSPoint.getX(), CurrentUCS->UCSPoint.getY(), CurrentUCS->UCSPoint.getZ());
			}
			else if(Str == "R")
			{
				//if(MAINDllOBJECT->getShapesList().count() > 2 || MAINDllOBJECT->getCurrentUCS().getId() == 0)
				MAINDllOBJECT->AddNewUCS_DROclear();
				MAINDllOBJECT->getCurrentUCS().currentDroR(DROCoordinate[3]);
				MAINDllOBJECT->getCurrentUCS().currentDroY(CurrentUCS->currentDroY());
				MAINDllOBJECT->getCurrentUCS().currentDroZ(CurrentUCS->currentDroZ());
				MAINDllOBJECT->getCurrentUCS().currentDroX(CurrentUCS->currentDroX());
				MAINDllOBJECT->getCurrentUCS().UCSPoint.set(CurrentUCS->UCSPoint.getX(), CurrentUCS->UCSPoint.getY(), CurrentUCS->UCSPoint.getZ());
			}
			RWrapper::RW_DRO::MYINSTANCE()->UpdateDRO_textbox();
			RWrapper::RW_DRO::MYINSTANCE()->UpdateCenterScreen();
		}
		//else
		//{
		//	//Used to Lock/Unlock the axess...//
		//	std::string command = "*B";
		//	std::string command1 = "";
		//	char* tem;
		//	if(sender->Equals(MyPanelButtons[5]))
		//		*(AxisLock + 3) = !*(AxisLock + 3);
		//	else if(sender->Equals(MyPanelButtons[4]))
		//		*(AxisLock + 2) = !*(AxisLock + 2);
		//	else if(sender->Equals(MyPanelButtons[3]))
		//		*(AxisLock + 1) = !*(AxisLock + 1);
		//	//else if(sender->Equals() == "Rlock Click")
		//		//*(AxisLock) = !*(AxisLock);
		//	if(*AxisLock)
		//		command1 += "1";
		//	else
		//		command1 += "0";
		//	for(int i = 1; i <= 3; i++)
		//	{
		//		if(*(AxisLock + i))
		//		{
		//			//Set the command string and change the button Image... to Lock...!!!
		//			command1 += "1";
		//			this->MyPanelButtons[i + 2]->Content = LockbmpList[3 - i];
		//		}
		//		else
		//		{
		//			//Clear the send command and set the button status to Unlock state....!!
		//			command1 += "0";
		//			this->MyPanelButtons[i + 2]->Content = UnLockbmpList[3 - i];
		//		}
		//	}
		//	System::String^ str = gcnew String((char*)command1.c_str());
		//	str = System::Convert::ToString(System::Convert::ToInt16(str,2),16);
		//	strcat((char*)command.c_str(),(char*)(void*)Marshal::StringToHGlobalAnsi(str));
		//	DROInstance->sendCommand(command.c_str());
		//}
		//RcadControls::MYINSTANCE()->UpperElementHost->Invoke(_crossHairCallBack);
		//UpdateCenterScreen();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0020", ex);	}
}

void RWrapper::RW_DRO::Raise_DROTargetReached(int type)
{
	DROTargetReached::raise(type);
}

void RWrapper::RW_DRO::Handle_DROTargetReached(int type)
{
	try
	{
		if(type == 0) // Target Reached CallBack.. CNC or camera zoom in etc..
		{			
			if(!RWrapper::RW_PartProgram::MYINSTANCE()->BatchOfCommandCompleted)
				RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
			else
			{
				RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(true);			
			}
		}
		else if(type == 1) //set new card and send command for version number...
		{
			DROInstance->SetMachineCardFlag(1);
			RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag = 1;
			if (DROInstance->GetCardVersionNumber())
				RWrapper::RW_MainInterface::MYINSTANCE()->MachineConnectedStatus = true;

		}
		else if(type == 2) //Machine Disconnected during load
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->MachineConnectedStatus = false;
			if (MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::VISIONHEADONLY)
			//{
				MAINDllOBJECT->ShowMsgBoxString("RW_DROHandle_DROTargetReached01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION);
			//}
			//else
			//{
			//	//System::Threading::Thread::Sleep(25000);
			//	//System::Threading::Thread^ tt = System::Threading::Thread::CurrentThread;
			//	//tt->Abort();
			//}
				SendMachineSwitchedOff::raise();

		}
		else if(type == 3) //Connection is successful..
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->MachineConnectedStatus = true;

		}
		else if(type == 4) //Failed to reconnect to the hardware..
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->MachineConnectedStatus = false;
			MAINDllOBJECT->ShowMsgBoxString("RW_DROHandle_DROTargetReached02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION);
		}
		else if(type == 5) //reconnect to the hardware..Success
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->MachineConnectedStatus = true;
			 MAINDllOBJECT->SetStatusCode("RW_DRO03");
			MAINDllOBJECT->ShowMsgBoxString("RW_DROHandle_DROTargetReached03", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION);
		}
		else if(type == 6) //failed in read port
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->MachineConnectedStatus = false;
			MAINDllOBJECT->SetStatusCode("RW_DRO04");
			MAINDllOBJECT->ShowMsgBoxString("RW_DROHandle_DROTargetReached04", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION);
		}
		else if(type == 7) //Resend last command
		{
			Handle_PartProgramActionRedo();
		}
		else if(type == 8)
		{

		}
		else if(type == 9)
		{	
			if(!RWrapper::RW_PartProgram::MYINSTANCE()->BatchOfCommandCompleted)
			{
				RWrapper::RW_PartProgram::MYINSTANCE()->BatchOfCommandCompleted = true;
				RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
			}
			else
				RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(true);
		}
		else if(type == 10)
		{
			Handle_DRO_AxisValueChanged();
		}
		else if(type == 11)
		{
			if (RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom && !(RWrapper::RW_DBSettings::MYINSTANCE()->CheckForTargetReach))
				RWrapper::RW_DBSettings::MYINSTANCE()->UpdateVideoUpp_DROCallback(DROInstance->DROCoordinateValue[3]);
			else
			{
				//if (RotaryScanDelayCount > RWrapper::TIS_Control::MYINSTANCE()->ScanDelayCount)
				//{
				//	RWrapper::TIS_Control::MYINSTANCE()->RotaryScanForCurrentTask();
				//	RotaryScanDelayCount = 0;
				//}
				//else
				//	RotaryScanDelayCount++;
			}
		}
		else if(type == 12)
		{
			Handle_DRO_ProbeHit();
		}
		else if(type == 13) //LimitSwitchCallback
		{
			if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag < 2)
			//{
			//}
			//else
			{
				DROInstance->ActivateCNC();
				if (RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
					RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
			}
			else
			{
				if (PPCALCOBJECT->IsPartProgramRunning())
				{
					RWrapper::RW_PartProgram::MYINSTANCE()->PartProgram_Pause();
					//System::Windows::MessageBox::Show("Limit Switch has been hit and hence the Proram Run has paused. Kindly move the position and Click on Continue", 
					//	"Rapid-I 5.0", System::Windows::MessageBoxButton::OK, System::Windows::MessageBoxImage::Error);
				}
			}

		}
		else if(type == 14) //set old card ...
		{
			DROInstance->SetMachineCardFlag(0);
			RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag = 0;
			SendCommandAcknowledge::raise();
		}
		else if(type == 15) //set new card and don't send command for version number...
		{
			DROInstance->SetMachineCardFlag(1);
			RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag = 1;
			SendCommandAcknowledge::raise();
		}
		else if(type == 16) // if target is not reached second time also...
		{
			//please reload the program or restart software then try again...
			RWrapper::RW_MainInterface::MYINSTANCE()->Update_MsgBoxStatus("RW_DROHandle_DROTargetReached05", RWrapper::RW_Enum::RW_MSGBOXTYPE::MSG_OK, RWrapper::RW_Enum::RW_MSGBOXICONTYPE::MSG_INFORMATION, false, "");
		}
		else if(type == 17) // if Raxis target is reached
		{
			if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag < 2)
				DROInstance->ActivateCNC();
		}
		else if (type == 18) // we hvae v33.2 card
		{
			DROInstance->SetMachineCardFlag(2);
			RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag = 2;
			SendCommandAcknowledge::raise();
		}
		else if (type == 19) // we hvae v33.2 card and CNC Mode has been enabled
		{
			MAINDllOBJECT->IsCNCMode(true);
			RWrapper::RW_CNC::MYINSTANCE()->CNCmode = true;
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCModeChanged(1);
		}
		else if (type == 20) // we hvae v33.2 card and Manual Mode has been enabled
		{
			MAINDllOBJECT->IsCNCMode(false);
			RWrapper::RW_CNC::MYINSTANCE()->CNCmode = false;
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCModeChanged(2);
		}
		else if (type == 21) // we have finished current step in 2 step Homing. let us call the next step
		{
			RWrapper::RW_PartProgram::MYINSTANCE()->HandlePartprogram();
		}
		else if (type > 21 && type < 25) //We have a DRO Jump in particular axis
		{
			RWrapper::RW_CNC::MYINSTANCE()->Activate_ManualModeDRO();
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCModeChanged(2);
			cli::array<String^>^ Axes = gcnew cli::array<String^> {"X", "Y", "Z", "C"};
			System::Windows::MessageBox::Show("Please check earthing or power supply quality. The DRO has jumped in " + Axes[type - 22] + " and could cause an accident.", "Rapid-I 5.0", System::Windows::MessageBoxButton::OK, System::Windows::MessageBoxImage::Error);
			System::IO::StreamWriter^ sw = gcnew System::IO::StreamWriter(RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath + "\\Log\\ErrorLog\\LogFile\\Stoppages.log", true);
			sw->WriteLine("Stopped " + Axes[type - 22] + " Axis on " + DateTime::Now.ToShortDateString() + " at " + DateTime::Now.ToString("HH:mm:ss.fff"));
			sw->Close();
		}
		else if (type == 25)
		{
			//Swivel axis has hit limit switch and position is Vertical
			if (AAxisPositionEventRaised != type)
			{
				FinishedSwivelling_A_Axis::raise("H");
				//RWrapper::TIS_Control::MYINSTANCE()->A_Axis_isVertical = false;
				MAINDllOBJECT->AAxisHorizontal = true;
				//MAINDllOBJECT->RotateGrafix(true);
				//MAINDllOBJECT->ProjectionType = RapidEnums::RAPIDPROJECTIONTYPE::XZ;
				//MAINDllOBJECT->ChangeWindowViewOrientation(0, RapidEnums::RAPIDPROJECTIONTYPE::XZ);
				//MAINDllOBJECT->ChangeWindowViewOrientation(1, RapidEnums::RAPIDPROJECTIONTYPE::XZ);
				AAxisPositionEventRaised = type;
			}
		}
		else if (type == 26)
		{
			//Swivel axis has hit limit switch and position is Vertical
			if (AAxisPositionEventRaised != type)
			{
				FinishedSwivelling_A_Axis::raise("V");
				//RWrapper::TIS_Control::MYINSTANCE()->A_Axis_isVertical = true;
				MAINDllOBJECT->AAxisHorizontal = false;
				//MAINDllOBJECT->RotateGrafix(false);
				//MAINDllOBJECT->ProjectionType = RapidEnums::RAPIDPROJECTIONTYPE::XY;
				//MAINDllOBJECT->ChangeWindowViewOrientation(0, RapidEnums::RAPIDPROJECTIONTYPE::XY);
				//MAINDllOBJECT->ChangeWindowViewOrientation(1, RapidEnums::RAPIDPROJECTIONTYPE::XY);
				AAxisPositionEventRaised = type;
			}
		}
		else if (type == 27)
		{
			//Version number did not work since machine might be switched off

		}
		else if (type == 28)
		{
			//this->IsSpuriousTouchPoint = true;
			//Probe was hit in CNC mode for a non-probe command
			if (PPCALCOBJECT->IsPartProgramRunning())
			{
				PausePartPartProgram::raise();
			}
			else if (!RW_SphereCalibration::MYINSTANCE()->DoingCalibration)
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2); // goto manual mode
		}
		else if (type == 29)
		{
			//Command is not transferred properly or command is wrong and not accepted by controller

			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2); // goto manual mode
			if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HOBCHECKER)
				RWrapper::RW_DelphiCamModule::MYINSTANCE()->StopProcess();
		}
		else if (type == 30)
		{
			//Drill Checking System cards are present. 
			DROInstance->SetMachineCardFlag(3);
			RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag = 3;
			//Send acknowledgement of card type to controller so it stops sending string unnecessarily
			//DROInstance->sendMachineCommand_One((char*)Marshal::StringToHGlobalAnsi("51").ToPointer(), true, true);
			SendCommandAcknowledge::raise();
		}
		//else if (type == 31) //Handled in front-end
		else if (type == 32)
		{
			//Movement to next position is complete. We can now run part program on current component
			// Handled in the front-End itself
		}
		else if (type == 33)
		{
			//Finished ejecting components in pass and/or reject bins. we are ready to go to the next command
			//Handled in Front End itself.
		}
		else if (type == 34)
		{
			//Probe touch happened during retract. Check if we are running part program or if its HobChecker. Pause and Resume for hob checker
			if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HOBCHECKER)
			{
				RWrapper::RW_DelphiCamModule::MYINSTANCE()->countinue_pause_delphicammodule(false);
				Sleep(100);
				RWrapper::RW_DelphiCamModule::MYINSTANCE()->countinue_pause_delphicammodule(true);
			}
		}

	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0020", ex);	}
}

void RWrapper::RW_DRO::setTargetReachedTypeCallback(TargetReachedCallback type)
{
	HandleTarget = type;
}

void RWrapper::RW_DRO::FunctionTargetReached(bool ApplyDelay)
{
	try
	{
		//DateTime startTime = DateTime::Now;
		//System::Windows::Forms::MessageBox::Show("Reached Target" + " , " + ((int)HandleTarget).ToString(), "Rapid-I");

		if(MAINDllOBJECT->IsPartProgramPause)
		{
			System::Windows::Forms::MessageBox::Show("Incremented Program Step");
			Handle_PartProgramActionIncrement();
			return;
		}
		
		if(RWrapper::RW_PartProgram::MYINSTANCE()->PartProgramPaused)
			return;

		if(ApplyDelay)
			Sleep(RWrapper::RW_MainInterface::MYINSTANCE()->Delay_AfterTargetReached); // Wait for sometime....
		
		if(HandleTarget == TargetReachedCallback::PARTPROGRAM_ACTION)
			Handle_PartProgramCurrentAction();
		else if(HandleTarget == TargetReachedCallback::PARTPROGRAM_INCREMENT)
			Handle_PartProgramActionIncrement();
		else if(HandleTarget == TargetReachedCallback::PARTPROGRAM_PROBEHIT)
			Handle_PartProgramProbeHit();
		else if(HandleTarget == TargetReachedCallback::PARTPROGRAM_PROBEGOBACK)
			Handle_ProbeGoBack();
		else if(HandleTarget == TargetReachedCallback::PARTPROGRAM_RERUN)
			Handle_ProgramRerun();
		else if (HandleTarget == TargetReachedCallback::PARTPROGRAM_FINISHED)
		{
			//System::Windows::Forms::MessageBox::Show("Reached Finished Target", "Rapid-I");
			Handle_ProgramFinished();
		}
		else if(HandleTarget == TargetReachedCallback::PARTPROGRAM_NOTHING)
			Handle_ProgramStepDoNothing();
		else if(HandleTarget == TargetReachedCallback::PROFILEAUTOFOCUSSTART)
			Handle_AutoFocusSpan();
		else if(HandleTarget == TargetReachedCallback::PARTPROGRAM_AUTOFOCUSSPAN)
			Handle_ProgramAutoFocusSpan();
		else if(HandleTarget == TargetReachedCallback::PARTPROGRAM_AUTOFOCUSFINISHED)
			Handle_ProgramAutoFocusFinished();
		else if(HandleTarget == TargetReachedCallback::PARTPROGRAM_AUTOFOCUSPOSITIONREACHED)
			Handle_ProgramAutoFocusPosReached();

		else if(HandleTarget == TargetReachedCallback::FOCUSDEPTH_SPAN)
			Handle_FocusDepthSpan();
		else if(HandleTarget == TargetReachedCallback::FOCUSCHECK_SPAN)
			Handle_FocusCheckSpan();
		else if(HandleTarget == TargetReachedCallback::FOCUSDEPTH_CONTINUEACTIVESPAN)
			Handle_FocusDepthContinueSpan();
		else if(HandleTarget == TargetReachedCallback::FOCUSDEPTH_FINISHED)
			Handle_FocusDepthFinished();
		else if(HandleTarget == TargetReachedCallback::FOCUSDEPTH_POSITIONREACHED)
			Handle_FocusDepthFocusPositionReached();
		else if(HandleTarget == TargetReachedCallback::FOCUSDEPTH_AFSMODEPOSITION)
			Handle_FocusDepthAutoFocusScanNextPosition();
		else if(HandleTarget == TargetReachedCallback::FOCUSDEPTH_START_DURINGPROGRAM)
			Handle_FocusDepthStartDuringProgram();
		else if(HandleTarget == TargetReachedCallback::FOCUSDEPTH_SPAN_DURINGPROGRAM)
			Handle_FocusDepthSpanDuringProgram();
		else if(HandleTarget == TargetReachedCallback::FOCUSDEPTH_CONTINUEACTIVESPAN_PARTPROGRAM)
			Handle_FocusDepthContinueSpanDuringProgram();
		else if(HandleTarget == TargetReachedCallback::FOCUSDEPTH_POSITIONREACHED_DURINGPROGRAM)
			Handle_FocusDepthFocusPositionReachedDuringProgram();
		else if(HandleTarget == TargetReachedCallback::FOCUSDEPTH_DRO_DURINGPROGRAM)
			Handle_FocusDepthDROPositionDuringProgram();
		else if(HandleTarget == TargetReachedCallback::TRACEPATH_INTERPOLATION)
			RWrapper::RW_CircularInterPolation::MYINSTANCE()->GotoCommandForNextShape(false);
		else if(HandleTarget == TargetReachedCallback::CIRCULARINTERPOLATION)
		{
			if(PPCALCOBJECT->IsPartProgramRunning())
				Handle_PartProgramActionIncrement();
			else
				RWrapper::RW_CircularInterPolation::MYINSTANCE()->StopAddingPtstoSelectedShapes(true);
		}
		else if(HandleTarget == TargetReachedCallback::LINEARINTERPOLATION)
		{
			if(PPCALCOBJECT->IsPartProgramRunning())
				Handle_PartProgramActionIncrement();
			else
				RWrapper::RW_CircularInterPolation::MYINSTANCE()->StopAddingPtstoSelectedShapes(true);
		}
		else if(HandleTarget == TargetReachedCallback::CIRCULARINTERPOLATION_CURRENTPOS)
			RWrapper::RW_CircularInterPolation::MYINSTANCE()->Handle_CircularGotoCommand();
		else if(HandleTarget == TargetReachedCallback::PROFILESCAN_MODE)
			Handle_ProfileScan();
		else if(HandleTarget == TargetReachedCallback::PROFILESCAN_AUTOFOCUSSPAN)
			Handle_ProfileScanAutoFocusSpan();
		else if(HandleTarget == TargetReachedCallback::PROFILESCAN_AUTOFOCUSFINISHED)
			Handle_ProfileScanAutoFocusFinished();
		else if(HandleTarget == TargetReachedCallback::PROFILESCAN_AUTOFOCUSPOSITIONREACHED)
			Handle_ProfileScanAutoFocusPosReached();
		else if(HandleTarget == TargetReachedCallback::AUTO_CALIBRATIONMODE)
			Handle_AutoCalibrationMode();
		else if(HandleTarget == TargetReachedCallback::AUTO_WIDTHMEASUREMENTMODE)
			Handle_AutoWidthMeasureMode();
		else if(HandleTarget == TargetReachedCallback::AUTO_CALIBRATIONNEXTORNTATN)
			Handle_AutoCalibrationNextOrnt();
		else if(HandleTarget == TargetReachedCallback::AUTO_CALIBRATIONLASTORN)
			Handle_AutoCalibrationLastOrnt();
		else if(HandleTarget == TargetReachedCallback::AUTO_ZOOMINOUTMODE)
			Handle_AutoZoomingMode();
		else if(HandleTarget == TargetReachedCallback::AUTO_ZOOMINOUTDURINGEXELOAD)
			Handle_AutoZoomingModeDuringExeLoad();
		else if(HandleTarget == TargetReachedCallback::AUTO_SGWIDTH)
			 Handle_Auto_SGWidthReapet();
		else if(HandleTarget == TargetReachedCallback::MACHINE_GOTOCOMMAND)
			Handle_MachineGotoCommand();
		else if(HandleTarget == TargetReachedCallback::MACHINE_GOTORCADWINDOWCLICK)
			Handle_MachineGotoRcadWindowClick();
		else if(HandleTarget == TargetReachedCallback::MACHINE_GOTOSAOICELLCLICK)
			Handle_MachineGotoSAOICellClick();
		/*else if(HandleTarget == TargetReachedCallback::MACHINE_GOTOAUTOMEASUREMODE)
			AUTOSHAPEMEASUREOBJECT->get_FramGrabParameter();*/
		else if(HandleTarget == TargetReachedCallback::GOTO_REPEATAUTOMEASURE)
			Handle_CircularRepeatAutoMeasure();
		else if(HandleTarget == TargetReachedCallback::REPEATAUTOMEASURE_AUTOFOCUSSPAN)
			Handle_CircularRepeatAutoMeasureFocusSpan();
		else if(HandleTarget == TargetReachedCallback::REPEATAUTOMEASURE_AUTOFOCUSFINISHED)
			Handle_CircularRepeatAutoMeasureFocusFinished();
		else if(HandleTarget == TargetReachedCallback::REPEATAUTOMEASURE_AUTOFOCUSPOSITIONREACHED)
			AUTOMEASUREOBJECT->get_FramGrabParameter();
		else if(HandleTarget == TargetReachedCallback::FIXTURE_CALIBRATION)		
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);	
		else if(HandleTarget == TargetReachedCallback::FIXTURE_CALIBRATION_FRAMGRAB)		
			RWrapper::RW_FixtureCalibration::MYINSTANCE()->get_FramGrab_Parameter();	
		else if(HandleTarget == TargetReachedCallback::NEXT_PROFILE_SCAN)		
			RWrapper::RW_AutoProfileScanMeasure::MYINSTANCE()->StartNextProfileScan();	
		else if(HandleTarget == TargetReachedCallback::STYLUS_SPHERECALLIBRATION)		
			RWrapper::RW_SphereCalibration::MYINSTANCE()->GetPointsInCircularPath();	
		else if(HandleTarget == TargetReachedCallback::MACHINE_PROBE_PATHPOINT)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->ProbeCount++;
			RWrapper::RW_MainInterface::MYINSTANCE()->ProbeGotoRequiredPosition_GetProbePts();			
		}
		else if(HandleTarget == TargetReachedCallback::PROBE_MEASURE_GOBACK)
			Handle_AutoMeasure_ProbeGoBack();
		else if(HandleTarget == TargetReachedCallback::VBLOCK_GOTOCOMMAND)
			Handle_SetCNCStatus();
		else if(HandleTarget == TargetReachedCallback::MACHINE_HOMINGMODE)
			Handle_MachineHomingMode();
		else if(HandleTarget == TargetReachedCallback::MACHINE_MOVELITTLE_AFTERHOMING)
			Handle_MachineMoveLittleAfterHoming();
		else if(HandleTarget == TargetReachedCallback::MACHINE_MOVESLOW_FORHOMING)
			Handle_MachineMoveSlowForHoming();
		else if(HandleTarget == TargetReachedCallback::MACHINE_CIRCULARMOVEMODE)
			Handle_MoveCircularStep();
		else if(HandleTarget == TargetReachedCallback::VBLOCK_CALLIBRATION)
			RWrapper::RW_VBlockCallibration::MYINSTANCE()->Handle_AxisLock();
		else if(HandleTarget == TargetReachedCallback::VBLOCK_CALLIBRATION_LINE)
		{
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
			Handle_VblockCallibration();
		}
		else if(HandleTarget == TargetReachedCallback::VBLOCKCALLIBRATION_AUTOFOCUSSPAN)
			Handle_VblockCallibrationAutoFocusSpan();
		else if(HandleTarget == TargetReachedCallback::VBLOCKCALLIBRATION_AUTOFOCUSFINISHED)
			Handle_VblockCallibrationAutoFocusFinished();
		else if(HandleTarget == TargetReachedCallback::VBLOCKCALLIBRATION_AUTOFOCUSPOSITIONREACHED)
		{
			RWrapper::RW_VBlockCallibration::MYINSTANCE()->create_Line(false);
			RWrapper::RW_VBlockCallibration::MYINSTANCE()->create_AngleBisector();
		}
		else if(HandleTarget == TargetReachedCallback::SPHERECALLIBRATION)
			RWrapper::RW_SphereCalibration::MYINSTANCE()->SendDrotoNextposition();
		else if(HandleTarget == TargetReachedCallback::DELPHICOMPONENTMODULE_MANUAL)
			RWrapper::RW_DelphiCamModule::MYINSTANCE()->CalculatePtsFor_BC_Axis(M_PI / 2);
		else if (HandleTarget == TargetReachedCallback::DELPHICOMPONENTMODULE)
		{
			RWrapper::RW_DelphiCamModule::MYINSTANCE()->SendDrotoNextposition();
			//System::IO::StreamWriter^ timeWriter = gcnew System::IO::StreamWriter(RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath + "\\Log\\AxisLog\\CommandLines.csv", true);
			//TimeSpan elapsed = DateTime::Now - startTime;
			//timeWriter->WriteLine(elapsed.Seconds + "." + elapsed.Milliseconds);
			//timeWriter->Close();
		}
		else if(HandleTarget == TargetReachedCallback::IMAGEDELAYCALIBRATION)
			RWrapper::RW_ImageDelayCalibration::MYINSTANCE()->StopImageCalibration();
		else if(HandleTarget == TargetReachedCallback::SPHERECALLIBRATION_POINT)
			RWrapper::RW_SphereCalibration::MYINSTANCE()->SendDrotoTakeNextPoint();
		else if(HandleTarget == TargetReachedCallback::SPHERECALIBRATIONMANUALPOINT)
			RWrapper::RW_SphereCalibration::MYINSTANCE()->SendDrotoNextposition();
		else if(HandleTarget == TargetReachedCallback::CONTOURTRACER_CALLIBRATION_PATH)
			RWrapper::RW_CT_SphereCallibration::MYINSTANCE()->SendDrotoGetPoint();
		else if(HandleTarget == TargetReachedCallback::CONTOURTRACER_CALLIBRATION_POINT)
			RWrapper::RW_CT_SphereCallibration::MYINSTANCE()->SendDrotoNextPosition();
		else if(HandleTarget == TargetReachedCallback::PARTPROGRAM_SAVEIMAGE)
		{
			if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::IMAGE_ACTION)
			{
				CameraSoundEvent::raise();
				int Image_Count = ((AddImageAction*)PPCALCOBJECT->getCurrentAction())->ImageCount;
				System::String^ filePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath + "//Image//" + System::Convert::ToString(Image_Count) + ".jpg";
				RWrapper::RW_MainInterface::MYINSTANCE()->SaveVideoWindowImage(filePath, false, false);
			}
			Handle_PartProgramActionIncrement();
		}
		else if(HandleTarget == TargetReachedCallback::IMAGEGRID_PROGRAM)
			RWrapper::RW_ImageGridProgram::MYINSTANCE()->Goto_NextCellPosition(false);	
		else if(HandleTarget == TargetReachedCallback::GRIDIMAGE_SUPERIMPOSE)
			SuperImposeImageObj->CaptureImage();
		else if (HandleTarget == TargetReachedCallback::TIS_COMMAND_TARGETREACHED)
		{
			//RWrapper::TIS_Control::MYINSTANCE()->CurrentStepIndex++;
			//RWrapper::TIS_Control::MYINSTANCE()->RunCommandsFromList();
			//RWrapper::TIS_Control::MYINSTANCE()->CurrentStepIndex++;
			//RWrapper::TIS_Control::MYINSTANCE()->DoCommandActions();
		}
		else if (HandleTarget == TargetReachedCallback::SPHEREVISIONPROBECALIBRATION)
			MAINDllOBJECT->TakeFrameGrabForSphere();
		else if (HandleTarget == TargetReachedCallback::LBLOCK_MODULE)
		{
			if (RWrapper::RW_LBlock::MYINSTANCE()->FirstStepRunning)
				RWrapper::RW_LBlock::MYINSTANCE()->Run_LBlock();
			else
			{
				if (!RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag)
					RWrapper::RW_LBlock::MYINSTANCE()->HandleCompletedCurrentStep();
			}
		}
		else if (HandleTarget == TargetReachedCallback::C_AXIS_RECALCULATE)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->CaptureCAxis();
		}
		else if (HandleTarget == TargetReachedCallback::ROTARY_AXIS_SCAN)
		{
			//System::Windows::Forms::MessageBox::Show("Started Rotary Scan Handler Reached", "Rapid-I");
			RW_MainInterface::MYINSTANCE()->SetContinuousScanMode(false);
			RW_MainInterface::MYINSTANCE()->Start_StopRotationalScan(false);
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0021", ex);	}
}

void RWrapper::RW_DRO::Synchronise_Handler_Action()
{
	try
	{	
		RWrapper::RW_PartProgram::MYINSTANCE()->NumberOfCommandSend--;	
		if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION || PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION)
		{
			if(RWrapper::RW_PartProgram::MYINSTANCE()->InterPolationCommandCount >= 1)
			{
				RWrapper::RW_PartProgram::MYINSTANCE()->InterPolationCommandCount--;
				if(RWrapper::RW_PartProgram::MYINSTANCE()->InterPolationCommandCount == 0)
					RWrapper::RW_CircularInterPolation::MYINSTANCE()->StopAddingPtstoSelectedShapes(true);
			}
		}	
		PPCALCOBJECT->IncrementActionForSynchronisation();
		if(PPCALCOBJECT->getCurrentAction() == NULL)
		{
			RWrapper::RW_PartProgram::MYINSTANCE()->HandlePartprogram();
			return;
		}
		if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_NOTHING);
		else if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::PARTPROGRAM_INCREMENT);
		else if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION)
		{
			 RWrapper::RW_CircularInterPolation::MYINSTANCE()->AddPointsFlag = ((CircularInterPolationAction*)PPCALCOBJECT->getCurrentAction())->GetAddPointInfo();
			 RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::CIRCULARINTERPOLATION);
		}
		else if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION)
		{
			 RWrapper::RW_CircularInterPolation::MYINSTANCE()->AddPointsFlag = ((LinearInterPolationAction*)PPCALCOBJECT->getCurrentAction())->GetAddPointInfo();
			 RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::LINEARINTERPOLATION);
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0021a", ex);	}
}

void RWrapper::RW_DRO::Handle_PartProgramCurrentAction()
{
	try
	{
		if(!PPCALCOBJECT->IsPartProgramRunning())
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->WriteCommandSendLog("Extra Target Reached Command");
			return;
		}
		if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT || 
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
			PPCALCOBJECT->RunCurrentStep();
		else
		{
			if(RWrapper::RW_PartProgram::MYINSTANCE()->AutoFocusDuringPartprogram)
				DoAutoFocusDuringProgramRun();
			else
			{
				
				//FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
				//switch (fg->FGtype)
				//{
				//case RapidEnums::RAPIDFGACTIONTYPE::ROTATIONSCAN_FIXEDRECTANGLE:
				//case RapidEnums::RAPIDFGACTIONTYPE::ROTATION_ANGULARRECTANGLEFRAMEGRAB:
				//case RapidEnums::RAPIDFGACTIONTYPE::ROTATION_ARCFRAMEGRAB:
				//case RapidEnums::RAPIDFGACTIONTYPE::ROTATION_CIRCLEFRAMEGRAB:
				//case RapidEnums::RAPIDFGACTIONTYPE::ROTATION_RECTANGLEFRAMEGRAB:

				//	PPCALCOBJECT->drawRunpp();
				//	break;
				//default:
				//	break;
				//}

				if(RWrapper::RW_PartProgram::MYINSTANCE()->RunningFailedAction)
				{
					RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
				}
				PPCALCOBJECT->RunCurrentStep();
			}
		}
		
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_PartProgramActionIncrement()
{
	try
	{
		MAINDllOBJECT->IsPartProgramPause = false;
		if(PPCALCOBJECT->getCurrentAction() == NULL)
		{
			if(PPCALCOBJECT->IsPartProgramRunning())
				RWrapper::RW_PartProgram::MYINSTANCE()->HandlePartprogram();
			return;
		}
		if(RWrapper::RW_PartProgram::MYINSTANCE()->RunningFailedAction)
		{
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
			PPCALCOBJECT->getCurrentAction()->notifyAll(DATACHANGED, PPCALCOBJECT->getCurrentAction());
			PPCALCOBJECT->PPrerunActionList++;
			RWrapper::RW_PartProgram::MYINSTANCE()->RunFailedAction();
			return;
		}
		
		if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
		{
			FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
			if(MAINDllOBJECT->RcadZoomExtents())
				MAINDllOBJECT->getWindow(1).ZoomToExtents(1);
			if (fg != NULL)
			{
				RC_ITER ppCurrAction = PPCALCOBJECT->getPPActionlist().getList().begin();
				RAction* action = (RAction*)(*ppCurrAction).second;
				FramegrabBase* fb1 = ((AddPointAction*)action)->getFramegrab();
				if (fg->ActionFinished() && fb1 == fg)
				{
					switch (fg->FGtype)
					{
					case RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB:
					case RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB:
					case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB:
					case RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB:
					case RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE:
						if (fg->noofpts < 0.1 * fg->noofptstaken_build)
						{
							//PPCALCOBJECT->getCurrentAction()->notifyAll(DATACHANGED, PPCALCOBJECT->getCurrentAction());
							if (RWrapper::RW_FixtureCalibration::MYINSTANCE()->CellHeight == 0)
							{
								double target[4] = { DROCoordinate[0] - 0.2, DROCoordinate[1], DROCoordinate[2], DROCoordinate[3] };
								double feedrate[4] = { feedRate[0] - 0.2, feedRate[1], feedRate[2], feedRate[3] };
								RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], PARTPROGRAM_ACTION);
								
								FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
								RC_ITER ppCurrAction = PPCALCOBJECT->getPPActionlist().getList().begin();
								RAction* action = (RAction*)(*ppCurrAction).second;
								FramegrabBase* fb1 = ((AddPointAction*)action)->getFramegrab();
								if (fg->ActionFinished() && fb1 == fg)
								{
									switch (fg->FGtype)
									{
									case RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB:
									case RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB:
									case RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB:
									case RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB:
									case RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE:
										if (fg->noofpts < 0.1 * fg->noofptstaken_build)
										{
											/*double target[4] = { DROCoordinate[0] + 0.2, DROCoordinate[1], DROCoordinate[2], DROCoordinate[3] };
											double feedrate[4] = { feedRate[0] + 0.2, feedRate[1], feedRate[2], feedRate[3] };
											RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], HandleTarget);
											PPCALCOBJECT->getCActionType(RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB);
											FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
											if (MAINDllOBJECT->RcadZoomExtents())
												MAINDllOBJECT->getWindow(1).ZoomToExtents(1);
											if (fg != NULL)
											{
												RC_ITER ppCurrAction = PPCALCOBJECT->getPPActionlist().getList().begin();
												RAction* action = (RAction*)(*ppCurrAction).second;
												FramegrabBase* fb1 = ((AddPointAction*)action)->getFramegrab();
												if (fg->noofpts < 0.1 * fg->noofptstaken_build)
												{
													MAINDllOBJECT->ShowMsgBoxString("RW_DRO_PartProgram_Terminated01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_ERROR);
													RW_PartProgram::MYINSTANCE()->Abort_PartProgram();
													return;
												}
											}*/
										}
									}
								}

								
							
							}
							//MAINDllOBJECT->ShowMsgBoxString("RW_DRO_PartProgram_Terminated01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_ERROR);
							//RW_PartProgram::MYINSTANCE()->Abort_PartProgram();
							//return;
						}
					default:
						break;
					}
				}
				PPCALCOBJECT->getCurrentAction()->notifyAll(DATACHANGED, PPCALCOBJECT->getCurrentAction());
			//	RWrapper::RW_PartProgram::MYINSTANCE()->UpdateCurrentPPStepStatus(fg->getId(), fg->noofpts, fg->PartprogrmActionPassStatus());    //	to be deleted later, of no use
			}
			if(((AddPointAction*)PPCALCOBJECT->getCurrentAction())->CriticalAction && !fg->PartprogrmActionPassStatus())
			{
				MAINDllOBJECT->IsPartProgramPause = true;
				PausePartPartProgram::raise();	
				MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgramPause01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
				RWrapper::RW_PartProgram::MYINSTANCE()->PartProgram_Pause();
				//RWrapper::RW_PartProgram::MYINSTANCE()->Abort_PartProgram();
				return;
			}
			if(((AddPointAction*)PPCALCOBJECT->getCurrentAction())->PausableAction)
			{
				PPCALCOBJECT->IncrementAction();
				PausePartPartProgram::raise();
				MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgramPause01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
				RWrapper::RW_PartProgram::MYINSTANCE()->PartProgram_Pause();
				return;
			}
		}
		else if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::PERIMETERACTION)
		{
			FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
			if(MAINDllOBJECT->RcadZoomExtents())
				MAINDllOBJECT->getWindow(1).ZoomToExtents(1);
			if(fg != NULL)
			{
				PPCALCOBJECT->getCurrentAction()->notifyAll(DATACHANGED, PPCALCOBJECT->getCurrentAction());
			//	RWrapper::RW_PartProgram::MYINSTANCE()->UpdateCurrentPPStepStatus(fg->getId(), fg->noofpts, fg->PartprogrmActionPassStatus());  //	to be deleted later, of no use
			}
			if(((AddPointAction*)PPCALCOBJECT->getCurrentAction())->CriticalAction && !fg->PartprogrmActionPassStatus())
			{
				MAINDllOBJECT->IsPartProgramPause = true;
				PausePartPartProgram::raise();	
				MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgramPause01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
				RWrapper::RW_PartProgram::MYINSTANCE()->PartProgram_Pause();
				//RWrapper::RW_PartProgram::MYINSTANCE()->Abort_PartProgram();
				return;
					/*RWrapper::RW_PartProgram::MYINSTANCE()->Abort_PartProgram();
					return;*/
			}
			if(((PerimeterAction*)PPCALCOBJECT->getCurrentAction())->PausableAction)
			{
				PPCALCOBJECT->IncrementAction();
				PausePartPartProgram::raise();
				MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgramPause01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
				RWrapper::RW_PartProgram::MYINSTANCE()->PartProgram_Pause();
				return;
			}
		}

		if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
		{
			if(((AddProbePathAction*)PPCALCOBJECT->getCurrentAction())->PausableAction)
			{
				PPCALCOBJECT->IncrementAction();
				PausePartPartProgram::raise();
				MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgramPause01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
				RWrapper::RW_PartProgram::MYINSTANCE()->PartProgram_Pause();
				return;
			}
		}
		if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::IMAGE_ACTION)
		{
			if(((AddImageAction*)PPCALCOBJECT->getCurrentAction())->PausableAction)
			{
				PPCALCOBJECT->IncrementAction();
				PausePartPartProgram::raise();
				MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgramPause01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
				RWrapper::RW_PartProgram::MYINSTANCE()->PartProgram_Pause();
				return;
			}
		}
		if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::AREA_ACTION)
		{
			if(MAINDllOBJECT->RcadZoomExtents())
				MAINDllOBJECT->getWindow(1).ZoomToExtents(1);
			if(((AreaShapeAction*)PPCALCOBJECT->getCurrentAction())->PausableAction)
			{
				PPCALCOBJECT->IncrementAction();
				PausePartPartProgram::raise();
				MAINDllOBJECT->ShowMsgBoxString("RW_PartProgramPartProgramPause01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
				RWrapper::RW_PartProgram::MYINSTANCE()->PartProgram_Pause();
				return;
			}
		}
		
		if(RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag > 0 && 
			RWrapper::RW_MainInterface::MYINSTANCE()->UseLookAhead && 
			RWrapper::RW_PartProgram::MYINSTANCE()->NumberOfCommandSend > 1)
		{
			Synchronise_Handler_Action();
			return;		
		}
		
		if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::INTERPOLATION_ACTION || 
			PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION)
		{
			RWrapper::RW_CircularInterPolation::MYINSTANCE()->StopAddingPtstoSelectedShapes(true);
		}
		
		if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION)
			PPCALCOBJECT->IncrementActionThreadAction();
		else
		{
			if(RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag > 0 && RWrapper::RW_MainInterface::MYINSTANCE()->UseLookAhead)
			{			
				//DWORD d1, d2, d3;
				//d1 = GetTickCount();
				PPCALCOBJECT->IncrementAction();
				if(PPCALCOBJECT->getCurrentAction() == NULL)
				{
					RWrapper::RW_PartProgram::MYINSTANCE()->HandlePartprogram();
					return;
				}
				if(RWrapper::RW_PartProgram::MYINSTANCE()->SendCommandInBatch())
				{
					//d2 = GetTickCount();
					//d3 = d2 - d1;
					return;
				}
				/*else
					PPCALCOBJECT->IncrementAction();*/		
			}
			else
				PPCALCOBJECT->IncrementAction();
		}

		if(RWrapper::RW_PartProgram::MYINSTANCE()->RunStepsOneByOne && PPCALCOBJECT->getCurrentAction() != NULL)
		{
			PausePartPartProgram::raise();
			RWrapper::RW_PartProgram::MYINSTANCE()->PartProgram_Pause();
			return;
		}
		RWrapper::RW_PartProgram::MYINSTANCE()->HandlePartprogram();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_PartProgramActionRedo()
{
	try
	{
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROInstance->LastTarget[0], DROInstance->LastTarget[1], DROInstance->LastTarget[2], DROInstance->LastTarget[3]};
		//RWrapper::RW_CNC::MYINSTANCE()->Activate_CNCModeDRO();
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], HandleTarget);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022a", ex);	}
}

void RWrapper::RW_DRO::Handle_PartProgramProbeHit()
{
	try
	{
		RWrapper::RW_PartProgram::MYINSTANCE()->TakeProbePoint();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_ProbeGoBack()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(PPCALCOBJECT->ProgramMadeUsingTwoStepHoming() && !PPCALCOBJECT->FinishedManualTwoStepAlignment())
			{
				HandleTarget = TargetReachedCallback::PARTPROGRAM_INCREMENT;
				FunctionTargetReached(false);
			}
			else
			{
				if(RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 0)
					DROInstance->ActivateCNC();
				HandleTarget = TargetReachedCallback::PARTPROGRAM_INCREMENT;
				FunctionTargetReached(false);
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_ProgramRerun()
{
	try
	{
		Sleep(1000);
		MAINDllOBJECT->changeUCS(0);
		RWrapper::RW_PartProgram::MYINSTANCE()->Rerun_Program();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_ProgramFinished()
{
	try
	{
		//System::Windows::Forms::MessageBox::Show("Started Target Reached on Finished Program", "Rapid-I");
		if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT ||
			MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT)
		{
			if(!RW_PartProgram::MYINSTANCE()->SingleComponentinOneShot && PPCALCOBJECT->TotalNoOfImages != PPCALCOBJECT->CurrentProramCount)
			{
				RWrapper::RW_PartProgram::MYINSTANCE()->Rerun_Program();
			}
			else
			{
				PPCALCOBJECT->ClearImageListMemory();
				PPCALCOBJECT->LoadFirstActionAfterProgramFinish();
				//MAINDllOBJECT->ShowMsgBoxString("RW_DROHandle_ProgramFinished01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION);
				MAINDllOBJECT->SetStatusCode("RW_DRO05");
			}
		}
		else if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
		{
			PPCALCOBJECT->ClearImageListMemory();
			PPCALCOBJECT->LoadFirstActionAfterProgramFinish();
			MAINDllOBJECT->SetStatusCode("RW_DRO05");
		}
		else
		{
			if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ROTARY_DEL_FI)
			{
				System::String^ WriteData = "32" + RWrapper::RW_MeasureParameter::MYINSTANCE()->CurrentGradeValue + "31";
				RWrapper::RW_LaserComm::MYINSTANCE()->SendDataTOPort(WriteData);
			}

			PPCALCOBJECT->LoadFirstActionAfterProgramFinish();
			MAINDllOBJECT->SetStatusCode("RW_DRO06");
		}
		MAINDllOBJECT->changeUCS(0);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_ProgramStepDoNothing()
{
	try
	{
		RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = false;
		Handle_PartProgramActionIncrement();
		/*if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
			{
				FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
				fg->noofpts = 0;
				fg->PartprogrmActionPassStatus(false);
			}
			Handle_PartProgramActionIncrement();
		}*/
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_AutoFocusSpan()
{
	try
	{
		int factor = 1; 
		/*if(FOCUSCALCOBJECT->NoofIterationCount > 5)
		{
			   if(FOCUSCALCOBJECT->FocusValueColl[FOCUSCALCOBJECT->NoofIterationCount-1] >  FOCUSCALCOBJECT->FocusValueColl[0])
				factor = -1;
		}*/
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
		for(int i = 0; i < 4; i++)
		{
			feedrate[i] = RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpeed;
			if(i == RWrapper::RW_FocusDepth::MYINSTANCE()->GetSelectedAxis())
				target[i] = DROCoordinate[i] + RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpan * factor * 2; // Changed sign on 2 april 2011... As per narayanan sir requirement
		}
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::PARTPROGRAM_AUTOFOCUSFINISHED);
		this->AutoFocusMode_Flag = true;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_ProgramAutoFocusSpan()
{
	try
	{
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
		for(int i = 0; i < 4; i++)
			feedrate[i] = 0.4;
		target[2] = target[2] + 2;
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::PARTPROGRAM_AUTOFOCUSFINISHED);
		this->AutoFocusMode_Flag = true;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_ProgramAutoFocusFinished()
{
	try
	{
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
		this->AutoFocusMode_Flag = false;
		double zvalue = FOCUSCALCOBJECT->Calculate_BestAutoFocus();
		if(abs(DROCoordinate[2] - zvalue) < RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpan * 2 + 1)
			target[2] = zvalue;
		if(MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::FOCUSONRIGHTCLICKHANDLER)
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::PARTPROGRAM_AUTOFOCUSPOSITIONREACHED);
		else
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::DONOTHING);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_ProgramAutoFocusPosReached()
{
	try
	{
		PPCALCOBJECT->RunCurrentStep();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_FocusDepthSpan()
{
	try
	{
		ReDoScanCount = 0;
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
		for(int i = 0; i < 4; i++)
		{
			feedrate[i] = RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpeed;
			if(i == RWrapper::RW_FocusDepth::MYINSTANCE()->GetSelectedAxis())
				target[i] = DROCoordinate[i] + RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpan * 2; // Changed sign on 2 april 2011... As per narayanan sir requirement
		}
		DROInstance->notFocusing = false;
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::FOCUSDEPTH_FINISHED);
		FOCUSCALCOBJECT->setFocusSpan(RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpan, RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpeed);
		RWrapper::RW_FocusDepth::MYINSTANCE()->Focusflag = true;
	
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_FocusCheckSpan()
{
		int factor = 1; 
		if(FOCUSCALCOBJECT->NoofIterationCount > 5)
		{
		   if(FOCUSCALCOBJECT->FocusValueColl[FOCUSCALCOBJECT->NoofIterationCount-1] >  FOCUSCALCOBJECT->FocusValueColl[0])
				factor = -1;   
		}
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
		for(int i = 0; i < 4; i++)
		{
			feedrate[i] = RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpeed;
			if(i == RWrapper::RW_FocusDepth::MYINSTANCE()->GetSelectedAxis())
				target[i] = DROCoordinate[i] + RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpan * factor * 2; // Changed sign on 2 april 2011... As per narayanan sir requirement
		}
		DROInstance->notFocusing = false;
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::FOCUSDEPTH_FINISHED);
		RWrapper::RW_FocusDepth::MYINSTANCE()->Focusflag = true;
}

void RWrapper::RW_DRO::Handle_FocusDepthContinueSpan()
{
	try
	{
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
		for(int i = 0; i < 4; i++)
		{
			feedrate[i] = RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpeed;
			if(i == RWrapper::RW_FocusDepth::MYINSTANCE()->GetSelectedAxis())
				 target[i] = DROCoordinate[i] + FocusMoveAgainValue; //Do focus Again
		}
		DROInstance->notFocusing = false;
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::FOCUSDEPTH_FINISHED);
		RWrapper::RW_FocusDepth::MYINSTANCE()->Focusflag = true;
		
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_AutoMeasure_ProbeGoBack()
{
	try
	{
		setTargetReachedTypeCallback(TargetReachedCallback::MACHINE_PROBE_PATHPOINT);
		FunctionTargetReached(false);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_SetCNCStatus()
{
	try
	{
		if(RWrapper::RW_MainInterface::MYINSTANCE()->MachineCNCStatus)
		{
			if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
		}
		else
		{
			if(RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022a", ex); }
}

void RWrapper::RW_DRO::Handle_FocusDepthFinished()
{
	try
	{
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
		if(FOCUSCALCOBJECT->CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::AUTOFOCUSSCANMODE || FOCUSCALCOBJECT->CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::AUTOCONTOURSCANMODE)
		{
			RWrapper::RW_FocusDepth::MYINSTANCE()->Focusflag = false;
			DROInstance->notFocusing = true;
			FOCUSCALCOBJECT->CalculateBestFocus();
			//RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
		}
		else if(FOCUSCALCOBJECT->CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::SCANFORRIGHTCLICK)
		{
			RWrapper::RW_FocusDepth::MYINSTANCE()->Focusflag = false;
			DROInstance->notFocusing = true;
			FOCUSCALCOBJECT->CalculateBestFocus();
			int i = RW_FocusDepth::MYINSTANCE()->GetSelectedAxis();
			target[i] = FOCUSCALCOBJECT->FocusZValue();
			if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
				target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::FOCUSDEPTH_POSITIONREACHED);
		}
		else
		{
			RWrapper::RW_FocusDepth::MYINSTANCE()->Focusflag = false;
			DROInstance->notFocusing = true;
			bool Checkflag, Dirflag; double ZmovValue = 0;
			Checkflag = FOCUSCALCOBJECT->CheckActiveWidthSpam(&ZmovValue, &Dirflag);
			if(Checkflag && ReDoScanCount < 2)
			{
				ReDoScanCount++;
				FocusMoveAgainValue = ZmovValue;
				HandleTarget = TargetReachedCallback::FOCUSDEPTH_CONTINUEACTIVESPAN;
				if(Dirflag)
				{
					FunctionTargetReached(false);
				}
				else
				{
					int i = RW_FocusDepth::MYINSTANCE()->GetSelectedAxis();
					target[i] = target[i] - (RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpan * 2 + FocusMoveAgainValue);
					if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
						 target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
					RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::FOCUSDEPTH_CONTINUEACTIVESPAN);
				}
			}
			else
			{
				ReDoScanCount = 0;
				FOCUSCALCOBJECT->CalculateBestFocus();
				int i = RW_FocusDepth::MYINSTANCE()->GetSelectedAxis();
				target[i] = FOCUSCALCOBJECT->FocusZValue();
				if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
					target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
				RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::FOCUSDEPTH_POSITIONREACHED);
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_FocusDepthFocusPositionReached()
{
	try
	{
		if(FOCUSCALCOBJECT->CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::HOMING_MODE)
		{
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
			PPCALCOBJECT->SetReferenceDotPosition();
			MAINDllOBJECT->ShowHideWaitCursor(2);
			MAINDllOBJECT->update_VideoGraphics(true);
			MAINDllOBJECT->update_RcadGraphics(true);
			return;
		}
		if(FOCUSCALCOBJECT->CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::REFERENCEDOT_SECOND)
		{
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
			PPCALCOBJECT->SetSecondReferenceDotPosition();
			MAINDllOBJECT->ShowHideWaitCursor(2);
			MAINDllOBJECT->update_VideoGraphics(true);
			MAINDllOBJECT->update_RcadGraphics(true);
			return;
		}
		else if(FOCUSCALCOBJECT->CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::CALIBRATION_MODE)
		{
			RWrapper::RW_FocusDepth::MYINSTANCE()->FocusCalibration_NextRegion();
			return;
		}
		else if(FOCUSCALCOBJECT->CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::PROFILESCAN_FOCUS)
		{
			HELPEROBJECT->ProfileScan();
			return;
		}
		else if(PPCALCOBJECT->IsPartProgramRunning() && FOCUSCALCOBJECT->CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::CONTOURFRAMEGRAB)
		{
			MAINDllOBJECT->ShowHideWaitCursor(2);
			MAINDllOBJECT->update_VideoGraphics(true);
			MAINDllOBJECT->update_RcadGraphics(true);
			PPCALCOBJECT->partProgramReached();
			return;
		}
		else if(FOCUSCALCOBJECT->CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::PROBEVISIONOFFSETFOCUS)
		{
		   RWrapper::RW_SphereCalibration::MYINSTANCE()->DoVisionCalibration(FOCUSCALCOBJECT->FocusZValue());
		   return;
		}
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
			RWrapper::RW_FocusDepth::MYINSTANCE()->CurrentFocusValue = Microsoft::VisualBasic::Strings::Format(FOCUSCALCOBJECT->FocusZValue()/25.4,"0.0000");
		else
			RWrapper::RW_FocusDepth::MYINSTANCE()->CurrentFocusValue = Microsoft::VisualBasic::Strings::Format(FOCUSCALCOBJECT->FocusZValue(),"0.0000");
		RWrapper::RW_FocusDepth::MYINSTANCE()->RaiseFocusEvents(0);
		MAINDllOBJECT->ShowHideWaitCursor(2);
		if(FOCUSCALCOBJECT->CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::FOCUS_VBLOCK_CALLIBRATION)
			 RWrapper::RW_VBlockCallibration::MYINSTANCE()->SetFocus_Zvalue();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_FocusDepthAutoFocusScanNextPosition()
{
	try
	{
		FOCUSCALCOBJECT->AutoAdjustLighting(true);
		RWrapper::RW_FocusDepth::MYINSTANCE()->AFS_ScanCurrentPosition(true);
		FOCUSCALCOBJECT->AutoAdjustLighting(false);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_FocusDepthStartDuringProgram()
{
	try
	{
		PPCALCOBJECT->RunCurrentStep();
		FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
		if(fg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_4BOX)
		{
			RW_FocusDepth::MYINSTANCE()->SetSelectedAxis((int)fg->myPosition[4].getZ());
			RWrapper::RW_FocusDepth::MYINSTANCE()->SetFocusRectangleProperties(fg->myPosition[1].getX(), fg->myPosition[1].getY(), fg->myPosition[1].getZ(), fg->myPosition[0].getX(),fg->myPosition[0].getY(),fg->myPosition[4].getX(), fg->myPosition[4].getY());
		}
		else
		{
			RW_FocusDepth::MYINSTANCE()->SetSelectedAxis((int)fg->myPosition[2].getZ());
			RWrapper::RW_FocusDepth::MYINSTANCE()->SetFocusRectangleProperties(fg->myPosition[1].getX(), fg->myPosition[1].getY(), fg->myPosition[1].getZ(), fg->myPosition[0].getX(),fg->myPosition[0].getY(),fg->myPosition[2].getX(), fg->myPosition[2].getY());
		}

		RWrapper::RW_FocusDepth::MYINSTANCE()->RaiseFocusEvents(4);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_FocusDepthSpanDuringProgram()
{
	try
	{
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
		PPCALCOBJECT->RunCurrentStep();
		FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
		if(fg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_4BOX)
		{
			RW_FocusDepth::MYINSTANCE()->SetSelectedAxis((int)fg->myPosition[4].getZ());
			RWrapper::RW_FocusDepth::MYINSTANCE()->SetFocusRectangleProperties(fg->myPosition[1].getX(), fg->myPosition[1].getY(), fg->myPosition[1].getZ(), fg->myPosition[0].getX(),fg->myPosition[0].getY(),fg->myPosition[4].getX(), fg->myPosition[4].getY());
		}
		else
		{
			RW_FocusDepth::MYINSTANCE()->SetSelectedAxis((int)fg->myPosition[2].getZ());
			RWrapper::RW_FocusDepth::MYINSTANCE()->SetFocusRectangleProperties(fg->myPosition[1].getX(), fg->myPosition[1].getY(), fg->myPosition[1].getZ(), fg->myPosition[0].getX(),fg->myPosition[0].getY(),fg->myPosition[2].getX(), fg->myPosition[2].getY());
		}
		FOCUSCALCOBJECT->ClearLastFocusValues(); //Clear the last focus values...
		FOCUSCALCOBJECT->ShiftInPartProgramX = 	int((FOCUSCALCOBJECT->FocusStartPosition.getX() - DROCoordinate[0]) / MAINDllOBJECT->getWindow(0).getUppX());
		FOCUSCALCOBJECT->ShiftInPartProgramY = 	int((FOCUSCALCOBJECT->FocusStartPosition.getY() - DROCoordinate[1]) / MAINDllOBJECT->getWindow(0).getUppX());
		FOCUSCALCOBJECT->CalculateRectangle();
		for(int i = 0; i < 4; i++)
		{
			feedrate[i] = RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpeed;
			if(i == RW_FocusDepth::MYINSTANCE()->GetSelectedAxis())
				target[i] = DROCoordinate[i] + RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpan * 2; // Changed sign on 2 april 2011... As per narayanan sir requirement
		}
		if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
		   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
		Sleep(20);
		MAINDllOBJECT->update_VideoGraphics();
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		MAINDllOBJECT->update_VideoGraphics();
		MAINDllOBJECT->Wait_VideoGraphicsUpdate();
		RWrapper::RW_FocusDepth::MYINSTANCE()->Focusflag = true;
		DROInstance->notFocusing = false;
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::FOCUSDEPTH_POSITIONREACHED_DURINGPROGRAM);
		FOCUSCALCOBJECT->setFocusSpan(RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpan, RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpeed);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_FocusDepthContinueSpanDuringProgram()
{
	try
	{
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
		for(int i = 0; i < 4; i++)
		{
			feedrate[i] = RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpeed;
			if(i == RWrapper::RW_FocusDepth::MYINSTANCE()->GetSelectedAxis())
				target[i] = DROCoordinate[i] + FocusMoveAgainValue; //Do focus 
		}
		DROInstance->notFocusing = false;
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::FOCUSDEPTH_POSITIONREACHED_DURINGPROGRAM);
		RWrapper::RW_FocusDepth::MYINSTANCE()->Focusflag = true;
		
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_FocusDepthFocusPositionReachedDuringProgram()
{
	try
	{
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
		RWrapper::RW_FocusDepth::MYINSTANCE()->Focusflag = false;
		DROInstance->notFocusing = true;
		bool Checkflag, Dirflag; double ZmovValue = 0;
		Checkflag = FOCUSCALCOBJECT->CheckActiveWidthSpam(&ZmovValue, &Dirflag);
		if(Checkflag && ReDoScanCount < 2)
		{
			ReDoScanCount++;
			FocusMoveAgainValue = ZmovValue;
			HandleTarget = TargetReachedCallback::FOCUSDEPTH_CONTINUEACTIVESPAN_PARTPROGRAM;
			if(Dirflag)
			{
				FunctionTargetReached(false);
			}
			else
			{
				for(int i = 0; i < 4; i++)
				{
					if(i == RW_FocusDepth::MYINSTANCE()->GetSelectedAxis())
						target[i] = target[i] - (RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpan * 2 + FocusMoveAgainValue);
				}
				if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
				   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
				RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::FOCUSDEPTH_CONTINUEACTIVESPAN_PARTPROGRAM);
			}
		}
		else
		{
			FOCUSCALCOBJECT->CalculateBestFocus();
			FramegrabBase* fg = PPCALCOBJECT->getFrameGrab();
			if(fg->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH)
			{
				FOCUSCALCOBJECT->AddTheDepthAction_Program();
				RWrapper::RW_FocusDepth::MYINSTANCE()->CurrentFocusValue = Microsoft::VisualBasic::Strings::Format(FOCUSCALCOBJECT->FocusZValue(),"0.0000");
			}
			FOCUSCALCOBJECT->FocusOnStatus(false);
			PPCALCOBJECT->partProgramReached();
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_FocusDepthDROPositionDuringProgram()
{
	try
	{
		FOCUSCALCOBJECT->AddDepthAction_Zposition_Program();
		PPCALCOBJECT->partProgramReached();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_ProfileScan()
{
	try
	{
		//HELPEROBJECT->ProfileScan();
		//Check for focus if required...
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};	
		FOCUSCALCOBJECT->ClearLastFocusValues();
		if(MAINDllOBJECT->getSurface())
		{
			HELPEROBJECT->ProfileScan();
		}
		else
		{
			int RectArray[4] = { 100,100, MAINDllOBJECT->currCamWidth - 200 , MAINDllOBJECT->currCamHeight - 200 };
			FOCUSCALCOBJECT->SetAutoFocusRectangle(&RectArray[0]);
			if(HELPEROBJECT->CheckCurrentFocus_ProfileScan(MAINDllOBJECT->MinimumPointsInEdge()))
				HELPEROBJECT->ProfileScan();
			else
			{
				//Calcualte the target postion:  current DRO + span;
				target[2] = target[2] - 1;
				RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::PROFILESCAN_AUTOFOCUSSPAN);
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_ProfileScanAutoFocusSpan()
{
	try
	{		
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
		for(int i = 0; i < 4; i++)
			feedrate[i] = 0.4;
		target[2] = target[2] + 2;
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::PROFILESCAN_AUTOFOCUSFINISHED);
		this->AutoFocusMode_Flag = true;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_ProfileScanAutoFocusFinished()
{
	try
	{
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
		this->AutoFocusMode_Flag = false;
		double zvalue = FOCUSCALCOBJECT->Calculate_BestAutoFocus();
		target[2] = zvalue;
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::PROFILESCAN_AUTOFOCUSPOSITIONREACHED);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_ProfileScanAutoFocusPosReached()
{
	try
	{
		HELPEROBJECT->ProfileScan();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_VblockCallibration()
{
	try
	{
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
		int RectArray[4] = { 100,100, MAINDllOBJECT->currCamWidth - 200 , MAINDllOBJECT->currCamHeight - 200 };
		FOCUSCALCOBJECT->ClearLastFocusValues();
		FOCUSCALCOBJECT->SetAutoFocusRectangle(&RectArray[0]);
		if(HELPEROBJECT->CheckCurrentFocus_ProfileScan())
		{
			RWrapper::RW_VBlockCallibration::MYINSTANCE()->create_Line(false);
			RWrapper::RW_VBlockCallibration::MYINSTANCE()->create_AngleBisector();
		}
		else
		{
			target[2] = target[2] - 2;
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::VBLOCKCALLIBRATION_AUTOFOCUSSPAN);
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0023", ex);	}
}

void RWrapper::RW_DRO::Handle_VblockCallibrationAutoFocusSpan()
{
	try
	{
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
		for(int i = 0; i < 4; i++)
			feedrate[i] = 0.4;
		target[2] = target[2] + 4;
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::VBLOCKCALLIBRATION_AUTOFOCUSFINISHED);
		this->AutoFocusMode_Flag = true;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0024", ex);	}
}

void RWrapper::RW_DRO::Handle_VblockCallibrationAutoFocusFinished()
{
	try
	{
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
		this->AutoFocusMode_Flag = false;
		double zvalue = FOCUSCALCOBJECT->Calculate_BestAutoFocus();
		target[2] = zvalue;
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::VBLOCKCALLIBRATION_AUTOFOCUSPOSITIONREACHED);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0025", ex);	}
}

void RWrapper::RW_DRO::Handle_CircularRepeatAutoMeasure()
{
	try
	{
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
		int RectArray[4] = { 100,100, MAINDllOBJECT->currCamWidth - 200 , MAINDllOBJECT->currCamHeight - 200 };
		FOCUSCALCOBJECT->ClearLastFocusValues();
		FOCUSCALCOBJECT->SetAutoFocusRectangle(&RectArray[0]);
		if(HELPEROBJECT->CheckCurrentFocus_ProfileScan(350))
		{
			AUTOMEASUREOBJECT->get_FramGrabParameter();
		}
		else
		{
			target[2] = target[2] - 1;
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::REPEATAUTOMEASURE_AUTOFOCUSSPAN);
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0023", ex);	}
}

void RWrapper::RW_DRO::Handle_CircularRepeatAutoMeasureFocusSpan()
{
	try
	{
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
		for(int i = 0; i < 4; i++)
			feedrate[i] = 0.4;
		target[2] = target[2] + 2;
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::REPEATAUTOMEASURE_AUTOFOCUSFINISHED);
		this->AutoFocusMode_Flag = true;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0024", ex);	}
}

void RWrapper::RW_DRO::Handle_CircularRepeatAutoMeasureFocusFinished()
{
	try
	{
		double feedrate[4] = {feedRate[0], feedRate[1], feedRate[2], feedRate[3]};
		double target[4] = {DROCoordinate[0], DROCoordinate[1], DROCoordinate[2], DROCoordinate[3]};
		this->AutoFocusMode_Flag = false;
		double zvalue = FOCUSCALCOBJECT->Calculate_BestAutoFocus();
		target[2] = zvalue;
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::REPEATAUTOMEASURE_AUTOFOCUSPOSITIONREACHED);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0025", ex);	}
}

void RWrapper::RW_DRO::Handle_AutoCalibrationMode()
{
	try
	{
		RWrapper::RW_AutoCalibration::MYINSTANCE()->FrameGrabPointsToSelectedLine();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_AutoWidthMeasureMode()
{
	try
	{
		RWrapper::RW_AutoCalibration::MYINSTANCE()->FrameGrabPoints_WidthMeasure();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_AutoCalibrationNextOrnt()
{
	try
	{
		RWrapper::RW_AutoCalibration::MYINSTANCE()->StartNextOrientation();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_AutoCalibrationLastOrnt()
{
	try
	{
		RWrapper::RW_AutoCalibration::MYINSTANCE()->GotoNextStep(false);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022b", ex);	}
}

void RWrapper::RW_DRO::Handle_AutoZoomingMode()
{
	try
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->StartHideWaitStatusMessage(10);
		if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
			RWrapper::RW_DBSettings::MYINSTANCE()->UpdateVideoUpp_DROCallback(OriginalDROCoordinate[3]);
		Sleep(30);
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_AutoZoomingModeDuringExeLoad()
{
	try
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->StartHideWaitStatusMessage(10);
		//if (!MAINDllOBJECT->IsProbePresent())
		double Target[4] = { RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] };
		DROInstance->ToReset(Target);
		Sleep(30);
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_Auto_SGWidthReapet()
{
	try
	{
		RWrapper::RW_AutoCalibration::MYINSTANCE()->Handle_MeasureSlipGaugeWidthClicked();
	}
   catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_MachineGotoCommand()
{
	try
	{
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(0);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_MachineGotoRcadWindowClick()
{
	try
	{
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_MachineGotoSAOICellClick()
{
	try
	{
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_MachineGotoAutoMeasureMode()
{
	try
	{
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_MachineHomingMode()
{
	try
	{
		RWrapper::RW_AutoCalibration::MYINSTANCE()->ReachedHomePosition();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_MachineMoveLittleAfterHoming()
{
	try
	{
		RWrapper::RW_AutoCalibration::MYINSTANCE()->ReachedNearHomePosition();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

void RWrapper::RW_DRO::Handle_MachineMoveSlowForHoming()
{
	try
	{
		RWrapper::RW_AutoCalibration::MYINSTANCE()->ReachedTargetPosition();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022b", ex);	}
}

void RWrapper::RW_DRO::Handle_MoveCircularStep()
{
	try
	{
		RWrapper::RW_CNC::MYINSTANCE()->MoveCircularStep();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);	}
}

int RWrapper::RW_DRO::SetProbeRevertDistSpeed(int AxisNo, double RevertDist, double RevertSpeed, double ApproachDist, double ApproachSpeed, double ProbeSensitivity, bool ProbeMoveUpFlag)

{
	try
	{
		int UpdatedValues = 0;
		if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag < 1 && !MAINDllOBJECT->IsProbePresent()) return 0;

		RevertDist = System::Math::Abs(RevertDist);
		if (RevertDist > 30) RevertDist = 30;
		if (this->RevertDistance != RevertDist) UpdatedValues++;
		this->RevertDistance = RevertDist;

		RevertSpeed = System::Math::Abs(RevertSpeed);
		if (RevertSpeed < 0.1) RevertSpeed = 0.1;
		if (RevertSpeed > 20) RevertSpeed = 20;
		if (this->RevertSpeed != RevertSpeed) UpdatedValues++;
		this->RevertSpeed = RevertSpeed;

		ApproachDist = System::Math::Abs(ApproachDist);
		if (ApproachDist > 30) ApproachDist = 30;
		if (this->ApproachDistance != ApproachDist) UpdatedValues++;
		this->ApproachDistance = ApproachDist;

		ApproachSpeed = System::Math::Abs(ApproachSpeed);
		if (ApproachSpeed < 0.1) ApproachSpeed = 0.1;
		if (ApproachSpeed > 20) ApproachSpeed = 20;
		if (this->ApproachSpeed != ApproachSpeed) UpdatedValues++;
		this->ApproachSpeed = ApproachSpeed;

		ProbeSensitivity = System::Math::Abs(ProbeSensitivity);
		if (ProbeSensitivity < 1) ProbeSensitivity = 1;
		if (ProbeSensitivity > 500) ProbeSensitivity = 500;
		if (this->ProbeSensitivity != ProbeSensitivity) UpdatedValues++;
		this->ProbeSensitivity = ProbeSensitivity;

		//ProbeParameters^ thisProbeParams;
		//bool GotProbeParams = false;
		//for (int ii = 0; ii < this->ProbeParams->Count; ii++)
		//{
		//	if (this->ProbeParams[ii]->AxisLabel == AxisNo)
		//	{
		//		thisProbeParams = this->ProbeParams[ii];
		//		GotProbeParams = true;
		//		break;
		//	}
		//}
		//if (!GotProbeParams)
		//	thisProbeParams = gcnew ProbeParameters;

		//thisProbeParams->AxisLabel = AxisNo;
		//thisProbeParams->ApproachDist = ApproachDist;
		//thisProbeParams->ApproachSpeed = ApproachSpeed;
		//thisProbeParams->RetractDist = RevertDist;

		//if (!GotProbeParams)
		//	this->ProbeParams->Add(thisProbeParams);
		//if (UpdatedValues > 0)
		{
			//if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 2)
				DROInstance->SetPorbeRevertbackDistance(AxisNo, RevertDist, RevertSpeed, ApproachDist, ApproachSpeed, ProbeSensitivity, ProbeMoveUpFlag);
			/*else
			{
				for (int i = 0; i < 4; i++)
					DROInstance->SetPorbeRevertbackDistance(i, RevertDist, RevertSpeed, ApproachDist, ApproachSpeed, ProbeSensitivity, ProbeMoveUpFlag);
			}*/
		}
		//Sleep(50);
		return UpdatedValues;
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0022", ex);
		return -1;
	}
}

void RWrapper::RW_DRO::SetScaleDirection(bool PositiveFlag, int Axisno, bool Startup)
{
	try
	{
		if (Startup && RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag != 1) return;
		char AxisCnt[2] = "\0";
		_itoa(Axisno, AxisCnt, 10);
		std::string CommandStr, AxisStr;
		if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag < 2)
		{
			AxisStr = AxisCnt;
			if (AxisStr.length() > 1)
				AxisStr = AxisStr.substr(0, 1);

			unsigned int ScaleDir = 0x0000;
			if (PositiveFlag) ScaleDir = 0x0080;
			this->ScaleData[Axisno] = (this->ScaleData[Axisno] & 0XFF0F) | ScaleDir;
			const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(Convert::ToString(this->ScaleData[Axisno], 16))).ToPointer();
			//if (PositiveFlag)
			CommandStr = chars;
			if (CommandStr.length() < 4)	
				CommandStr = "0" + CommandStr;
			CommandStr = "1" + AxisStr + "00" + CommandStr; // "0F80";
													   //else
													   //	CommandStr = "1" + AxisStr + "00" + "0F00";
			Marshal::FreeHGlobal(IntPtr((void*)chars));

			//if (PositiveFlag)
			//	CommandStr = "1" + AxisStr + "00" + "0F80";
			//else
			//	CommandStr = "1" + AxisStr + "00" + "0F00";
			char tempstr[12] = "\0";
			for (int i = 0; i < CommandStr.length(); i++)
			{
				tempstr[i] = toupper(CommandStr.substr(i, 1).front());
			}
			CommandStr = (const char*)tempstr;
			std::reverse(CommandStr.begin(), CommandStr.end());
			CommandStr = "E" + CommandStr;
		}
		else
		{
			System::String^ tempVal = "0";
			if (!PositiveFlag) tempVal = "1";
			System::String^ tempScaleDirectionVal;
			for (int i = 0; i < NumberofAxes; i++)
			{
				if (i == Axisno)
					tempScaleDirectionVal = tempVal + tempScaleDirectionVal;

				else
					tempScaleDirectionVal = this->ScaleDirectionString->Substring(NumberofAxes - i - 1, 1) + tempScaleDirectionVal;
			}
			this->ScaleDirectionString = tempScaleDirectionVal;
			const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(this->ScaleDirectionString)).ToPointer();
			CommandStr = chars;
			CommandStr = CommandStr;
			CommandStr = CommandStr.substr(0, 4);
			Marshal::FreeHGlobal(IntPtr((void*)chars));
			ScaleDirectionCallsCount++;
			if (ScaleDirectionCallsCount < NumberofAxes) return;
			std::reverse(CommandStr.begin(), CommandStr.end());
			CommandStr = "02" + CommandStr;
			std::reverse(CommandStr.begin(), CommandStr.end());
			CommandStr = "E" + CommandStr;
		}
		ScaleDirectionCallsCount = 0;
		DROInstance->sendMachineCommand_One((char*)CommandStr.c_str(), true);
	}
	catch (Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0023", ex); }
}

void RWrapper::RW_DRO::SetMotorDirection(bool PositiveFlag, int Axisno, bool Startup)
{
	try
	{
		if (Startup && RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag != 1) return;
		char AxisCnt[2] = "\0";
		_itoa(Axisno, AxisCnt, 10);
		std::string CommandStr, AxisStr;

		if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag < 2)
		{
			AxisStr = AxisCnt;
			if (AxisStr.length() > 1)
				AxisStr = AxisStr.substr(0, 1);

			unsigned int MotrDir = 0x0000;
			if (PositiveFlag) MotrDir = 0x0008;
			this->ScaleData[Axisno] = (this->ScaleData[Axisno] & 0XFFF0) | MotrDir;
			const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(Convert::ToString(this->ScaleData[Axisno], 16))).ToPointer();
			CommandStr = chars;
			if (CommandStr.length() < 4)	CommandStr = "0" + CommandStr;
			//if (PositiveFlag)
			CommandStr = "1" + AxisStr + "00" + CommandStr; // "0F80";
			//else
			//	CommandStr = "1" + AxisStr + "00" + "0F00";
			Marshal::FreeHGlobal(IntPtr((void*)chars));
			char tempstr[12] = "\0";
			for (int i = 0; i < CommandStr.length(); i++)
			{
				tempstr[i] = toupper(CommandStr.substr(i, 1).front());
			}
			CommandStr = (const char*)tempstr;
			std::reverse(CommandStr.begin(), CommandStr.end());
			CommandStr = "E" + CommandStr;
		}
		else
		{
			System::String^ tempVal = "0";
			if (!PositiveFlag) tempVal = "1";
			System::String^ tempScaleDirectionVal;
			for (int i = 0; i < NumberofAxes; i++)
			{
				if (i == Axisno)
					tempScaleDirectionVal = tempVal + tempScaleDirectionVal;

				else
					tempScaleDirectionVal = this->MotorDirectionString->Substring(NumberofAxes - i - 1, 1) + tempScaleDirectionVal;
			}
			this->MotorDirectionString = tempScaleDirectionVal;
			const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(this->MotorDirectionString)).ToPointer();
			CommandStr = chars;
			CommandStr = CommandStr;
			CommandStr = CommandStr.substr(0, 4);
			Marshal::FreeHGlobal(IntPtr((void*)chars));
			MotorDirectionCallsCount++;
			if (MotorDirectionCallsCount < NumberofAxes) return;
			std::reverse(CommandStr.begin(), CommandStr.end());
			CommandStr = "01" + CommandStr;
			std::reverse(CommandStr.begin(), CommandStr.end());
			CommandStr = "E" + CommandStr;
		}
		MotorDirectionCallsCount = 0;
		DROInstance->sendMachineCommand_One((char*)CommandStr.c_str(), true);
	}
	catch (Exception^ ex) { RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0023", ex); }
}


void RWrapper::RW_DRO::SetAccuracy(int value, int Axisno, bool Startup)
{
	try
	{
		if(Startup || RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag != 1) return;
		char AxisCnt[2] = "\0";
		char axTarget[5] = "\0";
		std::string CommandStr, AxisStr, tempCalculate, AxisTarget;
		_itoa(Axisno, AxisCnt, 10);		
		AxisStr = AxisCnt;
		if(AxisStr.length() > 1)
			AxisStr = AxisStr.substr(0,1);
		_ltoa(long(value), axTarget, 16);
		tempCalculate = axTarget;
		if(tempCalculate.length() > 4)
			tempCalculate = tempCalculate.substr(0,4);
		for(int i = tempCalculate.length(); i < 4; i++)
			tempCalculate = "0"+ tempCalculate;
		AxisTarget = "1" + AxisStr + "01" + tempCalculate;
		char tempstr[9] = "\0";
		for (int i = 0; i < AxisTarget.length(); i++)
		{
			if (i == 9) break;
			tempstr[i] = toupper(AxisTarget.substr(i,1).front());
		}
		CommandStr = (const char*)tempstr;
		std::reverse(CommandStr.begin(), CommandStr.end());
		CommandStr = "E" + CommandStr;
		DROInstance->sendMachineCommand_One((char*)CommandStr.c_str(), true);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0024", ex);	}
}

void RWrapper::RW_DRO::SetDeceleration(int value, int Axisno, bool Startup)
{
	try
	{
		if (Startup && RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag != 1) return;
		char AxisCnt[2] = "\0";
		char axTarget[5] = "\0";
		std::string CommandStr, AxisStr, tempCalculate, AxisTarget;
		if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag < 2)
		{
			_itoa(Axisno, AxisCnt, 10);
			AxisStr = AxisCnt;
			if (AxisStr.length() > 1)
				AxisStr = AxisStr.substr(0, 1);
			_ltoa(long(value), axTarget, 16);
			tempCalculate = axTarget;
			if (tempCalculate.length() > 4)
				tempCalculate = tempCalculate.substr(0, 4);
			for (int i = tempCalculate.length(); i < 4; i++)
				tempCalculate = "0" + tempCalculate;
			AxisTarget = "1" + AxisStr + "04" + tempCalculate;
			char tempstr[9] = "\0";
			for (int i = 0; i < AxisTarget.length(); i++)
			{
				if (i == 9) break;
				tempstr[i] = toupper(AxisTarget.substr(i, 1).front());
			}
			CommandStr = (const char*)tempstr;
			std::reverse(CommandStr.begin(), CommandStr.end());
			CommandStr = "E" + CommandStr;
		}
		else
		{
			_itoa(Axisno + 21, AxisCnt, 16);
			AxisStr = AxisCnt;
			if (AxisStr.length() < 2)
				AxisStr = "0" + AxisStr;
			if (AxisStr.length() > 2)
				AxisStr = AxisStr.substr(0, 2);
			_ltoa(long(value), axTarget, 16);
			tempCalculate = axTarget;
			if (tempCalculate.length() > 4)
				tempCalculate = tempCalculate.substr(0, 4);
			for (int i = tempCalculate.length(); i < 4; i++)
				tempCalculate = "0" + tempCalculate;
			AxisTarget = AxisStr + tempCalculate;
			char tempstr[6] = "\0";
			for (int i = 0; i < AxisTarget.length(); i++)
			{
				tempstr[i] = toupper(AxisTarget.substr(i, 1).front());
			}
			CommandStr = (const char*)tempstr;
			CommandStr = CommandStr.substr(0, 6);
			std::reverse(CommandStr.begin(), CommandStr.end());
			CommandStr = "E" + CommandStr;
		}

		DROInstance->sendMachineCommand_One((char*)CommandStr.c_str(), true);
	}
	catch (Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0025", ex); }
}

void RWrapper::RW_DRO::SetSpeedControls(cli::array<int>^ InputWriteValues) // int AccelFactor_Manual, int AccelFactor_CNC, int JS_JogSpeedValue)
{
	try
	{
		if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag < 2) return;

		//cli::array<int>^ WriteValues = gcnew cli::array<int> {AccelFactor_Manual, AccelFactor_CNC, JS_JogSpeedValue};
		cli::array<String^>^ Headers = gcnew cli::array<String^> {"65", "66", "67"};
		for (int i = 0; i < 3; i++)
		{
			std::string CommandStr;

			System::String^ hexValue = Convert::ToString(InputWriteValues[i], 16);
			if (hexValue->Length > 4)
				hexValue = hexValue->Substring(0, 4);
			else if (hexValue->Length < 4)
			{
				for (int jj = hexValue->Length; jj < 4; jj++)
					hexValue = "0" + hexValue;
			}
			hexValue = Headers[i] + hexValue;

			const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(hexValue)).ToPointer();
			CommandStr = chars;
			CommandStr = CommandStr;
			CommandStr = CommandStr.substr(0, 6);
			Marshal::FreeHGlobal(IntPtr((void*)chars));

			char tempstr[6] = "\0";
			for (int i = 0; i < CommandStr.length(); i++)
			{
				tempstr[i] = toupper(CommandStr.substr(i, 1).front());
			}
			CommandStr = (const char*)tempstr;
			CommandStr = CommandStr.substr(0, 6);
			std::reverse(CommandStr.begin(), CommandStr.end());
			CommandStr = "E" + CommandStr;
			DROInstance->sendMachineCommand_One((char*)CommandStr.c_str(), true);
		}
	}
	catch (Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0026B", ex); }
}


void RWrapper::RW_DRO::SetAcceleration(int value, int Axisno, bool Startup)
{
	try
	{
		if (Startup && RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag != 1) return;
		char AxisCnt[2] = "\0";
		char axTarget[5] = "\0";
		std::string CommandStr, AxisStr, tempCalculate, AxisTarget;
		if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag < 2)
		{
			_itoa(Axisno, AxisCnt, 10);
			AxisStr = AxisCnt;
			if (AxisStr.length() > 1)
				AxisStr = AxisStr.substr(0, 1);
			_ltoa(long(value), axTarget, 16);
			tempCalculate = axTarget;
			if (tempCalculate.length() > 4)
				tempCalculate = tempCalculate.substr(0, 4);
			for (int i = tempCalculate.length(); i < 4; i++)
				tempCalculate = "0" + tempCalculate;
			AxisTarget = "1" + AxisStr + "05" + tempCalculate;
			char tempstr[9] = "\0";
			for (int i = 0; i < AxisTarget.length(); i++)
			{
				if (i == 9) break;
				tempstr[i] = toupper(AxisTarget.substr(i, 1).front());
			}
			CommandStr = (const char*)tempstr;
			std::reverse(CommandStr.begin(), CommandStr.end());
			CommandStr = "E" + CommandStr;
		}
		else
		{
			_itoa(Axisno + 11, AxisCnt, 16);
			AxisStr = AxisCnt;
			if (AxisStr.length() < 2)
				AxisStr = "0" + AxisStr;
			if (AxisStr.length() > 2)
				AxisStr = AxisStr.substr(0, 2);
			_ltoa(long(value), axTarget, 16);
			tempCalculate = axTarget;
			if (tempCalculate.length() > 4)
				tempCalculate = tempCalculate.substr(0, 4);
			for (int i = tempCalculate.length(); i < 4; i++)
				tempCalculate = "0" + tempCalculate;
			AxisTarget = AxisStr + tempCalculate;
			char tempstr[6] = "\0";
			for (int i = 0; i < AxisTarget.length(); i++)
			{
				tempstr[i] = toupper(AxisTarget.substr(i, 1).front());
			}
			CommandStr = (const char*)tempstr;
			CommandStr = CommandStr.substr(0, 6);
			std::reverse(CommandStr.begin(), CommandStr.end());
			CommandStr = "E" + CommandStr;
		}
		DROInstance->sendMachineCommand_One((char*)CommandStr.c_str(), true);
	}
	catch (Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0026", ex); }
}

void RWrapper::RW_DRO::SetPitch(int value, int Axisno, bool Startup)
{
	try
	{
		//if(Startup && RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag != 1) return;
		char AxisCnt[2] = "\0";
		char axTarget[5] = "\0";
		std::string CommandStr, AxisStr, tempCalculate, AxisTarget;
		if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag < 2)
		{
			_itoa(Axisno, AxisCnt, 10);
			AxisStr = AxisCnt;
			if (AxisStr.length() > 1)
				AxisStr = AxisStr.substr(0, 1);
			_ltoa(long(value), axTarget, 16);
			tempCalculate = axTarget;
			if (tempCalculate.length() > 4)
				tempCalculate = tempCalculate.substr(0, 4);
			for (int i = tempCalculate.length(); i < 4; i++)
				tempCalculate = "0" + tempCalculate;
			AxisTarget = "1" + AxisStr + "08" + tempCalculate;
			char tempstr[9] = "\0";
			for (int i = 0; i < AxisTarget.length(); i++)
			{
				if (i == 9) break;
				tempstr[i] = toupper(AxisTarget.substr(i, 1).front());
			}
			CommandStr = (const char*)tempstr;
		}
		else
		{
			_itoa(Axisno + 31, AxisCnt, 16);
			AxisStr = AxisCnt;
			if (AxisStr.length() < 2)
				AxisStr = "0" + AxisStr;
			if (AxisStr.length() > 2)
				AxisStr = AxisStr.substr(0, 2);
			_ltoa(long(value), axTarget, 16);
			tempCalculate = axTarget;
			if (tempCalculate.length() > 4)
				tempCalculate = tempCalculate.substr(0, 4);
			for (int i = tempCalculate.length(); i < 4; i++)
				tempCalculate = "0" + tempCalculate;
			AxisTarget = AxisStr + tempCalculate;
			char tempstr[6] = "\0";
			for (int i = 0; i < AxisTarget.length(); i++)
			{
				tempstr[i] = toupper(AxisTarget.substr(i, 1).front());
			}
			CommandStr = (const char*)tempstr;
			CommandStr = CommandStr.substr(0, 6);
		}
		std::reverse(CommandStr.begin(), CommandStr.end());
		CommandStr = "E" + CommandStr;
		DROInstance->sendMachineCommand_One((char*)CommandStr.c_str(), true);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0027", ex);	}
}

void RWrapper::RW_DRO::SetJoysticSpeedFactor(int value, int Axisno, bool Startup)
{
	try
	{
		//if(Startup && RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag != 1) return;
		char AxisCnt[2] = "\0";
		char axTarget[5] = "\0";
		std::string CommandStr, AxisStr, tempCalculate, AxisTarget;
		if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag < 2)
		{
			_itoa(Axisno, AxisCnt, 10);
			AxisStr = AxisCnt;
			if (AxisStr.length() > 1)
				AxisStr = AxisStr.substr(0, 1);
			_ltoa(long(value), axTarget, 16);
			tempCalculate = axTarget;
			if (tempCalculate.length() > 4)
				tempCalculate = tempCalculate.substr(0, 4);
			for (int i = tempCalculate.length(); i < 4; i++)
				tempCalculate = "0" + tempCalculate;
			AxisTarget = "1" + AxisStr + "0A" + tempCalculate;				//vinod..................
			char tempstr[9] = "\0";
			for (int i = 0; i < AxisTarget.length(); i++)
			{
				if (i == 9) break;
				tempstr[i] = toupper(AxisTarget.substr(i, 1).front());
			}
			CommandStr = (const char*)tempstr;
		}
		else
		{
			if (Axisno == 0) this->JoystickSpeedFactorString = "";
			long spValue = abs((long)value);
			if (spValue > 10) spValue = 10;

			System::String^ hexValue = Convert::ToString(spValue, 16);
			hexValue = hexValue->Substring(0, 1);

			this->JoystickSpeedFactorString += hexValue;
			if (this->JoystickSpeedFactorString->Length < NumberofAxes) return;

			const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(this->JoystickSpeedFactorString)).ToPointer();
			CommandStr = chars;
			CommandStr = CommandStr;
			CommandStr = CommandStr.substr(0, 4);
			Marshal::FreeHGlobal(IntPtr((void*)chars));

			CommandStr = "28" + CommandStr; //Joystick speed factor header = 40; hex value is 28...
			char tempstr[6] = "\0";
			for (int i = 0; i < 6; i++)
				tempstr[i] = toupper(CommandStr.substr(i, 1).front());

			CommandStr = (const char*)tempstr;
			CommandStr = CommandStr.substr(0, 6);
		}
		std::reverse(CommandStr.begin(), CommandStr.end());
		CommandStr = "E" + CommandStr;
		DROInstance->sendMachineCommand_One((char*)CommandStr.c_str(), true);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0028", ex);	}
}

//by narayanan sir
void RWrapper::RW_DRO::SetResolution(int value, int Axisno, bool Startup)
{
	try
	{
		if(Startup || RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag != 1) return;
		char AxisCnt[2] = "\0";
		char axTarget[5] = "\0";
		std::string CommandStr, AxisStr, tempCalculate, AxisTarget;
		_itoa(Axisno, AxisCnt, 10);		
		AxisStr = AxisCnt;
		if(AxisStr.length() > 1)
			AxisStr = AxisStr.substr(0,1);
		_ltoa(long(value), axTarget, 16);
		tempCalculate = axTarget;
		if(tempCalculate.length() > 4)
			tempCalculate = tempCalculate.substr(0,4);
		for(int i = tempCalculate.length(); i < 4; i++)
			tempCalculate = "0"+ tempCalculate;
		AxisTarget = "1" + AxisStr + "09" + tempCalculate;
		char tempstr[9] = "\0";
		for (int i = 0; i < AxisTarget.length(); i++)
		{
			if (i == 9) break;
			tempstr[i] = toupper(AxisTarget.substr(i,1).front());
		}
		CommandStr = (const char*)tempstr;
		std::reverse(CommandStr.begin(), CommandStr.end());
		CommandStr = "E" + CommandStr;
		DROInstance->sendMachineCommand_One((char*)CommandStr.c_str(), true);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0027", ex);	}
}

//Don't reverse string it is already in reverse order..
void RWrapper::RW_DRO::Activate_DeactivateProbe(bool X_Axis, bool Y_Axis, bool Z_Axis, bool R_Axis, bool Startup)
{
	try
	{
		//Probe is ALWAYS active for v33.2. This command does not exist!!
		if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 2) return;

		if(Startup && RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 0) 
			DROInstance->Activate_Deactivate_Probe(X_Axis, Y_Axis, Z_Axis, R_Axis, true);
		else
			DROInstance->Activate_Deactivate_Probe(X_Axis, Y_Axis, Z_Axis, R_Axis, false);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0029", ex);	}
}

//Don't reverse string it is already in reverse order..
void RWrapper::RW_DRO::Activate_Deactivate_Zoom_Cnc(bool CncFlag, bool ZoomFlag, bool SmoothMovement, bool RetractProbePoint, bool Startup, int AxisEnable, bool DRO_24Bit)
{
	try
	{
		if(Startup && RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag != 1) return;
		if(RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 0) 
			RetractProbePoint = false;
		DROInstance->SetProbePositionSmoothMovement(SmoothMovement);
		std::string CommandStr, AxisStr;
		int Binary_Zoom = 1, Binary_Val = 0;
		if(!ZoomFlag) Binary_Zoom = 0;
		int Binary_Cnc = 1;
		if(!CncFlag) Binary_Cnc = 0;
		if (CncFlag)
			Binary_Cnc <<= 2;

		Binary_Val = Binary_Zoom | Binary_Cnc;
		char TmpArray[2] = {DROInstance->dec_hex(Binary_Val), '\0'};
		std::string ValStr = (const char*)TmpArray;
		int TmpValue = 0, TmpBinaryValue = 1;
		if(SmoothMovement)
		{
			TmpBinaryValue <<= 2;
			TmpValue = TmpValue | TmpBinaryValue;
		}
		if(RetractProbePoint)
		{
			TmpBinaryValue = 1;
			TmpBinaryValue <<= 3;
			TmpValue = TmpValue | TmpBinaryValue;
		}
		char TmpValue_Array[2] = {DROInstance->dec_hex(TmpValue), '\0'};
		std::string ValStr_Hex = (const char*)TmpValue_Array;

		int droval = 0;
		if (DRO_24Bit)
			droval = 1;
		droval <<= 3;
		char DROArray[2] = { DROInstance->dec_hex(droval), '\0' };
		std::string dro_Str = (const char*)DROArray;

		char AxisEnableArray[2] = { DROInstance->dec_hex(AxisEnable), '\0' };
		std::string AxisEn_str = (const char*)AxisEnableArray;
		//if(RWrapper::RW_DBSettings::MYINSTANCE()->NoofMachineAxis == 4)
		//{
			CommandStr = AxisEn_str + ValStr_Hex + dro_Str + ValStr + "00F1";
		//}
		//else
		//{
		//	CommandStr = "7" + ValStr_Hex + "0" + ValStr + "00F1";
		//}
		char tempstr[9] = "\0";
		for (int i = 0; i < 8; i++)
		{
			if (i == 9) break;
			tempstr[i] = toupper(CommandStr.substr(i,1).front());
		}
		CommandStr = (const char*)tempstr;
		CommandStr = "E" + CommandStr;
		DROInstance->sendMachineCommand_One((char*)CommandStr.c_str(), true);
		this->RetractProbePointMethod = RetractProbePoint;
		DROInstance->SetProbeOnward_RetractMethod(RetractProbePoint);
		
	}
		catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0030", ex);	}
}

void RWrapper::RW_DRO::SetInterPolationParameter(bool enableInterPolation, bool SmoothInterpolation)
{
	try
	{
		if(RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag != 1) return;
		DROInstance->SetInterpolationFlag(enableInterPolation);
		std::string CommandStr = "3";
		if(enableInterPolation)
		{
			if(SmoothInterpolation)
				CommandStr = "4";
		}
		else 
		{
			if(SmoothInterpolation)
				CommandStr = "2";
			else
				CommandStr = "1";
		}
		CommandStr = "0" + CommandStr;
		DROInstance->sendMachineCommand_One((char*)CommandStr.c_str(), true);
	}
		catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0031", ex);	}
}

void RWrapper::RW_DRO::SetProbeReractModeForManual(bool enableRetract)
{
	try
	{
		if(RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag != 1) return;
		std::string CommandStr = "C8";
		if(enableRetract)
			 CommandStr = "C7";
		DROInstance->sendMachineCommand_One((char*)CommandStr.c_str(), true);
	}
		catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0032", ex);	}
}

void RWrapper::RW_DRO::WriteToEEPROM()
{
	try
	{
		std::string CommandStr = "E000046";
		DROInstance->sendMachineCommand_One((char*)CommandStr.c_str(), true);
	}
	catch (Exception ^ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0031a", ex);
	}
}

void RWrapper::RW_DRO::ResetMachineStuckFlag()
{
	try
	{
		DROInstance->SetMachineStuckFlag();
	}
		catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0032", ex);	}
}

void RWrapper::RW_DRO::RetractAlongDirectionOfMovement(bool flag)
{
	try
	{
		DROInstance->ProbeRetractAlongDirectionOfMovement(flag);
	}
		catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0033", ex);	}
}

void RWrapper::RW_DRO::ResetDroValue(cli::array<double> ^ResetValue, bool Zeroing)
{
	try
	{
		double Value[4] = {0};
		for(int i = 0; i < 4; i++)
			Value[i] = ResetValue[i];
		DROInstance->ToReset(Value);
	}
		catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0034", ex);	}
}

void RWrapper::RW_DRO::SendMachineAtZeroingPosition(cli::array<double> ^OffsetValue)
{
	try
	{
		if(MAINDllOBJECT->ReferenceDotShape != NULL)
		{
			double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
			double target[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
			target[0] = OffsetValue[0] + ((Circle*)MAINDllOBJECT->ReferenceDotShape)->getCenter()->getX();
			target[1] = OffsetValue[1] + ((Circle*)MAINDllOBJECT->ReferenceDotShape)->getCenter()->getY();
			target[2] = OffsetValue[2] + ((Circle*)MAINDllOBJECT->ReferenceDotShape)->getCenter()->getZ();
			if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::MACHINE_GOTOCOMMAND);
		}
	}
		catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0035", ex);	}
}

void RWrapper::RW_DRO::ResetOffsetPosition()
{
	try
	{
		if(MAINDllOBJECT->ReferenceDotShape != NULL)
		{
			cli::array<double>^ Offset = gcnew cli::array<double>(3);
			Offset[0] = MAINDllOBJECT->getCurrentDRO().getX() - ((Circle*)MAINDllOBJECT->ReferenceDotShape)->getCenter()->getX();
			Offset[1] = MAINDllOBJECT->getCurrentDRO().getY() - ((Circle*)MAINDllOBJECT->ReferenceDotShape)->getCenter()->getY();
			Offset[2] = MAINDllOBJECT->getCurrentDRO().getZ() - ((Circle*)MAINDllOBJECT->ReferenceDotShape)->getCenter()->getZ();
			ZeroingOffsetEvent::raise(Offset);
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRDRO0036", ex);	}
}

void RWrapper::RW_DRO::Set2StepPP_Flag(bool SetFlag)
{
	DROInstance->ActivateNextStepFor_2sPP = SetFlag;
}

void RWrapper::RW_DRO::ConnectOnlineSystem(bool StopValues)
{
	DROInstance->ConnectToOnlineSystem(StopValues);
}
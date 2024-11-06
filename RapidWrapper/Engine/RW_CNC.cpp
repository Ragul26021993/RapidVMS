
#include "StdAfx.h"
#include "RW_MainInterface.h"
#include "RW_DelphiCamModule.h"
#include "..\MAINDLL\Engine\PartProgramFunctions.h"
#include "RW_LBlock.h"	

void CALLBACK HandleTakeProbePoint(double* Position, bool GotoPos)
{
	if(GotoPos)
	{
		if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
		double target[4] = {Position[0], Position[1], Position[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::MACHINE_GOTOCOMMAND);
	}
	else
		RWrapper::RW_CNC::MYINSTANCE()->RaiseGotoProbePointEvent(Position);
}

void CALLBACK CameraReconnected_Handler()
{
	RWrapper::RW_CNC::MYINSTANCE()->UpdateLight_ForZoomChange();
}

RWrapper::RW_CNC::RW_CNC()
{
	try
	{
		this->GotoProbePoint = false;
		this->myCNC = this;
		this->CNCmode = false;
		this->LightingON = false;
		this->UpdatingCurrentLight = false;
		this->CurrentLightProperty = gcnew cli::array<int, 1>(RW_DBSettings::MYINSTANCE()->NoOfCards);
		this->LastLightProperty = gcnew cli::array<int, 1>(RW_DBSettings::MYINSTANCE()->NoOfCards);
		this->LastLightValues = gcnew cli::array<int, 1>(RW_DBSettings::MYINSTANCE()->NoOfCards);
		this->ProbeGotoPosition = gcnew cli::array<double>(3);
		for(int i = 0; i < 8; i++)
		{
			this->LastLightProperty[i] = 0;
			this->CurrentLightProperty[i] = 0;
			this->LastLightValues[i] = 0;
		}
		this->intermediatePts = NULL;
		this->gotoProbehit = false;
		this->CurrentOnlineSystemState = "00000000"; //Initialise all levers to be in the low state..
		this->PrevTarget = gcnew cli::array<double>(4);
		this->Reset_PrevTarget();
		MAINDllOBJECT->TakeProbePointCallback = &HandleTakeProbePoint;
		MAINDllOBJECT->CameraReConnected = &CameraReconnected_Handler;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0001", ex); }
}

RWrapper::RW_CNC::~RW_CNC()
{
}

RWrapper::RW_CNC^ RWrapper::RW_CNC::MYINSTANCE()
{
	return myCNC;
}

void RWrapper::RW_CNC::SetCNCFeedRate(cli::array<double, 1>^ frate)
{
	try
	{
		for(int i = 0; i < 4; i++)
		{
			if(frate[i] == 0.0)
				RWrapper::RW_DRO::MYINSTANCE()->feedRate[i] = 2;
			else
				RWrapper::RW_DRO::MYINSTANCE()->feedRate[i] = frate[i];
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0015", ex); }
}

void RWrapper::RW_CNC::Activate_AbortDRO()
{
	try
	{
		if(DROInstance == NULL) return;
		Sleep(30);
		DROInstance->ToAbort();
		//RW_CNC::MYINSTANCE()->CNCmode = false;
		//MAINDllOBJECT->IsCNCMode(false);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0009", ex); }
}

void RWrapper::RW_CNC::Activate_CNCModeDRO()
{
	try
	{
		if(DROInstance == NULL) return;
		if(!PPCALCOBJECT->IsPartProgramRunning() && RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag > 0) // == 1)
			DROInstance->ToResetLineNumber();
		//Sleep(30);
		//MAINDllOBJECT->IsCNCMode(true);
		DROInstance->ActivateCNC();
		if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag < 2) RaiseCNCModeChanged(1);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0010", ex); }
}

void RWrapper::RW_CNC::Activate_ManualModeDRO()
{
	try
	{
		if(DROInstance == NULL) return;
		if(!PPCALCOBJECT->IsPartProgramRunning() && RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag > 0) //  == 1)
			DROInstance->ToResetLineNumber();
		Sleep(30);
		DROInstance->ToManual();
		//RWrapper::RW_CNC::MYINSTANCE()->CNCmode = false;
		//MAINDllOBJECT->IsCNCMode(false);
		//MAINDllOBJECT->UseGotoPostionInAction(false);
		//RWrapper::RW_FocusDepth::MYINSTANCE()->Focusflag = false;
		if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag < 2) RaiseCNCModeChanged(2);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0011", ex); }
}

void RWrapper::RW_CNC::Lock_Axis(System::String^ AxisName)
{
	try
	{
		if(DROInstance == NULL) return;
		Sleep(30);
		if(AxisName == "X")
			DROInstance->AxisLock(0);
		else if(AxisName == "Y")
			DROInstance->AxisLock(1);
		else if(AxisName == "Z")
			DROInstance->AxisLock(2);
		else if(AxisName == "R")
			DROInstance->AxisLock(3);
		else if (AxisName = "All")
		{
			for (int i = 0; i < RW_DBSettings::MYINSTANCE()->NoOfCards; i++)
				DROInstance->AxisLock(i);
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0011a", ex); }
}

void RWrapper::RW_CNC::Unlock_Axis(System::String^ AxisName)
{
	try
	{
		if(DROInstance == NULL) return;
		Sleep(30);
		if(AxisName == "X")
			DROInstance->AxisUnlock(0);
		else if(AxisName == "Y")
			DROInstance->AxisUnlock(1);
		else if(AxisName == "Z")
			DROInstance->AxisUnlock(2);
		else if(AxisName == "R")
			DROInstance->AxisUnlock(3);
		else if (AxisName = "All")
		{
			for (int i = 0; i < RW_DBSettings::MYINSTANCE()->NoOfCards; i++) 
				DROInstance->AxisUnlock(i);
			//DROInstance->AxisUnlockAll();
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0011b", ex); }
}

void RWrapper::RW_CNC::RaiseCNCEvents(int Cnt)
{
	if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 3) return;
	if(Cnt == 0)
		CNCCommandTargetReached::raise();
	else if(Cnt == 1)
		GenerateCNCBtnClick::raise("CNC");
	else if (Cnt == 2)
	{
		GenerateCNCBtnClick::raise("MANUAL");
		this->Reset_PrevTarget();
	}
	else if(Cnt == 3)
		LightCommandUpdated::raise();
	//if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag < 2)
	//	RaiseCNCModeChanged(Cnt);
}

void RWrapper::RW_CNC::RaiseCNCModeChanged(int Cnt)
{
	if (Cnt == 1)
	{
		RWrapper::RW_CNC::MYINSTANCE()->CNCmode = true;
		MAINDllOBJECT->IsCNCMode(RWrapper::RW_CNC::MYINSTANCE()->CNCmode);
	}
	else if (Cnt == 2)
	{
		RWrapper::RW_CNC::MYINSTANCE()->CNCmode = false;
		MAINDllOBJECT->IsCNCMode(RWrapper::RW_CNC::MYINSTANCE()->CNCmode);
		MAINDllOBJECT->UseGotoPostionInAction(RWrapper::RW_CNC::MYINSTANCE()->CNCmode);
		RWrapper::RW_FocusDepth::MYINSTANCE()->Focusflag = RWrapper::RW_CNC::MYINSTANCE()->CNCmode;
	}
	else
	{
		LightCommandUpdated::raise();
		return;
	}
	//DROInstance->ClearCommandsList();
	CNCModeChange_Event::raise(Cnt);
}

bool RWrapper::RW_CNC::getCNCMode()
{
	return CNCmode;
}

bool RWrapper::RW_CNC::IsLightUpdating()
{
	return UpdatingCurrentLight;
}

void RWrapper::RW_CNC::Start_ZoomInOut(bool ZoomIn)
{
	try
	{
		if(DROInstance == NULL) return;
		DROInstance->StartAutoZoomInOut(ZoomIn);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0011b", ex); }
}

void RWrapper::RW_CNC::Stop_ZoomInOut(bool ZoomIn)
{
	try
	{
		if(DROInstance == NULL) return;
		DROInstance->StopAutoZoomInOut(ZoomIn);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0011b", ex); }
}

void RWrapper::RW_CNC::SwitchOnOffLight(bool Status)
{
	try
	{
		if(DROInstance == NULL) return;
		if(!RWrapper::RW_MainInterface::MYINSTANCE()->MachineConnectedStatus) return;
		if(this->UpdatingCurrentLight) 
			return;
		this->UpdatingCurrentLight = true;
		this->LightingON = Status;
		//Send light command.. Convert System String to char*..
		int Noofcards = RW_DBSettings::MYINSTANCE()->NoOfCards;
		System::String^ LightingValue = "";
		for(int i = 0; i < Noofcards; i++)
			LightingValue = LightingValue + "00";
		DROInstance->sendLightCommand((char*)Marshal::StringToHGlobalAnsi(LightingValue).ToPointer(), true);	
		MAINDllOBJECT->setLightProperty((char*)Marshal::StringToHGlobalAnsi(LightingValue).ToPointer());
		//Caclulate the light intensity....///
		cli::array<int, 1>^ lightvalue = gcnew cli::array<int, 1>(Noofcards); // {0, 0, 0, 0, 0, 0, 0, 0};
		CalcualteSurfaceorProfileLight(lightvalue);
		this->UpdatingCurrentLight = false;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0015", ex); }
}

void RWrapper::RW_CNC::UpdateLight(cli::array<int, 1>^ lvalue)
{
	try
	{
		if(DROInstance == NULL) return;
		//if(!RWrapper::RW_MainInterface::MYINSTANCE()->MachineConnectedStatus) return;
		if(this->UpdatingCurrentLight) 
		{
			int Noofcards = RW_DBSettings::MYINSTANCE()->NoOfCards;
		    PendingLightCommand = true;
			for(int i = 0; i < Noofcards; i++)
			  LastLightValues[i] = lvalue[i];
			return;
		}
		PendingLightCommand = false;
		//if(!LightingON) return;
		this->UpdatingCurrentLight = true;
		cli::array<int, 1>^ lightvalue = gcnew cli::array<int, 1>(lvalue->Length); // { lvalue[0], lvalue[1], lvalue[2], lvalue[3], lvalue[4], lvalue[5], 0, 0 };
		lvalue->CopyTo(lightvalue, 0);
		//if(!RWrapper::RW_DBSettings::MYINSTANCE()->AuxilaryLightPresent)
		//	lightvalue[5] = 0;
		//if(RWrapper::RW_DBSettings::MYINSTANCE()->CoAxialLightPresent)
		//	lightvalue[6] = lvalue[6];
		if (RWrapper::RW_DBSettings::MYINSTANCE()->ElevenXProfilePresent)
		{
			lightvalue[RW_DBSettings::MYINSTANCE()->Profile11X_Zone_No] = lvalue[RW_DBSettings::MYINSTANCE()->Profile_Zone_No];
			if (lvalue[RW_DBSettings::MYINSTANCE()->Profile_Zone_No] > RW_DBSettings::MYINSTANCE()->ProfileLightLevelFor11X)
				lightvalue[RW_DBSettings::MYINSTANCE()->Profile_Zone_No] = RW_DBSettings::MYINSTANCE()->ProfileLightLevelFor11X;
			else
				lightvalue[RW_DBSettings::MYINSTANCE()->Profile_Zone_No] = lvalue[RW_DBSettings::MYINSTANCE()->Profile_Zone_No];
		}
		else
			lightvalue[RW_DBSettings::MYINSTANCE()->Profile11X_Zone_No] = 0;

		if (lightvalue[RW_DBSettings::MYINSTANCE()->Profile_Zone_No] >= 64)
			lightvalue[RW_DBSettings::MYINSTANCE()->Profile_Zone_No] = 63;
		if(MAINDllOBJECT->CurrentLenseType == RapidEnums::RAPIDLENSETYPE::LENSE_2X)
			lightvalue[RW_DBSettings::MYINSTANCE()->Profile_Zone_No] += 64;
		else if(MAINDllOBJECT->CurrentLenseType == RapidEnums::RAPIDLENSETYPE::LENSE_5X)
			lightvalue[RW_DBSettings::MYINSTANCE()->Profile_Zone_No] += 128;
		System::String^ Clightvalue = GetLightValue(lightvalue);
		DROInstance->sendLightCommand((char*)(void*)Marshal::StringToHGlobalAnsi(Clightvalue).ToPointer(), true);	
		MAINDllOBJECT->setLightProperty((char*)(void*)Marshal::StringToHGlobalAnsi(Clightvalue).ToPointer());
		CalcualteSurfaceorProfileLight(lightvalue);
		MAINDllOBJECT->update_VideoGraphics();
		this->UpdatingCurrentLight = false;
		if (PendingLightCommand)
		{
			UpdateLight(LastLightValues);
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0015", ex); }
}

void RWrapper::RW_CNC::LightCallbackFromDRO(char* Lightvalue)
{
	try
	{
		System::String^ Light_dro = gcnew System::String(Lightvalue);
		MAINDllOBJECT->setLightProperty(Lightvalue);
		RWrapper::RW_CNC::MYINSTANCE()->ConvertLightValueToInt(Light_dro);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0015", ex); }
}

void RWrapper::RW_CNC::ConvertLightValueToInt(System::String^ Light_FromDRO)
{
	try
	{
		if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HOBCHECKER) return;
		System::String^ currentLighting = Microsoft::VisualBasic::Strings::StrReverse(Light_FromDRO);
		System::String^ currentLightingValue = "";
		int Noofcards = RW_DBSettings::MYINSTANCE()->NoOfCards;
		currentLighting = currentLighting->Substring(0, Noofcards * 2);
		cli::array<int, 1>^ lightvalue = gcnew cli::array<int, 1>(Noofcards); // = { 0, 0, 0, 0, 0, 0, 0, 0 };
		for(int i = 0; i < Noofcards; i++)
			lightvalue[i] = System::Convert::ToInt32(currentLighting->Substring(2 * i, 2), 16);
		for(int i = 0; i < Noofcards; i++)
			CurrentLightProperty[i] = lightvalue[i];
		
		if(RWrapper::RW_DBSettings::MYINSTANCE()->ElevenXProfilePresent)
			CurrentLightProperty[RW_DBSettings::MYINSTANCE()->Profile_Zone_No] = lightvalue[RW_DBSettings::MYINSTANCE()->Profile11X_Zone_No];
		if(MAINDllOBJECT->CurrentLenseType == RapidEnums::RAPIDLENSETYPE::LENSE_2X)
			CurrentLightProperty[RW_DBSettings::MYINSTANCE()->Profile_Zone_No] -= 64;
		else if(MAINDllOBJECT->CurrentLenseType == RapidEnums::RAPIDLENSETYPE::LENSE_5X)
			CurrentLightProperty[RW_DBSettings::MYINSTANCE()->Profile_Zone_No] -= 128;
		//CalcualteSurfaceorProfileLight(lightvalue);
		RaiseCNCEvents(3);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0015", ex); }
}

System::String^ RWrapper::RW_CNC::GetLightValue(cli::array<int, 1>^ lightvalue)
{
	try
	{
		System::String^ currentLighting = "";
		int i = 0;
		cli::array<System::String^>^ Lightingarray = gcnew cli::array<System::String^>(lightvalue->Length); // {"00", "00", "00", "00", "00", "00", "00", "00"};
		for each(System::Int32^ Currentvalue in lightvalue)
		{
			Lightingarray[i] = Microsoft::VisualBasic::Conversion::Hex(Currentvalue);
			if(Lightingarray[i]->Length == 1)
				Lightingarray[i] = "0" + Lightingarray[i];
			Lightingarray[i] = Microsoft::VisualBasic::Strings::StrReverse(Lightingarray[i]);
			i += 1;
		}
		int Noofcards = RW_DBSettings::MYINSTANCE()->NoOfCards;
		for(int i = 0; i < Noofcards; i++)
			currentLighting += Lightingarray[Noofcards - 1 - i];
		return currentLighting;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0015", ex); }
}

void RWrapper::RW_CNC::CalcualteSurfaceorProfileLight(cli::array<int, 1>^ lightvalue)
{
	try
	{
		int Noofcards = RW_DBSettings::MYINSTANCE()->NoOfCards;
		bool SurfacelightON = false, ProfilelightON = false;
		if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::VISIONHEADONLY)
			ProfilelightON = true;
		//for(int i = 0; i < Noofcards; i++)
		//{
		//	if (i != RW_DBSettings::MYINSTANCE()->Profile_Zone_No && i != RW_DBSettings::MYINSTANCE()->Profile11X_Zone_No)
		//	{
		//		if(System::Math::Abs(lightvalue[i]) > 0) // - LastLightProperty[i]) > 0)
		//		{
		//			SurfacelightON = true;
		//			break;
		//		}
		//	}
		//	else
		//	{
		//		if (System::Math::Abs(lightvalue[RW_DBSettings::MYINSTANCE()->Profile_Zone_No]) > 0 || System::Math::Abs(lightvalue[RW_DBSettings::MYINSTANCE()->Profile11X_Zone_No]) > 0) // - LastLightProperty[RW_DBSettings::MYINSTANCE()->Profile_Zone_No]) > 0)
		//			ProfilelightON = true;
		//	}
		//}
		int off_Ct = 0;
		for (int i = 0; i < lightvalue->Length; i++)
		{
			if (i == RW_DBSettings::MYINSTANCE()->Profile_Zone_No || i == RW_DBSettings::MYINSTANCE()->Profile11X_Zone_No) continue;
			if (lightvalue[i] == 0)
				off_Ct++;
		}
		if (off_Ct < lightvalue->Length - 2)
			SurfacelightON = true;
		if (lightvalue[RW_DBSettings::MYINSTANCE()->Profile_Zone_No] > 0 || lightvalue[RW_DBSettings::MYINSTANCE()->Profile11X_Zone_No] > 0)
			ProfilelightON = true;

		if (this->RefDotCamMode)
			MAINDllOBJECT->SetCamProperty(MAINDllOBJECT->RefDotCamSettings); // RefDotCamSettings);
		else
		{
			MAINDllOBJECT->SetProfileLightMode(ProfilelightON);
			MAINDllOBJECT->SetSurfaceLightMode(SurfacelightON);
		}
		//if(SurfacelightON)
		//{
		//	if(lightvalue[0] == 0 && lightvalue[1] == 0 && lightvalue[2] == 0 && lightvalue[3] == 0 && lightvalue[5] == 0 && lightvalue[6] == 0)
		//		MAINDllOBJECT->SetSurfaceLightMode(false);
		//	else
		//		MAINDllOBJECT->SetSurfaceLightMode(true);
		//}
		//else if(ProfilelightON)
		//{
		//	if (lightvalue[RW_DBSettings::MYINSTANCE()->Profile_Zone_No] == 0 && lightvalue[RW_DBSettings::MYINSTANCE()->Profile11X_Zone_No] == 0)
		//		MAINDllOBJECT->SetProfileLightMode(false);
		//	else
		//		MAINDllOBJECT->SetProfileLightMode(true);
		//}
		//if (lightvalue[RW_DBSettings::MYINSTANCE()->Profile_Zone_No] == 0 && lightvalue[RW_DBSettings::MYINSTANCE()->Profile11X_Zone_No] == 0)
		//	MAINDllOBJECT->SetProfileLightMode(false); 
		//else
		//	MAINDllOBJECT->SetProfileLightMode(true);
		////if(lightvalue[0] == 0 && lightvalue[1] == 0 && lightvalue[2] == 0 && lightvalue[3] == 0 && lightvalue[5] == 0 && lightvalue[6] == 0)
		//int off_Ct = 0;
		//for (int i = 0; i < lightvalue->Length; i++)
		//{
		//	if (i == RW_DBSettings::MYINSTANCE()->Profile_Zone_No || i == RW_DBSettings::MYINSTANCE()->Profile11X_Zone_No) continue;
		//	if (lightvalue[i] == 0)
		//		off_Ct++;
		//}
		//if (off_Ct == lightvalue->Length - 2)
		//	MAINDllOBJECT->SetSurfaceLightMode(false);
		//else
		//	MAINDllOBJECT->SetSurfaceLightMode(true);

		lightvalue->CopyTo(LastLightProperty, 0);
		//for(int i = 0; i < Noofcards; i++)
		//	LastLightProperty[i] = lightvalue[i];
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0015", ex); }
}

void RWrapper::RW_CNC::UpdateLightStatusAfterClear()
{
	CalcualteSurfaceorProfileLight(LastLightProperty);
}

void RWrapper::RW_CNC::UpdateLight_ForZoomChange()
{
	UpdateLight(CurrentLightProperty);
}

//void CALLBACK CameraReconnected_EventHandler()
//{
//	RWrapper::RW_CNC::MYINSTANCE()->UpdateLight(lastligh)
//}


void RWrapper::RW_CNC::SendDROToGetProbePoint(bool gotoforpoint)
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())	
		{
			MAINDllOBJECT->SetStatusCode("RW_CNC01");
			return;
		}
		if(!this->CNCmode)
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);	
		double ProbeOffsetPosition[4] = {MAINDllOBJECT->ProbeOffsetX, MAINDllOBJECT->ProbeOffsetY, 0, 0};
		double target[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		for(int i = 0; i < 3; i++)
		{
			if(RWrapper::RW_DRO::MYINSTANCE()->AxisProperty[i])
				target[i] = ProbeGotoPosition[i] - ProbeOffsetPosition[i];
			else
				target[i] = ProbeGotoPosition[i] - ProbeOffsetPosition[i] *(System::Math::PI/180);
		}
		
		//Store the current RW_DRO Values to Move back to the original position , if required..//
		RWrapper::RW_DRO::MYINSTANCE()->SetReturnPosition();
		int ProbeDirection = 1;
		double GotoDcs[3] = {0}, PointArray[6] = {target[0], target[1], target[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2]};
		RMATH3DOBJECT->DCs_TwoEndPointsofLine(PointArray, GotoDcs);
		double Tmp_Value = abs(GotoDcs[0]);
		if(abs(GotoDcs[1]) > Tmp_Value)
		{
			Tmp_Value = abs(GotoDcs[1]);
			ProbeDirection = 5;
		}
		if(abs(GotoDcs[2]) > Tmp_Value)
		{
			Tmp_Value = abs(GotoDcs[2]);
			ProbeDirection = 9;
		}
		if(gotoforpoint)
		{
			this->GotoProbePoint = true;
			RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = true;
			DROInstance->SetProbeDirection(ProbeDirection);
			this->gotoProbehit = true;
		}
		SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::MACHINE_GOTOCOMMAND);
		this->gotoProbehit = false;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0012a", ex); }
}

void RWrapper::RW_CNC::GotoPosition(cli::array<System::String^>^ position)
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())	
		{
			MAINDllOBJECT->SetStatusCode("RW_CNC01");
			return;
		}
		double target[4] = {0}, DROToPosition[4] = {0};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		for(int i = 0; i < 3; i++)
		{
			if(position[i] != "")
				DROToPosition[i] = System::Convert::ToDouble(position[i]);
			if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
				DROToPosition[i] = DROToPosition[i] * 25.4;
		}
		if(position[3] != "")
		{
			System::String^ RPosition = position[3];
			cli::array<System::String^>^ RPositionArray = RPosition->Split(System::Char(','));
			for(int i = 0; i < RPositionArray->Length; i++)
				DROToPosition[3] += (System::Convert::ToDouble(RPositionArray[i]))/(System::Math::Pow(60, i));
		}
		if(!(position[0] == "" && position[1] == "" && position[2] == "" && position[3] == ""))
		{
			for(int i = 0; i < 4; i++)
			{
				//Check if we are in UCS and not in MCS
				if (MAINDllOBJECT->getCurrentUCS().getId() == 0)
				{
					if (position[i] == "")
						target[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
					else
					{
						if (RWrapper::RW_DRO::MYINSTANCE()->AxisProperty[i])
							target[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i] + (DROToPosition[i] - RWrapper::RW_DRO::MYINSTANCE()->CurrentUCSDRO_IntUse[i]);
						else
							target[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i] + (DROToPosition[i] * (System::Math::PI / 180) - RWrapper::RW_DRO::MYINSTANCE()->CurrentUCSDRO_IntUse[i]);
					}
				}
				else
				{
					double RevTransform[9] = { 0 };
					RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 3, 1, RevTransform);
					double ans[4] = { 0 }; 
					target[0] = DROToPosition[0]; target[1] = DROToPosition[1]; target[2] = 1;
					int ss1[2] = { 3, 3 }, ss2[2] = { 3, 1 };
					RMATH2DOBJECT->MultiplyMatrix1(RevTransform, ss1, target, ss2, ans);
					target[0] = ans[0]; target[1] = ans[1];
					target[2] = DROToPosition[2] + MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ();
					target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
				}
			}
		}
		if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
		   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
		//Store the current RW_DRO Values to Move back to the original position , if required..//
		RWrapper::RW_DRO::MYINSTANCE()->SetReturnPosition();
		SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::MACHINE_GOTOCOMMAND);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0012", ex); }
}

void RWrapper::RW_CNC::GoBackToOldPosition()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())	
		{
			MAINDllOBJECT->SetStatusCode("RW_CNC01");
			return;
		}
		if(DROInstance == NULL) return;
		double target[4] = {0};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		for(int i = 0; i < 4; i++)
			target[i] = RWrapper::RW_DRO::MYINSTANCE()->returnPosition[i];
		RWrapper::RW_DRO::MYINSTANCE()->SetReturnPosition();
		SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::MACHINE_GOTOCOMMAND);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0013", ex); }
}

void RWrapper::RW_CNC::MovetoPosition(cli::array<System::String^>^ position)
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())	
		{
			MAINDllOBJECT->SetStatusCode("RW_CNC01");
			return;
		}
		if(DROInstance == NULL) return;
		double target[4] = {0}, DROToPosition[4] = {0};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		for(int i = 0; i < 3; i++)
		{
			if(position[i] != "")
				DROToPosition[i] = System::Convert::ToDouble(position[i]);
		}
		if(position[3] != "")
		{
			System::String^ RPosition = position[3];
			cli::array<System::String^>^ RPositionArray = RPosition->Split(System::Char(','));
			for(int i = 0; i < RPositionArray->Length; i++)
				DROToPosition[3] += (System::Convert::ToDouble(RPositionArray[i]))/(System::Math::Pow(60, i));
		}
		for(int i = 0; i < 3; i++)
		{
			if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
				DROToPosition[i] = DROToPosition[i] * 25.4;
		}
		for(int i = 0; i < 4; i++)
		{
			if(position[i] == "")
				target[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
			else
			{
				if(RWrapper::RW_DRO::MYINSTANCE()->AxisProperty[i])
					target[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i] + DROToPosition[i];
				else
					target[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i] + DROToPosition[i] *(System::Math::PI/180);
			}
		}
		if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
		   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
		RWrapper::RW_DRO::MYINSTANCE()->SetReturnPosition();
		SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::MACHINE_GOTOCOMMAND);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0014", ex); }
}

void RWrapper::RW_CNC::MoveProbeToVisionPosition()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())	
		{
			MAINDllOBJECT->SetStatusCode("RW_CNC01");
			return;
		}
		if(DROInstance == NULL) return;
		double target[4] = {0}, ProbeOffsetPosition[4] = {MAINDllOBJECT->ProbeOffsetX, MAINDllOBJECT->ProbeOffsetY, MAINDllOBJECT->ProbeOffsetZ, 0};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		
		for(int i = 0; i < 4; i++)
		{
			if(RWrapper::RW_DRO::MYINSTANCE()->AxisProperty[i])
				target[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i] - ProbeOffsetPosition[i];
			else
				target[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i] - ProbeOffsetPosition[i] *(System::Math::PI/180);
		}
		if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
		   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
		target[2] += 1;
		SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::MACHINE_GOTOCOMMAND);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0014", ex); }
}


void RWrapper::RW_CNC::SendDROGotoPosition(double* target, double* feedrate, TargetReachedCallback TRtype)
{
	try
	{
		//If current command is to the same position as previous command, do not bother sending it!
		//int SameCommand = 0;
		//for (int ii = 0; ii < 4; ii++)
		//{
		//	if (abs(target[ii] - this->PrevTarget[ii]) < 0.001) SameCommand++;
		//	this->PrevTarget[ii] = target[ii];
		//}

		RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TRtype);
		
		//if (SameCommand == 4)
		//{	//Same command, we will simulate target reached!!
		//	if (PPCALCOBJECT->IsPartProgramRunning())
		//	{
		//		RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
		//		return;
		//	}
		//}

		if(MAINDllOBJECT->UseLocalisedCorrection && TRtype != TargetReachedCallback::MACHINE_HOMINGMODE)
			BESTFITOBJECT->correctXYCoord2(&target[0], &target[1], true);
		if(TRtype == TargetReachedCallback::MACHINE_HOMINGMODE)
			DROInstance->GotoCommand(target, feedrate, true);
		else
		{
			if(PPCALCOBJECT->IsPartProgramRunning() || 
				(RWrapper::RW_SphereCalibration::instantiated && RWrapper::RW_SphereCalibration::MYINSTANCE()->DoingCalibration) || 
				(RWrapper::RW_DelphiCamModule::instantiated && RWrapper::RW_DelphiCamModule::MYINSTANCE()->DoingCalibration) ||
				(RWrapper::RW_LBlock::instantiated && (RWrapper::RW_LBlock::MYINSTANCE()->FirstStepRunning || RWrapper::RW_LBlock::MYINSTANCE()->CycleStarted)))
			{
			    DROInstance->GotoCommand(target, feedrate, false, RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag);
			}
			else if(this->GotoProbePoint)
			{
				if(this->gotoProbehit && MAINDllOBJECT->UseGotoPointsforProbeHitPartProg())
				{
				   MAINDllOBJECT->UseGotoPostionInAction(true);
				   MAINDllOBJECT->GotoPostionForAction.set(target[0], target[1], target[2]);
				}
				else
					 MAINDllOBJECT->UseGotoPostionInAction(false);
				DROInstance->GotoCommand(target, feedrate, false, RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag);		
			}
			else
				DROInstance->GotoCommand(target, feedrate);
		}
		this->GotoProbePoint = false;
		DROInstance->ResetLookingSymbol();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0014", ex); }
}

void RWrapper::RW_CNC::GotoCommandforCircularMovement(cli::array<double, 1>^ target, cli::array<double, 1>^ intpt, bool CircularFlag)
{
	try
	{
		/*if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
		double TargetPos[4] = {target[0], target[1], target[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		double intermediateTarget[3] = {intpt[0], intpt[1], intpt[2]};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::CIRCULARINTERPOLATION);
		DROInstance->GotoCommandWithCircularInterpolation(intermediateTarget, TargetPos, feedrate, CircularFlag);*/
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0014a", ex); }
}

void RWrapper::RW_CNC::StartCircularMovement(cli::array<double, 1>^ target, cli::array<double, 1>^ intpt, int noofintstops)
{
	try
	{
		if(intermediatestep != 0)
			return;
		if(intermediatePts != NULL)
			free(intermediatePts);
		maxnoofsteps = noofintstops;
		double intptdbl[3];
		double targetdbl[3];
		for (int i = 0; i < 3; i++)
		{
			intptdbl[i] = intpt[i];
			targetdbl[i] = target[i];
		}
		double currentpos[3] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2]};
		intermediatePts = (double*)calloc(3 * noofintstops, sizeof(double));

		if(!RMATH3DOBJECT->GetIntermediateCirclePts(currentpos, intptdbl, targetdbl, noofintstops, intermediatePts))
			return;
		if((MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::DSP) && (MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP))
	      	RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);   // new change
		RWrapper::RW_CNC::MYINSTANCE()->MoveCircularStep();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0015", ex); }
}

void RWrapper::RW_CNC::MoveCircularStep()
{
	try
	{
		double target[4] = {intermediatePts[3 * intermediatestep], intermediatePts[3 * intermediatestep + 1], intermediatePts[3 * intermediatestep + 2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		
		intermediatestep++;
		if (intermediatestep < maxnoofsteps)
		{
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::MACHINE_CIRCULARMOVEMODE);
		}
		else
		{
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::MACHINE_GOTOCOMMAND);
			intermediatestep = 0;
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0016", ex); }
}

void RWrapper::RW_CNC::Lock_AxisMovement(int Axis, bool lock_Axis)
{
	try
	{
		AxisLockevent::raise(Axis, lock_Axis);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0016", ex); }
}

void RWrapper::RW_CNC::RotateCAxis360()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())	
		{
			MAINDllOBJECT->SetStatusCode("RW_CNC01");
			return;
		}
		if(DROInstance == NULL) return;
		double target[4] = {0};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		for(int i = 0; i < 4; i++)
		{
			if(i < 3)
				target[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
			else
				target[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i] + 2 * System::Math::PI;
		}
		SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::TIS_COMMAND_TARGETREACHED);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0018", ex); }
}

void RWrapper::RW_CNC::RotateAAxis(bool ToHorizontal)
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())	
		{
			MAINDllOBJECT->SetStatusCode("RW_CNC01");
			return;
		}
		if(DROInstance == NULL) return;
		DROInstance->RotateAAxis(ToHorizontal);
		MAINDllOBJECT->AAxisHorizontal = ToHorizontal;
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0019", ex);
	}
}

void RWrapper::RW_CNC::RaiseGotoProbePointEvent(double* Position)
{
	try
	{
		ProbeGotoPosition[0] = Position[0];
		ProbeGotoPosition[1] = Position[1];
		ProbeGotoPosition[2] = Position[2];
		GotoProbePointEvent::raise();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRCNC0016", ex); }
}

void RWrapper::RW_CNC::MoveToC_Axis_Y_Position()
{	double target[4] = {0}, DROToPosition[4] = {0};
	double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], 
						  RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
	
	target[0] = MAINDllOBJECT->TIS_CAxis[0] - (RWrapper::RW_DelphiCamModule::MYINSTANCE()->HobOD + 10);	//first come out from tis_axis in x direction by an extra 10mm
	if (target[0] > RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0]) target[0] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0];
	target[1] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1];
	target[2] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
	target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
	if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);

	SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::MACHINE_GOTOCOMMAND);

	target[0] = MAINDllOBJECT->TIS_CAxis[0] - (RWrapper::RW_DelphiCamModule::MYINSTANCE()->HobOD + 10);	//first come out from tis_axis in x direction by an extra 10mm
	if (target[0] > RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0]) target[0] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0];
	target[1] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1];
	double tx[2];

	target[2] = RWrapper::RW_DelphiCamModule::MYINSTANCE()->ToothMidPt[2];//RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
	target[3] = RWrapper::RW_DelphiCamModule::MYINSTANCE()->ToothMidPt[3];//RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
	//MAINDllOBJECT->GetC_Axis_XY(target[2], &tx[0]);
	
	target[1] = MAINDllOBJECT->TIS_CAxis[1] - RWrapper::RW_DelphiCamModule::MYINSTANCE()->HobParameters[4] * MAINDllOBJECT->ProbeRadius();
	//target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];																					//make r_axis value also zero..vinod..
	if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);

	SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::MACHINE_GOTOCOMMAND);
}

void RWrapper::RW_CNC::Swivel_A_Axis()
{
	DROInstance->Swivel_A_Axis();
}

void RWrapper::RW_CNC::MoveTable(bool PassPrevious, bool RejectPrevToPrevious, double Speed)
{
	//Ensure we do not run at a speed faster than 3 mm/s
	//if (Speed > 3) Speed = 3;
	if (Speed == 0) Speed = 1;
	Speed *= 10; //multiply by 10. 
	String^ Sp = Convert::ToString((int)Speed, 16); // convert to Hex
	Sp = Sp->ToUpper();
	if (Sp->Length < 2) Sp = "0" + Sp;
	//Do Not Operate the Reject and Pass lever by default
	String^ revSp = "";
	for (int i = 0; i < Sp->Length; i++)
		revSp += Sp->Substring(Sp->Length - i - 1, 1);
	String^ Reject = "0";
	String^ Pass = "0";
	if (RejectPrevToPrevious) Reject = "1";
	if (PassPrevious) Pass = "1";
	//Add the reject and pass lever command to main command
	revSp = revSp + Reject + Pass;
	//Reverse the command..

	//Add the service number
	revSp = "4" + revSp;

	//send the command....
	DROInstance->sendMachineCommand_One((char*)Marshal::StringToHGlobalAnsi(revSp).ToPointer(), true, true);
}

void RWrapper::RW_CNC::OperateStep(int Position, int Value)
{
	//Remember, the Position is one indexed!
	//Here we can give individual commands, one at a time. But we remember where we were from the previous command. 
	System::String^ tempStr = this->CurrentOnlineSystemState;
	String^ val = "0";
	if (Value != 0) val = "1";
	//Replace the character at the position. So we take the string until the previous position, add the new value and then attach the rest. 
	int tLen = Position - 1;
	//cli::array<Char>^ character_array = tempStr->ToCharArray();
	//character_array[Position - 1] = val;
	//if (tLen == 0) tLen = 1; //This is required when we need to replace the first lever! 
	String^ revSp = "";
	if (Position == 1)
		revSp = val + tempStr->Substring(Position);
	else
		revSp = tempStr->Substring(0, tLen) + val + tempStr->Substring(Position);

	if (revSp->Length == this->CurrentOnlineSystemState->Length)
		this->CurrentOnlineSystemState = revSp;
	revSp = "";
	//Reverse the command
	for (int i = 0; i < this->CurrentOnlineSystemState->Length; i++)
		revSp += this->CurrentOnlineSystemState->Substring(this->CurrentOnlineSystemState->Length - i - 1, 1);

	//Send the command
	DROInstance->sendMachineCommand_One((char*)Marshal::StringToHGlobalAnsi("2" + revSp).ToPointer(), true, true);
}

void RWrapper::RW_CNC::StopOnlineSystem()
{
	DROInstance->sendMachineCommand_One((char*)Marshal::StringToHGlobalAnsi("81").ToPointer(), true, true);
}

void RWrapper::RW_CNC::Reset_PrevTarget()
{
	for (int i = 0; i < 4; i++)
		PrevTarget[i] = -100000;
}

#pragma once
#include "stdafx.h"
#include <Windows.h>
#include "HardwareCommunication.h"
#include <tchar.h>
#include <process.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <time.h>
#include <sstream>
#define _USE_MATH_DEFINES
#include <math.h>
//#include "Gage_Ctrl.h"
//Gage_Info_Public Gage_Info[GAGE_MAX_COUNT];

//Static Instance of this Class.........
DigitalReadOut* DigitalReadOut::_instanceDRO = 0;

//flag to check whether instance is already created or not
bool singleInstance = false;

//Constructor for DigitalReadOut Class..
DigitalReadOut::DigitalReadOut(char* Commandfpath, WCHAR* portname, int Strlength)
{
	try
	{
		DROErrorCallback = NULL; //Function to Handle the error..
		DROTargetReachedCallback = NULL; // Function Execute When Target Reached.						
		DROUpdateLightIntensity = NULL;
		CheckPartProgramRunningStatus = false;
		newCardFlag = 1;
		PositiveAcknowledgementCnt = 0;
		this->LineNumberResetFlag = false;
		this->VersionControlflag = true;
		this->StopCommunication = false;
		this->EnableInterpolation = false;
		this->HomePositionMode = false;
		for(int i = 0; i < 4; i++)
		{
			DROCoordinateValue[i] = 0;
			ProbeCoordinate[i] = 0;
			AxisPropCollection[i].CurrentProbePosition = 0;
			AxisPropCollection[i].CurrentPosition = 0;
			Axis_Accuracy[i] = 0.04;
			AxisPropCollection[i].MachineStartflag = true; //Set Machine start flag.
			AxisPropCollection[i].McsZero = 0;
		}
		Probe_Hit_Direction = 0;
		ProbeHitRaiseFlag = false;
		for(int i = 0; i < 3; i++)
			CTDroCoordinate[i] = 0;
			 
		ProbeActivated_X = false;
		ProbeActivated_Y = false;
		ProbeActivated_Z = false;
		ProbeActivated_R = false;
		PorbeActivated = false;
		CNCMode = false;
		ReadDroValues = false;
		WritingCommands = false;
		MachineConnetedStatus = false; ProbeRetractPointMethod = false;
		NumberofMachineAxes = 4;
		LookTargetReachedSym = false;
		CheckForLimihit = false; 
		CanRaiseMachineStuckMessage = false;
		SmoothMovementAtProbePos = false;
		CheckForTargetReached = false;
		Probe_HitFlag = false;
		Check_RaxisTargetReached = false;
		CheckForSecondTimeTarget = 0;
		notFocusing = true;
		ReadBuffer = "";
		LastLightCommand = "";
		Delaybetweencommnads = 0;
		ProbDirection = 1;
		AxisLockValue = 0;
		LineNumberExecuting = 0;
		LastLineExecuting = 0;
		LimitHit_DroCycleCount = 250;
		MachineDisconnect_CycleCount = 50;
		GageConnected = false;
		std::string Cpathtemp = (const char*)Commandfpath;
		CommnadSendFilePath = Cpathtemp;
		wcscpy_s(_ComName, Strlength, portname);
		ActivateNextStepFor_2sPP = false;
		SixAxisD33CardisPresent = false;
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0001", __FILE__, __FUNCSIG__); }
}

//destructor..
DigitalReadOut::~DigitalReadOut()
{
	try
	{
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0002", __FILE__, __FUNCSIG__); }
}

//Get Digital ReadOut Instance..............
DigitalReadOut* DigitalReadOut::getDROInstance()
{
	return _instanceDRO;
}

//Set and raise the error happened..
void DigitalReadOut::SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname)
{
	try
	{
		if (DROErrorCallback != NULL)
			DROErrorCallback((char*)ecode.c_str(), (char*)filename.c_str(), (char*)fctnname.c_str());
	}
	catch (...)
	{

	}
}

//Function Used to Begin All Thread to start Reading, Callback etc...
void DigitalReadOut::beginThread()
{
	try
	{
		WritingCommands = false;
		ReadDroValues = true;
		//Begin Reading DRO Thread.................
		_beginthread(&ReadingDROValue, 0, this);
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0003", __FILE__, __FUNCSIG__); }
}

//Close/release the memory..
void DigitalReadOut::ColseSoftware()
{
	try
	{
		if(this->StopCommunication)
			sendMachineCommand_One("C0", true);
		Sleep(10);
		ReadDroValues = false;
		//DROInstance->DROTargetReachedCallback(0);
		Sleep(10);
		this->MachineConnetedStatus = false;
		CloseHandle(PortHandle);
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0004", __FILE__, __FUNCSIG__); }
}

bool DigitalReadOut::GetCardVersionNumber()
{
	try
	{
		DROInstance->VersionControlflag = false;
		if(sendMachineCommand_One_NewProtocol2((char*)CalculateCRC((const char*)"C2").c_str()))//; sendMachineCommand_One("C2", true))
		{
			if(sendMachineCommand_One_NewProtocol2((char*)CalculateCRC((const char*)"C1").c_str()))   //sendMachineCommand_One("C1", true))
			{
				this->StopCommunication = true;
				DROInstance->DROTargetReachedCallback(15);
				return true;
			}
		}
		//else

	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0005", __FILE__, __FUNCSIG__); }
	return false;
}

void DigitalReadOut::DeleteInstance()
{
	try
	{
		delete _instanceDRO;
	}
	catch(...){}
}

//Create static instance..
bool DigitalReadOut::CreateInstance(char* compath, char* comparam, int Strlength)
{
	std::string COMParameter = (const char*)comparam;
	//Checking Flag whether the Instance of Class is Created or Not.........
	if(singleInstance) return false;
	singleInstance = true;
	WCHAR COMPort[10];
	for(int i = 0; i < COMParameter.length(); i++)
		COMPort[i] = COMParameter.at(i);
	COMPort[COMParameter.length()] = '\0';
	//Creating Instance of DigitalReadOut Class.
	_instanceDRO =  new DigitalReadOut(compath, &COMPort[0], Strlength);
	return true;
}

//Connect to rapid-I hardware
bool DigitalReadOut::ConnectToHardware(bool ReconnectTohardware)
{
	bool ConnectedToHardware = false;
	try
	{
		if(ReconnectTohardware)
		{
			if(MachineConnetedStatus) return true;
			CloseHandle(PortHandle);
			Sleep(100);
		}
		this->VersionControlflag = true;
		PortHandle = CreateFile(_ComName, CREATE_ALWAYS | GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if(PortHandle == INVALID_HANDLE_VALUE)
		{
			MachineConnetedStatus = false;
			if(ReconnectTohardware) DROInstance->DROTargetReachedCallback(4);
			else DROInstance->DROTargetReachedCallback(2);
		}
		else
		{
			_DCB PortConfigs;
			Sleep(1000);
			GetCommState(PortHandle, &PortConfigs);
			PortConfigs.BaudRate = 460800;	//baud rate of 460800.
			PortConfigs.ByteSize = 8;  //character of 8bit wide.
			PortConfigs.fNull = 0;
			Sleep(1000);
			SetCommState(PortHandle, &PortConfigs);	//set the configurations.
			Sleep(1000);

			COMMTIMEOUTS TimeoutConfigs;
			TimeoutConfigs.ReadIntervalTimeout = 20;
			TimeoutConfigs.ReadTotalTimeoutMultiplier = 20;
			TimeoutConfigs.ReadTotalTimeoutConstant = 20; 
			TimeoutConfigs.WriteTotalTimeoutConstant = 20;
			TimeoutConfigs.WriteTotalTimeoutMultiplier = 20;
			SetCommTimeouts(PortHandle, &TimeoutConfigs);
			PurgeComm(PortHandle, PURGE_RXCLEAR);
			MachineConnetedStatus = true;
			if(ReconnectTohardware) DROInstance->DROTargetReachedCallback(5); 
			else DROInstance->DROTargetReachedCallback(3);
			
			////Let us initialise the Gage if its present
			////Initialise the USB Gage Library
			//long result = IGS_SystemStatus_Set("IGS_SYSTEM.INIT");
			//if (result == IGS_ERR_SUCCESS)
			//{			
			//	// Get the gage list
			//	short Gage_Count;
			//	//Let us connect to the gage now
			//	result = IGS_Gage_Connect(&Gage_Count, Gage_Info);
			//	if (result == 0)
			//	{
			//		//We got a gage connected to the computer...
			//		GageConnected = true;
			//	}
			//}
			DROInstance->beginThread();
			ConnectedToHardware = true;
		}
	}
	catch(...){ DROInstance->SetAndRaiseErrorMessage("HCDLL006", __FILE__, __FUNCSIG__); }
	return ConnectedToHardware;
}

//Set number of axis..
void DigitalReadOut::setNumberofAxis(int n)
{
	try
	{
	   NumberOfAxis = n;
	   for(int i = 0; i < n; i++)
	   {
		   AxisProperties Ap;
		   AxisPropCollection[i] = Ap;
	   }
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0007", __FILE__, __FUNCSIG__); }
}

//number axis .. to send command..because some machine will take only three axis.. it will fail, if we pass 4!
void DigitalReadOut::setNumberofMachineAxis(int n)
{
	NumberofMachineAxes = n;
}

void DigitalReadOut::setDelayBetweenCommands(int d)
{
	Delaybetweencommnads = d;
}

//Set axis properties..
void DigitalReadOut::setDROProperties(bool* mt, bool* md, double* lv, double* mf, double* cpu, bool* mov)
{
	try
	{
		for(int i = 0; i < NumberOfAxis; i++)
		{
			if (this->newCardFlag == 2)
				AxisPropCollection[i].AxisdataLength = 8;
			else
				AxisPropCollection[i].AxisdataLength = 6;

			 AxisPropCollection[i].MovementType = mt[i]; //Movement type of Axis.
			 AxisPropCollection[i].MovementDirection = md[i]; //Movement Direction.
			 AxisPropCollection[i].LashCompensation = lv[i];  //Lash Value.
			 if(lv[i] >= 0.0)
				AxisPropCollection[i].LashDirection = true;
			 else
				AxisPropCollection[i].LashDirection = false;
			 AxisPropCollection[i].MultipleFactor = mf[i]; //Multiplication factor.
			 AxisPropCollection[i].CountsPerUnit = cpu[i]; //Counts per Rotation.
			 AxisPropCollection[i].MoveOppositeToVision = mov[i]; //Moves in Opposite Direction to Vision for this machine.
		}
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0008", __FILE__, __FUNCSIG__); }
}

//To Reset dro..to 0,0,0. consider the current position as MCS position.
void DigitalReadOut::ResetMachineStartFalg()
{
	try
	{
		//DROInstance->WritedebugInfo("Software Start");
		for(int i = 0; i < NumberOfAxis; i++)
			 AxisPropCollection[i].MachineStartflag = true; //Set Machine start flag.
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0009", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::SetMachineCardFlag(int flag)
{
	this->newCardFlag = flag;
	if (flag == 2)
	{
		for (int i = 0; i < NumberOfAxis; i++)
		{
			AxisPropCollection[i].AxisdataLength = 8;
			AxisPropCollection[i].McsZero = 268435456;
		}
	}
}

void DigitalReadOut::SetInterpolationFlag(bool enable)
{
	this->EnableInterpolation = enable;
}

void DigitalReadOut::SetProbeDirection(int dir)
{
	this->ProbDirection = dir;
}

//Set MCS position.. can be used to restore the DRO.
void DigitalReadOut::setMCSPosition(double* mcsvalue)
{
	try
	{
		int Cnt = NumberofMachineAxes;
		double PosSet[4] = {0}, CurrentMcsZero[4], ShiftedMcsZero[4];
		for(int i = 0; i < Cnt; i++)
		{
			CurrentMcsZero[i] = AxisPropCollection[i].McsZero;
			PosSet[i] = mcsvalue[i] * AxisPropCollection[i].CountsPerUnit / AxisPropCollection[i].MultipleFactor;
			if(!AxisPropCollection[i].MovementType)
				PosSet[i] = PosSet[i] / 2 * M_PI;
		}
		for(int i = 0; i < Cnt; i++)
		{
			if(AxisPropCollection[i].MovementDirection)
				ShiftedMcsZero[i] = CurrentMcsZero[i] - PosSet[i];
			else
				ShiftedMcsZero[i] = CurrentMcsZero[i] + PosSet[i];
		}
		for(int i = 0; i < Cnt; i++)
		{
			AxisPropCollection[i].McsZero = ShiftedMcsZero[i];
			AxisPropCollection[i].MachineStartflag = false;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0010", __FILE__, __FUNCSIG__); }
}

//Converting Hexadecimal to Decimal Value........
double DigitalReadOut::hex_dec(std::string hexval)
{
	try
	{
		double dec_val = 0;
		int pw = 0;
		//Hex to Dec conversion : Multiply each digit by (16 power digit value)
		for(int j = 0; j < (int)hexval.length(); j++)
		{
			// gives the dec equivalent for calculations( eg. A * 16^1 =10 * 16^1) 
			double Hex_de = dec_equ(hexval.at(j)); 
			if(Hex_de < 0)continue;
			pw = hexval.length() - j - 1;
			dec_val += Hex_de * pow(16.0, pw);
		}
		return dec_val;
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0011", __FILE__, __FUNCSIG__); return -1000;} 
}

int DigitalReadOut::hex_int(std::string hexValue)
{
	try
	{
		int int_val = 0;
		int pw = 0;
		//Hex to int conversion : Multiply each digit by (16 power digit value)
		for(int j = 0; j < hexValue.length(); j++)
		{
			// gives the int equivalent for calculations( eg. A * 16^1 =10 * 16^1) 
			int Hex_de = int_equ(hexValue.at(j)); 
			if(Hex_de < 0)continue;
			pw = hexValue.length() - j - 1;
			int_val += (int)(Hex_de * pow(16.0, pw));
		}
		return int_val;
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0012", __FILE__, __FUNCSIG__); return -1000;} 
}

//pass string with even number of char...
void DigitalReadOut::ConvertToByteArray(std::string hexValue, BYTE* byteArray)
{
	try
	{
		char* hexChar = (char*)hexValue.c_str();
		for(int i = 0; i < hexValue.length() / 2; i++)
		{
			int TmpVal1 = hex_int(hexValue.substr(2 * i, 1));
			int TmpVal2 = hex_int(hexValue.substr(2 * i + 1, 1));
			byteArray[i] = (TmpVal1 << 4) + TmpVal2;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0013", __FILE__, __FUNCSIG__);} 
}

//Converting Hex to Decimal Equivalent for Single Character..
double DigitalReadOut::dec_equ(char t)
{
	try
	{
		char* tem;
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
			return double(strtoul(&t, &tem, 16));
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0014", __FILE__, __FUNCSIG__); return -100;}   
}

int DigitalReadOut::int_equ(char t)
{
	try
	{
		char* tem;
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
			return int(strtoul(&t, &tem, 16));
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0015", __FILE__, __FUNCSIG__); return -100;}   
}

//Converting decimal to equivalent hex character
char DigitalReadOut::dec_hex(int decval)
{
	try
	{
		char HexChars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
		if(decval < 0)
			return HexChars[0];
		else if(decval > 15)
			return HexChars[15];
		return HexChars[decval];
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0016", __FILE__, __FUNCSIG__); return '0';}   
}

//To Activate All Axis to CNC........
void DigitalReadOut::ActivateCNC()
{
	try
	{
		this->Probe_HitFlag = false;
		this->CanRaiseMachineStuckMessage = true;
		this->CheckForLimihit = true;
		if (this->newCardFlag < 2) this->CNCMode = true;
		list<std::string> CncCommandList;	
		if (SixAxisD33CardisPresent)
			CncCommandList.push_back("23F");
		else
			CncCommandList.push_back("2F");

		CncCommandList.push_back("10");
		if (this->newCardFlag < 2)
		{
			CncCommandList.push_back("81");
		}
		sendMachineCommand_Multi(CncCommandList, true);
		
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0017", __FILE__, __FUNCSIG__); }
}

//To Abort Current Running Process
void DigitalReadOut::ToAbort()
{
	try
	{
		this->Probe_HitFlag = false;
		this->CanRaiseMachineStuckMessage = false;
		this->CheckForLimihit = false;
		//this->CNCMode = false;
		list<std::string> CncCommandList;	
		CncCommandList.push_back("10");
		if (this->newCardFlag < 2)
		{
			CncCommandList.push_back("81");
		}
		sendMachineCommand_Multi(CncCommandList, true);		
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0018", __FILE__, __FUNCSIG__); }
}

//To Activate Manual from CNC.
void DigitalReadOut::ToManual()
{
	try
	{
		this->Probe_HitFlag = false;
		this->CanRaiseMachineStuckMessage = false;
		this->CheckForLimihit = false;
		if (this->newCardFlag < 2) this->CNCMode = false;
		this->LookTargetReachedSym = false;
		if (SixAxisD33CardisPresent)
			sendMachineCommand_One("200", true);
		else
			sendMachineCommand_One("20", true);
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0019", __FILE__, __FUNCSIG__); }
}

//To Reset Both Controller and DRO.
void DigitalReadOut::ToReset()
{
	try
	{
		double DroValue[4] = {0};
		DROInstance->WriteVersionNumber(DroValue, 1);
		//bool X_Axis = ProbeActivated_X;
		//bool Y_Axis = ProbeActivated_Y;
		//bool Z_Axis = ProbeActivated_Z;
		//bool R_Axis = ProbeActivated_R;
		//if(this->newCardFlag == 1)
		//	this->Activate_Deactivate_Probe(false, false, false, false, false); 
		//sendMachineCommand_One("30", true);
		//if(this->newCardFlag == 1)
		//	this->Activate_Deactivate_Probe(X_Axis, Y_Axis, Z_Axis, R_Axis, false); 
		//If the card is v33.2, we send *3F to reset all axes...
		if (this->newCardFlag == 2)
		{
			if (SixAxisD33CardisPresent)
				sendMachineCommand_One("33F", true);
			else
				sendMachineCommand_One("3F", true);
		}
		else
		{
			sendMachineCommand_One("30", true);
		}
		Sleep(50);
		for(int i = 0; i < NumberOfAxis; i++)
			AxisPropCollection[i].MachineStartflag = true;
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0020", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::ResetDROReadings()
{
	try
	{
		sendMachineCommand_One("30", true);
	}
	catch (...)
	{
		SetAndRaiseErrorMessage("HCDLL0020a", __FILE__, __FUNCSIG__);
	}
}

void DigitalReadOut::ResetZeroVal(int axis)
{
	if (axis > -1)
	{
		if (this->newCardFlag == 2)
			DROInstance->AxisPropCollection[axis].McsZero = 268435456; //mmCalculation;
		else
			DROInstance->AxisPropCollection[axis].McsZero = 1048576; //mmCalculation;
	}
	else
	{
		for (int i = 0; i < 4; i++)
			this->ResetZeroVal(i);
	}
}


void DigitalReadOut::ToReset(double* ResetValue)
{
	try
	{
		DROInstance->WriteVersionNumber(ResetValue, 1);
		//bool X_Axis = ProbeActivated_X;
		//bool Y_Axis = ProbeActivated_Y;
		//bool Z_Axis = ProbeActivated_Z;
		//bool R_Axis = ProbeActivated_R;
		//this->Activate_Deactivate_Probe(false, false, false, false, false); 
		//sendMachineCommand_One("30", true);
		//this->Activate_Deactivate_Probe(X_Axis, Y_Axis, Z_Axis, R_Axis, false); 
		//Sleep(50);
		double PosSet[4] = {0};
		for(int i = 0; i < NumberofMachineAxes; i++)
		{
			if(AxisPropCollection[i].MovementType)
				PosSet[i] = ResetValue[i] * AxisPropCollection[i].CountsPerUnit / AxisPropCollection[i].MultipleFactor; // -AxisPropCollection[i].CurrentPosition)
			else
				PosSet[i] = ResetValue[i] * AxisPropCollection[i].CountsPerUnit * AxisPropCollection[i].MultipleFactor / (2 * M_PI); // - AxisPropCollection[i].CurrentPosition)
		}
		for(int i = 0; i < NumberofMachineAxes; i++)
		{
			if(AxisPropCollection[i].MovementDirection)
				AxisPropCollection[i].McsZero += PosSet[i];
			else
				AxisPropCollection[i].McsZero -= PosSet[i];
		}
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0021", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::ToResetLineNumber()
{
	try
	{
		if (this->newCardFlag < 2)
		{
			if (sendMachineCommand_One("60", true))
				this->LineNumberResetFlag = true;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0022", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::PauseCurrentcommand()
{
	try
	{
		this->Probe_HitFlag = false;
		this->CanRaiseMachineStuckMessage = false;
		this->CheckForLimihit = false;
		this->LookTargetReachedSym = false;
		sendMachineCommand_One("71", true);
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0023", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::ResumeCurrentCommand()
{
	try
	{	
		this->Probe_HitFlag = false;
		this->CanRaiseMachineStuckMessage = true;
		this->CheckForLimihit = true;
		if(DROInstance->newCardFlag == 0)
			this->LookTargetReachedSym = true;	
		sendMachineCommand_One("81", true);
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0024", __FILE__, __FUNCSIG__); }
}

//To Lock desired axis
void DigitalReadOut::AxisLock(int AxisIndx)
{
	try
	{
		if (this->newCardFlag == 3) return;
		int Binaryval = 1;
		switch(AxisIndx)
		{
		case 1:
			Binaryval<<=1;
			break;
		case 2:
			Binaryval<<=2;
			break;
		case 3:
			Binaryval<<=3;
			break;
		}
		AxisLockValue = AxisLockValue | Binaryval;
		char TmpArray[2] = {dec_hex(AxisLockValue), '\0'};
		std::string AxisLockValStr = (const char*)TmpArray;
		
		list<std::string> CncCommandList;
		CncCommandList.push_back("B" + AxisLockValStr);
		if(this->CNCMode)
		{
			TmpArray[0] = dec_hex(15 - AxisLockValue);
			std::string AxisCNCValStr = (const char*)TmpArray;
			CncCommandList.push_back("2" + AxisCNCValStr);
		}
		CncCommandList.push_back("10");
		if (this->newCardFlag < 2)
		{
			CncCommandList.push_back("81");
		}
		sendMachineCommand_Multi(CncCommandList, true);
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0025", __FILE__, __FUNCSIG__); }
}

//To unlock desired axis
void DigitalReadOut::AxisUnlock(int AxisIndx)
{
	try
	{
		if (this->newCardFlag == 3) return;
		int Binaryval = 1;
		switch(AxisIndx)
		{
		case 1:
			Binaryval<<=1;
			break;
		case 2:
			Binaryval<<=2;
			break;
		case 3:
			Binaryval<<=3;
			break;
		}
		AxisLockValue = AxisLockValue & (~Binaryval);
		char TmpArray[2] = {dec_hex(AxisLockValue), '\0'};
		std::string AxisLockValStr = (const char*)TmpArray;
		
		list<std::string> CncCommandList;
		CncCommandList.push_back("B" + AxisLockValStr);
		if(this->CNCMode)
		{
			TmpArray[0] = dec_hex(15 - AxisLockValue);
			std::string AxisCNCValStr = (const char*)TmpArray;
			CncCommandList.push_back("2" + AxisCNCValStr);
		}
		CncCommandList.push_back("10");
		if (this->newCardFlag < 2)
		{
			CncCommandList.push_back("81");
		}
		sendMachineCommand_Multi(CncCommandList, true);
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0026", __FILE__, __FUNCSIG__); }
}

//Start zoom in or out
void DigitalReadOut::StartAutoZoomInOut(bool ZoomIn)
{
	try
	{
		std::string ZInStr = "1";
		if(!ZoomIn) ZInStr = "2";
		std::string CmdStr = "F" + ZInStr + "9";
		//this->CNCMode = false;
		this->LookTargetReachedSym = false;
		sendMachineCommand_One((char*)CmdStr.c_str(), true);
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0027", __FILE__, __FUNCSIG__); }
}

//Stop Zoom in or out
void DigitalReadOut::StopAutoZoomInOut(bool ZoomIn)
{
	try
	{
		std::string ZInStr = "2";
		if(!ZoomIn) ZInStr = "1";
		std::string CmdStr = "F" + ZInStr + "0";
		//this->CNCMode = false;
		this->LookTargetReachedSym = false;
		sendMachineCommand_One((char*)CmdStr.c_str(), true);
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0028", __FILE__, __FUNCSIG__); }
}

//Reset Looking symbol....!!
void DigitalReadOut::ResetLookingSymbol()
{
	LookTargetReachedSym = true;
}

void DigitalReadOut::GotoCommandWithCircularInterpolation(double* CircleParam, double SweepAngle, double EnteredFRate, int NoOfHops)
{
	try
	{
		if(!MachineConnetedStatus) return;

		if(!this->EnableInterpolation)
		{
			ShowMessageBox("HWComm003", false, "");
			return;
		}
		if(!this->CNCMode)
		{
			WriteCommandSent("Goto Command called without CNC Mode");
			return;
		}
		list<std::string> CncCommandList;
		char axTarget[10]; //assumed that maximum length of char used
		std::string tempCalculate, AxisTarget;
		double Dist = 0;
		double Center[3] = {CircleParam[0], CircleParam[1], CircleParam[2]};
		double NormalVector[3] = {CircleParam[3], CircleParam[4], CircleParam[5]};
		double Radius = CircleParam[6];
		//center of circle...
		for(int i = 0; i < 3; i++)
		{
			double Centervalue = 0;
			Center[i] *= AxisPropCollection[i].CountsPerUnit / AxisPropCollection[i].MultipleFactor;
			(AxisPropCollection[i].MovementDirection) ? Centervalue = Center[i] : Centervalue = -Center[i];
			Dist = AxisPropCollection[i].McsZero + Centervalue; 
			_ltoa(long(Dist), axTarget, 16);
			tempCalculate = axTarget;
			for(int j = tempCalculate.length(); j < 6; j++)
				tempCalculate = "0"+ tempCalculate;
			std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
		}
		//calculate normal vector of circle...
		for(int i = 0; i < 3; i++)
		{
			double Nvalue = 0;
			(AxisPropCollection[i].MovementDirection) ? Nvalue = NormalVector[i] * 1000 : Nvalue = -NormalVector[i] * 1000;
			Dist = Nvalue + DROInstance->AxisPropCollection[i].McsZero;
			_ltoa(long(Dist), axTarget, 16);
			tempCalculate = axTarget;
			for(int j = tempCalculate.length(); j < 6; j++)
				tempCalculate = "0"+ tempCalculate;
			std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
		}
		//radius of circle...
		Dist = Radius * 2000; 
		_ltoa(long(Dist), axTarget, 16);
		tempCalculate = axTarget;
		for(int j = tempCalculate.length(); j < 6; j++)
			tempCalculate = "0"+ tempCalculate;
		std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
		//calculate sweep angle....
		Dist = DROInstance->AxisPropCollection[3].McsZero + SweepAngle * 1000;
		_ltoa(long(Dist), axTarget, 16);
		tempCalculate = axTarget;
		for(int j = tempCalculate.length(); j < 6; j++)
			tempCalculate = "0"+ tempCalculate;
		std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
		//calculate feedrate.....
		_ltoa(long(EnteredFRate * 10), axTarget, 16);
		tempCalculate = axTarget;
		for(int j = tempCalculate.length(); j < 2; j++)
			tempCalculate = "0"+ tempCalculate;
		if(tempCalculate == "00") tempCalculate = "01";
		std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
		//number of hops....
		_ltoa(long(NoOfHops), axTarget, 16);
		tempCalculate = axTarget;
		for(int j = tempCalculate.length(); j < 2; j++)
			tempCalculate = "0"+ tempCalculate;
		if(tempCalculate == "00") tempCalculate = "01";
		std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
		//reverse target string...
		std::reverse(AxisTarget.begin(), AxisTarget.end());
		CncCommandList.push_back("42");
		AxisTarget = "5" + AxisTarget;
		CncCommandList.push_back((char*)AxisTarget.c_str());
		CncCommandList.push_back("61");
		sendMachineCommand_Multi(CncCommandList, true);
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0029", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::GotoForBatchOfCommand(std::map<int, BatchCommandTarget> BatchTargetPos, int CommandCount)
{
	try
	{
		DROInstance->HomePositionMode = false;
		bool firstTime = true;
		double Feedrate[4] = {0}, TargetPos[4] = {0};
		std::list<std::string>* CncCommandList = new std::list<std::string>();
		for(int count = 0; count < CommandCount; count++)
		{
			char axTarget[10]; //assumed that maximum length of char used
			std::string tempCalculate, AxisTarget;
			double Dist = 0, delay = 0;
			if(BatchTargetPos[count].LinearInterPolation)
			{
				double TargetPos[4] = {BatchTargetPos[count].Target_Feedrate[0], BatchTargetPos[count].Target_Feedrate[1], BatchTargetPos[count].Target_Feedrate[2], BatchTargetPos[count].Target_Feedrate[3]};
				double Feedrate[4] = {BatchTargetPos[count].Target_Feedrate[4], BatchTargetPos[count].Target_Feedrate[5], BatchTargetPos[count].Target_Feedrate[6], BatchTargetPos[count].Target_Feedrate[7]};

				//Goto Position ... Xg Yg Zg Rg
				for(int i = 0; i < NumberofMachineAxes; i++)
				{
					double currPos = 0, GotoPos = 0;
					if(AxisPropCollection[i].MovementType)
						TargetPos[i] *= AxisPropCollection[i].CountsPerUnit / AxisPropCollection[i].MultipleFactor;
					else
					{
						TargetPos[i] += AxisPropCollection[i].MultipleFactor  * sin( TargetPos[i]);
						TargetPos[i] *= AxisPropCollection[i].CountsPerUnit / (2 * M_PI);
					}
					(AxisPropCollection[i].MovementDirection) ? GotoPos = TargetPos[i] : GotoPos = -TargetPos[i];
					currPos = AxisPropCollection[i].McsZero;
					Dist = currPos + GotoPos; 
					_ltoa(long(Dist), axTarget, 16);
					tempCalculate = axTarget;
					for(int j = tempCalculate.length(); j < AxisPropCollection[i].AxisdataLength; j++)
						tempCalculate = "0"+ tempCalculate;
					std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
				}
				if(NumberofMachineAxes < 4)
					std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);

				//Adding Feedrate for Axis...Xs Ys Zs Rs
				for(int i = 0; i < NumberofMachineAxes; i++)
				{
					_ltoa(long(Feedrate[i] * 10), axTarget, 16);
					tempCalculate = axTarget;
					for(int j = tempCalculate.length(); j < 2; j++)
						tempCalculate = "0"+ tempCalculate;
					if(tempCalculate == "00") tempCalculate = "01";
					std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
				}
				if(NumberofMachineAxes < 4)
					std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);

				//Adding Delay....D
				_ltoa(long(delay), axTarget, 16);
				tempCalculate = axTarget;
				for(int i = tempCalculate.length(); i < 2; i++)
					tempCalculate = "0"+ tempCalculate;
				std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
		
				char Pdir[2] = "\0";
				_itoa(BatchTargetPos[count].ProbeDir, Pdir, 16);
				tempCalculate = Pdir;
				std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);

				//Final string .. goto Xg Yg Zg Rg  + Xs Ys Zs Rs + D.. Total 35 Bytes of data..
				//Reverse the complete string and add 5 to start and then send
				std::reverse(AxisTarget.begin(), AxisTarget.end());
				CncCommandList->push_back("41");
				CncCommandList->push_back("5" + AxisTarget);
				if(firstTime)
				{
					firstTime = false;
					CncCommandList->push_back("61");
				}
			}
			else
			{
				double Center[3] = {BatchTargetPos[count].Target_Feedrate[0], BatchTargetPos[count].Target_Feedrate[1], BatchTargetPos[count].Target_Feedrate[2]};
				double NormalVector[3] = {BatchTargetPos[count].Target_Feedrate[3], BatchTargetPos[count].Target_Feedrate[4], BatchTargetPos[count].Target_Feedrate[5]};
				double Radius = BatchTargetPos[count].Target_Feedrate[6], SweepAngle = BatchTargetPos[count].SweepAngle,  EnteredFRate = BatchTargetPos[count].Target_Feedrate[7];
				int NoOfHops = BatchTargetPos[count].NumberOfHops;
				//center of circle...
				for(int i = 0; i < 3; i++)
				{
					double Centervalue = 0;
					Center[i] *= AxisPropCollection[i].CountsPerUnit / AxisPropCollection[i].MultipleFactor;
					(AxisPropCollection[i].MovementDirection) ? Centervalue = Center[i] : Centervalue = -Center[i];
					Dist = AxisPropCollection[i].McsZero + Centervalue; 
					_ltoa(long(Dist), axTarget, 16);
					tempCalculate = axTarget;
					for(int j = tempCalculate.length(); j < 6; j++)
						tempCalculate = "0"+ tempCalculate;
					std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
				}
				//calculate normal vector of circle...
				for(int i = 0; i < 3; i++)
				{
					double Nvalue = 0;
					(AxisPropCollection[i].MovementDirection) ? Nvalue = NormalVector[i] * 1000 : Nvalue = -NormalVector[i] * 1000;
					Dist = Nvalue + DROInstance->AxisPropCollection[i].McsZero;
					_ltoa(long(Dist), axTarget, 16);
					tempCalculate = axTarget;
					for(int j = tempCalculate.length(); j < 6; j++)
						tempCalculate = "0"+ tempCalculate;
					std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
				}
				//radius of circle...
				Dist = Radius * 2000; 
				_ltoa(long(Dist), axTarget, 16);
				tempCalculate = axTarget;
				for(int j = tempCalculate.length(); j < 6; j++)
					tempCalculate = "0"+ tempCalculate;
				std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
				//calculate sweep angle....
				Dist = DROInstance->AxisPropCollection[3].McsZero + SweepAngle * 1000;
				_ltoa(long(Dist), axTarget, 16);
				tempCalculate = axTarget;
				for(int j = tempCalculate.length(); j < 6; j++)
					tempCalculate = "0"+ tempCalculate;
				std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
				//calculate feedrate.....
				_ltoa(long(EnteredFRate * 10), axTarget, 16);
				tempCalculate = axTarget;
				for(int j = tempCalculate.length(); j < 2; j++)
					tempCalculate = "0"+ tempCalculate;
				if(tempCalculate == "00") tempCalculate = "01";
				std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
				//number of hops....
				_ltoa(long(NoOfHops), axTarget, 16);
				tempCalculate = axTarget;
				for(int j = tempCalculate.length(); j < 2; j++)
					tempCalculate = "0"+ tempCalculate;
				if(tempCalculate == "00") tempCalculate = "01";
				std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
				//reverse target string...
				std::reverse(AxisTarget.begin(), AxisTarget.end());
				CncCommandList->push_back("42");
				CncCommandList->push_back("5" + AxisTarget);
				if(firstTime)
				{
					firstTime = false;
					CncCommandList->push_back("61");
				}
			}
		}
		
		_beginthread(&WriteCommandInDiffthread, 0, CncCommandList);
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0030", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::GotoForBatchOfCommand(std::map<int, double*> TargetPosition, std::map<int, int> ProbeDir, double* EnteredFRate, int CommandCount)
{
	try
	{
		
		if(!this->CNCMode)
		{
			WriteCommandSent("Goto Command called without CNC Mode");
			return;
		}
		DROInstance->HomePositionMode = false;
		std::list<std::string>* CncCommandList = new std::list<std::string>();
		for(int count = 0; count < CommandCount; count++)
		{
			char axTarget[10]; //assumed that maximum length of char used
			std::string tempCalculate, AxisTarget;
			double Dist, delay = 0;
			int NOofaxis = NumberofMachineAxes;
			double Feedrate[4] = {EnteredFRate[0], EnteredFRate[1], EnteredFRate[2], EnteredFRate[3]};
			double TargetPos[4] = {TargetPosition[count][0], TargetPosition[count][1], TargetPosition[count][2], TargetPosition[count][3]};
			double CurrentPos[4] = {AxisPropCollection[0].CurrentPosition, AxisPropCollection[1].CurrentPosition, AxisPropCollection[2].CurrentPosition, AxisPropCollection[3].CurrentPosition};
			LastTarget[0] = TargetPos[0]; LastTarget[1] = TargetPos[1]; LastTarget[2] = TargetPos[2]; LastTarget[3] = TargetPos[3];
			//Goto Position ... Xg Yg Zg Rg
			for(int i = 0; i < NOofaxis; i++)
			{
				double currPos = 0, GotoPos = 0;
				if(AxisPropCollection[i].MovementType)
					TargetPos[i] *= AxisPropCollection[i].CountsPerUnit / AxisPropCollection[i].MultipleFactor;
				else
				{
					TargetPos[i] += AxisPropCollection[i].MultipleFactor  * sin( TargetPos[i]);
					TargetPos[i] *= AxisPropCollection[i].CountsPerUnit / (2 * M_PI);
				}
				(AxisPropCollection[i].MovementDirection) ? GotoPos = TargetPos[i] : GotoPos = -TargetPos[i];
				currPos = AxisPropCollection[i].McsZero;
				Dist = currPos + GotoPos; 
				_ltoa(long(Dist), axTarget, 16);
				tempCalculate = axTarget;
				for(int j = tempCalculate.length(); j < AxisPropCollection[i].AxisdataLength; j++)
					tempCalculate = "0"+ tempCalculate;
				std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
			}
			if(NOofaxis < 4)
				std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);

			//Adding Feedrate for Axis...Xs Ys Zs Rs
			for(int i = 0; i < NOofaxis; i++)
			{
				_ltoa(long(Feedrate[i] * 10), axTarget, 16);
				tempCalculate = axTarget;
				for(int j = tempCalculate.length(); j < 2; j++)
					tempCalculate = "0"+ tempCalculate;
				if(tempCalculate == "00") tempCalculate = "01";
				std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
			}
			if(NOofaxis < 4)
				std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);

			//Adding Delay....D
			_ltoa(long(delay), axTarget, 16);
			tempCalculate = axTarget;
			for(int i = tempCalculate.length(); i < 2; i++)
				tempCalculate = "0"+ tempCalculate;
			std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
		
			char Pdir[2] = "\0";
			_itoa(ProbeDir[count], Pdir, 16);
			tempCalculate = Pdir;
			std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);

			//Final string .. goto Xg Yg Zg Rg  + Xs Ys Zs Rs + D.. Total 35 Bytes of data..
			//Reverse the complete string and add 5 to start and then send
			std::reverse(AxisTarget.begin(), AxisTarget.end());
			CncCommandList->push_back("5" + AxisTarget);
		}	
		_beginthread(&WriteCommandInDiffthread, 0, CncCommandList);
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0031", __FILE__, __FUNCSIG__); }
}

//Generating GOTO Command for the Given Target Position
void DigitalReadOut::GotoCommand(double* Target, double* EnteredFRate, bool GotoHomePositionMode, bool probePoint)
{
	try
	{
		if(DROInstance->newCardFlag < 2 && !this->CNCMode)
		{
			WriteCommandSent("Goto Command called without CNC Mode");
			return;
		}
		std::ostringstream ss;
		ss << "Target : ";
		for (int i = 0; i < 4; i++)
		{
			ss << Target[i];
			ss << ", ";
		}
		std::string tst = ss.str();
		tst = tst.substr(0, tst.length() - 2);
		WriteCommandSent(tst);

		CheckForTargetReached = false;
		HomePositionMode = GotoHomePositionMode;
		list<std::string> CncCommandList;
		char axTarget[10]; //assumed that maximum length of char used
		std::string tempCalculate, AxisTarget;
		double Dist, delay = 0;
		int NOofaxis = NumberofMachineAxes;
		double Feedrate[4] = {EnteredFRate[0], EnteredFRate[1], EnteredFRate[2], EnteredFRate[3]};
		double TargetPos[4] = {Target[0], Target[1], Target[2], Target[3]};
		double CurrentPos[4] = {AxisPropCollection[0].CurrentPosition, AxisPropCollection[1].CurrentPosition, AxisPropCollection[2].CurrentPosition, AxisPropCollection[3].CurrentPosition};
		LastTarget[0] = TargetPos[0]; LastTarget[1] = TargetPos[1]; LastTarget[2] = TargetPos[2]; LastTarget[3] = TargetPos[3];
		
		//DROInstance->WritedebugInfo(LastTarget);

		if(DROInstance->notFocusing && newCardFlag == 0)
	    	CalculateEffFeedRate(&CurrentPos[0], &TargetPos[0],  EnteredFRate, &Feedrate[0]);
		else
			for(int i = 0; i < 4; i++){Feedrate[i] = EnteredFRate[i];}
		//Goto Position ... Xg Yg Zg Rg
		for(int i = 0; i < NOofaxis; i++)
		{
			double currPos, GotoPos;
			if(AxisPropCollection[i].MovementType)
				Target[i] *= AxisPropCollection[i].CountsPerUnit / AxisPropCollection[i].MultipleFactor;
			else
			{
				Target[i] += AxisPropCollection[i].MultipleFactor  * sin( Target[i]);
				Target[i] *= AxisPropCollection[i].CountsPerUnit / (2 * M_PI);
			}
			(AxisPropCollection[i].MovementDirection) ? GotoPos = Target[i] : GotoPos = -Target[i];
			//(GotoHomePositionMode) ? currPos = 1048576 : currPos = AxisPropCollection[i].McsZero;
			currPos = AxisPropCollection[i].McsZero;
			Dist = currPos + GotoPos; 
			_ltoa(long(Dist), axTarget, 16);
			tempCalculate = axTarget;
			for(int j = tempCalculate.length(); j < AxisPropCollection[i].AxisdataLength; j++)
				tempCalculate = "0"+ tempCalculate;
			std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
		}
		if (NOofaxis < 4)
		{
			AxisTarget += "1";
			for (int j = 1; j < AxisPropCollection[0].AxisdataLength; j++)
				AxisTarget += "0";
		}
		/*if(NOofaxis < 4)
			std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);*/

			//Adding Feedrate for Axis...Xs Ys Zs Rs
		if (DROInstance->newCardFlag == 2)
		{
			//if we have 6-axis card with encoder only in 4 axes, we need to append the two dummy targer positions. 
			if (SixAxisD33CardisPresent)
			{
				std::transform(ExtraAxesValues[0].begin(), ExtraAxesValues[0].end(), std::back_inserter(AxisTarget), toupper);
				std::transform(ExtraAxesValues[1].begin(), ExtraAxesValues[1].end(), std::back_inserter(AxisTarget), toupper);
			}
			
			for (int i = 0; i < NOofaxis; i++)
			{
				if (Feedrate[i] > 1) Feedrate[i] = (int)Feedrate[i];
				if (Feedrate[i] < 0.1) Feedrate[i] = 0.1;
				if (Feedrate[i] > 25) Feedrate[i] = 25;
			}

			for (int i = 0; i < NOofaxis; i++)
			{
				//if (Feedrate[i] > 20) Feedrate[i] = 20; // EnteredFRate[i];
				int ff = Feedrate[i] * 10;
				ff = abs(ff);
				if (ff > 254) ff = 254;
				_ltoa(ff, axTarget, 16);
				tempCalculate = axTarget;
				for (int j = tempCalculate.length(); j < 2; j++)
					tempCalculate = "0" + tempCalculate;
				if (tempCalculate == "00") tempCalculate = "01";
				std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
			}
			if (SixAxisD33CardisPresent)
			{
				std::string tFR = "00";
				std::transform(tFR.begin(), tFR.end(), std::back_inserter(AxisTarget), toupper);
				std::transform(tFR.begin(), tFR.end(), std::back_inserter(AxisTarget), toupper);
			}
		}
		else
		{
			for (int i = 0; i < NOofaxis; i++)
			{
				if (Feedrate[i] == 0) Feedrate[i] = 1;
				if (Feedrate[i] < 0.1)
				{
					long NewFeedrate = 200 + 100 * Feedrate[i]; // 0.2 / Feedrate[i];
					if (NewFeedrate > 255)
						NewFeedrate = 255;
					_ltoa(NewFeedrate, axTarget, 16);
				}
				else
				{
					_ltoa(long(Feedrate[i] * 10), axTarget, 16);
				}
				tempCalculate = axTarget;
				for (int j = tempCalculate.length(); j < 2; j++)
					tempCalculate = "0" + tempCalculate;
				if (tempCalculate == "00") tempCalculate = "01";
				std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
			}
		}

		if (NOofaxis < 4)
			std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);

		//Adding Delay....D
		if (DROInstance->newCardFlag == 2)
		{
			//First add the line number
			_ltoa(DROInstance->LastLineExecuting + 1, axTarget, 16);
			tempCalculate = axTarget;
			for (int j = tempCalculate.length(); j < 4; j++)
				tempCalculate = "0" + tempCalculate;
			//Add interpolation. Currently nothing is implemented so we send 0
			tempCalculate += "0";
			std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
			//Add Probe Direction. Probe not done on 02 Dec 2014. Hence add a 0!!
			CheckForTargetReached = true;
			if (probePoint)
			{
				int tDir = ProbDirection;
				for (int ii = 0; ii < 4; ii++)
				{	// Check which axis the Probe has to hit. Invert the probe hit direction if we have the stylus moving opposite to Vision movement
					
					if ((int)(ProbDirection / 2) == ii)
					{
						//if (AxisPropCollection[ii].MoveOppositeToVision)
						//	//ProbDirection = 2 * (ii + 1);
						//	//ProbDirection += 1;
						////else
						//	if (ProbDirection > 2 * ii)
						//		ProbDirection = 2 * ii;
						//	else
						//		ProbDirection = 2 * ii + 1;
						//break;
						if (AxisPropCollection[ii].MoveOppositeToVision && !AxisPropCollection[ii].MovementDirection)
						{
							if (ProbDirection == 2 * ii)
								tDir = 2 * ii + 1;
							else //if (tempDir == 3)
								tDir = 2 * ii;
						}
					}
				}
				_ltoa(tDir + 1, axTarget, 16);
				tempCalculate = axTarget;
				CheckForTargetReached = false;
			}
			else
				tempCalculate = "0";
			tempCalculate = tempCalculate.substr(0, 1);
			//for (int j = tempCalculate.length(); j < 2; j++)
			//	tempCalculate = "0" + tempCalculate;
			//tempCalculate += "0";
			//Concatenate and get complete string ready for sending now.
			std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
		}
		else
		{
			_ltoa(long(delay), axTarget, 16);
			tempCalculate = axTarget;
			for(int i = tempCalculate.length(); i < 2; i++)
				tempCalculate = "0"+ tempCalculate;
			std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
			if(DROInstance->newCardFlag == 1)
			{
				if(probePoint)
				{
					CheckForTargetReached = false;
					if (ProbDirection < 12)
					{
						char probdir[2] = "\0";
						_itoa(ProbDirection, probdir, 16);
						std::string TmpStr = probdir;
						AxisTarget += toupper(TmpStr.substr(0, 1).front());
					}
					else
						AxisTarget += "0";
				}
				else
				{
					CheckForTargetReached = true;
					AxisTarget += "0";
				}
				CncCommandList.push_back("41");
			}
			else // if (DROInstance->newCardFlag == 0)
				CncCommandList.push_back("41010");
		
		}

		//Final string .. goto Xg Yg Zg Rg  + Xs Ys Zs Rs + D.. Total 34 or 35 Bytes of data..
		//Reverse the complete string and add 5 to start and then send
		std::reverse(AxisTarget.begin(), AxisTarget.end());
		if (DROInstance->newCardFlag < 2)
		{
			AxisTarget = "5" + AxisTarget;
			CncCommandList.push_back((char*)AxisTarget.c_str());
			CncCommandList.push_back("61");
			sendMachineCommand_Multi(CncCommandList, true);
		}
		else
		{
			AxisTarget = "4" + AxisTarget;
			CncCommandList.push_back((char*)AxisTarget.c_str());
			CncCommandList.push_back("61");
			sendMachineCommand_Multi(CncCommandList, true);
		}
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0032", __FILE__, __FUNCSIG__); }
}

//calculate the effective feedrate to move in optimised way..
void DigitalReadOut::CalculateEffFeedRate(double* CurrentPos, double* TargetPos, double* CurrentFeedrate, double* EffFeedrate)
{
	try
	{
		double valueDiff[4], FeedRatio[4], maxgotoPos, feedrate = CurrentFeedrate[0];

		valueDiff[0] = abs(CurrentPos[0] - TargetPos[0]); valueDiff[1] = abs(CurrentPos[1] - TargetPos[1]);
		valueDiff[2] = abs(CurrentPos[2] - TargetPos[2]); valueDiff[3] = abs(CurrentPos[3] - TargetPos[3]);

		//Calculate maximum goto position and corresponding feedrate given by user..
		maxgotoPos = valueDiff[0];
		for(int i = 1; i < 4; i++)
		{
			if(maxgotoPos < valueDiff[i])
			{
				maxgotoPos = valueDiff[i];
				feedrate = CurrentFeedrate[i];
			}
		}
		if (maxgotoPos == 0)
			return;
		//feedrate ratio.. multiplier and feedarate calculation
		for(int i = 0; i < 4; i++)
		{
			FeedRatio[i] = valueDiff[i]/maxgotoPos;
			EffFeedrate[i] = feedrate * FeedRatio[i];
		}

		for(int i = 0; i < 4; i++)
		{
			if(EffFeedrate[i] < 0.5)
				EffFeedrate[i] = 0.5;
		}
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0033", __FILE__, __FUNCSIG__); }
}

//send Light Command.
bool DigitalReadOut::sendLightCommand_NewProtocol(char* Lcommand, char* LCommandWithCRC)
{
	try
	{
		if(!MachineConnetedStatus) return false;
		std::string WriteBuffer = (const char*)LCommandWithCRC, LightcommandStr = (const char*)Lcommand;
		if(LastLightCommand == WriteBuffer) return false;	
		LastLightCommand == LightcommandStr;
		WriteBuffer = "*" + WriteBuffer + "^";
		int SendFailedCount = 0;
SendAgain:
		DROInstance->PositiveAcknowledgementCnt = 0;
		WriteCommandSent("CNC Command :" + WriteBuffer);
		if (DROInstance->newCardFlag < 2)
		{
			for each(char name in WriteBuffer)
				WriteEachByte(name);
		}
		else
		{
			for each(char name in WriteBuffer)
				WriteEachByte2(name);
		}
		Sleep(20);
		for(int i = 0; i < 3; i++)
		{
			if(DROInstance->PositiveAcknowledgementCnt == 0)
				Sleep(20);
			else
				break;
		}
		if(DROInstance->PositiveAcknowledgementCnt != 2)
		{
			WriteEachByte('!');
			SendFailedCount++;
			if(SendFailedCount >= 5)
			{
				WriteCommandSent("Failed (Count is greater than 5): Light Command");
				ShowMessageBox("HWComm002", false, "");
				return false;
			}
			if(DROInstance->PositiveAcknowledgementCnt == 1)
				WriteCommandSent("ACK is Negative, Sending Again!");
			else
				WriteCommandSent("Cannot Read Acknowledgement, Sending Again!");
			goto SendAgain;
		}
		else
		{
			WriteEachByte('@');
			DROInstance->PositiveAcknowledgementCnt = 0;
			WriteCommandSent("Recieve     : PositiveAcknowledgement");
		}
		DROUpdateLightIntensity((char*)LightcommandStr.c_str());
		Sleep(10);
		return true;
	}
	catch(...){SetAndRaiseErrorMessage("HCDLL0034", __FILE__, __FUNCSIG__); return false;}
}

bool DigitalReadOut::sendLightCommand_OldProtocol(char* Lcommand, bool CheckCommunication)
{
	try
	{
		if(!MachineConnetedStatus) return false;
		std::string WriteBuffer = (const char*)Lcommand;
		if(LastLightCommand == WriteBuffer) return false;
		std::string DummyString = "0000";
		LastLightCommand = WriteBuffer;
		WritingCommands = true;
		WriteCommandSent("");
		DWORD d1, d2, d3;
		d1 = GetTickCount();
		if(CheckCommunication)
		{
			WriteBuffer = "9" + WriteBuffer;
			int SendFailedCount = 0, ClearCount = 0;
			unsigned long bufflen, strlength;
			BOOL CheckClr = 0;
			char tempbuffer[1000];
			char ntfySymbol = '*';
			std::string OriginalWStr = "";
			for each(char name in WriteBuffer)
			{
				OriginalWStr = OriginalWStr + "#";
				OriginalWStr = OriginalWStr + name;
			}
			strlength = OriginalWStr.size();

SendAgain:
			WriteCommandSent("Lighting Command :" + WriteBuffer);
			WriteEachByte(ntfySymbol);
			Sleep(20);

ClearAgain:
			CheckClr = PurgeComm(PortHandle, PURGE_RXCLEAR);
			if(!CheckClr)
			{
				ClearCount++;
				WriteCommandSent("Failed in Input Buffer clear command");
				if(ClearCount >= 5)
				{
					WriteCommandSent("Failed (Count is greater than 5): Clear Command");
					ShowMessageBox("HWComm001", false, "");
					//MessageBox(NULL, L"Failed in Communication.. Please Switch off and Switch On the machine and Try again", L"Rapid-I", MB_TOPMOST);
					return false;
				}
				goto ClearAgain;
			}

			for each(char name in WriteBuffer)
				WriteEachByte(name);
			for each(char name in DummyString)
				WriteEachByte(name);
			Sleep(20);
			ReadFile(PortHandle, tempbuffer, strlength, &bufflen, NULL);
			tempbuffer[strlength] = '\0';
			std::string CommsnRecStr = (const char*)&tempbuffer[0];
			WriteCommandSent("Sent     : " + OriginalWStr);
			WriteCommandSent("Recieved : " + CommsnRecStr);
			if(CommsnRecStr != OriginalWStr)
			{
				SendFailedCount++;
				if(SendFailedCount >= 5)
				{
					WriteCommandSent("Failed (Count is greater than 5): Light Command");
					ShowMessageBox("HWComm002", false, "");
					//MessageBox(NULL, L"Failed in Communication.. Please Switch off and Switch On the machine and Try again", L"Rapid-I", MB_TOPMOST);
					return false;
				}
				PurgeComm(PortHandle, PURGE_RXCLEAR);
				WriteCommandSent("ACK is Not Equal, Sending Again!");
				goto SendAgain;
			}
			std::string reverString = WriteBuffer.substr(1);
			DROUpdateLightIntensity((char*)reverString.c_str());
			Sleep(20);
			PurgeComm(PortHandle, PURGE_RXCLEAR);
		}
		else
		{
			WriteBuffer = "9" + WriteBuffer;
			char ntfySymbol = '*';
			WriteCommandSent(WriteBuffer);
			WriteEachByte(ntfySymbol);
			for each(char name in WriteBuffer)
				WriteEachByte(name);
			for each(char name in DummyString)
				WriteEachByte(name);
			std::string reverString = WriteBuffer.substr(1);
			DROUpdateLightIntensity((char*)reverString.c_str());
			Sleep(20);
			PurgeComm(PortHandle, PURGE_RXCLEAR);
		}
		d2 = GetTickCount();
		d3 = GetTickCount();
		d3 = d2 - d1;
		d3 = GetTickCount();
		Sleep(20);
		WritingCommands = false;
		return true;
	}
	catch(...){WritingCommands = false; SetAndRaiseErrorMessage("HCDLL0035", __FILE__, __FUNCSIG__); return false;}
}

bool DigitalReadOut::sendLightCommand(char* Lcommand, bool CheckCommunication)
{
	try
	{
		if(!MachineConnetedStatus) return false;
		if(DROInstance->newCardFlag > 0)
		{
			std::string LStr = (const char*)Lcommand;
			std::string FinalLightcommand = "9" + LStr;
			return sendLightCommand_NewProtocol(Lcommand, (char*)CalculateCRC(FinalLightcommand).c_str());
		}
		else //if (DROInstance->newCardFlag == 0)
			return sendLightCommand_OldProtocol(Lcommand, CheckCommunication);
		//else
		//	return false;
	}
	catch(...){WritingCommands = false; SetAndRaiseErrorMessage("HCDLL0036", __FILE__, __FUNCSIG__); return false;}
}

void DigitalReadOut::RotateAAxis(bool ToHorizontal)
{
	try
	{
		std::string AAxisCommandStr = "C";
		if(ToHorizontal)
			AAxisCommandStr += "0";
		else
			AAxisCommandStr += "1";
		AAxisCommandStr += "0A";
		sendMachineCommand_One((char*)AAxisCommandStr.c_str(), true);
	}
	catch(...){WritingCommands = false; SetAndRaiseErrorMessage("HCDLL0018b", __FILE__, __FUNCSIG__);}
}

//Send Machine command.
bool DigitalReadOut::sendMachineCommand_One(char* CncCommand, bool CheckCommunication, bool AttachStar)
{
	try
	{
		if(DROInstance->newCardFlag > 0)
		{
			//std::string crcValue = CalculateCRC((const char*)CncCommand);
			//crcValue = CncCommand + ', ' + crcValue;
			//std::wstring crcv = std::wstring(crcValue.begin(), crcValue.end());
			//LPCWSTR dispText = crcv.c_str();
			////if (CncCommand == "20")
			//	MessageBox(NULL, dispText, L"Rapid-I", MB_TOPMOST);
			return sendMachineCommand_One_NewProtocol((char*)CalculateCRC((const char*)CncCommand).c_str());

		}
		else
			return sendMachineCommand_One_OldProtocol(CncCommand, CheckCommunication, AttachStar);
	}
	catch(...){ WritingCommands = false; SetAndRaiseErrorMessage("HCDLL0037", __FILE__, __FUNCSIG__); return false; }
}

bool DigitalReadOut::sendMachineCommand_One_OldProtocol(char* CncCommand, bool CheckCommunication, bool AttachStar)
{
	bool returnFlag = false;
	try
	{
		if(!MachineConnetedStatus) return false;
		WritingCommands = true;
		WriteCommandSent(""); 
		std::string WriteBuffer = (const char*)CncCommand;
		std::string DummyString = "0000";
		if(CheckCommunication)
		{
			int SendFailedCount = 0, ClearCount = 0;
			unsigned long bufflen, strlength;
			BOOL CheckClr = 0;
			char tempbuffer[1000];
			char ntfySymbol = '*';
			std::string OriginalWStr = "";
			for each(char name in WriteBuffer)
			{
				OriginalWStr = OriginalWStr + "#";
				OriginalWStr = OriginalWStr + name;
			}
			strlength = OriginalWStr.size();

SendAgain:
			if(AttachStar)
			{
				WriteCommandSent("CNC Command :" + WriteBuffer);
				WriteEachByte(ntfySymbol);
				Sleep(20);
			}

ClearAgain:
			CheckClr = PurgeComm(PortHandle, PURGE_RXCLEAR);
			if(!CheckClr)
			{
				ClearCount++;
				WriteCommandSent("Failed in Input Buffer clear command");
				if(ClearCount >= 5)
				{
					WriteCommandSent("Failed (Count is greater than 5): Clear Command");
					ShowMessageBox("HWComm001", false, "");
					//MessageBox(NULL, L"Failed in Communication.. Pelase Switch off and Switch On the machine and Try again", L"Rapid-I", MB_TOPMOST);
					return false;
				}
				goto ClearAgain;
			}

			for each(char name in WriteBuffer)
				WriteEachByte(name);
			for each(char name in DummyString)
				WriteEachByte(name);
			Sleep(20);
			ReadFile(PortHandle, tempbuffer, strlength, &bufflen, NULL);
			tempbuffer[strlength] = '\0';
			std::string CommsnRecStr = (const char*)&tempbuffer[0];
			WriteCommandSent("Sent     : " + OriginalWStr);
			WriteCommandSent("Recieved : " + CommsnRecStr);
			if(CommsnRecStr != OriginalWStr)
			{
				SendFailedCount++;
				if(SendFailedCount >= 5)
				{
					WriteCommandSent("Failed (Count is greater than 5): CNC Command");
					ShowMessageBox("HWComm002", false, "");
					//MessageBox(NULL, L"Failed in Communication.. Pelase Switch off and Switch On the machine and Try again", L"Rapid-I", MB_TOPMOST);
					return false;
				}
				PurgeComm(PortHandle, PURGE_RXCLEAR);
				WriteCommandSent("ACK is Not Equal, Sending Again!");
				goto SendAgain;
			}
			else
				returnFlag = true;
			Sleep(20);
			PurgeComm(PortHandle, PURGE_RXCLEAR);
		}
		else
		{
			WriteCommandSent(WriteBuffer);
			char ntfySymbol = '*';
			if(AttachStar)
				WriteEachByte(ntfySymbol);
			for each(char name in WriteBuffer)
				WriteEachByte(name);
			for each(char name in DummyString)
				WriteEachByte(name);
			Sleep(20);
			PurgeComm(PortHandle, PURGE_RXCLEAR);
		}
		Sleep(20);
		WritingCommands = false;
		return returnFlag;
	}
	catch(...){ WritingCommands = false; SetAndRaiseErrorMessage("HCDLL0038", __FILE__, __FUNCSIG__); return returnFlag; }
}

bool DigitalReadOut::sendMachineCommand_One_NewProtocol(char* CncCommand)
{
	bool returnFlag = false;
	try
	{
		if(!MachineConnetedStatus) return false;
		std::string WriteBuffer = (const char*)CncCommand;
		int SendFailedCount = 0;
		WriteBuffer = '*' + WriteBuffer + '^';
SendAgain:
		DROInstance->PositiveAcknowledgementCnt = 0;
		WriteCommandSent("CNC Command :" + WriteBuffer);
		if (DROInstance->newCardFlag < 2)
		{
			for each(char name in WriteBuffer)
				WriteEachByte(name);
		}
		else
		{
			for each(char name in WriteBuffer)
				WriteEachByte2(name);
		}
		Sleep(20);
		//int DelayBetweenC = Delaybetweencommnads * 50000;
		//for (int kkj = 0; kkj < DelayBetweenC; kkj++)
		//{
		//}

		for(int i = 0; i < 3; i++)
		{
			if (DROInstance->PositiveAcknowledgementCnt == 0)
			{
				Sleep(20);
				//int DelayBetweenC = Delaybetweencommnads * 50000;
				//for (int kkj = 0; kkj < DelayBetweenC; kkj++)
				//{
				//}
			}
			else
				break;
		}
		if(DROInstance->PositiveAcknowledgementCnt != 2)
		{
			WriteEachByte('!');
			SendFailedCount++;
			if(SendFailedCount >= 5)
			{
				WriteCommandSent("Failed (Count is greater than 5): CNC Command");
				ShowMessageBox("HWComm002", false, "");
				//MessageBox(NULL, L"Failed in Communication.. Pelase Switch off and Switch On the machine and Try again", L"Rapid-I", MB_TOPMOST);
				return false;
			}
			if(DROInstance->PositiveAcknowledgementCnt == 1)
				WriteCommandSent("ACK is Negative, Sending Again!");
			else
				WriteCommandSent("Cannot Read Acknowledgement, Sending Again!");
			goto SendAgain;
		}
		else
		{
			WriteEachByte('@');
			DROInstance->PositiveAcknowledgementCnt = 0;
			WriteCommandSent("Recieve     : PositiveAcknowledgement");
			returnFlag = true;
		}
		return returnFlag;
	}
	catch(...){SetAndRaiseErrorMessage("HCDLL0039", __FILE__, __FUNCSIG__); return returnFlag; }
}

bool DigitalReadOut::sendMachineCommand_One_NewProtocol2(char* CncCommand)
{
	bool returnFlag = false;
	try
	{
		//if (!MachineConnetedStatus) return false;
		std::string WriteBuffer = (const char*)CncCommand;
		int SendFailedCount = 0;
		WriteBuffer = '*' + WriteBuffer + '^';
	SendAgain:
		DROInstance->PositiveAcknowledgementCnt = 0;
		WriteCommandSent("CNC Command :" + WriteBuffer);
		if (DROInstance->newCardFlag < 2)
		{
			for each(char name in WriteBuffer)
				WriteEachByte(name);
		}
		else
		{
			for each(char name in WriteBuffer)
				WriteEachByte2(name);
		}
		Sleep(20);
		//int DelayBetweenC = Delaybetweencommnads * 50000;
		//for (int kkj = 0; kkj < DelayBetweenC; kkj++)
		//{
		//}

		for (int i = 0; i < 3; i++)
		{
			if (DROInstance->PositiveAcknowledgementCnt == 0)
			{
				Sleep(20);
				//int DelayBetweenC = Delaybetweencommnads * 50000;
				//for (int kkj = 0; kkj < DelayBetweenC; kkj++)
				//{
				//}
			}
			else
				break;
		}
		if (DROInstance->PositiveAcknowledgementCnt != 2)
		{
			WriteEachByte('!');
			SendFailedCount++;
			if (SendFailedCount >= 5)
			{
				WriteCommandSent("Failed (Count is greater than 5): CNC Command");
				ShowMessageBox("HWComm002", false, "");
				//MessageBox(NULL, L"Failed in Communication.. Pelase Switch off and Switch On the machine and Try again", L"Rapid-I", MB_TOPMOST);
				return false;
			}
			if (DROInstance->PositiveAcknowledgementCnt == 1)
				WriteCommandSent("ACK is Negative, Sending Again!");
			else
				WriteCommandSent("Cannot Read Acknowledgement, Sending Again!");
			goto SendAgain;
		}
		else
		{
			WriteEachByte('@');
			DROInstance->PositiveAcknowledgementCnt = 0;
			WriteCommandSent("Recieve     : PositiveAcknowledgement");
			returnFlag = true;
		}
		return returnFlag;
	}
	catch (...) { SetAndRaiseErrorMessage("HCDLL0039", __FILE__, __FUNCSIG__); return returnFlag; }
}


void DigitalReadOut::sendMachineCommand_Multi_NewProtocol(list<std::string> &CncCommandList)
{
	try
	{
		if(!MachineConnetedStatus) return;
		int SendFailedCount = 0, CommandSendCount = 0;
SendAgain:
			int Count = 0;
			for each(std::string WriteBuffer in CncCommandList)
			{
				if(CommandSendCount > Count)
				{
					Count++;
					continue;
				}
				WriteBuffer = '*' + WriteBuffer + '^';
				DROInstance->PositiveAcknowledgementCnt = 0;
				WriteCommandSent("CNC Command :" + WriteBuffer);
				if (DROInstance->newCardFlag < 2)
				{
					for each(char name in WriteBuffer)
						WriteEachByte(name);
				}
				else
				{
					for each(char name in WriteBuffer)
						WriteEachByte2(name);
				}
				Sleep(20);
				//int DelayBetweenC = Delaybetweencommnads * 50000;
				//for (int kkj = 0; kkj < DelayBetweenC; kkj++)
				//{
				//}

				for(int i = 0; i < 3; i++)
				{
					if (DROInstance->PositiveAcknowledgementCnt == 0)
					{
						//int DelayBetweenC = Delaybetweencommnads * 50000;
						//for (int kkj = 0; kkj < DelayBetweenC; kkj++)
						//{
						//}
						Sleep(20);
					}
					else
						break;
				}
				if(DROInstance->PositiveAcknowledgementCnt != 2)
				{
					WriteEachByte('!');
					SendFailedCount++;
					Sleep(100);
					if(SendFailedCount >= 5)
					{
						WriteCommandSent("Failed (Count is greater than 5): CNC Command");
						ShowMessageBox("HWComm002", false, "");
						return;
					}
					if(DROInstance->PositiveAcknowledgementCnt == 1)
						WriteCommandSent("ACK is Negative, Sending Again!");
					else
						WriteCommandSent("Cannot Read Acknowledgement, Sending Again!");
					goto SendAgain;
				}
				else
				{
					WriteEachByte('@');
					Count++;
					CommandSendCount++;
					SendFailedCount = 0;
					DROInstance->PositiveAcknowledgementCnt = 0;
					WriteCommandSent("Sent     : PositiveAcknowledgement");
				}
			}
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0040", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::sendMachineCommand_Multi_OldProtocol(list<std::string> &CncCommandList, bool CheckCommunication)
{
	try
	{
		if(!MachineConnetedStatus) return;
		WritingCommands = true;
		WriteCommandSent("");
		std::string DummyString = "0000";
		DWORD d1, d2, d3;
		d1 = GetTickCount();
		if(CheckCommunication)
		{
			unsigned long bufflen, strlength, templength = 1;
			char tempbuffer[3500];
			char ntfySymbol = '*';
	
	SendAgain:
			for each(std::string WriteBuffer in CncCommandList)
			{
				int SendFailedCount = 0, ClearCount = 0;
				BOOL CheckClr = 0;
				std::string OriginalWStr = "";
				for each(char name in WriteBuffer)
				{
					OriginalWStr = OriginalWStr + "#";
					OriginalWStr = OriginalWStr + name;
				}
				strlength = OriginalWStr.size();

				WriteCommandSent("CNC Command :" + WriteBuffer);
				WriteEachByte(ntfySymbol);
				Sleep(20);
	ClearAgain:
				CheckClr = PurgeComm(PortHandle, PURGE_RXCLEAR);
				if(!CheckClr)
				{
					ClearCount++;
					WriteCommandSent("Failed in Input Buffer clear command");
					if(ClearCount >= 5)
					{
						WriteCommandSent("Failed (Count is greater than 5): Clear Command");
						ShowMessageBox("HWComm001", false, "");
						//MessageBox(NULL, L"Failed in Communication.. Pelase Switch off and Switch On the machine and Try again", L"Rapid-I", MB_TOPMOST);
						return;
					}
					goto ClearAgain;
				}


				for each(char name in WriteBuffer)
					WriteEachByte(name);
				for each(char name in DummyString)
					WriteEachByte(name);
				Sleep(20);
				ReadFile(PortHandle, tempbuffer, strlength, &bufflen, NULL);
				tempbuffer[strlength] = '\0';
				std::string CommsnRecStr = (const char*)&tempbuffer[0];
				WriteCommandSent("Sent     : " + OriginalWStr);
				WriteCommandSent("Recieved : " + CommsnRecStr);
				if(CommsnRecStr != OriginalWStr)
				{
					SendFailedCount++;
					if(SendFailedCount >= 5)
					{
						WriteCommandSent("Failed (Count is greater than 5): CNC Command");
						ShowMessageBox("HWComm002", false, "");
						//MessageBox(NULL, L"Failed in Communication.. Pelase Switch off and Switch On the machine and Try again", L"Rapid-I", MB_TOPMOST);
						return;
					}
					PurgeComm(PortHandle, PURGE_RXCLEAR);
					WriteCommandSent("ACK is Not Equal, Sending Again!");
					goto SendAgain;
				}

			}
			Sleep(20);
			PurgeComm(PortHandle, PURGE_RXCLEAR);
		}
		else
		{
			for each(std::string WriteBuffer in CncCommandList)
			{
				WriteCommandSent(WriteBuffer);
				char ntfySymbol = '*';
				WriteEachByte(ntfySymbol);
				for each(char name in WriteBuffer)
					WriteEachByte(name);
				for each(char name in DummyString)
					WriteEachByte(name);
			}
			Sleep(20);
			PurgeComm(PortHandle, PURGE_RXCLEAR);
		}
		d2 = GetTickCount();
		d3 = GetTickCount();
		d3 = d2 - d1;
		d3 = GetTickCount();
		Sleep(20);
		WritingCommands = false;
	}
	catch(...){ WritingCommands = false; SetAndRaiseErrorMessage("HCDLL0041", __FILE__, __FUNCSIG__); }
}

//Send machine command..
void DigitalReadOut::sendMachineCommand_Multi(list<std::string> &CncCommandList, bool CheckCommunication)			// ???      ???
{
	try
	{
		if(DROInstance->newCardFlag > 0)
		{
			std::list<std::string> CncCommandListWithCRC;
			for each(std::string Str in CncCommandList)
			{
				CncCommandListWithCRC.push_back(CalculateCRC(Str));
			}
			sendMachineCommand_Multi_NewProtocol(CncCommandListWithCRC);
		}
		else
			sendMachineCommand_Multi_OldProtocol(CncCommandList, CheckCommunication);
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0042", __FILE__, __FUNCSIG__); }
}

//write single byte.. If required give delay after write.. depends on Computer!
void DigitalReadOut::WriteEachByte(char name)
{
	try
	{
		unsigned long WriteResult;
		LPCVOID test = &name;
		BOOL CheckWrite = WriteFile(PortHandle, test, 1, &WriteResult, NULL);
		//if (DROInstance->newCardFlag < 2)
		//{
			if(Delaybetweencommnads != 0)
				Sleep(Delaybetweencommnads);
		//}
		//else
		//{
			//int DelayBetweenC = Delaybetweencommnads * 50000;
			//for (int kkj = 0; kkj < DelayBetweenC; kkj++)
			//{
			//}
		//}
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0043", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::WriteEachByte2(char name)
{
	try
	{
		unsigned long WriteResult;
		LPCVOID test = &name;
		BOOL CheckWrite = WriteFile(PortHandle, test, 1, &WriteResult, NULL);
		//if (DROInstance->newCardFlag < 2)
		//{
			//if(Delaybetweencommnads != 0)
			//	Sleep(Delaybetweencommnads);
		//}
		//else
		//{
		//int DelayBetweenC = Delaybetweencommnads * 10000; //250000000;//
		//for (int kkj = 0; kkj < DelayBetweenC; kkj++)
		//{
		//}
		Sleep(Delaybetweencommnads);
		//}
	}
	catch (...) { SetAndRaiseErrorMessage("HCDLL0043", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::ClearInBuffer()
{
	PurgeComm(PortHandle, PURGE_RXCLEAR);
}

void DigitalReadOut::ConnectToOnlineSystem(bool StopInput)
{
	if (StopInput)
		sendMachineCommand_One("51", true);
	else
		sendMachineCommand_One("50", true);
}

//Set Probe Revert back dist..
void DigitalReadOut::SetPorbeRevertbackDistance(int AxisNo, double RevertDist, double RevertSpeed, double ApproachDist, double ApproachSpeed, double ProbeSensitivity, bool ProbeMoveUpFlag)
{
	try
	{
		if(!MachineConnetedStatus) return;
		std::string tempCalculate, tempCalculate1, AxisTarget;
		char axTarget[5] = "\0";
		char axTarget1[3] = "\0";
		char tempstr[18] = "\0";
		if (DROInstance->newCardFlag == 1)
		{
			RevertDist *= AxisPropCollection[AxisNo].CountsPerUnit / AxisPropCollection[AxisNo].MultipleFactor;
			_ltoa(long(RevertDist), axTarget, 16);
			tempCalculate = axTarget;
			if (tempCalculate.length() > 4)
				tempCalculate = tempCalculate.substr(0, 4);
			for (int i = tempCalculate.length(); i < 4; i++)
				tempCalculate = "0" + tempCalculate;
			RevertSpeed *= 10;
			_ltoa(long(RevertSpeed), axTarget1, 16);
			tempCalculate1 = axTarget1;
			if (tempCalculate1.length() > 2)
				tempCalculate1 = tempCalculate1.substr(0, 2);
			for (int i = tempCalculate1.length(); i < 2; i++)
				tempCalculate1 = "0" + tempCalculate1;
			tempCalculate += tempCalculate1;
			ApproachDist *= AxisPropCollection[AxisNo].CountsPerUnit / AxisPropCollection[AxisNo].MultipleFactor;
			_ltoa(long(ApproachDist), axTarget, 16);
			tempCalculate1 = axTarget;
			if (tempCalculate1.length() > 4)
				tempCalculate1 = tempCalculate1.substr(0, 4);
			for (int i = tempCalculate1.length(); i < 4; i++)
				tempCalculate1 = "0" + tempCalculate1;
			tempCalculate += tempCalculate1;
			ApproachSpeed *= 10;
			_ltoa(long(ApproachSpeed), axTarget1, 16);
			tempCalculate1 = axTarget1;
			if (tempCalculate1.length() > 2)
				tempCalculate1 = tempCalculate1.substr(0, 2);
			for (int i = tempCalculate1.length(); i < 2; i++)
				tempCalculate1 = "0" + tempCalculate1;
			tempCalculate += tempCalculate1;
			//ProbeMoveUp was an ill-conceived idea. We shall now hijack it to program the controller to switch between Forward touch and retract probe touch methods!
			if (ProbeMoveUpFlag)
				tempCalculate += "1";
			else
				tempCalculate += "0";
			_ltoa(long(ProbeSensitivity), axTarget, 16);
			tempCalculate1 = axTarget;
			if (tempCalculate1.length() > 4)
				tempCalculate1 = tempCalculate1.substr(0, 4);
			for (int i = tempCalculate1.length(); i < 4; i++)
				tempCalculate1 = "0" + tempCalculate1;
			tempCalculate += tempCalculate1;
			for (int i = 0; i < tempCalculate.length(); i++)
			{
				if (i == 18) break;
				tempstr[i] = toupper(tempCalculate.substr(i, 1).front());
			}
			AxisTarget = (const char*)tempstr;
			std::reverse(AxisTarget.begin(), AxisTarget.end());
			AxisTarget = "A" + AxisTarget;
		}
		else if (DROInstance->newCardFlag == 2)
		{
			if (ApproachDist == 0) ApproachDist = 1;
			if (RevertDist == 0) RevertDist = 1;
			if (ApproachSpeed == 0) ApproachSpeed = 0.1;
			if (RevertSpeed == 0) RevertSpeed = 2;

			if (AxisPropCollection[AxisNo].MovementType)
			{
				ApproachDist *= AxisPropCollection[AxisNo].CountsPerUnit / AxisPropCollection[AxisNo].MultipleFactor;
				RevertDist *= AxisPropCollection[AxisNo].CountsPerUnit / AxisPropCollection[AxisNo].MultipleFactor;
			}
			else
			{
				ApproachDist *= AxisPropCollection[AxisNo].CountsPerUnit / 360;
				RevertDist *= AxisPropCollection[AxisNo].CountsPerUnit / 360;
			}
			//RevertDist *= AxisPropCollection[0].CountsPerUnit / AxisPropCollection[0].MultipleFactor;

			int values[4] = { ApproachDist, ApproachSpeed * 10, RevertDist, RevertSpeed * 10 };
			int headers[4] = { 50, 60, 70, 80 };
			int commandlengths[4] = { 4, 4, 4, 4 };
			for (int i = 0; i < 4; i++)
			{
				char AxisCnt[2]; // = "\0";
				char axTarget[4]; // = "\0";
				char tempS[6] = "\0";
				std::string CommandStr, AxisStr, tempCalculate, AxisTarget;
				_itoa(long(headers[i] + AxisNo + 1), AxisCnt, 16);
				AxisStr = AxisCnt;
				if (AxisStr.length() > 2)
					AxisStr = AxisStr.substr(0, 2);
				_ltoa(long(values[i]), axTarget, 16);
				tempCalculate = axTarget;
				if (tempCalculate.length() > 4)
					tempCalculate = tempCalculate.substr(0, 4);
				for (int j = tempCalculate.length(); j < commandlengths[i]; j++)
					tempCalculate = "0" + tempCalculate;
				AxisTarget = AxisStr + tempCalculate; 
				for (int j = 0; j < AxisTarget.length(); j++)
					tempS[j] = toupper(AxisTarget.substr(j, 1).front());

				AxisTarget = (const char*)tempS;
				AxisTarget = AxisTarget.substr(0, commandlengths[i] + 2);
				std::reverse(AxisTarget.begin(), AxisTarget.end());
				AxisTarget = "E" + AxisTarget;
				sendMachineCommand_One((char*)AxisTarget.c_str(), true);
				
			}
			return;
		}
		else
		{
			for (int i = 0; i < tempCalculate.length(); i++)
			{
				if (i == 7) break;
				tempstr[i] = toupper(tempCalculate.substr(i,1).front());
			}
			AxisTarget = (const char*)tempstr;
			std::reverse(AxisTarget.begin(), AxisTarget.end());
			AxisTarget = "A" + AxisTarget;
		}	
		sendMachineCommand_One((char*)AxisTarget.c_str(), true);			
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0044", __FILE__, __FUNCSIG__); }
}

std::string DigitalReadOut::CalculateCRC(std::string AxisTarget)
{
	std::string FinalCRCStr = "";
	try
	{
		if(AxisTarget.length() % 2 != 0)
			AxisTarget += "0";
		int n = AxisTarget.length() / 2;
		BYTE* ByteArray = new BYTE[n + 2];
		ConvertToByteArray(AxisTarget, ByteArray);
		ByteArray[n] = 0;
		ByteArray[n + 1] = 0;
		unsigned __int16 CRCValue = Get16bitCRC(ByteArray, n);
		char crcTest[5] = "\0";
		_itoa(CRCValue, crcTest, 16);
		std::string CRCStr = crcTest;
		char TmpChar[5] = "\0";
		for (int i = 0; i < CRCStr.length(); i++)
		{
			if (i == CRCStr.length()) break;
			TmpChar[i] = toupper(CRCStr.substr(i,1).front());
		}
		CRCStr = TmpChar;
		for(int i = CRCStr.length(); i < 4; i++)
			CRCStr = "0"+ CRCStr;
		FinalCRCStr = AxisTarget + CRCStr;
		delete [] ByteArray;
		return FinalCRCStr;
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0045", __FILE__, __FUNCSIG__); return FinalCRCStr;}
}

unsigned __int16 DigitalReadOut::Get16bitCRC(BYTE* ptr, int length)
{
	unsigned __int16 CRCValue = (ptr[0] << 8) | ptr[1];
	try
	{	
		for( int i = 0; i < length * 8; i ++)
		{
			//unsigned __int16 NextBit = (ptr[(int)(i / 8) + 2] & (1 << (7 - (i % 8)))) >> (7 - (i % 8));
			unsigned __int16 NextBit = (ptr[(int)(i / 8) + 2] & (1 << (7 - (i % 8)))) ? 1 : 0;
			if(CRCValue & 0x8000)
				CRCValue = ((CRCValue << 1) + NextBit) ^ 0x8005;
			else
				CRCValue = (CRCValue << 1) + NextBit;
		}
		return CRCValue;
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0046", __FILE__, __FUNCSIG__); return CRCValue;}
}

bool DigitalReadOut::CheckCRC(std::string DroString)
{
	try
	{
		bool ReturnFlag = false;
		int n = DroString.length() / 2;
		BYTE* ByteArray = new BYTE[n + 2];
		ConvertToByteArray(DroString, ByteArray);
		ByteArray[n] = 0;
		ByteArray[n + 1] = 0;
		unsigned __int16 CRCValue = Get16bitCRC(ByteArray, n);
		delete [] ByteArray;
		if(CRCValue == 0)
			ReturnFlag = true;
		return ReturnFlag;
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0046", __FILE__, __FUNCSIG__); return false;}
}
//Reading Data from Controller Continuously.
void ReadingDROValue(void* anything)
{
	try
	{	
		DROInstance->Probe_HitFlag = false;
		int SaveValueCnt = 20; //Save 20 previous position values..Used in finding the movement direction for probe hit//
		int TargetReachedCount = 0; //variable to count the target reached symbol//
		int MachineDisconnetCount = 0; //To count the machine disconnected..i.if the readbuffer is zero then increment//
		int CounterValue[4] = {0}; //Counter for three axis...used to check the limit switch hit.//
		int lineNumberChangedCounter = 0;
		int CycleCounter = 0; //Counter used to check the limit switch hit.//
		double PreviousDROValue[4] = {-1000}; //store last dor value..used to check the limit switch hit//
		double LastChangedPosition[4] = { -1000 }; //Last changed Count value after which we have a major jump in DRO
		int JumpCycleCount[4] = { 0 }; //Counts the number of cycles we observe if the DRO jump is a stable jump
		bool DroJumpedFlag[4] = { false };
		double Version_Number[5] = { 1001 };
		double mmCalculation = 0; //to hold the current axis mm value
		static bool JoystickErrorWrittentoFile = false;
		
		list<double> ValuesX, ValuesY, ValuesZ; //array to hold the 20 values..for probe hit direction
		
		bool DroChanged = false, CardStatusFlag = true;
		unsigned long bufferLength;
		char tempbuffer[10];
		int LimitReachedCount = 0; // , lastAxisLimitHit;
		int Finished_2sPP_StepCt = 0;
		std::string RBuffer(""); 
		std::string TempReadBuffer(""); // = "";
		//DigitalReadOut* readingThread = (DigitalReadOut*)anything;
		int RCounter = 0, H_Position = -1000;
		double CurrentCompvalue[4] = {0}; //for value change comparision.. according to the axis type set the value
		for(int i = 0; i < 4; i++)
		{
			if(DROInstance->AxisPropCollection[i].MovementType)
			{
				CurrentCompvalue[i] = 0.0005;
				if(i == 3) CurrentCompvalue[i] = 4 * M_PI / DROInstance->AxisPropCollection[i].CountsPerUnit;
			}
			else
				CurrentCompvalue[i] = 4 * M_PI / DROInstance->AxisPropCollection[i].CountsPerUnit;
		}
		int NumberOfChar = 32;
		/*if (DROInstance->newCardFlag == 3)
			NumberOfChar = 6;
		else*/ if (DROInstance->newCardFlag == 0)
			NumberOfChar = 27;
		else if (DROInstance->newCardFlag == 1)
			NumberOfChar = 32;
		else
			NumberOfChar = 46;

		//int DidNotGetXforThisManyCycles = 0;
		bool CalledActivateOnlineSystm = false;
		int CyclecCt = 0; 

		/*DWORD d1, d2, d3;
		d1 = GetTickCount();*/
		while(DROInstance->ReadDroValues)
		{
			try
			{
			//If writing command then dont do anything..You can't get any data from controller
				if(DROInstance->newCardFlag == 0)
				{
					if(DROInstance->WritingCommands) continue; 
				}
				//Read the values..
				bool ReadFlag = true, Xpresent = false;
				TempReadBuffer = "";

				while(ReadFlag)
				{
					ReadFile(DROInstance->PortHandle, tempbuffer, 1, &bufferLength, NULL);

					if (bufferLength == 0 || DROInstance->PortHandle == INVALID_HANDLE_VALUE)
					{
						if (DROInstance->newCardFlag == 3)
						{
							MachineDisconnetCount++;
							if (MachineDisconnetCount > 20)
							{
								//MachineDisconnetCount = 0;
								if (!DROInstance->VersionControlflag)
								{
									CyclecCt++;
									//We are all set. So let us not waste time reading continuously in the hope of receiving data. we are not going to !!!
									ReadFlag = false;
									//DROInstance->ReadDroValues = false;
									break;
								}
								else
								{
									//Software hanged or closed down without machine resetting to send XDRILL. We have started software alone. 
									//So ask machine to start sending XDRILL so initialisation can start.
									DROInstance->ReadDroValues = true;
									if (!CalledActivateOnlineSystm)
									{
										CalledActivateOnlineSystm = true;
										//_beginthread(&WriteCommandInDiffthread, 0, "50");
										DROInstance->DROTargetReachedCallback(31);
									}
								}
							}
							else
								continue;
						}
						else
						{
							MachineDisconnetCount++;
							if (MachineDisconnetCount > DROInstance->MachineDisconnect_CycleCount)
							{
								DROInstance->MachineConnetedStatus = false;
								if (CardStatusFlag && DROInstance->VersionControlflag)
								{
									NumberOfChar = 32;
									CardStatusFlag = false;
									MachineDisconnetCount = 0;
									DROInstance->DROTargetReachedCallback(1);
								}
								else if (!DROInstance->VersionControlflag)
								{
									DROInstance->DROTargetReachedCallback(2);
									_endthread();
								}
								if (!DROInstance->VersionControlflag) //If there is no data then say machine is disconnected and end the continuous read
								{
									/*d2 = GetTickCount();
									d3 = d2 - d1;*/
									DROInstance->DROTargetReachedCallback(6);
									_endthread(); 
								}
								//else if(CardStatusFlag && DROInstance->VersionControlflag && MachineDisconnetCount > 20)
								//{
								//	NumberOfChar = 32;
								//	CardStatusFlag = false;
								//	DROInstance->DROTargetReachedCallback(2);	
								//	_endthread();

							}
							//}

						}
						continue;
					}
					else
						MachineDisconnetCount = 0;

					tempbuffer[1] = '\0';
					std::string str = tempbuffer;
					if(str == "X")
					{
						if(TempReadBuffer.size() > NumberOfChar)
						{
							char temp = TempReadBuffer.at(TempReadBuffer.size() - 1);
							if(temp == '{') //If X is the probe value..
							{
								ReadFlag = true;
								TempReadBuffer = TempReadBuffer + str;
							}
							else //Else stop reading.
							{
								ReadFlag = false;
								if(!Xpresent)
									TempReadBuffer = "X" + TempReadBuffer;
							}
						}
						else //if X is in middle or starting
						{
							Xpresent = true;
							TempReadBuffer = TempReadBuffer + str;
							ReadFlag = true;
						}
					}
					else if(str == "+")
						DROInstance->PositiveAcknowledgementCnt = 2;
					else if(str == "-")
						DROInstance->PositiveAcknowledgementCnt = 1;
					else if (str == "S")
					{
						DROInstance->DROTargetReachedCallback(32);
					}
					else if (str == "T")
					{
						DROInstance->DROTargetReachedCallback(33);
					}
					else if (str == "Y")
					{
						if (DROInstance->newCardFlag == 3)
						{
							if (TempReadBuffer.size() > 8 && TempReadBuffer.find("X") == -1)
								ReadFlag = false;
							else
							{
								TempReadBuffer = TempReadBuffer + str;
								ReadFlag = true;
							}
						}
						else
							TempReadBuffer = TempReadBuffer + str;
					}
					else
					{
						TempReadBuffer = TempReadBuffer + str;
						if (TempReadBuffer.length() > 960 && Xpresent == false)
						{
							//We are not getting any normal value in input. So let us send a * and see if we start getting values...
							DROInstance->ClearInBuffer();
							Sleep(100);
							DROInstance->WriteEachByte('*');
							TempReadBuffer = "";
						}
					}
				}

				if (DROInstance->ActivateNextStepFor_2sPP)
				{
					Finished_2sPP_StepCt++;
					if (Finished_2sPP_StepCt >= 3)
					{
						//We raise event to go to do next step in 2-step homing
						DROInstance->DROTargetReachedCallback(21);
						DROInstance->ActivateNextStepFor_2sPP = false;
						Finished_2sPP_StepCt = 0;
					}
				}
				//Finished reading in buffer. Let us now process the input data
				//Check to see if we have any error messages. If not, we can reset those flags - 
				H_Position = TempReadBuffer.find("H");
				if (H_Position >= 0)
				{
					//We got an error message! Keep reading characters until we get an "X"
					//Let us read the error type
					//TempReadBuffer = TempReadBuffer + str;
					//ReadFile(DROInstance->PortHandle, tempbuffer, 1, &bufferLength, NULL);
					//str = tempbuffer;
					//TempReadBuffer = TempReadBuffer + str;

					std::string str = TempReadBuffer.substr(H_Position + 1, 1);
					if (str == "0")
					{
						if (!JoystickErrorWrittentoFile)
						{
							//Joystick is not communicating with machine
							DROInstance->SetAndRaiseErrorMessage("HCDLL0047j", __FILE__, __FUNCSIG__);
							DROInstance->DROTargetReachedCallback(27);
							JoystickErrorWrittentoFile = true;
						}
					}
					else if (str == "1")
					{
						//Central Card and Axis cards are not communicating properly.
						DROInstance->SetAndRaiseErrorMessage("HCDLL0047c", __FILE__, __FUNCSIG__);
					}
					else if (str == "2")
					{
						CycleCounter++;
						//Limit has been hit
						if (CycleCounter > DROInstance->LimitHit_DroCycleCount)
						{
							if (DROInstance->HomePositionMode)
							{
								//We wait until we reach limit in ALL axes...
								int limitHitAxis = atoi(TempReadBuffer.substr(2, 1).c_str());
								DROInstance->AxisPropCollection[limitHitAxis].LimitSwitchActivated = true;

								DROInstance->WriteCommandSent("Home Position Reached");

							}
							else
							{
								str = TempReadBuffer.substr(TempReadBuffer.find("H") + 2, 1);
								if (str == "9")
								{
									DROInstance->DROTargetReachedCallback(25);
									DROInstance->A_Axis_isHorizontal = true;
								}
								else if (str == "A")
								{
									DROInstance->A_Axis_isHorizontal = false;
									DROInstance->DROTargetReachedCallback(26);
								}
								else  //if (DROInstance->newCardFlag < 2)
								{
									if (DROInstance->CanRaiseMachineStuckMessage)
									{										//raise message machine seems to be stuck....
										DROInstance->WriteCommandSent("Limit Switch hit: " + TempReadBuffer);
										//DROInstance->DROTargetReachedCallback(13);
										char cd[4] = "\0";
										_itoa(H_Position, cd, 10); //DROInstance->LastLineExecuting
										DROInstance->WriteCommandSent(cd);
										DROInstance->WriteCommandSent(str);
										//DROInstance->ToManual();
										DROInstance->DROTargetReachedCallback(13);
										DROInstance->CanRaiseMachineStuckMessage = false;
									}
								}
							}
							//DROInstance->ToManual();
							//DROInstance->DROTargetReachedCallback(13);
							//DROInstance->CanRaiseMachineStuckMessage = false;
						}
					}
					else if (str == "3")
					{
						str = TempReadBuffer.substr(H_Position + 2, 1);
						DROInstance->WriteCommandSent("Probe hit before target reached in Line");
						//DROInstance->DROTargetReachedCallback(13);
						char cd[4] = "\0";
						_itoa(DROInstance->LastLineExecuting, cd, 10);		
						DROInstance->WriteCommandSent(cd);
						DROInstance->WriteCommandSent("Probe hit while moving in Axis");
						DROInstance->WriteCommandSent(str);
						//DROInstance->ToManual();
						DROInstance->DROTargetReachedCallback(28);
					}
					else if (str == "4")
					{
						DROInstance->WriteCommandSent("There was a problem in transmitting the command to the controller or the command syntax was wrong");
						DROInstance->DROTargetReachedCallback(29);
					}
				}
				else
					JoystickErrorWrittentoFile = false;
				
				if (TempReadBuffer.find("H") < 0)
					CycleCounter = 0;

				DROInstance->MachineConnetedStatus = true;
				//if(DROInstance->VersionControlflag && CardStatusFlag)
				//{
				//	CardStatusFlag = false;
				//	//MessageBox(NULL,(LPCWSTR)TempReadBuffer.c_str(), NULL, NULL);
				//	if (TempReadBuffer == "XDRILL")
				//	{
				//		NumberOfChar = 6;
				//		DROInstance->DROTargetReachedCallback(30);
				//	}
				//	else if(TempReadBuffer.find("L") != -1)
				//	{
				//		int x_index = TempReadBuffer.find("X");
				//		int y_index = TempReadBuffer.find("Y");
				//		if (y_index == -1 && TempReadBuffer.substr(x_index + 1, 5) == "DRILL")
				//		{
				//			NumberOfChar = 6;
				//			DROInstance->DROTargetReachedCallback(30);
				//			//Send Write COmmand in a different thread. Callback runs in the same thread! and hence we dont read the acknowledgements!!!
				//			//_beginthread(&WriteCommandInDiffthread, 0, "51");
				//			//DROInstance->ClearInBuffer();
				//		}
				//		else if (y_index - x_index == 9)
				//		{
				//			NumberOfChar = 96;
				//			DROInstance->DROTargetReachedCallback(18); // we have v33.2 cards here since we have 8-char input
				//		}
				//		else
				//		{
				//			NumberOfChar = 32;
				//			DROInstance->DROTargetReachedCallback(15);
				//		}

				//	}
				//	else
				//	{
				//		NumberOfChar = 27;
				//		DROInstance->DROTargetReachedCallback(14);
				//	}
				//	int chCount = TempReadBuffer.find("V");
				//	if (NumberOfChar == 32 && chCount < 0)
				//	{
				//		//DROInstance->VersionControlflag = false;
				//		//TempReadBuffer = "";
				//	}
				//	else if (NumberOfChar != 96 && chCount >= 0)
				//	{
				//		for (int axisCount = 0; axisCount < DROInstance->NumberofMachineAxes + 1; axisCount++)
				//		{
				//			std::string HexValue = TempReadBuffer.substr(chCount + 1 + 4 * axisCount, 4);
				//			std::reverse(HexValue.begin(), HexValue.end());
				//			Version_Number[axisCount] = (atof((const char*)HexValue.c_str())) / 100;
				//		}
				//		DROInstance->WriteVersionNumber(Version_Number, 0);
				//		TempReadBuffer = "";
				//	}
				//	else if (chCount >= 0)
				//		DROInstance->SixAxisD33CardisPresent = true;

				//	DROInstance->VersionControlflag = false;
				//}

				if (DROInstance->VersionControlflag && CardStatusFlag)
				{
					CardStatusFlag = false;
					//MessageBox(NULL,(LPCWSTR)TempReadBuffer.c_str(), NULL, NULL);
					if (TempReadBuffer == "XDRILL")
					{
						NumberOfChar = 6;
						DROInstance->DROTargetReachedCallback(30);
					}
					else if (TempReadBuffer.find("L") != -1)
					{
						int x_index = TempReadBuffer.find("X");
						int y_index = TempReadBuffer.substr(x_index).find("Y");
						if (y_index == -1 && TempReadBuffer.substr(x_index + 1, 5) == "DRILL")
						{
							NumberOfChar = 6;
							DROInstance->DROTargetReachedCallback(30);
							//Send Write COmmand in a different thread. Callback runs in the same thread! and hence we dont read the acknowledgements!!!
							//_beginthread(&WriteCommandInDiffthread, 0, "51");
							//DROInstance->ClearInBuffer();
						}
						else if (y_index == 9)
						{
							NumberOfChar = 96;
							DROInstance->DROTargetReachedCallback(18); // we have v33.2 cards here since we have 8-char input
						}
						else
						{
							NumberOfChar = 32;
							DROInstance->DROTargetReachedCallback(15);
						}

					}
					else
					{
						if (TempReadBuffer.find("Y") != -1 && TempReadBuffer.find("X") == -1)
						{
							NumberOfChar = 6;
							DROInstance->DROTargetReachedCallback(30);
							//CardStatusFlag = true;
							//char *ccc = "50";
							//_beginthread(&WriteCommInDiffthread, 0, ccc);
							//TempReadBuffer = "";
							//continue;
						}
						else
						{
							NumberOfChar = 27;
							DROInstance->DROTargetReachedCallback(14);
						}
					}
					int chCount = TempReadBuffer.find("V");
					if (NumberOfChar == 32 && chCount < 0)
					{
						//DROInstance->VersionControlflag = false;
						//TempReadBuffer = "";
					}
					else if (NumberOfChar != 96 && chCount >= 0)
					{
						for (int axisCount = 0; axisCount < DROInstance->NumberofMachineAxes + 1; axisCount++)
						{
							std::string HexValue = TempReadBuffer.substr(chCount + 1 + 4 * axisCount, 4);
							std::reverse(HexValue.begin(), HexValue.end());
							Version_Number[axisCount] = (atof((const char*)HexValue.c_str())) / 100;
						}
						DROInstance->WriteVersionNumber(Version_Number, 0);
						TempReadBuffer = "";
					}
					else if (chCount >= 0)
						DROInstance->SixAxisD33CardisPresent = true;

					DROInstance->VersionControlflag = false;
				}


				if(DROInstance->newCardFlag > 0)
				{
					//int CCount = TempReadBuffer.find("=");
					//if(CCount != -1)
					//{
					//	std::string DroString = "";
					//	if (DROInstance->newCardFlag == 1 && TempReadBuffer.size() > CCount + 5 && TempReadBuffer.substr(CCount + 5).find("E") != -1)
					//		DroString = DROInstance->GetStringForCRC(TempReadBuffer.substr(CCount + 5), "E", 2);
					//	else if (DROInstance->newCardFlag == 1 && TempReadBuffer.find("J") != -1)
					//	{
					//		DroString = DROInstance->GetStringForCRC(TempReadBuffer, "J", 2);
					//	}
					//	int chCount = TempReadBuffer.find("X");
					//	if(TempReadBuffer.find("{") != -1 && TempReadBuffer.find("}") != -1)
					//	{
					//		for(int i = 0; i < 4; i++)
					//		{
					//			std::string HexValue = TempReadBuffer.substr(chCount + 1 + (DROInstance->AxisPropCollection[i].AxisdataLength + 1) * i, DROInstance->AxisPropCollection[i].AxisdataLength);
					//			DroString += HexValue;
					//		}
					//		int LCount = TempReadBuffer.find("L");
					//		if(LCount != -1)
					//		{
					//			std::string HexValue = TempReadBuffer.substr(LCount + 1, 4);
					//			DroString += HexValue;
					//			int PCount = TempReadBuffer.find("{");
					//			if(PCount != -1)
					//			{
					//				for(int i = 0; i < 4; i++)
					//				{
					//					HexValue = TempReadBuffer.substr(PCount + 2 + (DROInstance->AxisPropCollection[i].AxisdataLength + 1) * i, DROInstance->AxisPropCollection[i].AxisdataLength);
					//					DroString += HexValue;
					//				}
					//				//Read Probe Direction
					//				HexValue = TempReadBuffer.substr(TempReadBuffer.find("}") + 1, 4);
					//				DroString += HexValue;
					//				//Now read the input CRC
					//				if(CCount != -1)
					//				{
					//					HexValue = TempReadBuffer.substr(CCount + 1, 4);
					//					std::reverse(HexValue.begin(), HexValue.end());
					//					DroString += HexValue;
					//					//if(!DROInstance->CheckCRC(DroString))
					//						//continue;	
					//				}
					//			}
					//		}
					//	}
					//	else if(TempReadBuffer.size() > NumberOfChar)
					//	{
					//		for(int i = 0; i < 4; i++)
					//		{
					//			std::string HexValue = TempReadBuffer.substr(chCount + 1 + (DROInstance->AxisPropCollection[i].AxisdataLength + 1) * i, DROInstance->AxisPropCollection[i].AxisdataLength);
					//			DroString += HexValue;
					//		}
					//		chCount = TempReadBuffer.find("L");
					//		if(chCount != -1)
					//		{
					//			std::string HexValue = TempReadBuffer.substr(chCount + 1 , 4);
					//			DroString += HexValue;
					//			if(CCount != -1)
					//			{
					//				HexValue = TempReadBuffer.substr(CCount + 1, 4);
					//				std::reverse(HexValue.begin(), HexValue.end());
					//				DroString += HexValue;
					//				//if(!DROInstance->CheckCRC(DroString))
					//					//continue;	
					//			}	
					//		}
					//	}
					//}
					
					if (DROInstance->newCardFlag == 2)
					{
						if (TempReadBuffer.find("#") != -1)
						{
							DROInstance->CNCModeCount++;
							if (DROInstance->CNCModeCount > 10) DROInstance->CNCModeCount = 10;
						}
						else
						{
							DROInstance->CNCModeCount--;
							if (DROInstance->CNCModeCount < 0) DROInstance->CNCModeCount = 0;
						}
						
						if (DROInstance->CNCModeCount > 9)
						{
							if (!DROInstance->CNCMode)
							{
								DROInstance->CNCMode = true;
								DROInstance->DROTargetReachedCallback(19);
							}
						}
						else if (DROInstance->CNCModeCount < 1)
						{
							if (DROInstance->CNCMode)
							{
								DROInstance->CNCMode = false;
								DROInstance->DROTargetReachedCallback(20);
							}
						}
					}
				}
				
				RBuffer = TempReadBuffer;
				DROInstance->ReadBuffer = TempReadBuffer;		

				if (DROInstance->newCardFlag == 3)
				{				//If its Drill Check System, look for S and T for knowing we have reached target. 
				//	int found_S = RBuffer.find("S");
				//	int found_T = RBuffer.find("T");
				//	if (found_S >= 0)
				//		DROInstance->DROTargetReachedCallback(31);
				//	else if (found_T >=0)
				//		DROInstance->DROTargetReachedCallback(32);
				//}
				//int found_X = RBuffer.find("X"), found_Y = RBuffer.find("Y"), found_Z = RBuffer.find("Z");
				//if (found_X == -1 || found_Y == -1 || found_Z == -1)
				//{
				//	// we have Online System. We will not get X Y or Z values in...
					//Sleep(50);
					if (CyclecCt > 200)
					{
						DROInstance->DROTargetReachedCallback(10);
						//DROInstance->ReadDroValues = false;
						CyclecCt = 0;
					}
					continue;
				}
				//Probe Related processing...

				int Pi = RBuffer.find("{"), Pj = RBuffer.find("}");
				if(Pi == -1 && Pj == -1)
				{
					if (!DROInstance->Probe_HitFlag)
					{
						DROInstance->AxisPropCollection[0].CurrentProbePosition = -1000;
						DROInstance->AxisPropCollection[1].CurrentProbePosition = -1000;
						DROInstance->AxisPropCollection[2].CurrentProbePosition = -1000;
						DROInstance->AxisPropCollection[3].CurrentProbePosition = -1000;
					}
					//else
					//{
					//	if (DROInstance->newCardFlag == 2 && DROInstance->Probe_HitFlag)
					//		DROInstance->Probe_HitFlag = false;
					//}
				}
				if (DROInstance->PorbeActivated)
				{
					if (DROInstance->newCardFlag < 2)
						DROInstance->FindProbeHitDirection(ValuesX, ValuesY, ValuesZ);
					
					int Ppi = RBuffer.find("P");
					if(Ppi == -1)
					{				
						DROInstance->PorbeActivated = false;
						if(DROInstance->newCardFlag == 0 || !DROInstance->CNCMode)
						{
							DROInstance->Probe_HitFlag = false;
							//if(DROInstance->ProbeRetractPointMethod)
							//{
							//	DROInstance->FindProbeHitDirection(ValuesX, ValuesY, ValuesZ);
							//	DROInstance->LookTargetReachedSym = false;
							//	DROInstance->DROTargetReachedCallback(12);							
							//}
							//DROInstance->DROTargetReachedCallback(0);
						}
					}
				}
				if (Pi > 0 && Pj > 0 && (Pj > Pi))
				{
					if (!DROInstance->AxisPropCollection[0].MachineStartflag && !DROInstance->Probe_HitFlag)
					{
						//DROInstance->WritedebugInfo(RBuffer);
						int chCount = RBuffer.find("{");
						bool ProbeFlag = false;
						//std::wofstream myfile;
						//std::string filePath = "E:\\TouchPts.csv";
						//myfile.open(filePath.c_str(), std::ios_base::app);
						for (int axisCount = 0; axisCount < DROInstance->NumberofMachineAxes; axisCount++)
						{
							//This Calculation is Common for both Linear and Rotation..........
							std::string HexValue = RBuffer.substr(chCount + 2 + (DROInstance->AxisPropCollection[axisCount].AxisdataLength + 1) * axisCount, DROInstance->AxisPropCollection[axisCount].AxisdataLength);
							std::reverse(HexValue.begin(), HexValue.end());
							DROInstance->AxisPropCollection[axisCount].ProbeHexaDecimalValue = HexValue;
							mmCalculation = DROInstance->hex_dec(HexValue);
							(DROInstance->AxisPropCollection[axisCount].MovementDirection) ? mmCalculation = mmCalculation - DROInstance->AxisPropCollection[axisCount].McsZero : mmCalculation = DROInstance->AxisPropCollection[axisCount].McsZero - mmCalculation;
							//Looking for Linear or Rotation.......
							//myfile << mmCalculation << ",";
							if (DROInstance->AxisPropCollection[axisCount].MovementType)
							{//The Multiplication Factor for the axis..
								mmCalculation *= DROInstance->AxisPropCollection[axisCount].MultipleFactor;
								mmCalculation /= DROInstance->AxisPropCollection[axisCount].CountsPerUnit;
								if (abs(DROInstance->AxisPropCollection[axisCount].CurrentProbePosition - mmCalculation) > 0.001)
									ProbeFlag = true;
							}
							else
							{
								mmCalculation = (mmCalculation * 2 * M_PI) / DROInstance->AxisPropCollection[axisCount].CountsPerUnit;
								//We give a correction for the non-concentricity of encoder 
								mmCalculation -= DROInstance->AxisPropCollection[axisCount].MultipleFactor * sin(mmCalculation);
								if (abs(DROInstance->AxisPropCollection[axisCount].CurrentProbePosition - mmCalculation) > 0.01)
									ProbeFlag = true;
							}
							//myfile << mmCalculation << ",";
							//Looking for Change in DRO..............
							if (ProbeFlag)// abs(DROInstance->AxisPropCollection[axisCount].CurrentProbePosition - mmCalculation) > 0.5)
							{
								//ProbeFlag = true;
								if (DROInstance->newCardFlag == 0 || !DROInstance->CNCMode)
									DROInstance->PorbeActivated = true;
								DROInstance->AxisPropCollection[axisCount].CurrentProbePosition = mmCalculation;
								DROInstance->ProbeCoordinate[axisCount] = mmCalculation;
							}
						}
						//myfile << endl;
						//myfile.close();
						if (ProbeFlag && !DROInstance->Probe_HitFlag)
						{
							//DROInstance->WritedebugInfo("---Next Point---");
							DROInstance->Probe_HitFlag = true;
							//MessageBox(NULL, L"Got ProbeHit", L"Rapid-I", MB_TOPMOST);
							if (DROInstance->newCardFlag == 2)
							{
								std::string HexValue = TempReadBuffer.substr(Pj + 1, 4);
								//std::wstring stemp = std::wstring(HexValue.begin(), HexValue.end());
								//LPCWSTR sw = stemp.c_str();
								//MessageBox(NULL, sw, L"Rapid-I", MB_TOPMOST);
								std::reverse(HexValue.begin(), HexValue.end());
								DROInstance->FindProbeHitDirection(DROInstance->hex_dec(HexValue));
							}
							else
							{
								if (!DROInstance->ProbeRetractPointMethod)
									DROInstance->FindProbeHitDirection(ValuesX, ValuesY, ValuesZ);
							}
							DROInstance->LookTargetReachedSym = false;
							DROInstance->DROTargetReachedCallback(12);
						}
					}
				}
				else
					DROInstance->Probe_HitFlag = false;
				//End of the Probe value Processing..
				//

				//
				//Normal DRO value processing...

				DroChanged = false;
				bool RaxisValueChanged = false;
				double CurrentDROvalue[4] = {0}, CurrentDiffvalue[4] = {0};
				int chCount = RBuffer.find("X");
				
				//If we have the 6-axis D33 card for TIS do the following - 
				if (DROInstance->SixAxisD33CardisPresent)
				{
					int u_index = RBuffer.find("U");
					int V_index = RBuffer.find("V");
					if (u_index >= 0)
					{
						DROInstance->ExtraAxesValues[0] = RBuffer.substr(u_index + 1, 8);
						std::reverse(DROInstance->ExtraAxesValues[0].begin(), DROInstance->ExtraAxesValues[0].end());

					}
					if (V_index >= 0)
					{
						DROInstance->ExtraAxesValues[1] = RBuffer.substr(V_index + 1, 8);
						std::reverse(DROInstance->ExtraAxesValues[1].begin(), DROInstance->ExtraAxesValues[1].end());
					}
				}
				
				for(int axisCount = 0; axisCount < DROInstance->NumberofMachineAxes; axisCount++)
				{
					//This Calculation is Common for both Linear and Rotation.......
					std::string HexValue = RBuffer.substr(chCount + 1 + (DROInstance->AxisPropCollection[axisCount].AxisdataLength + 1) * axisCount, DROInstance->AxisPropCollection[axisCount].AxisdataLength);
					std::reverse(HexValue.begin(), HexValue.end());
					DROInstance->AxisPropCollection[axisCount].ActualCounts = HexValue;
					mmCalculation = DROInstance->hex_dec(HexValue);
					
					double tdiff = abs(LastChangedPosition[axisCount] - mmCalculation);
					double tth = DROInstance->DROJumpThreshold;
					if (!DROInstance->AxisPropCollection[axisCount].MachineStartflag && DROInstance->CNCMode &&  tdiff > tth)
					{

						JumpCycleCount[axisCount]++;
						if (JumpCycleCount[axisCount] > 5 && !DroJumpedFlag[axisCount])
						{
							DroJumpedFlag[axisCount] = true;
							DROInstance->DROTargetReachedCallback(22 + axisCount);
						}
						if (JumpCycleCount[axisCount] > 1000) JumpCycleCount[axisCount] = 6;
					}
					else
					{
						JumpCycleCount[axisCount] --;
						if (JumpCycleCount[axisCount] == 0) DroJumpedFlag[axisCount] = false;
					}
					LastChangedPosition[axisCount] = mmCalculation;

					if(DROInstance->AxisPropCollection[axisCount].MachineStartflag)
					{
						//DROInstance->WritedebugInfo(HexValue);
						DROInstance->AxisPropCollection[axisCount].MachineStartflag = false;
						if (NumberOfChar == 96)
							DROInstance->AxisPropCollection[axisCount].McsZero = 268435456; //mmCalculation;
						else
							DROInstance->AxisPropCollection[axisCount].McsZero = 1048576; //mmCalculation;
					}
					(DROInstance->AxisPropCollection[axisCount].MovementDirection)? mmCalculation = mmCalculation - DROInstance->AxisPropCollection[axisCount].McsZero: mmCalculation = DROInstance->AxisPropCollection[axisCount].McsZero - mmCalculation;

					//Looking for Linear or Rotation.......
					if (DROInstance->AxisPropCollection[axisCount].MovementType)
					{
						//The Multiplication Factor for the axis..
						mmCalculation *= DROInstance->AxisPropCollection[axisCount].MultipleFactor;
						mmCalculation /= DROInstance->AxisPropCollection[axisCount].CountsPerUnit;
					}
					else
					{

						mmCalculation = (mmCalculation * 2 * M_PI) / DROInstance->AxisPropCollection[axisCount].CountsPerUnit;
						//We give a correction for the non-concentricity of encoder 
						mmCalculation -= DROInstance->AxisPropCollection[axisCount].MultipleFactor * sin(mmCalculation);
					}
					//Looking for Lash Value.......
					if(DROInstance->AxisPropCollection[axisCount].LashDirection && (mmCalculation - DROInstance->AxisPropCollection[axisCount].CurrentPosition) > 0)
						mmCalculation += DROInstance->AxisPropCollection[axisCount].LashCompensation;
					if(!(DROInstance->AxisPropCollection[axisCount].LashDirection) && (mmCalculation - DROInstance->AxisPropCollection[axisCount].CurrentPosition) < 0)
						mmCalculation += DROInstance->AxisPropCollection[axisCount].LashCompensation;
					//Looking for Change in DRO..............
					CurrentDiffvalue[axisCount] = abs(DROInstance->AxisPropCollection[axisCount].CurrentPosition - mmCalculation);
					DROInstance->AxisPropCollection[axisCount].CurrentPosition = mmCalculation;
					CurrentDROvalue[axisCount] = mmCalculation;
					if(CurrentDiffvalue[axisCount] > CurrentCompvalue[axisCount]) //check for dro change
					{
						if(axisCount == 3) RaxisValueChanged = true;
						DroChanged = true;
					}	
					if (DROInstance->GageConnected && axisCount == 2)
					{
						//We have gage connected. Let us read the value and expose it....
						//double MeasuredValue, tempZeroPos, TempPrevVal;

						//int result = IGS_Get_MeasuredValue(Gage_Info[0].GageID, GAGE_MEASUREDDATA, &MeasuredValue);
						//if (result == 0)
						//{
						//	if (DROInstance->AxisPropCollection[axisCount + 1].MachineStartflag)
						//	{
						//		DROInstance->AxisPropCollection[axisCount + 1].MachineStartflag = false;
						//		DROInstance->GageZeroPosition = MeasuredValue;
						//		DROInstance->PrevGagePosition = 0;
						//	}
						//	TempPrevVal = DROInstance->PrevGagePosition;
						//	tempZeroPos = DROInstance->GageZeroPosition;
						//	DROInstance->PrevGagePosition = DROInstance->GagePosition;
						//	DROInstance->GagePosition = (MeasuredValue - DROInstance->GageZeroPosition) / 1000;
						//	CurrentDROvalue[axisCount + 1] = DROInstance->GagePosition;
						//	DROInstance->AxisPropCollection[axisCount + 1].CurrentPosition = DROInstance->GagePosition;
						//	if (abs(DROInstance->GagePosition - DROInstance->PrevGagePosition) > 0.0002)
						//		DroChanged = true;
						//}
					}
				}
				if(DROInstance->NumberofMachineAxes == 3 && !DROInstance->GageConnected)
					DROInstance->AxisPropCollection[3].CurrentPosition = 0;
				//code to check current line number executing and function target reached logic.........
				int Line_Number = RBuffer.find("L");

				if(Line_Number != -1)
				{ 
					std::string HexValue = RBuffer.substr(Line_Number + 1, 4);
					std::reverse(HexValue.begin(), HexValue.end());
					DROInstance->LineNumberExecuting = DROInstance->hex_int(HexValue);
					if (DROInstance->LineNumberExecuting > 100) continue;
					int lnExec = DROInstance->LineNumberExecuting;
					int lastLine = DROInstance->LineNumberExecuting;

					if(!DROInstance->LineNumberResetFlag)
					{
						if (DROInstance->LineNumberExecuting != DROInstance->LastLineExecuting && DROInstance->CheckForLimihit) //  lineNumberChangedCounter > 5
						{
							lineNumberChangedCounter++;
							if (lineNumberChangedCounter > 0) // DROInstance->LastLineExecuting != 0)
							{
								DROInstance->LastLineExecuting = DROInstance->LineNumberExecuting;
								if (DROInstance->LineNumberExecuting == 0)
								{
									//condition to check for target reached .....for probe position and goto position....
									if (DROInstance->CheckForTargetReached && DROInstance->CheckPartProgramRunningStatus() && !DROInstance->SmoothMovementAtProbePos)
									{
										if ((abs(DROInstance->LastTarget[0] - CurrentDROvalue[0]) > DROInstance->Axis_Accuracy[0] * 10 ||
											abs(DROInstance->LastTarget[1] - CurrentDROvalue[1]) > DROInstance->Axis_Accuracy[1] * 10 ||
											abs(DROInstance->LastTarget[2] - CurrentDROvalue[2]) > DROInstance->Axis_Accuracy[2] * 10))
										{
											if (DROInstance->CheckForSecondTimeTarget == 0)
											{
												DROInstance->CheckForSecondTimeTarget++;
												DROInstance->WriteCommandSent("Line number change before target reached");
												DROInstance->DROTargetReachedCallback(7); // send last command again...
											}
											else if (DROInstance->CheckForSecondTimeTarget == 1)
											{
												DROInstance->CheckForSecondTimeTarget = 0;
												DROInstance->WriteCommandSent("Line number change second time before target reached");
												DROInstance->DROTargetReachedCallback(16); //show message  that please restart the software and try again...
											}
										}
										else
										{
											DROInstance->CheckForTargetReached = false;
											DROInstance->CheckForSecondTimeTarget = 0;
											DROInstance->DROTargetReachedCallback(0);
										}
									}
									else
									{
										DROInstance->CheckForTargetReached = false;
										DROInstance->CheckForSecondTimeTarget = 0;
										if (DROInstance->ProbeRetractPointMethod && DROInstance->Probe_HitFlag)
										{
											if (DROInstance->newCardFlag < 2)
											{
												DROInstance->FindProbeHitDirection(ValuesX, ValuesY, ValuesZ);
												//else
												//	DROInstance->FindProbeHitDirection
												DROInstance->DROTargetReachedCallback(12);
											}
										}
										DROInstance->DROTargetReachedCallback(0);
										DROInstance->Probe_HitFlag = false;
									}
								}
								else
								{
									for (int i = 0; i < DROInstance->LineNumberExecuting - DROInstance->LastLineExecuting; i++)
									{
										if (DROInstance->ProbeRetractPointMethod && DROInstance->Probe_HitFlag)
										{
											DROInstance->FindProbeHitDirection(ValuesX, ValuesY, ValuesZ);
											DROInstance->DROTargetReachedCallback(12);
										}
										DROInstance->DROTargetReachedCallback(0);
										DROInstance->Probe_HitFlag = false;
									}
								}
							}
						}
						else if(DROInstance->Check_RaxisTargetReached)
						{
							if((abs(DROInstance->LastTarget[0] - CurrentDROvalue[0]) < DROInstance->Axis_Accuracy[0] * 10 && 
								abs(DROInstance->LastTarget[1] - CurrentDROvalue[1]) < DROInstance->Axis_Accuracy[1] * 10 && 
								abs(DROInstance->LastTarget[3] - CurrentDROvalue[3]) < DROInstance->Axis_Accuracy[3]))
							{
								DROInstance->CTDroCoordinate[0] = CurrentDROvalue[0]; DROInstance->CTDroCoordinate[1] = CurrentDROvalue[1]; 
								DROInstance->CTDroCoordinate[2] = CurrentDROvalue[2];
								DROInstance->Check_RaxisTargetReached = false;
								DROInstance->DROTargetReachedCallback(17);
							}
							lineNumberChangedCounter = 0;
						}
						else
						{
							lineNumberChangedCounter = 0;
							//DROInstance->LastLineExecuting = DROInstance->LineNumberExecuting;				
						}
					}
					else
					{
						if(DROInstance->LineNumberExecuting == 0)
						{
							DROInstance->LastLineExecuting = 0;
							DROInstance->LineNumberResetFlag = false;
							lineNumberChangedCounter = 0;
						}
					}
					//Logic to handle limit hit.......
					static int resendcount = 0;
					if (DROInstance->newCardFlag < 2 && DROInstance->LastLineExecuting != 0 && DROInstance->CheckForLimihit && DROInstance->CanRaiseMachineStuckMessage)
					{					
						bool LimitHit = false;
						if(abs(PreviousDROValue[0] - CurrentDROvalue[0]) < CurrentCompvalue[0] * 10 &&
							abs(PreviousDROValue[1] - CurrentDROvalue[1]) < CurrentCompvalue[1] * 10 &&
							abs(PreviousDROValue[2] - CurrentDROvalue[2]) < CurrentCompvalue[2] * 10)
						{
							CycleCounter++;
						}
						else
						{
							for(int i = 0; i < 3; i++)
							{
								PreviousDROValue[i] = CurrentDROvalue[i];
							}
								CycleCounter = 0;
						}

						if(abs(PreviousDROValue[3] - CurrentDROvalue[3]) < CurrentCompvalue[3] * 10)
					    	RCounter++;
						else
						{
							PreviousDROValue[3] = CurrentDROvalue[3];
							RCounter = 0;
						}

						if(CycleCounter >= DROInstance->LimitHit_DroCycleCount && RCounter > DROInstance->LimitHit_DroCycleCount)
						{
							LimitHit = true;
						}
						if(LimitHit)
						{
							CycleCounter = 0;
							RCounter = 0;
							TargetReachedCount = 0;
							for(int i = 0; i < 3; i++)
							{
								PreviousDROValue[i] = -1000;
							}
							if(DROInstance->HomePositionMode)
							{
								DROInstance->DROTargetReachedCallback(13);
								DROInstance->WriteCommandSent("Home Position Reached");
								// This Condition is used for Homing....
								resendcount = 0;
							}
							else
							{
								if(resendcount < 3)
								{
									resendcount++;
								}
								else if(resendcount == 3)
								{
									//raise message machine seems to be stuck....
									DROInstance->WriteCommandSent("Machine Seems to be Stuck or could not get probe point");
									DROInstance->DROTargetReachedCallback(13);
									DROInstance->CanRaiseMachineStuckMessage = false;
									char cd[4] = "\0";
									_itoa(DROInstance->LastLineExecuting, cd, 10);
									DROInstance->WriteCommandSent(cd);
									_itoa(DROInstance->LineNumberExecuting, cd, 10);
									DROInstance->WriteCommandSent(cd);
								}
							}
						}
					}
					else
					{
						resendcount = 0;
						//CycleCounter = 0;
						RCounter = 0;
						for(int i = 0; i < 3; i++)
						{
							PreviousDROValue[i] = -1000;
						}
					}
				}
				//Save the last 20 values to Check the direction during probe hit..! 
				if(DroChanged)
				{				
					for(int i = 0; i < 4; i++)
						DROInstance->DROCoordinateValue[i] = CurrentDROvalue[i];
					DROInstance->DROTargetReachedCallback(10);						
					if(ValuesX.size() > SaveValueCnt)
					{
						ValuesX.pop_back();
						ValuesY.pop_back();
						ValuesZ.pop_back();
					}
					ValuesX.push_front(CurrentDROvalue[0]);
					ValuesY.push_front(CurrentDROvalue[1]);
					ValuesZ.push_front(CurrentDROvalue[2]);
				}

				if(RaxisValueChanged)
				{
					DROInstance->DROCoordinateValue[3] = CurrentDROvalue[3];
					DROInstance->DROTargetReachedCallback(11);
				}
				DroChanged = false;
				static int resendcount = 0;
				//Looking for Additional Symbols like Target Reached and Other...
				if(DROInstance->newCardFlag == 0)
				{
					if(DROInstance->LookTargetReachedSym)
					{
						int SymIndex = RBuffer.find('&');
						if(SymIndex >= 0)
							TargetReachedCount++;
						else TargetReachedCount = 0;
						if(TargetReachedCount == 20)
						{						
							resendcount = 0;
							TargetReachedCount = 0;
							DROInstance->LookTargetReachedSym = false;							
							DROInstance->DROTargetReachedCallback(0);						
						}
						else
						{
							//worst case ..if command is beyond the mahcine..i.e.limit switch hit
							//Used in homing mode also..
							bool LimitHit = false;

							if(abs(PreviousDROValue[0] - CurrentDROvalue[0]) < CurrentCompvalue[0] * 10 &&
								abs(PreviousDROValue[1] - CurrentDROvalue[1]) < CurrentCompvalue[1] * 10 &&
								abs(PreviousDROValue[2] - CurrentDROvalue[2]) < CurrentCompvalue[2] * 10)
							{
								CycleCounter++;
							}
							else
							{
								for(int i = 0; i < 3; i++)
								{
									PreviousDROValue[i] = CurrentDROvalue[i];
								}
								   CycleCounter = 0;
							}

							if(abs(PreviousDROValue[3] - CurrentDROvalue[3]) < CurrentCompvalue[3])
					    		RCounter++;
							else
							{
								PreviousDROValue[3] = CurrentDROvalue[3];
								RCounter = 0;
							}

							if(CycleCounter >= DROInstance->LimitHit_DroCycleCount && RCounter > DROInstance->LimitHit_DroCycleCount)
							{
								LimitHit = true;
							}
							if(LimitHit && DROInstance->CanRaiseMachineStuckMessage)
							{
								CycleCounter = 0;
								RCounter = 0;
								TargetReachedCount = 0;
								for(int i = 0; i < 3; i++)
								{
									PreviousDROValue[i] = -1000;
								}
								if(DROInstance->HomePositionMode)
								{
									// This Condition is used for Homing....
									DROInstance->DROTargetReachedCallback(13);
									DROInstance->WriteCommandSent("Home Position Reached");
									resendcount = 0;
								}
								else
								{
									if(resendcount < 3)
									{
										// we shouldn't send goto command again because electronics is not going to handle this command....
										//DROInstance->DROTargetReachedCallback(7);
										resendcount++;
									}
									else if(resendcount == 3)
									{
										//raise message machine seems to be stuck....
										DROInstance->WriteCommandSent("Machine Seems to be Stuck");
										DROInstance->DROTargetReachedCallback(13);
										DROInstance->CanRaiseMachineStuckMessage = false;
									}
								}
							}	
						}
					}
					else
					{
						resendcount = 0;
						CycleCounter = 0;
						RCounter = 0;
						for(int i = 0; i < 3; i++)
						{
							PreviousDROValue[i] = -1000;
						}
					}
				}
			}
			catch(...)
			{
				DROInstance->SetAndRaiseErrorMessage("HCDLL0047", __FILE__, __FUNCSIG__); 
				DROInstance->MachineConnetedStatus = false;
				DROInstance->DROTargetReachedCallback(6);
				_endthread();
			}
		}
		_endthread();
	}
	catch(...)
	{
			DROInstance->SetAndRaiseErrorMessage("HCDLL0048", __FILE__, __FUNCSIG__); 
			DROInstance->MachineConnetedStatus = false;
			DROInstance->LineNumberResetFlag = false;
			DROInstance->DROTargetReachedCallback(6);
			_endthread();
	}
}

void WriteCommandInDiffthread(void* CncCommandList)
{
	try
	{	
		std::list<std::string>* CommandList = (std::list<std::string>*)CncCommandList;
		for each(std::string command in *CommandList)
		{
			DROInstance->sendMachineCommand_One((char*)command.c_str(), true);
		}

		DROInstance->sendMachineCommand_One("61", true);
		CommandList->clear();
		delete CommandList;
		_endthread();
	}
	catch(...){ DROInstance->SetAndRaiseErrorMessage("HCDLL0049", __FILE__, __FUNCSIG__); _endthread();}
}

void WriteCommandInDiffthread(char* command)
{
	DROInstance->sendMachineCommand_One(command, true);
	_endthread();
}


//Writing Command in txt file That Commands are given by User..........
void DigitalReadOut::WriteCommandSent(std::string data)
{
	try
	{
		CommandSendLog((char*)data.c_str());
	}
	catch(...){ DROInstance->SetAndRaiseErrorMessage("HCDLL0050", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::SetDroCycleCount(int Count)
{
	DROInstance->LimitHit_DroCycleCount = Count;
}

void DigitalReadOut::SetMachineDisconnectCount(int Count)
{
	DROInstance->MachineDisconnect_CycleCount = Count;
}

void DigitalReadOut::SetProbeOnward_RetractMethod(bool CurrentStatus)
{
	DROInstance->ProbeRetractPointMethod = CurrentStatus;
}

void DigitalReadOut::FindProbeHitDirection(list<double> ValuesX, list<double> ValuesY, list<double> ValuesZ)
{
	if (DROInstance->newCardFlag == 2) return;
	try
	{
		//DROInstance->WritedebugInfo(DROInstance->ProbeCoordinate);
		//DROInstance->WritedebugInfo(DROInstance->DROCoordinateValue);
		if (ValuesX.size() < 10 || ValuesY.size() < 10 || ValuesZ.size() < 10) return;
		DROInstance->ProbeHitRaiseFlag = true;
		int CompareCnt = 14; //If the movement dir count(+/-) is greater than this in any axis then consider that direction//
		double ProbeVPrecision = 0.001; //0.6 um.. //precision for direction calculation
		//int XaxisPCnt = 0, XaxisNCnt = 0, YaxisPCnt = 0, YaxisNCnt = 0, ZaxisPCnt = 0, ZaxisNCnt = 0;
		int X_Ct = 0, Y_Ct = 0, Z_Ct = 0; // , YaxisNCnt = 0, ZaxisPCnt = 0, ZaxisNCnt = 0;
		double Cdvalue = 0;
		double PrX = ValuesX.front(); // DROInstance->AxisPropCollection[0].CurrentProbePosition;
		double PrY = ValuesY.front(); // DROInstance->AxisPropCollection[1].CurrentProbePosition;
		double PrZ = ValuesZ.front(); // DROInstance->AxisPropCollection[2].CurrentProbePosition;
		for each(double Px in ValuesX)
		{
			Cdvalue = Px - PrX;
			if(abs(Cdvalue) > ProbeVPrecision)
			{
				if(Cdvalue > 0)
					X_Ct++; // readingThread->ProbeDirection = readingThread->PROBEDIR::XRIGHT2LEFT;
				else if(Cdvalue < 0)
					X_Ct--; // readingThread->ProbeDirection = readingThread->PROBEDIR::XLEFT2RIGHT;
			}
			PrX = Px;
		}
		for each(double Py in ValuesY)
		{
			Cdvalue = Py - PrY;
			if(abs(Cdvalue) > ProbeVPrecision)
			{
				if(Cdvalue > 0)
					Y_Ct++; // readingThread->ProbeDirection = readingThread->PROBEDIR::YTOP2BOTTOM;
				else if(Cdvalue < 0)
					Y_Ct--; // readingThread->ProbeDirection = readingThread->PROBEDIR::YBOTTOM2TOP;
			}
			PrY = Py;			
		}
		for each(double Pz in ValuesZ)
		{
			Cdvalue = Pz - PrZ;
			if(abs(Cdvalue) > ProbeVPrecision)
			{
				if(Cdvalue > 0)
					Z_Ct++; // readingThread->ProbeDirection = readingThread->PROBEDIR::ZTOP2BOTTOM;
				else if(Cdvalue < 0)
					Z_Ct--; // readingThread->ProbeDirection = readingThread->PROBEDIR::ZBOTTOM2TOP;
			}		
			PrZ = Pz;
		}
		DROInstance->Probe_Hit_Direction = 9; //current probe hit direction..

		//Now check which axis is the largest in change.. 
		int Axis_Ct[3] = {abs(X_Ct), abs(Y_Ct), abs(Z_Ct)};
		int max = Axis_Ct[0], maxIndex = 0;
		for (int i = 1; i < 3; i++)
		{
			if (Axis_Ct[i] > max)
			{
				max = Axis_Ct[i]; maxIndex = i;	
			}
		}
		if (max < CompareCnt) 
			DROInstance->ProbeHitRaiseFlag = false;
		else
		{
			switch (maxIndex)
			{
			case 0: // X Axis is the one that is hit
				if (X_Ct > 0)
					DROInstance->Probe_Hit_Direction = 0;
				else
					DROInstance->Probe_Hit_Direction = 1;
				break;
			case 1: // Y-Axis
				if (Y_Ct > 0)
					DROInstance->Probe_Hit_Direction = 2;
				else
					DROInstance->Probe_Hit_Direction = 3;
				break;
			case 2: // Z-Axis
				if (Z_Ct > 0)
					DROInstance->Probe_Hit_Direction = 4;
				else
					DROInstance->Probe_Hit_Direction = 5;
				break;
			}
		}
		//if(XaxisPCnt > CompareCnt) DROInstance->Probe_Hit_Direction = 0; // X-axis right to left//
		//else if(XaxisNCnt > CompareCnt) DROInstance->Probe_Hit_Direction = 1; // X-axis left to right//
		//else if(YaxisPCnt > CompareCnt) DROInstance->Probe_Hit_Direction = 2; // Y-axis top to bottom//
		//else if(YaxisNCnt > CompareCnt) DROInstance->Probe_Hit_Direction = 3; // Y-axis bottom to top//
		//else if(ZaxisPCnt > CompareCnt) DROInstance->Probe_Hit_Direction = 4; // Z-axis moving down//
		//else if(ZaxisNCnt > CompareCnt) DROInstance->Probe_Hit_Direction = 5; // Z-axis moving Up//
		//else DROInstance->ProbeHitRaiseFlag = false; //If the direction is in both then leave that.. User is very bad in Operating a simple machine!!
		if(!DROInstance->ProbeHitRaiseFlag)
			DROInstance->WriteCommandSent("Probe Hit..The movement direction was improper. On both axis /Movement was very very slow.. ");
		else
			DROInstance->WriteCommandSent("Probe Hit.");
		//if(DROInstance->ProbeRetractPointMethod)
		//{
		//	switch(DROInstance->Probe_Hit_Direction)
		//	{
		//	case 0:
		//		DROInstance->Probe_Hit_Direction = 1;
		//		break;
		//	case 1:
		//		DROInstance->Probe_Hit_Direction = 0;
		//		break;
		//	case 2:
		//		DROInstance->Probe_Hit_Direction = 3;
		//		break;
		//	case 3:
		//		DROInstance->Probe_Hit_Direction = 2;
		//		break;
		//	case 4:
		//		DROInstance->Probe_Hit_Direction = 5;
		//		break;
		//	case 5:
		//		DROInstance->Probe_Hit_Direction = 4;
		//		break;
		//	}
		//}
	}
	catch(...){ DROInstance->SetAndRaiseErrorMessage("HCDLL0051", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::FindProbeHitDirection(int pDir_FromController)
{
	try
	{
		int tempDir = pDir_FromController - 1;
		//if (pDir_FromController == 1)
		//	MessageBox(NULL, L"Positive X", L"Rapid-I", MB_TOPMOST);
		//else if (pDir_FromController == 2)
		//	MessageBox(NULL, L"Negative X", L"Rapid-I", MB_TOPMOST);
		//else if (pDir_FromController == 3)
		//	MessageBox(NULL, L"Positive Y", L"Rapid-I", MB_TOPMOST);
		//else if (pDir_FromController == 4)
		//	MessageBox(NULL, L"Negative Y", L"Rapid-I", MB_TOPMOST);
		//else if (pDir_FromController == 0)
		//	MessageBox(NULL, L"Got 0", L"Rapid-I", MB_TOPMOST);

		for (int ii = 0; ii < 4; ii++)
		{
			if ((int)(tempDir / 2) == ii)
			{
				if (AxisPropCollection[ii].MoveOppositeToVision && !AxisPropCollection[ii].MovementDirection)
				{
					if (tempDir > 2 * ii)
						tempDir = 2 * ii;
					else //if (tempDir == 3)
						tempDir = 2 * ii + 1;
				}
			}
		}
		this->Probe_Hit_Direction = tempDir;
		
		//if (tempDir == 1)
		//	MessageBox(NULL, L"Stored Positive X", L"Rapid-I", MB_TOPMOST);
		//else if (tempDir == 2)
		//	MessageBox(NULL, L"Stored Negative X", L"Rapid-I", MB_TOPMOST);
		//else if (tempDir == 3)
		//	MessageBox(NULL, L"Stored Positive Y", L"Rapid-I", MB_TOPMOST);
		//else if (tempDir == 4)
		//	MessageBox(NULL, L"Stored Negative Y", L"Rapid-I", MB_TOPMOST);
		//else if (tempDir == 0)
		//	MessageBox(NULL, L"Stored 0", L"Rapid-I", MB_TOPMOST);

		//for (int i = 0; i < 4; i++)
		//{
		//	if (tempDir < (i + 1) * 2)
		//	{
		//		if (AxisPropCollection[i].MovementDirection)
		//			this->Probe_Hit_Direction = tempDir;
		//		else
		//		if (tempDir % 2 == 0)
		//			this->Probe_Hit_Direction = tempDir + 1;
		//		else
		//			this->Probe_Hit_Direction = tempDir - 1;

		//		break;
		//	}
		//}
	}
	catch (...){ DROInstance->SetAndRaiseErrorMessage("HCDLL0056", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::WritedebugInfo(std::string Info)
{
	try
	{
		std::string filePath = "D:\\DebugInfo.txt";
		std::wofstream InfoWriter;
		InfoWriter.open(filePath.c_str(), std::ios_base::app);
		InfoWriter << (const char*)Info.c_str() << endl;
		//InfoWriter << "-------------" << endl;
		InfoWriter.close();
	}
	catch(...){ DROInstance->SetAndRaiseErrorMessage("HCDLL0052", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::SetMachineStuckFlag()
{
	try
	{
		this->CanRaiseMachineStuckMessage = true;
	}
	catch(...){ DROInstance->SetAndRaiseErrorMessage("HCDLL0052", __FILE__, __FUNCSIG__); }
}

std::string DigitalReadOut::GetStringForCRC(std::string OriginalStr, std::string FindStr, int Length)
{
	std::string ReturnStr = "";
	try
	{
		bool StrPresent = true;
		while(StrPresent)
		{
			int Cnt = OriginalStr.find(FindStr);
			if(Cnt != -1 && OriginalStr.length() > Cnt + Length)
			{
				ReturnStr += OriginalStr.substr(Cnt + 1, Length);
				OriginalStr = OriginalStr.substr(Cnt + 1, OriginalStr.length() - Cnt - 1);
			}
			else
				StrPresent = false;
		}
		return ReturnStr;
	}
	catch(...){ return ReturnStr; DROInstance->SetAndRaiseErrorMessage("HCDLL0053", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::SetProbePositionSmoothMovement(bool flag)
{
	SmoothMovementAtProbePos = flag;
}

void DigitalReadOut::ProbeRetractAlongDirectionOfMovement(bool flag)
{
	try
	{
		if(flag)
			sendMachineCommand_One("C3", true);
		else
			sendMachineCommand_One("C4", true);
	}
	catch(...){ DROInstance->SetAndRaiseErrorMessage("HCDLL0054", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::WritedebugInfo(double *TargetValue)
{
	try
	{
		std::string TargetWriteString = "";
		for(int i = 0; i < 3; i++)
		{
			char test[12] = "\0";
			int TmpValue = TargetValue[i] * 1000;
			_itoa(TmpValue, test, 10);
			std::string TargetString = (const char*)(test);
			TargetWriteString += TargetString + ",";
		}
		WriteCommandSent(TargetWriteString);
	}
	catch(...){ DROInstance->SetAndRaiseErrorMessage("HCDLL0055", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::Activate_Deactivate_Probe(bool X_Axis, bool Y_Axis, bool Z_Axis, bool R_Axis, bool DonotSendCommand)
{
	try
	{
		ProbeActivated_X = X_Axis;
		ProbeActivated_Y = Y_Axis;
		ProbeActivated_Z = Z_Axis;
		ProbeActivated_R = R_Axis;
		if(!DonotSendCommand)
		{
			std::list<std::string> CommandListStr;
			if(X_Axis)
				CommandListStr.push_back("E1000F001");
			else
				CommandListStr.push_back("E0000F001");
			if(Y_Axis)
				CommandListStr.push_back("E1000F011");
			else
				CommandListStr.push_back("E0000F011");
			if(Z_Axis)
				CommandListStr.push_back("E1000F021");
			else
				CommandListStr.push_back("E0000F021");
			if(R_Axis)
				CommandListStr.push_back("E1000F031");
			else
				CommandListStr.push_back("E0000F031");
			DROInstance->sendMachineCommand_Multi(CommandListStr, true);
		}
	}
	catch(...){ DROInstance->SetAndRaiseErrorMessage("HCDLL0055", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::Swivel_A_Axis()
{
	try
	{
		//if (!this->CNCMode)
		//{
		//	this->Probe_HitFlag = false;
		//	this->CanRaiseMachineStuckMessage = true;
		//	this->CheckForLimihit = true;
		//	if (this->newCardFlag < 2) this->CNCMode = true;
		list<std::string> CncCommandList;
		CncCommandList.push_back("210");
		CncCommandList.push_back("10");
		sendMachineCommand_Multi(CncCommandList, true);
		//}
		double Target[6] = { 0 };
		for (int i = 0; i < NumberofMachineAxes; i++)
			Target[i] = AxisPropCollection[i].CurrentPosition;

		if (A_Axis_isHorizontal)
			Target[4] = -1000;
		else
			Target[4] = 1000;
		//A_Axis_isHorizontal = !A_Axis_isHorizontal;

		std::ostringstream ss;
		ss << "Target : ";
		for (int i = 0; i < 4; i++)
		{
			ss << Target[i];
			ss << ", ";
		}
		std::string tst = ss.str();
		tst = tst.substr(0, tst.length() - 2);
		WriteCommandSent(tst);

		CheckForTargetReached = false;
		CncCommandList.clear();
		//list<std::string> CncCommandList;
		char axTarget[10]; //assumed that maximum length of char used
		std::string tempCalculate, AxisTarget;
		double Dist, delay = 0;
		int NOofaxis = NumberofMachineAxes;
		double Feedrate[5] = { 0,0,0,0,1.2 };
		double TargetPos[5] = { Target[0], Target[1], Target[2], Target[3] };

		//Goto Position ... Xg Yg Zg Rg
		double currPos, GotoPos;
		for (int i = 0; i < 4; i++)
		{
			if (AxisPropCollection[i].MovementType)
				Target[i] *= AxisPropCollection[i].CountsPerUnit / AxisPropCollection[i].MultipleFactor;
			else
			{
				Target[i] += AxisPropCollection[i].MultipleFactor  * sin(Target[i]);
				Target[i] *= AxisPropCollection[i].CountsPerUnit / (2 * M_PI);
			}
			(AxisPropCollection[i].MovementDirection) ? GotoPos = Target[i] : GotoPos = -Target[i];
			//(GotoHomePositionMode) ? currPos = 1048576 : currPos = AxisPropCollection[i].McsZero;
			currPos = AxisPropCollection[i].McsZero;
			Dist = currPos + GotoPos;
			_ltoa(long(Dist), axTarget, 16);
			tempCalculate = axTarget;
			for (int j = tempCalculate.length(); j < AxisPropCollection[i].AxisdataLength; j++)
				tempCalculate = "0" + tempCalculate;
			std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
		}

		//Give target for swivel axis
		Target[4] *= AxisPropCollection[0].CountsPerUnit / AxisPropCollection[0].MultipleFactor;
		currPos = AxisPropCollection[0].McsZero;
		Dist = currPos + Target[4];
		_ltoa(long(Dist), axTarget, 16);
		tempCalculate = axTarget;
		for (int j = tempCalculate.length(); j < AxisPropCollection[0].AxisdataLength; j++)
			tempCalculate = "0" + tempCalculate;
		std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);

		// we have 6-axis card with encoder only in 4 axes, we need to append the two dummy targer positions for 6th Axis. 
		std::transform(ExtraAxesValues[1].begin(), ExtraAxesValues[1].end(), std::back_inserter(AxisTarget), toupper);

		for (int i = 0; i < 5; i++)
		{
			//if (Feedrate[i] > 20) Feedrate[i] = 20; // EnteredFRate[i];
			int ff = Feedrate[i] * 10;
			ff = abs(ff);
			if (ff > 254) ff = 254;
			_ltoa(ff, axTarget, 16);
			tempCalculate = axTarget;
			for (int j = tempCalculate.length(); j < 2; j++)
				tempCalculate = "0" + tempCalculate;
			if (tempCalculate == "00") tempCalculate = "01";
			std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
		}
		if (SixAxisD33CardisPresent)
		{
			std::string tFR = "00";
			std::transform(tFR.begin(), tFR.end(), std::back_inserter(AxisTarget), toupper);
		}

		//Adding Delay....D
		//First add the line number
		_ltoa(DROInstance->LastLineExecuting + 1, axTarget, 16);
		tempCalculate = axTarget;
		for (int j = tempCalculate.length(); j < 4; j++)
			tempCalculate = "0" + tempCalculate;
		//Add interpolation. Currently nothing is implemented so we send 0
		tempCalculate += "0";
		std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
		CheckForTargetReached = true;
		tempCalculate = "0";
		tempCalculate = tempCalculate.substr(0, 1);

		//Concatenate and get complete string ready for sending now.
		std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);

		//Final string .. goto Xg Yg Zg Rg  + Xs Ys Zs Rs + D.. Total 34 or 35 Bytes of data..
		//Reverse the complete string and add 5 to start and then send
		std::reverse(AxisTarget.begin(), AxisTarget.end());
		AxisTarget = "4" + AxisTarget;
		CncCommandList.push_back((char*)AxisTarget.c_str());
		CncCommandList.push_back("61");
		sendMachineCommand_Multi(CncCommandList, true);
	}
	catch (...) { SetAndRaiseErrorMessage("HCDLL0051", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::ControlPins(int PinNumber, bool ON_State)
{
	try
	{
		switch (PinNumber)
		{
		case 1://Pin RC9 
			if (ON_State) 
				sendMachineCommand_One("CC", true);
			else
				sendMachineCommand_One("CD", true);
			break;
		case 2:
			if (ON_State)
				sendMachineCommand_One("CE", true);
			else
				sendMachineCommand_One("CF", true);
			break;
		default:
			break;
		}
		sendMachineCommand_One("C3", true);
	}
	catch (...)
	{

	}
}

void DigitalReadOut::SetOneShotLightParams(double pSlope, int pBaseValue, double sSlope, int sBaseValue)
{
	char AxisCnt[2]; // = "\0";
	char axTarget[4]; // = "\0";
	char tempS[16] = "\0";
	std::string CommandStr, tempCalculate, AxisTarget;
	CommandStr = "\0";
	int params[4] = { (int)(100.0 * pSlope), pBaseValue, (int)(100.0 * sSlope), sBaseValue };
	for (int j = 0; j < 4; j++)
	{
		_itoa(params[j], axTarget, 16);
		tempCalculate = axTarget;
		for (int j = tempCalculate.length(); j < 4; j++)
			tempCalculate = "0" + tempCalculate;
		CommandStr = CommandStr + tempCalculate;
	}
	for (int j = 0; j < CommandStr.length(); j++)
		tempS[j] = toupper(CommandStr.substr(j, 1).front());

	AxisTarget = (const char*)tempS;
	AxisTarget = AxisTarget.substr(0, 16);
	std::reverse(AxisTarget.begin(), AxisTarget.end());
	AxisTarget = "5" + AxisTarget;
	sendMachineCommand_One((char*)AxisTarget.c_str(), true);
}

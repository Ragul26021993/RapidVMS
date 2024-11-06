#pragma once
#include "stdafx.h"
#include "HardwareCommunication.h"
#include <tchar.h>
#include <process.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>



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
		newCardFlag = false;
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
		MachineDisconnect_CycleCount = 500;
		std::string Cpathtemp = (const char*)Commandfpath;
		CommnadSendFilePath = Cpathtemp;
		wcscpy_s(_ComName, Strlength, portname);
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
	if(DROErrorCallback != NULL)
		DROErrorCallback((char*)ecode.c_str(), (char*)filename.c_str(), (char*)fctnname.c_str());
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

void DigitalReadOut::GetCardVersionNumber()
{
	try
	{
		DROInstance->VersionControlflag = false;
		if(sendMachineCommand_One("C2", true))
		{
			if(sendMachineCommand_One("C1", true))
			{
				this->StopCommunication = true;
				DROInstance->DROTargetReachedCallback(15);
			}
		}		
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0005", __FILE__, __FUNCSIG__); }
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
void DigitalReadOut::ConnectToHardware(bool ReconnectTohardware)
{
	try
	{
		if(ReconnectTohardware)
		{
			if(MachineConnetedStatus) return;
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
			GetCommState(PortHandle, &PortConfigs);
			PortConfigs.BaudRate = 460800;	//baud rate of 460800.
			PortConfigs.ByteSize = 8;	//character of 8bit wide.
			SetCommState(PortHandle, &PortConfigs);	//set the configurations.

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
			DROInstance->beginThread();
		}
	}
	catch(...){ DROInstance->SetAndRaiseErrorMessage("HCDLL006", __FILE__, __FUNCSIG__); }
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
void DigitalReadOut::setDROProperties(bool* mt, bool* md, double* lv, double* mf, double* cpu)
{
	try
	{
		for(int i = 0; i < NumberOfAxis; i++)
		{
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

void DigitalReadOut::SetMachineCardFlag(bool flag)
{
	this->newCardFlag = flag;
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
		this->CNCMode = true;
		list<std::string> CncCommandList;	
		CncCommandList.push_back("2F");
		CncCommandList.push_back("10");
		CncCommandList.push_back("81");
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
		this->CNCMode = false;
		list<std::string> CncCommandList;	
		CncCommandList.push_back("10");
		CncCommandList.push_back("81");
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
		this->CNCMode = false;
		this->LookTargetReachedSym = false;
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
		bool X_Axis = ProbeActivated_X;
		bool Y_Axis = ProbeActivated_Y;
		bool Z_Axis = ProbeActivated_Z;
		bool R_Axis = ProbeActivated_R;
		if(this->newCardFlag)
			this->Activate_Deactivate_Probe(false, false, false, false, false); 
		sendMachineCommand_One("30", true);
		if(this->newCardFlag)
			this->Activate_Deactivate_Probe(X_Axis, Y_Axis, Z_Axis, R_Axis, false); 
		Sleep(50);
		for(int i = 0; i < NumberOfAxis; i++)
			AxisPropCollection[i].MachineStartflag = true;
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0020", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::ToReset(double* ResetValue)
{
	try
	{
		DROInstance->WriteVersionNumber(ResetValue, 1);
		bool X_Axis = ProbeActivated_X;
		bool Y_Axis = ProbeActivated_Y;
		bool Z_Axis = ProbeActivated_Z;
		bool R_Axis = ProbeActivated_R;
		this->Activate_Deactivate_Probe(false, false, false, false, false); 
		sendMachineCommand_One("30", true);
		this->Activate_Deactivate_Probe(X_Axis, Y_Axis, Z_Axis, R_Axis, false); 
		Sleep(50);
		double PosSet[4] = {0};
		for(int i = 0; i < NumberofMachineAxes; i++)
		{
			if(AxisPropCollection[i].MovementType)
				PosSet[i] = (ResetValue[i] - AxisPropCollection[i].CurrentPosition) * AxisPropCollection[i].CountsPerUnit / AxisPropCollection[i].MultipleFactor;
			else
				PosSet[i] = (ResetValue[i] - AxisPropCollection[i].CurrentPosition) * AxisPropCollection[i].CountsPerUnit * AxisPropCollection[i].MultipleFactor / (2 * M_PI);
		}
		for(int i = 0; i < NumberofMachineAxes; i++)
		{
			if(AxisPropCollection[i].MovementDirection)
				AxisPropCollection[i].McsZero -= PosSet[i];
			else
				AxisPropCollection[i].McsZero += PosSet[i];
		}
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0021", __FILE__, __FUNCSIG__); }
}

void DigitalReadOut::ToResetLineNumber()
{
	try
	{
		if(sendMachineCommand_One("60", true))
			this->LineNumberResetFlag = true;
		
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
		if(!DROInstance->newCardFlag)
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
		CncCommandList.push_back("81");
		sendMachineCommand_Multi(CncCommandList, true);
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0025", __FILE__, __FUNCSIG__); }
}

//To unlock desired axis
void DigitalReadOut::AxisUnlock(int AxisIndx)
{
	try
	{
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
		CncCommandList.push_back("81");
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
		this->CNCMode = false;
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
		this->CNCMode = false;
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
			Dist = Nvalue + 1048576;
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
		Dist = 1048576 + SweepAngle * 1000; 
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
						TargetPos[i] *= AxisPropCollection[i].CountsPerUnit * AxisPropCollection[i].MultipleFactor / (2 * M_PI);
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
					Dist = Nvalue + 1048576;
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
				Dist = 1048576 + SweepAngle * 1000; 
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
					TargetPos[i] *= AxisPropCollection[i].CountsPerUnit * AxisPropCollection[i].MultipleFactor / (2 * M_PI);
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
		if(!this->CNCMode)
		{
			WriteCommandSent("Goto Command called without CNC Mode");
			return;
		}
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

		if(DROInstance->notFocusing && !newCardFlag)
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
				Target[i] *= AxisPropCollection[i].CountsPerUnit * AxisPropCollection[i].MultipleFactor / (2 * M_PI);
			(AxisPropCollection[i].MovementDirection) ? GotoPos = Target[i] : GotoPos = -Target[i];
			(GotoHomePositionMode) ? currPos = 1048576 : currPos = AxisPropCollection[i].McsZero;
			Dist = currPos + GotoPos; 
			_ltoa(long(Dist), axTarget, 16);
			tempCalculate = axTarget;
			for(int j = tempCalculate.length(); j < AxisPropCollection[i].AxisdataLength; j++)
				tempCalculate = "0"+ tempCalculate;
			std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);
		}
		if(NOofaxis < 4)
			AxisTarget += "100000";
		/*if(NOofaxis < 4)
			std::transform(tempCalculate.begin(), tempCalculate.end(), std::back_inserter(AxisTarget), toupper);*/

		//Adding Feedrate for Axis...Xs Ys Zs Rs
		for(int i = 0; i < NOofaxis; i++)
		{
			if(Feedrate[i] < 0.1)
			{
				long NewFeedrate = 200 + 0.2 / Feedrate[i];
				if(NewFeedrate > 255)
					NewFeedrate = 255;
				_ltoa(NewFeedrate, axTarget, 16);
			}
			else
			{
				_ltoa(long(Feedrate[i] * 10), axTarget, 16);
			}
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
		if(newCardFlag)
		{
			if(probePoint)
			{
				CheckForTargetReached = false;
				char probdir[2] = "\0";
				_itoa(ProbDirection, probdir, 16);
				std::string TmpStr = probdir;
				AxisTarget += toupper(TmpStr.substr(0,1).front());
			}
			else
			{
				CheckForTargetReached = true;
				AxisTarget += "0";
			}
			CncCommandList.push_back("41");
		}
		else
			CncCommandList.push_back("41010");

		//Final string .. goto Xg Yg Zg Rg  + Xs Ys Zs Rs + D.. Total 34 or 35 Bytes of data..
		//Reverse the complete string and add 5 to start and then send
		std::reverse(AxisTarget.begin(), AxisTarget.end());

		AxisTarget = "5" + AxisTarget;
		CncCommandList.push_back((char*)AxisTarget.c_str());
		CncCommandList.push_back("61");
		sendMachineCommand_Multi(CncCommandList, true);
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
		for each(char name in WriteBuffer)
			WriteEachByte(name);
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
				WriteCommandSent("ACK is Negative, Seding Again!");
			else
				WriteCommandSent("Cannot Read Acknowledgement, Seding Again!");
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
					WriteCommandSent("Failed (Count is greater than 5): Light Command");
					ShowMessageBox("HWComm002", false, "");
					//MessageBox(NULL, L"Failed in Communication.. Please Switch off and Switch On the machine and Try again", L"Rapid-I", MB_TOPMOST);
					return false;
				}
				PurgeComm(PortHandle, PURGE_RXCLEAR);
				WriteCommandSent("ACK is Not Equal, Seding Again!");
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
		if(DROInstance->newCardFlag)
		{
			std::string LStr = (const char*)Lcommand;
			std::string FinalLightcommand = "9" + LStr;
			return sendLightCommand_NewProtocol(Lcommand, (char*)CalculateCRC(FinalLightcommand).c_str());
		}
		else
			return sendLightCommand_OldProtocol(Lcommand, CheckCommunication);	
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
		if(DROInstance->newCardFlag)
		{
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
				WriteCommandSent("ACK is Not Equal, Seding Again!");
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
		for each(char name in WriteBuffer)
			WriteEachByte(name);
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
				WriteCommandSent("Failed (Count is greater than 5): CNC Command");
				ShowMessageBox("HWComm002", false, "");
				//MessageBox(NULL, L"Failed in Communication.. Pelase Switch off and Switch On the machine and Try again", L"Rapid-I", MB_TOPMOST);
				return false;
			}
			if(DROInstance->PositiveAcknowledgementCnt == 1)
				WriteCommandSent("ACK is Negative, Seding Again!");
			else
				WriteCommandSent("Cannot Read Acknowledgement, Seding Again!");
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
				for each(char name in WriteBuffer)
					WriteEachByte(name);
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
						WriteCommandSent("Failed (Count is greater than 5): CNC Command");
						ShowMessageBox("HWComm002", false, "");
						return;
					}
					if(DROInstance->PositiveAcknowledgementCnt == 1)
						WriteCommandSent("ACK is Negative, Seding Again!");
					else
						WriteCommandSent("Cannot Read Acknowledgement, Seding Again!");
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
					WriteCommandSent("ACK is Not Equal, Seding Again!");
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
		if(DROInstance->newCardFlag)
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
		if(Delaybetweencommnads != 0)
			Sleep(Delaybetweencommnads);
	}
	catch(...){ SetAndRaiseErrorMessage("HCDLL0043", __FILE__, __FUNCSIG__); }
}

//Set Probe Revert back dist..
void DigitalReadOut::SetPorbeRevertbackDistance(double RevertDist, double RevertSpeed, double ApproachDist, double ApproachSpeed, double ProbeSensitivity, bool ProbeMoveUpFlag)
{
	try
	{
		if(!MachineConnetedStatus) return;
		std::string tempCalculate, tempCalculate1, AxisTarget;
		char axTarget[5] = "\0";
		char axTarget1[3] = "\0";
		char tempstr[18] = "\0";
		RevertDist *= AxisPropCollection[0].CountsPerUnit / AxisPropCollection[0].MultipleFactor;
		_ltoa(long(RevertDist), axTarget, 16);	
		tempCalculate = axTarget;
		if(tempCalculate.length() > 4)
			tempCalculate = tempCalculate.substr(0,4);
		for(int i = tempCalculate.length(); i < 4; i++)
			tempCalculate = "0"+ tempCalculate;
		RevertSpeed*= 10;
		_ltoa(long(RevertSpeed), axTarget1, 16);
		tempCalculate1 = axTarget1;
		if(tempCalculate1.length() > 2)
			tempCalculate1 = tempCalculate1.substr(0,2);
		for(int i = tempCalculate1.length(); i < 2; i++)
			tempCalculate1 = "0"+ tempCalculate1;
		tempCalculate += tempCalculate1;
		if(DROInstance->newCardFlag)
		{
			ApproachDist *= AxisPropCollection[0].CountsPerUnit / AxisPropCollection[0].MultipleFactor;
			_ltoa(long(ApproachDist), axTarget, 16);
			tempCalculate1 = axTarget;
			if(tempCalculate1.length() > 4)
				tempCalculate1 = tempCalculate1.substr(0,4);
			for(int i = tempCalculate1.length(); i < 4; i++)
				tempCalculate1 = "0"+ tempCalculate1;
			tempCalculate += tempCalculate1;
			ApproachSpeed*= 10;
			_ltoa(long(ApproachSpeed), axTarget1, 16);
			tempCalculate1 = axTarget1;
			if(tempCalculate1.length() > 2)
				tempCalculate1 = tempCalculate1.substr(0,2);
			for(int i = tempCalculate1.length(); i < 2; i++)
				tempCalculate1 = "0"+ tempCalculate1;
			tempCalculate += tempCalculate1;
			if(ProbeMoveUpFlag)
				tempCalculate += "1";
			else
				tempCalculate += "0";
			_ltoa(long(ProbeSensitivity), axTarget, 16);
			tempCalculate1 = axTarget;
			if(tempCalculate1.length() > 4)
				tempCalculate1 = tempCalculate1.substr(0,4);
			for(int i = tempCalculate1.length(); i < 4; i++)
				tempCalculate1 = "0"+ tempCalculate1;
			tempCalculate += tempCalculate1;		
			for (int i = 0; i < tempCalculate.length(); i++)
			{
				if (i == 18) break;
				tempstr[i] = toupper(tempCalculate.substr(i,1).front());
			}
			AxisTarget = (const char*)tempstr;
			std::reverse(AxisTarget.begin(), AxisTarget.end());
			AxisTarget = "A" + AxisTarget;
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
		if(CRCValue == 0)
			ReturnFlag = true;
		delete [] ByteArray;
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
		int CycleCounter = 0; //Counter used to check the limit switch hit.//
		double PreviousDROValue[4] = {-1000}; //store last dor value..used to check the limit switch hit//
		double Version_Number[5] = {1001};
		double mmCalculation = 0; //to hold the current axis mm value
		
		list<double> ValuesX, ValuesY, ValuesZ; //array to hold the 20 values..for probe hit direction
		bool DroChanged = false, CardStatusFlag = true;
		unsigned long bufferLength;
		char tempbuffer[10];
		std::string RBuffer, TempReadBuffer = "";		
		//DigitalReadOut* readingThread = (DigitalReadOut*)anything;
		int RCounter = 0;
		double CurrentCompvalue[4] = {0}; //for value change comparision.. according to the axis type set the value
		for(int i = 0; i < 4; i++)
		{
			if(DROInstance->AxisPropCollection[i].MovementType)
			{
				CurrentCompvalue[i] = 0.0005;
				if(i == 3) CurrentCompvalue[i] = 0.0000001;
			}
			else
				CurrentCompvalue[i] = 0.00006;
		}
		int NumberOfChar = 27;
		if(DROInstance->newCardFlag)
			NumberOfChar = 32;
		/*DWORD d1, d2, d3;
		d1 = GetTickCount();*/
		while(DROInstance->ReadDroValues)
		{
			try
			{
			//If writing command then dont do anything..You can't get any data from controller
				if(!DROInstance->newCardFlag)
				{
					if(DROInstance->WritingCommands) continue; 
				}
				//Read the values..
				bool ReadFlag = true, Xpresent = false;
				TempReadBuffer = "";
				while(ReadFlag)
				{
					ReadFile(DROInstance->PortHandle, tempbuffer, 1, &bufferLength, NULL);
					if(bufferLength == 0 || DROInstance->PortHandle == INVALID_HANDLE_VALUE)
					{
						MachineDisconnetCount++;
						if(MachineDisconnetCount > DROInstance->MachineDisconnect_CycleCount && !DROInstance->VersionControlflag) //If there is no data then say machine is disconnected and end the continuous read
						{
							/*d2 = GetTickCount();
							d3 = d2 - d1;*/
							DROInstance->MachineConnetedStatus = false;
							DROInstance->DROTargetReachedCallback(6);
							_endthread(); 
						}
						else if(CardStatusFlag && DROInstance->VersionControlflag && MachineDisconnetCount > 20)
						{
							NumberOfChar = 32;
							CardStatusFlag = false;
							DROInstance->DROTargetReachedCallback(1);	
						}
					}
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
					else if(str == "V")
					{
						TempReadBuffer = TempReadBuffer + "V";
						for(int i = 0; i < 20; i++)
						{
							ReadFile(DROInstance->PortHandle, tempbuffer, 1, &bufferLength, NULL);
							if(bufferLength == 0 || DROInstance->PortHandle == INVALID_HANDLE_VALUE)
							{
								TempReadBuffer = "";
								break;
							}
							else
							{
								tempbuffer[1] = '\0';
							    str = tempbuffer;
								TempReadBuffer = TempReadBuffer + str;
							}
						}
						int chCount = TempReadBuffer.find("V");
						for(int axisCount = 0; axisCount < DROInstance->NumberofMachineAxes + 1; axisCount++)
						{
							std::string HexValue = TempReadBuffer.substr(chCount + 1 + 4 * axisCount, 4);
							std::reverse(HexValue.begin(), HexValue.end());
							Version_Number[axisCount] = (atof((const char*)HexValue.c_str())) / 100;
						}
						DROInstance->WriteVersionNumber(Version_Number, 0);
						TempReadBuffer = "";
					}
					else
						TempReadBuffer = TempReadBuffer + str;
				}
				DROInstance->MachineConnetedStatus = true;
				if(DROInstance->VersionControlflag && CardStatusFlag)
				{
					CardStatusFlag = false;
					if(TempReadBuffer.find("L") != -1)
					{
						NumberOfChar = 32;
						DROInstance->DROTargetReachedCallback(15);
					}		
					else
					{
						NumberOfChar = 27;
						DROInstance->DROTargetReachedCallback(14);
					}
				}
				DROInstance->VersionControlflag = false;
				MachineDisconnetCount = 0;
				if(DROInstance->newCardFlag)
				{
					int CCount = TempReadBuffer.find("=");
					if(CCount != -1)
					{
						std::string DroString = "";
						if(TempReadBuffer.size() > CCount + 5 && TempReadBuffer.substr(CCount + 5).find("E") != -1)
							DroString = DROInstance->GetStringForCRC(TempReadBuffer.substr(CCount + 5), "E", 2);
						else if(TempReadBuffer.find("J") != -1)
						{
							DroString = DROInstance->GetStringForCRC(TempReadBuffer, "J", 2);
						}
						int chCount = TempReadBuffer.find("X");
						if(TempReadBuffer.find("{") != -1 && TempReadBuffer.find("{") != -1)
						{
							for(int i = 0; i < 4; i++)
							{
								std::string HexValue = TempReadBuffer.substr(chCount + 1 + 7 * i, 6);
								DroString += HexValue;
							}
							int LCount = TempReadBuffer.find("L");
							if(LCount != -1)
							{
								std::string HexValue = TempReadBuffer.substr(LCount + 1, 4);
								DroString += HexValue;
								int PCount = TempReadBuffer.find("{");
								if(PCount != -1)
								{
									for(int i = 0; i < 4; i++)
									{
										HexValue = TempReadBuffer.substr(PCount + 2 + 7 * i, 6);
										DroString += HexValue;
									}
									if(CCount != -1)
									{
										HexValue = TempReadBuffer.substr(CCount + 1, 4);
										std::reverse(HexValue.begin(), HexValue.end());
										DroString += HexValue;
										if(!DROInstance->CheckCRC(DroString))
											continue;	
									}
								}
							}
						}
						else if(TempReadBuffer.size() > 37)
						{
							for(int i = 0; i < 4; i++)
							{
								std::string HexValue = TempReadBuffer.substr(chCount + 1 + 7 * i, 6);
								DroString += HexValue;
							}
							chCount = TempReadBuffer.find("L");
							if(chCount != -1)
							{
								std::string HexValue = TempReadBuffer.substr(chCount + 1 , 4);
								DroString += HexValue;
								if(CCount != -1)
								{
									HexValue = TempReadBuffer.substr(CCount + 1, 4);
									std::reverse(HexValue.begin(), HexValue.end());
									DroString += HexValue;
									if(!DROInstance->CheckCRC(DroString))
										continue;	
								}	
							}
						}
					}
				}
				
				RBuffer = TempReadBuffer;
				DROInstance->ReadBuffer = TempReadBuffer;		
			
				int found_X = RBuffer.find("X"), found_Y = RBuffer.find("Y"), found_Z = RBuffer.find("Z");
				if(found_X == -1 || found_Y == -1 || found_Z == -1)
					continue;
				//Probe Related processing...
				int Pi = RBuffer.find("{"), Pj = RBuffer.find("}");
				if(Pi == -1 && Pj == -1 && !DROInstance->Probe_HitFlag)
				{
					DROInstance->AxisPropCollection[0].CurrentProbePosition = -1000;
					DROInstance->AxisPropCollection[1].CurrentProbePosition = -1000;
					DROInstance->AxisPropCollection[2].CurrentProbePosition = -1000;
					DROInstance->AxisPropCollection[3].CurrentProbePosition = -1000;
				}
				if(DROInstance->PorbeActivated)
				{
					int Ppi = RBuffer.find("P");
					if(Ppi == -1)
					{				
						DROInstance->PorbeActivated = false;
						if(!DROInstance->newCardFlag || !DROInstance->CNCMode)
						{
							DROInstance->Probe_HitFlag = false;
							if(DROInstance->ProbeRetractPointMethod)
							{
								DROInstance->FindProbeHitDirection(ValuesX, ValuesY, ValuesZ);
								DROInstance->LookTargetReachedSym = false;
								DROInstance->DROTargetReachedCallback(12);							
							}
							DROInstance->DROTargetReachedCallback(0);
						}
					}
				}
				if(Pi > 0 && Pj > 0 && (Pj > Pi) && !DROInstance->AxisPropCollection[0].MachineStartflag && !DROInstance->Probe_HitFlag)
				{
					//DROInstance->WritedebugInfo(RBuffer);
					int chCount = RBuffer.find("{");
					bool ProbeFlag = false;
					for(int axisCount = 0; axisCount < DROInstance->NumberofMachineAxes; axisCount++)
					{
						//This Calculation is Common for both Linear and Rotation..........
						std::string HexValue = RBuffer.substr(chCount + 2 + 7 * axisCount, 6);
						std::reverse(HexValue.begin(), HexValue.end());
						DROInstance->AxisPropCollection[axisCount].ProbeHexaDecimalValue = HexValue;
						mmCalculation = DROInstance->hex_dec(HexValue);
						(DROInstance->AxisPropCollection[axisCount].MovementDirection)? mmCalculation = mmCalculation - DROInstance->AxisPropCollection[axisCount].McsZero: mmCalculation = DROInstance->AxisPropCollection[axisCount].McsZero - mmCalculation;
						//The Multiplication Factor for the axis..
						mmCalculation *= DROInstance->AxisPropCollection[axisCount].MultipleFactor;
						//Looking for Linear or Rotation.......
						if(DROInstance->AxisPropCollection[axisCount].MovementType)
							mmCalculation /= DROInstance->AxisPropCollection[axisCount].CountsPerUnit;
						else
							mmCalculation = (mmCalculation * 2 * M_PI) / DROInstance->AxisPropCollection[axisCount].CountsPerUnit;
						//Looking for Change in DRO..............
						if(abs(DROInstance->AxisPropCollection[axisCount].CurrentProbePosition - mmCalculation) > 0.5)
						{
							ProbeFlag = true;
							if(!DROInstance->newCardFlag || !DROInstance->CNCMode)
								DROInstance->PorbeActivated = true;
							DROInstance->AxisPropCollection[axisCount].CurrentProbePosition = mmCalculation;
							DROInstance->ProbeCoordinate[axisCount] = mmCalculation;
						}
					}
					if(ProbeFlag && !DROInstance->Probe_HitFlag)		 			
					{
						//DROInstance->WritedebugInfo("---Next Point---");
						if(!DROInstance->ProbeRetractPointMethod)
						{
							DROInstance->FindProbeHitDirection(ValuesX, ValuesY, ValuesZ);
							DROInstance->LookTargetReachedSym = false;
							DROInstance->DROTargetReachedCallback(12);							
						}
						DROInstance->Probe_HitFlag = true;
					}
				}
				//End of the Probe value Processing..
				//

				//
				//Normal DRO value processing...

				DroChanged = false;
				bool RaxisValueChanged = false;
				double CurrentDROvalue[4] = {0}, CurrentDiffvalue[4] = {0};
				int chCount = RBuffer.find("X");
				for(int axisCount = 0; axisCount < DROInstance->NumberofMachineAxes; axisCount++)
				{
					//This Calculation is Common for both Linear and Rotation.......
					std::string HexValue = RBuffer.substr(chCount + 1 + 7 * axisCount, 6);
					std::reverse(HexValue.begin(), HexValue.end());
					DROInstance->AxisPropCollection[axisCount].ActualCounts = HexValue;
					mmCalculation = DROInstance->hex_dec(HexValue);
					if(DROInstance->AxisPropCollection[axisCount].MachineStartflag)
					{
						//DROInstance->WritedebugInfo(HexValue);
						DROInstance->AxisPropCollection[axisCount].MachineStartflag = false;
						DROInstance->AxisPropCollection[axisCount].McsZero = mmCalculation;
					}
					(DROInstance->AxisPropCollection[axisCount].MovementDirection)? mmCalculation = mmCalculation - DROInstance->AxisPropCollection[axisCount].McsZero: mmCalculation = DROInstance->AxisPropCollection[axisCount].McsZero - mmCalculation;

					//The Multiplication Factor for the axis..
					mmCalculation *= DROInstance->AxisPropCollection[axisCount].MultipleFactor;
					//Looking for Linear or Rotation.......
					if(DROInstance->AxisPropCollection[axisCount].MovementType)
						mmCalculation /= DROInstance->AxisPropCollection[axisCount].CountsPerUnit;
					else
						mmCalculation = (mmCalculation * 2 * M_PI) / DROInstance->AxisPropCollection[axisCount].CountsPerUnit;
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
				}
				if(DROInstance->NumberofMachineAxes == 3)
					DROInstance->AxisPropCollection[3].CurrentPosition = 0;
				//code to check current line number executing and function target reached logic.........
				int Line_Number = RBuffer.find("L");
				if(Line_Number != -1)
				{ 
					std::string HexValue = RBuffer.substr(Line_Number + 1, 4);
					std::reverse(HexValue.begin(), HexValue.end());
					DROInstance->LineNumberExecuting = DROInstance->hex_int(HexValue);
					if(!DROInstance->LineNumberResetFlag)
					{
						if(DROInstance->LineNumberExecuting != DROInstance->LastLineExecuting && DROInstance->CheckForLimihit)
						{
							if(DROInstance->LastLineExecuting != 0)
							{
								if(DROInstance->LineNumberExecuting == 0)
								{	
									//condition to check for target reached .....for probe position and goto position....
									if(DROInstance->CheckForTargetReached && DROInstance->CheckPartProgramRunningStatus() && !DROInstance->SmoothMovementAtProbePos)
									{
										if((abs(DROInstance->LastTarget[0] - CurrentDROvalue[0]) > DROInstance->Axis_Accuracy[0] * 10 || 
											abs(DROInstance->LastTarget[1] - CurrentDROvalue[1]) > DROInstance->Axis_Accuracy[1] * 10 || 
											abs(DROInstance->LastTarget[2] - CurrentDROvalue[2]) > DROInstance->Axis_Accuracy[2] * 10))
										{
											if(DROInstance->CheckForSecondTimeTarget == 0)
											{
												DROInstance->CheckForSecondTimeTarget++;
												DROInstance->WriteCommandSent("Line number change before target reached");
												DROInstance->DROTargetReachedCallback(7); // send last command again...
											}
											else if(DROInstance->CheckForSecondTimeTarget == 1)
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
										if(DROInstance->ProbeRetractPointMethod && DROInstance->Probe_HitFlag)
										{
											DROInstance->FindProbeHitDirection(ValuesX, ValuesY, ValuesZ);
											DROInstance->DROTargetReachedCallback(12);							
										}
										DROInstance->DROTargetReachedCallback(0);
										DROInstance->Probe_HitFlag = false;
									}
								}
								else
								{						
									for(int i = 0; i < DROInstance->LineNumberExecuting - DROInstance->LastLineExecuting; i++)
									{
										if(DROInstance->ProbeRetractPointMethod && DROInstance->Probe_HitFlag)
										{
											DROInstance->FindProbeHitDirection(ValuesX, ValuesY, ValuesZ);
											DROInstance->DROTargetReachedCallback(12);							
										}
										DROInstance->DROTargetReachedCallback(0);
										DROInstance->Probe_HitFlag = false;
									}
								}
							}
							DROInstance->LastLineExecuting = DROInstance->LineNumberExecuting;				
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
						}
						else
						{
							DROInstance->LastLineExecuting = DROInstance->LineNumberExecuting;				
						}
					}
					else
					{
						if(DROInstance->LineNumberExecuting == 0)
						{
							DROInstance->LastLineExecuting = 0;
							DROInstance->LineNumberResetFlag = false;
						}
					}
					//Logic to handle limit hit.......
					static int resendcount = 0;
					if(DROInstance->LastLineExecuting != 0 && DROInstance->CheckForLimihit && DROInstance->CanRaiseMachineStuckMessage)
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
									DROInstance->WriteCommandSent("Machine Seems to be Stuck");
									DROInstance->DROTargetReachedCallback(8);
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
						CycleCounter = 0;
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
				if(!DROInstance->newCardFlag)
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
										DROInstance->DROTargetReachedCallback(8);
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
	try
	{
		//DROInstance->WritedebugInfo(DROInstance->ProbeCoordinate);
		//DROInstance->WritedebugInfo(DROInstance->DROCoordinateValue);
		DROInstance->ProbeHitRaiseFlag = true;
		int CompareCnt = 14; //If the movement dir count(+/-) is greater than this in any axis then consider that direction//
		double ProbeVPrecision = 0.0006; //0.6 um.. //precision for direction calculation
		int XaxisPCnt = 0, XaxisNCnt = 0, YaxisPCnt = 0, YaxisNCnt = 0, ZaxisPCnt = 0, ZaxisNCnt = 0;
		double Cdvalue = 0;
		double PrX = DROInstance->AxisPropCollection[0].CurrentProbePosition;
		double PrY = DROInstance->AxisPropCollection[1].CurrentProbePosition;
		double PrZ = DROInstance->AxisPropCollection[2].CurrentProbePosition;
		for each(double Px in ValuesX)
		{
			Cdvalue = Px - PrX;
			if(abs(Cdvalue) > ProbeVPrecision)
			{
				if(Cdvalue > 0)
					XaxisPCnt++; // readingThread->ProbeDirection = readingThread->PROBEDIR::XRIGHT2LEFT;
				else if(Cdvalue < 0)
					XaxisNCnt++; // readingThread->ProbeDirection = readingThread->PROBEDIR::XLEFT2RIGHT;
				PrX = Px;
			}
		}
		for each(double Py in ValuesY)
		{
			Cdvalue = Py - PrY;
			if(abs(Cdvalue) > ProbeVPrecision)
			{
				if(Cdvalue > 0)
					YaxisPCnt++; // readingThread->ProbeDirection = readingThread->PROBEDIR::YTOP2BOTTOM;
				else if(Cdvalue < 0)
					YaxisNCnt++; // readingThread->ProbeDirection = readingThread->PROBEDIR::YBOTTOM2TOP;
				PrY = Py;
			}
							
		}
		for each(double Pz in ValuesZ)
		{
			Cdvalue = Pz - PrZ;
			if(abs(Cdvalue) > ProbeVPrecision)
			{
				if(Cdvalue > 0)
					ZaxisPCnt++; // readingThread->ProbeDirection = readingThread->PROBEDIR::ZTOP2BOTTOM;
				else if(Cdvalue < 0)
					ZaxisNCnt++; // readingThread->ProbeDirection = readingThread->PROBEDIR::ZBOTTOM2TOP;
				PrZ = Pz;
			}		
		}
		DROInstance->Probe_Hit_Direction = 9; //current probe hit direction..
		if(XaxisPCnt > CompareCnt) DROInstance->Probe_Hit_Direction = 0; // X-axis right to left//
		else if(XaxisNCnt > CompareCnt) DROInstance->Probe_Hit_Direction = 1; // X-axis left to right//
		else if(YaxisPCnt > CompareCnt) DROInstance->Probe_Hit_Direction = 2; // Y-axis top to bottom//
		else if(YaxisNCnt > CompareCnt) DROInstance->Probe_Hit_Direction = 3; // Y-axis bottom to top//
		else if(ZaxisPCnt > CompareCnt) DROInstance->Probe_Hit_Direction = 4; // Z-axis moving down//
		else if(ZaxisNCnt > CompareCnt) DROInstance->Probe_Hit_Direction = 5; // Z-axis moving Up//
		else DROInstance->ProbeHitRaiseFlag = false; //If the direction is in both then leave that.. User is very bad in Operating a simple machine!!
		if(!DROInstance->ProbeHitRaiseFlag)
			DROInstance->WriteCommandSent("Probe Hit..The movement direction was improper. On both axis /Movement was very very slow.. ");
		else
			DROInstance->WriteCommandSent("Probe Hit.");
		if(DROInstance->ProbeRetractPointMethod)
		{
			switch(DROInstance->Probe_Hit_Direction)
			{
			case 0:
				DROInstance->Probe_Hit_Direction = 1;
				break;
			case 1:
				DROInstance->Probe_Hit_Direction = 0;
				break;
			case 2:
				DROInstance->Probe_Hit_Direction = 3;
				break;
			case 3:
				DROInstance->Probe_Hit_Direction = 2;
				break;
			case 4:
				DROInstance->Probe_Hit_Direction = 5;
				break;
			case 5:
				DROInstance->Probe_Hit_Direction = 4;
				break;
			}
		}
	}
	catch(...){ DROInstance->SetAndRaiseErrorMessage("HCDLL0051", __FILE__, __FUNCSIG__); }
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
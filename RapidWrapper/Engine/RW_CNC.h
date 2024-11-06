//Class to Wrap all CNC and Lighting Related Commands..
//This class handles partprogram related commands also...handle current step according to the type..
#pragma once

namespace RWrapper 
{
public ref class RW_CNC
{	
public:
	delegate void CNCButtonClickEventHandler(System::String^ btntext);
	delegate void CNCEventHandler();
	delegate void AxisLockEventHandler(int axis, bool lock_Axis);
	delegate void TakeProbePointEventHandler();
	delegate void CNCModeChangeEventHandler(int direction);
	bool CNCmode, RefDotCamMode;
	System::Double RotaryScanSpan, RotaryScanSpeed;

private:
	static RW_CNC^ myCNC;//This class instance..

	bool gotoProbehit;//Flag for CNC mode../
	bool UpdatingCurrentLight, LightingON, PendingLightCommand, GotoProbePoint;
	cli::array<int, 1> ^LastLightProperty;
	cli::array<int, 1> ^LastLightValues;
	cli::array<double> ^ProbeGotoPosition;
	double* intermediatePts;
	int intermediatestep;
	int maxnoofsteps;
	cli::array<double> ^PrevTarget;  

public:
	event CNCButtonClickEventHandler^ GenerateCNCBtnClick;
	event CNCEventHandler^ CNCCommandTargetReached;
	event CNCEventHandler^ LightCommandUpdated;
	event AxisLockEventHandler^ AxisLockevent;
	event TakeProbePointEventHandler^ GotoProbePointEvent;
	event CNCModeChangeEventHandler^ CNCModeChange_Event;

public:
	RW_CNC();
	~RW_CNC();
	static RW_CNC^ MYINSTANCE();
	
	cli::array<int, 1> ^CurrentLightProperty;
	void SetCNCFeedRate(cli::array<double, 1>^ frate);
	void Activate_AbortDRO();
	void Activate_CNCModeDRO();
	void Activate_ManualModeDRO();
	void Lock_Axis(System::String^ AxisName);
	void Unlock_Axis(System::String^ AxisName);
	bool getCNCMode();
	bool IsLightUpdating();
	System::String^ CurrentOnlineSystemState;

	void Start_ZoomInOut(bool ZoomIn);
	void Stop_ZoomInOut(bool ZoomIn);

	System::String^ GetLightValue(cli::array<int, 1>^ lightvalue);

	//Calculates the light intensity for mouse scroll...
	void CalcualteSurfaceorProfileLight(cli::array<int, 1>^ lightvalue);

	void SwitchOnOffLight(bool Status);
	void UpdateLight(cli::array<int, 1>^ lvalue);
	void LightCallbackFromDRO(char* Lightvalue);
	void ConvertLightValueToInt(System::String^ Light_FromDRO);	
	void UpdateLightStatusAfterClear();
	void UpdateLight_ForZoomChange();

	void RaiseCNCEvents(int Cnt);
	void RaiseCNCModeChanged(int Cnt);

	void SendDROToGetProbePoint(bool gotoforpoint);
	void GotoPosition(cli::array<System::String^>^ position);
	void GoBackToOldPosition();
	void MovetoPosition(cli::array<System::String^>^ position);	
	void MoveToC_Axis_Y_Position();
	void Swivel_A_Axis();

	void MoveProbeToVisionPosition();

	void SendDROGotoPosition(double* target, double* feedrate, TargetReachedCallback TRtype);
	void GotoCommandforCircularMovement(cli::array<double, 1>^ target, cli::array<double, 1>^ intpt, bool CircularFlag);
	//move in circular path from curr DRO to target through the given intermediate point in noofintstops number of steps
	void StartCircularMovement(cli::array<double, 1>^ target, cli::array<double, 1>^ intpt, int noofintstops);
	void MoveCircularStep();
	void Lock_AxisMovement(int Axis, bool lock_Axis);
	void RaiseGotoProbePointEvent(double* Position);
	//TIS Functions
	void RotateCAxis360();
	void RotateAAxis(bool ToHorizontal);

	//Online System Commands
	void OperateStep(int Position, int Value);
	void MoveTable(bool PassPrevious, bool RejectPrevToPrevious, double Speed);
	void StopOnlineSystem();
	void Reset_PrevTarget();
};
}

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!

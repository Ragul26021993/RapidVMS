//Used for communication with the controller.
//Contains Port Settings/ Open port functions
//Read/Write functions.

#pragma once
#include <string>
#include <map>
#include <list>
#include "..\Maindll.h"
using namespace std;

#define DROInstance DigitalReadOut::getDROInstance() // This calss Instance.

class DRO_API DigitalReadOut
{
private:
	static DigitalReadOut* _instanceDRO; 
	int NumberOfAxis, NumberofMachineAxes, AxisLockValue, LineNumberExecuting, LastLineExecuting, CheckForSecondTimeTarget;
	bool CNCMode, MachineConnetedStatus, LookTargetReachedSym, CheckForLimihit, ProbeRetractPointMethod, CheckForTargetReached, Probe_HitFlag; //Look for targetReached Symbol.
	bool PorbeActivated, WritingCommands, ReadDroValues, newCardFlag, LineNumberResetFlag, VersionControlflag, StopCommunication, EnableInterpolation, HomePositionMode, CanRaiseMachineStuckMessage, SmoothMovementAtProbePos;
	bool ProbeActivated_X, ProbeActivated_Y, ProbeActivated_Z, ProbeActivated_R;
	int PositiveAcknowledgementCnt, MachineDisconnect_CycleCount, LimitHit_DroCycleCount;
	int Delaybetweencommnads, ProbDirection;
	int R_Axis_HSB, PreviousR_Value, r_jump;
	std::string CommnadSendFilePath, LastLightCommand, ReadBuffer;
	//To Store COM Port.
	WCHAR _ComName[10];
	//To Store Port Handle.
	HANDLE PortHandle;
	
	struct AxisProperties
	{
		double LashCompensation, CurrentPosition, McsZero, CountsPerUnit, MultipleFactor, CurrentProbePosition;
		int AxisdataLength;
		bool MovementDirection, LashDirection, MovementType, MachineStartflag;
		std::string ActualCounts, ProbeHexaDecimalValue;
	};
	map<int, AxisProperties> AxisPropCollection;
	
	//Protected Constructor..............
	DigitalReadOut(char* Commandfpath, WCHAR* portname, int Strlength);
	//Thread Function..............
	friend void ReadingDROValue(void *anything);
	friend void WriteCommandInDiffthread(void* CncCommandList);

	//Convert Hex to Dec...............
	double hex_dec(std::string hexval);
	int hex_int(std::string hexValue);
	void ConvertToByteArray(std::string hexValue, BYTE* byteArray);
	unsigned __int16 Get16bitCRC(BYTE* ptr, int length);
	std::string CalculateCRC(std::string AxisTarget);
	bool CheckCRC(std::string DroString);
	double dec_equ(char t);
	int int_equ(char t);
	//Send one command at time for new protocol..
	bool sendMachineCommand_One_OldProtocol(char* CncCommand, bool CheckCommunication, bool AttachStar = true);	
	void sendMachineCommand_Multi_OldProtocol(list<std::string> &CncCommandList, bool CheckCommunication);
	bool sendLightCommand_OldProtocol(char* Lcommand, bool CheckCommunication);
	bool sendMachineCommand_One_NewProtocol(char* CncCommand);
	void sendMachineCommand_Multi_NewProtocol(list<std::string> &CncCommandList);
	bool sendLightCommand_NewProtocol(char* Lcommand, char* LCommandWithCRC);
	void SetAndRaiseErrorMessage(std::string ecode, std::string filename, std::string fctnname);
	//Write each byte to the hardware..
	void WriteEachByte(char name);
	std::string GetStringForCRC(std::string OriginalStr, std::string FindStr, int Length);
public:
	~DigitalReadOut();
	static bool CreateInstance(char* compath, char* comparam, int Strlength);
	static void DeleteInstance();
	static DigitalReadOut* getDROInstance();
	double LastTarget[4], DROCoordinateValue[4], ProbeCoordinate[4], Axis_Accuracy[4], CTDroCoordinate[3];
	int Probe_Hit_Direction;
	bool ProbeHitRaiseFlag;
	struct BatchCommandTarget
	{
		double Target_Feedrate[8];
		double SweepAngle;
		int NumberOfHops, ProbeDir;
		bool LinearInterPolation;	
	};
public:

	//Convert Dec to Hex.........
	char dec_hex(int decval);
	bool notFocusing, Check_RaxisTargetReached;
	//Begin Thread to read DigitalReadOut/Handle Callback.
	void beginThread();
	void ConnectToHardware(bool ReconnectTohardware = false);
	//Number of Axis.......
	void setNumberofAxis(int n);
	void setNumberofMachineAxis(int n);
	void setDelayBetweenCommands(int d);
	//Set All properties for DRO settings.....
	void setDROProperties(bool* mt, bool* md, double* lv, double* mf, double* cpu);
	void ResetMachineStartFalg();
	void SetMachineCardFlag(bool flag);
	void SetInterpolationFlag(bool enable);
	//Set MCS start position
	void setMCSPosition(double* mcsvalue);
	void SetProbeDirection(int dir);
	//Funciton when closing software...........
	void ColseSoftware();
	void GetCardVersionNumber();
	//CNC Commands to Controller Functions
	void ActivateCNC();
	void ToAbort();
	void ToManual();
	void ToReset();
	void ToReset(double* ResetValue);
	void ToResetLineNumber();
	void PauseCurrentcommand();
	void ResumeCurrentCommand();
	void AxisLock(int AxisIndx);	
	void AxisUnlock(int AxisIndx);
	
	
	void StartAutoZoomInOut(bool ZoomIn);
	void StopAutoZoomInOut(bool ZoomIn);
	void GotoCommandWithCircularInterpolation(double* CircleParam, double SweepAngle, double EnteredFRate, int NoOfHops);
	void GotoForBatchOfCommand(std::map<int, double*> TargetPos, std::map<int, int> ProbeDir, double* EnteredFRate, int CommandCount);
	void GotoForBatchOfCommand(std::map<int, BatchCommandTarget> TargetPos, int CommandCount);
	//Goto Command to Move to Target Position....
	void GotoCommand(double* Target, double* EnteredFRate, bool GotoHomePositionMode = false, bool probePoint = false);
	//calculate the effective feedrate to move in optimised way..
	void CalculateEffFeedRate(double* CurrentPos, double* TargetPos, double* CurrentFeedrate, double* EffFeedrate);
	//Send one command at time..
	bool sendMachineCommand_One(char* CncCommand, bool CheckCommunication, bool AttachStar = true);	
	//Send multi line for goto commands..
	void sendMachineCommand_Multi(list<std::string> &CncCommandList, bool CheckCommunication);
	//Send light commands..
	bool sendLightCommand(char* Lcommand, bool CheckCommunication);
	//Set Probe Revert Back distance...
	void SetPorbeRevertbackDistance(double RevertDist, double RevertSpeed, double ApproachDist, double ApproachSpeed, double ProbeSensitivity, bool ProbeMoveUpFlag);
	//Rotate TIS A Axis
	void RotateAAxis(bool ToHorizontal);
	//Reset Target Reached Looking flag.. to check Notification Symbol...
	void ResetLookingSymbol();

	//Write to file : comand sent, probe hit etc..
	void WriteCommandSent(std::string data);
		
	//Callback Functions............
	void (CALLBACK *DROTargetReachedCallback)(int i);//Functions calls when Hardware is disconnected / Target reached / Other...
	void (CALLBACK *DROUpdateLightIntensity)(char* StrValue);//After Sending light to hardware.. light update successful...
	void (CALLBACK *DROErrorCallback)(char* hdcerrorcode, char* hdcFileName, char* hdcFunName); //To Raise generated error.
	void (CALLBACK *CommandSendLog)(char* Command);//To writecommand send log........
	bool (CALLBACK *CheckPartProgramRunningStatus)();
	bool (CALLBACK *ShowMessageBox)(char* MessageCode, bool Insert_TempStrFlag, char* TempStr);
	void (CALLBACK *WriteVersionNumber)(double* VersionNo, int i);

	void SetDroCycleCount(int Count);
	void SetMachineDisconnectCount(int Count);
	void SetProbeOnward_RetractMethod(bool CurrentStatus);
	void FindProbeHitDirection(list<double> ValuesX, list<double> ValuesY, list<double> ValuesZ);
	void WritedebugInfo(std::string Info);
	void WritedebugInfo(double *TargetValue);
	void SetMachineStuckFlag();
	void SetProbePositionSmoothMovement(bool flag);
	void ProbeRetractAlongDirectionOfMovement(bool flag);
	void Activate_Deactivate_Probe(bool X_Axis, bool Y_Axis, bool Z_Axis, bool R_Axis, bool DonotSendCommand);
};

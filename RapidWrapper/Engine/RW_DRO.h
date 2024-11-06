//To Handle DRO Initialisation, callbacks and Partprogram target reached etc..
#pragma once
#include "RW_LCParams.h"
#include "LC_Multiplier.h"

//Enumeration to handle the CNC commands for different functions...//
enum TargetReachedCallback
{
	PARTPROGRAM_ACTION,
	PARTPROGRAM_INCREMENT,
	PARTPROGRAM_PROBEHIT,
	PARTPROGRAM_PROBEGOBACK,
	PARTPROGRAM_RERUN,
	PARTPROGRAM_FINISHED,
	PARTPROGRAM_NOTHING,
	PARTPROGRAM_AUTOFOCUSSPAN,
	PARTPROGRAM_AUTOFOCUSFINISHED,
	PARTPROGRAM_AUTOFOCUSPOSITIONREACHED,

	FOCUSDEPTH_SPAN,
	FOCUSDEPTH_CONTINUEACTIVESPAN,
	FOCUSDEPTH_FINISHED,
	FOCUSDEPTH_POSITIONREACHED,
	FOCUSDEPTH_AFSMODEPOSITION,
	FOCUSDEPTH_START_DURINGPROGRAM,
	FOCUSDEPTH_SPAN_DURINGPROGRAM,
	FOCUSDEPTH_CONTINUEACTIVESPAN_PARTPROGRAM,
	FOCUSDEPTH_POSITIONREACHED_DURINGPROGRAM,
	FOCUSDEPTH_DRO_DURINGPROGRAM,
	
	PROFILESCAN_MODE,
	PROFILESCAN_AUTOFOCUSSPAN,
	PROFILESCAN_AUTOFOCUSFINISHED,
	PROFILESCAN_AUTOFOCUSPOSITIONREACHED,

	AUTO_CALIBRATIONMODE,
	AUTO_WIDTHMEASUREMENTMODE,
	AUTO_CALIBRATIONNEXTORNTATN,
	AUTO_ZOOMINOUTMODE,
	AUTO_ZOOMINOUTDURINGEXELOAD,
	AUTO_SGWIDTH,
	
	MACHINE_GOTOCOMMAND,
	MACHINE_GOTORCADWINDOWCLICK,
	MACHINE_GOTOSAOICELLCLICK,
	MACHINE_GOTOAUTOMEASUREMODE,
	MACHINE_HOMINGMODE,
	MACHINE_MOVELITTLE_AFTERHOMING,
	MACHINE_CIRCULARMOVEMODE,
	MACHINE_PROBE_PATHPOINT,
	PROBE_MEASURE_GOBACK,
	FIXTURE_CALIBRATION,
	VBLOCK_CALLIBRATION,
	VBLOCK_CALLIBRATION_LINE,
	VBLOCKCALLIBRATION_AUTOFOCUSSPAN,
	VBLOCKCALLIBRATION_AUTOFOCUSFINISHED,
	VBLOCKCALLIBRATION_AUTOFOCUSPOSITIONREACHED,
	VBLOCK_GOTOCOMMAND,
	SPHERECALLIBRATION,
	SPHERECALLIBRATION_POINT,
	PARTPROGRAM_SAVEIMAGE,
	IMAGEGRID_PROGRAM,
	CIRCULARINTERPOLATION,
	CIRCULARINTERPOLATION_CURRENTPOS,
	LINEARINTERPOLATION,
	TRACEPATH_INTERPOLATION,
	FIXTURE_CALIBRATION_FRAMGRAB,
	STYLUS_SPHERECALLIBRATION,
	GRIDIMAGE_SUPERIMPOSE,
	FOCUSCHECK_SPAN,
	PROFILEAUTOFOCUSSTART,
	GOTO_REPEATAUTOMEASURE,
	REPEATAUTOMEASURE_AUTOFOCUSSPAN,
	REPEATAUTOMEASURE_AUTOFOCUSFINISHED,
	REPEATAUTOMEASURE_AUTOFOCUSPOSITIONREACHED,
	DONOTHING,
	NEXT_PROFILE_SCAN,
	MACHINE_MOVESLOW_FORHOMING,
	SPHERECALIBRATIONMANUALPOINT,
	AUTO_CALIBRATIONLASTORN,
	CONTOURTRACER_CALLIBRATION_PATH,
	CONTOURTRACER_CALLIBRATION_POINT,
	TIS_COMMAND_TARGETREACHED,
	SPHEREVISIONPROBECALIBRATION,
	DELPHICOMPONENTMODULE,
	IMAGEDELAYCALIBRATION,
	DELPHICOMPONENTMODULE_MANUAL,
	LBLOCK_MODULE,
	C_AXIS_RECALCULATE,
	ROTARY_AXIS_SCAN,
};

public ref struct ProbeParameters
{
	int AxisLabel = 0;
	double ApproachDist = 1.0;
	double RetractDist = 1.0;
	double ApproachSpeed = 0.5;
	double RetractSpeed = 0.5;
};

namespace RWrapper
{
	public ref class RW_DRO
	{
	public:
		delegate void DROEventHandler();
		delegate void DROTargetReachedEventHandler(int type);
		delegate void TouchedProbeEventHandler();
		delegate void PausePartProgramEventHandler();
		delegate void CameraClickSoundEventHandler();
		delegate void SendCommandAtStartup();
		delegate void ZeroingOffsetEventHandler(cli::array<double> ^OffsetValue);
		delegate void SwivelFinishedEventHandler(System::String^ Position);

	private:
		//RW_DRO instance..//
		static RW_DRO^ DROInst;
		static cli::array<double>^ MousePosOnVideo;
		cli::array<double> ^Last_DROValue;
		double FocusMoveAgainValue;
		double AxisAngle, AxisAngleXZ, AxisAngleYZ;
		int ReDoScanCount;
		int FixtureCalibAdjust;
		bool RetractProbePointMethod;
		cli::array<RW_LCParams ^> ^CorrectionLookup;
		//System::Collections::Generic::List<System::Collections::Hashtable^>^ LC_CorrectionZoneCollection;
		System::Collections::Hashtable^ LC_CorrectionZoneCollection;
		System::Collections::Generic::List<RWrapper::LC_Multiplier^>^ LC_MultiplierList;
		cli::array<double>^ gridSize;
		cli::array<double>^ Range;
		//std::map<int, SinglePoint*> ProbePointCollection;
		TargetReachedCallback HandleTarget;
		//Convert radian to degree.. R axis value..//
		System::String^ radianToDegree(double _value);
		System::String^ GetCommPortName();
		System::String^ ScaleDirectionString;
		System::String^ MotorDirectionString;
		System::String^ JoystickSpeedFactorString;

		void Handle_DRO_AxisValueChanged();
		//void Handle_DRO_RAxisValueChanged();
		void Handle_DRO_ProbeHit();
		void Synchronise_Handler_Action();
		//Functions to handle DRO target reached..
		void Handle_PartProgramCurrentAction();
		void Handle_PartProgramActionIncrement();
		void Handle_PartProgramActionRedo();//Redo action because target reach has been called early
		void Handle_PartProgramProbeHit();
		void Handle_ProbeGoBack();
		void Handle_ProgramRerun();
		void Handle_ProgramFinished();
		void Handle_ProgramStepDoNothing();
		void Handle_ProgramAutoFocusSpan();
		void Handle_ProgramAutoFocusFinished();
		void Handle_ProgramAutoFocusPosReached();
		void Handle_AutoMeasure_ProbeGoBack();
		void Handle_SetCNCStatus();
		void Handle_AutoFocusSpan();

		void Handle_FocusDepthSpan();
		void Handle_FocusCheckSpan();
		void Handle_FocusDepthContinueSpan();
		void Handle_FocusDepthFinished();
		void Handle_FocusDepthFocusPositionReached();
		void Handle_FocusDepthAutoFocusScanNextPosition();
		void Handle_FocusDepthStartDuringProgram();
		void Handle_FocusDepthSpanDuringProgram();
		void Handle_FocusDepthContinueSpanDuringProgram();
		void Handle_FocusDepthFocusPositionReachedDuringProgram();
		void Handle_FocusDepthDROPositionDuringProgram();

		void Handle_ProfileScan();
		void Handle_ProfileScanAutoFocusSpan();
		void Handle_ProfileScanAutoFocusFinished();
		void Handle_ProfileScanAutoFocusPosReached();

		void Handle_VblockCallibration();
		void Handle_VblockCallibrationAutoFocusSpan();
		void Handle_VblockCallibrationAutoFocusFinished();

		void Handle_CircularRepeatAutoMeasure();
		void Handle_CircularRepeatAutoMeasureFocusSpan();
		void Handle_CircularRepeatAutoMeasureFocusFinished();

		void Handle_AutoCalibrationMode();
		void Handle_AutoWidthMeasureMode();
		void Handle_AutoCalibrationNextOrnt();
		void Handle_AutoCalibrationLastOrnt();
		void Handle_AutoZoomingMode();
		void Handle_AutoZoomingModeDuringExeLoad();
		void Handle_Auto_SGWidthReapet();

		void Handle_MachineGotoCommand();
		void Handle_MachineGotoRcadWindowClick();
		void Handle_MachineGotoSAOICellClick();
		void Handle_MachineGotoAutoMeasureMode();
		void Handle_MachineHomingMode();
		void Handle_MachineMoveLittleAfterHoming();
		void Handle_MachineMoveSlowForHoming();
		void Handle_MoveCircularStep();
		void DoAutoFocusDuringProgramRun();
		//void Handle_CAxisRotated();

	public:

		bool TakeProbePointFlag, AutoFocusMode_Flag, Is_Delphicam, IsSpuriousTouchPoint;					//for new feature of delphicam polilline deviation
		bool AutoCalibrationMode, LocalisedCalibrationDone;
		bool HomingMovementMode, HomingisDone, UpdatingDROTextBox;
		bool DontUpdateLocalisedCalibration, DontTakeDroValues;
		bool XAxisHomeToPositive, YAxisHomeToPositive, ContourScan_Callibration;
	
		//bool HeightGauge_ScanningModeON;
		int XAxisPositiveHomeValue, YAxisPositiveHomeValue, CurrentDROCallBackFlag, RotationalScanCount;
		double Contour_ScalingCoefficient, Contour_Reference_Rlevel;
		cli::array<double> ^Contour_Offset, ^Contour_AxisOfRotation;
		int GageAlignmentAxis;
		//Exposed DRO values..
		static cli::array<double> ^DROValue, ^CurrentUCSDRO, ^CurrentUCSDRO_IntUse;
		static System::String ^RAxisValue, ^RAxisGradeValue;
		cli::array<double> ^HardwareVersionNumber;
		cli::array<double> ^C_AxisErrorTable;
		//DRO properties..
		bool *AxisProperty, *AxisLock, SelectProbePointSide;
		double *OriginalDROCoordinate, *DROCoordinate, *ProbeCoordinate, *feedRate, *returnPosition;
		//System::Collections::Generic::List<ProbeParameters^>^ ProbeParams; //
		cli::array<System::Double>^ Corrections;
		cli::array<int> ^ScaleData;
		double RevertDistance, ApproachDistance, RevertSpeed, ApproachSpeed, ProbeSensitivity;

		//Dro changed event....//
		event DROEventHandler^ DROChanged;
		event DROTargetReachedEventHandler^ DROTargetReached;
		event TouchedProbeEventHandler^ TouchedProbePostion;
		event PausePartProgramEventHandler^ PausePartPartProgram;
		event CameraClickSoundEventHandler^ CameraSoundEvent;
		event SendCommandAtStartup^ SendCommandAcknowledge;
		event SendCommandAtStartup^ SendMachineSwitchedOff;
		event ZeroingOffsetEventHandler^ ZeroingOffsetEvent;
		event SwivelFinishedEventHandler^ FinishedSwivelling_A_Axis;

		//Constructor and destructor...//
		RW_DRO();
		~RW_DRO();
		static RW_DRO^ MYINSTANCE();


		void GetDROSettingsValues(bool updateFeedRate);
		void GetLocalisedCalibrationDetails();
		void GetContourScanCallibrationDetails();
		void GetFocusCalibrationDetails();
		bool ConnectTo_RapidIhardware();
		void AxisAngleCorrections();
		void SetReturnPosition();
		void Handle_DROCallback(double* currentDro);
		//Functions to display the DRO values in DRO textboxes: Raise event to front end//
		static void UpdateDRO_textbox();
		static void UpdateCenterScreen();
		//Functions to display the mouseposition on VIDEO during draw mode.
		void DisplayVideoMouseMove(double* mousepos);
		void HandleProbeHitCallback(int direction, double* currentDro);
		void Handle_DROButtonClick(System::String^ Str, bool LeftClick);
		void Raise_DROTargetReached(int type);
		void Handle_DROTargetReached(int type);
		void setTargetReachedTypeCallback(TargetReachedCallback type);
		void FunctionTargetReached(bool ApplyDelay);
		void GetProbePositionInTouch(int currentSide);
		void RaiseCameraClickSoundEvent();
		void RaiseTouchedProbePostion();
		void ResetMachineStuckFlag();
		void Set2StepPP_Flag(bool SetFlag);
		//Added on 30 Jun 12............
#pragma region Set EEPROM values
		int SetProbeRevertDistSpeed(int AxisNo, double RevertDist, double RevertSpeed, double ApproachDist, double ApproachSpeed, double ProbeSensitivity, bool ProbeMoveUpFlag);
		void SetScaleDirection(bool PositiveFlag, int Axisno, bool Startup);
		void SetMotorDirection(bool PositiveFlag, int Axisno, bool Startup);
		void SetAccuracy(int value, int Axisno, bool Startup);
		void SetDeceleration(int value, int Axisno, bool Startup);
		void SetAcceleration(int value, int Axisno, bool Startup);
		void SetPitch(int value, int Axisno, bool Startup);
		void SetJoysticSpeedFactor(int value, int Axisno, bool Startup);
		void SetResolution(int value, int Axisno, bool Startup);
		void Activate_DeactivateProbe(bool X_Axis, bool Y_Axis, bool Z_Axis, bool R_Axis, bool Startup);
		void Activate_Deactivate_Zoom_Cnc(bool CncFlag, bool ZoomFlag, bool SmoothMovement, bool RetractProbePoint, bool Startup, int AxisEnable, bool DRO_24Bit);
		void SetInterPolationParameter(bool enableInterPolation, bool SmoothInterpolation);
		void SetProbeReractModeForManual(bool enableRetract);
		void SetSpeedControls(cli::array<int>^ WriteValues);
		void RetractAlongDirectionOfMovement(bool flag);
		void ResetDroValue(cli::array<double> ^ResetValue, bool Zeroing);
		void WriteToEEPROM();
#pragma endregion
		void SendMachineAtZeroingPosition(cli::array<double> ^OffsetValue);
		void ResetOffsetPosition();
		//void RotatePoint_Around_C_Axis(double *Position);
		void ConnectOnlineSystem(bool StopValues);
	};
}

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!

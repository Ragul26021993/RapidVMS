//Class Used for autoCalibration..
//All the button actions, event handling from / to the front end for the autocalibration part..
#pragma once
class LocalisedCorrectionAction; 
class RAction;
//Slip gauge orientation...
enum CURRENTLINETYPE
{
	VERTICAL_LINE, //Placed paralllel to X-axis..
	HORIZONTAL_LINE, //Placed Parallel to Y-Aixs..
	INCLINED_45_LINE, //Placed at an inclination of 45 degree
	NOTHING, //Perform nothing
};
namespace RWrapper 
{
	
	public ref class RW_AutoCalibration
	{
		//Delegate for events..
	public:
		delegate void CalibrationEventHandler();
		//All variable declaration..
	private:
		static RW_AutoCalibration^ AutoCalInstance; //hold the instance..
		int RowCount, ColumnCount, MeasurementCount;
		int TotalRows, TotalColumns;
		int RepeatCalibCount;
		int CurrentLineGrabCount, LinesAddedForCurrentMeasure;
		double MachineWidthX, MachineWidthY, SlipgaugeLength, ShiftLength, OverlapWidth;
		double NextCellGap;
		System::String^ MachineType;
		bool MachinetypeSetFlag, SlipGaugeWidthSetFlag, MeasurementAsMedian;
		CURRENTLINETYPE CurrentLType;
		// To pass to the settings..
		System::Collections::ArrayList^ AllMeasurementValues;
		bool StartedRunning;
		cli::array<double> ^WidthStartDROValue;
		bool CalculateBothWidthMeasurement;
		//Functions to handle the RW_CNC movement.. next step, cell or row or column etc..
		void GotoNextGrabPosition();
		void GotoSecondLineStartPosition();
		void GotoNextOrientation();
		void RepeatCurrentOrientaion(int i);
		void GotoNextCell();
		LocalisedCorrectionAction *CurrentAction;
		bool Home_X_Axis, Home_Y_Axis, Home_Z_Axis;

	public:
		//Hold the Machine type and slipgauge types..
		System::Collections::Generic::List<System::String^>^ MachineTypeColl;
		System::Collections::Generic::List<System::String^>^ SlipGaugeTypeColl;
		//Events .. To set the front end state..
		event CalibrationEventHandler^ DisableButtonState;
		event CalibrationEventHandler^ ResetButtonState;
		double SlipGaugeWidth_Frame;

	public:
		RW_AutoCalibration(); //Default constructor..
		~RW_AutoCalibration(); //Destructor.. Clear the memory..
		static RW_AutoCalibration^ MYINSTANCE(); //Expose the class instance..
		bool DoSingleFramePerSide;
		//Handle the button click actions..
		void Handle_ResetDROClicked();
		void Handle_SetMachineType(System::String^ Mtype);
		void Handle_SetSlipGaugeWidth(System::String^ Swidth);
		void Handle_CheckBoxChanged(bool flag);
		void Handle_RunClicked();
		void Handle_StopClicked();
		void Handle_MeasureWidthClicked();
		void Handle_MeasureSlipGaugeWidthClicked();
		void Handle_MeasureSlipGaugeWidthOnClick(int RepeatCount);
		void Handle_CalibrateClicked();
		void Handle_DoneClicked();
		void Handle_ClearClicked();
		void Handle_CloseClicked();

		void GoToPreviousAction(RAction* preAtn);
		void GoToNextAction(RAction *curAtn, bool redo);

		//Hanlde homing, Measurement, RW_CNC related actions.
		void FinishedMeasurement();
		void FrameGrabPoints_WidthMeasure();
		void FrameGrabPointsToSelectedLine();
		void GotoNextStep_WidthMeasurement();
		void GotoNextStep(bool addLocalisedaction);
		void StartNextOrientation();
		void Reset_Status();
		void Handle_HomeClicked(bool X_Axis, bool Y_Axis, bool Z_Axis);
		void ReachedHomePosition();
		void ReachedNearHomePosition();
		void ReachedTargetPosition();
		void SetMeasurementAsMedian(bool isMedian, double shiftLength);
		void SetOverlapWidth( double overlapWidth);
		void GotoLastPostion();
	};
}

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
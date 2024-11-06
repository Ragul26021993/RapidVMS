#pragma once
#include <Windows.h>
#include "RW_MainInterface.h"
#include "..\..\MainDLL\Shapes\Vector.h"

//using System::Collections

namespace RWrapper
{
	public enum class TISMeasureTool
	{
		GetAxis,
		ShankAlignment,
		ShankDia,
		CuttingDia,
		FaceRunout,
		PointAngle,
		StepLength,
		BallNose,
		MaxLine,
		MaxArc,
		Helix,
		CoreDia,
		CornerDia,
		BackTaper,
	};

	public enum class TISTargetType
	{
		AbsolutePosition,
		RelativeToStart,
		RelativeToPreviousCommand,
	};

	public enum class TISActionType
	{
		TravelCommand,
		StaticFrameGrab,
		RotaryScan,
		Y_AutoFocus,
		Z_AutoFocus,
		R_AutoFocus,
		Swivel_A_Axis,
	};

	public ref class TISCommand
	{
	public:
		cli::array<double>^ TargetPosition;
		cli::array<double>^ FeedRate;
		int SwivelTarget; //0 for no change; + 1 to go to vertical; -1 to go to Horizontal
		//bool RotaryScanForThisStep;
		//FramegrabBase* FGforThisStep;
		TISTargetType TargetType;
		TISActionType ActionType;
		cli::array<double>^ ActionParameters;
		int ShapeFromStep; //if its -1, we make a new shape. if its a positive whole number, we recall the shape from that item in the list.
		String^ ShapeType;
		String^ FG_Type;
		int ShapeID; //ID of the shape made during this step. 
		cli::array<double*>^ RotaryScannedPtsArray;
		cli::array<double*>^ RotaryScanPositions;
		int CurrentFrameIndex, TotalFramesReceived;
		int StepSize;
		TISCommand::TISCommand(int AxisCount)
		{
			TargetPosition = gcnew cli::array<double>(AxisCount);
			FeedRate = gcnew cli::array<double>(AxisCount);
		}

		TISCommand::TISCommand()
		{
		}
		TISCommand::~TISCommand()
		{
			if (RotaryScannedPtsArray)
			{
				for (int i = 0; i < RotaryScannedPtsArray->Length; i++)
				{
					free(RotaryScannedPtsArray[i]);
					free(RotaryScanPositions[i]);
				}
			}
		}
	};

	public ref class ShankAlignStep
	{
	public:
		property int Level;
		property bool Enabled;
		property double Height;
		property double Runout; // { get; set; }

		ShankAlignStep(int ReqLevel, double RequiredHeight, bool EnableLevel)
		{
			Level = ReqLevel;
			Height = RequiredHeight;
			Enabled = EnableLevel;
		}
	};


	public ref class TIS_Control
	{
	public:
		static TIS_Control^ MYINSTANCE();
		TIS_Control();
		~TIS_Control();

		//All exposed public variables
		TISMeasureTool CurrentTool;

		//Properties
		int ScanDelayCount, CurrentStepIndex, PointPickingDelay, CurrentScanDoneCount;
		bool AoR_Done, ShankAlign_Done, DatumTaken, TopRefTaken, A_Axis_isVertical, RotaryScanisON;
		double RotaryScanSpeed, RotaryTravelSpeed,CuttingTipScanSpan;
		cli::array<double> ^RotaryStartPosition;
		cli::array<double> ^DatumPoint;
		cli::array<double> ^TopRefPoint;
		cli::array<double> ^TIS_Axis;

		System::Collections::Generic::List<TISCommand^>^ CommandsList;
		System::Collections::Generic::List<ShankAlignStep^>^ ShankAlignColl;

		//Event Handler Delegates & events
		delegate void RotaryScanningEventHandler(int FrameCount);
		delegate void FinishedRotaryScanningEventHandler(cli::array<double>^ Answers);

		event RotaryScanningEventHandler^ RotationalScanEvent;
		event FinishedRotaryScanningEventHandler^ FinishedRotaryScanEvent;


		//---------------------Methods-----------------------
		//Get Axis for calculating Axis of Rotation
		
		//bool SetParameters(cli::array<double>^ Params);
		bool StartMeasurement(System::Collections::Generic::List<double>^ Parameters);
		void RotaryScanForCurrentTask(bool BeginTask);
		void Finish_Measurement(bool InformAllSubscribers);
		int GenerateCommandsList(System::Collections::Generic::List<double>^ Parameters); //TISMeasureTool selectedType
		void RestoreAxisofRotation();
		void RunCommandsFromList();
		void DoCommandActions();
		void ResetTISAxis();
		void SetPoint(cli::array<double>^ GivenPt, int ForRefPoint, bool RefPtTaken); //RefPoint = 0 for Datum, = 1 for TopRef
		//bool PauseMeasurement();
		//bool ResumeMeasurement();
		//bool SetTISAxis(cli::array<double>^ AxisParams);

	private:
		static TIS_Control^ currentTIS;

		//Variables
		int CyclesCount; 
		std::list<Vector>* ScannedPtsCollection;
		bool ScanningIsInProgress;
		ShapeWithList* CurrentShape;
		double* ShankOrientationPts;
		double* ShankCorrection;
		int ShankPtsCt;
		//Methods
		void Calculate_PtsAround_CAxis();
		void RefDot_in_SeparateThread();
		void FrameGrab_in_SeparateThread();
		void AxisAlignmentProcessingPerStage_Threaded(TISCommand^ stageCommand);
			//bool ScanForPoint();
		//bool AddScannedPointToShape();
		//bool FinishMeasurement();

	};
}
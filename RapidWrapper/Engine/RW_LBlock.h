#include "..\MainDLL\Shapes\CloudPoints.h"
#include "..\RapidWrapper\Engine\RW_DelphiCamModule.h"
#include "..\MainDLL\Shapes\Cone.h"

#pragma once


namespace RWrapper
{
	public ref class CNCCommand
	{
	private:
		int AxisNumber;
	public:
		//Variables
		cli::array<double>^ Position;
		cli::array<double>^ Speed;
		bool ProbePoint;
		int ProbeHitDirection;

		//Methods
		CNCCommand(int NoofAxes);
		~CNCCommand();
		void MoveAxisTo(int AxisIndex, double MoveTo);
		void MoveAxisBy(int AxisIndex, double MoveBy);
		void SetAxisSpeed(int AxisIndex, double AxisSpeed);

		bool SetPosition(cli::array<double>^ Target);
		bool SetSpeed(cli::array<double>^ FeedRate);
		bool Set(cli::array<double>^ Target, cli::array<double>^ FeedRate, bool probePt, int ProbDir);
	};
	
	
	public ref class RW_LBlock
	{
	public:
		RW_LBlock();
		~RW_LBlock();
		static RW_LBlock^ LBlock;
		static RW_LBlock^ MYINSTANCE();

		// All variables come here
	public:
		cli::array<double>^ Params;
		cli::array<double>^	StartPosition;
		bool FirstStepRunning, CycleStarted;
		static bool instantiated;

	private:
		cli::array<double>^ cPosition; //All the Heights and lengths of the L-Block to be measured. 
		//Params in same order as written in dB Measurement(index) - Lt(0), Ht(1), L1(2), L2(3), H1(4), H2(5)
		int CurrentStepNumber, TotalStepNumber; 
		bool RunningCone;

		int currentAxis, direction; //Denotes which axis is being calibrated

		System::Collections::Generic::List<RWrapper::CNCCommand^>^ PointsToGoList;

	public:

		//All methods come here
		void HandleCompletedCurrentStep();
		void CalculateSteps(int AxistobeRun, int NegativeDir, bool forTouch);

	public:
		bool Initialise_LBlockSettings(cli::array<double>^ LBlockParams, int AxistobeRun, bool NegativeDir, bool byTouch);
		void Run_LBlock(); // int AxistobeRun, cli::array<double^>^ LBlockParams, bool NegativeDir, bool byTouch);
		void Stop_LBlock();
		//void Pause_Resume();
		int GetAnswer(cli::array<double>^ Answers);

		//Use the L-Block FrameWork to do AUtomated Cone as well...
		//bool Initialise_ConeSettings(cli::array<double>^ ConeParams, bool NegativeDir, bool byTouch);
		void CalculateConeSteps(int NumberofLevels, double Z_StepSize, double ConeAngle);
		void RunAutoCone();
		int GetProbeDirection(cli::array<double>^ CurrentPosition, cli::array<double>^ TargetPosition);

		//All Events and Handlers come here
	public:
		delegate void CompletedthisStep(double FractionCompleted);

		event CompletedthisStep^ LBlockStepCompleted;

	};
}
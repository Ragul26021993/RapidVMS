#include "..\MAINDLL\Shapes\Cylinder.h"
#include "RW_DRO.h"

#pragma once

	class RPoint;
	class Circle;
	class Line;

namespace RWrapper 
{
	public ref class PointsToGo
	{
	  public:
		  System::Double Pt_X, Pt_Y, Pt_Z, Pt_R;
		  System::Double Sp_X, Sp_Y, Sp_Z, Sp_R;
		bool path;
	};
};

namespace RWrapper 
{
	public ref class RW_DelphiCamModule	
	{
		int probedirection;
		System::Collections::Generic::List<RWrapper::PointsToGo^>^ PointsToGoList;
		cli::array<double> ^InnerCirclePoint, ^LastTargetValue;
		TargetReachedCallback CurrentTargetType;
		static RW_DelphiCamModule^ DelphiCamModule; 
	    bool firsttime, InPauseState;
		Circle* MasterCylinder;
		CloudPoints* MasterCyl_RO;

		RPoint* lastPoint;
		Circle* parentCircle; 
		Circle* topC;
		CloudPoints* topC_RO;
		Circle* botC;
		CloudPoints* botC_RO;
		CloudPoints* RakePts;
		CloudPoints* IndexingPts;
		CloudPoints* LeadPts;
		CloudPoints* FirstToothPts;
		Line *parenLine;
		cli::array<double> ^startPosition, ^cPosition;
		double CylinderDia; 
		double *BC_CentrePositionPts;
		double *BC_Diameters;
		int MasterLength, NumberofLevels;
		void SelectShapeforThisRun(ShapeWithList* CShape, System::String^ sType);
		void WritePositionCommandstoFile(System::String^ ProcessType);

	public:
		static bool instantiated;
		delegate void CompletedthisTask(double TaskType);
		delegate void StartCentreCalibration(double diameter);
		cli::array<double> ^ToothMidPt;
		event CompletedthisTask^ DelphiCamEvent;
		RW_DelphiCamModule(); 
		~RW_DelphiCamModule(); 
		cli::array<double> ^ReferenceCenter;
		cli::array<double> ^BC_Values;
		int DelphiCamType; // = 0 for ID Cam, 1 for OD Cam; 2 for BC axis in HC; 3 for Measure 1; 4 for MEasure 2 and 5 for Measure 3 of Hobchecker
		int RunoutCorrectionStage; // = 0 not running; = 1 running level 1; = 2 running level 2; = 100 completed and working; = -1 not done;
		int RO_PtsCt; // Number of points to be taken for one revolution for runout calculations
		static RW_DelphiCamModule^ MYINSTANCE(); 
		bool DoingCalibration, FirstToothDone;
		double HobOD;	//used in rwcnc in fn MoveToC_Axis_Y_Position()
		bool Getting_BC_Axis, GotAxis, DoingHobMeasurement, HobMeasureisComplete, AutoMeasureMode, AllowIncrement, WaitingForTouchPt;
		bool ManualPoint, HobParametersUpdated;
		cli::array<double> ^Hob_RunoutParams;
		int Listcount;
		cli::array<double> ^HelicalGashParams;

		//Helical Hob Parameters
		cli::array<double>^ HobParameters;
		//index and corresponding values - 
		//0 = MeasurementNo int
		//1 = start_depth double
		//2 = Tooth_depth double
		//3 = ODiameter double
		//4 = Helix Face -1 or +1 Front = 1;  back = -1
		//5 = Hand of helix -1 or +1
		//6 = NoOfGashes int
		//7 = PtsPerTooth int
		//8 = Number of Teeth to Do int
		//9 = hobLength double int active length
		//10 = Rake Angle double
		//11 = NoofStarts int
		//12 = Module double
		//13 = Gash_Pitch double
		//14 = Hub Diameter double
		//15 = OD_Calculated double
		//16 = Pressure Angle double
		//17 = Hob Length Total
		//18 = First Tooth Number from Top
		//19 = 2 states only Form&Pos; adding 4 to it means we have indexing as well; adding 8 means we have gash lead as well (bitwise addition) 
		//20 = 0 for Normal Hobs, 1 for cutters with no helical progression of cutting teeth. 
		cli::array<double>^ HobSettings;
		//0 = Runout Pre-run Rotation angle
		//1 = How many times to repeat the runout measurement
		//2 = How many times to repeat the Hob measurement
		//3 = Speed at which we have to run the step that touches for runout
		//4 = What the multiple of above speed at which we have to run the axis that has to run faster for travelling from one position to the next on the hub
		//5 = Hub/Cylinder Radius is divided by this number to calculate the extra distance for getting to position of the touch point

		void SendDrotoNextposition();		
		//bool Start(double MaxRadius, double MinRadius, bool Is_ID_tool);//, vinod  tool type is od or id and max,min ineer/outer radius / according to new fe..		
		bool Start(double MaxRadius, double MinRadius, bool Is_ID_tool, double DepthFromTop);//, vinod  tool type is od or id and max,min ineer/outer radius / according to new fe..		
		//Hob Checker measurement cycle running
		bool Start(int MeasurementNo);

		void countinue_pause_delphicammodule(bool is_pause);// continue or pause implemented		vinod
		//static void Fill__Circle_Centre(double x_coord, double y_coord, double z_coord, double slope_, double intercept_, double radius_, double sector_angle);// ray properties,circle center value filling in array		vinod	
		static void Fill__Circle_Centre(double x_coord, double y_coord, double z_coord, double slope_, double intercept_, double radius_, double sector_angle, double ZPosition);// ray properties,circle center value filling in array		vinod	

		void StopProcess();
		void ClearAll();
		int getMovementdirection(double* targetPos);
		void GetBC_Axis(double diameter, int masterL, int LevelNumber);
		void CalculatePtsFor_BC_Axis(double Move_to_CPosAfterCompletion);
		void Get_Cylinder_Y_Position();
		void Reset_BC_Axis();
		void Set_C_Axis();
		void Set_Runout();
		void Reset_RunoutValues();
		void CalculatePts_for_C_Axis_Zero();
		void GetBacktoHomePosition();
		void RunMasterCylinder_Rotation(int Step);
		void CalculatePtsfor_Runout_Master(int ptsCount, double Z_Level);
		void CalculatePtsfor_Runout_Y(int ptsCount, double Z_Level, double dia);

		//Runout Correction Module functions
		void CalculatePtsfor_Runout(int ptsCount, double Z_Level);
		//void Reset_RunoutCorrection();
		bool Run_RunoutCorrection(int StepNumber, int ptsCt, bool Y_Direction);
		void Run_StartToothParameters(int StepNumber);
		void CalculatePts_StartToothCentre();
		void CalculatePts_StartTooth_OD(double Z_Level);
		void CalculatePts_StartToothCentre_2Pts();
		bool GetRunoutforShape(ShapeWithList* cShape, int Runout_Axis_Index, double* answer, bool Sinusoid_Answer);
		void CalculatePts_NoRotation(double Z_Level, int touchDir, bool SkipFirstSteps);
		double GetRunout_at_this_Z(double Z_Level);
		void Move_To_NextHub(bool inY);
		void Hub_Position_7Pts(double TouchSearchSpeed, bool DoFirst_Y_Pt, bool TakePtsClock_Wise);
		void Hub_Position_3Pts(double TouchSearchSpeed, bool DoFirst_Y_Pt, bool TakePtsClock_Wise);
	};
}

//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!

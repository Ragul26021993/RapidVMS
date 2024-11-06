
#pragma once

namespace RWrapper 
{
	public ref class ProbePathPoint
	{
	  public:
		System::Double Pt_X, Pt_Y, Pt_Z;
		bool path;
		bool circularPath;
		bool changeZ;
		double circlel, circlem, circlen;
		int ProbeDir;
	};
};

namespace RWrapper 
{
	public ref class RW_SphereCalibration
	{
		System::Collections::Generic::List<RWrapper::ProbePathPoint^>^ ProbePathPointList;
		cli::array<double> ^Target, ^Feedrate, ^CalibratedOffset, ^ProbeRadius, ^StartPoint, ^ReferenceCenter, ^CurrentGotoPosition, ^CurrentSpherePoints;
		cli::array<int> ^ProbeDir;
		double SphereRadius, UserDefinedProbeRadius;
		bool TakeManualPoint, RefSphereCalibrate;
        int PointTakenCount, CurrentSide, probedirection, StepsCount, LastSide, CurrentProbetype;
		TargetReachedCallback CurrentTargetType;
		static RW_SphereCalibration^ SphereCalibration; 
		void CreatePointsListForLRProbe(bool LeftProbe); // for left right probe
		void CreatePointsListForLRProbeCircular(bool LeftProbe); // for left right probe
		void CreatePointsListForFBProbe(bool FrontProbe); // for front back probe 		
		void CreatePointsListForFBProbeCircular(bool FrontProbe);
		void CreatePointsListForNormalProbe();
		void CreatePointsListForNormalProbeCircular();
		int getMovementdirection(double* targetPos);
		int InspectionType;
		void RaisePointsCountEvent();
		void AddPointsToSelectedShape(int dir, double *pnt);
	public:
		static bool instantiated;
		bool DoingCalibration, TakeProbeHit, CircularInterpolation;

		RW_SphereCalibration(); 
		~RW_SphereCalibration(); 
		static RW_SphereCalibration^ MYINSTANCE(); 

		delegate void PointsCountEventHandler(int PtsCnt, int TotalPts);
		delegate void FinishedSphereCycleHandler();
		//type = 1 for probe vision offset..
		delegate void ProbeOffsetEventHandler(cli::array<double>^ Values, int type);
		event PointsCountEventHandler^ PointsCountEvent;
		event ProbeOffsetEventHandler^ ProbeOffsetEvent;
		event FinishedSphereCycleHandler^ FinishedSphereEvent;
		void SendDrotoNextposition();
		void SendDrotoTakeNextPoint();
		void StartSphereCallibration(cli::array<double, 1>^ values, int inspectionType, int currentSide, int currentProbetype, bool circular, bool addfactor);//, int currentProbetype, bool circular
		void ProbeHitHandle(int dir, double *pnt);
		void CalibrateCurrentSide();
		void PauseCalibration();
		void ContinueCalibration();
		void StopCalibration();
		void ClearAll();
		void StartSphereCallibration_WithStylus(double Sphere_Radius);
		void GetPointsInCircularPath();
		void ChangeHandlerToVision();
		void GotoFocusPosition(double x, double y, double z);
		void DoVisionCalibration(double z);
		void SetCurrentSide(int currSide);
		//void UpdateProbeValuestoDB(int ProbeType);
	};
}

//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
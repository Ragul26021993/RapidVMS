//Class Used for Circular Interpolation ...

#pragma once
namespace RWrapper 
{
public ref class RW_CircularInterPolation
{
	static RW_CircularInterPolation^ CircularInterPolationInstance; //hold the instance..
	cli::array<double>^ IntermediatePos;
	cli::array<double>^ TargetPos;
	bool InterFlag, TargetFlag, LineArcFlag;
	PointCollection* CloudPts_HeightGauge;
	ref struct PathPts
	{
		cli::array<double>^ PtsArray;
		bool Circular;
		PathPts(){PtsArray = gcnew cli::array<double>(6);}
		~PathPts(){ }	
	};
	 cli::array<double>^ IntPtsArray;
	 System::Collections::Generic::List<PathPts^>^ PathPtsColl;
	 int CommandCount;
	 double StartPosition;
	 void CreateGotoCommandList();
	 ShapeWithList* GetNextShape(ShapeWithList* CurrentShape, std::list<int> IdList, double* NextPt, bool* returnFlag);
	 void AddPathtoList(ShapeWithList* Psh, double* NextPt);

public:
	bool AddPointsFlag;
	double  HeightGaugeMax, HeightGaugeMin;
	delegate void DisableAddPointFlagEvent();
	event DisableAddPointFlagEvent^ DisableAddpPointEvent;
	RW_CircularInterPolation(); //Default constructor..
	~RW_CircularInterPolation(); //Destructor.. Clear the memory..
	static RW_CircularInterPolation^ MYINSTANCE(); //Expose the class instance..
	void SetIntermediate_TargetPosition(bool Intermediateflag);
	void StartCircularInterpolation(double angle, int NumberOfHops);
	void ClearAll();
	void deleteTempCloudPtsColl();
	void SetAddPointFlag_HeightGauge(bool flag);
	void StopAddingPtstoSelectedShapes(bool DisableAddPoint);
	void AddPointToSelectedCloudPoints(double x, double y, double z);
	void Handle_CircularGotoCommand();
	void AddPathPoint(int PtNumber);
	void AddLine_CircularPath(bool CircularFlag);
	void ResetPath();
	void CreateInterPolationCommands(bool DxfShapeFlag, bool LineArc);
	void GotoCommandForNextShape(bool firsttime);
	
};
}

//Created by Rahul Singh Bhadauria on 10 Jan 13.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
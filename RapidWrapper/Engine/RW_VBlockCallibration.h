//Class Used for autoCalibration..
//All the button actions, event handling from / to the front end for the autocalibration part..
#pragma once
namespace RWrapper 
{
public ref class RW_VBlockCallibration
{
	static RW_VBlockCallibration^ VBlockCalInstance; //hold the instance..
	
private:
	bool UpperlineFlag, LowerLineFlag, AngleBisectorLineFlag, HolderCalibration, CallibrationDoneFlag, PauseCallibration;
	int UpperLineId, LowerLineId, AngleBisectorLineId;
	double Cradius, Axis_Y1value, Axis_Y2value, Axis_Zvalue, Axis_X1value, Axis_X2value, IdealRadius, new_CylinderRadius, newZ_Value, FixtureCylinderRadius, CylinderOverlapDist;
	cli::array<double> ^TargetValue;
	void GetXYCoordinate(int i, double* XY_Coordinate);
public:
	delegate void VblockCallibrationEventHandler(bool CalibrationDone);
	event VblockCallibrationEventHandler^ VblockCallibrationCompletedEvent;
	RW_VBlockCallibration(); //Default constructor..
	~RW_VBlockCallibration(); //Destructor.. Clear the memory..
	static RW_VBlockCallibration^ MYINSTANCE(); //Expose the class instance..
	void create_AngleBisector();
	void Get_ZvalueOfCylinderAxis();
	void Handle_AxisLock();
	void ClearAll();
	void Create_AxisLine(double dia);
	void SetCylinderRadius(double dia, bool holderType, double fixedCylinderDia, double CylinderOverlap);
	void create_Line(bool firstLine);
	void SetFocus_Zvalue();
	void GotoCommandForVblock(int i);
	void ContinuePauseVblockCallibration(bool PauseFlag);
};
}

//Created by Rahul Singh Bhadauria.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
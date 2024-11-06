//Step Length Measurement Handler Class.. For the Intelligent Measurement Mode..
//Inherited from Mouse Handler.
//According to the Current Light settings this will intialise a Local handler..
//Once the user selects a handler, that will be used till the end of the tool..
#pragma once
#include "..\Handlers\MouseHandler.h"

class SM_PointAngleMeasurementHandler:
	public MouseHandler
{
private:
	Shape* AxisShape;
	Shape *ParentShape1, *ParentShape2, *ParentShape3;
	DimBase* CurrentMeasure; //Current Angle measurement..
	
	int CurrentMeasurementShCount;
	bool AddNewShapeForMeasurement, FirstTime;
	bool MeasurementAdded, AddAngleMeasurement;
	
	
	void init(); //Initialise default settings..
	void InitialiseNewDistMeasure(); //Initialsie new angle measurement
	void AddDistanceMeasurement(); //Add Angle Measurement Object..
	void CreateAngleMeasurement(Shape* Cshape1, Shape* Cshape2, bool AxisLine);
	void AddShapeMeasureRelationShip(Shape* CShape);
	void CheckAllTheConditions(); // Check all the conditions during mouse down..
	void CheckRightClickConditions(); // Check the conditions for right mouse click...
public:
	RapidEnums::RAPIDHANDLERTYPE CurrentHandlerType;
	SM_PointAngleMeasurementHandler(); //default constructor..
	~SM_PointAngleMeasurementHandler(); //destructor... clear the memory related..!
	//All Virtual functions..//
	//Select the Required Handler.. Local..!! User can choose any Point Picking handler..
	virtual void ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle);
	virtual void mouseMove();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void LmouseUp(double x, double y);
	virtual void LmouseDown();
	virtual void RmouseDown(double x, double y);
	virtual void MmouseDown(double x, double y);
	virtual void LmaxmouseDown();
	virtual void MouseScroll(bool flag);
	virtual void EscapebuttonPress();
	void RotationStopedAddMeasure();
};

//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!!
//Developed by Amit mishra... 
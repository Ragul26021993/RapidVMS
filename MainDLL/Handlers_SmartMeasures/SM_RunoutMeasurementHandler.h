//Step Length Measurement Handler Class.. For the Intelligent Measurement Mode..
//Inherited from Mouse Handler.
//According to the Current Light settings this will intialise a Local handler..
//Once the user selects a handler, that will be used till the end of the tool..
#pragma once
#include "..\Handlers\MouseHandler.h"
class DimBase;

class SM_RunoutMeasurementHandler:
	public MouseHandler
{
private:
	list<Shape*> StepLines; //Parent shapes for the measurement...
	DimBase* CurrentMeasure; //Current Angle measurement..
	
	int CurrentMeasurementShCount, StepCount, RunOutType;
	bool AddNewShapeForMeasurement, FirstTime;
	bool MeasurementAdded;
	RapidEnums::RAPIDHANDLERTYPE CurrentHandlerType;
	
	void init(); //Initialise default settings..
	void InitialiseNewDistMeasure(); //Initialsie new angle measurement
	void CreateLineParelleltoToolAxis();
	void AddDistanceMeasurement(); //Add Angle Measurement Object..
	void AddShapeMeasureRelationShip(Shape* CShape);
	void CheckAllTheConditions(bool dontcheckformax = false); // Check all the conditions during mouse down..
	void CheckRightClickConditions(); // Check the conditions for right mouse click...

public:
	//RunOutType 0 Axial RunOut, 1 RadialRunOut, 2 RadialRunOutParellel;
	SM_RunoutMeasurementHandler(int stepcnt, int Type); //default constructor..
	~SM_RunoutMeasurementHandler(); //destructor... clear the memory related..!
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
//Developed by Sathyanarayana... @Sep 2011
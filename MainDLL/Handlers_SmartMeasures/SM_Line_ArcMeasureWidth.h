//Step Length Measurement Handler Class.. For the Intelligent Measurement Mode..
//Inherited from Mouse Handler.
//According to the Current Light settings this will intialise a Local handler..
//Once the user selects a handler, that will be used till the end of the tool..
#pragma once
#include "..\Handlers\MouseHandler.h"

class SM_Line_ArcMeasureWidth:
	public MouseHandler
{
private:
	Shape* ParentShape1; //Parent shapes for the measurement...
	Shape* ParentShape2;
	DimBase* CurrentMeasure; //Current Angle measurement..
	
	int CurrentMeasurementShCount;
	bool AddNewShapeForMeasurement;
	bool MeasurementAdded;
	
	
	void init(); //Initialise default settings..
	void InitialiseNewDistMeasure(); //Initialsie new angle measurement
	void AddDistanceMeasurement(); //Add Angle Measurement Object..
	void AddShapeMeasureRelationShip(Shape* CShape);
	void CheckAllTheConditions(); // Check all the conditions during mouse down..
	void CheckRightClickConditions(); // Check the conditions for right mouse click...
public:
	RapidEnums::RAPIDHANDLERTYPE CurrentHandlerType;
	SM_Line_ArcMeasureWidth(); //default constructor..
	~SM_Line_ArcMeasureWidth(); //destructor... clear the memory related..!
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
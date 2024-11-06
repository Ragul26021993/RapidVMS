//Angle Measure Handler Class.. For the Intelligent Measurement Mode..
//Inherited from Mouse Handler.
//According to the Current Light settings this will intialise a Local handler..
//Once the user selects a handler, that will be used till the end of the tool..
#pragma once
#include "..\Handlers\MouseHandler.h"

class SM_AngleMeasureWithYAxis:
	public MouseHandler
{
private:
	Shape* ParentShape1; // Parent shapes for the measurement...
	Shape* ParentShape2;
	DimBase* CurrentMeasure; // Current Angle measurement..

	int AngleMeasureQuad;
	bool AddNewShapeForMeasurement, FirstTime, SelectShapeForMeasurement;
	bool MeasurementAdded, AddAngleMeasurement;
	RapidEnums::RAPIDHANDLERTYPE CurrentHandlerType;
	
	void init(); //Initialise default settings..
	void InitialiseNewAngleMeasure();
	void AddLine_AngleMeasurement();
	void AngleMeasurement_MouseMove();
	void SetMeasurementPosition();
	void CheckAllTheConditions();
	void CheckRightClickConditions();

public:
	SM_AngleMeasureWithYAxis(); //default constructor..
	~SM_AngleMeasureWithYAxis(); //destructor... clear the memory related..!
	//Virtual functions..//
	//Select the Required Handler.. Local..!! User can choose any Point Picking handler..
	virtual void ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle);
	virtual void mouseMove();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void LmouseDown();
	virtual void LmouseUp(double x, double y);
	virtual void RmouseDown(double x,double y);
	virtual void LmaxmouseDown();
	virtual void MouseScroll(bool flag);
	virtual void EscapebuttonPress();
};

//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!!
//Developed by Sathyanarayana... @Sep 2011
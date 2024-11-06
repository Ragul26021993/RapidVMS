//Rake Angle Measurement Handler Class.. For the Intelligent Measurement Mode..
//Inherited from Mouse Handler.
//According to the Current Light settings this will intialise a Local handler..
//Once the user selects a handler, that will be used till the end of the tool..
#pragma once
#include "..\Handlers\MouseHandler.h"

class SM_RakeAngleMeasureHandler:
	public MouseHandler
{
private:
	Shape* ParentShape1; //Parent shapes for the measurement...
	Shape* ParentShape2;
	DimBase* CurrentMeasure; //Current Angle measurement..
	
	int CurrentMeasurementShCount, AngleMeasureQuad;
	bool AddNewMeasurement, MeasurementAdded;
	RapidEnums::RAPIDHANDLERTYPE CurrentHandlerType;
		
	void init(); //Initialise default settings..
	void InitialiseNewRakeAngleMeasure(); //Initialsie new angle measurement
	void AddRakeAngleMeasurement(); //Add Angle Measurement Object..
	void RakeAngleMeasurement_MouseMove(); // To set the measurement draw positions...
	void CheckAllTheConditions(); // Check all the conditions during mouse down..
	void CheckRightClickConditions(); // Check the conditions for right mouse click...

public:
	SM_RakeAngleMeasureHandler(); //default constructor..
	~SM_RakeAngleMeasureHandler(); //destructor... clear the memory related..!
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
	virtual void SetAnyData(double *data);
};

//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!!
//Developed by Sathyanarayana... @Sep 2011
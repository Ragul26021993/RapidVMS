//Spline Measure Handler Class.. For the Intelligent Measurement Mode..
//Inherited from Mouse Handler.
//According to the Current Light settings this will intialise a Local handler..
//Once the user selects a handler, that will be used till the end of the tool..#pragma once
#include "..\Handlers\MouseHandler.h"

class SM_SplineMeasurementHandler:
	public MouseHandler
{
private:
	Shape* ParentShape1; // Parent shapes for the measurement...
	DimBase* CurrentMeasure; // Current Angle measurement..
	int SplineCount;
	bool AddNewShapeForMeasurement, FirstTime;
	bool AddDiaMeasurement;
	
	void init(); //Initialise default settings..
	void InitialiseNewDiaMeasure();
	void AddDia_Measurement();
	void SetMeasurementPosition();
	void CheckAllTheConditions();
	void CheckRightClickConditions();

public:
	SM_SplineMeasurementHandler(); //default constructor..
	~SM_SplineMeasurementHandler(); //destructor... clear the memory related..!
	//Virtual functions..//
	//Select the Required Handler.. Local..!! User can choose any Point Picking handler..
	virtual void ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle);
	virtual void mouseMove();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void LmouseUp(double x, double y);
	virtual void LmouseDown();
	virtual void RmouseDown(double x, double y);
	virtual void LmaxmouseDown();
	virtual void MmouseDown(double x, double y);
	virtual void MouseScroll(bool flag);
	virtual void EscapebuttonPress();
	virtual void SetAnyData(double *data);
};

//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!!
//Developed by Sathyanarayana.
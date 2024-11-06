#pragma once
#include "..\Handlers\MouseHandler.h"

class SM_AutoGrooveMeasurementHandler:
	public MouseHandler
{
private:
	Shape* ParentShape1; //Parent shapes for the measurement...
	DimBase* CurrentMeasure; //Current Angle measurement..
	
	bool FirstClick, AddNewShapeMeasurement, DoMeasurement;
	RapidEnums::RAPIDHANDLERTYPE CurrentHandlerType;
	
	void init(); //Initialise default settings..
	void InitialiseNewDistMeasure(); //Initialsie new angle measurement
	
public:
	SM_AutoGrooveMeasurementHandler(bool InitialiseShape = true); //default constructor..
	~SM_AutoGrooveMeasurementHandler(); //destructor... clear the memory related
	//below is for having chamfer distance measurement called from line-arc action class
	void Addmeasurement(TCHAR* Name, RapidEnums::MEASUREMENTTYPE CurrentMeasurementType, bool maxwidth = true);
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
};

//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!!
//Developed by Sathyanarayana... @Sep 2011
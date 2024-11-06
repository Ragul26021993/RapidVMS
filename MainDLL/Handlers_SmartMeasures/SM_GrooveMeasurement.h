#pragma once
#include "..\Handlers\MouseHandler.h"

class SM_GrooveMeasurement:
	public MouseHandler
{
private:
	Shape* ParentShape1; //Parent shapes for the measurement...
	Shape* ParentShape2;
	Shape* ParentShape3;
	Shape* ParentShape4;
	Shape* ParentShape5;
	DimBase* CurrentMeasure; //Current Angle measurement..
	
	int CurrentMeasurementShCount;
	bool AddNewShapeForMeasurement, FirstTime;
	bool MeasurementAdded, AddAngleMeasurement;
	RapidEnums::RAPIDHANDLERTYPE CurrentHandlerType;
	
	void init(); //Initialise default settings..
	void InitialiseNewDistMeasure(); //Initialsie new angle measurement
	void AddDistanceMeasurement(); //Add Angle Measurement Object..
	void AddShapeMeasureRelationShip(Shape* CShape);
	void CheckAllTheConditions(); // Check all the conditions during mouse down..
	void CheckRightClickConditions(); // Check the conditions for right mouse click...

	void AddPt2PtDistanceMeasure(Vector* Pt1, Vector* Pt2, Shape* CShape1, Shape* CShape2, Shape* CShape3, Shape* CShape4, double* MousePt,  bool linear = false);
	void AddLine2Line_AngleMeasure(Shape* CShape1, Shape* CShape2);
	
public:
	SM_GrooveMeasurement(bool InitialiseShape = true); //default constructor..
	~SM_GrooveMeasurement(); //destructor... clear the memory related..!
	//below is for having chamfer distance measurement called from line-arc action class
	void call_Groove_dist_measurement(Shape* csh1, Shape* csh2, Shape* csh3,  Shape* csh4, Shape* csh5);
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
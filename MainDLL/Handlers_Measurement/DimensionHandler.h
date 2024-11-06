//Dimension Handler to Handle the all types of measurement type..i.e. between the shapes...//
//Whenever user clicks on Measurement button.. this handler initialises!!
//Developed by Sathyanarayana...
//F * * K.., lot of conditions!! All Shape Types for left click and right click options are handled here..
//Say around 30+ measurent types are handled w.r.t to shape selected.

#pragma once
#include "..\Handlers\MouseHandler.h"
#include "..\Measurement\DimBase.h"

class DimensionHandler:
	public MouseHandler
{
//Variables used for measurements..///

private:
	Shape *Cshape[4], *Related_Pt1[2], *Related_Pt2[2]; // Pointer to Selected/related Shape.
	Vector* vPointer[2]; //Pointer to the selected Point.
	int ShapeCount, PointCount, WrongProceedureCnt;
	bool RightClick, MeasureDir; //Used for direction,pp, rightclick etc.
	double CSline[6], AngularityLand, AngularityAngle;
	RapidEnums::CURRENTRIGHTCLICKEDSHAPE CurrentRClickType; 
	Shape* CurrentPCdCircle;
	
	void init();
	void Handle_RightClickMode(double x, double y, double z);
	void CompareSelectedEntities(double x, double y, double z);
	bool CompareShapesFor3DTP();
	void AddMeasurement_TwoShapes(double x, double y, double z);
	void AddMeasurement_OneShapeOnePoint(double x, double y, double z);
	void AddMeasurement_TwoPoints(double x, double y, double z);
	void AddShapeMeasureRelationShip(Shape* CShape);
	void CalculateDigitalMicrometer(double x, double y, double z);

	bool AddMeasurement_RightClickOnLine();
	bool AddMeasurement_RightClickOnCircle();
	bool AddMeasurement_RightClickOnLine3D();
	bool AddMeasurement_RightClickOnPlane();
	bool AddMeasurement_RightClickOnCylinder();
	bool AddMeasurement_RightClickOnSphere();
	bool AddMeasurement_RightClickOnCone();

	void ClickedonWrongPoint();
	void ClickedonWrongShape();
	void WrongProceedureFollowed();

	bool Check2DMeasurement_TwoShapes();
	bool Check2DMeasurement_OneShapeOnePoint();
	bool Check2DMeasurement_TwoPoints();
	bool CheckShapeSelected_PCD(Shape* CShape);
	void Handle_MouseDownForPcdMeasure();
	
	bool RightClickOnPoint();
	bool RightClickOnLine();
	bool RightClickOnCircle();
	bool RightClickOnLine3D();
	bool RightClickOnPlane();
	bool RightClickOnCylinder();
	bool RightClickOnSphere();
	bool RightClickOnPerimeter();
	bool RightClickOnCone();
	bool RightClickOnCircle3D();
	bool RightClickOnCloudPoints();
	void Add2DGDnTMeasurement();
	void AddTPGDnTMeasurement(double x, double y, double z);
	void AddTPGDnTMeasurementForPoint(double x, double y, double z);
	void Add3DTPGDnTMeasurement(double x, double y, double z, bool ForPoint);
	void Add3DGDnTMeasurement();

	void drawSelectedShapesOnVideo(double wupp);
	void drawSelectedShapesOnRcad(double wupp);
	void ResetShapeHighlighted();
public:
	DimBase* dim; //Current Dimension...
	RapidEnums::MEASUREMENTTYPE CurrentMeasurementType;
	DimensionHandler();
	virtual ~DimensionHandler();
	
	//Virtual function...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void RmouseDown(double x, double y);
	virtual void draw(int windowno, double WPixelWidth);
	virtual void keyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void EscapebuttonPress();
	virtual void SetAnyData(double *data);

	bool SetCurrentMeasurementTypeForRightClick(RapidEnums::MEASUREMENTTYPE Cmeasuretype, int type);
	void SetTrueCenter(double x, double y, int en, bool forPoint);
	void SetTrueCenterPoint(double x, double y, int en);
	void SetTrueCenter3D(double x, double y, int en, bool forPoint);
	void MeasurementForHighlighted_Shape(int MeasureType, bool AutomateFixMeasure);
	void HighlightShapeForMeasurement(int id);
	void HighlightShapeRightClickForMeasurement(int id);
};
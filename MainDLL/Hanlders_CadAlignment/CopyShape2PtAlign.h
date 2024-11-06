#pragma once
#include "CadAlignHandler.h"
#include <map>

class DimBase;
class CopyShape2PtAlign:
	public CadAlignHandler
{
	Vector Difference, TransPt, points[4];
	double Transform_Matrix[9];
	void CreateCopyOfShapes_Measure_Action();
	void getCoordinateFor_NextShape(bool FirstTime);
	void getShape_Parameter(bool firsttime);
	void UpdateDerivedShape();
	void CreateActionForShapes();
	void CreateMeasurement();
	map<int, int> ShapeMeasure_relation;
	std::list<Shape*>::iterator  Shapeitr;
	std::list<Shape*> ShapeTypeList;
	std::list<DimBase*> MeasurementTypeList;
public:
	CopyShape2PtAlign();
	~CopyShape2PtAlign();
protected:
	virtual void SelectPoints_Shapes();
	virtual void Align_Finished();
	virtual void drawSelectedShapesOndxf();
	virtual void drawSelectedShapesOnvideo();
	virtual void Align_mouseMove(double x, double y);
	virtual void HandlePartProgramData();
};

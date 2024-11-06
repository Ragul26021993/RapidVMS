#pragma once
#include <list>
#include "Raction.h"

class ShapeWithList;
class BaseItem;
class Shape;

class CloudComparisionAction:
	public RAction
{
	std::list<Shape*> DeviationShape;
	std::list<BaseItem*> DeviationMeasure;
	ShapeWithList* ParentShape1, *ParentShape2, *ParentShape3;
	bool ArcBestFit, ClosedLoop, SurfaceOnFlag;
	int BestFitPtsCount, MeasurementsCount;
	double Interval;
	
public:
	//Constructor and destructor....//
	CloudComparisionAction();
	~CloudComparisionAction();

	//Add / remove the action...//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();
	static void SetCloudCompareParam(double Angle, ShapeWithList* PShape1, ShapeWithList* PShape2, ShapeWithList* PShape3, bool ArcBestFit, int NumberOfPts, std::list<Shape*> CreatedShapeList);
	static void SetCloudCompareParam(double interval, ShapeWithList* PShape1, ShapeWithList* PShape2, bool ArcBestFit, int NumberOfPts, std::list<Shape*> CreatedShapeList);
	static void SetCloudCompareParam(bool SurfaceOn, int MeasureCount, ShapeWithList* PShape1, ShapeWithList* PShape2, bool ArcBestFit, int NumberOfPts, std::list<Shape*> CreatedShapeList);
	
	bool GetCloudComparisionParam(std::list<int>* ShapeList, double* interval, int* ParentShapeId, int* NumberOfPts);
	bool GetCloudComparisionParam(std::list<int>* ShapeList, int* MeasureCount, int* ParentShapeId, int* NumberOfPts);
	bool GetClosedLoopFlag();
	bool GetSurfaceLightFlag();
	ShapeWithList* GetThirdParentShape();

	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, CloudComparisionAction&);
	friend wistream& operator>>(wistream&, CloudComparisionAction&);	
};
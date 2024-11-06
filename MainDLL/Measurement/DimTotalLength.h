#pragma once
#include "DimBase.h"
class Shape;
class SinglePoint;
class PointCollection;

class DimTotalLength:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix);
	double center[3];
	int Measurement_Type, PointsToBundle;
	bool ArcBestFitType;
	void init();
	std::list<Shape*> ParentShapeCollection;
	void findProjectedPoint(bool ArcBestFit, int BestFitPts, map<int,SinglePoint*>::iterator SptItem, PointCollection *PtsList, double* ProjectedPt);
public:
	DimTotalLength(TCHAR* myname, std::list<Shape*> ShapeColl, int MeasureType);
	DimTotalLength(bool simply);
	virtual ~DimTotalLength();

	void CalculateMeasurement(bool CalculateAgain = true);
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);
	//Read/ write partprogram data...//
	friend wostream& operator<<(wostream&, DimTotalLength&);
	friend wistream& operator>>(wistream&, DimTotalLength&);
};
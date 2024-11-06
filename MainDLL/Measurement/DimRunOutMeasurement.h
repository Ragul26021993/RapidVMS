//Class used to calculate the point to line distance........///
#pragma once
#include "DimBase.h"
class Shape;
class Vector;

class DimRunOutMeasurement:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix);
	double IntersectionPnts[5];
public:

	list<Shape*> ParentsShape;
	DimRunOutMeasurement(TCHAR* myname = _T("RO"), RapidEnums::MEASUREMENTTYPE mtype = RapidEnums::MEASUREMENTTYPE::DIM_RUNOUTMEASUREMENTFARTHEST);
	DimRunOutMeasurement(bool simply);
	virtual ~DimRunOutMeasurement();

	void CalculateMeasurement();
	
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Write / read part program data...//
	friend wostream& operator<<(wostream&, DimRunOutMeasurement&);
	friend wistream& operator>>(wistream&, DimRunOutMeasurement&);
};
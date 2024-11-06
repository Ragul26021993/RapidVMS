//Class to measure the circle/ arc Diameter... 
#pragma once
#include "DimBase.h"
class Shape;
class Circle;
class PointCollection;

class DimSplineMeasurement:
	public DimBase
{
private:
	double center[3], rad;
	TCHAR* genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE ctype);
	static int splinecnt, splinemic, splinemcc;
	PointCollection *CurrentPointColl;
	Circle* TempCircle;
	void CalculateSplineCount();
	void CalculateSplineMic();
	void CalculateSplineMcc();
	int EnteredSplineCount;
public:
	DimSplineMeasurement(TCHAR* myname, RapidEnums::MEASUREMENTTYPE ctype);
	DimSplineMeasurement(bool simply);
	virtual ~DimSplineMeasurement();

	void CalculateMeasurement(Shape* s1, int userentspcount);
	void ArrangePtsCalculateSplinecount();
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	static void reset();

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimSplineMeasurement&);
	friend wistream& operator>>(wistream&, DimSplineMeasurement&);
	friend void ReadOldPP(wistream& is, DimSplineMeasurement& x);
};
//Class Used To Maintain the Line Angularity Measurements..
#pragma once
#include "DimBase.h"

class Shape;

class DimAngularity:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype);
	void init();
	static int angularityno, parallelityno, perpendicularityno;
	double AngularityAngle, Land;
public:
	DimAngularity(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype);
	DimAngularity(bool simply);
	virtual ~DimAngularity();

	void CalculateMeasurement(Shape *s1, Shape *s2);
	void SetLandAndAngle(double angle, double land);
	//Virtual Functions
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	static void reset();

	//Read/ write partprogram data...//
	friend wostream& operator<<(wostream&, DimAngularity&);
	friend wistream& operator>>(wistream&, DimAngularity&);
	friend void ReadOldPP(wistream& is, DimAngularity& x);
};
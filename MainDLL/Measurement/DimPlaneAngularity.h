#pragma once
#include "DimBase.h"
class Shape;

class DimPlaneAngularity:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype);
	void init();
	static int angularityno, parallelityno, perpendicularityno;
	double AngularityAngle, Land;
	double PlaneMidPt[3];
public:
	DimPlaneAngularity(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype);
	DimPlaneAngularity(bool simply);
	virtual ~DimPlaneAngularity();

	void CalculateMeasurement(Shape *s1, Shape *s2);
	void SetLandAndAngle(double angle, double land);
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	static void reset();

	//Read/ write partprogram data...//
	friend wostream& operator<<(wostream&, DimPlaneAngularity&);
	friend wistream& operator>>(wistream&, DimPlaneAngularity&);
	friend void ReadOldPP(wistream& is, DimPlaneAngularity& x);
};
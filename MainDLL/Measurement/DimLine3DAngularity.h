//Class Used To Maintain the line to line measurement
#pragma once
#include "DimBase.h"
class Shape;

class DimLine3DAngularity:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype);
	static int angularityno, parallelityno, perpendicularityno;
	double AngularityAngle, Land;
	double LineMidPt[3];
	void init();
public:
	DimLine3DAngularity(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype);
	DimLine3DAngularity(bool simply);
	virtual ~DimLine3DAngularity();

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
	friend wostream& operator<<(wostream&, DimLine3DAngularity&);
	friend wistream& operator>>(wistream&, DimLine3DAngularity&);
	friend void ReadOldPP(wistream& is, DimLine3DAngularity& x);
};
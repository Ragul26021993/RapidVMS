//Class Used To Maintain the line to line measurement
#pragma once
#include "DimBase.h"
class Shape;

class DimCylinderAngularity:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype);
	static int angularityno, perpendicularityno, coaxilityno, parallelismNo;
	double AngularityAngle, Land;
	double CylinderParam[7], CylinderEndPoints[6];
	void init();
public:
	DimCylinderAngularity(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype);
	DimCylinderAngularity(bool simply);
	virtual ~DimCylinderAngularity();

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
	friend wostream& operator<<(wostream&, DimCylinderAngularity&);
	friend wistream& operator>>(wistream&, DimCylinderAngularity&);
	friend void ReadOldPP(wistream& is, DimCylinderAngularity& x);
};
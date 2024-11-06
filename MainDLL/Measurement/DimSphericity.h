//Class to measure the circle/ arc dimensions... 
//like center, radius, diameter, arclength, arcangle etc...
#pragma once
#include "DimBase.h"
class Shape;
class DimSphericity:
	public DimBase
{
	//Variable declaration..//
private:
	double SphereParam[4];
	TCHAR* genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype);
	static int sphericitycnt, mccno, micno;
public:
	DimSphericity(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype);
	DimSphericity(bool simply);
	virtual ~DimSphericity();

	void CalculateMeasurement(Shape* s1);
	
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	static void reset();

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimSphericity&);
	friend wistream& operator>>(wistream&, DimSphericity&);
	friend void ReadOldPP(wistream& is, DimSphericity& x);
};
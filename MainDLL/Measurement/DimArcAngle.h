//Class Used to maintain Arc Angle and Arc length Measurement.
#pragma once
#include "DimBase.h"

class Shape;

class DimArcAngle:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype);
	static int arclenno, arcangno;
public:
	DimArcAngle(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype);
	DimArcAngle(bool simply);
	virtual ~DimArcAngle();

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
	friend wostream& operator<<(wostream&, DimArcAngle&);
	friend wistream& operator>>(wistream&, DimArcAngle&);
	friend void ReadOldPP(wistream& is, DimArcAngle& x);
};
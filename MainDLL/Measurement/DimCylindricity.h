#pragma once
#include "DimBase.h"
class Shape;
class DimCylindricity:
	public DimBase
{
private:
	double CylinderParam[7], CylinderEndPoints[6];
	TCHAR* genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype);
	static int cylindricitycnt, micno, mccno;
public:
	DimCylindricity(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype);
	DimCylindricity(bool simply);
	virtual ~DimCylindricity();

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
	friend wostream& operator<<(wostream&, DimCylindricity&);
	friend wistream& operator>>(wistream&, DimCylindricity&);
	friend void ReadOldPP(wistream& is, DimCylindricity& x);
};
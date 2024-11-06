//Class for point to point distance.. normal, linear.//
#pragma once
#include "DimBase.h"
class Vector;
class Shape;

class DimPointToPointDistLAngle:
	public DimBase
{	
private:
	TCHAR* genName(const TCHAR* prefix);
public:
	DimPointToPointDistLAngle(TCHAR* myname = _T("Dist"));
	DimPointToPointDistLAngle(bool simply);
	virtual ~DimPointToPointDistLAngle();

	void CalculateMeasurement(Vector *v1, Vector *v2, Shape* Cshape);
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimPointToPointDistLAngle&);
	friend wistream& operator>>(wistream&, DimPointToPointDistLAngle&);
	friend void ReadOldPP(wistream& is, DimPointToPointDistLAngle& x);
};
#pragma once
#include "DimBase.h"
class Vector;
class Shape;

class DimPointToSphereDistance:
	public DimBase
{
private:
	double SphereParam[4];
	TCHAR* genName(const TCHAR* prefix);
	void init();
public:
	DimPointToSphereDistance(TCHAR* myname = _T("Dist"));
	DimPointToSphereDistance(bool simply);
	virtual ~DimPointToSphereDistance();
	
	void CalculateMeasurement(Shape *s1, Vector* p1);

	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimPointToSphereDistance&);
	friend wistream& operator>>(wistream&, DimPointToSphereDistance&);
	friend void ReadOldPP(wistream& is, DimPointToSphereDistance& x);
};
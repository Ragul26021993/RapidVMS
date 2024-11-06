#pragma once
#include "DimBase.h"
class Shape;

class DimSphereToSphereDistance:
	public DimBase
{
private:
	double Sphere1Param[4], Sphere2Param[4];
	TCHAR* genName(const TCHAR* prefix);
	void init();
public:
	DimSphereToSphereDistance(TCHAR* myname = _T("Dist"));
	DimSphereToSphereDistance(bool simply);
	virtual ~DimSphereToSphereDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimSphereToSphereDistance&);
	friend wistream& operator>>(wistream&, DimSphereToSphereDistance&);
	friend void ReadOldPP(wistream& is, DimSphereToSphereDistance& x);
};
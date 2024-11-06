#pragma once
#include "DimBase.h"
class Shape;

class DimLineToSphereDistance:
	public DimBase
{
private:
	double LineParam[6], SphereParam[4], LineEndPoints[6];
	TCHAR* genName(const TCHAR* prefix);
	void init();
public:
	DimLineToSphereDistance(TCHAR* myname = _T("Dist"));
	DimLineToSphereDistance(bool simply);
	virtual ~DimLineToSphereDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimLineToSphereDistance&);
	friend wistream& operator>>(wistream&, DimLineToSphereDistance&);
	friend void ReadOldPP(wistream& is, DimLineToSphereDistance& x);
};
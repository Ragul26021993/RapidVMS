#pragma once
#include "DimBase.h"
class Shape;

class DimSphereToConeDistance:
	public DimBase
{
private:
	double ConeParam[8], SphereParam[4], ConeEndPts[6];
	TCHAR* genName(const TCHAR* prefix);
	void init();
public:
	DimSphereToConeDistance(TCHAR* myname = _T("Dist"));
	DimSphereToConeDistance(bool simply);
	virtual ~DimSphereToConeDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimSphereToConeDistance&);
	friend wistream& operator>>(wistream&, DimSphereToConeDistance&);
	friend void ReadOldPP(wistream& is, DimSphereToConeDistance& x);
};
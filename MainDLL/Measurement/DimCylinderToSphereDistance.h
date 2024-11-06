#pragma once
#include "DimBase.h"
class Shape;

class DimCylinderToSphereDistance:
	public DimBase
{
private:
	double CylinderParam[7], SphereParam[4], CylinderEndPts[6];
	TCHAR* genName(const TCHAR* prefix);
	void init();
public:
	DimCylinderToSphereDistance(TCHAR* myname = _T("Dist"));
	DimCylinderToSphereDistance(bool simply);
	virtual ~DimCylinderToSphereDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimCylinderToSphereDistance&);
	friend wistream& operator>>(wistream&, DimCylinderToSphereDistance&);
	friend void ReadOldPP(wistream& is, DimCylinderToSphereDistance& x);
};
#pragma once
#include "DimBase.h"
class Shape;
class Vector;

class DimPointToCylinderDistance:
	public DimBase
{
private:
	double CylinderParam[7], CylinderEndPoints[6];
	TCHAR* genName(const TCHAR* prefix);
	void init();
public:
	DimPointToCylinderDistance(TCHAR* myname = _T("Dist"));
	DimPointToCylinderDistance(bool simply);
	virtual ~DimPointToCylinderDistance();
	
	void CalculateMeasurement(Shape *s1, Vector* p1);

	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimPointToCylinderDistance&);
	friend wistream& operator>>(wistream&, DimPointToCylinderDistance&);
	friend void ReadOldPP(wistream& is, DimPointToCylinderDistance& x);
};
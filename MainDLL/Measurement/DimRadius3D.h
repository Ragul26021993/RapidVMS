#pragma once
#include "DimBase.h"
class Shape;

class DimRadius3D:
	public DimBase
{
private:
	double CylinderParam[7], CylinderEndPoints[6];
	TCHAR* genName(const TCHAR* prefix);
public:
	DimRadius3D(TCHAR* myname = _T("Rad"));
	DimRadius3D(bool simply);
	virtual ~DimRadius3D();

	void CalculateMeasurement(Shape* s1);
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);
	
	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimRadius3D&);
	friend wistream& operator>>(wistream&, DimRadius3D&);
	friend void ReadOldPP(wistream& is, DimRadius3D& x);
};
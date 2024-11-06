#pragma once
#include "DimBase.h"
class Shape;

class DimDiameter3D:
	public DimBase
{
private:
	double CylinderParam[7], CylinderEndPoints[6];
	TCHAR* genName(const TCHAR* prefix);
public:
	DimDiameter3D(TCHAR* myname = _T("Dia"));
	DimDiameter3D(bool simply);
	virtual ~DimDiameter3D();

	void CalculateMeasurement(Shape* s1);
	
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimDiameter3D&);
	friend wistream& operator>>(wistream&, DimDiameter3D&);
	friend void ReadOldPP(wistream& is, DimDiameter3D& x);
};
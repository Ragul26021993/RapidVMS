#pragma once
#include "DimBase.h"

class Shape;

class DimAngle3D:
	public DimBase
{
private:
	double ConeParam[8], ConeEndPoints[6];
	TCHAR* genName(const TCHAR* prefix);
	void init();
	double SfLine1[6], SfEndPnts1[6], SfLine2[6], SfEndPnts2[6];
public:
	DimAngle3D(TCHAR* myname = _T("Ang"));
	DimAngle3D(bool simply);
	virtual ~DimAngle3D();

	void CalculateMeasurement(Shape* s1);
	
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimAngle3D&);
	friend wistream& operator>>(wistream&, DimAngle3D&);
	friend void ReadOldPP(wistream& is, DimAngle3D& x);
};
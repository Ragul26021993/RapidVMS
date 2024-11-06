//Class to measure the circle/ arc Diameter... 
#pragma once
#include "DimBase.h"
class Shape;

class DimDiameter2D:
	public DimBase
{
private:
	double center[3];
	TCHAR* genName(const TCHAR* prefix);
public:
	DimDiameter2D(TCHAR* myname = _T("Dia"));
	DimDiameter2D(bool simply);
	virtual ~DimDiameter2D();

	void CalculateMeasurement(Shape* s1);
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimDiameter2D&);
	friend wistream& operator>>(wistream&, DimDiameter2D&);
	friend void ReadOldPP(wistream& is, DimDiameter2D& x);
};
//Class to measure the circle/ arc Radius..!
#pragma once
#include "DimBase.h"
class Shape;

class DimRadius2D:
	public DimBase
{
private:
	double center[3], LastMPositionX, LastMPositionY;
	TCHAR* genName(const TCHAR* prefix);
public:
	DimRadius2D(TCHAR* myname = _T("Rad"));
	DimRadius2D(bool simply);
	virtual ~DimRadius2D();

	void CalculateMeasurement(Shape* s1);
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimRadius2D&);
	friend wistream& operator>>(wistream&, DimRadius2D&);
	friend void ReadOldPP(wistream& is, DimRadius2D& x);
};
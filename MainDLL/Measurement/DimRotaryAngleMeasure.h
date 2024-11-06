//Class Used To Maintain the line to line measurement
#pragma once
#include "DimBase.h"
class Shape;

class DimRotaryAngleMeasure:
	public DimBase
{
private:
	double Endpoints[8];
	double angle1, intercept1,angle2, intercept2;
	static int Rotaryanglecnt;
	TCHAR* genName(const TCHAR* prefix);

public:
	DimRotaryAngleMeasure(TCHAR* myname = _T("R_Ang"));
	DimRotaryAngleMeasure(bool simply);
	virtual ~DimRotaryAngleMeasure();

	void CalculateMeasurement(Shape *s1, Shape *s2);
	
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	static void reset();

	//Read/ write partprogram data...//
	friend wostream& operator<<(wostream&, DimRotaryAngleMeasure&);
	friend wistream& operator>>(wistream&, DimRotaryAngleMeasure&);
	friend void ReadOldPP(wistream& is, DimRotaryAngleMeasure& x);
};
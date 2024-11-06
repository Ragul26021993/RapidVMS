#pragma once
#include "DimBase.h"
class Shape;

class DimFlatness:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix);
	static int flatnesscnt;
	double PlaneMidPt[4];
public:
	DimFlatness(TCHAR* myname = _T("Flt"));
	DimFlatness(bool simply);
	virtual ~DimFlatness();

	void CalculateMeasurement(Shape* s1);
	
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	static void reset();

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimFlatness&);
	friend wistream& operator>>(wistream&, DimFlatness&);
	friend void ReadOldPP(wistream& is, DimFlatness& x);
};
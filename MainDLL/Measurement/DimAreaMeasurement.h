#pragma once
#include "DimBase.h"

class DimAreaMeasurement:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix);
	double center[3];
	void init();
public:
	DimAreaMeasurement(TCHAR* myname);
	DimAreaMeasurement(bool simply);
	virtual ~DimAreaMeasurement();

	void CalculateMeasurement(Shape* s1);
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);
	//Read/ write partprogram data...//
	friend wostream& operator<<(wostream&, DimAreaMeasurement&);
	friend wistream& operator>>(wistream&, DimAreaMeasurement&);
};
#pragma once
#include "DimBase.h"
class Shape;

class DimAxisLength:
	public DimBase
{
private:
	double Conics3DParam[17], Max, Min;
	TCHAR* genName(const TCHAR* prefix);
public:
	DimAxisLength(TCHAR* myname = _T("Ang"));
	DimAxisLength(bool simply);
	virtual ~DimAxisLength();

	void CalculateMeasurement(Shape* s1);
	
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimAxisLength&);
	friend wistream& operator>>(wistream&, DimAxisLength&);
	friend void ReadOldPP(wistream& is, DimAxisLength& x);
};
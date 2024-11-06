#pragma once
#include "DimBase.h"
class Shape;

class DimConcentricity:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix);
	static int ConcenCnt;
public:
	DimConcentricity(TCHAR* myname = _T("Concen"));
	DimConcentricity(bool simply);
	virtual ~DimConcentricity();

	void CalculateMeasurement(Shape* s1, Shape* s2);
	
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	static void reset();

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimConcentricity&);
	friend wistream& operator>>(wistream&, DimConcentricity&);
	friend void ReadOldPP(wistream& is, DimConcentricity& x);
};
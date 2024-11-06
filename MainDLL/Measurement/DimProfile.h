#pragma once
#include "DimBase.h"
class Shape;

class DimProfile :
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix);
	static int ProfileCnt;

	Shape *OriShape, *OriDatum1, *OriDatum2, *OriDatum3;

public:
	DimProfile(TCHAR* myname = _T("Pfl"));
	DimProfile(bool simply);
	virtual ~DimProfile();

	void CalculateMeasurement(Shape* s1, Shape* s2, Shape* s3, Shape* s4, Shape* refShape);

	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	static void reset();

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimProfile&);
	friend wistream& operator>>(wistream&, DimProfile&);
	friend void ReadOldPP(wistream& is, DimProfile& x);
};
#pragma once
#include "DimBase.h"
class Shape;
class Vector;


class DimPointToConeDistance:
	public DimBase
{
private:
	double ConeParam[7], ConeEndPoints[6];
	TCHAR* genName(const TCHAR* prefix);
	void init();
	double SfLine1[6], SfEndPnts1[6], SfLine2[6], SfEndPnts2[6];
public:
	DimPointToConeDistance(TCHAR* myname = _T("Dist"));
	DimPointToConeDistance(bool simply);
	virtual ~DimPointToConeDistance();
	
	void CalculateMeasurement(Shape *s1, Vector* p1);

	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimPointToConeDistance&);
	friend wistream& operator>>(wistream&, DimPointToConeDistance&);
	friend void ReadOldPP(wistream& is, DimPointToConeDistance& x);
};
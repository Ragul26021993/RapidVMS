//Class used to calculate the dmeasurement between point and Circle/arc.
#pragma once
#include "DimBase.h"
class Shape;
class Vector;

class DimPointToCircleDistance:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix);
	void init();
public:
	DimPointToCircleDistance(TCHAR* myname = _T("Dist"));
	DimPointToCircleDistance(bool simply);
	virtual ~DimPointToCircleDistance();

	void CalculateMeasurement(Shape *s1, Vector* p1);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimPointToCircleDistance&);
	friend wistream& operator>>(wistream&, DimPointToCircleDistance&);
	friend void ReadOldPP(wistream& is, DimPointToCircleDistance& x);
};
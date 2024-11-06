//Class used to calculate the dmeasurement between Circle and Circle/Arc..
#pragma once
#include "DimBase.h"
class Shape;

class DimCircleToCircleDistance:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix);
	void init();
	int CirclePositon[2];
public:
	DimCircleToCircleDistance(TCHAR* myname = _T("Dist"));
	DimCircleToCircleDistance(bool simply);
	virtual ~DimCircleToCircleDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimCircleToCircleDistance&);
	friend wistream& operator>>(wistream&, DimCircleToCircleDistance&);
	friend void ReadOldPP(wistream& is, DimCircleToCircleDistance& x);
};
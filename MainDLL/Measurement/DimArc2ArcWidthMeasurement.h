//Class used to calculate the dmeasurement between Circle and Circle/Arc..
#pragma once
#include "DimBase.h"
class Shape;

class DimArc2ArcWidthMeasurement:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix);
	void init();
	int CirclePositon[2];
	double MeasurePoints[6];
	bool findPosition;
public:
	DimArc2ArcWidthMeasurement(TCHAR* myname = _T("Dist"));
	DimArc2ArcWidthMeasurement(bool simply);
	virtual ~DimArc2ArcWidthMeasurement();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimArc2ArcWidthMeasurement&);
	friend wistream& operator>>(wistream&, DimArc2ArcWidthMeasurement&);
};
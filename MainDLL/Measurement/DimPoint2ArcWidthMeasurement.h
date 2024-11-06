//Class used to calculate the dmeasurement between arc and point width..
#pragma once
#include "DimBase.h"
class Shape;

class DimPoint2ArcWidthMeasurement:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix);
	void init();
	int CirclePositon;
	double MeasurePoints[3];
	bool findPosition;
public:
	DimPoint2ArcWidthMeasurement(TCHAR* myname = _T("Dist"));
	DimPoint2ArcWidthMeasurement(bool simply);
	virtual ~DimPoint2ArcWidthMeasurement();
	
	void CalculateMeasurement(Shape *s1, Vector* p1);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimPoint2ArcWidthMeasurement&);
	friend wistream& operator>>(wistream&, DimPoint2ArcWidthMeasurement&);
};
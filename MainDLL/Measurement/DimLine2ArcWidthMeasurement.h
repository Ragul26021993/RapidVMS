//Class used to calculate the dmeasurement between circle and Line..
#pragma once
#include "DimBase.h"
class Shape;

class DimLine2ArcWidthMeasurement:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix);
	void init();
	int CirclePositon;
	double MeasurePoints[6];
	bool findPosition;
public:
	DimLine2ArcWidthMeasurement(TCHAR* myname = _T("Dist"));
	DimLine2ArcWidthMeasurement(bool simply);
	virtual ~DimLine2ArcWidthMeasurement();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimLine2ArcWidthMeasurement&);
	friend wistream& operator>>(wistream&, DimLine2ArcWidthMeasurement&);
};
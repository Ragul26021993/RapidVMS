//Class used to calculate the dmeasurement between circle and Line..
#pragma once
#include "DimBase.h"
class Shape;

class DimCircleToLineDistance:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix);
	void init();
private:
	enum CIRCLELINEMESURETYPE
	{
		CIRCLE_FLINE,
		CIRCLE_XRAY,
		CIRCLE_XLINE
	}CLMeasuretype;
public:
	DimCircleToLineDistance(TCHAR* myname = _T("Dist"));
	DimCircleToLineDistance(bool simply);
	virtual ~DimCircleToLineDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimCircleToLineDistance&);
	friend wistream& operator>>(wistream&, DimCircleToLineDistance&);
	friend void ReadOldPP(wistream& is, DimCircleToLineDistance& x);
};
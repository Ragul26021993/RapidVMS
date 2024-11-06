#pragma once
#include "DimBase.h"
class Shape;

class DimIntersectionShapeToPlaneDistance:
	public DimBase
{
private:
	double PlaneParam[4], PointToUse[3], PlaneEndPts[12];
	TCHAR* genName(const TCHAR* prefix);
	void init();

public:
	DimIntersectionShapeToPlaneDistance(TCHAR* myname = _T("Dist"));
	DimIntersectionShapeToPlaneDistance(bool simply);
	virtual ~DimIntersectionShapeToPlaneDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimIntersectionShapeToPlaneDistance&);
	friend wistream& operator>>(wistream&, DimIntersectionShapeToPlaneDistance&);
	friend void ReadOldPP(wistream& is, DimIntersectionShapeToPlaneDistance& x);
};
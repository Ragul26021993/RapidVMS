#pragma once
#include "DimBase.h"
class Shape;

class DimPlaneToPlaneDistance:
	public DimBase
{
private:
	double Plane1Param[4], Plane2Param[4], Plane1EndPoints[12], Plane2EndPoints[12], IntersectionLine[6];
	void init();
	bool setMeasureName;
	void SetMeasureName();
	enum PLANEMEASURETYPE
	{
		DISTANCEONLY,
		ANGLEONLY,
	}PlaneMtype;

public:

	DimPlaneToPlaneDistance(bool simply);
	virtual ~DimPlaneToPlaneDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimPlaneToPlaneDistance&);
	friend wistream& operator>>(wistream&, DimPlaneToPlaneDistance&);
	friend void ReadOldPP(wistream& is, DimPlaneToPlaneDistance& x);
};
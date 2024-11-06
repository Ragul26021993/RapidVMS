#pragma once
#include "DimBase.h"
class Shape;

class DimLineToPlaneDistance:
	public DimBase
{
private:
	double LineParam[6], PlaneParam[4], LineEndPoints[6], PlaneEndPoints[12];
	void init();
	bool setMeasureName;
	void SetMeasureName();

	enum LINE_PLANEMEASURETYPE
	{
		DISTANCE,
		ANGLE,
	}Line_PlaneMType;

public:

	DimLineToPlaneDistance(bool simply);
	virtual ~DimLineToPlaneDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimLineToPlaneDistance&);
	friend wistream& operator>>(wistream&, DimLineToPlaneDistance&);
	friend void ReadOldPP(wistream& is, DimLineToPlaneDistance& x);
};
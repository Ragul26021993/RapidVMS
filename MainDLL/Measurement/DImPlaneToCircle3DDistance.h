#pragma once
#include "DimBase.h"
class Shape;

class DimPlaneToCircle3DDistance:
	public DimBase
{
private:
	double Plane2Param[4], Circle3D[7], Plane2EndPoints[12], CirclePln[4];
	void init();
	bool setMeasureName;
	void SetMeasureName();

	enum CIRCLE3D_PLANEMEASURETYPE
	{
		DISTANCE,
		ANGLE,
	}Circle3D_PlaneMType;

public:

	DimPlaneToCircle3DDistance(bool simply);
	virtual ~DimPlaneToCircle3DDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimPlaneToCircle3DDistance&);
	friend wistream& operator>>(wistream&, DimPlaneToCircle3DDistance&);
	friend void ReadOldPP(wistream& is, DimPlaneToCircle3DDistance& x);
};
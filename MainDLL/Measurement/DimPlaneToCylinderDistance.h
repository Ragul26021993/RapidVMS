#pragma once
#include "DimBase.h"
class Shape;

class DimPlaneToCylinderDistance:
	public DimBase
{
private:
	double PlaneParam[4], CylinderParam[7], CylinderEndPts[6], PlaneEndPts[12];
	void init();
	bool setMeasureName;
	void SetMeasureName();

	enum CYLINDER_PLANEMEASURETYPE
	{
		DISTANCE,
		ANGLE,
	}Cylinder_PlaneMType;

public:

	DimPlaneToCylinderDistance(bool simply);
	virtual ~DimPlaneToCylinderDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimPlaneToCylinderDistance&);
	friend wistream& operator>>(wistream&, DimPlaneToCylinderDistance&);
	friend void ReadOldPP(wistream& is, DimPlaneToCylinderDistance& x);
};
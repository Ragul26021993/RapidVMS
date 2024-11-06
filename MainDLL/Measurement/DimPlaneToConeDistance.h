#pragma once
#include "DimBase.h"
class Shape;


class DimPlaneToConeDistance:
	public DimBase
{
private:
	double PlaneParam[4], ConeParam[8], ConeEndPts[6], PlaneEndPts[12];
	void init();
	bool setMeasureName;
	void SetMeasureName();

	enum CONE_PLANEMEASURETYPE
	{
		DISTANCE,
		ANGLE,
	}Cone_PlaneMType;
public:

    DimPlaneToConeDistance(bool simply);
	virtual ~DimPlaneToConeDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimPlaneToConeDistance&);
	friend wistream& operator>>(wistream&, DimPlaneToConeDistance&);
	friend void ReadOldPP(wistream& is, DimPlaneToConeDistance& x);
};
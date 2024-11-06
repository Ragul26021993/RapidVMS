#pragma once
#include "DimBase.h"
class Shape;

class DimCylinderToConeDistance:
	public DimBase
{
//variable declaration...///
private:
	double ConeParam[8], CylinderParam[7], ConeEndPts[6], CylinderEndPts[6];
	void init();
    bool setMeasureName;
 	void SetMeasureName();

	enum MEASURETYPE
	{
		DISTANCEONLY,
		ANGLEONLY,
		DISTANCEANDANGLE
	}MType;

public:

	DimCylinderToConeDistance(bool simply);
	virtual ~DimCylinderToConeDistance();

	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimCylinderToConeDistance&);
	friend wistream& operator>>(wistream&, DimCylinderToConeDistance&);
	friend void ReadOldPP(wistream& is, DimCylinderToConeDistance& x);
};
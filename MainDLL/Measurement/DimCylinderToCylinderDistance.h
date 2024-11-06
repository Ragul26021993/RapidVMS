#pragma once
#include "DimBase.h"
class Shape;

class DimCylinderToCylinderDistance:
	public DimBase
{
//variable declaration...///
private:
	double Cylinder1Param[7], Cylinder2Param[7], Cylinder1EndPts[6], Cylinder2EndPts[6];
	void init();
	bool setMeasureName;
 	void SetMeasureName();

	enum CYLINDERMEASURETYPE
	{
		DISTANCEONLY,
		ANGLEONLY,
		DISTANCEANDANGLE
	}CylinderMType;

public:

	DimCylinderToCylinderDistance(bool simply);
	virtual ~DimCylinderToCylinderDistance();

	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimCylinderToCylinderDistance&);
	friend wistream& operator>>(wistream&, DimCylinderToCylinderDistance&);
	friend void ReadOldPP(wistream& is, DimCylinderToCylinderDistance& x);
};
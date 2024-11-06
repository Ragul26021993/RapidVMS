#pragma once
#include "DimBase.h"
class Shape;

class DimLineToCylinderDistance:
	public DimBase
{
//variable declaration...///
private:
	double LineParam[6], CylinderParam[7], LineEndPoints[6], CylinderEndPoints[6];
	bool setMeasureName;
	void SetMeasureName();

	enum LINECYLINDERMEASURETYPE
	{
		DISTANCEONLY,
		ANGLEONLY,
		DISTANCEANDANGLE
	}LineCylinderMtype;

public:

	DimLineToCylinderDistance(bool simply);
	virtual ~DimLineToCylinderDistance();
	void init();
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimLineToCylinderDistance&);
	friend wistream& operator>>(wistream&, DimLineToCylinderDistance&);
	friend void ReadOldPP(wistream& is, DimLineToCylinderDistance& x);
};
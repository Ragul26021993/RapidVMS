#pragma once
#include "DimBase.h"
class Shape;

class DimCylinderToConics3DDistance:
	public DimBase
{
private:
	double Conics3DParam[17], CylinderParam[7], CylinderEndPts[6];
	void init();
	//Need to optimize later
	bool ProjectLine, setMeasureName;
 	void SetMeasureName();

	enum CYLINDER_CONICSMEASURETYPE
	{
		DISTANCE,
		ANGLE,
	}Cylinder_ConicsMType;

public:
	
	DimCylinderToConics3DDistance(bool simply);
	virtual ~DimCylinderToConics3DDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimCylinderToConics3DDistance&);
	friend wistream& operator>>(wistream&, DimCylinderToConics3DDistance&);
	friend void ReadOldPP(wistream& is, DimCylinderToConics3DDistance& x);
};
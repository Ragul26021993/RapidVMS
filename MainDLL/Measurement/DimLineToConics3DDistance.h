#pragma once
#include "DimBase.h"
class Shape;

class DimLineToConics3DDistance:
	public DimBase
{
private:
	double LineParam[6], LineEndPoints[6], Conics3DParam[17], Max, Min;
	void init();
	//Need to optimise later..
	bool ProjectLine, setMeasureName;
	void SetMeasureName();

	enum LINE_CONICSMEASURETYPE
	{
		DISTANCE,
		ANGLE,
	}Line_ConicsMType;

public:
	
	DimLineToConics3DDistance(bool simply);
	virtual ~DimLineToConics3DDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimLineToConics3DDistance&);
	friend wistream& operator>>(wistream&, DimLineToConics3DDistance&);
	friend void ReadOldPP(wistream& is, DimLineToConics3DDistance& x);
};
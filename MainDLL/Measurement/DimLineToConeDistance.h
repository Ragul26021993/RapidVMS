#pragma once
#include "DimBase.h"
class Shape;

class DimLineToConeDistance:
	public DimBase
{
//variable declaration...///
private:
	double LineParam[6], ConeParam[8], LineEndPoints[6], ConeEndPoints[6];
	bool setMeasureName;
	void SetMeasureName();

	enum LINECONEMEASURETYPE
	{
		DISTANCEONLY,
		ANGLEONLY,
		DISTANCEANDANGLE
	}LineConeMtype;

public:
	
	DimLineToConeDistance(bool simply);
	virtual ~DimLineToConeDistance();
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
	friend wostream& operator<<(wostream&, DimLineToConeDistance&);
	friend wistream& operator>>(wistream&, DimLineToConeDistance&);
	friend void ReadOldPP(wistream& is, DimLineToConeDistance& x);
};
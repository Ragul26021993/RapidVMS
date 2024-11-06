#pragma once
#include "DimBase.h"
class Shape;

class DimConeToConeDistance:
	public DimBase
{
//variable declaration...///
private:
	double Cone1Param[8], Cone2Param[8], Cone1EndPts[6], Cone2EndPts[6];
	void init();
	bool setMeasureName;
 	void SetMeasureName();
	enum CONEMEASURETYPE
	{
		DISTANCEONLY,
		ANGLEONLY,
		DISTANCEANDANGLE
	}ConeMType;

public:
	
	DimConeToConeDistance(bool simply);
	virtual ~DimConeToConeDistance();

	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimConeToConeDistance&);
	friend wistream& operator>>(wistream&, DimConeToConeDistance&);
	friend void ReadOldPP(wistream& is, DimConeToConeDistance& x);
};
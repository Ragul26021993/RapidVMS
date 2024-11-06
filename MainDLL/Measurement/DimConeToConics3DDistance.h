#pragma once
#include "DimBase.h"
class Shape;

class DimConeToConics3DDistance:
	public DimBase
{
private:
	double Conics3DParam[17], ConeParam[8], ConeEndPts[6];
	double SfLine1[6], SfEndPnts1[6], SfLine2[6], SfEndPnts2[6];
	void init();
	//Need to optimize later
	bool ProjectLine;
	bool setMeasureName;
 	void SetMeasureName();

	enum CONE_CONICSMEASURETYPE
	{
		DISTANCEONLY,
		ANGLEONLY,
	}ConeConicsMtype;

public:
	
    DimConeToConics3DDistance(bool simply);
	virtual ~DimConeToConics3DDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimConeToConics3DDistance&);
	friend wistream& operator>>(wistream&, DimConeToConics3DDistance&);
	friend void ReadOldPP(wistream& is, DimConeToConics3DDistance& x);
};
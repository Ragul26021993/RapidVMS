#pragma once
#include "DimBase.h"
class Shape;

class DimCircle3DToConeDistance:
	public DimBase
{
private:
	double Circle3DParam[7], ConeParam[8], ConeEndPts[6],Circle3DAsPln[4];
	double SfLine1[6], SfEndPnts1[6], SfLine2[6], SfEndPnts2[6];
	void init();
	//Need to optimize later
	bool ProjectLine, setMeasureName;
 	void SetMeasureName();

	enum CIRCLE3DCONEMEASURETYPE
	{
		DISTANCEONLY,
		ANGLEONLY,
	}Circle3DConeMtype;
	
public:

    DimCircle3DToConeDistance(bool simply);
	virtual ~DimCircle3DToConeDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimCircle3DToConeDistance&);
	friend wistream& operator>>(wistream&, DimCircle3DToConeDistance&);
	friend void ReadOldPP(wistream& is, DimCircle3DToConeDistance& x);
};
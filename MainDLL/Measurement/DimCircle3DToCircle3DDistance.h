#pragma once
#include "DimBase.h"
class Shape;

class DimCircle3DToCircle3DDistance:
	public DimBase
{
private:
	double Circle3D1[7], Circle3D2[7];
	void init();
	bool setMeasureName;
 	void SetMeasureName();

	enum CIRCLE3DMEASURETYPE
	{
		DISTANCEONLY,
		ANGLEONLY,
	}Circle3DMtype;
	
public:
                                                                                                                                                                            	DimCircle3DToCircle3DDistance(bool simply);
	virtual ~DimCircle3DToCircle3DDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimCircle3DToCircle3DDistance&);
	friend wistream& operator>>(wistream&, DimCircle3DToCircle3DDistance&);
	friend void ReadOldPP(wistream& is, DimCircle3DToCircle3DDistance& x);
};
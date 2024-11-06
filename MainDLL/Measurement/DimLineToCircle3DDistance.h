#pragma once
#include "DimBase.h"
class Shape;

class DimLineToCircle3DDistance:
	public DimBase
{
private:

	double LineParam[6], LineEndPoints[6], Circle3D[7];
    RapidProperty<bool> distanceType;
	void init();
	bool ProjectLine;
	bool setMeasureName;
	void SetMeasureName();

	enum CIRCLE3DLINEMESURETYPE
	{
		ANGLE,
		DISTANCE,
	}CLMeasuretype;

public:
	
	DimLineToCircle3DDistance(bool simply);
	virtual ~DimLineToCircle3DDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimLineToCircle3DDistance&);
	friend wistream& operator>>(wistream&, DimLineToCircle3DDistance&);
	friend void ReadOldPP(wistream& is, DimLineToCircle3DDistance& x);
};
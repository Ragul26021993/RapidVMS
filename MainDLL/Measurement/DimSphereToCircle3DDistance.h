#pragma once
#include "DimBase.h"
class Shape;

class DimSphereToCircle3DDistance:
	public DimBase
{
private:
	double Circle3D[7], SphereParam[4];
	TCHAR* genName(const TCHAR* prefix);
	void init();
public:
	DimSphereToCircle3DDistance(TCHAR* myname);
	DimSphereToCircle3DDistance(bool simply);
	virtual ~DimSphereToCircle3DDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimSphereToCircle3DDistance&);
	friend wistream& operator>>(wistream&, DimSphereToCircle3DDistance&);
	friend void ReadOldPP(wistream& is, DimSphereToCircle3DDistance& x);
};
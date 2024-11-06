#pragma once
#include "DimBase.h"
class Shape;

class DimSphereToConics3DDistance:
	public DimBase
{
private:
	double Conics3DParam[17], SphereParam[4];
	TCHAR* genName(const TCHAR* prefix);
	void init();
public:
	DimSphereToConics3DDistance(TCHAR* myname);
	DimSphereToConics3DDistance(bool simply);
	virtual ~DimSphereToConics3DDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimSphereToConics3DDistance&);
	friend wistream& operator>>(wistream&, DimSphereToConics3DDistance&);
	friend void ReadOldPP(wistream& is, DimSphereToConics3DDistance& x);
};
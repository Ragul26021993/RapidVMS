//Class used to calculate the dmeasurement between point and Circle3D/arc3D.
#pragma once
#include "DimBase.h"
class Shape;

class DimPointToCircle3DDistance:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix);
	void init();
	double pt[3];
public:
	DimPointToCircle3DDistance(TCHAR* myname = _T("Dist"));
	DimPointToCircle3DDistance(bool simply);
	virtual ~DimPointToCircle3DDistance();

	void CalculateMeasurement(Shape *s1, Vector* p1);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimPointToCircle3DDistance&);
	friend wistream& operator>>(wistream&, DimPointToCircle3DDistance&);
	friend void ReadOldPP(wistream& is, DimPointToCircle3DDistance& x);
};
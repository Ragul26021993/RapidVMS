//Class used to calculate the point to line distance........///
#pragma once
#include "DimBase.h"
class Shape;

class DimPointToLineDistance3D:
	public DimBase
{
private:
	double LineEndpoints[6], LineParam[6];
	TCHAR* genName(const TCHAR* prefix);

public:
	DimPointToLineDistance3D(TCHAR* myname);
	DimPointToLineDistance3D(bool simply);
	virtual ~DimPointToLineDistance3D();

	void CalculateMeasurement(Shape *s1, Vector* p1);
	
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Write / read part program data...//
	friend wostream& operator<<(wostream&, DimPointToLineDistance3D&);
	friend wistream& operator>>(wistream&, DimPointToLineDistance3D&);
	friend void ReadOldPP(wistream& is, DimPointToLineDistance3D& x);
};
#pragma once
#include "DimBase.h"
class Shape;
class Vector;

class DimPointToPlaneDistance:
	public DimBase
{
private:
	double PlaneParam[4], PlaneEndPoints[12];
	TCHAR* genName(const TCHAR* prefix);
	void init();
public:
	DimPointToPlaneDistance(TCHAR* myname = _T("Dist"));
	DimPointToPlaneDistance(bool simply);
	virtual ~DimPointToPlaneDistance();
	
	void CalculateMeasurement(Shape *s1, Vector* p1);

	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimPointToPlaneDistance&);
	friend wistream& operator>>(wistream&, DimPointToPlaneDistance&);
	friend void ReadOldPP(wistream& is, DimPointToPlaneDistance& x);
};
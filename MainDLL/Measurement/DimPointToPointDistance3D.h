#pragma once
#include "DimBase.h"
class Vector;

class DimPointToPointDistance3D:
	public DimBase
{	
private:
	TCHAR* genName(const TCHAR* prefix);
public:
	DimPointToPointDistance3D(TCHAR* myname = _T("Dist"));
	DimPointToPointDistance3D(bool simply);
	virtual ~DimPointToPointDistance3D();

	void CalculateMeasurement(Vector *v1, Vector *v2);
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimPointToPointDistance3D&);
	friend wistream& operator>>(wistream&, DimPointToPointDistance3D&);
	friend void ReadOldPP(wistream& is, DimPointToPointDistance3D& x);
};
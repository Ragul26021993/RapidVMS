//Class for point to point distance.. normal, linear.//
#pragma once
#include "DimBase.h"
class Vector;

class DimPointToPointDistance:
	public DimBase
{	
private:
	bool CurrentStatus, firstNear, secondNear;
	TCHAR* genName(const TCHAR* prefix);
public:
	DimPointToPointDistance(TCHAR* myname = _T("Dist"), RapidEnums::MEASUREMENTTYPE mtype = RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCE);
	DimPointToPointDistance(bool simply);
	virtual ~DimPointToPointDistance();

	void CalculateMeasurement(Vector *v1, Vector *v2);
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimPointToPointDistance&);
	friend wistream& operator>>(wistream&, DimPointToPointDistance&);
	friend void ReadOldPP(wistream& is, DimPointToPointDistance& x);
};
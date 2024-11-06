#pragma once
#include "DimBase.h"
class Shape;
class Vector;

class DimOrdinate:
	public DimBase
{	
private:
	TCHAR* genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype);
	static int ordinatenoX, ordinatenoY, ordinatenoZ;
public:
	DimOrdinate(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype);
	DimOrdinate(bool simply);
	virtual ~DimOrdinate();

	void CalculateMeasurement(Vector *v1);
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);
	
	static void reset();

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimOrdinate&);
	friend wistream& operator>>(wistream&, DimOrdinate&);
	friend void ReadOldPP(wistream& is, DimOrdinate& x);
};
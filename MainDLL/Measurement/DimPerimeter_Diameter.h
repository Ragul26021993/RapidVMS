//Class Used To Maintain the line to line measurement
#pragma once
#include "DimBase.h"
class Shape;


class DimPerimeter_Diameter:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype);
	static int pdiacnt, ppericnt, pareacnt;
	double center[3];
	void init();

public:
	DimPerimeter_Diameter(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype);
	DimPerimeter_Diameter(bool simply);
	virtual ~DimPerimeter_Diameter();

	void CalculateMeasurement(Shape *s1);
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	static void reset();
	//Read/ write partprogram data...//
	friend wostream& operator<<(wostream&, DimPerimeter_Diameter&);
	friend wistream& operator>>(wistream&, DimPerimeter_Diameter&);
	friend void ReadOldPP(wistream& is, DimPerimeter_Diameter& x);
};
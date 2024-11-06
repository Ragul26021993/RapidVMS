//Class to measure the circle/ arc Diameter... 
#pragma once
#include "DimBase.h"
class Shape;

class DimSurfaceRoughnessMeasurement:
	public DimBase
{
private:
	double center[3];
	static int roughnesscnt;
	TCHAR* genName(RapidEnums::MEASUREMENTTYPE Mtype);
public:
	DimSurfaceRoughnessMeasurement(RapidEnums::MEASUREMENTTYPE Mtype);
	DimSurfaceRoughnessMeasurement(bool simply);
	virtual ~DimSurfaceRoughnessMeasurement();
	
	void CalculateMeasurement(Shape* s1, bool calculate = true);
	void SetCenter(double *center);
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	static void reset();

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimSurfaceRoughnessMeasurement&);
	friend wistream& operator>>(wistream&, DimSurfaceRoughnessMeasurement&);

};
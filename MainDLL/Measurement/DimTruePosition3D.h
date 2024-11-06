#pragma once
#include "DimBase.h"

class DimTruePosition3D:
	public DimBase
{
private:
	double tpposition[2];
	TCHAR* genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype);
	void init();
	static int tpnoG;
public:
	DimTruePosition3D(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype);
	DimTruePosition3D(bool simply);
	virtual ~DimTruePosition3D();

	void CalculateMeasurement(Shape* s1, Shape* s2, Shape* s3, Shape* s4);
	
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	void SetTPposition(double x, double y);
	static void reset();

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimTruePosition3D&);
	friend wistream& operator>>(wistream&, DimTruePosition3D&);
};
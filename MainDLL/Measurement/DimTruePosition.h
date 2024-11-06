#pragma once
#include "DimBase.h"

class DimTruePosition:
	public DimBase
{
private:
	double tpposition[2];
	TCHAR* genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype);
	void init();
	static int tpnoG, tpnoX, tpnoY;
public:
	DimTruePosition(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype);
	DimTruePosition(bool simply);
	virtual ~DimTruePosition();

	void CalculateMeasurement(Shape* s1, Shape* s2, Shape* s3);
	
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
	friend wostream& operator<<(wostream&, DimTruePosition&);
	friend wistream& operator>>(wistream&, DimTruePosition&);
	friend void ReadOldPP(wistream& is, DimTruePosition& x);
};
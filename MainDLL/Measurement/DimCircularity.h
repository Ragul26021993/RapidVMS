//Class to Maintain Circularity, MIC, MCC of a Circle..
#pragma once
#include "DimBase.h"

class Shape;

class DimCircularity:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype);
	static int circularityno, micno, mccno;
public:
	DimCircularity(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype);
	DimCircularity(bool simply);
	virtual ~DimCircularity();
	void CalculateMeasurement(Shape* s1);
	
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);
	
	static void reset();

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimCircularity&);
	friend wistream& operator>>(wistream&, DimCircularity&);
	friend void ReadOldPP(wistream& is, DimCircularity& x);
};
//Class to measure the line width...
#pragma once
#include "DimBase.h"

class Line;

class DimLine:
	public DimBase
{
private:
	bool CurrentStatus;
	TCHAR* genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype);
	static int straightnessno;
public:
	DimLine(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype);
	DimLine(bool simply);
	virtual ~DimLine();

	void CalculateMeasurement(Line*);
	//Virtual functions....//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	static void reset();
	// read/ write the part program data...//
	friend wostream& operator<<(wostream&, DimLine&);
	friend wistream& operator>>(wistream&, DimLine&);
	friend void ReadOldPP(wistream& is, DimLine& x);
};
//Class to measure the line width...
#pragma once
#include "DimBase.h"
class Line;

class DimLine3D:
	public DimBase
{
private:
	double LineMidPt[4];
	TCHAR* genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE Mtype);
	static int straightnessno;
public:
	DimLine3D(TCHAR* myname, RapidEnums::MEASUREMENTTYPE Mtype);
	DimLine3D(bool simply);
	virtual ~DimLine3D();

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
	friend wostream& operator<<(wostream&, DimLine3D&);
	friend wistream& operator>>(wistream&, DimLine3D&);
	friend void ReadOldPP(wistream& is, DimLine3D& x);
};
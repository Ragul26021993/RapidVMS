#pragma once
#include "DimBase.h"
class Shape;
class Line;

class DimGrooveMeasurement:
	public DimBase
{
private:

	void init(RapidEnums::MEASUREMENTTYPE ctype);

	TCHAR* genName(const TCHAR* prefix, RapidEnums::MEASUREMENTTYPE ctype);
	Line* TempLine1,* TempLine2;
	Vector* Vector1,* Vector2;
	static int grovdpthcnt,grovwidthcnt, grovangcnt;
	bool WidthType;
	double CornerPnts[8] ;
	void CalculateGrooveCornerPnts();
	bool GrovCornrPnts_TwoLines(double slope, double intercept1, double intercept2, double dis);
	void CalculateGrooveWidths(bool maxWidth);
	void CalculateGrooveDepth();
	void CalculateGrooveAngle();
	bool firstset, secondset, thirdset, forthset;
public:
	DimGrooveMeasurement(TCHAR* myname, RapidEnums::MEASUREMENTTYPE ctype);
	DimGrooveMeasurement(bool simply, RapidEnums::MEASUREMENTTYPE ctype);
	virtual ~DimGrooveMeasurement();

	void CalculateMeasurement(Shape* s1);
	void CalculateDiffGrooveMeasurements(bool maxwidth);
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	static void reset();

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimGrooveMeasurement&);
	friend wistream& operator>>(wistream&, DimGrooveMeasurement&);
	friend void ReadOldPP(wistream& is, DimGrooveMeasurement& x);
};
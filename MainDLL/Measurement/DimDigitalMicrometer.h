#pragma once
#include "DimBase.h"

class DimDigitalMicrometer:
	public DimBase
{	
private:
	Vector *Endpoint1, *Endpoint2, *ClickedPoint1, *ClickedPoint2;
	TCHAR* genName(const TCHAR* prefix);
	static int digitalumno;
public:
	bool DrawneartheMousePosition;
	DimDigitalMicrometer(TCHAR* myname = _T("duM"));
	DimDigitalMicrometer(bool simply);
	virtual ~DimDigitalMicrometer();

	void CalculateMeasurement(Vector *v1, Vector *v2);
	void SetFirstOriginalPoint1(double x, double y, double z);
	void SetFirstOriginalPoint2(double x, double y, double z);
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	static void reset();

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimDigitalMicrometer&);
	friend wistream& operator>>(wistream&, DimDigitalMicrometer&);
	friend void ReadOldPP(wistream& is, DimDigitalMicrometer& x);
};
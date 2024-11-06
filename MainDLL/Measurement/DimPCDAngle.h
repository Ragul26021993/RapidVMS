//Class Used To Maintain the PCD Angle Measurement
#pragma once
#include "DimBase.h"
class Shape;

class DimPCDAngle:
	public DimBase
{
private:
	double angle1, angle2, intercept1, intercept2, mouseAngle;
	double PcdCenter[3], ShCenter1[2], ShCenter2[2];
	bool drawOrietation;
	TCHAR* genName(const TCHAR* prefix);
	static int pcdangle;
public:
	DimPCDAngle(TCHAR* myname = _T("pcdAngle"));
	DimPCDAngle(bool simply);
	virtual ~DimPCDAngle();

	void CalculateMeasurement(Shape *s1, Shape *s2, Shape *s3);
	//Virtual Functions..
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	static void reset();

	//Read/ write partprogram data...//
	friend wostream& operator<<(wostream&, DimPCDAngle&);
	friend wistream& operator>>(wistream&, DimPCDAngle&);
	friend void ReadOldPP(wistream& is, DimPCDAngle& x);
};
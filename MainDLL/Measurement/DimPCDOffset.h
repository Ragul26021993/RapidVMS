//Class Used To Maintain the PCD Offset Measurement
#pragma once
#include "DimBase.h"
class Shape;

class DimPCDOffset:
	public DimBase
{	
private:
	TCHAR* genName(const TCHAR* prefix);
	double center[3];
	static int pcdoffsetcnt;
public:
	DimPCDOffset(TCHAR* myname = _T("Offset"));
	DimPCDOffset(bool simply);
	virtual ~DimPCDOffset();

	void CalculateMeasurement(Shape *s1, Shape *s2);
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	static void reset();

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimPCDOffset&);
	friend wistream& operator>>(wistream&, DimPCDOffset&);
	friend void ReadOldPP(wistream& is, DimPCDOffset& x);
};
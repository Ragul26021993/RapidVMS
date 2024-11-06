//Class for point to point distance.. normal, linear.//
#pragma once
#include "DimBase.h"

class Vector;
class DimCloudPointDepth:
	public DimBase
{	
private:
	TCHAR* genName(const TCHAR* prefix);
public:
	DimCloudPointDepth(TCHAR* myname = _T("Dist"));
	DimCloudPointDepth(bool simply);
	virtual ~DimCloudPointDepth();

	void CalculateMeasurement(Shape *s1);
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimCloudPointDepth&);
	friend wistream& operator>>(wistream&, DimCloudPointDepth&);
	friend void ReadOldPP(wistream& is, DimCloudPointDepth& x);
};
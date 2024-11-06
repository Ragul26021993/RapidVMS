#pragma once
#include "DimBase.h"
class Shape;

class DimDepth:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix);
	static int depthcnt;
	void init();
public:
	DimDepth(TCHAR* myname = _T("Depth"));
	DimDepth(bool simply);
	virtual ~DimDepth();
	int dummyShapesType[2];
	RapidProperty<bool> TwoBoxDepthMeasure;
	RapidProperty<int> CurrentDMeasure;
	RapidProperty<double> ValueZ1;
	RapidProperty<double> ValueZ2;
	RapidProperty<double> DepthValue;
	void SetShapes(Shape *s1, Shape *s2);
	
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);
	double getZFromDiffShapes(Shape *cShape, int type);
	static void reset();

	//Write / read part program data...//
	friend wostream& operator<<(wostream&, DimDepth&);
	friend wistream& operator>>(wistream&, DimDepth&);
	friend void ReadOldPP(wistream& is, DimDepth& x);
};
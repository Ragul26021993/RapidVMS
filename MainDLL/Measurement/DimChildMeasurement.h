#pragma once
#include "DimBase.h"

class MAINDLL_API DimChildMeasurement:
	public DimBase
{
private:
	void init();

public:
	DimChildMeasurement(bool simply);
	virtual ~DimChildMeasurement();
	
	RapidProperty<double> ChildMeasureValue;
	RapidProperty<int> ParentMeasureId;
	
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Write / read part program data...//
	friend wostream& operator<<(wostream&, DimChildMeasurement&);
	friend wistream& operator>>(wistream&, DimChildMeasurement&);
	friend void ReadOldPP(wistream& is, DimChildMeasurement& x);
};
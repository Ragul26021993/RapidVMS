#pragma once
#include "DimBase.h"
class Vector;
class Shape;

class DimPointToConics3DDistance:
	public DimBase
{
private:
	double Conics3DParam[17], Max, Min;
	TCHAR* genName(const TCHAR* prefix);
	void init();
public:
	DimPointToConics3DDistance(TCHAR* myname = _T("Dist"));
	DimPointToConics3DDistance(bool simply);
	virtual ~DimPointToConics3DDistance();
	
	void CalculateMeasurement(Shape *s1, Vector* p1);

	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimPointToConics3DDistance&);
	friend wistream& operator>>(wistream&, DimPointToConics3DDistance&);
	friend void ReadOldPP(wistream& is, DimPointToConics3DDistance& x);
};
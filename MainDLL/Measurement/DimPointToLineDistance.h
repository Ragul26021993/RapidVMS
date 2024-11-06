//Class used to calculate the point to line distance........///
#pragma once
#include "DimBase.h"
class Shape;
class Vector;

class DimPointToLineDistance:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix);
private: 
	enum Distancetype
	{
		PT_LINE,
		PT_XRAY,
		PT_XLINE
	}disttype;
public:
	DimPointToLineDistance(TCHAR* myname = _T("Dist"));
	DimPointToLineDistance(bool simply);
	virtual ~DimPointToLineDistance();

	void CalculateMeasurement(Shape *s1, Vector* p1);
	
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Write / read part program data...//
	friend wostream& operator<<(wostream&, DimPointToLineDistance&);
	friend wistream& operator>>(wistream&, DimPointToLineDistance&);
	friend void ReadOldPP(wistream& is, DimPointToLineDistance& x);
};
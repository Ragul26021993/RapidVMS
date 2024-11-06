#pragma once
#include "DimBase.h"
//#include <list>
class DimChildMeasurement;
class RCollectionBase;
class Vector;

class MAINDLL_API DimPointCoordinate:
	public DimBase
{	
private:
	TCHAR* genName(const TCHAR* prefix);
	void init();
	DimChildMeasurement* AddChildMeasurement(std::string str);
	static int centerno;
	std::string ConvertToString(double d, int NoOfDec, bool inchMode);
public:
	DimPointCoordinate(TCHAR* myname = _T("Center"));
	DimPointCoordinate(bool simply);
	virtual ~DimPointCoordinate();
	DimChildMeasurement *DimPointX, *DimPointY, *DimPointZ;
	list<int> PChildMeasureIdCollection;

	void CalculateMeasurement(Vector *v1);
	void AddChildeMeasureToCollection(RCollectionBase& MColl);
	//virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);
	
	static void reset();

	//Read/ write part program data...//
	friend wostream& operator<<(wostream&, DimPointCoordinate&);
	friend wistream& operator>>(wistream&, DimPointCoordinate&);
	friend void ReadOldPP(wistream& is, DimPointCoordinate& x);
};
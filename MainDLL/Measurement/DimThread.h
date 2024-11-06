//Class Used To Maintain the line to line measurement
#pragma once
#include "DimBase.h"
#include <map>

class DimChildMeasurement;
class RCollectionBase;
class Shape;
class ShapeWithList;
class AddThreadMeasureAction;

class MAINDLL_API DimThread:
	public DimBase
{
private:
	TCHAR* genName(const TCHAR* prefix);
	void init();
	DimChildMeasurement* AddChildMeasurement(std::string str);
	static int threadcnt;
	double ThrdMajordia, ThrdMinordia, ThrdEffdiaM, ThrdEffdiaG, ThrdPitch, ThrdAngle, ThrdTopRad, ThrdRootRad, ThrdPitchDia;
	void getaverage(map<int, double>& tmplist, double* ptr_average);
	void showPitch_Measurement(double *intersectPt1, double *intersectPt2, Shape* Cline);
public:
	AddThreadMeasureAction* PointerToAction;
public:
	DimThread(TCHAR* myname = _T("Thread"));
	DimThread(bool simply);
	virtual ~DimThread();
	DimChildMeasurement *DimThrdMajordia, *DimThrdMinordia, *DimThrdEffdiaM, *DimThrdPitchDia;
	DimChildMeasurement *DimThrdEffdiaG, *DimThrdPitch, *DimThrdAngle;
	DimChildMeasurement *DimThrdTopRadius, *DimThrdRootRadius;
	list<int> TChildMeasureIdCollection;

	void CalculateMeasurement();
	void AddChildeMeasureToCollection(RCollectionBase& MColl);

	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	static void reset();
	//Read/ write partprogram data...//
	friend wostream& operator<<(wostream&, DimThread&);
	friend wistream& operator>>(wistream&, DimThread&);
	friend void ReadOldPP(wistream& is, DimThread& x);
};
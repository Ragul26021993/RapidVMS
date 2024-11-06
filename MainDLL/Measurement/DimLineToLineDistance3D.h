#pragma once
#include "DimBase.h"
class Shape;

class DimLineToLineDistance3D:
	public DimBase
{
private:
	double Line1Param[6], Line2Param[6], Line1EndPoints[6], Line2EndPoints[6];
	void init();
	bool setMeasureName;
 	void SetMeasureName();

	enum LINE3DMEASURETYPE
	{
		DISTANCEONLY,
		ANGLEONLY,
		DISTANCEANDANGLE
	}Line3DMtype;
	
public:
	DimLineToLineDistance3D(bool simply);
	virtual ~DimLineToLineDistance3D();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	virtual void MeasurementTypeChanged();

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimLineToLineDistance3D&);
	friend wistream& operator>>(wistream&, DimLineToLineDistance3D&);
	friend void ReadOldPP(wistream& is, DimLineToLineDistance3D& x);
};
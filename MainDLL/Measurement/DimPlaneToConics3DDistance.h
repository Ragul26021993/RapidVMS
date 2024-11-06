#pragma once
#include "DimBase.h"
class Shape;

class DimPlaneToConics3DDistance:
	public DimBase
{
private:
	double PlaneParam[4], Conics3DParam[17], PlaneEndPoints[12];
	void init();
	bool setMeasureName;
	void SetMeasureName();
	enum PLANECONICSMEASURETYPE
	{
		DISTANCEONLY,
		ANGLEONLY,
	}PlaneConicsMtype;
public:


	DimPlaneToConics3DDistance(bool simply);
	virtual ~DimPlaneToConics3DDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimPlaneToConics3DDistance&);
	friend wistream& operator>>(wistream&, DimPlaneToConics3DDistance&);
	friend void ReadOldPP(wistream& is, DimPlaneToConics3DDistance& x);
};
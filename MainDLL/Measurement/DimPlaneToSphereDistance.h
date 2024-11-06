#pragma once
#include "DimBase.h"
class Shape;

class DimPlaneToSphereDistance:
	public DimBase
{
private:
	double PlaneParam[4], SphereParam[4], PlaneEndPoints[12];
	TCHAR* genName(const TCHAR* prefix);
	void init();
public:
	DimPlaneToSphereDistance(TCHAR* myname);
	DimPlaneToSphereDistance(bool simply);
	virtual ~DimPlaneToSphereDistance();
	
	void CalculateMeasurement(Shape *s1, Shape *s2);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimPlaneToSphereDistance&);
	friend wistream& operator>>(wistream&, DimPlaneToSphereDistance&);
	friend void ReadOldPP(wistream& is, DimPlaneToSphereDistance& x);
};
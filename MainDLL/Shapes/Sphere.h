#pragma once
#include "shapewithlist.h"
#include "vector.h"
#include <list>

class MAINDLL_API Sphere:
	public ShapeWithList
{
private:
	static int shapenumber;
	void init();
	TCHAR* genName(const TCHAR* prefix);
	Vector center, MinZpoint;
	RapidProperty<bool> IdSphere;
	
	void ProbePointCalculations_Calibration(int PointsCnt);
	void updateSphere_UsingPorbe_Calibration();

	void ProbePointCalculations(int PointsCnt);
	void CheckPointIsForPorbe();
	void CheckIdorOdSphere();
	void updateSphere_UsingPorbe();
	bool updateSphere(int PointsCnt);

public:
	RapidProperty<double> Radius, MinInscribedia, MaxCircumdia, Sphericity;
	RapidProperty<double> theta_min, theta_max, phi_min, phi_max;
	
public:
	Sphere(TCHAR* myname = _T("SH"));
	Sphere(bool simply);
	~Sphere();
	
	void setCenter(Vector);
	Vector* getCenter();

	void getParameters(double* sphereparam);
	void getSurfacePoint(double *dir, bool forward, double *SFPoint);

	//Virtual Functions...
	virtual void drawCurrentShape(int windowno, double WPixelWidth);
	virtual void drawGDntRefernce();
	virtual void Transform(double*);
	virtual bool Shape_IsInWindow(Vector& corner1,double Tolerance);
	virtual bool Shape_IsInWindow(Vector& corner1,Vector& corner2);
	virtual bool Shape_IsInWindow(double *SelectionLine, double Tolerance);
	virtual Shape* Clone(int n = 0, bool copyOriginalProperty = true);
	virtual Shape* CreateDummyCopy();
	virtual void CopyShapeParameters(Shape*);
	virtual void UpdateBestFit();
	virtual bool GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom);
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual void Translate(Vector& Position);
	virtual void ResetShapeParameters();
	virtual bool WithinCirclularView(double *SelectionLine, double Tolerance);
	virtual double GetZlevel();
	virtual void AlignToBasePlane(double* trnasformM);
	virtual void UpdateEnclosedRectangle();
	virtual void GetShapeCenter(double *cPoint);

	void calculateAttributes();

	static void reset();
	//Partprogram data.. read/ write..//
	friend wostream& operator<<(wostream& os, Sphere& x);
	friend wistream& operator>>(wistream& is, Sphere& x); 
	friend void ReadOldPP(wistream& is, Sphere& x);
};
#pragma once
#include "shapewithlist.h"
                
class MAINDLL_API Cylinder:
	public ShapeWithList
{                                   
private:	
	static int shapenumber;
	Vector center1,center2;
	RapidProperty<bool> IdCylinder;
	TCHAR* genName(const TCHAR* prefix);
	void init();

	void ProbePointCalculations(int PointsCnt);
	void CheckPointIsForPorbe();
	void CheckIdorOdCylinder();
	void updateCylinder_UsingPorbe();

	bool updateCylinder(int PointsCount);

public:
	RapidProperty<double> Radius1, Radius2;
	RapidProperty<double> dir_l, dir_m, dir_n;
	RapidProperty<double> Length, MinInscribedia, MaxCircumdia, Cylindricity, FilteredCylindricity, phi_min, phi_max;
	
public:
	Cylinder(TCHAR* myname = _T("CL"));
	Cylinder(bool simply);
	~Cylinder();
	
	//Set and get Cylinder Properties....
	void SetCenter1(Vector& t);
	void SetCenter2(Vector& t);
	Vector* getCenter1();
	Vector* getCenter2();

	void SetCenters(Vector& c1, Vector& c2);
	void SetRadius(double r1, double r2);
	void SetCylinderProperties(double l, double m, double n);
	void updateParameters();

	void getParameters(double* cylinderparam);
	void getEndPoints(double* endpoints);
	void getSurfaceLine(double *dir, bool forward, double *SFline, double *SFlineEndpts);
	void getAxisLine(double *LineParam3D); //Added by RAN 16 Jun 2015 for using cylinder axis to get a parallel line3d
		
	//Virtual functions...
	virtual void UpdateBestFit();
	virtual bool GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom);
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual void drawCurrentShape(int windowno, double WPixelWidth);
	virtual void drawGDntRefernce();
	virtual void Transform(double* transform);
	virtual bool Shape_IsInWindow(Vector& corner1,double Tolerance);
	virtual bool Shape_IsInWindow(Vector& corner1,Vector& corner2);
	virtual bool Shape_IsInWindow(double *SelectionLine, double Tolerance);
	virtual Shape* Clone(int n, bool copyOriginalProperty = true);
	virtual Shape* CreateDummyCopy();
	virtual void CopyShapeParameters(Shape*);
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
	friend wostream& operator<<(wostream& os, Cylinder& x);
	friend wistream& operator>>(wistream& is, Cylinder& x); 
	friend void ReadOldPP(wistream& is, Cylinder& x);
};	

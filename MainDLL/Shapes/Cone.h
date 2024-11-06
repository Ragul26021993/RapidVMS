#pragma once
#include "shapewithlist.h"

class MAINDLL_API Cone:
	public ShapeWithList
{
private:	
	static int shapenumber;
	Vector center1,center2;
	RapidProperty<bool> IdCone;
	TCHAR* genName(const TCHAR* prefix);
	void init();

	void ProbePointCalculations(int PointsCnt);
	void CheckPointIsForProbe();
	void CheckIdorOdCone();
	void updateCone_UsingProbe();

	void updateCone(int PointsCount);

public:
	RapidProperty<double> Radius1, Radius2, ApexAngle;
	RapidProperty<double> dir_l, dir_m, dir_n;
	RapidProperty<double> Length, MinInscribeApexAng, MaxCircumApexAng, Conicality, phi_min, phi_max;
	double prevguess[8];

public:
	Cone(TCHAR* myname = _T("CN"));
	Cone(bool simply);
	~Cone();
	
	//Set and get Cone Properties....
	void SetCenter1(Vector& t);
	void SetCenter2(Vector& t);
	Vector* getCenter1();
	Vector* getCenter2();

	void SetCenters(Vector& c1, Vector& c2);
	void SetRadius(double r1, double r2);
	void SetConeProperties(double l, double m, double n);
	void updateParameters();

	void getParameters(double* Coneparam);
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
	friend wostream& operator<<(wostream& os, Cone& x);
	friend wistream& operator>>(wistream& is, Cone& x); 
	friend void ReadOldPP(wistream& is, Cone& x);
};	

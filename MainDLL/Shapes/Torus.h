#pragma once
#include "shapewithlist.h"

class MAINDLL_API Torus:
	public ShapeWithList
{
private:	
	static int shapenumber;
	Vector center;
	//RapidProperty<bool> IdTorus;
	TCHAR* genName(const TCHAR* prefix);
	void init();

	void ProbePointCalculations(int PointsCnt);
	void CheckPointIsForPorbe();
	//void CheckIdorOdTorus();
	void updateTorus_UsingPorbe();
	bool updateTorus(int PointsCount);

public:
	RapidProperty<double> Radius1, Radius2;
	RapidProperty<double> dir_l, dir_m, dir_n;
		
public:
	Torus(TCHAR* myname = _T("TR"));
	Torus(bool simply);
	~Torus();
	
	//Set and get Cylinder Properties....
	void SetCenter(Vector& t);
	Vector* getCenter();

	void SetRadius(double r1, double r2);
	void SetTorusProperties(double l, double m, double n);
	void updateParameters();
	void getParameters(double* Torusparam);
		
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
	/*virtual bool WithinCirclularView(double *SelectionLine, double Tolerance);*/
	virtual void AlignToBasePlane(double* TrnasformM);
	virtual void UpdateEnclosedRectangle();
	virtual void GetShapeCenter(double *cPoint);
	void calculateAttributes();

	static void reset();
	//Partprogram data.. read/ write..//
	friend wostream& operator<<(wostream& os, Torus& x);
	friend wistream& operator>>(wistream& is, Torus& x); 
	friend void ReadOldPP(wistream& is, Torus& x);
};	
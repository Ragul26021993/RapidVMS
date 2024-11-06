#pragma once
#include "shapewithlist.h"

class MAINDLL_API Conics3D:
	public ShapeWithList
{
private:	
	static int shapenumber;
	RapidProperty<bool> IdConic;
	TCHAR* genName(const TCHAR* prefix);
	void init();
	void ProbePointCalculations(int PointsCnt);
	void CheckPointIsForProbe();
	void CheckIdorOdConic();
	void updateConic_UsingProbe();
	void updateConic(int PointsCount);
	bool best_fit_succeeded;
	double best_fit_answer[14];

public:
	RapidProperty<double> dir_l, dir_m, dir_n, D;
	RapidProperty<double> eccentricity;
	Vector focus, focus2;
	double Diretrix[6];
	RapidProperty<double> startAngle, endAngle, dist_focus_diretrix;
	
public:
	Conics3D(TCHAR* myname);
	Conics3D(bool simply);
	Conics3D(RapidEnums::SHAPETYPE stype = RapidEnums::SHAPETYPE::ELLIPSE3D);
	Conics3D(TCHAR* myname, RapidEnums::SHAPETYPE stype = RapidEnums::SHAPETYPE::ELLIPSE3D);
	~Conics3D();
	//Set and get Conic Properties....
	void getParameters(double* Conicparam);
	void setParameters(double* Conicparam);
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
	virtual void AlignToBasePlane(double* trnasformM);
	virtual void UpdateEnclosedRectangle();
	virtual void GetShapeCenter(double *cPoint);

	static void reset();
	//Partprogram data.. read/ write..//
	friend wostream& operator<<(wostream& os, Conics3D& x);
	friend wistream& operator>>(wistream& is, Conics3D& x); 
	friend void ReadOldPP(wistream& is, Conics3D& x);
};	

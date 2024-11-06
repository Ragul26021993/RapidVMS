#pragma once
#include "shapewithlist.h"
#include "vector.h"

class SinglePoint;
class PointCollection;

class MAINDLL_API Spline:
	public ShapeWithList
{
private:
	static int Lshapenumber;
	void init();
	TCHAR* genName(const TCHAR* prefix);
	void ClearAllMemory();
	void CleartmpMemory(bool AllMemoryflag = false);
	void UpdateControlPointList();
public:
	Spline(TCHAR* myname = _T("L"));
	Spline(bool simply);
	~Spline();
	int Spline_degree, Spline_cpts, Spline_knots;
	std::list<SinglePoint*> pointsOnCurve;
	PointCollection *ControlPointsList; 
	double* CtrPtsArray;
	bool ClosedSpline, DrawSpline, framgrabShape;
	double* KnotsArray;
	//Virtual functions../;/
	virtual void drawCurrentShape(int windowno, double WPixelWidth);
	virtual void drawGDntRefernce();
	virtual void Transform(double*);

	virtual bool Shape_IsInWindow(Vector& corner1,double Tolerance);
	virtual bool Shape_IsInWindow(Vector& corner1,Vector& corner2);
	virtual bool Shape_IsInWindow(double *SelectionLine, double Tolerance);
	virtual Shape* Clone(int n, bool copyOriginalProperty = true);
	virtual Shape* CreateDummyCopy();
	virtual void CopyShapeParameters(Shape*);
	virtual void Translate(Vector& Position);
	virtual void UpdateBestFit();
	virtual bool GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom);
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual void ResetShapeParameters();
	virtual void AlignToBasePlane(double* trnasformM);
	virtual void UpdateEnclosedRectangle();

	//Write and Read to/ from the file..///
	friend wostream& operator<<(wostream& os, Spline& x);
	friend wistream& operator>>(wistream& is, Spline& x);
};
//Created by Rahul Singh Bhadauria.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
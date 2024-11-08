#pragma once
#include "shapewithlist.h"
#include "vector.h"
#include <list>

class PointCollection;

class MAINDLL_API Plane:
	public ShapeWithList
{
private:
	static int shapenumber;
	TCHAR* genName(const TCHAR* prefix);
	void init();
	Vector point1, point2, point3, point4;
	Vector MidPoint;
	RapidProperty<bool> AddIntercept;
	//Functions for probe point manipulation,.. according to the direction of hit/ orientation..
	void ProbePointCalculations(int PointsCnt);
	void CheckPointIsForPorbe();
	void CheckAddOrSubtractIntercept();
	void updatePlane_UsingPorbe();
	void updatePlane(int PointsCount);
	void updateDerivedPlane();

	void drawAxisShapes(int windowno, double WPixelWidth);
	void drawNormalShapes(int windowno, double WPixelWidth);
public:
	RapidProperty<double> dir_a, dir_b, dir_c, dir_d, Flatness, FilteredFlatness, Offset;
	RapidProperty<bool> DefinedOffset;
	PointCollection *planePolygonPts;
	Pt new_pt;
	LineArcList planePolygonLines;
	bool planePolygon;
	RapidEnums::PLANETYPE PlaneType;
	double ScanDir[3];

	Plane(TCHAR* myname = _T("PL"));
	Plane(bool simply);
	~Plane();
		
	//Set and Get the end points of the place..//
	void setPoint1(Vector& t);
	void setPoint2(Vector& t);
	void setPoint3(Vector& t);
	void setPoint4(Vector& t);
	Vector* getPoint1();
	Vector* getPoint2();
	Vector* getPoint3();
	Vector* getPoint4();
	Vector* getMidPoint();

	void SetPlaneParameters(double a, double b, double c, double d);
	void SetPlanePoints(Vector& v1, Vector& v2, Vector& v3, Vector& v4);
	void getParameters(double* planeparam);
	void getEndPoints(double* endpoints);
	//All Virtual Functions...
	virtual void drawCurrentShape(int windowno, double WPixelWidth);
	virtual void drawGDntRefernce();
	virtual void Transform(double*);
	virtual bool Shape_IsInWindow(Vector& corner1,double Tolerance);
	virtual bool Shape_IsInWindow(Vector& corner1,Vector& corner2);
	virtual bool Shape_IsInWindow(double *SelectionLine, double Tolerance);
	virtual Shape* Clone(int n, bool copyOriginalProperty = true);
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

	static void decrement();
	static void reset();
	//Partprogram data.. read/ write..//
	friend wostream& operator<<(wostream& os, Plane& x);
	friend wistream& operator>>(wistream& is, Plane& x);
	friend void ReadOldPP(wistream& is, Plane& x);
};
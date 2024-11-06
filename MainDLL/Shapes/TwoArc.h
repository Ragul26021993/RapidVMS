#pragma once
#include "shapewithlist.h"
#include "Circle.h"
#include "Line.h"

class MAINDLL_API TwoArc:
	public ShapeWithList
{
private:	
	static int shapenumber;
	Vector Center;
	//RapidProperty<bool> IdTorus;
	TCHAR* genName(const TCHAR* prefix);
	void init();
	bool firstTime; 
	void ProbePointCalculations(int PointsCnt);
	void CheckPointIsForPorbe();
	void CheckIdorOdTwoArc();
	void updateTwoArc_UsingPorbe();
	bool updateTwoArc(int PointsCount);
	void addGroupShapes();
	void OtherParameters();

public:
	RapidProperty<double> Radius1, Radius2;
	RapidProperty<double> Angle;
	RapidProperty<bool> IdCircle;
	Circle *Arc1, *Arc2;
	Line *ReflectionLine;
	bool AddReflectionLine;
    list<int> ChildsId;

	TwoArc(TCHAR* myname);
	TwoArc(bool simply);
	~TwoArc();
	
	//Set and get Cylinder Properties....
	void SetCenter(Vector& t);
	Vector* getCenter();

	void SetRadius(double r1, double r2);
	void SetSlope(double ang);
	void updateParameters();
	void getParameters(double* TwoArcparam);
		
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

	void SetSubArcParam(double *tmpPnts, double *center, double *radius, int pointsCount, Circle* CurrentArc);
	void UpdateForSubshapeChange(Shape *childShape);
	void AssignParamForGroupShapes();
	static void reset();
	//Partprogram data.. read/ write..//
	friend wostream& operator<<(wostream& os, TwoArc& x);
	friend wistream& operator>>(wistream& is, TwoArc& x); 
	friend void ReadOldPP(wistream& is, TwoArc& x);
};	
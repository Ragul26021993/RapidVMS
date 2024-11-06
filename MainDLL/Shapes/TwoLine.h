#pragma once
#include "shapewithlist.h"
#include "Line.h"

class MAINDLL_API TwoLine:
	public ShapeWithList
{
private:	
	static int shapenumber;
	Vector Center;
	bool firstTime;
	TCHAR* genName(const TCHAR* prefix);
	void init();

	void ProbePointCalculations(int PointsCnt);
	void CheckPointIsForPorbe();
	int CheckIdorOdTwoLine();
	void updateTwoLine_UsingPorbe(int i);
	bool updateTwoLine(int PointsCount);
    void addGroupShapes();

public:
	RapidProperty<double> LineAngle, Angle, PerpendicularDistance;
	RapidProperty<bool> IdTwoLine;
	Line *Line1, *Line2, *ReflectionLine;
	list<int> ChildsId;

	TwoLine(TCHAR* myname);
	TwoLine(bool simply);
	~TwoLine();
	
	//Set and get Cylinder Properties....
	bool AddReflectionLine;
	void SetCenter(Vector& t);
	Vector* getCenter();
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

	void SetSubLineParam(double* points, double angle, double intercept, int pointsCount, Line *currentLine);
	void UpdateForSubshapeChange(Shape *childShape);
	void AssignParamForGroupShapes();

	static void reset();
	//Partprogram data.. read/ write..//
	friend wostream& operator<<(wostream& os, TwoLine& x);
	friend wistream& operator>>(wistream& is, TwoLine& x); 
	friend void ReadOldPP(wistream& is, TwoLine& x);
};	
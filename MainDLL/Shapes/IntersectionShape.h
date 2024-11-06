//Point(shape) class...
#pragma once
#include "shapewithlist.h"
#include "vector.h"

class MAINDLL_API IntersectionShape:
	public ShapeWithList
{
private:
	static int shapenumber;

	//Get the name of the shape..//
	TCHAR* genName(const TCHAR* prefix);
	void init();	
	void getIntersectionPoints(Shape *ParentShape1, Shape *ParentShape2);
	Vector centerOfGravity;

public:
	double *intersectPts;
	int pntscount;
	Vector* getCenterOfGravity();
	IntersectionShape(TCHAR* myname = _T("IS"));
	IntersectionShape(bool simply);
	~IntersectionShape();
	
	//Virtual functions..//
	virtual void Transform(double* transform);
	virtual void drawCurrentShape(int windowno, double WPixelWidth);
	virtual bool Shape_IsInWindow(Vector& corner1,double Tolerance);
	virtual bool Shape_IsInWindow(Vector& corner1,Vector& corner2);
	virtual bool Shape_IsInWindow(double *SelectionLine, double Tolerance);
	virtual Shape* Clone(int n, bool copyOriginalProperty = true);
	virtual Shape* CreateDummyCopy();
	virtual void CopyShapeParameters(Shape*);
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual void ResetShapeParameters();
    virtual void GetShapeCenter(double *cPoint);
	virtual void UpdateBestFit();
	virtual void drawGDntRefernce();
	virtual void Translate(Vector& Position);
	virtual void AlignToBasePlane(double* trnasformM);
	virtual void UpdateEnclosedRectangle();
	virtual bool GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom);
	//Reset the point count..//
	static void reset();
	//PartProgram Data.. Write/ read ..
	friend wostream& operator<<(wostream& os, IntersectionShape& x);
	friend wistream& operator>>(wistream& is, IntersectionShape& x);
};	
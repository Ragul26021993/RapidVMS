//PolyLine Class.. Like Cloudpoints.. Set of lines is one shape..//
#pragma once
#include "shapewithlist.h"
#include "vector.h"
#include <list>

class MAINDLL_API Pline:
	public ShapeWithList
{
private:
	static int shapenumber;
	Vector point1;
	TCHAR* genName(const TCHAR* prefix);
	void init();
public:
	Pline(TCHAR* myname = _T("PL"));
	Pline(bool simply);
	~Pline();
	
	void setPoint1(Vector);
	Vector* getPoint1();

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
	virtual void AlignToBasePlane(double* trnasformM);
	virtual void UpdateEnclosedRectangle();

	static void reset();
};	
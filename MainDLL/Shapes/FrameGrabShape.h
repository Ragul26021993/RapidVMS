#pragma once
#include "shapewithlist.h"
#include "vector.h"
#include <list>

class MAINDLL_API FrameGrabShape:
	public ShapeWithList
{
private:
	static int shapenumber;
	void init();
	TCHAR* genName(const TCHAR* prefix);
	Vector center;

public:

	Vector Points[2];
	FrameGrabShape(TCHAR* myname = _T("FG"));
	~FrameGrabShape();
	
	Vector* getCenter();

	//Virtual Functions...
	virtual void drawCurrentShape(int windowno, double WPixelWidth);
	virtual void drawGDntRefernce();
	virtual void Transform(double*);
	virtual bool Shape_IsInWindow(Vector& corner1,double Tolerance);
	virtual bool Shape_IsInWindow(Vector& corner1,Vector& corner2);
	virtual bool Shape_IsInWindow(double *SelectionLine, double Tolerance);
	virtual Shape* Clone(int n = 0, bool copyOriginalProperty = true);
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

	static void reset();
};
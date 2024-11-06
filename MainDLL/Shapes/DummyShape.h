#pragma once
#include "shapewithlist.h"
#include "vector.h"

class MAINDLL_API DummyShape:
	public ShapeWithList
{
private:
	void init();

public:
	DummyShape();
	DummyShape(bool simply);
	~DummyShape();

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
	friend wostream& operator<<(wostream& os, DummyShape& x);
	friend wistream& operator>>(wistream& is, DummyShape& x);
};
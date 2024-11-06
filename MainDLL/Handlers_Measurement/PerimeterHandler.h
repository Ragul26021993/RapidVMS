//Handler to handle the perimetrer shape..//
//Handler is similar to Fast trace handler, but here shape are not added to the main collection
//Shape are just drawn to show the perimeter..
#pragma once
#include "..\Handlers\MouseHandler.h"
#include "..\Engine\PointCollection.h"

class Shape;
class Perimeter;


//Derived from the mousehandler..//
class PerimeterHandler:
	public MouseHandler
{
//Variable declaration...//
private:
	FramegrabBase *firstClick, *rightClick;
	bool Rflag, undoflag, Pflag, runningPartprogramValid, valid;
	bool ShapeHighilghted, SnapPointHighlighted;
	bool  Edit_LastPoint;
	Shape *cshape, *HighlightedShape;
	Perimeter *Pshape;
	SinglePoint* SnapPointer;
	SinglePoint TempClick;
	double point3[2], center[2], radius, startangle, sweepangle;
	map<int, int> SelectedShapePointColl;
	void init();
	Shape* getNearestShape(map<int,Shape*> Shapecollection, double x, double y, double snapdist);
	void getNearestPoint(map<int,Shape*> Shapecollection, double x, double y, double snapdist);
	void drawSnapPointOfShape();
	void drawSnapPoint();
public:
	//Constructor...//
	PerimeterHandler();
	PerimeterHandler(Shape* ParentShape, int ChildId, bool flag);
	PerimeterHandler(Shape *s);
	~PerimeterHandler();

	void SettheClickvalue(int i, double x, double y, double z);
	//Virtual functions..//
	virtual void mouseMove();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void LmouseDown();
	virtual void LmouseUp(double x,double y);
	virtual void LmaxmouseDown();
	virtual void RmouseDown(double x, double y);
	virtual void MmouseDown(double x, double y);
	//Handle the partprogram data...//
	virtual void PartProgramData();
	virtual void EscapebuttonPress();
};
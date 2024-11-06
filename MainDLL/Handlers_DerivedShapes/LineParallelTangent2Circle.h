//To calculate the parallel tangent to two circles..
#pragma once
#include "..\Handlers\MouseHandler.h"
#include "..\Engine\RCadApp.h"
class Shape;
class Line;

class LineParallelTangent2Circle:
	public MouseHandler
{
//Variable declaration...//
private:
	Shape *Cshape[2];
	Line *myLineFirst, *myLineSecnd;
	int ShapeCnt;
	LineArc_Circle Circle1, Circle2;
	LineArc Line1, Line2;
	double p_Dist, Linelength;
	bool runningPartprogramValid, validflag;

	void init();
	void getCircledimensions(Shape* Csh1, Shape* Csh2);
	void ResetShapeHighlighted();
public:
	LineParallelTangent2Circle();
	~LineParallelTangent2Circle();

	//Virtual functions..//
	virtual void LmouseDown();
	virtual void RmouseDown(double x, double y);
	virtual void mouseMove();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void MouseScroll(bool flag);
	virtual void PartProgramData();
	virtual void SetAnyData(double *data);
};
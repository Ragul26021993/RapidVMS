//Class to draw the derived points.. point on line)
#pragma once
#include "..\Handlers\MouseHandler.h"
#include "..\Shapes\RPoint.h"
class Circle;

class PointOnCircle:
	public MouseHandler
{
private:
	Circle* ParentShape;
	double center[2], radius;
	double point[2];
	bool runningPartprogramValid;

	void init();
	void ResetShapeHighlighted();
public:
	PointOnCircle();
	~PointOnCircle();
	
	//Virtual functions...
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();

	virtual void PartProgramData();
};
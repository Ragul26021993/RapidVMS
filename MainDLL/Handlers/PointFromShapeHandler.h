//
//Class used to draw the selection rectangle....//
#pragma once
#include "MouseHandler.h"

class PointFromShapeHandler:
	public MouseHandler
{
private:
	bool runningPartprogramValid;	
	bool ActionAdded;
public:
	PointFromShapeHandler();
	PointFromShapeHandler(bool runningPartprogramValid);
	~PointFromShapeHandler();
	ShapeWithList *DrawShape;
public:
	//Virtual functions..//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void LmouseUp(double x,double y);
	virtual void RmouseDown(double x, double y);
	virtual void MouseScroll(bool flag);
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void PartProgramData();
};
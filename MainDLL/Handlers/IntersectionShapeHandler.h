//Class to draw lines.. 
// Finite line, Infinite line, Ray..
#pragma once
#include "MouseHandler.h"
class Shape;

class IntersectionShapeHandler:
	public MouseHandler
{
//Variable declaration...//
private:
	Shape *Parentshape[2];
	Shape* myshape;
	bool runningPartprogramValid;
	
public:
	IntersectionShapeHandler();
	~IntersectionShapeHandler();

	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void RmouseDown(double x,double y);
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();

	//Virtual function to handle the partprogram data...//
	virtual void PartProgramData();
};
//Class to handle the new UCS.. with respect to the selected line and point...//
#pragma once
#include "MouseHandler.h"

class vector;
class Shape;

class UCSLineHandle:
	public MouseHandler
{
//Variable declaration...//
private:
	Shape *s1, *s0;
	bool valid;
	double RotateAngle, UIntercept;
	Vector* Centerpt;
public:
	UCSLineHandle();
	~UCSLineHandle();

	//Get the line dimension..//
	bool getLineDimension(Shape *s);
	//Virtual functions....../
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
};
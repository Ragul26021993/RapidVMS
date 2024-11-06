//
//Class used to draw the selection rectangle....//
#pragma once
#include "MouseHandler.h"
#include "..\Shapes\Vector.h"
class SelectionHandler :
	public MouseHandler
{
private:
	Vector PointsDRO[3];
	
public:
	SelectionHandler();
	~SelectionHandler();
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
};
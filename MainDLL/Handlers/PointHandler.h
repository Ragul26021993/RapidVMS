//Class to draw the point...
#pragma once
#include "MouseHandler.h"

class PointHandler :
	public MouseHandler
{
//Variable declarations///
private:
	bool runningPartprogramValid, firstclick;
	bool TwoDPoint;

	void AddNewPoint();
public:
	PointHandler(bool shapeas2d);
	PointHandler();
	~PointHandler();

	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void RmouseDown(double x, double y);
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void PartProgramData();
	virtual void EscapebuttonPress();
};
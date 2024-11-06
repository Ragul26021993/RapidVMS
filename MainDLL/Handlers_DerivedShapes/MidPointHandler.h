//Class to draw the point...
#pragma once
#include "..\Handlers\MouseHandler.h"
#include "..\Shapes\Vector.h"

class MidPointHandler:
	public MouseHandler
{
//Variable declarations///
private:
	bool runningPartprogramValid;
	bool FirstPoint_Related, SecondPoint_Related;
	Vector Point1, Point2, MidPoint;

	void init();
public:
	MidPointHandler();
	~MidPointHandler();

	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void PartProgramData();
	virtual void EscapebuttonPress();
};
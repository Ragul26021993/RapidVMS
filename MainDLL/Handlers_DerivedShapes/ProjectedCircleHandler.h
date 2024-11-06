//Class to draw the point...
#pragma once
#include "..\Handlers\MouseHandler.h"

class ProjectedCircleHandler:
	public MouseHandler
{

//Variable declarations//
public:
	ProjectedCircleHandler();
	~ProjectedCircleHandler();

	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void PartProgramData();
	virtual void EscapebuttonPress();
};
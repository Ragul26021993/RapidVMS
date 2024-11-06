//Class to draw the point...
#pragma once
#include "..\Handlers\MouseHandler.h"

class NearestPointHandler:
	public MouseHandler
{
	bool shapeAs2D;
//Variable declarations//
public:
	NearestPointHandler(bool NearestPoint, bool shapeas2d = true);
	~NearestPointHandler();

	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void PartProgramData();
	virtual void EscapebuttonPress();
};
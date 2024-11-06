//Class to draw rectangle..
#pragma once
#include "MouseHandler.h"

class ZoomInOutHandler:
	public MouseHandler
{
private:
	Vector point[2], Mpoint[2];
	bool Zoomin_outflag;

public:
	ZoomInOutHandler(bool zoomin);
	~ZoomInOutHandler();

	virtual void LmouseUp(double x, double y);
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
};
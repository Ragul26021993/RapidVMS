//Class to add new UCS with selected point as UCS center...//
#pragma once
#include "MouseHandler.h"

class UCSPointHandle:
	public MouseHandler
{

public:
	UCSPointHandle();
	~UCSPointHandle();
	//Virtual function....
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
};
//Default handlr: to do nothing...//
#pragma once
#include "MouseHandler.h"

class ProbeHandler :
	public MouseHandler
{
public:
	//Constructor and destructor...//
	ProbeHandler(bool flag = true);
	~ProbeHandler();

	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
};
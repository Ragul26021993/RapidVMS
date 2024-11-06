//Default handlr: to do nothing...//
#pragma once
#include "MouseHandler.h"

class DefaultHandler :
	public MouseHandler
{
public:
	//Constructor and destructor...//
	DefaultHandler(bool flag = true);
	~DefaultHandler();

	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
};
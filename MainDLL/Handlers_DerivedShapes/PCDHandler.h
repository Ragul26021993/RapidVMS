//Handler for PCD...//

#pragma once
#include "..\Handlers\MouseHandler.h"

class PCDHandler:
	public MouseHandler
{

public:
	PCDHandler(bool AddPcdCircle = true);
	~PCDHandler();

	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void RmouseDown(double x, double y);
};
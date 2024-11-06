//Handler for PCD...//

#pragma once
#include "..\Handlers\MouseHandler.h"

class PCDHandler3D:
	public MouseHandler
{

public:
	PCDHandler3D(bool AddPcdCircle = true);
	~PCDHandler3D();

	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void RmouseDown(double x, double y);
};
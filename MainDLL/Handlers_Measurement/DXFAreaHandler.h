//To Measure the Area of DXF loop...
//Developed by Sathyanarayana...
#pragma once
#include "..\Handlers\MouseHandler.h"
#include "..\Shapes\Perimeter.h"

class DXFAreaHandler:
	public MouseHandler
{
private:
	Shape *Cshape;
	
	void init();
public:
	DXFAreaHandler();
	virtual ~DXFAreaHandler();
	
	//Virtual function...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
};
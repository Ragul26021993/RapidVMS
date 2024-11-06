//Handler for generating a best fit combo of arc-arc tangential to each other...//

#pragma once
#include "..\Handlers\MouseHandler.h"
class Line;
class Circle;

class ArcTanToLineHandler:
	public MouseHandler
{
private:
	Line* line1;
	Circle* arc2;
	bool linedone;
	bool arcdone;

	void init();
	void ResetShapeHighlighted();
public:
	ArcTanToLineHandler();
	~ArcTanToLineHandler();
	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
};
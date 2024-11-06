//Handler for generating a best fit combo of arc-arc tangential to each other...//

#pragma once
#include "..\Handlers\MouseHandler.h"
class Circle;

class ArcArcHandler:
	public MouseHandler
{
private:
	Circle* arc1;
	Circle* arc2;

	void init();
	void ResetShapeHighlighted();
public:
	ArcArcHandler();
	~ArcArcHandler();
	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
};
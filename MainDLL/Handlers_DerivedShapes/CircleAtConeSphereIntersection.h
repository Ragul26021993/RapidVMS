//Handler for generating a best fit combo of arc-arc tangential to each other...//

#pragma once
#include "..\Handlers\MouseHandler.h"
class Shape;

class CircleAtConeSphereIntersection:
	public MouseHandler
{
private:
	void init();
	Shape *shapeArray[4];
public:

	CircleAtConeSphereIntersection();
	~CircleAtConeSphereIntersection();
	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
};
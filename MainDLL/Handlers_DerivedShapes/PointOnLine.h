//Class to draw the derived points.. point on line)
#pragma once
#include "..\Handlers\MouseHandler.h"
class Line;

class PointOnLine:
	public MouseHandler
{
private:
	Line* ParentShape;
	double angle, intercept;
	double point[2];
	bool runningPartprogramValid;

	void init();
	void ResetShapeHighlighted();
public:
	PointOnLine();
	~PointOnLine();
	
	//Virtual functions...
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();

	virtual void PartProgramData();
};
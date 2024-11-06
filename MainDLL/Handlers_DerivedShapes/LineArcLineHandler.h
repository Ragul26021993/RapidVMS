//Handler for generating a best fit combo of line-arc-line s.t arc is tangential to the two lines at its ends...//

#pragma once
#include "..\Handlers\MouseHandler.h"

class Shape;
class Line;
class Circle;

class LineArcLineHandler:
	public MouseHandler
{
private:
	Shape *ParentShape1, *ParentShape2, *ParentShape3;
	Line* line1;
	Line* line2;
	Circle* circle1;
	int line_ct, arc_ct;

	void init();
	void ResetShapeHighlighted();
public:
	LineArcLineHandler();
	~LineArcLineHandler();
	//function to update the best fit line-arc-line
	void updatelinearcline();
	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void PartProgramData();
	virtual void EscapebuttonPress();
};
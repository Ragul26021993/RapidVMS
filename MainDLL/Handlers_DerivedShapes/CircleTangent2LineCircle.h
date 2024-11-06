//Class to draw, calculate the Circle tangential to line and cirlce:
//Shape selection, Mouse move, new shape add etc..
#pragma once
#include "..\Handlers\MouseHandler.h"
class Line;
class Circle;

class CircleTangent2LineCircle:
	public MouseHandler
{
//Variable declaration...//
private:
	Line* ParentLine;
	Circle* ParentCircle;
	bool LineSelected, CircleSelected;
	double angle, intercept, cen[2], center_ans[2], rad, circle_rad, Minradius;
	bool runningPartprogramValid, Intersecting, validflag;

	void init();
	void drawExtensionForTangentialCircle(double x, double y, double r);
	void getShapeDimensions(); // shape Parameters.
	void ResetShapeHighlighted();
public:
	CircleTangent2LineCircle();
	~CircleTangent2LineCircle();

	//Virtual functions..//
	virtual void LmouseDown();
	virtual void mouseMove();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void MouseScroll(bool flag);
	virtual void PartProgramData();
	virtual void SetAnyData(double *data);
};
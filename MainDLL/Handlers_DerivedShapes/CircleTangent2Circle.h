//Class calculates Tangential circle to a given circle.
#pragma once
#include "..\Handlers\MouseHandler.h"
class Circle;

class CircleTangent2Circle:
	public MouseHandler
{
//Variable declaration...//
private:
	Circle *ParentShape;
	double center[2], radius, circle_radius, center_ans[2];
	bool runningPartprogramValid, DefinedDia;

	void init();
	void ResetShapeHighlighted();
public:
	CircleTangent2Circle();
	~CircleTangent2Circle();

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
//To calculate the tangential circle to two circles..
#pragma once
#include "..\Handlers\MouseHandler.h"

class Circle;

class CircleTangent2TwoCircle1:
	public MouseHandler
{
//Variable declaration...//
private:
	Circle *ParentShape1, *ParentShape2;
	double center1[2], center2[2], radius1, radius2, center_ans[2], circle_rad, Minradius, MaxRadius;
	bool runningPartprogramValid, Intersecting, validflag;
	bool OneCircleInsideOther;

	void init();
	void getCircledimensions();
	void CheckAllConditions();
	void ResetShapeHighlighted();
public:
	CircleTangent2TwoCircle1();
	~CircleTangent2TwoCircle1();

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
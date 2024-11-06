//Class to calculate the Tangential circle to two lines..
#pragma once
#include "..\Handlers\MouseHandler.h"
class Line;

class CircleTangent2TwoLine:
	public MouseHandler
{
//Variable declaration...//
private:
	Line *ParentShape1, *ParentShape2;
	double angle1, intercept1, angle2, intercept2, center_ans[2], circle_rad;
	bool runningPartprogramValid, validflag;

	void init();
	void drawExtensionForTangentialCircle(double x, double y, double r);
	void getLineDimensions();
	void ResetShapeHighlighted();
public:
	
	CircleTangent2TwoLine();
	~CircleTangent2TwoLine();

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
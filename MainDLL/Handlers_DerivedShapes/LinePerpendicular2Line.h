//Class to handle the derived lines...//
//Contains all types of derived lines..//
#pragma once
#include "..\Handlers\MouseHandler.h"
class Line;

class LinePerpendicular2Line:
	public MouseHandler
{
private:	
	Line* ParentShape;// parent Shape..
	double LineLength, angle, intercept, point1[2], point2[2];
	bool runningPartprogramValid, LengthDefined;
	int linepos;

	void init();
	void ResetShapeHighlighted();
public:
	//Constructor..//
	LinePerpendicular2Line();
	~LinePerpendicular2Line();

	//Virtual functions..
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void MouseScroll(bool flag);
	virtual void PartProgramData();
	virtual void SetAnyData(double *data);
	virtual void RmouseDown(double x, double y);
};
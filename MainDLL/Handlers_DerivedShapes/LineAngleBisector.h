//Class to handle the derived lines...//
//Contains all types of derived lines..//
#pragma once
#include "..\Handlers\MouseHandler.h"
class Line;

class LineAngleBisector:
	public MouseHandler
{
private:	
	Line* ParentShape1, *ParentShape2;// parent Shape..
	double LineLength, angle1, intercept1, angle2, intercept2, point1[2], point2[2];
	bool runningPartprogramValid, LengthDefined;
	int linepos;

	void getLinedimensions();
	void init();
	void ResetShapeHighlighted();
public:
	//Constructor..//
	LineAngleBisector();
	~LineAngleBisector();

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
//Class to handle the derived lines...//
//Contains all types of derived lines..//
#pragma once
#include "..\Handlers\MouseHandler.h"
class Circle;

class LineTangent2Circle:
	public MouseHandler
{
private:	
	Circle* ParentShape;// parent Shape..
	double LineLength, CCenter[2], radius, point1[2], point2[2];
	bool runningPartprogramValid, LengthDefined;
	int linepos;

	void init();
	void ResetShapeHighlighted();
public:
	//Constructor..//
	LineTangent2Circle();
	~LineTangent2Circle();

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
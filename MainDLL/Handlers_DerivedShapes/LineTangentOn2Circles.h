//Class to handle the derived lines...//
//Contains all types of derived lines..//
#pragma once
#include "..\Handlers\MouseHandler.h"
class Circle;

class LineTangentOn2Circles:
	public MouseHandler
{
private:	
	Circle* ParentShape1, *ParentShape2;// parent Shape..
	double LineLength, Center1[2], radius1, Center2[2], radius2, point1[2], point2[2];
	bool runningPartprogramValid, LengthDefined;

	void init();
	void ResetShapeHighlighted();
public:
	//Constructor..//
	LineTangentOn2Circles();
	~LineTangentOn2Circles();

	//Virtual functions..
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void MouseScroll(bool flag);
	virtual void PartProgramData();
	virtual void SetAnyData(double *data);
};
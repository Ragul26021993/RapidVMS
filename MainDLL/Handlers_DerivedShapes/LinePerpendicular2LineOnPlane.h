//Class to handle the derived Planes...//

#pragma once
#include "..\Handlers\MouseHandler.h"
class Plane;
class Line;

class LinePerpendicular2LineOnPlane:
	public MouseHandler
{
private:	
	Plane* ParentPlane;
	Line *ParentLine;
	void init();

public:
	//Constructor..//
	LinePerpendicular2LineOnPlane();
	~LinePerpendicular2LineOnPlane();

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
//
//Class used to draw the selection rectangle....//
#pragma once
#include "MouseHandler.h"
#include "..\Shapes\Vector.h"

class ShapeWithList;

class CloudPoints3DStitching :
	public MouseHandler
{
private:
	bool FirstShape;
	ShapeWithList *RefrenceShape, *ShapeTobeAdded;
	Vector PointsDRO[2];
	double PointsRef[1000], PointsObj[1000];
	int PointsRefCount, PointsObjCount;
public:
	CloudPoints3DStitching();
	~CloudPoints3DStitching();
public:
	//Virtual functions..//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void LmouseUp(double x,double y);
	virtual void RmouseDown(double x, double y);
	virtual void MouseScroll(bool flag);
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
};
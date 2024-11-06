#pragma once
#include "MouseHandler.h"
#include "..\Shapes\Vector.h"

class Shape;

class MoveShapeHandler :
	public MouseHandler
{
private:
	bool moving;
	double stepsize;
	std::list<Shape*> MShapeList;
	Vector totalmovement, v, tt;
	void UpdateDxfDeviation();
public:
	MoveShapeHandler();
	~MoveShapeHandler();
	void setRCollectionBase(std::list<Shape*> coll);
	
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void LmouseUp(double x, double y);
	virtual void draw(int windowno, double WPixelWidth);
	virtual void keyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void EscapebuttonPress();
	virtual void SetAnyData(double *data);
	virtual void PartProgramData();
};
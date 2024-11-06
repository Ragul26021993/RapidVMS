#pragma once
#include "..\Handlers\MouseHandler.h"
class Shape;
class Vector;

class ProjectionHandler:
	public MouseHandler
{
//Variable declaration...//
private:
	Shape *ParentShape1, *PointParent1, *PointParent2;
	bool runningPartprogramValid, validflag;
	Vector *ParentPoint;
	void init();
    void ResetShapeHighlighted();

public:
	
	ProjectionHandler();
	~ProjectionHandler();

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
#pragma once
#include "..\Handlers\MouseHandler.h"
class Shape;

class PlaneThrough2Lines3D:
	public MouseHandler
{
//Variable declaration...//
private:
	Shape *ParentShape1, *ParentShape2;
	bool runningPartprogramValid, validflag;

	void init();
    void ResetShapeHighlighted();
public:
	
	PlaneThrough2Lines3D();
	~PlaneThrough2Lines3D();

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
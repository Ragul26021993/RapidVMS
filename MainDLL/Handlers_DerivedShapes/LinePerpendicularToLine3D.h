//Class to handle the derived Lines...//

#pragma once
#include "..\Handlers\MouseHandler.h"
class Line;

class LinePerpendicularToLine3D:
	public MouseHandler
{
private:	

	Line *CurrentLine, *ParentShape;// parent Shape..
	double Length, ParentlineParam[6], NewLinePoints[6];
	void init();

public:
	//Constructor..//
	LinePerpendicularToLine3D();
	~LinePerpendicularToLine3D();

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
//Class to handle the derived Lines...//
//Contains all types of derived Lines..//
#pragma once
#include "..\Handlers\MouseHandler.h"
class Line;
class LineParallelToLine3D:
	public MouseHandler
{
private:	
	Shape* ParentShape;// parent Shape..
	double offset[3], ParentlineParam[6];
	bool OffsetDefined;

	void init();

public:
	//Constructor..//
	LineParallelToLine3D();
	~LineParallelToLine3D();

	
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
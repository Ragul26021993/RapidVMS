//Class to handle the derived planes...//
//Contains all types of derived planes..//
#pragma once
#include "..\Handlers\MouseHandler.h"
class Line;

class PlanePerpendicular2LineHandler:
	public MouseHandler
{
private:	
	Line* ParentShape;// parent Shape..
	double offset, planePnts[12];
	bool OffsetDefined;
	void init();

public:

	//Constructor..//
	PlanePerpendicular2LineHandler();
	~PlanePerpendicular2LineHandler();
	
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
	void CalculatePerpendicularPlane();

};
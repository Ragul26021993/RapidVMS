//Class to handle the derived Planes...//

#pragma once
#include "..\Handlers\MouseHandler.h"
class Plane;
class FramegrabBase;

class PlanePerpendicular2Plane:
	public MouseHandler
{
private:	
	Plane* ParentShape, *CurrentPlane;
	double offset;
	bool OffsetDefined;
	void init();
	void ResetShapeHighlighted();
	double planePnts[12], twoclick[6];
	list<FramegrabBase*> PointActions;
public:
	//Constructor..//
	PlanePerpendicular2Plane();
	~PlanePerpendicular2Plane();

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
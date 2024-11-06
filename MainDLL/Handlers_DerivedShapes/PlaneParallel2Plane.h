//Class to handle the derived planes...//
//Contains all types of derived planes..//
#pragma once
#include "..\Handlers\MouseHandler.h"
class Plane;

class PlaneParallel2Plane:
	public MouseHandler
{
private:	
	Plane* ParentShape;// parent Shape..
	double planePnts[12], offset;
	bool OffsetDefined;
	void init();

public:
	//Constructor..//
	PlaneParallel2Plane();
	~PlaneParallel2Plane();
	
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
	void CalculateParallelPlane();
};
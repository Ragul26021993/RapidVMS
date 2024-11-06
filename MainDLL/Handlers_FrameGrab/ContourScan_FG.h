//
//Class used to draw the selection rectangle....//
#pragma once
#include "..\Handlers\MouseHandler.h"

class ContourScan_FG :
	public MouseHandler
{
public:
    int SeletionFrames_Width, SeletionFrames_Height;
	bool scanning;
	enum SELECTIONSHAPETYPE
	{
	   CIRCULAR,
	 //  ANGULARRECTANGLE,
	   ARC,
	   DEFAULT,
	}SelectionShapeType;
	Vector PointsDRO[3];
	Shape *ShapeToAddPnts; 

	ContourScan_FG();
	~ContourScan_FG();
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
    virtual void PartProgramData();
	bool CreateUcs(double *points);
}; 
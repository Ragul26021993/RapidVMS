//
//Class used to draw the selection rectangle....//
#pragma once
#include "..\Handlers\MouseHandler.h"

class ImageAnalysisFrameGrabeHander :
	public MouseHandler
{
public:
	Vector PointsDRO[2];
	Shape *ShapeToAddPnts; 
	int FilterCount, MatrixSize;
	double ThresholdPixel;
	ImageAnalysisFrameGrabeHander();
	~ImageAnalysisFrameGrabeHander();

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
}; 
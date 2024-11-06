#pragma once
#include "CadAlignHandler.h"

class Program1PtHandler:
	public CadAlignHandler
{
//Variable declaration...//
private:
	Vector points[2];
	int PtsCount;
protected:
	//Virtual functions...//
	virtual void SelectPoints_Shapes();
	virtual void Align_Finished();
	virtual void drawSelectedShapesOndxf();
	virtual void drawSelectedShapeOnRCad();
	virtual void drawSelectedShapesOnvideo();
	virtual void Align_mouseMove(double x, double y);
	
public:
	Program1PtHandler(); // Constructor..
	~Program1PtHandler(); // Destructor..Release memory..
};
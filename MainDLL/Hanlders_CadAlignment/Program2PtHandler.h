#pragma once
#include "CadAlignHandler.h"

class Program2PtHandler:
	public CadAlignHandler
{
//Variable declaration...//
private:
	Vector points[4];;
protected:
	//Virtual functions...//
	virtual void SelectPoints_Shapes();
	virtual void Align_Finished();
	virtual void drawSelectedShapesOndxf();
	virtual void drawSelectedShapeOnRCad();
	virtual void drawSelectedShapesOnvideo();
	virtual void Align_mouseMove(double x, double y);
	
public:
	Program2PtHandler(); // Constructor..
	~Program2PtHandler(); // Destructor..Release memory..
};
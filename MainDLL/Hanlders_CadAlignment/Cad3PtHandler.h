#pragma once
#include "CadAlignHandler.h"

class Cad3PtHandler:
	public CadAlignHandler
{
//Variable declaration...//
private:
	Vector Point1[3], Point2[3];
protected:
	//Virtual functions...//
	virtual void SelectPoints_Shapes();
	virtual void Align_Finished();
	virtual void drawSelectedShapesOndxf();
	virtual void drawSelectedShapesOnvideo();
	virtual void drawSelectedShapeOnRCad();
	virtual void Align_mouseMove(double x, double y);
public:
	Cad3PtHandler(); // Constructor..
	~Cad3PtHandler(); // Destructor..Release memory..
};
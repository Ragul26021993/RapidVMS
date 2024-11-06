//Cad 2 point Handler.. derived From cad Align Handler...
//Used to get the point for CAD alignment
//Calculates the transformation matrix and set..
#pragma once
#include "CadAlignHandler.h"

class Cad2PtHandler:
	public CadAlignHandler
{
	Vector points[2];
	Vector *targetP1, *targetP2;
	int clickWindow[2];
	bool runningPartprogramValid;
//Variable declaration...//
protected:
	//Virtual functions...//
	virtual void SelectPoints_Shapes();
	virtual void Align_Finished();
	virtual void drawSelectedShapesOndxf();
	virtual void drawSelectedShapesOnvideo();
	virtual void Align_mouseMove(double x, double y);
	virtual void HandlePartProgramData();
public:
	Cad2PtHandler(); // Constructor..
	~Cad2PtHandler(); // Destructor..Release memory..
	
};


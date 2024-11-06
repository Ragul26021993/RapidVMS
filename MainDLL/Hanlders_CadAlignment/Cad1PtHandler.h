//Cad 1 point Handler.. derived From cad Align Handler...
//Used to get the point for CAD alignment
//Calculates the transformation matrix and set..
#pragma once
#include "CadAlignHandler.h"

class Cad1PtHandler:
	public CadAlignHandler
{
//Variable declaration...//
private:

	Vector point1, oldPoint;
	Vector *point2;
	bool runningPartprogramValid;

protected:

	//Virtual functions...//
	virtual void SelectPoints_Shapes();
	virtual void Align_Finished();
	virtual void drawSelectedShapesOndxf();
	virtual void drawSelectedShapesOnvideo();
	virtual void Align_mouseMove(double x, double y);
	virtual void LmaxmouseDown();
	virtual void HandlePartProgramData();

public:

	Cad1PtHandler(); // Constructor..
	~Cad1PtHandler(); // Destructor..Release memory..
	void SetVideoPointFlag(Vector& pt2);
};


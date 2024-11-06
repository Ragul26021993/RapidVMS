//Cad 1 point 1 line Handler.. derived From cad Align Handler...
//Used to get the point for CAD alignment
//Calculates the transformation matrix and set..
#pragma once
#include "CadAlignHandler.h"

class Cad1Pt1LineHandler:
	public CadAlignHandler
{
//Variable declaration...//
private:
	Vector Dxfpoint;
	Vector *VideoPoint;
	bool runningPartprogramValid;
	Shape *DxfShape, *VideoShape;
	bool CheckHighlightedShape();
	void ResetShapeHighlighted();
protected:
	//Virtual functions...//
	virtual void SelectPoints_Shapes();
	virtual void Align_Finished();
	virtual void drawSelectedShapesOndxf();
	virtual void drawSelectedShapesOnvideo();
	virtual void Align_mouseMove(double x, double y);
	virtual void HandlePartProgramData();
public:
	Cad1Pt1LineHandler(); // Constructor..
	~Cad1Pt1LineHandler(); // Destructor..Release memory..
};
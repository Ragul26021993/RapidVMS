//Used to get the point for CAD alignment
//Calculates the transformation matrix and set..
#pragma once
#include "CadAlignHandler.h"

class Cad2LineHandler:
	public CadAlignHandler
{
//Variable declaration...//
private:
	int VideoShapeCnt, DxfShapeCnt;
	Shape *DxfShapes[2], *VideoShapes[2];
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
	Cad2LineHandler(); // Constructor..
	~Cad2LineHandler(); // Destructor..Release memory..
};


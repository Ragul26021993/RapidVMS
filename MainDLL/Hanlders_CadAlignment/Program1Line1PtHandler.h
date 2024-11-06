#pragma once
#include "CadAlignHandler.h"

class Program1Line1PtHandler:
	public CadAlignHandler
{
//Variable declaration...//
private:
	Vector Dxfpoint;
	Shape *DxfShape, *VideoShape;
	bool CheckHighlightedShape();
	void ResetShapeHighlighted();
protected:
	//Virtual functions...//
	virtual void SelectPoints_Shapes();
	virtual void Align_Finished();
	virtual void drawSelectedShapesOndxf();
	virtual void drawSelectedShapeOnRCad();
	virtual void drawSelectedShapesOnvideo();
	virtual void Align_mouseMove(double x, double y);
	
public:
	Program1Line1PtHandler(); // Constructor..
	~Program1Line1PtHandler(); // Destructor..Release memory..
};
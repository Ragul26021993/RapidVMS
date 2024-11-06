//Cad 1 point 1 line Handler.. derived From cad Align Handler...
//Used to get the point for CAD alignment
//Calculates the transformation matrix and set..
#pragma once
#include "CadAlignHandler.h"

class Cad1Pt1LineHandler3D:
	public CadAlignHandler
{
//Variable declaration...//
private:
	Vector Centrepoint, Line1Point1, Line1Point2, Line2Point1, Line2Point2, Dxfpoint;
	bool DxfSphereFlag, DxfSphereFlag1, DxfPointFlag, DxfPointFlag1, DxfCylinderFlag, DxfLineFlag;
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
public:
	Cad1Pt1LineHandler3D(); // Constructor..
	~Cad1Pt1LineHandler3D(); // Destructor..Release memory..
};

//Created by Rahul Singh Bhadauria.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
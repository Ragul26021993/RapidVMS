//Cad 1 point 1 line Handler.. derived From cad Align Handler...
//Used to get the point for CAD alignment
//Calculates the transformation matrix and set..
#pragma once
#include "CadAlignHandler.h"

class PP1Pt1LineHandler:
	public CadAlignHandler
{
//Variable declaration...//
private:
	Vector Rcadpoint, VideoPoint;
	bool VideoPointFlag, VideoShapeFlag, RcadPointFlag, RcadShapeFlag;
	Shape *VideoShape, *RcadShape;
protected:
	//Virtual functions...//
	virtual void SelectPoints_Shapes();
	virtual void Align_Finished();
	virtual void drawSelectedShapesOndxf();
	virtual void drawSelectedShapeOnRCad();
	virtual void drawSelectedShapesOnvideo();
	virtual void Align_mouseMove(double x, double y);
public:
	PP1Pt1LineHandler(); // Constructor..
	~PP1Pt1LineHandler(); // Destructor..Release memory..
};

//Created by Rahul Singh Bhadauria....
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
//Cad 2 point Handler.. derived From cad Align Handler...
//Used to get the point for CAD alignment
//Calculates the transformation matrix and set..
#pragma once
#include "CadAlignHandler.h"

class PP2PtHandler:
	public CadAlignHandler
{
//Variable declaration...//
private:
	Vector points[4];
	int VideoCnt, RcadCnt;
protected:
	//Virtual functions...//
	virtual void SelectPoints_Shapes();
	virtual void Align_Finished();
	virtual void drawSelectedShapesOndxf();
	virtual void drawSelectedShapeOnRCad();
	virtual void drawSelectedShapesOnvideo();
	virtual void Align_mouseMove(double x, double y);
public:
	PP2PtHandler(); // Constructor..
	~PP2PtHandler(); // Destructor..Release memory..
};

//Created by Rahul Singh Bhadauria.....
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
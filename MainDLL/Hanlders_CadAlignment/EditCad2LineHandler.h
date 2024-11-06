//Cad 1 point Handler.. derived From cad Align Handler...
//Used to get the point for CAD alignment
//Calculates the transformation matrix and set..
#pragma once
#include "CadAlignHandler.h"

class EditCad2LineHandler:
	public CadAlignHandler
{
//Variable declaration...//
private:
	int VideoShapeCnt, DxfShapeCnt;
	Shape *DxfShapes[2], *VideoShapes[2];
	bool CheckHighlightedShape();
	void ResetShapeHighlighted();
	bool runningPartprogramValid;

protected:
	//Virtual functions...//
	virtual void SelectPoints_Shapes();
	virtual void Align_Finished();
	virtual void drawSelectedShapesOndxf();
	virtual void drawSelectedShapesOnvideo();
	virtual void drawSelectedShapeOnRCad();
	virtual void Align_mouseMove(double x, double y);
	virtual void HandlePartProgramData();
	
public:
	EditCad2LineHandler(); // Constructor..
	~EditCad2LineHandler(); // Destructor..Release memory..
};

//Created by Rahul Singh Bhadauria.......on 27 Jul 12
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
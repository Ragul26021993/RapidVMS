//Cad 1 point 1 line Handler.. derived From cad Align Handler...
//Used to get the point for CAD alignment
//Calculates the transformation matrix and set..
#pragma once
#include "CadAlignHandler.h"

class EditCad1Pt1LineHandler:
	public CadAlignHandler
{
//Variable declaration...//
private:
	Vector Dxfpoint, VideoPoint;
	bool VideoPointFlag, VideoShapeFlag, DxfPointFlag, DxfShapeFlag, runningPartprogramValid, AlignMentCompleted;
	Shape *DxfShape, *VideoShape;
	bool CheckHighlightedShape();
	void ResetShapeHighlighted();
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
	EditCad1Pt1LineHandler(); // Constructor..
	~EditCad1Pt1LineHandler(); // Destructor..Release memory..
};

//Created by Rahul Singh Bhadauria.......
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
//Cad 1 point Handler.. derived From cad Align Handler...
//Used to get the point for CAD alignment
//Calculates the transformation matrix and set..
#pragma once
#include "CadAlignHandler.h"

class EditCad1PtHandler:
	public CadAlignHandler
{
//Variable declaration...//
private:
	Vector points[2];
	bool VideoFlag, runningPartprogramValid, AlignMentCompleted;

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
	EditCad1PtHandler(); // Constructor..
	~EditCad1PtHandler(); // Destructor..Release memory..
	void SelectPoints_HeightGauge(double x, double y, double z);
};

//Created by Rahul Singh Bhadauria.......
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
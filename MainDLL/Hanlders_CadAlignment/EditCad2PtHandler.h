//Cad 2 point Handler.. derived From cad Align Handler...
//Used to get the point for CAD alignment
//Calculates the transformation matrix and set..
#pragma once
#include "CadAlignHandler.h"

class EditCad2PtHandler:
	public CadAlignHandler
{
//Variable declaration...//
private:
	Vector points[4];
	Vector Old_Point;
	int VideoCnt, RcadCnt;
	bool runningPartprogramValid, AlignMentCompleted;
protected:
	//Virtual functions...//
	virtual void SelectPoints_Shapes();
	virtual void Align_Finished();
	virtual void drawSelectedShapesOndxf();
	virtual void drawSelectedShapeOnRCad();
	virtual void drawSelectedShapesOnvideo();
	virtual void Align_mouseMove(double x, double y);
	virtual void HandlePartProgramData();
public:
	EditCad2PtHandler(); // Constructor..
	~EditCad2PtHandler(); // Destructor..Release memory
	void SelectPoints_HeightGauge(double x, double y, double z);

};

//Created by Rahul Singh Bhadauria.....
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
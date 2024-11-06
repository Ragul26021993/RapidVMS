#pragma once
#include "CadAlignHandler.h"

class AutoAlignHandler:
	public CadAlignHandler
{
	void DoAutoAlignment(bool SelectedPt, int alignment_mode);
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
	AutoAlignHandler(); // Constructor..
	~AutoAlignHandler(); // Destructor..Release memory..
	AutoAlignHandler(bool SelectedShapes, int alignment_mode);
};

//Created by Rahul Singh Bhadauria on 18 Jul 12.........
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
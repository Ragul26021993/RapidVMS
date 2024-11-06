//Handler for parallel arc...//

#pragma once
#include "MouseHandler.h"

class DeviationHandler :
	public MouseHandler
{
	bool runningPartprogramValid;
	void UpdateDeviationParam();
public:
	DeviationHandler();
	~DeviationHandler();

	void SetAnyData(double *data);
	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	//Virtual funtion to handle partprogram...//
	virtual void PartProgramData();
};
// Created  by Rahul Singh Bhadauria on 19 Jul 12........
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
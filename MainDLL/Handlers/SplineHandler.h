#pragma once
#include "MouseHandler.h"
class ShapeWithList;

class SplineHandler :
	public MouseHandler
{
	ShapeWithList* myshape;
	double LastPt[3];
	bool runningPartprogramValid, firstClick, MouseMoveFlag;
	void init();
	int NumberOfPts;
	bool Pflag, UndoFlag, RedoFlag;
	void UpdateControlPointList();
public:
	//Constructor and destructor...//
	SplineHandler();
	SplineHandler(ShapeWithList* sh, bool undoFlag);
	~SplineHandler();
	void PartProgramData();
	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void MmouseDown(double x,double y);
	virtual void RmouseDown(double x,double y);
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
};
//Created by Rahul Singh Bhadauria.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
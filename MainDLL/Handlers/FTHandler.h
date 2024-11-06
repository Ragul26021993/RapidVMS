//Class used to draw the Fasttrace.. combination of line and arc...
//Here two types of arc.. end point and middlepoint...//
#pragma once
#include "MouseHandler.h"
#include "../Shapes/SinglePoint.h"

class FramegrabBase;
class ShapeWithList;
class RAction;

class FTHandler :
	public MouseHandler
{
//Variable declaration..//
private:
	ShapeWithList *cshape;
	FramegrabBase *firstClick, *rightClick;
	SinglePoint TempClick;
	list<RAction*> CurrentActionCollection;

	bool Rflag, undoflag, valid, CurrentFTType_Mid;
	bool EditShapeFor_Shiftdrag, Edit_LastPoint;
	double center[2], point3[2], radius, startangle, sweepangle;

public:
	FTHandler(bool FtType_Middle);
	FTHandler(bool FtType_Middle, ShapeWithList* csh, bool unflag);
	~FTHandler();

	void setPoint(FramegrabBase* actn, int i);
	void setRflag(bool flag);
	void SettheClickvalue(int i, double x, double y, double z);

	//Virtual functions...//
	virtual void mouseMove();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void LmouseUp(double x,double y);
	virtual void RmouseDown(double x,double y);
	virtual void MmouseDown(double x,double y);
	virtual void EscapebuttonPress();
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
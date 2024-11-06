//Focus Depth Handler Class... 
//Inherited from Mouse Handler.. Used to Handle Two box Focus Depth User actions....
#pragma once
#include "..\Handlers\MouseHandler.h"

class FocusDepthMultiBoxHandler:
	public MouseHandler
{
private:
	int Rectwidth;
	bool runningPartprogramValid; //Used in Partprogram
	bool Validflag, TwoBoxDepth_Mode;
	bool FocusDepthStarted;
	void init(); //Initialise default settings..
	int RectIntArray[20];
	double RectDroArray[20];
	int ClickCount;
public:
	FocusDepthMultiBoxHandler(bool TwoBoxMode); //default constructor..
	FocusDepthMultiBoxHandler(FramegrabBase *fb); //Used for already  existing point action .. in Partprogram!
	~FocusDepthMultiBoxHandler(); //destructor... clear the memory related..!

	//Virtual functions..//
	virtual void mouseMove();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void EscapebuttonPress();
	virtual void PartProgramData();
	virtual void MouseScroll(bool flag);
	bool FocusButtonClicked();
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
//AddPoint Handler Class... 
//Inherited from Mouse Handler.. Used to add Points to selected shape using Flexible crosshair..
#pragma once
#include "..\Handlers\MouseHandler.h"

class ScanModeHandler:
	public MouseHandler
{
private:
	int Rectwidth;
	bool runningPartprogramValid; //Used in Partprogram
	void init(); //Initialise default settings..

public:
	ScanModeHandler(); //default constructor..
	ScanModeHandler(FramegrabBase *fb); //Used for already  existing point action .. in Partprogram!
	~ScanModeHandler(); //destructor... clear the memory related..!

	//Virtual functions..//
	virtual void mouseMove();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void EscapebuttonPress();
	virtual void PartProgramData();
	virtual void MouseScroll(bool flag);
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
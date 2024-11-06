//AddPoint Handler Class... 
//Inherited from Mouse Handler.. Used to add Points to selected shape using Flexible crosshair..
#pragma once
#include "..\Handlers\MouseHandler.h"
#include "..\FrameGrab\FramegrabBase.h"

class FlexibleCrossHairHandler:
	public MouseHandler
{
private:
	bool valid, runningPartprogramValid; //Used in Partprogram
	void init(); //Initialise default settings..
	void AddPointToSelectedShape();
public:
	FlexibleCrossHairHandler(); //default constructor..
	FlexibleCrossHairHandler(FramegrabBase *fb); //Used for already  existing point action .. in Partprogram!
	~FlexibleCrossHairHandler(); //destructor... clear the memory related..!

	//Virtual functions..//
	virtual void mouseMove();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void EscapebuttonPress();
	virtual void PartProgramData();
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
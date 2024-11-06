//Cad Align Hanlders.. Derived From Mouse handler...
//Base class to Cad alignment handlers..

#pragma once
#include "..\Handlers\MouseHandler.h"

class CadAlignHandler:
	public MouseHandler
{
public:
	CadAlignHandler(); // Constructor..
	virtual ~CadAlignHandler(); // Destructor..Release memory..

	//Virtual functions...//
	virtual void LmouseDown();
	virtual void mouseMove();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void PartProgramData();
	//bool FloatwithMouse;
protected:
	virtual void SelectPoints_Shapes() = 0;
	virtual void Align_Finished() = 0;
	virtual void drawSelectedShapesOndxf() = 0;
	virtual void drawSelectedShapesOnvideo() = 0;	
	virtual void Align_mouseMove(double x, double y) = 0;
	virtual void drawSelectedShapeOnRCad();
	virtual void HandlePartProgramData();
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
//Base class to handle the Frame grab draw..
//Derived from mouse handler..
#pragma once
#include "..\Handlers\MouseHandler.h"
#include "..\Shapes\Perimeter.h"

class FrameGrabIdAndOd:
	public MouseHandler
{

private:
	RapidEnums::RAPIDFGACTIONTYPE CurrentFGType;
	
public:
	FrameGrabIdAndOd(RapidEnums::RAPIDFGACTIONTYPE Fgtype);
	~FrameGrabIdAndOd();

	//Vitrtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void PartProgramData();
	void SetAddPointAction();
};
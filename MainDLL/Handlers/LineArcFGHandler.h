//LineArc FrameGrab Handler Class..Derived from MouseHandler..
//Used to select the first action FrameGrab from list of all linearc framegrabs 
#pragma once
#include "MouseHandler.h"
#include "..\Handlers_FrameGrab\FrameGrabAngularRect.h"
#include "..\Handlers_FrameGrab\FrameGrabArc.h"

class AddPointAction;

class LineArcFGHandler:
	public MouseHandler
{
private:
	bool fgHighlighted, valid, linearcfirstptactiondone;
	map<int, AddPointAction*> pointactioncoll;
	AddPointAction* highlightedPointAction;
	AddPointAction* selectedPointAction;
	void init();
	AddPointAction* getNearestPA(double x, double y, double snapdist);

public:
	LineArcFGHandler(map<int, AddPointAction*> ptactcoll); 
	~LineArcFGHandler(); // Destructor.. Release the memory..

	//Virtual Functions../
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
};

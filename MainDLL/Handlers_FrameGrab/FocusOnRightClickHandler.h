#pragma once
#include "..\Handlers\MouseHandler.h"

class FocusOnRightClickHandler:
	public MouseHandler
{
private:
	int Rectwidth;
	void init(); //Initialise default settings..
	int RectIntArray[20];
	double RectDroArray[20];

public:
	FocusOnRightClickHandler(); //default constructor..
	~FocusOnRightClickHandler(); //destructor... clear the memory related..!

	//Virtual functions..//
	virtual void mouseMove();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void EscapebuttonPress();
	virtual void MouseScroll(bool flag);
	virtual void RmouseDown(double x, double y);
};


//Class to draw Angular Rect frame Grab..
//Derived from the Framegrab handler class..
#pragma once
#include "FrameGrabHandler.h"

class FrameGrabAngularRect:
	public FrameGrabHandler
{
//Variable declaration...//
private:
	int FgAngRectwidth;
	bool WidthDirection;
	bool fgselected;

protected:
	virtual void FG_mouseMove();
	virtual void FG_LmouseDown();
	virtual void FG_LmaxmouseDown();
	virtual void FG_ChangeDrirection();
	virtual void FG_ChangeScanDirection();
	virtual void FG_draw(double wupp, int windowno = 0);
	virtual void FG_MouseScroll(bool flag, double wupp);
	virtual void FG_UpdateFirstFrameppLoad();

public:
	void fgangrect_draw(double wupp, bool selected_fg = false);
	FrameGrabAngularRect();
	FrameGrabAngularRect(FramegrabBase* Cfg);
	~FrameGrabAngularRect();
};
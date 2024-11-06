//Class to handle Edge detection for Mouse click..
//Derived from the Framegrab handler class..
#pragma once
#include "FrameGrabHandler.h"

class FrameGrabEdgeMouseClick:
	public FrameGrabHandler
{
protected:
	virtual void FG_mouseMove();
	virtual void FG_LmouseDown();
	virtual void FG_LmaxmouseDown();
	virtual void FG_ChangeDrirection();
	virtual void FG_draw(double wupp, int windowno = 0);
	virtual void FG_MouseScroll(bool flag, double wupp);
	virtual void FG_UpdateFirstFrameppLoad();

public:
	FrameGrabEdgeMouseClick();
	FrameGrabEdgeMouseClick(FramegrabBase* Cfg);
	~FrameGrabEdgeMouseClick();
};
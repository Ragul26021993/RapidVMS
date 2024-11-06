//Mouse handler class.. base class of handlers..
//Hanldes the all Mouse related events..
//According to the current type this calls respective child..
#pragma once
#include <map>
#include <list>
#include "..\Shapes\Vector.h"
#include "..\BestFit\RapidCamControl.h"
#include "..\Engine\RapidEnums.h"

class Shape;
class FramegrabBase;

class MAINDLL_API MouseHandler
{
private:
	int clicksdone, maxclicks;
	Vector L_pixeldown;

public:
	Shape* CurrentShape; //Expose Current Selected Shape..
	FramegrabBase* baseaction; //Current Base Action..
	MouseHandler* CurrentdrawHandler; //Current handler(For Intelligent Measurement).. To draw the lines..
	double ShapeThickness; //Current Shape Thickness
	Vector clicks[15];	//15 doesn't mean anything, just a max limit..
	bool FinishedCurrentDrawing, AlignmentModeFlag;
public:
	int CurrentWindow;
	bool PreventDoublePt;
	MouseHandler();
	virtual ~MouseHandler();

	void resetClicks();
	void setMaxClicks(int clicks);
	void setClicksDone(int clicks);
	int getMaxClicks();
	int getClicksDone();

	void setClicksValue(int i, Vector* pt);
	void setClicksValue(int i, double x, double y, double z);
	void raxis_valueSet(double r);																	//vinod   16/02/2014
	void setBaseRProperty(double x, double y, double z, bool Probeflag = false);
	void drawFlexibleCh();
	bool LmouseDown_x(double x, double y, double z = 0, double R = 0, bool ConsiderZ = false, bool dontcheckucs = true);
	void mouseMove_x(double x, double y, double z = 0, bool ConsiderZ = false, bool dontcheckucs = true);
	void L_mouseDownPixel(double x, double y);
	void setMouseClick(double x, double y, double z, bool dountChceckStatus); 
	Vector& L_mouseDownPixel();
	Vector& getClicksValue(int i);
	
	//Virtual functions......///
	virtual void mouseMove() = 0;
	virtual void LmouseDown() = 0;
	virtual void draw(int windowno, double WPixelWidth) = 0;
	virtual void EscapebuttonPress() = 0;
	virtual void LmouseUp(double x, double y);
	virtual void LmaxmouseDown();
	virtual void RmouseUp(double x, double y);
	virtual void RmouseDown(double x, double y);
	virtual void MmouseUp(double x, double y);
	virtual void MmouseDown(double x, double y);
	virtual void MouseScroll(bool flag);
	virtual void keyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void keyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void PartProgramData();
	virtual void ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle);
	virtual void SetAnyData(double *data);
};
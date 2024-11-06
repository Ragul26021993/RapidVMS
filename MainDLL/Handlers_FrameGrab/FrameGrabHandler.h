//Base class to handle the Frame grab draw..
//Derived from mouse handler..
#pragma once
#include "..\Handlers\MouseHandler.h"
#include "..\FrameGrab\FramegrabBase.h"

class Shape;
class FrameGrabHandler:
	public MouseHandler
{

private:
	void DoSurfaceFramegrab();
	void AddSelectedPointToShape();
	friend void AddSelectedPointToShape_Thread(void *anything);
	map<int, Shape*> SurfaceFGShapecollection;
	bool DrawTempSFGShapes;
	bool AddSelectedPointToShape_ThreadRunning;
	void SetAreaAction(Shape* shape);
protected:
	double scandir[3];
	bool withDirectionScan, drawscanDir;
	bool runningPartprogramValid, ValidFlag;
	Vector PointsDRO[3], PointsPixel[3];
	RapidEnums::RAPIDFGACTIONTYPE CurrentFGType;
	void setAddpointaction();
	void setAddpointactioninRcad();

	virtual void FG_mouseMove() = 0;
	virtual void FG_LmouseDown() = 0;
	virtual void FG_LmaxmouseDown() = 0;
	virtual void FG_ChangeDrirection() = 0;
	virtual void FG_draw(double wupp, int windowno = 0);
	virtual void FG_MouseScroll(bool flag, double wupp) = 0;
	virtual void FG_UpdateFirstFrameppLoad() = 0;
public:
	FrameGrabHandler();
	FrameGrabHandler(FramegrabBase* Cfg);
	virtual ~FrameGrabHandler();

	//Vitrtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmouseUp(double x, double y);
	virtual void RmouseDown(double x, double y);
	virtual void MmouseDown(double x, double y);
	virtual void FG_ChangeScanDirection();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void MouseScroll(bool flag);
	virtual void PartProgramData();
	void updateGraphicsProgram();
	void setDrawFG(FramegrabBase* fb);
	int SelectSurfaceGrabbedShape(char* ShName);
	void HighlightFGShape(char* ShName);
	void RedoAlgo6ForSurfaceFG();
	//void Set_PtsandPixels(FramegrabBase* fb);
};
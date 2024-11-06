#pragma once
#include "..\Handlers\MouseHandler.h"
#include "..\Actions\LineArcAction.h"


class SM_LineArcFGHandler:
	public MouseHandler
{

private:
	int RectDirection;
	PointCollection TempPointColl;
	bool sort_pts_for_linearcsep;
    
protected:
	bool runningPartprogramValid, ValidFlag, finalShape;;
	Vector PointsDRO[3], PointsPixel[3]; 
	void AddpointCollectionFG(double WPixelWidth);
	void ChangeDirection();

public:
	//LineArcAction* ProfileScanLineArcAction;
	SM_LineArcFGHandler();
    ~SM_LineArcFGHandler();

	void AddpointCollFromCloudPnt();
	bool CalculateDiffShapes(double tolerance, double max_radius, double min_radius, double min_angle_cutoff, double noise_factor, double max_dist_betn_neighboring_pts, bool closed_loop, bool shape_as_fast_trace, bool join_all_pts);
	void AddFinalShapes(bool PerimeterShapeFlag = true);
	void ClearLineArc();

//Vitrtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmouseUp(double x, double y);
	virtual void RmouseDown(double x, double y);
	virtual void MmouseDown(double x, double y);
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void MouseScroll(bool flag);
};
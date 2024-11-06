#pragma once
#include "..\Handlers\MouseHandler.h"

class SphereFrameGrabHandler:
	public MouseHandler
{
private:
	Shape* SelectedShape; //Parent shapes for the measurement...
	bool FirstTime;	
	void init(); 	
	int count; 
public:

	SphereFrameGrabHandler(); 
	~SphereFrameGrabHandler(); //destructor... clear the memory related..!
	
	virtual void mouseMove();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void LmouseUp(double x, double y);
	virtual void LmouseDown();
	virtual void RmouseDown(double x, double y);
	virtual void MmouseDown(double x, double y);
	virtual void LmaxmouseDown();
	virtual void MouseScroll(bool flag);
	virtual void EscapebuttonPress();
	void CheckAllTheConditions();
	void create_newFramgrab();
	void get_FramGrabParameter();
};

//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!!
//Developed by Amit mishra... 
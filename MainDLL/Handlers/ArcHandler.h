//Arc Handler Class..Derived from MouseHandler..
//Used to draw new arc shape and Add its properties.. Point action, type etc..
#pragma once
#include "MouseHandler.h"
class Circle;
            
class ArcHandler:
	public MouseHandler
{
private:
	Circle* myarc; //Current arc..//
	//Variables to hold the points and arc parameters...//
	double center[2], radius, startangle, sweepangle;
	bool valid, FirstClick, RightClickFlag;
	bool runningPartprogramValid;
	RapidEnums::CIRCLETYPE ARCtype; // Current arc type
	void init();
	void AddNewArc();

public:
	ArcHandler(); //Default Constructor..//
	ArcHandler(RapidEnums::CIRCLETYPE d); //Contructor with Arc type..
	ArcHandler(RapidEnums::CIRCLETYPE d, Circle* sh, bool flag = true);
	~ArcHandler(); // Destructor.. Release the memory..

	//Virtual Functions../
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void RmouseDown(double x, double y);
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void PartProgramData();
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
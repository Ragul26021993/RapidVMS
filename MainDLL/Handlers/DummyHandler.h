//AddPoint Handler Class... 
//Inherited from Mouse Handler.. Used to add Points to selected shape using Flexible crosshair..
#pragma once
#include "..\Handlers\MouseHandler.h"

class FramegrabBase;
class Shape;

class DummyHandler:
	public MouseHandler
{
private:
	bool valid, runningPartprogramValid; //Used in Partprogram
	void init(); //Initialise default settings..
	void AddPointToSelectedShape();
	void AddNewShape();
	void AddNewShape(Shape *cs);
public:
	RapidEnums::SHAPETYPE ShapeType;
	DummyHandler(RapidEnums::SHAPETYPE d);
	DummyHandler(Shape *cs);
	DummyHandler(); //default constructor..
	DummyHandler(FramegrabBase *fb); //Used for already  existing point action .. in Partprogram!
	~DummyHandler(); //destructor... clear the memory related..!

	//Virtual functions..//
	virtual void mouseMove();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void EscapebuttonPress();
	virtual void PartProgramData();
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
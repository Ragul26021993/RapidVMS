#pragma once 
#include "..\Helper\General.h"
#include "..\Engine\BaseItem.h"

//base class Used for Fixed shapes
class FixedShape
{
protected:
	//Shape color Properties..//
	double r, g, b;
	//Center of the window..//
	double cx, cy;
	//Position and Id of the shape..//
	int _x, _y, _id;
public:
	RapidEnums::FIXEDSHAPETYPE FShapeType;
	RapidProperty<double> TextSize;
	RapidToggleProperty<bool> HatcedShp;
	RapidProperty<bool> DontClear;
public:
	//Constructor and destructor...//
	FixedShape();
	virtual ~FixedShape();

	//Virtual functions...//
	virtual void drawFixedGraphics(int pixelgp = 2) = 0;
	virtual void setToolParameter(double _first, double _second, double _third, void*) = 0;
	virtual void setShapeWidth(bool increase) = 0;
	//Common functions... Set the color and set window center...
	void setColor(double r, double g, double b);
	//Set the center of the window..//
	void setCamCenter(double x, double y);
	//Set and get the position of the fixed shape..//
	int getX();
	int getY();
	void setPosition(int x, int y);
	//Set and get the shape Id...//
	void setId(int _id);
	int getId();
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
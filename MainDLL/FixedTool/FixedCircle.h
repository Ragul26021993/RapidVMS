//Fixed circle shape...//
#pragma once
#include "FixedShape.h"
class FixedCircle:
	public FixedShape
{
private:
	double _cx, _cy, _rad; // Circle center, radius...
public:
	bool DiameterType;
	//Constructor and destructor...//
	FixedCircle();
	~FixedCircle();
	//Set the circle parameter...
	virtual void setToolParameter(double _first, double _second, double _third, void* anyThing);
	//Draw the circle...
	virtual void drawFixedGraphics(int pixelgp = 2);
	virtual void setShapeWidth(bool increase);
	//return circle parameters...//
	double Radius();
	double getCenterX();
	double getCenterY();
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
#include "stdafx.h"
#include "FixedShape.h"

//Base class.. Constructor..
FixedShape::FixedShape()
{
	//this->r = 1.0; this->g = 0.5; this->b = 1.0;
	this->r = 1.0; this->g = 0.0; this->b = 0.0;
	this->HatcedShp(false);
	this->DontClear(false);
}

//Destructor...//
FixedShape::~FixedShape()
{
}

//Set the shape color..
void FixedShape::setColor(double r, double g, double b)
{
	this->r = r / 255.0;
	this->g = g / 255.0;
	this->b = b / 255.0;
}

//Set the center of the window..//
void FixedShape::setCamCenter(double x, double y)
{
	this->cx = x;
	this->cy = y;
}

//get the X position
int FixedShape::getX()
{
	return _x;
}

//get the Y position..
int FixedShape::getY()
{
	return _y;
}

//current position
void FixedShape::setPosition(int x, int y)
{
	_x = x; _y = y;
}

//Set the shape Id...//
void FixedShape::setId(int _id)
{
	this->_id = _id;
}

//Returns the fixed text id...//
int FixedShape::getId()
{
	return _id;
}
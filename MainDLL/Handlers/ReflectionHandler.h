//Reflection handler: to do reflection...//
#pragma once
#include "MouseHandler.h"
class Shape;

class ReflectionHandler :
	public MouseHandler
{

private:

	bool ReflectionAlongPlane;
	double ReflectionParam[6];
	Shape *ShapeAsReflectionSurface;

public:
	//Constructor and destructor...//
	ReflectionHandler();
	~ReflectionHandler();

	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void PartProgramData();
};
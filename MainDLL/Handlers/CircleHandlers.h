//Circle Handler Class..Derived from MouseHandler..
//Used to draw new Circle shape and Add its properties.. Point action, type etc..
#pragma once
#include "MouseHandler.h"
class Circle;

class CircleHandlers:
	public MouseHandler
{
	//Variable declaration..
private:
	Circle* mycircle;
	double radius;
	bool Rflag, FirstClick, Circle_DefinedRadius;
	bool runningPartprogramValid;
	RapidEnums::CIRCLETYPE currentCircle;
	double cen[2], pnt2[2];

private:
	void AddNewCircle();

public:
	CircleHandlers(); // Default constructor..
	CircleHandlers(RapidEnums::CIRCLETYPE d); //Constructor with circle type..
	CircleHandlers(RapidEnums::CIRCLETYPE d, Circle* sh, bool flag = true);
	~CircleHandlers();
	
	//Virtual functions..//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void RmouseDown(double x, double y);
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void PartProgramData();
	virtual void SetAnyData(double *data);
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!

#pragma once
#include "MouseHandler.h"

class ShapeWithList;
class Shape;

class GroupShapeHandler :
	public MouseHandler
{
//Variable declaration...//
private:
	double angle, point2[2], point3[2];
	ShapeWithList* myshape;
	bool valid, FirstClick;
	bool runningPartprogramValid;
	void AddNewGroupShape(RapidEnums::SHAPETYPE stype);

public:
	GroupShapeHandler(Shape *sh, RapidEnums::SHAPETYPE stype);
	GroupShapeHandler(ShapeWithList* sh, bool flag);
	GroupShapeHandler(RapidEnums::SHAPETYPE stype);
	~GroupShapeHandler();

	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void RmouseDown(double x,double y);
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();

	//Virtual function to handle the partprogram data...//
	virtual void PartProgramData();
	
};
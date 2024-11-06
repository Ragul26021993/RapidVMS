//Class to draw lines.. 
// Finite line, Infinite line, Ray..
#pragma once
#include "MouseHandler.h"

class ShapeWithList;
class Shape;

class LineHandlers :
	public MouseHandler
{
//Variable declaration...//
private:
	double angle, point2[3], point3[3];
	ShapeWithList* myshape;
	bool valid, FirstClick;
	bool runningPartprogramValid;
	void AddNewLine();
public:
	LineHandlers(RapidEnums::LINETYPE d);
	LineHandlers(RapidEnums::LINETYPE d, ShapeWithList* sh, bool flag = true);
	LineHandlers();
	~LineHandlers();

	RapidEnums::LINETYPE CurrentLineType;
	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void RmouseDown(double x,double y);
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();

	//Virtual function to handle the partprogram data...//
	virtual void PartProgramData();
	void SetAngle(double ang);
	Shape* getShape();
	void UpdateRepeatShapeAction(FramegrabBase* fb);
};
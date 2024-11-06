//Circle Handler Class..Derived from MouseHandler..
//Used to draw new Circle shape and Add its properties.. Point action, type etc..
#pragma once
#include "MouseHandler.h"
class CircleInvolute;
class ShapeWithList;

class CircleInvoluteHandler:
	 public MouseHandler
{
private:
	ShapeWithList* myshape;
	double LastPt[3] , radius;
	bool Pflag, FirstClick ,runningPartprogramValid, MouseMoveFlag;	
	double cen[2] ; 
	int NumberOfPts;
	bool UndoFlag, RedoFlag;
	void init();

private :
	void AddNewCircleInvolute();
public:
	CircleInvoluteHandler();
	CircleInvoluteHandler(ShapeWithList* sh, bool undoFlag);
	~CircleInvoluteHandler();

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


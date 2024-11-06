//Class to calculate the Tangential circle to three lines.. i.e. to a triangle..
#pragma once
#include "..\Handlers\MouseHandler.h"
class Line;

class CircleInsideATraingle:
	public MouseHandler
{
//Variable declaration...//
private:
	Line *ParentShape1, *ParentShape2, *ParentShape3;
	bool Inside_Triangle;

	void init();
	void AddDerivedCircleToLines();
	void ResetShapeHighlighted();
public:
	
	CircleInsideATraingle(bool InsideTriangle);
	~CircleInsideATraingle();

	//Virtual functions..//
	virtual void LmouseDown();
	virtual void mouseMove();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void PartProgramData();
};
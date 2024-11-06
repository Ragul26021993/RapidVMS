//Rotary Measurement Handler.. Developed for Delphi tvs.. Special Measurement type..
//Developed by Sathyanarayana...
#pragma once
#include "..\Handlers\MouseHandler.h"
#include "..\Measurement\DimBase.h"

class RotaryDimensionHandler:
	public MouseHandler
{
private:
	Shape *Cshape[2]; // Pointer to Selected Shape
	int ShapeCount;
	void init();
	void AddShapeMeasureRelationShip(Shape* CShape);
	void ResetShapeHighlighted();
public:
	DimBase* dim; //Current Dimension
	RotaryDimensionHandler();
	virtual ~RotaryDimensionHandler();
	
	//Virtual function...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
};
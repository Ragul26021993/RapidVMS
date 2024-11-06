//To Measure the PCD Offset and Distance...
//Developed by Sathyanarayana...
#pragma once
#include "..\Handlers\MouseHandler.h"
#include "..\Measurement\DimBase.h"
#include "..\Shapes\PCDCircle.h"

class PCDMeasureHandler:
	public MouseHandler
{
//Variables used for measurements..///
private:
	Shape* CurrentPCdCircle;
	Shape *Cshape[2];
	int ShapeCount;
	
	void init();
	void AddShapeMeasureRelationShip(Shape* CShape);
	bool CheckShapeSelected(Shape* CShape);
	void ResetShapeHighlighted();
public:
	DimBase* dim; //Current Dimension...
	PCDMeasureHandler();
	virtual ~PCDMeasureHandler();
	
	//Virtual function...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
};
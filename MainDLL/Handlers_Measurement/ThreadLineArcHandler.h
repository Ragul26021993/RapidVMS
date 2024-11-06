//Handler to handle the Thread Profile scan and Line Arc seperation//
//According to the user's select decide the start and end of the shape collection and shapes to the main collection
//
#pragma once
#include "..\Handlers\MouseHandler.h"
#include "..\Engine\PointCollection.h"
class Line;
class Shape;
class Circle;

//Derived from the mousehandler..//
class ThreadLineArcHandler:
	public MouseHandler
{
//Variable declaration...//
private:
	bool ShapeHighilghted, ValidFlag;
	bool line_arc_line_thrd;
	Shape *HighlightedShape;
	map<int, Shape*>* ptr_TempShapeCollection;
	map<int, Shape*> NotNeededShapes;
	map<int, Shape*> UpperThrdShapes;
	map<int, Shape*> LowerThrdShapes;
	map<int, Line*> upperlines, lowerlines;
	map<int, Circle*> upperarcs, lowerarcs;
	map<int, Shape*> FinalThrdShapes;
	int ThreadCount;
	PointCollection IntPt1_upper, IntPt2_upper, IntPt1_lower, IntPt2_lower;
	int z;
	Shape* SelectedShapes[2];
	void init();
	Shape* getNearestShape(map<int, Shape*> Shapecollection, double x, double y, double snapdist);
	void CreateThreadShapes(map<int, Shape*> Shapecollection);
	bool CreateThrdMeasurements();
	void getaverage(map<int, double>& list, double* ptr_average);
	void ChkaddShape(Shape*, bool);
	bool MaxMouseActionSuccess;
	void getLine2LineAngle(Shape* csh1, Shape* csh2, double* angle);

public:
	//Constructor...//
	ThreadLineArcHandler();
	~ThreadLineArcHandler();

	//Virtual functions..//
	virtual void mouseMove();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void EscapebuttonPress();
	virtual void SetAnyData(double *data);
};
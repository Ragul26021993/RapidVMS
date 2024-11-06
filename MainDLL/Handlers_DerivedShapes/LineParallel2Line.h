//Class to handle the derived lines...//
//Contains all types of derived lines..//
#pragma once
#include "..\Handlers\MouseHandler.h"
class Line;

class LineParallel2Line:
	public MouseHandler
{
private:	
	Line* ParentShape;// parent Shape..
	double LineLength, angle, intercept, offset, point1[2], point2[2];
	bool DefinedOffset, runningPartprogramValid, LengthDefined, MultiPointParellelLine;
	int linepos;

	void init();
	void ResetShapeHighlighted();
public:
	//Constructor..//
	LineParallel2Line();
	LineParallel2Line(bool MultiPointsLine);
	~LineParallel2Line();
	
	
	void SetShapeandClickdone(Shape* Csh, double len);
	
	//Virtual functions..
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void MouseScroll(bool flag);
	virtual void PartProgramData();
	virtual void SetAnyData(double *data);
	virtual void RmouseDown(double x, double y);
};
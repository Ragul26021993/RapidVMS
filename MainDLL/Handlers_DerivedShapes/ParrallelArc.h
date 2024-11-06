//Handler for parallel arc...//

#pragma once
#include "..\Handlers\MouseHandler.h"
class Circle;

class ParallelArc :
	public MouseHandler
{
private:
	Circle* ParentShape;
	double center[3], radius, startang, sweepang, circle_radius, offset;
	bool DefinedOffset, runningPartprogramValid;
	void init();
	void ResetShapeHighlighted();
public:
	ParallelArc();
	~ParallelArc();

	bool getDimension(Shape *s);
	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	//Virtual funtion to handle partprogram...//
	virtual void SetAnyData(double *data);
	virtual void PartProgramData();
	virtual void EscapebuttonPress();
};
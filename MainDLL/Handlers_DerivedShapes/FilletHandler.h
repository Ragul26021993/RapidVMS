//Handler for parallel arc...//

#pragma once
#include "..\Handlers\MouseHandler.h"

class Shape;

class FilletHandler:
	public MouseHandler
{
private:
	int CurrentType;
	Shape *ParentShape1, *ParentShape2;
	Shape *FilletShape;
	double radius, minimumdia;
	double Acenter1[2], Acenter2[2], Aangle1[2], Aangle2[2], Aradius1, Aradius2, APoints11[2], APoints12[2], APoints21[2], APoints22[2];
	double Langle1, Langle2, Lintercept1, Lintercept2, LPoints11[2], LPoints12[2], LPoints21[2], LPoints22[2];
	bool runningPartprogramValid;

	void init();
	void CalculateFilletForTwoLine();
	void CalculateFilletForTwoArc();
	void CalculateFilletForOneLineOneArc();
	void getLineDimensions(Shape* Csh, int cnt);
	void getArcDimensions(Shape* Csh, int cnt);
	void ResetShapeHighlighted();
public:
	FilletHandler();
	~FilletHandler();

	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void PartProgramData();
	virtual void EscapebuttonPress();
	virtual void SetAnyData(double *data);
};
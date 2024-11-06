//Digital Micrometer Drawing... in PP Also...
//Developed by Sathyanarayana...
//F star star K.., lot of conditions!! according to the selected shape and first click..
#pragma once
#include "..\Handlers\MouseHandler.h"
#include "..\Measurement\DimBase.h"

class DMMHandler:
	public MouseHandler
{

private:
	Shape *Cshape[3]; // Pointer to Selected/related Shape.
	Vector ClickValue[4];
	int PointCount, DmMeasureType, DmMeasureTypeLinear, NoofDec;
	bool MeasureDir, HorizontalMeasure, DrawNearMousePoint, FloatFormouseMove, runningPartprogramValid; //Used for direction,pp, rightclick etc.
	bool CheckIntesectFlag;
	char cd[40];
	double point1[2], point2[2], point3[2], DMMvalue;
	FramegrabBase* FirstAction_DMM;

	void init();
	void CalculateDigitalMicrometer(double x, double y, double z);
	//bool CalculateDistanceDuringPP(Shape* s1, Shape* s2, double* point1, double* point3);
public:
	DimBase* dim; //Current Dimension...
	DMMHandler();
	virtual ~DMMHandler();
	
	//Virtual function...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void RmouseDown(double x, double y);
	virtual void draw(int windowno, double WPixelWidth);
	virtual void keyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void EscapebuttonPress();
	virtual void PartProgramData();
};
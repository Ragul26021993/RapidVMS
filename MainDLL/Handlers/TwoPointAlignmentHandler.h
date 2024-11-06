//Cad 2 point Handler.. derived From cad Align Handler...
//Used to get the point for CAD alignment
//Calculates the transformation matrix and set..
#pragma once
#include "MouseHandler.h"

class TwoPointAlignmentHandler:
	public MouseHandler
{
	//Variable declaration...//
private:
	Vector points[4];
	Vector *RotationPoint;
	double TranslateParam[6], StartPos[3], Angle;
	
public:
	TwoPointAlignmentHandler(); // Constructor..
	~TwoPointAlignmentHandler(); // Destructor..Release memory..
		//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void PartProgramData();
	virtual void SetAnyData(double *data);
	bool CalculateMatrixesForRotation(double *currentShapeCenter, double *rotateMatrix, double *targetPos = NULL, bool targetGiven = false);

};


//RotateShapeHandler: to do Rotation...//
#pragma once
#include "MouseHandler.h"
class Shape;

class RotateShapeHandler :
	public MouseHandler
{

private:

	bool AngleDefined;
	double OriginalShapesCenter[3], RotationOrigin[3], RotationPlane[4];
	bool GetSelectedShapeCenter(double *currentCenter);
	bool CalculateMatrixesForRotation(double *currentShapeCenter, double * newMatrix);
	void Init();
public:
	double Angle;
	Shape *ShapeAsRotationAxis;
	Vector *RotationPoint;
	//Constructor and destructor...//
	RotateShapeHandler();
	~RotateShapeHandler();

	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void PartProgramData();
	virtual void SetAnyData(double *data);
};
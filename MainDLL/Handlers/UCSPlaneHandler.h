//Class to handle the new UCS.. with respect to the selected line and point...//
#pragma once
#include "MouseHandler.h"
class Shape;
class Vector;

class UCSPlaneHandle:
	public MouseHandler
{
//Variable declaration...//
private:
	Shape* Cshape[3];
	bool valid;
	double TransformMatrix[16], PCenter[3];
	Vector* Cpoint[3];
	list<list<double>> ShapesParam;
	int PointSelctNum, ShapSelctNum;
	int SelctNum[3];

public:
	UCSPlaneHandle();
	~UCSPlaneHandle();

	void AddShape(map<int, Shape*> ShapesForBasePlane);

	//Virtual functions....../
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	/*bool UCSCalculation(list<double*> ShapesParam, double* TransformMatrix, double* PCenter );*/
};
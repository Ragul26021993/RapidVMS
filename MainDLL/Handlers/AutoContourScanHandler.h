//Reflection handler: to do reflection...//
#pragma once
#include "MouseHandler.h"
#include "..\Engine\RCadApp.h"

class AutoContourLine;

class AutoContourScanHandler :
	public MouseHandler
{
	             
private:
    ShapeWithList* currentshape;
	Vector *PointOfRotation;
    double AvgZ, DirZ, angleofrotation, LineParam[6];
    int FRow, FCol, FWidth, Fgap, FHeight, dirfactor, numberOfRotation;
	void CalculateNextGotoPosition(bool samePlace = false);
	map<int, double> pointCollection;
	bool scanning, rotateOnMouseMove, conti;
	list<AutoContourLine*> AutoContourLineCollection;
	list<AutoContourLine*>::iterator AutoContourLineIter;
	bool GetSelectedShapeCenter(double *currentCenter);
	bool CalculateMatrixesForRotation(double *currentShapeCenter, double *rotateMatrix);
public:
	bool failed;
	//Constructor and destructor...//
	AutoContourScanHandler();
	~AutoContourScanHandler();

	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void RmouseDown(double x, double y);

	bool StartAutoFocus();
	void ContinueAutoContourScan();
	void RotateAutoContourScanPoints(double rotationAngle, int step);
    void UpdateAutoContourScanPoints(double* Zvalues);
	void ClearAll();
};
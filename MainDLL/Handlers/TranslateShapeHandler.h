//TranslateShapeHandler: to do Translatation...//
#pragma once
#include "MouseHandler.h"

class TranslateShapeHandler :
	public MouseHandler
{

private:
	double translationDirection[3], OriginalCenter[3], TranslateParam[6];
	void GetShapesCenter(double *shapesCenter);
	bool translationDefined, PointToPoint;
public:
	//Constructor and destructor...//
	TranslateShapeHandler(bool pointtopoint);
	~TranslateShapeHandler();

	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void PartProgramData();
	virtual void SetAnyData(double *data);
};
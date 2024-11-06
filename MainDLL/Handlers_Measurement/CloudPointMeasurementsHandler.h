#pragma once
#include "..\Handlers\MouseHandler.h"

class CloudPointMeasurementsHandler:
	public MouseHandler
{
private:
	Vector PointsDRO[2];
	void Getpostion(double *points, int cnt, double *tmpPos, double *pos);
public:
	void AddMeasurement(list<string>);
	CloudPointMeasurementsHandler();
	virtual ~CloudPointMeasurementsHandler();
	
	//Virtual function...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
};
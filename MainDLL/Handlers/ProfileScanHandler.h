#pragma once
#include "MouseHandler.h"
#include "..\Shapes\Vector.h"

class MAINDLL_API ProfileScanHandler :
	public MouseHandler
{
	Vector point1, point2;
public:
	//Constructor and destructor...//
	ProfileScanHandler();
	~ProfileScanHandler();

	void SetProfileScanParam();
	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void PartProgramData();
};
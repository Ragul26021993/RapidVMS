//Handler for parallel arc...//

#pragma once
#include "MouseHandler.h"
#include "..\Shapes\Vector.h"

class Shape;

class TrimmingHandler :
	public MouseHandler
{
	Shape *s1, *s2;
	Vector cen, endp1, endp2;
	RapidEnums::SHAPETYPE CurrentShapeType;
	RapidEnums::CIRCLETYPE CurrentCircleType;
	RapidEnums::LINETYPE CurrentLineType;
	Vector firstpoint, secondpt;
	double startang,rangle, intrcn[2], sweepang,radius, angle, intercept;
	bool runningPP;
public:
	TrimmingHandler();
	~TrimmingHandler();

	bool getDimension(Shape *s);
	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	//Virtual funtion to handle partprogram...//
	virtual void PartProgramData();
};
// modified  by Rahul Singh Bhadauria on 14 Jun 12........
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
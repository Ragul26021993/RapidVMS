//Class for fixed line..//
#pragma once
#include "FixedShape.h"

class FixedLine:
	public FixedShape
{
	//Variable declaration..
private:
	double _offSetX,_offSetY, Offset;
	double _angle,Ltype;
public:
	//Constructor and destructor..
	FixedLine();
	~FixedLine();
	//Sets the line parameters..//
	virtual void setToolParameter(double _first,double _second,double _third,void* anyThing);
	//Draws the Line parameters...//
	virtual void drawFixedGraphics(int pixelgp = 2);
	virtual void setShapeWidth(bool increase);
	//get line parameters..
	double Angle();
	double getOffset();
	int GetLineType();
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!          
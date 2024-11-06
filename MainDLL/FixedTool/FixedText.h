//Class for fixed text..
#include "FixedShape.h"
class FixedText:
	public FixedShape
{
//Variable declarations...//
private:
	double _sx, _sy, angle;
	char* _fixedText;
	RGraphicsFont FTextFont;
	bool BuildFontlist;
public:
	//Constructor and destructor...
	FixedText();
	~FixedText();
	//Sets the fixed txt parameters...
	virtual void setToolParameter(double _first,double _second,double _third,void* anyThing);
	virtual void drawFixedGraphics(int pixelgp = 2);
	virtual void setShapeWidth(bool increase);
	//Current Text properties....!!
	char* getText();
	double getPointX();
	double getPointY();
	double getAngle();
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
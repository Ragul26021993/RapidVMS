#include "stdafx.h"
#include "FixedCircle.h"
#include "..\Engine\RCadApp.h"
//Constructor..
FixedCircle::FixedCircle()
{
	try
	{
		this->_cx = 0; this->_cy = 0; this->_rad = 0;
		this->DiameterType = 0;
		this->FShapeType = RapidEnums::FIXEDSHAPETYPE::FCIRCLE;
		this->_x = 400; this->_y = 300;
		this->TextSize(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCIRC0001", __FILE__, __FUNCSIG__); }
}

//Destructor.//
FixedCircle::~FixedCircle()
{

}

//Set the Circle parameters..//
void FixedCircle::setToolParameter(double _first, double _second, double _third, void* anyThing)
{
	try
	{
		this->_cx = _first; this->_cy = _second;
		this->DiameterType = (bool)_third;
		this->_rad = *((double*)anyThing);
		cx = MAINDllOBJECT->getWindow(0).getCenter().x;
		cy = MAINDllOBJECT->getWindow(0).getCenter().y;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCIRC0002", __FILE__, __FUNCSIG__); }
}

//Draw the fixed circle....//
void FixedCircle::drawFixedGraphics(int pixelgp)
{
	try
	{
		GRAFIX->SetGraphicsLineWidth(TextSize());
		GRAFIX->SetColor_Double(r, g, b);
		double radius = _rad;
		if(DiameterType) radius = radius/2;
		if(HatcedShp())
			GRAFIX->drawCircleStipple(_cx + cx, _cy + cy, radius, pixelgp);
		else
			GRAFIX->drawCircle(_cx + cx, _cy + cy, radius);
		GRAFIX->drawPoint(_cx + cx, _cy + cy, 0);
		GRAFIX->SetGraphicsLineWidth(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCIRC0003", __FILE__, __FUNCSIG__); }
}

void FixedCircle::setShapeWidth(bool increase)
{
	try
	{
		double Fsize = this->TextSize();
		if(increase)
		{
			if(Fsize < 5) Fsize += 0.5;
		}
		else
		{
			if(Fsize > 1) Fsize -= 0.5;
		}
		this->TextSize(Fsize);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDCIRC0004", __FILE__, __FUNCSIG__); }
}

//Returns the circle radius...//
double FixedCircle::Radius()
{
	return _rad;
}

//returns circle center X
double FixedCircle::getCenterX()
{
	return _cx;
}

//Returns circle center X
double FixedCircle::getCenterY()
{
	return _cy;
}
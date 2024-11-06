#include "stdafx.h"
#include "FixedLine.h"
#include "..\Engine\RCadApp.h"

//Constructor..//
FixedLine::FixedLine()
{
	try
	{
		_offSetX = 0;_offSetY = 0; Offset = 0;
		_angle = 0;Ltype = 0;
		this->FShapeType = RapidEnums::FIXEDSHAPETYPE::FLINE;
		this->_x = 400; this->_y = 300;
		this->TextSize(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDLINE0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
FixedLine::~FixedLine()
{
}

//Set the fixed line parameter.../
void FixedLine::setToolParameter(double _first,double _second,double _third,void* anyThing)
{
	try
	{
		Offset = _first;
		if(_second == 0) 
		{
			_offSetX = _first;
			_offSetY = 0;
		}
		else if(_second == 1) 
		{
			_offSetY = _first;
			_offSetX = 0;
		}
		else
		{
			_offSetX = _first;
			_offSetY = _second;
		}
		_angle = _third;
		Ltype = *((double*)anyThing);
		cx = MAINDllOBJECT->getWindow(0).getCenter().x;
		cy = MAINDllOBJECT->getWindow(0).getCenter().y;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDLINE0002", __FILE__, __FUNCSIG__); }
}

//Draw the fixed line..//
void FixedLine::drawFixedGraphics(int pixelgp)
{
	try
	{
		GRAFIX->SetColor_Double(r, g, b);
		GRAFIX->SetGraphicsLineWidth(TextSize());
		if(HatcedShp())
		{
			if(Ltype == 0)
				GRAFIX->drawXLineOrXRayStipple(_angle, _offSetX + cx, _offSetY + cy, MAINDllOBJECT->getWindow(0).getUppX(), false, pixelgp);
			else
				GRAFIX->drawXLineOrXRayStipple(_angle, _offSetX + cx, _offSetY + cy, MAINDllOBJECT->getWindow(0).getUppX(), true, pixelgp);
		}
		else
		{
			if(Ltype == 0)
				GRAFIX->drawXLineOrXRay(_angle, _offSetX + cx, _offSetY + cy, MAINDllOBJECT->getWindow(0).getUppX(), false);
			else
				GRAFIX->drawXLineOrXRay(_angle, _offSetX + cx, _offSetY + cy, MAINDllOBJECT->getWindow(0).getUppX(), true);
		}
		GRAFIX->SetGraphicsLineWidth(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDLINE0003", __FILE__, __FUNCSIG__); }
}

void FixedLine::setShapeWidth(bool increase)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDLINE0004", __FILE__, __FUNCSIG__); }
}

//get line angle..
double FixedLine::Angle()
{
	return _angle;
}

//get line offset..
double FixedLine::getOffset()
{
	return Offset;
}

//get line type...
int FixedLine::GetLineType()
{
	return Ltype;
}
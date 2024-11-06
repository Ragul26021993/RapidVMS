#include "stdafx.h"
#include "FixedText.h"
#include "..\Engine\RCadApp.h"

//Fixed text constructor///
FixedText::FixedText()
{
	try
	{
		this->_sx = 0; this->_sy = 0;
		this->angle = 0;
		this->_fixedText  = "";
		this->FShapeType = RapidEnums::FIXEDSHAPETYPE::FTEXT;
		this->_x = 400; this->_y = 300;
		this->TextSize(20);
		this->BuildFontlist = true;
		//MAINDllOBJECT->InitaliseDefaultFontList(&FTextFont);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDTEXT0001", __FILE__, __FUNCSIG__); }
}

//Destructor..//
FixedText::~FixedText()
{

}

//Set the fixed text parameters...//
void FixedText::setToolParameter(double _first, double _second, double _third, void* anyThing)
{
	try
	{
		this->_sx = _first;
		this->_sy = _second;
		this->angle = _third;
		cx = MAINDllOBJECT->getWindow(0).getCenter().x;
		cy = MAINDllOBJECT->getWindow(0).getCenter().y;
		if(anyThing != NULL)
			this->_fixedText = ((char*)anyThing);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDTEXT0002", __FILE__, __FUNCSIG__); }
}

//Draw the fixed text...//
void FixedText::drawFixedGraphics(int pixelgp)
{
	try
	{
		/*if(BuildFontlist)
		{
			BuildFontlist = false;
			MAINDllOBJECT->InitaliseDefaultFontList(&FTextFont);
		}
		
		FTextFont.drawcurrent_Rfont(_sx + cx,_sy + cy, 0, 1, 1, 1, _fixedText);*/
		GRAFIX->SetColor_Double(r, g, b);
		GRAFIX->drawString(_sx + cx,_sy + cy, 0, angle, _fixedText, MAINDllOBJECT->getWindow(0).getUppX() * TextSize());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDTEXT0003", __FILE__, __FUNCSIG__); }
}

void FixedText::setShapeWidth(bool increase)
{
	try
	{
		double Fsize = this->TextSize();
		if(increase)
		{
			if(Fsize < 60) Fsize += 1;
		}
		else
		{
			if(Fsize > 10) Fsize -= 1;
		}
		this->TextSize(Fsize);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FIXEDTEXT0004", __FILE__, __FUNCSIG__); }
}

//Returns the fixed text...//
char* FixedText::getText()
{
	return _fixedText;
}

//returns the X position
double FixedText::getPointX()
{
	return _sx;
}

//returns the Y position
double FixedText::getPointY()
{
	return _sy;
}

//returns the angle..
double FixedText::getAngle()
{
	return angle;
}
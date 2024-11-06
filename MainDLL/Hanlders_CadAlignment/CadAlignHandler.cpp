#include "stdafx.h"
#include "CadAlignHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\DXF\DXFExpose.h"

//Constructor...
CadAlignHandler::CadAlignHandler()
{
	try
	{
		MAINDllOBJECT->CURRENTRAPIDHANDLER = RapidEnums::RAPIDDRAWHANDLERTYPE::CADALIGNMENT_HANDLER;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CADALGNMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
CadAlignHandler::~CadAlignHandler()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CADALGNMH0002", __FILE__, __FUNCSIG__); }
}

//Left mousedown.. select the shape(line / point) and calculates the transforamtion matrix..//
void CadAlignHandler::LmouseDown()
{
	try{ SelectPoints_Shapes(); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CADALGNMH0003", __FILE__, __FUNCSIG__); }
}

//Handle mouse move..
void CadAlignHandler::mouseMove()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone());
		double x = pt->getX(), y = pt->getY();
		Align_mouseMove(x, y); //if (FloatwithMouse)
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CADALGNMH0004", __FILE__, __FUNCSIG__); }
}

//Hanlde draw
void CadAlignHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		GRAFIX->translateMatrix(0.0, 0.0, 2.0);
		if(windowno == 0)
			drawSelectedShapesOnvideo();
		else if(windowno == 1)
			drawSelectedShapeOnRCad();
		else if(windowno == 2)
			drawSelectedShapesOndxf();
		GRAFIX->translateMatrix(0.0, 0.0, -2.0);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CADALGNMH0005", __FILE__, __FUNCSIG__); }
}

//Handle escape button press.. Clear the current selection..
void CadAlignHandler::EscapebuttonPress()
{
	try{ Align_Finished(); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CADALGNMH0006", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data...//
void CadAlignHandler::PartProgramData()
{
	try
	{
		this->HandlePartProgramData();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CADALGNMH0007", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data...//
void CadAlignHandler::drawSelectedShapeOnRCad()
{
	try
	{
		return;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CADALGNMH0008", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data...//
void CadAlignHandler::HandlePartProgramData()
{
	try
	{
		return;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CADALGNMH0009", __FILE__, __FUNCSIG__); }
}
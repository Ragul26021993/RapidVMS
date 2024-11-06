#include "stdafx.h"
#include "PCDHandler.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"

//Constructor..//
PCDHandler::PCDHandler(bool AddPcdCircle)
{
	try
	{
		setMaxClicks(2);
		if(AddPcdCircle)
			MAINDllOBJECT->AddNewPcdCircle();
		/*MAINDllOBJECT->SetStatusCode("PCD01");*/
		MAINDllOBJECT->SetStatusCode("PCDHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor..
PCDHandler::~PCDHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0002", __FILE__, __FUNCSIG__); }
}


//Handle the mosuemove..
void PCDHandler::mouseMove()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse down... Select the shapes..
void PCDHandler::LmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone() - 1);
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone( getClicksDone() - 1);
			return;
		}
		Shape* Sh = MAINDllOBJECT->highlightedShape();
		if(Sh->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || Sh->ShapeType == RapidEnums::SHAPETYPE::ARC || Sh->ShapeType == RapidEnums::SHAPETYPE::RPOINT)
		{
			if(Sh->ShapeType == RapidEnums::SHAPETYPE::CIRCLE)
			{
				if(((Circle*)Sh)->CircleType != RapidEnums::CIRCLETYPE::PCDCIRCLE)
					MAINDllOBJECT->AddCircleToCurrentPCD(Sh->getId());
			}
			else
				MAINDllOBJECT->AddCircleToCurrentPCD(Sh->getId());
		}
		setClicksDone(getClicksDone() - 1);
		MAINDllOBJECT->DerivedShapeCallback();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0005", __FILE__, __FUNCSIG__); }
}


//Handle the escape button press..
void PCDHandler::EscapebuttonPress()
{
	try{}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0006", __FILE__, __FUNCSIG__); }
}

//Handle the right click...
void PCDHandler::RmouseDown(double x, double y)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0007", __FILE__, __FUNCSIG__); }
}


//Draw function
void PCDHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0008", __FILE__, __FUNCSIG__); }
}
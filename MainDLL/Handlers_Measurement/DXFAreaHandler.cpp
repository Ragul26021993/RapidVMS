#include "StdAfx.h"
#include "DXFAreaHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Line.h"
#include "..\DXF\DXFExpose.h"
#include "..\DXF\DXFCollection.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"

DXFAreaHandler::DXFAreaHandler()
{
	try
	{
		init();
		setMaxClicks(3);
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DAMH0001", __FILE__, __FUNCSIG__); }
}

DXFAreaHandler::~DXFAreaHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DAMH0002", __FILE__, __FUNCSIG__); }
}

void DXFAreaHandler::init()
{
	try
	{
		Cshape = NULL;
		resetClicks();
		/*MAINDllOBJECT->SetStatusCode("DXFAreaHandler01");*/
		MAINDllOBJECT->SetStatusCode("DXFAreaHandler03");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DAMH0003", __FILE__, __FUNCSIG__); }
}

void DXFAreaHandler::LmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone() - 1);
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			init();
			return;
		}
		Cshape = MAINDllOBJECT->highlightedShape();
		bool ShapePresent = false;
		list<Shape*> CloseDxfLoop;
		for(RC_ITER Citem = MAINDllOBJECT->getDXFList().getList().begin(); Citem != MAINDllOBJECT->getDXFList().getList().end(); Citem++)
		{
			DXFCollection* CurrentDXF = (DXFCollection*)(*Citem).second;
			for each(list<Shape*> CloseLoop in CurrentDXF->ClosedLoopCollections)
			{
				for each(Shape* Cdshape in CloseLoop)
				{
					if(Cdshape->getId() == Cshape->getId())
					{
						ShapePresent = true;
						CloseDxfLoop = CloseLoop;
						break;
					}
				}
			}
		}
		if(ShapePresent)
		{
			if(Cshape->ShapeType == RapidEnums::SHAPETYPE::LINE || Cshape->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				Shape* NewPerimeter = new Perimeter();
				for each(Shape* Cshape in CloseDxfLoop)
					((Perimeter*)NewPerimeter)->AddShape(Cshape->CreateDummyCopy());
				AddShapeAction::addShape(NewPerimeter);
				Shape* Csh = (*CloseDxfLoop.begin());
				Vector Temp;
				if(Csh->ShapeType == RapidEnums::SHAPETYPE::LINE)
					Temp.set(((Line*)Csh)->getPoint1()->getX(), ((Line*)Csh)->getPoint1()->getY());
				else
					Temp.set(((Circle*)Csh)->getendpoint1()->getX(), ((Circle*)Csh)->getendpoint1()->getY());
				setBaseRProperty(Temp.getX(), Temp.getY(), Temp.getZ());
				//((Perimeter*)NewPerimeter)->StartingPoint.set(Temp);
				AddPointAction::pointAction((ShapeWithList*)NewPerimeter, baseaction);
				//NewPerimeter->UpdateBestFit();
			}
			else
			{
				init();
				MAINDllOBJECT->SetStatusCode("DXFAreaHandler02");
			}
		}
		else
		{
			init();
			MAINDllOBJECT->SetStatusCode("DXFAreaHandler02");
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DAMH0004", __FILE__, __FUNCSIG__); }
}

void DXFAreaHandler::mouseMove()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DAMH0005", __FILE__, __FUNCSIG__); }
}

void DXFAreaHandler::LmaxmouseDown()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DAMH0006", __FILE__, __FUNCSIG__); }
}

void DXFAreaHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DAMH0007", __FILE__, __FUNCSIG__); }
}

void DXFAreaHandler::EscapebuttonPress()
{
	try{init(); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DAMH0008", __FILE__, __FUNCSIG__); }
}
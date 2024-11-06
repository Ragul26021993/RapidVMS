#include "stdafx.h"
#include "PP1PtHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\DXF\DXFExpose.h"

//Constructor...
PP1PtHandler::PP1PtHandler()
{
	try
	{
		this->AlignmentModeFlag = true;
		VideoFlag = false; RcadFlag = false;
		setMaxClicks(3);
		MAINDllOBJECT->SetStatusCode("DXFALGN0001");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP1PTALIGN01", __FILE__, __FUNCSIG__); }
}

//Destructor...//
PP1PtHandler::~PP1PtHandler()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP1PTALIGN02", __FILE__, __FUNCSIG__); }
}

//Left mousedown.. select point and calculates the transforamtion matrix..//
void PP1PtHandler::SelectPoints_Shapes()
{
	try
	{
		if(MAINDllOBJECT->getCurrentWindow() == 1 && getClicksDone() == 1)
		{
			if(MAINDllOBJECT->highlightedPoint() == NULL)
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
			points[0].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
			RcadFlag = true;
			MAINDllOBJECT->SetStatusCode("DXFALGN0005");
		}
		else if(MAINDllOBJECT->getCurrentWindow() == 0 && getClicksDone() == 2)
		{
			if(MAINDllOBJECT->highlightedPoint() != NULL)
				points[1].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
			else
				points[1].set(getClicksValue(1).getX(), getClicksValue(1).getY());
			VideoFlag = true;

			/*double pt1[2] = {points[0].getX(), points[0].getY()}, pt2[2] = {points[1].getX(), points[1].getY()}, transform[9] = {0};
			RMATH2DOBJECT->TransformM_OnePtAlign(&pt1[0], &pt2[0], &transform[0]);
			DXFEXPOSEOBJECT->TransformMatrix(&transform[0]);*/
			MAINDllOBJECT->SetStatusCode("DXFALGN0015");
			resetClicks();
			DXFEXPOSEOBJECT->AlginTheDXF(true);
			return;			
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			MAINDllOBJECT->SetStatusCode("DXFALGN0000");
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP1PTALIGN03", __FILE__, __FUNCSIG__); }
}

//Handle escape button press.. Clear the current selection..
void PP1PtHandler::Align_Finished()
{
	try
	{
		VideoFlag = false; RcadFlag = false;
		resetClicks();
		MAINDllOBJECT->SetStatusCode("DXFALGN0001");
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP1PTALIGN04", __FILE__, __FUNCSIG__); }
}

//draw selected point on dxf window..
void PP1PtHandler::drawSelectedShapesOndxf()
{
}

void PP1PtHandler::drawSelectedShapeOnRCad()
{
	try
	{
		GRAFIX->SetColor_Double(0, 1, 0);
		if(RcadFlag)
			GRAFIX->drawPoint(points[0].getX(), points[0].getY(), 0, 8);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP1PTALIGN05", __FILE__, __FUNCSIG__); }
}

//draw selected point on video..
void PP1PtHandler::drawSelectedShapesOnvideo()
{
	try
	{
		GRAFIX->SetColor_Double(0, 1, 0);
		if(VideoFlag)
			GRAFIX->drawPoint(points[1].getX(), points[1].getY(), 0, 8);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP1PTALIGN06", __FILE__, __FUNCSIG__); }
}

 void PP1PtHandler::Align_mouseMove(double x, double y)
{
	 try
	 {
		 if(MAINDllOBJECT->getCurrentWindow() == 0 && getClicksDone() == 1)
		{ 
			points[1].set(x, y);
			double pt1[2] = {points[0].getX(), points[0].getY()}, pt2[2] = {points[1].getX(), points[1].getY()}, transform[9] = {0};
			RMATH2DOBJECT->TransformM_OnePtAlign(&pt1[0], &pt2[0], &transform[0]);
			points[0].set(x, y);
			DXFEXPOSEOBJECT->SetTransVector(pt1, pt2);
		}
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP1PTALIGN07", __FILE__, __FUNCSIG__); }
}
#include "stdafx.h"
#include "Program1PtHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"
//Constructor...
Program1PtHandler::Program1PtHandler()
{
	try
	{
		this->AlignmentModeFlag = true;
		this->PtsCount = 0;
		setMaxClicks(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM1PTALIGN01", __FILE__, __FUNCSIG__); }
}

//Destructor...//
Program1PtHandler::~Program1PtHandler()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM1PTALIGN02", __FILE__, __FUNCSIG__); }
}

//Left mousedown.. select point and calculates the transforamtion matrix..//
void Program1PtHandler::SelectPoints_Shapes()
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
			points[PtsCount++].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
		}
		else if((MAINDllOBJECT->getCurrentWindow() == 0 || MAINDllOBJECT->getCurrentWindow() == 1) && getClicksDone() == 2)
		{
			if(MAINDllOBJECT->highlightedPoint() != NULL)
				points[PtsCount++].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
			else
				points[PtsCount++].set(getClicksValue(1).getX(), getClicksValue(1).getY());	
			if(MAINDllOBJECT->highlightedShape() != NULL)
				MAINDllOBJECT->highlightedShape()->Normalshape(false);
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			MAINDllOBJECT->SetStatusCode("DXFALGN0000");
			return;
		}
		if(PtsCount == 2)
		{
			Vector Difference;
			Difference.set(points[1].getX() - points[0].getX(), points[1].getY() - points[0].getY(), points[1].getZ() - points[0].getZ());
			PPCALCOBJECT->TranslateProgramShapeMeasureAction(Difference);
			Align_Finished();
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM1PTALIGN03", __FILE__, __FUNCSIG__); }
}

//Handle escape button press.. Clear the current selection..
void Program1PtHandler::Align_Finished()
{
	try
	{
		PPCALCOBJECT->UpdateFistFramegrab(true);
		this->PtsCount = 0;
		resetClicks();
		MAINDllOBJECT->SetStatusCode("DXFALGN0001");
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM1PTALIGN04", __FILE__, __FUNCSIG__); }
}

//draw selected point on dxf window..
void Program1PtHandler::drawSelectedShapesOndxf()
{
}

void Program1PtHandler::drawSelectedShapeOnRCad()
{
	try
	{
		GRAFIX->SetColor_Double(0, 1, 0);
		if(PtsCount > 0)
			GRAFIX->drawPoint(points[0].getX(), points[0].getY(), 0, 8);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM1PTALIGN05", __FILE__, __FUNCSIG__); }
}

//draw selected point on video..
void Program1PtHandler::drawSelectedShapesOnvideo()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM1PTALIGN06", __FILE__, __FUNCSIG__); }
}

 void Program1PtHandler::Align_mouseMove(double x, double y)
{
	 try
	 {
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM1PTALIGN07", __FILE__, __FUNCSIG__); }
}
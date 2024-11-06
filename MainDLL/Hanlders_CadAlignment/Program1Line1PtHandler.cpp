#include "stdafx.h"
#include "Program1Line1PtHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Shapes\Line.h"
//Constructor...
Program1Line1PtHandler::Program1Line1PtHandler()
{
	try
	{
		this->AlignmentModeFlag = true;
		DxfShape =  NULL; VideoShape = NULL;
		setMaxClicks(5);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM1LN1PTALIGN01", __FILE__, __FUNCSIG__); }
}

//Destructor...//
Program1Line1PtHandler::~Program1Line1PtHandler()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM1LN1PTALIGN02", __FILE__, __FUNCSIG__); }
}

//Left mousedown.. select point and calculates the transforamtion matrix..//
void Program1Line1PtHandler::SelectPoints_Shapes()
{
	try
	{
		if(MAINDllOBJECT->getCurrentWindow() != 2 && getClicksDone() < 3)
		{
			if(MAINDllOBJECT->highlightedPoint() == NULL && MAINDllOBJECT->highlightedShape() == NULL)
			{
				setClicksDone(getClicksDone() - 1);
				MAINDllOBJECT->SetStatusCode("DXFALGN0000");
				return;
			}
			if(getClicksDone() == 2 && MAINDllOBJECT->highlightedPoint() != NULL)
			{
				Dxfpoint.set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
				MAINDllOBJECT->SetStatusCode("DXFALGN0010");
			}
			else if(getClicksDone() == 1 && CheckHighlightedShape())
			{
				DxfShape = MAINDllOBJECT->highlightedShape();
				DxfShape->HighlightedForMeasurement(true);
				MAINDllOBJECT->DXFShape_Updated();
			}
			else
			{
				setClicksDone(getClicksDone() - 1);
				MAINDllOBJECT->SetStatusCode("DXFALGN0000");
				return;
			}
		}
		else if(getClicksDone() == 3 && MAINDllOBJECT->getCurrentWindow() != 2)
		{
			if(CheckHighlightedShape())
			{
				VideoShape = MAINDllOBJECT->highlightedShape();
				VideoShape->HighlightedForMeasurement(true);
				MAINDllOBJECT->Shape_Updated();
			}
			else
			{
				setClicksDone(getClicksDone() - 1);
				MAINDllOBJECT->SetStatusCode("DXFALGN0000");
				return;
			}
		}
		else if(getClicksDone() == 4 && MAINDllOBJECT->getCurrentWindow() != 2)
		{
			if(MAINDllOBJECT->highlightedPoint() != NULL )
			{
				double pt1[2] = {Dxfpoint.getX(), Dxfpoint.getY()}, pt2[2] = {MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY()};
				double slope1 = ((Line*)DxfShape)->Angle(), slope2 = ((Line*)VideoShape)->Angle(), transform[9] = {0};
				double RotatedAngle, ShiftedOrigin[2] = { 0 };
				RMATH2DOBJECT->TransformM_PointLineAlign(&pt1[0], ((Line*)DxfShape)->Angle(), ((Line*)DxfShape)->Intercept(), &pt2[0], 
								((Line*)VideoShape)->Angle(), ((Line*)VideoShape)->Intercept(), &transform[0], &RotatedAngle, ShiftedOrigin);
				PPCALCOBJECT->TransformProgramShapeMeasureAction(transform);
				Align_Finished();
				return;
			}
			else
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			MAINDllOBJECT->SetStatusCode("DXFALGN0000");
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM1LN1PTALIGN03", __FILE__, __FUNCSIG__); }
}

//Handle escape button press.. Clear the current selection..
void Program1Line1PtHandler::Align_Finished()
{
	try
	{
		PPCALCOBJECT->UpdateFistFramegrab(true);
		DxfShape =  NULL; VideoShape = NULL;
		resetClicks();
		MAINDllOBJECT->SetStatusCode("DXFALGN0001");
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM1LN1PTALIGN04", __FILE__, __FUNCSIG__); }
}

//draw selected point on dxf window..
void Program1Line1PtHandler::drawSelectedShapesOndxf()
{
}

void Program1Line1PtHandler::drawSelectedShapeOnRCad()
{
	try
	{
		double PixelWidth = MAINDllOBJECT->getWindow(2).getUppX();
		GRAFIX->SetColor_Double(0, 1, 0);
		if(getClicksDone() >= 2)
			GRAFIX->drawPoint(Dxfpoint.getX(), Dxfpoint.getY(), 0, 8);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM1LN1PTALIGN05", __FILE__, __FUNCSIG__); }
}

//draw selected point on video..
void Program1Line1PtHandler::drawSelectedShapesOnvideo()
{
	try
	{
		double PixelWidth = MAINDllOBJECT->getWindow(2).getUppX();
		GRAFIX->SetColor_Double(0, 1, 0);
		if(getClicksDone() >= 2)
			GRAFIX->drawPoint(Dxfpoint.getX(), Dxfpoint.getY(), 0, 8);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM1LN1PTALIGN06", __FILE__, __FUNCSIG__); }
}

 void Program1Line1PtHandler::Align_mouseMove(double x, double y)
{
	 try
	 {
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM1LN1PTALIGN07", __FILE__, __FUNCSIG__); }
}

 //Check for the highlighted shape..
bool Program1Line1PtHandler::CheckHighlightedShape()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)return false;
		if(MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::XLINE
			|| MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::XRAY) return true;
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM1LN1PTALIGN08", __FILE__, __FUNCSIG__); return false; }
}

void Program1Line1PtHandler::ResetShapeHighlighted()
{
	try
	{
		if(DxfShape != NULL)
		{
			DxfShape->HighlightedForMeasurement(false);
			DxfShape =  NULL;
		}
		if(VideoShape != NULL)
		{
			VideoShape->HighlightedForMeasurement(false);
			VideoShape =  NULL;
		}
		MAINDllOBJECT->DXFShape_Updated_Alignment();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM1LN1PTALIGN09", __FILE__, __FUNCSIG__); }
}

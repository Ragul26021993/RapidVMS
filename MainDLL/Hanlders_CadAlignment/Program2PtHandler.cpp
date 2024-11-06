#include "stdafx.h"
#include "Program2PtHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"
//Constructor...
Program2PtHandler::Program2PtHandler()
{
	try
	{
		this->AlignmentModeFlag = true;
		setMaxClicks(5);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM2PTALIGN01", __FILE__, __FUNCSIG__); }
}

//Destructor...//
Program2PtHandler::~Program2PtHandler()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM2PTALIGN02", __FILE__, __FUNCSIG__); }
}

//Left mousedown.. select point and calculates the transforamtion matrix..//
void Program2PtHandler::SelectPoints_Shapes()
{
	try
	{
		if(MAINDllOBJECT->getCurrentWindow() != 2 && getClicksDone() < 3)
		{
			if(MAINDllOBJECT->highlightedPoint() != NULL)
		       points[getClicksDone() - 1].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
			else
				points[getClicksDone() - 1].set(getClicksValue(getClicksDone() - 1).getX(), getClicksValue(getClicksDone() - 1).getY());
		}
		else if(getClicksDone() == 3 && MAINDllOBJECT->getCurrentWindow() != 2)
		{
			if(MAINDllOBJECT->highlightedPoint() != NULL)
		       points[getClicksDone() - 1].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
			else
				points[getClicksDone() - 1].set(getClicksValue(getClicksDone() - 1).getX(), getClicksValue(getClicksDone() - 1).getY());
		}
		else if(getClicksDone() == 4 && MAINDllOBJECT->getCurrentWindow() != 2)
		{
			if(MAINDllOBJECT->highlightedPoint() != NULL)
		       points[getClicksDone() - 1].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
			else
				points[getClicksDone() - 1].set(getClicksValue(getClicksDone() - 1).getX(), getClicksValue(getClicksDone() - 1).getY());
			double TransformMatrix[9] = {0};
			double Point1[3] = {points[0].getX(), points[0].getY(), points[0].getZ()}, Point2[3] = {points[1].getX(), points[1].getY(), points[1].getZ()}, Point3[3] = {points[2].getX(), points[2].getY(), points[2].getZ()}, Point4[3] = {points[3].getX(), points[3].getY(), points[3].getZ()};
			double RotatedAngle, ShiftedOrigin[2] = { 0 };
			RMATH2DOBJECT->TransformM_TwoPointAlign(Point1, Point2, Point3, Point4, TransformMatrix, &RotatedAngle, ShiftedOrigin);
			PPCALCOBJECT->TransformProgramShapeMeasureAction(TransformMatrix);
			Align_Finished();
			resetClicks();
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM2PTALIGN03", __FILE__, __FUNCSIG__); }
}

//Handle escape button press.. Clear the current selection..
void Program2PtHandler::Align_Finished()
{
	try
	{
		PPCALCOBJECT->UpdateFistFramegrab(true);
		resetClicks();
		MAINDllOBJECT->SetStatusCode("DXFALGN0001");
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM2PTALIGN04", __FILE__, __FUNCSIG__); }
}

//draw selected point on dxf window..
void Program2PtHandler::drawSelectedShapesOndxf()
{
}

void Program2PtHandler::drawSelectedShapeOnRCad()
{
	try
	{
		double PixelWidth = MAINDllOBJECT->getWindow(2).getUppX();
		GRAFIX->SetColor_Double(0, 1, 0);
		if(getClicksDone() >= 1)
		{
			GRAFIX->drawPoint(points[0].getX(), points[0].getY(), 0, 8);
			if(getClicksDone() >= 2)
			{
				GRAFIX->drawPoint(points[1].getX(), points[1].getY(), 0, 8);
				if(getClicksDone() >= 3)
					GRAFIX->drawPoint(points[2].getX(), points[2].getY(), 0, 8);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM2PTALIGN05", __FILE__, __FUNCSIG__); }
}

//draw selected point on video..
void Program2PtHandler::drawSelectedShapesOnvideo()
{
	try
	{
		double PixelWidth = MAINDllOBJECT->getWindow(2).getUppX();
		GRAFIX->SetColor_Double(0, 1, 0);
		if(getClicksDone() >= 1)
		{
			GRAFIX->drawPoint(points[0].getX(), points[0].getY(), 0, 8);
			if(getClicksDone() >= 2)
			{
				GRAFIX->drawPoint(points[1].getX(), points[1].getY(), 0, 8);
				if(getClicksDone() >= 3)
					GRAFIX->drawPoint(points[2].getX(), points[2].getY(), 0, 8);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM2PTALIGN06", __FILE__, __FUNCSIG__); }
}

 void Program2PtHandler::Align_mouseMove(double x, double y)
{
	 try
	 {
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PROGRAM2PTALIGN07", __FILE__, __FUNCSIG__); }
}
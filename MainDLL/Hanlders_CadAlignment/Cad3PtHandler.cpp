#include "stdafx.h"
#include "Cad3PtHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\DXF\DXFExpose.h"
//Constructor...
Cad3PtHandler::Cad3PtHandler()
{
	try
	{
		this->AlignmentModeFlag = true;
		setMaxClicks(7);
		MAINDllOBJECT->CheckStlHighlightedPt = true;	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PT1LINEMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
Cad3PtHandler::~Cad3PtHandler()
{
	try
	{
		MAINDllOBJECT->CheckStlHighlightedPt = false;	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PT1LINEMH0002", __FILE__, __FUNCSIG__); }
}

//Left mousedown.. select point and calculates the transforamtion matrix..//
void Cad3PtHandler::SelectPoints_Shapes()
{
	try
	{
		if(MAINDllOBJECT->getCurrentWindow() == 1 && getClicksDone() < 4)
		{
			if(MAINDllOBJECT->highlightedPoint() == NULL && !MAINDllOBJECT->StlHighlightedPointFlag)
			{
				setClicksDone(getClicksDone() - 1);
				MAINDllOBJECT->SetStatusCode("DXFALGN0000");
				return;
			}
			double PtsValue[3] = {0};
			if(MAINDllOBJECT->highlightedPoint() != NULL)
			{
				PtsValue[0] = MAINDllOBJECT->highlightedPoint()->getX(); PtsValue[1] = MAINDllOBJECT->highlightedPoint()->getY(); PtsValue[2] = MAINDllOBJECT->highlightedPoint()->getZ(); 
			}
			else if(MAINDllOBJECT->StlHighlightedPointFlag)
			{
				PtsValue[0] = MAINDllOBJECT->StlHighlightedPt.getX(); PtsValue[1] = MAINDllOBJECT->StlHighlightedPt.getY(); PtsValue[2] = MAINDllOBJECT->StlHighlightedPt.getZ(); 
			}
			else
			{
				setClicksDone(getClicksDone() - 1);
				MAINDllOBJECT->SetStatusCode("DXFALGN0000");
				return;
			}
			Point1[getClicksDone() - 1].set(PtsValue[0], PtsValue[1], PtsValue[2]);
		}
		else if((MAINDllOBJECT->getCurrentWindow() == 1 || MAINDllOBJECT->getCurrentWindow() == 0) && getClicksDone() > 3)
		{
			if(MAINDllOBJECT->highlightedPoint() == NULL)
			{
				setClicksDone(getClicksDone() - 1);
				MAINDllOBJECT->SetStatusCode("DXFALGN0000");
				return;
			}
			Point2[getClicksDone() - 4].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY(), MAINDllOBJECT->highlightedPoint()->getZ());
		}
		else 
		{
			setClicksDone(getClicksDone() - 1);
			MAINDllOBJECT->SetStatusCode("DXFALGN0000");
			return;
		}
		if(getClicksDone() == 6)
		{
			double transformMatrix1[16] = {0}, translation[3] = {0};
			double  pt1[3] = {Point1[0].getX(), Point1[0].getY(), Point1[0].getZ()}, pt2[3] = {Point2[0].getX(), Point2[0].getY(), Point2[0].getZ()};
			double line1[6] = {Point1[1].getX(), Point1[1].getY(), Point1[1].getZ(), Point1[2].getX(), Point1[2].getY(), Point1[2].getZ()};
			double line2[6] = {Point2[1].getX(), Point2[1].getY(), Point2[1].getZ(), Point2[2].getX(), Point2[2].getY(), Point2[2].getZ()};
			RMATH3DOBJECT->TransformationMatrix_IGESAlignment_1Pt1Line(line1, pt1, line2, pt2, transformMatrix1);
			DXFEXPOSEOBJECT->TransformMatrix3D_1Line(&transformMatrix1[0], pt2, pt1, false);
			MAINDllOBJECT->CheckStlHighlightedPt = false;	
			MAINDllOBJECT->SetStatusCode("DXFALGN0015");
			resetClicks();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PT1LINEMH0003", __FILE__, __FUNCSIG__); }
}

//Handle escape button press.. Clear the current selection..
void Cad3PtHandler::Align_Finished()
{
	try
	{
		resetClicks();
		MAINDllOBJECT->SetStatusCode("DXFALGN0001");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PT1LINEMH0005", __FILE__, __FUNCSIG__); }
}

//draw selected point on dxf window..
void Cad3PtHandler::drawSelectedShapesOndxf()
{
	try
	{	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PT1LINEMH0006", __FILE__, __FUNCSIG__); }
}

void Cad3PtHandler::drawSelectedShapeOnRCad()
{
	try
	{
		double PixelWidth = MAINDllOBJECT->getWindow(1).getUppX();
		GRAFIX->SetColor_Double(0, 1, 0);
		if(getClicksDone() == 1)
			GRAFIX->drawPoint(Point1[0].getX(), Point1[1].getY(), Point1[0].getZ(), 8);
		else if(getClicksDone() == 2)
		{
			GRAFIX->drawPoint(Point1[0].getX(), Point1[0].getY(), Point1[0].getZ(), 8);
			GRAFIX->drawPoint(Point1[1].getX(), Point1[1].getY(), Point1[1].getZ(), 8);
		}
		else if(getClicksDone() == 3)
		{
			GRAFIX->drawPoint(Point1[0].getX(), Point1[0].getY(), Point1[0].getZ(), 8);
			GRAFIX->drawPoint(Point1[1].getX(), Point1[1].getY(), Point1[1].getZ(), 8);
			GRAFIX->drawPoint(Point1[2].getX(), Point1[2].getY(), Point1[2].getZ(), 8);
		}
		else if(getClicksDone() == 4)
		{
			GRAFIX->drawPoint(Point1[0].getX(), Point1[0].getY(), Point1[0].getZ(), 8);
			GRAFIX->drawPoint(Point1[1].getX(), Point1[1].getY(), Point1[1].getZ(), 8);
			GRAFIX->drawPoint(Point1[2].getX(), Point1[2].getY(), Point1[2].getZ(), 8);
			GRAFIX->drawPoint(Point2[0].getX(), Point2[0].getY(), Point2[0].getZ(), 8);
		}
		else if(getClicksDone() == 5)
		{
			GRAFIX->drawPoint(Point1[0].getX(), Point1[0].getY(), Point1[0].getZ(), 8);
			GRAFIX->drawPoint(Point1[1].getX(), Point1[1].getY(), Point1[1].getZ(), 8);
			GRAFIX->drawPoint(Point1[2].getX(), Point1[2].getY(), Point1[2].getZ(), 8);
			GRAFIX->drawPoint(Point2[0].getX(), Point2[0].getY(), Point2[0].getZ(), 8);
			GRAFIX->drawPoint(Point2[1].getX(), Point2[1].getY(), Point2[1].getZ(), 8);
		}		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PT1LINEMH0006", __FILE__, __FUNCSIG__); }
}

//draw selected point on video..
void Cad3PtHandler::drawSelectedShapesOnvideo()
{
	try
	{
		double PixelWidth = MAINDllOBJECT->getWindow(0).getUppX();
		GRAFIX->SetColor_Double(0, 1, 0);
		if(getClicksDone() == 4)
		{
			GRAFIX->drawPoint(Point2[0].getX(), Point2[0].getY(), 0, 8);
		}
		if(getClicksDone() == 5)
		{
			GRAFIX->drawPoint(Point2[0].getX(), Point2[0].getY(), 0, 8);
			GRAFIX->drawPoint(Point2[1].getX(), Point2[1].getY(), 0, 8);
		}		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PT1LINEMH0007", __FILE__, __FUNCSIG__); }
}

void Cad3PtHandler::Align_mouseMove(double x, double y)
{
	 try
	 {

	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PT1LINEMH0008", __FILE__, __FUNCSIG__); }
}
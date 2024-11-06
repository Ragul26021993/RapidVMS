#include "stdafx.h"
#include "PP2PtHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\DXF\DXFExpose.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor...
PP2PtHandler::PP2PtHandler()
{
	try
	{
		this->AlignmentModeFlag = true;
		VideoCnt = 0; RcadCnt = 0;
		setMaxClicks(5);
		MAINDllOBJECT->SetStatusCode("DXFALGN0002");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP2PTALIGN01", __FILE__, __FUNCSIG__); }
}

//Destructor...//
PP2PtHandler::~PP2PtHandler()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP2PTALIGN02", __FILE__, __FUNCSIG__); }
}

//Left mousedown.. select point and calculates the transforamtion matrix..//
void PP2PtHandler::SelectPoints_Shapes()
{
	try
	{
		if(PPCALCOBJECT->PPAlignEditFlag())
		{
			if(MAINDllOBJECT->getCurrentWindow() == 1 && getClicksDone() < 3)
			{
				if(RcadCnt != 2 && MAINDllOBJECT->highlightedPoint() != NULL)
					points[RcadCnt++].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
				else
				{
					setClicksDone(getClicksDone() - 1);
					return;
				}
				if(RcadCnt == 1)
					MAINDllOBJECT->SetStatusCode("DXFALGN0006");
				else
					MAINDllOBJECT->SetStatusCode("DXFALGN0007");

			}
			else if(getClicksDone() < 4 && MAINDllOBJECT->getCurrentWindow() == 0 && RcadCnt == 2)
			{
				if(MAINDllOBJECT->highlightedPoint() != NULL)
					points[VideoCnt + 2].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
				else
					points[VideoCnt + 2].set(getClicksValue(getClicksDone() - 1).getX(), getClicksValue(getClicksDone() - 1).getY());
				VideoCnt++;
				double pt1[2] = {points[0].getX(), points[0].getY()}, pt2[2] = {points[2].getX(), points[2].getY()}, transform[9] = {0};
				RMATH2DOBJECT->TransformM_OnePtAlign(&pt1[0], &pt2[0], &transform[0]);
				points[0].set(points[2].getX(), points[2].getY());
				Vector TransPt;
				TransPt.set(transform[2], transform[5]);
				DXFEXPOSEOBJECT->SetTransVector(TransPt);
				points[1].set(MAINDllOBJECT->TransformMultiply(transform, points[1].getX(), points[1].getY()));
				MAINDllOBJECT->SetStatusCode("DXF008");	
			}
			else if(getClicksDone() > 3 && MAINDllOBJECT->getCurrentWindow() == 0)
			 {
				if(MAINDllOBJECT->highlightedPoint() != NULL)
					points[VideoCnt + 2].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
				else
					points[VideoCnt + 2].set(getClicksValue(getClicksDone() - 1).getX(), getClicksValue(getClicksDone() - 1).getY());
				if(getClicksDone() == 4)
				{
					/*double pt1[2] = {points[0].getX(), points[0].getY()}, pt2[2] = {points[1].getX(), points[1].getY()},
						pt3[2] = {points[2].getX(), points[2].getY()}, pt4[2] = {points[3].getX(), points[3].getY()};
					double transform[9] = {0};
					RMATH2DOBJECT->TransformM_TwoPointAlign(&pt1[0], &pt2[0], &pt3[0], &pt4[0], &transform[0]);
					DXFEXPOSEOBJECT->TransformMatrix(&transform[0]);*/
					MAINDllOBJECT->SetStatusCode("DXFALGN0015");
					resetClicks();
					DXFEXPOSEOBJECT->AlginTheDXF(true);
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
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP2PTALIGN03", __FILE__, __FUNCSIG__); }
}

//Handle escape button press.. Clear the current selection..
void PP2PtHandler::Align_Finished()
{
	try
	{
		VideoCnt = 0; RcadCnt = 0;
		resetClicks();
		MAINDllOBJECT->SetStatusCode("DXFALGN0001");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP2PTALIGN04", __FILE__, __FUNCSIG__); }
}

//draw selected point on dxf window..
void PP2PtHandler::drawSelectedShapesOndxf()
{}	


void PP2PtHandler::drawSelectedShapeOnRCad()
{
	try
	{
		if(PPCALCOBJECT->PPAlignEditFlag())
		{
			GRAFIX->SetColor_Double(0, 1, 0);
			if(RcadCnt >= 1)
				GRAFIX->drawPoint(points[0].getX(), points[0].getY(), 0, 8);
			if(RcadCnt == 2)
				GRAFIX->drawPoint(points[1].getX(), points[1].getY(), 0, 8);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP2PTALIGN05", __FILE__, __FUNCSIG__); }
}

//draw selected point on video..
void PP2PtHandler::drawSelectedShapesOnvideo()
{
	try
	{
		GRAFIX->SetColor_Double(0, 1, 0);
		if(VideoCnt >= 1)
			GRAFIX->drawPoint(points[2].getX(), points[2].getY(), 0, 8);
		if(VideoCnt == 2)
			GRAFIX->drawPoint(points[3].getX(), points[3].getY(), 0, 8);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP2PTALIGN06", __FILE__, __FUNCSIG__); }
}

void PP2PtHandler::Align_mouseMove(double x, double y)
{
	 try
	 {			
		 if(MAINDllOBJECT->getCurrentWindow() == 0 && getClicksDone() == 3)
		 { 
			 points[3].set(x, y);
			 double Angle1 = RMATH2DOBJECT->ray_angle(points[0].getX(), points[0].getY(), points[1].getX(), points[1].getY());
			 double Angle2 = RMATH2DOBJECT->ray_angle(points[2].getX(), points[2].getY(), points[3].getX(), points[3].getY());
			 double AngDiff = Angle2 - Angle1;
			 double pt[2] = {points[2].getX(), points[2].getY()};
			 double transform[9] = {0};
			 RMATH2DOBJECT->TransformM_RotateAboutPoint(&pt[0], AngDiff, &transform[0]);
			 Vector TransPt;
			 TransPt.set(transform[2], transform[5]);
			 transform[2] = 0; transform[5] = 0;
			 DXFEXPOSEOBJECT->TransformMatrix(&transform[0]);
			 DXFEXPOSEOBJECT->SetTransVector(TransPt);
			 points[1].set(x, y);
		}		
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP2PTALIGN07", __FILE__, __FUNCSIG__); }
}
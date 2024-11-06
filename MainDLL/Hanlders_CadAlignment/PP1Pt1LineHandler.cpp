#include "stdafx.h"
#include "PP1Pt1LineHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\Line.h"
#include "..\DXF\DXFExpose.h"
//Constructor...
PP1Pt1LineHandler::PP1Pt1LineHandler()
{
	try
	{
		this->AlignmentModeFlag = true;
		VideoPointFlag = false; VideoShapeFlag = false;
		RcadPointFlag = false; RcadShapeFlag = false;
		setMaxClicks(5);
		MAINDllOBJECT->SetStatusCode("DXFALGN0003");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP1PT1LINEALIGN01", __FILE__, __FUNCSIG__); }
}

//Destructor...//
PP1Pt1LineHandler::~PP1Pt1LineHandler()
{
	try
	{		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP1PT1LINEALIGN02", __FILE__, __FUNCSIG__); }
}

//Left mousedown.. select point and calculates the transforamtion matrix..//
void PP1Pt1LineHandler::SelectPoints_Shapes()
{
	try
	{
		if(MAINDllOBJECT->getCurrentWindow() == 1 && getClicksDone() < 3)
		{
			if(MAINDllOBJECT->highlightedPoint() == NULL && MAINDllOBJECT->highlightedShape() == NULL)
			{
				setClicksDone(getClicksDone() - 1);
				MAINDllOBJECT->SetStatusCode("DXFALGN0000");
				return;
			}
			if(!RcadPointFlag && MAINDllOBJECT->highlightedPoint() != NULL)
			{
				Rcadpoint.set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
				RcadPointFlag = true;
				if(!RcadShapeFlag)
					MAINDllOBJECT->SetStatusCode("DXFALGN0009");
			}
			else if(!RcadShapeFlag && MAINDllOBJECT->highlightedShape() != NULL)
			{
				RcadShape = MAINDllOBJECT->highlightedShape();
				RcadShapeFlag = true;
				if(!RcadPointFlag)
					MAINDllOBJECT->SetStatusCode("DXFALGN0001");
			}
			else
			{
				setClicksDone(getClicksDone() - 1);
				MAINDllOBJECT->SetStatusCode("DXFALGN0000");
				return;
			}
			if(RcadPointFlag && RcadShapeFlag)
				MAINDllOBJECT->SetStatusCode("DXFALGN0010");

		}
		else if(getClicksDone() > 2 && MAINDllOBJECT->getCurrentWindow() == 0)
		{
			if(!VideoShapeFlag && MAINDllOBJECT->highlightedShape() != NULL)
			{
				VideoShape = MAINDllOBJECT->highlightedShape();
				VideoShapeFlag = true;
				if(!VideoPointFlag)
					MAINDllOBJECT->SetStatusCode("DXFALGN0005");
			}
			if(VideoShapeFlag)
			{
				if(MAINDllOBJECT->highlightedPoint() != NULL)
					VideoPoint.set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
				else
					VideoPoint.set(getClicksValue(getClicksDone() - 1).getX(), getClicksValue(getClicksDone() - 1).getY());				
			}
			else
			{
				if(!VideoShapeFlag)
				{
					setClicksDone(getClicksDone() - 1);
					MAINDllOBJECT->SetStatusCode("DXFALGN0000");
					return;
				}
			}
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			MAINDllOBJECT->SetStatusCode("DXFALGN0000");
			return;
		}
		if(getClicksDone() == 4)
		{
			/*double pt1[2] = {Rcadpoint.getX(), Rcadpoint.getY()}, pt2[2] = {VideoPoint.getX(), VideoPoint.getY()};
			double slope1 = ((Line*)RcadShape)->Angle(), slope2 = ((Line*)VideoShape)->Angle(), transform[9] = {0};
			RMATH2DOBJECT->TransformM_PointLineAlign(&pt1[0], ((Line*)RcadShape)->Angle(), ((Line*)RcadShape)->Intercept(), &pt2[0], ((Line*)VideoShape)->Angle(), ((Line*)VideoShape)->Intercept(), &transform[0]);
			DXFEXPOSEOBJECT->TransformMatrix(&transform[0]);*/
			MAINDllOBJECT->SetStatusCode("DXFALGN0015");	
			resetClicks();
			DXFEXPOSEOBJECT->AlginTheDXF(true);
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP1PT1LINEALIGN04", __FILE__, __FUNCSIG__); }
}

//Handle escape button press.. Clear the current selection..
void PP1Pt1LineHandler::Align_Finished()
{
	try
	{
		VideoPointFlag = false; VideoShapeFlag = false;
		RcadPointFlag = false; RcadShapeFlag = false;
		
		resetClicks();
		MAINDllOBJECT->SetStatusCode("DXFALGN0001");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP1PT1LINEALIGN06", __FILE__, __FUNCSIG__); }
}

//draw selected point on dxf window..
void PP1Pt1LineHandler::drawSelectedShapesOndxf()
{
}

void PP1Pt1LineHandler::drawSelectedShapeOnRCad()
{
	try
	{
		double PixelWidth = MAINDllOBJECT->getWindow(1).getUppX();
		GRAFIX->SetColor_Double(0, 1, 0);
		if(RcadPointFlag)
			GRAFIX->drawPoint(Rcadpoint.getX(), Rcadpoint.getY(), 0, 8);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP1PT1LINEALIGN07", __FILE__, __FUNCSIG__); }
}

//draw selected point on video..
void PP1Pt1LineHandler::drawSelectedShapesOnvideo()
{
	try
	{
		double PixelWidth = MAINDllOBJECT->getWindow(2).getUppX();
		GRAFIX->SetColor_Double(0, 1, 0);
		if(VideoPointFlag)
			GRAFIX->drawPoint(VideoPoint.getX(), VideoPoint.getY(), 0, 8);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP1PT1LINEALIGN08", __FILE__, __FUNCSIG__); }
}

void PP1Pt1LineHandler::Align_mouseMove(double x, double y)
{
	 try
	 {	 
		 if(MAINDllOBJECT->getCurrentWindow() == 0 && getClicksDone() == 3)
		 {
			 if(VideoShapeFlag)
			 {
				 VideoPoint.set(x, y);
				 double pt1[2] = {Rcadpoint.getX(), Rcadpoint.getY()}, pt2[2] = {VideoPoint.getX(), VideoPoint.getY()};
				 double slope1 = ((Line*)RcadShape)->Angle(), slope2 = ((Line*)VideoShape)->Angle(), transform[9] = {0};
				 double RotatedAngle, ShiftedOrigin[2] = { 0 };
				 RMATH2DOBJECT->TransformM_PointLineAlign(&pt1[0], ((Line*)RcadShape)->Angle(), ((Line*)RcadShape)->Intercept(), &pt2[0], 
					 ((Line*)VideoShape)->Angle(), ((Line*)VideoShape)->Intercept(), &transform[0], &RotatedAngle, ShiftedOrigin);
				 Rcadpoint.set(x, y);
				 Vector TransPt;
				 TransPt.set(transform[2], transform[5]);
				 transform[2] = 0; transform[5] = 0;
				 DXFEXPOSEOBJECT->TransformMatrix(&transform[0]);
				 DXFEXPOSEOBJECT->SetTransVector(TransPt);
			 }
		 }
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP1PT1LINEALIGN09", __FILE__, __FUNCSIG__); }
}
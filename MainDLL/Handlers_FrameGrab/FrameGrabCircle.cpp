#include "StdAfx.h"
#include "FrameGrabCircle.h"
#include "..\Engine\RCadApp.h"

//Constructor..//
FrameGrabCircle::FrameGrabCircle()
{
	try
	{
		CurrentFGType = RapidEnums::RAPIDFGACTIONTYPE::CIRCLEFRAMEGRAB;
		FgCirclewidth = MAINDllOBJECT->FrameGrab_Circular.FrameGrab_CurrentWidth;
		WidthDirection = MAINDllOBJECT->FGDirection_Circular();
		withDirectionScan =  MAINDllOBJECT->FGDirection_Circular();
		MAINDllOBJECT->SetStatusCode("FrameGrabCircle01");
		setMaxClicks(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGCIRCLEMH0001", __FILE__, __FUNCSIG__); }
}

//Used in partprogram
FrameGrabCircle::FrameGrabCircle(FramegrabBase* Cfg):FrameGrabHandler(Cfg)
{
	try
	{
		baseaction = Cfg;
		FgCirclewidth = baseaction->FGWidth;
		WidthDirection = (bool)baseaction->FGdirection;
		setMaxClicks(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGCIRCLEMH0002", __FILE__, __FUNCSIG__); }
}

//Destructor...//
FrameGrabCircle::~FrameGrabCircle()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGCIRCLEMH0003", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move......//
void FrameGrabCircle::FG_mouseMove()
{
	try
	{
		if(getClicksDone() > 0 && getClicksDone() < getMaxClicks())
		{
			PointsDRO[getClicksDone()].set(getClicksValue(getClicksDone()));
			PointsPixel[getMaxClicks() - 1].set(MAINDllOBJECT->getWindow(0).getLastWinMouse().x, MAINDllOBJECT->getWindow(0).getLastWinMouse().y);
		}
		if(getClicksDone() == getMaxClicks() - 1)
			ValidFlag = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGCIRCLEMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse down event......//
void FrameGrabCircle::FG_LmouseDown()
{
	try
	{
		if(getClicksDone() == 1)
			FgCirclewidth = MAINDllOBJECT->FrameGrab_Circular.FrameGrab_CurrentWidth;
		PointsDRO[getClicksDone() - 1].set(getClicksValue(getClicksDone() - 1));
		PointsPixel[getClicksDone() - 1].set(MAINDllOBJECT->getWindow(0).getLastWinMouse().x, MAINDllOBJECT->getWindow(0).getLastWinMouse().y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGCIRCLEMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the maximum click..//
void FrameGrabCircle::FG_LmaxmouseDown()
{
	try
	{
		baseaction->FGdirection = WidthDirection;
		baseaction->FGWidth = FgCirclewidth;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGCIRCLEMH0006", __FILE__, __FUNCSIG__); }
}

//Change the grab direction..
void FrameGrabCircle::FG_ChangeDrirection()
{
	try
	{
		WidthDirection = MAINDllOBJECT->FGDirection_Circular();
		WidthDirection = !WidthDirection;
		MAINDllOBJECT->FGDirection_Circular(WidthDirection);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGCIRCLEMH0007", __FILE__, __FUNCSIG__); }
}

void FrameGrabCircle::FG_ChangeScanDirection()
{
	withDirectionScan = !withDirectionScan;
}

//Draw Angular rect framegrab//
void FrameGrabCircle::FG_draw(double wupp, int windowno)
{
	try
	{
		double width = FgCirclewidth * wupp;
		double radius = 0, radius1 = 0, radius2 = 0, cen[2] = {0};
		double Wwid1 = 35 * wupp, Wwid2 = 2 * wupp;
		PointsDRO[2].set((PointsDRO[0].getX() + PointsDRO[1].getX())/2 , (PointsDRO[0].getY() + PointsDRO[1].getY())/2, PointsDRO[0].getZ());
		radius = RMATH2DOBJECT->Pt2Pt_distance(PointsDRO[0].getX(), PointsDRO[0].getY(), PointsDRO[2].getX(), PointsDRO[2].getY());
		radius1 = radius + width + wupp;
		radius2 = radius - width - wupp;
		cen[0] = PointsDRO[2].getX(); cen[1] = PointsDRO[2].getY();
		GRAFIX->translateMatrix(0.0, 0.0, 2.0);
		if(windowno == 0)
		{
			GRAFIX->SetColor_Double(0, 0, 1);
			if(!WidthDirection)
			{
				double pt1[2] = {cen[0] + radius1 + Wwid1, cen[1]}, pt2[2] = {cen[0] + radius1, cen[1] + Wwid1}, pt3[2] = {cen[0] + radius1, cen[1] - Wwid1};
				GRAFIX->SetColor_Double(0, 0, 1);
				GRAFIX->drawTriangle(pt1[0], pt1[1], pt2[0], pt2[1], pt3[0], pt3[1], true);
			}
			else
			{
				double pt1[2] = {cen[0] + radius2 - Wwid1, cen[1]}, pt2[2] = {cen[0] + radius2 - Wwid2, cen[1] + Wwid1}, pt3[2] = {cen[0] + radius2 - Wwid2, cen[1] - Wwid1};
				GRAFIX->SetColor_Double(0, 0, 1);
				GRAFIX->drawTriangle(pt1[0], pt1[1], pt2[0], pt2[1], pt3[0], pt3[1], true);
			}
			if(this->drawscanDir)
			{
				if(!withDirectionScan)
				{
					double pt1[2] = {cen[0], cen[1]+ radius1 + Wwid1}, pt2[2] = {cen[0] + Wwid1, cen[1] + radius1}, pt3[2] = {cen[0] - Wwid1, cen[1] + radius1};
					GRAFIX->SetColor_Double(0, 1, 0);
					GRAFIX->drawTriangle(pt1[0], pt1[1], pt2[0], pt2[1], pt3[0], pt3[1], true);
					scandir[0] = pt1[0] - cen[0];
				    scandir[1] = pt1[1] - cen[1] - radius1;
				    scandir[2] = 0;
					RMATH3DOBJECT->DirectionCosines(scandir, scandir);
				}
				else
				{
					double pt1[2] = {cen[0], cen[1] + radius2 - Wwid1}, pt2[2] = {cen[0] + Wwid1, cen[1] + radius2 - Wwid2}, pt3[2] = {cen[0] - Wwid1, cen[1] + radius2 - Wwid2};
					GRAFIX->SetColor_Double(0, 1, 0);
					GRAFIX->drawTriangle(pt1[0], pt1[1], pt2[0], pt2[1], pt3[0], pt3[1], true);
					scandir[0] = pt1[0] - cen[0];
				    scandir[1] = pt1[1] - cen[1] - radius1;
				    scandir[2] = 0;
				}
				GRAFIX->SetColor_Double(0, 0, 1);
				RMATH3DOBJECT->DirectionCosines(scandir, scandir);
			}
		}
		else
		{
    		GRAFIX->SetColor_Double(1, 0, 0);
    	}
		GRAFIX->drawCircle(PointsDRO[2].getX(), PointsDRO[2].getY(), radius1);
		GRAFIX->drawCircle(PointsDRO[2].getX(), PointsDRO[2].getY(), radius2);
		GRAFIX->translateMatrix(0.0, 0.0, -2.0);
		if(windowno == 0)
		{
		    GRAFIX->SetColor_Double(0.4f, 0.4f, 0.4f); 
		    GRAFIX->drawCircle(PointsDRO[2].getX(), PointsDRO[2].getY(), radius - width, radius + width);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGCIRCLEMH0008", __FILE__, __FUNCSIG__); }
}

//Increase the width of the framegrab...//
void FrameGrabCircle::FG_MouseScroll(bool flag, double wupp)
{
	try
	{
		FgCirclewidth = MAINDllOBJECT->FrameGrab_Circular.FrameGrab_CurrentWidth;
		if(flag){if(FgCirclewidth < MAINDllOBJECT->FrameGrab_Circular.FrameGrab_MaxWidth) FgCirclewidth += 5;}
		else {if(FgCirclewidth > MAINDllOBJECT->FrameGrab_Circular.FrameGrab_MinWidth) FgCirclewidth -= 5; }
		MAINDllOBJECT->FrameGrab_Circular.FrameGrab_CurrentWidth = FgCirclewidth;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGCIRCLEMH0009", __FILE__, __FUNCSIG__); }
}

//Set the frame settings..
void FrameGrabCircle::FG_UpdateFirstFrameppLoad()
{
	try
	{
		FgCirclewidth = baseaction->FGWidth;
		WidthDirection = (bool)baseaction->FGdirection;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGCIRCLEMH0010", __FILE__, __FUNCSIG__); }
}
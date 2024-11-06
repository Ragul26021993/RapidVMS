#include "StdAfx.h"
#include "FrameGrabArc.h"
#include "..\Engine\RCadApp.h"

//Constructor..//
FrameGrabArc::FrameGrabArc()
{
	try
	{
		CurrentFGType = RapidEnums::RAPIDFGACTIONTYPE::ARCFRAMEGRAB;
		FgArcwidth = MAINDllOBJECT->FrameGrab_Arc.FrameGrab_CurrentWidth;
		WidthDirection = MAINDllOBJECT->FGDirection_Arc();
		withDirectionScan = MAINDllOBJECT->FGDirection_Arc();
		fgselected = false;
		setMaxClicks(3);
		//MAINDllOBJECT->SetStatusCode("FGAR001");
		MAINDllOBJECT->SetStatusCode("FrameGrabArc01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGARCMH0001", __FILE__, __FUNCSIG__); }
}

//Used in partprogram
FrameGrabArc::FrameGrabArc(FramegrabBase* Cfg):FrameGrabHandler(Cfg)
{
	try
	{
		baseaction = Cfg;
		FgArcwidth = baseaction->FGWidth;
		WidthDirection = (bool)baseaction->FGdirection;
		fgselected = false;
		setMaxClicks(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGARCMH0002", __FILE__, __FUNCSIG__); }
}

//Destructor...//
FrameGrabArc::~FrameGrabArc()
{
	/*try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGARCMH0003", __FILE__, __FUNCSIG__); }*/
}

//Handle the mouse move......//
void FrameGrabArc::FG_mouseMove()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGARCMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse down event......//
void FrameGrabArc::FG_LmouseDown()
{
	try
	{
		if(getClicksDone() == 1)
			FgArcwidth = MAINDllOBJECT->FrameGrab_Arc.FrameGrab_CurrentWidth;
		PointsDRO[getClicksDone() - 1].set(getClicksValue(getClicksDone() - 1));
		PointsPixel[getClicksDone() - 1].set(MAINDllOBJECT->getWindow(0).getLastWinMouse().x, MAINDllOBJECT->getWindow(0).getLastWinMouse().y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGARCMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the maximum click..//
void FrameGrabArc::FG_LmaxmouseDown()
{
	try
	{
		baseaction->FGdirection = WidthDirection;
		baseaction->FGWidth = FgArcwidth;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGARCMH0006", __FILE__, __FUNCSIG__); }
}

//Change the grab direction..
void FrameGrabArc::FG_ChangeDrirection()
{
	try
	{
		WidthDirection = MAINDllOBJECT->FGDirection_Arc();
		WidthDirection = !WidthDirection;
		withDirectionScan = WidthDirection;
		MAINDllOBJECT->FGDirection_Arc(WidthDirection);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGARCMH0007", __FILE__, __FUNCSIG__); }
}


void FrameGrabArc::FG_ChangeScanDirection()
{
	/*withDirectionScan = !withDirectionScan;*/
}

void FrameGrabArc::fgarc_draw(double wupp, bool selected_fg)
{
	try
	{
		fgselected = selected_fg;
		FG_draw(wupp);
		fgselected = false;
	}
	catch(...) { MAINDllOBJECT->SetAndRaiseErrorMessage("FGARCMH0007", __FILE__, __FUNCSIG__); }
}

//Draw arc framegrab//
void FrameGrabArc::FG_draw(double wupp, int windowno)
{
	try
	{
		double width = FgArcwidth * wupp;
		double point1[2] = {PointsDRO[0].getX(), PointsDRO[0].getY()}, point2[2] = {PointsDRO[1].getX(), PointsDRO[1].getY()},
		point3[2] = {PointsDRO[2].getX(), PointsDRO[2].getY()};
		double cen[2] = {0}, radius = 0, radius1 = 0, radius2 = 0, startangle = 0, sweepangle = 0, ep1[4] = {0}, ep2[4] = {0};
		RMATH2DOBJECT->Arc_3Pt(&point1[0], &point2[0], &point3[0], &cen[0], &radius, &startangle, &sweepangle);
		if (radius == 0) return;

		radius1 = radius + width + wupp;
		radius2 = radius - width - wupp;
		RMATH2DOBJECT->ArcFrameGrabEndPts(&cen[0], radius1, radius2, startangle, sweepangle, &ep1[0], &ep2[0]);
		if(windowno == 0)
		{
			double pts[14] = {0}, pts1[14] = {0}, p1[4] = {0}, p2[4] = {0};
			if (fgselected)
				GRAFIX->SetColor_Double(0, 1, 0);
			else
				GRAFIX->SetColor_Double(0.4f, 0.4f, 0.4f);

			GRAFIX->drawFillArcFG(cen[0], cen[1], radius1 - wupp, radius2 + wupp, startangle + wupp / radius, sweepangle - 2 * wupp / radius);
			GRAFIX->translateMatrix(0.0, 0.0, 2.0);
			GRAFIX->SetColor_Double(0, 0, 1);
			double Wwid1 = 35 * wupp, Wwid2 = 2 * wupp;
	/*		if(!WidthDirection)
			{					
				double angle = RMATH2DOBJECT->ray_angle(ep1[0], ep1[1], ep2[0], ep2[1]);
				double midpoint[2] = {cen[0] + (radius1) *  cos(startangle + sweepangle/2), cen[1] + (radius1) *  sin(startangle + sweepangle/2)};
				double midpoint1[2] = {cen[0] + (radius1 + Wwid1) * cos(startangle + sweepangle/2), cen[1] + (radius1 + Wwid1) * sin(startangle + sweepangle/2)};
				double pt1[2] = {midpoint[0] + Wwid1 * cos(angle),midpoint[1] + Wwid1 * sin(angle)}, pt2[2] = {midpoint[0] - Wwid1 * cos(angle),midpoint[1] - Wwid1 * sin(angle)};
				GRAFIX->drawTriangle(midpoint1[0], midpoint1[1], pt1[0], pt1[1], pt2[0], pt2[1], true);
			}
			else
			{
				double angle = RMATH2DOBJECT->ray_angle(ep1[0], ep1[1], ep2[0], ep2[1]);
				double midpoint[2] = {cen[0] + (radius2 - Wwid2) *  cos(startangle + sweepangle/2), cen[1] + (radius2 - Wwid2) *  sin(startangle + sweepangle/2)};
				double midpoint1[2] = {cen[0] + (radius2 - Wwid1) *  cos(startangle + sweepangle/2), cen[1] + (radius2 - Wwid1) *  sin(startangle + sweepangle/2)};
				double pt1[2] = {midpoint[0] + Wwid1 * cos(angle),midpoint[1] + Wwid1 * sin(angle)}, pt2[2] = {midpoint[0] - Wwid1 * cos(angle),midpoint[1] - Wwid1 * sin(angle)};
				GRAFIX->drawTriangle(midpoint1[0], midpoint1[1], pt1[0], pt1[1], pt2[0], pt2[1], true);
			}*/
			if(!WidthDirection)
			{					
				double angle = RMATH2DOBJECT->ray_angle(ep1[0], ep1[1], ep2[0], ep2[1]);
				double midpoint[2] = {cen[0] + (radius1) *  cos(startangle + sweepangle/2), cen[1] + (radius1) *  sin(startangle + sweepangle/2)};
				double midpoint1[2] = {cen[0] + (radius1 + Wwid1) * cos(startangle + sweepangle/2), cen[1] + (radius1 + Wwid1) * sin(startangle + sweepangle/2)};
				double pt1[2] = {midpoint[0] + Wwid1 * cos(angle),midpoint[1] + Wwid1 * sin(angle)}, pt2[2] = {midpoint[0] - Wwid1 * cos(angle),midpoint[1] - Wwid1 * sin(angle)};
				GRAFIX->drawTriangle(midpoint1[0], midpoint1[1], pt1[0], pt1[1],pt2[0], pt2[1], true);
				if(drawscanDir)
				{
				   scandir[0] = midpoint1[0] - midpoint[0];
				   scandir[1] = midpoint1[1] - midpoint[1];
				   scandir[2] = 0;
				   RMATH3DOBJECT->DirectionCosines(scandir, scandir);
				}
			}
			else
			{
				double angle = RMATH2DOBJECT->ray_angle(ep1[0], ep1[1], ep2[0], ep2[1]);
				double midpoint[2] = {cen[0] + (radius2 - Wwid2) *  cos(startangle + sweepangle/2), cen[1] + (radius2 - Wwid2) *  sin(startangle + sweepangle/2)};
				double midpoint1[2] = {cen[0] + (radius2 - Wwid1) *  cos(startangle + sweepangle/2), cen[1] + (radius2 - Wwid1) *  sin(startangle + sweepangle/2)};
				double pt1[2] = {midpoint[0] + Wwid1 * cos(angle),midpoint[1] + Wwid1 * sin(angle)}, pt2[2] = {midpoint[0] - Wwid1 * cos(angle),midpoint[1] - Wwid1 * sin(angle)};
				GRAFIX->drawTriangle(midpoint1[0], midpoint1[1], pt1[0], pt1[1],pt2[0], pt2[1], true);
				if(drawscanDir)
				{
				   scandir[0] = midpoint1[0] - midpoint[0];
				   scandir[1] = midpoint1[1] - midpoint[1];
				   scandir[2] = 0;
				   RMATH3DOBJECT->DirectionCosines(scandir, scandir);
				}
			}
		}
		else
		{
			GRAFIX->translateMatrix(0.0, 0.0, 2.0);
			GRAFIX->SetColor_Double(1, 0, 0);
		}
		GRAFIX->drawArc(cen[0], cen[1], radius1, startangle, sweepangle, wupp);
		GRAFIX->drawArc(cen[0], cen[1], radius2, startangle, sweepangle, wupp);
		GRAFIX->drawLine(ep1[0], ep1[1], ep1[2], ep1[3]);
		GRAFIX->drawLine(ep2[0], ep2[1], ep2[2], ep2[3]);
		GRAFIX->translateMatrix(0.0, 0.0, -2.0);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGARCMH0008", __FILE__, __FUNCSIG__); }
}

//Increase the Arcwidth of the framegrab...//
void FrameGrabArc::FG_MouseScroll(bool flag, double wupp)
{
	try
	{
		FgArcwidth = MAINDllOBJECT->FrameGrab_Arc.FrameGrab_CurrentWidth;
		if(flag){if(FgArcwidth < MAINDllOBJECT->FrameGrab_Arc.FrameGrab_MaxWidth) FgArcwidth += 5;}
		else {if(FgArcwidth > MAINDllOBJECT->FrameGrab_Arc.FrameGrab_MinWidth) FgArcwidth -= 5; }
		MAINDllOBJECT->FrameGrab_Arc.FrameGrab_CurrentWidth = FgArcwidth;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGARCMH0009", __FILE__, __FUNCSIG__); }
}

//Set the frame settings..
void FrameGrabArc::FG_UpdateFirstFrameppLoad()
{
	try
	{
		FgArcwidth = baseaction->FGWidth;
		WidthDirection = (bool)baseaction->FGdirection;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGARCMH0010", __FILE__, __FUNCSIG__); }
}
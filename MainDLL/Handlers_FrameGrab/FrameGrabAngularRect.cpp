#include "StdAfx.h"
#include "FrameGrabAngularRect.h"
#include "..\Engine\RCadApp.h"

//Constructor..//
FrameGrabAngularRect::FrameGrabAngularRect()
{
	try
	{
		CurrentFGType = RapidEnums::RAPIDFGACTIONTYPE::ANGULARRECTANGLEFRAMEGRAB;
		FgAngRectwidth = MAINDllOBJECT->FrameGrab_AngularRect.FrameGrab_CurrentWidth;
		WidthDirection = MAINDllOBJECT->FGDirection_AngularRect();
		withDirectionScan = MAINDllOBJECT->FGDirection_AngularRect();
		fgselected = false;
		setMaxClicks(2);
		/*MAINDllOBJECT->SetStatusCode("FGAR001");*/
		MAINDllOBJECT->SetStatusCode("FrameGrabAngularRect01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGANGRECTMH0001", __FILE__, __FUNCSIG__); }
}

//Used in partprogram
FrameGrabAngularRect::FrameGrabAngularRect(FramegrabBase* Cfg):FrameGrabHandler(Cfg)
{
	try
	{
		baseaction = Cfg;
		FgAngRectwidth = baseaction->FGWidth;
		WidthDirection = (bool)baseaction->FGdirection;
		fgselected = false;
		setMaxClicks(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGANGRECTMH0002", __FILE__, __FUNCSIG__); }
}

//Destructor...//
FrameGrabAngularRect::~FrameGrabAngularRect()
{
	/*try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGANGRECTMH0003", __FILE__, __FUNCSIG__); }*/
}

//Handle the mouse move......//
void FrameGrabAngularRect::FG_mouseMove()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGANGRECTMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse down event......//
void FrameGrabAngularRect::FG_LmouseDown()
{
	try
	{
		if(getClicksDone() == 1)
			FgAngRectwidth = MAINDllOBJECT->FrameGrab_AngularRect.FrameGrab_CurrentWidth;
		PointsDRO[getClicksDone() - 1].set(getClicksValue(getClicksDone() - 1));
		PointsPixel[getClicksDone() - 1].set(MAINDllOBJECT->getWindow(0).getLastWinMouse().x, MAINDllOBJECT->getWindow(0).getLastWinMouse().y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGANGRECTMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the maximum click..//
void FrameGrabAngularRect::FG_LmaxmouseDown()
{
	try
	{
		baseaction->FGdirection = WidthDirection;
		baseaction->FGWidth = FgAngRectwidth;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGANGRECTMH0006", __FILE__, __FUNCSIG__); }
}

//Change the grab direction..
void FrameGrabAngularRect::FG_ChangeDrirection()
{
	try
	{
		WidthDirection = MAINDllOBJECT->FGDirection_AngularRect();
		WidthDirection = !WidthDirection;
		MAINDllOBJECT->FGDirection_AngularRect(WidthDirection);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGANGRECTMH0007", __FILE__, __FUNCSIG__); }
}

void FrameGrabAngularRect::FG_ChangeScanDirection()
{
	withDirectionScan = !withDirectionScan;
}

//Draw Angular rect framegrab//
void FrameGrabAngularRect::FG_draw(double wupp, int windowno)
{
	try
	{
		double width = FgAngRectwidth * wupp;
		double pts[14] = {0}, point1[2] = {PointsDRO[0].getX(), PointsDRO[0].getY()}, point2[2] = {PointsDRO[1].getX(), PointsDRO[1].getY()};
		if(windowno == 0)
		{
			double ptsInside[14] = {0};
			RMATH2DOBJECT->RectangleFG(&point1[0], &point2[0], width, wupp, &pts[0], WidthDirection);
			double slope = RMATH2DOBJECT->ray_angle(point1, point2);
			double firstInsidePt[2] = { PointsDRO[0].getX(), PointsDRO[0].getY() };
			double secondInsidePt[2] = { PointsDRO[1].getX(), PointsDRO[1].getY() };
			firstInsidePt[0] += wupp * cos(slope); firstInsidePt[1] += wupp * sin(slope);
			secondInsidePt[0] -= wupp * cos(slope); secondInsidePt[1] -= wupp * sin(slope);

			RMATH2DOBJECT->RectangleFG(&firstInsidePt[0], &secondInsidePt[0], width - wupp, wupp, &ptsInside[0], WidthDirection);
			if (fgselected)
				GRAFIX->SetColor_Double(0, 1, 0);
			else
				GRAFIX->SetColor_Double(0.4f, 0.4f, 0.4f);
			GRAFIX->drawRectangle(ptsInside[0], ptsInside[1], ptsInside[2], ptsInside[3], ptsInside[6], ptsInside[7], ptsInside[4], ptsInside[5], true);
			GRAFIX->translateMatrix(0.0, 0.0, 2.0);
			GRAFIX->SetColor_Double(0, 0, 1);
			GRAFIX->drawTriangle(pts[8], pts[9], pts[10], pts[11], pts[12], pts[13], true);
			if(this->drawscanDir)
			{
				GRAFIX->SetColor_Double(0, 1, 0);
			   	RMATH2DOBJECT->RectangleFG(&point1[0], &point2[0], width, wupp, &pts[0], withDirectionScan);
			    double dirrat[3] = {pts[10] - pts[8], pts[11] - pts[9], 0}, dircos[3] = {0}, pts2[6] = {0};
				RMATH3DOBJECT->DirectionCosines(dirrat, dircos);
				double length = RMATH2DOBJECT->Pt2Pt_distance(pts[8], pts[9], pts[10], pts[11]);
				pts2[0] = pts[8] + length * dircos[0];
				pts2[1] = pts[9] + length * dircos[1];
				pts2[2] = pts[10] + length * dircos[0];
				pts2[3] = pts[11] + length * dircos[1];
				pts2[4] = pts[12] + length * dircos[0];
				pts2[5] = pts[13] + length * dircos[1];
				GRAFIX->drawTriangle(pts2[0], pts2[1], pts2[2], pts2[3], pts2[4], pts2[5], true);
				GRAFIX->SetColor_Double(0, 0, 1);
				scandir[0] = pts2[4] - (pts2[0] + pts2[2])/2;
				scandir[1] = pts2[5] - (pts2[1] + pts2[3])/2;
				scandir[2] = 0;
				RMATH3DOBJECT->DirectionCosines(scandir, scandir);
			}
			GRAFIX->drawRectangle(pts[0], pts[1], pts[2], pts[3], pts[6], pts[7], pts[4], pts[5], false);
			GRAFIX->translateMatrix(0.0, 0.0, -2.0);
		}
		else if(windowno == 1)
		{
		   	RMATH2DOBJECT->RectangleFG(&point1[0], &point2[0], width, wupp, &pts[0], false);
			GRAFIX->translateMatrix(0.0, 0.0, 2.0);
			GRAFIX->SetColor_Double(1, 0, 0);		
			GRAFIX->drawRectangle(pts[0], pts[1], pts[2], pts[3], pts[6], pts[7], pts[4], pts[5], false);
			GRAFIX->translateMatrix(0.0, 0.0, -2.0);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGANGRECTMH0008", __FILE__, __FUNCSIG__); }
}

void FrameGrabAngularRect::fgangrect_draw(double wupp, bool selected_fg)
{
	try
	{
		fgselected = selected_fg;
		FG_draw(wupp);
		fgselected = false;
	}
	catch(...) { MAINDllOBJECT->SetAndRaiseErrorMessage("FGANGRECTMH0008", __FILE__, __FUNCSIG__); }
}
//Increase the width of the framegrab...//
void FrameGrabAngularRect::FG_MouseScroll(bool flag, double wupp)
{
	try
	{
		FgAngRectwidth = MAINDllOBJECT->FrameGrab_AngularRect.FrameGrab_CurrentWidth;
		if(flag){if(FgAngRectwidth < MAINDllOBJECT->FrameGrab_AngularRect.FrameGrab_MaxWidth) FgAngRectwidth += 5;}
		else {if(FgAngRectwidth > MAINDllOBJECT->FrameGrab_AngularRect.FrameGrab_MinWidth) FgAngRectwidth -= 5; }
		MAINDllOBJECT->FrameGrab_AngularRect.FrameGrab_CurrentWidth = FgAngRectwidth;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGANGRECTMH0009", __FILE__, __FUNCSIG__); }
}

//Set the frame settings..
void FrameGrabAngularRect::FG_UpdateFirstFrameppLoad()
{
	try
	{
		FgAngRectwidth = baseaction->FGWidth;
		WidthDirection = (bool)baseaction->FGdirection;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGANGRECTMH0010", __FILE__, __FUNCSIG__); }
}
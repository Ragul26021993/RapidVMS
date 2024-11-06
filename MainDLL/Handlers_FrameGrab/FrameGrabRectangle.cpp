#include "StdAfx.h"
#include "FrameGrabRectangle.h"
#include "..\Engine\RCadApp.h"

//Constructor..//
FrameGrabRectangle::FrameGrabRectangle(bool detectalledge)
{
	try
	{
		if(detectalledge)
			CurrentFGType = RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB_ALLEDGES;
		else
			CurrentFGType = RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB;
		RectDirection = MAINDllOBJECT->FGDirection_Rectangle();
		scanDirection = MAINDllOBJECT->FGDirection_Rectangle();
		MAINDllOBJECT->SetStatusCode("FrameGrabRectangle01");
		setMaxClicks(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGRECTMH0001", __FILE__, __FUNCSIG__); }
}

//Used in partprogram
FrameGrabRectangle::FrameGrabRectangle(FramegrabBase* Cfg):FrameGrabHandler(Cfg)
{
	try
	{
		baseaction = Cfg;
		RectDirection = baseaction->FGdirection;
		setMaxClicks(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGRECTMH0002", __FILE__, __FUNCSIG__); }
}

//Destructor...//
FrameGrabRectangle::~FrameGrabRectangle()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGRECTMH0003", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move......//
void FrameGrabRectangle::FG_mouseMove()
{
	try
	{
		if(getClicksDone() > 0 && getClicksDone() < getMaxClicks())
		{
			PointsDRO[getClicksDone()].set(getClicksValue(getClicksDone()));
			PointsPixel[getMaxClicks() - 1].set(MAINDllOBJECT->getWindow(0).getLastWinMouse().x, MAINDllOBJECT->getWindow(0).getLastWinMouse().y,  MAINDllOBJECT->getCurrentDRO().getZ());
		}
		if(getClicksDone() == getMaxClicks() - 1)
			ValidFlag = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGRECTMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse down event......//
void FrameGrabRectangle::FG_LmouseDown()
{
	try
	{
		if(getClicksDone() == 1)
			RectDirection = MAINDllOBJECT->FGDirection_Rectangle();
		PointsDRO[getClicksDone() - 1].set(getClicksValue(getClicksDone() - 1));
		PointsPixel[getClicksDone() - 1].set(MAINDllOBJECT->getWindow(0).getLastWinMouse().x, MAINDllOBJECT->getWindow(0).getLastWinMouse().y,  MAINDllOBJECT->getCurrentDRO().getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGRECTMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the maximum click..//
void FrameGrabRectangle::FG_LmaxmouseDown()
{
	try
	{
		baseaction->FGdirection = RectDirection;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGRECTMH0006", __FILE__, __FUNCSIG__); }
}

//Change the grab direction..
void FrameGrabRectangle::FG_ChangeDrirection()
{
	try
	{
		RectDirection = MAINDllOBJECT->FGDirection_Rectangle();
		if(RectDirection < 3) RectDirection++;
		else RectDirection = 0;
		MAINDllOBJECT->FGDirection_Rectangle(RectDirection);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGRECTMH0007", __FILE__, __FUNCSIG__); }
}

void FrameGrabRectangle::FG_ChangeScanDirection()
{
	if(scanDirection < 3) scanDirection++;
	else scanDirection = 0;
}

//Draw rect framegrab//
void FrameGrabRectangle::FG_draw(double wupp, int windowno)
{
	try
	{
		if(windowno == 0)
		{
			double point1[2] = {PointsDRO[0].getX(), PointsDRO[0].getY()}, point2[2] = {PointsDRO[1].getX(), PointsDRO[1].getY()};
			double pts[6] = {0};
			GRAFIX->SetColor_Double(0.4f, 0.4f, 0.4f);
			GRAFIX->drawRectangle(point1[0] + wupp, point1[1] - wupp , point2[0] - wupp, point2[1] + wupp, true);
			GRAFIX->translateMatrix(0.0, 0.0, 2.0);
			GRAFIX->SetColor_Double(0, 0, 1);
			GRAFIX->drawRectangle(point1[0], point1[1], point2[0], point2[1]);
			RMATH2DOBJECT->RectangleFGDirection(&point1[0], &point2[0], RectDirection, wupp, &pts[0]);
			GRAFIX->drawTriangle(pts[0], pts[1], pts[2], pts[3], pts[4], pts[5], true);
			if(this->drawscanDir)
			{
				GRAFIX->SetColor_Double(0, 1, 0);
				RMATH2DOBJECT->RectangleFGDirection(&point1[0], &point2[0], scanDirection, wupp, &pts[0]);
				double dirrat[3] = {pts[2] - pts[0], pts[3] - pts[1], 0}, dircos[3] = {0}, pts2[6] = {0};
				RMATH3DOBJECT->DirectionCosines(dirrat, dircos);
				double length = RMATH2DOBJECT->Pt2Pt_distance(pts[0], pts[1], pts[2], pts[3]);
				pts2[0] = pts[0] + length * dircos[0];
				pts2[1] = pts[1] + length * dircos[1];
				pts2[2] = pts[2] + length * dircos[0];
				pts2[3] = pts[3] + length * dircos[1];
				pts2[4] = pts[4] + length * dircos[0];
				pts2[5] = pts[5] + length * dircos[1];
				GRAFIX->drawTriangle(pts2[0], pts2[1], pts2[2], pts2[3], pts2[4], pts2[5], true);
				GRAFIX->SetColor_Double(0, 0, 1);
				scandir[0] = pts2[0] - (pts2[4] + pts2[2])/2;
				scandir[1] = pts2[1] - (pts2[5] + pts2[3])/2;
				scandir[2] = 0;
				RMATH3DOBJECT->DirectionCosines(scandir, scandir);
			}
			GRAFIX->translateMatrix(0.0, 0.0, -2.0);
		}
		else if(windowno == 1)
		{
		   GRAFIX->SetColor_Double(1, 0, 0);
		   GRAFIX->drawRectangle(getClicksValue(0).getX(), getClicksValue(0).getY(), getClicksValue(1).getX(), getClicksValue(1).getY());
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGRECTMH0008", __FILE__, __FUNCSIG__); }
}

//Increase the width of the framegrab...//
void FrameGrabRectangle::FG_MouseScroll(bool flag, double wupp)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGRECTMH0009", __FILE__, __FUNCSIG__); }
}

//Set the frame settings..
void FrameGrabRectangle::FG_UpdateFirstFrameppLoad()
{
	try
	{
		RectDirection = baseaction->FGdirection;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGRECTMH0010", __FILE__, __FUNCSIG__); }
}
#include "StdAfx.h"
#include "FrameGrabFixedRectangle.h"
#include "..\Engine\RCadApp.h"

//Constructor..//
FrameGrabFixedRectangle::FrameGrabFixedRectangle()
{
	try
	{
		CurrentFGType = RapidEnums::RAPIDFGACTIONTYPE::FIXEDRECTANGLE;
		FixedWidth = MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_CurrentWidth; 
		FixedHeight = MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_CurrentHeight; 
		RectDirection = MAINDllOBJECT->FGDirection_FixedRectangle();
		MAINDllOBJECT->SetStatusCode("FrameGrabFixedRectangle01");
		setMaxClicks(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGFIXRECTMH0001", __FILE__, __FUNCSIG__); }
}

//Used in partprogram
FrameGrabFixedRectangle::FrameGrabFixedRectangle(FramegrabBase* Cfg):FrameGrabHandler(Cfg)
{
	try
	{
		baseaction = Cfg;
		RectDirection = baseaction->FGdirection;
		double wupp = MAINDllOBJECT->getWindow(0).getUppX();
		FixedWidth = abs(PointsPixel[0].getX() - PointsPixel[1].getX());
		FixedHeight = abs(PointsPixel[0].getY() - PointsPixel[1].getY());
		setMaxClicks(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGFIXRECTMH0002", __FILE__, __FUNCSIG__); }
}

//Destructor...//
FrameGrabFixedRectangle::~FrameGrabFixedRectangle()
{
	try{ 
		FixedWidth = 0;
		FixedHeight = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGFIXRECTMH0003", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move......//
void FrameGrabFixedRectangle::FG_mouseMove()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGFIXRECTMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse down event......//
void FrameGrabFixedRectangle::FG_LmouseDown()
{
	try
	{
		RectDirection = MAINDllOBJECT->FGDirection_FixedRectangle();
		double wupp = MAINDllOBJECT->getWindow(0).getUppX();
		Vector Temp(MAINDllOBJECT->getCurrentDRO());

		int wwidth = MAINDllOBJECT->getWindow(0).getWinDim().x / 2, wheight = MAINDllOBJECT->getWindow(0).getWinDim().y / 2;
		PointsPixel[0].set(wwidth - FixedWidth/2, wheight - FixedHeight/2);
		PointsPixel[1].set(wwidth + FixedWidth/2, wheight + FixedHeight/2);
		
		PointsDRO[0].set(Temp.getX() - FixedWidth * wupp/2,  Temp.getY() + FixedHeight * wupp/2,  MAINDllOBJECT->getCurrentDRO().getZ());
		PointsDRO[1].set(Temp.getX() + FixedWidth * wupp/2,  Temp.getY() - FixedHeight * wupp/2,  MAINDllOBJECT->getCurrentDRO().getZ());
		ValidFlag = true;
		this->mouseMove();
		MAINDllOBJECT->update_VideoGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGFIXRECTMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the maximum click..//
void FrameGrabFixedRectangle::FG_LmaxmouseDown()
{
	try
	{
		baseaction->FGdirection = RectDirection;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGFIXRECTMH0006", __FILE__, __FUNCSIG__); }
}

//Change the grab direction..
void FrameGrabFixedRectangle::FG_ChangeDrirection()
{
	try
	{
		RectDirection = MAINDllOBJECT->FGDirection_FixedRectangle();
		if(RectDirection < 3) RectDirection++;
		else RectDirection = 0;
		MAINDllOBJECT->FGDirection_FixedRectangle(RectDirection);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGFIXRECTMH0007", __FILE__, __FUNCSIG__); }
}

//Draw rect framegrab//
void FrameGrabFixedRectangle::FG_draw(double wupp, int windowno)
{
	try
	{
		Vector Temp(MAINDllOBJECT->getCurrentDRO());
		PointsDRO[0].set(Temp.getX() - FixedWidth * wupp/2,  Temp.getY() + FixedHeight * wupp/2,  MAINDllOBJECT->getCurrentDRO().getZ());
		PointsDRO[1].set(Temp.getX() + FixedWidth * wupp/2,  Temp.getY() - FixedHeight * wupp/2,  MAINDllOBJECT->getCurrentDRO().getZ());
		double point1[2] = {PointsDRO[0].getX(), PointsDRO[0].getY()}, point2[2] = {PointsDRO[1].getX(), PointsDRO[1].getY()};
		double pts[6];
		GRAFIX->SetColor_Double(0.4f, 0.4f, 0.4f);
		GRAFIX->drawRectangle(point1[0] + wupp, point1[1] - wupp , point2[0] - wupp, point2[1] + wupp, true);
		GRAFIX->translateMatrix(0.0, 0.0, 2.0);
		GRAFIX->SetColor_Double(0, 0, 1);
		GRAFIX->drawRectangle(point1[0], point1[1], point2[0], point2[1]);
		RMATH2DOBJECT->RectangleFGDirection(&point1[0], &point2[0], RectDirection, wupp, &pts[0]);
		GRAFIX->drawTriangle(pts[0], pts[1], pts[2], pts[3], pts[4], pts[5], true);
		GRAFIX->translateMatrix(0.0, 0.0, -2.0);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGFIXRECTMH0008", __FILE__, __FUNCSIG__); }
}

//Increase the width of the framegrab...//
void FrameGrabFixedRectangle::FG_MouseScroll(bool flag, double wupp)
{
	try
	{
		int wwidth = MAINDllOBJECT->getWindow(0).getWinDim().x, wheight = MAINDllOBJECT->getWindow(0).getWinDim().y;
		int IncGap = 20;
		if(RectDirection == 0 || RectDirection == 2)
		{
			FixedWidth = MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_CurrentWidth; 
			if(flag){if(FixedWidth < MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_MaxWidth) FixedWidth += IncGap;}
			else {if(FixedWidth > MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_MinWidth) FixedWidth -= IncGap; }
			MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_CurrentWidth = FixedWidth;
		}
		else
		{
			FixedHeight = MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_CurrentHeight; 
			if(flag){if(FixedHeight < MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_MaxHeight) FixedHeight += IncGap;}
			else {if(FixedHeight > MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_MinHeight) FixedHeight -= IncGap; }
			MAINDllOBJECT->FrameGrab_FixedRectangle.FrameGrab_CurrentHeight = FixedHeight;
		}
		Vector Temp = MAINDllOBJECT->getCurrentDRO().getX();

		PointsPixel[0].set(wwidth/2 - FixedWidth/2, wheight/2 - FixedHeight/2);
		PointsPixel[1].set(wwidth/2 + FixedWidth/2, wheight/2 + FixedHeight/2);
		
		PointsDRO[0].set(Temp.getX() - FixedWidth * wupp/2,  Temp.getY() + FixedHeight * wupp/2,  MAINDllOBJECT->getCurrentDRO().getZ());
		PointsDRO[1].set(Temp.getX() + FixedWidth * wupp/2,  Temp.getY() - FixedHeight * wupp/2,  MAINDllOBJECT->getCurrentDRO().getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGFIXRECTMH0009", __FILE__, __FUNCSIG__); }
}

//Set the frame settings..
void FrameGrabFixedRectangle::FG_UpdateFirstFrameppLoad()
{
	try
	{
		RectDirection = baseaction->FGdirection;
		double wupp = MAINDllOBJECT->getWindow(0).getUppX();
		FixedWidth = abs(PointsPixel[0].getX() - PointsPixel[1].getX()); 
		FixedHeight = abs(PointsPixel[0].getY() - PointsPixel[1].getY());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGFIXRECTMH0010", __FILE__, __FUNCSIG__); }
}
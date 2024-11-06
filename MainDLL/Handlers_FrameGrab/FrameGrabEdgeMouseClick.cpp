#include "StdAfx.h"
#include "FrameGrabEdgeMouseClick.h"
#include "..\Engine\RCadApp.h"

//Constructor..//
FrameGrabEdgeMouseClick::FrameGrabEdgeMouseClick()
{
	try
	{
		CurrentFGType = RapidEnums::RAPIDFGACTIONTYPE::EDGE_MOUSECLICK;
		setMaxClicks(1);
		/*MAINDllOBJECT->SetStatusCode("FGAR001");*/
		MAINDllOBJECT->SetStatusCode("FrameGrabEdgeMouseClick01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMCLKMH0001", __FILE__, __FUNCSIG__); }
}

//Used in partprogram
FrameGrabEdgeMouseClick::FrameGrabEdgeMouseClick(FramegrabBase* Cfg):FrameGrabHandler(Cfg)
{
	try
	{
		baseaction = Cfg;
		setMaxClicks(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMCLKMH0002", __FILE__, __FUNCSIG__); }
}

//Destructor...//
FrameGrabEdgeMouseClick::~FrameGrabEdgeMouseClick()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMCLKMH0003", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move......//
void FrameGrabEdgeMouseClick::FG_mouseMove()
{
	try
	{
		if(getClicksDone() < getMaxClicks())
		{
			PointsDRO[getClicksDone()].set(getClicksValue(getClicksDone()));
			PointsPixel[getMaxClicks() - 1].set(MAINDllOBJECT->getWindow(0).getLastWinMouse().x, MAINDllOBJECT->getWindow(0).getLastWinMouse().y);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMCLKMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse down event......//
void FrameGrabEdgeMouseClick::FG_LmouseDown()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMCLKMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the maximum click..//
void FrameGrabEdgeMouseClick::FG_LmaxmouseDown()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMCLKMH0006", __FILE__, __FUNCSIG__); }
}

//Change the grab direction..
void FrameGrabEdgeMouseClick::FG_ChangeDrirection()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMCLKMH0007", __FILE__, __FUNCSIG__); }
}

//Draw Angular rect framegrab//
void FrameGrabEdgeMouseClick::FG_draw(double wupp, int windowno)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMCLKMH0008", __FILE__, __FUNCSIG__); }
}

//Increase the width of the framegrab...//
void FrameGrabEdgeMouseClick::FG_MouseScroll(bool flag, double wupp)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMCLKMH0009", __FILE__, __FUNCSIG__); }
}

//Set the frame settings..
void FrameGrabEdgeMouseClick::FG_UpdateFirstFrameppLoad()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGMCLKMH0010", __FILE__, __FUNCSIG__); }
}
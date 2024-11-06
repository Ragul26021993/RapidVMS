#include "StdAfx.h"
#include "LiveScanModeHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\FrameGrab\FramegrabBase.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Helper\Helper.h"

//Default constuctor..
LiveScanModeHandler::LiveScanModeHandler()
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0001", __FILE__, __FUNCSIG__); }
}

//Constructor with point action as parameter... Used in partprogram..
LiveScanModeHandler::LiveScanModeHandler(FramegrabBase* fb)
{
	try
	{
		init();
		baseaction = fb;
		Rectwidth = baseaction->FGWidth;
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0002", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
LiveScanModeHandler::~LiveScanModeHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0003", __FILE__, __FUNCSIG__); }
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void LiveScanModeHandler::init()
{
	try
	{
		setMaxClicks(1);
		runningPartprogramValid = false;
		Validflag = false;
		Rectwidth = MAINDllOBJECT->FrameGrab_ScanMode.FrameGrab_CurrentWidth;// *HELPEROBJECT->CamSizeRatio;
		/*MAINDllOBJECT->SetStatusCode("PTHDLR0001");*/
		MAINDllOBJECT->SetStatusCode("LiveScanModeHandler01");
		MAINDllOBJECT->CURRENTRAPIDHANDLER = RapidEnums::RAPIDDRAWHANDLERTYPE::RAPIDFRAMEGRAB_HANDLER;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0004", __FILE__, __FUNCSIG__); }
}

//Function to handle the mouse move..
void LiveScanModeHandler::mouseMove()
{
	try
	{
		if(MAINDllOBJECT->getCurrentWindow() == 0)
			MAINDllOBJECT->ToolSelectedMousemove(getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY(), getClicksValue(getClicksDone()).getZ());
		if(!(PPCALCOBJECT->IsPartProgramRunning() && MAINDllOBJECT->IsCNCMode()))
			Validflag = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0005", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void LiveScanModeHandler::LmouseDown()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0006", __FILE__, __FUNCSIG__); }
}

//Escape buttons press handling..
void LiveScanModeHandler::EscapebuttonPress()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0007", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void LiveScanModeHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(windowno != 0) return;
		if(!Validflag) return;
		double wdth = Rectwidth * WPixelWidth/2;
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(MAINDllOBJECT->IsCNCMode())
			{
				GRAFIX->drawFlexibleCrossHair(baseaction->points[0].getX(), baseaction->points[0].getY());
				GRAFIX->drawLiveScanCross(baseaction->points[0].getX(), baseaction->points[0].getY(), wdth, WPixelWidth, false);
			}
			else if(MAINDllOBJECT->MouseOnVideo())
			{
				GRAFIX->drawFlexibleCrossHair(getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY());
				GRAFIX->drawLiveScanCross(getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY(), wdth, WPixelWidth, false);
			}
		}
		else if(MAINDllOBJECT->MouseOnVideo())
		{
			GRAFIX->drawFlexibleCrossHair(getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY());
			GRAFIX->drawLiveScanCross(getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY(), wdth, WPixelWidth, false);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0008", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data..//
void LiveScanModeHandler::PartProgramData()
{
	try
	{
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
		if(MAINDllOBJECT->IsCNCMode())
		{
			int ptr[4] = {baseaction->myPosition[0].getX(), baseaction->myPosition[0].getY(), Rectwidth, Rectwidth};
			double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y, upp = MAINDllOBJECT->getWindow(0).getUppX();
			baseaction->points[0].set(cx + (ptr[0] + Rectwidth/2) * upp, cy - (ptr[1] + Rectwidth/2) * upp);
			Validflag = true;
			MAINDllOBJECT->update_VideoGraphics();
			Sleep(300);
			MAINDllOBJECT->SelectScanPoint(&ptr[0], RapidEnums::RAPIDFGACTIONTYPE::LIVESCANCROSSHAIR, true);
			PPCALCOBJECT->partProgramReached();
			Validflag = false;
			return;
		}
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0009", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void LiveScanModeHandler::LmaxmouseDown()
{
	try
	{
		//int rw = Rectwidth * HELPEROBJECT->CamSizeRatio;
		//Vector* pt = new Vector(L_mouseDownPixel().getX() * HELPEROBJECT->CamSizeRatio, L_mouseDownPixel().getY() * HELPEROBJECT->CamSizeRatio);
		Vector pt = L_mouseDownPixel();
		int ptr[4] = {pt.getX() - Rectwidth / 2, pt.getY() - Rectwidth / 2, Rectwidth, Rectwidth };
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				MAINDllOBJECT->SelectScanPoint(&ptr[0], RapidEnums::RAPIDFGACTIONTYPE::LIVESCANCROSSHAIR, true);
				PPCALCOBJECT->partProgramReached();
				return;
			}
		}
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
		if(CurrentShape == NULL)
		{
			MAINDllOBJECT->SetStatusCode("PTHDLR0003");
			return;
		}
		MAINDllOBJECT->SelectScanPoint(&ptr[0], RapidEnums::RAPIDFGACTIONTYPE::LIVESCANCROSSHAIR, false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0010", __FILE__, __FUNCSIG__); }
}


void LiveScanModeHandler::MouseScroll(bool flag)
{
	try
	{
		Rectwidth = MAINDllOBJECT->FrameGrab_ScanMode.FrameGrab_CurrentWidth;
		if(flag){if(Rectwidth < MAINDllOBJECT->FrameGrab_ScanMode.FrameGrab_MaxWidth) Rectwidth += 5;}
		else {if(Rectwidth > MAINDllOBJECT->FrameGrab_ScanMode.FrameGrab_MinWidth) Rectwidth -= 5; }
		MAINDllOBJECT->FrameGrab_ScanMode.FrameGrab_CurrentWidth = Rectwidth;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SCANMH0011", __FILE__, __FUNCSIG__); }
}
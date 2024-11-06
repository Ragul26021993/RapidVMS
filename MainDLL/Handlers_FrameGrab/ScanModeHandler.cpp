#include "StdAfx.h"
#include "ScanModeHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\FrameGrab\FramegrabBase.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Helper\Helper.h"

//Default constuctor..
ScanModeHandler::ScanModeHandler()
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SCANMH0001", __FILE__, __FUNCSIG__); }
}

//Constructor with point action as parameter... Used in partprogram..
ScanModeHandler::ScanModeHandler(FramegrabBase* fb)
{
	try
	{
		init();
		baseaction = fb;
		Rectwidth = baseaction->FGWidth;
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SCANMH0002", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
ScanModeHandler::~ScanModeHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SCANMH0003", __FILE__, __FUNCSIG__); }
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void ScanModeHandler::init()
{
	try
	{
		setMaxClicks(1);
		runningPartprogramValid = false;
		Rectwidth = MAINDllOBJECT->FrameGrab_ScanMode.FrameGrab_CurrentWidth;// *HELPEROBJECT->CamSizeRatio;
		/*MAINDllOBJECT->SetStatusCode("PTHDLR0001");*/
		MAINDllOBJECT->SetStatusCode("ScanModeHandler01");
		MAINDllOBJECT->CURRENTRAPIDHANDLER = RapidEnums::RAPIDDRAWHANDLERTYPE::RAPIDFRAMEGRAB_HANDLER;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SCANMH0004", __FILE__, __FUNCSIG__); }
}

//Function to handle the mouse move..
void ScanModeHandler::mouseMove()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SCANMH0005", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void ScanModeHandler::LmouseDown()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SCANMH0006", __FILE__, __FUNCSIG__); }
}

//Escape buttons press handling..
void ScanModeHandler::EscapebuttonPress()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SCANMH0007", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void ScanModeHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(windowno == 0)
		{
			double wdth = Rectwidth * WPixelWidth/2;
			GRAFIX->drawScanMode(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), wdth, WPixelWidth, MAINDllOBJECT->HatchedCrossHairMode(), false);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SCANMH0008", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data..//
void ScanModeHandler::PartProgramData()
{
	try
	{
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
		if(MAINDllOBJECT->IsCNCMode())
		{
			MAINDllOBJECT->update_VideoGraphics();
			Sleep(300);
			int ptr[4] = { MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).getWinDim().x / 2 - Rectwidth/2, 
						   MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).getWinDim().y / 2 - Rectwidth/2, Rectwidth, Rectwidth};
			MAINDllOBJECT->SelectScanPoint(&ptr[0], RapidEnums::RAPIDFGACTIONTYPE::SCANCROSSHAIR, true);
			PPCALCOBJECT->partProgramReached();
			return;
		}
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SCANMH0009", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void ScanModeHandler::LmaxmouseDown()
{
	try
	{
		//int rw = Rectwidth * HELPEROBJECT->CamSizeRatio;
		int ptr[4] = {MAINDllOBJECT->getWindow( MAINDllOBJECT->getCurrentWindow()).getWinDim().x / 2 - Rectwidth / 2, 
					  MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).getWinDim().y / 2 - Rectwidth / 2, Rectwidth, Rectwidth };
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				MAINDllOBJECT->SelectScanPoint(&ptr[0], RapidEnums::RAPIDFGACTIONTYPE::SCANCROSSHAIR, true);
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
		MAINDllOBJECT->SelectScanPoint(&ptr[0], RapidEnums::RAPIDFGACTIONTYPE::SCANCROSSHAIR, false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SCANMH0010", __FILE__, __FUNCSIG__); }
}

void ScanModeHandler::MouseScroll(bool flag)
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
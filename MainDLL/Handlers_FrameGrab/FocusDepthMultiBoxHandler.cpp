 #include "StdAfx.h"
#include "FocusDepthMultiBoxHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\FocusFunctions.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Helper\Helper.h"

//Default constuctor..
FocusDepthMultiBoxHandler::FocusDepthMultiBoxHandler(bool TwoBoxMode)
{
	try
	{
		init();
		TwoBoxDepth_Mode = TwoBoxMode;
		setMaxClicks(8);
		if(TwoBoxMode)
			MAINDllOBJECT->SetStatusCode("FocusDepthMultiBoxHandler01");
		else
			MAINDllOBJECT->SetStatusCode("FocusDepthMultiBoxHandler02");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0001", __FILE__, __FUNCSIG__); }
}

//Constructor with point action as parameter... Used in partprogram..
FocusDepthMultiBoxHandler::FocusDepthMultiBoxHandler(FramegrabBase* fb)
{
	try
	{
		init();
		baseaction = fb;
		Rectwidth = baseaction->FGWidth;
		if(baseaction->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_2BOX)
			TwoBoxDepth_Mode = true;
		else
			TwoBoxDepth_Mode = false;
		setMaxClicks(8);
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0002", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
FocusDepthMultiBoxHandler::~FocusDepthMultiBoxHandler()
{
	try
	{
		Validflag = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0003", __FILE__, __FUNCSIG__); }
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void FocusDepthMultiBoxHandler::init()
{
	try
	{
		MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_NULL);
		ClickCount = 0;
		resetClicks();
		runningPartprogramValid = false;
		FocusDepthStarted = false;
		Validflag = false;
		Rectwidth = MAINDllOBJECT->FrameGrab_FocusDepthMultiBox.FrameGrab_CurrentWidth;
		MAINDllOBJECT->SetStatusCode("PTHDLR0001");
		MAINDllOBJECT->CURRENTRAPIDHANDLER = RapidEnums::RAPIDDRAWHANDLERTYPE::RAPIDFRAMEGRAB_HANDLER;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0004", __FILE__, __FUNCSIG__); }
}

//Function to handle the mouse move..
void FocusDepthMultiBoxHandler::mouseMove()
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
void FocusDepthMultiBoxHandler::LmouseDown()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(MAINDllOBJECT->getCurrentWindow() != 0)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(TwoBoxDepth_Mode)
		{
			if(getClicksDone() == 2)
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
			if(getClicksDone() == 3)
			{
				FOCUSCALCOBJECT->CalculateDepthRectangle(&RectIntArray[0], 2);
				FOCUSCALCOBJECT->setRowColumn(2, 1, 40, 0, 0);
				FOCUSCALCOBJECT->StarFocusDepth_BoxDepth(true);
				FocusDepthStarted = true;
				return;
			}
			else if(getClicksDone() > 3)
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
		}
		else
		{
			if(getClicksDone() == 4)
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
			if(getClicksDone() == 5)
			{
				FOCUSCALCOBJECT->CalculateDepthRectangle(&RectIntArray[0], 4);
				FOCUSCALCOBJECT->setRowColumn(4, 1, 40, 0, 0);
				FOCUSCALCOBJECT->StarFocusDepth_BoxDepth(true);
				FocusDepthStarted = true;
				return;
			}
			else if(getClicksDone() > 5)
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
		}
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y, upp = MAINDllOBJECT->getWindow(0).getUppX() / HELPEROBJECT->CamSizeRatio;
		ClickCount = getClicksDone();
		int cc = ClickCount - 1;
		Vector pt = L_mouseDownPixel();
		RectIntArray[4 * cc] =  pt.getX() * HELPEROBJECT->CamSizeRatio - Rectwidth/2;
		RectIntArray[4 * cc + 1] =  pt.getY() * HELPEROBJECT->CamSizeRatio - Rectwidth/2;
		RectIntArray[4 * cc + 2] =  Rectwidth; 
		RectIntArray[4 * cc + 3] =  Rectwidth;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0006", __FILE__, __FUNCSIG__); }
}

bool FocusDepthMultiBoxHandler::FocusButtonClicked()
{
	try
	{
		if(TwoBoxDepth_Mode)
		{
			if(getClicksDone() == 2)
			{	
				FOCUSCALCOBJECT->CalculateDepthRectangle(&RectIntArray[0], 2);
				FOCUSCALCOBJECT->setRowColumn(2, 1, 40, 0, 0);
				setClicksDone(3);
				FocusDepthStarted = true;
				return true;
			}
			else 
				return false;
		}
		else
		{
			if(getClicksDone() == 4)
			{
				FOCUSCALCOBJECT->CalculateDepthRectangle(&RectIntArray[0], 4);
				FOCUSCALCOBJECT->setRowColumn(4, 1, 40, 0, 0);
				setClicksDone(5);
				FocusDepthStarted = true;
				return true;
			}
			else
				return false;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0007", __FILE__, __FUNCSIG__); return false;}
}

//Escape buttons press handling..
void FocusDepthMultiBoxHandler::EscapebuttonPress()
{
	try
	{
		if(FocusDepthStarted)
			FOCUSCALCOBJECT->StarFocusDepth_BoxDepth(false);
		init();
		MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_DEFAULT);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0007", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void FocusDepthMultiBoxHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(windowno != 0) return;
		if(!Validflag) return;
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y, upp = MAINDllOBJECT->getWindow(0).getUppX() / HELPEROBJECT->CamSizeRatio;
		double wdth = Rectwidth * WPixelWidth/2;
		if(MAINDllOBJECT->MouseOnVideo() && !PPCALCOBJECT->IsPartProgramRunning() && !PPCALCOBJECT->PartprogramLoaded())
		{
			if(TwoBoxDepth_Mode)
			{
				if(getClicksDone() < 2)
					GRAFIX->drawMultiScanCross(getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY(), wdth, WPixelWidth);
			}
			else
			{
				if(getClicksDone() < 4)
					GRAFIX->drawMultiScanCross(getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY(), wdth, WPixelWidth);
			}
		}
		for(int i = 0; i < ClickCount; i++)
		{
			RectDroArray[4 * i] = cx + (RectIntArray[4 * i] + RectIntArray[4 * i + 2]/2) * upp;
			RectDroArray[4 * i + 1] = cy - (RectIntArray[4 * i + 1] + RectIntArray[4 * i + 2]/2) * upp;
			RectDroArray[4 * i + 2] = RectIntArray[4 * i + 2] * upp/2;
			RectDroArray[4 * i + 3] = RectIntArray[4 * i + 2] * upp/2;
		}
		if(getClicksDone() > 0)
		{
			for(int i = 0; i < ClickCount; i++)
				GRAFIX->drawMultiScanCross(RectDroArray[4 * i], RectDroArray[4 * i + 1], RectDroArray[4 * i + 2], WPixelWidth);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0008", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data..//
void FocusDepthMultiBoxHandler::PartProgramData()
{
	try
	{
		baseaction = PPCALCOBJECT->getFrameGrab();
		baseaction->myPosition[0].FillDoublePointer_Int(&RectIntArray[0], 4);
		baseaction->myPosition[1].FillDoublePointer_Int(&RectIntArray[4], 4);
		baseaction->myPosition[2].FillDoublePointer_Int(&RectIntArray[8], 4);
		baseaction->myPosition[3].FillDoublePointer_Int(&RectIntArray[12], 4);
		if(baseaction->FGtype == RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_2BOX)
		{
			FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::TWOBOXDEPTH_MODE;
			TwoBoxDepth_Mode = true;
			FOCUSCALCOBJECT->CalculateDepthRectangle(&RectIntArray[0], 2);
			FOCUSCALCOBJECT->setRowColumn(2, 1, 40, 0, 0);
			ClickCount = 2;
			setClicksDone(3);
		}
		else
		{
			FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::FOURBOXDEPTH_MODE;
			TwoBoxDepth_Mode = false;
			FOCUSCALCOBJECT->CalculateDepthRectangle(&RectIntArray[0], 4);
			FOCUSCALCOBJECT->setRowColumn(4, 1, 40, 0, 0);
			ClickCount = 4;
			setClicksDone(5);
		}
		Validflag = true;
		MAINDllOBJECT->update_VideoGraphics();
		Sleep(100);
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0009", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void FocusDepthMultiBoxHandler::LmaxmouseDown()
{
	try
	{
		setClicksDone(getMaxClicks() - 1);
		return;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LSCANMH0010", __FILE__, __FUNCSIG__); }
}


void FocusDepthMultiBoxHandler::MouseScroll(bool flag)
{
	try
	{
		Rectwidth = MAINDllOBJECT->FrameGrab_FocusDepthMultiBox.FrameGrab_CurrentWidth;
		if(flag){if(Rectwidth < MAINDllOBJECT->FrameGrab_FocusDepthMultiBox.FrameGrab_MaxWidth) Rectwidth += 5;}
		else {if(Rectwidth > MAINDllOBJECT->FrameGrab_FocusDepthMultiBox.FrameGrab_MinWidth) Rectwidth -= 5; }
		MAINDllOBJECT->FrameGrab_FocusDepthMultiBox.FrameGrab_CurrentWidth = Rectwidth;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SCANMH0011", __FILE__, __FUNCSIG__); }
}
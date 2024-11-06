#include "StdAfx.h"
#include "CircleInvoluteHandler.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Shapes\CircleInvolute.h"

CircleInvoluteHandler::CircleInvoluteHandler()
{
	try
	{
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIVH001", __FILE__, __FUNCSIG__); }
}

CircleInvoluteHandler::CircleInvoluteHandler(ShapeWithList* sh, bool undoFlag)
{
	try
	{
		MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CIRCLEINVOLUTE_HANDLER;
		myshape = sh;
		if(undoFlag)
		{
			UndoFlag = true; RedoFlag = false;
		}
		else
		{
			UndoFlag = false; RedoFlag = true;
		}
		setClicksDone(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIVH002", __FILE__, __FUNCSIG__); }
}

CircleInvoluteHandler::~CircleInvoluteHandler()
{
}

void CircleInvoluteHandler::init()
{
	try
	{
		MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::CIRCLEINVOLUTE_HANDLER;
		myshape = NULL;
		runningPartprogramValid = false;
		FirstClick = true; MouseMoveFlag = false;
		LastPt[0] = 0; LastPt[1] = 0; LastPt[2] = 0;
		NumberOfPts = 6;
		Pflag = false;
		UndoFlag = false;
		RedoFlag = false;
		setClicksDone(0);
		setMaxClicks(10);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIVH003", __FILE__, __FUNCSIG__); }
}

void CircleInvoluteHandler::mouseMove()
{
	try
	{
		if(UndoFlag || RedoFlag)
		{
			UndoFlag = false; RedoFlag = false;
			//UpdateControlPointList();
		}
		Vector* pt = &getClicksValue(getClicksDone());
		if(MAINDllOBJECT->getCurrentWindow() == 0)
			MAINDllOBJECT->ToolSelectedMousemove(pt->getX(), pt->getY(), pt->getZ());
		if(getClicksDone() == 1)
		{
			LastPt[0] = pt->getX(); LastPt[1] = pt->getY(); LastPt[2] = pt->getZ();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIVH004", __FILE__, __FUNCSIG__); }
}

void CircleInvoluteHandler::LmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone() - 1);		
		if(getClicksDone() == 1 && FirstClick)
		{
			MouseMoveFlag = true;
			FirstClick = false;
			myshape = new CircleInvolute(_T("S"));
			((CircleInvolute*)myshape)->DrawCircleInvolute = false;
			CurrentShape = myshape;
			AddShapeAction::addShape(myshape);
		}		
		if(myshape != NULL)
		{			
			setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
			AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);		
			//UpdateControlPointList();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIVH005", __FILE__, __FUNCSIG__); }
}

void CircleInvoluteHandler::RmouseDown(double x, double y)
{
}

void CircleInvoluteHandler::LmaxmouseDown()
{
}

void CircleInvoluteHandler::draw(int windowno, double WPixelWidth)
{

}

void CircleInvoluteHandler::EscapebuttonPress()
{
}

void CircleInvoluteHandler::PartProgramData()
{
}

void CircleInvoluteHandler::SetAnyData(double *data)
{
}

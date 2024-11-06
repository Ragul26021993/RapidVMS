#include "StdAfx.h"
#include "PointHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\RPoint.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor.....//
PointHandler::PointHandler(bool shapeas2d)
{	
	try
	{
		TwoDPoint = shapeas2d;
		setMaxClicks(1);
		AddNewPoint();
		runningPartprogramValid = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0001", __FILE__, __FUNCSIG__); }
}

PointHandler::PointHandler()
{	
	try
	{
		TwoDPoint = true;
		setMaxClicks(1);
		runningPartprogramValid = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0001a", __FILE__, __FUNCSIG__); }
}

PointHandler::~PointHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0003", __FILE__, __FUNCSIG__); }
}

void PointHandler::AddNewPoint()
{
	try
	{
		runningPartprogramValid = false;
		firstclick = true;
		if(TwoDPoint)
		{
			CurrentShape = new RPoint(_T("P"), RapidEnums::SHAPETYPE::RPOINT);
			((RPoint*)CurrentShape)->PointType = RPoint::RPOINTTYPE::POINT;
			MAINDllOBJECT->SetStatusCode("PointHandler01");
		}
		else
		{
			CurrentShape = new RPoint(_T("P3D"), RapidEnums::SHAPETYPE::RPOINT3D);
			((RPoint*)CurrentShape)->PointType = RPoint::RPOINTTYPE::POINT3D;
			MAINDllOBJECT->SetStatusCode("PointHandler02");
		}
		AddShapeAction::addShape(CurrentShape);
		/*MAINDllOBJECT->SetStatusCode("P001");*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0003", __FILE__, __FUNCSIG__); }
}

//Handle mousedown..//
void PointHandler::LmouseDown()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0004", __FILE__, __FUNCSIG__); }
}


void PointHandler::RmouseDown(double x, double y)
{
	try
	{
		if(!PPCALCOBJECT->IsPartProgramRunning())
		{
			if(getClicksDone() == 0)
				AddNewPoint();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move...//
void PointHandler::mouseMove()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone());
		if(MAINDllOBJECT->getCurrentWindow() == 0)
			MAINDllOBJECT->ToolSelectedMousemove(pt->getX(), pt->getY(),pt->getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0006", __FILE__, __FUNCSIG__); }
}	

//Draw the mid point..
void PointHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		drawFlexibleCh();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0007", __FILE__, __FUNCSIG__); }
}

void PointHandler::EscapebuttonPress()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0008", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data...//
void PointHandler::PartProgramData()
{
	try
	{
		CurrentShape = (ShapeWithList*)(MAINDllOBJECT->getShapesList().selectedItem());
		baseaction = PPCALCOBJECT->getFrameGrab();
		if(baseaction->CurrentWindowNo == 1)
		{
			if(baseaction->Pointer_SnapPt != NULL)
				setBaseRProperty(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
			else
				setBaseRProperty(0, 0, 0);
			((ShapeWithList*)CurrentShape)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
			PPCALCOBJECT->partProgramReached();
			return;
		}
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0009", __FILE__, __FUNCSIG__); }
}

//max mouse down....//
void PointHandler::LmaxmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(0);
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
				((ShapeWithList*)CurrentShape)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
				PPCALCOBJECT->partProgramReached();
			}
			else
				setClicksDone(getMaxClicks() - 1);
			return;
		}

		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		if(!firstclick)
		{
			if(TwoDPoint)
			{
				CurrentShape = new RPoint(_T("P"), RapidEnums::SHAPETYPE::RPOINT);
				((RPoint*)CurrentShape)->PointType = RPoint::RPOINTTYPE::POINT;
			}
			else
			{
				CurrentShape = new RPoint(_T("P3D"), RapidEnums::SHAPETYPE::RPOINT3D);
				((RPoint*)CurrentShape)->PointType = RPoint::RPOINTTYPE::POINT3D;
			}
			AddShapeAction::addShape(CurrentShape);
		}
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
		firstclick = false;
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0010", __FILE__, __FUNCSIG__); }
}
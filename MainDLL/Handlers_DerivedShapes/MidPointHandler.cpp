#include "StdAfx.h"
#include "MidPointHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\RPoint.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor for midpoint,nearest point and farthest point....//
MidPointHandler::MidPointHandler()
{
	try
	{
		runningPartprogramValid = false;
		setMaxClicks(2);
		MAINDllOBJECT->SetStatusCode("MidPointHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MIDPTMH0001", __FILE__, __FUNCSIG__); }

}
MidPointHandler::~MidPointHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MIDPTMH0002", __FILE__, __FUNCSIG__); }
}

void MidPointHandler::init()
{
	try
	{
		FirstPoint_Related = false; SecondPoint_Related = false;
		runningPartprogramValid = false;
		resetClicks();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MIDPTMH0003", __FILE__, __FUNCSIG__); }
}

//Handle mousedown..//
void MidPointHandler::LmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone() - 1);
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
				setClicksDone(getClicksDone() - 1);
			return;
		}
		if(MAINDllOBJECT->highlightedPoint() != NULL)
		{
			Point1.set(*MAINDllOBJECT->highlightedPoint());
			FirstPoint_Related = true;
		}
		else
			Point1.set(*pt);
		MAINDllOBJECT->DerivedShapeCallback();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MIDPTMH0004", __FILE__, __FUNCSIG__); }
}


//Handle the mouse move...//
void MidPointHandler::mouseMove()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone());
		if(getClicksDone() == getMaxClicks() - 1)
			MidPoint.set((getClicksValue(0).getX() + pt->getX())/2, (getClicksValue(0).getY() + pt->getY())/2,0);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0006", __FILE__, __FUNCSIG__); }
}	

//Draw the mid point..
void MidPointHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1)
			GRAFIX->drawPoint(MidPoint.getX(), MidPoint.getY(), 0, 8);
		GRAFIX->SetColor_Double(0, 1, 0);
		if(getClicksDone() > 0)
			GRAFIX->drawPoint(Point1.getX(),Point1.getY(), 0 , 8);
		if(getClicksDone() > 1)
			GRAFIX->drawPoint(Point2.getX(),Point2.getY(), 0 , 8);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0007", __FILE__, __FUNCSIG__); }
}

void MidPointHandler::EscapebuttonPress()
{
	try
	{
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0008", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data...//
void MidPointHandler::PartProgramData()
{
	try
	{
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
		CurrentShape->IsValid(false);
		double mp[2] = {0};
		baseaction = PPCALCOBJECT->getFrameGrab();
		if(baseaction->CurrentWindowNo == 1)
		{
			if(baseaction->Pointer_SnapPt != NULL)
				setBaseRProperty(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
			else
				setBaseRProperty(mp[0], mp[1], 0);
			((ShapeWithList*)CurrentShape)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
			setClicksDone(0);
			PPCALCOBJECT->partProgramReached();
			return;
		}
		setClicksDone(((ShapeWithList*)CurrentShape)->PointsList->Pointscount());
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0009", __FILE__, __FUNCSIG__); }
}

//max mouse down....//
void MidPointHandler::LmaxmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getMaxClicks() - 1);
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
		
		if(MAINDllOBJECT->highlightedPoint() != NULL)
		{
			Point2.set(*MAINDllOBJECT->highlightedPoint());
			SecondPoint_Related = true;
		}
		else
			Point2.set(*pt);

		CurrentShape = new RPoint(_T("P"), RapidEnums::SHAPETYPE::RPOINT);
		((RPoint*)CurrentShape)->PointType = RPoint::RPOINTTYPE::MIDPOINT;
		AddShapeAction::addShape(CurrentShape);

		if(FirstPoint_Related)
			MAINDllOBJECT->SetSnapPointHighlighted(Point1.getX(), Point1.getY(), Point1.getZ());
		setBaseRProperty(Point1.getX(), Point1.getY(), Point1.getZ());
		MAINDllOBJECT->SnapPointHighlighted(false);
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);

		if(SecondPoint_Related)
			MAINDllOBJECT->SetSnapPointHighlighted(Point2.getX(), Point2.getY(), Point2.getZ());
		setBaseRProperty(Point2.getX(), Point2.getY(), Point2.getZ());
		MAINDllOBJECT->SnapPointHighlighted(false);
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
		
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0010", __FILE__, __FUNCSIG__); }
}
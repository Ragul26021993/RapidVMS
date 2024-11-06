#include "StdAfx.h"
#include "PointOnCircle.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor..//
PointOnCircle::PointOnCircle()
{
	try
	{
		init();
		MAINDllOBJECT->SetStatusCode("PointOnCircle01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTSHMH0002", __FILE__, __FUNCSIG__); }
}

//Destructor..//
PointOnCircle::~PointOnCircle()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTSHMH0003", __FILE__, __FUNCSIG__); }
}

void PointOnCircle::ResetShapeHighlighted()
{
	try
	{
		if(ParentShape != NULL)
		{
			ParentShape->HighlightedForMeasurement(false);
			MAINDllOBJECT->Shape_Updated();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTSHMH0003", __FILE__, __FUNCSIG__); }
}


//Initialise Default settings..
void PointOnCircle::init()
{
	try
	{
		resetClicks();
		setMaxClicks(2);
		runningPartprogramValid = false;
		ParentShape = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTSHMH0003", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move..//
void PointOnCircle::mouseMove()
{
	try
	{
		double mp[2] = {getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY()};
		if(getClicksDone() == 1)
			RMATH2DOBJECT->Point_onCircle(&center[0], radius, &mp[0], &point[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTSHMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mousedown event.....//
void PointOnCircle::LmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone() - 1);
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if(Csh->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || Csh->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			ParentShape = (Circle*)Csh;
			ParentShape->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
		}
		else //Wrong proceedure...
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		MAINDllOBJECT->DerivedShapeCallback();
		ParentShape->getCenter()->FillDoublePointer(&center[0]);
		radius = ParentShape->Radius();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTSHMH0005", __FILE__, __FUNCSIG__); }
}

//Draw the point...//
void PointOnCircle::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() == 1)
			GRAFIX->drawPoint(point[0], point[1], 0, 5);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTSHMH0006", __FILE__, __FUNCSIG__); }
}

void PointOnCircle::EscapebuttonPress()
{
	try
	{
		ResetShapeHighlighted();
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTSHMH0007", __FILE__, __FUNCSIG__); }
}

//Max mouse click...//
void PointOnCircle::LmaxmouseDown()
{
	try
	{
		//Part program handling............
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				setBaseRProperty(point[0],point[1], MAINDllOBJECT->getCurrentDRO().getZ());
				((ShapeWithList*)CurrentShape)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
				PPCALCOBJECT->partProgramReached();
			}
			else
				setClicksDone(getMaxClicks() - 1);
			return;
		}	
		CurrentShape = new RPoint();
		setBaseRProperty(point[0],point[1], MAINDllOBJECT->getCurrentDRO().getZ());
		((RPoint*)CurrentShape)->PointType = RPoint::RPOINTTYPE::POINTONCIRCLE;
		AddShapeAction::addShape(CurrentShape);
		ParentShape->addChild(CurrentShape);
		CurrentShape->addParent(ParentShape);
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
		ResetShapeHighlighted();
		init();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTSHMH0010", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data..//
void PointOnCircle::PartProgramData()
{
	try
	{
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
		CurrentShape->IsValid(false);
		
		list<BaseItem*>::iterator ptr = CurrentShape->getParents().end(); 
		ptr--;
		ParentShape = (Circle*)(*ptr);
		ParentShape->getCenter()->FillDoublePointer(&center[0]);
		radius = ParentShape->Radius();
		ParentShape->HighlightedForMeasurement(true);
		MAINDllOBJECT->Shape_Updated();
		double mp[2];
		((RPoint*)CurrentShape)->getPosition()->FillDoublePointer(&mp[0]);

		baseaction = PPCALCOBJECT->getFrameGrab();
		if(baseaction->CurrentWindowNo == 1)
		{
			if(baseaction->Pointer_SnapPt != NULL)
				setBaseRProperty(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
			else
				setBaseRProperty(mp[0], mp[1], MAINDllOBJECT->getCurrentDRO().getZ());
			((ShapeWithList*)CurrentShape)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
			setClicksDone(0);
			PPCALCOBJECT->partProgramReached();
			return;
		}
		setClicksDone(getMaxClicks() - 1);
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTSHMH0011", __FILE__, __FUNCSIG__); }
}
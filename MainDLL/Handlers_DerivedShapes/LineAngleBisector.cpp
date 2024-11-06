#include "StdAfx.h"
#include "LineAngleBisector.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor..//
LineAngleBisector::LineAngleBisector()
{
	try
	{ 
		init(); 
		MAINDllOBJECT->SetStatusCode("LineAngleBisector01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
LineAngleBisector::~LineAngleBisector()
{
	try
	{
		ResetShapeHighlighted(); 
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0002", __FILE__, __FUNCSIG__); }
}

void LineAngleBisector::ResetShapeHighlighted()
{
	try
	{
		if(ParentShape1 != NULL)
			ParentShape1->HighlightedForMeasurement(false);
		if(ParentShape2 != NULL)
			ParentShape2->HighlightedForMeasurement(false);
		MAINDllOBJECT->Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0002", __FILE__, __FUNCSIG__); }
}

//Initialise Default settings..
void LineAngleBisector::init()
{
	try
	{
		setMaxClicks(3);
		LineLength = 1;
		LengthDefined = false;
		linepos = 0;
		runningPartprogramValid = false;
		ParentShape1 = NULL; ParentShape2 = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0003", __FILE__, __FUNCSIG__); }
}

//Handling Left mousedown...//
void LineAngleBisector::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if(Csh->ShapeType == RapidEnums::SHAPETYPE::LINE || Csh->ShapeType == RapidEnums::SHAPETYPE::XLINE)
		{
			if(getClicksDone() == 1)
				ParentShape1 = (Line*)Csh;
			else
				ParentShape2 = (Line*)Csh;
			Csh->HighlightedForMeasurement(true);
			if(getClicksDone() == 2)
			{
				MAINDllOBJECT->ClearShapeSelections();
				getLinedimensions();
				MAINDllOBJECT->DerivedShapeCallback();
			}
			else
				MAINDllOBJECT->Shape_Updated();
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		MAINDllOBJECT->DerivedShapeCallback();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move..//
void LineAngleBisector::mouseMove()
{
	try
	{
		double mp[2] = {getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY()};
		if(getClicksDone() == getMaxClicks() - 1)
			RMATH2DOBJECT->Angle_bisector(angle1, intercept1, angle2, intercept2, &mp[0], LineLength, &point1[0], &point2[0], linepos);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0005", __FILE__, __FUNCSIG__); }
}

//draw perpendicular line..
void LineAngleBisector::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1)
			GRAFIX->drawLine(point1[0], point1[1], point2[0], point2[1]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0006", __FILE__, __FUNCSIG__); }
}

///Handle the mouse scroll... calculate the line end points for change in length...//
void LineAngleBisector::MouseScroll(bool flag)
{
	try
	{
		if(LengthDefined) return;
		if(flag) LineLength += 0.15;
		else {if(LineLength > 0.5) LineLength -= 0.15; }
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0008", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data...//
void LineAngleBisector::PartProgramData()
{
	try
	{
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
		CurrentShape->IsValid(false);

		list<BaseItem*>::iterator ptr = CurrentShape->getParents().end(); 
		ptr--;
		ParentShape1 = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr--)->getId()];
		ParentShape2 = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()];
		getLinedimensions();

		LineLength = ((Line*)CurrentShape)->Length();
		linepos = ((Line*)CurrentShape)->LinePosition();
		//double mp[2] = {((Line*)CurrentShape)->getMidPoint()->getX(), ((Line*)CurrentShape)->getMidPoint()->getX()};

		double mp[2] = {((Line*)CurrentShape)->getMidPoint()->getX(), ((Line*)CurrentShape)->getMidPoint()->getY()};
		
		baseaction = PPCALCOBJECT->getFrameGrab();
		if(PPCALCOBJECT->getFrameGrab()->CurrentWindowNo == 1)
		{
			if(baseaction->Pointer_SnapPt != NULL)
				setBaseRProperty(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
			else
				setBaseRProperty(mp[0], mp[1], MAINDllOBJECT->getCurrentDRO().getZ());
			((ShapeWithList*)CurrentShape)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
			setClicksDone(0);
			return;
		}
		setClicksDone(getMaxClicks() - 1);
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0010", __FILE__, __FUNCSIG__); }
}

//Reset the current handler settings.,
void LineAngleBisector::EscapebuttonPress()
{
	try
	{
		ResetShapeHighlighted();
		init(); 
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0011", __FILE__, __FUNCSIG__); }
}

void LineAngleBisector::getLinedimensions()
{
	try
	{
		angle1 = ParentShape1->Angle();
		intercept1 = ParentShape1->Intercept();
		angle2 = ParentShape2->Angle();
		intercept2 = ParentShape2->Intercept();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC0005", __FILE__, __FUNCSIG__); }
}

//Handle the maximum click...//
void LineAngleBisector::LmaxmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getMaxClicks() - 1);
		//Part program handling............
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
		CurrentShape = new Line();
		((Line*)CurrentShape)->LineType = RapidEnums::LINETYPE::ANGLE_BISECTORL;
		((Line*)CurrentShape)->Length(LineLength);
		((Line*)CurrentShape)->DefinedLength(LengthDefined);
		((Line*)CurrentShape)->LinePosition(linepos);
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		ParentShape1->addChild(CurrentShape);
		ParentShape2->addChild(CurrentShape);
		CurrentShape->addParent(ParentShape1);
		CurrentShape->addParent(ParentShape2);
		AddShapeAction::addShape(CurrentShape);
		if(baseaction->CurrentWindowNo == 1)
			baseaction->DroMovementFlag = false;
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
		MAINDllOBJECT->DerivedShapeCallback();
		((ShapeWithList*)CurrentShape)->RAxisVal = (((ShapeWithList*)ParentShape1)->RAxisVal + ((ShapeWithList*)ParentShape2)->RAxisVal) / 2;
		ResetShapeHighlighted();
		init();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0012", __FILE__, __FUNCSIG__); }
}

void LineAngleBisector::SetAnyData(double *data)
{
	if(data[0] != 0)
	{
		LengthDefined = true;
		LineLength = data[0];
	}
}

void LineAngleBisector::RmouseDown(double x, double y)
{
	try
	{
		if(linepos < 2) linepos++;
		else linepos = 0;
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0012", __FILE__, __FUNCSIG__); }
}
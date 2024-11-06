#include "StdAfx.h"
#include "LineOnePointAngle.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor..//
LineOnePointAngle::LineOnePointAngle()
{
	try
	{ 
		init(); 
		MAINDllOBJECT->SetStatusCode("LineOnePointAngle01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEONPTANGMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
LineOnePointAngle::~LineOnePointAngle()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEONPTANGMH0002", __FILE__, __FUNCSIG__); }
}

void LineOnePointAngle::ResetShapeHighlighted()
{
	try
	{
		if(ParentShape != NULL)
		{
			ParentShape->HighlightedForMeasurement(false);
			MAINDllOBJECT->Shape_Updated();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEONPTANGMH0002", __FILE__, __FUNCSIG__); }
}

//Initialise Default settings..
void LineOnePointAngle::init()
{
	try
	{
		setMaxClicks(3);
		LineLength = 1;
		LengthDefined = false;
		linepos = 0;
		runningPartprogramValid = false;
		ParentShape = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEONPTANGMH0003", __FILE__, __FUNCSIG__); }
}

//Handling Left mousedown...//
void LineOnePointAngle::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::XLINE)
		{
			ParentShape = (Line*)MAINDllOBJECT->highlightedShape();
			angle = ParentShape->Angle();
			intercept = ParentShape->Intercept();
			if(ParentShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
				LineLength = ParentShape->Length();
			ParentShape->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
			MAINDllOBJECT->DerivedShapeCallback();
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		MAINDllOBJECT->DerivedShapeCallback();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEONPTANGMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move..//
void LineOnePointAngle::mouseMove()
{
	try
	{
		double mp[2] = {getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY()};
		if(getClicksDone() == getMaxClicks() - 1)
			RMATH2DOBJECT->EquiDistPointsOnLine(angle + newangle, LineLength, &mp[0], &point1[0], &point2[0], linepos);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEONPTANGMH0005", __FILE__, __FUNCSIG__); }
}

//draw perpendicular line..
void LineOnePointAngle::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1)
			GRAFIX->drawLine(point1[0], point1[1], point2[0], point2[1]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEONPTANGMH0006", __FILE__, __FUNCSIG__); }
}

///Handle the mouse scroll... calculate the line end points for change in length...//
void LineOnePointAngle::MouseScroll(bool flag)
{
	try
	{
		if(LengthDefined) return;
		if(flag) LineLength += 0.15;
		else {if(LineLength > 0.5) LineLength -= 0.15; }
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEONPTANGMH0008", __FILE__, __FUNCSIG__); }
}

void LineOnePointAngle::SetAnyData(double *data)
{
	try
	{
		newangle = data[0];
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
			newangle = data[0]/25.4;
		if(data[1] != 0)
		{
			LengthDefined = true;
			LineLength = data[1];
		}
		setClicksDone(getMaxClicks() - 1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEONPTANGMH0009", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data...//
void LineOnePointAngle::PartProgramData()
{
	try
	{
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
		CurrentShape->IsValid(false);

		list<BaseItem*>::iterator ptr = CurrentShape->getParents().end(); 
		ptr--;
		ParentShape = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()];		
		angle = ParentShape->Angle(); intercept = ParentShape->Intercept();

		newangle = ((Line*)CurrentShape)->Offset();	
		LineLength = ((Line*)CurrentShape)->Length();
		linepos = ((Line*)CurrentShape)->LinePosition();
		double mp[2] = {((Line*)CurrentShape)->getMidPoint()->getX(),  ((Line*)CurrentShape)->getMidPoint()->getY()};

		baseaction = PPCALCOBJECT->getFrameGrab();
		if(PPCALCOBJECT->getFrameGrab()->CurrentWindowNo == 1)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEONPTANGMH0010", __FILE__, __FUNCSIG__); }
}

//Reset the current handler settings.,
void LineOnePointAngle::EscapebuttonPress()
{
	ResetShapeHighlighted();
	init(); 
}

//Handle the maximum click...//
void LineOnePointAngle::LmaxmouseDown()
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
		((Line*)CurrentShape)->LineType = RapidEnums::LINETYPE::ONEPT_OFFSETLINE;
		((Line*)CurrentShape)->Length(LineLength);
		((Line*)CurrentShape)->DefinedLength(LengthDefined);
		((Line*)CurrentShape)->LinePosition(linepos);
		((Line*)CurrentShape)->Offset(newangle);
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());

		ParentShape->addChild(CurrentShape);
		CurrentShape->addParent(ParentShape);
		AddShapeAction::addShape(CurrentShape);
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
		ResetShapeHighlighted();
		init();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEONPTANGMH0012", __FILE__, __FUNCSIG__); }
}


void LineOnePointAngle::RmouseDown(double x, double y)
{
	try
	{
		if(linepos < 2) linepos++;
		else linepos = 0;
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0012", __FILE__, __FUNCSIG__); }
}
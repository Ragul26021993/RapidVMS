#include "StdAfx.h"
#include "LineParallelTangent.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor..//
LineParallelTangent::LineParallelTangent()
{
	try
	{ 
		init(); 
		MAINDllOBJECT->SetStatusCode("LineParallelTangent01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPATMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
LineParallelTangent::~LineParallelTangent()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPATMH0002", __FILE__, __FUNCSIG__); }
}

void LineParallelTangent::ResetShapeHighlighted()
{
	try
	{
		if(ParentLine != NULL)
			ParentLine->HighlightedForMeasurement(false);
		if(ParentCircle != NULL)
			ParentCircle->HighlightedForMeasurement(false);
		MAINDllOBJECT->Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPATMH0002", __FILE__, __FUNCSIG__); }
}

//Initialise Default settings..
void LineParallelTangent::init()
{
	try
	{
		setMaxClicks(3);
		LineLength = 1;
		LengthDefined = false;
		linepos = 0;
		LineSelected = false; CircleSelected = false;
		runningPartprogramValid = false;
		ParentLine = NULL; ParentCircle = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPATMH0003", __FILE__, __FUNCSIG__); }
}

//Handling Left mousedown...//
void LineParallelTangent::LmouseDown()
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
			if(!LineSelected)
			{
				LineSelected =  true;
				ParentLine = (Line*)MAINDllOBJECT->highlightedShape();
				angle1 = ParentLine->Angle();
				intercept1 = ParentLine->Intercept();
				if(ParentLine->ShapeType == RapidEnums::SHAPETYPE::LINE)
					LineLength = ParentLine->Length();
				ParentLine->HighlightedForMeasurement(true);
				MAINDllOBJECT->Shape_Updated();
			}
			else
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
		}
		else if(MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			if(!CircleSelected)
			{
				CircleSelected = true;
				ParentCircle = (Circle*)MAINDllOBJECT->highlightedShape();
				ParentCircle->getCenter()->FillDoublePointer(&Ccenter[0]);
				radius = ParentCircle->Radius();
				ParentCircle->HighlightedForMeasurement(true);
				MAINDllOBJECT->Shape_Updated();
			}
			else
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(getClicksDone() == getMaxClicks() - 1)
			MAINDllOBJECT->DerivedShapeCallback();
		MAINDllOBJECT->DerivedShapeCallback();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPATMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move..//
void LineParallelTangent::mouseMove()
{
	try
	{
		double mp[2] = {getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY()};
		if(getClicksDone() == getMaxClicks() - 1)
			RMATH2DOBJECT->Tangent_Parallel(&Ccenter[0], radius, angle1, intercept1, LineLength, &mp[0], &point1[0], &point2[0], linepos);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPATMH0005", __FILE__, __FUNCSIG__); }
}

//draw perpendicular line..
void LineParallelTangent::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1)
			GRAFIX->drawLine(point1[0], point1[1], point2[0], point2[1]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPATMH0006", __FILE__, __FUNCSIG__); }
}

///Handle the mouse scroll... calculate the line end points for change in length...//
void LineParallelTangent::MouseScroll(bool flag)
{
	try
	{
		if(LengthDefined) return;
		if(flag) LineLength += 0.15;
		else {if(LineLength > 0.5) LineLength -= 0.15; }
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPATMH0008", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data...//
void LineParallelTangent::PartProgramData()
{
	try
	{
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
		CurrentShape->IsValid(false);

		list<BaseItem*>::iterator ptr = CurrentShape->getParents().end(); 
		ptr--;
		ParentCircle = (Circle*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr--)->getId()];
		ParentLine = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()];
		angle1 = ParentLine->Angle(); intercept1 = ParentLine->Intercept();
		ParentCircle->getCenter()->FillDoublePointer(&Ccenter[0]);
		radius = ParentCircle->Radius();

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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPATMH0010", __FILE__, __FUNCSIG__); }
}

//Reset the current handler settings.,
void LineParallelTangent::EscapebuttonPress()
{
	ResetShapeHighlighted();
	init(); 
}

//Handle the maximum click...//
void LineParallelTangent::LmaxmouseDown()
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
		((Line*)CurrentShape)->LineType = RapidEnums::LINETYPE::PARALLEL_TANGENTL;
		((Line*)CurrentShape)->Length(LineLength);
		((Line*)CurrentShape)->DefinedLength(LengthDefined);
		((Line*)CurrentShape)->LinePosition(linepos);
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());

		ParentLine->addChild(CurrentShape);
		ParentCircle->addChild(CurrentShape);
		CurrentShape->addParent(ParentLine);
		CurrentShape->addParent(ParentCircle);
		AddShapeAction::addShape(CurrentShape);
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
		ResetShapeHighlighted();
		init();
		MAINDllOBJECT->DerivedShapeCallback();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPATMH0012", __FILE__, __FUNCSIG__); }
}

void LineParallelTangent::SetAnyData(double *data)
{
	if(data[0] != 0)
	{
		LengthDefined = true;
		LineLength = data[0];
	}
}

void LineParallelTangent::RmouseDown(double x, double y)
{
	try
	{
		if(linepos < 2) linepos++;
		else linepos = 0;
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0012", __FILE__, __FUNCSIG__); }
}
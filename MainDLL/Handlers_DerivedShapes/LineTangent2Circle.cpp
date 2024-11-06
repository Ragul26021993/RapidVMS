#include "StdAfx.h"
#include "LineTangent2Circle.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor..//
LineTangent2Circle::LineTangent2Circle()
{
	try
	{ 
		init(); 
		//MAINDllOBJECT->SetStatusCode("DL003");
		MAINDllOBJECT->SetStatusCode("LineTangent2Circle01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETANMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
LineTangent2Circle::~LineTangent2Circle()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETANMH0002", __FILE__, __FUNCSIG__); }
}

void LineTangent2Circle::ResetShapeHighlighted()
{
	try
	{
		if(ParentShape != NULL)
		{
			ParentShape->HighlightedForMeasurement(false);
			MAINDllOBJECT->Shape_Updated();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARAMH0002", __FILE__, __FUNCSIG__); }
}

//Initialise Default settings..
void LineTangent2Circle::init()
{
	try
	{
		setMaxClicks(2);
		LineLength = 1;
		LengthDefined = false;
		linepos = 0;
		runningPartprogramValid = false;
		ParentShape = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETANMH0003", __FILE__, __FUNCSIG__); }
}

//Handling Left mousedown...//
void LineTangent2Circle::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			ParentShape = (Circle*)MAINDllOBJECT->highlightedShape();
			LineLength = 1;
			ParentShape->getCenter()->FillDoublePointer(&CCenter[0]);
			radius = ParentShape->Radius();
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETANMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move..//
void LineTangent2Circle::mouseMove()
{
	try
	{
		double mp[2] = {getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY()};
		if(getClicksDone() == getMaxClicks() - 1)
			RMATH2DOBJECT->Tangent2aCircle(&CCenter[0], radius, &mp[0], LineLength, &point1[0], &point2[0], linepos);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETANMH0005", __FILE__, __FUNCSIG__); }
}

//draw perpendicular line..
void LineTangent2Circle::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1)
			GRAFIX->drawLine(point1[0], point1[1], point2[0], point2[1]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETANMH0006", __FILE__, __FUNCSIG__); }
}

///Handle the mouse scroll... calculate the line end points for change in length...//
void LineTangent2Circle::MouseScroll(bool flag)
{
	try
	{
		if(LengthDefined) return;
		if(flag) LineLength += 0.15;
		else {if(LineLength > 0.5) LineLength -= 0.15; }
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETANMH0008", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data...//
void LineTangent2Circle::PartProgramData()
{
	try
	{
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
		CurrentShape->IsValid(false);

		list<BaseItem*>::iterator ptr = CurrentShape->getParents().end(); 
		ptr--;
		ParentShape = (Circle*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()];
		ParentShape->getCenter()->FillDoublePointer(&CCenter[0]);
		radius = ParentShape->Radius();

		LineLength = ((Line*)CurrentShape)->Length();
		linepos = ((Line*)CurrentShape)->LinePosition();
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
			PPCALCOBJECT->partProgramReached();
			return;
		}
		setClicksDone(getMaxClicks() - 1);
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETANMH0010", __FILE__, __FUNCSIG__); }
}

//Reset the current handler settings.,
void LineTangent2Circle::EscapebuttonPress()
{
	ResetShapeHighlighted();
	init(); 
}

//Handle the maximum click...//
void LineTangent2Circle::LmaxmouseDown()
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
		((Line*)CurrentShape)->LineType = RapidEnums::LINETYPE::TANGENT_ON_CIRCLEL;
		((Line*)CurrentShape)->Length(LineLength);
		((Line*)CurrentShape)->DefinedLength(LengthDefined);
		((Line*)CurrentShape)->LinePosition(linepos);
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		ParentShape->addChild(CurrentShape);
		CurrentShape->addParent(ParentShape);
		AddShapeAction::addShape(CurrentShape);
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
		ResetShapeHighlighted();
		init();
		MAINDllOBJECT->DerivedShapeCallback();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETANMH0012", __FILE__, __FUNCSIG__); }
}


void LineTangent2Circle::SetAnyData(double *data)
{
	if(data[0] != 0)
	{
		LengthDefined = true;
		LineLength = data[0];
	}
}

void LineTangent2Circle::RmouseDown(double x, double y)
{
	try
	{
		if(linepos < 2) linepos++;
		else linepos = 0;
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0012", __FILE__, __FUNCSIG__); }
}
#include "StdAfx.h"
#include "LinePerpendicular2Line.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Vector.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor..//
LinePerpendicular2Line::LinePerpendicular2Line()
{
	try
	{ 
		init(); 
		MAINDllOBJECT->SetStatusCode("LinePerpendicular2Line01");
		/*MAINDllOBJECT->SetStatusCode("DL003");*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPERMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
LinePerpendicular2Line::~LinePerpendicular2Line()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPERMH0002", __FILE__, __FUNCSIG__); }
}


void LinePerpendicular2Line::ResetShapeHighlighted()
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
void LinePerpendicular2Line::init()
{
	try
	{
		setMaxClicks(2);
		LineLength = 1;
		this->CurrentWindow = -1;
		LengthDefined = false;
		linepos = 0;
		runningPartprogramValid = false;
		ParentShape = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPERMH0003", __FILE__, __FUNCSIG__); }
}

//Handling Left mousedown...//
void LinePerpendicular2Line::LmouseDown()
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
			if(ParentShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
				LineLength = ParentShape->Length();
			angle = ParentShape->Angle();
			intercept = ParentShape->Intercept();
			ParentShape->HighlightedForMeasurement(true);
			MAINDllOBJECT->ClearShapeSelections();
			MAINDllOBJECT->DerivedShapeCallback();
			MAINDllOBJECT->SetStatusCode("DL004");
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		MAINDllOBJECT->DerivedShapeCallback();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPERMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move..//
void LinePerpendicular2Line::mouseMove()
{
	try
	{
		double mp[2] = {getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY()};
		if(getClicksDone() == getMaxClicks() - 1)
			RMATH2DOBJECT->Perpedicularlline(angle, intercept, &mp[0], LineLength, &point1[0], &point2[0], linepos);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPERMH0005", __FILE__, __FUNCSIG__); }
}

//draw perpendicular line..
void LinePerpendicular2Line::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1)
			GRAFIX->drawLine(point1[0], point1[1], point2[0], point2[1]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPERMH0006", __FILE__, __FUNCSIG__); }
}

///Handle the mouse scroll... calculate the line end points for change in length...//
void LinePerpendicular2Line::MouseScroll(bool flag)
{
	try
	{
		if(LengthDefined) return;
		if(flag) LineLength += 0.15;
		else {if(LineLength > 0.5) LineLength -= 0.15; }
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPERMH0008", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data...//
void LinePerpendicular2Line::PartProgramData()
{
	try
	{
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
		CurrentShape->IsValid(false);
		
		list<BaseItem*>::iterator ptr = CurrentShape->getParents().begin(); 
		ParentShape = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()];
		angle = ParentShape->Angle(); intercept = ParentShape->Intercept();

		Line* myline = (Line*)CurrentShape;
		LineLength = myline->Length();
		linepos = ((Line*)CurrentShape)->LinePosition();
		double mp[2] = {myline->getMidPoint()->getX(), myline->getMidPoint()->getY() };
			
		baseaction = PPCALCOBJECT->getFrameGrab();
		if(baseaction->CurrentWindowNo == 1)
		{
			if(baseaction->Pointer_SnapPt != NULL)
				setBaseRProperty(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
			else
				setBaseRProperty(mp[0], mp[1], ParentShape->getPoint1()->getZ());
			((ShapeWithList*)myline)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
			setClicksDone(0);
			return;
		}
		setClicksDone(getMaxClicks() - 1);
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPERMH0010", __FILE__, __FUNCSIG__); }
}

//Reset the current handler settings.,
void LinePerpendicular2Line::EscapebuttonPress()
{
	ResetShapeHighlighted();
	init(); 
}

//Handle the maximum click...//
void LinePerpendicular2Line::LmaxmouseDown()
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
		((Line*)CurrentShape)->LineType = RapidEnums::LINETYPE::PERPENDICULAR_LINEL;
		((Line*)CurrentShape)->Length(LineLength);
		((Line*)CurrentShape)->DefinedLength(LengthDefined);
		((Line*)CurrentShape)->LinePosition(linepos);
		if(MAINDllOBJECT->getVectorPointer(pt) == NULL)
			setBaseRProperty(pt->getX(), pt->getY(), ParentShape->getPoint1()->getZ());
		else
	    	setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		ParentShape->RemoveChild(CurrentShape);
		CurrentShape->RemoveParent(ParentShape);
		ParentShape->addChild(CurrentShape);
		CurrentShape->addParent(ParentShape);
		AddShapeAction::addShape(CurrentShape);
		if(baseaction->CurrentWindowNo == 1)
			baseaction->DroMovementFlag = false;
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
		ResetShapeHighlighted();
		init();
		MAINDllOBJECT->DerivedShapeCallback();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPERMH0012", __FILE__, __FUNCSIG__); }
}

void LinePerpendicular2Line::SetAnyData(double *data)
{
	if(data[0] != 0)
	{
		LengthDefined = true;
		LineLength = data[0];
	}
}

void LinePerpendicular2Line::RmouseDown(double x, double y)
{
	try
	{
		if(linepos < 2) linepos++;
		else linepos = 0;
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0012", __FILE__, __FUNCSIG__); }
}
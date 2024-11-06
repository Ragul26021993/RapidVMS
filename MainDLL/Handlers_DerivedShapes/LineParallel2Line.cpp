#include "StdAfx.h"
#include "LineParallel2Line.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor..//
LineParallel2Line::LineParallel2Line()
{
	try
	{ 
		MultiPointParellelLine = false;
		init(); 
		/*MAINDllOBJECT->SetStatusCode("DL001");*/
		MAINDllOBJECT->SetStatusCode("LineParallel2Line01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARAMH0001", __FILE__, __FUNCSIG__); }
}

LineParallel2Line::LineParallel2Line(bool MultiPointsLine)
{
	try
	{ 
		MultiPointParellelLine = MultiPointsLine;
		init(); 
		/*MAINDllOBJECT->SetStatusCode("DL001");*/
		MAINDllOBJECT->SetStatusCode("LineParallel2Line01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARAMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
LineParallel2Line::~LineParallel2Line()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARAMH0002", __FILE__, __FUNCSIG__); }
}

void LineParallel2Line::ResetShapeHighlighted()
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
void LineParallel2Line::init()
{
	try
	{
		resetClicks();
		LineLength = 1;
		linepos = 0;
		LengthDefined = false;
		runningPartprogramValid = false;
		DefinedOffset = false;
		ParentShape = NULL;
		setMaxClicks(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARAMH0003", __FILE__, __FUNCSIG__); }
}

//Handling Left mousedown...//
void LineParallel2Line::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::XRAY || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::XLINE)
		{
			ParentShape = (Line*)MAINDllOBJECT->highlightedShape();
			if(ParentShape->ShapeType == RapidEnums::SHAPETYPE::LINE)
				LineLength = ParentShape->Length();
			angle = ParentShape->Angle();
			intercept = ParentShape->Intercept();
			ParentShape->HighlightedForMeasurement(true);
			MAINDllOBJECT->ClearShapeSelections();
			if(MultiPointParellelLine)
			{
				AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
				Shape* TmpShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
				((Line*)TmpShape)->LineType = RapidEnums::LINETYPE::MULTIPOINTSLINEPARALLEL2LINE;
				TmpShape->addParent(ParentShape);
				ParentShape->addChild(TmpShape);
				resetClicks();
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB);
				return;
			}
			MAINDllOBJECT->DerivedShapeCallback();
			MAINDllOBJECT->SetStatusCode("DL002");
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARAMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move..//
void LineParallel2Line::mouseMove()
{
	try
	{
		double mp[2] = {getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY()};
		if(getClicksDone() == getMaxClicks() - 1 && !MultiPointParellelLine)
			RMATH2DOBJECT->Parallelline(angle, intercept, &mp[0], LineLength, &point1[0], &point2[0], &offset, DefinedOffset, linepos);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARAMH0005", __FILE__, __FUNCSIG__); }
}

//draw parallel line..
void LineParallel2Line::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1 && !MultiPointParellelLine)
			GRAFIX->drawLine(point1[0], point1[1], point2[0], point2[1]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARAMH0006", __FILE__, __FUNCSIG__); }
}

//Set parent shape for parallel line
void LineParallel2Line::SetShapeandClickdone(Shape* Csh, double len)
{
	try
	{
		ParentShape = (Line*)Csh;
		LineLength = len;
		angle = ((Line*)ParentShape)->Angle();
		intercept = ((Line*)ParentShape)->Intercept();
		setClicksDone(getMaxClicks() - 1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARAMH0007", __FILE__, __FUNCSIG__); }
}


///Handle the mouse scroll... calculate the line end points for change in length...//
void LineParallel2Line::MouseScroll(bool flag)
{
	try
	{
		if(LengthDefined) return;
		if(flag) LineLength += 0.15;
		else {if(LineLength > 0.5) LineLength -= 0.15; }
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARAMH0008", __FILE__, __FUNCSIG__); }
}

//Set Offset distance..
void LineParallel2Line::SetAnyData(double *data)
{
	try
	{
		offset = data[0];
		if(data[1] != 0)
		{
			LengthDefined = true;
			LineLength = data[1];
		}
		if(offset != 0)
			DefinedOffset = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARAMH0009", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data...//
void LineParallel2Line::PartProgramData()
{
	try
	{
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
		CurrentShape->IsValid(false);
		
		list<BaseItem*>::iterator ptr = CurrentShape->getParents().begin(); 
		ParentShape = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()];
		angle = ParentShape->Angle(); intercept = ParentShape->Intercept();

		Line* myline = (Line*)CurrentShape;
		offset = myline->Offset();
		DefinedOffset = myline->DefinedOffset();
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARAMH0010", __FILE__, __FUNCSIG__); }
}

//Reset the current handler settings.
void LineParallel2Line::EscapebuttonPress()
{
	ResetShapeHighlighted();
	init();
}

//Handle the maximum click...//
void LineParallel2Line::LmaxmouseDown()
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
		((Line*)CurrentShape)->LineType = RapidEnums::LINETYPE::PARALLEL_LINEL;
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
		((Line*)CurrentShape)->Offset(offset);
		((Line*)CurrentShape)->DefinedOffset(DefinedOffset);
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
		ResetShapeHighlighted();
		init();
		MAINDllOBJECT->DerivedShapeCallback();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARAMH0012", __FILE__, __FUNCSIG__); }
}

void LineParallel2Line::RmouseDown(double x, double y)
{
	try
	{
		if(linepos < 2) linepos++;
		else linepos = 0;
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0012", __FILE__, __FUNCSIG__); }
}
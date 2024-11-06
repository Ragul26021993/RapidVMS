#include "StdAfx.h"
#include "LineTangentThruPoint.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor..//
LineTangentThruPoint::LineTangentThruPoint()
{
	try
	{ 
		init(); 
		MAINDllOBJECT->SetStatusCode("LineTangentThruPoint01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETANPTMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
LineTangentThruPoint::~LineTangentThruPoint()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETANPTMH0002", __FILE__, __FUNCSIG__); }
}

void LineTangentThruPoint::ResetShapeHighlighted()
{
	try
	{
		if(ParentCircle != NULL)
		{
			ParentCircle->HighlightedForMeasurement(false);
			MAINDllOBJECT->Shape_Updated();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARAMH0002", __FILE__, __FUNCSIG__); }
}

//Initialise Default settings..
void LineTangentThruPoint::init()
{
	try
	{
		setMaxClicks(3);
		LineLength = 1;
		linepos = 0;
		LengthDefined = false;
		Pointselected = false; CircleSelected = false;
		runningPartprogramValid = false;
		ParentCircle = NULL; vPointer = NULL;
		ParentShape1 = NULL; ParentShape2 = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETANPTMH0003", __FILE__, __FUNCSIG__); }
}

//Handling Left mousedown...//
void LineTangentThruPoint::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL && MAINDllOBJECT->highlightedPoint() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(MAINDllOBJECT->highlightedShape() != NULL && !CircleSelected)
		{
			CircleSelected = true;
			ParentCircle = (Circle*)MAINDllOBJECT->highlightedShape();
			ParentCircle->getCenter()->FillDoublePointer(&Center[0]);
			radius = ParentCircle->Radius();
			ParentCircle->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();		
		}
		else if(MAINDllOBJECT->highlightedPoint() != NULL && !Pointselected)
		{
			Pointselected = true;
			vPointer = MAINDllOBJECT->highlightedPoint();
			ParentShape1 = MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1();
			ParentShape2 = MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2();
			vPointer->FillDoublePointer(&Vpoint[0]);			
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(getClicksDone() == getMaxClicks() - 1)
			MAINDllOBJECT->DerivedShapeCallback();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETANPTMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move..//
void LineTangentThruPoint::mouseMove()
{
	try
	{
		double mp[2] = {getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY()};
		if(getClicksDone() == getMaxClicks() - 1)
			RMATH2DOBJECT->Tangent_ThruPoint(&Center[0], radius, &Vpoint[0], &mp[0], LineLength, &point1[0], &point2[0], linepos);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETANPTMH0005", __FILE__, __FUNCSIG__); }
}

//draw perpendicular line..
void LineTangentThruPoint::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1)
			GRAFIX->drawLine(point1[0], point1[1], point2[0], point2[1]);
		if(getClicksDone() > 0)
		{
			GRAFIX->SetColor_Double(0, 1, 0);
			if(vPointer != NULL)
				GRAFIX->drawPoint(vPointer->getX(), vPointer->getY(), vPointer->getZ(), 8);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETANPTMH0006", __FILE__, __FUNCSIG__); }
}

///Handle the mouse scroll... calculate the line end points for change in length...//
void LineTangentThruPoint::MouseScroll(bool flag)
{
	try
	{
		if(LengthDefined) return;
		if(flag) LineLength += 0.15;
		else {if(LineLength > 0.5) LineLength -= 0.15; }
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETANPTMH0008", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data...//
void LineTangentThruPoint::PartProgramData()
{
	try
	{
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
		CurrentShape->IsValid(false);

		list<BaseItem*>::iterator ptr = CurrentShape->getParents().end(); 
		ptr--;
		Circle* ParentCircle = (Circle*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()];
		ParentCircle->getCenter()->FillDoublePointer(&Center[0]);
		radius = ParentCircle->Radius();
			
		vPointer = ((Line*)CurrentShape)->VPointer;
		vPointer->FillDoublePointer(&Vpoint[0]);
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETANPTMH0010", __FILE__, __FUNCSIG__); }
}

//Reset the current handler settings.,
void LineTangentThruPoint::EscapebuttonPress()
{
	ResetShapeHighlighted();
	init(); 
}

//Handle the maximum click...//
void LineTangentThruPoint::LmaxmouseDown()
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
		((Line*)CurrentShape)->LineType = RapidEnums::LINETYPE::TANGENT_THRU_POINT;
		((Line*)CurrentShape)->Length(LineLength);
		((Line*)CurrentShape)->DefinedLength(LengthDefined);
		((Line*)CurrentShape)->LinePosition(linepos);
		((Line*)CurrentShape)->VPointer = vPointer;
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		
		ParentShape1->addChild(CurrentShape);		
		CurrentShape->addParent(ParentShape1);
		if(ParentShape2 != NULL)
		{
			ParentShape2->addChild(CurrentShape);
			CurrentShape->addParent(ParentShape2);
		}
		ParentCircle->addChild(CurrentShape);
		CurrentShape->addParent(ParentCircle);
		AddShapeAction::addShape(CurrentShape);
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
		ResetShapeHighlighted();
		init();
		MAINDllOBJECT->DerivedShapeCallback();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETANPTMH0012", __FILE__, __FUNCSIG__); }
}

void LineTangentThruPoint::SetAnyData(double *data)
{
	if(data[0] != 0)
	{
		LengthDefined = true;
		LineLength = data[0];
	}
}

void LineTangentThruPoint::RmouseDown(double x, double y)
{
	try
	{
		if(linepos < 2) linepos++;
		else linepos = 0;
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0012", __FILE__, __FUNCSIG__); }
}
#include "StdAfx.h"
#include "LineTangentOn2Circles.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor..//
LineTangentOn2Circles::LineTangentOn2Circles()
{
	try
	{ 
		init(); 
		MAINDllOBJECT->SetStatusCode("LineTangentOn2Circles01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETAN2CMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
LineTangentOn2Circles::~LineTangentOn2Circles()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETAN2CMH0002", __FILE__, __FUNCSIG__); }
}

void LineTangentOn2Circles::ResetShapeHighlighted()
{
	try
	{
		if(ParentShape1 != NULL)
			ParentShape1->HighlightedForMeasurement(false);
		if(ParentShape2 != NULL)
			ParentShape2->HighlightedForMeasurement(false);
		MAINDllOBJECT->Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC20001", __FILE__, __FUNCSIG__); }
}

//Initialise Default settings..
void LineTangentOn2Circles::init()
{
	try
	{
		setMaxClicks(3);
		LineLength = 1;
		LengthDefined = false;
		runningPartprogramValid = false;
		ParentShape1 = NULL; ParentShape2 = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETAN2CMH0003", __FILE__, __FUNCSIG__); }
}

//Handling Left mousedown...//
void LineTangentOn2Circles::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if(Csh->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || Csh->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			if(getClicksDone() == 1)
			{
				ParentShape1 = (Circle*)Csh;
				ParentShape1->getCenter()->FillDoublePointer(&Center1[0]);
				radius1 = ParentShape1->Radius();
			}
			else
			{
				ParentShape2 = (Circle*)Csh;
				ParentShape2->getCenter()->FillDoublePointer(&Center2[0]);
				radius2 = ParentShape2->Radius();
				MAINDllOBJECT->DerivedShapeCallback();
			}
			Csh->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETAN2CMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move..//
void LineTangentOn2Circles::mouseMove()
{
	try
	{
		double mp[2] = {getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY()};
		if(getClicksDone() == getMaxClicks() - 1)
		{
			RMATH2DOBJECT->Tangent_TwoCircles(&Center1[0], radius1, &Center2[0], radius2, LineLength, &mp[0], &point1[0], &point2[0]);
			if(!LengthDefined)
			{
				double lineparam[6] = {point1[0], point1[1], 0, 0, 0, 0}, dir[3] = {point2[0] - point1[0], point2[1] - point1[1], 0};
				RMATH3DOBJECT->DirectionCosines(dir, &lineparam[3]);
				double endpnts[6] = {0};
				RMATH3DOBJECT->Projection_Point_on_Line(Center1, lineparam, endpnts);
				RMATH3DOBJECT->Projection_Point_on_Line(Center2, lineparam, &endpnts[3]);
				point1[0] = endpnts[0];
				point1[1] = endpnts[1];
				point2[0] = endpnts[3];
				point2[1] = endpnts[4];
				LineLength = RMATH2DOBJECT->Pt2Pt_distance(point1[0], point1[1], point2[0], point2[1]);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETAN2CMH0005", __FILE__, __FUNCSIG__); }
}

//draw perpendicular line..
void LineTangentOn2Circles::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1)
			GRAFIX->drawLine(point1[0], point1[1], point2[0], point2[1]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETAN2CMH0006", __FILE__, __FUNCSIG__); }
}

///Handle the mouse scroll... calculate the line end points for change in length...//
void LineTangentOn2Circles::MouseScroll(bool flag)
{
	try
	{
		if(LengthDefined) return;
		if(flag) LineLength += 0.15;
		else {if(LineLength > 0.5) LineLength -= 0.15; }
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETAN2CMH0008", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data...//
void LineTangentOn2Circles::PartProgramData()
{
	try
	{
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
		CurrentShape->IsValid(false);

		list<BaseItem*>::iterator ptr = CurrentShape->getParents().end(); 
		ptr--;
		ParentShape1 = (Circle*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr--)->getId()];
		ParentShape2 = (Circle*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()];
		ParentShape1->getCenter()->FillDoublePointer(&Center1[0]);
		radius1 = ParentShape1->Radius();
		ParentShape2->getCenter()->FillDoublePointer(&Center2[0]);
		radius2 = ParentShape2->Radius();
		
		LineLength = ((Line*)CurrentShape)->Length();
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETAN2CMH0010", __FILE__, __FUNCSIG__); }
}

//Reset the current handler settings.,
void LineTangentOn2Circles::EscapebuttonPress()
{
	ResetShapeHighlighted();
	init(); 
}

//Handle the maximum click...//
void LineTangentOn2Circles::LmaxmouseDown()
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
		((Line*)CurrentShape)->LineType = RapidEnums::LINETYPE::TANGENT_ON_2_CIRCLE;
		((Line*)CurrentShape)->DefinedLength(LengthDefined);
		((Line*)CurrentShape)->Length(LineLength);
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());

		ParentShape1->addChild(CurrentShape);
		ParentShape2->addChild(CurrentShape);
		CurrentShape->addParent(ParentShape1);
		CurrentShape->addParent(ParentShape2);
		AddShapeAction::addShape(CurrentShape);
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
		ResetShapeHighlighted();
		init();
		MAINDllOBJECT->DerivedShapeCallback();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINETAN2CMH0012", __FILE__, __FUNCSIG__); }
}

void LineTangentOn2Circles::SetAnyData(double *data)
{
	if(data[0] != 0)
	{
		LengthDefined = true;
		LineLength = data[0];
	}
}
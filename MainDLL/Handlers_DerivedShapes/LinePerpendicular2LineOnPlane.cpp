#include "StdAfx.h"
#include "LinePerpendicular2LineOnPlane.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Plane.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"

//Constructor..//
LinePerpendicular2LineOnPlane::LinePerpendicular2LineOnPlane()
{
	try
	{ 
		init(); 
		/*MAINDllOBJECT->SetStatusCode("LinePerpendicular2Line01");*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPERMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
LinePerpendicular2LineOnPlane::~LinePerpendicular2LineOnPlane()
{
}


//Initialise Default settings..
void LinePerpendicular2LineOnPlane::init()
{
	try
	{
		resetClicks();
		setMaxClicks(4);
		ParentPlane = NULL;
		ParentLine = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPERMH0003", __FILE__, __FUNCSIG__); }
}

//Handling Left mousedown...//
void LinePerpendicular2LineOnPlane::LmouseDown()
{
	try
	{
		if(getClicksDone() == 3)
		{
		    LmaxmouseDown();
			return;
		}
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(getClicksDone() == 1 && (MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE3D || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::XLINE3D || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE))
		{
			ParentLine = (Line*)MAINDllOBJECT->highlightedShape();
			return;
		}
		else if(getClicksDone() == 2 && MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::PLANE)
		{
			double lineParam[6] = {0}, planeParam[4] = {0}, tmpDir[3] = {0};
			ParentLine->getParameters(lineParam);
			((Plane*)MAINDllOBJECT->highlightedShape())->getParameters(planeParam);
			if(RMATH3DOBJECT->Checking_Perpendicular_Line_to_Plane(lineParam, planeParam))
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
			ParentPlane = (Plane*)MAINDllOBJECT->highlightedShape();
			return;
		}
		else 
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPERMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move..//
void LinePerpendicular2LineOnPlane::mouseMove()
{
}

//draw perpendicular line..
void LinePerpendicular2LineOnPlane::draw(int windowno, double WPixelWidth)
{
}

///Handle the mouse scroll... calculate the line end points for change in length...//
void LinePerpendicular2LineOnPlane::MouseScroll(bool flag)
{
}

//Handle the partprogram data...//
void LinePerpendicular2LineOnPlane::PartProgramData()
{
}

//Reset the current handler settings.,
void LinePerpendicular2LineOnPlane::EscapebuttonPress()
{
	init(); 
}

//Handle the maximum click...//
void LinePerpendicular2LineOnPlane::LmaxmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedPoint() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		CurrentShape = new Line(_T("L3D"), RapidEnums::SHAPETYPE::LINE3D);
		Vector* pt = &getClicksValue(getMaxClicks() - 1);
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		baseaction->DroMovementFlag = false;
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction, false);
		((Line*)CurrentShape)->LineType = RapidEnums::LINETYPE::LINEPERPENDICULAR2LINEONPLANE;
		ParentLine->addChild(CurrentShape);
		CurrentShape->addParent(ParentLine);
		ParentPlane->addChild(CurrentShape);
		CurrentShape->addParent(ParentPlane);
		CurrentShape->UpdateBestFit();
		AddShapeAction::addShape(CurrentShape);
		init();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPERMH0012", __FILE__, __FUNCSIG__); }
}

void LinePerpendicular2LineOnPlane::SetAnyData(double *data)
{
}

void LinePerpendicular2LineOnPlane::RmouseDown(double x, double y)
{
}

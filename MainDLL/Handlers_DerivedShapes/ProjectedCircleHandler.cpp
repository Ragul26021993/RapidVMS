#include "StdAfx.h"
#include "ProjectedCircleHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\Circle.h"
#include "..\Actions\AddShapeAction.h"

//Constructor.....//
ProjectedCircleHandler::ProjectedCircleHandler()
{	
	try
	{ 	
		setMaxClicks(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor..
ProjectedCircleHandler::~ProjectedCircleHandler()
{
}

//Handle mousedown..//
void ProjectedCircleHandler::LmouseDown()
{}

//Handle the mouse move...//
void ProjectedCircleHandler::mouseMove()
{}	

//Nothing..
void ProjectedCircleHandler::draw(int windowno, double WPixelWidth)
{}

void ProjectedCircleHandler::EscapebuttonPress()
{}

//Handle the partprogram data...//
void ProjectedCircleHandler::PartProgramData()
{}

//max mouse down....//
void ProjectedCircleHandler::LmaxmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if(Csh->ShapeType == RapidEnums::SHAPETYPE::PLANE)
		{
			CurrentShape = new Circle(_T("C3D"), RapidEnums::SHAPETYPE::CIRCLE3D);
	     	AddShapeAction::addShape(CurrentShape);
	     	((Circle*)CurrentShape)->CircleType = RapidEnums::CIRCLETYPE::PROJECTEDCIRCLE;
			CurrentShape->addParent(Csh);
			Csh->addChild(CurrentShape);
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
			return;
		}
		setClicksDone(getClicksDone() - 1);
		return;	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0010", __FILE__, __FUNCSIG__); }
}
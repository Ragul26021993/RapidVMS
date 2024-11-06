#include "StdAfx.h"
#include "NearestPointHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\RPoint.h"
#include "..\Actions\AddShapeAction.h"

//Constructor.....//
NearestPointHandler::NearestPointHandler(bool NearestPoint, bool shapeas2d)
{	
	try
	{
	   shapeAs2D = shapeas2d;
		if(shapeas2d)
		{
			CurrentShape = new RPoint();
		    AddShapeAction::addShape(CurrentShape);
			if(NearestPoint)
			{
				((RPoint*)CurrentShape)->PointType = RPoint::RPOINTTYPE::NEARESTPOINT;
				MAINDllOBJECT->SetStatusCode("NearestPointHandler011");
			}
			else
			{
				((RPoint*)CurrentShape)->PointType = RPoint::RPOINTTYPE::FARTHESTPOINT;
				MAINDllOBJECT->SetStatusCode("NearestPointHandler022");
			}
		}
		else
		{
			CurrentShape = new RPoint(_T("P3D"), RapidEnums::SHAPETYPE::RPOINT3D);
	     	AddShapeAction::addShape(CurrentShape);
	     	if(NearestPoint)
			{
				((RPoint*)CurrentShape)->PointType = RPoint::RPOINTTYPE::NEARESTPOINT3D;
				MAINDllOBJECT->SetStatusCode("NearestPointHandler011");
			}
			else
			{
				((RPoint*)CurrentShape)->PointType = RPoint::RPOINTTYPE::FARTHESTPOINT3D;
				MAINDllOBJECT->SetStatusCode("NearestPointHandler022");
			}
		}
		setMaxClicks(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor..
NearestPointHandler::~NearestPointHandler()
{
}

//Handle mousedown..//
void NearestPointHandler::LmouseDown()
{}

//Handle the mouse move...//
void NearestPointHandler::mouseMove()
{}	

//Nothing..
void NearestPointHandler::draw(int windowno, double WPixelWidth)
{}

void NearestPointHandler::EscapebuttonPress()
{}

//Handle the partprogram data...//
void NearestPointHandler::PartProgramData()
{}

//max mouse down....//
void NearestPointHandler::LmaxmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if(shapeAs2D)
		{
			if(Csh->ShapeType == RapidEnums::SHAPETYPE::LINE || Csh->ShapeType == RapidEnums::SHAPETYPE::XLINE || Csh->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || Csh->ShapeType == RapidEnums::SHAPETYPE::ARC)
			{
				CurrentShape->addParent(Csh);
				Csh->addChild(CurrentShape);
				if(((RPoint*)CurrentShape)->PointType == RPoint::RPOINTTYPE::NEARESTPOINT)
					MAINDllOBJECT->SetStatusCode("NearestPtHandler03");
				else
					MAINDllOBJECT->SetStatusCode("NearestPtHandler04");
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
				return;
			}
		}
		else
		{
		    if(Csh->ShapeType == RapidEnums::SHAPETYPE::PLANE || Csh->ShapeType == RapidEnums::SHAPETYPE::LINE3D || Csh->ShapeType == RapidEnums::SHAPETYPE::XLINE3D || Csh->ShapeType == RapidEnums::SHAPETYPE::SPHERE)
			{
				CurrentShape->addParent(Csh);
				Csh->addChild(CurrentShape);
				if(((RPoint*)CurrentShape)->PointType == RPoint::RPOINTTYPE::NEARESTPOINT3D)
					MAINDllOBJECT->SetStatusCode("NearestPtHandler03");
				else
					MAINDllOBJECT->SetStatusCode("NearestPtHandler04");
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
				return;
			}
		}
		setClicksDone(getClicksDone() - 1);
		return;	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("POINTMH0010", __FILE__, __FUNCSIG__); }
}
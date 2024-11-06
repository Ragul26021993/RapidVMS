#include "StdAfx.h"
#include "ReflectionHandler.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Plane.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\ReflectionAction.h"
//Default handler .. do nothing.....!!!!
ReflectionHandler::ReflectionHandler()
{
	setMaxClicks(1);
}

ReflectionHandler::~ReflectionHandler()
{
}

void ReflectionHandler::LmouseDown()
{

}

void ReflectionHandler::mouseMove()
{
}	

void ReflectionHandler::draw(int windowno, double WPixelWidth)
{
}

void ReflectionHandler::LmaxmouseDown()
{
		ShapeAsReflectionSurface = NULL;
		ShapeAsReflectionSurface = (Shape*)(MAINDllOBJECT->highlightedShape());
		if(MAINDllOBJECT->highlightedShape() == NULL ||  MAINDllOBJECT->getSelectedShapesList().getList().empty())
		{
		   setClicksDone(0);
		   return;
		}
		else if((ShapeAsReflectionSurface->ShapeType != RapidEnums::SHAPETYPE::LINE) && (ShapeAsReflectionSurface->ShapeType == RapidEnums::SHAPETYPE::PLANE))
		{
		   setClicksDone(0);
		   return;
		}
		bool alongPlane = false;
		if(ShapeAsReflectionSurface->ShapeType == RapidEnums::SHAPETYPE::PLANE )
		{
			((Plane*)ShapeAsReflectionSurface)->getParameters(ReflectionParam);
			alongPlane = true;
		}
		else if(ShapeAsReflectionSurface->ShapeType == RapidEnums::SHAPETYPE::LINE)
				((Line*)ShapeAsReflectionSurface)->getParameters(ReflectionParam);
	
		RCollectionBase& rshapes = MAINDllOBJECT->getCurrentUCS().getShapes();
		ReflectionAction *newAction = new ReflectionAction();
		newAction->ReflectionSurfaceId = ShapeAsReflectionSurface->getId();
		for(RC_ITER item = MAINDllOBJECT->getSelectedShapesList().getList().begin(); item != MAINDllOBJECT->getSelectedShapesList().getList().end(); item++)
		{
			Shape* Current_Shape = (Shape*)(*item).second;
			if(!Current_Shape->Normalshape()) continue;
			Shape *reflectedShape = Current_Shape->Clone(0, false);
			Current_Shape->UpdateRefelctedShape(reflectedShape, ReflectionParam, true);
			if (reflectedShape != NULL)
			{
				reflectedShape->IsValid(true);
				rshapes.addItem(reflectedShape, false);
				newAction->ReflectedShapesId.push_back(reflectedShape->getId());
				newAction->SelectedshapesId.push_back(Current_Shape->getId());
			}
		}	
		MAINDllOBJECT->addAction(newAction);
		setClicksDone(0);
}

void ReflectionHandler::EscapebuttonPress()
{
}

void ReflectionHandler::PartProgramData()
{

}

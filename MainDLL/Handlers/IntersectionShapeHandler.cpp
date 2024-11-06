#include "StdAfx.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Shapes\Plane.h"
#include "..\Shapes\Cone.h"
#include "..\Shapes\Cylinder.h"
#include "..\Shapes\IntersectionShape.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Actions\AddPointAction.h"
#include "IntersectionShapeHandler.h"

//Constructor...//
//According to the line type, set the clicks required and set the required flags....//
IntersectionShapeHandler::IntersectionShapeHandler()
{
	try
	{
		setMaxClicks(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0001", __FILE__, __FUNCSIG__); }
}

//Desructor/..////
IntersectionShapeHandler::~IntersectionShapeHandler()
{ 
}

//Handle the mousemove...//
void IntersectionShapeHandler::mouseMove()
{
}

//Handle the mousedown event..//
void IntersectionShapeHandler::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL) 
		{
		  setClicksDone(getClicksDone() - 1);
		  return;
		}
		if(MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::SPHERE || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::CONE || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::CYLINDER || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::PLANE )
		{
		   Parentshape[0] = MAINDllOBJECT->highlightedShape();
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0006", __FILE__, __FUNCSIG__); }
}


void IntersectionShapeHandler::RmouseDown(double x,double y)
{
}

void IntersectionShapeHandler::draw(int windowno, double WPixelWidth)
{
}

//Handle the partprogram data...//
void IntersectionShapeHandler::PartProgramData()
{
	CurrentShape = (IntersectionShape*)(MAINDllOBJECT->getShapesList().selectedItem());
	CurrentShape->UpdateBestFit();
	PPCALCOBJECT->partProgramReached();
}

void IntersectionShapeHandler::EscapebuttonPress()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0010", __FILE__, __FUNCSIG__); }
}

//Handle the Maximum mouse click...//
void IntersectionShapeHandler::LmaxmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL) 
		{
		  setClicksDone(getMaxClicks() - 1);
		  return;
		}
		if(MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::SPHERE || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::CONE || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::CYLINDER || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::PLANE )
		{
			Parentshape[1] = MAINDllOBJECT->highlightedShape();
			if(compareboth<int>(Parentshape[0]->ShapeType, Parentshape[1]->ShapeType, RapidEnums::SHAPETYPE::CONE, RapidEnums::SHAPETYPE::CYLINDER))
			{
				  if(Parentshape[0]->ShapeType ==  RapidEnums::SHAPETYPE::CONE)
				  {
					 Shape *tmpShape = Parentshape[0];
					 Parentshape[0] = Parentshape[1];
					 Parentshape[1] = tmpShape;
				  }
			}
			else if(compareboth<int>(Parentshape[0]->ShapeType, Parentshape[1]->ShapeType, RapidEnums::SHAPETYPE::CONE, RapidEnums::SHAPETYPE::PLANE))
			{
	    		 if(Parentshape[1]->ShapeType ==  RapidEnums::SHAPETYPE::CONE)
				 {
					Shape *tmpShape = Parentshape[0];
					Parentshape[0] = Parentshape[1];
					Parentshape[1] = tmpShape;
				 }
			}
			else if(compareboth<int>(Parentshape[0]->ShapeType, Parentshape[1]->ShapeType, RapidEnums::SHAPETYPE::PLANE, RapidEnums::SHAPETYPE::CYLINDER))
			{
				 if(Parentshape[1]->ShapeType ==  RapidEnums::SHAPETYPE::CYLINDER)
				 {
					Shape *tmpShape = Parentshape[0];
					Parentshape[0] = Parentshape[1];
					Parentshape[1] = tmpShape;
				 }
			}	
			else if(compareboth<int>(Parentshape[0]->ShapeType, Parentshape[1]->ShapeType, RapidEnums::SHAPETYPE::CONE, RapidEnums::SHAPETYPE::SPHERE))
			{
				 if(Parentshape[1]->ShapeType ==  RapidEnums::SHAPETYPE::CONE)
				 {
					Shape *tmpShape = Parentshape[0];
					Parentshape[0] = Parentshape[1];
					Parentshape[1] = tmpShape;
				 }
			}	
			else
			{
				setClicksDone(getMaxClicks() - 1);
				return;
			}
		    Shape *nShape = new IntersectionShape();
			nShape->addParent(Parentshape[0]);
			nShape->addParent(Parentshape[1]);
			Parentshape[0]->addChild(nShape);
			Parentshape[1]->addChild(nShape);
			nShape->UpdateBestFit();
			AddShapeAction::addShape(nShape);
			setBaseRProperty(0, 0, 0);
	     	baseaction->DroMovementFlag = false;
	    	AddPointAction::pointAction((ShapeWithList*)nShape, baseaction, false);
			resetClicks();
			MAINDllOBJECT->SetStatusCode("Finished");
		}
		else
		{
		    setClicksDone(getMaxClicks() - 1);
		    return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0011", __FILE__, __FUNCSIG__); }
}
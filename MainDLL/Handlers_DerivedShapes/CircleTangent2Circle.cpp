#include "stdafx.h"
#include "CircleTangent2Circle.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor..
CircleTangent2Circle::CircleTangent2Circle()
{
	init();
	setMaxClicks(2);
	MAINDllOBJECT->SetStatusCode("CircleTangent2Circle01");
}

//Destructor..
CircleTangent2Circle::~CircleTangent2Circle()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTC0001", __FILE__, __FUNCSIG__); }
}

void CircleTangent2Circle::ResetShapeHighlighted()
{
	try
	{
		if(ParentShape != NULL)
		{
			ParentShape->HighlightedForMeasurement(false);
			MAINDllOBJECT->Shape_Updated();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTC0001", __FILE__, __FUNCSIG__); }
}

//Initialise settings
void CircleTangent2Circle::init()
{
	try
	{
		runningPartprogramValid = false;
		ParentShape = NULL;
		DefinedDia = false;
		resetClicks();
		circle_radius = 250;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTC0001", __FILE__, __FUNCSIG__); }
}

//Handle mouse down..
void CircleTangent2Circle::LmouseDown()
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
			ParentShape = (Circle*)Csh;
			ParentShape->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
		}
		else //Wrong proceedure...
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		MAINDllOBJECT->ClearShapeSelections();
		ParentShape->getCenter()->FillDoublePointer(&center[0]);
		radius = ParentShape->Radius();
		MAINDllOBJECT->DerivedShapeCallback();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTC0002", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move..
void CircleTangent2Circle::mouseMove()
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1)
		{
			double mp[2] = {getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY()};
			if(DefinedDia)
				RMATH2DOBJECT->Circle_Tangent2CircleFixedDia(&center[0], radius, circle_radius, &mp[0], &center_ans[0]);
			else
				RMATH2DOBJECT->Circle_Tangent2Circle(&center[0], radius, &mp[0], &center_ans[0], &circle_radius);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTC0003", __FILE__, __FUNCSIG__); }
}

//draw the tangent circle..
void CircleTangent2Circle::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1)
			GRAFIX->drawCircle(center_ans[0], center_ans[1], circle_radius, true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTC0004", __FILE__, __FUNCSIG__); }
}

void CircleTangent2Circle::EscapebuttonPress()
{
	ResetShapeHighlighted();
	init();
}

void CircleTangent2Circle::SetAnyData(double *data)
{
	try
	{
		DefinedDia = true;
		circle_radius = data[0] / 2;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTC0005", __FILE__, __FUNCSIG__); }
}

//Increase/decrease the diameter for ouse scroll event....//
void CircleTangent2Circle::MouseScroll(bool flag)
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning()) return;
		if(!DefinedDia)
		{
			if(flag) circle_radius += 0.005;
			else {if(circle_radius > 0.05) circle_radius -= 0.005;}
		}
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTC0006", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram....//
void CircleTangent2Circle::PartProgramData()
{
	try
	{
		CurrentShape = (Circle*)(MAINDllOBJECT->getShapesList().selectedItem());
		CurrentShape->IsValid(false);
		circle_radius = ((Circle*)CurrentShape)->Radius();
		double mp[2] = {((Circle*)CurrentShape)->getCenter()->getX(), ((Circle*)CurrentShape)->getCenter()->getY()};

		list<BaseItem*>::iterator ptr = CurrentShape->getParents().end(); 
		ptr--;
		ParentShape = (Circle*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()];
		ParentShape->getCenter()->FillDoublePointer(&center[0]);
		radius = ParentShape->Radius();
		baseaction = PPCALCOBJECT->getFrameGrab();
		if(baseaction->CurrentWindowNo == 1)
		{
			if(baseaction->Pointer_SnapPt != NULL)
				setBaseRProperty(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
			else
				setBaseRProperty(mp[0], mp[1], MAINDllOBJECT->getCurrentDRO().getZ());
			((ShapeWithList*)CurrentShape)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
			PPCALCOBJECT->partProgramReached();
			return;
		}
		setClicksDone(getMaxClicks() - 1);
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTC0007", __FILE__, __FUNCSIG__); }
}

//Max mouse click..//
void CircleTangent2Circle::LmaxmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getMaxClicks() - 1);
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
		CurrentShape = new Circle();
		((Circle*)CurrentShape)->CircleType = RapidEnums::CIRCLETYPE::TANCIRCLETO_CIRCLE;
		((Circle*)CurrentShape)->Radius(circle_radius);
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTC0008", __FILE__, __FUNCSIG__); }
}
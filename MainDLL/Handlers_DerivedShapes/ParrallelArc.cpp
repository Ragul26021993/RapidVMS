#include "stdafx.h"
#include "ParrallelArc.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor..//
ParallelArc::ParallelArc()
{
	try
	{
		init();
		setMaxClicks(2);
		/*MAINDllOBJECT->SetStatusCode("DA001");*/
		MAINDllOBJECT->SetStatusCode("ParallelArc01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PAARCMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor..//
ParallelArc::~ParallelArc()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PAARCMH0002", __FILE__, __FUNCSIG__); }
}

void ParallelArc::ResetShapeHighlighted()
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

void ParallelArc::init()
{
	try
	{
		runningPartprogramValid = false;
		ParentShape = NULL;
		offset = 0;
		DefinedOffset = false;
		resetClicks();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PAARCMH0003", __FILE__, __FUNCSIG__); }
}
//Mousemove....//
void ParallelArc::mouseMove()
{
	try
	{
		if(getClicksDone() == 1)
			circle_radius = RMATH2DOBJECT->Pt2Pt_distance(center[0], center[1], getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PAARCMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the left mousedown....//
void ParallelArc::LmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone() - 1);
		if(getClicksDone() == 1 && MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone( getClicksDone() - 1);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if(Csh->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			ParentShape = (Circle*)Csh;
			ParentShape->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		MAINDllOBJECT->DerivedShapeCallback();
		ParentShape->getCenter()->FillDoublePointer(&center[0],3);
		radius = ParentShape->Radius();
		startang = ParentShape->Startangle();
		sweepang = ParentShape->Sweepangle();
		MAINDllOBJECT->SetStatusCode("DA002");
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PAARCMH0005", __FILE__, __FUNCSIG__); }
}

//Draw the parallel arc..//
void ParallelArc::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() == 1)
			GRAFIX->drawArc(center[0], center[1], circle_radius, startang, sweepang, WPixelWidth);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PAARCMH0006", __FILE__, __FUNCSIG__); }
}

void ParallelArc::EscapebuttonPress()
{
	ResetShapeHighlighted();
	init();
}

void ParallelArc::SetAnyData(double *data)
{
	try
	{		
		if(data[0] >= 0 || (data[0] < 0 && data[0] > -radius))
		{
			offset = data[0];
			DefinedOffset = true;
			LmaxmouseDown();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARAMH0009", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data.....//
void ParallelArc::PartProgramData()
{
	try
	{
		CurrentShape = (Circle*)(MAINDllOBJECT->getShapesList().selectedItem());
		CurrentShape->IsValid(false);
		circle_radius = ((Circle*)CurrentShape)->Radius();
		double mp[3] = {((Circle*)CurrentShape)->getCenter()->getX(), ((Circle*)CurrentShape)->getCenter()->getY(), ((Circle*)CurrentShape)->getCenter()->getZ()};
		DefinedOffset = ((Circle*)CurrentShape)->OffsetDefined;
		offset = ((Circle*)CurrentShape)->Offset();
		list<BaseItem*>::iterator ptr = CurrentShape->getParents().end(); 
		ptr--;
		ParentShape = (Circle*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()];
		ParentShape->getCenter()->FillDoublePointer(&center[0], 3);
		radius = ParentShape->Radius();
		startang = ParentShape->Startangle();
		sweepang = ParentShape->Sweepangle();
		if(DefinedOffset)
		{
			double tmpang = startang + sweepang / 2;
			double tmpPoint[3] = {0}, dir[3] = {cos(tmpang), sin(tmpang), 0};
			circle_radius = offset + radius;
			for(int i = 0; i < 3; i++)
			{
			    tmpPoint[i] = center[i] + circle_radius * dir[i];
			}
			setBaseRProperty(tmpPoint[0], tmpPoint[1], tmpPoint[2]);
			((ShapeWithList*)CurrentShape)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
			PPCALCOBJECT->partProgramReached();
			return;
		}
		else if(PPCALCOBJECT->getFrameGrab()->CurrentWindowNo == 1)
		{
			if(baseaction->Pointer_SnapPt != NULL)
				setBaseRProperty(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
			else
				setBaseRProperty(mp[0], mp[1], mp[2]);
			((ShapeWithList*)CurrentShape)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
			PPCALCOBJECT->partProgramReached();
			return;
		}
		setClicksDone(getMaxClicks() - 1);
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PAARCMH0008", __FILE__, __FUNCSIG__); }
}

//Handle the max click...
//Set the  parallel arc parameters...
void ParallelArc::LmaxmouseDown()
{
	try
	{
		if(DefinedOffset)
		{
			double tmpang = startang + sweepang / 2;
			double tmpPoint[3] = {0}, dir[3] = {cos(tmpang), sin(tmpang), 0};
			circle_radius = offset + radius;
			for(int i = 0; i < 3; i++)
			{
			    tmpPoint[i] = center[i] + circle_radius * dir[i];
			}
			setBaseRProperty(tmpPoint[0], tmpPoint[1], tmpPoint[2]);
		}
		else
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
			setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
     	}
		
		CurrentShape = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
		((Circle*)CurrentShape)->CircleType = RapidEnums::CIRCLETYPE::PARALLELARCCIR;
		((Circle*)CurrentShape)->Radius(circle_radius);
		if(DefinedOffset) 
		{
			((Circle*)CurrentShape)->OffsetDefined = true;
			((Circle*)CurrentShape)->Offset(offset);
		}
		ParentShape->addChild(CurrentShape);
		CurrentShape->addParent(ParentShape);
		AddShapeAction::addShape(CurrentShape);
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
		ResetShapeHighlighted();
		init();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PAARCMH0009", __FILE__, __FUNCSIG__); }
}
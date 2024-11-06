#include "stdafx.h"
#include "LineParallelTangent2Circle.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Circle.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor...//
LineParallelTangent2Circle::LineParallelTangent2Circle()
{
	init();
	setMaxClicks(3);
	MAINDllOBJECT->SetStatusCode("LineParallelTangent2Circle01");
}

LineParallelTangent2Circle::~LineParallelTangent2Circle()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC0001", __FILE__, __FUNCSIG__); }
}

void LineParallelTangent2Circle::ResetShapeHighlighted()
{
	try
	{
		if(Cshape[0] != NULL)
			Cshape[0]->HighlightedForMeasurement(false);
		if(Cshape[1] != NULL)
			Cshape[1]->HighlightedForMeasurement(false);
		MAINDllOBJECT->Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC0001", __FILE__, __FUNCSIG__); }
}

void LineParallelTangent2Circle::init()
{
	try
	{
		runningPartprogramValid = false;
		validflag = false;
		ShapeCnt = 0;
		p_Dist = 0;
		Linelength = 1;
		resetClicks();
		myLineFirst = NULL;  myLineSecnd = NULL;
		Cshape[0] = NULL; Cshape[1] = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC0001", __FILE__, __FUNCSIG__); }
}

void LineParallelTangent2Circle::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if(Csh->ShapeType ==  RapidEnums::SHAPETYPE::CIRCLE || Csh->ShapeType ==  RapidEnums::SHAPETYPE::ARC)
		{
			Cshape[ShapeCnt++] = Csh;
			Cshape[ShapeCnt - 1]->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
		}
		else //Wrong proceedure...
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(getClicksDone() == 2)
		{
			getCircledimensions(Cshape[0], Cshape[1]);
			MAINDllOBJECT->DerivedShapeCallback();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC0002", __FILE__, __FUNCSIG__); }
}

void LineParallelTangent2Circle::RmouseDown(double x, double y)
{
	p_Dist = -1 * p_Dist;
	mouseMove();
}

void LineParallelTangent2Circle::mouseMove()
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1 && validflag)
		{
			double mp[2] = {getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY()};
			//Call the Function. To calculate the line..
			BESTFITOBJECT->fnparallel_tangent_2_arcs(Circle1, Circle2, p_Dist, Linelength, mp[0], mp[1], &Line1, &Line2);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC0003", __FILE__, __FUNCSIG__); }
}

void LineParallelTangent2Circle::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1 && validflag)
		{
			GRAFIX->drawLine(Line1.start_pt.x, Line1.start_pt.y, Line1.end_pt.x, Line1.end_pt.y);
			GRAFIX->drawLine(Line2.start_pt.x, Line2.start_pt.y, Line2.end_pt.x, Line2.end_pt.y);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC0004", __FILE__, __FUNCSIG__); }
}

void LineParallelTangent2Circle::EscapebuttonPress()
{
	ResetShapeHighlighted();
	init();
}

void LineParallelTangent2Circle::getCircledimensions(Shape* Csh1, Shape* Csh2)
{
	try
	{
		Circle1.Center_X = ((Circle*)Csh1)->getCenter()->getX();
		Circle1.Center_Y = ((Circle*)Csh1)->getCenter()->getY();
		Circle1.radius = ((Circle*)Csh1)->Radius();

		Circle2.Center_X = ((Circle*)Csh2)->getCenter()->getX();
		Circle2.Center_Y = ((Circle*)Csh2)->getCenter()->getY();
		Circle2.radius = ((Circle*)Csh2)->Radius();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC0005", __FILE__, __FUNCSIG__); }
}

void LineParallelTangent2Circle::SetAnyData(double *data)
{
	try
	{
		double d = data[0];
		p_Dist = d;
		Linelength = data[1];
		double mp[2] = {getClicksValue(getClicksDone() - 1).getX(), getClicksValue(getClicksDone() - 1).getY()};
		bool flag = false; 
		//Check whether there is possible line for the given distance..
		
		flag = BESTFITOBJECT->fnparallel_tangent_2_arcs(Circle1, Circle2, p_Dist, Linelength, mp[0], mp[1], &Line1, &Line2);
		if(flag) // Continue..
		{
			validflag = true;
		}
		else
		{
			init();
			MAINDllOBJECT->ShowMsgBoxString("LineParallelTangent2CircleSetAnyData01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC0006", __FILE__, __FUNCSIG__); }
}

//Increase/decrease the diameter for ouse scroll event....//
void LineParallelTangent2Circle::MouseScroll(bool flag)
{
	try
	{
		if(flag) Linelength += 0.15;
		else {if(Linelength > 0.5) Linelength -= 0.15; }
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC0007", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram....//
void LineParallelTangent2Circle::PartProgramData()
{
	try
	{
		myLineFirst = (Line*)(MAINDllOBJECT->getShapesList().selectedItem());
		myLineSecnd = (Line*)(*myLineFirst->getChild().begin());
		myLineFirst->IsValid(false);
		myLineSecnd->IsValid(false);
		
		list<BaseItem*>::iterator pShape = myLineFirst->getParents().begin();
		Cshape[0] = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*pShape++)->getId()];
		Cshape[1] = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*pShape)->getId()];
		getCircledimensions(Cshape[0], Cshape[1]);
		p_Dist = ((Line*)myLineFirst)->Offset();
		Linelength = ((Line*)myLineFirst)->Length();
	
		double mpoint[2] = {((Line*)myLineFirst)->getMidPoint()->getX(), ((Line*)myLineFirst)->getMidPoint()->getY()};

		bool flag = false; 
		flag = BESTFITOBJECT->fnparallel_tangent_2_arcs(Circle1, Circle2, p_Dist, Linelength, mpoint[0], mpoint[1], &Line1, &Line2);
		//Check whether there is possible line for the given distance..

		if(flag) // Continue..
		{
			validflag = true;
		}
		else
		{
			init();
			PPCALCOBJECT->partProgramReached();
		}
		setClicksDone(getMaxClicks() - 1);
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC0008", __FILE__, __FUNCSIG__); }
}

//Max mouse click..//
void LineParallelTangent2Circle::LmaxmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getMaxClicks() - 1);
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				init();
				PPCALCOBJECT->partProgramReached();
			}
			else
				setClicksDone(getMaxClicks() - 1);
			return;
		}
		myLineFirst = new Line();
		myLineFirst->LineType = RapidEnums::LINETYPE::PARALLEL_TANGENT2TWOCIRLE;
		AddShapeAction::addShape(myLineFirst);
		baseaction->AllPointsList.deleteAll();
		AddPointAction::pointAction(myLineFirst, baseaction);
		baseaction->AllPointsList.Addpoint(new SinglePoint(Line1.start_pt.x, Line1.start_pt.y, ((Circle*)Cshape[0])->getCenter()->getZ()));
		baseaction->AllPointsList.Addpoint(new SinglePoint(Line1.end_pt.x, Line1.end_pt.y, ((Circle*)Cshape[0])->getCenter()->getZ()));
		((ShapeWithList*)myLineFirst)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
		((Line*)myLineFirst)->Offset(p_Dist);
		((Line*)myLineFirst)->UpdateBestFit();


		myLineSecnd = new Line();
		myLineSecnd->LineType = RapidEnums::LINETYPE::PARALLEL_TANGENT2TWOCIRLE;
		AddShapeAction::addShape(myLineSecnd);
		baseaction->AllPointsList.deleteAll();
		baseaction->AllPointsList.Addpoint(new SinglePoint(Line2.start_pt.x, Line2.start_pt.y, ((Circle*)Cshape[0])->getCenter()->getZ()));
		baseaction->AllPointsList.Addpoint(new SinglePoint(Line2.end_pt.x, Line2.end_pt.y, ((Circle*)Cshape[0])->getCenter()->getZ()));
		((ShapeWithList*)myLineSecnd)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
		((Line*)myLineSecnd)->UpdateBestFit();

		Cshape[0]->addChild(myLineFirst);
		Cshape[1]->addChild(myLineFirst);
		Cshape[0]->addChild(myLineSecnd);
		Cshape[1]->addChild(myLineSecnd);

		myLineFirst->addParent(Cshape[0]);
		myLineFirst->addParent(Cshape[1]);
		myLineSecnd->addParent(Cshape[0]);
		myLineSecnd->addParent(Cshape[1]);

		myLineFirst->addChild(myLineSecnd);
		
		myLineFirst->IsValid(true);
		myLineSecnd->IsValid(true);
		ResetShapeHighlighted();
		init();
		MAINDllOBJECT->DerivedShapeCallback();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC0009", __FILE__, __FUNCSIG__); }
}

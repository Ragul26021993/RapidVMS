#include "stdafx.h"
#include "CircleTangent2TwoLine.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor...//
CircleTangent2TwoLine::CircleTangent2TwoLine()
{
	init();
	setMaxClicks(3);
	MAINDllOBJECT->SetStatusCode("CircleTangent2TwoLine01");
}

//Destructor..
CircleTangent2TwoLine::~CircleTangent2TwoLine()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLL0001", __FILE__, __FUNCSIG__); }
}

void CircleTangent2TwoLine::ResetShapeHighlighted()
{
	try
	{
		if(ParentShape1 != NULL)
			ParentShape1->HighlightedForMeasurement(false);
		if(ParentShape2 != NULL)
			ParentShape2->HighlightedForMeasurement(false);
		MAINDllOBJECT->Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLL0001", __FILE__, __FUNCSIG__); }
}

//Initialise flags..counters..
void CircleTangent2TwoLine::init()
{
	try
	{
		runningPartprogramValid = false;
		validflag = false;
		resetClicks();
		ParentShape1 = NULL; ParentShape2 = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLL0001", __FILE__, __FUNCSIG__); }
}

//
void CircleTangent2TwoLine::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if(Csh->ShapeType ==  RapidEnums::SHAPETYPE::LINE || Csh->ShapeType ==  RapidEnums::SHAPETYPE::XLINE)
		{
			if(getClicksDone() == 1)
				ParentShape1 = (Line*)Csh;
			else
				ParentShape2 = (Line*)Csh;
			Csh->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
		}
		else //Wrong proceedure...
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(getClicksDone() == 2)
		{
			getLineDimensions();
			MAINDllOBJECT->DerivedShapeCallback();
		}
		MAINDllOBJECT->ClearShapeSelections();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLL0002", __FILE__, __FUNCSIG__); }
}

void CircleTangent2TwoLine::mouseMove()
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1 && validflag)
		{
			double mp[2] = {getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY()};
			RMATH2DOBJECT->Circle_Tangent2TwoLines(angle1, intercept1, angle2, intercept2, &circle_rad, &mp[0], &center_ans[0]);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLL0003", __FILE__, __FUNCSIG__); }
}

void CircleTangent2TwoLine::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1 && validflag)
		{
			GRAFIX->drawCircle(center_ans[0], center_ans[1], circle_rad, true);
			drawExtensionForTangentialCircle(center_ans[0], center_ans[1], circle_rad);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLL0004", __FILE__, __FUNCSIG__); }
}

void CircleTangent2TwoLine::drawExtensionForTangentialCircle(double x, double y, double r)
{
	try
	{
		double Line1point1[2] = {ParentShape1->getPoint1()->getX(), ParentShape1->getPoint1()->getY()};
		double Line1point2[2] = {ParentShape1->getPoint2()->getX(), ParentShape1->getPoint2()->getY()};
		double Line2point1[2] = {ParentShape2->getPoint1()->getX(), ParentShape2->getPoint1()->getY()};
		double Line2point2[2] = {ParentShape2->getPoint2()->getX(), ParentShape2->getPoint2()->getY()};
		double intersect1[4], intersect2[4], topleft[2], bottomRight[2];
		double center[2] = {x, y};
		int Cnt[2];
		bool drawExtensionline[2];
		if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::LINE)
		{
			RMATH2DOBJECT->Finiteline_circle(&Line1point1[0], &Line1point2[0], angle1, intercept1, &center[0], r, &intersect1[0], &intersect2[0], &Cnt[0]);
			if(Cnt[1] == 1) drawExtensionline[0] = true;
			else drawExtensionline[0] = false;
			if(drawExtensionline[0])
			{
				if(RMATH2DOBJECT->Pt2Pt_distance(intersect2[0], intersect2[1], Line1point1[0], Line1point1[1]) < RMATH2DOBJECT->Pt2Pt_distance(intersect2[0], intersect2[1], Line1point2[0], Line1point2[1]))
					GRAFIX->drawLineStipple(intersect2[0], intersect2[1], Line1point1[0], Line1point1[1]);
				else
					GRAFIX->drawLineStipple(intersect2[0], intersect2[1], Line1point2[0], Line1point2[1]);
			}
		}
		Cnt[0] = 0; Cnt[1] = 0;
		if(ParentShape2->ShapeType == RapidEnums::SHAPETYPE::LINE)
		{
			RMATH2DOBJECT->Finiteline_circle(&Line2point1[0], &Line2point2[0], angle2, intercept2, &center[0], r, &intersect1[0], &intersect2[0], &Cnt[0]);
			if(Cnt[1] == 1) drawExtensionline[1] = true;
			else drawExtensionline[1] = false;
			if(drawExtensionline[1])
			{
				if(RMATH2DOBJECT->Pt2Pt_distance(intersect2[0], intersect2[1], Line2point1[0], Line2point1[1]) < RMATH2DOBJECT->Pt2Pt_distance(intersect2[0], intersect2[1], Line2point2[0], Line2point2[1]))
					GRAFIX->drawLineStipple(intersect2[0], intersect2[1], Line2point1[0], Line2point1[1]);
				else
					GRAFIX->drawLineStipple(intersect2[0], intersect2[1], Line2point2[0], Line2point2[1]);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLL0005", __FILE__, __FUNCSIG__); }
}

void CircleTangent2TwoLine::EscapebuttonPress()
{
	ResetShapeHighlighted();
	init();
}

void CircleTangent2TwoLine::getLineDimensions()
{
	try
	{
		angle1 = ParentShape1->Angle();
		intercept1 = ParentShape1->Intercept();
		angle2 = ParentShape2->Angle();
		intercept2 = ParentShape2->Intercept();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLL0006", __FILE__, __FUNCSIG__); }
}


void CircleTangent2TwoLine::SetAnyData(double *data)
{
	circle_rad = data[0] / 2;
	validflag = true;
}


//Increase/decrease the diameter for ouse scroll event....//
void CircleTangent2TwoLine::MouseScroll(bool flag)
{
	try
	{
		if(flag) circle_rad += 0.002;
		else
		{
			if(circle_rad > 0.05) circle_rad -= 0.002;
			else circle_rad = 0.05;
		}
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLL0007", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram....//
void CircleTangent2TwoLine::PartProgramData()
{
	try
	{
		CurrentShape = (Circle*)(MAINDllOBJECT->getShapesList().selectedItem());
		CurrentShape->IsValid(false);
		circle_rad = ((Circle*)CurrentShape)->Radius();
		double ccen[2] = {((Circle*)CurrentShape)->getCenter()->getX(), ((Circle*)CurrentShape)->getCenter()->getY()};

		list<BaseItem*>::iterator ptr = CurrentShape->getParents().end(); 
		ptr--;
		ParentShape1 = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr--)->getId()];
		ParentShape2 = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()];
		getLineDimensions();

		RMATH2DOBJECT->Circle_Tangent2TwoLines(angle1, intercept1, angle2, intercept2, &circle_rad, &ccen[0], &center_ans[0]);
		((Circle*)CurrentShape)->setCenter(Vector(center_ans[0], center_ans[1], 0));
		((Circle*)CurrentShape)->Radius(circle_rad);
		setMaxClicks(3);
		baseaction = PPCALCOBJECT->getFrameGrab();
		if(baseaction->CurrentWindowNo == 1)
		{
			if(baseaction->Pointer_SnapPt != NULL)
				setBaseRProperty(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
			else
				setBaseRProperty(center_ans[0], center_ans[1], MAINDllOBJECT->getCurrentDRO().getZ());
			((ShapeWithList*)CurrentShape)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
			setClicksDone(0);
			return;
		}
		setClicksDone(getMaxClicks() - 1);
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLL0008", __FILE__, __FUNCSIG__); }
}

//Max mouse click..//
void CircleTangent2TwoLine::LmaxmouseDown()
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
				((Circle*)CurrentShape)->setCenter(Vector(center_ans[0], center_ans[1], 0));
				((Circle*)CurrentShape)->Radius(circle_rad);
				((Circle*)CurrentShape)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
				init();
				PPCALCOBJECT->partProgramReached();
			}
			else
				setClicksDone(getMaxClicks() - 1);
			return;
		}
		CurrentShape = new Circle();
		((Circle*)CurrentShape)->CircleType = RapidEnums::CIRCLETYPE::TANCIRCLETO_2LINES;
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		ParentShape1->addChild(CurrentShape);
		ParentShape2->addChild(CurrentShape);
		CurrentShape->addParent(ParentShape1);
		CurrentShape->addParent(ParentShape2);
		AddShapeAction::addShape(CurrentShape);
		((Circle*)CurrentShape)->setCenter(Vector(center_ans[0], center_ans[1], 0));
		((Circle*)CurrentShape)->Radius(circle_rad);
		if(baseaction->CurrentWindowNo == 1)
			baseaction->DroMovementFlag = false;
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
		ResetShapeHighlighted();
		init();
		MAINDllOBJECT->DerivedShapeCallback();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLL0009", __FILE__, __FUNCSIG__); }
}
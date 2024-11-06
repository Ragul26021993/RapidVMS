#include "stdafx.h"
#include "CircleTangent2TwoCircle2.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor...//
CircleTangent2TwoCircle2::CircleTangent2TwoCircle2()
{
	init();
	setMaxClicks(3);
	MAINDllOBJECT->SetStatusCode("CircleTangent2TwoCircle203");
}


CircleTangent2TwoCircle2::~CircleTangent2TwoCircle2()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC20001", __FILE__, __FUNCSIG__); }
}

void CircleTangent2TwoCircle2::ResetShapeHighlighted()
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

void CircleTangent2TwoCircle2::init()
{
	try
	{
		runningPartprogramValid = false;
		validflag = false;
		Minradius = 0;
		resetClicks();
		ParentShape1 = NULL; ParentShape2 = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC20001", __FILE__, __FUNCSIG__); }
}

void CircleTangent2TwoCircle2::LmouseDown()
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
			if(getClicksDone() == 1)
				ParentShape1 = (Circle*)Csh;
			else
				ParentShape2 = (Circle*)Csh;
			Csh->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
		}
		else //Wrong proceedure...
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(getClicksDone() == 2)
			CheckAllConditions();
		MAINDllOBJECT->ClearShapeSelections();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC20002", __FILE__, __FUNCSIG__); }
}

void CircleTangent2TwoCircle2::CheckAllConditions()
{
	try
	{
		getCircledimensions();
		double Dist = RMATH2DOBJECT->Pt2Pt_distance(&center1[0], &center2[0]);
		if(Dist < 0.0001)
		{
			init();
			MAINDllOBJECT->SetStatusCode("CircleTangent2TwoCircle201");
			return;
		}
		bool flag = RMATH2DOBJECT->CircleInsideAnotherCircle(&center1[0], radius1, &center2[0], radius2);
		if(flag) // One Circle is inside other..
		{
			init();
			MAINDllOBJECT->SetStatusCode("CircleTangent2TwoCircle202");
			return;
		}
		Minradius = (Dist + radius1 + radius2)/2;
		if(!PPCALCOBJECT->IsPartProgramRunning())
			MAINDllOBJECT->DerivedShapeCallback();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC20002", __FILE__, __FUNCSIG__); }
}

void CircleTangent2TwoCircle2::mouseMove()
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1 && validflag)
		{
			double mp[2] = {getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY()};
			RMATH2DOBJECT->Circle_Tangent2Circles_Outside(&center1[0], radius1, &center2[0], radius2, &mp[0], &circle_rad, &center_ans[0]);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC20003", __FILE__, __FUNCSIG__); }
}

void CircleTangent2TwoCircle2::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1 && validflag)
			GRAFIX->drawCircle(center_ans[0], center_ans[1], circle_rad, true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC20004", __FILE__, __FUNCSIG__); }
}

void CircleTangent2TwoCircle2::EscapebuttonPress()
{
	ResetShapeHighlighted();
	init();
}

void CircleTangent2TwoCircle2::getCircledimensions()
{
	try
	{
		ParentShape1->getCenter()->FillDoublePointer(&center1[0]);
		radius1 = ParentShape1->Radius();
		ParentShape2->getCenter()->FillDoublePointer(&center2[0]);
		radius2 = ParentShape2->Radius();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC20005", __FILE__, __FUNCSIG__); }
}

void CircleTangent2TwoCircle2::SetAnyData(double *data)
{
	try
	{
		double d = data[0];
		if(d < 2 * Minradius)
		{
			circle_rad = Minradius;
			std::string unitType;
			if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
				unitType = "In";
			else if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::MM)
				unitType = "mm";
			char mindia[20];
			RMATH2DOBJECT->Double2String(2 * Minradius, MAINDllOBJECT->MeasurementNoOfDecimal(), &mindia[0]); 
			std::string Dia = (const char*)(mindia) + unitType;
			MAINDllOBJECT->ShowMsgBoxString("CircleTangent2TwoCircle2SetAnyData01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION, true, Dia);
		}
		else
			circle_rad = d/2;
		validflag = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC20006", __FILE__, __FUNCSIG__); }

}

//Increase/decrease the diameter for ouse scroll event....//
void CircleTangent2TwoCircle2::MouseScroll(bool flag)
{
	try
	{
		if(flag) circle_rad += 0.002;
		else
		{
			if(circle_rad > Minradius) circle_rad -= 0.002;
			else circle_rad = Minradius;
		}
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC20007", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram....//
void CircleTangent2TwoCircle2::PartProgramData()
{
	try
	{
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
		CurrentShape->IsValid(false);
		circle_rad = ((Circle*)CurrentShape)->Radius();
		
		list<BaseItem*>::iterator ptr = CurrentShape->getParents().end(); 
		ptr--;
		ParentShape1 = (Circle*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr--)->getId()];
		ParentShape2 = (Circle*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()];
		CheckAllConditions();

		double ccen[2] = {((Circle*)CurrentShape)->getCenter()->getX(), ((Circle*)CurrentShape)->getCenter()->getY()};
		if(circle_rad < Minradius) circle_rad = Minradius;

		RMATH2DOBJECT->Circle_Tangent2Circles_Outside(&center1[0], radius1, &center2[0], radius2, &ccen[0], &circle_rad, &center_ans[0]);
		((Circle*)CurrentShape)->Radius(circle_rad);
		((Circle*)CurrentShape)->setCenter(Vector(center_ans[0], center_ans[1], ((Circle*)CurrentShape)->getCenter()->getZ()));
		setMaxClicks(3); 
		baseaction = PPCALCOBJECT->getFrameGrab();
		if(baseaction->CurrentWindowNo == 1)
		{
			if(baseaction->Pointer_SnapPt != NULL)
				setBaseRProperty(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
			else
				setBaseRProperty(center_ans[0], center_ans[1], MAINDllOBJECT->getCurrentDRO().getZ());
			((ShapeWithList*)CurrentShape)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
			PPCALCOBJECT->partProgramReached();
			return;
		}
		setClicksDone(getMaxClicks() - 1);
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC20008", __FILE__, __FUNCSIG__); }
}

//Max mouse click..//
void CircleTangent2TwoCircle2::LmaxmouseDown()
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
		((Circle*)CurrentShape)->CircleType = RapidEnums::CIRCLETYPE::TANCIRCLETO_2CIRCLES_OUT;
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTCC20009", __FILE__, __FUNCSIG__); }
}
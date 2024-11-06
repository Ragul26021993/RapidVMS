#include "stdafx.h"
#include "CircleTangent2LineCircle.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor...//
CircleTangent2LineCircle::CircleTangent2LineCircle()
{
	init();
	setMaxClicks(3);
	MAINDllOBJECT->SetStatusCode("CircleTangent2LineCircle01");
}

//Destructor..
CircleTangent2LineCircle::~CircleTangent2LineCircle()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLC0001", __FILE__, __FUNCSIG__); }
}

void CircleTangent2LineCircle::ResetShapeHighlighted()
{
	try
	{
		if(ParentLine != NULL)
			ParentLine->HighlightedForMeasurement(false);
		if(ParentCircle != NULL)
			ParentCircle->HighlightedForMeasurement(false);
		MAINDllOBJECT->Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLC0001", __FILE__, __FUNCSIG__); }
}

//Initialise settings..
void CircleTangent2LineCircle::init()
{
	try
	{
		runningPartprogramValid = false;
		Intersecting = false;
		validflag = false;
		LineSelected = false;
		CircleSelected = false;
		Minradius = 0;
		resetClicks();
		ParentLine = NULL; ParentCircle = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLC0001", __FILE__, __FUNCSIG__); }
}

//Handle the left mouse down.. Select the required shapes...
void CircleTangent2LineCircle::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if(Csh->ShapeType == RapidEnums::SHAPETYPE::LINE ||Csh->ShapeType == RapidEnums::SHAPETYPE::XLINE)
		{
			if(!LineSelected)
			{
				LineSelected =  true;
				ParentLine = (Line*)Csh;
				ParentLine->HighlightedForMeasurement(true);
				MAINDllOBJECT->Shape_Updated();
			}
			else
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
		}
		else if(Csh->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || Csh->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			if(!CircleSelected)
			{
				CircleSelected = true;
				ParentCircle = (Circle*)Csh;
				ParentCircle->HighlightedForMeasurement(true);
				MAINDllOBJECT->Shape_Updated();
			}
			else
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(getClicksDone() == 2)
		{
			getShapeDimensions();
			double Dist = RMATH2DOBJECT->Pt2Line_Dist(cen[0], cen[1], angle, intercept);
			if((Dist - rad) <= 0) Intersecting = true;
			else Intersecting = false;
			if(!Intersecting)
				Minradius = (Dist - rad) / 2;
			MAINDllOBJECT->DerivedShapeCallback();
		}
		MAINDllOBJECT->ClearShapeSelections();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLC0002", __FILE__, __FUNCSIG__); }
}

void CircleTangent2LineCircle::mouseMove()
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1 && validflag)
		{
			double mp[2] = {getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY()};
			RMATH2DOBJECT->Circle_Tangent2LineCircle(&cen[0], rad, angle, intercept, &mp[0], circle_rad, &center_ans[0]);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLC0003", __FILE__, __FUNCSIG__); }
}

void CircleTangent2LineCircle::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1 && validflag)
		{
			GRAFIX->drawCircle(center_ans[0], center_ans[1], circle_rad, true);
			drawExtensionForTangentialCircle(center_ans[0], center_ans[1], circle_rad);			
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLC0004", __FILE__, __FUNCSIG__); }
}

void CircleTangent2LineCircle::drawExtensionForTangentialCircle(double x, double y, double r)
{
	try
	{
		double intersect1[2], cnter[2] = {x, y};
		double Linepoint1[2] = {ParentLine->getPoint1()->getX(), ParentLine->getPoint1()->getY()};
		double Linepoint2[2] = {ParentLine->getPoint2()->getX(), ParentLine->getPoint2()->getY()};
		RMATH2DOBJECT->Line_circleintersection(ParentLine->Angle(), ParentLine->Intercept(), &cnter[0], r, &intersect1[0]);
		bool flag = RMATH2DOBJECT->Ptisinwindow(&intersect1[0], &Linepoint1[0], &Linepoint2[0]);
		if(!flag)
		{
			if(RMATH2DOBJECT->Pt2Pt_distance(intersect1[0], intersect1[1], Linepoint1[0], Linepoint1[1]) < RMATH2DOBJECT->Pt2Pt_distance(intersect1[0], intersect1[1], Linepoint2[0], Linepoint2[1]))
				GRAFIX->drawLineStipple(intersect1[0], intersect1[1], Linepoint1[0], Linepoint1[1]);
			else
				GRAFIX->drawLineStipple(intersect1[0], intersect1[1], Linepoint2[0], Linepoint2[1]);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLC0005", __FILE__, __FUNCSIG__); }
}

void CircleTangent2LineCircle::EscapebuttonPress()
{
	ResetShapeHighlighted();
	init();
}

void CircleTangent2LineCircle::getShapeDimensions()
{
	try
	{
		ParentCircle->getCenter()->FillDoublePointer(&cen[0]);
		rad = ParentCircle->Radius();
		angle = ParentLine->Angle();
		intercept = ParentLine->Intercept();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLC0006", __FILE__, __FUNCSIG__); }
}

void CircleTangent2LineCircle::SetAnyData(double *data)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLC0008", __FILE__, __FUNCSIG__); }
}

//Increase/decrease the diameter for ouse scroll event....//
void CircleTangent2LineCircle::MouseScroll(bool flag)
{
	try
	{
		if(flag) circle_rad += 0.002;
		else
		{
			if(Intersecting)
			{
				if(circle_rad > 0.05) circle_rad -= 0.002;
			}
			else 
			{
				if(circle_rad > Minradius) circle_rad -= 0.002;
				else circle_rad = Minradius;
			}
		}
		mouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLC0009", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram....//
void CircleTangent2LineCircle::PartProgramData()
{
	try
	{
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
		CurrentShape->IsValid(false);
		circle_rad = ((Circle*)CurrentShape)->Radius();

		list<BaseItem*>::iterator ptr = CurrentShape->getParents().end(); 
		ptr--;
		ParentCircle = (Circle*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr--)->getId()];
		ParentLine = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()];
		getShapeDimensions();
		
		double Dist = RMATH2DOBJECT->Pt2Line_Dist(cen[0], cen[1], angle, intercept);
		if((Dist - rad) <= 0) Intersecting = true;
		else Intersecting = false;
		if(!Intersecting)
			Minradius = (Dist - rad) / 2;

		RMATH2DOBJECT->Circle_Tangent2LineCircle(&cen[0], rad, angle, intercept, &cen[0], circle_rad, &center_ans[0]);
		((Circle*)CurrentShape)->Radius(circle_rad);
		((Circle*)CurrentShape)->setCenter(Vector(center_ans[0], center_ans[1], ((Circle*)CurrentShape)->getCenter()->getZ()));
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
		validflag = true;
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLC0010", __FILE__, __FUNCSIG__); }
}

//Max mouse click..//
void CircleTangent2LineCircle::LmaxmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getMaxClicks() - 1);
		//Part program handling............
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				((Circle*)CurrentShape)->setCenter(Vector(center_ans[0], center_ans[1], 0));
				((Circle*)CurrentShape)->Radius(circle_rad);
				((Circle*)CurrentShape)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
				PPCALCOBJECT->partProgramReached();
			}
			else
				setClicksDone(getMaxClicks() - 1);
			return;
		}	
		CurrentShape = new Circle();
		((Circle*)CurrentShape)->CircleType = RapidEnums::CIRCLETYPE::TANCIRCLETO_LINECIRCLE;
		((Circle*)CurrentShape)->setCenter(Vector(center_ans[0], center_ans[1], 0));
		((Circle*)CurrentShape)->Radius(circle_rad);
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());

		ParentLine->addChild(CurrentShape);
		ParentCircle->addChild(CurrentShape);
		CurrentShape->addParent(ParentLine);
		CurrentShape->addParent(ParentCircle);
		AddShapeAction::addShape(CurrentShape);
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
		MAINDllOBJECT->DerivedShapeCallback();
		ResetShapeHighlighted();
		init();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLC0011", __FILE__, __FUNCSIG__); }
}
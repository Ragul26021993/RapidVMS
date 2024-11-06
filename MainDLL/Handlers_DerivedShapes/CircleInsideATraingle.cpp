#include "stdafx.h"
#include "CircleInsideATraingle.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Helper\Helper.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor...//
CircleInsideATraingle::CircleInsideATraingle(bool InsideTriangle)
{
	Inside_Triangle = InsideTriangle;
	init();
	setMaxClicks(4);
	if(InsideTriangle)
		MAINDllOBJECT->SetStatusCode("CircleInsideATraingle01");
	else
		MAINDllOBJECT->SetStatusCode("CircleInsideATraingle02");
}

//Destructor..
CircleInsideATraingle::~CircleInsideATraingle()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CINTA0001", __FILE__, __FUNCSIG__); }
}

void CircleInsideATraingle::ResetShapeHighlighted()
{
	try
	{
		if(ParentShape1 != NULL)
			ParentShape1->HighlightedForMeasurement(false);
		if(ParentShape2 != NULL)
			ParentShape2->HighlightedForMeasurement(false);
		if(ParentShape3 != NULL)
			ParentShape3->HighlightedForMeasurement(false);
		MAINDllOBJECT->Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CINTA0001", __FILE__, __FUNCSIG__); }
}

//Initialise flags..counters..
void CircleInsideATraingle::init()
{
	try
	{
		resetClicks();
		ParentShape1 = NULL; ParentShape2 = NULL; ParentShape3 = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CINTA0001", __FILE__, __FUNCSIG__); }
}

//
void CircleInsideATraingle::LmouseDown()
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
			else if(getClicksDone() == 2)
				ParentShape2 = (Line*)Csh;
			else if(getClicksDone() == 3)
				ParentShape3 = (Line*)Csh;
			Csh->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
		}
		else //Wrong proceedure...
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(getClicksDone() == 3)
		{
			bool CheckFlag1 = HELPEROBJECT->ChecktheParallelism(ParentShape1, ParentShape2);
			bool CheckFlag2 = HELPEROBJECT->ChecktheParallelism(ParentShape2, ParentShape3);
			bool CheckFlag3 = HELPEROBJECT->ChecktheParallelism(ParentShape3, ParentShape1);
			if(!(CheckFlag1 || CheckFlag2 || CheckFlag3))
				AddDerivedCircleToLines();
			else
			{
				ResetShapeHighlighted();
				init();
			}
		}
		MAINDllOBJECT->ClearShapeSelections();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CINTA0002", __FILE__, __FUNCSIG__); }
}

void CircleInsideATraingle::mouseMove()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CINTA0003", __FILE__, __FUNCSIG__); }
}

void CircleInsideATraingle::draw(int windowno, double WPixelWidth)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CINTA0004", __FILE__, __FUNCSIG__); }
}

void CircleInsideATraingle::EscapebuttonPress()
{
	ResetShapeHighlighted();
	init();
}

void CircleInsideATraingle::AddDerivedCircleToLines()
{
	try
	{
		double Circle_Param[3] = {0};
		double angle1 = ParentShape1->Angle(), intercept1 = ParentShape1->Intercept();
		double angle2 = ParentShape2->Angle(), intercept2 = ParentShape2->Intercept();
		double angle3 = ParentShape3->Angle(), intercept3 = ParentShape3->Intercept();
		if(Inside_Triangle)
			RMATH2DOBJECT->Circle_InsideATriangle(angle1, intercept1, angle2, intercept2, angle3, intercept3, &Circle_Param[0]);
		else
			RMATH2DOBJECT->Circle_Circum2Triangle(angle1, intercept1, angle2, intercept2, angle3, intercept3, &Circle_Param[0]);
		Vector* pt = &getClicksValue(getMaxClicks() - 1);
		CurrentShape = new Circle();
		if(Inside_Triangle)
			((Circle*)CurrentShape)->CircleType = RapidEnums::CIRCLETYPE::TANCIRCLETO_TRIANG_IN;
		else
			((Circle*)CurrentShape)->CircleType = RapidEnums::CIRCLETYPE::TANCIRCLETO_TRIANG_OUT;
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		ParentShape1->addChild(CurrentShape);
		ParentShape2->addChild(CurrentShape);
		ParentShape3->addChild(CurrentShape);
		CurrentShape->addParent(ParentShape1);
		CurrentShape->addParent(ParentShape2);
		CurrentShape->addParent(ParentShape3);
		AddShapeAction::addShape(CurrentShape);
		((Circle*)CurrentShape)->setCenter(Vector(Circle_Param[0], Circle_Param[1], 0));
		((Circle*)CurrentShape)->Radius(Circle_Param[2]);
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
		ResetShapeHighlighted();
		init();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CINTA0006", __FILE__, __FUNCSIG__); }
}


//Handle the partprogram....//
void CircleInsideATraingle::PartProgramData()
{
	try
	{
		CurrentShape = (Circle*)(MAINDllOBJECT->getShapesList().selectedItem());
		CurrentShape->IsValid(false);
		if(((Circle*)CurrentShape)->CircleType == RapidEnums::CIRCLETYPE::TANCIRCLETO_TRIANG_IN)
			Inside_Triangle = true;
		else
			Inside_Triangle = false;
		list<BaseItem*>::iterator ptr = CurrentShape->getParents().end(); 
		ptr--;
		ParentShape1 = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr--)->getId()];
		ParentShape2 = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr--)->getId()];
		ParentShape3 = (Line*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()];

		bool CheckFlag1 = HELPEROBJECT->ChecktheParallelism(ParentShape1, ParentShape2);
		bool CheckFlag2 = HELPEROBJECT->ChecktheParallelism(ParentShape2, ParentShape3);
		bool CheckFlag3 = HELPEROBJECT->ChecktheParallelism(ParentShape3, ParentShape1);
		if(CheckFlag1 || CheckFlag2 || CheckFlag3)
		{
			PPCALCOBJECT->partProgramReached();
			return;
		}
		
		double Circle_Param[3] = {0};
		double angle1 = ParentShape1->Angle(), intercept1 = ParentShape1->Intercept();
		double angle2 = ParentShape2->Angle(), intercept2 = ParentShape2->Intercept();
		double angle3 = ParentShape3->Angle(), intercept3 = ParentShape3->Intercept();
		
		if(Inside_Triangle)
			RMATH2DOBJECT->Circle_InsideATriangle(angle1, intercept1, angle2, intercept2, angle3, intercept3, &Circle_Param[0]);
		else
			RMATH2DOBJECT->Circle_Circum2Triangle(angle1, intercept1, angle2, intercept2, angle3, intercept3, &Circle_Param[0]);
		((Circle*)CurrentShape)->setCenter(Vector(Circle_Param[0], Circle_Param[1], 0));
		((Circle*)CurrentShape)->Radius(Circle_Param[2]);
		CurrentShape->notifyAll(ITEMSTATUS::DATACHANGED, CurrentShape);
		resetClicks();
		CurrentShape->IsValid(true);
		PPCALCOBJECT->partProgramReached();	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CINTA0008", __FILE__, __FUNCSIG__); }
}
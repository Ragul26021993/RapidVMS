#include "stdafx.h"
#include "LineArcLineHandler.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor..//
LineArcLineHandler::LineArcLineHandler()
{
	try
	{
		init();
		setMaxClicks(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LALMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor..
LineArcLineHandler::~LineArcLineHandler()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LALMH0002", __FILE__, __FUNCSIG__); }
}

void LineArcLineHandler::ResetShapeHighlighted()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LALMH0003", __FILE__, __FUNCSIG__); }
}

//Initialise the settings..
void LineArcLineHandler::init()
{
	try
	{
		ParentShape1 = NULL; ParentShape2 = NULL; ParentShape3 = NULL;
		line1 = NULL; line2 = NULL; circle1 = NULL;
		line_ct = 0; arc_ct = 0;
		resetClicks();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LALMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mosuemove..
void LineArcLineHandler::mouseMove()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LALMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the mouse down... Select the shapes..
void LineArcLineHandler::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone( getClicksDone() - 1);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if(Csh->ShapeType ==  RapidEnums::SHAPETYPE::LINE || Csh->ShapeType ==  RapidEnums::SHAPETYPE::ARC)
		{
			if(getClicksDone() == 1)
			{
				ParentShape1 = Csh;
				if (Csh->ShapeType ==  RapidEnums::SHAPETYPE::LINE)
					line_ct = 1;
				else
					arc_ct = 1;
			}
			else
			{
				if (getClicksDone() == 2)
				{
					if(ParentShape1->getId() == Csh->getId()) //Wrong proceedure...
					{
						setClicksDone(getClicksDone() - 1);
						return;
					}
					if ((arc_ct == 1) && (Csh->ShapeType ==  RapidEnums::SHAPETYPE::ARC)) //wrong procedure...
					{
						setClicksDone(getClicksDone() - 1);
						return;
					}
					ParentShape2 = Csh;
					if (Csh->ShapeType ==  RapidEnums::SHAPETYPE::LINE)
						line_ct++;
					else
						arc_ct++;
				}
			}
			Csh->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
		}
		else //Wrong proceedure...
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LALMH0006", __FILE__, __FUNCSIG__); }
}

void LineArcLineHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0012", __FILE__, __FUNCSIG__); }
}

//Handle the escape button press..
void LineArcLineHandler::EscapebuttonPress()
{
	try
	{
		ResetShapeHighlighted();
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0013", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data.....//
void LineArcLineHandler::PartProgramData()
{
	try
	{
		CurrentShape = (Circle*)(MAINDllOBJECT->getShapesList().selectedItem());
		circle1 = (Circle*) CurrentShape;
		circle1->updateLineArcLineShape();
		resetClicks();
		PPCALCOBJECT->partProgramReached();	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0014", __FILE__, __FUNCSIG__); }
}

//Handle the max click...3 in this case
void LineArcLineHandler::LmaxmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL) //set clicks done back to prev stage
		{
			setClicksDone(2);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if ((Csh->ShapeType !=  RapidEnums::SHAPETYPE::LINE) && (Csh->ShapeType !=  RapidEnums::SHAPETYPE::ARC))
		{
			setClicksDone(2);
			return;
		}
		if ((arc_ct == 1) && (Csh->ShapeType ==  RapidEnums::SHAPETYPE::ARC)) //wrong procedure...
		{
			setClicksDone(2);
			return;
		}
		if ((line_ct == 2) && (Csh->ShapeType ==  RapidEnums::SHAPETYPE::LINE)) //wrong procedure...
		{
			setClicksDone(2);
			return;
		}
		arc_ct = 1; line_ct = 2;
		ParentShape3 = Csh;
		//now we have 3 parent shapes... get the line-arc-line best fit function
		//using parameters received, draw these three derived shapes.
		if (ParentShape1->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			Shape* tmpshape = ParentShape1;
			ParentShape1 = ParentShape2;
			ParentShape2 = tmpshape;
		}
		else if (ParentShape3->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			Shape* tmpshape = ParentShape3;
			ParentShape3 = ParentShape2;
			ParentShape2 = tmpshape;
		}

		updatelinearcline();
		ResetShapeHighlighted();
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LALMH0015", __FILE__, __FUNCSIG__); }
}

//function to update line-arc-line...
void LineArcLineHandler::updatelinearcline()
{
	try
	{
		Vector* pt = &getClicksValue(getMaxClicks() - 1);
		if(!(PPCALCOBJECT->IsPartProgramRunning()))
		{

			circle1 = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
			circle1->CircleType = RapidEnums::CIRCLETYPE::LINEARCLINE_CIRCLE;

			CurrentShape = (Shape*) circle1;
			setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());

			ParentShape1->addChild(circle1);
			ParentShape2->addChild(circle1);
			ParentShape3->addChild(circle1);
			circle1->addParent(ParentShape1);
			circle1->addParent(ParentShape2);
			circle1->addParent(ParentShape3);
			AddShapeAction::addShape(circle1, false);

			AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
		}
		if (circle1 != NULL)
			circle1->updateLineArcLineShape();

		MAINDllOBJECT->Shape_Updated();
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("LALMH0016", __FILE__, __FUNCSIG__); 
	}
}
#include "stdafx.h"
#include "ArcTanToLineHandler.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"

//Constructor..//
ArcTanToLineHandler::ArcTanToLineHandler()
{
	try
	{
		init();
		setMaxClicks(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ATLMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor..
ArcTanToLineHandler::~ArcTanToLineHandler()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ATLMH0002", __FILE__, __FUNCSIG__); }
}

void ArcTanToLineHandler::ResetShapeHighlighted()
{
	try
	{
		if(line1 != NULL)
			line1->HighlightedForMeasurement(false);
		if(arc2 != NULL)
			arc2->HighlightedForMeasurement(false);
		MAINDllOBJECT->Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ATLMH0003", __FILE__, __FUNCSIG__); }
}

//Initialise the settings..
void ArcTanToLineHandler::init()
{
	try
	{
		line1 = NULL; arc2 = NULL; 
		linedone = false;
		arcdone = false;
		resetClicks();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ATLMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mosuemove..
void ArcTanToLineHandler::mouseMove()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ATLMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the mouse down... Select the shapes..
void ArcTanToLineHandler::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone( getClicksDone() - 1);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if((Csh->ShapeType == RapidEnums::SHAPETYPE::ARC) && (arcdone == false))
		{
			arc2 = (Circle*) Csh;
			Csh->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
			arcdone = true;
		}
		else if ((Csh->ShapeType == RapidEnums::SHAPETYPE::LINE) && (linedone == false))
		{
			line1 = (Line*) Csh;
			Csh->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
			linedone = true;
		}
		else //Wrong proceedure...
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ATLMH0006", __FILE__, __FUNCSIG__); }
}

void ArcTanToLineHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0012", __FILE__, __FUNCSIG__); }
}

//Handle the escape button press..
void ArcTanToLineHandler::EscapebuttonPress()
{
	try
	{
		ResetShapeHighlighted();
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0013", __FILE__, __FUNCSIG__); }
}

//Handle the max click...2 in this case
void ArcTanToLineHandler::LmaxmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL) //set clicks done back to prev stage
		{
			setClicksDone(1);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if ((arcdone == false) && (Csh->ShapeType !=  RapidEnums::SHAPETYPE::ARC))
		{
			setClicksDone(1);
			return;
		}
		if ((linedone == false) && (Csh->ShapeType !=  RapidEnums::SHAPETYPE::LINE))
		{
			setClicksDone(1);
			return;
		}
		if (linedone)
			arc2 = (Circle*) Csh;
		else
			line1 = (Line*) Csh;
		linedone = true;
		arcdone = true;

		line1->addChild(arc2);
		arc2->addParent(line1);
		arc2->CircleType = RapidEnums::CIRCLETYPE::ARC_TAN_TO_LINE;
		arc2->UpdateBestFit();

		MAINDllOBJECT->Shape_Updated();

		ResetShapeHighlighted();
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ATLMH0015", __FILE__, __FUNCSIG__); }
}
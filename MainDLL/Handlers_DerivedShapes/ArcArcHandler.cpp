#include "stdafx.h"
#include "ArcArcHandler.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"

//Constructor..//
ArcArcHandler::ArcArcHandler()
{
	try
	{
		init();
		setMaxClicks(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AAMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor..
ArcArcHandler::~ArcArcHandler()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AAMH0002", __FILE__, __FUNCSIG__); }
}

void ArcArcHandler::ResetShapeHighlighted()
{
	try
	{
		if(arc1 != NULL)
			arc1->HighlightedForMeasurement(false);
		if(arc2 != NULL)
			arc2->HighlightedForMeasurement(false);
		MAINDllOBJECT->Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AAMH0003", __FILE__, __FUNCSIG__); }
}

//Initialise the settings..
void ArcArcHandler::init()
{
	try
	{
		arc1 = NULL; arc2 = NULL; 
		resetClicks();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AAMH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mosuemove..
void ArcArcHandler::mouseMove()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AAMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the mouse down... Select the shapes..
void ArcArcHandler::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone( getClicksDone() - 1);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if(Csh->ShapeType == RapidEnums::SHAPETYPE::ARC)
		{
			arc1 = (Circle*) Csh;
			Csh->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
		}
		else //Wrong proceedure...
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AAMH0006", __FILE__, __FUNCSIG__); }
}

void ArcArcHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0012", __FILE__, __FUNCSIG__); }
}

//Handle the escape button press..
void ArcArcHandler::EscapebuttonPress()
{
	try
	{
		ResetShapeHighlighted();
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FILLMH0013", __FILE__, __FUNCSIG__); }
}

//Handle the max click...2 in this case
void ArcArcHandler::LmaxmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL) //set clicks done back to prev stage
		{
			setClicksDone(1);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if (Csh->ShapeType !=  RapidEnums::SHAPETYPE::ARC)
		{
			setClicksDone(1);
			return;
		}
		arc2 = (Circle*) Csh;

		arc1->addChild(arc2);
		arc2->addParent(arc1);
		arc2->CircleType = RapidEnums::CIRCLETYPE::ARC_TAN_TO_ARC;
		//arc1->CircleType = RapidEnums::CIRCLETYPE::ARC_ARC_SIBLINGS;
		//arc2->CircleType = RapidEnums::CIRCLETYPE::ARC_ARC_SIBLINGS;

		////these functions will clear any existing sibling relationships.
		//arc1->clearSiblings();
		//arc2->clearSiblings();

		////this will add both arc1 and arc2 to each others' siblings list
		//arc1->addSibling(arc2);
		//arc2->addSibling(arc1);

		//this will update arc2
		arc2->UpdateBestFit();

		MAINDllOBJECT->Shape_Updated();

		ResetShapeHighlighted();
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AAMH0015", __FILE__, __FUNCSIG__); }
}
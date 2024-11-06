#include "StdAfx.h"
#include "PlanePerpendicular2LineHandler.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\Plane.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"

//Constructor..//
PlanePerpendicular2LineHandler::PlanePerpendicular2LineHandler()
{
	try
	{ 
		init(); 
		/*MAINDllOBJECT->SetStatusCode("LineParallel2Line01");*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERPLN2LNH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
PlanePerpendicular2LineHandler::~PlanePerpendicular2LineHandler()
{
}


//Initialise Default settings..
void PlanePerpendicular2LineHandler::init()
{
	try
	{
		resetClicks();
		OffsetDefined = false;
		offset = 0;
		ParentShape = NULL;
		setMaxClicks(2);
		memset(planePnts, 0, 12 * sizeof(double));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERPLN2LNH0002", __FILE__, __FUNCSIG__); }
}

//Handling Left mousedown...//
void PlanePerpendicular2LineHandler::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE3D)
		{
			ParentShape = (Line*)MAINDllOBJECT->highlightedShape();
			/*MAINDllOBJECT->SetStatusCode("DL002");*/
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERPLN2LNH0003", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move..//
void PlanePerpendicular2LineHandler::mouseMove()
{
	if(getClicksDone() == 1)
		CalculatePerpendicularPlane();
}

//draw parallel line..
void PlanePerpendicular2LineHandler::draw(int windowno, double WPixelWidth)
{
	if(getClicksDone() == getMaxClicks() - 1)
			GRAFIX->draw_PlanePolygon(planePnts, 4, true, false);
}

///Handle the mouse scroll... calculate the line end points for change in length...//
void PlanePerpendicular2LineHandler::MouseScroll(bool flag)
{
}

//Set Offset distance..
void PlanePerpendicular2LineHandler::SetAnyData(double *data)
{
	try
	{
		offset = data[0];
		if(data[0] != 0 && getClicksDone() == 1)
			OffsetDefined = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERPLN2LNH0004", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data...//
void PlanePerpendicular2LineHandler::PartProgramData()
{
}

//Reset the current handler settings.
void PlanePerpendicular2LineHandler::EscapebuttonPress()
{
	init();
}

//Handle the maximum click...//
void PlanePerpendicular2LineHandler::LmaxmouseDown()
{
	try
	{
		double clickPoint[6] = {0}, parentParam[6] = {0};
		ParentShape->getParameters(parentParam);
		if(MAINDllOBJECT->highlightedPoint() != NULL)
		{
			Vector* pt = MAINDllOBJECT->highlightedPoint();
			clickPoint[0] = pt->getX();
			clickPoint[1] = pt->getY();
			clickPoint[2] = pt->getZ();
		}
		else
		{
			double MSelectionLine[6] = {0}, tmpLine[6] = {0};
			MAINDllOBJECT->getWindow(1).CalculateSelectionLine(MSelectionLine);
			double parentmidPnts[3] = {0};
	    	ParentShape->getMidPoint()->FillDoublePointer(parentmidPnts,3);
			for(int i = 0; i < 3; i++)
			{ 
				tmpLine[i] = parentmidPnts[i];
				tmpLine[3 + i] = parentParam[3 + i];
			}
			RMATH3DOBJECT->Points_Of_ClosestDist_Btwn2Lines(MSelectionLine, parentParam, clickPoint);						
		}
		CurrentShape = new Plane();
		setBaseRProperty(clickPoint[0], clickPoint[1], clickPoint[2]);
		baseaction->DroMovementFlag = false;
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction, false);
		((Plane*)CurrentShape)->PlaneType = RapidEnums::PLANETYPE::PLANEPERPENDICULAR2LINE;
		ParentShape->addChild(CurrentShape);
		CurrentShape->addParent(ParentShape);
		CurrentShape->UpdateBestFit();
		AddShapeAction::addShape(CurrentShape);
		init();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PARALLELPLNH0006", __FILE__, __FUNCSIG__); }
}

void PlanePerpendicular2LineHandler::RmouseDown(double x, double y)
{
}

void PlanePerpendicular2LineHandler::CalculatePerpendicularPlane()
{
	try
	{
		double MSelectionLine[6], tmpLine[6]= {0}, tmpPoints[6] = {0}, parentParam[6] = {0}, midPnt[3] = {0};
		MAINDllOBJECT->getWindow(1).CalculateSelectionLine(MSelectionLine);
	    ParentShape->getParameters(parentParam);
		ParentShape->getMidPoint()->FillDoublePointer(midPnt, 3);
		for(int i = 0; i < 3; i++)
		{ 
			tmpLine[i] = midPnt[i];
			tmpLine[3 + i] = parentParam[3 + i];
		}
		RMATH3DOBJECT->Points_Of_ClosestDist_Btwn2Lines(MSelectionLine, tmpLine, tmpPoints);
		double dir[3] = {1,0,0}, dir1[3] = {0}, dir2[3] = {0};
		if(abs(RMATH3DOBJECT->Angle_Btwn_2Directions(dir, &parentParam[3], false, true)) > .9)
		{
			dir[0] = 0;
			dir[1] = 1;
	    }
		RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(dir, &parentParam[3], dir1);
		RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(dir1, &parentParam[3], dir2); 
		double length = ParentShape->Length();
		if(length == 0) length = 1;
		for(int i = 0; i < 3; i++) 
		{
			planePnts[i] = tmpPoints[i] + length * dir1[i];
			planePnts[3 + i] = tmpPoints[i] + length * dir2[i];
			planePnts[6 + i] = tmpPoints[i] - length * dir1[i];
			planePnts[9 + i] = tmpPoints[i] - length * dir2[i];
		}

	}
	catch(...)
	{
	   MAINDllOBJECT->SetAndRaiseErrorMessage("PARALLELPLNH0007", __FILE__, __FUNCSIG__); 
	}
}


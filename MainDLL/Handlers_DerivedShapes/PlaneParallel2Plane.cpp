#include "StdAfx.h"
#include "PlaneParallel2Plane.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\Plane.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"

//Constructor..//
PlaneParallel2Plane::PlaneParallel2Plane()
{
	try
	{ 
		init(); 
		/*MAINDllOBJECT->SetStatusCode("LineParallel2Line01");*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PARALLELPLNH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
PlaneParallel2Plane::~PlaneParallel2Plane()
{
	try
	{
		if(ParentShape != NULL)
			ParentShape = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PARALLELPLNH0002", __FILE__, __FUNCSIG__); }
}


//Initialise Default settings..
void PlaneParallel2Plane::init()
{
	try
	{
		resetClicks();
		OffsetDefined = false;
		offset = 0;
		ParentShape = NULL;
		setMaxClicks(2);
		memset(planePnts, 0, sizeof(double) * 12);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PARALLELPLNH0003", __FILE__, __FUNCSIG__); }
}

//Handling Left mousedown...//
void PlaneParallel2Plane::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::PLANE)
		{
			ParentShape = (Plane*)MAINDllOBJECT->highlightedShape();
			MAINDllOBJECT->DerivedShapeCallback();

			/*	MAINDllOBJECT->SetStatusCode("DL002");*/
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PARALLELPLNH0004", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move..//
void PlaneParallel2Plane::mouseMove()
{
	if(getClicksDone() == 1)
		CalculateParallelPlane();
}

//draw parallel line..
void PlaneParallel2Plane::draw(int windowno, double WPixelWidth)
{
	if(getClicksDone() == getMaxClicks() - 1)
			GRAFIX->draw_PlanePolygon(planePnts,4, true, false);
}

///Handle the mouse scroll... calculate the line end points for change in length...//
void PlaneParallel2Plane::MouseScroll(bool flag)
{
}

//Set Offset distance..
void PlaneParallel2Plane::SetAnyData(double *data)
{
	try
	{
		offset = data[1];
		if(data[1] != 0 && getClicksDone() == 1)
			OffsetDefined = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PARALLELPLNH0005", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data...//
void PlaneParallel2Plane::PartProgramData()
{
}

//Reset the current handler settings.
void PlaneParallel2Plane::EscapebuttonPress()
{
	init();
}

//Handle the maximum click...//
void PlaneParallel2Plane::LmaxmouseDown()
{
	try
	{
		double clickPoint[6] = {0}, parentParam[4] = {0};
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
			double parentEndPnts[12] = {0};
	    	ParentShape->getEndPoints(parentEndPnts);
			double midPnt[3] = {(parentEndPnts[0] + parentEndPnts[6])/2, (parentEndPnts[1] + parentEndPnts[7])/2, (parentEndPnts[2] + parentEndPnts[8])/2};
			for(int i = 0; i < 3; i++)
			{ 
				tmpLine[i] = midPnt[i];
				tmpLine[3 + i] = parentParam[i];
			}
			RMATH3DOBJECT->Points_Of_ClosestDist_Btwn2Lines(MSelectionLine, parentParam, clickPoint);						
		}
		if(RMATH3DOBJECT->Checking_Point_on_Plane(clickPoint, parentParam))
		{
		   setClicksDone(getMaxClicks() - 1);
			return;
		}
		CurrentShape = new Plane();
		setBaseRProperty(clickPoint[0], clickPoint[1], clickPoint[2]);
		baseaction->DroMovementFlag = false;
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction, false);
		((Plane*)CurrentShape)->PlaneType = RapidEnums::PLANETYPE::PARALLELPLANE;
		ParentShape->addChild(CurrentShape);
		CurrentShape->addParent(ParentShape);
		((Plane*)CurrentShape)->Offset(offset);
		((Plane*)CurrentShape)->DefinedOffset(OffsetDefined);
		CurrentShape->UpdateBestFit();
		AddShapeAction::addShape(CurrentShape);
		//baseaction->AllPointsList.deleteAll();
		init();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PARALLELPLNH0006", __FILE__, __FUNCSIG__); }
}

void PlaneParallel2Plane::RmouseDown(double x, double y)
{
}

void PlaneParallel2Plane::CalculateParallelPlane()
{
	try
	{
		double MSelectionLine[6], tmpLine[6]= {0}, tmpPoints[6] = {0}, parentParam[4] = {0}, parentEndPnts[12] = {0}, curD = 0;
		MAINDllOBJECT->getWindow(1).CalculateSelectionLine(MSelectionLine);
	    ParentShape->getParameters(parentParam);
		ParentShape->getEndPoints(parentEndPnts);
		double midPnt[3] = {(parentEndPnts[0] + parentEndPnts[6])/2, (parentEndPnts[1] + parentEndPnts[7])/2, (parentEndPnts[2] + parentEndPnts[8])/2};
		for(int i = 0; i < 3; i++)
		{ 
			tmpLine[i] = midPnt[i];
			tmpLine[3 + i] = parentParam[i];
		}
		RMATH3DOBJECT->Points_Of_ClosestDist_Btwn2Lines(MSelectionLine, tmpLine, tmpPoints);
		curD = parentParam[0] * tmpPoints[0] + parentParam[1] * tmpPoints[1] + parentParam[2] * tmpPoints[2];
		if (OffsetDefined)
			parentParam[3] += offset;
		else
			parentParam[3] = curD;

		for(int i = 0; i < 4; i++)
			RMATH3DOBJECT->Projection_Point_on_Plane(&parentEndPnts[3 *i], parentParam, &planePnts[3 *i]);	
	}
	catch(...)
	{}
}


#include "StdAfx.h"
#include "PlanePerpendicular2Plane.h"
#include "..\Shapes\Plane.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"

//Constructor..//
PlanePerpendicular2Plane::PlanePerpendicular2Plane()
{
	try
	{ 
		init(); 
		/*MAINDllOBJECT->SetStatusCode("LinePerpendicular2Line01");*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANEPERMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
PlanePerpendicular2Plane::~PlanePerpendicular2Plane()
{
}

//Initialise Default settings..
void PlanePerpendicular2Plane::init()
{
	try
	{
		resetClicks();
		setMaxClicks(4);
		OffsetDefined = false;
		offset = 0;
		ParentShape = NULL;
		CurrentPlane = NULL;
		memset(planePnts, 0, sizeof(double) * 12);
		memset(twoclick, 0, sizeof(double) * 6);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANEPERMH0002", __FILE__, __FUNCSIG__); }
}

//Handling Left mousedown...//
void PlanePerpendicular2Plane::LmouseDown()
{
	try
	{
		if(getClicksDone() == 1)
		{
			if (MAINDllOBJECT->highlightedShape() == NULL)
				setClicksDone(getClicksDone() - 1);
			else if( MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::PLANE)
			{
				ParentShape = (Plane*)MAINDllOBJECT->highlightedShape();
				CurrentPlane = new Plane();
			    CurrentShape = CurrentPlane;
			}
			else
				setClicksDone(getClicksDone() - 1);
			return;			
		}
		else if(getClicksDone() == 2)
		{
			if(MAINDllOBJECT->highlightedPoint() != NULL)
			{
				Vector* pt = MAINDllOBJECT->highlightedPoint();
	    		twoclick[0] = pt->getX();
				twoclick[1] = pt->getY();
				twoclick[2] = pt->getZ();
			}
			else
			{
				double MSelectionLine[6] = {0};
			    MAINDllOBJECT->getWindow(1).CalculateSelectionLine(MSelectionLine);
				double parentParam[4] = {0};
	    	    ParentShape->getParameters(parentParam);
			    if(RMATH3DOBJECT->Intersection_Line_Plane(MSelectionLine, parentParam, twoclick) == 0)
				{
					setClicksDone(getClicksDone() - 1);
			        return;	
				}
			}
			setBaseRProperty(twoclick[0],twoclick[1], twoclick[2]);
			baseaction->DroMovementFlag = false;
			PointActions.push_back(baseaction);
		}		
		else if(getClicksDone() == 3)
		{
			if(MAINDllOBJECT->highlightedPoint() != NULL)
			{
				Vector* pt = MAINDllOBJECT->highlightedPoint();
	    		twoclick[3] = pt->getX();
				twoclick[4] = pt->getY();
				twoclick[5] = pt->getZ();
			}
			else
			{
				double MSelectionLine[6] = {0};
			    MAINDllOBJECT->getWindow(1).CalculateSelectionLine(MSelectionLine);
				double parentParam[4] = {0};
	    	    ParentShape->getParameters(parentParam);
			    if(RMATH3DOBJECT->Intersection_Line_Plane(MSelectionLine, parentParam, &twoclick[3]) == 0)
				{
					setClicksDone(getClicksDone() - 1);
			        return;	
				}
			}
			if(abs(RMATH3DOBJECT->Distance_Point_Point(twoclick, &twoclick[3])) < 0.001)
			{
			    setClicksDone(getClicksDone() - 1);
			    return;	
			}
			setBaseRProperty(twoclick[3],twoclick[4], twoclick[5]);
			baseaction->DroMovementFlag = false;
		    PointActions.push_back(baseaction);
		}		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANEPERMH0003", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move..//
void PlanePerpendicular2Plane::mouseMove()
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1)
		{
			CalculatePerpendicularPlane();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PLANEPERMH0004", __FILE__, __FUNCSIG__); }
}

//draw perpendicular line..
void PlanePerpendicular2Plane::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1)
			GRAFIX->draw_PlanePolygon(planePnts,4, true, false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPERMH0006", __FILE__, __FUNCSIG__); }
}

///Handle the mouse scroll... calculate the line end points for change in length...//
void PlanePerpendicular2Plane::MouseScroll(bool flag)
{
}

//Handle the partprogram data...//
void PlanePerpendicular2Plane::PartProgramData()
{
}

//Reset the current handler settings.,
void PlanePerpendicular2Plane::EscapebuttonPress()
{
	if(CurrentPlane != NULL)
	   delete CurrentPlane;
	list<FramegrabBase*>::iterator It = PointActions.begin();
	while(It != PointActions.end())
	{
		delete *It;
	}
	PointActions.clear();
	init(); 
}

//Handle the maximum click...//
void PlanePerpendicular2Plane::LmaxmouseDown()
{
	try
	{
		double intsectPnt[6] = {0};
		if(MAINDllOBJECT->highlightedPoint() != NULL)
		{
			Vector* pt = MAINDllOBJECT->highlightedPoint();
			intsectPnt[0] = pt->getX();
			intsectPnt[1] = pt->getY();
			intsectPnt[2] = pt->getZ();
		}
		else
		{
			double MSelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(1).CalculateSelectionLine(MSelectionLine);
			double parentParam[4] = {0};
	    	ParentShape->getParameters(parentParam);
			if(RMATH3DOBJECT->Intersection_Line_Plane(MSelectionLine, parentParam, intsectPnt) == 0)
			{
				setClicksDone(getClicksDone() - 1);
			    return;	
			}			
		}
		for(list<FramegrabBase*>::iterator It = PointActions.begin(); It != PointActions.end(); It++)
		{
		   AddPointAction::pointAction((ShapeWithList*)CurrentShape, *It, false);
		}
		setBaseRProperty(intsectPnt[0], intsectPnt[1], intsectPnt[2]);
		baseaction->DroMovementFlag = false;
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction, false);
		CurrentPlane->PlaneType = RapidEnums::PLANETYPE::PERPENDICULARPLANE;
		ParentShape->addChild(CurrentPlane);
		CurrentPlane->addParent(ParentShape);
		CurrentPlane->UpdateBestFit();
		AddShapeAction::addShape(CurrentPlane);
		init();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPERMH0012", __FILE__, __FUNCSIG__); }
}

void PlanePerpendicular2Plane::SetAnyData(double *data)
{
	offset = data[0];
	if(data[0] != 0 && getClicksDone() == 3)
	{
		OffsetDefined = true;
	}
}

void PlanePerpendicular2Plane::RmouseDown(double x, double y)
{
	try
	{

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEABMH0012", __FILE__, __FUNCSIG__); }
}

void PlanePerpendicular2Plane::CalculatePerpendicularPlane()
{
	try
	{
			double MSelectionLine[6], tmpLine[6]= {0}, tmpPoints[6] = {0};
			MAINDllOBJECT->getWindow(1).CalculateSelectionLine(MSelectionLine);
			double length = 0.5, curPlnDir[3] = {0}, parentParam[4] = {0}, tmpCos[3] = {0}, tmpCos2[3] = {0};
	    	ParentShape->getParameters(parentParam);
			double tmpDir[3] = {twoclick[0] - twoclick[3], twoclick[1] - twoclick[4], twoclick[2] - twoclick[5]};
			RMATH3DOBJECT->DirectionCosines(tmpDir, tmpCos);
			RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(parentParam, tmpCos, curPlnDir);
			RMATH3DOBJECT->Create_Perpendicular_Direction_2_2Directions(curPlnDir, tmpCos, tmpDir);
		    double midPnt[3] = {(twoclick[0] + twoclick[3]) / 2, (twoclick[1] + twoclick[4]) / 2, (twoclick[2] + twoclick[5]) / 2};
			for(int i = 0; i < 3; i++)
			{ 
			   tmpLine[i] = midPnt[i];
			   tmpLine[3 + i] = curPlnDir[i];
			}
			RMATH3DOBJECT->Points_Of_ClosestDist_Btwn2Lines(MSelectionLine, tmpLine, tmpPoints);
			double tmpDir2[3] = {tmpPoints[0] - midPnt[0], tmpPoints[1] - midPnt[1], tmpPoints[2] - midPnt[2]};
			RMATH3DOBJECT->DirectionCosines(tmpDir2, tmpCos2);
			double dist = RMATH3DOBJECT->Distance_Point_Point(tmpPoints, midPnt);
			for(int i = 0; i < 3; i++)
			{
				planePnts[i] = (twoclick[i] + tmpDir[i] * length) + dist * tmpCos2[i];
				planePnts[3 + i] = (twoclick[i] - tmpDir[i] * length) + dist * tmpCos2[i];
				planePnts[6 + i] = (twoclick[3 + i] + tmpDir[i] * length) + dist * tmpCos2[i];
				planePnts[9 + i] = (twoclick[3 + i] - tmpDir[i] * length) + dist * tmpCos2[i];
			}
	}
	catch(...)
	{}
}
#include "stdafx.h"
#include "PlaneThrough2Lines3D.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Plane.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\Cylinder.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor...//
PlaneThrough2Lines3D::PlaneThrough2Lines3D()
{
	init();
	setMaxClicks(2);
	/*MAINDllOBJECT->SetStatusCode("CircleTangent2TwoLine01");*/
}

//Destructor..
PlaneThrough2Lines3D::~PlaneThrough2Lines3D()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLL0001", __FILE__, __FUNCSIG__); }
}

void PlaneThrough2Lines3D::ResetShapeHighlighted()
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
void PlaneThrough2Lines3D::init()
{
	try
	{
		runningPartprogramValid = false;
		validflag = false;
		resetClicks();
		ParentShape1 = NULL; ParentShape2 = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PT2L0001", __FILE__, __FUNCSIG__); }
}

//
void PlaneThrough2Lines3D::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if(Csh->ShapeType !=  RapidEnums::SHAPETYPE::CYLINDER && Csh->ShapeType !=  RapidEnums::SHAPETYPE::LINE && Csh->ShapeType !=  RapidEnums::SHAPETYPE::LINE3D)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		ParentShape1 = Csh;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PT2L0002", __FILE__, __FUNCSIG__); }
}

void PlaneThrough2Lines3D::mouseMove()
{
}

void PlaneThrough2Lines3D::draw(int windowno, double WPixelWidth)
{
}

void PlaneThrough2Lines3D::EscapebuttonPress()
{
	ResetShapeHighlighted();
	init();
}

void PlaneThrough2Lines3D::SetAnyData(double *data)
{
}


//Increase/decrease the diameter for ouse scroll event....//
void PlaneThrough2Lines3D::MouseScroll(bool flag)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PT2L0007", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram....//
void PlaneThrough2Lines3D::PartProgramData()
{
	try
	{
		runningPartprogramValid = true;
		LmaxmouseDown();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLL0008", __FILE__, __FUNCSIG__); }
}

//Max mouse click..//
void PlaneThrough2Lines3D::LmaxmouseDown()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			CurrentShape = (Plane*)(MAINDllOBJECT->getShapesList().selectedItem());
			list<BaseItem*>::iterator ptr = CurrentShape->getParents().end(); 
			ptr--;
			ParentShape2 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr--)->getId()];
			ParentShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[(*ptr)->getId()];
		}
		else
		{
			if(MAINDllOBJECT->highlightedShape() == NULL)
			{
				setClicksDone(getMaxClicks() - 1);
				return;
			}
			Shape* Csh = MAINDllOBJECT->highlightedShape();
			if(Csh->ShapeType !=  RapidEnums::SHAPETYPE::CYLINDER && Csh->ShapeType !=  RapidEnums::SHAPETYPE::LINE &&  Csh->ShapeType !=  RapidEnums::SHAPETYPE::LINE3D)
			{
				setClicksDone(getMaxClicks() - 1);
				return;
			}
			if(ParentShape1->getId() == Csh->getId())
			{
			   setClicksDone(getMaxClicks() - 1);
			   return;
			}
			ParentShape2 = Csh;
		}
		double line1Param[7] = {0}, line2Param[7] = {0}, cenPnt[6] = {0}, crossVect[3] = {0}, planeParam[4] = {0};
		if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
		    ((Cylinder*)ParentShape1)->getParameters(line1Param);
		else
			((Line*)ParentShape1)->getParameters(line1Param);
		if(ParentShape2->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
		    ((Cylinder*)ParentShape2)->getParameters(line2Param);
		else
			((Line*)ParentShape2)->getParameters(line2Param);
		if(RMATH3DOBJECT->Points_Of_ClosestDist_Btwn2Lines(line1Param, line2Param, cenPnt))
		{
			double tempPnt[6] = {0}, planePnts[12] = {0};
		    RMATH3DOBJECT->CrossProductOf2Vectors(&line1Param[3], &line2Param[3], crossVect);
			RMATH3DOBJECT->DirectionCosines(crossVect, planeParam); 
			for(int i = 0; i < 3; i++) {planeParam[3] += planeParam[i] * cenPnt[i];}
			if(ParentShape1->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
				((Cylinder*)ParentShape1)->getEndPoints(planePnts);
			else
				((Line*)ParentShape1)->getEndPoints(planePnts);
			if(ParentShape2->ShapeType == RapidEnums::SHAPETYPE::CYLINDER)
				((Cylinder*)ParentShape2)->getEndPoints(tempPnt);
			else
				((Line*)ParentShape2)->getEndPoints(tempPnt);
			RMATH3DOBJECT->Projection_Point_on_Plane(tempPnt, planeParam, &planePnts[6]);
			RMATH3DOBJECT->Projection_Point_on_Plane(&tempPnt[3], planeParam, &planePnts[9]);

			PointCollection TempPtColl;
			TempPtColl.Addpoint(new SinglePoint(planePnts[0], planePnts[1], planePnts[2]));
		    TempPtColl.Addpoint(new SinglePoint(planePnts[3], planePnts[4], planePnts[5]));
			TempPtColl.Addpoint(new SinglePoint(planePnts[6], planePnts[7], planePnts[8]));
		    TempPtColl.Addpoint(new SinglePoint(planePnts[9], planePnts[10], planePnts[11]));

			if(PPCALCOBJECT->IsPartProgramRunning())
			{
				if(runningPartprogramValid)
				{
					runningPartprogramValid = false;
					((ShapeWithList*)CurrentShape)->AddPoints(&TempPtColl);
					PPCALCOBJECT->getFrameGrab()->ActionFinished(true);
					PPCALCOBJECT->partProgramReached();
				}
				else
					setClicksDone(getMaxClicks() - 1);
				return;
			}
			else
			{
				CurrentShape = new Plane();
				((Plane*)CurrentShape)->PlaneType = RapidEnums::PLANETYPE::PLANETHROUGHTWOLINE;
				setBaseRProperty(cenPnt[0], cenPnt[1], cenPnt[2]);
				ParentShape1->addChild(CurrentShape);
				ParentShape2->addChild(CurrentShape);
				CurrentShape->addParent(ParentShape1);
				CurrentShape->addParent(ParentShape2);
				AddShapeAction::addShape(CurrentShape);
				AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
			 	((ShapeWithList*)CurrentShape)->AddPoints(&TempPtColl);
				ResetShapeHighlighted();
				init();
				MAINDllOBJECT->SetStatusCode("Finished");
				return;
			}
		}
	    setClicksDone(getMaxClicks() - 1);
		return;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CTLL0009", __FILE__, __FUNCSIG__); }
}
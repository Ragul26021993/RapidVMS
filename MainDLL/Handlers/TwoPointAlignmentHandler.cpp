#include "StdAfx.h"
#include "TwoPointAlignmentHandler.h"
#include "..\Shapes\RPoint.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Actions\TranslationAction.h"
#include "..\Actions\RotationAction.h"

TwoPointAlignmentHandler::TwoPointAlignmentHandler()
{	
	RotationPoint = NULL;
	Angle = 0;
	memset(StartPos, 0, sizeof(double) * 3);
	memset(TranslateParam, 0, sizeof(double) * 6);
	setMaxClicks(4);
}

TwoPointAlignmentHandler::~TwoPointAlignmentHandler()
{
	/*try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("TPAHANDLER001a", __FILE__, __FUNCSIG__); }*/
}

void TwoPointAlignmentHandler::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedPoint() == NULL || MAINDllOBJECT->getSelectedShapesList().getList().empty())
		{
		   setClicksDone(getClicksDone() - 1);
		   return;
		}
		else if(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1() != NULL && MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1()->selected())
		{
			setClicksDone(getClicksDone() - 1);
		    return;
		}
		else if(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2() != NULL && MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2()->selected())
		{
			setClicksDone(getClicksDone() - 1);
		    return;
		}
		if(getClicksDone() == 1)
		{
		    points[0].set(*MAINDllOBJECT->highlightedPoint());
			TranslateParam[0] =  points[0].getX();
			TranslateParam[1] =  points[0].getY();
			TranslateParam[2] =  points[0].getZ();
		}
		else if(getClicksDone() == 2)
		{
			if(!(*MAINDllOBJECT->highlightedPoint() ==  points[0]) &&  MAINDllOBJECT->getSelectedShapesList().getList().size() > 0)
			{
				RotationPoint = MAINDllOBJECT->highlightedPoint();
		        points[1].set(*MAINDllOBJECT->highlightedPoint());
				double translationDirection[3] = {points[1].getX() - points[0].getX(), points[1].getY() - points[0].getY(), points[1].getZ() - points[0].getZ()};
			   	TranslationAction *newAction = new TranslationAction();
                newAction->TranslateDirection.set(translationDirection[0], translationDirection[1], translationDirection[2]);
				for(RC_ITER item = MAINDllOBJECT->getSelectedShapesList().getList().begin(); item != MAINDllOBJECT->getSelectedShapesList().getList().end(); item++)
				{
					Shape* Current_Shape = (Shape*)(*item).second;
					if(!Current_Shape->Normalshape()) continue;
					newAction->TranslatedShapesId.push_back(Current_Shape->getId());
					Current_Shape->TranslateShape(translationDirection);
				}
				TranslateParam[0] =  points[1].getX();
				TranslateParam[1] =  points[1].getY();
				TranslateParam[2] =  points[1].getZ();
				MAINDllOBJECT->addAction(newAction);
			}
			else
			{
			   setClicksDone(getClicksDone() - 1);
		       return;
			}
		}
		else if(getClicksDone() == 3)
		{
		    if(!((*MAINDllOBJECT->highlightedPoint() == points[0]) || (*MAINDllOBJECT->highlightedPoint() ==  points[1])))
			{
		        points[2].set(*MAINDllOBJECT->highlightedPoint());
			    StartPos[0] =  points[2].getX();
				StartPos[1] =  points[2].getY();
				StartPos[2] =  points[2].getZ();
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
	}
	catch(...)
	{
	   MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("TPAHANDLER001b", __FILE__, __FUNCSIG__);  
	}
}

void TwoPointAlignmentHandler::mouseMove()
{
	if(getClicksDone() == 1 &&  MAINDllOBJECT->getSelectedShapesList().getList().size() > 0)
	{
		double mSelectionLine[6] = {0};
		MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(mSelectionLine);
		RMATH3DOBJECT->Projection_Point_on_Line(TranslateParam, mSelectionLine, &TranslateParam[3]);
		for(RC_ITER item = MAINDllOBJECT->getSelectedShapesList().getList().begin(); item != MAINDllOBJECT->getSelectedShapesList().getList().end(); item++)
		{
			Shape* Current_Shape = (Shape*)(*item).second;
			if(!Current_Shape->Normalshape() || Current_Shape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS) continue;
			Current_Shape->Translate(Vector(TranslateParam[3] - TranslateParam[0], TranslateParam[4] -TranslateParam[1], TranslateParam[5] -TranslateParam[2]));
		}
		for(int i = 0; i < 3; i++) {TranslateParam[i] = TranslateParam[3 + i];}
		MAINDllOBJECT->UpdateShapesChanged();
	}
	else if(getClicksDone() == 3 &&  MAINDllOBJECT->getSelectedShapesList().getList().size() > 0)
	{
		double newMatrix[9] = {0};
		CalculateMatrixesForRotation(StartPos, newMatrix);
		for(RC_ITER item = MAINDllOBJECT->getSelectedShapesList().getList().begin(); item != MAINDllOBJECT->getSelectedShapesList().getList().end(); item++)
		{
			Shape* Current_Shape = (Shape*)(*item).second;
			if(!Current_Shape->Normalshape() || Current_Shape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS) continue;
			Current_Shape->Translate(Vector(-TranslateParam[0], -TranslateParam[1], -TranslateParam[2]));
			Current_Shape->Transform(newMatrix);
			Current_Shape->Translate(Vector(TranslateParam[0], TranslateParam[1], TranslateParam[2]));
		}
		MAINDllOBJECT->UpdateShapesChanged();
	}
}	

void TwoPointAlignmentHandler::draw(int windowno, double WPixelWidth)
{
}

void TwoPointAlignmentHandler::LmaxmouseDown()
{
	try
	{
		if(MAINDllOBJECT->getSelectedShapesList().getList().size() > 0 && MAINDllOBJECT->highlightedPoint() == NULL || *MAINDllOBJECT->highlightedPoint() ==  points[0] || *MAINDllOBJECT->highlightedPoint() ==  points[1] || *MAINDllOBJECT->highlightedPoint() ==  points[2])
		{
		   setClicksDone(getMaxClicks() - 1);
		   return;
		}
		RotationAction *newAction = new RotationAction();
	    if(RotationPoint != NULL)
	    	newAction->RotationPoint = RotationPoint;

		points[3].set(*MAINDllOBJECT->highlightedPoint());

		double newMatrix[9] = {0}, targetPos[3] = {points[3].getX(), points[3].getY(), points[3].getZ()};
		newAction->RotationOrigin[0] =  points[1].getX();
		newAction->RotationOrigin[1] =  points[1].getY();
		newAction->RotationOrigin[2] =  points[1].getZ();
		CalculateMatrixesForRotation(StartPos, newAction->RotateMatrix, targetPos, true);

		newAction->Angle = Angle;

		for(RC_ITER item = MAINDllOBJECT->getSelectedShapesList().getList().begin(); item != MAINDllOBJECT->getSelectedShapesList().getList().end(); item++)
		{
			Shape* Current_Shape = (Shape*)(*item).second;
			if(!Current_Shape->Normalshape()) continue;
			newAction->RotatedShapesId.push_back(Current_Shape->getId());
			Current_Shape->RotateShape(newAction->RotateMatrix, newAction->RotationOrigin);
		}
		MAINDllOBJECT->UpdateShapesChanged();
		MAINDllOBJECT->addAction(newAction);
    	setClicksDone(0);
	}
	catch(...)
	{
	
	}
}

void TwoPointAlignmentHandler::EscapebuttonPress()
{
}

void TwoPointAlignmentHandler::PartProgramData()
{
}

bool TwoPointAlignmentHandler::CalculateMatrixesForRotation(double *currentShapeCenter, double *rotateMatrix, double *targetPos, bool targetGiven)
{
	try
	{
	    int s1[2] = {3,3}, s2[2] = {3,3}, s3[2] = {3,1};
		double RotationPlane[4] = {0,0,1, points[2].getZ()}, mSelectionLine[6] = {0}, targetPosition[3] = {0}, targetDir[3] = {0}, currentDir[3] = {0}; 
		if(targetGiven)
		{
		   targetPosition[0] = targetPos[0];
		   targetPosition[1] = targetPos[1];
		   targetPosition[2] = targetPos[2];
		}
		else
		{
			MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(mSelectionLine);
			RMATH3DOBJECT->Intersection_Line_Plane(mSelectionLine, RotationPlane, targetPosition);
		}

		for(int i = 0; i < 3; i++)
		{
		   targetDir[i] = targetPosition[i] - TranslateParam[i];
		   currentDir[i] = currentShapeCenter[i] - TranslateParam[i];
		   currentShapeCenter[i] = targetPosition[i];
		}
	    Angle = RMATH3DOBJECT->Angle_Btwn_2Directions(targetDir, currentDir, true, false);
		double vectorDir[3] = {(currentDir[1] * targetDir[2] - currentDir[2] * targetDir[1]), (currentDir[2] * targetDir[0] - currentDir[0] * targetDir[2]), (currentDir[0] * targetDir[1] - currentDir[1] * targetDir[0])};
		RMATH3DOBJECT->DirectionCosines(vectorDir, vectorDir);
		if(RMATH3DOBJECT->Angle_Btwn_2Directions(vectorDir, RotationPlane, false, true) < 0)
			Angle *= -1;
		RMATH3DOBJECT->RotationAround3DAxisThroughOrigin(Angle, RotationPlane, rotateMatrix);
		return true;
	}
	catch(...)
	{
	  return false;
	}
}

void TwoPointAlignmentHandler::SetAnyData(double*)
{

}

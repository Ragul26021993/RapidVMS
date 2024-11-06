#include "StdAfx.h"
#include "RotateShapeHandler.h"
#include "..\Shapes\RPoint.h"
#include "..\Shapes\Line.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\RotationAction.h"
#include "..\Engine\PartProgramFunctions.h"

RotateShapeHandler::RotateShapeHandler()
{
	Init();
}

RotateShapeHandler::~RotateShapeHandler()
{
}

void RotateShapeHandler::Init()
{
	setMaxClicks(2);
	RotationPoint = NULL;
	ShapeAsRotationAxis = NULL;
	AngleDefined = false;
	Angle = 0;
}

void RotateShapeHandler::LmouseDown()
{
	memset(RotationOrigin, 0, sizeof(double) * 3);
	memset(RotationPlane, 0, sizeof(double) * 4);
	memset(OriginalShapesCenter, 0, 3 * sizeof(double));
	bool conti = GetSelectedShapeCenter(OriginalShapesCenter);
	if(conti && (MAINDllOBJECT->highlightedPoint() != NULL))
	{   
		RotationPoint = MAINDllOBJECT->highlightedPoint();
		RotationOrigin[0] = RotationPoint->getX(); RotationOrigin[1] = RotationPoint->getY(); RotationOrigin[2] = OriginalShapesCenter[2];
	    RotationPlane[0] = 0; RotationPlane[1] = 0; RotationPlane[2] = 1; 
		RotationPlane[3] = OriginalShapesCenter[2];
	}
	else if(conti && (MAINDllOBJECT->highlightedShape() != NULL) && MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE3D)
	{
		double lineParam[6] = {0};
		ShapeAsRotationAxis = (Shape*)(MAINDllOBJECT->highlightedShape());
		((Line*)ShapeAsRotationAxis)->getParameters(lineParam);
		RMATH3DOBJECT->Projection_Point_on_Line(OriginalShapesCenter, lineParam, RotationOrigin);
		for(int i = 0; i < 3; i++)
		{
			RotationPlane[i] = lineParam[3 + i];
			RotationPlane[3] += RotationPlane[i] * RotationOrigin[i];
		}
	}
	else
	{
	   setClicksDone(getClicksDone() - 1);
	   return;
	}
	MAINDllOBJECT->DerivedShapeCallback();
}

void RotateShapeHandler::mouseMove()
{
	if(getClicksDone() == 1)
	{
		double currentShapeCenter[3] = {0};
		GetSelectedShapeCenter(currentShapeCenter);
		double newMatrix[9] = {0};
		CalculateMatrixesForRotation(currentShapeCenter, newMatrix);
		for(RC_ITER item = MAINDllOBJECT->getSelectedShapesList().getList().begin(); item != MAINDllOBJECT->getSelectedShapesList().getList().end(); item++)
		{
			Shape* Current_Shape = (Shape*)(*item).second;
			if(!Current_Shape->Normalshape() || Current_Shape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS) continue;
			Current_Shape->Translate(Vector(-RotationOrigin[0], -RotationOrigin[1], -RotationOrigin[2]));
			Current_Shape->Transform(newMatrix);
			Current_Shape->Translate(Vector(RotationOrigin[0], RotationOrigin[1], RotationOrigin[2]));
		}
		MAINDllOBJECT->UpdateShapesChanged();
	}
}	

void RotateShapeHandler::draw(int windowno, double WPixelWidth)
{
}

void RotateShapeHandler::LmaxmouseDown()
{
	RotationAction *newAction = new RotationAction();
	if(ShapeAsRotationAxis != NULL)
    	newAction->RotationAxisId = ShapeAsRotationAxis->getId();
	if(RotationPoint != NULL)
		newAction->RotationPoint = RotationPoint;

	if(AngleDefined)
		RMATH3DOBJECT->RotationAround3DAxisThroughOrigin(Angle, RotationPlane, newAction->RotateMatrix);
	else
		CalculateMatrixesForRotation(OriginalShapesCenter, newAction->RotateMatrix);
	
	newAction->Angle = Angle;

	for(int i = 0; i < 3; i++) { newAction->RotationOrigin[i] = RotationOrigin[i]; }
	for(RC_ITER item = MAINDllOBJECT->getSelectedShapesList().getList().begin(); item != MAINDllOBJECT->getSelectedShapesList().getList().end(); item++)
	{
		ShapeWithList* Current_Shape = (ShapeWithList*)(*item).second;
		if(!Current_Shape->Normalshape()) continue;
		newAction->RotatedShapesId.push_back(Current_Shape->getId());
	    Current_Shape->RotateShape(newAction->RotateMatrix, RotationOrigin);
	}		
	MAINDllOBJECT->addAction(newAction);
	Init();
}

void RotateShapeHandler::EscapebuttonPress()
{
}

void RotateShapeHandler::PartProgramData()
{
	if(PPCALCOBJECT->IsPartProgramRunning())
	{
		double rotateMatrix[9] = {0};
		memset(RotationOrigin, 0, sizeof(double) * 3);
		memset(RotationPlane, 0, sizeof(double) * 4);
		memset(OriginalShapesCenter, 0, 3 * sizeof(double));
		RotationAction* atn = (RotationAction*)PPCALCOBJECT->getCurrentAction();
		Angle = atn->Angle;
		   
		MAINDllOBJECT->selectShape(&(atn->RotatedShapesId));
		GetSelectedShapeCenter(OriginalShapesCenter);
		
		if(atn->RotationAxisId != -1)
		{
			double lineParam[6] = {0};
			ShapeAsRotationAxis = (Shape*)(MAINDllOBJECT->getCurrentUCS().getShapes().getList()[atn->RotationAxisId]);
			((Line*)ShapeAsRotationAxis)->getParameters(lineParam);
			RMATH3DOBJECT->Projection_Point_on_Line(OriginalShapesCenter, lineParam, RotationOrigin);
			for(int i = 0; i < 3; i++)
			{
				RotationPlane[i] = lineParam[3 + i];
				RotationPlane[3] += RotationPlane[i] * RotationOrigin[i];
			}
		}
		else if(atn->RotationPoint != NULL)
		{
			RotationOrigin[0] = atn->RotationPoint->getX(); RotationOrigin[1] = atn->RotationPoint->getY(); RotationOrigin[2] = OriginalShapesCenter[2];
			RotationPlane[0] = 0; RotationPlane[1] = 0; RotationPlane[2] = 1; 
			RotationPlane[3] = OriginalShapesCenter[2];
		}

		RMATH3DOBJECT->RotationAround3DAxisThroughOrigin(Angle, RotationPlane, rotateMatrix);
		for(RC_ITER item = MAINDllOBJECT->getSelectedShapesList().getList().begin(); item != MAINDllOBJECT->getSelectedShapesList().getList().end(); item++)
		{
			ShapeWithList* Current_Shape = (ShapeWithList*)(*item).second;
			if(!Current_Shape->Normalshape()) continue;
			if(!Current_Shape->PointsList->getList().empty())
			   Current_Shape->RotateShape(rotateMatrix, RotationOrigin);
		}
	}
}

bool RotateShapeHandler::GetSelectedShapeCenter(double *currentCenter)
{
	    int i = 0;
		for(RC_ITER item = MAINDllOBJECT->getSelectedShapesList().getList().begin(); item != MAINDllOBJECT->getSelectedShapesList().getList().end(); item++)
		{
			Shape* Current_Shape = (Shape*)(*item).second;
			if(!Current_Shape->Normalshape()) continue;
			double tmpCenter[3] = {0};
			Current_Shape->GetShapeCenter(tmpCenter);
			for(int k = 0; k < 3; k++) {currentCenter[k] += tmpCenter[k];}
			i++;
		}	
		if(i > 0)
		{
	    	for(int j = 0; j < 3; j++) {currentCenter[j] /= i;}
			return true;
		}
		return false;
}

bool RotateShapeHandler::CalculateMatrixesForRotation(double *currentShapeCenter, double *rotateMatrix)
{
	    int s1[2] = {3,3}, s2[2] = {3,3}, s3[2] = {3,1};
		double mSelectionLine[6] = {0}, targetPosition[3] = {0}, targetDir[3] = {0}, currentDir[3] = {0}; 
		MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(mSelectionLine);
		RMATH3DOBJECT->Intersection_Line_Plane(mSelectionLine, RotationPlane, targetPosition);
		for(int i = 0; i < 3; i++)
		{
		   targetDir[i] = targetPosition[i] - RotationOrigin[i];
		   currentDir[i] = currentShapeCenter[i] - RotationOrigin[i];
		}
	    Angle = RMATH3DOBJECT->Angle_Btwn_2Directions(targetDir, currentDir, true, false);
		double vectorDir[3] = {(currentDir[1] * targetDir[2] - currentDir[2] * targetDir[1]), (currentDir[2] * targetDir[0] - currentDir[0] * targetDir[2]), (currentDir[0] * targetDir[1] - currentDir[1] * targetDir[0])};
		RMATH3DOBJECT->DirectionCosines(vectorDir, vectorDir);
		if(RMATH3DOBJECT->Angle_Btwn_2Directions(vectorDir, RotationPlane, false, true) < 0)
			Angle *= -1;
		RMATH3DOBJECT->RotationAround3DAxisThroughOrigin(Angle, RotationPlane, rotateMatrix);
		return true;
}

void RotateShapeHandler::SetAnyData(double *data)
{
	if(data[0] != 0)
	{
		AngleDefined = true;
		Angle = data[0] * M_PI / 180;
		LmaxmouseDown();
		AngleDefined = false;
	}
}
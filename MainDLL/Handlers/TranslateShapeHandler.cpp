#include "StdAfx.h"
#include "TranslateShapeHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\ShapeWithList.h"
#include "..\Actions\TranslationAction.h"
//Default handler .. do nothing.....!!!!
TranslateShapeHandler::TranslateShapeHandler(bool pointtopoint)
{
	translationDefined = false;
	setMaxClicks(2);
	PointToPoint = pointtopoint;
}

TranslateShapeHandler::~TranslateShapeHandler()
{
}

void TranslateShapeHandler::LmouseDown()
{
	if(MAINDllOBJECT->getSelectedShapesList().getList().empty())
	{
			setClicksDone( getClicksDone() - 1);
			return;
	}
	memset(OriginalCenter, 0, 3 * sizeof(double));
	memset(translationDirection, 0, 3 * sizeof(double));
	if(PointToPoint)
	{
		if(MAINDllOBJECT->highlightedPoint() == NULL)
		{
		    setClicksDone( getClicksDone() - 1);
			return;
		}
		OriginalCenter[0] = getClicksValue(0).getX();
		OriginalCenter[1] = getClicksValue(0).getY();
		OriginalCenter[2] = getClicksValue(0).getZ();
	}
	else
	{
	   GetShapesCenter(OriginalCenter);
	   MAINDllOBJECT->DerivedShapeCallback();
	}	
}

void TranslateShapeHandler::mouseMove()
{
	if(getClicksDone() == 1 && !PointToPoint)
	{
		memset(TranslateParam, 0, 6 * sizeof(double));
	    GetShapesCenter(TranslateParam);
		double mSelectionLine[6] = {0};
		MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(mSelectionLine);
		RMATH3DOBJECT->Projection_Point_on_Line(TranslateParam, mSelectionLine, &TranslateParam[3]);
		for(RC_ITER item = MAINDllOBJECT->getSelectedShapesList().getList().begin(); item != MAINDllOBJECT->getSelectedShapesList().getList().end(); item++)
		{
			Shape* Current_Shape = (Shape*)(*item).second;
			if(!Current_Shape->Normalshape()) continue;
			Current_Shape->Translate(Vector(TranslateParam[3] - TranslateParam[0], TranslateParam[4] -TranslateParam[1], TranslateParam[5] -TranslateParam[2]));
		}
	}
	MAINDllOBJECT->UpdateShapesChanged();
}	

void TranslateShapeHandler::draw(int windowno, double WPixelWidth)
{
}

void TranslateShapeHandler::LmaxmouseDown()
{
	if(PointToPoint)
	{
	    if(MAINDllOBJECT->highlightedPoint() == NULL)
		{
		    setClicksDone( getClicksDone() - 1);
			return;
		}
	    for(int i = 0; i < 3; i++){TranslateParam[i] = OriginalCenter[i];}

		TranslateParam[3] = getClicksValue(1).getX();
		TranslateParam[4] = getClicksValue(1).getY();
		TranslateParam[5] = getClicksValue(1).getZ();

	    translationDirection[0] = TranslateParam[3] - TranslateParam[0];
		translationDirection[1] = TranslateParam[4] - TranslateParam[1];
		translationDirection[2] = TranslateParam[5] - TranslateParam[2];
	}
	else if(!translationDefined)
	{
		double mSelectionLine[6] = {0};	
		for(int i = 0; i < 3; i++){TranslateParam[i] = OriginalCenter[i];}
		MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(mSelectionLine);
		RMATH3DOBJECT->Projection_Point_on_Line(TranslateParam, mSelectionLine, &TranslateParam[3]);
		translationDirection[0] = TranslateParam[3] - TranslateParam[0];
		translationDirection[1] = TranslateParam[4] - TranslateParam[1];
		translationDirection[2] = TranslateParam[5] - TranslateParam[2];
	}
	TranslationAction *newAction = new TranslationAction();
    newAction->TranslateDirection.set(translationDirection[0], translationDirection[1], translationDirection[2]);
	for(RC_ITER item = MAINDllOBJECT->getSelectedShapesList().getList().begin(); item != MAINDllOBJECT->getSelectedShapesList().getList().end(); item++)
	{
		Shape* Current_Shape = (Shape*)(*item).second;
		if(!Current_Shape->Normalshape()) continue;
		newAction->TranslatedShapesId.push_back(Current_Shape->getId());
		if((Current_Shape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS) && !PointToPoint) continue;
        if(!((ShapeWithList*)Current_Shape)->PointsList->getList().empty())
		    Current_Shape->TranslateShape(translationDirection);
	}	
	MAINDllOBJECT->addAction(newAction);
}

void TranslateShapeHandler::EscapebuttonPress()
{
}

void TranslateShapeHandler::PartProgramData()
{
}

void TranslateShapeHandler::GetShapesCenter(double *centerValue)
{
	int i = 0;
	double tmpCenter[3] = {0};
	for(RC_ITER item = MAINDllOBJECT->getSelectedShapesList().getList().begin(); item != MAINDllOBJECT->getSelectedShapesList().getList().end(); item++)
	{
		Shape* Current_Shape = (Shape*)(*item).second;
		if(!Current_Shape->Normalshape()) continue;
		Current_Shape->GetShapeCenter(tmpCenter);
		for(int k = 0; k < 3; k++) {centerValue[k] += tmpCenter[k];}
		i++;
	}	
	if (i > 0)
		for(int j = 0; j < 3; j++) {centerValue[j] /= i;}
}

void TranslateShapeHandler::SetAnyData(double *data)
{
	if(data[0] != 0 || data[1] != 0 || data[2] != 0)
	{
		translationDefined = true;
		translationDirection[0] = data[0];
		translationDirection[1] = data[1];
		translationDirection[2] = data[2];
		LmaxmouseDown();
		translationDefined = false;
	}
}

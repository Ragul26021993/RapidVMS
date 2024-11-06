#include "stdafx.h"
#include "Cad2LineHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\DXF\DXFExpose.h"
#include "..\Shapes\Line.h"
#include "..\Actions\Cad2LineAction.h"

//Constructor...
Cad2LineHandler::Cad2LineHandler()
{
	try
	{
		this->AlignmentModeFlag = true;
		DxfShapes[0] = NULL; DxfShapes[1] = NULL;
		VideoShapes[0] = NULL; VideoShapes[1] = NULL;
		setMaxClicks(5);
		MAINDllOBJECT->SetStatusCode("Cad2LineHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD2LINEMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
Cad2LineHandler::~Cad2LineHandler()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD2LINEMH0002", __FILE__, __FUNCSIG__); }
}

void Cad2LineHandler::ResetShapeHighlighted()
{
	try
	{
		for(int i = 0; i < 2; i++)
		{
			if(DxfShapes[i] != NULL)
			{
				DxfShapes[i]->HighlightedForMeasurement(false);
				DxfShapes[i] =  NULL;
			}
			if(VideoShapes[i] != NULL)
			{
				VideoShapes[i]->HighlightedForMeasurement(false);
				VideoShapes[i] =  NULL;
			}
		}
		MAINDllOBJECT->DXFShape_Updated_Alignment();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PT1LINEMH0001", __FILE__, __FUNCSIG__); }
}

//Left mousedown.. select point and calculates the transforamtion matrix..//
void Cad2LineHandler::SelectPoints_Shapes()
{
	try
	{

		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(MAINDllOBJECT->highlightedShape()->ShapeType != RapidEnums::SHAPETYPE::LINE)
		{
			setClicksDone(getClicksDone() - 1);
			MAINDllOBJECT->SetStatusCode("DXFALGN0000");
			return;
		}
		if(MAINDllOBJECT->getCurrentWindow() == 2 && getClicksDone() < 3)
		{
			DxfShapes[getClicksDone() - 1] = MAINDllOBJECT->highlightedShape();
			if(getClicksDone() == 1)
				MAINDllOBJECT->SetStatusCode("DXFALGN0012");
			else
				MAINDllOBJECT->SetStatusCode("DXFALGN0013");
			DxfShapes[getClicksDone() - 1]->HighlightedForMeasurement(true);
			MAINDllOBJECT->DXFShape_Updated();
		}
		else if(getClicksDone() > 2 && MAINDllOBJECT->getCurrentWindow() != 2)
		{
			VideoShapes[getClicksDone() - 3] = MAINDllOBJECT->highlightedShape();
			if(getClicksDone() == 3)
				MAINDllOBJECT->SetStatusCode("DXFALGN0014");
			VideoShapes[getClicksDone() - 3]->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			MAINDllOBJECT->SetStatusCode("DXFALGN0000");
			return;
		}
		if(getClicksDone() == 4)
		{
			double TransformMatrix[9] = {0};
			double Line1Param[2] = {((Line*)DxfShapes[0])->Angle(), ((Line*)DxfShapes[0])->Intercept()};
			double Line2Param[2] = {((Line*)DxfShapes[1])->Angle(), ((Line*)DxfShapes[1])->Intercept()};
			double Line3Param[2] = {((Line*)VideoShapes[0])->Angle(), ((Line*)VideoShapes[0])->Intercept()};
			double Line4Param[2] = {((Line*)VideoShapes[1])->Angle(), ((Line*)VideoShapes[1])->Intercept()};
			double RotatedAngle, ShiftedOrigin[2] = { 0 };
			RMATH2DOBJECT->TransformM_TwoLineAlign(&Line1Param[0], &Line2Param[0], &Line3Param[0], &Line4Param[0], &TransformMatrix[0], &RotatedAngle, ShiftedOrigin);
			Vector TransPt;
			TransPt.set(TransformMatrix[2], TransformMatrix[5]);
			TransformMatrix[2] = 0; TransformMatrix[5] = 0;
			DXFEXPOSEOBJECT->TransformMatrix(&TransformMatrix[0]);
     		DXFEXPOSEOBJECT->SetTransVector(TransPt);
			Cad2LineAction::SetCad2LineAlign(DxfShapes[0], DxfShapes[1], VideoShapes[0], VideoShapes[1], MAINDllOBJECT->getCurrentUCS().getId(), MAINDllOBJECT->getCurrentWindow(), DXFEXPOSEOBJECT->CurrentDXFShapeList);
			MAINDllOBJECT->SetStatusCode("DXFALGN0015");
			resetClicks();
			DXFEXPOSEOBJECT->AlginTheDXF(true);
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD2LINEMH0003", __FILE__, __FUNCSIG__); }
}

//Check for the highlighted shape..
bool Cad2LineHandler::CheckHighlightedShape()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)return false;
		if(MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE) return true;
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD2LINEMH0004", __FILE__, __FUNCSIG__); return false; }
}

//Handle escape button press.. Clear the current selection..
void Cad2LineHandler::Align_Finished()
{
	try
	{
		ResetShapeHighlighted();
		resetClicks();
		MAINDllOBJECT->SetStatusCode("DXFALGN0001");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD2LINEMH0005", __FILE__, __FUNCSIG__); }
}

//draw selected point on dxf window..
void Cad2LineHandler::drawSelectedShapesOndxf()
{
}

//draw selected point on video..
void Cad2LineHandler::drawSelectedShapesOnvideo()
{
}

void Cad2LineHandler::Align_mouseMove(double x, double y)
{
}

void Cad2LineHandler::HandlePartProgramData()
{
	try
	{
		map<int, int> Idlist;
		Cad2LineAction *caction = (Cad2LineAction*)PPCALCOBJECT->getCurrentAction();
		DxfShapes[0] = caction->DxfShape1;
		DxfShapes[1] = caction->DxfShape2;
		VideoShapes[0] = caction->MainShape1;
		VideoShapes[1] = caction->MainShape2;

		double TransformMatrix[9] = {0};
		double Line1Param[2] = {((Line*)DxfShapes[0])->Angle(), ((Line*)DxfShapes[0])->Intercept()};
		double Line2Param[2] = {((Line*)DxfShapes[1])->Angle(), ((Line*)DxfShapes[1])->Intercept()};
		double Line3Param[2] = {((Line*)VideoShapes[0])->Angle(), ((Line*)VideoShapes[0])->Intercept()};
		double Line4Param[2] = {((Line*)VideoShapes[1])->Angle(), ((Line*)VideoShapes[1])->Intercept()};
		double RotatedAngle, ShiftedOrigin[2] = { 0 };
		RMATH2DOBJECT->TransformM_TwoLineAlign(&Line1Param[0], &Line2Param[0], &Line3Param[0], &Line4Param[0], &TransformMatrix[0], &RotatedAngle, ShiftedOrigin);
		Vector TransPt;
		TransPt.set(TransformMatrix[2], TransformMatrix[5]);
		TransformMatrix[2] = 0; TransformMatrix[5] = 0;
		caction->TransformShape(&TransformMatrix[0]);
     	caction->TranslateShape(TransPt);
		Align_Finished();
		DXFEXPOSEOBJECT->AlginTheDXF(true);
		PPCALCOBJECT->partProgramReached();
	}
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD2LNH0011", __FILE__, __FUNCSIG__); }
}
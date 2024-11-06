#include "stdafx.h"
#include "EditCad2LineHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\Line.h"
#include "..\DXF\DXFExpose.h"
#include "..\Actions\EditCad2LineAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor...
EditCad2LineHandler::EditCad2LineHandler()
{
	try
	{
		this->AlignmentModeFlag = true;
		VideoShapeCnt = 0; DxfShapeCnt = 0;
		DxfShapes[0] = NULL; DxfShapes[1] = NULL;
		VideoShapes[0] = NULL; VideoShapes[1] = NULL;
	    runningPartprogramValid = false;
		setMaxClicks(5);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD2LNH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
EditCad2LineHandler::~EditCad2LineHandler()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD2LNH0002", __FILE__, __FUNCSIG__); }
}

//Left mousedown.. select point and calculates the transforamtion matrix..//
void EditCad2LineHandler::SelectPoints_Shapes()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning()) return;
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
		if(MAINDllOBJECT->getCurrentWindow() != 2 && getClicksDone() < 3)
		{
			DxfShapes[DxfShapeCnt++] = MAINDllOBJECT->highlightedShape();
			if(DxfShapeCnt == 1)
				MAINDllOBJECT->SetStatusCode("DXFALGN0014");
			else
				MAINDllOBJECT->SetStatusCode("DXFALGN0013");
			DxfShapes[DxfShapeCnt - 1]->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
		}
		else if(getClicksDone() > 2 && MAINDllOBJECT->getCurrentWindow() != 2)
		{
			VideoShapes[VideoShapeCnt++] = MAINDllOBJECT->highlightedShape();
			if(VideoShapeCnt == 1)
				MAINDllOBJECT->SetStatusCode("DXFALGN0014");
			VideoShapes[VideoShapeCnt - 1]->HighlightedForMeasurement(true);
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
			DXFEXPOSEOBJECT->TransformMatrix(&TransformMatrix[0], false);
     		DXFEXPOSEOBJECT->SetTransVector(TransPt, false);
			std::list<int> idlist;
			idlist.push_back(DxfShapes[0]->getId()); idlist.push_back(DxfShapes[1]->getId());
			idlist.push_back(VideoShapes[0]->getId()); idlist.push_back(VideoShapes[1]->getId());
			//add action for 2 line alignment..........
			EditCad2LineAction::SetCad2LineAlign(&idlist);
			resetClicks();
			Align_Finished();
			DXFEXPOSEOBJECT->AlginTheDXF(true);
			DXFEXPOSEOBJECT->SetAlignedProperty();
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD2LNH0003", __FILE__, __FUNCSIG__); }
}

void EditCad2LineHandler::ResetShapeHighlighted()
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
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD2LNH0004", __FILE__, __FUNCSIG__); }
}

//Check for the highlighted shape..
bool EditCad2LineHandler::CheckHighlightedShape()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)return false;
		if(MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE) return true;
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD2LNH0005", __FILE__, __FUNCSIG__); return false; }
}

//Handle escape button press.. Clear the current selection..
void EditCad2LineHandler::Align_Finished()
{
	try
	{    
		VideoShapeCnt = 0; DxfShapeCnt = 0;
		ResetShapeHighlighted();
		resetClicks();
		MAINDllOBJECT->SetStatusCode("DXFALGN0001");
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD2LNH0006", __FILE__, __FUNCSIG__); }
}

//draw selected point on dxf window..
void EditCad2LineHandler::drawSelectedShapesOndxf()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD2LNH0007", __FILE__, __FUNCSIG__); }
}

void EditCad2LineHandler::drawSelectedShapeOnRCad()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD2LNH0008", __FILE__, __FUNCSIG__); }
}

//draw selected point on video..
void EditCad2LineHandler::drawSelectedShapesOnvideo()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD2LNH0009", __FILE__, __FUNCSIG__); }
}

 void EditCad2LineHandler::Align_mouseMove(double x, double y)
{
	 try
	 {
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD2LNH0010", __FILE__, __FUNCSIG__); }
}

 void EditCad2LineHandler::HandlePartProgramData()
{
	try
	{
		map<int, int> Idlist;
		((EditCad2LineAction*)PPCALCOBJECT->getCurrentAction())->getLineIdList(&Idlist);

		DxfShapes[0] = (Shape*)MAINDllOBJECT->getShapesList().getList()[Idlist[0]];
		DxfShapes[1] = (Shape*)MAINDllOBJECT->getShapesList().getList()[Idlist[1]];
		VideoShapes[0] = (Shape*)MAINDllOBJECT->getShapesList().getList()[Idlist[2]];
		VideoShapes[1] = (Shape*)MAINDllOBJECT->getShapesList().getList()[Idlist[3]];

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
		DXFEXPOSEOBJECT->TransformMatrix(&TransformMatrix[0], false);
     	DXFEXPOSEOBJECT->SetTransVector(TransPt, false);
		resetClicks();
		Align_Finished();
		DXFEXPOSEOBJECT->AlginTheDXF(true);
		DXFEXPOSEOBJECT->SetAlignedProperty();
		PPCALCOBJECT->partProgramReached();
	}
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD2LNH0011", __FILE__, __FUNCSIG__); }
}

 
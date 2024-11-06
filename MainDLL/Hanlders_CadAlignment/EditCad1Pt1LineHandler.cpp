#include "stdafx.h"
#include "EditCad1Pt1LineHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\DXF\DXFExpose.h"
#include "..\Actions\EditCad1Pt1LineAction.h"
#include "..\Shapes\Line.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor...
EditCad1Pt1LineHandler::EditCad1Pt1LineHandler()
{
	try
	{
		this->AlignmentModeFlag = true;
		VideoPointFlag = false; VideoShapeFlag = false;
		DxfPointFlag = false; DxfShapeFlag = false;
		runningPartprogramValid = false; AlignMentCompleted = false;
		DxfShape =  NULL; VideoShape = NULL;
		setMaxClicks(5);
		MAINDllOBJECT->SetStatusCode("Cad1Pt1LineHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PT1LINEMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
EditCad1Pt1LineHandler::~EditCad1Pt1LineHandler()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PT1LINEMH0002", __FILE__, __FUNCSIG__); }
}

void EditCad1Pt1LineHandler::ResetShapeHighlighted()
{
	try
	{
		if(DxfShape != NULL)
		{
			DxfShape->HighlightedForMeasurement(false);
			DxfShape =  NULL;
		}
		if(VideoShape != NULL)
		{
			VideoShape->HighlightedForMeasurement(false);
			VideoShape =  NULL;
		}
		MAINDllOBJECT->DXFShape_Updated_Alignment();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PT1LINEMH0001", __FILE__, __FUNCSIG__); }
}

//Left mousedown.. select point and calculates the transforamtion matrix..//
void EditCad1Pt1LineHandler::SelectPoints_Shapes()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid && (MAINDllOBJECT->getCurrentWindow() == 0))
			{
				Vector* pt = &getClicksValue(0);
				runningPartprogramValid = false;
				Dxfpoint = *((EditCad1Pt1LineAction*)PPCALCOBJECT->getCurrentAction())->getPoint1();
				VideoPoint.set(pt->getX(), pt->getY(), MAINDllOBJECT->getCurrentDRO().getZ());				
				double pt1[2] = {Dxfpoint.getX(), Dxfpoint.getY()}, pt2[2] = {VideoPoint.getX(), VideoPoint.getY()};
				double slope1 = ((Line*)DxfShape)->Angle(), slope2 = ((Line*)VideoShape)->Angle(), transform[9] = {0};
				double RotatedAngle, ShiftedOrigin[2] = { 0 };
				RMATH2DOBJECT->TransformM_PointLineAlign(&pt1[0], ((Line*)DxfShape)->Angle(), ((Line*)DxfShape)->Intercept(), &pt2[0], ((Line*)VideoShape)->Angle(), ((Line*)VideoShape)->Intercept(), &transform[0], &RotatedAngle, ShiftedOrigin);
				Vector TransPt;
				TransPt.set(transform[2], transform[5]);
				transform[2] = 0; transform[5] = 0;
				DXFEXPOSEOBJECT->TransformMatrix(&transform[0], false);
				DXFEXPOSEOBJECT->SetTransVector(TransPt, false);
				DXFEXPOSEOBJECT->SetAlignedProperty();
				Align_Finished();
				PPCALCOBJECT->partProgramReached();
			}
			else
				setClicksDone(getClicksDone() - 1);
			return;
		}

		if(MAINDllOBJECT->getCurrentWindow() != 2 && getClicksDone() < 3)
		{
			if(MAINDllOBJECT->highlightedPoint() == NULL && MAINDllOBJECT->highlightedShape() == NULL)
			{
				setClicksDone(getClicksDone() - 1);
				MAINDllOBJECT->SetStatusCode("DXFALGN0000");
				return;
			}
			if(!DxfPointFlag && MAINDllOBJECT->highlightedPoint() != NULL)
			{
				Dxfpoint.set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
				DxfPointFlag = true;
				if(!DxfShapeFlag)
					MAINDllOBJECT->SetStatusCode("DXFALGN0009");
			}
			else if(!DxfShapeFlag && CheckHighlightedShape())
			{
				DxfShape = MAINDllOBJECT->highlightedShape();
				DxfShapeFlag = true;
				if(!DxfPointFlag)
					MAINDllOBJECT->SetStatusCode("DXFALGN0001");
				DxfShape->HighlightedForMeasurement(true);
				MAINDllOBJECT->DXFShape_Updated();
			}
			else
			{
				setClicksDone(getClicksDone() - 1);
				MAINDllOBJECT->SetStatusCode("DXFALGN0000");
				return;
			}
			if(DxfPointFlag && DxfShapeFlag)
				MAINDllOBJECT->SetStatusCode("DXFALGN0010");

		}
		else if(getClicksDone() > 2 && MAINDllOBJECT->getCurrentWindow() != 2)
		{
			if(!VideoShapeFlag) VideoShape = MAINDllOBJECT->highlightedShape();
			if(!VideoShapeFlag && CheckHighlightedShape())
			{
				VideoShape = MAINDllOBJECT->highlightedShape();
				//add action for 1point 1line alignment..........
				double ShapeId[2] = {DxfShape->getId(), VideoShape->getId()};
				EditCad1Pt1LineAction::SetCad1Pt1LineAlign(ShapeId, MAINDllOBJECT->getCurrentUCS().getId());
				VideoShapeFlag = true;
				if(!VideoPointFlag)
					MAINDllOBJECT->SetStatusCode("DXFALGN0005");
				VideoShape->HighlightedForMeasurement(true);
				MAINDllOBJECT->Shape_Updated();
			}
			else
			{
				if(!VideoShapeFlag)
				{
					setClicksDone(getClicksDone() - 1);
					MAINDllOBJECT->SetStatusCode("DXFALGN0000");
					return;
				}
			}
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			MAINDllOBJECT->SetStatusCode("DXFALGN0000");
			return;
		}
		if(getClicksDone() == 4)
		{
			AlignMentCompleted = true;
			//set current shape for creating base action....
			for(RC_ITER Shpitem = MAINDllOBJECT->getShapesList().getList().begin(); Shpitem != MAINDllOBJECT->getShapesList().getList().end(); Shpitem++)
			{
				Shape* CShape = (Shape*)((*Shpitem).second);
				if(!CShape->Normalshape()) continue;
				if(!CShape->IsDxfShape()) continue;
				if(CShape->IsAlignedShape()) continue;
				CurrentShape = CShape;
				break;
			}
			setBaseRProperty(VideoPoint.getX(), VideoPoint.getY(), VideoPoint.getZ());		
			EditCad1Pt1LineAction::SetCad1Pt1LineAlign(VideoPoint, baseaction, MAINDllOBJECT->getCurrentUCS().getId());
			MAINDllOBJECT->SetStatusCode("DXFALGN0015");		
			resetClicks();
			Align_Finished();
			DXFEXPOSEOBJECT->AlginTheDXF(true);
			DXFEXPOSEOBJECT->SetAlignedProperty();
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PT1LINEMH0003", __FILE__, __FUNCSIG__); }
}

//Check for the highlighted shape..
bool EditCad1Pt1LineHandler::CheckHighlightedShape()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)return false;
		if(MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::XLINE
			|| MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::XRAY) return true;
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PT1LINEMH0004", __FILE__, __FUNCSIG__); return false; }
}

//Handle escape button press.. Clear the current selection..
void EditCad1Pt1LineHandler::Align_Finished()
{
	try
	{
		VideoPointFlag = false; VideoShapeFlag = false;
		DxfPointFlag = false; DxfShapeFlag = false;
		AlignMentCompleted = false;
		ResetShapeHighlighted();
		resetClicks();
		MAINDllOBJECT->SetStatusCode("DXFALGN0001");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PT1LINEMH0005", __FILE__, __FUNCSIG__); }
}

//draw selected point on dxf window..
void EditCad1Pt1LineHandler::drawSelectedShapesOndxf()
{
	try
	{
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PT1LINEMH0006", __FILE__, __FUNCSIG__); }
}

//draw selected point on video..
void EditCad1Pt1LineHandler::drawSelectedShapesOnvideo()
{
	try
	{
		double PixelWidth = MAINDllOBJECT->getWindow(2).getUppX();
		GRAFIX->SetColor_Double(0, 1, 0);
		if(VideoPointFlag)
			GRAFIX->drawPoint(VideoPoint.getX(), VideoPoint.getY(), 0, 8);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PT1LINEMH0007", __FILE__, __FUNCSIG__); }
}

void EditCad1Pt1LineHandler::drawSelectedShapeOnRCad()
{
	try
	{
		double PixelWidth = MAINDllOBJECT->getWindow(2).getUppX();
		GRAFIX->SetColor_Double(0, 1, 0);
		if(DxfPointFlag)
			GRAFIX->drawPoint(Dxfpoint.getX(), Dxfpoint.getY(), 0, 8);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PT1LINEMH0006", __FILE__, __FUNCSIG__); }
}

void EditCad1Pt1LineHandler::Align_mouseMove(double x, double y)
{
	 try
	 {
		 if(AlignMentCompleted || PPCALCOBJECT->IsPartProgramRunning()) return;
		 if(MAINDllOBJECT->getCurrentWindow() != 2 && getClicksDone() == 3)
		 {
			 if(VideoShapeFlag)
			 {
				 if(MAINDllOBJECT->highlightedPoint() != NULL)
					 VideoPoint.set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
				 else
					 VideoPoint.set(x, y);
				 double pt1[2] = {Dxfpoint.getX(), Dxfpoint.getY()}, pt2[2] = {VideoPoint.getX(), VideoPoint.getY()};
				 double slope1 = ((Line*)DxfShape)->Angle(), slope2 = ((Line*)VideoShape)->Angle(), transform[9] = {0};
				 double RotatedAngle, ShiftedOrigin[2] = { 0 };
				 RMATH2DOBJECT->TransformM_PointLineAlign(&pt1[0], ((Line*)DxfShape)->Angle(), ((Line*)DxfShape)->Intercept(), &pt2[0], ((Line*)VideoShape)->Angle(), ((Line*)VideoShape)->Intercept(), &transform[0], &RotatedAngle, ShiftedOrigin);
				 Dxfpoint.set(x, y);
				 Vector TransPt;
				 TransPt.set(transform[2], transform[5]);
				 transform[2] = 0; transform[5] = 0;
				 DXFEXPOSEOBJECT->TransformMatrix(&transform[0], false);
				 DXFEXPOSEOBJECT->SetTransVector(TransPt, false);
			 }
		 }
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PT1LINEMH0007", __FILE__, __FUNCSIG__); }
}

void EditCad1Pt1LineHandler::HandlePartProgramData()
{
	try
	{
		if(((EditCad1Pt1LineAction*)PPCALCOBJECT->getCurrentAction())->getActionFlag())
		{
			double ShapeId[2];
			((EditCad1Pt1LineAction*)PPCALCOBJECT->getCurrentAction())->getShapeId(ShapeId);
			DxfShape = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeId[0]];
			VideoShape = (Shape*)MAINDllOBJECT->getShapesList().getList()[ShapeId[1]];
			PPCALCOBJECT->partProgramReached();
		}
		else
		{
			baseaction = ((EditCad1Pt1LineAction*)PPCALCOBJECT->getCurrentAction())->getFramegrab();
			if(baseaction->CurrentWindowNo == 1)
			{
				if(baseaction->Pointer_SnapPt != NULL)
				{
					Dxfpoint = *((EditCad1Pt1LineAction*)PPCALCOBJECT->getCurrentAction())->getPoint1();
					VideoPoint.set(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
					double pt1[2] = {Dxfpoint.getX(), Dxfpoint.getY()}, pt2[2] = {VideoPoint.getX(), VideoPoint.getY()};
					double slope1 = ((Line*)DxfShape)->Angle(), slope2 = ((Line*)VideoShape)->Angle(), transform[9] = {0};
					double RotatedAngle, ShiftedOrigin[2] = { 0 };
					RMATH2DOBJECT->TransformM_PointLineAlign(&pt1[0], ((Line*)DxfShape)->Angle(), ((Line*)DxfShape)->Intercept(), &pt2[0], ((Line*)VideoShape)->Angle(), ((Line*)VideoShape)->Intercept(), &transform[0], &RotatedAngle, ShiftedOrigin);
					Vector TransPt;
					TransPt.set(transform[2], transform[5]);
					transform[2] = 0; transform[5] = 0;
					DXFEXPOSEOBJECT->TransformMatrix(&transform[0], false);
					DXFEXPOSEOBJECT->SetTransVector(TransPt, false);
					DXFEXPOSEOBJECT->SetAlignedProperty();
					Align_Finished();
					PPCALCOBJECT->partProgramReached();
					return;
				}
				else
				{
					DXFEXPOSEOBJECT->SetAlignedProperty();
					Align_Finished();
					PPCALCOBJECT->partProgramReached();
					return;
				}
			}
			runningPartprogramValid = true;
			MAINDllOBJECT->SetStatusCode("DXFALGN0005");
		}
	}
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PT1LINEMH0008", __FILE__, __FUNCSIG__); }
}
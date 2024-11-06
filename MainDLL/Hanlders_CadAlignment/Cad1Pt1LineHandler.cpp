#include "stdafx.h"
#include "Cad1Pt1LineHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\DXF\DXFExpose.h"
#include "..\Actions\Cad1Pt1LineAction.h"
#include "..\Shapes\Line.h"

//Constructor...
Cad1Pt1LineHandler::Cad1Pt1LineHandler()
{
	try
	{
		this->AlignmentModeFlag = true;
		DxfShape =  NULL; VideoShape = NULL;
		runningPartprogramValid = false;
		setMaxClicks(5);
		MAINDllOBJECT->SetStatusCode("Cad1Pt1LineHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PT1LINEMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
Cad1Pt1LineHandler::~Cad1Pt1LineHandler()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PT1LINEMH0002", __FILE__, __FUNCSIG__); }
}

void Cad1Pt1LineHandler::ResetShapeHighlighted()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PT1LINEMH0001", __FILE__, __FUNCSIG__); }
}

//Left mousedown.. select point and calculates the transforamtion matrix..//
void Cad1Pt1LineHandler::SelectPoints_Shapes()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid && (MAINDllOBJECT->getCurrentWindow() == 0))
			{
				Cad1Pt1LineAction *caction = (Cad1Pt1LineAction*)PPCALCOBJECT->getCurrentAction();
				Vector* pt = &getClicksValue(0);
				runningPartprogramValid = false;
				Dxfpoint = *(caction->getPoint1());
				double pt1[2] = {Dxfpoint.getX(), Dxfpoint.getY()}, pt2[2] = {pt->getX(), pt->getY()};
				double slope1 = ((Line*)DxfShape)->Angle(), slope2 = ((Line*)VideoShape)->Angle(), transform[9] = {0};
				double RotatedAngle, ShiftedOrigin[2] = { 0 };
				RMATH2DOBJECT->TransformM_PointLineAlign(&pt1[0], ((Line*)DxfShape)->Angle(), ((Line*)DxfShape)->Intercept(), &pt2[0], 
								((Line*)VideoShape)->Angle(), ((Line*)VideoShape)->Intercept(), &transform[0], &RotatedAngle, ShiftedOrigin);
				Vector TransPt;
				TransPt.set(transform[2], transform[5]);
				transform[2] = 0; transform[5] = 0;
				caction->TransformShape(&transform[0]);
				caction->TranslateShape(TransPt);
				Align_Finished();
				PPCALCOBJECT->partProgramReached();
			}
			else
				setClicksDone(getClicksDone() - 1);
			return;
		}
		if(MAINDllOBJECT->getCurrentWindow() == 2 && getClicksDone() < 3)
		{
			if(MAINDllOBJECT->highlightedPoint() == NULL && MAINDllOBJECT->highlightedShape() == NULL)
			{
				setClicksDone(getClicksDone() - 1);
				MAINDllOBJECT->SetStatusCode("DXFALGN0000");
				return;
			}
			if(getClicksDone() == 2 && MAINDllOBJECT->highlightedPoint() != NULL)
			{
				Dxfpoint.set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
				//MAINDllOBJECT->SetStatusCode("DXFALGN0009");
				MAINDllOBJECT->SetStatusCode("DXFALGN0010");
			}
			else if(getClicksDone() == 1 && CheckHighlightedShape())
			{
				DxfShape = MAINDllOBJECT->highlightedShape();
				DxfShape->HighlightedForMeasurement(true);
				MAINDllOBJECT->DXFShape_Updated();
				//MAINDllOBJECT->SetStatusCode("DXFALGN0010");
			}
			else
			{
				setClicksDone(getClicksDone() - 1);
				MAINDllOBJECT->SetStatusCode("DXFALGN0000");
				return;
			}
		}
		else if(getClicksDone() == 3 && MAINDllOBJECT->getCurrentWindow() != 2)
		{
			if(CheckHighlightedShape())
			{
				VideoShape = MAINDllOBJECT->highlightedShape();
				/*MAINDllOBJECT->SetStatusCode("DXFALGN0005");*/
				VideoShape->HighlightedForMeasurement(true);
				MAINDllOBJECT->Shape_Updated();
			}
			else
			{
				setClicksDone(getClicksDone() - 1);
				MAINDllOBJECT->SetStatusCode("DXFALGN0000");
				return;
			}
		}
		else if(getClicksDone() == 4 && MAINDllOBJECT->getCurrentWindow() != 2)
		{
			if(MAINDllOBJECT->highlightedPoint() != NULL )
			{
				if(MAINDllOBJECT->getCurrentWindow() == 1)
			    	VideoPoint = MAINDllOBJECT->highlightedPoint();
				else
					VideoPoint = new Vector(*(MAINDllOBJECT->highlightedPoint()));
				double ShapeId[2] = {DxfShape->getId(), VideoShape->getId()};
				Cad1Pt1LineAction::SetCad1Pt1LineAlign(DxfShape, VideoShape, Dxfpoint, VideoPoint, MAINDllOBJECT->getCurrentUCS().getId(), MAINDllOBJECT->getCurrentWindow(), DXFEXPOSEOBJECT->CurrentDXFShapeList);
				MAINDllOBJECT->SetStatusCode("DXFALGN0015");		
				resetClicks();
				DXFEXPOSEOBJECT->AlginTheDXF(true);
				return;
			}
			else
			{
				setClicksDone(getClicksDone() - 1);
				MAINDllOBJECT->SetStatusCode("DXFALGN0000");
				return;
			}
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			MAINDllOBJECT->SetStatusCode("DXFALGN0000");
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PT1LINEMH0003", __FILE__, __FUNCSIG__); }
}

//Check for the highlighted shape..
bool Cad1Pt1LineHandler::CheckHighlightedShape()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)return false;
		if(MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::XLINE
			|| MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::XRAY) return true;
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PT1LINEMH0004", __FILE__, __FUNCSIG__); return false; }
}

//Handle escape button press.. Clear the current selection..
void Cad1Pt1LineHandler::Align_Finished()
{
	try
	{
		ResetShapeHighlighted();
		resetClicks();
		MAINDllOBJECT->SetStatusCode("DXFALGN0001");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PT1LINEMH0005", __FILE__, __FUNCSIG__); }
}

//draw selected point on dxf window..
void Cad1Pt1LineHandler::drawSelectedShapesOndxf()
{
	try
	{
		double PixelWidth = MAINDllOBJECT->getWindow(2).getUppX();
		GRAFIX->SetColor_Double(0, 1, 0);
		if(getClicksDone() >= 2)
			GRAFIX->drawPoint(Dxfpoint.getX(), Dxfpoint.getY(), 0, 8);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PT1LINEMH0006", __FILE__, __FUNCSIG__); }
}

//draw selected point on video..
void Cad1Pt1LineHandler::drawSelectedShapesOnvideo()
{
	try
	{
		/*double PixelWidth = MAINDllOBJECT->getWindow(2).getUppX();
		GRAFIX->SetColor_Double(0, 1, 0);
		if(getClicksDone() >= 3)
			GRAFIX->drawPoint(VideoPoint->getX(), VideoPoint->getY(), 0, 8);*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PT1LINEMH0007", __FILE__, __FUNCSIG__); }
}

void Cad1Pt1LineHandler::Align_mouseMove(double x, double y)
{
	 try
	 {
		 if(MAINDllOBJECT->getCurrentWindow() != 2 && getClicksDone() == 3)
		 {
			Vector tmp = Vector(x, y);
			double pt1[2] = {Dxfpoint.getX(), Dxfpoint.getY()}, pt2[2] = {tmp.getX(), tmp.getY()};
			double slope1 = ((Line*)DxfShape)->Angle(), slope2 = ((Line*)VideoShape)->Angle(), transform[9] = {0};
			double RotatedAngle, ShiftedOrigin[2] = { 0 };
			RMATH2DOBJECT->TransformM_PointLineAlign(&pt1[0], ((Line*)DxfShape)->Angle(), ((Line*)DxfShape)->Intercept(), &pt2[0], 
				((Line*)VideoShape)->Angle(), ((Line*)VideoShape)->Intercept(), &transform[0], &RotatedAngle, ShiftedOrigin);
			Dxfpoint.set(x, y);
			Vector TransPt;
			TransPt.set(transform[2], transform[5]);
			transform[2] = 0; transform[5] = 0;
			DXFEXPOSEOBJECT->TransformMatrix(&transform[0]);
			DXFEXPOSEOBJECT->SetTransVector(TransPt);
		 }
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PT1LINEMH0007", __FILE__, __FUNCSIG__); }
}

void Cad1Pt1LineHandler::HandlePartProgramData()
{
	try
	{
			Cad1Pt1LineAction *caction = (Cad1Pt1LineAction*)PPCALCOBJECT->getCurrentAction();
			DxfShape = caction->DxfShape;
			VideoShape = caction->MainShape;
			if(caction->windowNo == 1)
			{
				if(caction->targetP != NULL)
				{
					Dxfpoint = *((Cad1Pt1LineAction*)PPCALCOBJECT->getCurrentAction())->getPoint1();
					double pt1[2] = {Dxfpoint.getX(), Dxfpoint.getY()}, pt2[2] = {caction->targetP->getX(), caction->targetP->getY()};
					double slope1 = ((Line*)DxfShape)->Angle(), slope2 = ((Line*)VideoShape)->Angle(), transform[9] = {0};
					double RotatedAngle, ShiftedOrigin[2] = { 0 };
					RMATH2DOBJECT->TransformM_PointLineAlign(&pt1[0], ((Line*)DxfShape)->Angle(), ((Line*)DxfShape)->Intercept(), &pt2[0], 
						((Line*)VideoShape)->Angle(), ((Line*)VideoShape)->Intercept(), &transform[0], &RotatedAngle, ShiftedOrigin);
					Vector TransPt;
					TransPt.set(transform[2], transform[5]);
					transform[2] = 0; transform[5] = 0;
					caction->TransformShape(&transform[0]);
					caction->TranslateShape(TransPt);
					Align_Finished();
					PPCALCOBJECT->partProgramReached();
					return;
				}
				else
				{
					Align_Finished();
					PPCALCOBJECT->partProgramReached();
					return;
				}
			}
			runningPartprogramValid = true;
			MAINDllOBJECT->SetStatusCode("DXFALGN0005");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PT1LINEMH0008", __FILE__, __FUNCSIG__); }
}
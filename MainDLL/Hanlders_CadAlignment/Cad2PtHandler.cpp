#include "stdafx.h"
#include "Cad2PtHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\DXF\DXFExpose.h"
#include "..\Actions\Cad2PtAlignAction.h"

//Constructor...
Cad2PtHandler::Cad2PtHandler()
{
	try
	{
		this->AlignmentModeFlag = true;
	    setMaxClicks(5);
		runningPartprogramValid = false;
		MAINDllOBJECT->SetStatusCode("Cad2PtHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD2PTMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
Cad2PtHandler::~Cad2PtHandler()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD2PTMH0002", __FILE__, __FUNCSIG__); }
}

//Left mousedown.. select point and calculates the transforamtion matrix..//
void Cad2PtHandler::SelectPoints_Shapes()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid && (MAINDllOBJECT->getCurrentWindow() == 0))
			{
				Cad2PtAlignAction *cAction = (Cad2PtAlignAction*)(PPCALCOBJECT->getCurrentAction());
				if(!cAction->action1Finished)
	        	{
					Vector* pt = &getClicksValue(0);
					points[0] = *(cAction->getPoint1());
					targetP1 = pt;
				    cAction->action1Finished = true;
					HandlePartProgramData();
				}
				else if(!cAction->action2Finished)
				{
					Vector* pnt = &getClicksValue(1);
					points[1] =  *(cAction->getPoint2());
					targetP2 = pnt;
					Vector Shift;
					Shift.set(targetP1->getX() - points[0].getX(), targetP1->getY() - points[0].getY(), targetP1->getZ() - points[0].getZ());
					DXFEXPOSEOBJECT->SetTransVector(Shift, false);
					double Angle1 = RMATH2DOBJECT->ray_angle(points[0].getX(), points[0].getY(), points[1].getX(), points[1].getY());
					double Angle2 = RMATH2DOBJECT->ray_angle(targetP1->getX(), targetP1->getY(), targetP2->getX(), targetP2->getY());
					double AngDiff = Angle2 - Angle1;
					double pt[3] = {targetP1->getX(), targetP1->getY(), targetP1->getZ()};
					double transform[9] = {0};
					RMATH2DOBJECT->TransformM_RotateAboutPoint(&pt[0], AngDiff, &transform[0]);
					Vector TransPt;
					TransPt.set(transform[2], transform[5]);
					transform[2] = 0; transform[5] = 0;
					cAction->TransformShape(&transform[0]);
					cAction->TranslateShape(TransPt);
					cAction->action1Finished = false;
					runningPartprogramValid = false;
			    	resetClicks();
					PPCALCOBJECT->partProgramReached();
					return;
				}
			}
			else
				setClicksDone(getClicksDone() - 1);
			return;
		}
		if(MAINDllOBJECT->getCurrentWindow() == 2 && getClicksDone() < 3)
		{
			if(MAINDllOBJECT->highlightedPoint() != NULL)
		       points[getClicksDone() - 1].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY());
			else
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
			if(getClicksDone() == 1)
				MAINDllOBJECT->SetStatusCode("DXFALGN0006");
			else
				MAINDllOBJECT->SetStatusCode("DXFALGN0007");

		}
		else if(getClicksDone() == 3 && MAINDllOBJECT->getCurrentWindow() != 2)
		{
			if(MAINDllOBJECT->highlightedPoint() != NULL && MAINDllOBJECT->getCurrentWindow() == 1)
				targetP1 = MAINDllOBJECT->highlightedPoint();
			else
				targetP1 = new Vector(getClicksValue(getClicksDone() - 1));

			double pt1[2] = {points[0].getX(), points[0].getY()}, pt2[2] = {targetP1->getX(), targetP1->getY()}, transform[9] = {0};
			RMATH2DOBJECT->TransformM_OnePtAlign(&pt1[0], &pt2[0], &transform[0]);
			points[0].set(targetP1->getX(), targetP1->getY());
			Vector TransPt;
			TransPt.set(transform[2], transform[5]);
			DXFEXPOSEOBJECT->SetTransVector(TransPt);
			points[1].set(MAINDllOBJECT->TransformMultiply(transform, points[1].getX(), points[1].getY()));
			clickWindow[0] = MAINDllOBJECT->getCurrentWindow();
			MAINDllOBJECT->SetStatusCode("DXFALGN0008");
		}
		else if(getClicksDone() == 4 && MAINDllOBJECT->getCurrentWindow() != 2)
		{
			if(MAINDllOBJECT->highlightedPoint() != NULL && MAINDllOBJECT->getCurrentWindow() == 1)
		    	targetP2 = MAINDllOBJECT->highlightedPoint();
			else
				targetP2 = new Vector(getClicksValue(getClicksDone() - 1));
			
			clickWindow[1] = MAINDllOBJECT->getCurrentWindow();
			Cad2PtAlignAction::SetCad2PtAlign(points[0], points[1], targetP1, targetP2, MAINDllOBJECT->getCurrentUCS().getId(), clickWindow, DXFEXPOSEOBJECT->CurrentDXFShapeList);
			MAINDllOBJECT->SetStatusCode("DXFALGN0015");

			double Angle1 = RMATH2DOBJECT->ray_angle(points[0].getX(), points[0].getY(), points[1].getX(), points[1].getY());
			double Angle2 = RMATH2DOBJECT->ray_angle(targetP1->getX(), targetP1->getY(), targetP2->getX(), targetP2->getY());
			double AngDiff = Angle2 - Angle1;
			double pt[3] = {targetP1->getX(), targetP1->getY(), targetP1->getZ() };
			double transform[9] = {0};
			RMATH2DOBJECT->TransformM_RotateAboutPoint(&pt[0], AngDiff, &transform[0]);
			Vector TransPt;
			TransPt.set(transform[2], transform[5]);
			transform[2] = 0; transform[5] = 0;
			DXFEXPOSEOBJECT->TransformMatrix(&transform[0]);
			DXFEXPOSEOBJECT->SetTransVector(TransPt);


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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD2PTMH0003", __FILE__, __FUNCSIG__); }
}

//Handle escape button press.. Clear the current selection..
void Cad2PtHandler::Align_Finished()
{
	try
	{
		resetClicks();
		MAINDllOBJECT->SetStatusCode("DXFALGN0001");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD2PTMH0004", __FILE__, __FUNCSIG__); }
}

//draw selected point on dxf window..
void Cad2PtHandler::drawSelectedShapesOndxf()
{
	try
	{
		GRAFIX->SetColor_Double(0, 1, 0);
		if(getClicksDone() >= 1)
			GRAFIX->drawPoint(points[0].getX(), points[0].getY(), 0, 8);
		if(getClicksDone() == 2)
			GRAFIX->drawPoint(points[1].getX(), points[1].getY(), 0, 8);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD2PTMH0005", __FILE__, __FUNCSIG__); }
}

//draw selected point on video..
void Cad2PtHandler::drawSelectedShapesOnvideo()
{
	try
	{
		GRAFIX->SetColor_Double(0, 1, 0);
		if(getClicksDone() >= 3)
			GRAFIX->drawPoint(points[2].getX(), points[2].getY(), 0, 8);
		if(getClicksDone() == 4)
			GRAFIX->drawPoint(points[3].getX(), points[3].getY(), 0, 8);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD2PTMH0006", __FILE__, __FUNCSIG__); }
}

void Cad2PtHandler::Align_mouseMove(double x, double y)
{
	 try
	 {
		
		 if (MAINDllOBJECT->getCurrentWindow() != 2 && MAINDllOBJECT->FloatDXFwithMouseMove && getClicksDone() == 3)
		 { 
			 Vector tmp = Vector(x, y);
			 double Angle1 = RMATH2DOBJECT->ray_angle(points[0].getX(), points[0].getY(), points[1].getX(), points[1].getY());
			 double Angle2 = RMATH2DOBJECT->ray_angle(targetP1->getX(), targetP1->getY(), tmp.getX(), tmp.getY());
			 double AngDiff = Angle2 - Angle1;
			 double pt[3] = {targetP1->getX(), targetP1->getY(), targetP1->getZ()};
			 double transform[9] = {0};
			 RMATH2DOBJECT->TransformM_RotateAboutPoint(&pt[0], AngDiff, &transform[0]);
		     Vector TransPt;
			 TransPt.set(transform[2], transform[5]);
			 transform[2] = 0; transform[5] = 0;
			 DXFEXPOSEOBJECT->TransformMatrix(&transform[0]);
			 DXFEXPOSEOBJECT->SetTransVector(TransPt);
			 points[1].set(x, y);
		 }
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD2PTMH0007", __FILE__, __FUNCSIG__);}
}

 void Cad2PtHandler::HandlePartProgramData()
{
	try
	{
		if(PPCALCOBJECT->getCurrentAction()->CurrentActionType != RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION) return;
		Cad2PtAlignAction *cAction = (Cad2PtAlignAction*)(PPCALCOBJECT->getCurrentAction());
		runningPartprogramValid = true;
		if(!cAction->action1Finished)
		{
			if(cAction->clickWindow1 == 0)
				return;
			else
			{
			    points[0] = *((Cad2PtAlignAction*)PPCALCOBJECT->getCurrentAction())->getPoint1();
				targetP1 = cAction->targetP1;
			}
		}
		if(!cAction->action2Finished)
		{
			if(cAction->clickWindow1 == 0)
				return;
			else
			{
				Vector Shift, TransPt;
			    points[1] = *((Cad2PtAlignAction*)PPCALCOBJECT->getCurrentAction())->getPoint2();
				Shift.set(targetP1->getX() - points[0].getX(), targetP1->getY() - points[0].getY(), targetP1->getZ() - points[0].getZ());
				DXFEXPOSEOBJECT->SetTransVector(Shift, false);
				double Angle1 = RMATH2DOBJECT->ray_angle(points[0].getX(), points[0].getY(), points[1].getX(), points[1].getY());
				double Angle2 = RMATH2DOBJECT->ray_angle(targetP1->getX(), targetP1->getY(), cAction->targetP2->getX(), cAction->targetP2->getY());
				double AngDiff = Angle2 - Angle1;
				double pt[3] = {targetP1->getX(), targetP1->getY(), targetP1->getZ()};
				double transform[9] = {0};
				RMATH2DOBJECT->TransformM_RotateAboutPoint(&pt[0], AngDiff, &transform[0]);
	
				TransPt.set(transform[2], transform[5]);
				transform[2] = 0; transform[5] = 0;
				cAction->TransformShape(&transform[0]);
				cAction->TranslateShape(TransPt);
				cAction->action1Finished = false;
				resetClicks();
				PPCALCOBJECT->partProgramReached();
			}
		}
		runningPartprogramValid = false;
		MAINDllOBJECT->SetStatusCode("DXFALGN0005");
	}
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PTMH0010", __FILE__, __FUNCSIG__); }
}

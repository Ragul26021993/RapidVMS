#include "stdafx.h"
#include "Cad1PtHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\DXF\DXFExpose.h"
#include "..\Actions\Cad1PtAlignAction.h"
//Constructor...
Cad1PtHandler::Cad1PtHandler()
{
	try
	{
		this->AlignmentModeFlag = true;
		setMaxClicks(2);
		MAINDllOBJECT->SetStatusCode("Cad1PtHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PTMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
Cad1PtHandler::~Cad1PtHandler()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PTMH0002", __FILE__, __FUNCSIG__); }
}

//Left mousedown.. select point and calculates the transforamtion matrix..//
void Cad1PtHandler::SelectPoints_Shapes()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid && (MAINDllOBJECT->getCurrentWindow() == 0))
			{
				Vector* pt = &getClicksValue(0);
				runningPartprogramValid = false;
				point1 = *((Cad1PtAlignAction*)PPCALCOBJECT->getCurrentAction())->getPoint1();
				Vector Shift;
				Shift.set(pt->getX() - point1.getX(), pt->getY() - point1.getY(), pt->getZ() - point1.getZ());
				((Cad1PtAlignAction*)PPCALCOBJECT->getCurrentAction())->TranslateShape(Shift);
				DXFEXPOSEOBJECT->SetAlignedProperty();
			  	resetClicks();
				PPCALCOBJECT->partProgramReached();
			}
			else
				setClicksDone(getClicksDone() - 1);
			return;
		}
		else if(MAINDllOBJECT->getCurrentWindow() == 2 && getClicksDone() == 1)
		{
			if(MAINDllOBJECT->highlightedPoint() == NULL)
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
			point1.set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY(), MAINDllOBJECT->highlightedPoint()->getZ());
			oldPoint = point1;
			MAINDllOBJECT->SetStatusCode("DXFALGN0005");
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			MAINDllOBJECT->SetStatusCode("DXFALGN0000");
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PTMH0003", __FILE__, __FUNCSIG__); }
}

//Handle escape button press.. Clear the current selection..
void Cad1PtHandler::Align_Finished()
{
	try
	{
		Vector Shift;
		Shift.set(oldPoint.getX() - point1.getX(), oldPoint.getY() - point1.getY(), oldPoint.getZ() - point1.getZ());
		DXFEXPOSEOBJECT->SetTransVector(Shift);
		resetClicks();
		MAINDllOBJECT->SetStatusCode("DXFALGN0001");
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PTMH0004", __FILE__, __FUNCSIG__); }
}

//draw selected point on dxf window..
void Cad1PtHandler::drawSelectedShapesOndxf()
{
	try
	{
		GRAFIX->SetColor_Double(0, 1, 0);
		if(getClicksDone() == 1)
			GRAFIX->drawPoint(point1.getX(), point1.getY(), 0, 8);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PTMH0005", __FILE__, __FUNCSIG__); }
}

//draw selected point on video..
void Cad1PtHandler::drawSelectedShapesOnvideo()
{
	try
	{
		GRAFIX->SetColor_Double(0, 1, 0);
		if(getClicksDone() == 2)
			GRAFIX->drawPoint(point2->getX(), point2->getY(), 0, 8);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PTMH0006", __FILE__, __FUNCSIG__); }
}

void Cad1PtHandler::Align_mouseMove(double x, double y)
{
	 try
	 {
		 if(MAINDllOBJECT->getCurrentWindow() != 2 && getClicksDone() == 1)
		 { 
			double pt1[3] = {point1.getX(), point1.getY(), point1.getZ()}, pt2[3] = {x, y, MAINDllOBJECT->getCurrentDRO().getZ()};
			point1.set(x, y, pt1[2]);
			DXFEXPOSEOBJECT->SetTransVector(pt1, pt2);
		 }
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PTMH0007", __FILE__, __FUNCSIG__);}
}

void Cad1PtHandler::LmaxmouseDown()
{
    if(MAINDllOBJECT->getCurrentWindow() != 2 )
	{
		if(MAINDllOBJECT->highlightedPoint() != NULL && MAINDllOBJECT->getCurrentWindow() == 1)
			point2 = MAINDllOBJECT->highlightedPoint();
		else
			point2 = new Vector(getClicksValue(1).getX(), getClicksValue(1).getY(), MAINDllOBJECT->getCurrentDRO().getZ());
		Cad1PtAlignAction::SetCad1PtAlign(oldPoint, point2, MAINDllOBJECT->getCurrentUCS().getId(),  MAINDllOBJECT->getCurrentWindow(), DXFEXPOSEOBJECT->CurrentDXFShapeList);
		MAINDllOBJECT->SetStatusCode("DXFALGN0015");
		resetClicks();
		DXFEXPOSEOBJECT->AlginTheDXF(true);
		return;			
	}
	else
	{
		setClicksDone(getMaxClicks() - 1);
		MAINDllOBJECT->SetStatusCode("DXFALGN0000");
		return;
	}
}

void Cad1PtHandler::HandlePartProgramData()
{
	try
	{
		Cad1PtAlignAction *caction = (Cad1PtAlignAction*)(PPCALCOBJECT->getCurrentAction());
		if(caction->windowNo == 1)
		{
			if(caction->TargetPnt != NULL)
			{
				Vector Shift;
				point1 = *((Cad1PtAlignAction*)PPCALCOBJECT->getCurrentAction())->getPoint1();
				Shift.set(caction->TargetPnt->getX() - point1.getX(), caction->TargetPnt->getY() - point1.getY(), caction->TargetPnt->getZ() - point1.getZ());
				((Cad1PtAlignAction*)PPCALCOBJECT->getCurrentAction())->TranslateShape(Shift);
				DXFEXPOSEOBJECT->SetAlignedProperty();
				resetClicks();
				PPCALCOBJECT->partProgramReached();
				return;
			}
			else
			{
				DXFEXPOSEOBJECT->SetAlignedProperty();
				resetClicks();
				PPCALCOBJECT->partProgramReached();
				return;
			}
		}
		runningPartprogramValid = true;
		MAINDllOBJECT->SetStatusCode("DXFALGN0005");
	}
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PTMH0010", __FILE__, __FUNCSIG__); }
}
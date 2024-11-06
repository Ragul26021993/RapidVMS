#include "stdafx.h"
#include "EditCad1PtHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\DXF\DXFExpose.h"
#include "..\Actions\EditCad1PtAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor...
EditCad1PtHandler::EditCad1PtHandler()
{
	try
	{
		this->AlignmentModeFlag = true;
		VideoFlag = false; runningPartprogramValid = false; AlignMentCompleted = false;
		setMaxClicks(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PTMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
EditCad1PtHandler::~EditCad1PtHandler()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PTMH0002", __FILE__, __FUNCSIG__); }
}

void EditCad1PtHandler::SelectPoints_HeightGauge(double x, double y, double z)
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				VideoFlag = true,
				points[0] = *((EditCad1PtAction*)PPCALCOBJECT->getCurrentAction())->getPoint1();
				points[1].set(x, y, z);
				Vector Shift;
				Shift.set(points[1].getX() - points[0].getX(), points[1].getY() - points[0].getY(), 0);
				DXFEXPOSEOBJECT->SetTransVector(Shift, false);
				DXFEXPOSEOBJECT->SetAlignedProperty();
				Align_Finished();
				PPCALCOBJECT->partProgramReached();
			}
			return;
		}

		if(getClicksDone() == 1)
		{
			AlignMentCompleted = true;
			points[1].set(x, y, z);
			VideoFlag = true;		
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
			Vector Shift;
			Shift.set(points[1].getX() - points[0].getX(), points[1].getY() - points[0].getY(), 0);
			DXFEXPOSEOBJECT->SetTransVector(Shift, false);
			DXFEXPOSEOBJECT->SetAlignedProperty();
			setBaseRProperty(points[1].getX(), points[1].getY(), points[1].getZ());
			baseaction->CurrentWindowNo = 0;
			EditCad1PtAction::SetCad1PtAlign(points[1], baseaction, MAINDllOBJECT->getCurrentUCS().getId());
			MAINDllOBJECT->SetStatusCode("DXFALGN0015");
			resetClicks();
			DXFEXPOSEOBJECT->AlginTheDXF(true);
			return;			
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PTMH0003a", __FILE__, __FUNCSIG__); }
}

//Left mousedown.. select point and calculates the transforamtion matrix..//
void EditCad1PtHandler::SelectPoints_Shapes()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid && (MAINDllOBJECT->getCurrentWindow() == 0))
			{
				Vector* pt = &getClicksValue(0);
				runningPartprogramValid = false;
				VideoFlag = true,
				points[0] = *((EditCad1PtAction*)PPCALCOBJECT->getCurrentAction())->getPoint1();
				points[1].set(pt->getX(), pt->getY(), MAINDllOBJECT->getCurrentDRO().getZ());
				Vector Shift;
				Shift.set(points[1].getX() - points[0].getX(), points[1].getY() - points[0].getY(), points[1].getZ() - points[0].getZ());
				DXFEXPOSEOBJECT->SetTransVector(Shift, false);
				DXFEXPOSEOBJECT->SetAlignedProperty();
				Align_Finished();
				PPCALCOBJECT->partProgramReached();
			}
			else
				setClicksDone(getClicksDone() - 1);
			return;
		}

		if(MAINDllOBJECT->getCurrentWindow() != 2 && getClicksDone() == 1)
		{
			if(MAINDllOBJECT->highlightedPoint() == NULL)
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
			points[0].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY(), MAINDllOBJECT->highlightedPoint()->getZ());
			MAINDllOBJECT->SetStatusCode("DXFALGN0005");
		}
		else if(MAINDllOBJECT->getCurrentWindow() != 2 && getClicksDone() == 2)
		{
			AlignMentCompleted = true;
			if(MAINDllOBJECT->highlightedPoint() != NULL)
				points[1].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY(), MAINDllOBJECT->highlightedPoint()->getZ());
			else
				points[1].set(getClicksValue(1).getX(), getClicksValue(1).getY(), MAINDllOBJECT->getCurrentDRO().getZ());
			VideoFlag = true;		
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
			Vector Shift;
			Shift.set(points[1].getX() - points[0].getX(), points[1].getY() - points[0].getY(), points[1].getZ() - points[0].getZ());
			DXFEXPOSEOBJECT->SetTransVector(Shift, false);
			DXFEXPOSEOBJECT->SetAlignedProperty();
			setBaseRProperty(points[1].getX(), points[1].getY(), points[1].getZ());
			EditCad1PtAction::SetCad1PtAlign(points[1], baseaction, MAINDllOBJECT->getCurrentUCS().getId());
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PTMH0003", __FILE__, __FUNCSIG__); }
}

//Handle escape button press.. Clear the current selection..
void EditCad1PtHandler::Align_Finished()
{
	try
	{
		VideoFlag = false; AlignMentCompleted = false;
		resetClicks();
		MAINDllOBJECT->SetStatusCode("DXFALGN0001");
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PTMH0004", __FILE__, __FUNCSIG__); }
}

//draw selected point on dxf window..
void EditCad1PtHandler::drawSelectedShapesOndxf()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PTMH0005", __FILE__, __FUNCSIG__); }
}

void EditCad1PtHandler::drawSelectedShapeOnRCad()
{
	try
	{
		GRAFIX->SetColor_Double(0, 1, 0);
		if(VideoFlag)
			GRAFIX->drawPoint(points[0].getX(), points[0].getY(), 0, 8);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PTMH0005", __FILE__, __FUNCSIG__); }
}

//draw selected point on video..
void EditCad1PtHandler::drawSelectedShapesOnvideo()
{
	try
	{
		GRAFIX->SetColor_Double(0, 1, 0);
		if(VideoFlag)
			GRAFIX->drawPoint(points[1].getX(), points[1].getY(), 0, 8);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PTMH0006", __FILE__, __FUNCSIG__); }
}

 void EditCad1PtHandler::Align_mouseMove(double x, double y)
{
	 try
	 {
		 if(AlignMentCompleted || PPCALCOBJECT->IsPartProgramRunning()) return;
		 if(MAINDllOBJECT->getCurrentWindow() != 2 && getClicksDone() == 1)
		 { 
			points[1].set(x, y);
			double pt1[3] = {points[0].getX(), points[0].getY(), points[0].getZ()}, pt2[3] = {points[1].getX(), points[1].getY(), points[1].getZ()};
			points[0].set(x, y, pt1[2]);
			DXFEXPOSEOBJECT->SetTransVector(pt1, pt2, false);
		 }
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ECAD1PTMH0007", __FILE__, __FUNCSIG__); }
}

 void EditCad1PtHandler::HandlePartProgramData()
{
	try
	{
		baseaction = PPCALCOBJECT->getFrameGrab_Edit1PtAlign();
		if(baseaction->CurrentWindowNo == 1)
		{
			if(baseaction->Pointer_SnapPt != NULL)
			{
				VideoFlag = true;
				points[0] = *((EditCad1PtAction*)PPCALCOBJECT->getCurrentAction())->getPoint1();
				points[1].set(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
				Vector Shift;
				Shift.set(points[1].getX() - points[0].getX(), points[1].getY() - points[0].getY(), points[1].getZ() - points[0].getZ());
				DXFEXPOSEOBJECT->SetTransVector(Shift, false);
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
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PTMH0010", __FILE__, __FUNCSIG__); }
}

 
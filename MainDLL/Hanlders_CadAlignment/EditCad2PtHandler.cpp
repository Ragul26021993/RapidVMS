#include "stdafx.h"
#include "EditCad2PtHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\DXF\DXFExpose.h"
#include "..\Actions\EditCad2PtAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor...
EditCad2PtHandler::EditCad2PtHandler()
{
	try
	{
		this->AlignmentModeFlag = true;
		VideoCnt = 0; RcadCnt = 0; runningPartprogramValid = false; AlignMentCompleted = false;
		setMaxClicks(5);
		MAINDllOBJECT->SetStatusCode("DXFALGN0002");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP2PTALIGN01", __FILE__, __FUNCSIG__); }
}

//Destructor...//
EditCad2PtHandler::~EditCad2PtHandler()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP2PTALIGN02", __FILE__, __FUNCSIG__); }
}

void EditCad2PtHandler::SelectPoints_HeightGauge(double x, double y, double z)
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				int ActionCnt = ((EditCad2PtAction*)PPCALCOBJECT->getCurrentAction())->getActionCount();
				if(ActionCnt == 1)
				{
					points[0] = *((EditCad2PtAction*)PPCALCOBJECT->getCurrentAction())->getPoint1();
					points[2].set(x, y, z);
					PPCALCOBJECT->partProgramReached();
				}
				else if(ActionCnt == 2)
				{
					points[1] = *((EditCad2PtAction*)PPCALCOBJECT->getCurrentAction())->getPoint2();
					points[3].set(x, y, z);
					Vector Shift;
					Shift.set(points[2].getX() - points[0].getX(), points[2].getY() - points[0].getY(), points[2].getZ() - points[0].getZ());
					DXFEXPOSEOBJECT->SetTransVector(Shift, false);
					double Angle1 = RMATH2DOBJECT->ray_angle(points[0].getX(), points[0].getY(), points[1].getX(), points[1].getY());
					double Angle2 = RMATH2DOBJECT->ray_angle(points[2].getX(), points[2].getY(), points[3].getX(), points[3].getY());
					double AngDiff = Angle2 - Angle1;
					double pt[3] = {points[2].getX(), points[2].getY(), points[2].getZ()};
					double transform[9] = {0};
					RMATH2DOBJECT->TransformM_RotateAboutPoint(&pt[0], AngDiff, &transform[0]);
					Vector TransPt;
					TransPt.set(transform[2], transform[5]);
					transform[2] = 0; transform[5] = 0;
					DXFEXPOSEOBJECT->TransformMatrix(&transform[0], false);
					DXFEXPOSEOBJECT->SetTransVector(TransPt, false);
					Align_Finished();
					PPCALCOBJECT->partProgramReached();
					return;
				}
				else return;
			}
		}
		if(RcadCnt == 2 && VideoCnt == 0)
		{
			points[VideoCnt + 2].set(x, y, z);
			VideoCnt++;
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
			Vector TransPt;
			TransPt.set(points[2].getX() - points[0].getX(), points[2].getY() - points[0].getY(), points[2].getZ() - points[0].getZ());
			DXFEXPOSEOBJECT->SetTransVector(TransPt, false);
			points[0].set(points[2].getX(), points[2].getY(), points[2].getZ());
			points[1] += TransPt;
			setBaseRProperty(points[2].getX(), points[2].getY(), points[2].getZ());
			baseaction->CurrentWindowNo = 0;
			baseaction->DroMovementFlag = false;
			EditCad2PtAction::SetCad1PtAlign(points[2], baseaction, true);
			MAINDllOBJECT->SetStatusCode("DXF008");	
		}
		else if(RcadCnt == 2 && VideoCnt == 1)
		{
			points[VideoCnt + 2].set(x, y, z);
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
			double Angle1 = RMATH2DOBJECT->ray_angle(points[0].getX(), points[0].getY(), points[1].getX(), points[1].getY());
			double Angle2 = RMATH2DOBJECT->ray_angle(points[2].getX(), points[2].getY(), points[3].getX(), points[3].getY());
			double AngDiff = Angle2 - Angle1;
			double pt[3] = {points[2].getX(), points[2].getY(), points[2].getZ()};
			double transform[9] = {0};
			RMATH2DOBJECT->TransformM_RotateAboutPoint(&pt[0], AngDiff, &transform[0]);
			Vector TransPt;
			TransPt.set(transform[2], transform[5]);
			transform[2] = 0; transform[5] = 0;
			DXFEXPOSEOBJECT->TransformMatrix(&transform[0], false);
			DXFEXPOSEOBJECT->SetTransVector(TransPt, false);
			setBaseRProperty(points[3].getX(), points[3].getY(), points[3].getZ());
			baseaction->CurrentWindowNo = 0;
			baseaction->DroMovementFlag = false;
			EditCad2PtAction::SetCad1PtAlign(points[3], baseaction, false);
			MAINDllOBJECT->SetStatusCode("DXFALGN0015");
			resetClicks();
			DXFEXPOSEOBJECT->AlginTheDXF(true);
			return;	
		}		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP2PTALIGN02a", __FILE__, __FUNCSIG__); }
}

//Left mousedown.. select point and calculates the transforamtion matrix..//
void EditCad2PtHandler::SelectPoints_Shapes()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid && (MAINDllOBJECT->getCurrentWindow() == 0))
			{
				int ActionCnt = ((EditCad2PtAction*)PPCALCOBJECT->getCurrentAction())->getActionCount();
				if(ActionCnt == 1)
				{
					Vector* pt = &getClicksValue(0);
					points[0] = *((EditCad2PtAction*)PPCALCOBJECT->getCurrentAction())->getPoint1();
					points[2].set(pt->getX(), pt->getY(), MAINDllOBJECT->getCurrentDRO().getZ());
					setClicksDone(3);
					PPCALCOBJECT->partProgramReached();
				}
				else if(ActionCnt == 2)
				{
					Vector* pnt = &getClicksValue(3);
					points[1] = *((EditCad2PtAction*)PPCALCOBJECT->getCurrentAction())->getPoint2();
					points[3].set(pnt->getX(), pnt->getY(), MAINDllOBJECT->getCurrentDRO().getZ());
					Vector Shift;
					Shift.set(points[2].getX() - points[0].getX(), points[2].getY() - points[0].getY(), points[2].getZ() - points[0].getZ());
					DXFEXPOSEOBJECT->SetTransVector(Shift, false);
					double Angle1 = RMATH2DOBJECT->ray_angle(points[0].getX(), points[0].getY(), points[1].getX(), points[1].getY());
					double Angle2 = RMATH2DOBJECT->ray_angle(points[2].getX(), points[2].getY(), points[3].getX(), points[3].getY());
					double AngDiff = Angle2 - Angle1;
					double pt[3] = {points[2].getX(), points[2].getY(), points[2].getZ()};
					double transform[9] = {0};
					RMATH2DOBJECT->TransformM_RotateAboutPoint(&pt[0], AngDiff, &transform[0]);
					Vector TransPt;
					TransPt.set(transform[2], transform[5]);
					transform[2] = 0; transform[5] = 0;
					DXFEXPOSEOBJECT->TransformMatrix(&transform[0], false);
					DXFEXPOSEOBJECT->SetTransVector(TransPt, false);
					//DXFEXPOSEOBJECT->SetAlignedProperty();
					Align_Finished();
					PPCALCOBJECT->partProgramReached();
					return;
				}
			}
			else
				setClicksDone(getClicksDone() - 1);
			return;
		}

		if(MAINDllOBJECT->getCurrentWindow() != 2 && getClicksDone() < 3)
		{
			if(RcadCnt != 2 && MAINDllOBJECT->highlightedPoint() != NULL)
				points[RcadCnt++].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY(), MAINDllOBJECT->highlightedPoint()->getZ());
			else
			{
				setClicksDone(getClicksDone() - 1);
				return;
			}
			if(RcadCnt == 1)
				MAINDllOBJECT->SetStatusCode("DXFALGN0006");
			else
				MAINDllOBJECT->SetStatusCode("DXFALGN0007");

		}
		else if(getClicksDone() < 4 && MAINDllOBJECT->getCurrentWindow() != 2 && RcadCnt == 2)
		{
			if(MAINDllOBJECT->highlightedPoint() != NULL)
				points[VideoCnt + 2].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY(), MAINDllOBJECT->highlightedPoint()->getZ());
			else
				points[VideoCnt + 2].set(getClicksValue(getClicksDone() - 1).getX(), getClicksValue(getClicksDone() - 1).getY(), MAINDllOBJECT->getCurrentDRO().getZ());
			VideoCnt++;
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
			Vector TransPt;
			TransPt.set(points[2].getX() - points[0].getX(), points[2].getY() - points[0].getY(), points[2].getZ() - points[0].getZ());
			DXFEXPOSEOBJECT->SetTransVector(TransPt, false);
			points[0].set(points[2].getX(), points[2].getY(), points[2].getZ());
			points[1] += TransPt;
			setBaseRProperty(points[2].getX(), points[2].getY(), points[2].getZ());
			baseaction->DroMovementFlag = false;
			EditCad2PtAction::SetCad1PtAlign(points[2], baseaction, true);
			MAINDllOBJECT->SetStatusCode("DXF008");	
		}
		else if(getClicksDone() > 3 && MAINDllOBJECT->getCurrentWindow() != 2)
			{
			if(MAINDllOBJECT->highlightedPoint() != NULL)
				points[VideoCnt + 2].set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY(), MAINDllOBJECT->highlightedPoint()->getZ());
			else
				points[VideoCnt + 2].set(getClicksValue(getClicksDone() - 1).getX(), getClicksValue(getClicksDone() - 1).getY(),  MAINDllOBJECT->getCurrentDRO().getZ());
			if(getClicksDone() == 4)
			{
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
				setBaseRProperty(points[3].getX(), points[3].getY(), points[3].getZ());
				baseaction->DroMovementFlag = false;
				EditCad2PtAction::SetCad1PtAlign(points[3], baseaction, false);
				//DXFEXPOSEOBJECT->SetAlignedProperty();
				MAINDllOBJECT->SetStatusCode("DXFALGN0015");
				resetClicks();
				DXFEXPOSEOBJECT->AlginTheDXF(true);
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP2PTALIGN03", __FILE__, __FUNCSIG__); }
}

//Handle escape button press.. Clear the current selection..
void EditCad2PtHandler::Align_Finished()
{
	try
	{
		VideoCnt = 0; RcadCnt = 0;
		resetClicks();
		MAINDllOBJECT->SetStatusCode("DXFALGN0001");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP2PTALIGN04", __FILE__, __FUNCSIG__); }
}

//draw selected point on dxf window..
void EditCad2PtHandler::drawSelectedShapesOndxf()
{}	

void EditCad2PtHandler::drawSelectedShapeOnRCad()
{
	try
	{		
		GRAFIX->SetColor_Double(0, 1, 0);
		if(RcadCnt >= 1)
			GRAFIX->drawPoint(points[0].getX(), points[0].getY(), 0, 8);
		if(RcadCnt == 2)
			GRAFIX->drawPoint(points[1].getX(), points[1].getY(), 0, 8);		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP2PTALIGN05", __FILE__, __FUNCSIG__); }
}

//draw selected point on video..
void EditCad2PtHandler::drawSelectedShapesOnvideo()
{
	try
	{
		GRAFIX->SetColor_Double(0, 1, 0);
		if(VideoCnt >= 1)
			GRAFIX->drawPoint(points[2].getX(), points[2].getY(), 0, 8);
		/*if(VideoCnt == 2)
			GRAFIX->drawPoint(points[3].getX(), points[3].getY(), 0, 8);*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP2PTALIGN06", __FILE__, __FUNCSIG__); }
}

void EditCad2PtHandler::Align_mouseMove(double x, double y)
{
	 try
	 {			
		 if(AlignMentCompleted || PPCALCOBJECT->IsPartProgramRunning()) return;
		 if((MAINDllOBJECT->getCurrentWindow() == 0 || MAINDllOBJECT->getCurrentWindow() == 1) && getClicksDone() == 3)
		 { 
			 points[3].set(x, y);
			 double Angle1 = RMATH2DOBJECT->ray_angle(points[0].getX(), points[0].getY(), points[1].getX(), points[1].getY());
			 double Angle2 = RMATH2DOBJECT->ray_angle(points[2].getX(), points[2].getY(), points[3].getX(), points[3].getY());
			 double AngDiff = Angle2 - Angle1;
			 double pt[3] = {points[2].getX(), points[2].getY(), points[2].getZ()};
			 double transform[9] = {0};
			 RMATH2DOBJECT->TransformM_RotateAboutPoint(&pt[0], AngDiff, &transform[0]);
			 Vector TransPt;
			 TransPt.set(transform[2], transform[5]);
			 transform[2] = 0; transform[5] = 0;
			 DXFEXPOSEOBJECT->TransformMatrix(&transform[0], false);
			 DXFEXPOSEOBJECT->SetTransVector(TransPt, false);
			 points[1].set(x, y);
		}		
	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PP2PTALIGN07", __FILE__, __FUNCSIG__); }
}

 void EditCad2PtHandler::HandlePartProgramData()
{
	try
	{
		baseaction = PPCALCOBJECT->getFrameGrab_Edit2PtAlign();
		if(baseaction->CurrentWindowNo == 1)
		{
			if(baseaction->Pointer_SnapPt != NULL)
			{
				int ActionCnt = ((EditCad2PtAction*)PPCALCOBJECT->getCurrentAction())->getActionCount();
				if(ActionCnt == 1)
				{
					points[0] = *((EditCad2PtAction*)PPCALCOBJECT->getCurrentAction())->getPoint1();
					points[2].set(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
					setClicksDone(3);
					if(baseaction->DroMovementFlag)
						PPCALCOBJECT->partProgramReached();
					return;
				}
				else if(ActionCnt == 2)
				{
					points[1] = *((EditCad2PtAction*)PPCALCOBJECT->getCurrentAction())->getPoint2();
					points[3].set(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
					Vector Shift;
					Shift.set(points[2].getX() - points[0].getX(), points[2].getY() - points[0].getY(), points[2].getZ() - points[0].getZ());
					DXFEXPOSEOBJECT->SetTransVector(Shift, false);
					double Angle1 = RMATH2DOBJECT->ray_angle(points[0].getX(), points[0].getY(), points[1].getX(), points[1].getY());
					double Angle2 = RMATH2DOBJECT->ray_angle(points[2].getX(), points[2].getY(), points[3].getX(), points[3].getY());
					double AngDiff = Angle2 - Angle1;
					double pt[3] = {points[2].getX(), points[2].getY(), points[2].getZ()};
					double transform[9] = {0};
					RMATH2DOBJECT->TransformM_RotateAboutPoint(&pt[0], AngDiff, &transform[0]);
					Vector TransPt;
					TransPt.set(transform[2], transform[5]);
					transform[2] = 0; transform[5] = 0;
					DXFEXPOSEOBJECT->TransformMatrix(&transform[0], false);
					DXFEXPOSEOBJECT->SetTransVector(TransPt, false);
					Align_Finished();
					if(baseaction->DroMovementFlag)
						PPCALCOBJECT->partProgramReached();
					return;
				}
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
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1PTMH0010", __FILE__, __FUNCSIG__); }
}
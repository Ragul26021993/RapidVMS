#include "StdAfx.h"
#include "SplineHandler.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\Spline.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

SplineHandler::SplineHandler()
{
	try
	{
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SplnH001", __FILE__, __FUNCSIG__); }
}

SplineHandler::SplineHandler(ShapeWithList* sh, bool undoFlag)
{
	try
	{
		MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::SPLINE_HANDLER;
		myshape = sh;
		if(undoFlag)
		{
			UndoFlag = true; RedoFlag = false;
		}
		else
		{
			UndoFlag = false; RedoFlag = true;
		}
		setClicksDone(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SplnH002", __FILE__, __FUNCSIG__); }
}

SplineHandler::~SplineHandler()
{
}

void SplineHandler::init()
{
	try
	{
		MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::SPLINE_HANDLER;
		myshape = NULL;
		runningPartprogramValid = false;
		firstClick = true; MouseMoveFlag = false;
		LastPt[0] = 0; LastPt[1] = 0; LastPt[2] = 0;
		NumberOfPts = 0;
		Pflag = false;
		UndoFlag = false;
		RedoFlag = false;
		setClicksDone(0);
		setMaxClicks(3);
		if(!PPCALCOBJECT->IsPartProgramRunning())
		{
			myshape = new Spline(_T("S"));
			//((Spline*)myshape)->DrawSpline = false;
			CurrentShape = myshape;
			AddShapeAction::addShape(myshape);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SplnH003", __FILE__, __FUNCSIG__); }
}

void SplineHandler::LmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone() - 1);
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(NumberOfPts == myshape->PointsList->Pointscount() + 1 && !firstClick)
			{
				if(Pflag)
				{
					PC_ITER SptItem = myshape->PointsList->getList().begin();
					SinglePoint* Spt = (*SptItem).second;
					pt->set(Spt->X, Spt->Y, Spt->Z);
					((Spline*)myshape)->ClosedSpline = Pflag;
				}
				MouseMoveFlag = false;
				((Spline*)myshape)->DrawSpline = true;
			}
			if(runningPartprogramValid)
			{
				if(firstClick)
				{
					Pflag = ((Spline*)myshape)->ClosedSpline;
					NumberOfPts = ((Spline*)myshape)->ControlPointsList->Pointscount();
					if(Pflag)
						NumberOfPts -= 2;
					((Spline*)myshape)->DrawSpline = false;
					((Spline*)myshape)->ClosedSpline = false;
					MouseMoveFlag = true;
					firstClick = false;
				}
				runningPartprogramValid = false;
				if(getClicksDone() == 2)
					setClicksDone(getClicksDone() - 1);
				setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
				myshape->ManagePoint(baseaction, myshape->ADD_POINT);
				UpdateControlPointList();
				if(!PPCALCOBJECT->ProgramAutoIncrementMode())
					PPCALCOBJECT->partProgramReached();
			}
			else
				setClicksDone(getClicksDone() - 1);
			return;
		}
		if(getClicksDone() == 1 && firstClick)
		{
			MouseMoveFlag = true;
			firstClick = false;
			((Spline*)myshape)->framgrabShape = false;
			((Spline*)myshape)->DrawSpline = false;
			/*myshape = new Spline(_T("S"));
			((Spline*)myshape)->DrawSpline = false;
			CurrentShape = myshape;
			AddShapeAction::addShape(myshape);*/
		}
		if(getClicksDone() == 2)
			setClicksDone(getClicksDone() - 1);
		if(myshape != NULL)
		{
			setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
			AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);
			UpdateControlPointList();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SplnH004", __FILE__, __FUNCSIG__); }
}

void SplineHandler::MmouseDown(double x, double y)
{
	try
	{
		if(!PPCALCOBJECT->IsPartProgramRunning())
		{
			if(myshape->PointsList->Pointscount() > 2)
			{
				((Spline*)myshape)->DrawSpline = true;	
				MouseMoveFlag = false;
				setBaseRProperty(LastPt[0], LastPt[1], LastPt[2]);
				AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);
				UpdateControlPointList();
				MAINDllOBJECT->SetStatusCode("Finished");
				init();
				MAINDllOBJECT->UpdateShapesChanged();
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SplnH005", __FILE__, __FUNCSIG__); }
}

void SplineHandler::RmouseDown(double x, double y)
{
	try
	{
		if(!PPCALCOBJECT->IsPartProgramRunning())
		{
			if(myshape->PointsList->Pointscount() > 2)
			{
				((Spline*)myshape)->DrawSpline = true;	
				MouseMoveFlag = false;
				PC_ITER SptItem = myshape->PointsList->getList().begin();
				SinglePoint* Spt = (*SptItem).second;
				((Spline*)myshape)->ClosedSpline = true;
				setBaseRProperty(Spt->X, Spt->Y, Spt->Z);
				AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);
				UpdateControlPointList();
				MAINDllOBJECT->SetStatusCode("Finished");
				init();
				MAINDllOBJECT->UpdateShapesChanged();
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SplnH006", __FILE__, __FUNCSIG__); }
}

void SplineHandler::PartProgramData()
{
	try
	{
		myshape = (ShapeWithList*)(MAINDllOBJECT->getShapesList().selectedItem());
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SplnH007", __FILE__, __FUNCSIG__); }
}	

void SplineHandler::mouseMove()
{
	try
	{
		if(UndoFlag || RedoFlag)
		{
			UndoFlag = false; RedoFlag = false;
			UpdateControlPointList();
		}
		Vector* pt = &getClicksValue(getClicksDone());
		if(MAINDllOBJECT->getCurrentWindow() == 0)
			MAINDllOBJECT->ToolSelectedMousemove(pt->getX(), pt->getY(), pt->getZ());
		if(getClicksDone() == 1)
		{
			LastPt[0] = pt->getX(); LastPt[1] = pt->getY(); LastPt[2] = pt->getZ();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SplnH008", __FILE__, __FUNCSIG__); }
}		

void SplineHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(myshape == NULL) return;
		int ptsCnt = ((Spline*)myshape)->ControlPointsList->Pointscount();
		if(((Spline*)myshape)->ControlPointsList->Pointscount() >= 2)
		{	
			if(MouseMoveFlag)
			{
				double* new_ptsArray = new double[(ptsCnt + 1) * 3];
				for(int i = 0; i < ptsCnt * 3; i++)
					new_ptsArray[i] = myshape->pts[i];
				new_ptsArray[ptsCnt * 3] = LastPt[0]; new_ptsArray[ptsCnt * 3 + 1] = LastPt[1]; new_ptsArray[ptsCnt * 3 + 2] = LastPt[2];
				int nknots = ptsCnt + 5;
				double* new_knotsArray = new double[nknots];
				if(nknots >= 8)
				{
					for(int i = 4; i < nknots - 4; i++)
						new_knotsArray[i] = i - 3;
					new_knotsArray[0] = 0; new_knotsArray[1] = 0; new_knotsArray[2] = 0; new_knotsArray[3] = 0;
					new_knotsArray[nknots - 4] = ptsCnt - 2; new_knotsArray[nknots - 3] = ptsCnt - 2; new_knotsArray[nknots - 2] = ptsCnt - 2; new_knotsArray[nknots - 1] = ptsCnt - 2;
				}
				else
				{
					for(int i = 0; i < nknots; i++)
						new_knotsArray[i] = i;
				}
				GRAFIX->drawSpline(new_ptsArray, ptsCnt + 1, new_knotsArray);
				delete [] new_ptsArray;
				delete [] new_knotsArray;
			}
			else
				GRAFIX->drawSpline(((Spline*)myshape)->CtrPtsArray, ptsCnt, ((Spline*)myshape)->KnotsArray);
		}
		else if(myshape->PointsList->Pointscount() >= 1)
			GRAFIX->drawLine_3D(getClicksValue(0).getX(), getClicksValue(0).getY(), MAINDllOBJECT->getCurrentDRO().getZ() ,LastPt[0], LastPt[1], LastPt[2]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SplnH009", __FILE__, __FUNCSIG__); }
}

void SplineHandler::LmaxmouseDown()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SplnH0010", __FILE__, __FUNCSIG__); }
}

void SplineHandler::EscapebuttonPress()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SplnH0011", __FILE__, __FUNCSIG__); }
}

void SplineHandler::UpdateControlPointList()
{
	try
	{
		((Spline*)myshape)->ControlPointsList->deleteAll();
		if(((Spline*)myshape)->ClosedSpline)
		{
			SinglePoint* FirstPt = new SinglePoint();
			SinglePoint* Spt2 = myshape->PointsList->getList()[myshape->PointsList->Pointscount() - 2];
			FirstPt->SetValues(Spt2->X, Spt2->Y, Spt2->Z);
			((Spline*)myshape)->ControlPointsList->Addpoint(FirstPt);
			for(PC_ITER SptItem = myshape->PointsList->getList().begin(); SptItem != myshape->PointsList->getList().end(); SptItem++)
			{
				SinglePoint* Spt = (*SptItem).second;
				SinglePoint* Spt1 = new SinglePoint();
				Spt1->SetValues(Spt->X, Spt->Y, Spt->Z);
				((Spline*)myshape)->ControlPointsList->Addpoint(Spt1);
			}
			SinglePoint* LastPt = new SinglePoint();
			Spt2 = myshape->PointsList->getList()[1];
			LastPt->SetValues(Spt2->X, Spt2->Y, Spt2->Z);
			((Spline*)myshape)->ControlPointsList->Addpoint(LastPt);
		}
		else
		{
			for(PC_ITER SptItem = myshape->PointsList->getList().begin(); SptItem != myshape->PointsList->getList().end(); SptItem++)
			{
				SinglePoint* Spt = (*SptItem).second;
				SinglePoint* Spt1 = new SinglePoint();
				Spt1->SetValues(Spt->X, Spt->Y, Spt->Z);
				((Spline*)myshape)->ControlPointsList->Addpoint(Spt1);
			}
		}
		int nctPts = ((Spline*)myshape)->ControlPointsList->Pointscount(), nknots = nctPts + 4;
		if(((Spline*)myshape)->CtrPtsArray != NULL)
		{
			delete [] ((Spline*)myshape)->CtrPtsArray; 
			((Spline*)myshape)->CtrPtsArray = NULL;
		}
		((Spline*)myshape)->CtrPtsArray = new double[nctPts * 3];
		int n = 0;
		for(PC_ITER Spt = ((Spline*)myshape)->ControlPointsList->getList().begin(); Spt != ((Spline*)myshape)->ControlPointsList->getList().end(); Spt++)
		{
			SinglePoint* Pt = (*Spt).second;
			((Spline*)myshape)->CtrPtsArray[n++] = Pt->X;
			((Spline*)myshape)->CtrPtsArray[n++] = Pt->Y;
			((Spline*)myshape)->CtrPtsArray[n++] = Pt->Z;
		}
		if(((Spline*)myshape)->KnotsArray != NULL)
		{
			delete [] ((Spline*)myshape)->KnotsArray;
			((Spline*)myshape)->KnotsArray = NULL;
		}	 
		((Spline*)myshape)->KnotsArray = new double[nknots];
		if(((Spline*)myshape)->ClosedSpline)
		{
			for(int i = 0; i < nknots; i++)
				((Spline*)myshape)->KnotsArray[i] = i;
		}
		else
		{
			if(nknots > 8)
			{
				for(int i = 4; i < nknots - 4; i++)
					((Spline*)myshape)->KnotsArray[i] = i - 3;
			}
			((Spline*)myshape)->KnotsArray[0] = 0; ((Spline*)myshape)->KnotsArray[1] = 0; ((Spline*)myshape)->KnotsArray[2] = 0; ((Spline*)myshape)->KnotsArray[3] = 0; ((Spline*)myshape)->KnotsArray[nknots - 4] = nctPts - 3;
			((Spline*)myshape)->KnotsArray[nknots - 3] = nctPts - 3; ((Spline*)myshape)->KnotsArray[nknots - 2] = nctPts - 3; ((Spline*)myshape)->KnotsArray[nknots - 1] = nctPts - 3;
		}
		myshape->UpdateBestFit();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SplnH0012", __FILE__, __FUNCSIG__); }
}
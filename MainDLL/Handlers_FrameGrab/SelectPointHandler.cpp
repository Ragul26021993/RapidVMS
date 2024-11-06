#include "StdAfx.h"
#include "SelectPointHandler.h"
#include "..\Actions\AddPointAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"
#include <process.h>

//Default constuctor..
SelectPointHandler::SelectPointHandler()
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPMH0001", __FILE__, __FUNCSIG__); }
}

//Constructor with point action as parameter... Used in partprogram..
SelectPointHandler::SelectPointHandler(FramegrabBase* fb)
{
	try
	{
		init();
		baseaction = fb;
		ProbeisLoaded = false;
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPMH0002", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
SelectPointHandler::~SelectPointHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPMH0003", __FILE__, __FUNCSIG__); }
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void SelectPointHandler::init()
{
	try
	{
		setMaxClicks(1);
		runningPartprogramValid = false;
		MAINDllOBJECT->SetStatusCode("PTHDLR0001");
		MAINDllOBJECT->CURRENTRAPIDHANDLER = RapidEnums::RAPIDDRAWHANDLERTYPE::RAPIDFRAMEGRAB_HANDLER;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPMH0004", __FILE__, __FUNCSIG__); }
}

//Function to handle the mouse move..
void SelectPointHandler::mouseMove()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPMH0005", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void SelectPointHandler::LmouseDown()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPMH0006", __FILE__, __FUNCSIG__); }
}

//Escape buttons press handling..
void SelectPointHandler::EscapebuttonPress()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPMH0007", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void SelectPointHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPMH0008", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data..//
void SelectPointHandler::PartProgramData()
{
	try
	{
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
		if(MAINDllOBJECT->IsCNCMode())
		{
			AddPointsToSelectedShape(true);
			PPCALCOBJECT->partProgramReached();
			return;
		}
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPMH0009", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void SelectPointHandler::LmaxmouseDown()
{
	try
	{
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				AddPointsToSelectedShape(true);
				PPCALCOBJECT->partProgramReached();
				return;
			}
		}
		AddPointsToSelectedShape(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPMH0010", __FILE__, __FUNCSIG__); }
}

void SelectPointHandler::AddPointsToSelectedShape(bool runningpp)
{
	try
	{
		ShapeWithList* CShape = (ShapeWithList*)(MAINDllOBJECT->getShapesList().selectedItem());	

		if(CShape == NULL)
		{
			MAINDllOBJECT->SetStatusCode("PTHDLR0003");
			return;
		}
		Vector Cdro = MAINDllOBJECT->getCurrentUCS().GetCurrentUCS_DROpostn();
		if (runningpp)
			baseaction = PPCALCOBJECT->getFrameGrab();
		else
			baseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::SELECTPOINTCROSSHAIR);
		
		baseaction->points[0].set(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ());
		baseaction->PointDRO.set(MAINDllOBJECT->getCurrentDRO());
		if(MAINDllOBJECT->CameraConnected())
		{
			baseaction->camProperty = MAINDllOBJECT->getCamSettings();
			baseaction->lightProperty = MAINDllOBJECT->getLightProperty();
			baseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
		}
		baseaction->ProfileON = MAINDllOBJECT->getProfile();
		baseaction->SurfaceON = MAINDllOBJECT->getSurface();
		if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
			baseaction->AllPointsList.Addpoint(new SinglePoint(Cdro.getX(), Cdro.getY(), Cdro.getZ()));
		else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
			baseaction->AllPointsList.Addpoint(new SinglePoint(Cdro.getY(), Cdro.getZ(), Cdro.getX()));
		else if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
			baseaction->AllPointsList.Addpoint(new SinglePoint(Cdro.getX(), Cdro.getZ(), Cdro.getY()));
		else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
			baseaction->AllPointsList.Addpoint(new SinglePoint(Cdro.getY(), -Cdro.getZ(), Cdro.getX()));
		else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
			baseaction->AllPointsList.Addpoint(new SinglePoint(Cdro.getX(), -Cdro.getZ(), Cdro.getY()));
		if (runningpp)
			CShape->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
		//else if (AddScanPointswithoutAction)
		//	CShape->AddPoints(&baseaction->AllPointsList);
		else
			AddPointAction::pointAction(CShape, baseaction);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPMH0011", __FILE__, __FUNCSIG__); }
}

void SelectPointHandler::StartContinuousScanning(bool ScanStart)
{
	StartScanning = ScanStart;
	try
	{
		if (StartScanning)
		{
			//Begin Scanning Thread.................
			_beginthread(&ScaninSeparateThread, 0, this);
		}
		else
		{

		}
	}
	catch (...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("SPMH0012", __FILE__, __FUNCSIG__);
	}
}

void SelectPointHandler::AddScanPointstoShape()
{
	if (!ProbeisLoaded) return;
	try
	{
		ShapeWithList* CShape = (ShapeWithList*)(MAINDllOBJECT->getShapesList().selectedItem());

		if (CShape == NULL)
		{
			MAINDllOBJECT->SetStatusCode("PTHDLR0003");
			return;
		}
		Vector Cdro = MAINDllOBJECT->getCurrentUCS().GetCurrentUCS_DROpostn();
		baseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::SELECTPOINTCROSSHAIR);

		baseaction->points[0].set(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ());
		baseaction->PointDRO.set(MAINDllOBJECT->getCurrentDRO());
		if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
			baseaction->AllPointsList.Addpoint(new SinglePoint(Cdro.getX(), Cdro.getY(), Cdro.getZ()));
		else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
			baseaction->AllPointsList.Addpoint(new SinglePoint(Cdro.getY(), Cdro.getZ(), Cdro.getX()));
		else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
			baseaction->AllPointsList.Addpoint(new SinglePoint(Cdro.getX(), Cdro.getZ(), Cdro.getY()));
		else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
			baseaction->AllPointsList.Addpoint(new SinglePoint(Cdro.getY(), -Cdro.getZ(), Cdro.getX()));
		else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
			baseaction->AllPointsList.Addpoint(new SinglePoint(Cdro.getX(), -Cdro.getZ(), Cdro.getY()));
		
		if (CShape->PointsList->Pointscount() > 0 && baseaction->AllPointsList.Pointscount() > 0)
		{
			SinglePoint* Sp; 
			SinglePoint* ThisPt;
			for (PC_ITER Spt = CShape->PointsList->getList().begin(); Spt != CShape->PointsList->getList().end(); Spt++)
				Sp = Spt->second;
			for (PC_ITER Spt = baseaction->AllPointsList.getList().begin(); Spt != baseaction->AllPointsList.getList().end(); Spt++)
				ThisPt = Spt->second;
			//SinglePoint*  = baseaction->AllPointsList.getList().end()->second;
			/*SinglePoint* Sp = CShape->PointsList->getList().end()->second;*/
			if (ThisPt->X != Sp->X || ThisPt->Y != Sp->Y || ThisPt->Z != Sp->Z)
				CShape->AddPoints(&baseaction->AllPointsList);
		}
		else
			CShape->AddPoints(&baseaction->AllPointsList);
	}
	catch (...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("SPMH0013", __FILE__, __FUNCSIG__);
	}
}

void ScaninSeparateThread(void *anything)
{
	try
	{
		SelectPointHandler* ScanningThread = (SelectPointHandler*)anything;
		while (ScanningThread->StartScanning)
		{
			ScanningThread->AddScanPointstoShape();
		}
		//ScanningThread->StopScanning();
		_endthread();
	}
	catch (...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("SPMH0014", __FILE__, __FUNCSIG__);
		_endthread();
	}
}

void SelectPointHandler::StopScanning()
{
	//MAINDllOBJECT->dontUpdateGraphcis = true;
	//MAINDllOBJECT->Wait_VideoGraphicsUpdate();
	//MAINDllOBJECT->Wait_RcadGraphicsUpdate();
	//MAINDllOBJECT->Wait_DxfGraphicsUpdate();

}
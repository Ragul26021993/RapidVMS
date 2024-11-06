#include "StdAfx.h"
#include "FrameGrabIdAndOd.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddThreadPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Helper\Helper.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor..//
FrameGrabIdAndOd::FrameGrabIdAndOd(RapidEnums::RAPIDFGACTIONTYPE Fgtype)
{
	try
	{
		setMaxClicks(1);
		CurrentFGType = Fgtype;
		baseaction = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGIDODMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
FrameGrabIdAndOd::~FrameGrabIdAndOd()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGIDODMH0002", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move......//
void FrameGrabIdAndOd::mouseMove()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGIDODMH0003", __FILE__, __FUNCSIG__); }
}

//Handle the mouse down event......//
void FrameGrabIdAndOd::LmouseDown()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGIDODMH0004", __FILE__, __FUNCSIG__); }
}


//Draw the framegrabs.......//
void FrameGrabIdAndOd::draw(int windowno, double WPixelWidth)
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGIDODMH0005", __FILE__, __FUNCSIG__); }
}

//Esc key press..
void FrameGrabIdAndOd::EscapebuttonPress()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGIDODMH0006", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data....//
void FrameGrabIdAndOd::PartProgramData()
{
	try
	{
		/*if(PPCALCOBJECT->IsPartProgramRunning())
		{
			PPCALCOBJECT->partProgramReached();
			return;
		}*/
		CurrentShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		SetAddPointAction();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGIDODMH0007", __FILE__, __FUNCSIG__); }
}

//Add the framgrab points....//
void FrameGrabIdAndOd::SetAddPointAction()
{
	try
	{	
		if(!PPCALCOBJECT->IsPartProgramRunning())
		{
			CurrentShape = new Perimeter(true);
			((Perimeter*)CurrentShape)->TotalPerimeter(0);
			((Perimeter*)CurrentShape)->Diameter(0);
			AddShapeAction::addShape(CurrentShape);
			baseaction = new FramegrabBase(CurrentFGType);
			baseaction->PointDRO.set(MAINDllOBJECT->getCurrentDRO());
			if(MAINDllOBJECT->CameraConnected())
			{
				baseaction->lightProperty = MAINDllOBJECT->getLightProperty();
				baseaction->camProperty = MAINDllOBJECT->getCamSettings();
				baseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
			}
			baseaction->ProfileON = MAINDllOBJECT->getProfile();
			baseaction->SurfaceON = MAINDllOBJECT->getSurface();
			baseaction->Surfacre_FrameGrabtype = MAINDllOBJECT->SurfaceFgtype();
		}
		bool FrameGrabDone = HELPEROBJECT->DoCurrentFrameGrab_IdOdOrFull(baseaction);
		if(FrameGrabDone)
		{
			if(PPCALCOBJECT->IsPartProgramRunning())
				((ShapeWithList*)CurrentShape)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
			else
				AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
			CurrentShape->UpdateBestFit();
			//HELPEROBJECT->CalculateAreaOfShapeUsingOneShot(CurrentShape);
			if(PPCALCOBJECT->IsPartProgramRunning())
				PPCALCOBJECT->partProgramReached();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FGIDODMH0008", __FILE__, __FUNCSIG__); }
}
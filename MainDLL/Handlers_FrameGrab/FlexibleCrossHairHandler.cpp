#include "StdAfx.h"
#include "FlexibleCrossHairHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Engine\ThreadCalcFunctions.h"
#include "..\Actions\AddThreadPointAction.h"

//Default constuctor..
FlexibleCrossHairHandler::FlexibleCrossHairHandler()
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTMH0001", __FILE__, __FUNCSIG__); }
}

//Constructor with point action as parameter... Used in partprogram..
FlexibleCrossHairHandler::FlexibleCrossHairHandler(FramegrabBase* fb)
{
	try
	{
		init();
		baseaction = fb;
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTMH0002", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
FlexibleCrossHairHandler::~FlexibleCrossHairHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTMH0003", __FILE__, __FUNCSIG__); }
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void FlexibleCrossHairHandler::init()
{
	try
	{
		setMaxClicks(1);
		runningPartprogramValid = false;
		valid = false;
		/*MAINDllOBJECT->SetStatusCode("PTHDLR0001");*/
		MAINDllOBJECT->SetStatusCode("FlexibleCrossHairHandler01");
		MAINDllOBJECT->CURRENTRAPIDHANDLER = RapidEnums::RAPIDDRAWHANDLERTYPE::RAPIDFRAMEGRAB_HANDLER;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTMH0004", __FILE__, __FUNCSIG__); }
}

//Function to handle the mouse move..
void FlexibleCrossHairHandler::mouseMove()
{
	try
	{
		if(MAINDllOBJECT->getCurrentWindow() == 0)
			MAINDllOBJECT->ToolSelectedMousemove(getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY(), getClicksValue(getClicksDone()).getZ());
		valid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTMH0005", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void FlexibleCrossHairHandler::LmouseDown()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTMH0006", __FILE__, __FUNCSIG__); }
}

//Escape buttons press handling..
void FlexibleCrossHairHandler::EscapebuttonPress()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTMH0007", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void FlexibleCrossHairHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(!valid) return;
		if(windowno != 0) return;
		if (MAINDllOBJECT->MouseOnVideo())
		{
			GRAFIX->drawFlexibleCrossHair(getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY(), WPixelWidth / 2);
			//GRAFIX->drawTransparentRect(getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY(), WPixelWidth * 20, WPixelWidth);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTMH0008", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data..//
void FlexibleCrossHairHandler::PartProgramData()
{
	try
	{
		if(THREADCALCOBJECT->ThreadMeasurementMode())
		{
			CurrentShape = (ShapeWithList*)THREADCALCOBJECT->CurrentSelectedThreadAction->CurrentSelectedThreadShape;
		}
		else
		{
			CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
			if(PPCALCOBJECT->getFrameGrab()->CurrentWindowNo == 1)
			{
				baseaction = PPCALCOBJECT->getFrameGrab();
				setBaseRProperty(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
				((ShapeWithList*)CurrentShape)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
				PPCALCOBJECT->partProgramReached();
				return;
			}
		}
		valid = false;
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTMH0009", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void FlexibleCrossHairHandler::LmaxmouseDown()
{
	try
	{
		valid = false;
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
				if(THREADCALCOBJECT->ThreadMeasurementMode())
					CurrentShape = (ShapeWithList*)THREADCALCOBJECT->CurrentSelectedThreadAction->CurrentSelectedThreadShape;
				setBaseRProperty(getClicksValue(0).getX(), getClicksValue(0).getY(), getClicksValue(0).getZ());
				((ShapeWithList*)CurrentShape)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
				if(THREADCALCOBJECT->ThreadMeasurementMode())
					THREADCALCOBJECT->AddPointsToMainCollectionShape((ShapeWithList*)CurrentShape);
				PPCALCOBJECT->partProgramReached();
				return;
			}
		}
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
		if(THREADCALCOBJECT->ThreadMeasurementMode())
			CurrentShape = (ShapeWithList*)THREADCALCOBJECT->CurrentSelectedThreadAction->CurrentSelectedThreadShape;
		if(CurrentShape == NULL)
		{
			MAINDllOBJECT->SetStatusCode("PTHDLR0003");
			return;
		}
		AddPointToSelectedShape();
		MAINDllOBJECT->SetStatusCode("PTHDLR0002");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTMH0010", __FILE__, __FUNCSIG__); }
}

//Add the clicked point to the selected shape..With all relations, conditions etc..
void FlexibleCrossHairHandler::AddPointToSelectedShape()
{
	try
	{
		baseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::FLEXIBLECROSSHAIR);
		baseaction->points[0].set(getClicksValue(0).getX(), getClicksValue(0).getY(), MAINDllOBJECT->getCurrentDRO().getZ());
		baseaction->PointDRO.set(MAINDllOBJECT->getCurrentDRO());
		if(THREADCALCOBJECT->ThreadMeasurementMode())
		{
			if(MAINDllOBJECT->CameraConnected())
			{			
				baseaction->camProperty = MAINDllOBJECT->getCamSettings();
				baseaction->lightProperty = MAINDllOBJECT->getLightProperty();
				baseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
			}
			baseaction->ProfileON = MAINDllOBJECT->getProfile();
			baseaction->SurfaceON = MAINDllOBJECT->getSurface();
			SinglePoint* pt = new SinglePoint(getClicksValue(0).getX(), getClicksValue(0).getY(), getClicksValue(0).getZ());
			baseaction->AllPointsList.Addpoint(pt);
			AddThreadPointAction::ThreadpointAction((ShapeWithList*)CurrentShape, baseaction);
			THREADCALCOBJECT->AddPointsToMainCollectionShape((ShapeWithList*)CurrentShape);
			CurrentShape->FinishedThreadMeasurement(true);
			THREADCALCOBJECT->ThreadMeasurementModeStarted(true);
			MAINDllOBJECT->Shape_Updated();
		}
		else
		{
			if(MAINDllOBJECT->getCurrentWindow() == 1 && MAINDllOBJECT->highlightedPoint() != NULL && !THREADCALCOBJECT->ThreadMeasurementMode())
			{
				baseaction->CurrentWindowNo = 1;
				baseaction->Pointer_SnapPt = MAINDllOBJECT->highlightedPoint();
				baseaction->DerivedShape = true;
				bool PresentFlag1 = false;
				if(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1() != NULL)
				{
					for each(Shape* Csh in CurrentShape->getParents())
					{
						if(Csh->getId() == MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1()->getId())
						{
							PresentFlag1 = true;
							break;
						}
					}
					if(!PresentFlag1) CurrentShape->addParent(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1());
					PresentFlag1 = false;
					for each(Shape* Csh in MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1()->getChild())
					{
						if(Csh->getId() == CurrentShape->getId())
						{
							PresentFlag1 = true;
							break;
						}
					}
					if(!PresentFlag1) MAINDllOBJECT->getCurrentUCS().getIPManager().getParent1()->addChild(CurrentShape);
				}
				
				PresentFlag1 = false;
				if(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2() != NULL)
				{
					for each(Shape* Csh in CurrentShape->getParents())
					{
						if(Csh->getId() == MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2()->getId())
						{
							PresentFlag1 = true;
							break;
						}
					}
					if(!PresentFlag1) CurrentShape->addParent(MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2());
					PresentFlag1 = false;
					for each(Shape* Csh in MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2()->getChild())
					{
						if(Csh->getId() == CurrentShape->getId())
						{
							PresentFlag1 = true;
							break;
						}
					}
					if(!PresentFlag1) MAINDllOBJECT->getCurrentUCS().getIPManager().getParent2()->addChild(CurrentShape);
				}		
			}
			if(MAINDllOBJECT->CameraConnected())
			{			
				baseaction->camProperty = MAINDllOBJECT->getCamSettings();
				baseaction->lightProperty = MAINDllOBJECT->getLightProperty();
				baseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
			}
			baseaction->ProfileON = MAINDllOBJECT->getProfile();
			baseaction->SurfaceON = MAINDllOBJECT->getSurface();
			SinglePoint* pt = new SinglePoint(getClicksValue(0).getX(), getClicksValue(0).getY(), getClicksValue(0).getZ());
			baseaction->AllPointsList.Addpoint(pt);
			AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTMH0011", __FILE__, __FUNCSIG__); }
}
#include "StdAfx.h"
#include "DummyHandler.h"
#include "..\FrameGrab\FramegrabBase.h"
#include "..\Shapes\Plane.h"
#include "..\Shapes\Sphere.h"
#include "..\Shapes\Cylinder.h"
#include "..\Shapes\RPoint.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Conics3D.h"
#include "..\Shapes\CloudPoints.h"
#include "..\Shapes\Cone.h"
#include "..\Shapes\Torus.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Shapes\DotsCount.h"
#include "..\Shapes\CamProfile.h"

//Default constuctor..
DummyHandler::DummyHandler()
{
	try{init();}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DUHANDLER0001", __FILE__, __FUNCSIG__); }
}
DummyHandler::DummyHandler(RapidEnums::SHAPETYPE d)
{
	try
	{
		init();
		ShapeType = d;
		AddNewShape();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0001", __FILE__, __FUNCSIG__); }
}

DummyHandler::DummyHandler(Shape *csh)
{
	try
	{
		init();
		AddNewShape(csh);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0001", __FILE__, __FUNCSIG__); }
}

//Constructor with point action as parameter... Used in partprogram..
DummyHandler::DummyHandler(FramegrabBase* fb)
{
	try
	{
		init();
		baseaction = fb;
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DUHANDLER0002", __FILE__, __FUNCSIG__); }
}

void DummyHandler::AddNewShape()
{
	switch(ShapeType)
		{
			case RapidEnums::SHAPETYPE::RELATIVEPOINT:
			AddShapeAction::addShape(new RPoint(_T("RP"), RapidEnums::SHAPETYPE::RELATIVEPOINT));
			break;
			case RapidEnums::SHAPETYPE::PLANE:
			AddShapeAction::addShape(new Plane());
			break;
			case RapidEnums::SHAPETYPE::CYLINDER:
			AddShapeAction::addShape(new Cylinder());
			break;
			case RapidEnums::SHAPETYPE::CONE:
			AddShapeAction::addShape(new Cone());
			break;
			case RapidEnums::SHAPETYPE::TORUS:
			AddShapeAction::addShape(new Torus());
			break;
			case RapidEnums::SHAPETYPE::CIRCLE3D:
			AddShapeAction::addShape(new Circle(_T("C"), RapidEnums::SHAPETYPE::CIRCLE3D));
			break;
			case RapidEnums::SHAPETYPE::ELLIPSE3D:
			AddShapeAction::addShape(new Conics3D(_T("EL"), RapidEnums::SHAPETYPE::ELLIPSE3D));
			break;
			case RapidEnums::SHAPETYPE::PARABOLA3D:
			AddShapeAction::addShape(new Conics3D(_T("PB"), RapidEnums::SHAPETYPE::PARABOLA3D));
			break;
			case RapidEnums::SHAPETYPE::HYPERBOLA3D:
			AddShapeAction::addShape(new Conics3D(_T("HB"), RapidEnums::SHAPETYPE::HYPERBOLA3D));
			break;
			case RapidEnums::SHAPETYPE::SPHERE:
			AddShapeAction::addShape(new Sphere());
			break;
			case RapidEnums::SHAPETYPE::CLOUDPOINTS:
			AddShapeAction::addShape(new CloudPoints());
			break;						
			case RapidEnums::SHAPETYPE::DOTSCOUNT:
			AddShapeAction::addShape(new DotsCount());
			break;
			case RapidEnums::SHAPETYPE::CAMPROFILE:
			AddShapeAction::addShape(new CamProfile());
			break;
		}
	/*CurrentShape->
	setMaxClicks(getMaxClicks());*/
}

void DummyHandler::AddNewShape(Shape *csh)
{
	switch(csh->ShapeType)
		{
			case RapidEnums::SHAPETYPE::SPHERE:
			AddShapeAction::addShape(csh);
			break;
			case RapidEnums::SHAPETYPE::CLOUDPOINTS:
			AddShapeAction::addShape(csh);
			break;
		}
}

//Destructor.. Release the memory here!
DummyHandler::~DummyHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DUHANDLER0003", __FILE__, __FUNCSIG__); }
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void DummyHandler::init()
{
	try
	{
		setMaxClicks(1);
		runningPartprogramValid = false;
		valid = false;
		/*MAINDllOBJECT->SetStatusCode("PTHDLR0001");*/
		//MAINDllOBJECT->SetStatusCode("DHRL01");
		MAINDllOBJECT->CURRENTRAPIDHANDLER = RapidEnums::RAPIDDRAWHANDLERTYPE::RAPIDFRAMEGRAB_HANDLER;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DUHANDLER0004", __FILE__, __FUNCSIG__); }
}

//Function to handle the mouse move..
void DummyHandler::mouseMove()
{
	try
	{		
		if(MAINDllOBJECT->getCurrentWindow() == 0)
			MAINDllOBJECT->ToolSelectedMousemove(getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY(), getClicksValue(getClicksDone()).getZ());
		valid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DUHANDLER0005", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void DummyHandler::LmouseDown()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DUHANDLER0006", __FILE__, __FUNCSIG__); }
}

//Escape buttons press handling..
void DummyHandler::EscapebuttonPress()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DUHANDLER0007", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void DummyHandler::draw(int windowno, double WPixelWidth)
{
	try
	{		
		if(!valid) return;
		if(windowno != 0) return;
		if(MAINDllOBJECT->MouseOnVideo())
			GRAFIX->drawFlexibleCrossHair(getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DUHANDLER0008", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data..//
void DummyHandler::PartProgramData()
{
	try
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
		valid = false;
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DUHANDLER0009", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void DummyHandler::LmaxmouseDown()
{
	try
	{			
		valid = false;
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				setBaseRProperty(getClicksValue(0).getX(), getClicksValue(0).getY(), getClicksValue(0).getZ());
				((ShapeWithList*)CurrentShape)->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
				PPCALCOBJECT->partProgramReached();
				return;
			}
		}
		CurrentShape = (Shape*)(MAINDllOBJECT->getShapesList().selectedItem());
		if(CurrentShape == NULL)
		{
			MAINDllOBJECT->SetStatusCode("PTHDLR0003");
			return;
		}
		AddPointToSelectedShape();
		MAINDllOBJECT->SetStatusCode("PTHDLR0002");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DUHANDLER0010", __FILE__, __FUNCSIG__); }
}

//Add the clicked point to the selected shape..With all relations, conditions etc..
void DummyHandler::AddPointToSelectedShape()
{
	try
	{		
		baseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::POINT_FOR_SHAPE);
		baseaction->points[0].set(getClicksValue(0).getX(), getClicksValue(0).getY(), MAINDllOBJECT->getCurrentDRO().getZ());
		baseaction->PointDRO.set(MAINDllOBJECT->getCurrentDRO());
		if(MAINDllOBJECT->getCurrentWindow() == 1 && MAINDllOBJECT->highlightedPoint() != NULL)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTMH0011", __FILE__, __FUNCSIG__); }
}
#include "StdAfx.h"
#include "..\Handlers\HandlerHeaders.h"
#include "..\Engine\RCadApp.h"
#include "..\Helper\Helper.h"
#include "..\Shapes\Circle.h"
#include "..\Actions\AddShapeAction.h"

//Default constuctor..
SphereFrameGrabHandler::SphereFrameGrabHandler()
{
	try
	{
		count = 0;
		FirstTime = true;
		SelectedShape = new Circle();
		AddShapeAction::addShape(SelectedShape);
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SMAAMEASUREMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
SphereFrameGrabHandler::~SphereFrameGrabHandler()
{
	try
	{
	   if(SelectedShape != NULL)
		 delete SelectedShape;
	}
	catch(...)
	{
	
	}
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void SphereFrameGrabHandler::init()
{
	try
	{
		CurrentdrawHandler = NULL;
		setMaxClicks(1);
		if(FirstTime)
		{
		/*	ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB);*/
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ARCFG);
			if(CurrentdrawHandler != NULL)
			{
				delete CurrentdrawHandler;
				CurrentdrawHandler = NULL;
			}
			CurrentdrawHandler = new FrameGrabArc();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SMAAMEASUREMH0002", __FILE__, __FUNCSIG__); }
}

void SphereFrameGrabHandler::mouseMove()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->mouseMove_x(clicks[0].getX(), clicks[0].getY());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SMAAMEASUREMH0007", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void SphereFrameGrabHandler::LmouseDown()
{	
}

//Left Mouse Up
void SphereFrameGrabHandler::LmouseUp(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->LmouseUp(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SMAAMEASUREMH0008", __FILE__, __FUNCSIG__); }
}

//Middle Mouse down..
void SphereFrameGrabHandler::MmouseDown(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MmouseDown(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SMAAMEASUREMH0009", __FILE__, __FUNCSIG__); }
}

//Mouse Scroll
void SphereFrameGrabHandler::MouseScroll(bool flag)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MouseScroll(flag);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SMAAMEASUREMH0010", __FILE__, __FUNCSIG__); }
}

//Handle right mouse down... Add new line/ initialise new measurement..
void SphereFrameGrabHandler::RmouseDown(double x, double y)
{	
}

//Hanlde the left mouse down.. Last click for the current handler..
void SphereFrameGrabHandler::LmaxmouseDown()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CheckAllTheConditions();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SMAAMEASUREMH0013", __FILE__, __FUNCSIG__); }
}

//Check all the required conditions for the mouse down..
void SphereFrameGrabHandler::CheckAllTheConditions()
{
	try
	{
		if(CurrentdrawHandler->LmouseDown_x(clicks[0].getX(), clicks[0].getY()))
		{
			create_newFramgrab();			
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SMAAMEASUREMH0014", __FILE__, __FUNCSIG__); }
}

//Escape buttons press handling..
void SphereFrameGrabHandler::EscapebuttonPress()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->EscapebuttonPress();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SMAAMEASUREMH0016", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void SphereFrameGrabHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
			CurrentdrawHandler->draw(windowno, WPixelWidth);
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("SMAAMEASUREMH0017", __FILE__, __FUNCSIG__); 
	}
}

void SphereFrameGrabHandler::create_newFramgrab()
{
	try
	{
		count++;
		if(count == 4)
		{
			if(CurrentdrawHandler != NULL)
			{
				delete CurrentdrawHandler;
				CurrentdrawHandler = NULL;
			}
			MAINDllOBJECT->update_VideoGraphics();
			double pointOfRotation[3] = {0};
			((Circle*)SelectedShape)->getCenter()->FillDoublePointer(pointOfRotation, 3);
			MAINDllOBJECT->VisionSpherCalibrationOver(pointOfRotation[0], pointOfRotation[1], pointOfRotation[2]);
	    	return;
		}
		else
		{
			ShapeWithList* myshapeList = (ShapeWithList*)(SelectedShape);
			int j = myshapeList->PointAtionList.size();
			if(j > 0)
			{
				list<BaseItem*> actionList = myshapeList->PointAtionList;
				list<BaseItem*>::iterator framgrabitr = actionList.begin(); 
				RAction* Currentaction = static_cast<RAction*>((*framgrabitr));
				if(Currentaction->CurrentActionType != RapidEnums::ACTIONTYPE::ADDPOINTACTION) return;
				Vector &curDroPos = ((AddPointAction*)Currentaction)->getFramegrab()->PointDRO;
				Vector temp, temp1;
				double pointOfRotation[3] = {0}, transformMatrix[9] = {0}, RotationPlane[4] = {0, 0, 1, 0};
				((Circle*)myshapeList)->getCenter()->FillDoublePointer(pointOfRotation, 3);
				temp1.set(curDroPos.getX() - pointOfRotation[0], curDroPos.getY() - pointOfRotation[1], curDroPos.getZ() - pointOfRotation[2]);
				RMATH3DOBJECT->RotationAround3DAxisThroughOrigin(count * M_PI_2, RotationPlane, transformMatrix);
				temp = MAINDllOBJECT->TransformMultiply(transformMatrix, temp1.getX(), temp1.getY(), temp1.getZ());
				double LastTargetSend[3] = {temp.getX() + pointOfRotation[0], temp.getY() + pointOfRotation[1], temp.getZ() + pointOfRotation[2]};
				MAINDllOBJECT->MoveMachineToRequiredPos(LastTargetSend[0], LastTargetSend[1], LastTargetSend[2], true, RapidEnums::MACHINEGOTOTYPE::SPHERVISIONCALIBRATION);
			}
		}

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAM0008", __FILE__, __FUNCSIG__); }
}

void SphereFrameGrabHandler::get_FramGrabParameter()
{
	try
	{
		RapidEnums::RAPIDFGACTIONTYPE CurrentFGType;
		bool FrameGrabDone = false;
		ShapeWithList* myshapeList = (ShapeWithList*)(SelectedShape);
		list<BaseItem*> actionList = myshapeList->PointAtionList;
		list<BaseItem*>::iterator framgrabitr = actionList.begin(); 
		RAction* Currentaction = static_cast<RAction*>((*framgrabitr));
		AddPointAction* Caction = (AddPointAction*)(*framgrabitr);
		FramegrabBase* baseaction = new FramegrabBase(Caction->getFramegrab());
		Vector GotoOrigin, ShiftPt;
		double pointOfRotation[3] = {0}, transformMatrix[9] = {0}, RotationPlane[4] = {0, 0, 1, 0};
	    ((Circle*)myshapeList)->getCenter()->FillDoublePointer(pointOfRotation);
		GotoOrigin.set(-pointOfRotation[0], -pointOfRotation[1], -pointOfRotation[2]);
		baseaction->TranslatePosition(GotoOrigin, ((AddPointAction*)Caction)->getShape()->UcsId());
		RMATH3DOBJECT->RotationAround3DAxisThroughOrigin(count * M_PI_2, RotationPlane, transformMatrix);
		baseaction->TrasformthePosition(transformMatrix, ((AddPointAction*)Caction)->getShape()->UcsId());
		ShiftPt.set(pointOfRotation[0], pointOfRotation[1], pointOfRotation[2]);
		baseaction->TranslatePosition(ShiftPt, ((AddPointAction*)Caction)->getShape()->UcsId());
	
		MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ARCFG);
		if(CurrentdrawHandler != NULL)
		{
			delete CurrentdrawHandler;
			CurrentdrawHandler = NULL;
		}
		CurrentdrawHandler = new FrameGrabArc(baseaction);
		MAINDllOBJECT->update_VideoGraphics();
		Sleep(100);
		
		if(myshapeList != NULL)
		{
			//grab points from the surface....
			FrameGrabDone = HELPEROBJECT->DoCurrentFrameGrab_SurfaceOrProfile(baseaction, false, true);
			if(FrameGrabDone)//add points to the shape..........
			{
				AddPointAction::pointAction(myshapeList, baseaction);
				myshapeList->IgnoreManualCrosshairSteps(true);
			}
		}
		this->create_newFramgrab();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAM0009", __FILE__, __FUNCSIG__); }
}
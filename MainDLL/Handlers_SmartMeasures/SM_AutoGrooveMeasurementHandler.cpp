#include "StdAfx.h"
#include "..\Handlers\HandlerHeaders.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddDimAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Measurement\DimGrooveMeasurement.h"
#include "..\Measurement\DimLineToLineDistance.h"
#include "..\Shapes\Line.h"

//Default constuctor..
SM_AutoGrooveMeasurementHandler::SM_AutoGrooveMeasurementHandler(bool InitialiseShape)
{
	try
	{
		init();
		if(InitialiseShape)
			InitialiseNewDistMeasure();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
SM_AutoGrooveMeasurementHandler::~SM_AutoGrooveMeasurementHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0002", __FILE__, __FUNCSIG__); }
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void SM_AutoGrooveMeasurementHandler::init()
{
	try
	{
		CurrentdrawHandler = NULL;
		ParentShape1 = NULL;
		setMaxClicks(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0003", __FILE__, __FUNCSIG__); }
}

//Initialise handler for New Angle Measure..!!
void SM_AutoGrooveMeasurementHandler::InitialiseNewDistMeasure()
{
	try
	{
		AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
		ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		CurrentdrawHandler = new FrameGrabRectangle(true);
		CurrentdrawHandler->CurrentShape = ParentShape1;
		FinishedCurrentDrawing = false;
		AddNewShapeMeasurement = false; DoMeasurement = true;
    }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0004", __FILE__, __FUNCSIG__); }
}

//Change the current handler type..
void SM_AutoGrooveMeasurementHandler::ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle)
{
	try
	{
		if(!(CHandle == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER || CHandle == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR || CHandle == RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB || 
			CHandle == RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB || CHandle == RapidEnums::RAPIDHANDLERTYPE::CIRCLE_FRAMEGRAB || CHandle == RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB ||
			CHandle == RapidEnums::RAPIDHANDLERTYPE::FIXEDRECT_FRAMEGRAB))
			return;
		CurrentHandlerType = CHandle;
		MouseHandler* Crnthandle = MAINDllOBJECT->SetCurrentSMFGHandler(CHandle);
		if(Crnthandle != NULL)
		{
			if(CurrentdrawHandler != NULL)
			{
				delete CurrentdrawHandler;
				CurrentdrawHandler = NULL;
			}
			CurrentdrawHandler = Crnthandle;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0008", __FILE__, __FUNCSIG__); }
}

//Function to handle the mouse move..
void SM_AutoGrooveMeasurementHandler::mouseMove()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->mouseMove_x(clicks[0].getX(), clicks[0].getY());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0009", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void SM_AutoGrooveMeasurementHandler::LmouseDown()
{	
	try
	{  
		CurrentdrawHandler->LmouseDown_x(this->getClicksValue(getClicksDone()).getX(), this->getClicksValue(getClicksDone()).getY());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0010", __FILE__, __FUNCSIG__); }
}

//Left Mouse Up
void SM_AutoGrooveMeasurementHandler::LmouseUp(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->LmouseUp(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0011", __FILE__, __FUNCSIG__); }
}

//Middle Mouse down..
void SM_AutoGrooveMeasurementHandler::MmouseDown(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MmouseDown(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0012", __FILE__, __FUNCSIG__); }
}

//Mouse Scroll
void SM_AutoGrooveMeasurementHandler::MouseScroll(bool flag)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MouseScroll(flag);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0013", __FILE__, __FUNCSIG__); }
}

//Handle right mouse down... Add new line/ initialise new measurement..
void SM_AutoGrooveMeasurementHandler::RmouseDown(double x, double y)
{
	try
	{
		if(CurrentdrawHandler  != NULL)
		CurrentdrawHandler->RmouseDown(x, y);	
		if(AddNewShapeMeasurement)
			{
					AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
					ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
					DoMeasurement= true; AddNewShapeMeasurement = false; 
			}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0014", __FILE__, __FUNCSIG__); }
}


//Hanlde the left mouse down.. Last click for the current handler..
void SM_AutoGrooveMeasurementHandler::LmaxmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() != NULL) return;
		if(CurrentdrawHandler->LmouseDown_x(clicks[0].getX(), clicks[0].getY()) && DoMeasurement)
		{
			Addmeasurement(_T("Wth"), RapidEnums::MEASUREMENTTYPE::DIM_GROOVEWIDTH);
			Addmeasurement(_T("Wth"), RapidEnums::MEASUREMENTTYPE::DIM_GROOVEWIDTH, false);
			Addmeasurement(_T("Dpt"), RapidEnums::MEASUREMENTTYPE::DIM_GROOVEDEPTH);
			Addmeasurement(_T("Ang"), RapidEnums::MEASUREMENTTYPE::DIM_GROOVEANGLE);
			AddNewShapeMeasurement = true; DoMeasurement = false;
			FinishedCurrentDrawing = true;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0016", __FILE__, __FUNCSIG__); }
}

void SM_AutoGrooveMeasurementHandler::Addmeasurement(TCHAR* Name, RapidEnums::MEASUREMENTTYPE CurrentMeasurementType, bool maxwidth)
{
	    DimGrooveMeasurement* CurrentMeasure;
	    CurrentMeasure = new DimGrooveMeasurement(Name, CurrentMeasurementType);
		CurrentMeasure->CalculateMeasurement(ParentShape1);
		ParentShape1->addMChild(CurrentMeasure);
		CurrentMeasure->addParent(ParentShape1);
		CurrentMeasure->CalculateDiffGrooveMeasurements(maxwidth);
		AddDimAction::addDim(CurrentMeasure);
}

//Escape buttons press handling..
void SM_AutoGrooveMeasurementHandler::EscapebuttonPress()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR && CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
			CurrentdrawHandler->EscapebuttonPress();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0018", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void SM_AutoGrooveMeasurementHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(windowno == 1)
		{
			if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
				CurrentdrawHandler->draw(windowno, WPixelWidth);
		}
		else
			CurrentdrawHandler->draw(windowno, WPixelWidth);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0019", __FILE__, __FUNCSIG__); }
}


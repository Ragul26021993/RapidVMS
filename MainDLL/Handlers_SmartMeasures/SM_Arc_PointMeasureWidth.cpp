#include "StdAfx.h"
#include "..\Handlers\HandlerHeaders.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddDimAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Measurement\DimPoint2ArcWidthMeasurement.h"
#include "..\Helper\Helper.h"

//Default constuctor..
SM_Arc_PointMeasureWidth::SM_Arc_PointMeasureWidth()
{
	try
	{
		init();
		InitialiseNewDistMeasure();
//		MAINDllOBJECT->SetStatusCode("SM_Line_LineWidthMH01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Arc_PointWidthMH0`", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
SM_Arc_PointMeasureWidth::~SM_Arc_PointMeasureWidth()
{
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void SM_Arc_PointMeasureWidth::init()
{
	try
	{
		CurrentdrawHandler = NULL;
		CurrentMeasurementShCount = 0;
		AddNewShapeForMeasurement = false;
		ParentShape1 = NULL;
		ParentShape2 = NULL;
		CurrentMeasure = NULL;
		setMaxClicks(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Arc_PointWidthMH02", __FILE__, __FUNCSIG__); }
}

//Initialise handler for New Angle Measure..!!
void SM_Arc_PointMeasureWidth::InitialiseNewDistMeasure()
{
	try
	{
		Shape* CSh = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
		((Circle*)CSh)->CircleType = RapidEnums::CIRCLETYPE::ARC_MIDDLE;
		AddShapeAction::addShape(CSh);
		ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		if(MAINDllOBJECT->getProfile())
			ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB);
		else
			ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER);
		FinishedCurrentDrawing = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Arc_PointWidthMH03", __FILE__, __FUNCSIG__); }
}

void SM_Arc_PointMeasureWidth::AddShapeMeasureRelationShip(Shape* CShape)
{
	try
	{
		if(CShape != NULL)
		{
			CShape->addMChild(CurrentMeasure);
			CurrentMeasure->addParent(CShape);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Arc_PointWidthMH04", __FILE__, __FUNCSIG__); }
}

//Add New Angle measurement
void SM_Arc_PointMeasureWidth::AddDistanceMeasurement()
{
	try
	{
		double midpoint1[2] = {0}, midpoint2[2] = {0};
		((Circle*)ParentShape1)->getCenter()->FillDoublePointer(midpoint1); ((RPoint*)ParentShape2)->getPosition()->FillDoublePointer(midpoint2);
		double midpoint[2] = {(midpoint1[0] + midpoint2[0])/2, (midpoint1[1] + midpoint2[1])/2};
		CurrentMeasure = new DimPoint2ArcWidthMeasurement(_T("Dist"));
		CurrentMeasure->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_POINT2ARCWIDTHMEASUREMENT;
		CurrentMeasure->SetMousePosition(midpoint[0], midpoint[1], ((Circle*)ParentShape1)->getCenter()->getZ());
		((DimPoint2ArcWidthMeasurement*)CurrentMeasure)->CalculateMeasurement(ParentShape1, ((RPoint*)ParentShape2)->getPosition());
		CurrentMeasure->ContainsLinearMeasurementMode(true);
		CurrentMeasure->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
	
		double SelectionLine[6] = {0};
		MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
		CurrentMeasure->SetSelectionLine(&SelectionLine[0]);
		CurrentMeasure->LinearMeasurementMode(true);
		AddShapeMeasureRelationShip(ParentShape1);
		AddShapeMeasureRelationShip(ParentShape2);
		CurrentMeasure->IsValid(true);
		CurrentMeasure->UpdateMeasurement();
		AddDimAction::addDim(CurrentMeasure);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Arc_PointWidthMH05", __FILE__, __FUNCSIG__); }
}

//Change the current handler type..
void SM_Arc_PointMeasureWidth::ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle)
{
	try
	{
		if(!(CHandle == RapidEnums::RAPIDHANDLERTYPE::POINT_HANDLER || CHandle == RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER || CHandle == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR || CHandle == RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB || 
			CHandle == RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB || CHandle == RapidEnums::RAPIDHANDLERTYPE::CIRCLE_FRAMEGRAB || CHandle == RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB ||
			CHandle == RapidEnums::RAPIDHANDLERTYPE::FIXEDRECT_FRAMEGRAB || CHandle == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARC_FRAMEGRAB || CHandle == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARECT_FRAMEGRAB ||
			CHandle == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_CIRCLE_FRAMEGRAB || CHandle == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_RECT_FRAMEGRAB || CHandle == RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_FIXEDRECT_FRAMEGRAB))
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Arc_PointWidthMH06", __FILE__, __FUNCSIG__); }
}

//Function to handle the mouse move..
void SM_Arc_PointMeasureWidth::mouseMove()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->mouseMove_x(clicks[0].getX(), clicks[0].getY());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Arc_PointWidthMH07", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void SM_Arc_PointMeasureWidth::LmouseDown()
{	
}

//Left Mouse Up
void SM_Arc_PointMeasureWidth::LmouseUp(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->LmouseUp(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Arc_PointWidthMH08", __FILE__, __FUNCSIG__); }
}

//Middle Mouse down..
void SM_Arc_PointMeasureWidth::MmouseDown(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MmouseDown(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Arc_PointWidthMH09", __FILE__, __FUNCSIG__); }
}

//Mouse Scroll
void SM_Arc_PointMeasureWidth::MouseScroll(bool flag)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MouseScroll(flag);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Arc_PointWidthMH10", __FILE__, __FUNCSIG__); }
}

//Handle right mouse down... Add new line/ initialise new measurement..
void SM_Arc_PointMeasureWidth::RmouseDown(double x, double y)
{
	try
	{
		if(CurrentdrawHandler  != NULL)
		{
			if(CurrentdrawHandler->getClicksDone() == 0)
				CheckRightClickConditions();
			else
				CurrentdrawHandler->RmouseDown(x, y);
		}
		else
			CheckRightClickConditions();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Arc_PointWidthMH11", __FILE__, __FUNCSIG__); }
}

//Check the conditions for right click
void SM_Arc_PointMeasureWidth::CheckRightClickConditions()
{
	try
	{
		if(!AddNewShapeForMeasurement) return;
		if(CurrentMeasurementShCount == 0)
		{
			ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::POINT_HANDLER);
			ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			AddShapeAction::addShape(ParentShape2);
			CurrentMeasurementShCount++;
			AddNewShapeForMeasurement = false;
		}
    	else if(CurrentMeasurementShCount == 1)
		{
			CurrentMeasurementShCount = 0;
			init();
			InitialiseNewDistMeasure();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Arc_PointWidthMH12", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void SM_Arc_PointMeasureWidth::LmaxmouseDown()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CheckAllTheConditions();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Arc_PointWidthMH13", __FILE__, __FUNCSIG__); }
}

//Check all the required conditions for the mouse down..
void SM_Arc_PointMeasureWidth::CheckAllTheConditions()
{
	try
	{
		if(CurrentdrawHandler->LmouseDown_x(clicks[0].getX(), clicks[0].getY()))
		{
			AddNewShapeForMeasurement = true;
			if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER)
			{
				delete CurrentdrawHandler;
				CurrentdrawHandler = NULL;
			}
			else if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::POINT_HANDLER)
			{
				delete CurrentdrawHandler;
				CurrentdrawHandler = NULL;		
			}
			if(CurrentMeasurementShCount == 1)
			{		
				if(((ShapeWithList*)ParentShape2)->PointsList->Pointscount() > 0)
				{
					AddDistanceMeasurement();
					FinishedCurrentDrawing = true;
				}			
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Arc_PointWidthMH14", __FILE__, __FUNCSIG__); }
}

void SM_Arc_PointMeasureWidth::RotationStopedAddMeasure()
{
	if(CurrentMeasurementShCount == 1)
	{
		if(((ShapeWithList*)ParentShape2)->PointsList->Pointscount() > 1)
		{
			AddDistanceMeasurement();
			FinishedCurrentDrawing = true;
		}
	}
}

//Escape buttons press handling..
void SM_Arc_PointMeasureWidth::EscapebuttonPress()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR && CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER)
			CurrentdrawHandler->EscapebuttonPress();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Arc_PointWidthMH15", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void SM_Arc_PointMeasureWidth::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(windowno == 1)
		{
			if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER)
				CurrentdrawHandler->draw(windowno, WPixelWidth);
		}
		else
			CurrentdrawHandler->draw(windowno, WPixelWidth);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Arc_PointWidthMH16", __FILE__, __FUNCSIG__); }
}
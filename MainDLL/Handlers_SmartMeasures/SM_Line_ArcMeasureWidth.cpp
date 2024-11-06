#include "StdAfx.h"
#include "..\Handlers\HandlerHeaders.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddDimAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Measurement\DimLine2ArcWidthMeasurement.h"
#include "..\Helper\Helper.h"
#include "..\Shapes\Line.h"

//Default constuctor..
SM_Line_ArcMeasureWidth::SM_Line_ArcMeasureWidth()
{
	try
	{
		init();
		InitialiseNewDistMeasure();
//		MAINDllOBJECT->SetStatusCode("SM_Line_LineWidthMH01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Line_ArcWidthMH01", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
SM_Line_ArcMeasureWidth::~SM_Line_ArcMeasureWidth()
{
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void SM_Line_ArcMeasureWidth::init()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Line_ArcWidthMH02", __FILE__, __FUNCSIG__); }
}

//Initialise handler for New Angle Measure..!!
void SM_Line_ArcMeasureWidth::InitialiseNewDistMeasure()
{
	try
	{
		AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
		ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		if(MAINDllOBJECT->getProfile())
			ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB);
		else
			ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER);
		FinishedCurrentDrawing = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Line_ArcWidthMH03", __FILE__, __FUNCSIG__); }
}

void SM_Line_ArcMeasureWidth::AddShapeMeasureRelationShip(Shape* CShape)
{
	try
	{
		if(CShape != NULL)
		{
			CShape->addMChild(CurrentMeasure);
			CurrentMeasure->addParent(CShape);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Line_ArcWidthMH04", __FILE__, __FUNCSIG__); }
}

//Add New Angle measurement
void SM_Line_ArcMeasureWidth::AddDistanceMeasurement()
{
	try
	{
		double midpoint1[2] = {0}, midpoint2[2] = {0}, SelectionLine[6] = {0};
		((Line*)ParentShape1)->getMidPoint()->FillDoublePointer(midpoint1); ((Circle*)ParentShape2)->getCenter()->FillDoublePointer(midpoint2);
		double midpoint[2] = {(midpoint1[0] + midpoint2[0])/2, (midpoint1[1] + midpoint2[1])/2};
		CurrentMeasure = new DimLine2ArcWidthMeasurement(_T("Dist"));
		CurrentMeasure->SetMousePosition(midpoint[0], midpoint[1], ((Line*)ParentShape1)->getPoint1()->getZ());
		((DimLine2ArcWidthMeasurement*)CurrentMeasure)->CalculateMeasurement(ParentShape1, ParentShape2);
		CurrentMeasure->ContainsLinearMeasurementMode(true);
		CurrentMeasure->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
		MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
		CurrentMeasure->SetSelectionLine(&SelectionLine[0]);
		CurrentMeasure->LinearMeasurementMode(true);
		AddShapeMeasureRelationShip(ParentShape1);
		AddShapeMeasureRelationShip(ParentShape2);
		CurrentMeasure->IsValid(true);
		CurrentMeasure->UpdateMeasurement();
		AddDimAction::addDim(CurrentMeasure);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Line_ArcWidthMH05", __FILE__, __FUNCSIG__); }
}

//Change the current handler type..
void SM_Line_ArcMeasureWidth::ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle)
{
	try
	{
		if(!(CHandle == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER || CHandle == RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER || CHandle == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR || CHandle == RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB || 
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Line_ArcWidthMH06", __FILE__, __FUNCSIG__); }
}

//Function to handle the mouse move..
void SM_Line_ArcMeasureWidth::mouseMove()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->mouseMove_x(clicks[0].getX(), clicks[0].getY());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Line_ArcWidthMH07", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void SM_Line_ArcMeasureWidth::LmouseDown()
{	
}

//Left Mouse Up
void SM_Line_ArcMeasureWidth::LmouseUp(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->LmouseUp(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Line_ArcWidthMH08", __FILE__, __FUNCSIG__); }
}

//Middle Mouse down..
void SM_Line_ArcMeasureWidth::MmouseDown(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MmouseDown(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Line_ArcWidthMH09", __FILE__, __FUNCSIG__); }
}

//Mouse Scroll
void SM_Line_ArcMeasureWidth::MouseScroll(bool flag)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MouseScroll(flag);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Line_ArcWidthMH10", __FILE__, __FUNCSIG__); }
}

//Handle right mouse down... Add new line/ initialise new measurement..
void SM_Line_ArcMeasureWidth::RmouseDown(double x, double y)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Line_ArcWidthMH11", __FILE__, __FUNCSIG__); }
}

//Check the conditions for right click
void SM_Line_ArcMeasureWidth::CheckRightClickConditions()
{
	try
	{
		if(!AddNewShapeForMeasurement) return;
		if(CurrentMeasurementShCount == 0)
		{
			AddShapeAction::addShape(new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC));
			ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			((Circle*)ParentShape2)->CircleType = RapidEnums::CIRCLETYPE::ARC_MIDDLE;
			CurrentMeasurementShCount++;
			if(MAINDllOBJECT->getProfile())
				ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB);
			else
				ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER);
			AddNewShapeForMeasurement = false;
		}
    	else if(CurrentMeasurementShCount == 1)
		{
			CurrentMeasurementShCount = 0;
			init();
			InitialiseNewDistMeasure();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Line_ArcWidthMH12", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void SM_Line_ArcMeasureWidth::LmaxmouseDown()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CheckAllTheConditions();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Line_ArcWidthMH13", __FILE__, __FUNCSIG__); }
}

//Check all the required conditions for the mouse down..
void SM_Line_ArcMeasureWidth::CheckAllTheConditions()
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
				if(((ShapeWithList*)ParentShape2)->PointsList->Pointscount() > 2)
				{
					AddDistanceMeasurement();
					FinishedCurrentDrawing = true;
				}
			}
			else
			{
				if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
				{
					delete CurrentdrawHandler;
					CurrentdrawHandler = NULL;				
				}
				if(((ShapeWithList*)ParentShape2)->PointsList->Pointscount() > 1)
				{
					AddDistanceMeasurement();
					FinishedCurrentDrawing = true;
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Line_ArcWidthMH14", __FILE__, __FUNCSIG__); }
}

void SM_Line_ArcMeasureWidth::RotationStopedAddMeasure()
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
void SM_Line_ArcMeasureWidth::EscapebuttonPress()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER && CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR && CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
			CurrentdrawHandler->EscapebuttonPress();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Line_ArcWidthMH15", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void SM_Line_ArcMeasureWidth::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(windowno == 1)
		{
			if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER || CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
				CurrentdrawHandler->draw(windowno, WPixelWidth);
		}
		else
			CurrentdrawHandler->draw(windowno, WPixelWidth);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Line_ArcWidthMH16", __FILE__, __FUNCSIG__); }
}
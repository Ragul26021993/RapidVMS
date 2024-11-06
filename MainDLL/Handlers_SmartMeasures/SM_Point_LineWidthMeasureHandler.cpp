#include "StdAfx.h"
#include "..\Handlers\HandlerHeaders.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddDimAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Measurement\DimPointToLineDistance.h"
#include "..\Shapes\RPoint.h"
#include "..\Shapes\Line.h"
#include "..\Helper\Helper.h"
#include "..\Handlers_SmartMeasures\SM_Point_LineWidthMeasureHandler.h"

//Default constuctor..
SM_Point_LineWidthMeasureHandler::SM_Point_LineWidthMeasureHandler()
{
	try
	{
		init();
		InitialiseNewDistMeasure();
		//MAINDllOBJECT->SetStatusCode("SM_Line_LineWidthMH01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Point_LineWidthMH01", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
SM_Point_LineWidthMeasureHandler::~SM_Point_LineWidthMeasureHandler()
{
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void SM_Point_LineWidthMeasureHandler::init()
{
	try
	{
		CurrentdrawHandler = NULL;
		CurrentMeasurementShCount = 0;
		AddNewShapeForMeasurement = false;
		ParentShape1 = NULL;
		CurrentMeasure = NULL;
		setMaxClicks(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Point_LineWidthMH02", __FILE__, __FUNCSIG__); }
}

//Initialise handler for New Angle Measure..!!
void SM_Point_LineWidthMeasureHandler::InitialiseNewDistMeasure()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Point_LineWidthMH03", __FILE__, __FUNCSIG__); }
}

void SM_Point_LineWidthMeasureHandler::AddShapeMeasureRelationShip(Shape* CShape)
{
	try
	{
		if(CShape != NULL)
		{
			CShape->addMChild(CurrentMeasure);
			CurrentMeasure->addParent(CShape);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Point_LineWidthMH04", __FILE__, __FUNCSIG__); }
}

//Add New Angle measurement
void SM_Point_LineWidthMeasureHandler::AddDistanceMeasurement()
{
	try
	{
		double midpoint1[2] = {0}, midpoint2[2] = {0};
		((RPoint*)ParentShape2)->getPosition()->FillDoublePointer(midpoint1); ((Line*)ParentShape1)->getMidPoint()->FillDoublePointer(midpoint2);
		double midpoint[2] = {(midpoint1[0] + midpoint2[0])/2, (midpoint1[1] + midpoint2[1])/2};
		CurrentMeasure = new DimPointToLineDistance(_T("Dist"));
		CurrentMeasure->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_POINTTOLINEDISTANCE;
		CurrentMeasure->SetMousePosition(midpoint[0], midpoint[1], ((Line*)ParentShape1)->getPoint1()->getZ());
		((DimPointToLineDistance*)CurrentMeasure)->CalculateMeasurement(ParentShape1, ((RPoint*)ParentShape2)->getPosition());
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Point_LineWidthMH05", __FILE__, __FUNCSIG__); }
}

//Change the current handler type..
void SM_Point_LineWidthMeasureHandler::ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle)
{
	try
	{
		if(!(CHandle == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER || CHandle == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR || CHandle == RapidEnums::RAPIDHANDLERTYPE::POINT_HANDLER || CHandle == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR || CHandle == RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB || 
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Point_LineWidthMH06", __FILE__, __FUNCSIG__); }
}

//Function to handle the mouse move..
void SM_Point_LineWidthMeasureHandler::mouseMove()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->mouseMove_x(clicks[0].getX(), clicks[0].getY());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Point_LineWidthMH07", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void SM_Point_LineWidthMeasureHandler::LmouseDown()
{	
}

//Left Mouse Up
void SM_Point_LineWidthMeasureHandler::LmouseUp(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->LmouseUp(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Point_LineWidthMH08", __FILE__, __FUNCSIG__); }
}

//Middle Mouse down..
void SM_Point_LineWidthMeasureHandler::MmouseDown(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MmouseDown(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Point_LineWidthMH09", __FILE__, __FUNCSIG__); }
}

//Mouse Scroll
void SM_Point_LineWidthMeasureHandler::MouseScroll(bool flag)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MouseScroll(flag);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Point_LineWidthMH10", __FILE__, __FUNCSIG__); }
}

//Handle right mouse down... Add new line/ initialise new measurement..
void SM_Point_LineWidthMeasureHandler::RmouseDown(double x, double y)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Point_LineWidthMH11", __FILE__, __FUNCSIG__); }
}

//Check the conditions for right click
void SM_Point_LineWidthMeasureHandler::CheckRightClickConditions()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Point_LineWidthMH12", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void SM_Point_LineWidthMeasureHandler::LmaxmouseDown()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CheckAllTheConditions();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Point_LineWidthMH13", __FILE__, __FUNCSIG__); }
}

//Check all the required conditions for the mouse down..
void SM_Point_LineWidthMeasureHandler::CheckAllTheConditions()
{
	try
	{
		if(CurrentdrawHandler->LmouseDown_x(clicks[0].getX(), clicks[0].getY()))
		{
			AddNewShapeForMeasurement = true;
			if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
			{
				delete CurrentdrawHandler;
				CurrentdrawHandler = NULL;
				if(CurrentMeasurementShCount == 1)
				{
					AddDistanceMeasurement();
				    FinishedCurrentDrawing = true;
				}
			}
			else
			{
				if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::POINT_HANDLER)
				{
					delete CurrentdrawHandler;
					CurrentdrawHandler = NULL;
					if(CurrentMeasurementShCount == 1)
					{
						AddDistanceMeasurement();
						FinishedCurrentDrawing = true;
					}
				}
				else 
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
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Point_LineWidthMH14", __FILE__, __FUNCSIG__); }
}

void SM_Point_LineWidthMeasureHandler::RotationStopedAddMeasure()
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
void SM_Point_LineWidthMeasureHandler::EscapebuttonPress()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::POINT_HANDLER && CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
			CurrentdrawHandler->EscapebuttonPress();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Point_LineWidthMH15", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void SM_Point_LineWidthMeasureHandler::draw(int windowno, double WPixelWidth)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_Point_LineWidthMH16", __FILE__, __FUNCSIG__); }
}
#include "StdAfx.h"
#include "..\Handlers\HandlerHeaders.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddDimAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Measurement\DimLineToLineDistance.h"
#include "..\Shapes\Line.h"
#include "..\Helper\Helper.h"

//Default constuctor..
SM_PointAngleMeasurementHandler::SM_PointAngleMeasurementHandler()
{
	try
	{
		FirstTime = true;
		init();
		InitialiseNewDistMeasure();
		AxisShape = MAINDllOBJECT->Vblock_CylinderAxisLine;
		if(AxisShape == NULL)
		{
		   MAINDllOBJECT->SetStatusCode("PANGMH0001");
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PANGMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
SM_PointAngleMeasurementHandler::~SM_PointAngleMeasurementHandler()
{
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void SM_PointAngleMeasurementHandler::init()
{
	try
	{
		CurrentdrawHandler = NULL;
		CurrentMeasurementShCount = 0;
		AddNewShapeForMeasurement = false;
		AddAngleMeasurement = false;
		ParentShape1 = NULL;
		ParentShape2 = NULL;
		ParentShape3 = NULL;
		CurrentMeasure = NULL;
		setMaxClicks(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PANGMH0002", __FILE__, __FUNCSIG__); }
}

//Initialise handler for New Angle Measure..!!
void SM_PointAngleMeasurementHandler::InitialiseNewDistMeasure()
{
	try
	{
		AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
		ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		if(FirstTime)
		{
			if(MAINDllOBJECT->getProfile())
				ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB);
			else
				ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER);
		}
		else
			ChangeHanlder_Type(CurrentHandlerType);
		FinishedCurrentDrawing = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PANGMH0003", __FILE__, __FUNCSIG__); }
}

void SM_PointAngleMeasurementHandler::AddShapeMeasureRelationShip(Shape* CShape)
{
	try
	{
		if(CShape != NULL)
		{
			CShape->addMChild(CurrentMeasure);
			CurrentMeasure->addParent(CShape);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PANGMH0004", __FILE__, __FUNCSIG__); }
}

//Add New Angle measurement
void SM_PointAngleMeasurementHandler::AddDistanceMeasurement()
{
	try
	{
		FirstTime = false;
		if(ParentShape1 == NULL || ParentShape2 == NULL) return;

		//add  new reflected line....
		AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
		ParentShape3 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		((Line*)ParentShape3)->LineType = RapidEnums::LINETYPE::REFLECTEDLINE;
		ParentShape3->addParent(AxisShape);
		AxisShape->addChild(ParentShape3);
		ParentShape3->addParent(ParentShape2);
		ParentShape2->addChild(ParentShape3);
		ParentShape3->UpdateBestFit();

		if(ParentShape1 == NULL || ParentShape2 == NULL || ParentShape3 == NULL) return;

		//create measurement theta1 + theta2...
		CreateAngleMeasurement(ParentShape1, ParentShape3, false);
		CreateAngleMeasurement(ParentShape1, AxisShape, true);
		CreateAngleMeasurement(ParentShape2, AxisShape, true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PANGMH0005", __FILE__, __FUNCSIG__); }
}

void SM_PointAngleMeasurementHandler::CreateAngleMeasurement(Shape* Cshape1, Shape* Cshape2, bool AxisLinePresent)
{
	try
	{
		double midpoint1[2] = {0}, midpoint2[2] = {0};
		((Line*)Cshape1)->getMidPoint()->FillDoublePointer(midpoint1); 
		if(AxisLinePresent)
		{
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(((Line*)Cshape2)->Angle(), ((Line*)Cshape2)->Intercept(), midpoint1, midpoint2);
		}
		else
		{
			((Line*)Cshape2)->getMidPoint()->FillDoublePointer(midpoint2);
		}
		
		double midpoint[2] = {(midpoint1[0] + midpoint2[0])/2, (midpoint1[1] + midpoint2[1])/2};
		CurrentMeasure = new DimLineToLineDistance(_T("Ang"));
		CurrentMeasure->MeasurementType = RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEANGLE;
		((DimLineToLineDistance*)CurrentMeasure)->CalculateMeasurement(Cshape1, Cshape2);
		CurrentMeasure->SetMousePosition(midpoint[0], midpoint[1], ((Line*)Cshape1)->getPoint1()->getZ());
		CurrentMeasure->ContainsLinearMeasurementMode(true);
		CurrentMeasure->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
	
		double SelectionLine[6] = {0};
		MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
		CurrentMeasure->SetSelectionLine(&SelectionLine[0]);
		CurrentMeasure->LinearMeasurementMode(true);
		AddShapeMeasureRelationShip(Cshape1);
		AddShapeMeasureRelationShip(Cshape2);
		CurrentMeasure->IsValid(true);
		CurrentMeasure->UpdateMeasurement();
		AddDimAction::addDim(CurrentMeasure);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PANGMH0005a", __FILE__, __FUNCSIG__); }
}

//Change the current handler type..
void SM_PointAngleMeasurementHandler::ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle)
{
	try
	{
		if(!(CHandle == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER || CHandle == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR || CHandle == RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB || 
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PANGMH0006", __FILE__, __FUNCSIG__); }
}

//Function to handle the mouse move..
void SM_PointAngleMeasurementHandler::mouseMove()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->mouseMove_x(clicks[0].getX(), clicks[0].getY());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PANGMH0007", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void SM_PointAngleMeasurementHandler::LmouseDown()
{	
}

//Left Mouse Up
void SM_PointAngleMeasurementHandler::LmouseUp(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->LmouseUp(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PANGMH0008", __FILE__, __FUNCSIG__); }
}

//Middle Mouse down..
void SM_PointAngleMeasurementHandler::MmouseDown(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(AxisShape == NULL)
		{
			if(MAINDllOBJECT->highlightedShape() != NULL && (MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::XLINE))
			{
			   AxisShape = MAINDllOBJECT->highlightedShape();
			}
			else
		    	 MAINDllOBJECT->SetStatusCode("PANGMH0001");
			setClicksDone(0);
			return;
		}
		CurrentdrawHandler->MmouseDown(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PANGMH0009", __FILE__, __FUNCSIG__); }
}

//Mouse Scroll
void SM_PointAngleMeasurementHandler::MouseScroll(bool flag)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MouseScroll(flag);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PANGMH0010", __FILE__, __FUNCSIG__); }
}

//Handle right mouse down... Add new line/ initialise new measurement..
void SM_PointAngleMeasurementHandler::RmouseDown(double x, double y)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PANGMH0011", __FILE__, __FUNCSIG__); }
}

//Check the conditions for right click
void SM_PointAngleMeasurementHandler::CheckRightClickConditions()
{
	try
	{
		if(!AddNewShapeForMeasurement) return;
		if(AxisShape == NULL) return;
		if(CurrentMeasurementShCount == 0)
		{
			AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
			ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
    		CurrentMeasurementShCount++;
			ChangeHanlder_Type(CurrentHandlerType);
			AddNewShapeForMeasurement = false;
			/*double midPnt[3] = {0}, target[3] = {0};
			((Line*)ParentShape1)->getMidPoint()->FillDoublePointer(midPnt, 3);
			RMATH2DOBJECT->Point_PerpenIntrsctn_Line(((Line*)AxisShape)->Angle(), ((Line*)AxisShape)->Intercept(), midPnt, target);
			target[2] = midPnt[2];
			double dir[3] = {target[0] - midPnt[0], target[1] - midPnt[1], target[2] - midPnt[2]}, cos[3] = {0};
			RMATH3DOBJECT->DirectionCosines(dir, cos);
			double dist = RMATH3DOBJECT->Distance_Point_Point(midPnt, target);
			for(int i = 0; i < 3; i++)
			{
			   target[i] += dist * cos[i];
			}
			MAINDllOBJECT->MoveMachineToRequiredPos(target[0], target[1], target[2], true, RapidEnums::MACHINEGOTOTYPE::DONOTHING);*/
		}
    	else if(CurrentMeasurementShCount == 1)
		{
			CurrentMeasurementShCount = 0;
			init();
			InitialiseNewDistMeasure();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PANGMH0012", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void SM_PointAngleMeasurementHandler::LmaxmouseDown()
{
	try
	{
		if(AxisShape == NULL)
		{
			if(MAINDllOBJECT->highlightedShape() != NULL && (MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::XLINE))
			{
			   AxisShape = MAINDllOBJECT->highlightedShape();
			}
			else
				 MAINDllOBJECT->SetStatusCode("PANGMH0001");
			setClicksDone(0);
			return;
		}
		if(CurrentdrawHandler == NULL) return;
		CheckAllTheConditions();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PANGMH00013", __FILE__, __FUNCSIG__); }
}

//Check all the required conditions for the mouse down..
void SM_PointAngleMeasurementHandler::CheckAllTheConditions()
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
				if(CurrentMeasurementShCount == 1)
				{
					if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR)
					{
						if(((ShapeWithList*)ParentShape2)->PointsList->Pointscount() > 1)
						{
							AddDistanceMeasurement();
				        	FinishedCurrentDrawing = true;
						}
					}
					else
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PANGMH0014", __FILE__, __FUNCSIG__); }
}

void SM_PointAngleMeasurementHandler::RotationStopedAddMeasure()
{
	try
	{
		if(CurrentMeasurementShCount == 1)
		{
			if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR)
			{
				if(((ShapeWithList*)ParentShape2)->PointsList->Pointscount() > 1)
				{
					AddDistanceMeasurement();
					FinishedCurrentDrawing = true;
				}
			}
			else
			{
				if(((ShapeWithList*)ParentShape2)->PointsList->Pointscount() > 1)
				{
					AddDistanceMeasurement();
					FinishedCurrentDrawing = true;
				}
			}
		}
	}
	catch(...)
	{
	    MAINDllOBJECT->SetAndRaiseErrorMessage("PANGMH0015", __FILE__, __FUNCSIG__);
	}
}

//Escape buttons press handling..
void SM_PointAngleMeasurementHandler::EscapebuttonPress()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR && CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
			CurrentdrawHandler->EscapebuttonPress();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PANGMH0016", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void SM_PointAngleMeasurementHandler::draw(int windowno, double WPixelWidth)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PANGMH0017", __FILE__, __FUNCSIG__); }
}
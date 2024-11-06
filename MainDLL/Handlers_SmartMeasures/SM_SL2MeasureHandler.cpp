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
SM_SL2MeasureHandler::SM_SL2MeasureHandler()
{
	try
	{
		FirstTime = true;
		init();
		InitialiseNewDistMeasure();
		MAINDllOBJECT->SetStatusCode("SM_SL2MeasureHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL2MEASUREMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
SM_SL2MeasureHandler::~SM_SL2MeasureHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL2MEASUREMH0002", __FILE__, __FUNCSIG__); }
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void SM_SL2MeasureHandler::init()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL2MEASUREMH0003", __FILE__, __FUNCSIG__); }
}

//Initialise handler for New Angle Measure..!!
void SM_SL2MeasureHandler::InitialiseNewDistMeasure()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL2MEASUREMH0004", __FILE__, __FUNCSIG__); }
}

void SM_SL2MeasureHandler::AddShapeMeasureRelationShip(Shape* CShape)
{
	try
	{
		if(CShape != NULL)
		{
			CShape->addMChild(CurrentMeasure);
			CurrentMeasure->addParent(CShape);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL2MEASUREMH0005", __FILE__, __FUNCSIG__); }
}

//Add New Angle measurement
void SM_SL2MeasureHandler::AddDistanceMeasurement()
{
	try
	{
		FirstTime = false;
		Vector *Vpointer1, *Vpointer2;
		Vector temp;
		double IntersecPoint1[2], IntersecPoint2[2];
		Line* CSh1 = (Line*)ParentShape1; Line* CSh2 = (Line*)ParentShape2; Line* CSh3 = (Line*)ParentShape3;
		RMATH2DOBJECT->Line_lineintersection(CSh1->Angle(), CSh1->Intercept(), CSh2->Angle(), CSh2->Intercept(), &IntersecPoint1[0]);
		RMATH2DOBJECT->Line_lineintersection(CSh2->Angle(), CSh2->Intercept(), CSh3->Angle(), CSh3->Intercept(), &IntersecPoint2[0]);
		double midpoint[2] = {(IntersecPoint1[0] + IntersecPoint2[0])/2, (IntersecPoint1[1] + IntersecPoint2[1])/2};
		temp.set(IntersecPoint1[0], IntersecPoint1[1], MAINDllOBJECT->getCurrentDRO().getZ());
		Vpointer1 = MAINDllOBJECT->getVectorPointer(&temp);
		temp.set(IntersecPoint2[0], IntersecPoint2[1], MAINDllOBJECT->getCurrentDRO().getZ());
		Vpointer2 = MAINDllOBJECT->getVectorPointer(&temp);
		CurrentMeasure = HELPEROBJECT->CreatePointDim2D(Vpointer1, Vpointer2, midpoint[0] + 0.2, midpoint[1] + 0.2, 0);
		CurrentMeasure->LinearMeasurementMode(true);
		AddShapeMeasureRelationShip(ParentShape1);
		AddShapeMeasureRelationShip(ParentShape2);
		AddShapeMeasureRelationShip(ParentShape3);
		CurrentMeasure->IsValid(true);
		CurrentMeasure->UpdateMeasurement();
		AddDimAction::addDim(CurrentMeasure);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL2MEASUREMH0005", __FILE__, __FUNCSIG__); }
}

//Change the current handler type..
void SM_SL2MeasureHandler::ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle)
{
	try
	{
		if(!(CHandle == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER || CHandle == RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE || CHandle == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR || CHandle == RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB || 
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL2MEASUREMH0008", __FILE__, __FUNCSIG__); }
}

//Function to handle the mouse move..
void SM_SL2MeasureHandler::mouseMove()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->mouseMove_x(clicks[0].getX(), clicks[0].getY());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL2MEASUREMH0009", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void SM_SL2MeasureHandler::LmouseDown()
{	
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL2MEASUREMH0010", __FILE__, __FUNCSIG__); }
}

//Left Mouse Up
void SM_SL2MeasureHandler::LmouseUp(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->LmouseUp(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL2MEASUREMH0011", __FILE__, __FUNCSIG__); }
}

//Middle Mouse down..
void SM_SL2MeasureHandler::MmouseDown(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MmouseDown(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL2MEASUREMH0012", __FILE__, __FUNCSIG__); }
}

//Mouse Scroll
void SM_SL2MeasureHandler::MouseScroll(bool flag)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MouseScroll(flag);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL2MEASUREMH0013", __FILE__, __FUNCSIG__); }
}

//Handle right mouse down... Add new line/ initialise new measurement..
void SM_SL2MeasureHandler::RmouseDown(double x, double y)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL2MEASUREMH0014", __FILE__, __FUNCSIG__); }
}

//Check the conditions for right click
void SM_SL2MeasureHandler::CheckRightClickConditions()
{
	try
	{
		if(!AddNewShapeForMeasurement) return;
		if(CurrentMeasurementShCount == 0)
		{
			RC_ITER item = MAINDllOBJECT->getCurrentUCS().getShapes().getList().begin();
			item++;
			Shape* CSh = (Shape*)(*item).second;
			LastHandlerType = CurrentHandlerType;
			ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::PARALLEL_LINE);
			((LineParallel2Line*)CurrentdrawHandler)->SetShapeandClickdone(CSh, MAINDllOBJECT->getWindow(0).getUppX() * 800);
			CurrentMeasurementShCount++;
			AddNewShapeForMeasurement = false;
		}
		else if(CurrentMeasurementShCount == 1)
		{
			AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
			ParentShape3 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			CurrentMeasurementShCount++;
			ChangeHanlder_Type(LastHandlerType);
			AddAngleMeasurement = true;
			AddNewShapeForMeasurement = false;
		}
		else if(CurrentMeasurementShCount == 2)
		{
			CurrentMeasurementShCount = 0;
			init();
			InitialiseNewDistMeasure();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL2MEASUREMH0015", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void SM_SL2MeasureHandler::LmaxmouseDown()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CheckAllTheConditions();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL2MEASUREMH0016", __FILE__, __FUNCSIG__); }
}

//Check all the required conditions for the mouse down..
void SM_SL2MeasureHandler::CheckAllTheConditions()
{
	try
	{
		if(CurrentdrawHandler->LmouseDown_x(clicks[0].getX(), clicks[0].getY()))
		{
			AddNewShapeForMeasurement = true;
			if(CurrentMeasurementShCount == 1)
			{
				ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			}
			if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
			{
				delete CurrentdrawHandler;
				CurrentdrawHandler = NULL;
				if(CurrentMeasurementShCount == 2)
				{
					AddDistanceMeasurement();
					AddAngleMeasurement = false;
					FinishedCurrentDrawing = true;
				}
			}
			else
			{
				if(CurrentMeasurementShCount == 2 && AddAngleMeasurement)
				{
					if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR)
					{
						if(((ShapeWithList*)ParentShape3)->PointsList->Pointscount() > 1)
						{
							AddDistanceMeasurement();
							AddAngleMeasurement = false;
							FinishedCurrentDrawing = true;
						}
					}
					else
					{
						if(((ShapeWithList*)ParentShape3)->PointsList->Pointscount() > 1)
						{
							AddDistanceMeasurement();
							AddAngleMeasurement = false;
							FinishedCurrentDrawing = true;
						}
					}
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL2MEASUREMH0017", __FILE__, __FUNCSIG__); }
}

//Escape buttons press handling..
void SM_SL2MeasureHandler::EscapebuttonPress()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR && CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
			CurrentdrawHandler->EscapebuttonPress();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL2MEASUREMH0018", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void SM_SL2MeasureHandler::draw(int windowno, double WPixelWidth)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL2MEASUREMH0019", __FILE__, __FUNCSIG__); }
}
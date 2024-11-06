#include "StdAfx.h"
#include "..\Handlers\HandlerHeaders.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddDimAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Measurement\DimRunOutMeasurement.h"
#include "..\Shapes\Line.h"
#include "..\Helper\Helper.h"
#include "..\Handlers_SmartMeasures\SM_RunoutMeasurementHandler.h"
#include "..\Engine\SnapPointCollection.h"
#include "..\Handlers_FrameGrab\RotationScanHandler.h"

//Default constuctor..
SM_RunoutMeasurementHandler::SM_RunoutMeasurementHandler(int stepcnt, int Type)
{
	try
	{
		FirstTime = true;
		RunOutType = Type;
		StepCount= stepcnt;
		init();
		InitialiseNewDistMeasure();
		//MAINDllOBJECT->SetStatusCode("SM_StepLengthMeasurementHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_RunoutMeasurementHandler01", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
SM_RunoutMeasurementHandler::~SM_RunoutMeasurementHandler()
{
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void SM_RunoutMeasurementHandler::init()
{
	try
	{
		StepLines.clear();
		CurrentdrawHandler = NULL;
		CurrentMeasurementShCount = 0;
		AddNewShapeForMeasurement = false;
		CurrentMeasure = NULL;
		setMaxClicks(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_RunoutMeasurementHandler02", __FILE__, __FUNCSIG__); }
}

//Initialise handler for New Angle Measure..!!
void SM_RunoutMeasurementHandler::InitialiseNewDistMeasure()
{
	try
	{
		if(RunOutType == 0)
			CreateLineParelleltoToolAxis();
		AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
		if(RunOutType == 2)
		{
			if(MAINDllOBJECT->Vblock_CylinderAxisLine != NULL)
			{
				Line* CurrentLine = (Line*)MAINDllOBJECT->getShapesList().selectedItem();
				CurrentLine->LineType = RapidEnums::LINETYPE::MULTIPOINTSLINEPARALLEL2LINE;
				MAINDllOBJECT->Vblock_CylinderAxisLine->addChild(CurrentLine);
				CurrentLine->addParent(MAINDllOBJECT->Vblock_CylinderAxisLine);
			}
		}
		StepLines.push_back((Shape*)MAINDllOBJECT->getShapesList().selectedItem());
		if(FirstTime)
		{
			if(MAINDllOBJECT->getProfile())
			{
				if(MAINDllOBJECT->Continuous_ScanMode)
					ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARECT_FRAMEGRAB);
				else
					ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB);
			}
			else
				ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER);
		}
		else
			ChangeHanlder_Type(CurrentHandlerType);
		FinishedCurrentDrawing = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_RunoutMeasurementHandler03", __FILE__, __FUNCSIG__); }
}

void SM_RunoutMeasurementHandler::CreateLineParelleltoToolAxis()
{
	try
	{
		CurrentShape = new Line(_T("A_Axis"), RapidEnums::SHAPETYPE::XLINE);
		((Line*)CurrentShape)->LineType = RapidEnums::LINETYPE::PARALLEL_LINEL;
		((Line*)CurrentShape)->Length(10);
		((Line*)CurrentShape)->DefinedLength(10);
		setBaseRProperty(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ());
		MAINDllOBJECT->Vblock_CylinderAxisLine->addChild(CurrentShape);
		CurrentShape->addParent(MAINDllOBJECT->Vblock_CylinderAxisLine);
		AddShapeAction::addShape(CurrentShape);
		baseaction->CurrentWindowNo = 1;
		baseaction->DroMovementFlag = false;
		((Line*)CurrentShape)->Offset(MAINDllOBJECT->RunoutAxialRadius());
		((Line*)CurrentShape)->DefinedOffset(true);
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction);
		if(RMATH2DOBJECT->Pt2Line_Dist(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), ((Line*)CurrentShape)->Angle(), ((Line*)CurrentShape)->Intercept()) > abs(MAINDllOBJECT->RunoutAxialRadius()))
		{
			((Line*)CurrentShape)->Offset(-1 * ((Line*)CurrentShape)->Offset());
		}
		CurrentShape->UpdateBestFit();
		StepLines.push_back(CurrentShape);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SM_RunoutMeasurementHandler03a", __FILE__, __FUNCSIG__); }
}

void SM_RunoutMeasurementHandler::AddShapeMeasureRelationShip(Shape* CShape)
{
	try
	{
		if(CShape != NULL)
		{
			CShape->addMChild(CurrentMeasure);
			CurrentMeasure->addParent(CShape);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL1MEASUREMH0005", __FILE__, __FUNCSIG__); }
}

//Add New Angle measurement
void SM_RunoutMeasurementHandler::AddDistanceMeasurement()
{
	try
	{
		FirstTime = false;
		DimRunOutMeasurement* Cdim;
		if(RunOutType == 1 || RunOutType == 2 || RunOutType == 3)
		{
			Cdim = new DimRunOutMeasurement(_T("RO"));
			CurrentMeasure = Cdim;
			for(list<Shape*>::iterator It = StepLines.begin(); It != StepLines.end(); It++)
			{
				AddShapeMeasureRelationShip(*It);
				Cdim->ParentsShape.push_back(*It);
			}
			Cdim->CalculateMeasurement();
			Cdim->ContainsLinearMeasurementMode(true);
			Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
			Cdim->SetSelectionLine(&SelectionLine[0]);
			Cdim->IsValid(true);
			Cdim->UpdateMeasurement();
			AddDimAction::addDim(Cdim);

			if(RunOutType == 2 || RunOutType == 3)
			{
				if(RunOutType == 2 )
					Cdim = new DimRunOutMeasurement(_T("GCD"), RapidEnums::MEASUREMENTTYPE::DIM_GEOMETRICCUTTINGDIAMETER);
				if(RunOutType == 3)
					Cdim = new DimRunOutMeasurement(_T("MCD"), RapidEnums::MEASUREMENTTYPE::DIM_MICROMETRICCUTTINGDIAMETER);
				CurrentMeasure = Cdim;
				for(list<Shape*>::iterator It = StepLines.begin(); It != StepLines.end(); It++)
				{
					AddShapeMeasureRelationShip(*It);
					Cdim->ParentsShape.push_back(*It);
				}
				Cdim->CalculateMeasurement();
				Cdim->ContainsLinearMeasurementMode(true);
				Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
				double SelectionLine[6] = {0};
				MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
				Cdim->SetSelectionLine(&SelectionLine[0]);
				Cdim->IsValid(true);
				Cdim->UpdateMeasurement();
				AddDimAction::addDim(Cdim);





				Cdim = new DimRunOutMeasurement(_T("Dist"), RapidEnums::MEASUREMENTTYPE::DIM_DIST1);
				CurrentMeasure = Cdim;
				for(list<Shape*>::iterator It = StepLines.begin(); It != StepLines.end(); It++)
				{
					AddShapeMeasureRelationShip(*It);
					Cdim->ParentsShape.push_back(*It);
				}
				Cdim->CalculateMeasurement();
				Cdim->ContainsLinearMeasurementMode(true);
				Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
				MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
				Cdim->SetSelectionLine(&SelectionLine[0]);
				Cdim->IsValid(true);
				Cdim->UpdateMeasurement();
				AddDimAction::addDim(Cdim);




				Cdim = new DimRunOutMeasurement(_T("Dist"), RapidEnums::MEASUREMENTTYPE::DIM_DIST2);
				CurrentMeasure = Cdim;
				for(list<Shape*>::iterator It = StepLines.begin(); It != StepLines.end(); It++)
				{
					AddShapeMeasureRelationShip(*It);
					Cdim->ParentsShape.push_back(*It);
				}
				Cdim->CalculateMeasurement();
				Cdim->ContainsLinearMeasurementMode(true);
				Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
				MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
				Cdim->SetSelectionLine(&SelectionLine[0]);
				Cdim->IsValid(true);
				Cdim->UpdateMeasurement();
				AddDimAction::addDim(Cdim);
			}
			
		}
		else
		{
		   	Cdim = new DimRunOutMeasurement(_T("RO"), RapidEnums::MEASUREMENTTYPE::DIM_INTERSECTIONRUNOUTFARTHEST);
			CurrentMeasure = Cdim;
			for(list<Shape*>::iterator It = StepLines.begin(); It != StepLines.end(); It++)
			{
				AddShapeMeasureRelationShip(*It);
				Cdim->ParentsShape.push_back(*It);
			}
			Cdim->CalculateMeasurement();
			Cdim->ContainsLinearMeasurementMode(true);
			Cdim->LinearMeasurementMode(MAINDllOBJECT->LinearModeFlag());
			double SelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(MAINDllOBJECT->getCurrentWindow()).CalculateSelectionLine(SelectionLine);
			Cdim->SetSelectionLine(&SelectionLine[0]);
			Cdim->IsValid(true);
			Cdim->UpdateMeasurement();
			AddDimAction::addDim(Cdim);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL1MEASUREMH0005", __FILE__, __FUNCSIG__); }
}

//Change the current handler type..
void SM_RunoutMeasurementHandler::ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle)
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
			switch(CurrentHandlerType)
			{
				case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_CIRCLE_FRAMEGRAB:
				case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARC_FRAMEGRAB:
				case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARECT_FRAMEGRAB:
				case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_RECT_FRAMEGRAB:
				case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_FIXEDRECT_FRAMEGRAB:
				{
					((RotationScanHandler*)CurrentdrawHandler)->DrawFramGrab = true;
				}
				break;
			}
				
		}
	}
	catch(...)
	{ 
		MAINDllOBJECT->SetAndRaiseErrorMessage("SL1MEASUREMH0008", __FILE__, __FUNCSIG__);
	}
}

//Function to handle the mouse move..
void SM_RunoutMeasurementHandler::mouseMove()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->mouseMove_x(clicks[0].getX(), clicks[0].getY());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL1MEASUREMH0009", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void SM_RunoutMeasurementHandler::LmouseDown()
{	
}

//Left Mouse Up
void SM_RunoutMeasurementHandler::LmouseUp(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(MAINDllOBJECT->Vblock_CylinderAxisLine == NULL)
			setClicksDone(0);
		else
			CurrentdrawHandler->LmouseUp(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL1MEASUREMH0011", __FILE__, __FUNCSIG__); }
}

//Middle Mouse down..
void SM_RunoutMeasurementHandler::MmouseDown(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MmouseDown(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL1MEASUREMH0012", __FILE__, __FUNCSIG__); }
}

//Mouse Scroll
void SM_RunoutMeasurementHandler::MouseScroll(bool flag)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MouseScroll(flag);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL1MEASUREMH0013", __FILE__, __FUNCSIG__); }
}

//Handle right mouse down... Add new line/ initialise new measurement..
void SM_RunoutMeasurementHandler::RmouseDown(double x, double y)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL1MEASUREMH0014", __FILE__, __FUNCSIG__); }
}

//Check the conditions for right click
void SM_RunoutMeasurementHandler::CheckRightClickConditions()
{
	try
	{
		if(!AddNewShapeForMeasurement) return;
		char temp[10];
		itoa(CurrentMeasurementShCount+1, &temp[0], 10);
		std::string TempNoofpoints = (const char*)temp;
		MAINDllOBJECT->SetStatusCode("SM_RunoutMeasurementHandler01", true, TempNoofpoints);
		if(CurrentMeasurementShCount < StepCount - 1)
		{
			AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
			if(RunOutType == 2)
			{
				if(MAINDllOBJECT->Vblock_CylinderAxisLine != NULL)
				{
					Line* CurrentLine = (Line*)MAINDllOBJECT->getShapesList().selectedItem();
					CurrentLine->LineType = RapidEnums::LINETYPE::MULTIPOINTSLINEPARALLEL2LINE;
					MAINDllOBJECT->Vblock_CylinderAxisLine->addChild(CurrentLine);
					CurrentLine->addParent(MAINDllOBJECT->Vblock_CylinderAxisLine);
				}
			}
			StepLines.push_back((Shape*)MAINDllOBJECT->getShapesList().selectedItem());
			CurrentMeasurementShCount++;
			AddNewShapeForMeasurement = false;
			switch(CurrentHandlerType)
			{
				case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_CIRCLE_FRAMEGRAB:
				case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARC_FRAMEGRAB:
				case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_ARECT_FRAMEGRAB:
				case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_RECT_FRAMEGRAB:
				case RapidEnums::RAPIDHANDLERTYPE::ROTATIONSCAN_FIXEDRECT_FRAMEGRAB:
				{
					((RotationScanHandler*)CurrentdrawHandler)->UsePreviousFramGrabForScanning();
					CheckAllTheConditions(true);
				}
				break;
				default:
					ChangeHanlder_Type(CurrentHandlerType);
					break;
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL1MEASUREMH0015", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void SM_RunoutMeasurementHandler::LmaxmouseDown()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CheckAllTheConditions();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL1MEASUREMH0016", __FILE__, __FUNCSIG__); }
}

//Check all the required conditions for the mouse down..
void SM_RunoutMeasurementHandler::CheckAllTheConditions(bool dontcheckformax)
{
	try
	{
		if(dontcheckformax || CurrentdrawHandler->LmouseDown_x(clicks[0].getX(), clicks[0].getY()))
		{
			AddNewShapeForMeasurement = true;
			if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
			{
				delete CurrentdrawHandler;
				CurrentdrawHandler = NULL;
				if(CurrentMeasurementShCount == 2)
				{
					AddDistanceMeasurement();
					FinishedCurrentDrawing = true;
				}
			}
			else
			{
				if(CurrentMeasurementShCount == StepCount - 1 && StepLines.size() > 1)
				{
					list<Shape*>::iterator It = StepLines.end();
					It--;
					ShapeWithList *lastLine = (ShapeWithList*)(*It);
					if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR)
					{
						if(lastLine->PointsList->Pointscount() > 1)
						{
							AddDistanceMeasurement();
							FinishedCurrentDrawing = true;
						}
					}
					else
					{
						if(lastLine->PointsList->Pointscount() > 1)
						{
							AddDistanceMeasurement();
							FinishedCurrentDrawing = true;
						}
					}
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL1MEASUREMH0017", __FILE__, __FUNCSIG__); }
}

//Escape buttons press handling..
void SM_RunoutMeasurementHandler::EscapebuttonPress()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR && CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
			CurrentdrawHandler->EscapebuttonPress();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL1MEASUREMH0018", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void SM_RunoutMeasurementHandler::draw(int windowno, double WPixelWidth)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SL1MEASUREMH0019", __FILE__, __FUNCSIG__); }
}

void SM_RunoutMeasurementHandler::RotationStopedAddMeasure()
{
	if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
	{
		delete CurrentdrawHandler;
		CurrentdrawHandler = NULL;
		if(CurrentMeasurementShCount == StepCount - 1)
		{
			AddDistanceMeasurement();
			FinishedCurrentDrawing = true;
		}
	}
	else
	{
		if(CurrentMeasurementShCount == StepCount - 1 && StepLines.size() > 1)
		{
			list<Shape*>::iterator It = StepLines.end();
			It--;
			ShapeWithList *lastLine = (ShapeWithList*)(*It);
			if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR)
			{
				if(lastLine->PointsList->Pointscount() > 1)
				{
					AddDistanceMeasurement();
					FinishedCurrentDrawing = true;
				}
			}
			else
			{
				if(lastLine->PointsList->Pointscount() > 1)
				{
					AddDistanceMeasurement();
					FinishedCurrentDrawing = true;
				}
			}
		}
	}
}
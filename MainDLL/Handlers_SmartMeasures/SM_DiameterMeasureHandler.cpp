#include "StdAfx.h"
#include "..\Handlers\HandlerHeaders.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddDimAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Measurement\DimLineToLineDistance.h"
#include "..\Helper\Helper.h"

//Default constuctor..
SM_DiameterMeasureHandler::SM_DiameterMeasureHandler()
{
	try
	{
		FirstTime = true;
		init();
		InitialiseNewDiaMeasure();
		/*MAINDllOBJECT->SetStatusCode("DIAMEASURE0001");*/
		MAINDllOBJECT->SetStatusCode("SM_DiameterMeasureHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
SM_DiameterMeasureHandler::~SM_DiameterMeasureHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0002", __FILE__, __FUNCSIG__); }
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void SM_DiameterMeasureHandler::init()
{
	try
	{
		CurrentdrawHandler = NULL;
		AddNewShapeForMeasurement = false;
		AddDiaMeasurement = true;
		MeasurementAdded = false;
		ParentShape1 = NULL;
		CurrentMeasure = NULL;
		setMaxClicks(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0003", __FILE__, __FUNCSIG__); }
}

//Initialise handler for New Angle Measure..!!
void SM_DiameterMeasureHandler::InitialiseNewDiaMeasure()
{
	try
	{
		AddShapeAction::addShape(new Circle(_T("C"), RapidEnums::SHAPETYPE::CIRCLE));
		ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		if(FirstTime)
		{
			if (MAINDllOBJECT->getProfile())
			{
				if (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONE_SHOT ||
					MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::NORMAL_ONE_SHOT ||
					MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::ONLINESYSTEM)
					ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::CIRCLE_FRAMEGRAB);
				else
					ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB);
			}
			else
				ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::CIRCLE3P_HANDLER);
		}
		else
			ChangeHanlder_Type(CurrentHandlerType);
		FinishedCurrentDrawing = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0004", __FILE__, __FUNCSIG__); }
}

//Add New Angle measurement
void SM_DiameterMeasureHandler::AddDia_Measurement()
{
	try
	{
		FirstTime = false;
		CurrentMeasure = HELPEROBJECT->CreateRightClickDim2D(ParentShape1, RapidEnums::MEASUREMENTTYPE::DIM_DIAMETER2D, MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), 0);
		CurrentMeasure->addParent(ParentShape1);
		ParentShape1->addMChild(CurrentMeasure);
		CurrentMeasure->IsValid(false);
		CurrentMeasure->UpdateMeasurement();
		AddDimAction::addDim(CurrentMeasure);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move and update the current angle measurement..
void SM_DiameterMeasureHandler::DiaMeasurement_MouseMove(double x, double y)
{
	try
	{
		Circle* Cshape1;
		Cshape1 = (Circle*)CurrentMeasure->ParentShape1;
		double center[2] = {Cshape1->getCenter()->getX(), Cshape1->getCenter()->getY()};
		double posshift = 100 * MAINDllOBJECT->getWindow(0).getUppX();
		if(x < center[0])
		{
			if(y < center[1])
				CurrentMeasure->SetMousePosition(x - posshift, y - posshift, 0);
			else
				CurrentMeasure->SetMousePosition(x - posshift, y + posshift, 0);
		}
		else
		{
			if(y < center[1])
				CurrentMeasure->SetMousePosition(x + posshift, y - posshift, 0);
			else
				CurrentMeasure->SetMousePosition(x + posshift, y + posshift, 0);
		}
		double SelectionLine[6] = {0};
		MAINDllOBJECT->getWindow(1).GetSelectionLine(CurrentMeasure->getMousePosition()->getX(), CurrentMeasure->getMousePosition()->getY(), SelectionLine);
		CurrentMeasure->SetSelectionLine(&SelectionLine[0]);
		MAINDllOBJECT->SelectedMeasurement_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0006", __FILE__, __FUNCSIG__); }
}

//Set the Measurement Position : Flexible/ Fg
void SM_DiameterMeasureHandler::SetMeasurementPosition()
{
	try
	{
		Circle* Cshape1;
		Cshape1 = (Circle*)CurrentMeasure->ParentShape1;
		double center[2] = {Cshape1->getCenter()->getX(), Cshape1->getCenter()->getY()};
		double posshift = 100 * MAINDllOBJECT->getWindow(0).getUppX();
		CurrentMeasure->SetMousePosition(Cshape1->getCenter()->getX() + Cshape1->Radius() + posshift, Cshape1->getCenter()->getY() + posshift, 0);
		CurrentMeasure->IsValid(true);
		MAINDllOBJECT->SelectedMeasurement_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0007", __FILE__, __FUNCSIG__); }
}

//Change the current handler type..
void SM_DiameterMeasureHandler::ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle)
{
	try
	{
		if(!(CHandle == RapidEnums::RAPIDHANDLERTYPE::CIRCLE3P_HANDLER || CHandle == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR || CHandle == RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB || 
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0008", __FILE__, __FUNCSIG__); }
}

//Function to handle the mouse move..
void SM_DiameterMeasureHandler::mouseMove()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->mouseMove_x(clicks[0].getX(), clicks[0].getY());
		if(MeasurementAdded)
			DiaMeasurement_MouseMove(clicks[0].getX(), clicks[0].getY());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0009", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void SM_DiameterMeasureHandler::LmouseDown()
{	
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0010", __FILE__, __FUNCSIG__); }
}

//Handle Left Mouse Up
void SM_DiameterMeasureHandler::LmouseUp(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->LmouseUp(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0011", __FILE__, __FUNCSIG__); }
}

//Handle the Middle mouse down..
void SM_DiameterMeasureHandler::MmouseDown(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MmouseDown(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0012", __FILE__, __FUNCSIG__); }
}

//Handle mouse Scroll
void SM_DiameterMeasureHandler::MouseScroll(bool flag)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MouseScroll(flag);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0013", __FILE__, __FUNCSIG__); }
}

//Handle right mouse down... Add new line/ initialise new measurement..
void SM_DiameterMeasureHandler::RmouseDown(double x, double y)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0014", __FILE__, __FUNCSIG__); }
}

void SM_DiameterMeasureHandler::CheckRightClickConditions()
{
	try
	{
		if(!AddNewShapeForMeasurement) return;
		AddDiaMeasurement = true;
		AddNewShapeForMeasurement = false;
		init();
		InitialiseNewDiaMeasure();
		MAINDllOBJECT->SetStatusCode("DIAMEASURE0001");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0015", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void SM_DiameterMeasureHandler::LmaxmouseDown()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(MAINDllOBJECT->ProbeSmartMeasureflag)
		{
			if(MeasurementAdded)
			{
				MAINDllOBJECT->PREVIOUSHANDLE = RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER;
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PROBE_HANDLER);
				MeasurementAdded = false;
			}
			else if(AddDiaMeasurement)
			{
				if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::CIRCLE3P_HANDLER)
				{
					if(((ShapeWithList*)ParentShape1)->PointsList->Pointscount() > 0)
					{
						AddDia_Measurement();
						AddDiaMeasurement = false;
						MeasurementAdded = true;
					}
				}
			}
		}
		else if(CurrentdrawHandler->LmouseDown_x(clicks[0].getX(), clicks[0].getY()))
		{
			AddNewShapeForMeasurement = true;
			if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::CIRCLE3P_HANDLER)
			{
				delete CurrentdrawHandler;
				CurrentdrawHandler = NULL;
				FinishedCurrentDrawing = true;
				MAINDllOBJECT->SetStatusCode("DIAMEASURE0002");
			}
			else
			{
				if(AddDiaMeasurement)
				{
					if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR)
					{
						if(((ShapeWithList*)ParentShape1)->PointsList->Pointscount() > 1)
						{
							AddDia_Measurement();
							AddDiaMeasurement = false;
						}
					}
					else
					{
						if(((ShapeWithList*)ParentShape1)->PointsList->Pointscount() > 1)
						{
							AddDia_Measurement();
							AddDiaMeasurement = false;
						}
					}
				}
				if(!AddDiaMeasurement)
				{
					SetMeasurementPosition();
					FinishedCurrentDrawing = true;
					MAINDllOBJECT->SetStatusCode("DIAMEASURE0002");
				}
			}
			if(MeasurementAdded)
			{
				/*if(MAINDllOBJECT->ProbeSmartMeasureflag)
				{
					MAINDllOBJECT->PREVIOUSHANDLE = RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER;
					MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PROBE_HANDLER);
				}*/
				MeasurementAdded = false;
			}
		}
		else
		{
			if(AddDiaMeasurement)
			{
				if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::CIRCLE3P_HANDLER)
				{
					if(((ShapeWithList*)ParentShape1)->PointsList->Pointscount() > 0)
					{
						AddDia_Measurement();
						AddDiaMeasurement = false;
						MeasurementAdded = true;
					}
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0016", __FILE__, __FUNCSIG__); }
}

//Escape buttons press handling..
void SM_DiameterMeasureHandler::EscapebuttonPress()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR && CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::CIRCLE3P_HANDLER)
			CurrentdrawHandler->EscapebuttonPress();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0017", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void SM_DiameterMeasureHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(windowno == 1)
		{
			if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::CIRCLE3P_HANDLER)
				CurrentdrawHandler->draw(windowno, WPixelWidth);
		}
		else
			CurrentdrawHandler->draw(windowno, WPixelWidth);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0018", __FILE__, __FUNCSIG__); }
}
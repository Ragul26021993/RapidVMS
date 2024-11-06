#include "StdAfx.h"
#include "..\Handlers\HandlerHeaders.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddDimAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Measurement\DimLineToLineDistance.h"
#include "..\Helper\Helper.h"

//Default constuctor..
SM_RadiusMeasureHandler::SM_RadiusMeasureHandler()
{
	try
	{
		FirstTime = true;
		init();
		InitialiseNewRadiusMeasure();
		/*MAINDllOBJECT->SetStatusCode("RADMEASURE0001");*/
		MAINDllOBJECT->SetStatusCode("SM_RadiusMeasureHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
SM_RadiusMeasureHandler::~SM_RadiusMeasureHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0002", __FILE__, __FUNCSIG__); }
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void SM_RadiusMeasureHandler::init()
{
	try
	{
		CurrentdrawHandler = NULL;
		AddNewShapeForMeasurement = false;
		AddRadiusMeasurement = true;
		MeasurementAdded = false;
		ParentShape1 = NULL;
		CurrentMeasure = NULL;
		setMaxClicks(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0003", __FILE__, __FUNCSIG__); }
}

//Initialise handler for New Angle Measure..!!
void SM_RadiusMeasureHandler::InitialiseNewRadiusMeasure()
{
	try
	{
		Shape* CSh = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
		((Circle*)CSh)->CircleType = RapidEnums::CIRCLETYPE::ARC_MIDDLE;
		AddShapeAction::addShape(CSh);
		ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		if(FirstTime)
		{
			if(MAINDllOBJECT->getProfile())
				ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB);
			else
				ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER);
		}
		else
			ChangeHanlder_Type(CurrentHandlerType);
		FinishedCurrentDrawing = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0004", __FILE__, __FUNCSIG__); }
}

//Add New Angle measurement
void SM_RadiusMeasureHandler::AddRadius_Measurement()
{
	try
	{
		FirstTime = false;
		CurrentMeasure = HELPEROBJECT->CreateRadiusMeasurement(ParentShape1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move and update the current angle measurement..
void SM_RadiusMeasureHandler::RadiusMeasurement_MouseMove()
{
	try
	{
		Circle* Cshape1;
		Cshape1 = (Circle*)CurrentMeasure->ParentShape1;
		double midangle = Cshape1->Startangle() + Cshape1->Sweepangle()/2;
		double radius = Cshape1->Radius();
		double center[2] = {Cshape1->getCenter()->getX(), Cshape1->getCenter()->getY()};
		double ptonarc[2] = {center[0] + radius * cos(midangle), center[1] + radius * sin(midangle)};
		double posshift = 100 * MAINDllOBJECT->getWindow(0).getUppX();
		if(ptonarc[0] < center[0])
			CurrentMeasure->SetMousePosition(ptonarc[0] - posshift, ptonarc[1], 0);
		else
			CurrentMeasure->SetMousePosition(ptonarc[0] + posshift, ptonarc[1], 0);
		CurrentMeasure->IsValid(true);
		double SelectionLine[6] = {0};
		MAINDllOBJECT->getWindow(1).GetSelectionLine(CurrentMeasure->getMousePosition()->getX(), CurrentMeasure->getMousePosition()->getY(), SelectionLine);
		CurrentMeasure->SetSelectionLine(&SelectionLine[0]);
		MAINDllOBJECT->SelectedMeasurement_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0006", __FILE__, __FUNCSIG__); }
}

//Change the current handler type..
void SM_RadiusMeasureHandler::ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle)
{
	try
	{
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
void SM_RadiusMeasureHandler::mouseMove()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->mouseMove_x(clicks[0].getX(), clicks[0].getY());
		if(MeasurementAdded)
			RadiusMeasurement_MouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0009", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void SM_RadiusMeasureHandler::LmouseDown()
{	
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0010", __FILE__, __FUNCSIG__); }
}

//Handle Left Mouse up..
void SM_RadiusMeasureHandler::LmouseUp(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->LmouseUp(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0011", __FILE__, __FUNCSIG__); }
}

//Handle Middle mouse down..
void SM_RadiusMeasureHandler::MmouseDown(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MmouseDown(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0012", __FILE__, __FUNCSIG__); }
}

//Handle Mouse scroll.
void SM_RadiusMeasureHandler::MouseScroll(bool flag)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MouseScroll(flag);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0013", __FILE__, __FUNCSIG__); }
}

//Handle right mouse down... Add new line/ initialise new measurement..
void SM_RadiusMeasureHandler::RmouseDown(double x, double y)
{
	try
	{
		if(CurrentdrawHandler != NULL)
		{
			if(CurrentdrawHandler->getClicksDone() == 0)
				CheckRightClickConditions();
			else
			{
				CurrentdrawHandler->RmouseDown(x, y);
				if(AddRadiusMeasurement)
				{
					if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER)
					{
						if(((ShapeWithList*)ParentShape1)->PointsList->Pointscount() > 1)
						{
							AddRadius_Measurement();
							AddRadiusMeasurement = false;
							MeasurementAdded = true;
						}
					}
				}
			}
		}
		else
			CheckRightClickConditions();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0014", __FILE__, __FUNCSIG__); }
}

//Check all the Conditions for right click...
void SM_RadiusMeasureHandler::CheckRightClickConditions()
{
	try
	{
		if(!AddNewShapeForMeasurement) return;
		AddRadiusMeasurement = true;
		AddNewShapeForMeasurement = false;
		init();
		InitialiseNewRadiusMeasure();
		MAINDllOBJECT->SetStatusCode("RADMEASURE0001");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0015", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void SM_RadiusMeasureHandler::LmaxmouseDown()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(MAINDllOBJECT->ProbeSmartMeasureflag)
		{
		    if(AddRadiusMeasurement)
			{
				if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER)
				{
					if(((ShapeWithList*)ParentShape1)->PointsList->Pointscount() > 2)
					{
						AddRadius_Measurement();
						AddRadiusMeasurement = false;
						MAINDllOBJECT->PREVIOUSHANDLE = RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER;
						MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PROBE_HANDLER);
						MeasurementAdded = false;
					}
				}
			}
		}
		else if(CurrentdrawHandler->LmouseDown_x(clicks[0].getX(), clicks[0].getY()))
		{
			AddNewShapeForMeasurement = true;
			if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER)
			{
				delete CurrentdrawHandler;
				CurrentdrawHandler = NULL;
				FinishedCurrentDrawing = true;
				MAINDllOBJECT->SetStatusCode("RADMEASURE0002");
			}
			else
			{
				if(AddRadiusMeasurement)
				{
					if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR)
					{
						if(((ShapeWithList*)ParentShape1)->PointsList->Pointscount() > 2)
						{
							AddRadius_Measurement();
							AddRadiusMeasurement = false;
						}
					}
					else
					{
						if(((ShapeWithList*)ParentShape1)->PointsList->Pointscount() > 2)
						{
							AddRadius_Measurement();
							AddRadiusMeasurement = false;
						}
					}
					
				}
				if(!AddRadiusMeasurement)
				{
					HELPEROBJECT->SetRadiusMeasurementPosition(CurrentMeasure);
					FinishedCurrentDrawing = true;
					MAINDllOBJECT->SetStatusCode("RADMEASURE0002");
				}
			}
			if(MeasurementAdded)
			{
				MeasurementAdded = false;
			}
		}
		else
		{
			if(AddRadiusMeasurement)
			{
				if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER)
				{
					if(((ShapeWithList*)ParentShape1)->PointsList->Pointscount() > 1)
					{
						AddRadius_Measurement();
						AddRadiusMeasurement = false;
						MeasurementAdded = true;
					}
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0016", __FILE__, __FUNCSIG__); }
}

//Escape buttons press handling..
void SM_RadiusMeasureHandler::EscapebuttonPress()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR && CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER)
			CurrentdrawHandler->EscapebuttonPress();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0017", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void SM_RadiusMeasureHandler::draw(int windowno, double WPixelWidth)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0018", __FILE__, __FUNCSIG__); }
}
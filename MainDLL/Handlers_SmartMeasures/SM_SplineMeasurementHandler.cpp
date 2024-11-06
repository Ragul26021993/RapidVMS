#include "StdAfx.h"
#include "..\Handlers\HandlerHeaders.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddDimAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Measurement\DimLineToLineDistance.h"
#include "..\Helper\Helper.h"

//Default constuctor..
SM_SplineMeasurementHandler::SM_SplineMeasurementHandler()
{
	try
	{
		FirstTime = true;
		init();
		InitialiseNewDiaMeasure();
		/*MAINDllOBJECT->SetStatusCode("DIAMEASURE0001");*/
		MAINDllOBJECT->SetStatusCode("SM_SplineMeasurementHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
SM_SplineMeasurementHandler::~SM_SplineMeasurementHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0002", __FILE__, __FUNCSIG__); }
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void SM_SplineMeasurementHandler::init()
{
	try
	{
		CurrentdrawHandler = NULL;
		AddNewShapeForMeasurement = false;
		AddDiaMeasurement = true;
		SplineCount = 0;
		ParentShape1 = NULL;
		CurrentMeasure = NULL;
		setMaxClicks(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0003", __FILE__, __FUNCSIG__); }
}

//Initialise handler for New Angle Measure..!!
void SM_SplineMeasurementHandler::InitialiseNewDiaMeasure()
{
	try
	{
		AddShapeAction::addShape(new Circle(_T("C"), RapidEnums::SHAPETYPE::CIRCLE));
		ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		MouseHandler* Crnthandle = MAINDllOBJECT->SetCurrentSMFGHandler(RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB_ALLEDGE);
		if(Crnthandle != NULL)
		{
			if(CurrentdrawHandler != NULL)
			{
				delete CurrentdrawHandler;
				CurrentdrawHandler = NULL;
			}
			CurrentdrawHandler = Crnthandle;
		}
		ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB_ALLEDGE);
		FinishedCurrentDrawing = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0004", __FILE__, __FUNCSIG__); }
}

//Add New Angle measurement
void SM_SplineMeasurementHandler::AddDia_Measurement()
{
	try
	{
		FirstTime = false;
		CurrentMeasure = HELPEROBJECT->CreateRightClickDim2D(ParentShape1, RapidEnums::MEASUREMENTTYPE::DIM_SPLINECOUNT, MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), 0, SplineCount);
		char ch[10];
		_itoa(SplineCount, ch, 10);
		std::string temp = (const char*)(&ch[0]);
		std::string temp1 = CurrentMeasure->getModifiedName();
		CurrentMeasure->setModifiedName(temp1 + "_" + temp);
		CurrentMeasure->addParent(ParentShape1);
		ParentShape1->addMChild(CurrentMeasure);
		CurrentMeasure->IsValid(false);
		CurrentMeasure->UpdateMeasurement();
		AddDimAction::addDim(CurrentMeasure);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0005", __FILE__, __FUNCSIG__); }
}

//Set the Measurement Position : Flexible/ Fg
void SM_SplineMeasurementHandler::SetMeasurementPosition()
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
void SM_SplineMeasurementHandler::ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle)
{
	try
	{
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
void SM_SplineMeasurementHandler::mouseMove()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->mouseMove_x(clicks[0].getX(), clicks[0].getY());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0009", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void SM_SplineMeasurementHandler::LmouseDown()
{	
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0010", __FILE__, __FUNCSIG__); }
}

//Handle Left Mouse Up
void SM_SplineMeasurementHandler::LmouseUp(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->LmouseUp(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0011", __FILE__, __FUNCSIG__); }
}

//Handle the Middle mouse down..
void SM_SplineMeasurementHandler::MmouseDown(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MmouseDown(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0012", __FILE__, __FUNCSIG__); }
}

//Handle mouse Scroll
void SM_SplineMeasurementHandler::MouseScroll(bool flag)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MouseScroll(flag);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0013", __FILE__, __FUNCSIG__); }
}

//Handle right mouse down... Add new line/ initialise new measurement..
void SM_SplineMeasurementHandler::RmouseDown(double x, double y)
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

void SM_SplineMeasurementHandler::CheckRightClickConditions()
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
void SM_SplineMeasurementHandler::LmaxmouseDown()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(CurrentdrawHandler->LmouseDown_x(clicks[0].getX(), clicks[0].getY()))
		{
			AddNewShapeForMeasurement = true;
			if(AddDiaMeasurement)
			{
				if(((ShapeWithList*)ParentShape1)->PointsList->Pointscount() > 1)
				{
					AddDia_Measurement();
					AddDiaMeasurement = false;
				}
			}
			if(!AddDiaMeasurement)
			{
				SetMeasurementPosition();
				FinishedCurrentDrawing = true;
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0016", __FILE__, __FUNCSIG__); }
}

//Escape buttons press handling..
void SM_SplineMeasurementHandler::EscapebuttonPress()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->EscapebuttonPress();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0017", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void SM_SplineMeasurementHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(windowno == 0)
			CurrentdrawHandler->draw(windowno, WPixelWidth);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIAMEASUREMH0018", __FILE__, __FUNCSIG__); }
}

void SM_SplineMeasurementHandler::SetAnyData(double *data)
{
	SplineCount = (int)data[0];
}
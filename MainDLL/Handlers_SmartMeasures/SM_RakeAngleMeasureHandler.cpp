#include "StdAfx.h"
#include "..\Handlers\HandlerHeaders.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddDimAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Shapes\Line.h"
#include "..\Measurement\DimLineToLineDistance.h"
#include "..\Helper\Helper.h"

//Default constuctor..
SM_RakeAngleMeasureHandler::SM_RakeAngleMeasureHandler()
{
	try
	{
		init();
		InitialiseNewRakeAngleMeasure();
		MAINDllOBJECT->SetStatusCode("SM_RakeAngleMeasureHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAMEASUREMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
SM_RakeAngleMeasureHandler::~SM_RakeAngleMeasureHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAMEASUREMH0002", __FILE__, __FUNCSIG__); }
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void SM_RakeAngleMeasureHandler::init()
{
	try
	{
		CurrentdrawHandler = NULL;
		AngleMeasureQuad = 0;
		CurrentMeasurementShCount = 0;
		AddNewMeasurement = false;
		MeasurementAdded = false;
		ParentShape1 = NULL;
		ParentShape2 = NULL;
		CurrentMeasure = NULL;
		setMaxClicks(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAMEASUREMH0003", __FILE__, __FUNCSIG__); }
}

//Initialise handler for New Angle Measure..!!
void SM_RakeAngleMeasureHandler::InitialiseNewRakeAngleMeasure()
{
	try
	{
		ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::TANGENTIAL_CIRCLE);
		FinishedCurrentDrawing = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAMEASUREMH0004", __FILE__, __FUNCSIG__); }
}

//Add New Angle measurement
void SM_RakeAngleMeasureHandler::AddRakeAngleMeasurement()
{
	try
	{
		CurrentMeasure = HELPEROBJECT->CreateShapeToShapeDim2D(ParentShape1, ParentShape2, MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), 0);
		CurrentMeasure->addParent(ParentShape1);
		CurrentMeasure->addParent(ParentShape2);
		ParentShape1->addMChild(CurrentMeasure);
		ParentShape2->addMChild(CurrentMeasure);
		((DimLineToLineDistance*)CurrentMeasure)->AngleTypeWithAxis = DimLineToLineDistance::ANGLEWITHAXIS::DEFAULT;
		CurrentMeasure->IsValid(false);
		CurrentMeasure->UpdateMeasurement();
		AddDimAction::addDim(CurrentMeasure);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAMEASUREMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move and update the current angle measurement..
void SM_RakeAngleMeasureHandler::RakeAngleMeasurement_MouseMove()
{
	try
	{
		double midangle, radius, ptonarc[2] = {0, 0}, r_angle1, r_angle2, mid_angle;
		Line* Cshape1, *Cshape2;
		Cshape1 = (Line*)CurrentMeasure->ParentShape1;
		Cshape2 = (Line*)CurrentMeasure->ParentShape2;
		r_angle1 = RMATH2DOBJECT->ray_angle(Cshape1->getPoint1()->getX(), Cshape1->getPoint1()->getY(), Cshape1->getPoint2()->getX(), Cshape1->getPoint2()->getY());
		r_angle2 = RMATH2DOBJECT->ray_angle(Cshape2->getPoint1()->getX(), Cshape2->getPoint1()->getY(), Cshape2->getPoint2()->getX(), Cshape2->getPoint2()->getY());
	
		if(r_angle2 < r_angle1) r_angle2 += 2 * M_PI;
		mid_angle = (r_angle1 + r_angle2)/2;
		mid_angle += M_PI_2 * (AngleMeasureQuad + 1);
		if(mid_angle > 2 * M_PI) mid_angle -= 2 * M_PI;
		double langle1 = Cshape1->Angle(), langle2 =  Cshape2->Angle();
		RMATH2DOBJECT->Angle_FirstScndQuad(&langle1);
		RMATH2DOBJECT->Angle_FirstScndQuad(&langle2);
		if(abs(langle1 - langle2) > 0.00001)
			CurrentMeasure->MeasurementType =  RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEANGLE;
		else
			CurrentMeasure->MeasurementType =  RapidEnums::MEASUREMENTTYPE::DIM_LINETOLINEDISTANCE;
		if(abs(langle1 - langle2) > 0.00001)
			RMATH2DOBJECT->Line_lineintersection(Cshape1->Angle(), Cshape1->Intercept(), Cshape2->Angle(), Cshape2->Intercept(), &ptonarc[0]);
		radius = RMATH2DOBJECT->Pt2Pt_distance(ptonarc[0], ptonarc[1], (Cshape1->getPoint1()->getX() + Cshape1->getPoint2()->getX())/2, (Cshape1->getPoint1()->getY() + Cshape1->getPoint2()->getY())/2);
		CurrentMeasure->SetMousePosition(ptonarc[0] + radius * cos(mid_angle), ptonarc[1] + radius * sin(mid_angle), 0, false);
		((DimLineToLineDistance*)CurrentMeasure)->CalculateMeasurement(CurrentMeasure->ParentShape1, CurrentMeasure->ParentShape2, true);
		CurrentMeasure->IsValid(true);
		MAINDllOBJECT->SelectedMeasurement_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0006", __FILE__, __FUNCSIG__); }
}

//Change the current handler type..
void SM_RakeAngleMeasureHandler::ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAMEASUREMH0008", __FILE__, __FUNCSIG__); }
}

//Function to handle the mouse move..
void SM_RakeAngleMeasureHandler::mouseMove()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->mouseMove_x(clicks[0].getX(), clicks[0].getY());
		if(MeasurementAdded)
			RakeAngleMeasurement_MouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAMEASUREMH0009", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void SM_RakeAngleMeasureHandler::LmouseDown()
{	
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAMEASUREMH0010", __FILE__, __FUNCSIG__); }
}

//Left Mouse Up
void SM_RakeAngleMeasureHandler::LmouseUp(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->LmouseUp(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAMEASUREMH0011", __FILE__, __FUNCSIG__); }
}

//Middle Mouse down..
void SM_RakeAngleMeasureHandler::MmouseDown(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MmouseDown(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAMEASUREMH0012", __FILE__, __FUNCSIG__); }
}

//Mouse Scroll
void SM_RakeAngleMeasureHandler::MouseScroll(bool flag)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MouseScroll(flag);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAMEASUREMH0013", __FILE__, __FUNCSIG__); }
}

//Handle right mouse down... Add new line/ initialise new measurement..
void SM_RakeAngleMeasureHandler::RmouseDown(double x, double y)
{
	try
	{
		if(MeasurementAdded)
		{
			if(AngleMeasureQuad < 3) AngleMeasureQuad++;
			else AngleMeasureQuad = 0;
			RakeAngleMeasurement_MouseMove();
		}
		else
		{
			if(CurrentdrawHandler != NULL)
			{
				if(CurrentdrawHandler->getClicksDone() == 0)
					CheckRightClickConditions();
				else
					CurrentdrawHandler->RmouseDown(x, y);
			}
			else
				CheckRightClickConditions();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAMEASUREMH0014", __FILE__, __FUNCSIG__); }
}

//Check the conditions for right click
void SM_RakeAngleMeasureHandler::CheckRightClickConditions()
{
	try
	{
		if(!AddNewMeasurement) return;
		init();
		InitialiseNewRakeAngleMeasure();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAMEASUREMH0015", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void SM_RakeAngleMeasureHandler::LmaxmouseDown()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CheckAllTheConditions();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAMEASUREMH0016", __FILE__, __FUNCSIG__); }
}

//Check all the required conditions for the mouse down..
void SM_RakeAngleMeasureHandler::CheckAllTheConditions()
{
	try
	{
		if(CurrentdrawHandler->LmouseDown_x(clicks[0].getX(), clicks[0].getY()))
		{
			if(CurrentMeasurementShCount == 0)
			{
				Shape* CSh = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
				double CCenter[2];
				((Circle*)CSh)->getCenter()->FillDoublePointer(&CCenter[0]);
				AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
				ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
				ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER);
				CurrentdrawHandler->LmouseDown_x(CCenter[0], CCenter[1]);
				CurrentMeasurementShCount++;
			}
			else if(CurrentMeasurementShCount == 1)
			{
				Shape* CSh = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
				double CCenter[2];
				((Line*)CSh)->getPoint2()->FillDoublePointer(&CCenter[0]);
				AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
				ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
				ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER);
				CurrentdrawHandler->LmouseDown_x(CCenter[0], CCenter[1]);
				CurrentMeasurementShCount++;
				AddRakeAngleMeasurement();
				MeasurementAdded = true;
			}
			else if(CurrentMeasurementShCount == 2)
			{
				delete CurrentdrawHandler;
				init();
				AddNewMeasurement = true;
				FinishedCurrentDrawing = true;
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAMEASUREMH0017", __FILE__, __FUNCSIG__); }
}

//Escape buttons press handling..
void SM_RakeAngleMeasureHandler::EscapebuttonPress()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR && CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
			CurrentdrawHandler->EscapebuttonPress();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAMEASUREMH0018", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void SM_RakeAngleMeasureHandler::draw(int windowno, double WPixelWidth)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAMEASUREMH0019", __FILE__, __FUNCSIG__); }
}

void SM_RakeAngleMeasureHandler::SetAnyData(double *data)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->SetAnyData(data);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RAMEASUREMH0020", __FILE__, __FUNCSIG__); }
}
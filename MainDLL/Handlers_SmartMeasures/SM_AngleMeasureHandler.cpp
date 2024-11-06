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
SM_AngleMeasureHandler::SM_AngleMeasureHandler()
{
	try
	{
		FirstTime = true;
		init();
		InitialiseNewAngleMeasure();
		MAINDllOBJECT->SetStatusCode("SM_AngleMeasureHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
SM_AngleMeasureHandler::~SM_AngleMeasureHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0002", __FILE__, __FUNCSIG__); }
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void SM_AngleMeasureHandler::init()
{
	try
	{
		CurrentdrawHandler = NULL;
		CurrentMeasurementShCount = 0;
		AngleMeasureQuad = 0;
		PosDist = 0;
		AddNewShapeForMeasurement = false;
		AddAngleMeasurement = false;
		UserRightClicked = false;
		MeasurementAdded = false;
		ParentShape1 = NULL;
		ParentShape2 = NULL;
		CurrentMeasure = NULL;
		setMaxClicks(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0003", __FILE__, __FUNCSIG__); }
}

//Initialise handler for New Angle Measure..!!
void SM_AngleMeasureHandler::InitialiseNewAngleMeasure()
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
		MAINDllOBJECT->SetStatusCode("ANGLEMEASURE0001");
		FinishedCurrentDrawing = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0004", __FILE__, __FUNCSIG__); }
}

//Add New Angle measurement
void SM_AngleMeasureHandler::AddLine_AngleMeasurement()
{
	try
	{
		FirstTime = false;
		CurrentMeasure = HELPEROBJECT->CreateAngleMeasurment(ParentShape1, ParentShape2);
		CurrentMeasure->IsValid(false);
		MAINDllOBJECT->SelectedMeasurement_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move and update the current angle measurement..
void SM_AngleMeasureHandler::AngleMeasurement_MouseMove()
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
		radius += PosDist;
		CurrentMeasure->SetMousePosition(ptonarc[0] + radius * cos(mid_angle), ptonarc[1] + radius * sin(mid_angle), 0, false);
		((DimLineToLineDistance*)CurrentMeasure)->CalculateMeasurement(CurrentMeasure->ParentShape1, CurrentMeasure->ParentShape2, true);
		CurrentMeasure->IsValid(true);
		MAINDllOBJECT->SelectedMeasurement_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0006", __FILE__, __FUNCSIG__); }
}

//Set the measurement Position when Flexible crosshair / FG is used.
void SM_AngleMeasureHandler::SetMeasurementPosition()
{
	try
	{
		if(CurrentMeasure == NULL) return;
		HELPEROBJECT->SetAngleMeasurementPosition(CurrentMeasure, PosDist);
		CurrentMeasure->IsValid(true);
		MAINDllOBJECT->SelectedMeasurement_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0007", __FILE__, __FUNCSIG__); }
}

//Change the current handler type..
void SM_AngleMeasureHandler::ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle)
{
	try
	{
		if(!(CHandle == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER || CHandle == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR || CHandle == RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB || 
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0008", __FILE__, __FUNCSIG__); }
}

//Function to handle the mouse move..
void SM_AngleMeasureHandler::mouseMove()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->mouseMove_x(clicks[0].getX(), clicks[0].getY());
		if(MeasurementAdded)
		{
			if(UserRightClicked)
				AngleMeasurement_MouseMove();
			else
				SetMeasurementPosition();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0009", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void SM_AngleMeasureHandler::LmouseDown()
{	
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0010", __FILE__, __FUNCSIG__); }
}

//Left Mouse Up
void SM_AngleMeasureHandler::LmouseUp(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->LmouseUp(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0011", __FILE__, __FUNCSIG__); }
}

//Middle Mouse down..
void SM_AngleMeasureHandler::MmouseDown(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MmouseDown(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0012", __FILE__, __FUNCSIG__); }
}

//Mouse Scroll
void SM_AngleMeasureHandler::MouseScroll(bool flag)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(MeasurementAdded)
		{
			if(flag)
				PosDist += 0.05;
			else
				PosDist -= 0.05;
			SetMeasurementPosition();
		}
		else
		{
			CurrentdrawHandler->MouseScroll(flag);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0013", __FILE__, __FUNCSIG__); }
}

//Handle right mouse down... Add new line/ initialise new measurement..
void SM_AngleMeasureHandler::RmouseDown(double x, double y)
{
	try
	{
		if(MeasurementAdded)
		{
			UserRightClicked = true;
			if(AngleMeasureQuad < 3) AngleMeasureQuad++;
			else AngleMeasureQuad = 0;
			AngleMeasurement_MouseMove();
		}
		else
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
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0014", __FILE__, __FUNCSIG__); }
}

//Check the conditions for right click
void SM_AngleMeasureHandler::CheckRightClickConditions()
{
	try
	{
		if(!AddNewShapeForMeasurement) return;
		if(CurrentMeasurementShCount == 0)
		{
			AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
			ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			CurrentMeasurementShCount++;
			ChangeHanlder_Type(CurrentHandlerType);
			AddAngleMeasurement = true;
			AddNewShapeForMeasurement = false;
			MAINDllOBJECT->SetStatusCode("ANGLEMEASURE0003");
		}
		else if(CurrentMeasurementShCount == 1)
		{
			CurrentMeasurementShCount = 0;
			init();
			InitialiseNewAngleMeasure();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0015", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void SM_AngleMeasureHandler::LmaxmouseDown()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(MAINDllOBJECT->highlightedShape() != NULL)
		{
			Shape* Csh = MAINDllOBJECT->highlightedShape();
			if(Csh->ShapeType == RapidEnums::SHAPETYPE::LINE && Csh->getId() != ParentShape1->getId())
			{
				if(CurrentMeasurementShCount == 0)
				{
					Shape* Temp = ParentShape1;
					ParentShape1 = Csh;
					ParentShape2 = Temp;
					CurrentMeasurementShCount = 1;
					AddAngleMeasurement = true;
					AddNewShapeForMeasurement = false;
					MAINDllOBJECT->SetStatusCode("ANGLEMEASURE0005");
				}
				else
					CheckAllTheConditions();
			}
			else
				CheckAllTheConditions();
		}
		else
			CheckAllTheConditions();

	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0016", __FILE__, __FUNCSIG__); }
}

//Check all the required conditions for the mouse down..
void SM_AngleMeasureHandler::CheckAllTheConditions()
{
	try
	{
		if(CurrentdrawHandler->LmouseDown_x(clicks[0].getX(), clicks[0].getY()))
		{
			if(CurrentMeasurementShCount == 0)
			{
				int PtsCnt = ((ShapeWithList*)ParentShape1)->PointsList->Pointscount();
				if(PtsCnt > 1)
				{
					AddNewShapeForMeasurement = true;
					MAINDllOBJECT->SetStatusCode("ANGLEMEASURE0002");
				}
			}
			if(CurrentMeasurementShCount == 1)
			{
				int PtsCnt = ((ShapeWithList*)ParentShape2)->PointsList->Pointscount();
				if(PtsCnt > 1)
				{
					AddNewShapeForMeasurement = true;
					MAINDllOBJECT->SetStatusCode("ANGLEMEASURE0004");
				}
			}
			if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
			{
				delete CurrentdrawHandler;
				CurrentdrawHandler = NULL;
				if(CurrentMeasurementShCount == 1)
				{
					MAINDllOBJECT->SetStatusCode("ANGLEMEASURE0004");
					FinishedCurrentDrawing = true;
				}
			}
			else
			{
				if(CurrentMeasurementShCount == 1 && AddAngleMeasurement)
				{
					int PtsCnt = ((ShapeWithList*)ParentShape2)->PointsList->Pointscount();
					if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR)
					{
						if(PtsCnt > 1)
						{
							AddLine_AngleMeasurement();
							AddAngleMeasurement = false;
						}
					}
					else
					{
						if(PtsCnt > 1)
						{
							AddLine_AngleMeasurement();
							AddAngleMeasurement = false;
						}
					}
				}
				if(CurrentMeasurementShCount == 1 && !AddAngleMeasurement)
				{
					SetMeasurementPosition();
					MAINDllOBJECT->SetStatusCode("ANGLEMEASURE0004");
					FinishedCurrentDrawing = true;
				}
			}
			if(MeasurementAdded)
				MeasurementAdded = false;
		}
		else
		{
			if(CurrentMeasurementShCount == 1 && AddAngleMeasurement)
			{
				int PtsCnt = ((ShapeWithList*)ParentShape2)->PointsList->Pointscount();
				if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
				{
					if(PtsCnt > 0)
					{
						AddLine_AngleMeasurement();
						AddAngleMeasurement = false;
						MeasurementAdded = true;
						MAINDllOBJECT->SetStatusCode("ANGLEMEASURE0006");
					}
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0017", __FILE__, __FUNCSIG__); }
}

//Escape buttons press handling..
void SM_AngleMeasureHandler::EscapebuttonPress()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR && CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
			CurrentdrawHandler->EscapebuttonPress();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0018", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void SM_AngleMeasureHandler::draw(int windowno, double WPixelWidth)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0019", __FILE__, __FUNCSIG__); }
}
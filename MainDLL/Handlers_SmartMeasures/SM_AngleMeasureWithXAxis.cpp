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
SM_AngleMeasureWithXAxis::SM_AngleMeasureWithXAxis()
{
	try
	{
		FirstTime = true;
		init();
		FinishedCurrentDrawing = true;
		if(MAINDllOBJECT->getProfile())
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_ANGRECTFG);
		else
			MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
		/*MAINDllOBJECT->SetStatusCode("ANGLEXMEASURE0002");*/
		MAINDllOBJECT->SetStatusCode("SM_AngleMeasureWithXAxis01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGM_XMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
SM_AngleMeasureWithXAxis::~SM_AngleMeasureWithXAxis()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGM_XMH0002", __FILE__, __FUNCSIG__); }
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void SM_AngleMeasureWithXAxis::init()
{
	try
	{
		AngleMeasureQuad = 0;
		CurrentdrawHandler = NULL;
		CurrentdrawHandler = new DefaultHandler(false);
		AddNewShapeForMeasurement = false;
		SelectShapeForMeasurement = true;
		AddAngleMeasurement = true;
		MeasurementAdded = false;
		RC_ITER item = MAINDllOBJECT->getShapesList().getList().begin();
		item++;
		ParentShape1 = (Shape*)(*item).second;
		ParentShape2 = NULL;
		CurrentMeasure = NULL;
		setMaxClicks(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGM_XMH0003", __FILE__, __FUNCSIG__); }
}

//Initialise handler for New Angle Measure..!!
void SM_AngleMeasureWithXAxis::InitialiseNewAngleMeasure()
{
	try
	{
		if(FirstTime)
		{
			if(MAINDllOBJECT->getProfile())
				ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB);
			else
				ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER);
		}
		else
			ChangeHanlder_Type(CurrentHandlerType);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGM_XMH0004", __FILE__, __FUNCSIG__); }
}

//Add New Angle measurement
void SM_AngleMeasureWithXAxis::AddLine_AngleMeasurement()
{
	try
	{
		FirstTime = false;
		CurrentMeasure = HELPEROBJECT->CreateShapeToShapeDim2D(ParentShape1, ParentShape2, MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), 0);
		CurrentMeasure->addParent(ParentShape1);
		CurrentMeasure->addParent(ParentShape2);
		ParentShape1->addMChild(CurrentMeasure);
		ParentShape2->addMChild(CurrentMeasure);
		((DimLineToLineDistance*)CurrentMeasure)->AngleTypeWithAxis = DimLineToLineDistance::ANGLEWITHAXIS::ANGLE_XAXIS;
		CurrentMeasure->IsValid(false);
		CurrentMeasure->UpdateMeasurement();
		AddDimAction::addDim(CurrentMeasure);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGM_XMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move and update the current angle measurement..
void SM_AngleMeasureWithXAxis::AngleMeasurement_MouseMove()
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
			RMATH2DOBJECT->Line_lineintersection(Cshape2->Angle(), Cshape2->Intercept(), Cshape1->Angle(), Cshape2->getPoint1()->getY(), &ptonarc[0]);
		radius = RMATH2DOBJECT->Pt2Pt_distance(ptonarc[0], ptonarc[1], (Cshape2->getPoint1()->getX() + Cshape2->getPoint2()->getX())/2, (Cshape2->getPoint1()->getY() + Cshape2->getPoint2()->getY())/2);
		CurrentMeasure->SetMousePosition(ptonarc[0] + radius * cos(mid_angle), ptonarc[1] + radius * sin(mid_angle), 0, false);
		((DimLineToLineDistance*)CurrentMeasure)->CalculateMeasurement(CurrentMeasure->ParentShape1, CurrentMeasure->ParentShape2, true);
		CurrentMeasure->IsValid(true);
		MAINDllOBJECT->SelectedMeasurement_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGM_XMH0006", __FILE__, __FUNCSIG__); }
}

//Set the Measurement Position when Flexible / FG is used 
void SM_AngleMeasureWithXAxis::SetMeasurementPosition()
{
	try
	{
		if(CurrentMeasure == NULL) return;
		Line* Cshape1 = (Line*)CurrentMeasure->ParentShape2;
		double langle1 = Cshape1->Angle(), langle2 = 0, radius, r_angle1, mid_angle;
		double ptofIntersection[2], L1point1[2], L1point2[2];
		Cshape1->getMidPoint()->FillDoublePointer(&ptofIntersection[0]);
		Cshape1->getPoint1()->FillDoublePointer(&L1point1[0]); 
		Cshape1->getPoint2()->FillDoublePointer(&L1point2[0]);
		if(abs(langle1 - langle2) < 0.00001)
		{
			CurrentMeasure->SetMousePosition(ptofIntersection[0], ptofIntersection[1], 0, true);
		}
		else
		{
			r_angle1 = RMATH2DOBJECT->ray_angle(&L1point1[0], &L1point2[0]);
			if(r_angle1 < M_PI_2)
				mid_angle = r_angle1/2;
			else if(r_angle1 < M_PI)
				mid_angle = (r_angle1 + M_PI)/2;
			else if(r_angle1 < 3 * M_PI_2)
				mid_angle = (r_angle1 + M_PI)/2;
			else
				mid_angle = (r_angle1 + 2 * M_PI)/2;
			radius = RMATH2DOBJECT->Pt2Pt_distance(L1point1[0], L1point1[1], Cshape1->getMidPoint()->getX(), Cshape1->getMidPoint()->getY());
			CurrentMeasure->SetMousePosition(L1point1[0] + radius * cos(mid_angle), L1point1[1] + radius * sin(mid_angle), 0, true);
		}
		CurrentMeasure->IsValid(true);
		MAINDllOBJECT->SelectedMeasurement_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGMEASUREMH0007", __FILE__, __FUNCSIG__); }
}

//Change the current handler type..
void SM_AngleMeasureWithXAxis::ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle)
{
	try
	{
		if(!(CHandle == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER || CHandle == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR || CHandle == RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB || 
			CHandle == RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB || CHandle == RapidEnums::RAPIDHANDLERTYPE::CIRCLE_FRAMEGRAB || CHandle == RapidEnums::RAPIDHANDLERTYPE::RECT_FRAMEGRAB ||
			CHandle == RapidEnums::RAPIDHANDLERTYPE::FIXEDRECT_FRAMEGRAB))
			return;
		FirstTime = false;
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
		ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGM_XMH0008", __FILE__, __FUNCSIG__); }
}

//Function to handle the mouse move..
void SM_AngleMeasureWithXAxis::mouseMove()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->mouseMove_x(clicks[0].getX(), clicks[0].getY());
		if(MeasurementAdded)
			AngleMeasurement_MouseMove();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGM_XMH0009", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void SM_AngleMeasureWithXAxis::LmouseDown()
{	
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGM_XMH0010", __FILE__, __FUNCSIG__); }
}

//Left Mouse Up handling
void SM_AngleMeasureWithXAxis::LmouseUp(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->LmouseUp(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGM_XMH0011", __FILE__, __FUNCSIG__); }
}

//Handle Mouse scroll..
void SM_AngleMeasureWithXAxis::MouseScroll(bool flag)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MouseScroll(flag);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGM_XMH0012", __FILE__, __FUNCSIG__); }
}

//Handle right mouse down... Add new line/ initialise new measurement..
void SM_AngleMeasureWithXAxis::RmouseDown(double x, double y)
{
	try
	{
		if(MeasurementAdded)
		{
			if(AngleMeasureQuad < 3) AngleMeasureQuad++;
			else AngleMeasureQuad = 0;
			AngleMeasurement_MouseMove();
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
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGM_XMH0013", __FILE__, __FUNCSIG__); }
}

//Handle all the conditions for the right click
void SM_AngleMeasureWithXAxis::CheckRightClickConditions()
{
	try
	{
		if(!AddNewShapeForMeasurement) return;
		init();
		AddNewShapeForMeasurement = false;
		InitialiseNewAngleMeasure();
		MAINDllOBJECT->SetStatusCode("ANGLEXMEASURE0002");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGM_XMH0015", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void SM_AngleMeasureWithXAxis::LmaxmouseDown()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(SelectShapeForMeasurement)
		{
			FinishedCurrentDrawing = false;
			SelectShapeForMeasurement = false;
			if(MAINDllOBJECT->highlightedShape() != NULL)
			{
				Shape* Csh = MAINDllOBJECT->highlightedShape();
				if(Csh->ShapeType == RapidEnums::SHAPETYPE::LINE)
				{
					ParentShape2 = Csh;
					AddLine_AngleMeasurement();
					AddAngleMeasurement = false;
					MeasurementAdded = false;
					AddNewShapeForMeasurement = true;
					FinishedCurrentDrawing = true;
				}
			}
			else
			{
				AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
				ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
				InitialiseNewAngleMeasure();
				CheckAllTheConditions();
			}
		}
		else
			CheckAllTheConditions();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGM_XMH0016", __FILE__, __FUNCSIG__); }
}

//Check all the required conditions for the mouse down..
void SM_AngleMeasureWithXAxis::CheckAllTheConditions()
{
	try
	{
		if(MAINDllOBJECT->ProbeSmartMeasureflag)
		{
			if(MeasurementAdded)
			{
				MAINDllOBJECT->PREVIOUSHANDLE = RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER;
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::PROBE_HANDLER);
				MeasurementAdded = false;
			}
			else if(AddAngleMeasurement)
			{
				if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
				{
					if(((ShapeWithList*)ParentShape2)->PointsList->Pointscount() > 0)
					{
						AddLine_AngleMeasurement();
						AddAngleMeasurement = false;
						MeasurementAdded = true;
						MAINDllOBJECT->SetStatusCode("ANGLEXMEASURE0003");
					}
				}
			}
		}
		else if(CurrentdrawHandler->LmouseDown_x(clicks[0].getX(), clicks[0].getY()))
		{
			if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER || MeasurementAdded || CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::PROBE_HANDLER)
			{
				MeasurementAdded = false;
				delete CurrentdrawHandler;
				CurrentdrawHandler = NULL;
				CurrentdrawHandler = new DefaultHandler(false);
				MAINDllOBJECT->SetMousePointerType(RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS);
				AddNewShapeForMeasurement = true;
				FinishedCurrentDrawing = true;
				MAINDllOBJECT->SetStatusCode("ANGLEXMEASURE0001");
			}
			else
			{
				if(AddAngleMeasurement)
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
				if(!AddAngleMeasurement)
				{
					SetMeasurementPosition();
					AddNewShapeForMeasurement = true;
					FinishedCurrentDrawing = true;
				}
				MAINDllOBJECT->SetStatusCode("ANGLEXMEASURE0001");
			}
		}
		else
		{
			if(AddAngleMeasurement)
			{
				int PtsCnt = ((ShapeWithList*)ParentShape2)->PointsList->Pointscount();
				if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
				{
					if(PtsCnt > 0)
					{
						AddLine_AngleMeasurement();
						AddAngleMeasurement = false;
						MeasurementAdded = true;
						MAINDllOBJECT->SetStatusCode("ANGLEXMEASURE0003");
					}
				}
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGM_XMH0017", __FILE__, __FUNCSIG__); }
}

//Escape buttons press handling..
void SM_AngleMeasureWithXAxis::EscapebuttonPress()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR && CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
			CurrentdrawHandler->EscapebuttonPress();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGM_XMH0018", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void SM_AngleMeasureWithXAxis::draw(int windowno, double WPixelWidth)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ANGM_XMH0019", __FILE__, __FUNCSIG__); }
}
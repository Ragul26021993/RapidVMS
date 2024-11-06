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
SM_Chamfer_LineMeasureHandler::SM_Chamfer_LineMeasureHandler(bool InitialiseShape)
{
	try
	{
		FirstTime = true;
		init();
		if(InitialiseShape)
			InitialiseNewDistMeasure();
		MAINDllOBJECT->SetStatusCode("SM_Chamfer_LineMeasureHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
SM_Chamfer_LineMeasureHandler::~SM_Chamfer_LineMeasureHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0002", __FILE__, __FUNCSIG__); }
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void SM_Chamfer_LineMeasureHandler::init()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0003", __FILE__, __FUNCSIG__); }
}

//Initialise handler for New Angle Measure..!!
void SM_Chamfer_LineMeasureHandler::InitialiseNewDistMeasure()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0004", __FILE__, __FUNCSIG__); }
}

void SM_Chamfer_LineMeasureHandler::AddShapeMeasureRelationShip(Shape* CShape)
{
	try
	{
		if(CShape != NULL)
		{
			CShape->addMChild(CurrentMeasure);
			CurrentMeasure->addParent(CShape);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0005", __FILE__, __FUNCSIG__); }
}

//Add measurement
void SM_Chamfer_LineMeasureHandler::AddDistanceMeasurement()
{
	try
	{
		FirstTime = false;
		Vector *Vpointer1 = NULL, *Vpointer2 = NULL, *Vpointer3 = NULL;
		Vector temp;
		double IntersecPoint1[2] = {-10000, -10000}, IntersecPoint2[2] = {-10000, -10000}, IntersecPoint3[2] = {-10000, -10000};
		Line* CSh1 = (Line*)ParentShape1; Line* CSh2 = (Line*)ParentShape2; Line* CSh3 = (Line*)ParentShape3;

		RMATH2DOBJECT->Line_lineintersection(CSh1->Angle(), CSh1->Intercept(), CSh2->Angle(), CSh2->Intercept(), &IntersecPoint1[0]);
		RMATH2DOBJECT->Line_lineintersection(CSh2->Angle(), CSh2->Intercept(), CSh3->Angle(), CSh3->Intercept(), &IntersecPoint2[0]);
		RMATH2DOBJECT->Line_lineintersection(CSh1->Angle(), CSh1->Intercept(), CSh3->Angle(), CSh3->Intercept(), &IntersecPoint3[0]);

		double midpoint1[2] = {(IntersecPoint1[0] + IntersecPoint2[0])/2, (IntersecPoint1[1] + IntersecPoint2[1])/2};
		double midpoint2[2] = {(IntersecPoint2[0] + IntersecPoint3[0])/2, (IntersecPoint2[1] + IntersecPoint3[1])/2};
		double midpoint3[2] = {(IntersecPoint1[0] + IntersecPoint3[0])/2, (IntersecPoint1[1] + IntersecPoint3[1])/2};

		temp.set(IntersecPoint1[0], IntersecPoint1[1], MAINDllOBJECT->getCurrentDRO().getZ());
		Vpointer1 = MAINDllOBJECT->getVectorPointer(&temp);
		temp.set(IntersecPoint2[0], IntersecPoint2[1], MAINDllOBJECT->getCurrentDRO().getZ());
		Vpointer2 = MAINDllOBJECT->getVectorPointer(&temp);
		temp.set(IntersecPoint3[0], IntersecPoint3[1], MAINDllOBJECT->getCurrentDRO().getZ());
		Vpointer3 = MAINDllOBJECT->getVectorPointer(&temp);

		//if(Vpointer1 != NULL && Vpointer2 != NULL)
			//AddPt2PtDistanceMeasure(Vpointer1, Vpointer2, &midpoint1[0]);
		if(Vpointer2 != NULL && Vpointer3 != NULL)
			AddPt2PtDistanceMeasure(Vpointer2, Vpointer3, &midpoint2[0]);
		if(Vpointer1 != NULL && Vpointer3 != NULL)
			AddPt2PtDistanceMeasure(Vpointer1, Vpointer3, &midpoint3[0]);

		double angle1 = RMATH2DOBJECT->ray_angle(IntersecPoint3[0], IntersecPoint3[1], IntersecPoint1[0], IntersecPoint1[1]);
		double angle2 = RMATH2DOBJECT->ray_angle(IntersecPoint3[0], IntersecPoint3[1], IntersecPoint2[0], IntersecPoint2[1]);
		double angle3 = RMATH2DOBJECT->ray_angle(IntersecPoint1[0], IntersecPoint1[1], IntersecPoint2[0], IntersecPoint2[1]);
		double angle4 = RMATH2DOBJECT->ray_angle(IntersecPoint2[0], IntersecPoint2[1], IntersecPoint1[0], IntersecPoint1[1]);
		double Midangle1 = (angle1 + angle4)/2, Midangle2 = (angle2 + angle3)/2;
		double Angle1Pos[2] = {IntersecPoint1[0] + 0.5 * cos(Midangle1), IntersecPoint1[1] + 0.5 * sin(Midangle1)};
		double Angle2Pos[2] = {IntersecPoint2[0] + 0.5 * cos(Midangle2), IntersecPoint2[1] + 0.5 * sin(Midangle2)};
		AddLine2Line_AngleMeasure(ParentShape1, ParentShape2, &Angle1Pos[0]);
		AddLine2Line_AngleMeasure(ParentShape2, ParentShape3, &Angle2Pos[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0005", __FILE__, __FUNCSIG__); }
}

void SM_Chamfer_LineMeasureHandler::AddPt2PtDistanceMeasure(Vector* Pt1, Vector* Pt2, double* MousePt)
{
	try
	{
		CurrentMeasure = HELPEROBJECT->CreatePointDim2D(Pt1, Pt2, MousePt[0] + 0.2, MousePt[1] + 0.2, 0);
		AddShapeMeasureRelationShip(ParentShape1);
		AddShapeMeasureRelationShip(ParentShape2);
		AddShapeMeasureRelationShip(ParentShape3);
		CurrentMeasure->IsValid(true);
		CurrentMeasure->UpdateMeasurement();
		AddDimAction::addDim(CurrentMeasure);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0005a", __FILE__, __FUNCSIG__); }
}

void SM_Chamfer_LineMeasureHandler::AddLine2Line_AngleMeasure(Shape* CShape1, Shape* CShape2, double* MousePt)
{
	try
	{
		CurrentMeasure = HELPEROBJECT->CreateShapeToShapeDim2D(CShape1, CShape2, MousePt[0], MousePt[1], 0);
		CurrentMeasure->addParent(CShape1);
		CurrentMeasure->addParent(CShape2);
		CShape1->addMChild(CurrentMeasure);
		CShape2->addMChild(CurrentMeasure);
		((DimLineToLineDistance*)CurrentMeasure)->AngleTypeWithAxis = DimLineToLineDistance::ANGLEWITHAXIS::DEFAULT;
		CurrentMeasure->IsValid(true);
		CurrentMeasure->UpdateMeasurement();
		AddDimAction::addDim(CurrentMeasure);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0005b", __FILE__, __FUNCSIG__); }
}

//Change the current handler type..
void SM_Chamfer_LineMeasureHandler::ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0008", __FILE__, __FUNCSIG__); }
}

//Function to handle the mouse move..
void SM_Chamfer_LineMeasureHandler::mouseMove()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->mouseMove_x(clicks[0].getX(), clicks[0].getY());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0009", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void SM_Chamfer_LineMeasureHandler::LmouseDown()
{	
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0010", __FILE__, __FUNCSIG__); }
}

//Left Mouse Up
void SM_Chamfer_LineMeasureHandler::LmouseUp(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->LmouseUp(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0011", __FILE__, __FUNCSIG__); }
}

//Middle Mouse down..
void SM_Chamfer_LineMeasureHandler::MmouseDown(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MmouseDown(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0012", __FILE__, __FUNCSIG__); }
}

//Mouse Scroll
void SM_Chamfer_LineMeasureHandler::MouseScroll(bool flag)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MouseScroll(flag);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0013", __FILE__, __FUNCSIG__); }
}

//Handle right mouse down... Add new line/ initialise new measurement..
void SM_Chamfer_LineMeasureHandler::RmouseDown(double x, double y)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0014", __FILE__, __FUNCSIG__); }
}

//Check the conditions for right click
void SM_Chamfer_LineMeasureHandler::CheckRightClickConditions()
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
			AddNewShapeForMeasurement = false;
		}
		else if(CurrentMeasurementShCount == 1)
		{
			AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
			ParentShape3 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			CurrentMeasurementShCount++;
			ChangeHanlder_Type(CurrentHandlerType);
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0015", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void SM_Chamfer_LineMeasureHandler::LmaxmouseDown()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CheckAllTheConditions();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0016", __FILE__, __FUNCSIG__); }
}

//Check all the required conditions for the mouse down..
void SM_Chamfer_LineMeasureHandler::CheckAllTheConditions()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0017", __FILE__, __FUNCSIG__); }
}

//Escape buttons press handling..
void SM_Chamfer_LineMeasureHandler::EscapebuttonPress()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR && CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
			CurrentdrawHandler->EscapebuttonPress();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0018", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void SM_Chamfer_LineMeasureHandler::draw(int windowno, double WPixelWidth)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0019", __FILE__, __FUNCSIG__); }
}

void SM_Chamfer_LineMeasureHandler::call_chamfer_dist_measurement(Shape* csh1, Shape* csh2, Shape* csh3)
{
	try
	{
		this->ParentShape1 = csh1;
		this->ParentShape2 = csh2;
		this->ParentShape3 = csh3;

		this->AddDistanceMeasurement();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0020", __FILE__, __FUNCSIG__); }
}
#include "StdAfx.h"
#include "..\Handlers\HandlerHeaders.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddDimAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Shapes\Line.h"
#include "..\Measurement\DimLineToLineDistance.h"
#include "..\Helper\Helper.h"
#include "..\Engine\PartProgramFunctions.h"

//Default constuctor..
SM_Chamfer_ArcMeasureHandler::SM_Chamfer_ArcMeasureHandler(bool InitialiseShape, bool tangentialarc)
{
	try
	{
		FirstTime = true;
		this->TangentialArcConstraint = tangentialarc;
		init();
		if(InitialiseShape)
			InitialiseNewDistMeasure();
		MAINDllOBJECT->SetStatusCode("SM_Chamfer_ArcMeasureHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SARCCHMEASUREMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
SM_Chamfer_ArcMeasureHandler::~SM_Chamfer_ArcMeasureHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SARCCHMEASUREMH0002", __FILE__, __FUNCSIG__); }
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void SM_Chamfer_ArcMeasureHandler::init()
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SARCCHMEASUREMH0003", __FILE__, __FUNCSIG__); }
}

//Initialise handler for New Angle Measure..!!
void SM_Chamfer_ArcMeasureHandler::InitialiseNewDistMeasure()
{
	try
	{
		AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
		ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
		ParentShape3 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		AddShapeAction::addShape(new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC));
		ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();

		MAINDllOBJECT->selectShape(ParentShape1->getId(), false);

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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SARCCHMEASUREMH0004", __FILE__, __FUNCSIG__); }
}

void SM_Chamfer_ArcMeasureHandler::AddShapeMeasureRelationShip(Shape* CShape)
{
	try
	{
		if(CShape != NULL)
		{
			CShape->addMChild(CurrentMeasure);
			CurrentMeasure->addParent(CShape);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SARCCHMEASUREMH0005", __FILE__, __FUNCSIG__); }
}

//Add New Angle measurement
void SM_Chamfer_ArcMeasureHandler::AddDistanceMeasurement()
{
	try
	{
		FirstTime = false;
		Vector *Vpointer1 = NULL, *Vpointer2 = NULL, *Vpointer3 = NULL;
		Vector temp;
		double tempIntersec1[4] = {-10000, -10000, -10000, -10000}, tempIntersec2[4] = {-10000, -10000, -10000, -10000}, d1, d2;
		double IntersecPoint1[2], IntersecPoint2[2], IntersecPoint3[2] = {-10000, -10000};
		Line* CSh1 = (Line*)ParentShape1; Circle* CSh2 = (Circle*)ParentShape2; Line* CSh3 = (Line*)ParentShape3;
		if (this->TangentialArcConstraint)
		{
			if(!(PPCALCOBJECT->IsPartProgramRunning()))
			{
				//make ParentShape2 a child of ParentShape1 and ParentShape3
				ParentShape1->addChild(ParentShape2);
				ParentShape3->addChild(ParentShape2);
				ParentShape2->addParent(ParentShape1);
				ParentShape2->addParent(ParentShape3);
			}
			//make ParentShape2 a circle of specific type so it goes through different best fit function
			CSh2->CircleType = RapidEnums::CIRCLETYPE::ARC_TANGENT_2_LINES_CIRCLE;
			CSh2->UpdateShape();
		}

		double radius = CSh2->Radius();
		double center[2] = {CSh2->getCenter()->getX(), CSh2->getCenter()->getY()};

		RMATH2DOBJECT->Line_circleintersection(CSh1->Angle(), CSh1->Intercept(), &center[0], radius, &tempIntersec1[0]);
		RMATH2DOBJECT->Line_circleintersection(CSh3->Angle(), CSh3->Intercept(), &center[0], radius, &tempIntersec2[0]);
		RMATH2DOBJECT->Line_lineintersection(CSh1->Angle(), CSh1->Intercept(), CSh3->Angle(), CSh3->Intercept(), &IntersecPoint3[0]);

		d1 = RMATH2DOBJECT->Pt2Pt_distance(IntersecPoint3[0], IntersecPoint3[1], tempIntersec1[0], tempIntersec1[1]);
		d2 = RMATH2DOBJECT->Pt2Pt_distance(IntersecPoint3[0], IntersecPoint3[1], tempIntersec1[2], tempIntersec1[3]);
		if(d1 < d2)
		{
			IntersecPoint1[0] = tempIntersec1[0]; IntersecPoint1[1] = tempIntersec1[1];
		}
		else
		{
			IntersecPoint1[0] = tempIntersec1[2]; IntersecPoint1[1] = tempIntersec1[3];
		}

		d1 = RMATH2DOBJECT->Pt2Pt_distance(IntersecPoint3[0], IntersecPoint3[1], tempIntersec2[0], tempIntersec2[1]);
		d2 = RMATH2DOBJECT->Pt2Pt_distance(IntersecPoint3[0], IntersecPoint3[1], tempIntersec2[2], tempIntersec2[3]);
		if(d1 < d2)
		{
			IntersecPoint2[0] = tempIntersec2[0]; IntersecPoint2[1] = tempIntersec2[1];
		}
		else
		{
			IntersecPoint2[0] = tempIntersec2[2]; IntersecPoint2[1] = tempIntersec2[3];
		}

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

		double midangle = CSh2->Startangle() + CSh2->Sweepangle()/2;
		double ptonarc[2] = {center[0] + radius * cos(midangle), center[1] + radius * sin(midangle)};
		if(ptonarc[0] < center[0])
			ptonarc[0] = ptonarc[0] - 0.2;
		else
			ptonarc[0] = ptonarc[0] + 0.2;
		
		CurrentMeasure = HELPEROBJECT->CreateRightClickDim2D(ParentShape2, RapidEnums::MEASUREMENTTYPE::DIM_RADIUS2D, ptonarc[0], ptonarc[1], 0);
		CurrentMeasure->addParent(ParentShape2);
		ParentShape2->addMChild(CurrentMeasure);
		CurrentMeasure->IsValid(true);
		CurrentMeasure->UpdateMeasurement();
		AddDimAction::addDim(CurrentMeasure);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SARCCHMEASUREMH0005", __FILE__, __FUNCSIG__); }
}

void SM_Chamfer_ArcMeasureHandler::AddPt2PtDistanceMeasure(Vector* Pt1, Vector* Pt2, double* MousePt)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SARCCHMEASUREMH0005a", __FILE__, __FUNCSIG__); }
}

//Change the current handler type..
void SM_Chamfer_ArcMeasureHandler::ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle)
{
	try
	{
		if(!(CHandle == RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER || CHandle == RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER || CHandle == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR || CHandle == RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB || 
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SARCCHMEASUREMH0008", __FILE__, __FUNCSIG__); }
}

//Function to handle the mouse move..
void SM_Chamfer_ArcMeasureHandler::mouseMove()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->mouseMove_x(clicks[0].getX(), clicks[0].getY());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SARCCHMEASUREMH0009", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void SM_Chamfer_ArcMeasureHandler::LmouseDown()
{	
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SARCCHMEASUREMH0010", __FILE__, __FUNCSIG__); }
}

//Left Mouse Up
void SM_Chamfer_ArcMeasureHandler::LmouseUp(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->LmouseUp(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SARCCHMEASUREMH0011", __FILE__, __FUNCSIG__); }
}

//Middle Mouse down..
void SM_Chamfer_ArcMeasureHandler::MmouseDown(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MmouseDown(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SARCCHMEASUREMH0012", __FILE__, __FUNCSIG__); }
}

//Mouse Scroll
void SM_Chamfer_ArcMeasureHandler::MouseScroll(bool flag)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MouseScroll(flag);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SARCCHMEASUREMH0013", __FILE__, __FUNCSIG__); }
}

//Handle right mouse down... Add new line/ initialise new measurement..
void SM_Chamfer_ArcMeasureHandler::RmouseDown(double x, double y)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SARCCHMEASUREMH0014", __FILE__, __FUNCSIG__); }
}

//Check the conditions for right click
void SM_Chamfer_ArcMeasureHandler::CheckRightClickConditions()
{
	try
	{
		if(!AddNewShapeForMeasurement) return;
		if(CurrentMeasurementShCount == 0)
		{
			MAINDllOBJECT->selectShape(ParentShape2->getId(), false);
			CurrentMeasurementShCount++;
			if(MAINDllOBJECT->getProfile())
				ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::ARC_FRAMEGRAB);
			else
				ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER);
			AddNewShapeForMeasurement = false;
		}
		else if(CurrentMeasurementShCount == 1)
		{
			MAINDllOBJECT->selectShape(ParentShape3->getId(), false);
			CurrentMeasurementShCount++;
			if(MAINDllOBJECT->getProfile())
				ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::ARECT_FRAMEGRAB);
			else
				ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER);
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SARCCHMEASUREMH0015", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void SM_Chamfer_ArcMeasureHandler::LmaxmouseDown()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CheckAllTheConditions();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SARCCHMEASUREMH0016", __FILE__, __FUNCSIG__); }
}

//Check all the required conditions for the mouse down..
void SM_Chamfer_ArcMeasureHandler::CheckAllTheConditions()
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
			else if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::ARC_HANDLER)
			{
				delete CurrentdrawHandler;
				CurrentdrawHandler = NULL;
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SARCCHMEASUREMH0017", __FILE__, __FUNCSIG__); }
}

//Escape buttons press handling..
void SM_Chamfer_ArcMeasureHandler::EscapebuttonPress()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		if(CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR && CurrentHandlerType != RapidEnums::RAPIDHANDLERTYPE::LINE_HANDLER)
			CurrentdrawHandler->EscapebuttonPress();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SARCCHMEASUREMH0018", __FILE__, __FUNCSIG__); }
}

//draw Functions.. draw the flexible crosshair at mouse position..
void SM_Chamfer_ArcMeasureHandler::draw(int windowno, double WPixelWidth)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SARCCHMEASUREMH0019", __FILE__, __FUNCSIG__); }
}


void SM_Chamfer_ArcMeasureHandler::call_chamfer_dist_measurement(Shape* csh1, Shape* csh2, Shape* csh3)
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
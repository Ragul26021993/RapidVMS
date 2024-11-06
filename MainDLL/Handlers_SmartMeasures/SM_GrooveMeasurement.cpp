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
SM_GrooveMeasurement::SM_GrooveMeasurement(bool InitialiseShape)
{
	try
	{
		FirstTime = true;
		init();
		if(InitialiseShape)
			InitialiseNewDistMeasure();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor.. Release the memory here!
SM_GrooveMeasurement::~SM_GrooveMeasurement()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0002", __FILE__, __FUNCSIG__); }
}

//Initialise default settings, parameters etc.(flags, Mouse clicks required..)
void SM_GrooveMeasurement::init()
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
	    ParentShape4 = NULL;
		ParentShape5 = NULL;
		CurrentMeasure = NULL;
		setMaxClicks(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0003", __FILE__, __FUNCSIG__); }
}

//Initialise handler for New Angle Measure..!!
void SM_GrooveMeasurement::InitialiseNewDistMeasure()
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

void SM_GrooveMeasurement::AddShapeMeasureRelationShip(Shape* CShape)
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
void SM_GrooveMeasurement::AddDistanceMeasurement()
{
	try
	{
		FirstTime = false;
		Vector *Vpointer1 = NULL, *Vpointer2 = NULL, *Vpointer3 = NULL, *Vpointer4 = NULL, *Vpointer5 = NULL;
		Vector temp;
		double IntersecPoint1[2] = {-10000, -10000}, IntersecPoint2[2] = {-10000, -10000}, IntersecPoint3[2] = {-10000, -10000}, IntersecPoint4[2] = {-10000, -10000};
		Line* CSh1 = (Line*)ParentShape1; Line* CSh2 = (Line*)ParentShape2; Line* CSh3 = (Line*)ParentShape3; Line* CSh4= (Line*)ParentShape4;  Line* CSh5 = (Line*)ParentShape5; 

		RMATH2DOBJECT->Line_lineintersection(CSh1->Angle(), CSh1->Intercept(), CSh2->Angle(), CSh2->Intercept(), &IntersecPoint1[0]);
		RMATH2DOBJECT->Line_lineintersection(CSh2->Angle(), CSh2->Intercept(), CSh3->Angle(), CSh3->Intercept(), &IntersecPoint2[0]);
		RMATH2DOBJECT->Line_lineintersection(CSh3->Angle(), CSh3->Intercept(), CSh4->Angle(), CSh4->Intercept(), &IntersecPoint3[0]);
		RMATH2DOBJECT->Line_lineintersection(CSh4->Angle(), CSh4->Intercept(), CSh5->Angle(), CSh5->Intercept(), &IntersecPoint4[0]);

		double midpoint1[2] = {(IntersecPoint1[0] + IntersecPoint2[0])/2, (IntersecPoint1[1] + IntersecPoint2[1])/2};
		double midpoint2[2] = {(IntersecPoint2[0] + IntersecPoint3[0])/2, (IntersecPoint2[1] + IntersecPoint3[1])/2};
		double midpoint3[2] = {(IntersecPoint3[0] + IntersecPoint4[0])/2, (IntersecPoint3[1] + IntersecPoint4[1])/2};
		double midpoint4[2] = {(IntersecPoint1[0] + IntersecPoint4[0])/2, (IntersecPoint1[1] + IntersecPoint4[1])/2};

		temp.set(IntersecPoint1[0], IntersecPoint1[1], MAINDllOBJECT->getCurrentDRO().getZ());
		Vpointer1 = MAINDllOBJECT->getVectorPointer(&temp);
		temp.set(IntersecPoint2[0], IntersecPoint2[1], MAINDllOBJECT->getCurrentDRO().getZ());
		Vpointer2 = MAINDllOBJECT->getVectorPointer(&temp);
		temp.set(IntersecPoint3[0], IntersecPoint3[1], MAINDllOBJECT->getCurrentDRO().getZ());
		Vpointer3 = MAINDllOBJECT->getVectorPointer(&temp);
		temp.set(IntersecPoint4[0], IntersecPoint4[1], MAINDllOBJECT->getCurrentDRO().getZ());
		Vpointer4 = MAINDllOBJECT->getVectorPointer(&temp);
	/*	temp.set(midpoint4[0], midpoint4[1]);
		Vpointer5 = MAINDllOBJECT->getVectorPointer(&temp);*/
		
		if(Vpointer1 != NULL && Vpointer4 != NULL)
		    AddPt2PtDistanceMeasure(Vpointer1, Vpointer4, ParentShape1, ParentShape2, ParentShape4, ParentShape5, &midpoint4[0]);
		if(Vpointer3 != NULL && Vpointer4 != NULL)
			AddPt2PtDistanceMeasure(Vpointer1, Vpointer4, ParentShape3, ParentShape4, ParentShape5, NULL, &midpoint1[0], true);
		if(Vpointer2 != NULL && Vpointer3 != NULL)
		    AddPt2PtDistanceMeasure(Vpointer2, Vpointer3, ParentShape2, ParentShape3, ParentShape4, NULL, &midpoint2[0]);
		AddLine2Line_AngleMeasure(ParentShape2, ParentShape4);
   	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0005", __FILE__, __FUNCSIG__); }
}

void SM_GrooveMeasurement::AddPt2PtDistanceMeasure(Vector* Pt1, Vector* Pt2, Shape* CShape1, Shape* CShape2, Shape* CShape3, Shape* CShape4, double* MousePt, bool linear)
{
	try
	{   
		if(linear)
		    CurrentMeasure = HELPEROBJECT->CreatePointToShapeDim2D(Pt1, CShape1, MousePt[0] + 0.2, MousePt[1] + 0.2, 0 );
		else
		   CurrentMeasure = HELPEROBJECT->CreatePointDim2D(Pt1, Pt2, MousePt[0] + 0.2, MousePt[1] + 0.2, 0);
		CurrentMeasure->IsValid(true);
		AddShapeMeasureRelationShip(CShape1); AddShapeMeasureRelationShip(CShape3);
		AddShapeMeasureRelationShip(CShape3); AddShapeMeasureRelationShip(CShape4);
		CurrentMeasure->UpdateMeasurement();
		AddDimAction::addDim(CurrentMeasure);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0005a", __FILE__, __FUNCSIG__); }
}

void SM_GrooveMeasurement::AddLine2Line_AngleMeasure(Shape* CShape1, Shape* CShape2)
{
	try
	{
		CurrentMeasure = HELPEROBJECT->CreateShapeToShapeDim2D(CShape1, CShape2, 0, 0, 0);
		AddShapeMeasureRelationShip(CShape1);
		AddShapeMeasureRelationShip(CShape2);
		((DimLineToLineDistance*)CurrentMeasure)->AngleTypeWithAxis = DimLineToLineDistance::ANGLEWITHAXIS::DEFAULT;
		HELPEROBJECT->SetAngleMeasurementPosition(CurrentMeasure);
		CurrentMeasure->IsValid(true);
		CurrentMeasure->UpdateMeasurement();
		AddDimAction::addDim(CurrentMeasure);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0005b", __FILE__, __FUNCSIG__); }
}

//Change the current handler type..
void SM_GrooveMeasurement::ChangeHanlder_Type(RapidEnums::RAPIDHANDLERTYPE CHandle)
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
void SM_GrooveMeasurement::mouseMove()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->mouseMove_x(clicks[0].getX(), clicks[0].getY());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0009", __FILE__, __FUNCSIG__); }
}

//Left mouse down handling
void SM_GrooveMeasurement::LmouseDown()
{	
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0010", __FILE__, __FUNCSIG__); }
}

//Left Mouse Up
void SM_GrooveMeasurement::LmouseUp(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->LmouseUp(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0011", __FILE__, __FUNCSIG__); }
}

//Middle Mouse down..
void SM_GrooveMeasurement::MmouseDown(double x, double y)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MmouseDown(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0012", __FILE__, __FUNCSIG__); }
}

//Mouse Scroll
void SM_GrooveMeasurement::MouseScroll(bool flag)
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CurrentdrawHandler->MouseScroll(flag);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0013", __FILE__, __FUNCSIG__); }
}

//Handle right mouse down... Add new line/ initialise new measurement..
void SM_GrooveMeasurement::RmouseDown(double x, double y)
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
void SM_GrooveMeasurement::CheckRightClickConditions()
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
			AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
			ParentShape4 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			CurrentMeasurementShCount++;
			ChangeHanlder_Type(CurrentHandlerType);
			AddAngleMeasurement = true;
			AddNewShapeForMeasurement = false;
		}
		else if(CurrentMeasurementShCount == 3)
		{
			AddShapeAction::addShape(new Line(_T("L"), RapidEnums::SHAPETYPE::LINE));
			ParentShape5 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			CurrentMeasurementShCount++;
			ChangeHanlder_Type(CurrentHandlerType);
			AddAngleMeasurement = true;
			AddNewShapeForMeasurement = false;
		}
		else if(CurrentMeasurementShCount == 4)
		{
			CurrentMeasurementShCount = 0;
			init();
			InitialiseNewDistMeasure();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0015", __FILE__, __FUNCSIG__); }
}

//Hanlde the left mouse down.. Last click for the current handler..
void SM_GrooveMeasurement::LmaxmouseDown()
{
	try
	{
		if(CurrentdrawHandler == NULL) return;
		CheckAllTheConditions();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0016", __FILE__, __FUNCSIG__); }
}

//Check all the required conditions for the mouse down..
void SM_GrooveMeasurement::CheckAllTheConditions()
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
				if(CurrentMeasurementShCount == 4)
				{
					AddDistanceMeasurement();
					AddAngleMeasurement = false;
					FinishedCurrentDrawing = true;
				}
			}
			else
			{
				if(CurrentMeasurementShCount == 4 && AddAngleMeasurement)
				{
					if(CurrentHandlerType == RapidEnums::RAPIDHANDLERTYPE::FLEX_CROSSHAIR)
					{
						if(((ShapeWithList*)ParentShape5)->PointsList->Pointscount() > 1)
						{
							AddDistanceMeasurement();
							AddAngleMeasurement = false;
							FinishedCurrentDrawing = true;
						}
					}
					else
					{
						if(((ShapeWithList*)ParentShape5)->PointsList->Pointscount() > 1)
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
void SM_GrooveMeasurement::EscapebuttonPress()
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
void SM_GrooveMeasurement::draw(int windowno, double WPixelWidth)
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

void SM_GrooveMeasurement::call_Groove_dist_measurement(Shape* csh1, Shape* csh2, Shape* csh3,  Shape* csh4, Shape* csh5)
{
	try
	{
		this->ParentShape1 = csh1;
		this->ParentShape2 = csh2;
		this->ParentShape3 = csh3;
		this->ParentShape4 = csh4;
		this->ParentShape5 = csh5;

		this->AddDistanceMeasurement();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SLCHMEASUREMH0020", __FILE__, __FUNCSIG__); }
}
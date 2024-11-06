#include "StdAfx.h"
#include "CircleHandlers.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Default Constructor..
CircleHandlers::CircleHandlers()
{
    setMaxClicks(2);
	CurrentShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
	mycircle = (Circle*)CurrentShape;
	currentCircle = ((Circle*)mycircle)->CircleType;
	FirstClick = true;
	Circle_DefinedRadius = false;
	runningPartprogramValid = false;
}

//Constructor with circle type.Add new Circle..//
CircleHandlers::CircleHandlers(RapidEnums::CIRCLETYPE d)
{
	try
	{
		currentCircle = d;
		AddNewCircle();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIRCLEMH0001", __FILE__, __FUNCSIG__); }
}

//Constructor.. used in edit: undo, partprogram..
CircleHandlers::CircleHandlers(RapidEnums::CIRCLETYPE d, Circle* sh, bool flag)
{
	try
	{
		FirstClick = true;
		Circle_DefinedRadius = false;
		runningPartprogramValid = false;
		currentCircle = d;
		Rflag = false;
		mycircle = sh;
		switch(currentCircle)
		{
			case RapidEnums::CIRCLETYPE::TRHEEPTCIRCLE:
				setMaxClicks(3);
				break;

			case RapidEnums::CIRCLETYPE::CIRCLE_WITHCENTER:
			case RapidEnums::CIRCLETYPE::TWOPTCIRCLE:
				setMaxClicks(2);
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIRCLEMH0002", __FILE__, __FUNCSIG__); }
}

//Destructor...// Release memory..
CircleHandlers::~CircleHandlers()
{
}

//Add new circle according to the type..
void CircleHandlers::AddNewCircle()
{
	try
	{
		FirstClick = true;
		Circle_DefinedRadius = false;
		runningPartprogramValid = false;
		Rflag = false;
		switch(currentCircle)
		{
			case RapidEnums::CIRCLETYPE::TRHEEPTCIRCLE:
				setMaxClicks(3);
				mycircle = new Circle();
				mycircle->CircleType = RapidEnums::CIRCLETYPE::TWOPTCIRCLE;
				AddShapeAction::addShape(mycircle);
				break;

			case RapidEnums::CIRCLETYPE::CIRCLE_WITHCENTER:
			case RapidEnums::CIRCLETYPE::TWOPTCIRCLE:
				setMaxClicks(2);
				mycircle = new Circle();
				mycircle->CircleType = currentCircle;
				AddShapeAction::addShape(mycircle);
				break;
		}
		if(currentCircle == RapidEnums::CIRCLETYPE::CIRCLE_WITHCENTER)
		{
			MAINDllOBJECT->SetStatusCode("CircleHandlers01");
			/*MAINDllOBJECT->SetStatusCode("CIRCLE0001");*/
		}
		else
		{
			MAINDllOBJECT->SetStatusCode("CircleHandlers02");
			/*MAINDllOBJECT->SetStatusCode("CIRCLE0002");*/
		}
		CurrentShape = mycircle;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIRCLEMH0003", __FILE__, __FUNCSIG__); }
}

//Left mouse down click.. Add points to the current circle...//
void CircleHandlers::LmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone() - 1);
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				if(Circle_DefinedRadius)
					setBaseRProperty(pnt2[0], pnt2[1], pt->getZ());
		   		else
				    setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
				mycircle->ManagePoint(baseaction, mycircle->ADD_POINT);
				PPCALCOBJECT->partProgramReached();
			}
			else
				setClicksDone(getClicksDone() - 1);
			return;
		} 
		if(getClicksDone() == 1 && !FirstClick)
		{
			mycircle = new Circle();
			CurrentShape = mycircle;
			mycircle->CircleType = currentCircle;
			AddShapeAction::addShape(mycircle);
		}
		if(Circle_DefinedRadius)
			setBaseRProperty(pnt2[0], pnt2[1], pt->getZ());
		else
	    	setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		if(getClicksDone() == 2 && currentCircle == RapidEnums::CIRCLETYPE::TRHEEPTCIRCLE && !Rflag)
		{
			mycircle->ShapeThickness(ShapeThickness);
			mycircle->CircleType = RapidEnums::CIRCLETYPE::TWOPTCIRCLE;
			FirstClick = false;
			resetClicks();
			ShapeThickness = 1;
			AddPointAction::pointAction((ShapeWithList*)mycircle, baseaction);
			MAINDllOBJECT->SetStatusCode("CIRCLE0005");
			Circle_DefinedRadius = false;
			MAINDllOBJECT->DerivedShapeCallback();
			return;
		}
		if(getClicksDone() < getMaxClicks())
			AddPointAction::pointAction((ShapeWithList*)mycircle, baseaction);
		if(getClicksDone() == 1)
		{
			MAINDllOBJECT->SetStatusCode("CIRCLE0003");
			MAINDllOBJECT->DerivedShapeCallback();
		}
		if(getClicksDone() == 2 && Rflag)
			MAINDllOBJECT->SetStatusCode("CIRCLE0004");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIRCLEMH0004", __FILE__, __FUNCSIG__); }
}

//Handle Right Mouse down..
void CircleHandlers::RmouseDown(double x, double y)
{
	try
	{
		if(!PPCALCOBJECT->IsPartProgramRunning())
		{
			if(getClicksDone() == 0)
			{
				AddNewCircle();
				if(currentCircle == RapidEnums::CIRCLETYPE::CIRCLE_WITHCENTER)
					MAINDllOBJECT->SetStatusCode("CIRCLE0001");
				else
					MAINDllOBJECT->SetStatusCode("CIRCLE0002");
			}
			if(getClicksDone() == 1 && (currentCircle == RapidEnums::CIRCLETYPE::TRHEEPTCIRCLE || currentCircle == RapidEnums::CIRCLETYPE::TWOPTCIRCLE))
			{
				mycircle->CircleType = RapidEnums::CIRCLETYPE::TRHEEPTCIRCLE;
				currentCircle = RapidEnums::CIRCLETYPE::TRHEEPTCIRCLE;
				setMaxClicks(3);
				Rflag = true;
				LmouseDown_x(x, y);
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIRCLEMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move..
void CircleHandlers::mouseMove()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone());
		pnt2[0] = pt->getX(); pnt2[1] = pt->getY();
		if(MAINDllOBJECT->getCurrentWindow() == 0)
			MAINDllOBJECT->ToolSelectedMousemove(pt->getX(), pt->getY() , pt->getZ());
		double p1[6] = {getClicksValue(0).getX(), getClicksValue(0).getY(), getClicksValue(1).getX(), getClicksValue(1).getY(), getClicksValue(2).getX(), getClicksValue(2).getY()};
		if(currentCircle == RapidEnums::CIRCLETYPE::TRHEEPTCIRCLE && getClicksDone() == 1)
		{
			if(Circle_DefinedRadius)
			{
				RMATH2DOBJECT->Circle_1Pt_FixedDia(&p1[0], radius*2, &p1[2], &pnt2[0]);
				cen[0] = (p1[0] + pnt2[0])/2; cen[1] = (p1[1] + pnt2[1])/2;
			}
			else
			{
				radius = RMATH2DOBJECT->Pt2Pt_distance(p1[0], p1[1], p1[2], p1[3]);
				cen[0] = (p1[0] + p1[2])/2; cen[1] = (p1[1] + p1[3]) / 2;
				radius = radius/2;
			}
			if(!PPCALCOBJECT->IsPartProgramRunning())
			{
				MAINDllOBJECT->CurrentShapeProperties(cen[0], cen[1], radius, radius*2, 1);
				mycircle->setCenter(Vector(cen[0], cen[1], 0));
				mycircle->Radius(radius);
			}
		}
		else if(getClicksDone() == getMaxClicks() - 1)
		{
			switch(currentCircle)
			{
				case RapidEnums::CIRCLETYPE::TRHEEPTCIRCLE:
					if(Circle_DefinedRadius)
						RMATH2DOBJECT->Circle_3Pt_FixedDia(&p1[0], &p1[2], radius, &p1[4], &cen[0], &pnt2[0]);
					else
						RMATH2DOBJECT->Circle_3Pt(&p1[0], &p1[2], &p1[4], &cen[0], &radius);
					break;
				case RapidEnums::CIRCLETYPE::CIRCLE_WITHCENTER:
					if(Circle_DefinedRadius)
						RMATH2DOBJECT->Circle_1Pt_FixedDia(&p1[0], radius, &p1[2], &pnt2[0]);
					else
						radius = RMATH2DOBJECT->Pt2Pt_distance(p1[0], p1[1], p1[2], p1[3]);
					cen[0] = p1[0]; cen[1] = p1[1];
					break;
				case RapidEnums::CIRCLETYPE::TWOPTCIRCLE:
					radius = RMATH2DOBJECT->Pt2Pt_distance(p1[0], p1[1], p1[2], p1[3]);
					radius = radius/2;
					cen[0] = (p1[0] + p1[2])/2; cen[1] = (p1[1] + p1[3]) / 2;
					break;
			}
			if(!PPCALCOBJECT->IsPartProgramRunning())
			{
				MAINDllOBJECT->CurrentShapeProperties(cen[0], cen[1], radius, radius*2, 1);
				mycircle->setCenter(Vector(cen[0], cen[1], 0));
				mycircle->Radius(radius);
			}
			else
			{
				if(mycircle->getMchild().size() > 0 && MAINDllOBJECT->VideoMpointType == RapidEnums::RWINDOW_CURSORTYPE::CURSOR_CROSS)
				{
					mycircle->setCenter(Vector(cen[0], cen[1], 0));
					mycircle->Radius(radius);
					mycircle->notifyAll(ITEMSTATUS::DATACHANGED, mycircle);	
				}
			}
		}
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIRCLEMH0006", __FILE__, __FUNCSIG__); }
}

//draw the current circle..
void CircleHandlers::draw(int windowno, double WPixelWidth)
{
	try
	{
		drawFlexibleCh();
		GRAFIX->SetGraphicsLineWidth(ShapeThickness);
		if(getClicksDone() >= 1 && getClicksDone() < getMaxClicks())
			GRAFIX->drawCircle(cen[0], cen[1], radius);
		GRAFIX->SetGraphicsLineWidth(1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIRCLEMH0007", __FILE__, __FUNCSIG__); }
}

//Set the defined diameter...
void CircleHandlers::SetAnyData(double *data)
{
	try
	{
		double d = data[0];
		if(getClicksDone() == 2 && currentCircle == RapidEnums::CIRCLETYPE::TRHEEPTCIRCLE)
		{
			double minDiameter = RMATH2DOBJECT->Pt2Pt_distance(getClicksValue(0).getX(), getClicksValue(0).getY(), getClicksValue(1).getX(), getClicksValue(1).getY());
			if(d < minDiameter)
			{
				std::string unitType;
				if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
					unitType = "In";
				else if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::MM)
					unitType = "mm";
				char mindia[20];
				RMATH2DOBJECT->Double2String(minDiameter, MAINDllOBJECT->MeasurementNoOfDecimal(), &mindia[0]); 
				std::string Dia = (const char*)(mindia) + unitType;
				MAINDllOBJECT->ShowMsgBoxString("CircleTangent2TwoCircle2SetAnyData01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION, true, Dia);
				MAINDllOBJECT->DerivedShapeCallback();
				return;
			}
		}
		Circle_DefinedRadius = true;
		radius = d / 2;
		mouseMove();
		if (currentCircle == RapidEnums::CIRCLETYPE::CIRCLE_WITHCENTER)
		{
			//MAINDllOBJECT->OnLeftMouseDown_Video(400, 300);
			MAINDllOBJECT->OnLeftMouseDown_Rcad(1000, 1000);
			//this->setMaxClicks(1);
			//this->LmaxmouseDown();

		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIRCLEMH0009", __FILE__, __FUNCSIG__); }
}

//PartProgram data handling...//
void CircleHandlers::PartProgramData()
{
	try
	{
		mycircle = (Circle*)(MAINDllOBJECT->getShapesList().selectedItem());
		currentCircle = mycircle->CircleType;
		switch(currentCircle)
		{
			case RapidEnums::CIRCLETYPE::TRHEEPTCIRCLE:
				if(PPCALCOBJECT->getFrameGrab()->CurrentWindowNo == 1)
				{
					baseaction = PPCALCOBJECT->getFrameGrab();
					setBaseRProperty(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
					mycircle->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
					PPCALCOBJECT->partProgramReached();
					return;
				}
				setMaxClicks(3);
				break;
			case RapidEnums::CIRCLETYPE::CIRCLE_WITHCENTER:
			case RapidEnums::CIRCLETYPE::TWOPTCIRCLE:
				if(PPCALCOBJECT->getFrameGrab()->CurrentWindowNo == 1)
				{
					baseaction = PPCALCOBJECT->getFrameGrab();
					setBaseRProperty(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
					mycircle->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
					PPCALCOBJECT->partProgramReached();
					return;
				}
				setMaxClicks(2);
				break;
		}
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIRCLEMH0010", __FILE__, __FUNCSIG__); }
}

//Hanlde escape button press.. Delete the last circle Reset default settings..
void CircleHandlers::EscapebuttonPress()
{
	try
	{
		if(getClicksDone() > 0)
		{
			FirstClick = false;
			MAINDllOBJECT->deleteShape();// deleteForEscape();
			Rflag = false;
			Circle_DefinedRadius = false;
		}
		MAINDllOBJECT->SetStatusCode("CIRCLE0006");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIRCLEMH0011", __FILE__, __FUNCSIG__); }
}

//Handle Left Mouse down.. Max mouse down..
void CircleHandlers::LmaxmouseDown()
{
	try
	{
		FirstClick = false;
		mycircle->ShapeThickness(ShapeThickness);
		Rflag = false;
		Vector* pt = &getClicksValue(getMaxClicks() - 1);
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				if(Circle_DefinedRadius)
					setBaseRProperty(pnt2[0], pnt2[1], pt->getZ());
				else
			    	setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
				mycircle->ManagePoint(baseaction, mycircle->ADD_POINT);
				PPCALCOBJECT->partProgramReached();
				Circle_DefinedRadius = false;
				resetClicks();
			}
			else
				setClicksDone(getMaxClicks() - 1);
			return;
		}
		else
		{
			if(Circle_DefinedRadius)
			{
				setBaseRProperty(pnt2[0], pnt2[1], pt->getZ());
			}
			else
				setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
			AddPointAction::pointAction((ShapeWithList*)mycircle, baseaction);
		}
		MAINDllOBJECT->SetStatusCode("CIRCLE0005");
		Circle_DefinedRadius = false;
		resetClicks();
		MAINDllOBJECT->DerivedShapeCallback();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIRCLEMH0012", __FILE__, __FUNCSIG__); }
}
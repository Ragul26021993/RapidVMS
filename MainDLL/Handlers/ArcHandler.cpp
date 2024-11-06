#include "StdAfx.h"
#include "ArcHandler.h"
#include "..\Shapes\Circle.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Default Constructor..//
ArcHandler::ArcHandler()
{
	try
	{
		init();
		valid = false;
		CurrentShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		myarc = (Circle*)CurrentShape;
		ARCtype = ((Circle*)myarc)->CircleType;
		FirstClick = true;
		/*MAINDllOBJECT->SetStatusCode("ARC0001");*/
		MAINDllOBJECT->SetStatusCode("ArcHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ARCMH0001", __FILE__, __FUNCSIG__); }
}

//Constrcutor.. Takes Arc type.. i.e. middle point/ end point..
ArcHandler::ArcHandler(RapidEnums::CIRCLETYPE d)
{
	try
	{
		ARCtype = d;
		init();
		valid = false;
		AddNewArc();
		/*MAINDllOBJECT->SetStatusCode("ARC0001");*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ARCMH0002", __FILE__, __FUNCSIG__); }
}

//Constructor.. Used in undo.., pp .. Set already existing shape to edit..
ArcHandler::ArcHandler(RapidEnums::CIRCLETYPE d, Circle* sh, bool flag)
{
	try
	{
		ARCtype = d;
		myarc = sh;
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ARCMH0003", __FILE__, __FUNCSIG__); }
}

//Destructor...// Release Memory..
ArcHandler::~ArcHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ARCMH0004", __FILE__, __FUNCSIG__); }
}

//Initiallise Default Settings.. Flags, Mouse clicks required etc..
void ArcHandler::init()
{
	try
	{
		FirstClick = true;
		RightClickFlag = false;
		runningPartprogramValid = false;
		valid = true;
		setMaxClicks(3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ARCMH0005", __FILE__, __FUNCSIG__); }
}

//Add New Arc shape..
void ArcHandler::AddNewArc()
{
	try
	{
		myarc = new Circle(_T("A"), RapidEnums::SHAPETYPE::ARC);
		myarc->CircleType = ARCtype;
		AddShapeAction::addShape(myarc);
		setMaxClicks(3);
		CurrentShape = myarc;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ARCMH0006", __FILE__, __FUNCSIG__); }
}

//Handle the left mouse down..
void ArcHandler::LmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone() - 1);
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
				myarc->ManagePoint(baseaction, myarc->ADD_POINT);
				if(getClicksDone() == 1)
				{
					center[0] = pt->getX(); center[1] = pt->getY(); 
				}
				if(!PPCALCOBJECT->ProgramAutoIncrementMode())
					PPCALCOBJECT->partProgramReached();
			}
			else
				setClicksDone(getClicksDone() - 1);
			return;
		}
		if(getClicksDone() == 1 && !FirstClick)
			AddNewArc();
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		if(getClicksDone() == 2)
		{
			if(RightClickFlag)
				ARCtype = RapidEnums::CIRCLETYPE::ARC_ENDPOINT;
			else
				ARCtype = RapidEnums::CIRCLETYPE::ARC_MIDDLE;
			myarc->CircleType = ARCtype;
			valid = false;
			radius = 0; startangle = 0; sweepangle = 0;
		} 		
		if(getClicksDone() < getMaxClicks())
			AddPointAction::pointAction((ShapeWithList*)myarc, baseaction);
		//Update Status..
		if(getClicksDone() == 1)
			MAINDllOBJECT->SetStatusCode("ARC0002");
		else
		{
			if(ARCtype == RapidEnums::CIRCLETYPE::ARC_ENDPOINT)
				MAINDllOBJECT->SetStatusCode("ARC0003");
			else
				MAINDllOBJECT->SetStatusCode("ARC0004");
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ARCMH0007", __FILE__, __FUNCSIG__); }
}

//Handle the right mouse down..
void ArcHandler::RmouseDown(double x, double y)
{
	try
	{
		if(!PPCALCOBJECT->IsPartProgramRunning())
		{
			MAINDllOBJECT->dontUpdateGraphcis = true;
			if(getClicksDone() == 0)
			{
				AddNewArc();
				MAINDllOBJECT->SetStatusCode("ARC0001");
			}
			valid = false;
			radius = 0; startangle = 0; sweepangle = 0;
			if(getClicksDone() == 1 && (ARCtype == RapidEnums::CIRCLETYPE::ARC_ENDPOINT || ARCtype == RapidEnums::CIRCLETYPE::ARC_MIDDLE))
			{
				RightClickFlag = true;
				this->LmouseDown_x(x, y);
				RightClickFlag = false;
			}
			MAINDllOBJECT->dontUpdateGraphcis = false;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ARCMH0008", __FILE__, __FUNCSIG__); MAINDllOBJECT->dontUpdateGraphcis = false; }	
}

//Handle mouse move
void ArcHandler::mouseMove()
{
	try
	{
		if(MAINDllOBJECT->getCurrentWindow() == 0)
			MAINDllOBJECT->ToolSelectedMousemove(getClicksValue(getClicksDone()).getX(), getClicksValue(getClicksDone()).getY(), getClicksValue(getClicksDone()).getZ());
		if(getClicksDone() == 1)
			valid = true;
		if(getClicksDone() == 2)
		{
			if(getClicksValue(1).operator== (getClicksValue(2))) return;
			double tp[] = {getClicksValue(0).getX(), getClicksValue(0).getY(), getClicksValue(1).getX(), getClicksValue(1).getY(), getClicksValue(2).getX(), getClicksValue(2).getY()};
			switch(ARCtype)
			{
				case RapidEnums::CIRCLETYPE::ARC_ENDPOINT:
					RMATH2DOBJECT->Arc_3Pt(&tp[0], &tp[4], &tp[2], &center[0], &radius, &startangle, &sweepangle);
					break;
				case RapidEnums::CIRCLETYPE::ARC_MIDDLE:
					RMATH2DOBJECT->Arc_3Pt(&tp[0], &tp[2], &tp[4], &center[0], &radius, &startangle, &sweepangle);
					break;
			}	
			if(!PPCALCOBJECT->IsPartProgramRunning())
			{
				MAINDllOBJECT->CurrentShapeProperties(center[0], center[1], radius, sweepangle, 4);
				myarc->setCenter(Vector(center[0], center[1], 0));
				myarc->Radius(radius);
				myarc->Startangle(startangle);
				myarc->Sweepangle(sweepangle);
			}
			valid = true;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ARCMH0009", __FILE__, __FUNCSIG__); }
}

//Handle draw.. draw the current arc and Selected Points..
void ArcHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		drawFlexibleCh();
		if(valid)
		{
			GRAFIX->SetGraphicsLineWidth(ShapeThickness);
			if(getClicksDone() == 1)
				GRAFIX->drawLine(getClicksValue(0).getX(), getClicksValue(0).getY(), getClicksValue(1).getX(), getClicksValue(1).getY());
			else if(getClicksDone() > 1 && getClicksDone() < getMaxClicks())
				GRAFIX->drawArc(center[0], center[1], radius, startangle, sweepangle, WPixelWidth);
			GRAFIX->SetGraphicsLineWidth(1);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ARCMH0010", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data... Run.//
void ArcHandler::PartProgramData()
{
	try
	{
		myarc = (Circle*)(MAINDllOBJECT->getShapesList().selectedItem());
		ARCtype = myarc->CircleType;
		switch(ARCtype)
		{
		case RapidEnums::CIRCLETYPE::ARC_MIDDLE:
		case RapidEnums::CIRCLETYPE::ARC_ENDPOINT:
			if(PPCALCOBJECT->getFrameGrab()->CurrentWindowNo == 1)
			{
				baseaction = PPCALCOBJECT->getFrameGrab();
				setBaseRProperty(baseaction->Pointer_SnapPt->getX(), baseaction->Pointer_SnapPt->getY(), baseaction->Pointer_SnapPt->getZ());
				myarc->ManagePoint(baseaction, ShapeWithList::ADD_POINT);
				PPCALCOBJECT->partProgramReached();
				return;
			}
			setMaxClicks(3);
			break;
		}
		runningPartprogramValid = true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ARCMH0011", __FILE__, __FUNCSIG__); }
}

//Hanlde the Left mouse down(Max mouse down...)
void ArcHandler::LmaxmouseDown()
{
	try
	{
		myarc->ShapeThickness(ShapeThickness);
		FirstClick = false;
		Vector* pt = &getClicksValue(getMaxClicks() - 1);
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
				myarc->ManagePoint(baseaction, myarc->ADD_POINT);
				ARCtype = RapidEnums::CIRCLETYPE::ARC_MIDDLE;
				for(int i = 0; i < 5; i++)
					this->clicks[i].set(0, 0, 0);
				valid = false;
				PPCALCOBJECT->partProgramReached();
			}
			else
				setClicksDone(getMaxClicks() - 1);
			return;			
		}
		else
		{
			setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
			AddPointAction::pointAction((ShapeWithList*)myarc, baseaction);
		}
		MAINDllOBJECT->SetStatusCode("ARC0005");
		ARCtype = RapidEnums::CIRCLETYPE::ARC_MIDDLE;
		for(int i = 0; i < 5; i++)
			this->clicks[i].set(0, 0, 0);
		valid = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ARCMH0012", __FILE__, __FUNCSIG__); }
}

//Handle the Escape button Press.. delete the current drawing..reset the modes..
void ArcHandler::EscapebuttonPress()
{
	try
	{
		valid = false;
		if(getClicksDone() > 0)
		{
			FirstClick = false;
			MAINDllOBJECT->deleteShape();// deleteForEscape();
			ARCtype = RapidEnums::CIRCLETYPE::ARC_MIDDLE;
			for(int i = 0; i < 5; i++)
				this->clicks[i].set(0, 0, 0);
		}
		MAINDllOBJECT->SetStatusCode("ARC0006");	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ARCMH0013", __FILE__, __FUNCSIG__); }
}
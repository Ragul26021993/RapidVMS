#include "StdAfx.h"
#include "..\Engine\RCadApp.h"
#include "GroupShapeHandler.h"
#include "..\Shapes\ShapeWithList.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\TwoLine.h"
#include "..\Shapes\TwoArc.h"
#include "..\Shapes\Vector.h"
#include "..\Measurement\DimLineToLineDistance.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Engine\PartProgramFunctions.h"

//Constructor...//
//According to the line type, set the clicks required and set the required flags....//
GroupShapeHandler::GroupShapeHandler(RapidEnums::SHAPETYPE stype)
{
	try
	{
		FirstClick = true;
		runningPartprogramValid = false;
		AddNewGroupShape(stype);
		setMaxClicks(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0001", __FILE__, __FUNCSIG__); }
}

GroupShapeHandler::GroupShapeHandler(ShapeWithList* sh, bool flag)
{
	try
	{
		myshape = sh;
		runningPartprogramValid = false;
		setMaxClicks(2);
		valid = flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0001", __FILE__, __FUNCSIG__); }
}

//Used for partprogram...//
GroupShapeHandler::GroupShapeHandler(Shape *sh, RapidEnums::SHAPETYPE stype)
{
	try
	{
		ShapeThickness = 1;
		runningPartprogramValid = false;
		myshape = (ShapeWithList*)sh;
		setMaxClicks(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0003", __FILE__, __FUNCSIG__); }
}

//Desructor/..////
GroupShapeHandler::~GroupShapeHandler()
{ 
}


//Handle the mousemove...//
void GroupShapeHandler::mouseMove()
{
	try
	{
		//if(getClicksDone() == 0)return;
		double intercept, length;
		Vector* pt = &getClicksValue(getClicksDone());
		if(MAINDllOBJECT->getCurrentWindow() == 0)
			MAINDllOBJECT->ToolSelectedMousemove(pt->getX(), pt->getY(), pt->getZ());
		
   }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0005", __FILE__, __FUNCSIG__); }
}

//Handle the mousedown event..//
void GroupShapeHandler::LmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone() - 1);
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			if(runningPartprogramValid)
			{
				runningPartprogramValid = false;
				baseaction = PPCALCOBJECT->getFrameGrab();
				baseaction->AllPointsList.Addpoint(new SinglePoint(pt->getX(), pt->getY(), pt->getZ(), pt->getR(), pt->getP()));
				myshape->ManagePoint(baseaction, myshape->ADD_POINT);
				if(!PPCALCOBJECT->ProgramAutoIncrementMode())
					PPCALCOBJECT->partProgramReached();
			}
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(MAINDllOBJECT->getCurrentWindow() == 1 && !FirstClick)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(MAINDllOBJECT->getCurrentWindow() == 1 && MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(FirstClick)
		{
			FirstClick =false;
			if( MAINDllOBJECT->highlightedShape() != NULL && MAINDllOBJECT->highlightedShape()->Normalshape() && (MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE3D))
			{
				if(myshape->ShapeType == RapidEnums::SHAPETYPE::TWOARC)
				{
					((TwoArc*)myshape)->ReflectionLine = (Line*)MAINDllOBJECT->highlightedShape();
					((TwoArc*)myshape)->AddReflectionLine = false;
	     		}
				else 
				{
					((TwoLine*)myshape)->ReflectionLine = (Line*)MAINDllOBJECT->highlightedShape();
				    ((TwoLine*)myshape)->AddReflectionLine = false;
				}
				setClicksDone(getClicksDone() - 1);
				return;
			}
			else if(MAINDllOBJECT->getCurrentWindow() != 1)
			{
				if(myshape->ShapeType == RapidEnums::SHAPETYPE::TWOARC)
				{
					((TwoArc*)myshape)->ReflectionLine = new Line(_T("L"), RapidEnums::SHAPETYPE::LINE);
					((TwoArc*)myshape)->ReflectionLine->LineType = RapidEnums::LINETYPE::LINEINTWOARC;
				}
				else
				{
					((TwoLine*)myshape)->ReflectionLine = new Line(_T("L"), RapidEnums::SHAPETYPE::LINE);
					((TwoLine*)myshape)->ReflectionLine->LineType = RapidEnums::LINETYPE::LINEINTWOLINE;
				}
			}
		}
		setBaseRProperty(pt->getX(), pt->getY(), pt->getZ());
		if(getClicksDone() < getMaxClicks())
			AddPointAction::pointAction((ShapeWithList*)myshape, baseaction);
		setClicksDone(getClicksDone() - 1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0006", __FILE__, __FUNCSIG__); }
}

void GroupShapeHandler::RmouseDown(double x,double y)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0007", __FILE__, __FUNCSIG__); }
}

void GroupShapeHandler::AddNewGroupShape(RapidEnums::SHAPETYPE stype)
{
	try
	{
		switch(stype)
		{
		  case RapidEnums::SHAPETYPE::TWOARC:
			   myshape = new TwoArc(_T("TAR"));
			   break;
		  case RapidEnums::SHAPETYPE::TWOLINE:
			   myshape = new TwoLine(_T("TL"));
			   break;
		}
		myshape->ShapeType = stype;
		AddShapeAction::addShape(myshape);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0007", __FILE__, __FUNCSIG__); }
}

void GroupShapeHandler::EscapebuttonPress()
{
	try
	{
		valid = false;
		FirstClick = false;
		if(getClicksDone() > 0)
		{
			MAINDllOBJECT->deleteShape();// deleteForEscape();
		}
		MAINDllOBJECT->SetStatusCode("L001");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0010", __FILE__, __FUNCSIG__); }
}

void GroupShapeHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		drawFlexibleCh();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0008", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data...//
void GroupShapeHandler::PartProgramData()
{
	try
	{
		myshape = (ShapeWithList*)(MAINDllOBJECT->getShapesList().selectedItem());
		setMaxClicks(2);
		runningPartprogramValid = true;
	}	
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0010", __FILE__, __FUNCSIG__); }
}


//Handle the Maximum mouse click...//
void GroupShapeHandler::LmaxmouseDown()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LHMH0011", __FILE__, __FUNCSIG__); }
}
#include "StdAfx.h"
#include "MoveShapeHandler.h"
#include "..\Shapes\ShapeWithList.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\MoveShapesAction.h"
#include "..\Helper\Helper.h"
#include "..\Engine\PartProgramFunctions.h"
#include "DXFDeviationHandler.h"

//Constructor...//
MoveShapeHandler::MoveShapeHandler()
{
	try
	{
		moving = false;		
		setMaxClicks(3);
		stepsize = 0.01;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MSMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor..//
MoveShapeHandler::~MoveShapeHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MSMH0002", __FILE__, __FUNCSIG__); }
}

//Handle the mouse down event...//
void MoveShapeHandler::LmouseDown( )
{
	try
	{
		if(MShapeList.size() == 0) return;
		if(MAINDllOBJECT->RotateDXFflag)
		{
			if(MAINDllOBJECT->highlightedPoint() != NULL)
				v.set(*MAINDllOBJECT->highlightedPoint());
			else
				v.set(getClicksValue(getClicksDone() - 1));
		}
		else
	   		moving = !moving;
		if(moving)
		{
			v.set(getClicksValue(getClicksDone() - 1));
			totalmovement.set(0,0,0);
		}	
		if(getClicksDone() == 2)
		{			 
			 if(PPCALCOBJECT->getPPArrangedActionlist().count() > 1)
			 {
				HELPEROBJECT->moveShapeCollection(MShapeList, tt, !MAINDllOBJECT->RotateDXFflag);
				UpdateDxfDeviation();
			 }
			else
				MoveShapesAction::moveShapes(totalmovement, MShapeList, false);
			 resetClicks();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MSMH0003", __FILE__, __FUNCSIG__); }
}

void MoveShapeHandler::mouseMove()
{
	try
	{
		if(moving)
		{ 
			double x = getClicksValue(getClicksDone()).getX() - v.getX(), y = getClicksValue(getClicksDone()).getY() - v.getY();
			double x1 = totalmovement.getX(), y1 = totalmovement.getY();
			totalmovement.set(x + x1, y + y1);
			tt.set(x, y);
			HELPEROBJECT->moveShapeCollection(MShapeList, tt,true);
			MAINDllOBJECT->SelectedShape_Updated();
			MAINDllOBJECT->DXFShape_Updated();
			v.set(getClicksValue(getClicksDone()));
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MSMH0004", __FILE__, __FUNCSIG__); }
}

void MoveShapeHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MSMH0005", __FILE__, __FUNCSIG__); }
}

void MoveShapeHandler::keyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	try
	{
		//if (moving) return;	//mouse is already being used to nudge, so no effect on keys
	  	int code = nChar;
		Vector tt;
		double shift = 0;
		double slope = 0;
		bool canExecute = false;
		switch(code)
		{
		case 23:	//left
			tt.set(-stepsize, 0);
			canExecute = true;
			shift = -stepsize;
			break;
		case 24:	//up
			tt.set(0, stepsize);
			shift = stepsize;
			canExecute = true;
			break;
		case 25:	//right
			tt.set(stepsize, 0);
			shift = stepsize;
			canExecute = true;
			break;
		case 26:	//down
			tt.set(0, -stepsize);
			shift = -stepsize;
			canExecute = true;
			break;
		}
		if(canExecute)
		{
			if(MAINDllOBJECT->RotateDXFflag)
			{
				slope = ((shift/60)/180) * M_PI;
				tt = v;
			}
			if(PPCALCOBJECT->getPPArrangedActionlist().count() > 1)
			{
				HELPEROBJECT->moveShapeCollection(MShapeList, tt, !MAINDllOBJECT->RotateDXFflag, slope);
				UpdateDxfDeviation();
			}
			else
				MoveShapesAction::moveShapes(tt, MShapeList, true, !MAINDllOBJECT->RotateDXFflag, slope);
		}
		MAINDllOBJECT->UpdateShapesChanged();
		MAINDllOBJECT->DXFShape_Updated();
		//MessageBox(NULL, L"Called KeyDown", L"Rapid-I", MB_TOPMOST);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MSMH0006", __FILE__, __FUNCSIG__); }
}

void MoveShapeHandler::LmouseUp(double x,double y)
{
	try
	{
		if(getClicksDone() == 1)
			this->LmouseDown_x(x, y);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MSMH0007", __FILE__, __FUNCSIG__); }
}

void MoveShapeHandler::SetAnyData(double *data)
{
	if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
		stepsize = data[0]/25.4;
	if(!MAINDllOBJECT->RotateDXFflag)
		stepsize = data[0]/1000;
	else 
		stepsize = data[0];
}

void MoveShapeHandler::EscapebuttonPress()
{
}

void MoveShapeHandler::setRCollectionBase(std::list<Shape*> coll)
{
	MShapeList = coll;
}

void MoveShapeHandler::LmaxmouseDown()
{
}

//Handle the partprogram data.....//
void MoveShapeHandler::PartProgramData()
{
	try
	{
		std::list<Shape*> ShapeColl;
		ShapeColl = ((MoveShapesAction*)PPCALCOBJECT->getCurrentAction())->getShape();
		std::list<int> ShapeIdList;
		for each(Shape* Cshape in ShapeColl)
			ShapeIdList.push_back(Cshape->getId());
		MAINDllOBJECT->selectShape(&ShapeIdList);
		MAINDllOBJECT->DerivedShapeCallback();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MSMH0008", __FILE__, __FUNCSIG__); }
}

void MoveShapeHandler::UpdateDxfDeviation()
{
	try
	{
		DXFDeviationHandler* TmpObject = new DXFDeviationHandler();
		TmpObject->UpdateMeasurementAfterProgramRun();		
		delete TmpObject;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("MSMH0009", __FILE__, __FUNCSIG__); }
}
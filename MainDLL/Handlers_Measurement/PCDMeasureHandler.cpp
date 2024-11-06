#include "StdAfx.h"
#include "PCDMeasureHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddDimAction.h"
#include "..\Helper\Helper.h"

PCDMeasureHandler::PCDMeasureHandler()
{
	try
	{
		init();
		setMaxClicks(3);
		MAINDllOBJECT->SetStatusCode("PCDMeasureHandler01");
		CurrentPCdCircle = MAINDllOBJECT->CurrentPCDShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0001", __FILE__, __FUNCSIG__); }
}

PCDMeasureHandler::~PCDMeasureHandler()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0002", __FILE__, __FUNCSIG__); }
}

void PCDMeasureHandler::init()
{
	try
	{
		dim = NULL;
		ShapeCount = 0;
		Cshape[0] = NULL;
		Cshape[1] = NULL;
		resetClicks();
		MAINDllOBJECT->SetStatusCode("PCDMeasureHandler02");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0003", __FILE__, __FUNCSIG__); }
}

void PCDMeasureHandler::LmouseDown()
{
	try
	{
		bool Cliked_OnRightShape = false;
		Vector* pt = &getClicksValue(getClicksDone() - 1);
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		Cshape[ShapeCount++] = MAINDllOBJECT->highlightedShape();		
		Cliked_OnRightShape = CheckShapeSelected(Cshape[ShapeCount - 1]);
		if(!Cliked_OnRightShape)
		{
			ShapeCount--;
			setClicksDone(getClicksDone() - 1);
			if(ShapeCount == 0)
			{
				ResetShapeHighlighted();
				init();
			}
			else MAINDllOBJECT->SetStatusCode("PCDMeasureHandler03");
			return;
		}
		else
		{
			Cshape[ShapeCount - 1]->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
			if(ShapeCount == 2)
			{
				if(Cshape[0]->getId() == Cshape[1]->getId()) // Add PCD Offset Distance Measurement
				{
					dim = HELPEROBJECT->CreatePCDMeasurements(CurrentPCdCircle, Cshape[0], NULL, RapidEnums::MEASUREMENTTYPE::DIM_PCDOFFSET, pt->getX(), pt->getY(), pt->getZ());
					AddShapeMeasureRelationShip(CurrentPCdCircle);
					AddShapeMeasureRelationShip(Cshape[0]);
				}
				else // Add PCD Angle Measurement..
				{
					dim = HELPEROBJECT->CreatePCDMeasurements(CurrentPCdCircle, Cshape[0], Cshape[1], RapidEnums::MEASUREMENTTYPE::DIM_PCDANGLE, pt->getX(), pt->getY(), pt->getZ());
					AddShapeMeasureRelationShip(CurrentPCdCircle);
					AddShapeMeasureRelationShip(Cshape[0]);
					AddShapeMeasureRelationShip(Cshape[1]);
				}
				MAINDllOBJECT->SetStatusCode("PCDMeasureHandler04");
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0004", __FILE__, __FUNCSIG__); }
}

void PCDMeasureHandler::mouseMove()
{
	try
	{
		if(getClicksDone() == 0)return;
		Vector* pt = &getClicksValue(getClicksDone());
		if(dim != NULL)
			dim->SetMousePosition(pt->getX(), pt->getY(), pt->getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0005", __FILE__, __FUNCSIG__); }
}

void PCDMeasureHandler::LmaxmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getMaxClicks() - 1);
		AddDimAction::addDim(dim);
		ResetShapeHighlighted();
		init();
		//MAINDllOBJECT->changeHandler(RCadApp::DEFAULT_HANDLER, true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0006", __FILE__, __FUNCSIG__); }
}

void PCDMeasureHandler::ResetShapeHighlighted()
{
	try
	{
		if(Cshape[0] != NULL)
			Cshape[0]->HighlightedForMeasurement(false);
		if(Cshape[1] != NULL)
			Cshape[1]->HighlightedForMeasurement(false);
		MAINDllOBJECT->Shape_Updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DIMMH0011", __FILE__, __FUNCSIG__); }
}

bool PCDMeasureHandler::CheckShapeSelected(Shape* CShape)
{
	try
	{
		bool ShapePresent = false;
		for each(Shape* Csh in ((PCDCircle*)CurrentPCdCircle)->PCDParentCollection)
		{
			if(Csh->getId() == CShape->getId()) 
			{
				ShapePresent = true;
				break;
			}
		}
		return ShapePresent;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0007", __FILE__, __FUNCSIG__); }
}

void PCDMeasureHandler::AddShapeMeasureRelationShip(Shape* CShape)
{
	try
	{
		if(CShape != NULL)
		{
			CShape->addMChild(dim);
			dim->addParent(CShape);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0008", __FILE__, __FUNCSIG__); }
}

void PCDMeasureHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() > 0)
		{
			if(dim != NULL)
				dim->drawMeasurement(windowno, WPixelWidth);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCDMH0009", __FILE__, __FUNCSIG__); }
}

void PCDMeasureHandler::EscapebuttonPress()
{
	ResetShapeHighlighted();
	init();
}
#include "StdAfx.h"
#include "RotaryDimensionHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddDimAction.h"
#include "..\Helper\Helper.h"

RotaryDimensionHandler::RotaryDimensionHandler()
{
	try
	{
		setMaxClicks(3);
		init();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RDIMMH0001", __FILE__, __FUNCSIG__); }
}

RotaryDimensionHandler::~RotaryDimensionHandler()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RDIMMH0002", __FILE__, __FUNCSIG__); }
}

void RotaryDimensionHandler::init()
{
	try
	{
		dim = NULL;
		ShapeCount = 0;
		Cshape[0] = NULL;
		Cshape[1] = NULL;
		resetClicks();
		MAINDllOBJECT->SetStatusCode("RotaryDimensionHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RDIMMH0003", __FILE__, __FUNCSIG__); }
}

void RotaryDimensionHandler::LmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getClicksDone() - 1);
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		Shape* Csh = MAINDllOBJECT->highlightedShape();
		if(Csh->ShapeType == RapidEnums::SHAPETYPE::LINE || Csh->ShapeType == RapidEnums::SHAPETYPE::XLINE)
		{
			Cshape[ShapeCount++] = MAINDllOBJECT->highlightedShape();
			Cshape[ShapeCount - 1]->HighlightedForMeasurement(true);
			MAINDllOBJECT->Shape_Updated();
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			if(ShapeCount == 0) init();
			else MAINDllOBJECT->SetStatusCode("RotaryDimensionHandler02");
			return;
		}		
		if(getClicksDone() == 2)
		{
			dim = HELPEROBJECT->CreateRotaryMeasurement(Cshape[0], Cshape[1], pt->getX(), pt->getY(), pt->getZ());
			AddShapeMeasureRelationShip(Cshape[0]);
			AddShapeMeasureRelationShip(Cshape[1]);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RDIMMH0004", __FILE__, __FUNCSIG__); }
}

void RotaryDimensionHandler::mouseMove()
{
	try
	{
		if(getClicksDone() == 0)return;
		Vector* pt = &getClicksValue(getClicksDone());
		if(dim != NULL)
			dim->SetMousePosition(pt->getX(), pt->getY(), pt->getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RDIMMH0005", __FILE__, __FUNCSIG__); }
}

void RotaryDimensionHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() > 0)
		{
			/*glColor3f(0, 1, 0);
			for(int Cnt = 0; Cnt < 2; Cnt++)
			{
				if(Cshape[Cnt] != NULL)
					Cshape[Cnt]->draw(windowno, MAINDllOBJECT->getWindow(windowno).getUppX());
			}
			glColor3f(1, 1, 1);*/
			if(dim != NULL)
				dim->drawMeasurement(windowno, MAINDllOBJECT->getWindow(windowno).getUppX());
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RDIMMH0006", __FILE__, __FUNCSIG__); }
}

void RotaryDimensionHandler::EscapebuttonPress()
{
	try
	{
		ResetShapeHighlighted();
		init();
		MAINDllOBJECT->SetStatusCode("RotaryDimensionHandler01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RDIMMH0007", __FILE__, __FUNCSIG__); }
}

void RotaryDimensionHandler::LmaxmouseDown()
{
	try
	{
		Vector* pt = &getClicksValue(getMaxClicks() - 1);
		AddDimAction::addDim(dim);
		ResetShapeHighlighted();
		init();
		MAINDllOBJECT->SetStatusCode("Finished");
		//MAINDllOBJECT->Measurement_updated();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RDIMMH0008", __FILE__, __FUNCSIG__); }
}

void RotaryDimensionHandler::ResetShapeHighlighted()
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


void RotaryDimensionHandler::AddShapeMeasureRelationShip(Shape* CShape)
{
	try
	{
		if(CShape != NULL)
		{
			CShape->addMChild(dim);
			dim->addParent(CShape);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RDIMMH0009", __FILE__, __FUNCSIG__); }
}
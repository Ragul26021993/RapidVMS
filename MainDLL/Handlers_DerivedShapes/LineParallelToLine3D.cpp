#include "StdAfx.h"
#include "LineParallelToLine3D.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Cylinder.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"

//Constructor..//
LineParallelToLine3D::LineParallelToLine3D()
{
	try
	{ 
		init(); 
		/*MAINDllOBJECT->SetStatusCode("LineParallel2Line01");*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARALLEL3D001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
LineParallelToLine3D::~LineParallelToLine3D()
{
}


//Initialise Default settings..
void LineParallelToLine3D::init()
{
	try
	{
		resetClicks();
		OffsetDefined = false;
		setMaxClicks(2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARALLEL3D002", __FILE__, __FUNCSIG__); }
}

//Handling Left mousedown...//
void LineParallelToLine3D::LmouseDown()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL)
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
		if(MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE3D || 
			MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::XLINE3D || 
			MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE || 
			MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::XLINE)
		{
			ParentShape = MAINDllOBJECT->highlightedShape();
			Line* pL = (Line*)ParentShape;
			pL->getParameters(ParentlineParam);
			MAINDllOBJECT->DerivedShapeCallback();
			MAINDllOBJECT->SetStatusCode("DL002");
		}
		else if (MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::CYLINDER ||
			MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::CONE)
		{
			Cylinder* cc = (Cylinder*)MAINDllOBJECT->highlightedShape();
			ParentShape = cc;
			cc->getAxisLine(ParentlineParam);
			MAINDllOBJECT->DerivedShapeCallback();
			MAINDllOBJECT->SetStatusCode("DL002");
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			return;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARALLEL3D003", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move..//
void LineParallelToLine3D::mouseMove()
{

}

//draw parallel line..
void LineParallelToLine3D::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() == getMaxClicks() - 1)
		{
			double MSelectionLine[6] = {0}, cPoint[6] = {0}, points[6] = {0};
			MAINDllOBJECT->getWindow(1).CalculateSelectionLine(MSelectionLine);
	     	RMATH3DOBJECT->Points_Of_ClosestDist_Btwn2Lines(MSelectionLine, ParentlineParam, cPoint);
			if (OffsetDefined)
			{
				double perpendLine[6] = { 0 };
				RMATH3DOBJECT->Create_Perpendicular_Line_2_Line(cPoint, ParentlineParam, perpendLine);
				double PtsonLine[6] = { 0 };
				RMATH3DOBJECT->Create_Points_On_Line(perpendLine, &cPoint[3], offset[1], PtsonLine);
				double ParallelLine[6] = { 0 };
				if (RMATH3DOBJECT->Distance_Point_Point(cPoint, PtsonLine) < RMATH3DOBJECT->Distance_Point_Point(cPoint, &PtsonLine[3]))
					memcpy(ParallelLine, &PtsonLine[0], 3 * sizeof(double));
				else
					memcpy(ParallelLine, &PtsonLine[3], 3 * sizeof(double));
				memcpy(&ParallelLine[3], &ParentlineParam[3], 3 * sizeof(double));
				RMATH3DOBJECT->Create_Points_On_Line(ParallelLine, ParallelLine, offset[2] / 2, points);
			}
			else
			{
				for (int i = 0; i < 3; i++)
				{
					points[i] = cPoint[i] + ParentlineParam[3 + i];
					points[3 + i] = cPoint[i] - ParentlineParam[3 + i];
				}
			}
			GRAFIX->drawLine_3D(points[0], points[1], points[2], points[3], points[4], points[5]);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARALLEL3D004", __FILE__, __FUNCSIG__); }
}

///Handle the mouse scroll... calculate the line end points for change in length...//
void LineParallelToLine3D::MouseScroll(bool flag)
{
}

//Set Offset distance..
void LineParallelToLine3D::SetAnyData(double *data)
{
	try
	{
		if(getClicksDone() != getMaxClicks() - 1) return;
		memset(offset, 0, sizeof(double));
		if(data[0] != 0 || data[1] != 0 || data[1] != 0)
		{
			offset[0] = data[0];
			offset[1] = data[1];
			offset[2] = data[2];
			OffsetDefined = true;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARALLEL3D005", __FILE__, __FUNCSIG__); }
}

//Handle the partprogram data...//
void LineParallelToLine3D::PartProgramData()
{
	
}

void LineParallelToLine3D::EscapebuttonPress()
{
	init();
}

void LineParallelToLine3D::LmaxmouseDown()
{
	try
	{
		double cPoint[6] = {0};
		if(RMATH3DOBJECT->Checking_Point_on_Line(cPoint, ParentlineParam))
		{
		    setClicksDone(getClicksDone() - 1);
			return;
		}
		if(MAINDllOBJECT->highlightedPoint() != NULL)
		{
			Vector* pt = MAINDllOBJECT->highlightedPoint();
			cPoint[0] = pt->getX();
			cPoint[1] = pt->getY();
			cPoint[2] = pt->getZ();
		}
		else
		{
			double MSelectionLine[6] = {0};
			MAINDllOBJECT->getWindow(1).CalculateSelectionLine(MSelectionLine);
	     	RMATH3DOBJECT->Points_Of_ClosestDist_Btwn2Lines(MSelectionLine, ParentlineParam, cPoint);								
		}

		CurrentShape = new Line(_T("L3D"), RapidEnums::SHAPETYPE::LINE3D);
		CurrentShape->ShapeAs3DShape(true);
		((Line*)CurrentShape)->LineType = RapidEnums::LINETYPE::PARALLELLINE3D;
		setBaseRProperty(cPoint[0], cPoint[1], cPoint[2]);
		baseaction->DroMovementFlag = false;
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction, false);
		ParentShape->addChild(CurrentShape);
		CurrentShape->addParent(ParentShape);
		((Line*)CurrentShape)->DefinedOffset(OffsetDefined);
		((Line*)CurrentShape)->Offset(offset[1]);
		((Line*)CurrentShape)->DefinedLength(OffsetDefined);
		((Line*)CurrentShape)->Length(offset[2]);
		CurrentShape->UpdateBestFit();
		AddShapeAction::addShape(CurrentShape);
    	init();
		//MAINDllOBJECT->DerivedShapeCallback();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPARALLEL3D005", __FILE__, __FUNCSIG__); }
}

void LineParallelToLine3D::RmouseDown(double x, double y)
{

}

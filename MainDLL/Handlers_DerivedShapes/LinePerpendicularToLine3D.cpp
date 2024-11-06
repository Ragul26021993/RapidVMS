#include "StdAfx.h"
#include "LinePerpendicularToLine3D.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Vector.h"
#include "..\Engine\RCadApp.h"
#include "..\Actions\AddPointAction.h"
#include "..\Actions\AddShapeAction.h"

//Constructor..//
LinePerpendicularToLine3D::LinePerpendicularToLine3D()
{
	try
	{ 
		init(); 
		/*MAINDllOBJECT->SetStatusCode("LinePerpendicular2Line01");*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPER3D0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
LinePerpendicularToLine3D::~LinePerpendicularToLine3D()
{
}

void LinePerpendicularToLine3D::init()
{
	try
	{
		CurrentLine = NULL;
		resetClicks();
		setMaxClicks(3);
		ParentShape = NULL;
		Length = 1; 
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPER3D0002", __FILE__, __FUNCSIG__); }
}

//Handling Left mousedown...//
void LinePerpendicularToLine3D::LmouseDown()
{
	try
	{
		if(getClicksDone() == 1)
		{
			if(MAINDllOBJECT->highlightedShape() != NULL && (MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE3D || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::XLINE3D || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE || MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::XLINE))
	        {
				 	ParentShape = (Line*)MAINDllOBJECT->highlightedShape();
		        	ParentShape->getParameters(ParentlineParam);
					Length = ParentShape->Length();
			}
			else
			    setClicksDone(getClicksDone() - 1);
			return;
		}
		else if (getClicksDone() == 2)
		{
			CurrentShape = CurrentLine = new Line(_T("L3D"), RapidEnums::SHAPETYPE::LINE3D);
			double cPoint[6] = {0};
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
		    setBaseRProperty(cPoint[0], cPoint[1], cPoint[2]);
	    }
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPER3D0003", __FILE__, __FUNCSIG__); }
}

//Handle the mouse move..//
void LinePerpendicularToLine3D::mouseMove()
{
	    if(getClicksDone() == 2)
		{
			double MSelectionLine[6] = {0}, cPoint[3] = {0}, tmpDir[3] = {0}, tmpCos[3] = {0}, tmpPlane[4] = {0}, firstPoint[3] = {baseaction->points[0].getX(), baseaction->points[0].getY(), baseaction->points[0].getZ()};
			for(int i = 0; i < 3; i++)
			{
				tmpPlane[i] = ParentlineParam[3 + i];
				tmpPlane[3] += tmpPlane[i] * firstPoint[i];
			}
			MAINDllOBJECT->getWindow(1).CalculateSelectionLine(MSelectionLine);
	     	RMATH3DOBJECT->Intersection_Line_Plane(MSelectionLine, tmpPlane, cPoint);	
	        for(int i = 0; i < 3; i++){tmpDir[i] = cPoint[i] - firstPoint[i];}
			RMATH3DOBJECT->DirectionCosines(tmpDir, tmpCos);
			for(int i = 0; i < 3; i++)
			{
				NewLinePoints[i] = firstPoint[i] + tmpCos[i] * Length/2;
				NewLinePoints[3 + i] = firstPoint[i] - tmpCos[i] * Length/2;
			}
		}
}

//draw perpendicular line..
void LinePerpendicularToLine3D::draw(int windowno, double WPixelWidth)
{
	if(getClicksDone() == 2)
	    	GRAFIX->drawLine_3D(NewLinePoints[0], NewLinePoints[1], NewLinePoints[2], NewLinePoints[3], NewLinePoints[4], NewLinePoints[5]);
}

void LinePerpendicularToLine3D::MouseScroll(bool flag)
{
}

//Handle the partprogram data...//
void LinePerpendicularToLine3D::PartProgramData()
{
}

//Reset the current handler settings.,
void LinePerpendicularToLine3D::EscapebuttonPress()
{
	if(CurrentLine != NULL)
		delete CurrentLine;
	init(); 
}

void LinePerpendicularToLine3D::LmaxmouseDown()
{
	try
	{
		double cPoint[3] = {0};
		if(MAINDllOBJECT->highlightedPoint() != NULL)
		{
			Vector* pt = MAINDllOBJECT->highlightedPoint();
			cPoint[0] = pt->getX();
			cPoint[1] = pt->getY();
			cPoint[2] = pt->getZ();
		}
		else
		{
			double MSelectionLine[6] = {0}, tmpPlane[4] = {0}, firstPoint[3] = {baseaction->points[0].getX(), baseaction->points[0].getY(), baseaction->points[0].getZ()};
			for(int i = 0; i < 3; i++)
			{
				tmpPlane[i] = ParentlineParam[3 + i];
				tmpPlane[3] += tmpPlane[i] * firstPoint[i];
			}
			MAINDllOBJECT->getWindow(1).CalculateSelectionLine(MSelectionLine);
	     	RMATH3DOBJECT->Intersection_Line_Plane(MSelectionLine, tmpPlane, cPoint);								
		}
		double tmpDir[3] = {baseaction->points[0].getX() -  cPoint[0], baseaction->points[0].getY() - cPoint[1], baseaction->points[0].getZ() - cPoint[2]}, tmpCos[3] = {0};
		RMATH3DOBJECT->DirectionCosines(tmpDir, tmpCos);
		if(RMATH3DOBJECT->Angle_Btwn_2Directions(&ParentlineParam[3], tmpCos, false, false) == 0) 
		{
		    setClicksDone(getClicksDone() - 1);
			return;
		}
		((Line*)CurrentShape)->LineType = RapidEnums::LINETYPE::PERPENDICULARLINE3D;
		CurrentShape->ShapeAs3DShape(true);
		baseaction->DroMovementFlag = false;
	    AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction, false);
		setBaseRProperty(cPoint[0], cPoint[1], cPoint[2]);
		baseaction->DroMovementFlag = false;
		AddPointAction::pointAction((ShapeWithList*)CurrentShape, baseaction, false);
	    ParentShape->addChild(CurrentShape);
		CurrentShape->addParent(ParentShape);
		CurrentShape->UpdateBestFit();
		AddShapeAction::addShape(CurrentShape);
		init();
		MAINDllOBJECT->SetStatusCode("Finished");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LINEPER3D0005", __FILE__, __FUNCSIG__); }
}

void LinePerpendicularToLine3D::SetAnyData(double *data)
{

	if(data[0] != 0)
		Length = data[0];
}

void LinePerpendicularToLine3D::RmouseDown(double x, double y)
{
}

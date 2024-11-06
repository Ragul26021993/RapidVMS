#include "stdafx.h"
#include "Cad1LineHandler3D.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\Cylinder.h"
#include "..\Shapes\Cone.h"
#include "..\Shapes\Line.h"
#include "..\DXF\DXFExpose.h"
//Constructor...
Cad1LineHandler3D::Cad1LineHandler3D()
{
	try
	{
		this->AlignmentModeFlag = true;
		 DxfPointFlag = false, DxfPointFlag1 = false, DxfLineFlag = false;
		DxfShape =  NULL; VideoShape = NULL;
		setMaxClicks(4);
		//MAINDllOBJECT->SetStatusCode("DXFALGN0003");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1LINEMH0001", __FILE__, __FUNCSIG__); }
}

//Destructor...//
Cad1LineHandler3D::~Cad1LineHandler3D()
{
	try
	{
		ResetShapeHighlighted();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1LINEMH0002", __FILE__, __FUNCSIG__); }
}

void Cad1LineHandler3D::ResetShapeHighlighted()
{
	try
	{
		if(DxfShape != NULL)
		{
			DxfShape->HighlightedForMeasurement(false);
			DxfShape =  NULL;
		}
		if(VideoShape != NULL)
		{
			VideoShape->HighlightedForMeasurement(false);
			VideoShape =  NULL;
		}
		MAINDllOBJECT->DXFShape_Updated_Alignment();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1LINEMH0001", __FILE__, __FUNCSIG__); }
}

//Left mousedown.. select point and calculates the transforamtion matrix..//
void Cad1LineHandler3D::SelectPoints_Shapes()
{
	try
	{
		if(MAINDllOBJECT->getCurrentWindow() == 2 && getClicksDone() < 2)
		{
			if(MAINDllOBJECT->highlightedPoint() == NULL && MAINDllOBJECT->highlightedShape() == NULL)
			{
				setClicksDone(getClicksDone() - 1);
				MAINDllOBJECT->SetStatusCode("DXFALGN0000");
				return;
			}
			if( MAINDllOBJECT->highlightedShape() != NULL)
			{
				//if(MAINDllOBJECT->highlightedShape()->Normalshape());
				switch(MAINDllOBJECT->highlightedShape()->ShapeType)
				{					
				    case RapidEnums::SHAPETYPE::CYLINDER:						
							Line1Point1.set(((Cylinder*)MAINDllOBJECT->highlightedShape())->getCenter1()->getX(), ((Cylinder*)MAINDllOBJECT->highlightedShape())->getCenter1()->getY(), ((Cylinder*)MAINDllOBJECT->highlightedShape())->getCenter1()->getZ());
							Line1Point2.set(((Cylinder*)MAINDllOBJECT->highlightedShape())->getCenter2()->getX(), ((Cylinder*)MAINDllOBJECT->highlightedShape())->getCenter2()->getY(), ((Cylinder*)MAINDllOBJECT->highlightedShape())->getCenter2()->getZ());
							 DxfPointFlag = true;
							break;
					case RapidEnums::SHAPETYPE::CONE:						
							Line1Point1.set(((Cone*)MAINDllOBJECT->highlightedShape())->getCenter1()->getX(), ((Cone*)MAINDllOBJECT->highlightedShape())->getCenter1()->getY(), ((Cone*)MAINDllOBJECT->highlightedShape())->getCenter1()->getZ());
							Line1Point2.set(((Cone*)MAINDllOBJECT->highlightedShape())->getCenter2()->getX(), ((Cone*)MAINDllOBJECT->highlightedShape())->getCenter2()->getY(), ((Cone*)MAINDllOBJECT->highlightedShape())->getCenter2()->getZ());	
							 DxfPointFlag = true;
						break;
				 }	  	
		    }
		}
		else if((MAINDllOBJECT->getCurrentWindow() == 0 && getClicksDone() > 1) || (MAINDllOBJECT->getCurrentWindow() == 1 && getClicksDone() > 1))
		{
			if(MAINDllOBJECT->highlightedPoint() == NULL && MAINDllOBJECT->highlightedShape() == NULL)
			{
				setClicksDone(getClicksDone() - 1);
				MAINDllOBJECT->SetStatusCode("DXFALGN0000");
				return;
			}
			if(DxfPointFlag1 && MAINDllOBJECT->highlightedPoint() != NULL)
			{
				setClicksDone(getClicksDone() - 1);
				MAINDllOBJECT->SetStatusCode("DXFALGN0000");
				return;
			}
			if(DxfPointFlag && MAINDllOBJECT->highlightedPoint() != NULL)
			{
				Dxfpoint.set(MAINDllOBJECT->highlightedPoint()->getX(), MAINDllOBJECT->highlightedPoint()->getY(), MAINDllOBJECT->highlightedPoint()->getZ());
				
				DxfLineFlag = true;
				DxfPointFlag = false;
				DxfPointFlag1 = true;
			}
			
			if(DxfLineFlag && MAINDllOBJECT->highlightedShape() != NULL)
			{
				
			   Line2Point1.set(((Line*)MAINDllOBJECT->highlightedShape())->getPoint1()->getX(),((Line*)MAINDllOBJECT->highlightedShape())->getPoint1()->getY(),((Line*)MAINDllOBJECT->highlightedShape())->getPoint1()->getZ());
		       Line2Point2.set(((Line*)MAINDllOBJECT->highlightedShape())->getPoint2()->getX(),((Line*)MAINDllOBJECT->highlightedShape())->getPoint2()->getY(),((Line*)MAINDllOBJECT->highlightedShape())->getPoint2()->getZ());
			   
			}
			if(!DxfLineFlag && MAINDllOBJECT->highlightedShape() != NULL)
			{
				setClicksDone(getClicksDone() - 1);
				MAINDllOBJECT->SetStatusCode("DXFALGN0000");
				return;
			}
		}
		else
		{
			setClicksDone(getClicksDone() - 1);
			MAINDllOBJECT->SetStatusCode("DXFALGN0000");
			return;
		}
		if(getClicksDone() == 3)
		{
			double transformMatrix1[16], translation[3], DCs_line2[3], pt_projection[3];
			double pt2[3] = {Dxfpoint.getX(), Dxfpoint.getY(), Dxfpoint.getZ()};
			double line1[6] = {Line1Point1.getX(), Line1Point1.getY(), Line1Point1.getZ(), Line1Point2.getX(), Line1Point2.getY(), Line1Point2.getZ()};
			double line2[6] = {Line2Point1.getX(), Line2Point1.getY(), Line2Point1.getZ(), Line2Point2.getX(), Line2Point2.getY(), Line2Point2.getZ()};
			RMATH3DOBJECT->DCs_TwoEndPointsofLine(line2, DCs_line2);
			double line2_withDCs[6] = { line2[0],  line2[1],  line2[2], DCs_line2[0], DCs_line2[1], DCs_line2[2]};
			RMATH3DOBJECT->Projection_Point_on_Line(pt2, line2_withDCs, pt_projection);
			RMATH3DOBJECT->TransformationMatrix_IGESAlignment_1Line(line1, line2, pt2, transformMatrix1);
			DXFEXPOSEOBJECT->TransformMatrix3D_1Line(&transformMatrix1[0], pt_projection, &line1[0]);
			((CadAlignHandler*)MAINDllOBJECT->getCurrentHandler())->EscapebuttonPress();
			//MAINDllOBJECT->SetStatusCode("DXFALGN0015");
			resetClicks();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1LINEMH0003", __FILE__, __FUNCSIG__); }
}

//Check for the highlighted shape..
bool Cad1LineHandler3D::CheckHighlightedShape()
{
	try
	{
		if(MAINDllOBJECT->highlightedShape() == NULL) return false;
		if(MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::LINE) return true;
		if(MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::SPHERE) return true;
		if(MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::CYLINDER) return true;
		if(MAINDllOBJECT->highlightedShape()->ShapeType == RapidEnums::SHAPETYPE::CONE) return true;
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1LINEMH0004", __FILE__, __FUNCSIG__); return false; }
}

//Handle escape button press.. Clear the current selection..
void Cad1LineHandler3D::Align_Finished()
{
	try
	{
		 DxfPointFlag = false, DxfLineFlag = false;
		ResetShapeHighlighted();
		resetClicks();
		MAINDllOBJECT->SetStatusCode("DXFALGN0001");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1LINEMH0005", __FILE__, __FUNCSIG__); }
}

//draw selected point on dxf window..
void Cad1LineHandler3D::drawSelectedShapesOndxf()
{
	try
	{
		double PixelWidth = MAINDllOBJECT->getWindow(2).getUppX();
		GRAFIX->SetColor_Double(0, 1, 0);
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1LINEMH0006", __FILE__, __FUNCSIG__); }
}

//draw selected point on video..
void Cad1LineHandler3D::drawSelectedShapesOnvideo()
{
	try
	{
		double PixelWidth = MAINDllOBJECT->getWindow(2).getUppX();
		GRAFIX->SetColor_Double(0, 1, 0);
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1LINEMH0007", __FILE__, __FUNCSIG__); }
}

void Cad1LineHandler3D::Align_mouseMove(double x, double y)
{
	 try
	 {

	 }
	 catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CAD1LINEMH0008", __FILE__, __FUNCSIG__); }
}
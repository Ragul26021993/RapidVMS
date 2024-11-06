#include "StdAfx.h"
#include "FrameGrabShape.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"

FrameGrabShape::FrameGrabShape( TCHAR* myname):ShapeWithList(genName(myname))
{
	init();
}

FrameGrabShape::~FrameGrabShape()
{

}

void FrameGrabShape::init()
{
	try
	{
		this->ShapeType = RapidEnums::SHAPETYPE::SPHERE;
        this->ShapeAs3DShape(true);
		this->UseLightingProperties(true);
		this->Normalshape(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0001", __FILE__, __FUNCSIG__); }
}

TCHAR* FrameGrabShape::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 10, prefix);
		shapenumber++;
		TCHAR shapenumstr[10];
		_itot_s(shapenumber, shapenumstr, 10, 10);
		_tcscat_s(name, 10, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0002", __FILE__, __FUNCSIG__); return name; }
}

bool FrameGrabShape::Shape_IsInWindow(Vector& corner1,double Tolerance)
{
	try
	{
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0003", __FILE__, __FUNCSIG__); }
}

bool FrameGrabShape::Shape_IsInWindow(Vector& corner1,Vector& corner2 )
{
	return false;
}

bool FrameGrabShape::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	try
	{
	return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0004", __FILE__, __FUNCSIG__); return false;}
}

void FrameGrabShape::Translate(Vector& Position)
{
}

void FrameGrabShape::drawCurrentShape(int windowno, double WPixelWidth)
{
	try
	{
		GRAFIX->drawLine_3DStipple(Points[0].getX(), Points[0].getY(), Points[0].getZ(), Points[1].getX(), Points[0].getY(), Points[0].getZ());
		GRAFIX->drawLine_3DStipple(Points[1].getX(), Points[0].getY(), Points[0].getZ(), Points[1].getX(), Points[1].getY(), Points[1].getZ());
		GRAFIX->drawLine_3DStipple(Points[1].getX(), Points[1].getY(), Points[1].getZ(), Points[0].getX(), Points[1].getY(), Points[0].getZ());
		GRAFIX->drawLine_3DStipple(Points[0].getX(), Points[1].getY(), Points[0].getZ(), Points[0].getX(), Points[0].getY(), Points[0].getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0005", __FILE__, __FUNCSIG__); }
}

void FrameGrabShape::drawGDntRefernce()
{
}

void FrameGrabShape::ResetShapeParameters()
{ 
}

void FrameGrabShape::UpdateBestFit()
{
	center.set((Points[0].getX() + Points[1].getX())/2, (Points[0].getY() + Points[1].getY())/2, (Points[0].getZ() + Points[1].getZ())/2);
	this->IsValid(true);
}

bool FrameGrabShape::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{
	try
	{
		// int s1[2] = {4, 4}, s2[2] = {4, 1};
		// double cent[4] = {center.getX(), center.getY(), center.getZ(), 1}, new_cen[4] = {0};
		//RMATH2DOBJECT->MultiplyMatrix1(&transformMatrix[0], &s1[0], &cent[0], &s2[0], &new_cen[0]);
		//RMATH2DOBJECT->RectangleEnclosing_Circle(&new_cen[0], Radius(), &Lefttop[0], &Rightbottom[0]);
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0008", __FILE__, __FUNCSIG__); return false;}
}

void FrameGrabShape::UpdateForParentChange(BaseItem* sender)
{
	if(this->getParents().size() == 0)
		delete this;
}

void FrameGrabShape::UpdateEnclosedRectangle()
{
	try
	{
		/*double cen[3] = {0};
		center.FillDoublePointer(&cen[0], 3);
		if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 2)
		{		 
			 double temp_cen[3] = {cen[0], cen[1], cen[2]}, TransMatrix[9] = {0};
			 int Order1[2] = {3, 3}, Order2[2] = {3, 1};
			 RMATH2DOBJECT->OperateMatrix4X4(&MAINDllOBJECT->getCurrentUCS().transform[0], 3, 1, TransMatrix);
			 RMATH2DOBJECT->MultiplyMatrix1(TransMatrix, Order1, temp_cen, Order2, cen);		
		}
		RMATH2DOBJECT->RectangleEnclosing_Circle(&cen[0], Radius(), &ShapeLeftTop[0], &ShapeRightBottom[0]);*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0026", __FILE__, __FUNCSIG__); }
}

void FrameGrabShape::GetShapeCenter(double *cPoint)
{
	cPoint[0] = center.getX();
	cPoint[1] = center.getY();
	cPoint[2] = center.getZ();
}

void FrameGrabShape::Transform(double* transform)
{
}

void FrameGrabShape::AlignToBasePlane(double* trnasformM)
{
	center.set(MAINDllOBJECT->TransformMultiply_PlaneAlign(trnasformM, center.getX(), center.getY(), center.getZ()));
}

Vector* FrameGrabShape::getCenter()
{
	return(&this->center);
}

Shape* FrameGrabShape::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		std::wstring myname;
		if(n == 0)
			myname = _T("FG");
		else
			myname = _T("dFG");
		FrameGrabShape* CShape = new FrameGrabShape((TCHAR*)myname.c_str());
		if(copyOriginalProperty)
	    	CShape->CopyOriginalProperties(this);
		return CShape;		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0016", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* FrameGrabShape::CreateDummyCopy()
{
	try
	{
		FrameGrabShape* CShape = new FrameGrabShape();
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SPH0017", __FILE__, __FUNCSIG__); return NULL; }
}

void FrameGrabShape::CopyShapeParameters(Shape* s)
{
}

bool FrameGrabShape::WithinCirclularView(double *SelectionLine, double Tolerance)
{
	return false;
}

double FrameGrabShape::GetZlevel()
{
	return center.getZ();
}

int FrameGrabShape::shapenumber = 0;

void FrameGrabShape::reset()
{
	shapenumber = 0;
}

#include "StdAfx.h"
#include "DepthShape.h"
#include "..\Engine\RCadApp.h"

DepthShape::DepthShape(TCHAR* myname):ShapeWithList(genName(myname))
{
	init();
}

DepthShape::DepthShape(bool simply):ShapeWithList(false)
{
	init();
}

DepthShape::~DepthShape()
{
}

void DepthShape::init()
{
	this->ShapeType = RapidEnums::SHAPETYPE::DEPTHSHAPE;
	this->ShapeAs3DShape(true);
}

TCHAR* DepthShape::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 10, prefix);
		TCHAR shapenumstr[10];
		shapenumber++;
		_itot_s(shapenumber, shapenumstr, 10, 10);
		_tcscat_s(name, 10, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DEPTH0001", __FILE__, __FUNCSIG__); return name; }
}

bool DepthShape::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	return false;
}

bool DepthShape::Shape_IsInWindow(Vector& corner1, Vector& corner2)
{
	return false;
}

bool DepthShape::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	return false;
}

void DepthShape::Translate(Vector& Position)
{
}

void DepthShape::drawCurrentShape(int windowno, double WPixelWidth)
{
}

void DepthShape::drawGDntRefernce()
{
}

void DepthShape::ResetShapeParameters()
{
}

void DepthShape::UpdateBestFit()
{
	IsValid(true);
}

 bool DepthShape::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
 {return false;
 }

void DepthShape::UpdateForParentChange(BaseItem* sender)
{
}

void DepthShape::Transform(double* transform)
{
}

void DepthShape::AlignToBasePlane(double* trnasformM)
{
}

void DepthShape::UpdateEnclosedRectangle()
{
}

Shape* DepthShape::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		DepthShape* CShape = new DepthShape();
		if(copyOriginalProperty)
	    	CShape->CopyOriginalProperties(this);
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DEPTH0002", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* DepthShape::CreateDummyCopy()
{
	try
	{
		DepthShape* CShape = new DepthShape(false);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DEPTH0003", __FILE__, __FUNCSIG__); return NULL; }
}

void DepthShape::CopyShapeParameters(Shape* s)
{
}

int DepthShape::shapenumber = 0;
void DepthShape::reset()
{
	shapenumber = 0;
}

wostream& operator<<(wostream& os, DepthShape& x)
{
	try
	{
		os << (*static_cast<Shape*>(&x));
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DEPTH0004", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, DepthShape& x)
{
	try
	{
		is >> (*(Shape*)&x);
		return is;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DEPTH0005", __FILE__, __FUNCSIG__); return is; }
}
#include "StdAfx.h"
#include "DummyShape.h"
#include "..\Engine\RCadApp.h"

DummyShape::DummyShape():ShapeWithList(L"Dummy")
{
	init();
}

DummyShape::DummyShape(bool simply):ShapeWithList(false)
{
	init();
}

DummyShape::~DummyShape()
{
}

void DummyShape::init()
{
	this->ShapeType = RapidEnums::SHAPETYPE::DUMMY;
	this->ShapeAs3DShape(true);
	this->IsValid(true);
}

bool DummyShape::Shape_IsInWindow(Vector& corner1, double Tolerance)
{
	return false;
}

bool DummyShape::Shape_IsInWindow(Vector& corner1, Vector& corner2)
{
	return false;
}

bool DummyShape::Shape_IsInWindow(double *SelectionLine, double Tolerance)
{
	return false;
}

void DummyShape::Translate(Vector& Position)
{
}

void DummyShape::drawCurrentShape(int windowno, double WPixelWidth)
{
}

void DummyShape::drawGDntRefernce()
{
}

void DummyShape::ResetShapeParameters()
{
}

void DummyShape::UpdateBestFit()
{
	IsValid(true);
}

 bool DummyShape::GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom)
{return false;
}

void DummyShape::UpdateForParentChange(BaseItem* sender)
{
}

void DummyShape::Transform(double* transform)
{
}

void DummyShape::AlignToBasePlane(double* trnasformM)
{
}

void DummyShape::UpdateEnclosedRectangle()
{
}


Shape* DummyShape::Clone(int n, bool copyOriginalProperty)
{
	try
	{
		DummyShape* CShape = new DummyShape();
		if(copyOriginalProperty)
	    	CShape->CopyOriginalProperties(this);
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DUM0001", __FILE__, __FUNCSIG__); return NULL; }
}

Shape* DummyShape::CreateDummyCopy()
{
	try
	{
		DummyShape* CShape = new DummyShape(false);
		CShape->CopyOriginalProperties(this);
		CShape->setId(this->getId());
		return CShape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DUM0002", __FILE__, __FUNCSIG__); return NULL; }
}

void DummyShape::CopyShapeParameters(Shape* s)
{

}

wostream& operator<<(wostream& os, DummyShape& x)
{
	try
	{
		os << (*static_cast<Shape*>(&x));
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DUM0003", __FILE__, __FUNCSIG__); return os; }
}


wistream& operator>>( wistream& is, DummyShape& x )
{
	try
	{
		is >> (*(Shape*)&x);
		return is;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DUM0004", __FILE__, __FUNCSIG__); return is; }
}
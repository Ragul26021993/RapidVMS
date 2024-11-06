#include "StdAfx.h"
#include "SinglePoint.h"
#include "..\Engine\RCadApp.h"

SinglePoint::SinglePoint(double x, double y, double z, double r, double Probedir, int side)
{
	this->PtID = 0;
	this->IsSelected = false;
    this->InValid = false;
	this->X = x; this->Y = y; this->Z = z; this->R = r; this->Pdir = Probedir; this->PLR = side;
}

SinglePoint::~SinglePoint()
{
}

void SinglePoint::SetValues(double x, double y, double z, double r, double Probedir, int side)
{
	this->X = x; this->Y = y; this->Z = z; this->R = r; this->Pdir = Probedir; this->PLR = side;
}

bool SinglePoint::Point_IsInWindow(double minX, double minY, double maxX, double maxY)
{
	if(X >= minX && X <= maxX && Y >= minY &&Y <= maxY) return true;
	return false;
}

void SinglePoint::Translate(double *Position)
{
	X += Position[0];
	Y += Position[1];
	Z += Position[2];
}

void SinglePoint::Transform(double* transform)
{
	try
	{
		Vector tmpV = MAINDllOBJECT->TransformMultiply(transform, X, Y, Z);
		SetValues(tmpV.getX(), tmpV.getY(), tmpV.getZ());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CIR0026", __FILE__, __FUNCSIG__); }
}
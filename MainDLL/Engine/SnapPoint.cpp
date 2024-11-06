#include "stdafx.h"
#include "SnapPoint.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\Shape.h"
#include "RCadApp.h"

//Constructor....//
SnapPoint::SnapPoint(Shape* parent, Vector* pt, int id, bool flag)
{
	this->parent = parent;
	this->pt = pt;
	this->SPtId = id;
	this->IsValid = flag;
	std::string status;
	if (this->IsValid == true) {
		status = "true";
	}
	else {
		status = "false";
	}
	MAINDllOBJECT->SetAndRaiseErrorMessage(status, __FILE__, __FUNCSIG__);
}

//Destructor...//
SnapPoint::~SnapPoint()
{
}

//Returns the snap point...
Vector* SnapPoint::getPt()
{
	return pt;
}

//returns the parent...//
Shape* SnapPoint::getParent()
{
	return parent;
}
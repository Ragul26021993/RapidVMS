#include "stdafx.h"
#include "IsectPt.h"
#include "..\Shapes\ShapeWithList.h"
#include "..\Engine\RCadApp.h"

//Constructor.. Update the intersection Pointer to the Shape..
IsectPt::IsectPt(Shape *s1, Shape *s2)
{
	try
	{
		IsnPtId = Itemno++;
		IntersectionPoint = true; ImplicitIntersection = false;
		parent1 = s1; parent2 = s2;
		((ShapeWithList*)s1)->IsectPointsList->Addpoint(this);
		((ShapeWithList*)s2)->IsectPointsList->Addpoint(this);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISECT0001", __FILE__, __FUNCSIG__); }
}

//Destructor...// clear the relationship dimension pointers.//
IsectPt::~IsectPt()
{
	try{ Dimchild.clear(); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISECT0002", __FILE__, __FUNCSIG__); }
}

//Returns the parent shape...//
Shape* IsectPt::getParent1()
{
	return parent1;
}

//Returns the parent shape..//
Shape* IsectPt::getParent2()
{
	return parent2;
}

//Add related dimension..
void IsectPt::SetRelatedDim(DimBase* d)
{
	try
	{
		bool Presentflag = false;
		for each(DimBase* Cdim in Dimchild)
		{
			if(Cdim->getId() == d->getId())
			{
				Presentflag = true;
				break;
			}
		}
		if(!Presentflag)
			Dimchild.push_back(d);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISECT0003", __FILE__, __FUNCSIG__); }
}

list<BaseItem*>& IsectPt::getRelatedDim()
{
	return Dimchild;
}

//Set the intersection type.... Explicit / Implicit...//
void IsectPt::SetIntersectionType(bool Implicit)
{
	ImplicitIntersection = Implicit;
}

//Returns the intersection type..//
bool IsectPt::GetIntersectionType()
{
	return ImplicitIntersection;
}

//Intersection point id.. static number initialisation..
int IsectPt::Itemno = 0;
void IsectPt::reset()
{
	Itemno = 0;
}
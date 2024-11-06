// Class used to hold the intersection point and its properties..//
// Contains Pointer to parents.. Pointer to related dimensions..//
// Property.. Whether implicit or explici intersection point..//
#pragma once
#include "..\Shapes\Shape.h"
#include "..\Measurement\DimBase.h"
class IsectPt: 
	public Vector
{
private:
	Shape* parent1;
	Shape* parent2;
	list<BaseItem*> Dimchild;
	bool ImplicitIntersection;
	static int Itemno;
public:
	int IsnPtId;
	//Constructor...//
	IsectPt(Shape *s1, Shape *s2);
	virtual ~IsectPt();

	//returns the intersection point parents..//
	Shape* getParent1();
	Shape* getParent2();

	//Intersection point type... Explicit / implicit..//
	void SetIntersectionType(bool Implicit);
	bool GetIntersectionType();

	//Related Dimensions...
	void SetRelatedDim(DimBase* d);
	list<BaseItem*>& getRelatedDim();

	//Reset intersection point id counter..
	static void reset();
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
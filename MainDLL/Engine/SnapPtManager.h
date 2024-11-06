//Main Class used to Hold the intersection point related to shapes in an ucs..
// calculate, update the intersection point between the shapes..//
#pragma once
#include "CollectionListener.h"
#include "IsectPtCollection.h"
#include "..\Shapes\ShapeWithList.h"

class SnapPoint;
class RCollectionBase;

//Snap point manager..
//Derived from collectionlistener..
class SnapPtManager: public CollectionListener
{
private:
	//Collection of intersection points...//
	IsectPtCollection IsectPtList;
	RCollectionBase *CurrentUCSShapeList;
	//Parent shapes to intersection points.... and current shape...
	Shape *Parent1, *Parent2, *addedshape;

	// array to hold the intersection points... explicit and implicit points.../
	double ans[4], ans1[4];
	int SCount[2];
	
	//create intersection points function
	void calcIntPts(BaseItem* item);
	//Calculate the intersection point between the shapes..//
	void createIntPts(ShapeWithList* s1, ShapeWithList* s2);
	//To initilize new Isect point class and add it to collections....//
	void CopyPoints(ShapeWithList* s1, ShapeWithList* s2);
	//Added on 03/05/10... by Sathya..! Update the intersection point when the shape is updated..//
	void UpdateIntersectionWithshapes(ShapeWithList* s1, ShapeWithList* s2);

public:
	//Constructor and destructor...//
	SnapPtManager(RCollectionBase* shapes);
	~SnapPtManager();

	list<SnapPoint*> AllSnapPoints;
	int SnapPtId;

	//Item added, itemchaged events...//
	virtual void itemAdded(BaseItem* item, Listenable* sender);
	virtual void itemRemoved(BaseItem* item, Listenable* sender);
	virtual void itemChanged(BaseItem* item, Listenable* sender);
	virtual void selectionChanged(Listenable* sender);

	//Retrurns the intersection points collection////
	IsectPtCollection* getIsectPts();
	//Returns the parent shapes of the current intersection..//
	Shape* getParent1();
	Shape* getParent2();
	void SetParents(Shape* s1, Shape* s2);

	//Returns the intersection point...//
	Vector* getNearestPoint(double x, double y, double snapdistance, double z = 0, bool considerz = false);

	Vector* getNearestSnapPoint(std::list<Shape*> shapes, double x, double y, double snapdistance);
	Vector* getNearestSnapPoint(RCollectionBase& shapes, double x, double y, double snapdistance, bool ShapePasted, double z = 0, bool considerz = false);
	
	Vector* getNearestSnapPoint(RCollectionBase& shapes, double* Sline, double snapdistance, bool ShapePasted);
	Vector* getNearestSnapPointUCS(RCollectionBase& shapes, double* TransformMatrix, double x, double y, double snapdistance, bool ShapePasted);

	Vector* getNearestIntersectionPointOnShape(ShapeWithList* CurrentShape, double x, double y, double snapdistance, bool ShapePasted);
	Vector* getNearestIntersectionPointOnShape(ShapeWithList* CurrentShape, double* Sline, double snapdistance, bool ShapePasted);
	Vector* getNearestIntersectionPointOnShapeUCS(ShapeWithList* CurrentShape, double* TransformMatrix, double x, double y, double snapdistance, bool ShapePasted);
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
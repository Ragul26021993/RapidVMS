//Holds the DXF collections.. 
//Shape, Measurements.. Two two Collections..
// One to keep the original Parameters..
#pragma once
#include "..\Engine\RCollectionBase.h"
#include "..\Engine\SnapPtManager.h"
#include "..\Engine\BaseItem.h"

class DXFCollection:
	public BaseItem
{
private: //collection of shapes, measurements and snap points...//
	RCollectionBase ShapeCollection;
	RCollectionBase OriginalShapeCollection;
	RCollectionBase MeasureCollection;
	RCollectionBase OriginalMeasureCollection;
	SnapPtManager *dxfsnaps;
public:
	list< list<Shape*> > ClosedLoopCollections;
	//Constructor and destructor...//
	DXFCollection(TCHAR* myname);
	virtual ~DXFCollection();
	//Get the shape collection..//
	RCollectionBase& getShapeCollection();
	RCollectionBase& getOriginalShapeCollection();
	//Get the measurement collections...//
	RCollectionBase& getMeasureCollection();
	RCollectionBase& getOriginalMeasureCollection();
	//Get the snap point collection....//
	SnapPtManager& getIPManager();
	void ShapeArrangementForLoop();
};


//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
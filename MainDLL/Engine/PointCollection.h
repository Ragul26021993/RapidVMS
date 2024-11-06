//Used to maintain the Point Collection of entities.. Used in ShapeWith List..
//Stored in integer index base list.. We can say Hash table!
//Functions to Add/erase Points.. delete/ delete All etc..

#pragma once
#include <map>
#include "..\MainDLL.h"
#include "..\Shapes\SinglePoint.h"
using namespace std;

class MAINDLL_API PointCollection
{
public:
enum POINTARRANGEMENTTYPE
{
	NORMAL,
	FROMTOP,
	FROMLEFT,
	FROMRIGHT,
	FROMBOTTOM,
};

private:
	map<int, SinglePoint*> allitems; // Points Collection..
	SinglePoint* getFirstPoint(PointCollection::POINTARRANGEMENTTYPE type);

public:
	int index, SelectedPointsCnt;
	//Constructor, Destructor..
	PointCollection();
	~PointCollection();

	//Add/ Set the new point..
	void Addpoint(SinglePoint* Spt);
	void Addpoint(SinglePoint* Spt, int id);
	void SetPoint(SinglePoint* Spt, int Cindex);
	
	//Erase points
	void ErasePoint(int id);
	void EraseAll();
	
	//Delete points..
	void deletePoint(SinglePoint* Spt);
	void deletePoint(int id);
	void deleteAll();

	//Total Points count and point list..
	int Pointscount();
	map<int,SinglePoint*>& getList();

	void CopyAllPoints(PointCollection* OriginalColl);
	void ArrangePointsInOrder(PointCollection::POINTARRANGEMENTTYPE type);
	void drawAllpoints(double pointsize, double r, double g, double b);
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
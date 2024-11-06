//Used maintain the Intersection Point Collection.. In each Shape and in UCS..
//Stored in an index based list. (Hash table)
// Functions to Add/erase Points.. delete/ delete All etc..
#pragma once
#include <map>
#include "..\MainDLL.h"
#include "IsectPt.h"
using namespace std;
#define ISN_ITER map<int,IsectPt*>::iterator
class MAINDLL_API IsectPtCollection
{
private:
	map<int, IsectPt*> allitems; //Intersection Points Collection..
public:
	//Constructor, Destructor..
	IsectPtCollection();
	~IsectPtCollection();

	//Add/ Set the new point..
	void Addpoint(IsectPt* IsnPt);
	map<int,IsectPt*>& getList();

	//Erase points
	void ErasePoint(int id);
	void EraseAll();
	
	//Delete points..
	void deletePoint(int id);
	void deleteAll();
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
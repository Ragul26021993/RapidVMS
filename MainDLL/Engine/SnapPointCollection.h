//Created on March 2011... 2nd Week..
//Used maintain the Point Collection that is used in each shapes...
#pragma once
#include "..\MainDLL.h"
#include "SnapPoint.h"
using namespace std;

class MAINDLL_API SnapPointCollection
{
private:
	map<int, SnapPoint*> allitems;
public:
	//Constructor, Destructor..
	SnapPointCollection();
	~SnapPointCollection();
	//Add/ Set the new point..
	void Addpoint(SnapPoint* Snpt, int id);
	map<int,SnapPoint*>& getList();
};
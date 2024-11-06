#pragma once
#include <map>
#include "BaseItem.h"

class Vector;
class Shape;        

#define SPC_ITER map<int,SnapPoint*>::iterator
class MAINDLL_API SnapPoint
{
private:
	Shape* parent;
	Vector* pt;
public:
	int SPtId;
	bool IsValid;
	//Constructor and destructor..//
	SnapPoint(Shape* parent, Vector* pt, int id, bool flag = true);
	~SnapPoint();
	//Get the snap point...//
	Vector* getPt();
	//Get the parent
	Shape* getParent();
};
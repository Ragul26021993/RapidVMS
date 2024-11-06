#pragma once
#include "Circle.h"
#include "..\Engine\RCollectionBase.h"

class MAINDLL_API PCDCircle:
	public Circle
{
private:
	static int PCDshapenumber;
	TCHAR* genName(const TCHAR* prefix);
	RCollectionBase RemovedMeasureColl;
public:
	PCDCircle(TCHAR* myname,bool is3D=false);
	PCDCircle(bool simply);
	PCDCircle(PCDCircle* s);
	~PCDCircle();
	list<BaseItem*> PCDParentCollection;
	map<int, int> PCDShapePointRelation;
	//list<BaseItem*> PCDAllShapeCollection;

	void AddParentShapes(BaseItem* Csh);
	void RemoveParentShapes(BaseItem* Csh);
	void CalculatePcdCircle();
	void RemoveSelectedShapes();
	bool Is3D;
	void calculateAttributes();
	static void reset();

	//Write and read, to/from the file..//
	friend wostream& operator<<(wostream& os, PCDCircle& x);
	friend wistream& operator>>(wistream& is, PCDCircle& x);
	friend void ReadOldPP(wistream& is, PCDCircle& x);
};
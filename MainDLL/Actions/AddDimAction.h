//Class for measurement add action...
#pragma once
#include "RAction.h"

class DimBase;

class AddDimAction:
	public RAction
{
private:
	DimBase* dim; //Pointer to the Dimension
	bool PPStatusAction;
public:
	AddDimAction();
	virtual ~AddDimAction();

	//Set and get the current dim pointer..//
	void setDim(DimBase* dim);
	DimBase* getDim();

	//Virtaul Functions..
	virtual bool execute();
	virtual void undo();
	virtual bool redo();
	
	static void addDim(DimBase* dim, bool DisableAction = false);

	//Write and Read the values..
	friend wostream& operator<<(wostream&, AddDimAction&);
	friend wistream& operator>>(wistream&, AddDimAction&);
	friend void ReadOldPP(wistream& is, AddDimAction& action);
};
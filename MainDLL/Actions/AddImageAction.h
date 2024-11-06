//Class to add the Probe Point action..
#pragma once
#include "RAction.h"

class MAINDLL_API AddImageAction:
	public RAction
{
private:
	FramegrabBase* baseFG;	
public:
	int ImageCount;
	bool PausableAction;
	AddImageAction();
	virtual ~AddImageAction();

	//Add and remove the point action..//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();
	
	//Get the base action..//
	FramegrabBase* getFramegrab();

	static void AddImage_DroValue(FramegrabBase* v);

	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, AddImageAction&);
	friend wistream& operator>>(wistream&, AddImageAction&);
};
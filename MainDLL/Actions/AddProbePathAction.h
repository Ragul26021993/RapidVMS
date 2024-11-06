//Class to add the Probe Point action..
#pragma once
#include "RAction.h"

class AddProbePathAction:
	public RAction
{
private:
	FramegrabBase* baseFG;
public:
	bool PausableAction;
	AddProbePathAction();
	virtual ~AddProbePathAction();

	//Add and remove the point action..//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();
	
	//Get the base action..//
	FramegrabBase* getFramegrab();

	static void AddProbePath(FramegrabBase*);

	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, AddProbePathAction&);
	friend wistream& operator>>(wistream&, AddProbePathAction&);
	friend void ReadOldPP(wistream& is, AddProbePathAction& action);
};
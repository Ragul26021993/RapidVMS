//Class to handle the delete measurment action..//
#pragma once
#include "Raction.h"

class DeleteMeasureAction:
	public RAction
{
public:
	//Constructor and destructor...//
	DeleteMeasureAction();
	~DeleteMeasureAction();
	void SelectParentMeasurements(RCollectionBase &Measures);
	//list of measurements that were deleted
	RCollectionBase *delmeasure;
	
	//Add / remove the action from the list..//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();
};
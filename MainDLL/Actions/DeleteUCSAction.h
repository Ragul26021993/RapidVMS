//Class to handle the delete ucs action..//
#pragma once
#include "RAction.h"
class DeleteUCSAction:
	public RAction
{
private:
	BaseItem* ucs; //the deleted UCS
	RCollectionBase *delucschangeact;
public:
	//Constructor and destructor..//
	DeleteUCSAction();
	virtual ~DeleteUCSAction();

	//Add/ remove the delete ucs action..
	virtual bool execute();
	virtual void undo();
	virtual bool redo();
};
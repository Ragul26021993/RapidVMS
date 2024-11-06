//Class to handle the delete shape action..
#pragma once
#include "Raction.h"
#include "..\Shapes\PCDCircle.h"

class DeleteShapesAction:
	public RAction
{
public:
	//Constructor and destrcutor..//
	DeleteShapesAction();
	~DeleteShapesAction();
	//Delete the related shape..//
	void deleteRelatedShape(Shape*);
	void deleteRelatedMeasure(Shape*);
	void removeChild(Shape*);

	//list of shapes that were deleted
	RCollectionBase *delshapes;
	RCollectionBase *delMeasurement;

	//Add / Remove the deleshape action..//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();
};
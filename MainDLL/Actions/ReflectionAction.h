//Class to handle the Copy shape action..
#pragma once
#include "Raction.h"

class Shape;

class ReflectionAction:
	public RAction
{
	
public:
	ReflectionAction();
	~ReflectionAction();

	map<int, Shape*> ReflectedShapes;
	list<int> SelectedshapesId, ReflectedShapesId;
	int ReflectionSurfaceId;
	void AddshapesColl(list<int>selectedShapeId, list<int>reflectedShapeId, int reflectionSurfaceId);

	//Add / Remove the deleshape action..//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, ReflectionAction&);
	friend wistream& operator>>(wistream&, ReflectionAction&);
	friend void ReadOldPP(wistream& is, ReflectionAction& action);
};
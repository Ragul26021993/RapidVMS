#pragma once
#include "Raction.h"

class Shape;

class RotationAction:
	public RAction
{
	
public:
	RotationAction();
	~RotationAction();
	map<int, Shape*> RotatedShapes; 
	list<int> RotatedShapesId;
	int RotationAxisId;
	Vector *RotationPoint;
	double Angle, RotateMatrix[9], RotationOrigin[3];
	void AddshapesColl(list<int>rotatedShapeId, int rotationAxisId);
	
	//Add / Remove the deleshape action..//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, RotationAction&);
	friend wistream& operator>>(wistream&, RotationAction&);
	friend void ReadOldPP(wistream& is, RotationAction& action);
};
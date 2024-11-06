//class to handle the move shape action.../
#pragma once
#include "Raction.h"

class Shape;

class MoveShapesAction :
	public RAction
{
	//only thing needed to undo the movement
private:
	Vector movement;
	std::list<Shape*> MoveShapeList;
	bool doaction, isNudge, WriteActionInPPFlag;
	double angle;
public:
	//Constructor and destructor....//
	MoveShapesAction();
	~MoveShapesAction();

	//Get the Collection of Shapes...//
	std::list<Shape*> getShape();
	// Gets the current position...
	Vector& getPosition();

	//Add / remove the action...//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	//move selected shapes...//
	static void moveShapes(Vector &v,std::list<Shape*> collection, bool doaction,bool isNudge = true,double angle = 0);
	static void SetActionStatus();

	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, MoveShapesAction&);
	friend wistream& operator>>(wistream&, MoveShapesAction&);	
	friend void ReadOldPP(wistream& is, MoveShapesAction& action);

};
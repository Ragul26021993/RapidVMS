//Class to add the add shape action..//
#pragma once
#include "RAction.h"

class Shape;

class MAINDLL_API AddShapeAction:
	public RAction
{
//Pointer to the shape.//
private:
	Shape* shape;
	bool PPStatusAction;
public:
	//Constructor and destructor..//
	AddShapeAction();
	virtual ~AddShapeAction();

	// Set the current shape..//
	void setShape(Shape* shape);

	//Add and remove the current shape..tp shapeList..
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	//returns the current selected shape..//
	Shape* getShape();
	static void addShape(Shape* shape, bool SelectShape = true, bool DisableAction = false);

	//Read/ write the partprogram...
	friend wostream& operator<<(wostream&, AddShapeAction&);
	friend wistream& operator>>(wistream&, AddShapeAction&);
	friend void ReadOldPP(wistream& is, AddShapeAction& action);
};
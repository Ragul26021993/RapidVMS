//Class to add the add perimeter action..//
#pragma once
#include "RAction.h"

class ShapeWithList;
class Shape;

class MAINDLL_API PerimeterAction:
	public RAction
{
//Pointer to the shape.//
private: //Framegrab base..
	FramegrabBase* baseFG;
	ShapeWithList* shape; // Pointer to the shape..
	Shape* PerimeterShape;
	
public:
	bool PausableAction;
	PerimeterAction();
	PerimeterAction(Shape* PerimeterShape, ShapeWithList* s, FramegrabBase* v);
	virtual ~PerimeterAction();

	//Virtual functions..
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	//Add the point action to the current selected shape..//
	static void AddPerimeterAction(Shape* PerimeterShape, ShapeWithList* s, FramegrabBase* v);
	//Function to set the shape and point properties..//
	void setParams(Shape* PerimeterShape, ShapeWithList* s, FramegrabBase* v);

	//Get the base action..//
	FramegrabBase* getFramegrab();
	//Get the current selected shape..//
	ShapeWithList* getShape();

	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, PerimeterAction&);
	friend wistream& operator>>(wistream&, PerimeterAction&);

};
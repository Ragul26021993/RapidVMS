//Class to add the Thread Point action..
#pragma once
#include "RAction.h"

class ShapeWithList;
class FramegrabBase;

class AddThreadPointAction:
	public RAction
{
private: //Framegrab base..
	FramegrabBase* baseFG;
	ShapeWithList* shape; // Pointer to the shape..
public:
	//Constructor and destructor...//
	AddThreadPointAction(void);
	AddThreadPointAction(ShapeWithList* s, FramegrabBase* v);
	virtual ~AddThreadPointAction();

	//Virtual functions..
	virtual bool execute();
	virtual void undo();
	virtual bool redo();
	
	//Add the point action to the current selected shape.
	static void ThreadpointAction(ShapeWithList* s, FramegrabBase* v);
	//Function to set the shape and point properties..//
	void setParams(ShapeWithList* s, FramegrabBase* v);
	//Get the base action..//
	FramegrabBase* getFramegrab();
	//Get the current selected shape..//
	ShapeWithList* getShape();

	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, AddThreadPointAction&);
	friend wistream& operator>>(wistream&, AddThreadPointAction&);
	friend void ReadOldPP(wistream& is, AddThreadPointAction& action);
};
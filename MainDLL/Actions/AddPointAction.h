//Class to add the Point action..
#pragma once
#include "RAction.h"

class ShapeWithList;

class MAINDLL_API AddPointAction:
	public RAction
{
private: //Framegrab base..
	FramegrabBase* baseFG;
	ShapeWithList* shape; // Pointer to the shape..
	
public:
	bool CriticalAction, PausableAction, AddToShape;
	AddPointAction();
	AddPointAction(ShapeWithList* s, FramegrabBase* v);
	virtual ~AddPointAction();

	//Virtual functions..
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	//Add the point action to the current selected shape..//
	static void pointAction(ShapeWithList* s, FramegrabBase* v, bool addtoshape = true);
	//Function to set the shape and point properties..//
	void setParams(ShapeWithList* s, FramegrabBase* v);

	//Get the base action..//
	FramegrabBase* getFramegrab();
	//Get the current selected shape..//
	ShapeWithList* getShape();

	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, AddPointAction&);
	friend wistream& operator>>(wistream&, AddPointAction&);
	friend void ReadOldPP(wistream& is, AddPointAction& action);
};
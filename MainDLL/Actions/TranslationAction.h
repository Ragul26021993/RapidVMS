//Class to handle the Copy shape action..
#pragma once
#include "Raction.h"

class TranslationAction:
	public RAction
{
	
public:
	TranslationAction();
	~TranslationAction();
	list<int> TranslatedShapesId;
	Vector TranslateDirection;
	
	//Add / Remove the deleshape action..//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, TranslationAction&);
	friend wistream& operator>>(wistream&, TranslationAction&);
	friend void ReadOldPP(wistream& is, TranslationAction& action);
};
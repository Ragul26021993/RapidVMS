//class to handle the Trim shape action.../
#pragma once
#include "Raction.h"

class Shape;

class EditCadAction:
	public RAction
{
	int d_ucsid;
public:
	std::list<RAction*> EditedAlignMentAction;
	std::list<Shape*> EditedShapes;

	EditCadAction(int ucsid);
	~EditCadAction();

	virtual bool execute();
	virtual void undo();
	virtual bool redo();
};

//Class to handle the merge shape action of lines and arcs created through line-arc separation..
#pragma once
#include <map>
#include "RAction.h"

class Shape;

class MergeShapesAction:
	public RAction
{
private:
	bool line_arc;
	bool closed_loop;
	bool shape_as_fasttrace;
public:
	//Constructor and destrcutor..//
	MergeShapesAction(bool ln_arc, bool cl_lp, bool shp_as_ft);
	~MergeShapesAction();

	//list of shapes that were deleted
	std::map<int, Shape*> delshapes;
	//shape added
	Shape* cshape;

	virtual bool execute();
	virtual void undo();
	virtual bool redo();
};
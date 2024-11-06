#pragma once
#include "RAction.h"

class Shape;
class MAINDLL_API AreaShapeAction:
	public RAction
{
private:
	FramegrabBase* baseFG;	
	double AreaValue;
	Shape* ShapePtr;
public:
	bool PausableAction;
	AreaShapeAction();
	virtual ~AreaShapeAction();

	//Add and remove the point action..//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();
	Shape* getShape();
	//Get the base action..//
	FramegrabBase* getFramegrab();

	static void AddAreaShapeDroValue(Shape* Cshape, FramegrabBase* v, double value);

	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, AreaShapeAction&);
	friend wistream& operator>>(wistream&, AreaShapeAction&);
};
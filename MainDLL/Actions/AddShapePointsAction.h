#pragma once
class Shape;
#include "Raction.h"

class AddShapePointsAction:
	public RAction
{
private :
	bool PPStatusAction;
	Shape* shape;
public:	
	list<Shape*> AddedShapesId;
	int AddedShapeCount;

	// Set the current shape..//
	void setShape(Shape* shape);

	Shape* getShape();

	AddShapePointsAction(void);
	~AddShapePointsAction(void);
	
	
	//Add / Remove the deleshape action..//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, AddShapePointsAction&);
	friend wistream& operator>>(wistream&, AddShapePointsAction&);
};


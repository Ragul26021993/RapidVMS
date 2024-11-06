//Class to handle the Copy shape action..
#pragma once
#include "Raction.h"

class Shape;

class CopyShapesAction:
	public RAction
{
	
public:
	CopyShapesAction();
	~CopyShapesAction();
	//list of shapes that were deleted
	std::list<Shape*> copiedshapes;
	std::list<Shape*> PastedShapes;
	list<int> PastedShapesId;
	int sourceUcsId, targetUcsId;
	static void setCopiedShape(std::list<Shape*> CurrentCopiedShapeList, int sourceId, int targetId);
	void AddshapesColl(std::list<Shape*> CurrentCopiedShapeList);
	void CopyShapesDuringPartProgram();

	//Add / Remove the deleshape action..//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, CopyShapesAction&);
	friend wistream& operator>>(wistream&, CopyShapesAction&);
	friend void ReadOldPP(wistream& is, CopyShapesAction& action);
};
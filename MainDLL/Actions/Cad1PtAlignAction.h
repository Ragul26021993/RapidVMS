//class to handle the Trim shape action.../
#pragma once
#include "Raction.h"

class Shape;

class Cad1PtAlignAction:
	public RAction
{
	//only thing needed to undo and redo the shapes...
	Vector OriginalPos, Point1;
	
public:

	Vector* TargetPnt;
	bool IsEdited;
	int UcsID, windowNo;
    std::map<int, Shape*> AlignedShape;

	//Constructor and destructor....//
	Cad1PtAlignAction();
	~Cad1PtAlignAction();

	//Add / remove the action...//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	static void SetCad1PtAlign(Vector& pt, Vector* v, int ucsID, int windowno, std::list<Shape*> DXFShapeList);
	Vector* getPoint1();
	void SetPoint1(Vector& pt1);
	int getUcsId();
	
    void EditCurrentAlignment(map<int, int> shapemap);
	void TranslateShape(Vector& shift);

	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, Cad1PtAlignAction&);
	friend wistream& operator>>(wistream&, Cad1PtAlignAction&);	

};

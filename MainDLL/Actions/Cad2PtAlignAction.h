//class to handle the Trim shape action.../
#pragma once
#include "Raction.h"

class Shape;

class Cad2PtAlignAction:
	public RAction
{
	//only thing needed to undo and redo the shapes...
	Vector OriginalPnts[2];
   
   
public:
	bool IsEdited, action1Finished, action2Finished;
	int UcsId, clickWindow1, clickWindow2;
	Vector *targetP1, *targetP2;
	Vector Points[2];
	std::map<int, Shape*> AlignedShape;

	//Constructor and destructor....//
	Cad2PtAlignAction();
	~Cad2PtAlignAction();

	//Add / remove the action...//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	static void SetCad2PtAlign(Vector& pt1, Vector& pt2, Vector *targetPnt1, Vector *targetPnt2, int ucsId, int *windowNo, std::list<Shape*> DXFShapeList);
	
	int getUcsId();
	Vector* getPoint1();
	Vector* getPoint2();
	
	void SetPoint1(Vector& pt1);
	void SetPoint2(Vector& pt1);

	void EditCurrentAlignment(map<int, int> shapemap);
	void TranslateShape(Vector& shift);
	void TransformShape(double* matrix);

	friend wostream& operator<<(wostream&, Cad2PtAlignAction&);
	friend wistream& operator>>(wistream&, Cad2PtAlignAction&);	
};

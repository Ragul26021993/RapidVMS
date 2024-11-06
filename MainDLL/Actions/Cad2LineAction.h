#pragma once
#include "Raction.h"

class Shape;

class Cad2LineAction:
	public RAction
{
	
public:

	bool IsEdited;
	int UcsID, windowNo;
    std::map<int, Shape*> AlignedShape;
	Shape *DxfShape1, *DxfShape2, *MainShape1, *MainShape2;

	//Constructor and destructor....//
	Cad2LineAction();
	~Cad2LineAction();

	//Add / remove the action...//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	static void SetCad2LineAlign(Shape *dxfShape1, Shape *dxfShape2, Shape *mainShape1, Shape *mainShape2, int UcsID, int windowno, std::list<Shape*> DXFShapeList);
		
	int getUcsId();

	void EditCurrentAlignment(map<int, int> shapemap);
	void TranslateShape(Vector& shift);
	void TransformShape(double* matrix);

	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, Cad2LineAction&);
	friend wistream& operator>>(wistream&, Cad2LineAction&);		
};

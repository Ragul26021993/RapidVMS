#pragma once
#include "Raction.h"

class Shape;

class Cad1Pt1LineAction:
	public RAction
{
	Vector OldPoint;

public:

	Vector Point, *targetP;
	Shape *DxfShape, *MainShape;
	bool IsEdited;
	int UcsID, windowNo;
    std::map<int, Shape*> AlignedShape;

	//Constructor and destructor....//
	Cad1Pt1LineAction();
	~Cad1Pt1LineAction();

	//Add / remove the action...//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	static void SetCad1Pt1LineAlign(Shape *dxfShape, Shape *videoShape, Vector& pt, Vector* v, int UcsID, int windowno, std::list<Shape*> DXFShapeList);

	Vector* getPoint1();
	void SetPoint1(Vector& pt1);
	int getUcsId();
	
	void EditCurrentAlignment(map<int, int> shapemap);
	void TranslateShape(Vector& shift);
	void TransformShape(double* matrix);

	friend wostream& operator<<(wostream&, Cad1Pt1LineAction&);
	friend wistream& operator>>(wistream&, Cad1Pt1LineAction&);	
	

};
//Created by Rahul Singh Bhadauria on 10 Aug 12........
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
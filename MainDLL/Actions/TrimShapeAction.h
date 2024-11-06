//class to handle the Trim shape action.../
#pragma once
#include "Raction.h"

class Shape;
class Vector;

class TrimShapeAction:
	public RAction
{
	//only thing needed to undo and redo the shapes...
	RapidEnums::SHAPETYPE PreviousShapeType, CurrentShapeType;
    RapidEnums::CIRCLETYPE PreviousCircleType, CurrentCircleType;
	RapidEnums::LINETYPE PreviousLineType, CurrentLineType;
	Vector PreviousEndPt1, PreviousEndPt2, CurrentEndPt1, CurrentEndPt2;
	int ShapeId;
public:
	//Constructor and destructor....//
	TrimShapeAction();
	~TrimShapeAction();

	//Add / remove the action...//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	//Trimfunction for Line.......
	static void TrimShape(Vector &PrevEpoint1, Vector &PrevEpoint2, RapidEnums::SHAPETYPE PreSType, RapidEnums::LINETYPE preLtype, Shape* currentShape);

	//Trimfunction for circle.......
	static void TrimShape(Vector &PrevEpoint1, Vector &PrevEpoint2, RapidEnums::SHAPETYPE PreSType, RapidEnums::CIRCLETYPE preCtype, Shape* currentShape);
	
};
//Created by Rahul Singh Bhadauria on 14 Jun 12........
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
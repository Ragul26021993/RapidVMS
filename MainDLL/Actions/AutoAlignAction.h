#pragma once
#include "Raction.h"

class AutoAlignAction:
	public RAction
{
	bool IsSelectedShapes;
	int AlignmentMode;
	std::list<int> IDlist;
public:
	//Constructor and destructor....//
	AutoAlignAction();
	~AutoAlignAction();

	//Add / remove the action...//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	static void SetAutoAlignAction(std::list<int>* ShapeIdList, bool selectedShapes, int Alignment_mode);
	bool getshapeSelectionFlag();
	int getAlignmentMode();
	void getShapeIdList(std::list<int>* ShapeIdList);
	
	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, AutoAlignAction&);
	friend wistream& operator>>(wistream&, AutoAlignAction&);	
	friend void ReadOldPP(wistream& is, AutoAlignAction& action);
	
};
//Created by Rahul Singh Bhadauria on 15 Jul 12........
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
#pragma once
#include "Raction.h"

class EditCad2LineAction:
	public RAction
{
	std::list<int> IDList;
public:
	//Constructor and destructor....//
	EditCad2LineAction();
	~EditCad2LineAction();

	//Add / remove the action...//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	static void SetCad2LineAlign(std::list<int> *ShapeIdList);
	void getLineIdList(map<int, int>* ShapeIdList);
	
	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, EditCad2LineAction&);
	friend wistream& operator>>(wistream&, EditCad2LineAction&);	
	friend void ReadOldPP(wistream& is, EditCad2LineAction& action);
	
};
//Created by Rahul Singh Bhadauria on 27 Jul 12........
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
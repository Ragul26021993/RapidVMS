#pragma once
#include "Raction.h"

class EditCad1PtAction:
	public RAction
{
	Vector Point1;
	FramegrabBase* baseFG;
	int UcsID;
public:
	//Constructor and destructor....//
	EditCad1PtAction();
	~EditCad1PtAction();

	//Add / remove the action...//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	static void SetCad1PtAlign(Vector& pt, FramegrabBase* v, int UcsID);
	Vector* getPoint1();
	void SetPoint1(Vector& pt1);
	int getUcsId();
	
	FramegrabBase* getFramegrab();
	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, EditCad1PtAction&);
	friend wistream& operator>>(wistream&, EditCad1PtAction&);	
	friend void ReadOldPP(wistream& is, EditCad1PtAction& action);
	
};
//Created by Rahul Singh Bhadauria on 15 Jul 12........
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
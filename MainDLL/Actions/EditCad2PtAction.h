#pragma once
#include "Raction.h"

class EditCad2PtAction:
	public RAction
{
	Vector Point1, Point2;
	int ActionCnt;
	FramegrabBase* baseFG;
public:
	//Constructor and destructor....//
	EditCad2PtAction();
	~EditCad2PtAction();

	//Add / remove the action...//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	static void SetCad1PtAlign(Vector& pt, FramegrabBase* v, bool firstAction);
	Vector* getPoint1();
	Vector* getPoint2();
	void SetPoint1(Vector& pt1);
	void SetPoint2(Vector& pt2);
	int getActionCount();
	
	FramegrabBase* getFramegrab();
	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, EditCad2PtAction&);
	friend wistream& operator>>(wistream&, EditCad2PtAction&);	
	friend void ReadOldPP(wistream& is, EditCad2PtAction& action);
	
};
//Created by Rahul Singh Bhadauria on 15 Jul 12........
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
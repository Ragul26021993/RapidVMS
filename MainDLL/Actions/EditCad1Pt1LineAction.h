#pragma once
#include "Raction.h"

class EditCad1Pt1LineAction:
	public RAction
{
	Vector Point;
	double ShapeId[2];
	FramegrabBase* baseFG;
	int UcsID;
	bool firstActionFlag;
public:
	//Constructor and destructor....//
	EditCad1Pt1LineAction();
	~EditCad1Pt1LineAction();

	//Add / remove the action...//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	static void SetCad1Pt1LineAlign(double* Shpid, int UcsID);
	static void SetCad1Pt1LineAlign(Vector& pt, FramegrabBase* v, int UcsID);
	Vector* getPoint1();
	void SetPoint1(Vector& pt1);
	int getUcsId();
	void getShapeId(double* ShpID);
	bool getActionFlag();
	
	FramegrabBase* getFramegrab();
	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, EditCad1Pt1LineAction&);
	friend wistream& operator>>(wistream&, EditCad1Pt1LineAction&);	
	friend void ReadOldPP(wistream& is, EditCad1Pt1LineAction& action);
	
};
//Created by Rahul Singh Bhadauria on 10 Aug 12........
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
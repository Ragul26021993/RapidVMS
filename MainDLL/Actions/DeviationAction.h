#pragma once
#include "Raction.h"

class DeviationAction:
	public RAction
{
	//in this vector 4th parameter is used as boolean........
	//upperTolerance, Lowertolerance, interval, deviationwithYellowShapes, interval type...
	Vector Dparameter;
	std::list<int> IDlist;
	std::list<int> LineIDlist;
	bool TwoDdeviation, BestfitArcflag;
	int measureRefrenceAxis;  
public:
	//Constructor and destructor....//
	DeviationAction();
	~DeviationAction();

	//Add / remove the action...//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();
	static void SetDeviationParam(Vector& pt, std::list<int>* SelectedShapeIdList, std::list<int>* LineIdList, bool TwoDdeviation, int measureRefrenceAxis, bool BestfitArc);
	Vector* GetDeviationParam();
	void getShapeIdList(std::list<int>* ShapeIdList, bool* BestfitFlag, int* measureRefrenceAxis);
	void getLineIdList(map<int, int>* ShapeIdList);
	bool getdeviationTypeFlag();
	void SetTolerance(double Uppertolerance, double Lowertolerance);

	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, DeviationAction&);
	friend wistream& operator>>(wistream&, DeviationAction&);	
	friend void ReadOldPP(wistream& is, DeviationAction& action);
};
//Created by Rahul Singh Bhadauria on 19 Jul 12........
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
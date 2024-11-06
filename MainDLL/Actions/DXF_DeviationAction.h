#pragma once
#include <list>
#include "Raction.h"

class DimBase;
class Shape;

class DXF_DeviationAction:
	public RAction
{
	std::list<Shape*> DeviationShape;
	std::list<DimBase*> DeviationMeasure;
	std::list<int> IDlist;
	bool TwoD_Deviation, ClosedLoop;
	double tolerance, interval, UpperCutOffMeasurement;
	int IntervalType, UcsId;
	
public:
	//Constructor and destructor....//
	DXF_DeviationAction();
	~DXF_DeviationAction();

	//Add / remove the action...//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();
	static void SetDXF_DeviationParam(double UpperCutoff, double tolerance, double interval, int IntervalType, bool ClosedLoop, bool TwoD_Deviation, std::list<int>* SelectedShapeIdList, std::list<Shape*> CreatedShapeList, std::list<DimBase*> CreatedMeasureList);
	void GetDXF_DeviationParam(std::list<int>* SelectedShapeIdList, std::list<int>* LineIdList, double* UpperCutoff, double* tolerance, double* interval, int* IntervalType, bool* ClosedLoop,  bool* TwoD_Deviation);
	void SetTolerance(double tolerance);
	int getUcsId();
	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, DXF_DeviationAction&);
	friend wistream& operator>>(wistream&, DXF_DeviationAction&);	
};
//Created by Rahul Singh Bhadauria on 3 Dec 12........
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
#pragma once
#include "MouseHandler.h"
#include "../Shapes/SinglePoint.h"

class Shape;
class DimBase;

class DXFDeviationHandler :
	public MouseHandler
{
 class ShapeId_PointsCollection
{
public:
	std::map<int, SinglePoint*> PtsList;
	std::map<int, SinglePoint*> SortedPtsList;
	int Count;
	ShapeId_PointsCollection()
	{ this->Count = 0;}
	void Addpoint(SinglePoint* Spt)
	{ PtsList[Count++] = Spt;}
	~ShapeId_PointsCollection()
	{
		while(PtsList.size() != 0)
		{
			PC_ITER i = PtsList.begin();
			SinglePoint* Spt = (*i).second;
			PtsList.erase(Spt->PtID);
			delete Spt;
		}
		
		while(SortedPtsList.size() != 0)
		{
			PC_ITER i = SortedPtsList.begin();
			SinglePoint* Spt = (*i).second;
			SortedPtsList.erase((*i).first);
			delete Spt;
		}
	}
};
 struct UpdatedPts
 {
	 double EndPts[6];
	 bool negativeMeasurement;
 };
	std::map<int, UpdatedPts> UpdatedPtsList;
	ShapeId_PointsCollection* SptCollection;
	std::list<Shape*> CreatedShapeidlist;
	std::list<DimBase*> createdMeasureIdlist;
	bool runningPartprogramValid;
	void CreateSortedPts_ShapeIdRelation(bool TwoD_Deviation);
	void CreateSortedPtsList(bool TwoD_Deviation, double interval);
	bool CalculateDeviation(double UpperCutoff, double tolerance, double interval, int IntervalType, bool ClosedLoop, bool TwoD_Deviation);
	bool getNearestIntersectionpts(Shape* cshape, double* Pnts, double* intersectPts);
	void findAveragePoint(double* points, double CircleRad, std::map<int, SinglePoint*>* ArrangedPtsList, SinglePoint* DeviationPt);
	void Create2DLine_WithoutAction(double *Point1, double *Point2);
	void UpdateDXFDeviationParam(double UpperCutoff, std::list<int> LineIdlist, double tolerance, double interval, int IntervalType, bool ClosedLoop, bool TwoD_Deviation);
	
public:
	DXFDeviationHandler();
	~DXFDeviationHandler();
	void DXFDeviationData(double UpperCutoff, double tolerance, double interval, int IntervalType, bool ClosedLoop, bool TwoD_Deviation);
	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	//Virtual funtion to handle partprogram...//
	virtual void PartProgramData();
	void UpdateMeasurementAfterProgramRun();
};
// Created  by Rahul Singh Bhadauria on 29 Nov 12........
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
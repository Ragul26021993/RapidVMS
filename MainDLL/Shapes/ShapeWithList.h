//Class to handle the add point, remove points , 
//call the update of the current shape and to keep tack of all the points taken for shapes
//Class is derived form the base class shape..//
#pragma once
#include "Shape.h"
#include <list>
#include <vector>
	
class IsectPtCollection;
class PointCollection;
class SnapPointCollection;
class FramegrabBase;

class MAINDLL_API ShapeWithList:
	public Shape
{
private:
	void writePointsToHarddisc(FramegrabBase* CurrentFG);
	void writePointsToHarddisc(PointCollection* PtsList);
	
protected:
	void init();
	
public:
	//Enumeration type for point add/ remove..//
	enum PointOperations
	{
		ADD_POINT, 
		REMOVE_POINT
	};
	SnapPointCollection *SnapPointList; //Shape End point Collection
	PointCollection *PointsList; //Added points Collection
	PointCollection *PointsListOriginal; //Added points Copy Collection
	PointCollection *RemovedPointsList; //Removed, Undone points Collection
	PointCollection *RemovedPointsListOriginal; //Removed, Undone points Collection
	IsectPtCollection *IsectPointsList; //Intersection Points Collection
	list<BaseItem*> PointAtionList; //List To hold Pointer to the Actions added to this Shape 
	double *pts, *filterpts; //Double pointer to store the points according to projection type..//
	double RAxisVal;
	int SkipPtscount, Stepcount, filterptsCnt;
	BaseItem *CurrentShapeAddAction;

public:
	//Constructor
	ShapeWithList(TCHAR* name);
	ShapeWithList(bool simply);
	virtual ~ShapeWithList();
	void deleteAllPoints();
	//points list related operations
	void ManagePoint(FramegrabBase* CurrentFG, PointOperations);
	void Modify_Points(FramegrabBase* CurrentFG, PointOperations);
	void AddPoints(PointCollection* PtsList);
	void Delete_Points(list<int> *PtIdList);
	void Modify_Point(int id, double x, double y, double z);
	void UpdateShape(bool notifyall = true);
	void ResetCurrentParameters();
	void ResetCurrentPointsList();
	void CalculateEnclosedRectangle();
	//Draw Frame grabbed points..
	void drawFGpoints();
	void drawPointCrossMark(double wupp);
	//Do the line arc seperation.. And apply the changes..
	void DotheShapeUpdate();
	void DoLineArcSeperation();
	void TransformShapePoints(double* Transform);
	void GetDownSampledPoints();
	double Get2DShapeZLevel(PointCollection *Pointcol1, PointCollection *PointCol2, int ordinate);
};
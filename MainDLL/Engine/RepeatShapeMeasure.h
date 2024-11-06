#pragma once
#include "..\MainDLL.h"
#include <map>
#include <list>
#include "PointCollection.h"

class Shape;

#define AUTOSHAPEMEASUREOBJECT RepeatShapeMeasure::GetRSMInstance()

class MAINDLL_API RepeatShapeMeasure
{
	RepeatShapeMeasure();
	~RepeatShapeMeasure();
	static RepeatShapeMeasure* _RSMInstance;
	void initializeVariable();

public:
	static bool CheckRSMInstance();
	static RepeatShapeMeasure* GetRSMInstance();
	static void DeleteInstance();
	void ClearAll();
	void AutoMeasurementFor_IdenticalShapeMeasure(std::list<double> *cord_list);
	void InitialiseNextGrid();
	void SetOrientationOfComponent();

#pragma region All Callbacks
//0 for AutomatedMeasureModeFlag false, 1 means shape has been created
void (CALLBACK *ResetParam_RepeatShapeMeasure)(int id);
#pragma endregion

private:
	double Theta, Transform_Matrix[9];
	PointCollection Coord_points;
	map<int, int> ShapeMeasure_relation;
	std::list<Shape*>::iterator  Shapeitr;
	int SptCount;	
	//get coordinate from list of coordinate......
	void getCoordinateFor_NextShape();
	//get shape from list of shapes.......
    void getShape_Parameter(bool firsttime);
	//create new framgrab for selected shape......
	void create_newFramgrab(bool firsttime);
	void UpdateDerivedShape();
	void CreateActionForShapes();
	void CreateMeasurement();
};


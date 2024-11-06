#pragma once
#include "..\MainDLL.h"
#include <map>
#include <list>
//#include "..\Handlers\MouseHandler.h"

class Vector;
class Shape;
class BaseItem;

#define AUTOMEASUREOBJECT RepeatAutoMeasurement::GetRepeatAutoMeasureInstance()

class MAINDLL_API RepeatAutoMeasurement
{
	RepeatAutoMeasurement();
	~RepeatAutoMeasurement();
	static RepeatAutoMeasurement* _RepeatAutoMeasureInstance;
	int NumberOfSteps, StepsCount;
	bool AutoFocusEnable;
	double StepAngle, transformMatrix[9], LastTargetSend[3];
	std::list<Shape*>::iterator  Shapeitr;
	std::list<BaseItem*>::iterator Framgrabitr;
	std::list<BaseItem*> ActionList;
	map<int, int> ShapeMeasure_relation;

	void initializeVariable();
	void GetNextShape();
	void getShape_Parameter(bool firsttime);
	void create_newFramgrab(bool firsttime);
	void UpdateDerivedShape();
	void CreateMeasurement();
	
public:
	double pointOfRotation[3];
	bool TakeReferencePointFlag, PtOfRotationFlag;
	static bool CheckRepeatAutoMeasureInstance();
	static RepeatAutoMeasurement* GetRepeatAutoMeasureInstance();
	static void DeleteInstance();
	void ClearAll();
	void StartAutoMeasuremnet(double Angle, int Steps, bool AutoFocus);
	void get_FramGrabParameter();
	void ContinueAutoMeasurement();

#pragma region All Callbacks
	//0 for AutomatedMeasureModeFlag and 1 for PtOfRotationFlag = false and 2 for PtOfRotationFlag = true
	void (CALLBACK *ResetParam_CircularRepeatMeasure)(int id);
#pragma endregion
};


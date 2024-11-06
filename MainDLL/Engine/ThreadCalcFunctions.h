//Class to handle the Thread Module Flow..
//New thread initialisation..
//Graphics required etc...
//Modification on 2/10/2011
#pragma once
#include "..\MainDLL.h"
#include "..\Shapes\Shape.h"
#include "..\Measurement\DimBase.h"
#include "..\Actions\AddDimAction.h"
#include "..\Actions\AddThreadMeasureAction.h"
#include "RWindow.h"

class ShapeWithList;

class MAINDLL_API ThreadCalcFunctions
{

private:
#pragma region Shape Initialisation
	//Line shape for thread drawing
	Shape* LineForThread(double x1, double y1, double x2, double y2, int id, int Actid, bool forthreadwindow);
	//Arc for thread drawing
	Shape* ArcForThread(double x1, double y1, double x2, double y2, double x3, double y3, int id, int Actid, bool forthreadwindow);
	void CreateShapes_MainCollection();
#pragma endregion
	//To Lock the resource..
	bool Thread_GraphicsUpdating;
	RapidProperty<bool> TopSurfaceFlat;
	RapidProperty<bool> RootSurfaceFlat;
	int ThCnt;
	friend void ThreadGraphics_Thread(void *anything); //Video Graphics...
public:
	//Constructor ..
	ThreadCalcFunctions();
	//Destructor...
	~ThreadCalcFunctions();

void AddPointsToMainCollectionShape(ShapeWithList* shape, bool AddShape = false);

#pragma region All the Flags
	//All the flags used..
	RapidProperty<bool> IncludeMajorMinroDiaMeasure; //Include Major minr dia
	RapidProperty<bool> ExternalThreadMeasure; //External thread measure
	RapidProperty<bool> ThreadMeasurementMode; //Thread Measurement mode
	RapidProperty<bool> ThreadMeasurementModeStarted; //Thread measure mode started
	RapidProperty<bool> ThreadMeasurementModeStartedFromBegin; 
	RapidProperty<bool> ThreadShapeHighlighted;
	RapidProperty<bool> DontUpdatethreadGraphics;
	RapidProperty<bool> ThreadGraphics_Updated;
	
#pragma endregion
	
#pragma region Shape, Measure, Action
	//Current Thread Shape...
	Shape *nearestThreadShape;
	//Thread Graphics Module Instance..
	RGraphicsWindow* ThGraphics;
	//Thread Graphics window..
	//RWindow ThreadWindow;
	//Current thread measure action
	AddThreadMeasureAction* CurrentSelectedThreadAction;
#pragma endregion

#pragma region Window Mouse Event Handling
	 void OnLeftMouseDown_Thread(double x, double y); 
	 void OnRightMouseDown_Thread(double x, double y); 
	 void OnMiddleMouseDown_Thread(double x, double y); 
	 void OnLeftMouseUp_Thread(double x, double y); 
	 void OnRightMouseUp_Thread(double x, double y); 
	 void OnMiddleMouseUp_Thread(double x, double y); 
	 void OnMouseMove_Thread(double x, double y);
	 void OnMouseWheel_Thread(int evalue);
	 void OnMouseEnter_Thread();
	 void OnMouseLeave_Thread();
#pragma endregion

#pragma region Initialisation and Measurement Function
	 std::map<int, int> ThreadShape_MainCollection_Relation;
	//Initialise Thread Window..
	void InitialiseThreadOGlWindow(HWND handle, int width, int height);
	//Add new thread measurement..
	void AddNewThreadMeasurement(bool TopSurfaceFlat, bool RootSurfaceFlat);
	//Initialise thread shapes..
	void InitialiseThreadShapes(int threadcount, bool TopSurfaceFlat, bool RootSurfaceFlat, bool PartProgramLoad = false);
	//build thread Graphics..
	void buildThreadShapeList();
	//Calculate thread Shape parameters..
	void CalculateThreadParameters();
	//Select Thread shape for current frame grab
	void SelectThreadShape(int id);
	//Update thread graphics..
	void ThreadShape_Updated();
	//Current thread measurement..
	DimBase* CurrentThreadMeasure();
	//Update thread Graphics..
	void update_ThreadGraphics();
	//Wait Thread graphics update..
	void Wait_ThreadGraphicsUpdate();
	//Render thread window graphics..
	void Draw_ThreadWindowGraphics();
	//Clear All
	void ClearAllThread(bool updategraphics = true);
#pragma endregion
};


//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
//Class to add the Thread Measure action..
#pragma once
#include "RAction.h"
#include "..\Helper\General.h"

class DimBase;
class Shape;

class AddThreadMeasureAction:
	public RAction
{
private:
	DimBase* dim;
	RC_ITER ThreadShapePointer;
	void SelectTheProcessingShape(); 
	RC_ITER ppCurrentaction;
public:
	bool PausableAction;
	RCollectionBase ShapeCollection_ThreadWindow;
	RCollectionBase ShapeCollection_Actions;
	RCollectionBase ThrdMeasureCollection;
	RCollectionBase ActionCollection;	
	RapidProperty<bool> IncludeMajorMinorDia;
	RapidProperty<bool> ExternalThreadMeasurement;
	RapidProperty<bool> TopSurfaceFlat;
	RapidProperty<bool> RootSurfaceFlat;
	RapidProperty<int> ThreadCount;
	RC_ITER ThreadppCurrentaction;
	Shape* CurrentSelectedThreadShape;
	Shape* CurrentSelectedThreadShape_Threadwin;

	//Constructor and destructor...//
	AddThreadMeasureAction();
	virtual ~AddThreadMeasureAction();

	//Set and get the current dim pointer..//
	void setDim(DimBase* dim);
	DimBase* getDim();
	
	//Virtual functions..
	virtual bool execute();
	virtual void undo();
	virtual bool redo();
	
	static void addDim(DimBase* dim);
	void SelectTheFirstShape();
	void IncrementShapePointer();
	void SelecttheCurrentPointer(int id);
	void ResetAllActions(bool PartProgramLoad);
	RAction* getCurrentThAction();

	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, AddThreadMeasureAction&);
	friend wistream& operator>>(wistream&, AddThreadMeasureAction&);
	friend void ReadOldPP(wistream& is, AddThreadMeasureAction& action);
};
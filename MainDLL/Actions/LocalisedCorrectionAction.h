//Class to add the Point action..
#pragma once
#include "RAction.h"

class ShapeWithList;
class DimBase;

class MAINDLL_API LocalisedCorrectionAction:
	public RAction
{

private: 
	ShapeWithList *shape1, *shape2; // Pointer to the shape..

public:
	Vector DROPosition;
	int side, MeasureCnt, RowCnt, ColCnt;
	DimBase *measurement;
	LocalisedCorrectionAction();
	LocalisedCorrectionAction(double *curPos, int _side);
	virtual ~LocalisedCorrectionAction();

	//Virtual functions..
	virtual bool execute();
	virtual void undo();
	virtual bool redo();

	//Add the point action to the current selected shape..//
	void AddLocalisedCorrectionAction(ShapeWithList* shape1, ShapeWithList* shape2, DimBase* measurement);
	void SetLocalisedCorrectionProperties(int measureCnt, int rowCnt, int colCnt);

	friend wostream& operator<<(wostream&, LocalisedCorrectionAction&);
	friend wistream& operator>>(wistream&, LocalisedCorrectionAction&);
};
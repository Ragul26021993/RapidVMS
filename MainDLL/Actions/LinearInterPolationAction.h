//Class to add the Probe Point action..
#pragma once
#include "RAction.h"

class Shape;
class MAINDLL_API LinearInterPolationAction:
	public RAction
{
	Vector TargetPosition;
	Shape* shape;
	bool ShapeUsed, AddPtFlag;
public:
	bool PausableAction;
	LinearInterPolationAction();
	virtual ~LinearInterPolationAction();

	//Add and remove the point action..//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();
	static void AddLinearInterPolationAction(Shape* s, double* Target, bool AddpointFlag);
	static void AddLinearInterPolationAction(double* Target);
	void Translate(Vector pt1);
	void Transform(double* Tmatrix);
	Vector* GetTargetPosition();
	void SetTargetPosition(Vector& pt);
	Shape* getShape();
	bool GetShapeInfo();
	bool GetAddPointInfo();
	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, LinearInterPolationAction&);
	friend wistream& operator>>(wistream&, LinearInterPolationAction&);
};
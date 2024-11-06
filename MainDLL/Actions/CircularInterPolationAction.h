//Class to add the Probe Point action..
#pragma once
#include "RAction.h"

class Shape;
class MAINDLL_API CircularInterPolationAction:
	public RAction
{
	Vector Center, NormalVector, CurrentPos;
	double SweepAngle;
	int Hopscount;
	Shape* shape;
	bool ShapeUsed, AddPtFlag;
public:
	bool PausableAction;
	CircularInterPolationAction();
	virtual ~CircularInterPolationAction();

	//Add and remove the point action..//
	virtual bool execute();
	virtual void undo();
	virtual bool redo();
	static void AddCircularInterPolationAction(Shape* s, double* CircleCent, double* Nvector, double* CurrentPosition, double SweepAngle, int HopsCnt, bool AddpointFlag);
	static void AddCircularInterPolationAction(double* CircleCent, double* Nvector, double* CurrentPosition, double SweepAngle, int HopsCnt);
	void Translate(Vector pt1);
	void Transform(double* Tmatrix);
	Vector* GetCenter();
	Vector* GetNormalVector();
	Vector* GetCurrentPosition();
	double GetSweepAngle();
	int GetHopsCount();
	void SetCenter(Vector& pt);
	void SetCurrentPosition(Vector& pt);
	void SetNormalVector(Vector& pt);
	double GetRadius();
	Shape* getShape();
	bool GetShapeInfo();
	bool GetAddPointInfo();
	//Partprogram read/ write...//
	friend wostream& operator<<(wostream&, CircularInterPolationAction&);
	friend wistream& operator>>(wistream&, CircularInterPolationAction&);
};
#pragma once

#include "shapewithlist.h"
#include "Circle.h"
#include "Line.h"
class MAINDLL_API CircleInvolute :
	public ShapeWithList
{
private:
	static int shapenumber;
	Vector Center;
	TCHAR* genName(const TCHAR* prefix);
	void init();
	bool firstTime; 
	void ProbePointCalculations(int PointsCnt);
	void CheckPointIsForProbe();
	void CheckIdorOdCircleInvolute();
	void updateCircleInvolute_UsingPorbe();
	bool updateCircleInvolute(int PointsCount);	
public:
	bool DrawCircleInvolute;
	RapidProperty<double> startRadius, endRadius;
	RapidProperty<double> startAngle , endAngle , CircleRadius;
	RapidProperty<bool> IdInvolute;
	RapidProperty<double>AlphaAngle,Tangentlength;  
	static void reset();

	CircleInvolute(TCHAR* mtname);
	CircleInvolute(bool simply);
	~CircleInvolute(void);

	//Set and get CircleInvolute Properties....
	void getParameters(double* Involuteparam);
	void setParameters(double* Involuteparam);

	virtual void UpdateBestFit();
	virtual bool GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom);
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual void drawCurrentShape(int windowno, double WPixelWidth);
	virtual void drawGDntRefernce();
	virtual void Transform(double* transform);
	virtual bool Shape_IsInWindow(Vector& corner1,double Tolerance);
	virtual bool Shape_IsInWindow(Vector& corner1,Vector& corner2);
	virtual bool Shape_IsInWindow(double *SelectionLine, double Tolerance);
	virtual Shape* Clone(int n, bool copyOriginalProperty = true);
	virtual Shape* CreateDummyCopy();
	virtual void CopyShapeParameters(Shape*);
	virtual void Translate(Vector& Position);
	virtual void ResetShapeParameters();
	virtual void AlignToBasePlane(double* TrnasformM);
	virtual void UpdateEnclosedRectangle();
	virtual void GetShapeCenter(double *cPoint);	
};


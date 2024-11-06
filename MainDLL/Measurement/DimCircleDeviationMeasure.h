//Class used to calculate the deviation of circle................
#pragma once
#include "DimBase.h"
#include <map>
class Shape;

class DimCircleDeviationMeasure:
	public DimBase
{
private:
TCHAR* genName(const TCHAR* prefix);
	Shape* TempCircle;
	int DrawCount;
	double CentRadius[3];
	void ClearPointList();
	bool CalculateIntersectionPtOnCircle(double* pt, Shape* s1, double slope, double intercept, double* ans);
	void createtemporaryLine(double Pt1_X, double Pt1_Y, double Pt2_X, double Pt2_Y, int PtId); // create line and add in temp collection......
	void setLineParameter(double Pt1_X, double Pt1_Y, double Pt2_X, double Pt2_Y, int PtId);
	void setDefaultLineParameter();
	map<int, int> ShapePtRelation;
	int PtCount;
public:
	//PointCollection Point1, Point2, LastPt;
	DimCircleDeviationMeasure(TCHAR* myname = _T("Dev"));
	DimCircleDeviationMeasure(bool simply);
	virtual ~DimCircleDeviationMeasure();
	void CalculateMeasurement(Shape *s1);
	void CalculateMeasurement(Shape *s2, double *data);
	void CalculateMeasurement(Shape *s2, Shape *s1);
	//Virtual functions...//
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write the partprogram data//////
	friend wostream& operator<<(wostream&, DimCircleDeviationMeasure&);
	friend wistream& operator>>(wistream&, DimCircleDeviationMeasure&);
	friend void ReadOldPP(wistream& is, DimCircleDeviationMeasure& x);
};
//Created by Rahul Singh Bhadauria.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
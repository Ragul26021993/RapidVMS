//Class Used To Maintain the line to line measurement
#pragma once
#include "DimBase.h"
class Shape;

class DimLineToLineDistance:
	public DimBase
{
private:
	double angle1, angle2, intercept1, intercept2; //angles and intercepts.
	double Endpoints[8];
	int IntCnt;
	int angleZone; //Used for Part Programs to ensure shifting and rotating doesnt give complimentary angles
	double Meas_Location_Radius; //Radius at which original measurement was drawn. This will remain fixed....
	bool setMeasureName;
	void SetMeasureName();
private: 
	enum ANGLETYPE
	{
		LINE_LINE,
		XRAY_XRAY,
		XLINE_XLINE,
		LINE_XRAY,
		LINE_XLINE,
		XRAY_XLINE
	}angletype;
public:
	enum ANGLEWITHAXIS
	{
		DEFAULT,
		ANGLE_XAXIS,
		ANGLE_YAXIS
	}AngleTypeWithAxis;

public:
	
	DimLineToLineDistance(bool simply);
	virtual ~DimLineToLineDistance();

	void CalculateMeasurement(Shape *s1, Shape *s2, bool calculate = true);
	//Get the angle and intercept of the line..//
	void AngleIntercept(Shape *s, double *angle, double *intercept);

	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm);
	virtual void UpdateMeasurement();
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual DimBase* Clone(int n);
	virtual DimBase* CreateDummyCopy();
	virtual void CopyMeasureParameters(DimBase*);

	//Read/ write partprogram data...//
	friend wostream& operator<<(wostream&, DimLineToLineDistance&);
	friend wistream& operator>>(wistream&, DimLineToLineDistance&);
	friend void ReadOldPP(wistream& is, DimLineToLineDistance& x);
};
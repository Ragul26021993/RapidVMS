//Base class used for measurements....///
//Consists of color properties, dimensions, dimension position(mouse point...)
//Dimension types.. enumerations..//, Dimension width and height..(used to highilght ..)
//Class derived from Item base and related shape..
//
#pragma once
#include "..\Engine\BaseItem.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\RelatedShape.h"
#include "..\Helper\General.h"
#include "..\MainDLL.h"
#include "..\Shapes\Shape.h"

class MAINDLL_API DimBase:
	public BaseItem, public RelatedShape
{

private:
	void init();
 
protected:
	
	static TCHAR name[20];
	//count of all the dimensions
	static int count;
	static int distno;
	static int angleno;
	static int diano;
	static int radiusno;
	static TCHAR* genName(const TCHAR* dimname);
	char cdimension[50];

	RapidProperty<int> NormalmeasureType, LinearmeasureType;
	RapidProperty<int> TotalNormalmeasureType, TotalLinearmeasureType;
	RapidProperty<double> doubledimesion;
	double  TxtMid_Rcad[3], TxtLftTop_Rcad[3], TxtRghtBtm_Rcad[3], TxtLftTop_Video[3], TxtRghtBtm_Video[3], dimension;
	double TransformationMatrix[9], MeasurementPlane[4], DimSelectionLine[6];
	Vector mposition;

	//Virtual function.. draw: handles graphics function for all measurements.
	virtual void drawCurrentMeasure(int windowno, double WPixelWidth, double TextScaleFactor, double* Left_Top, double* Right_Btm) = 0;

public:
	//Measurement enumeration types...//
	RapidEnums::MEASUREMENTTYPE MeasurementType;
	RapidProperty<bool> MeasurementDirection, LinearMeasurementMode;
	RapidProperty<bool> IsValid, ContainsLinearMeasurementMode;
	RapidProperty<bool> MeasureAs3DMeasurement, MeasureAsGDnTMeasurement;
	RapidProperty<bool> DistanceMeasurement, HighlightedForMouseMove;
	RapidProperty<bool> ChildMeasurementType, IsNegativeMeasurement;
	RapidProperty<bool> MeasurementUsedForPPAbort, AddActionWhileEdit;
	RapidProperty<int> VideoFont_ScaleFactor, RcadFont_ScaleFactor;
	
	RapidProperty<double> NominalValue, UpperTolerance, LowerTolerance;
	RapidProperty<bool> Include_PassFail;
	
	RColor MeasureColor;
	Shape *ParentShape1, *ParentShape2, *ParentShape3, *ParentShape4, *ReferenceShape;
	Vector *ParentPoint1, *ParentPoint2;
	double TextHalfLength, DistanceFromMousePoint;
	BaseItem *CurrentDimAddAction;
public:
	//Constructor and Destructor..
	DimBase(TCHAR* = genName(_T("Dim"))); //Default Name is "M"..
	DimBase(bool simply);
	virtual ~DimBase();

	//Set and get the Mouse position..
	void SetMousePosition(double x, double y, double z, bool UpdateMeasure = true);
	void SetSelectionLine(double *Sline, bool UpdateMeasure = true);
	Vector* getMousePosition();
	void getLeftTopnRightBtm(double* Lefttop, double* RightBtm);
	
	//Transform.. measurement transformation...///
	void TranslatePosition(Vector& Position);
	void TransformPosition(double* transform);
	void BasePlaneAlignment(double* transform);
	
	//Set and get the dimension..//
	void setDimension(double d, bool RadianType = true);
	void setDimensionInteger(int d);
	void setDimension(char* cd);
	void setAngularDimension(double angle);
	double getDimension();
	char* getCDimension();
	double* getMeasurementPlane();
	
	//Function to select the measurements....
	bool Measure_IsInWindow(double x, double y, int windowno);
	bool Measure_IsInWindow(Vector& corner1, Vector& corner2);
	bool Measure_IsInWindow(double *Sline);

	void ChangeMeasurementType();
	void CopyOriginalProperties(DimBase* Cdim, bool updatemeasure = true);
	void drawMeasurement(int windowno, double WPixelWidth);

	void UpdateMeasurementColor();
	virtual void MeasurementTypeChanged();

	//Virtual functions.. should be implemented in child classes....//
	virtual void UpdateMeasurement() = 0;
	virtual DimBase* Clone(int n) = 0;//To get the clone of the Measurment...//
	virtual DimBase* CreateDummyCopy() = 0;
	virtual void CopyMeasureParameters(DimBase*) = 0;

	static void reset();
	//Write and Read the common Properties of a Shape..
	friend wostream& operator<<(wostream& os, DimBase& x);
	friend wistream& operator>>(wistream& is, DimBase& x); 
	friend void ReadOldPP(wistream& is, DimBase& x);
};
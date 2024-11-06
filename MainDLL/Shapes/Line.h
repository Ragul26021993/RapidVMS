//Line class..
// set and gets all properties of lines..//
#pragma once
#include "ShapeWithList.h"
#include "vector.h"
                          
class Shape;
class MAINDLL_API Line:
	public ShapeWithList
{

//Variable declarations..//
private:
	//variables for shape count..//
	static int Lshapenumber;
	static int XLshapenumber;
	static int Rshapenumber;
	static int L3dshapenumber;
	static int LDXFshapenumber;
	static int XLDXFshapenumber;
	static int RDXFshapenumber;
	static int L3dDXFshapenumber;
	static int XL3dDXFshapenumber;
	static int XL3dshapenumber;
	//Line end points..                       
	Vector point1, point2, Midpoint;
	Shape* ProbeCorrectionShape;
	RapidProperty<bool> AddIntercept;

	TCHAR* genName(const TCHAR* prefix, RapidEnums::SHAPETYPE Stype);
	void init();
	void init(RapidEnums::SHAPETYPE Stype);
	//Update Line according to the type... 
	void updateFiniteLine(int PointsCnt);
	void updateRayLine(int PointsCnt);
	void update3DLine(int PointsCnt);
	void updateFtFgline(int PointsCnt);
	void updateDerivedFiniteLine(int PointsCnt);

	void updateParallelLine(double* Mousept, double* EndPt1, double* EndPt2, bool flag);
	void updatePerpendicularLine(double* Mousept, double* EndPt1, double* EndPt2, bool flag);
	void updateAngleBisector(double* Mousept, double* EndPt1, double* EndPt2, bool flag);
	void UpdateIntersectionLine3D();
	void UpdateProjectionLine3D();
	void updateTangentOnCircle(double* Mousept, double* EndPt1, double* EndPt2, bool flag);
	void updateParallelTangent(double* Mousept, double* EndPt1, double* EndPt2, bool flag);
	void updatePerpendiculartanget(double* Mousept, double* EndPt1, double* EndPt2, bool flag);
	void updateTangentOn2Circle(double* Mousept, double* EndPt1, double* EndPt2, bool flag);
	void updateTangentthruPoint(double* Mousept, double* EndPt1, double* EndPt2, bool flag);
	void updateOnePtAngleLine(double* Mousept, double* EndPt1, double* EndPt2, bool flag);
	void updateParallelTangent2Circle(double* Mousept, double* EndPt1, double* EndPt2, bool flag);

	void CalculateLineBestFit(int PointsCnt, bool IsderivedLine);
	void CalculateLineBestFit3D(int PointsCnt);
	void ExtensionForDerivedline();

	void ProbePointCalculations_3DLine(int PointsCnt);
	void update3DLine_UsingPorbe();
	void ProbePointCalculations(int PointsCnt);
	void CheckPointIsForPorbe();
	void CheckAddOrSubtractIntercept();
	void updateLine_UsingPorbe();

	void updateNormalLines();
	void updateDerivedLines();
	void updateDerivedLineAcc_Type();
	void updateLinePerpendicular2LineOnPlane();
	void updateMultiPointsLineParallel2Line();
	void UpdateDerivedLine3D();
	void UpdateReflectedLine();

public:
	//Line Properties... Set and get Types...
	RapidProperty<double> Length, Angle, Intercept, dir_l, dir_m, dir_n, Straightness, FilteredStraightness, Offset, BuildAngle;
	RapidProperty<bool> DefinedOffset, DefinedLength, FastraceType_Mid, EndPointsFixed, SetAsCriticalShape;
	RapidProperty<int> LinePosition;
	RapidEnums::LINETYPE LineType;
	Vector* VPointer;
	double ScanDir[3];

public:
	//Constructor..//
	Line(TCHAR* myname = _T("L"), RapidEnums::SHAPETYPE Stype = RapidEnums::SHAPETYPE::LINE);
	Line(bool simply, RapidEnums::SHAPETYPE Stype = RapidEnums::SHAPETYPE::LINE);
	~Line();
	
	//Set and get Line end points....
	void setPoint1(Vector& t);
	void setPoint2(Vector& t);
	void setMidPoint(Vector& t);
	Vector* getPoint1();
	Vector* getPoint2();
	Vector* getMidPoint();
	//Calculate the line parameters.. length, intercept and angle..//
	void calculateAttributes();
	void setLineParameters(double ang, double intrcpt);
	void setLineParameters(double l, double m, double n);

	void getParameters(double* lineparam);
	void getEndPoints(double* endpoints);
	void SetProbeCorrectionShape(Shape* Shp);
	Shape* GetProbeCorrectionShape();


	//Virtual functions..//
	virtual void Transform(double*);
	virtual void drawCurrentShape(int windowno, double WPixelWidth);
	virtual void drawCurrentShape(int windowno, double WPixelWidth, double* TransFormM);
	virtual void drawGDntRefernce();
	virtual bool Shape_IsInWindow(Vector& corner1,double Tolerance);
	virtual bool Shape_IsInWindow(Vector& corner1, double Tolerance, double* TransformM);
	virtual bool Shape_IsInWindow(Vector& corner1,Vector& corner2);
	virtual bool Shape_IsInWindow(double *SelectionLine, double Tolerance);
	virtual Shape* Clone(int n, bool copyOriginalProperty = true);
	virtual Shape* CreateDummyCopy();
	virtual void CopyShapeParameters(Shape*);
	virtual void Translate(Vector& Position);
	virtual void UpdateBestFit();
	virtual bool GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom);
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual void ResetShapeParameters();
	virtual void AlignToBasePlane(double* trnasformM);
	virtual void UpdateEnclosedRectangle();
	virtual void GetShapeCenter(double *cPoint);

	//Reset the shape counts..//
	static void reset();
	static void decrement();
	//Partprogram data.. read/ write..//
	friend wostream& operator<<(wostream& os, Line& x);
	friend wistream& operator>>(wistream& is, Line& x); 
	friend void ReadOldPP(wistream& is, Line& x);
};	

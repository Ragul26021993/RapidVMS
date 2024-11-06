//Circle class...
//Set and gets all circle properties..//
#pragma once
#include "ShapeWithList.h"
#include "vector.h"

class MAINDLL_API Circle:
	public ShapeWithList
{
//Circle types.. both normal and derived circles..//

//variable declaration..//
private:
	//Counter for different type of circles..//
	static int Cshapenumber;
	static int Ashapenumber;
	static int CDXFshapenumber;
	static int ADXFshapenumber;
	
	//circle properties
	Vector center, endpoint1, endpoint2, Midpoint;
	RapidProperty<bool> IdCircle;
	//Add snap points..//
	void init();
	void init(RapidEnums::SHAPETYPE Stype);
	TCHAR* genName(const TCHAR* prefix, RapidEnums::SHAPETYPE Stype);	

	void updateCircleWithCenter(int PointsCnt);
	void update2Pt3PtCircle(int PointsCnt);
	void update3DCircle(int PointsCnt);
	void updateArc3D(int PointsCnt);
	void updateArc(int PointsCnt);
	void updateFtFgArc(int PointsCnt);
	void updateSiblingArcs(int PointsCnt);
	void updateArcTanToArc(int PointsCnt);
	void updateArcTanToLine(int PointsCnt);
	void updateProjectedCircle(int PointsCnt);

	void updateParallelArc(double* Mousept, bool flag);
	void updateTanCircleto2CirclesIn(double* Mousept, bool flag);
	void updateTanCircleto2CirclesOut(double* Mousept, bool flag);
	void updateTanCircletoCircle(double* Mousept, bool flag);
	void updateTanCircleto2Lines(double* Mousept, bool flag);
	void updateTanCircletoLineCircle(double* Mousept, bool flag);
	void updateCircleInsideTriangle();
	void updateCircleOutSideTriangle();

	void updateArcTangentToTwoLines(int* p_PointsCnt, double* ans);

	void drawExtensionForTangentialCircle();
	void CalculateCircleBestFit(int PointsCnt);

	void ProbePointCalculations(int PointsCnt);
	void CheckPointIsForPorbe();
	void CheckIdorOdCircle();
	void updateCircle_UsingPorbe();
	
	void updateNormalCircles();
	void updateDerivedCircles();
	void updateDerivedCircleAcc_Type();
public:
////Circle Properties... Set and get Types...
	RapidProperty<double> OriginalRadius, Radius, Startangle, Sweepangle, BuildRadius;
	RapidProperty<double> MinInscribedia, MaxCircumdia, Circularity, Length, Offset;
	RapidProperty<double> dir_l, dir_m, dir_n;
	RapidProperty<bool> SetAsCriticalShape;
	RapidEnums::CIRCLETYPE CircleType;
	bool added_line_arc_line_pt, OffsetDefined;
	Vector* VPointer;
	
public:
	Circle(TCHAR* myname = _T("C"), RapidEnums::SHAPETYPE Stype = RapidEnums::SHAPETYPE::CIRCLE);
	Circle(TCHAR* myname, bool ForPCd,bool is3D=false);
	Circle(bool simply, RapidEnums::SHAPETYPE Stype = RapidEnums::SHAPETYPE::CIRCLE);
	~Circle();
	
	//Virtual Functions..
	virtual bool Shape_IsInWindow(Vector& corner1,double Tolerance);
	virtual bool Shape_IsInWindow(Vector& corner1,double Tolerance, double* TransformM);
	virtual bool Shape_IsInWindow(Vector& corner1,Vector& corner2);
	virtual bool Shape_IsInWindow(double *SelectionLine, double Tolerance);
	virtual Shape* Clone(int n, bool copyOriginalProperty = true);
	virtual void CopyShapeParameters(Shape*);
	virtual Shape* CreateDummyCopy();
	virtual void UpdateBestFit();
	virtual bool GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom);
	virtual void drawCurrentShape(int windowno, double WPixelWidth);
	virtual void drawCurrentShape(int windowno, double WPixelWidth, double* TransFormM);
	virtual void drawGDntRefernce();
	virtual void Transform(double*);
	virtual void Translate(Vector& Position);
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual void ResetShapeParameters();
	virtual void AlignToBasePlane(double* trnasformM);
	virtual void UpdateEnclosedRectangle();
	virtual void GetShapeCenter(double *cPoint);
	// Set and Get the circle parameters..//
	void setCenter(Vector& t);
	void setendpoint1(Vector& t);
	void setendpoint2(Vector& t);
	void setMidPoint(Vector& t);
	void getParameters(double* circleparam);
	void getPlaneParameters(double* circlepln);
	Vector* getCenter();
	Vector* getendpoint1();
	Vector* getendpoint2();
	Vector* getMidPoint();

	void setCircleParameters(double l, double m, double n, double Radius);
	void updateBestFitArc();
	void updateLineArcLineShape();
	void calculateAttributes();
	static void reset();

	//Write and read, to/from the file..//
	friend wostream& operator<<(wostream& os, Circle& x);
	friend wistream& operator>>(wistream& is, Circle& x);
	friend void ReadOldPP(wistream& is, Circle& x);
};	
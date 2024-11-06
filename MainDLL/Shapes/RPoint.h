//Point(shape) class...
#pragma once
#include "shapewithlist.h"
#include "vector.h"

class MAINDLL_API RPoint:
	public ShapeWithList
{
private:
	static int shapenumber;
	static int DXFshapenumber;
	static int shapenumber3D;
	static int DXFshapenumber3D;
	double RelativeOffsetX, RelativeOffsetY, RelativeOffsetZ;
	Vector position;
	Shape* ProbeCorrectionShape;
	//Get the name of the shape..//
	TCHAR* genName(const TCHAR* prefix, RapidEnums::SHAPETYPE Stype);
	void init(RapidEnums::SHAPETYPE Stype);	

	void updateRelativePoint();
	void updateNormalPoints();
	void updateDerivedPoints();
	void updateProjectedPoints();
	void updateIntersectionPoint();
	void updateMidpoint(int PointsCount);
	void updateNearestpoint(int PointsCount);
	void updateFarthestpoint(int PointsCount);
	void updateNearestpoint3D(int PointsCount);
	void updateFarthestpoint3D(int PointsCount);
	void updatePointOnLine(int PointsCount);
	void updatePointOnCircle(int PointsCount);

	void ProbePointCalculations(int PointsCnt);
	void CheckPointIsForPorbe();
	void updatePoint_UsingPorbe();
	
public:
	//Enumeration for point types..//
	enum RPOINTTYPE
	{
		POINT,
		POINT3D,
		MIDPOINT,
		NEARESTPOINT,
		FARTHESTPOINT,
		POINTONLINE,
		POINTONCIRCLE,
		INTERSECTIONPOINT3D,
		RELATIVEPOINT3D,
		PROJECTEDPOINT3D,
		NEARESTPOINT3D,
		FARTHESTPOINT3D,
	}PointType;

public:
	RPoint(TCHAR* myname = _T("P"), RapidEnums::SHAPETYPE Stype = RapidEnums::SHAPETYPE::RPOINT);
	RPoint(bool simply, RapidEnums::SHAPETYPE Stype = RapidEnums::SHAPETYPE::RPOINT);
	~RPoint();

	void SetOffsets(double x, double y, double z);

	//set and get the point position..//
	void setpoint(Vector d);
	Vector* getPosition();
	void SetProbeCorrectionShape(Shape* Shp);
	Shape* GetProbeCorrectionShape();
	Vector *ParentPoint;
	//Virtual functions..//
	virtual void Transform(double*);
	virtual void drawCurrentShape(int windowno, double WPixelWidth);
	virtual void drawCurrentShape(int windowno, double WPixelWidth, double* TransFormM);
	virtual void drawGDntRefernce();
	virtual bool Shape_IsInWindow(Vector& corner1,double Tolerance);
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

	//Reset the point count..//
	static void reset();
	//PartProgram Data.. Write/ read ..
	friend wostream& operator<<(wostream& os, RPoint& x);
	friend wistream& operator>>(wistream& is, RPoint& x);
	friend void ReadOldPP(wistream& is, RPoint& x);
};	
//Cloudpoints class....
#pragma once
#include "shapewithlist.h"
#include "vector.h"

class SinglePoint;

class MAINDLL_API CloudPoints :
	public ShapeWithList
{
private:
	static int shapenumber;
	static int DXFshapenumber;
	double *PlaneParams, *PlaneMidPointColl;
	TCHAR* genName(const TCHAR* prefix);
	void init();
	Vector Minpoint, Maxpoint;
	void CalculateZLevel();
	bool toleranceComparision;
	double ToleranceValue;
	std::list<SinglePoint*> WithinToleranceList, OutOfToleranceList;
	ShapeWithList* IdealSurface;
	void CalculateColorWithRespectToSurface();
	void UpdateColorInfo();
	void ClearDeviationColorMemory();
	class PtWithColor
	{
	public:
		double X, Y, Z, R, G, B, Dist;
		PtWithColor()
		{
			X = 0; Y = 0; Z = 0; R = 0; G = 0; B = 0; Dist = 0;
		};
		~PtWithColor(){};
	};
	double MaximumDistance;
	std::list<PtWithColor*> PtsListWithColor;
public:
	//Constructor ...
	CloudPoints(TCHAR* myname = _T("CP"));
	CloudPoints(bool simply);
	~CloudPoints();
	RapidEnums::CLOUDPOINTSTYPE CloudPointsType;
	map< int, Shape*> TempShapeCollection;
	RapidProperty<bool> SelectedPtsOnly;
	bool CloudPointsForLine;
	void CreateLines();
	list<Pt> AllPointsCollection;
	double *AllTrianglelist, *CloudPointsForMeasure;
	int trianglecount, PointsForMeasureCnt;
	Vector* getMinpoint();
	Vector* getMaxpoint();
	void CompareWithStlSurface(double tolerance);
	void CopyPoints(Shape*);
	void UpdatePointsColorWRTDeviation();
	//Virtual functions...//
	virtual void drawCurrentShape(int windowno, double WPixelWidth);
	virtual void drawGDntRefernce();
	virtual void Transform(double*);
	virtual void Translate(Vector& Position);
	virtual bool Shape_IsInWindow(Vector& corner1, double Tolerance);
	virtual bool Shape_IsInWindow(Vector& corner1, Vector& corner2);
	virtual bool Shape_IsInWindow(double *SelectionLine, double Tolerance);
	virtual Shape* Clone(int n, bool copyOriginalProperty = true);
	virtual Shape* CreateDummyCopy();
	virtual void CopyShapeParameters(Shape*);
	virtual void UpdateBestFit();
	virtual bool GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom);
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual void ResetShapeParameters();
	virtual void AlignToBasePlane(double* trnasformM);
	virtual void UpdateEnclosedRectangle();
	virtual void GetShapeCenter(double *cPoint);

	//Reset the shape count..//
	static void reset();
	//write and read to/ from the rpp file...//
	friend wostream& operator<<(wostream& os, CloudPoints& x);
	friend wistream& operator>>(wistream& is, CloudPoints& x);
	friend void ReadOldPP(wistream& is, CloudPoints& x);
};
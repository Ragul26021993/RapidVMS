//PolyLine Class.. Like Cloudpoints.. Set of lines is one shape..//
#pragma once
#include "shapewithlist.h"
#include "vector.h"
#include <list>

class SinglePoint;

class MAINDLL_API TriangularSurface:
	public ShapeWithList
{
class TriangleIntersectPts
{
	public:
		double Line_Pts[6];
};
private:
	static int shapenumber;
	double MinimumZ, MaximumZ, CurrentZ, IntersectInterval;
	std::list<TriangleIntersectPts*> LineCollection;
	std::list<std::list<TriangleIntersectPts*>*> PolyLineCollection;
	std::list<SinglePoint*> SolidGeometryPtsList;
	//std::list<std::list<TriangleIntersectPts*>*> List_List_PtsArray;
	TCHAR* genName(const TCHAR* prefix);
	void init();	
	struct Triangle
	{
		double point1[3], point2[3], point3[3], dirCosine[3]; 
	};
	void ClearIntersectionPtsMemory();
	bool CheckTriangleIntersectionwithPlane(double* PtsArray, double Value, int Axisno = 2);
	void GetSurfaceIntersectPointCollection();
	void CreatePolyLineCollection();
	int CheckNeighbourLines(double* pt, double* endpts);
	void CreatePointsforSolidgeometry();
	int GetNumber_Of_Intersection(double* Pts);
	void ClearLinecollectionMemory();
	void ClearPolyLineMemory();
	void ClearSolidGemotrypointsMemory();
public:
	TriangularSurface(TCHAR* myname = _T("TL"));
	TriangularSurface(bool simply);
	~TriangularSurface();

	int TriangleCount;
	std::list<Triangle> TriangleCollection;
	std::list<Triangle>::iterator TShapeitr;
	
	void AddNewTriangle(double* Parameter);
	virtual void drawCurrentShape(int windowno, double WPixelWidth);
	virtual void drawGDntRefernce();
	virtual void Transform(double*);
	virtual bool Shape_IsInWindow(Vector& corner1,double Tolerance);
	virtual bool Shape_IsInWindow(Vector& corner1,Vector& corner2);
	virtual bool Shape_IsInWindow(double *SelectionLine, double Tolerance);
	virtual Shape* Clone(int n, bool copyOriginalProperty = true);
	virtual Shape* CreateDummyCopy();
	virtual void CopyShapeParameters(Shape*);
	virtual void UpdateBestFit();
	virtual bool GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom);
	virtual void UpdateForParentChange(BaseItem* sender);
	virtual void Translate(Vector& Position);
	virtual void ResetShapeParameters();
	virtual void AlignToBasePlane(double* trnasformM);
	virtual void UpdateEnclosedRectangle();
	virtual void GetShapeCenter(double *cPoint);
	static void reset();
	void CreateIntersectionPts_Triangle(double interval, int Axisno = 2);

//Created by Rahul Singh Bhadauria.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
};	
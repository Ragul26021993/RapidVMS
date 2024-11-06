//PolyLine Class.. Like Cloudpoints.. Set of lines is one shape..//
#pragma once
#include "shapewithlist.h"
#include "vector.h"
#include <list>

class MAINDLL_API AutoScanSurface:
	public ShapeWithList
{
	static int shapenumber;
	TCHAR* genName(const TCHAR* prefix);

	public:
			
	double *SelectedPoints, *SurfacePoints, *TriangleList, extentPnts[4], center[3];
	int SelectedPointsCnt, TriangleCnt, NumberOfRows, NumberOfCols;
	AutoScanSurface();
	AutoScanSurface(map<int, double> selectedPnts);
	~AutoScanSurface();
			
	bool CreateSurface(double width, double height);

	//Virtual functions../;/
	virtual void drawCurrentShape(int windowno, double WPixelWidth);
	virtual void drawGDntRefernce();
	virtual void Transform(double*);

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
	void reset();
	void GetShapeCenter(double *cPoint);

		//Write and Read to/ from the file..///
	friend wostream& operator<<(wostream& os, AutoScanSurface& x);
	friend wistream& operator>>(wistream& is, AutoScanSurface& x);
};	
//Reflection handler: to do reflection...//
#pragma once
#include "MouseHandler.h"
#include "..\Engine\RCadApp.h"

class AutoScanSurface;

class AutoFocusScanHandler :
	public MouseHandler
{
				 
private:
	enum DirList
	{
	  ALONGX,
	  ALONGYPOS,
	  ALONGYNEG,
	}  CurrentDir;

	list<AutoScanSurface*> SurfaceCollection, tmpSurfaceCollection;
	list<AutoScanSurface*>::iterator SurfaceIter;
	ShapeWithList* currentshape;
	double AvgZ, DirZ, MaxZ, MinZ, angleOfRotation;
	map<int, double> currentSurfacePnts;
	int CurrentCell, curSurfacePntsCnt, TotalCell, numberOfRotation;
	int FRow, FCol, FGap, FWidth, FHeight;
	void ClearTmpListptr();
	void CalculateNextGotoPosition(bool currentPlace = false);
	bool conti, drawSurface, rotateOnMouseMove;
	Vector *PointOfRotation;
public:
	//Constructor and destructor...//
	AutoFocusScanHandler();
	~AutoFocusScanHandler();

	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	virtual void RmouseDown(double x, double y);

	bool StartAutoFocus();
	void ContinueAutoFocus();
	void UpdateAutoFocusScanPoints(double* Zvalues);
	void RotateAutoFocusScanSurfaces(double rotationAngle, int step);
	bool GetEnclosedRectanglefor3dMode(double *transformMatrix, double *Lefttop, double *Rightbottom);
	bool GetEnclosedRectanglefor2dMode(double *Lefttop, double *Rightbottom);
	void ExportSurfacePoints(char* filename);
	void ImportSurfacePoint(char* filename);
	void ClearAll();
	bool GetSelectedShapeCenter(double *currentCenter);
	bool CalculateMatrixesForRotation(double *currentShapeCenter, double *rotateMatrix);
};
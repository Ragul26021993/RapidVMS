//Handler for parallel arc...//

#pragma once
#include "MouseHandler.h"

class ShapeWithList;
class PointCollection;
class SinglePoint;
class CloudComparisionHandler :
	public MouseHandler
{
	struct UpdatedPts
	{
		double EndPts[6];
	};
	double MaxInnerPtsAllowed, MaxOuterPtsAllowed;
	bool runningPartprogramValid;
	void UpdateDeviationParam(double interval, int SkipPts1, int SkipPts2, ShapeWithList* ParentShape1, ShapeWithList* ParentShape2, bool ArcBestFit, int NumberOfPts, std::list<int> LineIdList);
	bool CreateMeasurement(double interval, ShapeWithList* ParentShape1, ShapeWithList* ParentShape2, bool ArcBestFit, int NumberOfPts, int SkipPts1, int SkipPts2, std::list<Shape*>* LineIdList);
	void findAveragePoint(double* points, double radius, PointCollection *PtsList, double* DeviationPt);
	void findIntersectionPoint(bool ArcBestFit, int BestFitPts, map<int,SinglePoint*>::iterator SptItem, PointCollection *PtsList, double* ProjectedPt, double* Point1);
	void findProjectedPoint(bool ArcBestFit, int BestFitPts, map<int,SinglePoint*>::iterator SptItem, PointCollection *PtsList, double* ProjectedPt);
	void CreateLineMeasureMent(double* Point1, double* Point2, ShapeWithList* Shp1, ShapeWithList* Shp2, std::list<Shape*>* LineIdList, bool whitecolor = false, int MinDist = 0, ShapeWithList* ParentShape3 = NULL);
	void calculateIntersectionPtOnArc(double* pt, double* Center, double Radius, double* Angles, double* endpts, double* ans);
	void calculateIntersectiLine(double* Pnts, double Angle, double Intercept, double* endpts, double* ans);
	bool CreateMeasurementFor3MComponent(bool SurfaceOnFlag, int MeasurementCount, ShapeWithList* ParentShape1, ShapeWithList* ParentShape2, bool ArcBestFit, int NumberOfPts, int SkipPts1, int SkipPts2, std::list<Shape*>* LineIdList);
	void findNextPoint(double* Center, double* centroid, double Angle, bool ArcBestFit, int BestFitPts, PointCollection *PtsList, double* NextPt);
	void UpdateDeviationParamFor3MComponent(bool SurfaceOnFlag, int MeasurementCount, int SkipPts1, int SkipPts2, ShapeWithList* ParentShape1, ShapeWithList* ParentShape2, bool ArcBestFit, int NumberOfPts, std::list<int> LineIdList);
	void findLineArcIntersectPoint(bool ArcBestFit, double* ProjectedPt, int BestFitPts, PointCollection *PtsList, double* NextPt);
	void findNextPoint_UsingLength(bool SurfaceOn, bool IncrementPtr, map<int,SinglePoint*>::iterator SptItem, double MinLength, bool ArcBestFit, int BestFitPts, PointCollection *PtsList, double* NextPt, int* PreviousCount);
	bool CreateMeasurementFor_Delphi_Component(double Angle, ShapeWithList* ParentShape1, ShapeWithList* ParentShape2, ShapeWithList* ParentShape3, bool ArcBestFit, int NumberOfPts, int SkipPts, std::list<Shape*>* LineIdList);
	void UpdateMeasurementFor_Delphi_Component(double Angle, ShapeWithList* ParentShape1, ShapeWithList* ParentShape2, ShapeWithList* ParentShape3, bool ArcBestFit, int NumberOfPts, int SkipPts, std::list<int> LineIdList);
public:
	CloudComparisionHandler();
	~CloudComparisionHandler();

	void Point_CloudptComparatorParm(double Angle, bool ArcBestFit, int NumberOfPts);
	void CloudptComparatorParm(double interval, bool ArcBestFit, int NumberOfPts);
	void CloudptComparatorParm(int MeasureCount, bool ArcBestFit, int NumberOfPts);

	//Virtual functions...//
	virtual void mouseMove();
	virtual void LmouseDown();
	virtual void LmaxmouseDown();
	virtual void draw(int windowno, double WPixelWidth);
	virtual void EscapebuttonPress();
	//Virtual funtion to handle partprogram...//
	virtual void PartProgramData();
};
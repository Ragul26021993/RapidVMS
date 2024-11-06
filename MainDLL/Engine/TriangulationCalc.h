//To triangulate given set of point collection..

#pragma once
#include <map>
#include <list>
#include "..\Engine\PointCollection.h"
using namespace std;

//To hold each cube..
class PtCube
{
	public:
		SinglePoint LeftBtm, RightTop;
		PointCollection CPointColl;
};


class TriangulationCalc
{
private:

	struct Pt_Present
	{
		int Id;
		SinglePoint Point_Value;
	};

	//Bounds of enclosing rectangle/cube
	double MinX, MinY, MinZ, MaxX, MaxY, MaxZ;
	//Number of square/cube in X, Y, Z
	int NoofCellsX, NoofCellsY, NoofCellsZ;

	bool RunOnSeperateThread;
	int NumberOfThreads, ThreadFinishedCount, MinimumPointCount;
	map<int, PointCollection*> AllPointsCollectionColl;
	//Points copy collection
	PointCollection TempPointCollection;
	//Triangulated points collection
	PointCollection TrainglePointCollection, LinePointCollection;
	//Collection of Collection of Collection!.. 3 Dimensional.. Superb! Easy to code! 
	//ie. Collection of collection of Collections containing objects containing collection of objects.
	//Lists, Objects, Pointer to Objects.. C++ rocks!
	std::list<std::list<std::list<PtCube*>*>*> AllPtCubeCollection;

	//Add/Set point Collection
	void SetPointCollection(PointCollection& PointColl);
	void CalculateAllCubes(double tolerance);
	void CheckPointInsideEachCube();
	void TriangulatePoints();
	void AddOneTrianglePoints(SinglePoint* Point1, SinglePoint* Point2, SinglePoint* Point3);
	void AddOneLinePoints(SinglePoint* Point1, SinglePoint* Point2);
	bool CheckForPointinCube(std::list<PtCube*>* ZPtCubeColl, SinglePoint** Point);

	void ClearAllMemory();

	friend void CheckPointInCube_Thread(void* anything);

	void CalculateTriangle(PtCube* ZPtCubeColl11, PtCube* ZPtCubeColl12, PtCube* ZPtCubeColl22, PtCube* ZPtCubeColl21);

public:
	//Constructor, Destructor..
	TriangulationCalc(bool SplitCalcintothread = false, int PointStep = 1000);
	~TriangulationCalc();
	int Triangulate3DPointCollection(PointCollection& PointColl, double** TriangleCollection, double tolerance);
	/*int Triangulate3DPointCollection(PointCollection& PointColl, double** TriangleCollection, double tolerance, double** MeshList, int* LineCount);*/
};

//Created by Sathyanarayana. on 13 Jan 2011!!
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
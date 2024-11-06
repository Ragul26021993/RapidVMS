//Class used to handle the UCS properties.....
//Contains UCS properties... UCS type and its property
//Shapes, measurements, Dxf..
//All the Window instances..
//Snap Point manager instance..
#pragma once
#include "..\MainDLL.h"
#include "..\Shapes\Shape.h"
#include "RWindow.h"
#include "RCollectionBase.h"
#include "BaseItem.h"
#include "SnapPtManager.h"
#include "..\Helper\General.h"

class Line;
class Plane;

class MAINDLL_API UCS:
	public BaseItem
{
private:
	RWindow windows[5]; //Window Instances...//
	RCollectionBase shapes; //Shapes Collection
	RCollectionBase dimensions; //Measurement Collection
	RCollectionBase dxfColl; //Dxf Shape collections..
	SnapPtManager *isptmgr;	//intersection points
	RCollectionBase *PPshapesCollection_Original; //Original shapes..to copy during the next run.. start from load.
	RCollectionBase *PPshapesCollection_ForPath; //Shape Collection for Partprogram path
	RCollectionBase *PPMeasureCollection_Original; //Original Measure..to copy during the next run.. start from load.
	RCollectionBase *PPMeasureCollection_ForPath; //Measure Collection for Partprogram path

	Vector DROPostn; 
	static int UCSnumber;
	TCHAR name[10];
	TCHAR* genName(const TCHAR* prefix);
	void init();

public:
	Line *X_AxisLine, *Y_AxisLine, *Z_AxisLine, *MarkingShape;
	Plane *XY_CoordinatePlane,  *YZ_CoordinatePlane,  *ZX_CoordinatePlane;
	double transform[16]; //transformation matrix for each UCS
	double transform_B2S[16]; //transformation matrix updated in PP but before 2Step homing completed. 
	Vector UCSPoint;
	Vector OriginalUCSPoint;
	Vector UCSPt_B2S; // UCS home position before updating for 2Step Homing
	double UCSAngle_B2S;
	Vector* CenterPt;
	Shape* AxisLine;
	RapidProperty<double> currentDroX, currentDroY, currentDroZ, currentDroR;
	RapidProperty<bool> KeepGraphicsStraightMode, AddingFirstTime;
	RapidProperty<int> UCSMode, ParentUcsId, ChildUcsId;
	RapidProperty<double> UCSangle, UCSIntercept;
	RapidProperty<int> UCSProjectionType;

	RCollectionBase *RelatedUCSChangeAction;

public:
	//Constructor and destructor...//
	UCS(TCHAR *myname, bool AddAxis = true);
	~UCS();
	
	RCollectionBase& getShapes(); //Shape Collection.
	RCollectionBase& getDimensions(); //Measurement Collection.
	RCollectionBase& getDxfCollecion(); //Dxf Shape Collection.
	SnapPtManager& getIPManager(); //Snap Point Manager..
	RWindow& getWindow(int no = 0); //Return the pointer to the required Window.

	RCollectionBase& getPPShapes_Original(); //partprogram Shape Collection.
	RCollectionBase& getPPShapes_Path(); //partprogram Shape Collection for path.
	RCollectionBase& getPPMeasures_Original(); //partprogram measure Collection.
	RCollectionBase& getPPMeasures_Path(); //partprogram measure Collection for path.
	
	//Set and get the current DRO position..//
	void SetCurrentUCS_DRO(Vector* v);
	Vector GetCurrentUCS_DROpostn();
	map<int, Shape*> ShapesCol;
	map<int, Vector*> PointsCol;
	void SetUCSProjectionType(int i);

	void SetParameters(double Angle, Vector* Origin);

	static UCS *CreateUcs(int ucsmode, Vector* origin);

	//Clear all UCS properties...//
	void clearAll();
	static void reset();

	//Partprogram read and write...//
	friend wostream& operator<<(wostream& os, UCS& x);
	friend wistream& operator>>(wistream& is, UCS& x);
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
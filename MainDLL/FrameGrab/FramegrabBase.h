//Framegrab base class.. Handles the point pickig for different Crosshairs..
//Each point action will be noted by this class.. 
//i.e. point action( Normal crosshair, Flexible Crosshair, FrameGrabs, Probe point etc..)
//Or Probe path action.. 
// Used in Point actions and ProbePath Action
#pragma once
#include <iostream>
#include "..\Engine\BaseItem.h"
#include "..\Shapes\Vector.h"
#include "..\Shapes\SinglePoint.h"
#include "..\Engine\PointCollection.h"
#include "..\Helper\General.h"

class MAINDLL_API FramegrabBase:
	public BaseItem
{
private:
	//For unique name for each Action...
	static int fgNumber;
	static TCHAR name[10];
	TCHAR* genName(const TCHAR* prefix);
	void init();

public:
	int noofpts, noofptstaken_build, ChannelNo;
	int FGWidth, FGdirection, CurrentWindowNo, Surfacre_FrameGrabtype, MaskFactor, Binary_LowerFactor, Binary_UpperFactor, PixelThreshold, MaskingLevel;
	bool ProfileON, SurfaceON, UseDigitalZoom, DoAverageFiltering; // Current action light property..
	bool DerivedShape, DroMovementFlag; //Shape type taken for depth, derived etc..
	bool Auto_FG_AssistedMode; //When true, current FG will be done automatically even in CNC-Assisted mode. 

	//We will have a list of preset Surface Edge Detection Params
	std::list<SurfaceED_Params> SEDP_List;
	//bool TryMultipleSED_Params;
	//Have an array of double to store the Image Parameters for location of object within the image. 
	//double CG_Params[4];

	std::string camProperty, lightProperty, magLevel;; // Current action camera properties..
	RapidProperty<bool> ReferenceAction;// Whether action is an refernce action.. Made for refernce shape..
	RapidProperty<bool> ActionForTwoStepHoming; // Whether the action is made for two step alignment..
	RapidProperty<bool> ActionFinished; //Action is finished in program run..
	RapidProperty<bool> PartprogrmActionPassStatus; //Action status program run..
	RapidProperty<bool> isLineArcFirstPtAction; //This flag is set for the action chosen as first point action out of list of point actions created by line-arc action
	RapidProperty<bool> DontCheckProjectionType;

	Vector myPosition[5], points[3]; //Current mouse position..Pixel coordinats and DRO coordinates..
	Vector PointDRO, OriginalDRO; // Current DRO position..
	Vector* Pointer_SnapPt; // For derived shapes..
	list<int> PointActionIdList; // Id list of All the points that added..
	PointCollection AllPointsList; // Store the points untill shape is updated.. after that delete the points..
	int CucsId; //used in programload..
	RapidEnums::PROBEDIRECTION ProbeDirection, OriginalProbeDirection;
	RapidEnums::RAPIDFGACTIONTYPE FGtype;
	RapidEnums::PROBEPOSITION ProbePosition;

public:
	FramegrabBase(); //Default Constructor..
	FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE fg); // Consrtuctor with crosshair type as Paremeter..
	FramegrabBase(FramegrabBase* OriginalFb); //Copy Constructor
	~FramegrabBase(); // Destructor.. Clear the memory..
	
	//Get the camera properties.//Current light intensity..//
	char* getCamProperties();
	char* getLightIntensity();

	std::string getCurrentCrosshairTypeName();
	//Transform the points..
	void TrasformthePosition(double *transform, int ucsId);
	void TranslatePosition(Vector& diff, int ucsId);
	void TrasformthePosition_PlaneAlign(double *transform, int ucsId);
	void Transform_2S_Homing(double* transform, int ucsId);
	//Reset the FG name id counter..
	static void reset();
	//Read/write to/ from the partprogram file...//
	friend wostream& operator<<(wostream&, FramegrabBase&);
	friend wistream& operator>>(wistream&, FramegrabBase&);
	friend void ReadOldPP(wistream& is, FramegrabBase& x);
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
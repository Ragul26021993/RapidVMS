//Class to handle the Focus Related Functions.
//Does all the steps related to Focus scan and Focus Depth..
#pragma once
#include "..\MainDLL.h"
#include "..\FrameGrab\FramegrabBase.h"

class Shape;
class ShapeWithList;
class DimBase;

class MAINDLL_API FocusFunctions
{
public:
	enum FOCUSDEPTHSHAPETYPE
	{
	  PLANESHAPE,
	  SPHERESHAPE,
	  CYLINDERSHAPE,
	  POINTSHAPE,
	};
	list<double> DepthZcol;
	map<int, double> FocusColl;
	//Focus Row, Column, Gap, width and height.
	int focusRectangleRow, focusRectangleColumn, focusRectangleGap, focusRectangleWidth, focusRectangleHeight, NoOfRepeatAtPlace, DelayInMS;
	int ContourScanWidth, ContourScanHeight, TotalNoofCells;
	double FocusValueColl[7000000], ZvalueColl[400000];;
	FOCUSDEPTHSHAPETYPE ShapeForDepth;
	map<unsigned long, double> ZRecordOverTime;
	long focusDelay;
	double getZvalueBrforeDelay(map<unsigned long, double>);
	//Focus Span and Focus speed....
	double fspan, fspeed;
	double CurrentZvalue;
	bool Focus_firstvalue;
	bool FocusCalibrationSuccess;
	bool GetRawFocusValues;
	int TotalFocusValueCount, NoofIterationCount;
	int selectedAxis, framescancount, lowRowCutoff, upRowCutoff, lowColCutoff, upColCutoff;
	double *FiveFocus;
	RapidToggleProperty<bool> FocusScanMode;
	RapidToggleProperty<bool> ContourScanMode;
	RapidToggleProperty<int> CalRow, CalCol, CalWidth;

	PointCollection FocusCalibrationPtCollForPlane;
	PointCollection FocusCalibrationPtColl;
	PointCollection FocusCalibrationPtCollDeviation[4];
	
	Vector FocusStartPosition;

	//Variable for AutoFocus Scan..
	map<int, Shape*> ReferencePlanecollection;
	PointCollection AutoFocusScanGotoColl;
	SinglePoint AFSGotoPosition;
	SinglePoint LeftBottom;
	double GrainCellWidth, GrainCellHeight;
	int AFSPositionCount;
	Shape *CurrentRefPlane, *NextRefPlane;
	int AutoFocusBox[4];
public:	
	//constructor...
	FocusFunctions();
	//destructor..
	~FocusFunctions();
	void StartRightClickFocusThread();
	void StartFocusRelatedThrread();
	void StartFocusDelayThrread(int delay);
#pragma region All Variables, Properties
	RapidEnums::RAPIDFOCUSMODE CurrentFocusType;
	RapidProperty<bool> FocusCalibrationIsDone;
	RapidProperty<bool> FocusOnMode;
	RapidProperty<bool> FocusStatusFlag;
	RapidProperty<double> FocusZValue;
	RapidProperty<double> FocusActiveWidth;
	RapidProperty<double> MaxMinFocusRatioCutoff;
	RapidProperty<double> MaxMinZRatioCutoff;
	RapidProperty<double> NoiseFilterFactor;
	RapidProperty<double> SigmaActiveWdthRatioCutoff;
	RapidProperty<bool> RapidCamFocusIshappening;
	RapidProperty<bool> CalculateFocusMetric;
	RapidProperty<bool> ApplyFocusCorrection;
	RapidProperty<bool> AutoAdjustLighting;
	RapidProperty<double> AutoAdjustMinCutOff;
	RapidProperty<double> AutoAdjustMaxCutOff;
	RapidProperty<double> GrainFactor;
	RapidProperty<bool> ApplyPixelCorrection;
	RapidProperty<int> PixelCorrectionFactor;

	bool FocusAtOnePositionOnly;
	double FirstFocusvalue, *Cvalue;
	//holds Focus rectangle array. 
	int *Rectptr;

	int ShiftInPartProgramX;
	int ShiftInPartProgramY;
	bool isfivefocus;
	RapidProperty<int> FocusCalibrationCount;
	double* FocusCorrectionValues;
	double FocusErrCorrectionValue[4000];
	double FocusCalibError[4];
	double LastFocusValues;

	DimBase* CurrentDepthMeasure;
	Shape* CurrentDepthShape;
	FramegrabBase* Cbaseaction;

	list<int> DepthMeasureList;
	list<int> DepthDatumList;
	
	void (CALLBACK *FocusForReferenceDepth)();
	void (CALLBACK *StarFocusDepth_BoxDepth)(bool StartFocus);
	void (CALLBACK *GotoFocusScanPosition)();
	void (CALLBACK *changeFocusSpan)(double span);
	void (CALLBACK *UpdateCurrentFocusFillValue)(double FValue);
#pragma endregion

#pragma region All Functions.
	//Expose current focus value by calling cam functions, and copy value by getting double pointer as parameter.
	void GetCurrentFocusValue_Normal(double CZvalue);
	void GetCurrentFocusValue_RightClick(double CZvalue);
	//Set the current focus value for one box..
	void GetCurrentFocusValue_Profile(double CZvalue);
	void FocusMetricProfileLight();
	void FocusMetricSurfaceLight();
	void FocusMetricSurfaceLightOnRight();
	//Calculate rect structure value.
	void CalculateRectangle();
	//Set AutoFocus Rectangle Size
	void SetAutoFocusRectangle(int* rectArray);
	//Calculate rect structure value.
	void CalculateDepthRectangle(int *RectArray, int NoofBoxes);
	//Getting Number of Row, Column, Width, Height and Gap for Focus Metric.
	void setRowColumn(int row, int column, int gap, int wid, int hei);
	//Set Contour Tracing properties..
	void setContourScanWidth(int d1, int d2);
	//Flag to set whether Focus meteric is ON or OFF.
	void FocusOnStatus(bool);
	//Set Focus Span and Focus Speed.
	void setFocusSpan(double span, double speed);
	//Set Selected Axis
	void SetSelectedAxis(int axis);
	//Calculate the best focus..
	void CalculateBestFocus();
	//Allocate Focus variable the best focus..
	void setFiveFocus(double cellcount);	
	//Check for the active width spam..
	bool CheckActiveWidthSpam(double *Zvalue, bool *positive);
	void FilterFocus(double* xyzValues, int count, double focusSpanPos, double focusSpanNeg, double *planeParam, double* ZDevAvg);
	//Update Two Box Depth Measurement..
	void UpdateTwoBoxDepthMeasurement(double* Zvalues);
	//Update Four Box Depth Measurement..
	void UpdateFourBoxDepthMeasurement(double* Zvalues);
	//update calibration values..
	void UpdatePointsForFocusCalibration(double* Zvalues);
	//update calibration values..
	void CalculateFocusCalibrationValues();
	void DoAverageOfNearByPoints(ShapeWithList *cshape);
	
	//Add New Depth Measure...
	void AddNewDepthMeasure();
	//Handled the Set Z clciked...
	void AddTheDepthAction(int i);
	//Update Z value for program run..
	void AddTheDepthAction_Program();
	//set current Z position
	void AddDepthAction_Zposition(int i);
	//set current Z position during program
	void AddDepthAction_Zposition_Program();
	//Reset current Depth measure..
	void ResetSelectedDepthMeasure();
	//Get Z levels of Depth measurement..
	bool GetZ1_value(double* dval);
	//get z2 of focus Depth..
	bool GetZ2_value(double* dval);
	//get DepthValue
	bool GetDepth_value(double* dval);
	//Add new focus datum..
	void AddNewFocusDatum(int id);
	//delete current selecte focus datum..
	void DeleteFocusDatum();
	//Clear all focus datum
	void DeleteAllFocusDatum();
	//Calculate Focus for auto focus mode..
	double Calculate_BestAutoFocus();
	//draw Focus Rectangles..
	void drawFocusRectangels();
	//clear the last focus values..
	void ClearLastFocusValues();
	//Clear all Focus..
	void ClrearAllFocus(bool CleardatumReference = true);
	//SetFcousScanMode.
	void SetFocuScanMode();
	//Set ContourScanMode..
	void SetContourScanMode();
	//ResetFocusCalibrationvalues
	void ResetFocusCalibrationSettings();
	//Set Focus Calibration values..
	void SetFocusCalibrationValues(std::list<double> *Values,double* CalibError, double CellWidth, double Cellheight);
	//Check focus click
	bool FocusClicked_Handler();
	bool StartAutoFocusScan();
	void ContinueAutoFocus();
	void AutoFocusClearAll();
	void RotateInAutoFocus(double rotationAngle, int step);
	bool StartAutoContourScan();
	void ContinueAutoContourScan();
	void RotateInContourScan(double rotationAngle, int step);
	void AutoContourClearAll();
	double sort(double *point, int size);
	void CreateGrainSurface();
	void CreateLookUpTable();
	int getGrainSurfaceId(double x, double y, int grainRows, int grainCols);
	double CalculateFocus_DefinedPts(double* Pts, int NumberOfPts); //Function to calculate BF focus for an imported set of x-y values
	void (CALLBACK *ChangeCncMode)(int i);

	class GrainSurface
	{
	public:
	      GrainSurface(int id, double x, double y, int maxCnt)
		  {
		     Id = id;
			 X = x;
			 Y = y;
			 avgDev = 0;
			 cells = 0;
			 memset(planeParam, 0, sizeof(double) * 5);
			 planeParam[2] = 1;
			 PointArray = NULL;
			 PointArray = new double[3 * maxCnt];
		  }

		  void AddToList(double x, double y, double z)
		  {
		     PointArray[3 * cells] = x;
		     PointArray[3 * cells + 1] = y;
			 PointArray[3 * cells + 2] = z;
			 cells++;
		  }

		  ~GrainSurface()
	       {
		       delete [] PointArray;
		   }

		 int Id, cells;
		 double X, Y, avgDev, planeParam[5], *PointArray;
	};

	map<int, GrainSurface*> GrainSurfaceList;
	map<int, int> GrainLookUpTable;

#pragma endregion

};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!

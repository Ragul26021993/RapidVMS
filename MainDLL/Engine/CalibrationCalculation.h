//Class to handle the Calibration Process..
//To hold the Slipgauge parameters.. Machine parameters..Measurements etc..
// Does all the steps related to localised calibrations
#pragma once
#include "..\MainDLL.h"
#include "..\Engine\RCollectionBase.h"

class ShapeWithList;
class Shape;
class DimBase;
class FramegrabBase;
class RAction;
class LocalisedCorrectionAction;

class MAINDLL_API CalibrationCalculation
{
private:
	//Pointer to the added shapes..
	ShapeWithList *ParentLine1, *ParentLine2, *CurrentSelectedLine;
	ShapeWithList *ParentLine3, *ParentLine4;
	DimBase* WidthMeasure; //Current Measurement
	double SlipGaugeWidth; //Slip gauge width..
	int NoofRows, NoofColumns; // Cell properties..
	RCollectionBase MeasureCollection; // Measurement collection

	//Add Shape Measurement RelationShip
	void AddShapeMeasureRelationShip(Shape* CShape);
	void MedianOfMeasurement(double *measurements, double *medianMeasure);
	void AverageOfMeasurement(double *measurements, double *avgMeasure);
public:
	//Varibales used..
	RapidProperty<bool> CalibrationModeFlag, OneShotMode;
	RapidProperty<bool> XWidthMeasurementMode, YWidthMeasurementMode, AngleWidthMeasurementMode;
	RapidProperty<double> CurrentSlipGaugeEdgeAngle;
	RapidProperty<bool> SlipGaugeLeftToRight, SlipGaugeRightToLeft, SlipGaugeTopToBtm, SlipGaugeBtmToTop;
	RapidProperty<double> Width, Length;
	RapidProperty<double> LineX1, LineY1, LineX2, LineY2;
	double MeasureValues[5000];
	double CorrectedValues[2000];

	//Constructor
	CalibrationCalculation();
	//Destructor..
	~CalibrationCalculation();

	void AddFirstLine(bool addaction = true);
	void AddSecondLine(bool addaction = true);
	void AddFirstLine_WithoutLocalised();
	void AddSecondLine_WithoutLocalised();
	void SelectCurrentGrabbingLine(int LCnt);
	void CalculateWidthBnTwoLines(RAction *caction = NULL);
	void CalculateSGWidthBnTwoLines_Localised(bool flag);
	void CalculateSGWidthBnTwoLines_WithoutLocalised();
	void GrabPointsToSelectedLine(int type, bool addaction = true);
	void GrabPointsToSelectedLine(int type, int RowNum, int ColNum, int SlipGaugeWidth, int LineNumber);
	void GrabPoints_WidthMeasureToSelectedLine();
	void CheckCurrentSlipgaugeParam();
	void ClearAllCalibrationCalcualtions();
	void DotheCalibration(double *MeasurementMedians, bool isMedian);
	void DotheCalibration_Average(std::list<double> *Values);
	void SetCurrentMeasurementOrientation(int type);
	LocalisedCorrectionAction * GetLastAction();
	void ClearLocalisedAction();
	void SetCurrentMeasurementOrientation(int type, int RowNum, int ColNum, int SlipGaugeWidth);
	void SetSlipGaugeWidth(double w);
	void SetNofRowColumns(int r, int c);
	void SetCalibrationCorrectionValues(std::list<double> *Values, double slipgwidth, int rows, int columns);
	void AddAndRemoveMeasure(bool addmeasure, DimBase* cdim);
	FramegrabBase* FgofFirstLine();

	void CreateCorrectedMeasurements();
	void SetCurrentSelectedLine(Shape *Cshape);
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
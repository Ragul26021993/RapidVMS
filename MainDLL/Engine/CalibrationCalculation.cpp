#include "StdAfx.h"
#include "CalibrationCalculation.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\Line.h"
#include "..\Measurement\DimBase.h"
#include "..\Measurement\DimPointToLineDistance.h"
#include "..\Actions\AddDimAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\FrameGrab\FramegrabBase.h"
#include "..\Actions\AddPointAction.h"
#include "..\BestFit\RapidCamControl.h"
#include "..\FixedTool\CalibrationText.h"
#include "..\EdgeDetectionClassSingleChannel\Engine\EdgeDetectionClassSingleChannel.h"
#include "..\Helper\Helper.h"
#include "..\Actions\LocalisedCorrectionAction.h"

//static double Edpixelvalues[3200000];

CalibrationCalculation::CalibrationCalculation()
{
	try
	{
		CurrentSelectedLine = NULL; WidthMeasure = NULL;
		ParentLine1 = NULL; ParentLine2 = NULL;
		SlipGaugeWidth = 0; NoofRows = 0; NoofColumns = 0;
		CalibrationModeFlag(false);
		XWidthMeasurementMode(false); 
		YWidthMeasurementMode(false); 
		AngleWidthMeasurementMode(false);
		memset(MeasureValues, 0, 1000);
		memset(CorrectedValues, 0, 400);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0001", __FILE__, __FUNCSIG__); }
}

CalibrationCalculation::~CalibrationCalculation()
{
	try {}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0003", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::AddFirstLine(bool addaction)
{
	try
	{
		ParentLine1 = new Line();
		CurrentSelectedLine = ParentLine1;
		if(addaction)
			AddShapeAction::addShape(ParentLine1);
		else
		{
			ParentLine1->AddActionWhileEdit(false);
			RCollectionBase& rshapes = MAINDllOBJECT->getUCS(0)->getShapes();
			rshapes.addItem(ParentLine1);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0004", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::AddSecondLine(bool addaction)
{
	try
	{
		ParentLine2 = new Line();
		CurrentSelectedLine = ParentLine2;
		if(addaction)
			AddShapeAction::addShape(ParentLine2);
		else
		{
			ParentLine2->AddActionWhileEdit(false);
			RCollectionBase& rshapes = MAINDllOBJECT->getUCS(0)->getShapes();
			rshapes.addItem(ParentLine2);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0005", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::AddFirstLine_WithoutLocalised()
{
	try
	{
		ParentLine3 = new Line();
		CurrentSelectedLine = ParentLine3;
		AddShapeAction::addShape(ParentLine3);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0004", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::AddSecondLine_WithoutLocalised()
{
	try
	{
		ParentLine4 = new Line();
		CurrentSelectedLine = ParentLine4;
		AddShapeAction::addShape(ParentLine4);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0005", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::SelectCurrentGrabbingLine(int LCnt)
{
	try
	{
		if(LCnt == 1)
			CurrentSelectedLine = ParentLine1;
		else if(LCnt == 2)
			CurrentSelectedLine = ParentLine2;
		else if(LCnt == 3)
			CurrentSelectedLine = ParentLine3;
		else if(LCnt == 4)
			CurrentSelectedLine = ParentLine4;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0005", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::CalculateWidthBnTwoLines(RAction *caction)
{
	try
	{
		WidthMeasure = new DimPointToLineDistance(_T("Dist"));
		Vector *Center1 = ((Line*)ParentLine1)->getMidPoint(), *Center2 = ((Line*)ParentLine2)->getMidPoint();
		WidthMeasure->SetMousePosition((Center1->getX() + Center2->getX())/2, (Center1->getY() + Center2->getY())/2, 0, false);
		((DimPointToLineDistance*)WidthMeasure)->CalculateMeasurement(ParentLine2, ((Line*)ParentLine1)->getMidPoint());
		AddShapeMeasureRelationShip(ParentLine1);
		AddShapeMeasureRelationShip(ParentLine2);
		if(caction != NULL)
		{
			if(!WidthMeasure->ChildMeasurementType())
			{
				WidthMeasure->ParentPoint1 = MAINDllOBJECT->getVectorPointer(WidthMeasure->ParentPoint1);
				if(WidthMeasure->ParentPoint1->IntersectionPoint)
					((IsectPt*)WidthMeasure->ParentPoint1)->SetRelatedDim(WidthMeasure);
			}
			WidthMeasure->AddActionWhileEdit(false);
			RCollectionBase& rmeasure = MAINDllOBJECT->getDimList();
			rmeasure.addItem(WidthMeasure);
			if(WidthMeasure->selected())
				MAINDllOBJECT->getSelectedDimList().addItem(WidthMeasure);
			MAINDllOBJECT->selectMeasurement(WidthMeasure->getId(), false);
			MAINDllOBJECT->Measurement_updated();
			((LocalisedCorrectionAction*)caction)->AddLocalisedCorrectionAction(ParentLine1, ParentLine2, WidthMeasure);
		}
		else
			AddDimAction::addDim(WidthMeasure);
		MeasureCollection.addItem(WidthMeasure, false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0006", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::CalculateSGWidthBnTwoLines_Localised(bool flag)
{
	try
	{
		if(flag)
			WidthMeasure = new DimPointToLineDistance(_T("Dist_L"));
		else
			WidthMeasure = new DimPointToLineDistance(_T("Dist"));
		Vector *Center1 = ((Line*)ParentLine1)->getMidPoint(), *Center2 = ((Line*)ParentLine2)->getMidPoint();
		WidthMeasure->SetMousePosition((Center1->getX() + Center2->getX())/2, (Center1->getY() + Center2->getY())/2, 0, false);
		((DimPointToLineDistance*)WidthMeasure)->CalculateMeasurement(ParentLine2, ((Line*)ParentLine1)->getMidPoint());
		AddShapeMeasureRelationShip(ParentLine1);
		AddShapeMeasureRelationShip(ParentLine2);
		if(!MAINDllOBJECT->RepeatSGWidth)
		AddDimAction::addDim(WidthMeasure);
		CALIBRATIONTEXT->MeasureArray.push_back(WidthMeasure->getCDimension());
		CALIBRATIONTEXT->MeasurementArray.push_back(WidthMeasure->getDimension());
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0006", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::CalculateSGWidthBnTwoLines_WithoutLocalised()
{
	try
	{
		WidthMeasure = new DimPointToLineDistance(_T("Dist"));
		Vector *Center1 = ((Line*)ParentLine3)->getMidPoint(), *Center2 = ((Line*)ParentLine4)->getMidPoint();
		WidthMeasure->SetMousePosition((Center1->getX() + Center2->getX())/2, (Center1->getY() + Center2->getY())/2, 0, false);
		((DimPointToLineDistance*)WidthMeasure)->CalculateMeasurement(ParentLine4, ((Line*)ParentLine3)->getMidPoint());
		AddShapeMeasureRelationShip(ParentLine3);
		AddShapeMeasureRelationShip(ParentLine4);
		AddDimAction::addDim(WidthMeasure);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0006", __FILE__, __FUNCSIG__); }
}

//grabing points on edge to make a line
void CalibrationCalculation::GrabPointsToSelectedLine(int type, bool addaction)
{
	try
	{
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y;
		double upp_x = MAINDllOBJECT->getWindow(0).getUppX() / HELPEROBJECT->CamSizeRatio, upp_y = MAINDllOBJECT->getWindow(0).getUppY() / HELPEROBJECT->CamSizeRatio;
		int noofpts, RectFGArray[4] = {0, 0, MAINDllOBJECT->getWindow(0).getWinDim().x * HELPEROBJECT->CamSizeRatio, MAINDllOBJECT->getWindow(0).getWinDim().y * HELPEROBJECT->CamSizeRatio };
		MAINDllOBJECT->SetImageInEdgeDetection();
		noofpts = EDSCOBJECT->DetectEdgeStraightRect(&RectFGArray[0], EdgeDetectionSingleChannel::ScanDirection(type));				//actually points are taken in this function we send a window coordinates as rectangle
		if(addaction)
		{
			FramegrabBase* baseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB);
			baseaction->PointDRO.set(MAINDllOBJECT->getCurrentDRO());
			if(MAINDllOBJECT->CameraConnected())
			{
				baseaction->camProperty = MAINDllOBJECT->getCamSettings();
				baseaction->lightProperty = MAINDllOBJECT->getLightProperty();
				baseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
			}
			baseaction->ProfileON = MAINDllOBJECT->getProfile();
			baseaction->SurfaceON = MAINDllOBJECT->getSurface();
			for(int n = 0; n < noofpts; n++)
				baseaction->AllPointsList.Addpoint(new SinglePoint(cx + EDSCOBJECT->DetectedPointsList[n * 2] * upp_x, cy - EDSCOBJECT->DetectedPointsList[n * 2 + 1] * upp_y, 0.0));
			AddPointAction::pointAction(CurrentSelectedLine, baseaction);
		}
		else
		{
			PointCollection ptcol;
			for(int n = 0; n < noofpts; n++)
				ptcol.Addpoint(new SinglePoint(cx + EDSCOBJECT->DetectedPointsList[n * 2] * upp_x, cy - EDSCOBJECT->DetectedPointsList[n * 2 + 1] * upp_y, 0.0));
			CurrentSelectedLine->AddPoints(&ptcol);		
		}
		Sleep(100);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0007", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::GrabPointsToSelectedLine(int type, int RowNum, int ColNum, int SlipGaugeWidth, int LineNumber)
{
	try
	{
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y;
		double upp_x = MAINDllOBJECT->getWindow(0).getUppX() / HELPEROBJECT->CamSizeRatio, upp_y = MAINDllOBJECT->getWindow(0).getUppY() / HELPEROBJECT->CamSizeRatio;
		int noofpts, RectFGArray[4] = {0};
		
		if(type == 0)
		{
			RectFGArray[0] = ((ColNum + LineNumber) * SlipGaugeWidth / upp_x - 20);
			RectFGArray[1] = (MAINDllOBJECT->getWindow(0).getWinDim().y * HELPEROBJECT->CamSizeRatio - 1 - (RowNum + 1) * SlipGaugeWidth / upp_y - 100);
			RectFGArray[2] = 40;
			RectFGArray[3] = 100;
		}
		else
		{
			RectFGArray[0] = ((ColNum + 1) * SlipGaugeWidth / upp_x);
			RectFGArray[1] = (MAINDllOBJECT->getWindow(0).getWinDim().y * HELPEROBJECT->CamSizeRatio - 1 - (RowNum + LineNumber) * SlipGaugeWidth / upp_y - 20);
			RectFGArray[2] = 100;
			RectFGArray[3] = 40;
		}
		MAINDllOBJECT->SetImageInEdgeDetection();
		noofpts = EDSCOBJECT->DetectEdgeStraightRect(&RectFGArray[0], EdgeDetectionSingleChannel::ScanDirection(type));
		FramegrabBase* baseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB);
		baseaction->PointDRO.set(MAINDllOBJECT->getCurrentDRO());
		if(MAINDllOBJECT->CameraConnected())
		{
			baseaction->camProperty = MAINDllOBJECT->getCamSettings();
			baseaction->lightProperty = MAINDllOBJECT->getLightProperty();
			baseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
		}
		baseaction->ProfileON = MAINDllOBJECT->getProfile();
		baseaction->SurfaceON = MAINDllOBJECT->getSurface();
		for(int n = 0; n < noofpts; n++)
			baseaction->AllPointsList.Addpoint(new SinglePoint(cx + EDSCOBJECT->DetectedPointsList[n * 2] * upp_x, cy - EDSCOBJECT->DetectedPointsList[n * 2 + 1] * upp_y, 0.0));
		AddPointAction::pointAction(CurrentSelectedLine, baseaction);
		Sleep(100);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0007a", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::GrabPoints_WidthMeasureToSelectedLine()
{
	try
	{
		FramegrabBase* baseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB);
		baseaction->PointDRO.set(MAINDllOBJECT->getCurrentDRO());
		double bordersize = 20;
		double cR = HELPEROBJECT->CamSizeRatio;
		int noofpts, RectFGArray[4] = {bordersize, bordersize, MAINDllOBJECT->getWindow(0).getWinDim().x * cR - bordersize, MAINDllOBJECT->getWindow(0).getWinDim().y * cR - bordersize};
		if(MAINDllOBJECT->CameraConnected())
		{
			baseaction->camProperty = MAINDllOBJECT->getCamSettings();
			baseaction->lightProperty = MAINDllOBJECT->getLightProperty();
			baseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
			baseaction->ChannelNo = 1;
			baseaction->FGdirection = 0;
			baseaction->FGWidth = 80;
			baseaction->FGtype = RapidEnums::RAPIDFGACTIONTYPE::RECTANGLEFRAMEGRAB;
			Vector* Ptt = new Vector(bordersize, bordersize, 0);
			baseaction->myPosition[0] = *Ptt;
			Ptt = new Vector(MAINDllOBJECT->getWindow(0).getWinDim().x * cR - bordersize, MAINDllOBJECT->getWindow(0).getWinDim().y * cR - bordersize, 0);
			baseaction->myPosition[1] = *Ptt;
			baseaction->ProfileON = MAINDllOBJECT->getProfile();
			baseaction->SurfaceON = MAINDllOBJECT->getSurface();
		}
		HELPEROBJECT->DoCurrentFrameGrab_SurfaceOrProfile(baseaction, false, true);
		if (baseaction->noofpts < 200)
		{
			baseaction->FGdirection = 1;
			HELPEROBJECT->DoCurrentFrameGrab_SurfaceOrProfile(baseaction, false, true);
		}
		//double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y;
		//double upp_x = MAINDllOBJECT->getWindow(0).getUppX(), upp_y = MAINDllOBJECT->getWindow(0).getUppY();
		//MAINDllOBJECT->SetImageInEdgeDetection();
		//noofpts = EDSCOBJECT->DetectEdgeStraightRect(&RectFGArray[0], EdgeDetectionSingleChannel::ScanDirection::Rightwards);
		//if(noofpts < 400)
		//	noofpts = EDSCOBJECT->DetectEdgeStraightRect(&RectFGArray[0], EdgeDetectionSingleChannel::ScanDirection::Downwards);
		//for(int n = 0; n < noofpts; n++)
		//	baseaction->AllPointsList.Addpoint(new SinglePoint(cx + EDSCOBJECT->DetectedPointsList[n * 2] * upp_x, cy - EDSCOBJECT->DetectedPointsList[n * 2 + 1] * upp_y, 0.0));
		AddPointAction::pointAction(CurrentSelectedLine, baseaction);
		Sleep(100);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0007b", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::CheckCurrentSlipgaugeParam()
{
	try
	{
		double cR = HELPEROBJECT->CamSizeRatio;
		int wwidth = MAINDllOBJECT->getWindow(0).getWinDim().x * cR, wheight = MAINDllOBJECT->getWindow(0).getWinDim().y * cR;
		int ImLength = wwidth * wheight, QdWidth = wwidth/2, Qdheight = wheight/2;
		bool Tempflag = GetImageNow_double_1bpp(1, &MAINDllOBJECT->pixelvalues[0], true); // &Edpixelvalues[0], true);
		int BlckCnt_1Qd = 0, BlckCnt_2Qd = 0, BlckCnt_3Qd = 0, BlckCnt_4Qd = 0; 
		if(!Tempflag)
		{
			Sleep(100);
			Tempflag = GetImageNow_double_1bpp(1, &MAINDllOBJECT->pixelvalues[0], true);
		}
		for(int i = 0; i < Qdheight; i++)
		{
			for(int j = 0; j < QdWidth; j++)
			{
				double FirstQdValue = MAINDllOBJECT->pixelvalues[i * wwidth + j + QdWidth];
				double ScndQdValue = MAINDllOBJECT->pixelvalues[i * wwidth + j];
				double ThirdQdValue = MAINDllOBJECT->pixelvalues[(i + Qdheight) * wwidth + j];
				double FourthQdValue = MAINDllOBJECT->pixelvalues[(i + Qdheight) * wwidth + j + QdWidth];
				if(FirstQdValue < 50)
					BlckCnt_1Qd++;
				if(ScndQdValue < 50)
					BlckCnt_2Qd++;
				if(ThirdQdValue < 50)
					BlckCnt_3Qd++;
				if(FourthQdValue < 50)
					BlckCnt_4Qd++;
			}
		}

		int noofpts;
		int RectFGArray[4] = { 40, 40, MAINDllOBJECT->getWindow(0).getWinDim().x - 40, MAINDllOBJECT->getWindow(0).getWinDim().y - 40}; //multiply by camsize ratio here
		MAINDllOBJECT->SetImageInEdgeDetection();
		noofpts = EDSCOBJECT->DetectEdgeStraightRect(&RectFGArray[0], EdgeDetectionSingleChannel::ScanDirection::Rightwards);
		if(noofpts < 400)
			noofpts = EDSCOBJECT->DetectEdgeStraightRect(&RectFGArray[0], EdgeDetectionSingleChannel::ScanDirection::Downwards);

		double ans[2] = {0};
		double cx = MAINDllOBJECT->getWindow(0).getCam().x * cR, cy = MAINDllOBJECT->getWindow(0).getCam().y * cR; //multiply camsizeratio here
		double upp_x = MAINDllOBJECT->getWindow(0).getUppX() / HELPEROBJECT->CamSizeRatio, upp_y = MAINDllOBJECT->getWindow(0).getUppY() / HELPEROBJECT->CamSizeRatio;
		double* EdPoints = (double*)calloc(noofpts * 2, sizeof(double));
		for(int n = 0; n < noofpts; n++)
		{
			EdPoints[2 * n] = cx + EDSCOBJECT->DetectedPointsList[n * 2] * upp_x;
			EdPoints[2 * n + 1] = cy - EDSCOBJECT->DetectedPointsList[n * 2 + 1] * upp_y;
		}
		BESTFITOBJECT->Line_BestFit_2D(EdPoints, noofpts, &ans[0], false);
		RMATH2DOBJECT->Angle_FirstScndQuad(&ans[0]);
		CurrentSlipGaugeEdgeAngle(ans[0]);
		SlipGaugeLeftToRight(false); SlipGaugeRightToLeft(false); SlipGaugeTopToBtm(false); SlipGaugeBtmToTop(false);
		int TopCnt = BlckCnt_1Qd + BlckCnt_2Qd;
		int BtmCnt = BlckCnt_3Qd + BlckCnt_4Qd;
		int RightCnt = BlckCnt_1Qd + BlckCnt_4Qd;
		int LeftCnt = BlckCnt_2Qd + BlckCnt_3Qd;
		if((RightCnt > TopCnt) && (RightCnt > BtmCnt) && (RightCnt > LeftCnt))
			SlipGaugeLeftToRight(true);
		else if((LeftCnt > RightCnt) && (LeftCnt > BtmCnt) && (LeftCnt > TopCnt))
			SlipGaugeRightToLeft(true);
		else if((TopCnt > RightCnt) && (TopCnt > BtmCnt) && (TopCnt > LeftCnt))
			SlipGaugeBtmToTop(true);
		else if((BtmCnt > RightCnt) && (BtmCnt > LeftCnt) && (BtmCnt > TopCnt))
			SlipGaugeTopToBtm(true);
		double Modified_Angle = ans[0] - M_PI_2;
		RMATH2DOBJECT->Angle_FirstScndQuad(&Modified_Angle);
		
		if(SlipGaugeLeftToRight())
		{
			if(Modified_Angle > M_PI_2)
				Modified_Angle = Modified_Angle + M_PI;
		}
		else if(SlipGaugeRightToLeft())
		{
			if(Modified_Angle < M_PI_2)
				Modified_Angle = Modified_Angle + M_PI;
		}
		else if(SlipGaugeBtmToTop())
		{
		}
		else if(SlipGaugeTopToBtm())
		{
			Modified_Angle = Modified_Angle + M_PI;
		}
		CurrentSlipGaugeEdgeAngle(Modified_Angle);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0008", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::AddShapeMeasureRelationShip(Shape* CShape)
{
	try
	{
		if(CShape != NULL)
		{
			CShape->addMChild(WidthMeasure);
			WidthMeasure->addParent(CShape);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0008", __FILE__, __FUNCSIG__); }
}


void CalibrationCalculation::MedianOfMeasurement(double *measurements, double *medianMeasure)
{
	try
	{
	   if((measurements[0] >= measurements[1]) && (measurements[0] >= measurements[2]))
	   {
		   if(measurements[1] >= measurements[2])
			   *medianMeasure = measurements[1];
		   else
			   *medianMeasure = measurements[2];
	   }
	   else if((measurements[1] >= measurements[0]) && (measurements[1] >= measurements[2]))
	   {
		   if(measurements[0] >= measurements[2])
			   *medianMeasure = measurements[0];
		   else
			   *medianMeasure = measurements[2];
	   }
	   else if((measurements[2] >= measurements[0]) && (measurements[2] >= measurements[1]))
	   {
		   if(measurements[0] >= measurements[1])
			   *medianMeasure = measurements[0];
		   else
			   *medianMeasure = measurements[1];
	   }
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0008a", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::AverageOfMeasurement(double *measurements, double *avgMeasure)
{
	try
	{
		*avgMeasure = (measurements[0] + measurements[1] + measurements[2]) / 3;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0008b", __FILE__, __FUNCSIG__); }
}


FramegrabBase* CalibrationCalculation::FgofFirstLine()
{
	try
	{
		AddPointAction* Caction = (AddPointAction*)(*ParentLine1->PointAtionList.begin());
		return Caction->getFramegrab();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0009", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::ClearAllCalibrationCalcualtions()
{
	try
	{
		MeasureCollection.clearAll();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0010", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::DotheCalibration(double *MeasureMedianValues, bool isMedian)
{
	try
	{

		/*fncalibration(50, 4, 3, &MeasureValues[0]);
		for(int i = 0; i < 36; i++)
		{
			CorrectedValues[2 * i] = MeasureValues[5 * i + 3];
			CorrectedValues[2 * i + 1] = MeasureValues[5 * i + 4];
		}
		return;*/
		memset(CorrectedValues, 0, 400 * sizeof(double) * 3);
		int Cnt = 0;
		for(RC_ITER DimItem = MeasureCollection.getList().begin(); DimItem != MeasureCollection.getList().end(); DimItem++)
		{
			DimBase* Cdim = (DimBase*)(*DimItem).second;
			MeasureValues[Cnt++] = Cdim->getDimension();
			if(!OneShotMode())
			{
				if(Cnt % 3 == 0)
				{
				  if(isMedian)
					  MedianOfMeasurement(&MeasureValues[Cnt - 3], &MeasureMedianValues[Cnt/3 - 1]);
				  else
					  AverageOfMeasurement(&MeasureValues[Cnt - 3], &MeasureMedianValues[Cnt/3 - 1]);
				}
			}
		}
		if(OneShotMode())
			BESTFITOBJECT->fnOneShotCalibration(SlipGaugeWidth, NoofColumns, NoofRows, &MeasureValues[0], &CorrectedValues[0]);
		else
			BESTFITOBJECT->fncalibration2(SlipGaugeWidth, NoofColumns, NoofRows, &MeasureMedianValues[0], &CorrectedValues[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0011", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::DotheCalibration_Average(std::list<double> *Values)
{
	try
	{
		int Cnt = 0;
		for each(double val in *Values)
		{
			MeasureValues[Cnt++] = val;
		}
		if(this->OneShotMode())
			BESTFITOBJECT->fnOneShotCalibration(SlipGaugeWidth, NoofColumns, NoofRows, &MeasureValues[0], &CorrectedValues[0]);
		else
			BESTFITOBJECT->fncalibration2(SlipGaugeWidth, NoofColumns, NoofRows, &MeasureValues[0], &CorrectedValues[0]);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0012", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::SetCurrentMeasurementOrientation(int type)
{
	try
	{
		XWidthMeasurementMode(false); YWidthMeasurementMode(false); AngleWidthMeasurementMode(false);
		if(type == 0)
			XWidthMeasurementMode(true);
		else if(type == 1)
			YWidthMeasurementMode(true);
		else if(type == 2)
			AngleWidthMeasurementMode(true);
		MAINDllOBJECT->update_VideoGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0013", __FILE__, __FUNCSIG__); }
}

LocalisedCorrectionAction *CalibrationCalculation::GetLastAction()
{
	try
	{
		LocalisedCorrectionAction *cact = NULL;
		for(RC_ITER pp = MAINDllOBJECT->getActionsHistoryList().getList().begin(); pp != MAINDllOBJECT->getActionsHistoryList().getList().end(); pp++)
		{
			if(((RAction*)pp->second)->CurrentActionType == RapidEnums::ACTIONTYPE::LOCALISEDCORRECTION)
			{
				cact = (LocalisedCorrectionAction*)pp->second;
				MeasureCollection.addItem(((LocalisedCorrectionAction*)pp->second)->measurement, false);
			}
		}
		return cact;
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0013d", __FILE__, __FUNCSIG__);
	}
}

void CalibrationCalculation::ClearLocalisedAction()
{
	for(RC_ITER pp = MAINDllOBJECT->getActionsHistoryList().getList().begin(); pp != MAINDllOBJECT->getActionsHistoryList().getList().end(); pp++)
	{
		if(((RAction*)pp->second)->CurrentActionType == RapidEnums::ACTIONTYPE::LOCALISEDCORRECTION)
		{
			delete pp->second;
			MAINDllOBJECT->getActionsHistoryList().getList().erase(pp->second->getId());
		}
	}
}

void CalibrationCalculation::SetCurrentMeasurementOrientation(int type, int RowNum, int ColNum, int SlipGaugeWidth)
{
	try
	{
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y, upp = MAINDllOBJECT->getWindow(0).getUppX() / HELPEROBJECT->CamSizeRatio;
		int noofpts, RectFGArray[4] = {0};
		
		if(type == 0)
		{
			this->LineX1(cx + (ColNum + 1) * SlipGaugeWidth);
			LineY1(cy - (MAINDllOBJECT->getWindow(0).getWinDim().y * upp) + (RowNum + 1) * SlipGaugeWidth);
			LineX2(LineX1());
			LineY2(LineY1() + 100 * upp);
		}
		else
		{
			LineX1(cx + (ColNum + 1) * SlipGaugeWidth);
			LineY1(cy - (MAINDllOBJECT->getWindow(0).getWinDim().y * upp) + (RowNum + 1) * SlipGaugeWidth);
			LineX2(LineX1() + 100 * upp);
			LineY2(LineY1());
		}
		XWidthMeasurementMode(false); YWidthMeasurementMode(false); AngleWidthMeasurementMode(false);
		if(type == 0)
			XWidthMeasurementMode(true);
		else if(type == 1)
			YWidthMeasurementMode(true);
		else if(type == 2)
			AngleWidthMeasurementMode(true);
		MAINDllOBJECT->update_VideoGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0013a", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::SetSlipGaugeWidth(double w)
{
	SlipGaugeWidth = w;
}

void CalibrationCalculation::SetNofRowColumns(int r, int c)
{
	NoofRows = r;
	NoofColumns = c;
}

void CalibrationCalculation::SetCalibrationCorrectionValues(std::list<double> *Values, double slipgwidth, int rows, int columns)
{
	try
	{
		int Cnt = 0;
		for each(double val in *Values)
		{
			CorrectedValues[Cnt++] = val;
		}
		BESTFITOBJECT->setErrorCorrectionsArray2(slipgwidth, columns, rows, CorrectedValues);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0014", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::AddAndRemoveMeasure(bool addmeasure, DimBase* cdim)
{
	if(addmeasure)
	{
	   if(!MeasureCollection.ItemExists(cdim->getId()))
		   MeasureCollection.addItem(cdim);
	}
	else
	{
	   if(MeasureCollection.ItemExists(cdim->getId()))
		   MeasureCollection.removeItem(cdim);
	}
}

void CalibrationCalculation::CreateCorrectedMeasurements()
{
	try
	{
		DimBase::reset();
		int TempMCount = 0, ignoreOneOutThree = 0;
		for(RC_ITER DimItem = MeasureCollection.getList().begin(); DimItem != MeasureCollection.getList().end(); DimItem++)
		{
			ignoreOneOutThree++;
			if(ignoreOneOutThree % 3 != 0) continue;
			DimBase* Cdim = (DimBase*)(*DimItem).second;
			list<BaseItem*>::iterator ptr = Cdim->getParents().end(); 
			ptr--;
			ParentLine3 = (ShapeWithList*)(*ptr--);
			ParentLine4 = (ShapeWithList*)(*ptr);
			PointCollection Sh1PointColl, Sh2PointColl;
			AddFirstLine(); AddSecondLine();
			for(PC_ITER SptItem = ParentLine3->PointsList->getList().begin(); SptItem != ParentLine3->PointsList->getList().end(); SptItem++)
			{
				SinglePoint* Spt = (*SptItem).second;
				double DroValueX = Spt->X, DroValueY = Spt->Y;
				//if(MAINDllOBJECT->UseLocalisedCorrection)
				BESTFITOBJECT->correctXYCoord2(&DroValueX, &DroValueY);
				Sh1PointColl.Addpoint(new SinglePoint(DroValueX, DroValueY));
			}

			for(PC_ITER SptItem = ParentLine4->PointsList->getList().begin(); SptItem != ParentLine4->PointsList->getList().end(); SptItem++)
			{
				SinglePoint* Spt = (*SptItem).second;
				double DroValueX = Spt->X, DroValueY = Spt->Y;
				//if(MAINDllOBJECT->UseLocalisedCorrection)
				BESTFITOBJECT->correctXYCoord2(&DroValueX, &DroValueY);
				Sh2PointColl.Addpoint(new SinglePoint(DroValueX, DroValueY));
			}
			ParentLine1->AddPoints(&Sh1PointColl); 
			ParentLine2->AddPoints(&Sh2PointColl); 
			WidthMeasure = new DimPointToLineDistance(_T("Dist_L"));
			Vector *Center1 = ((Line*)ParentLine1)->getMidPoint();
			if(TempMCount == 2)
			{
				TempMCount = 0;
				WidthMeasure->SetMousePosition(Center1->getX() + 20, Center1->getY() + 20, 0, false);
			}
			else
				WidthMeasure->SetMousePosition(Center1->getX() - 10, Center1->getY() - 10, 0, false);
			((DimPointToLineDistance*)WidthMeasure)->CalculateMeasurement(ParentLine2, ((Line*)ParentLine1)->getMidPoint());
			WidthMeasure->MeasureColor.set(1, 1, 0);
			AddShapeMeasureRelationShip(ParentLine1);
			AddShapeMeasureRelationShip(ParentLine2);
			AddDimAction::addDim(WidthMeasure);
			TempMCount++;
		}
		MAINDllOBJECT->getDimList().clearSelection();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0014", __FILE__, __FUNCSIG__); }
}

void CalibrationCalculation::SetCurrentSelectedLine(Shape *Cshape)
{
	try
	{
		CurrentSelectedLine = (ShapeWithList*)Cshape;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CALBCALC0015", __FILE__, __FUNCSIG__); }
}

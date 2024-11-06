#include "StdAfx.h"
#include <process.h>
#include "FocusFunctions.h"
#include "..\Engine\RCadApp.h"
#include "..\Handlers_FrameGrab\FocusDepthMultiBoxHandler.h"
#include "..\Handlers_FrameGrab\ContourScan_FG.h"
#include "..\Measurement\DimDepth.h"
#include "..\Shapes\DepthShape.h"
#include "..\Shapes\DummyShape.h"
#include "..\Shapes\Plane.h"
#include "..\Shapes\Line.h"
#include "..\Shapes\Circle.h"
#include "..\Shapes\Cone.h"
#include "..\Shapes\Cylinder.h"
#include "..\Shapes\Sphere.h"
#include "..\Shapes\CloudPoints.h"
#include "..\Actions\AddDimAction.h"
#include "..\Actions\AddShapeAction.h"
#include "..\Actions\DeleteShapesAction.h"
#include "..\Actions\AddPointAction.h"
#include "..\Handlers\AutoFocusScanHandler.h"
#include "..\Handlers\AutoContourScanHandler.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Helper\Helper.h"

int FocusCtNumber;
//semaphores - Focus thread doesn't loop unnecessarily they wait till next SetEvent..
HANDLE AutoFocus_Semaphore, FocusDepth_Semaphore, RightClickFocus_Semaphore;

//double* FiveFocus;
void FocusfunctionThread(void* anything)
{
	try
	{
		//Create a semaphore for Focus Depth thread
		FocusDepth_Semaphore = CreateEvent(NULL, TRUE, FALSE, _T("DepthFocusThread"));
		while(MAINDllOBJECT->ThreadRunning)
		{
			try
			{
				if(FOCUSCALCOBJECT->CalculateFocusMetric())
				   FOCUSCALCOBJECT->FocusMetricSurfaceLight();
				FOCUSCALCOBJECT->CalculateFocusMetric(false);
				WaitForSingleObject(FocusDepth_Semaphore, INFINITE); 
				ResetEvent(FocusDepth_Semaphore);
			}
			catch(...)
			{ 
				FOCUSCALCOBJECT->RapidCamFocusIshappening(false); FOCUSCALCOBJECT->CalculateFocusMetric(false);
				MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALCTH0001", __FILE__, __FUNCSIG__); 
				_endthread();
			}
		}
		_endthread();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALCTH0001", __FILE__, __FUNCSIG__); }
}

void AutoFocusfunctionThread(void* anything)
{
	try
	{
		//Create a semaphore for Auto Focus thread
		AutoFocus_Semaphore = CreateEvent(NULL, TRUE, FALSE, _T("AutoFocusThread"));
		while(MAINDllOBJECT->ThreadRunning)
		{
			try
			{
				if(FOCUSCALCOBJECT->CalculateFocusMetric())
					FOCUSCALCOBJECT->FocusMetricProfileLight();
				FOCUSCALCOBJECT->CalculateFocusMetric(false);
				WaitForSingleObject(AutoFocus_Semaphore, INFINITE); 
				ResetEvent(AutoFocus_Semaphore);
			}
			catch(...)
			{ 
				FOCUSCALCOBJECT->RapidCamFocusIshappening(false); FOCUSCALCOBJECT->CalculateFocusMetric(false);
				MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALCTH0001", __FILE__, __FUNCSIG__); 
				_endthread();
			}
		}
		_endthread();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALCTH0001", __FILE__, __FUNCSIG__); }
}

void ZCollectionThread(void* anything)
{
	try
	{
		while(MAINDllOBJECT->ThreadRunning)
		{
			try
			{
				DWORD currentTime = GetTickCount();
				FOCUSCALCOBJECT->ZRecordOverTime.insert(std::make_pair(currentTime, PPCALCOBJECT->CurrentMachineDRO.getZ()));
				int i = FOCUSCALCOBJECT->ZRecordOverTime.size();
				if(FOCUSCALCOBJECT->ZRecordOverTime.size() > 200)
					FOCUSCALCOBJECT->ZRecordOverTime.erase(FOCUSCALCOBJECT->ZRecordOverTime.begin());
			}
			catch(...)
			{ 
				 MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALCTH0001", __FILE__, __FUNCSIG__); 
				_endthread();
			}
		}
		_endthread();
	}
	catch(...)
	{ 
		MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALCTH0001b", __FILE__, __FUNCSIG__); 
	}
}

//constructor..
FocusFunctions::FocusFunctions()
{
	try
	{
		Rectptr = NULL;
		Cvalue = NULL;
		focusDelay = 0;
		this->selectedAxis = 2;
		this->FocusCalibrationIsDone(false);
		this->ApplyFocusCorrection(true);
		this->RapidCamFocusIshappening(false);
		this->CalculateFocusMetric(false);
		this->FocusStatusFlag(false);
		this->FocusCalibrationSuccess = true;
		AutoFocusBox[0] = 100; AutoFocusBox[1] = 100;
		AutoFocusBox[0] = 600; AutoFocusBox[0] = 400;
		this->FiveFocus = NULL;
		ClrearAllFocus(); 
		this->isfivefocus = true;
		this->FocusCalibError[0] = this->FocusCalibError[1] = 1;
		this->FocusCalibError[2] = this->FocusCalibError[3] = 0;
		this->GrainFactor(0);
		this->ApplyPixelCorrection(false);
		this->PixelCorrectionFactor(2);
		this->DelayInMS = 0;
		this->FocusAtOnePositionOnly = false;
		FocusCtNumber = 0;
		CalRow(15);
		CalCol(20); 
		CalWidth(40);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0001", __FILE__, __FUNCSIG__); }
}

//destructor..
FocusFunctions::~FocusFunctions()
{
	try
	{
		CalculateFocusMetric(false);
		SetEvent(AutoFocus_Semaphore);
		SetEvent(FocusDepth_Semaphore);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0002", __FILE__, __FUNCSIG__); }
}

void FocusFunctions::StartFocusRelatedThrread()
{
	_beginthread(&AutoFocusfunctionThread, 0, NULL); Sleep(10);
	_beginthread(&FocusfunctionThread, 0, NULL); Sleep(10);
}

void FocusFunctions::StartFocusDelayThrread(int delay)
{
	DelayInMS = delay;
   _beginthread(&ZCollectionThread, 0, NULL);
}

//Get current focus from rapidcam..
void FocusFunctions::GetCurrentFocusValue_Normal(double CZvalue)
{
	try
	{
		if(this->RapidCamFocusIshappening())
			return;
		CalculateFocusMetric(true);
		CurrentZvalue = CZvalue;
		//FocusMetricSurfaceLight();
		SetEvent(FocusDepth_Semaphore);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0003", __FILE__, __FUNCSIG__); }
}

//Set current focus values.. For single box.. Used in auto focus..
void FocusFunctions::GetCurrentFocusValue_Profile(double CZvalue)
{
	try
	{
		if(this->RapidCamFocusIshappening()) 
			return;
		CalculateFocusMetric(true);
		CurrentZvalue = CZvalue;
		//FocusMetricProfileLight();
		SetEvent(AutoFocus_Semaphore);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0004", __FILE__, __FUNCSIG__); }
}

void FocusFunctions::FocusMetricProfileLight()
{
	try
	{
		RapidCamFocusIshappening(true);
		//double CFocusValue = 0;
		double FocusFillValue = 0;
		bool flag = false;
		flag = GetFocusMetric_Profile(Rectptr, &Cvalue[0], 1); //&AutoFocusBox[0]    &CFocusValue
		//if(flag)
		//{
		//	ZvalueColl[NoofIterationCount] = CurrentZvalue;
		//	FocusValueColl[NoofIterationCount] = CFocusValue;
		//	NoofIterationCount++;
		//}
		//Make it exactly like surface light focusing!
		if (flag)
		{
			//Set the First focus vlaue, used to fill the focus bottle.//
			if (Focus_firstvalue && Cvalue[0] != 0)
			{
				FirstFocusvalue = Cvalue[0];
				Focus_firstvalue = false;
			}
			if (DelayInMS == 0)
				ZvalueColl[NoofIterationCount] = CurrentZvalue;
			else
				ZvalueColl[NoofIterationCount] = getZvalueBrforeDelay(ZRecordOverTime); //function to be used for delayfilteration from zvalue */
			for (int i = 0; i < TotalNoofCells; i++)
			{
				FocusValueColl[TotalFocusValueCount] = Cvalue[i];
				TotalFocusValueCount++;
			}
			NoofIterationCount++;
			if (Cvalue[0] > 0)
				FocusFillValue = abs((Cvalue[0] - FirstFocusvalue) / Cvalue[0]);
			UpdateCurrentFocusFillValue(FocusFillValue);
		}

		RapidCamFocusIshappening(false);
		CalculateFocusMetric(false);
	}
	catch(...){ RapidCamFocusIshappening(false); CalculateFocusMetric(false); MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0001", __FILE__, __FUNCSIG__); }
}

void FocusFunctions::FocusMetricSurfaceLight()
{
	try
	{
		RapidCamFocusIshappening(true);
		bool flag = GetFocusMetric(Rectptr, &Cvalue[0], TotalNoofCells, true, 2); // 2 * HELPEROBJECT->CamSizeRatio);
		double FocusFillValue = 0;
		if(flag)
		{
			//Set the First focus vlaue, used to fill the focus bottle.//
			if(Focus_firstvalue && Cvalue[0] != 0) 
			{
				FirstFocusvalue = Cvalue[0];
				Focus_firstvalue = false;
			}
			if(DelayInMS == 0 )
				ZvalueColl[NoofIterationCount] = CurrentZvalue;
			else
				ZvalueColl[NoofIterationCount] = getZvalueBrforeDelay(ZRecordOverTime); //function to be used for delayfilteration from zvalue */
			for(int i = 0; i < TotalNoofCells; i++)
			{
				FocusValueColl[TotalFocusValueCount] = Cvalue[i];
				TotalFocusValueCount++;
			}			
			NoofIterationCount++;
			if(Cvalue[0] > 0) 
				FocusFillValue = abs((Cvalue[0] - FirstFocusvalue)/Cvalue[0]);
			UpdateCurrentFocusFillValue(FocusFillValue);
		}
		RapidCamFocusIshappening(false);
		CalculateFocusMetric(false);
	}
	catch(...)
	{
		RapidCamFocusIshappening(false);
		CalculateFocusMetric(false); 
		MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0001", __FILE__, __FUNCSIG__); 
	}
};


double FocusFunctions::getZvalueBrforeDelay(map<unsigned long, double> zCollOverTime)
{
	DWORD currentT = GetTickCount();
	double currentZ = 0;
	int currentDiff = DelayInMS;
	for(map<unsigned long, double>::iterator It = zCollOverTime.begin(); It != zCollOverTime.end(); It++)
	{
	  if((currentT - It->first) <= currentDiff)
	  {
		 currentZ = It->second;
		 //currentDiff = currentT - It->first;
		 break;
	  }
	}
	return currentZ;
}

//Calculate the best focus...for auto focus..
double FocusFunctions::Calculate_BestAutoFocus()
{
	try
	{
		double ZValue = 0, temp = FocusValueColl[0];
		for(int i = 1; i < NoofIterationCount; i++)
		{
			if(temp < FocusValueColl[i])
			{
				temp = FocusValueColl[i];
				ZValue = ZvalueColl[i];
			}
		}
		return ZValue;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0005", __FILE__, __FUNCSIG__); }
}

//Set the no. of row, column, gap and width for focus rectangle...//
void FocusFunctions::setRowColumn(int row, int column, int gap, int wid, int hei)
{
	try
	{
		focusRectangleRow = row; focusRectangleColumn = column;
		focusRectangleGap = gap; focusRectangleHeight = hei; // * HELPEROBJECT->CamSizeRatio
		focusRectangleWidth = wid;// *HELPEROBJECT->CamSizeRatio;
		TotalNoofCells = focusRectangleRow * focusRectangleColumn;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0006", __FILE__, __FUNCSIG__); }
}

void FocusFunctions::setContourScanWidth(int d1, int d2)
{
	this->ContourScanWidth = d1;// *HELPEROBJECT->CamSizeRatio;
	this->ContourScanHeight = d2;// *HELPEROBJECT->CamSizeRatio;
}

//Focus mode flag... ON/OFF...//
void FocusFunctions::FocusOnStatus(bool flag)
{
	try
	{
		FocusOnMode(flag);
		MAINDllOBJECT->update_VideoGraphics();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0007", __FILE__, __FUNCSIG__); }
}

//calculate focus rectangle parameters....//
void FocusFunctions::CalculateRectangle()
{
	try
	{
		if(CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::TWOBOXDEPTH_MODE || CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::FOURBOXDEPTH_MODE) return;
		if(Cvalue != NULL) { free(Cvalue); Cvalue = NULL;}
		Cvalue = (double*)calloc(focusRectangleRow * focusRectangleColumn, sizeof(double));
		if(Rectptr != NULL) { free(Rectptr); Rectptr = NULL;}
		Rectptr = (int*)calloc(focusRectangleRow * focusRectangleColumn * 4, sizeof(int));
		int counter = 0;
		//double startX = (MAINDllOBJECT->getWindow(0).getWinDim().x / 2) - (((focusRectangleColumn - 1) * focusRectangleGap) + focusRectangleWidth * HELPEROBJECT->CamSizeRatio) / 2;
		//double StartY = (MAINDllOBJECT->getWindow(0).getWinDim().y / 2) - (((focusRectangleRow - 1) * focusRectangleGap) + focusRectangleHeight * HELPEROBJECT->CamSizeRatio) / 2;
		double startX = (MAINDllOBJECT->currCamWidth / 2) - (((focusRectangleColumn - 1) * focusRectangleGap) + focusRectangleWidth) / 2;
		double StartY = (MAINDllOBJECT->currCamHeight / 2) - (((focusRectangleRow - 1) * focusRectangleGap) + focusRectangleHeight) / 2;

		if (CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::HOMING_MODE || CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::REFERENCEDOT_SECOND)
		{
			Line* cLine = new Line();
			PointCollection PtColl;
			double rDotCentre[4] = { 0 };
			MAINDllOBJECT->ReferenceDotShape->GetShapeCenter(&rDotCentre[0]);
			if (CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::REFERENCEDOT_SECOND)
				MAINDllOBJECT->SecondReferenceDotShape->GetShapeCenter(&rDotCentre[0]);

			PtColl.Addpoint(new SinglePoint(rDotCentre[0], rDotCentre[1]));
			Vector cDR = MAINDllOBJECT->getCurrentDRO();
			PtColl.Addpoint(new SinglePoint(cDR.getX(), cDR.getY()));
			cLine->AddPoints(&PtColl);
			double intsectPt[4] = { 0 }, anspt[2] = { 0 };
			RMATH2DOBJECT->Line_circleintersection(cLine->Angle(), cLine->Intercept(), &rDotCentre[0], ((Circle*)MAINDllOBJECT->ReferenceDotShape)->Radius(), &intsectPt[0]);
			double cddr[2] = { cDR.getX(), cDR.getY() };
			double dist1 = RMATH2DOBJECT->Nearestpoint_point(2, &intsectPt[0], &cddr[0], &anspt[0]);
			double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y, upp = MAINDllOBJECT->getWindow(0).getUppX() / HELPEROBJECT->CamSizeRatio;
			startX = int((anspt[0] - cx) / upp) - focusRectangleWidth / 2; StartY = int((cy - anspt[1]) / upp) - focusRectangleWidth / 2;
			//AutoFocusBox[0] = startX;
			//AutoFocusBox[1] = StartY;
			//AutoFocusBox[2] = focusRectangleWidth;
			//AutoFocusBox[3] = focusRectangleWidth;
			//return;
			//Rectptr[counter++] = int((anspt[0] - cx) / upp); if (Rectptr[0] < 0) Rectptr[0] = MAINDllOBJECT->currCamWidth / 2;
			//Rectptr[counter++] = int((cy - anspt[1]) / upp); if (Rectptr[1] < 0) Rectptr[1] = MAINDllOBJECT->currCamHeight / 2;
			//Rectptr[counter++] = focusRectangleWidth; // int(focusRectangleWidth * HELPEROBJECT->CamSizeRatio);
			//Rectptr[counter++] = focusRectangleHeight; // int(focusRectangleHeight * HELPEROBJECT->CamSizeRatio);
			//drawFocusRectangels();
			//GRAFIX->drawFocusRectangle(focusRectangleRow, focusRectangleColumn, focusRectangleGap, focusRectangleWidth, focusRectangleHeight, startX, StartY);
			//return;
		}
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			startX += ShiftInPartProgramX;
			StartY += ShiftInPartProgramY;
		}
		for(int i = 0; i < focusRectangleRow; i++)
		{
			for(int j = 0; j < focusRectangleColumn; j++)
			{
				//Rectptr[counter++] = int(startX + (j * focusRectangleGap * HELPEROBJECT->CamSizeRatio));
				//Rectptr[counter++] = int(StartY + (i * focusRectangleGap * HELPEROBJECT->CamSizeRatio));
				//Rectptr[counter++] = int(focusRectangleWidth * HELPEROBJECT->CamSizeRatio);
				//Rectptr[counter++] = int(focusRectangleHeight * HELPEROBJECT->CamSizeRatio);

				Rectptr[counter++] = int(startX  + (j * focusRectangleGap));
				Rectptr[counter++] = int(StartY  + (i * focusRectangleGap));
				Rectptr[counter++] = int(focusRectangleWidth);
				Rectptr[counter++] = int(focusRectangleHeight);
			}
		}
		if(CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::FOCUSSCAN_MODE)
		{
			CreateGrainSurface();
			CreateLookUpTable();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0008", __FILE__, __FUNCSIG__); }
}

//Set AutoFocus Rectangle Size
void FocusFunctions::SetAutoFocusRectangle(int* rectArray)
{
	try
	{
		if (Rectptr != NULL) { free(Rectptr); Rectptr = NULL; }
		Rectptr = (int*)calloc(focusRectangleRow * focusRectangleColumn * 4, sizeof(int));

		for(int i = 0; i < 4; i++)
			//AutoFocusBox[i] = rectArray[i];
			Rectptr[i] = rectArray[i];
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0008", __FILE__, __FUNCSIG__); }
}

void FocusFunctions::CalculateDepthRectangle(int *RectArray, int NoofBoxes)
{
	try
	{
		if(Cvalue != NULL) {free(Cvalue); Cvalue = NULL;}
		Cvalue = (double*)calloc(NoofBoxes, sizeof(double));
		if(Rectptr != NULL) { free(Rectptr); Rectptr = NULL;}
		Rectptr = (int*)calloc(NoofBoxes * 4, sizeof(int));
		for(int i = 0; i < NoofBoxes * 4; i++)
			Rectptr[i] = RectArray[i];
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0008", __FILE__, __FUNCSIG__); }
}

//Calculate best focus depth/ focus scan..
void FocusFunctions::CalculateBestFocus()
{
	try
	{
		double *FocusXYZWithInSpan = NULL, FCvalue = 0;
		double* ZValue = (double*)calloc(focusRectangleColumn * focusRectangleRow, sizeof(double));
		double* Points = (double*)calloc(NoofIterationCount * 2, sizeof(double));
		int SampleCount = NoofIterationCount, cellsWithInSpan = 0;
		int CellCount = focusRectangleColumn * focusRectangleRow;
		switch(CurrentFocusType)
		{
			case RapidEnums::RAPIDFOCUSMODE::FOCUSSCAN_MODE:
			case RapidEnums::RAPIDFOCUSMODE::CONTOOURSCAN_MODE:
			case RapidEnums::RAPIDFOCUSMODE::CONTOURFRAMEGRAB:
			{
				double cx = MAINDllOBJECT->getWindow(0).getCam().x, 
					cy = MAINDllOBJECT->getWindow(0).getCam().y , 
					upp = MAINDllOBJECT->getWindow(0).getUppX() / HELPEROBJECT->CamSizeRatio;

				//if (CurrentCamera() == 3)
				//	cy = cy - MAINDllOBJECT->currCamHeight * upp;

				double FocusStartZvalue = 0, correction = 0;
				if(selectedAxis == 2)
					FocusStartZvalue = FocusStartPosition.getZ();
				else if (selectedAxis == 1)
				{
					FocusStartZvalue = FocusStartPosition.getY();
					//Modified 08 Sep 2015 for Reflector... We assume that we use XZ- and YZ- projections ONLY with reflectors at this time
					if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
						FocusStartZvalue = FocusStartPosition.getZ();
				}
				else if (selectedAxis == 0)
				{
					FocusStartZvalue = FocusStartPosition.getX();
					//Modified 08 Sep 2015 for Reflector... We assume that we use XZ- and YZ- projections ONLY with reflectors at this time
					if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
						FocusStartZvalue = FocusStartPosition.getZ();
				}
				double FocusSpanPos = FocusStartZvalue + fspan;
				double FocusSpanNeg = FocusStartZvalue - fspan;
				FocusXYZWithInSpan = (double*)calloc(CellCount * 3, sizeof(double));
				for(int j = 0; j < CellCount; j++)
				{
					int PtCnt = 0;
					for(int i = 0; i < SampleCount; i++)
					{
						FCvalue = FocusValueColl[CellCount * i + j];
						if(FCvalue >= 0)
						{
							Points[PtCnt++] = ZvalueColl[i];
							Points[PtCnt++] = FCvalue;
						}
					}
					bool checkflag1 = BESTFITOBJECT->CalculateFocus(&Points[0], PtCnt/2, FocusActiveWidth(), &ZValue[j], MaxMinFocusRatioCutoff(), MaxMinZRatioCutoff(), SigmaActiveWdthRatioCutoff());
					if(checkflag1)
					{
						//if (CurrentCamera() == 3) ZValue[j] -= 0.1; 
						if((ZValue[j] <= FocusSpanPos) && (ZValue[j] >= FocusSpanNeg))
						{
							if(ApplyFocusCorrection())
							{
								if(BESTFITOBJECT->getZError(CalCol(), CalRow(), CalWidth(), FocusErrCorrectionValue, FocusCalibError, 
									Rectptr[4 * j] + focusRectangleWidth,  //Rectptr[4 * j] + focusRectangleWidth * HELPEROBJECT->CamSizeRatio / 2, 
									Rectptr[4 * j + 1] + focusRectangleHeight / 2, &correction)) //Rectptr[4 * j + 1] + focusRectangleHeight * HELPEROBJECT->CamSizeRatio / 2, &correction))
									ZValue[j] -= correction;		  
							}

							FocusXYZWithInSpan[3 * cellsWithInSpan] =  cx + (Rectptr[4 * j] + (Rectptr[4 * j + 2] / 2)) * upp;

							//if (CurrentFocusType != RapidEnums::RAPIDFOCUSMODE::CONTOOURSCAN_MODE)
							//if (CurrentCamera() == 3)
							//	FocusXYZWithInSpan[3 * cellsWithInSpan + 1] = cy + (Rectptr[4 * j + 1] + (Rectptr[4 * j + 3] / 2)) * upp;
							//else
								FocusXYZWithInSpan[3 * cellsWithInSpan + 1] = cy - (Rectptr[4 * j + 1] + (Rectptr[4 * j + 3] / 2)) * upp;

							//else
							//	FocusXYZWithInSpan[3 * cellsWithInSpan + 1] = MAINDllOBJECT->getWindow(0).getCam().y + (Rectptr[4 * j + 1] + (Rectptr[4 * j + 3] / 2)) * upp;

							FocusXYZWithInSpan[3 * cellsWithInSpan + 2] = ZValue[j] - MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ();
							if(CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::FOCUSSCAN_MODE)
							{
								GrainSurfaceList[GrainLookUpTable[j]]->AddToList(FocusXYZWithInSpan[3 * cellsWithInSpan], FocusXYZWithInSpan[3 * cellsWithInSpan + 1], FocusXYZWithInSpan[3 * cellsWithInSpan + 2]);
								GrainLookUpTable[cellsWithInSpan] = GrainLookUpTable[j];
							}
							cellsWithInSpan++;
						}
					}
					else
					{
					  ZValue[j] = 1000;
					}
	/*	             char temp[10];
					itoa(j, &temp[0], 10);
					std::string CCount = (const char*)temp;
					std::string FileName = "E:\\Focus\\FocusValueTest" + CCount  + ".csv";
					std::wofstream curfile;
					curfile.open((char*)FileName.c_str());
					for(int i = 0; i < SampleCount; i++)
					{
						curfile << Points[2* i] << " , " << Points[2* i + 1] << endl;	
					}
					curfile << "Zvalue" << " , " << ZValue[j] << endl;
					if(checkflag1)
					{
						curfile << "Status" << " , " << "True" << endl;
					}
					else
					{
						curfile << "Status" << " , " << "False" << endl;
					}
					curfile.close();*/
				}
				break;
			}
			default:
			{
				std::string FileName = "E:\\Focus\\FocusValueTest.csv";
				//char temp[10];
				//itoa(FocusCtNumber, &temp[0], 10);
				//std::string CCount = (const char*)temp;
				//std::string FileName = "E:\\Focus\\FocusValueTest" + CCount + ".csv";
				//FocusCtNumber++;
				std::wofstream curfile;
				curfile.open((char*)FileName.c_str(), 10); // ios_base::out || ios_base::app);

				for(int j = 0; j < CellCount; j++)
				{
					int PtCnt = 0;
					curfile << "-----" << endl;
					for(int i = 0; i < SampleCount; i++)
					{
						FCvalue = FocusValueColl[CellCount * i + j];
						if(FCvalue >= 0)
						{
							Points[PtCnt++] = ZvalueColl[i];
							Points[PtCnt++] = FCvalue;
						}
						curfile << ZvalueColl[i] << ", " << FCvalue << endl;
					}
					curfile << "-----" << endl;
					bool checkflag1 = BESTFITOBJECT->CalculateFocus(&Points[0], PtCnt/2, FocusActiveWidth(), &ZValue[j], MaxMinFocusRatioCutoff(), MaxMinZRatioCutoff(), SigmaActiveWdthRatioCutoff());
					this->FocusStatusFlag(checkflag1);
					//if (CurrentCamera() == 3) ZValue[j] += 0.08;
					if(!checkflag1)	{ ZValue[j] = 1000; }
					//Write the values to file...
				 //   char temp[10];
		   //         itoa(j, &temp[0], 10);
					//std::string CCount = (const char*)temp;
					////std::string FileName = "E:\\Focus\\FocusValueTest" + CCount + ".csv";
					//std::string FileName = "E:\\Focus\\FocusValueTest.csv";
					//std::wofstream curfile;
					//curfile.open((char*)FileName.c_str(), ios_base::out||ios_base::app);
					//for(int i = 0; i < SampleCount; i++)
					//{
					//	curfile << Points[2 * i] << " , " << Points[2* i + 1] << endl;	
					//}
					curfile << j << " , " << ZValue[j] << " , "; // << endl; "Zvalue"
					if(checkflag1)
					{
						curfile << "True" << endl; //<< "Status" << " , "
					}
					else
					{
						curfile << "False" << endl; //<< "Status" << " , " <<
					}
					//curfile.close();
				}
				curfile.close();

				break;
			}
		}
  
		ClearLastFocusValues();

		switch(CurrentFocusType)
		{
			case RapidEnums::RAPIDFOCUSMODE::SCANFORRIGHTCLICK:
			{
				if(ZValue[0] < 200)
				   FocusZValue(ZValue[0]);
				else
				   FocusZValue(MAINDllOBJECT->getCurrentDRO().getZ());
				free(Points);
				free(ZValue);
				return;
			}
			case RapidEnums::RAPIDFOCUSMODE::TWOBOXDEPTH_MODE:
			{
				UpdateTwoBoxDepthMeasurement(ZValue);
				FocusZValue(ZValue[0]);
				this->FocusOnMode(false);
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
				MAINDllOBJECT->ClearAllRadioButtonOtherThrd();
				free(Points);
				free(ZValue);
				return;
			}
			case RapidEnums::RAPIDFOCUSMODE::FOURBOXDEPTH_MODE:
			{
				UpdateFourBoxDepthMeasurement(ZValue);
				FocusZValue(ZValue[0]);
				this->FocusOnMode(false);
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
				MAINDllOBJECT->ClearAllRadioButtonOtherThrd();
				free(Points);
				free(ZValue);
				return;
			}
			case RapidEnums::RAPIDFOCUSMODE::CALIBRATION_MODE:
			{
				UpdatePointsForFocusCalibration(ZValue);
				int Cnt = FocusCalibrationCount();
				Cnt += 1; FocusCalibrationCount(Cnt);
				double FZvalue = ZValue[int(focusRectangleColumn * focusRectangleRow/2)];
				if(FZvalue > 200)
					FocusZValue(MAINDllOBJECT->getCurrentDRO().getZ());
				else
					FocusZValue(FZvalue);
				free(Points);
				free(ZValue);
				return;
			}
			case RapidEnums::RAPIDFOCUSMODE::AUTOFOCUSSCANMODE:
			{
				if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::AUTOFOCUSSCANHANDLER)
				   ((AutoFocusScanHandler*)MAINDllOBJECT->getCurrentHandler())->UpdateAutoFocusScanPoints(ZValue);
				free(Points);
				free(ZValue);
				return;
			}
			case RapidEnums::RAPIDFOCUSMODE::PROFILESCAN_FOCUS:
				{
					if(abs(ZValue[0]) < 200)
						FocusZValue(ZValue[0]);
					else
						FocusZValue(MAINDllOBJECT->getCurrentDRO().getZ());
					free(Points);
					free(ZValue);
					return;
				}
			case RapidEnums::RAPIDFOCUSMODE::AUTOCONTOURSCANMODE:
			{
				if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::AUTOCONTOURSCANHANDLER)
				((AutoContourScanHandler*)MAINDllOBJECT->getCurrentHandler())->UpdateAutoContourScanPoints(ZValue);
				free(Points);
				free(ZValue);
				return;
			}
			case RapidEnums::RAPIDFOCUSMODE::PROBEVISIONOFFSETFOCUS:
			{
				if(abs(ZValue[0]) < 200)
					FocusZValue(ZValue[0]);
				else
					FocusZValue(MAINDllOBJECT->getCurrentDRO().getZ());
				free(Points);
				free(ZValue);
				return;
			}
			default:
			{
				double FZvalue = 0;
				FZvalue = ZValue[int(focusRectangleColumn * focusRectangleRow/2)];
				if(abs(FZvalue) < 200)
					FocusZValue(FZvalue);
				else
					FocusZValue(MAINDllOBJECT->getCurrentDRO().getZ());
				if( CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::NOTHING ||  CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::FOCUSDEPTH_MODE)
				{
					DepthZcol.clear();
					int cells = focusRectangleRow * focusRectangleColumn;
					for(int i = 0; i < cells; i++)
						DepthZcol.push_back(ZValue[i]);
					free(Points);
					free(ZValue);
					return;
				}
				break;
			}
		}

		ShapeWithList* currentshape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		if(currentshape == NULL || CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::HOMING_MODE || CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::NOTHING || 
			CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::FOCUS_VBLOCK_CALLIBRATION || CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::REFERENCEDOT_SECOND)
		{
			free(Points);
			free(ZValue);
			return;
		}	

		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			Cbaseaction = PPCALCOBJECT->getFrameGrab();
		}
		else
		{
			switch(CurrentFocusType)
			{
				case RapidEnums::RAPIDFOCUSMODE::CONTOOURSCAN_MODE:
					Cbaseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::FOCUS_CONTOURSCAN);
					break;
				case RapidEnums::RAPIDFOCUSMODE::FOCUSSCAN_MODE:
					Cbaseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::FOCUS_SCAN);
					break;
				case RapidEnums::RAPIDFOCUSMODE::CONTOURFRAMEGRAB:
					Cbaseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::CONTOURSCAN_FG);
					break;
				default:
					free(Points);
					free(ZValue);
					break;
			}
		
			if(MAINDllOBJECT->CameraConnected())
			{
				Cbaseaction->camProperty = MAINDllOBJECT->getCamSettings();
				Cbaseaction->lightProperty = MAINDllOBJECT->getLightProperty();
				Cbaseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
			}
			Cbaseaction->PointDRO = FocusStartPosition; 
			Cbaseaction->ProfileON = MAINDllOBJECT->getProfile();
			Cbaseaction->SurfaceON = MAINDllOBJECT->getSurface();
			if(CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::CONTOURFRAMEGRAB)
			{
				ContourScan_FG *tmpHandler = ((ContourScan_FG*)MAINDllOBJECT->getCurrentHandler());
				Cbaseaction->points[0].set(tmpHandler->PointsDRO[0]);
				Cbaseaction->points[1].set(tmpHandler->PointsDRO[1]);
				Cbaseaction->points[2].set(tmpHandler->PointsDRO[2]);
				Cbaseaction->myPosition[0].set(tmpHandler->SelectionShapeType, tmpHandler->SeletionFrames_Width);
			}
			else
			{
				Cbaseaction->myPosition[0].set(focusRectangleWidth, focusRectangleHeight);
				//Cbaseaction->myPosition[0].set(focusRectangleWidth * HELPEROBJECT->CamSizeRatio, focusRectangleHeight * HELPEROBJECT->CamSizeRatio);
				Cbaseaction->myPosition[1].set(focusRectangleRow , focusRectangleColumn, focusRectangleGap);
				Cbaseaction->myPosition[2].set(fspan , fspeed, selectedAxis);
			}
		}
		if(MAINDllOBJECT->CurrentMCSType == RapidEnums::MCSTYPE::MCSYZ)
		{
			for(int i = 0; i < cellsWithInSpan; i ++)
			{
				double pnt[3] = {FocusXYZWithInSpan[3 * i], FocusXYZWithInSpan[3 * i + 1], FocusXYZWithInSpan[3 * i + 2]};
				FocusXYZWithInSpan[3 * i] = pnt[2];
				FocusXYZWithInSpan[3 * i + 1] = pnt[0];
				FocusXYZWithInSpan[3 * i + 2] = pnt[1];
			}
		}
		else if(MAINDllOBJECT->CurrentMCSType == RapidEnums::MCSTYPE::MCSNEGYZ)
		{
			for(int i = 0; i < cellsWithInSpan; i ++)
			{
				double pnt[3] = {FocusXYZWithInSpan[3 * i], FocusXYZWithInSpan[3 * i + 1], FocusXYZWithInSpan[3 * i + 2]};
				FocusXYZWithInSpan[3 * i] = pnt[2];
				FocusXYZWithInSpan[3 * i + 1] = -pnt[0];
				FocusXYZWithInSpan[3 * i + 2] = pnt[1];
			}
		}
		else if(MAINDllOBJECT->CurrentMCSType == RapidEnums::MCSTYPE::MCSXZ)
		{
			for(int i = 0; i < cellsWithInSpan; i ++)
			{
				double pnt[3] = {FocusXYZWithInSpan[3 * i], FocusXYZWithInSpan[3 * i + 1], FocusXYZWithInSpan[3 * i + 2]};
				FocusXYZWithInSpan[3 * i] = pnt[0];
				FocusXYZWithInSpan[3 * i + 1] = pnt[2];
				FocusXYZWithInSpan[3 * i + 2] = pnt[1];
			}
		}
		else if(MAINDllOBJECT->CurrentMCSType == RapidEnums::MCSTYPE::MCSNEGXZ)
		{
			for(int i = 0; i < cellsWithInSpan; i ++)
			{
				double pnt[3] = {FocusXYZWithInSpan[3 * i], FocusXYZWithInSpan[3 * i + 1], FocusXYZWithInSpan[3 * i + 2]};
				FocusXYZWithInSpan[3 * i] = -pnt[0];
				FocusXYZWithInSpan[3 * i + 1] = pnt[2];
				FocusXYZWithInSpan[3 * i + 2] = pnt[1];
			}
		}

		int ind1 = 0, ind2 = 1, ind3 = 2;
		if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
		{
		   ind1 = 1, ind2 = 2, ind3 = 0;
		}
		else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
		{
		   ind2 = 2, ind3 = 1;
		}
		
		if(currentshape->PointsListOriginal->Pointscount() == 0 && CurrentFocusType == RapidEnums::RAPIDFOCUSMODE::CONTOOURSCAN_MODE && this->ApplyPixelCorrection())
		{
			currentshape->ApplyPixelCorrection(true);
		}

		switch(CurrentFocusType)
		{
			case RapidEnums::RAPIDFOCUSMODE::CONTOURFRAMEGRAB:
			case RapidEnums::RAPIDFOCUSMODE::CONTOOURSCAN_MODE:
			{
				for(int i = 0; i < cellsWithInSpan; i ++)
				{
					Cbaseaction->AllPointsList.Addpoint(new SinglePoint(FocusXYZWithInSpan[3 * i + ind1], FocusXYZWithInSpan[3 * i + ind2], FocusXYZWithInSpan[3 * i + ind3]));	
				}
				break;
			}
			case RapidEnums::RAPIDFOCUSMODE::FOCUSSCAN_MODE:
			{
				for(int i = 1; i <= GrainSurfaceList.size(); i++)
				{
					if(GrainSurfaceList[i]->cells < 3) continue;
					BESTFITOBJECT->Plane_BestFit(GrainSurfaceList[i]->PointArray, GrainSurfaceList[i]->cells, GrainSurfaceList[i]->planeParam, MAINDllOBJECT->SigmaModeFlag());
					GrainSurfaceList[i]->planeParam[3] = -1 * GrainSurfaceList[i]->planeParam[3];
					for(int j = 0; j < GrainSurfaceList[i]->cells; j++)
					{
						GrainSurfaceList[i]->avgDev += abs(RMATH3DOBJECT->Distance_Point_Plane(&GrainSurfaceList[i]->PointArray[3 * j], GrainSurfaceList[i]->planeParam));
					}
					GrainSurfaceList[i]->avgDev /= GrainSurfaceList[i]->cells;
				}
				for(int i = 0; i < cellsWithInSpan; i ++)
				{
					if(GrainSurfaceList[GrainLookUpTable[i]]->cells < 3 || (abs(RMATH3DOBJECT->Distance_Point_Plane(&FocusXYZWithInSpan[3 * i], GrainSurfaceList[GrainLookUpTable[i]]->planeParam)) <= (NoiseFilterFactor() * GrainSurfaceList[GrainLookUpTable[i]]->avgDev)))
					{
						Cbaseaction->AllPointsList.Addpoint(new SinglePoint(FocusXYZWithInSpan[3 * i + ind1], FocusXYZWithInSpan[3 * i + ind2], FocusXYZWithInSpan[3 * i + ind3]));	
					}
				}
				for(int i = 1; i < GrainSurfaceList.size(); i++)
					delete GrainSurfaceList[i];
				GrainSurfaceList.clear();
				GrainLookUpTable.clear();
				break;
			}
		}

		Cbaseaction->noofpts = Cbaseaction->AllPointsList.getList().size();

		if(PPCALCOBJECT->IsPartProgramRunning())
			currentshape->ManagePoint(Cbaseaction, ShapeWithList::ADD_POINT);
		else
			AddPointAction::pointAction(currentshape, Cbaseaction);

		if(currentshape->ApplyMMCCorrection() && (!currentshape->ScanDirDetected()))
		{
			switch (currentshape->ShapeType)
			{
				case RapidEnums::SHAPETYPE::LINE:
				case RapidEnums::SHAPETYPE::LINE3D:
				case RapidEnums::SHAPETYPE::PLANE:
				case RapidEnums::SHAPETYPE::CLOUDPOINTS:
				{
					double dir[3] = {0};
					dir[selectedAxis] = 1;
					if(currentshape->ShapeType == RapidEnums::SHAPETYPE::PLANE)
					{
						for(int i = 0; i < 3; i++)
						((Plane*)currentshape)->ScanDir[i] = dir[i];					
					}
					else if(currentshape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
					{
						/* for(int i = 0; i < 3; i++)
						((CloudPoints*)currentshape)->ScanDir[i] = dir[i];*/
					}
					else
					{
						for(int i = 0; i < 3; i++)
						((Line*)currentshape)->ScanDir[i] = dir[i];
					}
					currentshape->ScanDirDetected(true);
					break;
				}
				case RapidEnums::SHAPETYPE::CIRCLE:
				case RapidEnums::SHAPETYPE::ARC:
				case RapidEnums::SHAPETYPE::CIRCLE3D:
				case RapidEnums::SHAPETYPE::ARC3D:
				{	
					double cenz = ((Circle*)currentshape)->getCenter()->getZ();
					if(cenz < ((Circle*)currentshape)->PointsList->getList().begin()->second->X)
						currentshape->OuterCorrection(true);
					else
						currentshape->OuterCorrection(false);
					currentshape->ScanDirDetected(true);
					break;
				}		
				case RapidEnums::SHAPETYPE::CYLINDER:
				{	
					double cenz = (((Cylinder*)currentshape)->getCenter1()->getZ() + ((Cylinder*)currentshape)->getCenter2()->getZ()) / 2;
					if(cenz < ((Cylinder*)currentshape)->PointsList->getList().begin()->second->X)
						currentshape->OuterCorrection(true);
					else
						currentshape->OuterCorrection(false);
					currentshape->ScanDirDetected(true);
					break;
				}		
				case RapidEnums::SHAPETYPE::CONE:
				{	
					double cenz = (((Cone*)currentshape)->getCenter1()->getZ() + ((Cone*)currentshape)->getCenter2()->getZ()) / 2;
					if(cenz < ((Cone*)currentshape)->PointsList->getList().begin()->second->X)
						currentshape->OuterCorrection(true);
					else
						currentshape->OuterCorrection(false);
					currentshape->ScanDirDetected(true);
					break;
				}	
				case RapidEnums::SHAPETYPE::SPHERE:
				{	
					double cenz = ((Sphere*)currentshape)->getCenter()->getZ();
					if(cenz < ((Sphere*)currentshape)->PointsList->getList().begin()->second->X)
						currentshape->OuterCorrection(true);
					else
						currentshape->OuterCorrection(false);
					currentshape->ScanDirDetected(true);
					break;
				}
			}
		}
		if(FocusXYZWithInSpan != NULL)
		{
			free(FocusXYZWithInSpan);
			FocusXYZWithInSpan = NULL;
		}
		free(Points);
		free(ZValue);
	}
	catch(...)
	{
		for(int i = 1; i < GrainSurfaceList.size(); i++)
			 delete GrainSurfaceList[i];
		GrainSurfaceList.clear();
		GrainLookUpTable.clear();
		MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0013", __FILE__, __FUNCSIG__); 
	}
}

void FocusFunctions::DoAverageOfNearByPoints(ShapeWithList *cShape)
{ 
	try
	{
		double checkdist = MAINDllOBJECT->getWindow(0).getUppX();
		bool alongx = true;
		if(cShape->PointsListOriginal->getList().size() > 2)
		{
		   SinglePoint* Sp1 = cShape->PointsListOriginal->getList().begin()->second; 
		   PC_ITER Spt = cShape->PointsListOriginal->getList().end();
		   Spt--;
		   SinglePoint* Sp2 = (*Spt).second;
		   if(abs(Sp1->Y - Sp2->Y) > abs(Sp1->X - Sp2->X))
		   {
			   alongx = false;
		   }
		}
		cShape->PointsList->deleteAll();
		if(alongx)
		{
			if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
			{
				for(PC_ITER Spt = cShape->PointsListOriginal->getList().begin(); Spt != cShape->PointsListOriginal->getList().end(); Spt++)
				{
					SinglePoint* Sp = (*Spt).second;
					double avgz = Sp->Z;
					int cnt = 1;
					for(PC_ITER Spt2 = cShape->PointsListOriginal->getList().begin(); Spt2 != cShape->PointsListOriginal->getList().end(); Spt2++)
					{
						SinglePoint* Sp2 = (*Spt2).second;
						if(Sp->PtID == Sp2->PtID) continue;
						if(abs(Sp->X - Sp2->X) <= (PixelCorrectionFactor() * checkdist))
						{
							avgz += Sp2->Z;
							cnt++;
						}
					}
					SinglePoint *tmpPnt = new SinglePoint(Sp->X, Sp->Y, avgz/cnt, Sp->R, Sp->Pdir, Sp->PLR);
					cShape->PointsList->Addpoint(tmpPnt, Sp->PtID);
				}
			}
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
			{
				for(PC_ITER Spt = cShape->PointsListOriginal->getList().begin(); Spt != cShape->PointsListOriginal->getList().end(); Spt++)
				{
					SinglePoint* Sp = (*Spt).second;
					double avgz = Sp->Y;
					int cnt = 1;
					for(PC_ITER Spt2 = cShape->PointsListOriginal->getList().begin(); Spt2 != cShape->PointsListOriginal->getList().end(); Spt2++)
					{
						SinglePoint* Sp2 = (*Spt2).second;
						if(Sp->PtID == Sp2->PtID) continue;
						if(abs(Sp->X - Sp2->X) <= (PixelCorrectionFactor() * checkdist))
						{
							avgz += Sp2->Y;
							cnt++;
						}
					}
					SinglePoint *tmpPnt = new SinglePoint(Sp->X, avgz/cnt, Sp->Z, Sp->R, Sp->Pdir, Sp->PLR);
					cShape->PointsList->Addpoint(tmpPnt, Sp->PtID);
				}
			}
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
			{
				for(PC_ITER Spt = cShape->PointsList->getList().begin(); Spt != cShape->PointsList->getList().end(); Spt++)
				{
					SinglePoint* Sp = (*Spt).second;
					double avgz = Sp->Y;
					int cnt = 1;
					for(PC_ITER Spt2 = cShape->PointsList->getList().begin(); Spt2 != cShape->PointsList->getList().end(); Spt2++)
					{
						SinglePoint* Sp2 = (*Spt2).second;
						if(Sp->PtID == Sp2->PtID) continue;
						if(abs(Sp->X - Sp2->X) <= (PixelCorrectionFactor() * checkdist))
						{
							avgz += Sp2->Y;
							cnt++;
						}
					}
					SinglePoint *tmpPnt = new SinglePoint(Sp->X, avgz/cnt, Sp->Z, Sp->R, Sp->Pdir, Sp->PLR);
					cShape->PointsList->Addpoint(tmpPnt, Sp->PtID);			
				}
			}
		}
		else
		{
			if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
			{
				for(PC_ITER Spt = cShape->PointsList->getList().begin(); Spt != cShape->PointsList->getList().end(); Spt++)
				{
					SinglePoint* Sp = (*Spt).second;
					double avgz = Sp->Z;
					int cnt = 1;
					for(PC_ITER Spt2 = cShape->PointsList->getList().begin(); Spt2 != cShape->PointsList->getList().end(); Spt2++)
					{
						SinglePoint* Sp2 = (*Spt2).second;
						if(Sp->PtID == Sp2->PtID) continue;
						if(abs(Sp->Y - Sp2->Y) <= (PixelCorrectionFactor() * checkdist))
						{
							avgz += Sp2->Z;
							cnt++;
						}
					}
					SinglePoint *tmpPnt = new SinglePoint(Sp->X, Sp->Y, avgz/cnt, Sp->R, Sp->Pdir, Sp->PLR);
					cShape->PointsList->Addpoint(tmpPnt, Sp->PtID);
				}
			}
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
			{
				for(PC_ITER Spt = cShape->PointsListOriginal->getList().begin(); Spt != cShape->PointsListOriginal->getList().end(); Spt++)
				{
					SinglePoint* Sp = (*Spt).second;
					double avgz = Sp->Y;
					int cnt = 1;
					for(PC_ITER Spt2 = cShape->PointsListOriginal->getList().begin(); Spt2 != cShape->PointsListOriginal->getList().end(); Spt2++)
					{
						SinglePoint* Sp2 = (*Spt2).second;
						if(Sp->PtID == Sp2->PtID) continue;
						if(abs(Sp->Y - Sp2->Y) <= (PixelCorrectionFactor() * checkdist))
						{
							avgz += Sp2->Y;
							cnt++;
						}
					}
					SinglePoint *tmpPnt = new SinglePoint(Sp->X, avgz/cnt, Sp->Z, Sp->R, Sp->Pdir, Sp->PLR);
					cShape->PointsList->Addpoint(tmpPnt, Sp->PtID);
				}
			}
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
			{
				for(PC_ITER Spt = cShape->PointsListOriginal->getList().begin(); Spt != cShape->PointsListOriginal->getList().end(); Spt++)
				{
					SinglePoint* Sp = (*Spt).second;
					double avgz = Sp->Y;
					int cnt = 1;
					for(PC_ITER Spt2 = cShape->PointsListOriginal->getList().begin(); Spt2 != cShape->PointsListOriginal->getList().end(); Spt2++)
					{
						SinglePoint* Sp2 = (*Spt2).second;
						if(Sp->PtID == Sp2->PtID) continue;
						if(abs(Sp->Y - Sp2->Y) <= (PixelCorrectionFactor() * checkdist))
						{
							avgz += Sp2->Y;
							cnt++;
						}
					}
					SinglePoint *tmpPnt = new SinglePoint(Sp->X, avgz/cnt, Sp->Z, Sp->R, Sp->Pdir, Sp->PLR);
					cShape->PointsList->Addpoint(tmpPnt, Sp->PtID);
				}
			}		
		}
		if (cShape->PointsList->Pointscount() > 0)
		{
			if(cShape->ShapeAs3DShape())
			{
				if(cShape->pts != NULL){free(cShape->pts); cShape->pts = NULL;}
				cShape->pts = (double*)malloc(sizeof(double) * cShape->PointsListOriginal->Pointscount() * 3);
				if(cShape->filterpts != NULL){free(cShape->filterpts); cShape->filterpts = NULL;}
				cShape->filterpts = (double*)malloc(sizeof(double) * cShape->PointsListOriginal->Pointscount() * 3);
				cShape->filterptsCnt = 0;
				int n = 0, j = 0;
				for(PC_ITER Spt = cShape->PointsList->getList().begin(); Spt != cShape->PointsList->getList().end(); Spt++)
				{
					SinglePoint* Pt = (*Spt).second;
					cShape->pts[n++] = Pt->X;
					cShape->pts[n++] = Pt->Y;
					cShape->pts[n++] = Pt->Z;
					cShape->filterpts[j++] = Pt->X;
					cShape->filterpts[j++] = Pt->Y;
					cShape->filterpts[j++] = Pt->Z;
					cShape->filterptsCnt++;
				}
			}
			else
			{
				if(cShape->pts != NULL){free(cShape->pts); cShape->pts = NULL;}
				cShape->pts = (double*)malloc(sizeof(double) * cShape->PointsListOriginal->Pointscount() * 2);
				if(cShape->filterpts != NULL){free(cShape->filterpts); cShape->filterpts = NULL;}
				cShape->filterpts = (double*)malloc(sizeof(double) * cShape->PointsListOriginal->Pointscount() * 2);
				cShape->filterptsCnt = 0;
				int n = 0, j = 0;
				for(PC_ITER Spt = cShape->PointsList->getList().begin(); Spt != cShape->PointsList->getList().end(); Spt++)
				{
					SinglePoint* Pt = (*Spt).second;
					cShape->pts[n++] = Pt->X;
					cShape->pts[n++] = Pt->Y;
					cShape->filterpts[j++] = Pt->X;
					cShape->filterpts[j++] = Pt->Y;
					cShape->filterptsCnt++;
				}
			}
		}
		cShape->UpdateBestFit();
		cShape->CalculateEnclosedRectangle();
		cShape->notifyAll(ITEMSTATUS::DATACHANGED, cShape);	
	}
	catch(...)
	{
	
	}
}

//Check for active width spam..
bool FocusFunctions::CheckActiveWidthSpam(double *Zvalue, bool *positive)
{
	try
	{
		int CellCount = focusRectangleColumn * focusRectangleRow;
		if(CellCount > 4) return false;
		double ZValues[4];
		double* Points = (double*)calloc(NoofIterationCount * 2, sizeof(double));
		int SampleCount = NoofIterationCount;

		for(int j = 0; j < CellCount; j++)
		{
			int cnt = 0;
			for(int i = 0; i < SampleCount; i++)
			{
				Points[cnt] = FocusValueColl[CellCount * i + j];
				cnt++;
			}
			double temp = Points[0];
			ZValues[j] = ZvalueColl[0];
			for(int i = 0; i < SampleCount; i++)
			{
				if(temp < Points[i])
				{
					temp = Points[i];
					ZValues[j] = ZvalueColl[i];
				}
			}
		}
		free(Points);
		double focusSpan = FocusActiveWidth();
		double FirstZValue = RMATH2DOBJECT->minimum(&ZvalueColl[0], SampleCount);
		double LastZValue = RMATH2DOBJECT->maximum(&ZvalueColl[0], SampleCount);
		double focusSpanDown[4] = {0}, focusSpanUp[4] = {0};
		for(int i = 0; i < CellCount; i++)
		{
			double focusSpan1 = abs(ZValues[i] - FirstZValue);
			double focusSpan2 = abs(ZValues[i] - LastZValue);
			if((focusSpan - focusSpan1) > 0.02) focusSpanDown[i] = (focusSpan - focusSpan1) + 0.4;
			if((focusSpan - focusSpan2) > 0.02) focusSpanUp[i] = (focusSpan - focusSpan2) + 0.4;
		}
		double focus_SpanDown = RMATH2DOBJECT->maximum(&focusSpanDown[0], 4);
		double focus_SpanUp = RMATH2DOBJECT->maximum(&focusSpanUp[0], 4);
		if(focus_SpanDown == 0 && focus_SpanUp == 0) return false;
		else
		{
			if(focus_SpanDown > focus_SpanUp)
			{
				*Zvalue = focus_SpanDown;
				*positive = false;
			}
			else
			{
				*Zvalue = focus_SpanUp;
				*positive = true;
			}
			return true;
		}
		return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0008", __FILE__, __FUNCSIG__);  return false;}
}

void FocusFunctions::FilterFocus(double* xyzValues, int count, double focusSpanPos, double focusSpanNeg, double *PlaneParam, double* ZDevAvg)
{
	int zcount = 0;
	BESTFITOBJECT->Plane_BestFit(xyzValues, count, &PlaneParam[0], MAINDllOBJECT->SigmaModeFlag());
	PlaneParam[3] = -1 * PlaneParam[3];
	for(int i = 0; i < count; i++)
	{
		if((xyzValues[3 * i + 2] <= focusSpanPos) && (xyzValues[3 * i + 2] >= focusSpanNeg))
		{
			 double d = RMATH3DOBJECT->Distance_Point_Plane(&xyzValues[3 * i], &PlaneParam[0]);
			*ZDevAvg += abs(d); 
			 zcount++;
		}		
	}
	*ZDevAvg /= zcount;
}

void FocusFunctions::UpdateTwoBoxDepthMeasurement(double* Zvalues)
{
	try
	{
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y, upp = MAINDllOBJECT->getWindow(0).getUppX() / HELPEROBJECT->CamSizeRatio;
		int cells = focusRectangleRow * focusRectangleColumn;
		double FocusXY[8];
		for(int i = 0; i < cells; i++)
		{
			FocusXY[2 * i] = cx + (Rectptr[4 * i] + (Rectptr[4 * i + 2]/2)) * upp;
			FocusXY[2 * i + 1] = cy - (Rectptr[4 * i + 1] + (Rectptr[4 * i + 2]/2)) * upp;
		}
		ShapeWithList* currentshape;
		DimDepth* Cdim;
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			currentshape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
			Cdim = (DimDepth*)(*currentshape->getMchild().begin());
			Cbaseaction = PPCALCOBJECT->getFrameGrab();
		}
		else
		{
			currentshape = new DummyShape();
			AddShapeAction::addShape(currentshape);
			Cdim = new DimDepth();
			Cdim->TwoBoxDepthMeasure(true);
			Cdim->SetShapes(currentshape, NULL);
			Cdim->addParent(currentshape);
			currentshape->addMChild(Cdim);
			AddDimAction::addDim(Cdim);
			Cbaseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_2BOX);
			if(MAINDllOBJECT->CameraConnected())
			{
				Cbaseaction->camProperty = MAINDllOBJECT->getCamSettings();
				Cbaseaction->lightProperty = MAINDllOBJECT->getLightProperty();
				Cbaseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
			}
			Cbaseaction->PointDRO = MAINDllOBJECT->getCurrentDRO();
			Cbaseaction->ProfileON = MAINDllOBJECT->getProfile();
			Cbaseaction->SurfaceON = MAINDllOBJECT->getSurface();
			Cbaseaction->myPosition[0].set(Rectptr[0], Rectptr[1], Rectptr[2], Rectptr[3]);
			Cbaseaction->myPosition[1].set(Rectptr[4], Rectptr[5], Rectptr[6], Rectptr[7]);
			Cbaseaction->myPosition[2].set(fspan, fspeed, selectedAxis);
		}
		for(int i = 0; i < cells; i++)
		{
			if(Zvalues[i] < 160)
			{
				if(ApplyFocusCorrection())
				{
					double correction = 0;
					if(BESTFITOBJECT->getZError(20, 15, 40, FocusErrCorrectionValue,  FocusCalibError, Rectptr[4 * i] + 20, Rectptr[4 * i + 1] + 20, &correction))
						Zvalues[i] -= correction;
				}
				Cbaseaction->AllPointsList.Addpoint(new SinglePoint(FocusXY[2 * i], FocusXY[2 * i + 1], Zvalues[i]));
			}
		}
		if(PPCALCOBJECT->IsPartProgramRunning())
			currentshape->ManagePoint(Cbaseaction, ShapeWithList::ADD_POINT);
		else
		{
			AddPointAction::pointAction(currentshape, Cbaseaction);
			MAINDllOBJECT->selectMeasurement(Cdim->getId(), false);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0008", __FILE__, __FUNCSIG__); }
}

void FocusFunctions::UpdateFourBoxDepthMeasurement(double* Zvalues)
{
	try
	{
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y, upp = MAINDllOBJECT->getWindow(0).getUppX();
		int cells = focusRectangleRow * focusRectangleColumn;
		double FocusXY[16];
		for(int i = 0; i < cells; i++)
		{
			FocusXY[2 * i] = cx + (Rectptr[4 * i] + (Rectptr[4 * i + 2]/2)) * upp;
			FocusXY[2 * i + 1] = cy - (Rectptr[4 * i + 1] + (Rectptr[4 * i + 2]/2)) * upp;
		}
		ShapeWithList* currentshape = NULL;
		DimDepth *Cdim1 = NULL, *Cdim2 = NULL;
		if(PPCALCOBJECT->IsPartProgramRunning())
		{
			currentshape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
			Cdim1 = (DimDepth*)(*currentshape->getMchild().begin());
			Cbaseaction = PPCALCOBJECT->getFrameGrab();
		}
		else
		{
			currentshape = new DummyShape();
			AddShapeAction::addShape(currentshape);
			Cdim1 = new DimDepth();
			Cdim1->TwoBoxDepthMeasure(true);
			Cdim1->CurrentDMeasure(0);
			Cdim1->SetShapes(currentshape, NULL);
			Cdim1->addParent(currentshape);
			currentshape->addMChild(Cdim1);
			AddDimAction::addDim(Cdim1);
			Cdim2 = new DimDepth();
			Cdim2->TwoBoxDepthMeasure(true);
			Cdim2->CurrentDMeasure(1);
			Cdim2->SetShapes(currentshape, NULL);
			Cdim2->addParent(currentshape);
			currentshape->addMChild(Cdim2);
			AddDimAction::addDim(Cdim2);

			Cbaseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_4BOX);
			if(MAINDllOBJECT->CameraConnected())
			{
				Cbaseaction->camProperty = MAINDllOBJECT->getCamSettings();
				Cbaseaction->lightProperty = MAINDllOBJECT->getLightProperty();
				Cbaseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
			}
			Cbaseaction->PointDRO = MAINDllOBJECT->getCurrentDRO();
			Cbaseaction->ProfileON = MAINDllOBJECT->getProfile();
			Cbaseaction->SurfaceON = MAINDllOBJECT->getSurface();
			Cbaseaction->myPosition[0].set(Rectptr[0], Rectptr[1], Rectptr[2], Rectptr[3]);
			Cbaseaction->myPosition[1].set(Rectptr[4], Rectptr[5], Rectptr[6], Rectptr[7]);
			Cbaseaction->myPosition[2].set(Rectptr[8], Rectptr[9], Rectptr[10], Rectptr[11]);
			Cbaseaction->myPosition[3].set(Rectptr[12], Rectptr[13], Rectptr[14], Rectptr[15]);
			Cbaseaction->myPosition[4].set(fspan , fspeed, selectedAxis);
		}
		for(int i = 0; i < cells; i++)
		{
			if(Zvalues[i] < 160)
			{
				if(ApplyFocusCorrection())
				{
					double correction = 0;
					if(BESTFITOBJECT->getZError(20, 15, 40, FocusErrCorrectionValue, FocusCalibError, Rectptr[4 * i] + 20, Rectptr[4 * i + 1] + 20, &correction))
						Zvalues[i] -= correction;
				}
				Cbaseaction->AllPointsList.Addpoint(new SinglePoint(FocusXY[2 * i], FocusXY[2 * i + 1], Zvalues[i]));
			}
		}
		if(PPCALCOBJECT->IsPartProgramRunning())
			currentshape->ManagePoint(Cbaseaction, ShapeWithList::ADD_POINT);
		else
		{
			AddPointAction::pointAction(currentshape, Cbaseaction);
			MAINDllOBJECT->selectMeasurement(Cdim1->getId(), false);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0008", __FILE__, __FUNCSIG__); }
}

double FocusFunctions::sort(double *point, int size)
{
	try
	{
		for(int j=0;j<size;j++)
		{
			for(int k=0;k< (size-1);k++)
			{
				if(point[k] > point[k+1])
				{
					double temp = point[k + 1];
					point[k + 1] = point[k];
					point[k] = temp;
				}
			}
		} 
		if(size % 2 == 0)
			return (point[size/2] + point[size/2 + 1])/2;
		else
			return (point[size/2 + 1]);
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0008", __FILE__, __FUNCSIG__);
	}
}

//void FocusFunctions::setFiveFocus(double cellcount)
//{
//	if(!isfivefocus)
//		return;
//	else
//	{
//	FiveFocus = (double*)malloc(sizeof(double) * cellcount*5);
//	for(int i = 0 ; i < cellcount*5 ; i++)
//	{
//		FiveFocus[i] = 0;
//	}
//	}
//}


void FocusFunctions::UpdatePointsForFocusCalibration(double* Zvalues)
{
	try
	{
		static int ccc = 1;
		std::wofstream cfile;
		int cells = focusRectangleRow * focusRectangleColumn;
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y, upp = MAINDllOBJECT->getWindow(0).getUppX();
		double *FocusXY = (double*)calloc( focusRectangleRow * focusRectangleColumn * 2, sizeof(double));
		for(int i = 0; i < focusRectangleRow; i++)
		{
			for(int j = 0; j < focusRectangleColumn; j++)
			{
				FocusXY[2 * ((i * focusRectangleColumn)  + j)] = cx + (Rectptr[4 * ((i * focusRectangleColumn)  + j)] + (Rectptr[4 * ((i * focusRectangleColumn)  + j) + 2]/2)) * upp;
				FocusXY[2 * ((i * focusRectangleColumn)  + j) + 1] = cy - (Rectptr[4 * ((i * focusRectangleColumn)  + j) + 1] + (Rectptr[4 * ((i * focusRectangleColumn)  + j) + 3]/2)) * upp;
				FiveFocus[((framescancount)* cells) + (i * focusRectangleColumn)  + j] =  Zvalues[(i * focusRectangleColumn)  + j];	
			}
		}
		if(framescancount == (NoOfRepeatAtPlace - 1))
		{
			char temp[10];
			itoa(ccc, &temp[0], 10);
			std::string CCount = (const char*)temp;
			cfile.open("E:\\FocusValue" + CCount + ".csv");
			ccc++;
			double medianPnts = 0;
			double *Fivepoint = new double[NoOfRepeatAtPlace];
			for(int i = 0; i < focusRectangleRow; i++)
			{
				for(int j = 0; j < focusRectangleColumn; j++)
				{
					for(int k = 0; k < NoOfRepeatAtPlace; k++)
					{
						Fivepoint[k] = FiveFocus[k * cells + (i * focusRectangleColumn)  + j];
					}
					medianPnts = sort(Fivepoint, NoOfRepeatAtPlace);
					if(medianPnts >= 1000)
						FocusCalibrationSuccess = false;
					/*if(i > 0 && j > 0 && i < focusRectangleRow - 1 && j < focusRectangleColumn - 1)
					{
						double ninePntsSum = 0;
						for(int tempi = i - 1; tempi <= i + 1; tempi++)
						{
							for(int tempj = j - 1; tempj <= j + 1; tempj++)
								ninePntsSum += medianPntsArray[(tempi * focusRectangleColumn)  + tempj];
						}
						medianPntsArray[(i * focusRectangleColumn)  + j] = ninePntsSum / 9;
					}*/
					if((j >= lowRowCutoff && j <= upRowCutoff) && (i >= lowColCutoff && i <= upColCutoff ))
						FocusCalibrationPtCollForPlane.Addpoint(new SinglePoint(FocusXY[2 * ((i * focusRectangleColumn)  + j)] , FocusXY[2 * ((i * focusRectangleColumn)  + j) + 1], medianPnts));
					FocusCalibrationPtColl.Addpoint(new SinglePoint(FocusXY[2 * ((i * focusRectangleColumn)  + j)] , FocusXY[2 * ((i * focusRectangleColumn)  + j) + 1], medianPnts));
					cfile << FocusXY[2 * ((i * focusRectangleColumn)  + j)] << "," << FocusXY[2 * ((i * focusRectangleColumn)  + j) + 1] << ","  << medianPnts <<  endl;
				}
			}			
			delete [] Fivepoint;
			cfile.close();
		}
		framescancount ++;	
		if(framescancount > (NoOfRepeatAtPlace - 1))
		{
			framescancount = 0;
			for(int i = 0 ; i < cells; i++) { FiveFocus[i]=0; }
		}		
		free(FocusXY);
	}
	catch(...)
	{ 
		MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0008", __FILE__, __FUNCSIG__);
	}
}

void FocusFunctions::CalculateFocusCalibrationValues()
{
	try
	{
		FocusCorrectionValues =(double *)malloc(sizeof(double)*focusRectangleColumn*focusRectangleRow);
		Plane PShape(false);
		int cells = 0, cellno = 0;
		double PlaneParam[4]={0,0,1,0};
		if(FocusAtOnePositionOnly)
		{
			double avg=0;			
			cells = FocusCalibrationPtColl.Pointscount(), cellno = 0;
			for(int i = 0 ; i < FocusCalibrationPtCollForPlane.Pointscount();i++)
				avg += FocusCalibrationPtCollForPlane.getList()[i]->Z;
			avg /= FocusCalibrationPtCollForPlane.Pointscount();
			PlaneParam[3]=avg;
		}
		else
		{
			cells = FocusCalibrationPtColl.Pointscount() / 4, cellno = 0;
			PShape.AddPoints(&FocusCalibrationPtCollForPlane);
			PShape.getParameters(&PlaneParam[0]);
		}
		
		std::wofstream cfile;
		cfile.open("D:\\FocusValue.csv");
		cfile << "Plane" << PlaneParam[0] << PlaneParam[1] << PlaneParam[2] << PlaneParam[3] << endl;
		cfile <<"Focus Value" << endl;
		for(int i = 0; i < cells; i++)
		{
			SinglePoint* Spt = FocusCalibrationPtColl.getList()[i];
			double Dist = 0, point[3] = {Spt->X, Spt->Y, Spt->Z};
			if(Spt->Z < 160)
			{
				Dist = RMATH3DOBJECT->Distance_Point_PlaneAlongSeletedAxis(point, PlaneParam, selectedAxis);
				cfile << Spt->X << "," << Spt->Y<< ","  << Spt->Z << "," << Dist << endl;
			}
			FocusCorrectionValues[i] = Dist;
		}
		cfile.close();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0008", __FILE__, __FUNCSIG__); }
}

//void FocusFunctions::CalculateFocusCalibrationDeviationValues(double *x, double *y)
//{
//	try
//	{
//		Plane PShape1(false);
//		Plane PShape2(false);
//		Plane PShape3(false);
//		double PlaneParam[3][4]={0};
//		PShape1.AddPoints(&FocusCalibrationPtCollDeviation[0]);
//		PShape1.getParameters(&PlaneParam[0][0]);
//		PShape2.AddPoints(&FocusCalibrationPtCollDeviation[1]);
//		PShape2.getParameters(&PlaneParam[1][0]);
//		PShape3.AddPoints(&FocusCalibrationPtCollDeviation[3]);
//		PShape3.getParameters(&PlaneParam[2][0]);
//		 *x = (PlaneParam[1][3]/PlaneParam[1][2]) - (PlaneParam[0][3]/PlaneParam[0][2]);
//		 *y = (PlaneParam[2][3]/PlaneParam[2][2]) - (PlaneParam[0][3]/PlaneParam[0][2]);
//	}
//	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0008", __FILE__, __FUNCSIG__); }
//}

//Add new depth measure..

void FocusFunctions::AddNewDepthMeasure()
{
	try
	{
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
		CurrentDepthMeasure = new DimDepth(_T("Depth"));
		AddDimAction::addDim(CurrentDepthMeasure);
		DepthMeasureList.push_back(CurrentDepthMeasure->getId());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0014", __FILE__, __FUNCSIG__); }
}

//Add depth measure actions
void FocusFunctions::AddTheDepthAction(int i)
{
	try
	{
		bool addAction = true;
		if(CurrentDepthMeasure == NULL || DepthZcol.size() == 0)
		{
			return; //no depth measure selected..
		}
		if(i == 1)
		{
			if(CurrentDepthMeasure->ParentShape1 == NULL)
			{
				Shape* CShape = new DepthShape();
				AddShapeAction::addShape(CShape);
				CurrentDepthMeasure->addParent(CShape);
				DepthDatumList.push_back(CShape->getId());
				CShape->addMChild(CurrentDepthMeasure);
				CurrentDepthMeasure->ParentShape1 = CShape;
				Cbaseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH);
				((DimDepth*)CurrentDepthMeasure)->dummyShapesType[0] = (int)ShapeForDepth;
			}
			else
			{
				MAINDllOBJECT->selectShape(CurrentDepthMeasure->ParentShape1->getId(),false);
				AddPointAction* Caction = (AddPointAction*)(*((ShapeWithList*)CurrentDepthMeasure->ParentShape1)->PointAtionList.begin());
				((ShapeWithList*)CurrentDepthMeasure->ParentShape1)->ResetCurrentPointsList();
				Caction->ActionStatus(true);
				Cbaseaction = Caction->getFramegrab();
				Cbaseaction->FGtype = RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH;
				Cbaseaction->AllPointsList.deleteAll();
				Cbaseaction->PointActionIdList.clear();
				addAction = false;
			}
		}
		else
		{
			if(CurrentDepthMeasure->ParentShape2 == NULL)
			{
				Shape* CShape = new DepthShape();
				AddShapeAction::addShape(CShape);
				CurrentDepthMeasure->addParent(CShape);
				DepthDatumList.push_back(CShape->getId());
				CShape->addMChild(CurrentDepthMeasure);
				CurrentDepthMeasure->ParentShape2 = CShape;
				Cbaseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH);
				((DimDepth*)CurrentDepthMeasure)->dummyShapesType[1] = (int)ShapeForDepth;
			}
			else
			{
				MAINDllOBJECT->selectShape(CurrentDepthMeasure->ParentShape2->getId(),false);
				AddPointAction* Caction = (AddPointAction*)(*((ShapeWithList*)CurrentDepthMeasure->ParentShape2)->PointAtionList.begin());
				((ShapeWithList*)CurrentDepthMeasure->ParentShape2)->ResetCurrentPointsList();
				Caction->ActionStatus(true);
				Cbaseaction = Caction->getFramegrab();
				Cbaseaction->FGtype = RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH;
				Cbaseaction->AllPointsList.deleteAll();
				Cbaseaction->PointActionIdList.clear();
				addAction = false;
			}
		}
		if(MAINDllOBJECT->CameraConnected())
		{
			Cbaseaction->camProperty = MAINDllOBJECT->getCamSettings();
			Cbaseaction->lightProperty = MAINDllOBJECT->getLightProperty();
			Cbaseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
		}

		double pgap =  focusRectangleGap * MAINDllOBJECT->getWindow(0).getUppX();
		Cbaseaction->PointDRO = FocusStartPosition;
		Cbaseaction->ProfileON = MAINDllOBJECT->getProfile();
		Cbaseaction->SurfaceON = MAINDllOBJECT->getSurface();
		Cbaseaction->myPosition[0].set(focusRectangleWidth, focusRectangleHeight); // * HELPEROBJECT->CamSizeRatio
		Cbaseaction->myPosition[1].set(focusRectangleRow , focusRectangleColumn, pgap);
		Cbaseaction->myPosition[2].set(fspan , fspeed, selectedAxis);
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y, upp = MAINDllOBJECT->getWindow(0).getUppX() / HELPEROBJECT->CamSizeRatio;
		int i = 0;
		double FocusStartZvalue = 0;
		if(selectedAxis == 2)
			FocusStartZvalue = FocusStartPosition.getZ();
		else if(selectedAxis == 1)
			FocusStartZvalue = FocusStartPosition.getY();
		else if(selectedAxis == 0)
			FocusStartZvalue = FocusStartPosition.getX();
		double FocusSpanPos = FocusStartZvalue + fspan;
		double FocusSpanNeg = FocusStartZvalue - fspan;	
		for(list<double>::iterator It = DepthZcol.begin(); It != DepthZcol.end(); It++)
		{
			double tmpZ = *It;
			if(ApplyFocusCorrection())
			{
				double correction = 0;
				if(BESTFITOBJECT->getZError(20, 15, 40, FocusErrCorrectionValue, FocusCalibError, Rectptr[4 * i] + 20, Rectptr[4 * i + 1] + 20, &correction))
					tmpZ -= correction;		  
			}
			double xyValue[2] = {0};
			xyValue[0] =  cx + (Rectptr[4 * i] + (Rectptr[4 * i + 2]/2)) * upp;;
			xyValue[1] = cy - (Rectptr[4 * i + 1] + (Rectptr[4 * i + 3]/2)) * upp;
			if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
				Cbaseaction->AllPointsList.Addpoint(new SinglePoint(xyValue[0], xyValue[1], tmpZ - MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ()));
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
				Cbaseaction->AllPointsList.Addpoint(new SinglePoint(xyValue[1], tmpZ - MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ(), xyValue[0]));
			else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
				Cbaseaction->AllPointsList.Addpoint(new SinglePoint(xyValue[0], tmpZ - MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ(), xyValue[1]));
			i++;
		}
		ShapeWithList* currentshape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		if(addAction)
			AddPointAction::pointAction(currentshape, Cbaseaction);
		else
			currentshape->ManagePoint(Cbaseaction, ShapeWithList::ADD_POINT);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0015", __FILE__, __FUNCSIG__); }
}

void FocusFunctions::AddTheDepthAction_Program()
{
	try
	{
		Cbaseaction = PPCALCOBJECT->getFrameGrab();
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y, upp = MAINDllOBJECT->getWindow(0).getUppX();
		int i = 0;
		double FocusStartZvalue = 0;
		if(selectedAxis == 2)
			FocusStartZvalue = FocusStartPosition.getZ();
		else if(selectedAxis == 1)
			FocusStartZvalue = FocusStartPosition.getY();
		else if(selectedAxis == 0)
			FocusStartZvalue = FocusStartPosition.getX();
		double FocusSpanPos = FocusStartZvalue + fspan;
		double FocusSpanNeg = FocusStartZvalue - fspan;	
		bool GoFocusPt = false;

		for(list<double>::iterator It = DepthZcol.begin(); It != DepthZcol.end(); It++)
		{
			double tmpZ = *It;
			if((tmpZ <= FocusSpanPos) && (tmpZ >= FocusSpanNeg))
			{
				if(ApplyFocusCorrection())
				{
					double correction = 0;
					if(BESTFITOBJECT->getZError(20, 15, 40, FocusErrCorrectionValue, FocusCalibError, Rectptr[4 * i] + 20, Rectptr[4 * i + 1] + 20, &correction))
						tmpZ -= correction;		  
				}
				double xyValue[2] = {0};
				xyValue[0] =  cx + (Rectptr[4 * i] + (Rectptr[4 * i + 2]/2)) * upp;;
				xyValue[1] = cy - (Rectptr[4 * i + 1] + (Rectptr[4 * i + 3]/2)) * upp;
				if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
					Cbaseaction->AllPointsList.Addpoint(new SinglePoint(xyValue[0], xyValue[1], tmpZ - MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ()));
				else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
					Cbaseaction->AllPointsList.Addpoint(new SinglePoint(xyValue[1], tmpZ - MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ(), xyValue[0]));
				else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
					Cbaseaction->AllPointsList.Addpoint(new SinglePoint(xyValue[0], tmpZ - MAINDllOBJECT->getCurrentUCS().UCSPoint.getZ(), xyValue[1]));
				GoFocusPt = true;
			}
			i++;
		}
		if (GoFocusPt)
		{
			ShapeWithList* currentshape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
			currentshape->ManagePoint(Cbaseaction, ShapeWithList::ADD_POINT);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0015", __FILE__, __FUNCSIG__); }
}

void FocusFunctions::AddDepthAction_Zposition(int i)
{
	try
	{
		bool addAction = true;
		if(CurrentDepthMeasure == NULL)
		{
			return; //no depth measure selected..
		}
		if(i == 1)
		{
			if(CurrentDepthMeasure->ParentShape1 == NULL)
			{
				Shape* CShape = new DepthShape();
				AddShapeAction::addShape(CShape);
				CurrentDepthMeasure->addParent(CShape);
				DepthDatumList.push_back(CShape->getId());
				CShape->addMChild(CurrentDepthMeasure);
				CurrentDepthMeasure->ParentShape1 = CShape;
				Cbaseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_DRO);
			}
			else
			{
				MAINDllOBJECT->selectShape(CurrentDepthMeasure->ParentShape1->getId(),false);
				AddPointAction* Caction = (AddPointAction*)(*((ShapeWithList*)CurrentDepthMeasure->ParentShape1)->PointAtionList.begin());
				((ShapeWithList*)CurrentDepthMeasure->ParentShape1)->ResetCurrentPointsList();
				Caction->ActionStatus(true);
				Cbaseaction = Caction->getFramegrab();
				Cbaseaction->FGtype = RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_DRO;
				Cbaseaction->AllPointsList.deleteAll();
				Cbaseaction->PointActionIdList.clear();
				addAction = false;
			}
		}
		else
		{
			if(CurrentDepthMeasure->ParentShape2 == NULL)
			{
				Shape* CShape = new DepthShape();
				AddShapeAction::addShape(CShape);
				CurrentDepthMeasure->addParent(CShape);
				DepthDatumList.push_back(CShape->getId());
				CShape->addMChild(CurrentDepthMeasure);
				CurrentDepthMeasure->ParentShape2 = CShape;
				Cbaseaction = new FramegrabBase(RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_DRO);
			}
			else
			{
				MAINDllOBJECT->selectShape(CurrentDepthMeasure->ParentShape2->getId(),false);
				AddPointAction* Caction = (AddPointAction*)(*((ShapeWithList*)CurrentDepthMeasure->ParentShape2)->PointAtionList.begin());
				((ShapeWithList*)CurrentDepthMeasure->ParentShape2)->ResetCurrentPointsList();
				Caction->ActionStatus(true);
				Cbaseaction = Caction->getFramegrab();
				Cbaseaction->FGtype = RapidEnums::RAPIDFGACTIONTYPE::FOCUS_DEPTH_DRO;
				Cbaseaction->AllPointsList.deleteAll();
				Cbaseaction->PointActionIdList.clear();
				addAction = false;
			}
		}
		if(addAction)
		{
			if(MAINDllOBJECT->CameraConnected())
			{
				Cbaseaction->camProperty = MAINDllOBJECT->getCamSettings();
				Cbaseaction->lightProperty = MAINDllOBJECT->getLightProperty();
				Cbaseaction->magLevel = MAINDllOBJECT->LastSelectedMagLevel;
			}
			double PPU = MAINDllOBJECT->getWindow(0).getUppX()/2;
			double pgap =  2 * focusRectangleGap * PPU;
			Cbaseaction->PointDRO = MAINDllOBJECT->getCurrentDRO();
			Cbaseaction->ProfileON = MAINDllOBJECT->getProfile();
			Cbaseaction->SurfaceON = MAINDllOBJECT->getSurface();
		}
		if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
			Cbaseaction->AllPointsList.Addpoint(new SinglePoint(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ()));
		else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
			Cbaseaction->AllPointsList.Addpoint(new SinglePoint(MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ(), MAINDllOBJECT->getCurrentDRO().getX()));
		else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
			Cbaseaction->AllPointsList.Addpoint(new SinglePoint(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getZ(), MAINDllOBJECT->getCurrentDRO().getY()));
		ShapeWithList* currentshape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		if(addAction)
			AddPointAction::pointAction(currentshape, Cbaseaction);
		else
			currentshape->ManagePoint(Cbaseaction, ShapeWithList::ADD_POINT);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0015", __FILE__, __FUNCSIG__); }
}

void FocusFunctions::AddDepthAction_Zposition_Program()
{
	try
	{
		Cbaseaction = PPCALCOBJECT->getFrameGrab();
		if(MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XY)
			Cbaseaction->AllPointsList.Addpoint(new SinglePoint(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ()));
		else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
			Cbaseaction->AllPointsList.Addpoint(new SinglePoint(MAINDllOBJECT->getCurrentDRO().getY(), MAINDllOBJECT->getCurrentDRO().getZ(), MAINDllOBJECT->getCurrentDRO().getX()));
		else if (MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZ || MAINDllOBJECT->getCurrentUCS().UCSProjectionType() == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
			Cbaseaction->AllPointsList.Addpoint(new SinglePoint(MAINDllOBJECT->getCurrentDRO().getX(), MAINDllOBJECT->getCurrentDRO().getZ(), MAINDllOBJECT->getCurrentDRO().getY()));
		ShapeWithList* currentshape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		currentshape->ManagePoint(Cbaseaction, ShapeWithList::ADD_POINT);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0015", __FILE__, __FUNCSIG__); }
}

void FocusFunctions::ResetSelectedDepthMeasure()
{
	try
	{
		if(CurrentDepthMeasure == NULL)
		{
			return; //no depth measure selected..
		}
		else
		{
			if(CurrentDepthMeasure->ParentShape1 != NULL)
			{
				CurrentDepthMeasure->ParentShape1->RemoveMChild(CurrentDepthMeasure);
				CurrentDepthMeasure->RemoveParent(CurrentDepthMeasure->ParentShape1);
				if(CurrentDepthMeasure->ParentShape1->getMchild().size() == 0)
				{
					MAINDllOBJECT->selectShape(CurrentDepthMeasure->ParentShape1->getId(), false);
					MAINDllOBJECT->Wait_VideoGraphicsUpdate();
					MAINDllOBJECT->Wait_RcadGraphicsUpdate();
					MAINDllOBJECT->deleteForEscape();
					MAINDllOBJECT->Wait_VideoGraphicsUpdate();
					MAINDllOBJECT->Wait_RcadGraphicsUpdate();
				}
				CurrentDepthMeasure->ParentShape1 = NULL;
			}
			if(CurrentDepthMeasure->ParentShape2 != NULL)
			{
				CurrentDepthMeasure->ParentShape2->RemoveMChild(CurrentDepthMeasure);
				CurrentDepthMeasure->RemoveParent(CurrentDepthMeasure->ParentShape2);
				if(CurrentDepthMeasure->ParentShape2->getMchild().size() == 0)
				{
					MAINDllOBJECT->selectShape(CurrentDepthMeasure->ParentShape2->getId(), false);
					MAINDllOBJECT->Wait_VideoGraphicsUpdate();
					MAINDllOBJECT->Wait_RcadGraphicsUpdate();
					MAINDllOBJECT->deleteForEscape();
					MAINDllOBJECT->Wait_VideoGraphicsUpdate();
					MAINDllOBJECT->Wait_RcadGraphicsUpdate();
				}
				CurrentDepthMeasure->ParentShape2 = NULL;
			}
			CurrentDepthMeasure->UpdateMeasurement();
			CurrentDepthMeasure->notifyAll(ITEMSTATUS::DATACHANGED, CurrentDepthMeasure);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0015", __FILE__, __FUNCSIG__); }
}

//Focus depth Z1 value..
bool FocusFunctions::GetZ1_value(double* dval)
{
	try
	{
		if(CurrentDepthMeasure == NULL) return false;
		DimDepth* Cdim = (DimDepth*)CurrentDepthMeasure;
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
			*dval = Cdim->ValueZ1()/25.4;
		else
			*dval = Cdim->ValueZ1();
		if(Cdim->ParentShape1 != NULL)
			return true;
		else
			return false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0015", __FILE__, __FUNCSIG__); }
}

//Focus depth Z2 value..
bool FocusFunctions::GetZ2_value(double* dval)
{
	try
	{
		if(CurrentDepthMeasure == NULL) return false;
		DimDepth* Cdim = (DimDepth*)CurrentDepthMeasure;
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
			*dval = Cdim->ValueZ2()/25.4;
		else
			*dval = Cdim->ValueZ2();
		if(Cdim->TwoBoxDepthMeasure())
		{
			if(Cdim->ParentShape1 != NULL)
				return true;
			else
				return false;
		}
		else
		{
			if(Cdim->ParentShape2 != NULL)
				return true;
			else
				return false;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0015", __FILE__, __FUNCSIG__); }
}

bool FocusFunctions::GetDepth_value(double* dval)
{
	try
	{
		if(CurrentDepthMeasure == NULL) return false;
		DimDepth* Cdim = (DimDepth*)CurrentDepthMeasure;
		if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
			*dval = Cdim->DepthValue()/25.4;
		else
			*dval = Cdim->DepthValue();
		if(Cdim->TwoBoxDepthMeasure())
		{
			if(Cdim->ParentShape1 != NULL)
				return true;
			else
				return false;
		}
		else
		{
			if(Cdim->ParentShape1 != NULL && Cdim->ParentShape2 != NULL)
				return true;
			else
				return false;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0015", __FILE__, __FUNCSIG__); }
}

//Add new focus datum.. Select from list..
void FocusFunctions::AddNewFocusDatum(int id)
{
	try
	{
		CurrentDepthShape = (Shape*)MAINDllOBJECT->getShapesList().getList()[id];
		if(CurrentDepthMeasure->ParentShape1 != NULL && CurrentDepthMeasure->ParentShape2 != NULL)	return;
		if(CurrentDepthMeasure->ParentShape1 == NULL)
		{
			CurrentDepthMeasure->ParentShape1 = CurrentDepthShape;
			((DimDepth*)CurrentDepthMeasure)->dummyShapesType[0] = (int)ShapeForDepth;
		}
		else if(CurrentDepthMeasure->ParentShape1->getId() != CurrentDepthShape->getId())
		{
			CurrentDepthMeasure->ParentShape2 = CurrentDepthShape;
			((DimDepth*)CurrentDepthMeasure)->dummyShapesType[1] = (int)ShapeForDepth;
		}
		else return;

		CurrentDepthMeasure->addParent(CurrentDepthShape);
		CurrentDepthShape->addMChild(CurrentDepthMeasure);
		FocusZValue(((ShapeWithList*)CurrentDepthShape)->PointsList->getList()[0]->Z);
		CurrentDepthMeasure->UpdateMeasurement();
		FocusForReferenceDepth();
		CurrentDepthMeasure->notifyAll(ITEMSTATUS::DATACHANGED, CurrentDepthMeasure);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0016", __FILE__, __FUNCSIG__); }
}

//delete selected focus datum.,..
void FocusFunctions::DeleteFocusDatum()
{
	try
	{
		MAINDllOBJECT->getShapesList().clearSelection(true);
		CurrentDepthShape->selected(true);
		DeleteShapesAction* dsa = new DeleteShapesAction();
		MAINDllOBJECT->addAction(dsa);
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0017", __FILE__, __FUNCSIG__); }
}

//delete all focus datum..
void FocusFunctions::DeleteAllFocusDatum()
{
	try
	{
		MAINDllOBJECT->getShapesList().clearSelection(true);
		MAINDllOBJECT->getDimList().clearSelection(true);
		for each(int id in DepthDatumList)
		{
			Shape* Cshape = (Shape*)MAINDllOBJECT->getShapesList().getList()[id];
			MAINDllOBJECT->getShapesList().notifyRemove(Cshape);
			MAINDllOBJECT->getShapesList().getList().erase(Cshape->getId());
			delete Cshape;
		}
		for each(int id in DepthMeasureList)
		{
			DimBase* Cdim = (DimBase*)MAINDllOBJECT->getDimList().getList()[id];
			MAINDllOBJECT->getDimList().notifyRemove(Cdim);
			MAINDllOBJECT->getDimList().getList().erase(Cdim->getId());
			delete Cdim;
		}
		DepthShape::reset();
		DimDepth::reset();
		DepthDatumList.clear();
		DepthMeasureList.clear();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0018", __FILE__, __FUNCSIG__); }
}

//Set the Focus scan speed and span..///
void FocusFunctions::setFocusSpan(double span, double speed)
{
	fspan = span; fspeed = speed;
}

//Set the Focus scan axis..///
void FocusFunctions::SetSelectedAxis(int axis)
{
	selectedAxis = axis;
}

void FocusFunctions::SetFocuScanMode()
{
	try
	{
		MAINDllOBJECT->SetStatusCode("FocusFunctionsSetFocuScanMode01");
		if(MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER)
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, false);
		FocusScanMode.Toggle();
		if(FocusScanMode())
		{
			ContourScanMode(false);
			FocusOnMode(true);
			CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::FOCUSSCAN_MODE;
		}
		else
		{
			CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::NOTHING;
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0019", __FILE__, __FUNCSIG__); }
}

//Set ContourScanMode..
void FocusFunctions::SetContourScanMode()
{
	try
	{
		if(MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER)
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, false);
		ContourScanMode.Toggle();
		if(ContourScanMode())
		{
			FocusScanMode(false);
			FocusOnMode(true);
			CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::CONTOOURSCAN_MODE;
		}
		else
		{
			CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::NOTHING;
			FocusOnMode(false);
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0019", __FILE__, __FUNCSIG__); }
}

//draw focus rectangles
void FocusFunctions::drawFocusRectangels()
{
	try
	{
		if(!this->FocusOnMode()) return;
		switch(CurrentFocusType)
		{
			case RapidEnums::RAPIDFOCUSMODE::FOCUSSCAN_MODE:
			case RapidEnums::RAPIDFOCUSMODE::FOCUSDEPTH_MODE:
			case RapidEnums::RAPIDFOCUSMODE::CALIBRATION_MODE:
			case RapidEnums::RAPIDFOCUSMODE::AUTOFOCUSSCANMODE:
			{
				if(focusRectangleGap >= focusRectangleWidth && focusRectangleGap >= focusRectangleHeight)
				{
					//GRAFIX->drawFocusRectangle(focusRectangleRow, focusRectangleColumn, focusRectangleGap, focusRectangleWidth, focusRectangleHeight, ShiftInPartProgramX * MAINDllOBJECT->getWindow(0).getUppX(), ShiftInPartProgramY * MAINDllOBJECT->getWindow(0).getUppX());
					GRAFIX->drawFocusRectangle(focusRectangleRow, focusRectangleColumn, focusRectangleGap / HELPEROBJECT->CamSizeRatio, focusRectangleWidth / HELPEROBJECT->CamSizeRatio, focusRectangleHeight / HELPEROBJECT->CamSizeRatio, ShiftInPartProgramX * MAINDllOBJECT->getWindow(0).getUppX(), ShiftInPartProgramY * MAINDllOBJECT->getWindow(0).getUppX());
				}
				else
				{
					//GRAFIX->drawFocusRectangle(1, 1, 0, (focusRectangleGap * (focusRectangleColumn - 1) + focusRectangleWidth), (focusRectangleGap * (focusRectangleRow - 1) + focusRectangleHeight) , ShiftInPartProgramX * MAINDllOBJECT->getWindow(0).getUppX(), ShiftInPartProgramY * MAINDllOBJECT->getWindow(0).getUppX());
					GRAFIX->drawFocusRectangle(1, 1, 0, (focusRectangleGap * (focusRectangleColumn - 1) + focusRectangleWidth) / HELPEROBJECT->CamSizeRatio, (focusRectangleGap * (focusRectangleRow - 1) + focusRectangleHeight) / HELPEROBJECT->CamSizeRatio, ShiftInPartProgramX * MAINDllOBJECT->getWindow(0).getUppX(), ShiftInPartProgramY * MAINDllOBJECT->getWindow(0).getUppX());
				}
				break;
				}
			case RapidEnums::RAPIDFOCUSMODE::AUTOCONTOURSCANMODE:
			case RapidEnums::RAPIDFOCUSMODE::CONTOOURSCAN_MODE:
				//GRAFIX->drawFocusRectangle(1, 1, 0, ContourScanWidth, ContourScanHeight, ShiftInPartProgramX * MAINDllOBJECT->getWindow(0).getUppX(), ShiftInPartProgramY * MAINDllOBJECT->getWindow(0).getUppX());
				//if (ContourScanHeight - 1 == focusRectangleHeight)
				//	GRAFIX->drawFocusRectangle(1, 1, 0, ContourScanWidth / HELPEROBJECT->CamSizeRatio, ContourScanHeight , ShiftInPartProgramX * MAINDllOBJECT->getWindow(0).getUppX(), ShiftInPartProgramY * MAINDllOBJECT->getWindow(0).getUppX());
				//else
					GRAFIX->drawFocusRectangle(1, 1, 0, ContourScanWidth / HELPEROBJECT->CamSizeRatio, ContourScanHeight / HELPEROBJECT->CamSizeRatio, ShiftInPartProgramX * MAINDllOBJECT->getWindow(0).getUppX(), ShiftInPartProgramY * MAINDllOBJECT->getWindow(0).getUppX());
				break;
			case RapidEnums::RAPIDFOCUSMODE::HOMING_MODE:
			case RapidEnums::RAPIDFOCUSMODE::REFERENCEDOT_SECOND:
				break;
			case RapidEnums::RAPIDFOCUSMODE::NOTHING:
				GRAFIX->drawFocusRectangle(focusRectangleRow, focusRectangleColumn, focusRectangleGap, focusRectangleWidth, focusRectangleHeight, ShiftInPartProgramX * MAINDllOBJECT->getWindow(0).getUppX(), ShiftInPartProgramY * MAINDllOBJECT->getWindow(0).getUppX());
				//GRAFIX->drawFocusRectangle(focusRectangleRow, focusRectangleColumn, focusRectangleGap / HELPEROBJECT->CamSizeRatio, focusRectangleWidth / HELPEROBJECT->CamSizeRatio, focusRectangleHeight / HELPEROBJECT->CamSizeRatio, ShiftInPartProgramX * MAINDllOBJECT->getWindow(0).getUppX(), ShiftInPartProgramY * MAINDllOBJECT->getWindow(0).getUppX());
				break;
			case RapidEnums::RAPIDFOCUSMODE::TWOBOXDEPTH_MODE:
				break;
			case RapidEnums::RAPIDFOCUSMODE::FOURBOXDEPTH_MODE:
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0020", __FILE__, __FUNCSIG__); }
}

//clear Last focus values..
void FocusFunctions::ClearLastFocusValues()
{
	try
	{
		memset(ZvalueColl, 0, 400000);
		memset(FocusValueColl, -1000, 3000000);
		Focus_firstvalue = true;
		TotalFocusValueCount = 0; NoofIterationCount = 0;
		this->CalculateFocusMetric(false); this->RapidCamFocusIshappening(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0021", __FILE__, __FUNCSIG__); }
}

//Clear All..
void FocusFunctions::ClrearAllFocus(bool CleardatumReference)
{
	try
	{
		this->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::NOTHING;
		this->FocusScanMode(false); this->ContourScanMode(false);
		this->FocusOnMode(false);
		this->CalculateFocusMetric(false); this->RapidCamFocusIshappening(false);
		this->focusRectangleColumn = 1; this->focusRectangleRow = 1;
		this->focusRectangleGap = 40;
		this->focusRectangleHeight = 40; this->focusRectangleWidth = 40;
		this->ContourScanWidth = 40; this->ContourScanHeight = 40;
		if(Cvalue != NULL) { free(Cvalue); Cvalue = NULL;}
		if(Rectptr != NULL) { free(Rectptr); Rectptr = NULL;}
		ClearLastFocusValues();
		ShiftInPartProgramX = 0;
		ShiftInPartProgramY = 0;
		DepthZcol.clear();
		ShapeForDepth = FOCUSDEPTHSHAPETYPE::POINTSHAPE;
		if(CleardatumReference)
		{
			CurrentDepthMeasure = NULL;
			DepthDatumList.clear(); 
			DepthMeasureList.clear();
		}
		if(FiveFocus != NULL)
		{
			free(FiveFocus);
			FiveFocus = NULL;
		}
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0022", __FILE__, __FUNCSIG__);
	}
}

void FocusFunctions::ResetFocusCalibrationSettings()
{
	try
	{
		CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::NOTHING;
		this->FocusCalibrationCount(0);
		FocusCalibrationPtColl.deleteAll();
		FocusCalibrationPtCollDeviation[0].deleteAll();
		FocusCalibrationPtCollDeviation[1].deleteAll();
		FocusCalibrationPtCollDeviation[2].deleteAll();
		FocusCalibrationPtCollDeviation[3].deleteAll();
		FocusCalibrationPtCollForPlane.deleteAll();
		framescancount = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0022", __FILE__, __FUNCSIG__); }
}

void FocusFunctions::SetFocusCalibrationValues(std::list<double> *Values,double *CalibError, double CellWidth, double Cellheight)
{
	try
	{
		int Cnt = 0;
		for each(double val in *Values)
		{
			FocusErrCorrectionValue[Cnt++] = val;
		}
		for(int i =0; i < 4; i++)
		{
			FocusCalibError[i] = CalibError[i];
		}
		//Set in RBF..
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0022", __FILE__, __FUNCSIG__); }
}

bool FocusFunctions::FocusClicked_Handler()
{
	try
	{
		FocusDepthMultiBoxHandler* Chandler = (FocusDepthMultiBoxHandler*)MAINDllOBJECT->getCurrentHandler();
		bool flag = Chandler->FocusButtonClicked();
		return flag;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("FOCUSCALC0022", __FILE__, __FUNCSIG__); }
}

bool FocusFunctions::StartAutoFocusScan()
{
   return ((AutoFocusScanHandler*)(MAINDllOBJECT->getCurrentHandler()))->StartAutoFocus();
}

void FocusFunctions::RotateInAutoFocus(double rotationAngle, int step)
{
	((AutoFocusScanHandler*)(MAINDllOBJECT->getCurrentHandler()))->RotateAutoFocusScanSurfaces(rotationAngle, step);
}

void FocusFunctions::ContinueAutoFocus()
{
	((AutoFocusScanHandler*)(MAINDllOBJECT->getCurrentHandler()))->ContinueAutoFocus();
}

void FocusFunctions::AutoFocusClearAll()
{
	((AutoFocusScanHandler*)(MAINDllOBJECT->getCurrentHandler()))->ClearAll();
}

bool FocusFunctions::StartAutoContourScan()
{
	 return ((AutoContourScanHandler*)(MAINDllOBJECT->getCurrentHandler()))->StartAutoFocus();
}

void FocusFunctions::ContinueAutoContourScan()
{
	  ((AutoContourScanHandler*)(MAINDllOBJECT->getCurrentHandler()))->ContinueAutoContourScan();
}

void FocusFunctions::RotateInContourScan(double rotationAngle, int step)
{
	((AutoContourScanHandler*)(MAINDllOBJECT->getCurrentHandler()))->RotateAutoContourScanPoints(rotationAngle, step);
}

void FocusFunctions::AutoContourClearAll()
{
	((AutoContourScanHandler*)(MAINDllOBJECT->getCurrentHandler()))->ClearAll();
}

void FocusFunctions::CreateGrainSurface()
{	
	//double scanWidth =  (focusRectangleColumn * (focusRectangleWidth * HELPEROBJECT->CamSizeRatio + focusRectangleGap)) * MAINDllOBJECT->getWindow(0).getUppX();
	//double scanHeight = (focusRectangleRow * (focusRectangleHeight * HELPEROBJECT->CamSizeRatio + focusRectangleGap)) * MAINDllOBJECT->getWindow(0).getUppX();
	double scanWidth = (focusRectangleColumn * (focusRectangleWidth + focusRectangleGap)) * MAINDllOBJECT->getWindow(0).getUppX();
	double scanHeight = (focusRectangleRow * (focusRectangleHeight + focusRectangleGap)) * MAINDllOBJECT->getWindow(0).getUppX();

	int cells = focusRectangleColumn * focusRectangleRow, rowNo = pow(2, GrainFactor()), colNo = pow(2, GrainFactor());
	GrainCellWidth = scanWidth / colNo;
	GrainCellHeight = scanHeight / rowNo;
	for(int i = 1; i <= rowNo; i++)
	{
	   for(int j = 1; j <= colNo; j++)
	   {
		   GrainSurfaceList[(i - 1) * rowNo + j] = new GrainSurface((i - 1) * rowNo + j, j * GrainCellWidth, i * GrainCellHeight, cells); 
	   }
	}
}

void FocusFunctions::CreateLookUpTable()
{
	int cells = focusRectangleColumn * focusRectangleRow, rowNo = pow(2, GrainFactor()), colNo = pow(2, GrainFactor());
	double upp = MAINDllOBJECT->getWindow(0).getUppX();
	for(int i = 0; i < cells; i++)
	{
		double x = (Rectptr[4 * i] + (Rectptr[4 * i + 2]/2)) * upp;
		double y = (Rectptr[4 * i + 1] + (Rectptr[4 * i + 3]/2)) * upp;
		int id = 1;
		if(rowNo > 1)
			id = getGrainSurfaceId(x, y, rowNo, colNo);
		GrainLookUpTable[i] = id;
	}
}

int FocusFunctions::getGrainSurfaceId(double x, double y, int grainRows, int grainCols)
{
	int Xindex = 0, Yindex = 0, low = 0, high = grainRows, mid = high / 2;
	double upp = MAINDllOBJECT->getWindow(0).getUppX(), startX = Rectptr[0] * upp, startY = Rectptr[1] * upp;
	while(low < high)
	{
	   if((x >= (startX + low * GrainCellWidth)) && (x <= (startX + mid * GrainCellWidth)))
	   {
		  if((high - low) == 2)
		  {
			 Xindex = mid;
			 break;
		  }
		  high = mid;
		  mid = low + (high - low)/2;
	   }
	   else
	   {
		  if((high - low) == 2)
		  {
			 Xindex = high;
			 break;
		  }
		  low = mid;
		  mid = low + (high - low)/2;
	   }
	}
	low = 0, high = grainCols, mid = high / 2;

	while(low < high)
	{
	   if((y >= (startY + low * GrainCellHeight)) && (y <= (startY + mid * GrainCellHeight)))
	   {
		  if((high - low) == 2)
		  {
			 Yindex = mid;
			 break;
		  }
		  high = mid;
		  mid = low + (high - low)/2;
	   }
	   else
	   {
		  if((high - low) == 2)
		  {
			 Yindex = high;
			 break;
		  }
		  low = mid;
		  mid = low + (high - low)/2;
	   }
	}
	return ((Yindex - 1) * grainCols + Xindex);
}

double FocusFunctions::CalculateFocus_DefinedPts(double* Pts, int NumberOfPts)
{
	double answer = 0;
	bool checkflag1 = BESTFITOBJECT->CalculateFocus(Pts, NumberOfPts, FocusActiveWidth(), &answer, MaxMinFocusRatioCutoff(), MaxMinZRatioCutoff(), SigmaActiveWdthRatioCutoff());
	return answer;
}

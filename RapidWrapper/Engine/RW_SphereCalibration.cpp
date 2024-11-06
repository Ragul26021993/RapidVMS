#include "Stdafx.h"
#include "RW_MainInterface.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "..\MAINDLL\Shapes\Sphere.h"
#include "..\MAINDLL\Engine\FocusFunctions.h"

int Listcount = 0;

void CALLBACK VisionSpherCalibrationOverCallback(double x, double y, double z)
{
	RWrapper::RW_SphereCalibration::MYINSTANCE()->GotoFocusPosition(x, y, z);
}

RWrapper::RW_SphereCalibration::RW_SphereCalibration()
{
	 try
	 {
		 SphereCalibration = this;
		 TakeManualPoint = true; RefSphereCalibrate = false;
		 DoingCalibration = false;
		 CalibratedOffset = gcnew cli::array<double>(12);
		 StartPoint = gcnew cli::array<double>(3); 
		 CurrentSpherePoints = gcnew cli::array<double>(27);
		 CurrentGotoPosition = gcnew cli::array<double>(27);
		 ProbeRadius = gcnew cli::array<double>(5);
		 Target = gcnew cli::array<double>(4);
		 Feedrate = gcnew cli::array<double>(4);
		 ReferenceCenter = gcnew cli::array<double>(4);
		 SphereRadius = 10;
		 PointTakenCount = 0;
		 UserDefinedProbeRadius = 0;
		 ProbeDir = gcnew cli::array<int>(9); 
		 CircularInterpolation = false;
		 MAINDllOBJECT->VisionSpherCalibrationOver = &VisionSpherCalibrationOverCallback;
		 instantiated = true;
	 }
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0001", ex); }
}

RWrapper::RW_SphereCalibration::~RW_SphereCalibration()
{
}

RWrapper::RW_SphereCalibration^ RWrapper::RW_SphereCalibration::MYINSTANCE()
{
	return SphereCalibration;
}

void RWrapper::RW_SphereCalibration::StartSphereCallibration(cli::array<double, 1>^ values, int inspectionType, int currentSide, int currentProbetype, bool circular, bool includeStrt)
{
	try
	{
		Shape* CShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		if(CShape != NULL)
		{
			if(CShape->getId() == MAINDllOBJECT->ToolAxisLineId - 1)
				RefSphereCalibrate = true;
		}
		DoingCalibration = true;
		TakeManualPoint = true;
		RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::SPHERECALIBRATIONMANUALPOINT);
		PointTakenCount = 0;
		CurrentSide = currentSide; 

		LastSide = currentSide;
		CurrentProbetype = currentProbetype;
		CircularInterpolation = circular;
		InspectionType = inspectionType;
		int addfactor = 0;
		if(includeStrt) addfactor = 1;
		if(currentProbetype == 2 && CurrentSide == 5)
           CurrentSide = 3;
		if(InspectionType == 0 || InspectionType == 4)
		{
			switch (currentProbetype)
			{
	    	   case 1:
                  CurrentSide = 1;
				  LastSide = 1;
	              break;
			   case 2:
				  //CurrentSide = 1;
				   CurrentSide = currentSide;
				   LastSide = 2 + addfactor;
	              break;
			   case 3:
				  //CurrentSide = 3;
				   CurrentSide = currentSide;
				   LastSide = 4 + addfactor;
	              break;
			   case 4:
				  //CurrentSide = 1;
				   CurrentSide = currentSide;
				   LastSide = 4 + addfactor;
	              break;
			}
		}
		SphereRadius = values[0];
		UserDefinedProbeRadius = values[1];	
		const char* temporarychar = (const char*)(Marshal::StringToHGlobalAnsi(RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath)).ToPointer();
		std::string FilePath = temporarychar;
		FilePath = FilePath + "\\Database";
		if(InspectionType == 0)
		{
			wofstream ofile;
			ofile.open(FilePath + "\\SphereCalibration.txt");
			ofile.close();
		}
		TakeProbeHit = true;
		probedirection = 1;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0004", ex); }
}

void RWrapper::RW_SphereCalibration::ProbeHitHandle(int dir, double *pnt)
{
	try
	{
		if(TakeManualPoint)
		{
			RaisePointsCountEvent();
			TakeManualPoint = false;
			for(int i = 0; i < 3; i++) {StartPoint[i] = pnt[i];}
			
			if(CurrentProbetype != 1)
			{
				if(CircularInterpolation)
				{
					if(CurrentSide == 1)
						CreatePointsListForLRProbeCircular(true);
					else if (CurrentSide == 2)
						CreatePointsListForLRProbeCircular(false);
					else if (CurrentSide == 3 && CurrentProbetype == 2)
						CreatePointsListForNormalProbeCircular();
					else if (CurrentSide == 3 && CurrentProbetype != 2)
						CreatePointsListForFBProbeCircular(true);
					else if (CurrentSide == 4)
						CreatePointsListForFBProbeCircular(false);		
					else if (CurrentSide == 5)
				    	 CreatePointsListForNormalProbeCircular();
				}
				else
				{
		     		if(CurrentSide == 1)
						CreatePointsListForLRProbe(true); // Left
					else if (CurrentSide == 2)
						CreatePointsListForLRProbe(false); //Right
					else if (CurrentSide == 3 && CurrentProbetype == 2) // T-Probe AlongX
						CreatePointsListForNormalProbe(); // Straight
					else if (CurrentSide == 3 && CurrentProbetype != 2) //Star Probe or T-Probe along Y
						CreatePointsListForFBProbe(true); //Front
					else if (CurrentSide == 4)
						CreatePointsListForFBProbe(false); //Back
					else if (CurrentSide == 5)
						CreatePointsListForNormalProbe();	//Normal
				}
			}
			else 
			{
				if(CircularInterpolation)
				   CreatePointsListForNormalProbeCircular();
				else
				   CreatePointsListForNormalProbe();	
	  		}
   		}
		else if(PointTakenCount < 9)
		{
			 if(InspectionType > 0)
			 {
				switch(CurrentProbetype)
				{
					case 2:
					{
						if(CurrentSide == 2)
						{
							pnt[0] += MAINDllOBJECT->X_AxisProbeArmLengthX();
							pnt[1] += MAINDllOBJECT->X_AxisProbeArmLengthY();
							pnt[2] += MAINDllOBJECT->X_AxisProbeArmLengthZ();
						}
						else if(CurrentSide == 3)
						{
							pnt[0] += MAINDllOBJECT->StarStraightOffsetX();
							pnt[1] += MAINDllOBJECT->StarStraightOffsetY();
							pnt[2] += MAINDllOBJECT->StarStraightOffsetZ();
						}
						break;
					}
					case 3:
					{
						if(CurrentSide == 4)
						{
							pnt[0] += MAINDllOBJECT->Y_AxisProbeArmLengthX();
							pnt[1] += MAINDllOBJECT->Y_AxisProbeArmLengthY();
							pnt[2] += MAINDllOBJECT->Y_AxisProbeArmLengthZ();
						}
						else if(CurrentSide == 5)
						{
							pnt[0] += MAINDllOBJECT->StarStraightOffsetX();
							pnt[1] += MAINDllOBJECT->StarStraightOffsetY();
							pnt[2] += MAINDllOBJECT->StarStraightOffsetZ();
						}
						break;
					}
					case 4:
					{
						if(CurrentSide == 3)
						{
							pnt[0] += MAINDllOBJECT->StarProbeRightOffsetX();
							pnt[1] += MAINDllOBJECT->StarProbeRightOffsetY();
							pnt[2] += MAINDllOBJECT->StarProbeRightOffsetZ();
						}
						else if(CurrentSide == 4)
						{
							pnt[0] += MAINDllOBJECT->StarProbeFrontOffsetX();
							pnt[1] += MAINDllOBJECT->StarProbeFrontOffsetY();
							pnt[2] += MAINDllOBJECT->StarProbeFrontOffsetZ();
						}
						else if(CurrentSide == 5)
						{
							pnt[0] += MAINDllOBJECT->StarStraightOffsetX();
							pnt[1] += MAINDllOBJECT->StarStraightOffsetY();
							pnt[2] += MAINDllOBJECT->StarStraightOffsetZ();
						}
		    			break;
					}
				}
			}
			for(int i = 0; i < 3; i ++) {CurrentSpherePoints[3 * PointTakenCount + i] = pnt[i];}
			ProbeDir[PointTakenCount] = dir;
			PointTakenCount++;
			if(RefSphereCalibrate)
				AddPointsToSelectedShape(dir, pnt);
			RaisePointsCountEvent();
			//SendDrotoNextposition();
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0004", ex); }
}

void RWrapper::RW_SphereCalibration::CalibrateCurrentSide()
{
	try
	{
	   const char* temporarychar = (const char*)(Marshal::StringToHGlobalAnsi(RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath)).ToPointer();
	   std::string FilePath = temporarychar;
	   FilePath = FilePath + "\\Log\\PointsLog";
  	   if(PointTakenCount == 0) return;
       double pnts[27] = {0}, pnts2[27] = {0}, ans[4] = {0};
	   wofstream ptsfile;
	   ptsfile.open(FilePath + "\\SpherePts.csv", ios::app);
	   int cnt = 0;
	   for (int j = 0; j < 27; j++)
	   {
		   pnts[j] = CurrentSpherePoints[j];
		   cnt++;
		   ptsfile << pnts[j] << ", ";
		   if (cnt == 3)
		   {
			   ptsfile << endl;
			   cnt = 0;
		   }
	   }
	   BESTFITOBJECT->Sphere_BestFit(pnts, 9, &ans[0], MAINDllOBJECT->SigmaModeFlag());
	   if(InspectionType > 0)
	   {
		  for(int i = 0; i < 9; i++)
		  {
    		double Pradius = 0;
			if(CurrentProbetype == 1)
			{
			    Pradius = MAINDllOBJECT->ProbeRadius();
			}
			else 
			{
				switch(CurrentSide)
				{
					case 1:
			    		Pradius = MAINDllOBJECT->X_AxisProbeRadiusLeft();
						break;
					case 2:
			       		Pradius = MAINDllOBJECT->X_AxisProbeRadiusRight();
						break;
					case 3:
			    		Pradius = MAINDllOBJECT->Y_AxisProbeRadiusTop();
						break;
					case 4:
			    		Pradius = MAINDllOBJECT->Y_AxisProbeRadiusBottom();
						break;
					case 5:
			    		Pradius = MAINDllOBJECT->X_AxisProbeRadiusStrt();
						break;
				 }
			 }
			//The best fit calculated the base sphere. We have to add the radius of the probe to get actual value (pnts2) stores the corrected positions
			RMATH3DOBJECT->Sphere_SurfacePt_for_Probe(&ans[0], &pnts[3 * i], Pradius, false, &pnts2[3 * i]);
		}
		if(InspectionType == 1 || InspectionType == 4)
		    BESTFITOBJECT->Sphere_BestFit(pnts2, 9, &ans[0], MAINDllOBJECT->SigmaModeFlag());
		else if(InspectionType == 2)
		{
			MAINDllOBJECT->AddPointsToAutoSphere(pnts2, 9);
			return;
		}
		else if(InspectionType == 3)
		{
			 BESTFITOBJECT->Sphere_BestFit(pnts2, 9, &ans[0], MAINDllOBJECT->SigmaModeFlag());
			 ReferenceCenter[0] = ans[0];
			 ReferenceCenter[1] = ans[1];
			 ReferenceCenter[2] = ans[2];
	    	 ChangeHandlerToVision();
			 return;
		}
	 }

	 ProbeRadius[CurrentSide - 1] =  ans[3] - SphereRadius;

	 if(InspectionType == 1 || InspectionType == 4)
	 {
		wofstream ofile;			
		ofile.open(FilePath + "\\ProbeInspection.csv");			
		if(CurrentSide == 1)
		{
			if(CurrentProbetype == 1)
				ofile << "NormalDia" << ", " << 2 * ans[3] << endl;
			else
		        ofile << "LeftDia" << ", " << 2 * ans[3] << endl;		
			ofile << "Center" << ", " << ans[0] << ", " << ans[1] << ", " << ans[2] << endl;
			ofile << endl;
		}
		else if(CurrentSide == 2)
		{
			ofile << "RightDia" << ", " << 2 * ans[3] << endl;
			ofile << "Center" << ", " << ans[0] << ", " << ans[1] << ", " << ans[2] << endl;
			ofile << endl;
		}
		else if(CurrentSide == 3 && CurrentProbetype != 2)
		{
			ofile << "FrontDia" << ", " << 2 * ans[3] << endl;
			ofile << "Center" << ", " << ans[0] << ", " << ans[1] << ", " << ans[2] << endl;
			ofile << endl;
		}
		else if(CurrentSide == 4)
		{
			ofile << "BackDia" << ", " << 2 * ans[3] << endl;
			ofile << "Center" << ", " << ans[0] << ", " << ans[1] << ", " << ans[2] << endl;
			ofile << endl;
		}
		else if(CurrentSide == 5 || CurrentProbetype == 2)
		{
			ofile << "StraightDia" << ", " << 2 * ans[3] << endl;
			ofile << "Center" << ", " << ans[0] << ", " << ans[1] << ", " << ans[2] << endl;
			ofile << endl;
		}
		ofile.close();
	 }
	 else
	 {		   
			if(CurrentProbetype == 3 && CurrentSide == 3)
			{
			    ReferenceCenter[0] = ans[0];
				ReferenceCenter[1] = ans[1];
				ReferenceCenter[2] = ans[2];	
			}
			else if(CurrentSide == 1)
			{
				ReferenceCenter[0] = ans[0];
				ReferenceCenter[1] = ans[1];
				ReferenceCenter[2] = ans[2];	
			}
			else
			{ 
				double currentCenter[3] = {ans[0], ans[1], ans[2]};
				for(int i = 0; i < 3; i++){ CalibratedOffset[3 * (CurrentSide - 2) + i] =  ReferenceCenter[i] - currentCenter[i];}
			}
			wofstream ofile;
			ofile.open(FilePath + "\\SphereCalibration.csv", ios::app);
			if(CurrentSide == 1)
			{
				if(CurrentProbetype == 1)
					ofile << "NormalDia" << ", " << 2 * ProbeRadius[0] << endl;
				else
					ofile << "LeftDia" << ", " << 2 * ProbeRadius[0] << endl;
				ofile << "Center" << ", " << ans[0] << ", " << ans[1] << ", " << ans[2] << endl;
			}
			else if(CurrentSide == 2)
			{
				ofile << "RightDia" << ", " << 2 * ProbeRadius[1] << endl;
				ofile << "Center" << ", " << ans[0] << ", " << ans[1] << ", " << ans[2] << endl;
				ofile << "Right Offset" << ", " << CalibratedOffset[0] << ", " << CalibratedOffset[1] << ", " << CalibratedOffset[2] << endl;
			}
			else if(CurrentSide == 3 && CurrentProbetype != 2)
			{
				ofile << "FrontDia" << ", " << 2 * ProbeRadius[2] << endl;
				ofile << "Center" << ", " << ans[0] << ", " << ans[1] << ", " << ans[2] << endl;
				if(CurrentProbetype != 3)
			    	ofile << "Front Offset" << ", " << CalibratedOffset[3] << ", " << CalibratedOffset[4] << ", " << CalibratedOffset[5] << endl;
			}
			else if(CurrentSide == 4)
			{
				ofile << "BackDia" << ", " << 2 * ProbeRadius[3] << endl;
				ofile << "Center" << ", " << ans[0] << ", " << ans[1] << ", " << ans[2] << endl;
				ofile << "Back Offset" << ", " << CalibratedOffset[6] << ", " << CalibratedOffset[7] << ", " << CalibratedOffset[8] << endl;
			}
			else if(CurrentSide == 5 || CurrentProbetype != 2)
			{
				ofile << "StraightDia" << ", " << 2 * ProbeRadius[4] << endl;
				ofile << "Center" << ", " << ans[0] << ", " << ans[1] << ", " << ans[2] << endl;
				ofile << "Straight Offset" << ", " << CalibratedOffset[9] << ", " << CalibratedOffset[10] << ", " << CalibratedOffset[11] << endl;
			}
			else if(CurrentSide == 2 || CurrentProbetype == 2)
			{
				ofile << "StraightDia" << ", " << 2 * ProbeRadius[2] << endl;
				ofile << "Center" << ", " << ans[0] << ", " << ans[1] << ", " << ans[2] << endl;
				ofile << "Straight Offset" << ", " << CalibratedOffset[3] << ", " << CalibratedOffset[4] << ", " << CalibratedOffset[5] << endl;
			}
			ofile.close();
	   }
   }
   catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0004", ex); }
}

void RWrapper::RW_SphereCalibration::PauseCalibration()
{
	RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
}

void RWrapper::RW_SphereCalibration::ContinueCalibration()
{
	double target[4] = {0}, feedrate[4] = {0};
	for(int i = 0; i < 4; i++)
	{
			target[i] = Target[i];
			feedrate[i] = Feedrate[i];
	}
	RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
	RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], CurrentTargetType);
}

void RWrapper::RW_SphereCalibration::StopCalibration()
{
	RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
	ClearAll();
}

void RWrapper::RW_SphereCalibration::ClearAll()
{
	try
	{		
		DoingCalibration = false;
		CurrentSide = 0;
		LastSide = 0;
		SphereRadius = 0;
		TakeManualPoint = true; RefSphereCalibrate = false;
		PointTakenCount = 0;
		UserDefinedProbeRadius = 0;
		TakeProbeHit = false;
		probedirection = 1;
		Listcount = 0;
		CircularInterpolation = false;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWFixCal0007", ex); }
}

void RWrapper::RW_SphereCalibration::CreatePointsListForLRProbe(bool LeftProbe)
{
	try
	{
		Listcount = 0;
		ProbePathPointList = gcnew  System::Collections::Generic::List<RWrapper::ProbePathPoint^>();
		double ApproachDist = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;
		int XAxisMult = -1;
		if(LeftProbe) XAxisMult = 1;
		RWrapper::ProbePathPoint^ ProbePath_LstEntity;
		ProbePath_LstEntity = gcnew ProbePathPoint();
		ProbePath_LstEntity->Pt_X =  StartPoint[0];
		if(LeftProbe)
			ProbePath_LstEntity->Pt_Y =  StartPoint[1] + SphereRadius + 5;
		else
			ProbePath_LstEntity->Pt_Y =  StartPoint[1] - SphereRadius - 5;
		ProbePath_LstEntity->Pt_Z =  StartPoint[2] + 5;
		ProbePath_LstEntity->path = true;
		ProbePathPointList->Add(ProbePath_LstEntity);

		for(int i = 0; i < 3; i++)
		{			
			int MultFactor = 1;
			double denominator = 2;
			double RadMulFactor = System::Math::Pow(4.0/3, 0.5);
			if(i == 1)
			{
				RadMulFactor = 1;
				MultFactor = -1;
				denominator = 1;
			}
			else if(i == 2)
				denominator = 2.0 / 3.0;	
			if(!LeftProbe)
				MultFactor *= -1;
			for (int j = 0; j < 8; j++)
			{
				double tempTarget[4] = { StartPoint[0], StartPoint[1], StartPoint[2] - SphereRadius / denominator, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] };
				ProbePath_LstEntity = gcnew ProbePathPoint();
				if (j == 2 || j == 3 || j == 5)
					tempTarget[0] = StartPoint[0] + XAxisMult * (SphereRadius + 5);
				else if (j == 4) //|| j == 1 || j == 7)
				{
					tempTarget[0] = StartPoint[0];// + XAxisMult * (SphereRadius / RadMulFactor);
					//tempTarget[0] = StartPoint[0] + XAxisMult * ((SphereRadius / RadMulFactor) - ApproachDist);
					if (LeftProbe)
						ProbePath_LstEntity->ProbeDir = 0;
					else
						ProbePath_LstEntity->ProbeDir = 1;
				}
				if (j == 0 || j == 2)
				{
					tempTarget[1] = StartPoint[1] + MultFactor * (SphereRadius + 5);
				}
				else if (j == 1)
				{
					tempTarget[1] = StartPoint[1];// + MultFactor * (SphereRadius / RadMulFactor);
					//tempTarget[1] = StartPoint[1] + MultFactor * ((SphereRadius / RadMulFactor) - ApproachDist);
					if (MultFactor == 1)
						ProbePath_LstEntity->ProbeDir = 2;
					else
						ProbePath_LstEntity->ProbeDir = 3;
				}
				else if (j == 7)
				{
					tempTarget[1] = StartPoint[1];// - MultFactor * (SphereRadius / RadMulFactor);
					//tempTarget[1] = StartPoint[1] - MultFactor * ((SphereRadius / RadMulFactor) - ApproachDist);
					if (MultFactor == 1)
						ProbePath_LstEntity->ProbeDir = 3;
					else
						ProbePath_LstEntity->ProbeDir = 2;
				}
				else if (j == 5 || j == 6)
					tempTarget[1] = StartPoint[1] - MultFactor * (SphereRadius + 5);

				ProbePath_LstEntity->Pt_X =  tempTarget[0];
				ProbePath_LstEntity->Pt_Y =  tempTarget[1];
				ProbePath_LstEntity->Pt_Z =  tempTarget[2];

				if(j == 1 || j == 4 || j == 7)
					ProbePath_LstEntity->path = false;
				else
					ProbePath_LstEntity->path = true;
				ProbePathPointList->Add(ProbePath_LstEntity);
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSphCal0007", ex); }
}

void RWrapper::RW_SphereCalibration::CreatePointsListForFBProbe(bool FrontProbe)
{
	try
	{
		Listcount = 0;
		ProbePathPointList = gcnew  System::Collections::Generic::List<RWrapper::ProbePathPoint^>();
		double ApproachDist = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;
		int YAxisMult = -1;
		if(FrontProbe) YAxisMult = 1;
		RWrapper::ProbePathPoint^ ProbePath_LstEntity;// = gcnew ProbePathPoint()
	/*	ProbePath_LstEntity->Pt_X =  StartPoint[0];
		ProbePath_LstEntity->Pt_Y =  StartPoint[1];
		ProbePath_LstEntity->Pt_Z =  StartPoint[2] + 5;
		ProbePath_LstEntity->path = true;
		ProbePathPointList->Add(ProbePath_LstEntity);

		ProbePath_LstEntity = gcnew ProbePathPoint();
		ProbePath_LstEntity->Pt_X =  StartPoint[0];
		ProbePath_LstEntity->Pt_Y =  StartPoint[1];
		ProbePath_LstEntity->Pt_Z =  StartPoint[2];
		ProbePath_LstEntity->path = false;
		ProbePathPointList->Add(ProbePath_LstEntity);*/

		ProbePath_LstEntity = gcnew ProbePathPoint();
		if (FrontProbe)
			ProbePath_LstEntity->Pt_X = StartPoint[0] - SphereRadius -5;
		else
			ProbePath_LstEntity->Pt_X = StartPoint[0] + SphereRadius +5;
		ProbePath_LstEntity->Pt_Y =  StartPoint[1];
		ProbePath_LstEntity->Pt_Z = StartPoint[2] +5;
		ProbePath_LstEntity->path = true;
		ProbePathPointList->Add(ProbePath_LstEntity);

		for(int i = 0; i < 3; i++)
		{
			int MultFactor = 1;
			double RadMulFactor = System::Math::Pow(4.0/3, 0.5);
			double denominator = 2;
			if(i == 1)
			{
				RadMulFactor = 1;
				MultFactor = -1;
				denominator = 1;
			}
			else if(i == 2)
				denominator = 2.0/3.0;	
			if(!FrontProbe)
				MultFactor *= -1;
			for(int j = 0; j < 8; j++)
			{
				double tempTarget[4] = {StartPoint[0], StartPoint[1], StartPoint[2] - SphereRadius / denominator, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
				ProbePath_LstEntity = gcnew ProbePathPoint();
				if(j == 2 || j == 3 || j == 5)
					tempTarget[1] = StartPoint[1] + YAxisMult * (SphereRadius + 5);
				else if (j == 4)
				{
					tempTarget[1] = StartPoint[1];// + YAxisMult * (SphereRadius / RadMulFactor);
					if (FrontProbe)
						ProbePath_LstEntity->ProbeDir = 2;
					else
						ProbePath_LstEntity->ProbeDir = 3;
				}
				if(j == 0 || j == 2)
					tempTarget[0] = StartPoint[0] - MultFactor * (SphereRadius + 5);
				else if (j == 1)
				{
					tempTarget[0] = StartPoint[0];// - MultFactor * (SphereRadius / RadMulFactor);
					if (MultFactor == 1)
						ProbePath_LstEntity->ProbeDir = 1;
					else
						ProbePath_LstEntity->ProbeDir = 0;
				}
				else if (j == 7)
				{
					tempTarget[0] = StartPoint[0];// + MultFactor * (SphereRadius / RadMulFactor);
					if (MultFactor == 1)
						ProbePath_LstEntity->ProbeDir = 0;
					else
						ProbePath_LstEntity->ProbeDir = 1;
				}
				else if(j == 5 || j == 6)
					tempTarget[0] = StartPoint[0] + MultFactor * (SphereRadius + 5);
				ProbePath_LstEntity->Pt_X =  tempTarget[0];
				ProbePath_LstEntity->Pt_Y =  tempTarget[1];
				ProbePath_LstEntity->Pt_Z =  tempTarget[2];
				if(j == 1 || j == 4 || j == 7)
					ProbePath_LstEntity->path = false;
				else
					ProbePath_LstEntity->path = true;
				ProbePathPointList->Add(ProbePath_LstEntity);
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSphCal0007", ex); }
}

void RWrapper::RW_SphereCalibration::CreatePointsListForLRProbeCircular(bool LeftProbe)
{
	try
	{
		Listcount = 0;
		ProbePathPointList = gcnew  System::Collections::Generic::List<RWrapper::ProbePathPoint^>();
		double ApproachDist = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;
		int XAxisMult = -1;
		if(LeftProbe) XAxisMult = 1;
		RWrapper::ProbePathPoint^ ProbePath_LstEntity;
	
		for(int i = 0; i < 3; i++)
		{			
			int MultFactor = 1;
			double denominator = 2;
			double RadMulFactor = System::Math::Pow(4.0/3, 0.5);
			if(i == 1)
			{
				RadMulFactor = 1;
				MultFactor = -1;
				denominator = 1;
			}
			else if(i == 2)
				denominator = 2.0 / 3.0;	
			if(!LeftProbe)
				MultFactor *= -1;
			for(int j = 0; j < 6; j++)
			{
				double tempTarget[4] = {StartPoint[0], StartPoint[1], StartPoint[2] - SphereRadius / denominator, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
				if(j == 0)
					tempTarget[1] = StartPoint[1] + MultFactor * (SphereRadius + 5);
				else if(j == 1)
					tempTarget[1] = StartPoint[1] + MultFactor * (SphereRadius / RadMulFactor);
				else if(j == 2)
					tempTarget[0] = StartPoint[0] + XAxisMult * (SphereRadius + 5);
				else if(j == 3)
					tempTarget[0] = StartPoint[0] + XAxisMult * (SphereRadius / RadMulFactor);	
				else if(j == 4)
					tempTarget[1] = StartPoint[1] - MultFactor * (SphereRadius + 5);
				else if(j == 5)
					tempTarget[1] = StartPoint[1] - MultFactor * (SphereRadius / RadMulFactor);
				
				ProbePath_LstEntity = gcnew ProbePathPoint();
				ProbePath_LstEntity->Pt_X =  tempTarget[0];
				ProbePath_LstEntity->Pt_Y =  tempTarget[1];
				ProbePath_LstEntity->Pt_Z =  tempTarget[2];

				if(j == 1 || j == 3 || j == 5)
				{
					ProbePath_LstEntity->path = false;
			    	ProbePath_LstEntity->circularPath = false;
				}
				else
				{
			     	ProbePath_LstEntity->circularPath = true;
					ProbePath_LstEntity->path = true;	
					if(j == 0)
						ProbePath_LstEntity->changeZ = false;
					else
						ProbePath_LstEntity->changeZ = true;
				}
				ProbePathPointList->Add(ProbePath_LstEntity);
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSphCal0007", ex); }
}

void RWrapper::RW_SphereCalibration::CreatePointsListForFBProbeCircular(bool FrontProbe)
{
	try
	{
		Listcount = 0;
		ProbePathPointList = gcnew  System::Collections::Generic::List<RWrapper::ProbePathPoint^>();
		double ApproachDist = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;
		int YAxisMult = -1;
		if(FrontProbe) YAxisMult = 1;
		RWrapper::ProbePathPoint^ ProbePath_LstEntity;
		
		for(int i = 0; i < 3; i++)
		{
			int MultFactor = 1;
			double RadMulFactor = System::Math::Pow(4.0/3, 0.5);
			double denominator = 2;
			if(i == 1)
			{
				RadMulFactor = 1;
				MultFactor = -1;
				denominator = 1;
			}
			else if(i == 2)
				denominator = 2.0/3.0;	
			if(!FrontProbe)
				MultFactor *= -1;
			for(int j = 0; j < 6; j++)
			{
				double tempTarget[4] = {StartPoint[0], StartPoint[1], StartPoint[2] - SphereRadius / denominator, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
	     		if(j == 0)
					tempTarget[0] = StartPoint[0] - MultFactor * (SphereRadius + 5);
				else if(j == 1)
					tempTarget[0] = StartPoint[0] - MultFactor * (SphereRadius / RadMulFactor);
				else if(j == 2)
					tempTarget[1] = StartPoint[1] + YAxisMult * (SphereRadius + 5);
				else if(j == 3)
					tempTarget[1] = StartPoint[1] + YAxisMult * (SphereRadius / RadMulFactor);	
				else if(j == 4)
					tempTarget[0] = StartPoint[0] + MultFactor * (SphereRadius + 5);
				else if(j == 5)
					tempTarget[0] = StartPoint[0] + MultFactor * (SphereRadius / RadMulFactor);

				ProbePath_LstEntity = gcnew ProbePathPoint();
				ProbePath_LstEntity->Pt_X =  tempTarget[0];
				ProbePath_LstEntity->Pt_Y =  tempTarget[1];
				ProbePath_LstEntity->Pt_Z =  tempTarget[2];

				if(j == 1 || j == 3 || j == 5)
				{
					ProbePath_LstEntity->path = false;
			    	ProbePath_LstEntity->circularPath = false;
					ProbePath_LstEntity->changeZ = false;
				}
				else
				{
					if(j == 0)
						ProbePath_LstEntity->changeZ = false;
					else
						ProbePath_LstEntity->changeZ = true;
			     	ProbePath_LstEntity->circularPath = true;
					ProbePath_LstEntity->path = true;					
				}
				ProbePathPointList->Add(ProbePath_LstEntity);
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSphCal0007", ex); }
}

void RWrapper::RW_SphereCalibration::CreatePointsListForNormalProbe()
{
	try
	{
		Listcount = 0;
		ProbePathPointList = gcnew  System::Collections::Generic::List<RWrapper::ProbePathPoint^>();
		double ApproachDist = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;
		RWrapper::ProbePathPoint^ ProbePath_LstEntity= gcnew ProbePathPoint();
		double RadMulFactor = System::Math::Pow(4.0/3, 0.5);

		//ProbePath_LstEntity->Pt_X =  StartPoint[0];
		//ProbePath_LstEntity->Pt_Y =  StartPoint[1];
		//ProbePath_LstEntity->Pt_Z =  StartPoint[2] + 4;
		//ProbePath_LstEntity->path = true;
		//ProbePathPointList->Add(ProbePath_LstEntity);

		ProbePath_LstEntity = gcnew ProbePathPoint();
		ProbePath_LstEntity->Pt_X =  StartPoint[0];
		ProbePath_LstEntity->Pt_Y =  StartPoint[1];
		ProbePath_LstEntity->Pt_Z = StartPoint[2] - SphereRadius;
		ProbePath_LstEntity->ProbeDir = 5; // Always hit vertically down.		
		ProbePath_LstEntity->path = false;
		ProbePathPointList->Add(ProbePath_LstEntity);
		
		for(int i = 0; i < 2; i++)
		{
			double RadMulFactor = System::Math::Pow(4/3, 0.5);
			double denominator = 2;
			if(i == 1) { denominator = 1; }
			for(int j = 0; j < 12; j++)
			{
				double tempTarget[4] = {StartPoint[0], StartPoint[1], StartPoint[2] - SphereRadius / denominator - UserDefinedProbeRadius, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
				ProbePath_LstEntity = gcnew ProbePathPoint();
				if(j == 0)
				{
					if( i == 0 )
				      tempTarget[2] = StartPoint[2] + 4;
					else
					{
						tempTarget[1] = StartPoint[1] - SphereRadius - 4;
						tempTarget[2] = StartPoint[2] - SphereRadius / 2 - UserDefinedProbeRadius;
					}
					tempTarget[0] = StartPoint[0] + SphereRadius + 4;
				}
				else if (j == 1)
				{
					tempTarget[0] = StartPoint[0] + SphereRadius + 4;
				}
				else if (j == 3)
				{
					tempTarget[0] = StartPoint[0] + SphereRadius + 4;
				    tempTarget[1] = StartPoint[1] + SphereRadius + 4;
				}
				else if(j == 4)
					tempTarget[1] = StartPoint[1] + SphereRadius + 4;
				else if(j == 6)
				{
					tempTarget[1] = StartPoint[1] + SphereRadius + 4;
					tempTarget[0] = StartPoint[0] - SphereRadius - 4;
				}
				else if(j == 7)
					tempTarget[0] = StartPoint[0] - SphereRadius - 4;
				else if(j == 9)
				{
					tempTarget[0] = StartPoint[0] - SphereRadius - 4;	
					tempTarget[1] = StartPoint[1] - SphereRadius - 4;
				}
				else if(j == 10)
					tempTarget[1] = StartPoint[1] - SphereRadius - 4;
				else if (j == 2)
				{
					tempTarget[0] = StartPoint[0];// + SphereRadius / RadMulFactor;
					ProbePath_LstEntity->ProbeDir = 0; 		
				}
				else if (j == 5)
				{
					tempTarget[1] = StartPoint[1];// + SphereRadius / RadMulFactor;
					ProbePath_LstEntity->ProbeDir = 2; 		
				}
				else if (j == 8)
				{
					tempTarget[0] = StartPoint[0];// - SphereRadius / RadMulFactor;
					ProbePath_LstEntity->ProbeDir = 1; 		
				}
				else if (j == 11)
				{
					tempTarget[1] = StartPoint[1];// - SphereRadius / RadMulFactor;
					ProbePath_LstEntity->ProbeDir = 3; 		
				}
				ProbePath_LstEntity->Pt_X =  tempTarget[0];
				ProbePath_LstEntity->Pt_Y =  tempTarget[1];
				ProbePath_LstEntity->Pt_Z =  tempTarget[2];

				if(j == 2 || j == 5 || j == 8 || j == 11)
				{
					ProbePath_LstEntity->path = false;
			    	ProbePath_LstEntity->circularPath = false;
					ProbePath_LstEntity->changeZ = false;
				}
				else
					ProbePath_LstEntity->path = true;					
				ProbePathPointList->Add(ProbePath_LstEntity);
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSphCal0007", ex); }
}

void RWrapper::RW_SphereCalibration::CreatePointsListForNormalProbeCircular()
{
	try
	{
		Listcount = 0;
		ProbePathPointList = gcnew  System::Collections::Generic::List<RWrapper::ProbePathPoint^>();
		double ApproachDist = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;
		RWrapper::ProbePathPoint^ ProbePath_LstEntity= gcnew ProbePathPoint();
		double RadMulFactor = System::Math::Pow(4.0/3, 0.5);

		ProbePath_LstEntity->Pt_X =  StartPoint[0];
		ProbePath_LstEntity->Pt_Y =  StartPoint[1];
		ProbePath_LstEntity->Pt_Z =  StartPoint[2] + 5;
		ProbePath_LstEntity->path = true;
		ProbePathPointList->Add(ProbePath_LstEntity);

		ProbePath_LstEntity = gcnew ProbePathPoint();
		ProbePath_LstEntity->Pt_X =  StartPoint[0];
		ProbePath_LstEntity->Pt_Y =  StartPoint[1];
		ProbePath_LstEntity->Pt_Z =  StartPoint[2];
		ProbePath_LstEntity->path = false;
		ProbePathPointList->Add(ProbePath_LstEntity);
		
		for(int i = 0; i < 2; i++)
		{
			double RadMulFactor = System::Math::Pow(4/3, 0.5);
			double denominator = 2;
			if(i == 1) { denominator = 1; }
			for(int j = 0; j < 8; j++)
			{
				double tempTarget[4] = {StartPoint[0], StartPoint[1], StartPoint[2] - SphereRadius / denominator, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
	     	   	if(j == 0)
					tempTarget[0] = StartPoint[0] + SphereRadius + 5;				
				else if(j == 2)
					tempTarget[1] = StartPoint[1] + SphereRadius + 5;
				else if(j == 4)
					tempTarget[0] = StartPoint[0] - SphereRadius - 5;	
				else if(j == 6)
					tempTarget[1] = StartPoint[1] - SphereRadius - 5;
				else if(j == 1 )
				    tempTarget[0] = StartPoint[0] + SphereRadius / RadMulFactor;
				else if(j == 3 )
				    tempTarget[1] = StartPoint[1] + SphereRadius / RadMulFactor;
				else if(j == 5 )
				    tempTarget[0] = StartPoint[0] - SphereRadius / RadMulFactor;
				else if(j == 7 )
				    tempTarget[1] = StartPoint[1] - SphereRadius / RadMulFactor;
				
				ProbePath_LstEntity = gcnew ProbePathPoint();
				ProbePath_LstEntity->Pt_X =  tempTarget[0];
				ProbePath_LstEntity->Pt_Y =  tempTarget[1];
				ProbePath_LstEntity->Pt_Z =  tempTarget[2];

				if(j == 1 || j == 3 || j == 5 || j == 7)
				{
					ProbePath_LstEntity->path = false;
			    	ProbePath_LstEntity->circularPath = false;
					ProbePath_LstEntity->changeZ = false;
				}
				else
				{
					if(j == 0)
						ProbePath_LstEntity->changeZ = false;
					else
						ProbePath_LstEntity->changeZ = true;
			     	ProbePath_LstEntity->circularPath = true;
					ProbePath_LstEntity->path = true;					
				}
				ProbePathPointList->Add(ProbePath_LstEntity);
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSphCal0007", ex); }
}

void RWrapper::RW_SphereCalibration::SendDrotoNextposition()
{
	try
	{
		if(ProbePathPointList->Count < 1) return;
		double ApproachDist = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue * 2;
		if(Listcount == ProbePathPointList->Count)
		{				
			TakeManualPoint = true;
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
			if(RefSphereCalibrate)
			{
				RefSphereCalibrate = false;
				RaisePointsCountEvent();
				//PointTakenCount++;
				//return;
			}
			else
			{
				CalibrateCurrentSide();
				PointTakenCount = 0;
			}
			PointTakenCount = 0;

			if(InspectionType == 0)
			{
				double offset[3] = { 0 };
				
				if (CurrentSide > 1)
				{
					offset[0] = CalibratedOffset[3 * (CurrentSide - 2)];
					offset[1] = CalibratedOffset[3 * (CurrentSide - 2) + 1];
					offset[2] = CalibratedOffset[3 * (CurrentSide - 2) + 2];
					RWrapper::RW_DBSettings::MYINSTANCE()->UpdateSphereCalibrationSettings_CurrentMachine(CurrentProbetype, CurrentSide, ProbeRadius[CurrentSide - 1] * 2, offset);
				}
				else
					RWrapper::RW_DBSettings::MYINSTANCE()->UpdateSphereCalibrationSettings_CurrentMachine(CurrentProbetype, CurrentSide, ProbeRadius[0] * 2, offset);
			}
			//CurrentSide++;
			//if(CurrentSide <= LastSide)
			//{
			//	//if(MAINDllOBJECT->ShowMsgBoxString("RW_SphereCalibration01", RapidEnums::MSGBOXTYPE::MSG_OK_CANCEL, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION))
			//	//{
					TakeProbeHit = true;
			//	//	RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::SPHERECALIBRATIONMANUALPOINT);
			//	//}
			//	//else
			//	//	ClearAll();
			//}
			//else
					//ClearAll();
		//PointTakenCount++;
		//	RaisePointsCountEvent();
					FinishedSphereEvent::raise();
			return;
		}
		double target[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		RWrapper::ProbePathPoint^ ProbePath_LstEntity = gcnew ProbePathPoint();
		ProbePath_LstEntity = ProbePathPointList[Listcount++];
		target[0] = ProbePath_LstEntity->Pt_X; target[1] = ProbePath_LstEntity->Pt_Y; target[2] = ProbePath_LstEntity->Pt_Z;		
		//if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 2) // && Listcount == 1)
		//	probedirection = ProbePath_LstEntity->ProbeDir;
		//else
			probedirection = getMovementdirection(target);
		CurrentTargetType = TargetReachedCallback::SPHERECALLIBRATION;
		if(!ProbePath_LstEntity->path)
		{
			//if(RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 1)
			//{
			//	switch(probedirection)
			//	{
			//		case 1:
			//			target[0] += ApproachDist;
			//			break;
			//		case 3:
			//			target[0] -= ApproachDist;
			//			break;
			//		case 5:
			//			target[1] += ApproachDist;
			//			break;
			//		case 7:
			//			target[1] -= ApproachDist;
			//			break;
			//		case 9:
			//			target[2] += ApproachDist;
			//			break;
			//		case 11:
			//			target[2] -= ApproachDist;
			//			break;
			//	}
			//}
			//else
			//{
			//	switch(probedirection)
			//	{
			//		case 1:
			//			target[0] -= ApproachDist;
			//			break;
			//		case 3:
			//			target[0] += ApproachDist;
			//			break;
			//		case 5:
			//			target[1] -= ApproachDist;
			//			break;
			//		case 7:
			//			target[1] += ApproachDist;
			//			break;
			//		case 9:
			//			target[2] -= ApproachDist;
			//			break;
			//		case 11:
			//			target[2] += ApproachDist;
			//			break;
			//	}
			//}
			DROInstance->SetProbeDirection(probedirection);
		}
		for(int i = 0; i < 4; i++)
		{
			Target[i] = target[i];
			Feedrate[i] = feedrate[i];
		}

		if(!ProbePath_LstEntity->path)
		{
			if(RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag > 0)
			{
				TakeProbeHit = true;
				RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = true;
			}
			else
				CurrentTargetType = TargetReachedCallback::SPHERECALLIBRATION_POINT;
		}
		else
			RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = false;
		
		if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
		if(ProbePath_LstEntity->circularPath)
		{
			double CirclePts[9] = {0}, circleParam[7] = {0}, dir[3] = {0};
			CirclePts[0] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0];
			CirclePts[1] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1];
			CirclePts[2] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
			CirclePts[6] = target[0];
			CirclePts[7] = target[1];
			CirclePts[8] = target[2];
			double cCenters[6] = {(CirclePts[0] + target[0]) / 2, (CirclePts[1] + target[1]) / 2, (CirclePts[2] + target[2]) / 2, StartPoint[0], StartPoint[1], StartPoint[2] - SphereRadius}; 
	        RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(CurrentTargetType);
			RMATH3DOBJECT->DCs_TwoEndPointsofLine(cCenters, dir);
			double rad = RMATH3DOBJECT->Distance_Point_Point(CirclePts, &cCenters[3]);
			for(int i = 0; i < 3; i++)
			{
			   CirclePts[3 + i] = cCenters[3 + i] + (SphereRadius + 5) * dir[i];
			}
			if(ProbePath_LstEntity->changeZ)
		    	CirclePts[5] = cCenters[2];
			RMATH3DOBJECT->Circle_3Pt_3D(CirclePts, circleParam);
			double dir1[3] = {CirclePts[0] - circleParam[0], CirclePts[1] - circleParam[1], CirclePts[2] - circleParam[2]}, dir2[3] = {CirclePts[6] - circleParam[0], CirclePts[7] - circleParam[1], CirclePts[8] - circleParam[2]};
			double ang = abs(RMATH3DOBJECT->Angle_Btwn_2Directions(dir1, dir2 , true, false));
			DROInstance->GotoCommandWithCircularInterpolation(&circleParam[0], ang, feedrate[0], 20);
		}
		else
		{
		    RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], CurrentTargetType);
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSphCal0007", ex); }
}

//int RWrapper::RW_SphereCalibration::getMovementdirection(double* targetPos)
//{
//	try
//	{
//		double target[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
//		double Xdist = targetPos[0] - target[0], Ydist = targetPos[1] - target[1], Zdist = targetPos[2] - target[2];
//		double dist = abs(Xdist);
//		int direction = 1;
//		if(Xdist < 0)
//			direction = 3;
//		if(abs(Ydist) > dist)
//		{
//			dist = abs(Ydist);
//			if(Ydist < 0)
//				direction = 7;
//			else
//				direction = 5;
//		}
//		if(abs(Zdist) > dist)
//		{
//			if(Zdist < 0)
//				direction = 11;
//			else
//				direction = 9;
//		}
//		return direction;
//	}
//	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSphCal0007", ex);return 1; }
//}

int RWrapper::RW_SphereCalibration::getMovementdirection(double* targetPos)
{
	try
	{
		double target[4] = { RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] };
		if (Listcount > 1)
		{
			RWrapper::ProbePathPoint^ ProbePath_LstEntity = gcnew ProbePathPoint();
			ProbePath_LstEntity = ProbePathPointList[Listcount - 2];
			target[0] = ProbePath_LstEntity->Pt_X; target[1] = ProbePath_LstEntity->Pt_Y; target[2] = ProbePath_LstEntity->Pt_Z; // , target[3] = ProbePath_LstEntity->Pt_R;
		}
		double Xdist = targetPos[0] - target[0], Ydist = targetPos[1] - target[1], Zdist = targetPos[2] - target[2], Rdist = targetPos[3] - target[3];
		double dist = abs(Xdist);
		int direction = 1;
		if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag < 2)
		{
			if (Xdist < 0)
				direction = 3;
			if (abs(Ydist) > dist)
			{
				dist = abs(Ydist);
				if (Ydist < 0)
					direction = 7;
				else
					direction = 5;
			}
			if (abs(Zdist) > dist)
			{
				if (Zdist < 0)
					direction = 11;
				else
					direction = 9;
			}
			//if (abs(Rdist) > dist)
			//{
			//	if (Rdist < 0)
			//		direction = 15;
			//	else
			//		direction = 13;
			//}
		}
		else
		{
			direction = 1;
			if (Xdist < 0)
				direction = 0;
			if (abs(Ydist) > dist)
			{
				dist = abs(Ydist);
				if (Ydist < 0)
					direction = 3;
				else
					direction = 2;
			}
			if (abs(Zdist) > dist)
			{
				if (Zdist < 0)
					direction = 5;
				else
					direction = 4;
			}
			if (abs(Rdist) > 0.002)
			{
				if (Rdist < 0)
					direction = 7;
				else
					direction = 8;
			}
		}
		return direction;
	}
	catch (Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule005", ex); return 1; }
}


void RWrapper::RW_SphereCalibration::SendDrotoTakeNextPoint()
{
	try
	{
		double ApproachDist = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;
		double target[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		CurrentTargetType = TargetReachedCallback::SPHERECALLIBRATION;
		switch(probedirection)
		{
		case 1:
			target[0] += (2 * ApproachDist);
			break;
		case 3:
			target[0] -= (2 * ApproachDist);
			break;
		case 5:
			target[1] += (2 * ApproachDist);
			break;
		case 7:
			target[1] -= (2 * ApproachDist);
			break;
		case 9:
			target[2] += (2 * ApproachDist);
			break;
		case 11:
			target[2] -= (2 * ApproachDist);
			break;
		}
		for(int i = 0; i < 4; i++)
		{
			Target[i] = target[i];
			Feedrate[i] = feedrate[i];
		}
		TakeProbeHit = true;
		RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = true;
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], CurrentTargetType);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSphCal0007", ex);}
}

void RWrapper::RW_SphereCalibration::StartSphereCallibration_WithStylus(double Sphere_Radius)
{
	try
	{
		ShapeWithList* CShape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		if(CShape == NULL) return;
		if(CShape->ShapeType != RapidEnums::SHAPETYPE::SPHERE) return;
		
		this->StepsCount = 0;
		SphereRadius = Sphere_Radius;
		if(MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::HEIGHT_GAUGE) return;
		double CurrentZ = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] + RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
		ReferenceCenter[0] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0]; ReferenceCenter[1] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1]; 
		ReferenceCenter[2] = CurrentZ - Sphere_Radius;

		((Sphere*)CShape)->Radius(SphereRadius);
		((Sphere*)CShape)->setCenter (Vector(ReferenceCenter[0], ReferenceCenter[1], ReferenceCenter[2]));

		double target[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0] + System::Math::Pow(7.0/16, 0.5) * Sphere_Radius * 0.9, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] - Sphere_Radius / 4, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
		StepsCount++;
		RWrapper::RW_CircularInterPolation::MYINSTANCE()->AddPointsFlag = true;
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::STYLUS_SPHERECALLIBRATION);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSphCal0008", ex);}
}

void RWrapper::RW_SphereCalibration::GetPointsInCircularPath()
{
	try
	{
		if(StepsCount >= 4)
		{
			RWrapper::RW_CircularInterPolation::MYINSTANCE()->StopAddingPtstoSelectedShapes(true);
			return;
		}
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		if(StepsCount == 2)
		{
			double target[4] = {ReferenceCenter[0] + System::Math::Pow(3.0/4, 0.5) * SphereRadius * 0.8, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] - SphereRadius / 4, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
			if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
			StepsCount++;
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::STYLUS_SPHERECALLIBRATION);
		}
		else 
		{
			double CircleParam[7] = {ReferenceCenter[0], ReferenceCenter[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], 0, 0, 1, SphereRadius}, SweepAngle = M_PI * 2;
			int NumberOfHops = 50;
			if(StepsCount == 1)
				CircleParam[6] = System::Math::Pow(7.0/16, 0.5) * SphereRadius * 0.9;
			else 
				CircleParam[6] = System::Math::Pow(3.0/4, 0.5) * SphereRadius * 0.8;
			if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
			RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::STYLUS_SPHERECALLIBRATION);
			StepsCount++;
			DROInstance->GotoCommandWithCircularInterpolation(CircleParam, SweepAngle, feedrate[0], NumberOfHops);
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSphCal0009", ex);}
}

void RWrapper::RW_SphereCalibration::ChangeHandlerToVision()
{
	RWrapper::RW_MainInterface::MYINSTANCE()->HandleDerivedShapeToolbar_Click("Sphere Vision Calibration");
}

void RWrapper::RW_SphereCalibration::GotoFocusPosition(double x, double y, double z)
{
	try
	{

		//RWrapper::RW_FocusDepth::MYINSTANCE()->SetFocusRectangleProperties(1, 1, 40, 40, 40, 1, 0.2);
		RWrapper::RW_FocusDepth::MYINSTANCE()->SetFocusRectangleProperties(1, 1, 40, 40, 40, RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpan, RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpeed);
		double target[4] = { x, y, z + SphereRadius, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] };
		CurrentGotoPosition[0] = x;
		CurrentGotoPosition[1] = y;
		CurrentGotoPosition[2] = z;
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
	    FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::PROBEVISIONOFFSETFOCUS;
		CurrentTargetType = TargetReachedCallback::FOCUSDEPTH_AFSMODEPOSITION;	
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], CurrentTargetType);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSphCal0009b", ex);}
}

void RWrapper::RW_SphereCalibration::DoVisionCalibration(double z)
{
	try
	{
		cli::array<double>^ Values = gcnew cli::array<double>(4);
		const char* temporarychar = (const char*)(Marshal::StringToHGlobalAnsi(RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath)).ToPointer();
		std::string FilePath = temporarychar;
		FilePath = FilePath + "\\Database";
		wofstream ofile;
		ofile.open(FilePath + "\\SphereVisionCalibration.txt", ios::app);
		Values[0] = CurrentGotoPosition[0] - ReferenceCenter[0];
		Values[1] = CurrentGotoPosition[1] - ReferenceCenter[1];
		Values[2] = z - ReferenceCenter[2];
		ofile << "Offset " << Values[0] << ", " << Values[1] << ", " << Values[2] << endl;
		ofile.close();
		
		ProbeOffsetEvent::raise(Values, 1);
	}
	catch(Exception^ ex)
	{
	   RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSphCal0009c", ex);
	}
}

void RWrapper::RW_SphereCalibration::RaisePointsCountEvent()
{
	try
	{
		PointsCountEvent::raise(PointTakenCount + 1, 10);
	}
	catch(Exception^ ex)
	{
	   RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSphCal0010", ex);
	}
}

void RWrapper::RW_SphereCalibration::AddPointsToSelectedShape(int dir, double *pnt)
{
	try
	{
		PointCollection ptColl;
		ptColl.Addpoint(new SinglePoint(pnt[0], pnt[1], pnt[2], dir, CurrentSide));
		Shape* CShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		if(CShape != NULL)
		{
			((ShapeWithList*)CShape)->AddPoints(&ptColl);
		}
	}
	catch(Exception^ ex)
	{
	   RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWSphCal0011", ex);
	}
}

void RWrapper::RW_SphereCalibration::SetCurrentSide(int currSide)
{
	CurrentSide = currSide;
}

//void RWrapper::RW_SphereCalibration::UpdateProbeValuestoDB(int ProbeType)
//{
//	switch (ProbeType)
//	{
//	case 1: //Normal Probe
//
//	case 2: //T-Probe Along X
//
//	case 3: //T-Probe ALong Y
//
//	case 4: //Star Probe
//
//
//	default:
//		break;
//	}
//}
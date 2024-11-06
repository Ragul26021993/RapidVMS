#include "Stdafx.h"
#include "RW_MainInterface.h"
#include "RW_DelphiCamModule.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "..\MAINDLL\Helper\Helper.h"
#include "..\MAINDLL\Shapes\RPoint.h"
#include "..\MAINDLL\Shapes\Line.h"
#include "..\MAINDLL\Shapes\Circle.h"
#include "..\MAINDLL\Actions\AddShapeAction.h"
#include "..\MAINDLL\FrameGrab\FrameGrabBase.h"
#include <iostream>
#include <fstream> 
#include <msclr/marshal_cppstd.h>

double circleCenter[3] = {0};
double _slope = 0.0, _intercept = 0.0, _radius = 0.0, cir_sec_angle = 0.0, ZPos = 0.0;
bool FirstStep, MasterCylinderFirstStep, FinishingRunoutCalculations;
bool Doing_Y_Check, Zeroing_in_C_Axis;
int StartToothStepNumber, BC_Center_C_Dir, BC_CentrePointNumber, HobMeasurementsDoneCount;

RWrapper::RW_DelphiCamModule::RW_DelphiCamModule()
{
	 try
	 {
		 DoingCalibration = false;
		 ManualPoint = false;
		 Getting_BC_Axis = false; GotAxis = false;
		 Listcount = 0;
		 DelphiCamModule = this;
		 ReferenceCenter = gcnew cli::array<double>(4);
		 BC_Values = gcnew cli::array<double>(7);
		 InnerCirclePoint = gcnew cli::array<double>(4);
		 LastTargetValue = gcnew cli::array<double>(4);
		 HobParameters = gcnew cli::array<double>(25);
		 ToothMidPt = gcnew cli::array<double>(4);
		 HelicalGashParams = gcnew cli::array<double>(4);
		 HobSettings = gcnew cli::array<double>(10);

		 Hob_RunoutParams = gcnew cli::array<double>(6);
		 //Hob_RunoutParams = (double*)malloc(7 * sizeof(double)); // gcnew cli::array<double>(7);
		 FinishingRunoutCalculations = false;

		 firsttime = true;
		 lastPoint = NULL;
		 DoingHobMeasurement = false;
		 HobMeasureisComplete = false;
		 HobParametersUpdated = false;
		 for (int i = 0; i < 4; i ++) ReferenceCenter[i] = 0;
		 for (int i = 0; i < HobParameters->Length; i ++) HobParameters[i] = 0;
		 for (int i = 0; i < 7; i ++) BC_Values[i] = 0;
		 for (int i = 0; i < 6; i ++) Hob_RunoutParams[i] = 0;
		 InPauseState = false;
		 instantiated = true;
	 }
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule001", ex); }
}

RWrapper::RW_DelphiCamModule::~RW_DelphiCamModule()
{
	//free(Hob_RunoutParams);
}

RWrapper::RW_DelphiCamModule^ RWrapper::RW_DelphiCamModule::MYINSTANCE()
{
	return DelphiCamModule;
}

//void RWrapper::RW_DelphiCamModule::Fill__Circle_Centre(double x_coord, double y_coord, double z_coord, double slope_, double intercept_, double radius_, double sector_angle)
//{
//	circleCenter[0]=x_coord;															// for next fn.RW_DelphiCamModule, we receive circle,ray,sector_angle parameters here..
//	circleCenter[1]=y_coord;
//	circleCenter[2]=z_coord;
//	_slope=slope_; 
//	_intercept=intercept_;
//	_radius=radius_;
//	cir_sec_angle=sector_angle;
//}

void RWrapper::RW_DelphiCamModule::Fill__Circle_Centre(double x_coord, double y_coord, double z_coord, double slope_, double intercept_, double radius_, double sector_angle, double ZPosition)
{
	circleCenter[0] = x_coord;															// for next fn.RW_DelphiCamModule, we receive circle,ray,sector_angle parameters here..
	circleCenter[1] = y_coord;
	circleCenter[2] = z_coord;
	_slope = slope_;
	_intercept = intercept_;
	_radius = radius_;
	cir_sec_angle = sector_angle;
	ZPos = ZPosition;
}

bool RWrapper::RW_DelphiCamModule::Start(double MaxRadius, double MinRadius, bool Is_ID_tool, double DepthFromTop)		//min radius used only for OD
{
	try
	{
		//OnStartup = true;
		DelphiCamType = 0;
		InPauseState = false;
		DoingCalibration = true;
		firsttime = true;
		Getting_BC_Axis = false;
		lastPoint = NULL;
		RCadApp::Set_IsDelphiCam(true);						//set bool in rcadapp so we know that now if point taken by touch probe,dont draw cross mark,looks clumsy 

		ReferenceCenter[0] = circleCenter[0];
		ReferenceCenter[1] = circleCenter[1];
		ReferenceCenter[2] = circleCenter[2];

		double interceptPoint[4] = { 0 };
		//double slope = parenLine->Angle(), intercept = parenLine->Intercept(), radius = parentCircle->Radius();
		if (RMATH2DOBJECT->Line_circleintersection(_slope, _intercept, circleCenter, _radius, interceptPoint) < 2) return false;
		double ProbeOffsetPosition[4] = { MAINDllOBJECT->ProbeOffsetX, MAINDllOBJECT->ProbeOffsetY, 0, 0 };
		RWrapper::PointsToGo^ ProbePath_LstEntity;
		Listcount = 0;
		PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();

		//ProbePath_LstEntity = gcnew PointsToGo();												//come back to back position
		//ProbePath_LstEntity->Pt_X =  ReferenceCenter[0] - ProbeOffsetPosition[0];
		//ProbePath_LstEntity->Pt_Y =  ReferenceCenter[1] - ProbeOffsetPosition[1];			//first of all go to center...
		//ProbePath_LstEntity->Pt_Z =  RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] - ProbeOffsetPosition[2];
		//ProbePath_LstEntity->path = true;
		//PointsToGoList->Add(ProbePath_LstEntity);
		//
		//ProbePath_LstEntity = gcnew PointsToGo();												//come back to back position
		//ProbePath_LstEntity->Pt_X =  ReferenceCenter[0] - ProbeOffsetPosition[0];
		//ProbePath_LstEntity->Pt_Y =  ReferenceCenter[1] - ProbeOffsetPosition[1];
		//ProbePath_LstEntity->Pt_Z =  ZPos - DepthFromTop - ProbeOffsetPosition[2];				//we have to move to the depth of 5 mm from Zpos of top surface/3D Point
		//ProbePath_LstEntity->path = true;
		//PointsToGoList->Add(ProbePath_LstEntity);

		int MeasureCount = 360 / cir_sec_angle;
		if (Is_ID_tool)
		{
			DelphiCamType = 1;
			for (int i = 0; i < MeasureCount; i++)						//for ID we choose a expected path and possible way to take touch probe points here....
			{
				double Tmpangle = i * cir_sec_angle * M_PI / 180, ProbePts[4] = { 0 };
				if (BESTFITOBJECT->RetrieveDelphiCamPts(_slope, Tmpangle, MaxRadius, ProbePts))
				{
					ProbePath_LstEntity = gcnew PointsToGo();												//come back to back position
					ProbePath_LstEntity->Pt_X = ReferenceCenter[0] + ProbePts[0] - ProbeOffsetPosition[0];
					ProbePath_LstEntity->Pt_Y = ReferenceCenter[1] + ProbePts[1] - ProbeOffsetPosition[1];
					ProbePath_LstEntity->Pt_Z = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] - ProbeOffsetPosition[2];
					ProbePath_LstEntity->path = true;
					PointsToGoList->Add(ProbePath_LstEntity);

					ProbePath_LstEntity = gcnew PointsToGo();																//target point... so it will hit on the way..
					ProbePath_LstEntity->Pt_X = ReferenceCenter[0] + ProbePts[2] - ProbeOffsetPosition[0];
					ProbePath_LstEntity->Pt_Y = ReferenceCenter[1] + ProbePts[3] - ProbeOffsetPosition[1];
					ProbePath_LstEntity->Pt_Z = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] - ProbeOffsetPosition[2];
					ProbePath_LstEntity->path = false;
					PointsToGoList->Add(ProbePath_LstEntity);
				}
			}
		}
		else															//for OD we choose a expected path and possible way to take touch probe points here.... vinod
		{
			for (int i = 0; i < MeasureCount; i++)
			{
				double Tmpangle = i * cir_sec_angle * M_PI / 180;
				ProbePath_LstEntity = gcnew PointsToGo();											//next back side position
				ProbePath_LstEntity->Pt_X = ReferenceCenter[0] + (MaxRadius + 2 * RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue)*cos(_slope + Tmpangle);	//(MaxRadius*1.3) safe side, so from this dist. it will come toward center
				ProbePath_LstEntity->Pt_Y = ReferenceCenter[1] + (MaxRadius + 2 * RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue)*sin(_slope + Tmpangle);
				ProbePath_LstEntity->Pt_Z = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
				ProbePath_LstEntity->path = true;
				PointsToGoList->Add(ProbePath_LstEntity);

				ProbePath_LstEntity = gcnew PointsToGo();								//target point... so it will hit on the way..
				ProbePath_LstEntity->Pt_X = ReferenceCenter[0] + (MinRadius - 2 * RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue)*cos(_slope + Tmpangle);
				ProbePath_LstEntity->Pt_Y = ReferenceCenter[1] + (MinRadius - 2 * RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue)*sin(_slope + Tmpangle);
				ProbePath_LstEntity->Pt_Z = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
				ProbePath_LstEntity->path = false;
				PointsToGoList->Add(ProbePath_LstEntity);

				ProbePath_LstEntity = gcnew PointsToGo();											//come back to same back position.. so touch probe will never crash continuously with tool... this point is for safety.... 
				ProbePath_LstEntity->Pt_X = ReferenceCenter[0] + (MaxRadius + 10 * RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue)*cos(_slope + Tmpangle);
				ProbePath_LstEntity->Pt_Y = ReferenceCenter[1] + (MaxRadius + 10 * RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue)*sin(_slope + Tmpangle);
				ProbePath_LstEntity->Pt_Z = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
				ProbePath_LstEntity->path = true;
				PointsToGoList->Add(ProbePath_LstEntity);
			}
		}

		SendDrotoNextposition();
		return true;
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule002", ex);
		return false;
	}
}

bool RWrapper::RW_DelphiCamModule::Start(int MeasurementNo)
{
	try
	{	//CHeck if its already running or if parameters are set or if Axis centre is done; 
		//if yes, stop and dont run until current run is complete
		if (DoingHobMeasurement || !HobParametersUpdated) return false; // || !FirstToothDone

		//set hobchecker MeasurementNo here .. so we will care for spacing of gashes measurement and rotate all the probe points with same 0 Raxis Reference for correct calculation we use this var in RWDRO (HandleProbeHitCallback fn)
		InPauseState = false;
		DoingCalibration = true;
		firsttime = true;
		Getting_BC_Axis = false;
		DoingHobMeasurement = true;
		lastPoint = NULL;
		RWrapper::PointsToGo^ ProbePath_LstEntity;
		PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();

		startPosition = gcnew cli::array<double>(4);
		cPosition = gcnew cli::array<double>(4);
		double rX[4]; 
		//RMATH3DOBJECT->GetC_Axis_XY(RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] - MAINDllOBJECT->TIS_CAxis[2], MAINDllOBJECT->TIS_CAxis, rX);
		
		for (int i = 0; i < 4; i ++)
		{	
			rX[i] = MAINDllOBJECT->TIS_CAxis[i];
			startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
			cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
			ReferenceCenter[i] = rX[i];
		}
		if (HobParameters[15] == 0) HobParameters[15] = HobParameters[3];

		Listcount = 0; FirstStep = true;
		double Y_Axis_ApproachDist = 0.4, R_ApproachDist = 0.5 * M_PI / 180;
		//for (int kk = 0; kk < RW_DRO::MYINSTANCE()->ProbeParams->Count; kk++)
		//{
		//	if (RW_DRO::MYINSTANCE()->ProbeParams[kk]->AxisLabel == 1)
		//	{
		Y_Axis_ApproachDist = RW_DRO::MYINSTANCE()->ApproachDistance; // ProbeParams[kk]->ApproachDist;
				//break;
			//}
			//if (RW_DRO::MYINSTANCE()->ProbeParams[kk]->AxisLabel == 3)
		R_ApproachDist = RW_DRO::MYINSTANCE()->ApproachDistance; // ProbeParams[kk]->ApproachDist;
		//}
		double YmovementForBackPosition = Y_Axis_ApproachDist; //RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue
		if (FirstToothDone)
		{
			startPosition[2] = ToothMidPt[2];
			startPosition[3] = ToothMidPt[3];
		}
		//if (abs(startPosition[3] - startPosition[3]) > M_PI / 10)
		//{
			//Get to Correct C axis position before starting...
		if (HobMeasurementsDoneCount == 0)
		{
			ProbePath_LstEntity = gcnew PointsToGo();
			ProbePath_LstEntity->Pt_X = ReferenceCenter[0] - HobParameters[3] / 2 - 10;
			ProbePath_LstEntity->Pt_Y = ReferenceCenter[1] - HobParameters[4] * (MAINDllOBJECT->ProbeRadius() + 0.5); //cPosition[1];
			ProbePath_LstEntity->Pt_Z = startPosition[2]; //ReferenceCenter[2];
			ProbePath_LstEntity->Pt_R = startPosition[3];
			ProbePath_LstEntity->path = true;
			PointsToGoList->Add(ProbePath_LstEntity);
		}
		MAINDllOBJECT->ClearShapeSelections();

		if(MeasurementNo == 1)
		{	DelphiCamType = 3;
			double StratPositionX = ReferenceCenter[0] - (HobParameters[15] / 2) + HobParameters[1];
			double StratPositionY = ReferenceCenter[1] - (MAINDllOBJECT->ProbeRadius() + YmovementForBackPosition) * HobParameters[4];
			if (HobParameters[10] != 0)
				StratPositionY -= HobParameters[4] * HobParameters[3] * sin(HobParameters[10]) / 2;

			//In auto mode if user forgets to enter the number of points to be done, default to 20.
			if (HobParameters[7] == 0) HobParameters[7] = 20;
			//Distance between successive points to travel in X.
			double PerPtToothDepth = HobParameters[2] / HobParameters[7];
			//In auto mode, the tooth depth is taken as 2.25*M
			if (AutoMeasureMode)
				PerPtToothDepth = (2.25 * HobParameters[12] - MAINDllOBJECT->ProbeRadius()) / HobParameters[7];

			//Make a line shape
			//RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Cloud Points");
			if (RakePts == NULL)
			{
				RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Cloud Points");
				MAINDllOBJECT->selectShape(Convert::ToInt32(RWrapper::RW_ShapeParameter::MYINSTANCE()->ShapeParameter_ArrayList[0]), false);
				RakePts = (CloudPoints*)MAINDllOBJECT->getShapesList().selectedItem();
			}
			else
			{
				//int index = (Shape*);
				MAINDllOBJECT->selectShape(RakePts->getId(), false);
				RWrapper::RW_MainInterface::MYINSTANCE()->HandleShapeParameter_Click("Reset Points");
			}
			for(int i = 0; i < HobParameters[7]; i++)
			{	
				//Move to the next point position along tooth
				ProbePath_LstEntity = gcnew PointsToGo();											
				ProbePath_LstEntity->Pt_X =  StratPositionX + (i * PerPtToothDepth);	
				ProbePath_LstEntity->Pt_Y =  StratPositionY; //cPosition[1];
				ProbePath_LstEntity->Pt_Z =  startPosition[2]; //ReferenceCenter[2];
				ProbePath_LstEntity->Pt_R =  startPosition[3];
				ProbePath_LstEntity->path = true;
				PointsToGoList->Add(ProbePath_LstEntity);
				//Set of commands to touch the tooth and take a point
				ProbePath_LstEntity = gcnew PointsToGo();								
				ProbePath_LstEntity->Pt_X =  StratPositionX + (i * PerPtToothDepth);
				ProbePath_LstEntity->Pt_Y =  StratPositionY + (YmovementForBackPosition * HobParameters[4]);
				ProbePath_LstEntity->Pt_Z =  startPosition[2];// ReferenceCenter[2];
				ProbePath_LstEntity->Pt_R =  startPosition[3];
				ProbePath_LstEntity->path = false;
				PointsToGoList->Add(ProbePath_LstEntity);
				//Note current position
				cPosition[0] = ProbePath_LstEntity->Pt_X;
				cPosition[1] = ProbePath_LstEntity->Pt_Y;
			}
			if (HobParameters[19] < 4)
			{			//set of commands to come back to home position
				ProbePath_LstEntity = gcnew PointsToGo();
				ProbePath_LstEntity->Pt_X = StratPositionX;
				ProbePath_LstEntity->Pt_Y = StratPositionY;
				ProbePath_LstEntity->Pt_Z = startPosition[2]; //ReferenceCenter[2];
				ProbePath_LstEntity->Pt_R = startPosition[3];
				ProbePath_LstEntity->path = true;
				PointsToGoList->Add(ProbePath_LstEntity);

				ProbePath_LstEntity = gcnew PointsToGo();
				ProbePath_LstEntity->Pt_X = startPosition[0];
				ProbePath_LstEntity->Pt_Y = StratPositionY; // ReferenceCenter[1] - HobParameters[4] * (MAINDllOBJECT->ProbeRadius() + 0.5);
				ProbePath_LstEntity->Pt_Z = startPosition[2]; //ReferenceCenter[2];
				ProbePath_LstEntity->Pt_R = startPosition[3];
				ProbePath_LstEntity->path = true;
				PointsToGoList->Add(ProbePath_LstEntity);
			}
		}
		else if(MeasurementNo == 2)
		{
			DelphiCamType = 4;
			double extraDist = 5.0;
			//RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Cloud Points");
			if (IndexingPts == NULL)
			{
				RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Cloud Points");
				MAINDllOBJECT->selectShape(Convert::ToInt32(RWrapper::RW_ShapeParameter::MYINSTANCE()->ShapeParameter_ArrayList[0]), false);
				IndexingPts = (CloudPoints*)MAINDllOBJECT->getShapesList().selectedItem();
			}
			else
			{
				//int index = (Shape*);
				MAINDllOBJECT->selectShape(IndexingPts->getId(), false);
				RWrapper::RW_MainInterface::MYINSTANCE()->HandleShapeParameter_Click("Reset Points");
			}
			//If we have DS33 controller, we will take points by rotating C Axis and not moving in Y
			//So let us set Ymovement as 0
			if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 2)
			{
				YmovementForBackPosition = 0;
			}

			startPosition[0] = ReferenceCenter[0] - (HobParameters[15] / 2) - extraDist;
			startPosition[1] = ReferenceCenter[1] - (MAINDllOBJECT->ProbeRadius() + YmovementForBackPosition) * HobParameters[4];
			if (HobParameters[10] != 0)
				startPosition[1] -= HobParameters[4] * HobParameters[3] * sin(HobParameters[10]) / 2;

			//startPosition[3] = ToothMidPt[3];

			for (int i = 0; i < 4; i++)
			{
				cPosition[i] = startPosition[i];
			}

			double PitchValue = abs(M_PI * HobParameters[12] * HobParameters[11]); // / cos(HobParameters[10] * M_PI / 180)); //((ODiameter / 2)-Tooth_depth);
			double IndexAngle = M_PI * 2 / HobParameters[6]; // -RWrapper::RW_DRO::MYINSTANCE()->ProbeParams[3]->ApproachDist * M_PI / 180;
			double Move_inZDirection = 0.0;
			Move_inZDirection = PitchValue / HobParameters[6];
			//if its a cutter and not a hob, we do not move in Z when doing indexing.
			if (HobParameters[20] == 1)	Move_inZDirection = 0;
			double Rotate_AxisForHelicalLead = 0;
			double helixRad = 0; double helixAngle = 0;
			if (HobParameters[13] != 0)
			{
				helixRad = (HobParameters[15] / 2) - 1.125 * HobParameters[12];
				helixAngle = atan(HobParameters[13] / (M_PI * helixRad * 2));
				Rotate_AxisForHelicalLead = abs(Move_inZDirection) * 2 * M_PI / HobParameters[13];
				Move_inZDirection *= sin(helixAngle);
			}
			HelicalGashParams[0] = helixRad;
			HelicalGashParams[1] = helixAngle;
			HelicalGashParams[2] = Move_inZDirection;
			HelicalGashParams[3] = Rotate_AxisForHelicalLead;

			//PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
			if (FirstToothDone)
				cPosition[0] = ReferenceCenter[0] - (HobParameters[15] / 2) + 1.125 * HobParameters[12];
			else
				cPosition[0] = startPosition[0] + extraDist + HobParameters[2];

			//Also set a small shift for the C-Axis so that we do not touch face when we move in X
			double C_Axis_Offset = HobParameters[4] * abs((MAINDllOBJECT->ProbeRadius() / 2 + 0.1) / (MAINDllOBJECT->TIS_CAxis[0] - cPosition[0]));
			
			//C_Axis_Offset = 0;

			//First ensure that we will be inside 360 degree rotation for new cards only...
			if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag < 2)
			{
				if (startPosition[3] + 2 * M_PI > 2 * M_PI + M_PI / 10)
				{
					ProbePath_LstEntity = gcnew PointsToGo();
					ProbePath_LstEntity->Pt_X = startPosition[0];
					ProbePath_LstEntity->Pt_Y = startPosition[1];
					ProbePath_LstEntity->Pt_Z = startPosition[2];
					ProbePath_LstEntity->Pt_R = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
					ProbePath_LstEntity->path = true;
					PointsToGoList->Add(ProbePath_LstEntity);

					startPosition[3] -= 2 * M_PI;
					cPosition[3] = startPosition[3];
					ProbePath_LstEntity = gcnew PointsToGo();
					ProbePath_LstEntity->Pt_X = startPosition[0];
					ProbePath_LstEntity->Pt_Y = startPosition[1];
					ProbePath_LstEntity->Pt_Z = startPosition[2];
					ProbePath_LstEntity->Pt_R = startPosition[3];
					ProbePath_LstEntity->path = true;
					PointsToGoList->Add(ProbePath_LstEntity);
				}
				C_Axis_Offset = 0;
			}
			//Now we cycle through each gash for indexing....

			for (int i = 0; i <= HobParameters[6]; i++)
			{
				//If we are doing indexing immediately after Form and Pos, the stylus will be inside the tooth, and close by. 
				//So we can simply skip this one step when we start the first tooth in indexing alone...
				if (i == 0 && !((int)HobParameters[19] % 4) == 0) goto EnterToothForIndexing;

				//Move in Z and rotate to next tooth. when i = 0; it will remain there
				ProbePath_LstEntity = gcnew PointsToGo();
				ProbePath_LstEntity->Pt_X = startPosition[0];
				ProbePath_LstEntity->Pt_Y = cPosition[1];
				ProbePath_LstEntity->Pt_Z = cPosition[2];
				ProbePath_LstEntity->Pt_R = cPosition[3] - C_Axis_Offset;
				ProbePath_LstEntity->path = true;
				PointsToGoList->Add(ProbePath_LstEntity);

		EnterToothForIndexing:
				//Now lets enter the tooth. 
				//if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag < 2)
				//{
					ProbePath_LstEntity = gcnew PointsToGo();			//outer start position for every tooth with appropriate z,r movement							
					ProbePath_LstEntity->Pt_X = cPosition[0];
					ProbePath_LstEntity->Pt_Y = cPosition[1];
					ProbePath_LstEntity->Pt_Z = cPosition[2]; //ReferenceCenter[2] + (i*Move_inZDirection);
					ProbePath_LstEntity->Pt_R = cPosition[3] - C_Axis_Offset;// *HobParameters[4]; // + (AngleBWTwoHelicalTooth*i*HobDirectionSign*M_PI/180);
					ProbePath_LstEntity->path = true;
					PointsToGoList->Add(ProbePath_LstEntity);
				//}
				//Take the point by moving in Y. 
				ProbePath_LstEntity = gcnew PointsToGo();	
				//If we have DSP cards, we do in Y. if its DS33 cards, we take point by rotating in C
				ProbePath_LstEntity->Pt_X = cPosition[0];// + (2 * Tooth_depth);
				ProbePath_LstEntity->Pt_Y = cPosition[1];
				ProbePath_LstEntity->Pt_Z = cPosition[2];
				ProbePath_LstEntity->Pt_R = cPosition[3];// -C_Axis_Offset; // *HobParameters[4];;
				if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag < 2)
					ProbePath_LstEntity->Pt_Y += (YmovementForBackPosition)* HobParameters[4];
				else
				{
					ProbePath_LstEntity->Pt_R += C_Axis_Offset; //2 * 
					//ProbePath_LstEntity->Sp_R = 0.1;
				}
				ProbePath_LstEntity->path = false;
				PointsToGoList->Add(ProbePath_LstEntity);

				//Come out of the hob.
				ProbePath_LstEntity = gcnew PointsToGo();								//again back to same position
				ProbePath_LstEntity->Pt_X = startPosition[0];
				ProbePath_LstEntity->Pt_Y = cPosition[1];
				ProbePath_LstEntity->Pt_Z = cPosition[2];
				ProbePath_LstEntity->Pt_R = cPosition[3] - C_Axis_Offset; // *HobParameters[4];
				ProbePath_LstEntity->path = true;
				PointsToGoList->Add(ProbePath_LstEntity);

				//First Calculate where to move in Z and R. 
				cPosition[3] += IndexAngle - Rotate_AxisForHelicalLead; // *HobParameters[5];
				cPosition[2] += Move_inZDirection  * HobParameters[5]; // *HobParameters[4]);
			}
			
			//GO back to home position
			if (HobParameters[19] < 8)
			{
				ProbePath_LstEntity = gcnew PointsToGo();								//again back to same position
				ProbePath_LstEntity->Pt_X = startPosition[0];
				ProbePath_LstEntity->Pt_Y = startPosition[1];
				ProbePath_LstEntity->Pt_Z = startPosition[2];
				ProbePath_LstEntity->Pt_R = startPosition[3];
				ProbePath_LstEntity->path = true;
				PointsToGoList->Add(ProbePath_LstEntity);
			}
		}
		/* Indexing by rotation in R


		//{	DelphiCamType = 4;
		//	double extraDist = 5.0;
		//	//RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Cloud Points");
		//	if (IndexingPts == NULL)
		//	{
		//		RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Cloud Points");
		//		IndexingPts = (CloudPoints*)MAINDllOBJECT->getShapesList().selectedItem();
		//	}
		//	else
		//	{
		//		//int index = (Shape*);
		//		MAINDllOBJECT->selectShape(IndexingPts->getId(), false);
		//		RWrapper::RW_MainInterface::MYINSTANCE()->HandleShapeParameter_Click("Reset Points");
		//	}
		//	startPosition[0] = ReferenceCenter[0] - (HobParameters[15] / 2) - extraDist; 
		//	startPosition[1] = ReferenceCenter[1] - (MAINDllOBJECT->ProbeRadius()) * HobParameters[4]; // + YmovementForBackPosition
		//	
		//	for (int i = 0; i < 4; i ++)
		//	{	
		//		cPosition[i] = startPosition[i];
		//	}

		//	double PitchValue = abs(M_PI * HobParameters[12] * HobParameters[11]); // / cos(HobParameters[10] * M_PI / 180)); //((ODiameter / 2)-Tooth_depth);
		//	double IndexAngle = M_PI * 2 / HobParameters[6];
		//	double Move_inZDirection = 0.0;
		//	Move_inZDirection = PitchValue / HobParameters[6];
		//	double Rotate_AxisForHelicalLead = 0;
		//	double helixRad = 0; double helixAngle = 0;
		//	if (HobParameters[13] != 0) 
		//	{
		//		helixRad = (HobParameters[15] / 2) - 1.125 * HobParameters[12];
		//		helixAngle = atan(HobParameters[13] / (M_PI * helixRad * 2));
		//		Rotate_AxisForHelicalLead = abs(Move_inZDirection) * 2 * M_PI / HobParameters[13];
		//		Move_inZDirection *= sin(helixAngle);
		//	}
		//	HelicalGashParams[0] = helixRad;
		//	HelicalGashParams[1] = helixAngle;
		//	HelicalGashParams[2] = Move_inZDirection;
		//	HelicalGashParams[3] = Rotate_AxisForHelicalLead;
		//			
		//	//PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
		//	double approachTheta = 0;
		//	if (FirstToothDone)
		//	{
		//		cPosition[0] = ReferenceCenter[0] - (HobParameters[15] / 2) + 1.125 * HobParameters[12];
		//		approachTheta = YmovementForBackPosition / ((HobParameters[15] - 2.25 * HobParameters[12]) / 2);
		//	}
		//	else
		//	{
		//		cPosition[0] = startPosition[0] + extraDist + HobParameters[2];
		//		approachTheta = YmovementForBackPosition / ((HobParameters[3] - HobParameters[2]) / 2);
		//	}
		//	 
		//	startPosition[3] = ToothMidPt[3] - approachTheta * HobParameters[4];
		//	cPosition[3] = startPosition[3];

		//	//Ensure we do not cross more than 393 degrees in C position
		//	if (startPosition[3] > M_PI / 10)
		//	{
		//		startPosition[3] -= 2 * M_PI;
		//		cPosition[3] = startPosition[3];
		//		ProbePath_LstEntity = gcnew PointsToGo();	
		//		ProbePath_LstEntity->Sp_X = -1; ProbePath_LstEntity->Sp_Y = -1; ProbePath_LstEntity->Sp_Z = -1; ProbePath_LstEntity->Sp_R = -1;
		//		ProbePath_LstEntity->Pt_X =  startPosition[0];
		//		ProbePath_LstEntity->Pt_Y =  startPosition[1];
		//		ProbePath_LstEntity->Pt_Z =  startPosition[2];
		//		ProbePath_LstEntity->Pt_R =  startPosition[3]; ProbePath_LstEntity->Sp_R = 8;
		//		ProbePath_LstEntity->path = true;
		//		PointsToGoList->Add(ProbePath_LstEntity);
		//	}

		//	for(int i = 0; i <= HobParameters[6]; i++)
		//	{	
		//		//Move in Z and rotate to next tooth. when i = 0; it will remain there
		//		ProbePath_LstEntity = gcnew PointsToGo();								
		//		ProbePath_LstEntity->Pt_X =  startPosition[0];
		//		ProbePath_LstEntity->Pt_Y =  cPosition[1];
		//		ProbePath_LstEntity->Pt_Z =  cPosition[2];
		//		ProbePath_LstEntity->Pt_R = cPosition[3]; 
		//		ProbePath_LstEntity->path = true;
		//		PointsToGoList->Add(ProbePath_LstEntity);

		//		//Now lets enter the tooth. 
		//		ProbePath_LstEntity = gcnew PointsToGo();			//outer start position for every tooth with appropriate z,r movement							
		//		ProbePath_LstEntity->Pt_X = cPosition[0];//	ProbePath_LstEntity->Sp_X = 2;
		//		ProbePath_LstEntity->Pt_Y =  cPosition[1] ;
		//		ProbePath_LstEntity->Pt_Z =  cPosition[2] ; //ReferenceCenter[2] + (i*Move_inZDirection);
		//		ProbePath_LstEntity->Pt_R =  cPosition[3]; // + (AngleBWTwoHelicalTooth*i*HobDirectionSign*M_PI/180);
		//		ProbePath_LstEntity->path = true;
		//		PointsToGoList->Add(ProbePath_LstEntity);

		//		//Take the point by moving in R (earlier it was in Y.
		//		ProbePath_LstEntity = gcnew PointsToGo();								//go for taking ppoints
		//		ProbePath_LstEntity->Pt_X =  cPosition[0];// + (2 * Tooth_depth);
		//		ProbePath_LstEntity->Pt_Y = cPosition[1]; // +(YmovementForBackPosition)* HobParameters[4]; //MAINDllOBJECT->ProbeRadius() +
		//		ProbePath_LstEntity->Pt_Z =  cPosition[2];
		//		ProbePath_LstEntity->Pt_R = cPosition[3] + approachTheta * HobParameters[4]; ProbePath_LstEntity->Sp_R = 0.05;
		//		ProbePath_LstEntity->path = false;
		//		PointsToGoList->Add(ProbePath_LstEntity);

		//		//Come out of the hob.
		//		ProbePath_LstEntity = gcnew PointsToGo();								//again back to same position
		//		ProbePath_LstEntity->Pt_X =  startPosition[0];
		//		ProbePath_LstEntity->Pt_Y =  cPosition[1] - YmovementForBackPosition * HobParameters[4];
		//		ProbePath_LstEntity->Pt_Z =  cPosition[2];
		//		ProbePath_LstEntity->Pt_R = cPosition[3]; // - approachTheta * HobParameters[4]; ProbePath_LstEntity->Sp_R = 2;
		//		ProbePath_LstEntity->path = true;
		//		PointsToGoList->Add(ProbePath_LstEntity);

		//		//First Calculate where to move in Z and R. 
		//		//R - rotate by one gash angle minus correction for helical gash and also reduce for approach angle
		//		cPosition[3] += IndexAngle + Rotate_AxisForHelicalLead * HobParameters[5]; //- approachTheta * HobParameters[4];
		//		//go down by one pitch minus correction of helical gash and number of starts. 
		//		cPosition[2] += Move_inZDirection  * HobParameters[5]; // *HobParameters[4]);
		//	}
		//	//GO back to home position
		//	ProbePath_LstEntity = gcnew PointsToGo();								//again back to same position
		//	ProbePath_LstEntity->Pt_X =  startPosition[0];
		//	ProbePath_LstEntity->Pt_Y =  startPosition[1];
		//	ProbePath_LstEntity->Pt_Z =  startPosition[2];
		//	ProbePath_LstEntity->Pt_R =  startPosition[3];
		//	ProbePath_LstEntity->path = true;
		//	PointsToGoList->Add(ProbePath_LstEntity);
		//}
*/

		else 						//default Measurement no. 3				
		{	DelphiCamType = 5;
			startPosition[0] = ReferenceCenter[0] - (HobParameters[15] / 2) - 10; 
			startPosition[1] = ReferenceCenter[1] - HobParameters[4] * (MAINDllOBJECT->ProbeRadius() + YmovementForBackPosition);
			if (HobParameters[10] != 0)
				startPosition[1] -= HobParameters[4] * HobParameters[3] * sin(HobParameters[10]) / 2;

			double PitchValue = abs(M_PI * HobParameters[12]); // / cos(HobParameters[10] * M_PI / 180)); 
			double Move_inZDirection = 0.0;
			double Rotate_AxisForHelicalLead = 0.0;
			
			//Get total number of teeth in the hob
			int totalTeethinHob = (int) Math::Round(HobParameters[9] / PitchValue, 0);
			//See if user wants more than one pt in a tooth. 
			if (HobParameters[8] > totalTeethinHob)
				HobParameters[7] = (int) Math::Round(HobParameters[8] / totalTeethinHob, 0);
			else
				HobParameters[7] = 1;
			//If user wants to skip teeth, we get the jumpsize
			int jumpSize =(int) Math::Round(totalTeethinHob /  HobParameters[8], 0);
			if (jumpSize < 1) jumpSize = 1;

			//Calculate how much to move from one point to the next
			Move_inZDirection = abs(PitchValue / HobParameters[7]);

			//TIlt COrrection for Gash Lead - 
			double X_CorrectionforTilt = Move_inZDirection * Hob_RunoutParams[0];
			//Gash Helical? Then we have to rotate to ensure we dont hit the component
			if (HobParameters[13] != 0) 
			{
				double helixRad = (HobParameters[15] / 2) - 1.125 * HobParameters[12];
				double helixAngle = atan(HobParameters[13] / (M_PI * helixRad * 2));
				PitchValue *= abs(sin(helixAngle));
				Move_inZDirection *= abs(sin(helixAngle));
				Rotate_AxisForHelicalLead = abs(Move_inZDirection * 2 * M_PI / HobParameters[13]);
			}
			
			if (LeadPts == NULL)
			{
				RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Cloud Points");
				MAINDllOBJECT->selectShape(Convert::ToInt32(RWrapper::RW_ShapeParameter::MYINSTANCE()->ShapeParameter_ArrayList[0]), false);
				LeadPts = (CloudPoints*)MAINDllOBJECT->getShapesList().selectedItem();
			}
			else
			{
				MAINDllOBJECT->selectShape(LeadPts->getId(), false);
				RWrapper::RW_MainInterface::MYINSTANCE()->HandleShapeParameter_Click("Reset Points");
			}

			if (FirstToothDone)
			{
				cPosition[0] = ReferenceCenter[0] - (HobParameters[15] / 2) + 1.125 * HobParameters[12]; 
			}
			else
			{
				cPosition[0] = ReferenceCenter[0] - (HobParameters[15] / 2) + HobParameters[1] + HobParameters[2];
				ToothMidPt[2] = startPosition[2];// + (HobParameters[18] - 1) * PitchValue;
			}
			double ZmoveforStartTooth = (HobParameters[18] - 1) * PitchValue; 
			cPosition[2] = ToothMidPt[2] + abs(ZmoveforStartTooth);
			cPosition[3] = startPosition[3];
			if (HobParameters[19] > 4)
				cPosition[3] += 2 * M_PI;
 
			if (HobParameters[13] != 0)
				cPosition[3] = startPosition[3] - (HobParameters[5]) * ZmoveforStartTooth * 2 * M_PI / HobParameters[13];

			//We to decide on helical gash hobs if we should first rotate and then move in Z or vice-versa.
			bool rotate_First = false;
			if (HobParameters[4] > 0 && HobParameters[5] < 0)
				rotate_First = true;
			if (HobParameters[4] < 0 && HobParameters[5] > 0)
				rotate_First = true;

			//First Move to the top most tooth - 
			ProbePath_LstEntity = gcnew PointsToGo();											
			ProbePath_LstEntity->Pt_X =  startPosition[0];
			ProbePath_LstEntity->Pt_Y =  startPosition[1];
			ProbePath_LstEntity->Pt_Z =  cPosition[2];
			ProbePath_LstEntity->Pt_R =  cPosition[3];
			ProbePath_LstEntity->path = true;
			PointsToGoList->Add(ProbePath_LstEntity);

			//Go inside tooth in X
			ProbePath_LstEntity = gcnew PointsToGo();											
			ProbePath_LstEntity->Pt_X =  cPosition[0];
			ProbePath_LstEntity->Pt_Y =  startPosition[1];
			ProbePath_LstEntity->Pt_Z =  cPosition[2];
			ProbePath_LstEntity->Pt_R =  cPosition[3];
			ProbePath_LstEntity->path = true;
			PointsToGoList->Add(ProbePath_LstEntity);

			for(int ii = 0; ii < HobParameters[8]; ii ++)
			{			
				//Move in Y to touch and take a point
				ProbePath_LstEntity = gcnew PointsToGo();								
				ProbePath_LstEntity->Pt_X =  cPosition[0];
				ProbePath_LstEntity->Pt_Y =  startPosition[1] + (YmovementForBackPosition * HobParameters[4]);
				ProbePath_LstEntity->Pt_Z =  cPosition[2];
				ProbePath_LstEntity->Pt_R =  cPosition[3];
				ProbePath_LstEntity->path = false;
				PointsToGoList->Add(ProbePath_LstEntity);	

				//Move down in Z or rotate if helical gash
				if (HobParameters[13] != 0)
				{
					//If we have to rotate first, we do that
					if (rotate_First)
						cPosition[3] += (HobParameters[5]) * Rotate_AxisForHelicalLead * jumpSize;
					else
					{
						if (ii != HobParameters[8] - 1)
							cPosition[2] -= (Move_inZDirection * jumpSize);
					}
					
					ProbePath_LstEntity = gcnew PointsToGo();
					ProbePath_LstEntity->Pt_X =  cPosition[0];
					ProbePath_LstEntity->Pt_Y =  startPosition[1];
					ProbePath_LstEntity->Pt_Z =  cPosition[2];
					ProbePath_LstEntity->Pt_R =  cPosition[3];
					ProbePath_LstEntity->path = true;
					PointsToGoList->Add(ProbePath_LstEntity);
					//First step over. Now move in Z or rotate next
					if (!rotate_First)
						cPosition[3] += (HobParameters[5]) * Rotate_AxisForHelicalLead * jumpSize;
					else
					{
						if (ii != HobParameters[8] - 1) 
							cPosition[2] -= (Move_inZDirection * jumpSize);
					}
					ProbePath_LstEntity = gcnew PointsToGo();
					ProbePath_LstEntity->Pt_X =  cPosition[0];
					ProbePath_LstEntity->Pt_Y =  startPosition[1];
					ProbePath_LstEntity->Pt_Z =  cPosition[2];
					ProbePath_LstEntity->Pt_R =  cPosition[3];
					ProbePath_LstEntity->path = true;
					PointsToGoList->Add(ProbePath_LstEntity);
				}
				else
				{
					if (ii != HobParameters[8] - 1)
						cPosition[2] -= (Move_inZDirection * jumpSize);
					
					ProbePath_LstEntity = gcnew PointsToGo();
					ProbePath_LstEntity->Pt_X =  cPosition[0];
					ProbePath_LstEntity->Pt_Y =  startPosition[1];
					ProbePath_LstEntity->Pt_Z =  cPosition[2];
					ProbePath_LstEntity->Pt_R =  cPosition[3];
					ProbePath_LstEntity->path = true;
					PointsToGoList->Add(ProbePath_LstEntity);
				}
				cPosition[0] += X_CorrectionforTilt;
			}
			cPosition[1] = ProbePath_LstEntity->Pt_Y;
			//Get out of hob area
			ProbePath_LstEntity = gcnew PointsToGo();								
			ProbePath_LstEntity->Pt_X =  startPosition[0];
			ProbePath_LstEntity->Pt_Y = startPosition[1]; //ReferenceCenter[1] + HobParameters[4] * (MAINDllOBJECT->ProbeRadius() + 4);
			ProbePath_LstEntity->Pt_Z =  cPosition[2];
			ProbePath_LstEntity->Pt_R = cPosition[3]; // -(HobParameters[5]) * Rotate_AxisForHelicalLead;
			ProbePath_LstEntity->path = true;
			PointsToGoList->Add(ProbePath_LstEntity);
			//return to home position
			ProbePath_LstEntity = gcnew PointsToGo();								
			ProbePath_LstEntity->Pt_X =  startPosition[0];
			ProbePath_LstEntity->Pt_Y =  startPosition[1];
			ProbePath_LstEntity->Pt_Z =  startPosition[2];
			ProbePath_LstEntity->Pt_R =  startPosition[3];
			ProbePath_LstEntity->path = true;
			PointsToGoList->Add(ProbePath_LstEntity);
		}
		System::String^ MeasType;
		if (MeasurementNo == 1)
			MeasType = "Form and Position";
		else if (MeasurementNo == 2)
			MeasType = "Spacing of Gashes";
		else
			MeasType = "Gash Lead";
		//System::String^ SfilePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath;
		//SfilePath = SfilePath + "\\Log\\AxisLog\\SpacingPts.csv";
		//System::IO::StreamWriter^ pointsFile = gcnew System::IO::StreamWriter(SfilePath, true);
		//pointsFile->WriteLine(MeasType);
		//pointsFile->Close();
		RWrapper::RW_DelphiCamModule::MYINSTANCE()->WritePositionCommandstoFile(MeasType);
		SendDrotoNextposition();
		return true;
	}
	catch(Exception^ ex)
	{ 
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule002", ex); 
		return false;
	}
}

void RWrapper::RW_DelphiCamModule::GetBC_Axis(double diameter, int masterL, int LevelNumber)
{	//First lets make the list of points it has to go to. 
	try
	{	HobOD = diameter;		//used in rwcnc in fn MoveToC_Axis_Y_Position()
		DelphiCamType = 2;
		if (GotAxis) return;
		//Check if we have already done the cylinder in this instance before
		//MAINDllOBJECT->CreateReferenceCylinder();
		ManualPoint = true; 
		Getting_BC_Axis = true;
		//Store the start position of the machine
		startPosition = gcnew cli::array<double>(4);
		cPosition = gcnew cli::array<double>(4);
		CylinderDia = diameter;
		MasterLength = masterL; 
		NumberofLevels = LevelNumber;
		//Doing_Y_Check = true;
		BC_Center_C_Dir = 1;
		if (RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] > 0) BC_Center_C_Dir = -1;
		 
		if (topC == NULL)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Circle");
			topC = (Circle*)MAINDllOBJECT->getShapesList().selectedItem();
		}
		else
		{
			//int index = (Shape*);
			MAINDllOBJECT->selectShape(topC->getId(), false);
			RWrapper::RW_MainInterface::MYINSTANCE()->HandleShapeParameter_Click("Reset Points");
		}

		BC_CentrePointNumber = 1;		
		BC_CentrePositionPts = (double*)malloc(BC_CentrePointNumber * 2 * sizeof(double));
		BC_Diameters = (double*)malloc(BC_CentrePointNumber * sizeof(double));
		//MasterCylinderFirstStep = true;
		RWrapper::PointsToGo^ ProbePath_LstEntity;
		ProbePath_LstEntity = gcnew PointsToGo;
		PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
		ProbePath_LstEntity->Pt_X = RW_DRO::MYINSTANCE()->DROCoordinate[0] + 50;
		ProbePath_LstEntity->Pt_Y = RW_DRO::MYINSTANCE()->DROCoordinate[1];
		ProbePath_LstEntity->Pt_Z = RW_DRO::MYINSTANCE()->DROCoordinate[2];
		ProbePath_LstEntity->Pt_R = RW_DRO::MYINSTANCE()->DROCoordinate[3];
		ProbePath_LstEntity->Sp_X = 2;
		ProbePath_LstEntity->path = false;
		PointsToGoList->Add(ProbePath_LstEntity);
		AllowIncrement = false;
		Listcount = 0; FirstStep = true;
		DoingCalibration = true;

		SendDrotoNextposition();
		return;
	}
	catch(Exception^ ex)
	{ 
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule003", ex); 
	}
}

void RWrapper::RW_DelphiCamModule::countinue_pause_delphicammodule(bool is_pause)// continue or pause implemented on same button of start as toggle          vinod..
{
	InPauseState = is_pause;
	if(is_pause)
	{
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
	}
	else
	{
		try
		{
			if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);

			double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
			double TargetValue[4] = {LastTargetValue[0], LastTargetValue[1], LastTargetValue[2], LastTargetValue[3]};
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(TargetValue, &feedrate[0], CurrentTargetType);
		}
		catch(Exception^ ex)
		{ 
			
		}
	}
}

void RWrapper::RW_DelphiCamModule::Reset_BC_Axis()
{	
	GotAxis = false;
	Listcount = 0;
	ClearAll();
	RWrapper::RW_MainInterface::MYINSTANCE()->HandleShapeParameter_Click("Reset Points");
}

void RWrapper::RW_DelphiCamModule::StopProcess()
{
	RCadApp::Set_IsDelphiCam(false);		//set bool in rcadapp so we know that process is over and we have to draw a cross whenever need in any drawing 
	try
	{
		DoingHobMeasurement = false;
		HobMeasurementsDoneCount = 0;
		MAINDllOBJECT->DeselectAll(false);
		PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
		ClearAll();
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule006b", ex); 
	}
}

void RWrapper::RW_DelphiCamModule::ClearAll()
{
	try
	{	
		//if (Listcount != PointsToGoList->Count) GotAxis = false;	
		Listcount = 0;
		lastPoint = NULL;
		parentCircle = NULL;
		parenLine = NULL;
		firsttime = true;
		DoingCalibration = false;
		Getting_BC_Axis = false;
		InPauseState = false;
		HobMeasurementsDoneCount = 0;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule002b", ex); }
}

void RWrapper::RW_DelphiCamModule::SendDrotoNextposition()
{
	try
	{	
		if(InPauseState) return;

		if(PointsToGoList->Count < 1) return;
		if(Listcount < 0) return;
		if (FinishingRunoutCalculations) return;
		double target[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		

		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		RWrapper::PointsToGo^ ProbePath_LstEntity = gcnew PointsToGo();
		
		//Check if we have reached the target...
		ProbePath_LstEntity = PointsToGoList[Listcount];
		////If we are running Indexing program, we have to note the C-Axis position for indexing measurements
		//if (DelphiCamType == 4 && !ProbePath_LstEntity->path)
		//	RWrapper::RW_DRO::MYINSTANCE()->HandleProbeHitCallback(HobParameters[4], target);

		if (ProbePath_LstEntity->path)
		{
			if (Listcount > 0) // && Listcount != PointsToGoList->Count)
			{
				double shift[4];
				shift[0] = abs(target[0] - ProbePath_LstEntity->Pt_X);
				shift[1] = abs(target[1] - ProbePath_LstEntity->Pt_Y);
				shift[2] = abs(target[2] - ProbePath_LstEntity->Pt_Z);
				shift[3] = abs(target[3] - ProbePath_LstEntity->Pt_R);
				for (int ii = 0; ii < 3; ii++)
				{
					if (shift[ii] > 2.0)
					{
						//We havent reaced the target yet. Let us just exit the loop 
						for (int i = 0; i < 1000000; i++)
						{
						}
						return;
					}
				}
			}
		}
		else
		{
			if (WaitingForTouchPt) 
			{
				//WaitingForTouchPt = false; 
				//return;
			}
		}
		
		if (AllowIncrement) Listcount ++;
		double ApproachDist = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;
		AllowIncrement = false;
		if (Listcount == PointsToGoList->Count) 
		{
			Sleep(30);
			DelphiCamEvent::raise(1);
			HobMeasurementsDoneCount++;
			return;
		}
		else
		{
			double prog = (double)(Listcount)/(double)PointsToGoList->Count;
			if (prog > 0.95) prog = 0.95;
			DelphiCamEvent::raise(prog);
		}

		//if (Listcount == PointsToGoList->Count - 2 && Doing_Y_Check) Doing_Y_Check = false;
		if(Listcount >= PointsToGoList->Count) // + 1)
		{	
			Listcount = -1;
			return;
		}
		else if(Listcount % 2 != 0 && DelphiCamType == 0)
		{
		   if (!DoingCalibration) MAINDllOBJECT->NotedownProbePath();
		}

		//RWrapper::PointsToGo^ NextStep_LstEntity = gcnew PointsToGo();
		bool IncrementListCt = true;
		//Check if previous command has been properly executed. Do not call next command until we are sure to have reached the previous one!
		bool GotoNextStep = true;
		double ExtraMoveForTouch = 0.0;
		
		ProbePath_LstEntity = PointsToGoList[Listcount];
		
		double delta[4] = {0};
		delta[0] = abs(target[0] - ProbePath_LstEntity->Pt_X);
		delta[1] = abs(target[1] - ProbePath_LstEntity->Pt_Y);
		delta[2] = abs(target[2] - ProbePath_LstEntity->Pt_Z);
		delta[3] = abs(target[3] - ProbePath_LstEntity->Pt_R);
		
		double speedFrac = 1; // 0.85; // 0.75;
		double givenFeeds[4] = { ProbePath_LstEntity->Sp_X, ProbePath_LstEntity->Sp_Y, ProbePath_LstEntity->Sp_Z, ProbePath_LstEntity->Sp_R };
		double MaxSpeed = 20, MinSpeed = 1.2;
		//if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 2)
		//{
			//MaxSpeed = 10;
			//MinSpeed = 4;
			//for (int ii = 0; ii < 4; ii++)
			//	feedrate[ii] = RWrapper::RW_DRO::MYINSTANCE()->feedRate[ii];
			//givenFeeds[3] = 5;
		//}
		//else
		//{

			for (int ii = 0; ii < 4; ii++)
			{
				if (givenFeeds[ii] > 0 && givenFeeds[ii] <= 20)
				{
					feedrate[ii] = givenFeeds[ii];
					if (feedrate[ii] <= 0.3) feedrate[ii] = 0.3;
				}
				else
				{
					if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 2)
					{
						feedrate[ii] = RWrapper::RW_DRO::MYINSTANCE()->feedRate[ii];
					}
					else
					{
						feedrate[ii] = delta[ii] * speedFrac;
						if (feedrate[ii] > MaxSpeed) feedrate[ii] = MaxSpeed;
						if (feedrate[ii] < MinSpeed) feedrate[ii] = MinSpeed;
						if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 1)
							if (feedrate[ii] > 1.5 && feedrate[ii] < 2.2) feedrate[ii] = 2.2;
					}
					
				}
			}
		//}

		if (Doing_Y_Check &&  Listcount == 2)
		{
			feedrate[1]= 2;
			Doing_Y_Check = false;
		}
		if (StartToothStepNumber == 103 && Listcount == 1 && !ProbePath_LstEntity->path) feedrate[0] = 2;
		
		if (StartToothStepNumber == 102 && !ProbePath_LstEntity->path) feedrate[2]= 0.5;
		
		if (givenFeeds[3] <= 0)
		{
			feedrate[3] = (delta[3] * 180 / M_PI) * 0.8;
			if (feedrate[3] > MaxSpeed) feedrate[3] = MaxSpeed;
			if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 1)
			{
				if (feedrate[3] > MaxSpeed * speedFrac) feedrate[3] = MaxSpeed * speedFrac;
				if (feedrate[3] < 0.6) feedrate[3] = 0.6;
				if (feedrate[3] > 1.5 && feedrate[3] < 2.2) feedrate[3] = 1.4;
			}
		}

		if (feedrate[3] > 0.85 * MaxSpeed) 
			feedrate[3] = 25;
		//feedrate[0] *= 0.8;
		//if (feedrate[2] > 5) feedrate[2] = 5;
		//feedrate[0] = 7; feedrate[1] = 7;

		target[0] = ProbePath_LstEntity->Pt_X; target[1] = ProbePath_LstEntity->Pt_Y; target[2] = ProbePath_LstEntity->Pt_Z, target[3] = ProbePath_LstEntity->Pt_R;		
	    //probedirection = getMovementdirection(target);
		CurrentTargetType = TargetReachedCallback::DELPHICOMPONENTMODULE;
		if (!ProbePath_LstEntity->path)
		{	//if (!GotoNextStep) Listcount ++;
			probedirection = getMovementdirection(target);
			//if (!GotoNextStep) Listcount --;
			if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 1)
			{
				switch (probedirection)
				{
				case 1:
					target[0] += ApproachDist + ExtraMoveForTouch;
					break;
				case 3:
					target[0] -= ApproachDist + ExtraMoveForTouch;
					break;
				case 5:
					target[1] += ApproachDist + ExtraMoveForTouch;
					break;
				case 7:
					target[1] -= ApproachDist + ExtraMoveForTouch;
					break;
				case 9:
					target[2] += ApproachDist + ExtraMoveForTouch;
					break;
				case 11:
					target[2] -= ApproachDist + ExtraMoveForTouch;
					break;
				case 13:
					ApproachDist = ApproachDist / ((HobParameters[3] - 2.25 * HobParameters[12]) / 2);
					target[3] += ApproachDist + ExtraMoveForTouch;
					break;
				case 15:
					ApproachDist = ApproachDist / ((HobParameters[3] - 2.25 * HobParameters[12]) / 2);
					target[3] -= ApproachDist + ExtraMoveForTouch;
					break;

				}
				DROInstance->SetProbeDirection(probedirection);
			}
			else
			{
				switch (probedirection)
				{
				case 0:
					//target[0] -= ApproachDist + ExtraMoveForTouch;
					//break;
				case 1:
					//target[0] += ApproachDist + ExtraMoveForTouch;
					RWrapper::RW_DRO::MYINSTANCE()->SetPitch(14746, 0, false); // 7373
					break;
				case 2:
					//target[1] -= ApproachDist + ExtraMoveForTouch;
					//break;
				case 3:
					//target[1] += ApproachDist + ExtraMoveForTouch;
					RWrapper::RW_DRO::MYINSTANCE()->SetPitch(14746, 1, false);
					break;
				case 4:
					//target[2] -= ApproachDist + ExtraMoveForTouch;
					//break;
				case 5:
					//target[2] += ApproachDist + ExtraMoveForTouch;
					break;
				case 6:
					//ApproachDist = ApproachDist / ((HobParameters[15] - 2.25 * HobParameters[12]) / 2);
					//target[3] -= ApproachDist + ExtraMoveForTouch;
					//break;
				case 7:
					//ApproachDist = ApproachDist / ((HobParameters[15] - 2.25 * HobParameters[12]) / 2);
					//target[3] += ApproachDist + ExtraMoveForTouch;
					RWrapper::RW_DRO::MYINSTANCE()->SetPitch(14746, 3, false);
					break;
				}
			}
			//}
			//if(!ProbePath_LstEntity->path)
			//{
			DROInstance->SetProbeDirection(probedirection);
			if (!firsttime)
			{
				//HELPEROBJECT->AddMeasurementForDelphiComponent(parenLine, parentCircle, lastPoint);				//draw measurement b/w 2 points
			}
			else
				firsttime = false;

			if (StartToothStepNumber == 101 || StartToothStepNumber == 201)
			{
				if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 1)
				{
					feedrate[3] = 0.05;
					if (probedirection < 12) DROInstance->SetProbeDirection(13);
				}
				else
					feedrate[3] = 1;
				//RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = false;
			}
			//else
			RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = true;
			WaitingForTouchPt = true;
		}
		else
		{
			WaitingForTouchPt = false;
			RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = false;
			if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 2)
			{
				RWrapper::RW_DRO::MYINSTANCE()->SetPitch(7373, 0, false); //  3686
				RWrapper::RW_DRO::MYINSTANCE()->SetPitch(7373, 1, false);
				RWrapper::RW_DRO::MYINSTANCE()->SetPitch(7373, 3, false);
			}
				//{
			//	if (DoingCalibration && feedrate[1] > feedrate[0] * 0.6)
			//		feedrate[1] = feedrate[0] * 0.7;
			//}
		}
		if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);

		for(int i = 0; i < 4; i++)
			LastTargetValue[i] = target[i];
	    RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], CurrentTargetType);
		//if (Listcount == 0) FirstStep = false;
		AllowIncrement = true;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule004", ex); }
}

int RWrapper::RW_DelphiCamModule::getMovementdirection(double* targetPos)
{
	try
	{
		double target[4] = { RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] };
		if (Listcount > 1)
		{
			RWrapper::PointsToGo^ ProbePath_LstEntity = gcnew PointsToGo();
			ProbePath_LstEntity = PointsToGoList[Listcount - 1];
			target[0] = ProbePath_LstEntity->Pt_X; target[1] = ProbePath_LstEntity->Pt_Y; target[2] = ProbePath_LstEntity->Pt_Z, target[3] = ProbePath_LstEntity->Pt_R;
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
			if (abs(Rdist) > dist)
			{
				if (Rdist < 0)
					direction = 15;
				else
					direction = 13;
			}
		}
		else
		{
			direction = 0;
			if (Xdist < 0)
				direction = 1;
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
					direction = 6;
			}
		}
		return direction;
	}
	catch (Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule005", ex); return 1; }
}

void RWrapper::RW_DelphiCamModule::Get_Cylinder_Y_Position()
{
	if (GotAxis) GotAxis = false;	
	try
	{
		RWrapper::PointsToGo^ ProbePath_LstEntity;
		PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
		double extraDist = CylinderDia / 10 - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;
		//if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 2)
		//	extraDist += RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;
		DoingCalibration = true;
		AllowIncrement = false;
		Listcount = 0; FirstStep = true;

		for (int i = 0; i < 4; i++)
		{
			startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->ProbeCoordinate[i];
			cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->ProbeCoordinate[i];
		}
		startPosition[0] += MAINDllOBJECT->ProbeRadius();
		cPosition[0] = startPosition[0];
		ManualPoint = false;
		startPosition[3] = 0;
		//if (MasterCylinder == NULL)
		//{
		//	RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Circle");
		//	MasterCylinder = (Circle*)MAINDllOBJECT->getShapesList().selectedItem();
		//	//startPosition[1] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] - CylinderDia / 2 - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue - MAINDllOBJECT->ProbeRadius();
		//}
		//else
		//{
		//	MAINDllOBJECT->selectShape(MasterCylinder->getId(), false);
		//	MAINDllOBJECT->ResetthePoints_selectedshape();
		//}

		if (MAINDllOBJECT->TIS_CAxis[0] == 0)
			ReferenceCenter[0] = DROInstance->ProbeCoordinate[0] + CylinderDia / 2 + MAINDllOBJECT->ProbeRadius();//  *1.4;
		else
			ReferenceCenter[0] = MAINDllOBJECT->TIS_CAxis[0];

		int dir = 1;
		if (cPosition[3] > 0) dir = -1;

		//First Let us rotate one half rotation to make the hob settle down properly
		ProbePath_LstEntity = gcnew PointsToGo();
		ProbePath_LstEntity->Pt_X = startPosition[0] - extraDist;
		ProbePath_LstEntity->Pt_Y = startPosition[1] + CylinderDia / 2 + extraDist; // *0.4); // 2 + extraDist);
		ProbePath_LstEntity->Pt_Z = startPosition[2];
		ProbePath_LstEntity->Pt_R = RW_DRO::MYINSTANCE()->DROCoordinate[3] + HobSettings[0]  * (M_PI / 180);// +dir * 2 * M_PI; ProbePath_LstEntity->Sp_R = 10;
		ProbePath_LstEntity->Sp_R = 25;
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);

		//Now rotate back to 0 position in C axis
		ProbePath_LstEntity = gcnew PointsToGo();
		ProbePath_LstEntity->Pt_X = startPosition[0] + CylinderDia / 2;;
		ProbePath_LstEntity->Pt_Y = startPosition[1] + CylinderDia / 2 + extraDist; // *0.4); // 2 + extraDist);
		ProbePath_LstEntity->Pt_Z = startPosition[2];
		ProbePath_LstEntity->Pt_R = startPosition[3];// +dir * 2 * M_PI; ProbePath_LstEntity->Sp_R = 10;
		ProbePath_LstEntity->Sp_R = 25;
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);

		////Set of commmands to go Positive Y outside.
		//ProbePath_LstEntity = gcnew PointsToGo();
		//ProbePath_LstEntity->Pt_X = startPosition[0] - extraDist;
		//ProbePath_LstEntity->Pt_Y = startPosition[1] + CylinderDia / 2 + extraDist; // *0.4); // 2 + extraDist);
		//ProbePath_LstEntity->Pt_Z = startPosition[2];
		//ProbePath_LstEntity->Pt_R = startPosition[3];// +dir * 2 * M_PI; ProbePath_LstEntity->Sp_R = 10;
		//ProbePath_LstEntity->path = true;
		//PointsToGoList->Add(ProbePath_LstEntity);
		//Set the current position
		//cPosition[0] = ProbePath_LstEntity->Pt_X;
		//cPosition[1] = startPosition[1] + CylinderDia / 2 + extraDist;
		////Move right in X axis to the centre
		//ProbePath_LstEntity = gcnew PointsToGo();
		//ProbePath_LstEntity->Pt_X = startPosition[0] + CylinderDia / 2;
		//ProbePath_LstEntity->Pt_Y = cPosition[1];// + CylinderDia/2  + extraDist;
		//ProbePath_LstEntity->Pt_Z = cPosition[2];
		//ProbePath_LstEntity->Pt_R = startPosition[3]; ProbePath_LstEntity->Sp_R = 20;
		//ProbePath_LstEntity->path = true;//
		//PointsToGoList->Add(ProbePath_LstEntity);

		//Move in -Y direction toward the cylinder to get the Y-position, and set that in Start Position and cPosition - 
		ProbePath_LstEntity = gcnew PointsToGo();
		ProbePath_LstEntity->Pt_X = startPosition[0] + CylinderDia / 2;
		ProbePath_LstEntity->Pt_Y = startPosition[1]; // ReferenceCenter[1]; // cPosition[1] - (CylinderDia / 2 + extraDist + 10);// + CylinderDia/2  + extraDist;
		ProbePath_LstEntity->Pt_Z = cPosition[2];
		ProbePath_LstEntity->Pt_R = startPosition[3];
		ProbePath_LstEntity->path = false;
		PointsToGoList->Add(ProbePath_LstEntity);

		//ProbePath_LstEntity = gcnew PointsToGo();
		//ProbePath_LstEntity->Pt_X = startPosition[0] + CylinderDia / 2;
		//ProbePath_LstEntity->Pt_Y = startPosition[1] + CylinderDia / 2 + 2 * extraDist; // ReferenceCenter[1]; // cPosition[1] - (CylinderDia / 2 + extraDist + 10);// + CylinderDia/2  + extraDist;
		//ProbePath_LstEntity->Pt_Z = cPosition[2];
		//ProbePath_LstEntity->Pt_R = startPosition[3];
		//ProbePath_LstEntity->path = true;
		//PointsToGoList->Add(ProbePath_LstEntity);


		RWrapper::RW_DelphiCamModule::MYINSTANCE()->WritePositionCommandstoFile("Check Hub Y Position");
		//Set the Flag for Checking Y position of Cylinder to true so that we know when to start touching for taking points on cylinder...
		//Doing_Y_Check = true;

		//Start RUnning all the points in CNC
		SendDrotoNextposition();
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule006", ex);
	}
}

void RWrapper::RW_DelphiCamModule::CalculatePtsFor_BC_Axis(double Move_CAxis_by_ThisAngle)
{	
	if (GotAxis) GotAxis = false;	
	//if (Doing_Y_Check)
	//{	
	//	//We got the X-position. Let us calculate positions and take the Y position. 
	//	//X position etc will be noted inside this function!
	//	Get_Cylinder_Y_Position();
	//	return;
	//}
	try
	{
		RWrapper::PointsToGo^ ProbePath_LstEntity;
		PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
		double extraDist = CylinderDia / 4 - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;
		if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 2)
			extraDist += RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;

		DoingCalibration = true;
		ManualPoint = false;
		//MAINDllOBJECT->ReferenceDotShape->selected(true);
		Getting_BC_Axis = true;
		//MessageBox::Show(MasterCylinder->PointsList->Pointscount().ToString(), "Rapid-I");

		Listcount = 0; FirstStep = true; AllowIncrement = false;
		double Dist_Z = (float)MasterLength / (float)(NumberofLevels - 1); //200 / NumberofLevels;
		bool PositiveYSide = true;
		StartToothStepNumber = 0;

		int StartPtNumber = 0;

		if (MasterCylinder->PointsList->Pointscount() > 0)// && NoofShapes == TargetShapeNo)
		{
			for (PC_ITER SptItem = MasterCylinder->PointsList->getList().begin(); SptItem != MasterCylinder->PointsList->getList().end(); SptItem++)
			{
				SinglePoint* Spt = (*SptItem).second;
				startPosition[1] = Spt->Y - CylinderDia / 2 - MAINDllOBJECT->ProbeRadius();
				StartPtNumber = 1;
				break;
			}
		}

		int z_direction = 1;
		int numberofRows = 5;
		double fracX[5] = { 1, 0.293, 0, 0.293, 1 };
		double fracY[5] = { 1, 0.707, 0, -0.707, -1 };
		int dirX[5] = { 0, -2, -1, -2, 0 };
		int dirY[5] = { 1, 1, 0, 1, -1 };

		//startPosition[1] = ReferenceCenter[1];
		double diaFraction = CylinderDia / (float)(numberofRows - 1);

		//Calculate the points at this C-position
		for (int j = StartPtNumber; j < numberofRows; j++)
		{
			if (j == numberofRows - 1)
			{
				ProbePath_LstEntity = gcnew PointsToGo();
				ProbePath_LstEntity->Pt_X = startPosition[0] + fracX[j - 1] * CylinderDia / 2 + dirX[j] * extraDist;
				ProbePath_LstEntity->Pt_Y = startPosition[1] + CylinderDia / 2 + dirY[j] * extraDist;
				ProbePath_LstEntity->Pt_Z = startPosition[2];
				ProbePath_LstEntity->Pt_R = startPosition[3];
				ProbePath_LstEntity->path = true;
				PointsToGoList->Add(ProbePath_LstEntity);
			}

			//Let us move in X and Y to go to the current row
			//Move in Y-axis by a fraction proportional to the number of rows!
			cPosition[0] = startPosition[0] + fracX[j] * CylinderDia / 2 + dirX[j] * extraDist;
			cPosition[1] = startPosition[1] + fracY[j] * CylinderDia / 2 + dirY[j] * extraDist;
			ProbePath_LstEntity = gcnew PointsToGo();
			ProbePath_LstEntity->Pt_X = cPosition[0];
			ProbePath_LstEntity->Pt_Y = cPosition[1];
			ProbePath_LstEntity->Pt_Z = startPosition[2];
			ProbePath_LstEntity->Pt_R = startPosition[3];
			ProbePath_LstEntity->path = true;
			PointsToGoList->Add(ProbePath_LstEntity);
			//Set of commands to touch the cylinder and take a point
			ProbePath_LstEntity = gcnew PointsToGo();
			ProbePath_LstEntity->Pt_X = cPosition[0] - dirX[j] * extraDist;
			ProbePath_LstEntity->Pt_Y = startPosition[1] + fracY[j] * CylinderDia / 2; // + MAINDllOBJECT->ProbeRadius());
			ProbePath_LstEntity->Pt_Z = startPosition[2];
			ProbePath_LstEntity->Pt_R = startPosition[3];
			ProbePath_LstEntity->path = false;
			PointsToGoList->Add(ProbePath_LstEntity);

		}

		//Move in X to start position
		ProbePath_LstEntity = gcnew PointsToGo();
		ProbePath_LstEntity->Pt_X = startPosition[0];
		ProbePath_LstEntity->Pt_Y = cPosition[1];
		ProbePath_LstEntity->Pt_Z = startPosition[2];
		ProbePath_LstEntity->Pt_R = startPosition[3];
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);

		startPosition[3] += Move_CAxis_by_ThisAngle;

		//Get back to start position
		ProbePath_LstEntity = gcnew PointsToGo();
		ProbePath_LstEntity->Pt_X = startPosition[0] - extraDist;
		ProbePath_LstEntity->Pt_Y = startPosition[1];
		ProbePath_LstEntity->Pt_Z = startPosition[2];
		ProbePath_LstEntity->Pt_R = startPosition[3];
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);

		RWrapper::RW_DelphiCamModule::MYINSTANCE()->WritePositionCommandstoFile("5-Point Master Cylinder Calibration");
		//Start RUnning all the points in CNC
		SendDrotoNextposition();
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule007", ex);
	}

}

void RWrapper::RW_DelphiCamModule::Set_C_Axis()
{
	try
	{
		System::IO::StreamReader^ pointsFile;
		System::String^ SfilePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath;
		SfilePath = SfilePath + "\\Log\\AxisLog\\MasterCylinder.csv";
		//pin_ptr<const wchar_t> wch = PtrToStringChars(SfilePath);
		System::String^ currentLine;
		pointsFile = gcnew System::IO::StreamReader(SfilePath);
		while (!pointsFile->EndOfStream)
		{
			currentLine = pointsFile->ReadLine();
		}
		pointsFile->Close();
		cli::array<String^>^ Values;
		String^ delimStr = ",";
		cli::array<Char>^ delimiter = delimStr->ToCharArray();
		Values = currentLine->Split(delimiter);
		for (int i = 0; i < 7; i++)
		{
			MAINDllOBJECT->TIS_CAxis[i] = Convert::ToDouble(Values[i]);
			//MAINDllOBJECT->BC_Axis[i] = Convert::ToDouble(Values[i]);
			BC_Values[i] = Convert::ToDouble(Values[i]);
		}
		//Set Runout as 0 so that we can do indexing without runout. 
		for (int i = 0; i < 6; i++)
		{
			MAINDllOBJECT->BC_Axis[i] = 0; // Hob_RunoutParams[i];
		}

		GotAxis = true;
		Getting_BC_Axis = false;
		DoingCalibration = false;
		ReferenceCenter[0] = MAINDllOBJECT->TIS_CAxis[0];
		ReferenceCenter[1] = MAINDllOBJECT->TIS_CAxis[1];
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule008", ex);
	}
}

void RWrapper::RW_DelphiCamModule::Set_Runout()
{
	try
	{
		System::IO::StreamReader^ pointsFile;
		System::String^ SfilePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath;
		SfilePath = SfilePath + "\\Log\\AxisLog\\Runout.csv";
		//pin_ptr<const wchar_t> wch = PtrToStringChars(SfilePath);
		System::String^ currentLine1;
		System::String^ currentLine2;
		System::String^ currentLine3;
		System::String^ currentLine4;
		System::String^ currentLine5;
		pointsFile = gcnew System::IO::StreamReader(SfilePath);
		cli::array<String^>^ Values;
		cli::array<String^>^ StoredValues;
		String^ delimStr = ",";
		cli::array<Char>^ delimiter = delimStr->ToCharArray();

		while (!pointsFile->EndOfStream)
		{
			currentLine1 = pointsFile->ReadLine();
			if (currentLine1->Contains("X")) //Values->Length == 8)
			{
				//StoredValues = currentLine1->Split(delimiter);			
				currentLine2 = pointsFile->ReadLine();
				currentLine3 = pointsFile->ReadLine();
				currentLine4 = pointsFile->ReadLine();
				//currentLine5 = pointsFile->ReadLine();
			}
		}
		pointsFile->Close();
		Values = currentLine2->Split(delimiter);
		for (int i = 0; i < 6; i++)
		{
			MAINDllOBJECT->TIS_CAxis[i] = Convert::ToDouble(Values[i]);
			BC_Values[i] = Convert::ToDouble(Values[i]);
		}
		Values = currentLine3->Split(delimiter);
		for (int i = 0; i < 6; i++)
		{
			//MAINDllOBJECT->TIS_CAxis[i] = Convert::ToDouble(Values[i]);
			Hob_RunoutParams[i] = Convert::ToDouble(Values[i]);
			MAINDllOBJECT->BC_Axis[i] = Convert::ToDouble(Values[i]);
		}
		//MAINDllOBJECT->TIS_CAxis[6] = Convert::ToDouble(Values[6]);
		//Values = currentLine3->Split(delimiter);
		//for (int ii = 0; ii < 6; ii++)
		//{
		//	Hob_RunoutParams[ii] = Convert::ToDouble(Values[ii]);
		//	MAINDllOBJECT->BC_Axis[ii] = Convert::ToDouble(Values[ii]);
		//}
		GotAxis = true;
		Getting_BC_Axis = false;
		DoingCalibration = false;
		ReferenceCenter[0] = MAINDllOBJECT->TIS_CAxis[0];
		ReferenceCenter[1] = MAINDllOBJECT->TIS_CAxis[1];
		RunoutCorrectionStage = 1000;
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule009", ex);
	}
}

double currentZ = 0.0;
bool DoingRotaryRunout = false;

bool RWrapper::RW_DelphiCamModule::Run_RunoutCorrection(int StepNumber, int ptsCt, bool Y_Direction)
{
	//This is the main function that will be called from outside for running runout correction or from the GotoNextPosition..
	// = 0 not running; = 1 running level 1; = 2 running level 2; = 100 completed and working; = -1 not done;
	bool finished = false;
	RunoutCorrectionStage = StepNumber;
	double centreval[6] = {0};

	switch(StepNumber)
	{
	case 1:
		try
		{
			currentZ = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
			//if (topC == NULL)
			//{
			//	RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Circle");
			//	topC = (Circle*)MAINDllOBJECT->getShapesList().selectedItem();
			//}
			//else
			//{
			//	//int index = (Shape*);
			//	MAINDllOBJECT->selectShape(topC->getId(), false);
			//	RWrapper::RW_MainInterface::MYINSTANCE()->HandleShapeParameter_Click("Reset Points");
			//}

			if (topC->PointsList->Pointscount() > 1)// && NoofShapes == TargetShapeNo)
			{
				//for (PC_ITER SptItem = topC->PointsList->getList().begin(); SptItem != topC->PointsList->getList().end(); SptItem++)
				//{
					SinglePoint* Spt = topC->PointsList->getList().at(1); // (*SptItem).second;
					startPosition[1] = Spt->Y - CylinderDia / 2 - MAINDllOBJECT->ProbeRadius();
				//	//StartPtNumber = 1;
				//	break;
				//}
			}

			if (MAINDllOBJECT->TIS_CAxis[1] == 0)
			{
				// ReferenceCenter[0] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0];// + HobParameters[3] / 2;
				ReferenceCenter[1] = startPosition[1];
				//ReferenceCenter[1] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1];
			}
			ManualPoint = false;

			////CalculatePtsfor_Runout(ptsCt, currentZ);
			//CalculatePts_NoRotation(currentZ, 1, true);
			//Hub_Position_7Pts(HobSettings[3], false, false);
			Hub_Position_3Pts(HobSettings[3], false, false);

			RWrapper::RW_DelphiCamModule::MYINSTANCE()->WritePositionCommandstoFile("Runout Correction XY Top Level");
			DoingCalibration = true; Getting_BC_Axis = true;
			////Start Running all the points in CNC
			SendDrotoNextposition();
			//Move_To_NextHub(Y_Direction);
			//SendDrotoNextposition();
			finished = true;
		}
		catch (Exception^ ex)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule010-1", ex);
		}
		break;

	case 2:
		try
		{
			currentZ = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
			//SelectShapeforThisRun((ShapeWithList*)topC_RO, "Cloud Points");
			if (topC_RO == NULL)
			{
				RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Cloud Points");
				topC_RO = (CloudPoints*)MAINDllOBJECT->getShapesList().selectedItem();
			}
			else
			{
				//int index = (Shape*);
				MAINDllOBJECT->selectShape(topC_RO->getId(), false);
				RWrapper::RW_MainInterface::MYINSTANCE()->HandleShapeParameter_Click("Reset Points");
			}

			if (Y_Direction)
				CalculatePtsfor_Runout_Y(12, currentZ, HobParameters[14]);
			else
				CalculatePtsfor_Runout(12, currentZ);

			RWrapper::RW_DelphiCamModule::MYINSTANCE()->WritePositionCommandstoFile("Runout Correction Rotation Top Level");

			//CalculatePts_NoRotation(currentZ, 1);
			DoingCalibration = true; Getting_BC_Axis = true;
			DoingRotaryRunout = true;
			//Start Running all the points in CNC
			SendDrotoNextposition();
			finished = true;
		}
		catch (Exception^ ex)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule010-2", ex);
		}
		break;
			
	case 3: //Run at level one or level 2. it will automatically move to correct position!!
		//If we do rotary runout we will be in centre at X, unless we are doing runout in Y. 
		try
		{
			if (DoingRotaryRunout)
			{
				if (Y_Direction)
					Move_To_NextHub(true);
				else
					Move_To_NextHub(false);
			}
			else
				Move_To_NextHub(true);

			RWrapper::RW_DelphiCamModule::MYINSTANCE()->WritePositionCommandstoFile("Runout Correction Move to Bottom");

			SendDrotoNextposition();
			finished = true;
		}
		catch (Exception^ ex)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule010-3", ex);
		}
		break;

	case 4:
		try{
			currentZ = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
			//SelectShapeforThisRun((ShapeWithList*)botC_RO, "Cloud Points");
			if (botC_RO == NULL)
			{
				RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Cloud Points");
				botC_RO = (CloudPoints*)MAINDllOBJECT->getShapesList().selectedItem();
			}
			else
			{
				//int index = (Shape*);
				MAINDllOBJECT->selectShape(botC_RO->getId(), false);
				RWrapper::RW_MainInterface::MYINSTANCE()->HandleShapeParameter_Click("Reset Points");
			}
			if (Y_Direction)
				CalculatePtsfor_Runout_Y(12, currentZ, HobParameters[14]);
			else
				CalculatePtsfor_Runout(12, currentZ);

			RWrapper::RW_DelphiCamModule::MYINSTANCE()->WritePositionCommandstoFile("Runout Correction Rotation Bottom");
			//CalculatePts_NoRotation(currentZ, -1);

			DoingCalibration = true; Getting_BC_Axis = true;
			//Start Running all the points in CNC
			SendDrotoNextposition();
			finished = true;
		}
		catch (Exception^ ex)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule010-4", ex);
		}
		break;
	
	case 5:
		try
		{
			currentZ = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
			if (botC == NULL)
			{
				RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Circle");
				botC = (Circle*)MAINDllOBJECT->getShapesList().selectedItem();
			}
			else
			{
				MAINDllOBJECT->selectShape(botC->getId(), false);
				RWrapper::RW_MainInterface::MYINSTANCE()->HandleShapeParameter_Click("Reset Points");
			}
			//if (centreval[2] == 0) centreval[2] = currentZ;
			//currentZ = centreval[2] - (HobParameters[9] + HobParameters[17]) / 2;
			////CalculatePtsfor_Runout(ptsCt, currentZ);
			//CalculatePts_NoRotation(currentZ, -1, false);

			//for (int i = 0; i < 4; i++)
			//	startPosition[i] = RW_DRO::MYINSTANCE()->DROCoordinate[i];
			startPosition[2] = RW_DRO::MYINSTANCE()->DROCoordinate[2];

			Hub_Position_3Pts(HobSettings[3], true, true);
			//Hub_Position_7Pts(HobSettings[3], true, true);

			DoingCalibration = true; Getting_BC_Axis = true;
			//Get back to home position
			RWrapper::PointsToGo^ ProbePath_LstEntity;
			ProbePath_LstEntity = gcnew PointsToGo();
			ProbePath_LstEntity->Pt_X = ReferenceCenter[0] - HobParameters[3] / 2 - 10;
			ProbePath_LstEntity->Pt_Y = ReferenceCenter[1] + HobParameters[14] / 2 + 5;
			ProbePath_LstEntity->Pt_Z = currentZ;
			ProbePath_LstEntity->Pt_R = startPosition[3];
			ProbePath_LstEntity->path = true;
			PointsToGoList->Add(ProbePath_LstEntity);


			ProbePath_LstEntity = gcnew PointsToGo();
			ProbePath_LstEntity->Pt_X = ReferenceCenter[0] - HobParameters[3] / 2 - 10;
			ProbePath_LstEntity->Pt_Y = ReferenceCenter[1];
			ProbePath_LstEntity->Pt_Z = currentZ + (HobParameters[9] + HobParameters[17]) / 2; ///4
			ProbePath_LstEntity->Pt_R = 0;
			ProbePath_LstEntity->path = true;
			PointsToGoList->Add(ProbePath_LstEntity);

			RWrapper::RW_DelphiCamModule::MYINSTANCE()->WritePositionCommandstoFile("Runout Correction XY Bottom");

			//Start Running all the points in CNC
			SendDrotoNextposition();
			//DelphiCamEvent::raise(1.0);
			finished = true;
		}
		catch (Exception^ ex)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule010-5", ex);
		}
		break;
				

	case -1: //Reset Runout Values
		topC = NULL;
		botC = NULL;
		DoingCalibration = false;
		try
		{
			RWrapper::RW_DelphiCamModule::MYINSTANCE()->Set_C_Axis();
		}
		catch(...)
		{
		}
		finished = true;
		break;

	case 1000: //Runout complete. Lets do the calculations
		
		double axispts[6] = {0.0};
		Getting_BC_Axis = false;
		FinishingRunoutCalculations = true;
		//topC->GetShapeCenter(&axispts[0]);
		//botC->GetShapeCenter(&axispts[3]);
		double axisLine[6] = {0};
		//BESTFITOBJECT->Line_BestFit_3D(axispts, 2, axisLine);
		//if (axisLine[0] != axisLine[0] || axisLine[1] != axisLine[1] || axisLine[2] != axisLine[2])
		//{
		//	MessageBox::Show("There was an error calculating the runout. Please try again.", "Rapid-I");
		//}
		//else
		//{
		//	for (int i = 0; i < 6; i ++)
		//	{
		//		MAINDllOBJECT->TIS_CAxis[i] = axisLine[i];
		//		if (i < 4)
		//			ReferenceCenter[i] = axisLine[i];
		//	}
		//	MAINDllOBJECT->TIS_CAxis[6] = topC->Radius() * 2; //topRunout[0]; //
		//	
		//}
		//MessageBox::Show("Started Runout calculation and writing in DLL", "Rapid-I");

		double topRunout[6] = {0.0};
		double botRunout[6] = {0.0};
		double topPt[3] = {0.0};
		double BotPt[3] = {0.0};
		try
		{
			//MessageBox::Show("started Runout Calc", "Rapid-I");
			//if (!RWrapper::RW_DelphiCamModule::MYINSTANCE()->GetRunoutforShape((ShapeWithList*)topC, 0, topPt, false))
			//{
			//	MessageBox::Show("Could not calculate centre of top hub. Please try again.", "Rapid-I");
			//}
			//if (!RWrapper::RW_DelphiCamModule::MYINSTANCE()->GetRunoutforShape((ShapeWithList*)botC, 0, BotPt, false))
			//{
			//	MessageBox::Show("Could not calculate centre of bottom hub. Please try again.", "Rapid-I");
			//}
			//Set the Z positions of top and bottom
			topPt[0] = topC->getCenter()->getX(); topPt[1] = topC->getCenter()->getY(); topPt[2] = topC->getCenter()->getZ();
			BotPt[0] = botC->getCenter()->getX(); BotPt[1] = botC->getCenter()->getY(); BotPt[2] = botC->getCenter()->getZ();

			axispts[5] = currentZ;
			axispts[2] = axispts[5] + (HobParameters[9] + HobParameters[17])/2;
			
			//Set Main Between Centres Axes
			for (int i = 0; i < 2; i++)
			{
				MAINDllOBJECT->TIS_CAxis[i] = topPt[i];
				BC_Values[i] = topPt[i];
			}
			MAINDllOBJECT->TIS_CAxis[2] = axispts[2];
			MAINDllOBJECT->TIS_CAxis[3] = 0;
			MAINDllOBJECT->TIS_CAxis[4] = 0;
			MAINDllOBJECT->TIS_CAxis[5] = 1;
			MAINDllOBJECT->TIS_CAxis[6] = topPt[2];


			//We got one point on the Runout Circle on top with topC (5-pt XY movement circle)
			//We also have position of where maximal runout is there. From this we calculate the actual centre
			// x0 = x1 - R* Cos theta. R = runout radius from sinusoid, and theta = theta0 from sinusoid
			axispts[0] = topPt[0] - topRunout[2] * cos(topRunout[1]) - MAINDllOBJECT->TIS_CAxis[0];
			axispts[1] = topPt[1] - topRunout[2] * sin(topRunout[1]) - MAINDllOBJECT->TIS_CAxis[1];

			//Repeat for bottom 
			axispts[3] = BotPt[0] - botRunout[2] * cos(botRunout[1]) - MAINDllOBJECT->TIS_CAxis[0];
			axispts[4] = BotPt[1] - botRunout[2] * sin(botRunout[1]) - MAINDllOBJECT->TIS_CAxis[1];

			//MessageBox::Show("Completed 1", "Rapid-I");

			//Equation of X axis runout
			Hob_RunoutParams[0] = (axispts[3] - axispts[0]) / (axispts[5] - axispts[2]); // / 2;
			Hob_RunoutParams[1] = axispts[0] - Hob_RunoutParams[0] * axispts[2];
			//Equation of Y_axis runout.
			Hob_RunoutParams[2] = (axispts[4] - axispts[1]) / (axispts[5] - axispts[2]); // / 2;
			Hob_RunoutParams[3] = axispts[1] - Hob_RunoutParams[2] * axispts[2];

			//Do Runout calculations if reuqired...
			if (DoingRotaryRunout)
			{
				if (!RWrapper::RW_DelphiCamModule::MYINSTANCE()->GetRunoutforShape((ShapeWithList*)topC_RO, 0, topRunout, true))
				{
					MessageBox::Show("Could not calculate runout at top hub. Please try again.", "Rapid-I");
				}
				if (!RWrapper::RW_DelphiCamModule::MYINSTANCE()->GetRunoutforShape((ShapeWithList*)botC_RO, 0, botRunout, true))
				{
					MessageBox::Show("Could not calculate runout at bottom hub. Please try again.", "Rapid-I");
				}
			}
			//Expose the Runouts at both levels
			Hob_RunoutParams[4] = 2 * topRunout[2];
			Hob_RunoutParams[5] = 2 * botRunout[2];
			for (int i = 0; i < 6; i++)
			{
				MAINDllOBJECT->BC_Axis[i] = Hob_RunoutParams[i];
			}

			GotAxis = true;
			Getting_BC_Axis = false;
			DoingCalibration = false;
			ReferenceCenter[0] = MAINDllOBJECT->TIS_CAxis[0];
			ReferenceCenter[1] = MAINDllOBJECT->TIS_CAxis[1];

			finished = true;
		}
		catch(...)
		{
			MessageBox::Show("There was a problem in calculating rotary runout", "Rapid-I");
		}

		try
		{
			//MessageBox::Show("Started Writing 2", "Rapid-I");			
			//Write the values to file and store data for each run.
			System::String^ SfilePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath;
			SfilePath = SfilePath + "\\Log\\AxisLog\\Runout.csv";
			System::IO::StreamWriter^ pointsFile = gcnew System::IO::StreamWriter(SfilePath, true);

			//ofstream pointsFile;
			//System::String^ SfilePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath;
			System::String^ currentTime = Microsoft::VisualBasic::DateAndTime::Now.ToShortDateString() + ", " + Microsoft::VisualBasic::DateAndTime::Now.ToString("HH:mm:ss");
			//SfilePath = SfilePath + "\\Log\\AxisLog\\Runout.csv";
			//pin_ptr<const wchar_t> wch = PtrToStringChars(SfilePath);
			//pin_ptr<const wchar_t> wch2 = PtrToStringChars(currentTime);
			//pointsFile.open(wch, ios::app);
			pointsFile->WriteLine(currentTime); // << wch2 << endl;
			pointsFile->WriteLine( "X" + ", " + "Y " + ", " + "Z" + ", " + "L" + ", " + "M" + ", " + "N" + ", " + "Dia1" + ", " + "Dia2"); // << endl;
			for (int ii = 0; ii < 7; ii++)
				pointsFile->Write(MAINDllOBJECT->TIS_CAxis[ii] + ", "); 
			//pointsFile->Write(botRunout[0]) ; 
			//pointsFile->Write(botC->Radius() * 2 + ", ");
			pointsFile->WriteLine(); // << endl;
			for (int ii = 0; ii < 6; ii++)
				pointsFile->Write(Hob_RunoutParams[ii] + ", "); 
			pointsFile->WriteLine(); // << endl;
			for (int ii = 0; ii < 3; ii++)
				pointsFile->Write(topRunout[ii] + ", "); 
			pointsFile->WriteLine(); // << endl;
			for (int ii = 0; ii < 3; ii++)
				pointsFile->Write(botRunout[ii] + ", "); 
			pointsFile->WriteLine(); // << endl;
			for (int ii = 0; ii < 3; ii++)
				pointsFile->Write(topPt[ii] + ", ");
			pointsFile->WriteLine(); // << endl;
			for (int ii = 0; ii < 3; ii++)
				pointsFile->Write(BotPt[ii] + ", ");
			pointsFile->WriteLine(); // << endl;
			//MessageBox::Show("Completed Runout calculation and writing to file", "Rapid-I");
		
			pointsFile->Close();
		}
		catch(...)
		{
			MessageBox::Show("There was a problem in writing runout values to file", "Rapid-I");
		}
		FinishingRunoutCalculations = false;
		//Finished work. Call stop process to reset all flags and CNC
		//Getting_BC_Axis = false;
		//StopProcess();
	}
	return finished;
}

void RWrapper::RW_DelphiCamModule::CalculatePtsfor_Runout(int ptsCount, double Z_Level)
{
	//Calculate the points coordinates for giving commands in CNC

	//If already running something, quit the command!!
	try
	{
		if (DoingHobMeasurement || !HobParametersUpdated) return;

		RWrapper::PointsToGo^ ProbePath_LstEntity;
		PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
		double extraDist = 6.0 - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;

		Listcount = 0; FirstStep = true;
		startPosition = gcnew cli::array<double>(4);
		cPosition = gcnew cli::array<double>(4);
		DoingCalibration = true;

		for (int i = 0; i < 4; i++)
		{
			startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
			cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		}
		//X axis will always be one radius + probe retract position to the left of the BC axis centre in X
		startPosition[0] = ReferenceCenter[0] - HobParameters[14] / 2 - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue - MAINDllOBJECT->ProbeRadius();// - 1.0;
		startPosition[1] = ReferenceCenter[1];
		startPosition[2] = Z_Level;
		cPosition[3] = 0;

		if (ptsCount < 3) ptsCount = 12;

		int rotateDir = 1;
		//if (startPosition[3] > 0) cPosition[3] = startPosition[3] - 2 * M_PI;
		//First Step - Go to Start Position 
		//Check where in Y we are. 
		//if (abs(cPosition[1] - startPosition[1]) > 2)
		//{
		ProbePath_LstEntity = gcnew PointsToGo();
		ProbePath_LstEntity->Pt_X = startPosition[0];
		ProbePath_LstEntity->Pt_Y = cPosition[1];
		ProbePath_LstEntity->Pt_Z = startPosition[2];
		ProbePath_LstEntity->Pt_R = cPosition[3];
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);

		ProbePath_LstEntity = gcnew PointsToGo();
		ProbePath_LstEntity->Pt_X = startPosition[0];
		ProbePath_LstEntity->Pt_Y = startPosition[1];
		ProbePath_LstEntity->Pt_Z = startPosition[2];
		ProbePath_LstEntity->Pt_R = cPosition[3];
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);

		//}

		//if current Z is not at target Z level, 
		double Z_Diff = abs(cPosition[2] - Z_Level);
		if (Z_Diff < 2.0 && Z_Diff > 0.1)
		{
			//We are very close to target position. Hence move directly, only in Z. 
			ProbePath_LstEntity = gcnew PointsToGo();
			ProbePath_LstEntity->Pt_X = startPosition[0];
			ProbePath_LstEntity->Pt_Y = startPosition[1];
			ProbePath_LstEntity->Pt_Z = startPosition[2];
			ProbePath_LstEntity->Pt_R = cPosition[3];
			ProbePath_LstEntity->path = true;
			PointsToGoList->Add(ProbePath_LstEntity);
		}
		else if (Z_Diff > 2.0)
		{
			////first move out in X and Y to outside the hob OD and to Y centre
			//ProbePath_LstEntity = gcnew PointsToGo();											
			//ProbePath_LstEntity->Pt_X = ReferenceCenter[0] - HobParameters[3] / 2 - extraDist;	
			//ProbePath_LstEntity->Pt_Y = startPosition[1];
			//ProbePath_LstEntity->Pt_Z = cPosition[2];
			//ProbePath_LstEntity->Pt_R = cPosition[3];
			//ProbePath_LstEntity->path = true;
			//PointsToGoList->Add(ProbePath_LstEntity);

			//Now move in Z at current XY position
			ProbePath_LstEntity = gcnew PointsToGo();
			ProbePath_LstEntity->Pt_X = ReferenceCenter[0] - HobParameters[3] / 2 - extraDist;
			ProbePath_LstEntity->Pt_Y = startPosition[1];
			ProbePath_LstEntity->Pt_Z = Z_Level;
			ProbePath_LstEntity->Pt_R = cPosition[3];
			ProbePath_LstEntity->path = true;
			PointsToGoList->Add(ProbePath_LstEntity);

			//Now move in X to correct startposition
			ProbePath_LstEntity = gcnew PointsToGo();
			ProbePath_LstEntity->Pt_X = startPosition[0];
			ProbePath_LstEntity->Pt_Y = startPosition[1];
			ProbePath_LstEntity->Pt_Z = Z_Level;
			ProbePath_LstEntity->Pt_R = cPosition[3];
			ProbePath_LstEntity->path = true;
			PointsToGoList->Add(ProbePath_LstEntity);
		}

		//Now we are sure we are at correct starting position
		//We can start taking points and rotating! We have two steps for each point 
		//one to go and take a point, and next to rotate to next pt!
		double RotateAngle = 2 * M_PI / ptsCount;

		for (int i = 0; i < ptsCount; i++)
		{
			cPosition[0] = startPosition[0] + RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;// + MAINDllOBJECT->ProbeRadius(); //+ 1.0;
			//Now move in X to correct startposition
			ProbePath_LstEntity = gcnew PointsToGo();
			ProbePath_LstEntity->Pt_X = cPosition[0];
			ProbePath_LstEntity->Pt_Y = startPosition[1];
			ProbePath_LstEntity->Pt_Z = Z_Level;
			ProbePath_LstEntity->Pt_R = cPosition[3];
			ProbePath_LstEntity->path = false;
			PointsToGoList->Add(ProbePath_LstEntity);

			if (i == ptsCount - 1) continue;
			cPosition[3] += RotateAngle * rotateDir;
			//Now move in X to correct startposition
			ProbePath_LstEntity = gcnew PointsToGo();
			ProbePath_LstEntity->Pt_X = startPosition[0] - MAINDllOBJECT->ProbeRadius();
			ProbePath_LstEntity->Pt_Y = startPosition[1];
			ProbePath_LstEntity->Pt_Z = Z_Level;
			ProbePath_LstEntity->Pt_R = cPosition[3];
			ProbePath_LstEntity->path = true;
			PointsToGoList->Add(ProbePath_LstEntity);
		}
		ProbePath_LstEntity = gcnew PointsToGo();
		ProbePath_LstEntity->Pt_X = ReferenceCenter[0] - HobParameters[3] / 2 - extraDist;
		ProbePath_LstEntity->Pt_Y = startPosition[1];
		ProbePath_LstEntity->Pt_Z = Z_Level;
		ProbePath_LstEntity->Pt_R = 0;
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule011", ex);
	}
}

void RWrapper::RW_DelphiCamModule::Run_StartToothParameters(int StepNumber)
{
	//We make a cloud pt shape and calculate the Z-axis centre of tooth, and the tip OD of the tooth as well. 
	//First Step, the user will touch at approx the centre of the tooth. After rotating the hob to touch at 
	//Y-centre - Probe-Radius. then the touch probe may be outside the tooth. 
	try
	{	//We start similar to Form&Pos of Cutting Face measurements 
		InPauseState = false;
		DoingCalibration = true;
		firsttime = true;
		Getting_BC_Axis = false;
		DoingHobMeasurement = true;
		lastPoint = NULL;
		StartToothStepNumber = StepNumber;
		int ii = 0;

		switch (StepNumber)
		{
		case 1: // take points on top and bottom of pressure angle faces. 
			CalculatePts_StartToothCentre();
			//Start Running all the points in CNC
			SendDrotoNextposition();
			break;
		case 201:
			CalculatePts_for_C_Axis_Zero();

			RWrapper::RW_DelphiCamModule::MYINSTANCE()->WritePositionCommandstoFile("First Tooth Rotation Homing");

			//Start Running all the points in CNC
			SendDrotoNextposition();
			break;

		case 101: // Take 0 point of rotation... 
			CalculatePts_for_C_Axis_Zero();

			RWrapper::RW_DelphiCamModule::MYINSTANCE()->WritePositionCommandstoFile("First Tooth Rotation Homing");

			//Start Running all the points in CNC
			SendDrotoNextposition();
			break;

		case 102:
			//RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2); //Inactivate CNC to take the point into current shape!
			Sleep(300);
			//RWrapper::RW_CNC::MYINSTANCE()->Activate_AbortDRO(); //Inactivate CNC to take the point into current shape!
			//Sleep(30);
			//RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1); //Inactivate CNC to take the point into current shape!
			//while (ii < 100000000)
			//{
			//	ii++;
			//	if (FirstToothPts->PointsList->Pointscount() > 0)
			//		break;
			//}
			//String^ ss;
			if (FirstToothPts->PointsList->Pointscount() > 0)// && NoofShapes == TargetShapeNo)
			{
				for (PC_ITER SptItem = FirstToothPts->PointsList->getList().begin(); SptItem != FirstToothPts->PointsList->getList().end(); SptItem++)
				{
					SinglePoint* Spt = (*SptItem).second;
					ToothMidPt[3] = Spt->R; //Pts[index * 2 + 1] = Spt->Z;
					//ss = Spt->R.ToString();
					break;
				}

			}
			else
			{
				ToothMidPt[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]; //Pts[index * 2 + 1] = Spt->Z;
			}
			//ss = ss + ", " + ToothMidPt[3].ToString();
			//System::Windows::MessageBox::Show(ss);
			//RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
			CalculatePts_StartToothCentre_2Pts();

			RWrapper::RW_DelphiCamModule::MYINSTANCE()->WritePositionCommandstoFile("First Tooth Mid Pt Process");

			//Start Running all the points in CNC
			SendDrotoNextposition();
			break;

		case 103:
			//ShapeWithList* CShape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
			if (FirstToothPts->PointsList->Pointscount() >= 1)// && NoofShapes == TargetShapeNo)
			{
				if (FirstToothPts->PointsList->Pointscount() == 1)
				{
					for (PC_ITER SptItem = FirstToothPts->PointsList->getList().begin(); SptItem != FirstToothPts->PointsList->getList().end(); SptItem++)
					{
						SinglePoint* Spt = (*SptItem).second;
						ToothMidPt[2] = Spt->Z; //Pts[index * 2 + 1] = Spt->Z;
						ToothMidPt[3] = Spt->R;
						break;
					}
				}
				else
				{
					double Pts[2] = { 0 };
					int index = 0;
					for (PC_ITER SptItem = FirstToothPts->PointsList->getList().begin(); SptItem != FirstToothPts->PointsList->getList().end(); SptItem++)
					{
						if (index < 2)
						{
							SinglePoint* Spt = (*SptItem).second;
							Pts[index] = Spt->Z; //Pts[index * 2 + 1] = Spt->Z;
							index++;
						}
					}
					ToothMidPt[2] = (Pts[0] + Pts[1]) / 2;
				}
				CalculatePts_StartTooth_OD(ToothMidPt[2]);

				RWrapper::RW_DelphiCamModule::MYINSTANCE()->WritePositionCommandstoFile("First Tooth OD Checking");

				SendDrotoNextposition();
			}
			else
			{
				MessageBox::Show("Could not calculate the position of the initial tooth. Please try again!", "Rapid-I");
			}
			break;
		case 2: // from the pressure angle faces, calculate the correct Z position of centre of tooth, and take one pt for OD
			//Generate the lines of the pressure angle from the points stored inside the cloud pt shape
		{	ShapeWithList* CShape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		if (CShape->PointsList->Pointscount() >= 4)// && NoofShapes == TargetShapeNo)
		{
			double LP1[4] = { 0 };
			double LP2[4] = { 0 };
			int index = 0;
			for (PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
			{
				SinglePoint* Spt = (*SptItem).second;
				if (index < 2)
				{
					LP1[2 * index] = Spt->X; LP1[2 * index + 1] = Spt->Z; // take the X and Z values since we want the Z axis mid-point
				}
				else
				{
					int ii = index - 2;
					LP2[2 * ii] = Spt->X; LP2[2 * ii + 1] = Spt->Z;
				}
				index++;
			}
			double LParam1[2] = { 0 };
			double LParam2[2] = { 0 };

			BESTFITOBJECT->Line_BestFit_2D(LP1, 2, LParam1, false);
			BESTFITOBJECT->Line_BestFit_2D(LP2, 2, LParam2, false);
			double intersectPt[4] = { 0 };
			//Get intersection pt in the XZ plane
			RMATH2DOBJECT->Line_lineintersection(LP1[0], LP1[1], LP2[0], LP2[1], intersectPt);
			ToothMidPt[2] = intersectPt[1];
			CalculatePts_StartTooth_OD(ToothMidPt[2]);
			SendDrotoNextposition();
		}
		}
		break;

		case 202:
			GetBacktoHomePosition();

			RWrapper::RW_DelphiCamModule::MYINSTANCE()->WritePositionCommandstoFile("Master Home Position Complete");

			//Start Running all the points in CNC
			SendDrotoNextposition();
			break;

		case 1000:

			//Calculate the radius as the distance between centre at this Z and the tip point
			if (FirstToothPts->PointsList->Pointscount() > 2)// && NoofShapes == TargetShapeNo)
			{
				double Ctr[4] = { 0 };
				SinglePoint* Sp = FirstToothPts->PointsList->getList()[2];
				Ctr[0] = Sp->X; Ctr[1] = Sp->Y; Ctr[2] = ToothMidPt[2]; Ctr[3] = ToothMidPt[3];
				RMATH3DOBJECT->UnRotatePoint_Around_C_Axis(Ctr, MAINDllOBJECT->BC_Axis, MAINDllOBJECT->TIS_CAxis, Ctr); //MAINDllOBJECT->BC_Axis,
				ToothMidPt[0] = Ctr[0]; // + ;
				ToothMidPt[1] = Ctr[1];
				if (Ctr[0] == 0.0 && Ctr[1] == 0.0 && Ctr[2] == 0.0)
				{
					HobParameters[15] = HobParameters[3];
				}
				else
				{
					//Got the tip coods. Now get the centre
					double rX[4] = { 0 };
					//Calculate the distance as radius, and assign the OD 
					HobParameters[15] = (RMATH2DOBJECT->Pt2Pt_distance(MAINDllOBJECT->TIS_CAxis, Ctr) - MAINDllOBJECT->ProbeRadius()) * 2;
				}
				FirstToothDone = true;
			}
			StopProcess();
			break;

		case 2000:
			if (FirstToothPts->PointsList->Pointscount() > 0)
			{
				SinglePoint* Sp = FirstToothPts->PointsList->getList()[0];
				ToothMidPt[2] = Sp->Z;
				ToothMidPt[3] = Sp->R;
			}
			else
			{
				ToothMidPt[2] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2]; //Pts[index * 2 + 1] = Spt->Z;
				ToothMidPt[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]; //Pts[index * 2 + 1] = Spt->Z;
			}
			FirstToothDone = true;
			HobParameters[15] = HobParameters[3];
			//Finished all things. Call Stop to clear settings and deselect all shapes made!
			StopProcess();
			break;
		}
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule012", ex);
	}
}

void RWrapper::RW_DelphiCamModule::CalculatePts_StartToothCentre()
{
	RWrapper::PointsToGo^ ProbePath_LstEntity;
	PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
	DoingCalibration = true;	
	Listcount = 0;
	startPosition = gcnew cli::array<double>(4);
	cPosition = gcnew cli::array<double>(4);
	//double rX[4]; 
	//RMATH3DOBJECT->GetC_Axis_XY(startPosition[2], MAINDllOBJECT->TIS_CAxis, rX);
	double MoveByForTouch = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;// + 1;

	for (int i = 0; i < 4; i ++)
	{	startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
	}
	startPosition[0] = ReferenceCenter[0] - (HobParameters[3] / 2) + HobParameters[1];
	startPosition[1] = ReferenceCenter[1] - (HobParameters[4] * MoveByForTouch);
	double PitchValue = abs(M_PI * HobParameters[12]); //* HobParameters[11]); 
	//Calculate estimated centre of tooth from given values in X
	cPosition[0] =  ReferenceCenter[0] - (HobParameters[3] / 2) + 1.125 * HobParameters[12];
	//Move to inside the tooth by about 0.5 mm so that we can touch inside top of tooth. 
	cPosition[1] = ReferenceCenter[1] + HobParameters[4] * MAINDllOBJECT->ProbeRadius();
	//Move up in Z axis by quarter of a pitch + touching distance
	cPosition[2] += PitchValue / 4 + MoveByForTouch;

	//Make a cloud pt entity to hold the points. 
	RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Cloud Points");

	//We first go up in Z
	ProbePath_LstEntity = gcnew PointsToGo();									
	ProbePath_LstEntity->Pt_X =  startPosition[0];	
	ProbePath_LstEntity->Pt_Y =  startPosition[1];
	ProbePath_LstEntity->Pt_Z =  cPosition[2]; 
	ProbePath_LstEntity->Pt_R =  cPosition[3]; 
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);


	//We first go to the top of the tooth at the centre; inside to touch and take one point.
	ProbePath_LstEntity = gcnew PointsToGo();									
	ProbePath_LstEntity->Pt_X =  cPosition[0];	
	ProbePath_LstEntity->Pt_Y =  cPosition[1] ;
	ProbePath_LstEntity->Pt_Z =  cPosition[2] ; 
	ProbePath_LstEntity->Pt_R =  cPosition[3]; 
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);

	//We move down in Z to take a point. 
	ProbePath_LstEntity = gcnew PointsToGo();										
	ProbePath_LstEntity->Pt_X =  cPosition[0];	
	ProbePath_LstEntity->Pt_Y =  cPosition[1] ;
	ProbePath_LstEntity->Pt_Z =  cPosition[2] - MoveByForTouch; 
	ProbePath_LstEntity->Pt_R =  cPosition[3]; 
	ProbePath_LstEntity->path = false;
	PointsToGoList->Add(ProbePath_LstEntity);

	//We move out by quarter of tooth depth in X and then come down in Z according to pressure angle and then take a point. 
	cPosition[0] -= 0.5625 * HobParameters[12];
	cPosition[2] -= 0.5625 * HobParameters[12] * tan(HobParameters[16]);

	ProbePath_LstEntity = gcnew PointsToGo();									
	ProbePath_LstEntity->Pt_X =  cPosition[0];	
	ProbePath_LstEntity->Pt_Y =  cPosition[1] ;
	ProbePath_LstEntity->Pt_Z =  cPosition[2]; 
	ProbePath_LstEntity->Pt_R =  cPosition[3]; 
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);

	//We move down in Z to take a point. 
	ProbePath_LstEntity = gcnew PointsToGo();										
	ProbePath_LstEntity->Pt_X =  cPosition[0];	
	ProbePath_LstEntity->Pt_Y =  cPosition[1] ;
	ProbePath_LstEntity->Pt_Z =  cPosition[2] - MoveByForTouch; 
	ProbePath_LstEntity->Pt_R =  cPosition[3]; 
	PointsToGoList->Add(ProbePath_LstEntity);

	//Move out of tooth
	ProbePath_LstEntity = gcnew PointsToGo();										
	ProbePath_LstEntity->Pt_X =  startPosition[0];	
	ProbePath_LstEntity->Pt_Y =  startPosition[1]; 
	ProbePath_LstEntity->Pt_Z =  cPosition[2]; 
	ProbePath_LstEntity->Pt_R =  cPosition[3]; 
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);

	//Move to bottom face of tooth at this X
	cPosition[2] -= PitchValue / 2 + 2 * MoveByForTouch;
	ProbePath_LstEntity = gcnew PointsToGo();										
	ProbePath_LstEntity->Pt_X =  startPosition[0];	
	ProbePath_LstEntity->Pt_Y =  startPosition[1]; // - (HobParameters[4] * MoveByForTouch);
	ProbePath_LstEntity->Pt_Z =  cPosition[2]; 
	ProbePath_LstEntity->Pt_R =  cPosition[3]; 
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);

	//Move in in Y axis
	ProbePath_LstEntity = gcnew PointsToGo();									
	ProbePath_LstEntity->Pt_X =  cPosition[0];	
	ProbePath_LstEntity->Pt_Y =  cPosition[1] ;
	ProbePath_LstEntity->Pt_Z =  cPosition[2]; 
	ProbePath_LstEntity->Pt_R =  cPosition[3];
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);

	//Move up and take a point
	ProbePath_LstEntity = gcnew PointsToGo();										
	ProbePath_LstEntity->Pt_X =  cPosition[0];	
	ProbePath_LstEntity->Pt_Y =  cPosition[1] ;
	ProbePath_LstEntity->Pt_Z =  cPosition[2] + MoveByForTouch; 
	ProbePath_LstEntity->Pt_R =  cPosition[3]; 
	ProbePath_LstEntity->path = false;
	PointsToGoList->Add(ProbePath_LstEntity);

	//Move up in Z and Move to the right in X to take second pt.
	cPosition[0] += 0.5625 * HobParameters[12];
	cPosition[2] += 0.5625 * HobParameters[12] * tan(HobParameters[16]);

	ProbePath_LstEntity = gcnew PointsToGo();										
	ProbePath_LstEntity->Pt_X =  cPosition[0];	
	ProbePath_LstEntity->Pt_Y =  cPosition[1] ;
	ProbePath_LstEntity->Pt_Z =  cPosition[2]; 
	ProbePath_LstEntity->Pt_R =  cPosition[3]; 
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);

	//We move down in Z to take a point. 
	ProbePath_LstEntity = gcnew PointsToGo();									
	ProbePath_LstEntity->Pt_X =  cPosition[0];	
	ProbePath_LstEntity->Pt_Y =  cPosition[1] ;
	ProbePath_LstEntity->Pt_Z =  cPosition[2] + MoveByForTouch;
	ProbePath_LstEntity->Pt_R =  cPosition[3]; 
	ProbePath_LstEntity->path = false;
	PointsToGoList->Add(ProbePath_LstEntity);

	//We move out in X and Y to start position
	ProbePath_LstEntity = gcnew PointsToGo();							
	ProbePath_LstEntity->Pt_X =  startPosition[0];	
	ProbePath_LstEntity->Pt_Y =  startPosition[1] ;
	ProbePath_LstEntity->Pt_Z =  cPosition[2]; 
	ProbePath_LstEntity->Pt_R =  cPosition[3]; 
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);
}


void RWrapper::RW_DelphiCamModule::CalculatePts_StartTooth_OD(double Z_Level)
{
	//We got the correct Z_Level now. 
	RWrapper::PointsToGo^ ProbePath_LstEntity;
	PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();

	startPosition = gcnew cli::array<double>(4);
	cPosition = gcnew cli::array<double>(4);
	Listcount = 0;
	//double rX[4]; 
	//RMATH3DOBJECT->GetC_Axis_XY(startPosition[2], MAINDllOBJECT->TIS_CAxis, rX);
	double MoveByForTouch = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue + 1;

	for (int i = 0; i < 4; i ++)
	{	startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
	}
	startPosition[0] = ReferenceCenter[0] - (HobParameters[3] / 2) - 5.0;
	startPosition[1] = ReferenceCenter[1] + HobParameters[4] * MAINDllOBJECT->ProbeRadius(); // - (HobParameters[4] * MoveByForTouch);
	//Make a 3D point	
	//RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Point3D");

	//We move out in X and Y to start position
	ProbePath_LstEntity = gcnew PointsToGo();			//outer start position for every tooth with appropriate z,r movement							
	ProbePath_LstEntity->Pt_X =  startPosition[0];	
	ProbePath_LstEntity->Pt_Y =  startPosition[1] ;
	ProbePath_LstEntity->Pt_Z =  Z_Level; //ReferenceCenter[2] + (i*Move_inZDirection);
	ProbePath_LstEntity->Pt_R =  cPosition[3]; // + (AngleBWTwoHelicalTooth*i*HobDirectionSign*M_PI/180);
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);

	//We next move to touch the point to take OD pt. 
	ProbePath_LstEntity = gcnew PointsToGo();			//outer start position for every tooth with appropriate z,r movement							
	ProbePath_LstEntity->Pt_X =  ReferenceCenter[0] - (HobParameters[3] / 2) + RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue + 1;;	
	ProbePath_LstEntity->Pt_Y =  startPosition[1] ;
	ProbePath_LstEntity->Pt_Z =  Z_Level; //ReferenceCenter[2] + (i*Move_inZDirection);
	ProbePath_LstEntity->Pt_R =  cPosition[3]; // + (AngleBWTwoHelicalTooth*i*HobDirectionSign*M_PI/180);
	ProbePath_LstEntity->path = false;
	PointsToGoList->Add(ProbePath_LstEntity);

	//We move out in X and Y to start position
	ProbePath_LstEntity = gcnew PointsToGo();			//outer start position for every tooth with appropriate z,r movement							
	ProbePath_LstEntity->Pt_X = startPosition[0];
	ProbePath_LstEntity->Pt_Y = ReferenceCenter[1] - HobParameters[4] * (MAINDllOBJECT->ProbeRadius() + 0.5);
	ProbePath_LstEntity->Pt_Z =  Z_Level; //ReferenceCenter[2] + (i*Move_inZDirection);
	ProbePath_LstEntity->Pt_R =  cPosition[3]; // + (AngleBWTwoHelicalTooth*i*HobDirectionSign*M_PI/180);
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);
}

void RWrapper::RW_DelphiCamModule::CalculatePts_StartToothCentre_2Pts()
{
	RWrapper::PointsToGo^ ProbePath_LstEntity;
	PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
	DoingCalibration = true;
	Listcount = 0;
	startPosition = gcnew cli::array<double>(4);
	cPosition = gcnew cli::array<double>(4);
	//double rX[4]; 
	//RMATH3DOBJECT->GetC_Axis_XY(startPosition[2], MAINDllOBJECT->TIS_CAxis, rX);
	double MoveByForTouch = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;

	for (int i = 0; i < 4; i ++)
	{	startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
	}
	startPosition[0] = MAINDllOBJECT->TIS_CAxis[0] - (HobParameters[3] / 2) - 5.0; //+ HobParameters[1];
	startPosition[1] -= HobParameters[4] * (MoveByForTouch + MAINDllOBJECT->ProbeRadius());
	if (HobParameters[10] != 0)
		startPosition[1] -= HobParameters[4] * (HobParameters[3] * sin(HobParameters[10]) / 2);
	startPosition[3] = ToothMidPt[3];
	cPosition[3] = ToothMidPt[3];

	double PitchValue = abs(M_PI * HobParameters[12]); //* HobParameters[11]); 
	//Calculate estimated centre of tooth from given values in X
	cPosition[0] =  MAINDllOBJECT->TIS_CAxis[0] - (HobParameters[3] / 2) + 2.25 * HobParameters[12] / 2;
	//We move out by quarter of tooth depth in X and then come down in Z according to pressure angle and then take a point. 
	cPosition[0] -= 0.5625 * HobParameters[12];
	//Calculate corrections if gash is helical
	double Move_inZDirection = PitchValue / 2; // 0.5625 * HobParameters[12] * tan(HobParameters[16]);
	double Rotate_AxisForHelicalLead = 0;
	bool rotate_First = false;

	if (HobParameters[13] != 0) 
	{
		double helixRad = (HobParameters[3] / 2) - 1.125 * HobParameters[12];
		double helixAngle = atan(HobParameters[13] / (M_PI * helixRad * 2));
		PitchValue *= sin(helixAngle);
		Move_inZDirection *= sin(helixAngle);
		Rotate_AxisForHelicalLead = Move_inZDirection * 2 * M_PI / HobParameters[13];
	}
	double ed = MAINDllOBJECT->ProbeRadius() / 2;

	//Move up in Z axis by half of a pitch
	cPosition[2] += PitchValue / 2; //Move_inZDirection + MAINDllOBJECT->ProbeRadius() + MoveByForTouch + ed;
	//cPosition[2] -= Move_inZDirection;
	cPosition[1] = startPosition[1];

	if (FirstToothPts == NULL)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Cloud Points");
		FirstToothPts = (CloudPoints*)MAINDllOBJECT->getShapesList().selectedItem();
	}
	else
	{
		//int index = (Shape*);
		MAINDllOBJECT->selectShape(FirstToothPts->getId(), false);
		RWrapper::RW_MainInterface::MYINSTANCE()->HandleShapeParameter_Click("Reset Points");
	}
	if (HobParameters[4] * HobParameters[5] < 0)
		rotate_First = true;

	
	
	if (HobParameters[13] != 0)
	{
		//We have to rotate for helical Gashes by this amount - 
		cPosition[3] -= Rotate_AxisForHelicalLead * (HobParameters[4] * HobParameters[5]);
		//First Step, we move away from the rake in Y and move out of tooth in X
		ProbePath_LstEntity = gcnew PointsToGo();										
		ProbePath_LstEntity->Pt_X =  cPosition[0];	
		ProbePath_LstEntity->Pt_Y =  startPosition[1]; //RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] - HobParameters[4] * MAINDllOBJECT->ProbeRadius();
		if (!rotate_First) 
		{
			ProbePath_LstEntity->Pt_Z =  startPosition[2]; 
			ProbePath_LstEntity->Pt_R =  cPosition[3]; 
			ProbePath_LstEntity->path = true;
		}
		else
		{
			ProbePath_LstEntity->Pt_Z =  cPosition[2]; 
			ProbePath_LstEntity->Pt_R =  startPosition[3]; 
			ProbePath_LstEntity->path = true;
		}
		PointsToGoList->Add(ProbePath_LstEntity);

		ProbePath_LstEntity = gcnew PointsToGo();										
		ProbePath_LstEntity->Pt_X =  cPosition[0];	
		ProbePath_LstEntity->Pt_Y =  startPosition[1]; //RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] - HobParameters[4] * MAINDllOBJECT->ProbeRadius();
		//if (rotate_First) 
		//{
		//	ProbePath_LstEntity->Pt_Z =  cPosition[2]; 
		//	ProbePath_LstEntity->Pt_R =  cPosition[3]; 
		//	ProbePath_LstEntity->path = true;
		//}
		//else
		//{
			ProbePath_LstEntity->Pt_Z =  cPosition[2]; 
			ProbePath_LstEntity->Pt_R =  cPosition[3]; 
			ProbePath_LstEntity->path = true;
		//}
		PointsToGoList->Add(ProbePath_LstEntity);
	}
	else
	{
		//First Step, we move away from the rake in Y and move out of tooth in X
		ProbePath_LstEntity = gcnew PointsToGo();										
		ProbePath_LstEntity->Pt_X =  cPosition[0];	
		ProbePath_LstEntity->Pt_Y =  startPosition[1]; //RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] - HobParameters[4] * MAINDllOBJECT->ProbeRadius();
		ProbePath_LstEntity->Pt_Z =  startPosition[2]; 
		ProbePath_LstEntity->Pt_R =  startPosition[3]; 
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);

		//Second step, we move up in Z and move in C axis
		ProbePath_LstEntity = gcnew PointsToGo();										
		ProbePath_LstEntity->Pt_X =  cPosition[0];	
		ProbePath_LstEntity->Pt_Y =  startPosition[1]; //RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] - HobParameters[4] * MAINDllOBJECT->ProbeRadius();
		ProbePath_LstEntity->Pt_Z =  cPosition[2]; 
		ProbePath_LstEntity->Pt_R =  startPosition[3]; 
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);

	}

	////Second step, we move up in Z and move in C axis
	//ProbePath_LstEntity = gcnew PointsToGo();										
	//ProbePath_LstEntity->Pt_X =  startPosition[0];	
	//ProbePath_LstEntity->Pt_Y =  startPosition[1]; //RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] - HobParameters[4] * MAINDllOBJECT->ProbeRadius();
	//ProbePath_LstEntity->Pt_Z =  cPosition[2]; 
	//ProbePath_LstEntity->Pt_R =  cPosition[3]; 
	//ProbePath_LstEntity->path = true;
	//PointsToGoList->Add(ProbePath_LstEntity);


	//We move inside tooth in Y. 
	//Move to inside the tooth by about 0.2 mm so that we can touch inside top of tooth. 
	cPosition[1] = ReferenceCenter[1] + HobParameters[4] * (MAINDllOBJECT->ProbeRadius() + 0.2);
	ProbePath_LstEntity = gcnew PointsToGo();										
	ProbePath_LstEntity->Pt_X =  cPosition[0];	
	ProbePath_LstEntity->Pt_Y =  cPosition[1];
	ProbePath_LstEntity->Pt_Z =  cPosition[2]; 
	ProbePath_LstEntity->Pt_R =  cPosition[3]; 
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);
	
	//We move down in Z to take a point. 
	ProbePath_LstEntity = gcnew PointsToGo();					
	ProbePath_LstEntity->Pt_X =  cPosition[0];	
	ProbePath_LstEntity->Pt_Y =  cPosition[1] ;
	ProbePath_LstEntity->Pt_Z =  startPosition[2];// - MoveByForTouch; 
	ProbePath_LstEntity->Pt_R =  cPosition[3]; 
	ProbePath_LstEntity->path = false;
	PointsToGoList->Add(ProbePath_LstEntity);

	//Come out in Y axis and unrotate the correction for helical gash.
	ProbePath_LstEntity = gcnew PointsToGo();										
	ProbePath_LstEntity->Pt_X =  startPosition[0];	
	ProbePath_LstEntity->Pt_Y =  startPosition[1];
	ProbePath_LstEntity->Pt_Z =  cPosition[2];
	ProbePath_LstEntity->Pt_R =  cPosition[3];
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);

	//Move to bottom face of tooth at this X
	Move_inZDirection *= 2; //PitchValue / 4 + 2 * MoveByForTouch;
	if (HobParameters[13] != 0) 
	{
		Rotate_AxisForHelicalLead = Move_inZDirection * 2 * M_PI / HobParameters[13];
		//cPosition[2] -= Move_inZDirection + 2 * (MoveByForTouch + MAINDllOBJECT->ProbeRadius() + ed);
		cPosition[3] += Rotate_AxisForHelicalLead * (HobParameters[4] * HobParameters[5]);
	}

	//Go Down in Z and rotate in C since we are outside the hob
	cPosition[2] -= PitchValue;
	ProbePath_LstEntity = gcnew PointsToGo();										
	ProbePath_LstEntity->Pt_X =  startPosition[0];	
	ProbePath_LstEntity->Pt_Y =  startPosition[1];
	ProbePath_LstEntity->Pt_Z =  cPosition[2];
	ProbePath_LstEntity->Pt_R =  cPosition[3];
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);

	//Move in X Y
	ProbePath_LstEntity = gcnew PointsToGo();										
	ProbePath_LstEntity->Pt_X =  cPosition[0];	
	ProbePath_LstEntity->Pt_Y =  cPosition[1];
	ProbePath_LstEntity->Pt_Z =  cPosition[2]; 
	ProbePath_LstEntity->Pt_R =  cPosition[3]; 
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);

	//Move up and take a point
	ProbePath_LstEntity = gcnew PointsToGo();										
	ProbePath_LstEntity->Pt_X =  cPosition[0];	
	ProbePath_LstEntity->Pt_Y =  cPosition[1] ;
	ProbePath_LstEntity->Pt_Z =  startPosition[2]; // + MoveByForTouch; 
	ProbePath_LstEntity->Pt_R =  cPosition[3]; 
	ProbePath_LstEntity->path = false;
	PointsToGoList->Add(ProbePath_LstEntity);

	//Move_inZDirection /= 2; //PitchValue / 4 + 2 * MoveByForTouch;
	//if (HobParameters[13] != 0) 
	//	Rotate_AxisForHelicalLead = Move_inZDirection * 2 * M_PI / HobParameters[13];
	//cPosition[3] += Rotate_AxisForHelicalLead * HobParameters[4];
	
	//We move out in X and Y to start position without moving in Z
	ProbePath_LstEntity = gcnew PointsToGo();			//outer start position for every tooth with appropriate z,r movement							
	ProbePath_LstEntity->Pt_X =  startPosition[0];	
	ProbePath_LstEntity->Pt_Y =  startPosition[1];
	ProbePath_LstEntity->Pt_Z =  cPosition[2]; //ReferenceCenter[2] + (i*Move_inZDirection);
	ProbePath_LstEntity->Pt_R =  cPosition[3]; // + (AngleBWTwoHelicalTooth*i*HobDirectionSign*M_PI/180);
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);

	//Rotate back to start position if gash is helical
	if (HobParameters[13] != 0)
	{
		ProbePath_LstEntity = gcnew PointsToGo();										
		ProbePath_LstEntity->Pt_X =  startPosition[0];	
		ProbePath_LstEntity->Pt_Y =  startPosition[1];
		ProbePath_LstEntity->Pt_Z =  startPosition[2]; 
		ProbePath_LstEntity->Pt_R =  startPosition[3];
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);
	}
}


void RWrapper::RW_DelphiCamModule::CalculatePts_for_C_Axis_Zero()
{
	RWrapper::PointsToGo^ ProbePath_LstEntity;
	PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
	DoingCalibration = true;
	Listcount = 0;
	//startPosition = gcnew cli::array<double>(4);
	//cPosition = gcnew cli::array<double>(4);
	//double rX[4]; 
	//RMATH3DOBJECT->GetC_Axis_XY(startPosition[2], MAINDllOBJECT->TIS_CAxis, rX);

	//double MoveByForTouch = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;

	//for (int i = 0; i < 4; i ++)
	//{	
	//	startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
	//	cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
	//}
	//startPosition[0] = rX[0] - (HobParameters[3] / 2) - 6; //+ HobParameters[1];
	//startPosition[1] = rX[1] - HobParameters[4] * MAINDllOBJECT->ProbeRadius();

	if (FirstToothPts == NULL)
	{ 
		RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Cloud Points");
		FirstToothPts = (CloudPoints*)MAINDllOBJECT->getShapesList().selectedItem();
	}
	else
	{
		MAINDllOBJECT->selectShape(FirstToothPts->getId(), false);
		RWrapper::RW_MainInterface::MYINSTANCE()->HandleShapeParameter_Click("Reset Points");
	}
	//for (int i = 0; i < 4; i++)
	//{
	//	startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
	//}

		//17 Jul 14. There are repeatability and consistency problems because Zero position is not automated. So inserting that here
	//First Up, we enter the tooth near the tip
	double initX = MAINDllOBJECT->TIS_CAxis[0] - (HobParameters[3] / 2) + MAINDllOBJECT->ProbeRadius() + HobParameters[1];//startPosition[0] + MAINDllOBJECT->ProbeRadius() + 20 + HobParameters[1];
	double initY = MAINDllOBJECT->TIS_CAxis[1] - HobParameters[4] * MAINDllOBJECT->ProbeRadius();
	ProbePath_LstEntity = gcnew PointsToGo();										
	ProbePath_LstEntity->Pt_X = initX;
	ProbePath_LstEntity->Pt_Y = initY;
	ProbePath_LstEntity->Pt_Z = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
	ProbePath_LstEntity->Pt_R = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);

	//Now we rotate in C until we get a touch. 
	ProbePath_LstEntity = gcnew PointsToGo();										
	ProbePath_LstEntity->Pt_X = initX;
	ProbePath_LstEntity->Pt_Y = initY;
	ProbePath_LstEntity->Pt_Z = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
	ProbePath_LstEntity->Pt_R = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] + HobParameters[4] * 2 * M_PI / HobParameters[6];
	ProbePath_LstEntity->path = false;
	PointsToGoList->Add(ProbePath_LstEntity);

	//if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 2)
	//{
	//	//Now we rotate in C until we get a touch. 
	//	ProbePath_LstEntity = gcnew PointsToGo();
	//	ProbePath_LstEntity->Pt_X = initX;
	//	ProbePath_LstEntity->Pt_Y = initY;
	//	ProbePath_LstEntity->Pt_Z = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
	//	ProbePath_LstEntity->Pt_R = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] - HobParameters[4] * 0.07;
	//	ProbePath_LstEntity->path = false;
	//	PointsToGoList->Add(ProbePath_LstEntity);
	//}

}

void RWrapper::RW_DelphiCamModule::GetBacktoHomePosition()
{
	RWrapper::PointsToGo^ ProbePath_LstEntity;
	PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
	DoingCalibration = true;
	Listcount = 0;
	//Now we move back in Y away from rake face 
	ProbePath_LstEntity = gcnew PointsToGo();
	ProbePath_LstEntity->Pt_X = ReferenceCenter[0] - (HobParameters[3] / 2) + MAINDllOBJECT->ProbeRadius() + HobParameters[1] - 10;
	ProbePath_LstEntity->Pt_Y = ReferenceCenter[1] - HobParameters[4] * (MAINDllOBJECT->ProbeRadius() * 2);
	ProbePath_LstEntity->Pt_Z = ToothMidPt[2]; //RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
	ProbePath_LstEntity->Pt_R = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
	ProbePath_LstEntity->path = false;
	PointsToGoList->Add(ProbePath_LstEntity);

	////Now we get out of tooth.  
	//ProbePath_LstEntity = gcnew PointsToGo();
	//ProbePath_LstEntity->Pt_X = ReferenceCenter[0] - (HobParameters[3] / 2) + MAINDllOBJECT->ProbeRadius() + HobParameters[1] - 20;
	//ProbePath_LstEntity->Pt_Y = ReferenceCenter[1] - HobParameters[4] * MAINDllOBJECT->ProbeRadius();
	//ProbePath_LstEntity->Pt_Z = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
	//ProbePath_LstEntity->Pt_R = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
	//ProbePath_LstEntity->path = false;
	//PointsToGoList->Add(ProbePath_LstEntity);
}

void RWrapper::RW_DelphiCamModule::Reset_RunoutValues()
{
	for (int i = 0; i < 7; i ++)
		MAINDllOBJECT->TIS_CAxis[i] = BC_Values[i];
}

bool RWrapper::RW_DelphiCamModule::GetRunoutforShape(ShapeWithList* CShape, int Runout_Axis_Index, double* answer, bool Sinusoid_Answer)
{
	bool finished = false;
	int PtsCt = CShape->PointsList->Pointscount();
	//MessageBox::Show(Convert::ToString(PtsCt), "Rapid-I");
	double* runoutPts = (double*)malloc(sizeof(double) * PtsCt * 2);
	int ii = 0;
	//double* OriPt = (double*)malloc(sizeof(double) * PtsCt * 4);
	double *RotatedPt = (double*)malloc(sizeof(double) * PtsCt * 4);
	double *CirclePts = (double*)malloc(sizeof(double) *PtsCt * 2);
	double *OriPt = (double*)malloc(sizeof(double) *PtsCt * 4);

	SinglePoint* Sp_Mid; 
	int ProbeDir; 
	bool OpenedFile = true;
	//MessageBox::Show("Done Init", "Rapid-I");
	//Let us tabulate all raw points for calculating runout 
	System::String^ SfilePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath;
	SfilePath = SfilePath + "\\Log\\AxisLog\\Runout_Pts.csv";
	System::IO::StreamWriter^ pointsFile;
	try
	{
		pointsFile = gcnew System::IO::StreamWriter(SfilePath, true);
	}
	catch(...)
	{
		OpenedFile = false;
	}
	//ofstream pointsFile;
	//System::String^ SfilePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath;
	System::String^ currentTime = Microsoft::VisualBasic::DateAndTime::Now.ToShortDateString() + ", " + Microsoft::VisualBasic::DateAndTime::Now.ToString("HH:mm:ss");

	//pointsFile = gcnew System::IO::StreamWriter(SfilePath, true);
	pointsFile->WriteLine(currentTime); // << wch2 << endl;
	pointsFile->WriteLine( "X" + ", " + "Y " + ", " + "Z" + ", " + "C"); // << endl;

	try
	{
		double currZ;

		//Iterate through each point in the given shape, and unratote them to get original point.
		//Collect the R vs axis points in runoutPts array
		if(PtsCt > 1)// && NoofShapes == TargetShapeNo)
		{			
			PtsList XYPtsCollection;
			for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
			{		
				SinglePoint* Spt = (*SptItem).second;
				currZ = Spt->Z;
				ProbeDir = Spt->Pdir;
				if ((int) (ii / 2) == (int)PtsCt / 2) Sp_Mid = Spt;
				//Un-rotate the point to get the original X, Y and theta values, and for sinusoid runout correction
				RotatedPt[ii * 2] = Spt->X; RotatedPt[ii * 2 + 1] = Spt->Y; RotatedPt[ii * 2 + 2] = Spt->Z; RotatedPt[ii * 2 + 3] = Spt->R;
				//RMATH3DOBJECT->UnRotatePoint_Around_C_Axis(RotatedPt + ii * 2, MAINDllOBJECT->TIS_CAxis, OriPt + ii * 2);
				//Collect R vs required axis data in a linear array for Best-fit
				runoutPts[ii] = RotatedPt[ii * 2 + 3]; //ii++;
				runoutPts[ii + 1] = RotatedPt[ii * 2 + Runout_Axis_Index]; 
				RMATH3DOBJECT->Rotate_Point_2D(RotatedPt + ii * 2, MAINDllOBJECT->TIS_CAxis, OriPt + ii * 2, -1 * RotatedPt[ii * 2 + 3]);
				CirclePts[ii] = OriPt[ii * 2];	CirclePts[ii + 1] = OriPt[ii * 2 + 1];
				pointsFile->WriteLine(Spt->X + ", " + Spt->Y + ", " + Spt->Z + ", " + Spt->R + ", " 
										+ OriPt[ii * 2] + ", " + OriPt[ii * 2 + 1] + ", " + OriPt[ii * 2 + 2] + ", " + OriPt[ii * 2 + 3]);
				//pointsFile->WriteLine();
				ii += 2;
			}
			double RunoutParams[3] = {0};
			if (BESTFITOBJECT->Circle_BestFit(CirclePts, PtsCt, RunoutParams, true, 1000000, true, 0, 1))
			{
				for (int i = 0; i < 3; i ++)
					answer[i] = RunoutParams[i];
				answer[2] -= MAINDllOBJECT->ProbeRadius();
				if (BESTFITOBJECT->Sinusoid_BestFit(runoutPts, PtsCt, RunoutParams))
				{
					pointsFile->WriteLine("Sinusoid Params");
					pointsFile->WriteLine(RunoutParams[0] + ", " + RunoutParams[1] + ", " + RunoutParams[2]);
					if (Sinusoid_Answer)
					{
						for (int i = 0; i < 3; i++)
							answer[i] = RunoutParams[i];
					}
				}
				finished = true;
			}
		}
		//MessageBox::Show("Got Pts", "Rapid-I");

	}
	catch(...)
	{
		MessageBox::Show("There was a problem in calculating rotary runout", "Rapid-I");
		finished = false;
	}
	if (OpenedFile)
	{	pointsFile->WriteLine();
		pointsFile->Close();
	}

	free (RotatedPt); free(runoutPts); free(OriPt); free(CirclePts);
	return finished;
}

void RWrapper::RW_DelphiCamModule::CalculatePts_NoRotation(double Z_Level, int touchDir, bool SkipFirstSteps)
{
	try
	{
		if (DoingHobMeasurement || !HobParametersUpdated) return; //|| !GotAxis

		RWrapper::PointsToGo^ ProbePath_LstEntity;
		PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();

		Listcount = 0; FirstStep = true;
		//startPosition = gcnew cli::array<double>(4);
		//cPosition = gcnew cli::array<double>(4);
		DoingCalibration = true;
		double extraDist = HobParameters[14] / 4 - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;

		if (!SkipFirstSteps)
		{
			for (int i = 0; i < 4; i++)
			{
				startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
				cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
			}
		}

		int pos_Dir = 1;
		if (RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] < ReferenceCenter[1]) pos_Dir = -1;
		//X axis will always be one radius + probe retract position to the left of the BC axis centre in X
		startPosition[0] = ReferenceCenter[0] - HobParameters[14] / 2; // -RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue; // +MAINDllOBJECT->ProbeRadius();// - 1.0;
		if (startPosition[3] < 0)
		{
			if (abs(startPosition[3]) > M_PI)
				startPosition[3] = -2 * M_PI;
			else
				startPosition[3] = 0;
		}
		else
		{
			if (abs(startPosition[3]) > M_PI)
				startPosition[3] = 2 * M_PI;
			else
				startPosition[3] = 0;
		}
		startPosition[2] = Z_Level;

		//if (abs(startPosition[1] - ReferenceCenter[1]) > HobParameters[14] / 2)
		//{	
		//	cPosition[1] = ReferenceCenter[1] + pos_Dir * (HobParameters[3] / 2 + extraDist);
		//	ProbePath_LstEntity = gcnew PointsToGo();											
		//	ProbePath_LstEntity->Pt_X = cPosition[0];	
		//	ProbePath_LstEntity->Pt_Y =  cPosition[1];
		//	ProbePath_LstEntity->Pt_Z =  startPosition[2];
		//	ProbePath_LstEntity->Pt_R =  startPosition[3];
		//	ProbePath_LstEntity->path = true;
		//	PointsToGoList->Add(ProbePath_LstEntity);
		//}
		//else
		//{
		//startPosition[1] = ReferenceCenter[1];
		//}
		//

		int rotateDir = 1;
		if (startPosition[3] > 0) rotateDir = -1;

		//int numberofRows = 5;
		//double fracX[5] = {1, 0.293, 0, 0.293, 1};
		//double fracY[5] = {1, 0.707, 0, -0.707, -1};
		//int dirX[5] = {0, 0, -1, 0, 0};
		//int dirY[5] = {1, 1, 0, -1, -1};
		int z_direction = 1;
		int numberofRows = 5;
		double fracX[5] = { 1, 0.293, 0, 0.293, 1 };
		double fracY[5] = { 1, 0.707, 0, -0.707, -1 };
		int dirX[5] = { 0, -1.5, -1, -2, 0 };
		int dirY[5] = { 1, 1, 0, 0, -1 };
		int ProbeRadMultiplier[5] = { 0, 1, 1, 1, 0 };

		//First Step - Go to Start Position 
		//if current Z is not at target Z level, 
		//double Z_Diff = abs(RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] - Z_Level);
		//if (Z_Diff < 2.0 && Z_Diff > 0.1)
		//{
		//	//We are very close to target position. Hence move directly, only in Z. 
		//	ProbePath_LstEntity = gcnew PointsToGo();											
		//	ProbePath_LstEntity->Pt_X = cPosition[0];	
		//	ProbePath_LstEntity->Pt_Y =  cPosition[1];
		//	ProbePath_LstEntity->Pt_Z =  startPosition[2];
		//	ProbePath_LstEntity->Pt_R =  startPosition[3];
		//	ProbePath_LstEntity->path = true;
		//	PointsToGoList->Add(ProbePath_LstEntity);
		//}
		//else if (Z_Diff > 2.0)
		//{
		//	//Now move in Z at current XY position
		//	ProbePath_LstEntity = gcnew PointsToGo();											
		//	ProbePath_LstEntity->Pt_X = cPosition[0]; //ReferenceCenter[0] - HobParameters[3] / 2 - extraDist;	
		//	ProbePath_LstEntity->Pt_Y = cPosition[1];
		//	ProbePath_LstEntity->Pt_Z = Z_Level;
		//	ProbePath_LstEntity->Pt_R = cPosition[3];
		//	ProbePath_LstEntity->path = true;
		//	PointsToGoList->Add(ProbePath_LstEntity);
		//	
		//	//Now move in Y to correct startposition
		//	ProbePath_LstEntity = gcnew PointsToGo();											
		//	ProbePath_LstEntity->Pt_X = cPosition[0];	
		//	ProbePath_LstEntity->Pt_Y = startPosition[1];
		//	ProbePath_LstEntity->Pt_Z = Z_Level;
		//	ProbePath_LstEntity->Pt_R = cPosition[3];
		//	ProbePath_LstEntity->path = true;
		//	PointsToGoList->Add(ProbePath_LstEntity);
		//}
		int jj = 0;
		startPosition[1] = ReferenceCenter[1];
		if (!SkipFirstSteps)
		{
			//if (abs(startPosition[1] - ReferenceCenter[1]) < HobParameters[14] / 2)
			//{
			//	//Set of commmands to go Positive Y outside.
			//	ProbePath_LstEntity = gcnew PointsToGo();
			//	ProbePath_LstEntity->Pt_X = startPosition[0] - extraDist;
			//	ProbePath_LstEntity->Pt_Y = startPosition[1] + (HobParameters[14] * 0.4);
			//	ProbePath_LstEntity->Pt_Z = startPosition[2];
			//	ProbePath_LstEntity->Pt_R = startPosition[3];
			//	ProbePath_LstEntity->path = true;
			//	PointsToGoList->Add(ProbePath_LstEntity);
			//	//Set the current position
			//	cPosition[0] = ProbePath_LstEntity->Pt_X;
			//	cPosition[1] = startPosition[1] + (HobParameters[14] / 2) + extraDist;
			//	//Move right in X axis to the centre
			//	ProbePath_LstEntity = gcnew PointsToGo();
			//	ProbePath_LstEntity->Pt_X = startPosition[0] + HobParameters[14] / 2 + MAINDllOBJECT->ProbeRadius();
			//	ProbePath_LstEntity->Pt_Y = cPosition[1];
			//	ProbePath_LstEntity->Pt_Z = startPosition[2];
			//	ProbePath_LstEntity->Pt_R = startPosition[3];
			//	ProbePath_LstEntity->path = true;
			//	PointsToGoList->Add(ProbePath_LstEntity);
			//}
		}
		else
		{
			jj = 1;
		}

		double diaFraction = CylinderDia / (float)(numberofRows - 1);

		for (int j = jj; j < numberofRows; j++)
		{
			//Let us move in X and Y to go to the current row
			//Move in Y-axis by a fraction proportional to the number of rows!
			cPosition[0] = startPosition[0] + fracX[j] * (HobParameters[14] / 2 + ProbeRadMultiplier[j] * MAINDllOBJECT->ProbeRadius()) + dirX[j] * extraDist;
			//if (fracX[j] == 0) cPosition[0] += MAINDllOBJECT->ProbeRadius();
			if (j == 1)
			{
				ProbePath_LstEntity = gcnew PointsToGo();
				ProbePath_LstEntity->Pt_X = cPosition[0];
				ProbePath_LstEntity->Pt_Y = startPosition[1] + touchDir * (HobParameters[14] / 2 + extraDist);
				ProbePath_LstEntity->Pt_Z = startPosition[2];
				ProbePath_LstEntity->Pt_R = startPosition[3];
				ProbePath_LstEntity->path = true;
				PointsToGoList->Add(ProbePath_LstEntity);

				cPosition[1] = startPosition[1] + touchDir * (fracY[j] * HobParameters[14] / 2); // +dirY[j] * extraDist);
				//ProbePath_LstEntity->Sp_Y = 4;
			}
			else
				cPosition[1] = startPosition[1] + touchDir * (fracY[j] * HobParameters[14] / 2 + dirY[j] * extraDist);

			ProbePath_LstEntity = gcnew PointsToGo();
			ProbePath_LstEntity->Pt_X = cPosition[0];
			ProbePath_LstEntity->Pt_Y = cPosition[1];// ProbePath_LstEntity->Sp_Y = 2;
			ProbePath_LstEntity->Pt_Z = startPosition[2];
			ProbePath_LstEntity->Pt_R = startPosition[3];
			ProbePath_LstEntity->path = true;
			//if (j % 2 == 0)
			//{
			//	ProbePath_LstEntity->Sp_X = 0.6 * RWrapper::RW_DRO::MYINSTANCE()->feedRate[0];
			//	ProbePath_LstEntity->Sp_Y = RWrapper::RW_DRO::MYINSTANCE()->feedRate[1];
			//}
			PointsToGoList->Add(ProbePath_LstEntity);

			//Set of commands to touch the cylinder and take a point
			ProbePath_LstEntity = gcnew PointsToGo();
			ProbePath_LstEntity->Pt_X = cPosition[0] - dirX[j] * extraDist; // - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue
			ProbePath_LstEntity->Pt_Y = startPosition[1] + touchDir * (fracY[j] * HobParameters[14] / 2); // + MAINDllOBJECT->ProbeRadius());
			ProbePath_LstEntity->Pt_Z = startPosition[2];
			ProbePath_LstEntity->Pt_R = startPosition[3];
			ProbePath_LstEntity->path = false;
			//if (dirX[j] != 0 && RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 2)
			//{
			//	ProbePath_LstEntity->Sp_X = 12;
			//	ProbePath_LstEntity->Sp_Y = 16;
			//}
			
			PointsToGoList->Add(ProbePath_LstEntity);
			if (j == numberofRows - 2) //> 1 && j < numberofRows - 1)
			{
				ProbePath_LstEntity = gcnew PointsToGo();
				ProbePath_LstEntity->Pt_X = startPosition[0];
				ProbePath_LstEntity->Pt_Y = startPosition[1] + touchDir * (fracY[j + 1] * HobParameters[14] / 2); // + MAINDllOBJECT->ProbeRadius());
				ProbePath_LstEntity->Pt_Z = startPosition[2];
				ProbePath_LstEntity->Pt_R = startPosition[3];
				ProbePath_LstEntity->path = true;
				PointsToGoList->Add(ProbePath_LstEntity);
			}

			//if (j == numberofRows / 2) 
			//{	ProbePath_LstEntity = gcnew PointsToGo();								
			//	ProbePath_LstEntity->Pt_X =  cPosition[0] + dirX[j] * extraDist;;
			//	ProbePath_LstEntity->Pt_Y =  cPosition[1];
			//	ProbePath_LstEntity->Pt_Z =  startPosition[2];
			//	ProbePath_LstEntity->Pt_R =  startPosition[3];
			//	ProbePath_LstEntity->path = true;
			//	PointsToGoList->Add(ProbePath_LstEntity);
			//}
		}
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule012", ex);
	}
}

void RWrapper::RW_DelphiCamModule::SelectShapeforThisRun(ShapeWithList* CShape, System::String^ sType)
{
		if (CShape == NULL)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click(sType);
			CShape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		}
		else
		{
			//int index = (Shape*);
			MAINDllOBJECT->selectShape(CShape->getId(), false);
			RWrapper::RW_MainInterface::MYINSTANCE()->HandleShapeParameter_Click("Reset Points");
		}

}

double RWrapper::RW_DelphiCamModule::GetRunout_at_this_Z(double Z_Level)
{
	double axisPt[2] = {0};
	double RunoutPt[2] = {0};
	//RMATH3DOBJECT->GetC_Axis_XY(Z_Level, MAINDllOBJECT->TIS_CAxis, axisPt);
	//RMATH3DOBJECT->GetC_Axis_XY(Z_Level, &Hob_RunoutParams[0], RunoutPt);
	return RMATH2DOBJECT->Pt2Pt_distance(axisPt, RunoutPt);
}

void RWrapper::RW_DelphiCamModule::Move_To_NextHub(bool inY)
{
	//Initialise variables etc
	RWrapper::PointsToGo^ ProbePath_LstEntity;
	PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
	DoingCalibration = true;
	Listcount = 0;
	startPosition = gcnew cli::array<double>(4);
	cPosition = gcnew cli::array<double>(4);
	//double rX[4]; 
	//RMATH3DOBJECT->GetC_Axis_XY(startPosition[2], MAINDllOBJECT->TIS_CAxis, rX);
	double MoveByForTouch = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;

	for (int i = 0; i < 4; i ++)
	{	
		startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
	}
	if (inY)
		startPosition[1] = ReferenceCenter[1] - (HobParameters[3] / 2) - 10; //+ HobParameters[1]; MAINDllOBJECT->TIS_CAxis[1]
	else
		startPosition[0] = ReferenceCenter[0] - (HobParameters[3] / 2) - 10; //+ HobParameters[1];MAINDllOBJECT->TIS_CAxis[0]

	//First Step, get out of hob in X axis.
	ProbePath_LstEntity = gcnew PointsToGo();										
	ProbePath_LstEntity->Pt_X =  startPosition[0];	
	ProbePath_LstEntity->Pt_Y =  startPosition[1]; //RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] - HobParameters[4] * MAINDllOBJECT->ProbeRadius();
	ProbePath_LstEntity->Pt_Z =  startPosition[2]; 
	ProbePath_LstEntity->Pt_R =  startPosition[3]; 
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);

	//Second, Move down hob to the lower hub. 
	cPosition[2] -= (HobParameters[9] + HobParameters[17])/2;
	ProbePath_LstEntity = gcnew PointsToGo();										
	ProbePath_LstEntity->Pt_X =  startPosition[0];	
	ProbePath_LstEntity->Pt_Y =  startPosition[1]; //RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] - HobParameters[4] * MAINDllOBJECT->ProbeRadius();
	ProbePath_LstEntity->Pt_Z =  cPosition[2]; 
	ProbePath_LstEntity->Pt_R =  startPosition[3]; 
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);
}


void RWrapper::RW_DelphiCamModule::RunMasterCylinder_Rotation(int Step)
{
	//We have done XY calculation. We dont know in which runout position the master cylinder is in. 
	//Hence we do a rotation and find out the runout

	//switch (Step)
	//{
	//case 1:
	//case 2: 
	//case 3: 
	//case 4: // Move in Y axis to centre of XY calculated Circle
	if (Step < 1000)
	{

		//Get the centre coordinates of the XY Circle
		double CylParam[7] = {0}; 
		CylParam[5] = 1;
		//ReferenceCenter[0] = CylParam[0]; ReferenceCenter[1] = CylParam[1];`
		MasterCylinder->getParameters(CylParam);

		BC_CentrePositionPts[2 * (Step - 2)] = CylParam[0];
		BC_CentrePositionPts[2 * (Step - 2) + 1] = CylParam[1];
		BC_Diameters[Step - 2] = CylParam[6];
		CalculatePtsFor_BC_Axis(2 * M_PI / BC_CentrePointNumber);
		//Start Running all the points in CNC
		SendDrotoNextposition();
	//	break;
	}
	else //case 1000: // We have completed all raw points collection. Calculate exact position of Axis of Rotation
	{
		if (Getting_BC_Axis)
		{	
			if (MasterCylinder->PointsList->Pointscount() < NumberofLevels * 4)
			{
				MessageBox::Show("Did not get sufficient points on Master Cylinder. Try shifting in Y axis and run again.", "Rapid-I");	
				GotAxis = false;
			}
			else
			{
				try
				{	
					//MessageBox::Show("Finished taking Master Cylinder Points.", "Rapid-I");
					//Calculate TIS_ Axis parameters
					double CylParam[7] = {0}; 
					double tempCircle[7] = {0};
					CylParam[5] = 1;
					MasterCylinder->getParameters(CylParam);
					BC_CentrePositionPts[2 * (BC_CentrePointNumber - 1)] = CylParam[0];
					BC_CentrePositionPts[2 * (BC_CentrePointNumber - 1) + 1] = CylParam[1];
					BC_Diameters[BC_CentrePointNumber - 1] = CylParam[6];
					if (BC_CentrePointNumber == 1)
					{
						tempCircle[0] = CylParam[0]; tempCircle[1] = CylParam[1]; tempCircle[2] = CylParam[6];
					}
					else
						BESTFITOBJECT->Circle_BestFit(BC_CentrePositionPts, BC_CentrePointNumber, tempCircle, true, 1000000, true, 0, 1);

					MAINDllOBJECT->TIS_CAxis[0] = tempCircle[0];
					MAINDllOBJECT->TIS_CAxis[1] = tempCircle[1];
					MAINDllOBJECT->TIS_CAxis[2] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
					MAINDllOBJECT->TIS_CAxis[3] = 0;
					MAINDllOBJECT->TIS_CAxis[4] = 0;
					MAINDllOBJECT->TIS_CAxis[5] = 1;
					MAINDllOBJECT->TIS_CAxis[6] = 2 * tempCircle[2];

					//MessageBox::Show("Finished setting all values.", "Rapid-I");

					//--------------------Finished TIS C Axis Calculation----- Now assign the values. 
					Getting_BC_Axis = false;
					if (Listcount == PointsToGoList->Count) GotAxis = true;
					ReferenceCenter[0] = MAINDllOBJECT->TIS_CAxis[0]; 
					ReferenceCenter[1] = MAINDllOBJECT->TIS_CAxis[1]; 
					for (int i = 0; i < 6; i ++)
						BC_Values[i] = MAINDllOBJECT->TIS_CAxis[i];
					
					//Set Runout as 0 so that we can do indexing without runout. 
					for (int i = 0; i < 6; i++)
					{
						MAINDllOBJECT->BC_Axis[i] = 0; // Hob_RunoutParams[i];
					}
					
					//Write Master Cylinder Values to file
					System::String^ currentTime = Microsoft::VisualBasic::DateAndTime::Now.ToShortDateString() + ", " + Microsoft::VisualBasic::DateAndTime::Now.ToString("HH:mm:ss");
					System::String^ SfilePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath;
					SfilePath = SfilePath + "\\Log\\AxisLog\\MasterCylinder.csv";
					System::IO::StreamWriter^ pointsFile = gcnew System::IO::StreamWriter(SfilePath, true);
					pointsFile->WriteLine(currentTime);
					pointsFile->WriteLine("X" + ", " + "Y " + ", " + "Z" + ", " + "L" + ", " + "M" + ", " + "N" + ", " + "Dia");			
					for (int ii = 0; ii < 7; ii++)
						 pointsFile->Write(MAINDllOBJECT->TIS_CAxis[ii] + ", "); 
					pointsFile->WriteLine();
					//for (int ii = 0; ii < BC_CentrePointNumber; ii++)
					//	 pointsFile->WriteLine(BC_CentrePositionPts[2 * ii] + ", " + BC_CentrePositionPts[2 * ii + 1] + ", " + BC_Diameters[ii]); 
					//pointsFile->WriteLine();
					//for (int ii = 0; ii < 3; ii++)
					//	 pointsFile->Write(tempCircle[ii] + ", "); 
					//pointsFile->WriteLine();
					//for (int ii = 0; ii < 3; ii++)
					//	 pointsFile->Write(RunoutParams[ii] + ", "); 

					//pointsFile->WriteLine();
					pointsFile->Close();
					//Trial - Make tilt as zero. 
					//MAINDllOBJECT->TIS_CAxis[3] = 0;
					//MAINDllOBJECT->TIS_CAxis[4] = 0;
					//MAINDllOBJECT->TIS_CAxis[5] = 1;
					//CalculatePtsFor_BC_Axis(0);
				}
				catch(Exception^ ex)
				{ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule006", ex); }
			}
		}
	}
//	} //End Switch Case

}

void RWrapper::RW_DelphiCamModule::CalculatePtsfor_Runout_Master(int ptsCount, double Z_Level)
{
	//Calculate the points coordinates for giving commands in CNC

	//If already running something, quit the command!!

	//if (DoingHobMeasurement || !GotAxis || !HobParametersUpdated) return;

	RWrapper::PointsToGo^ ProbePath_LstEntity;
	PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
	double extraDist = 6.0 - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;

	Listcount = 0; FirstStep = true;
	startPosition = gcnew cli::array<double>(4);
	cPosition = gcnew cli::array<double>(4);
	DoingCalibration = true;

	for (int i = 0; i < 4; i ++)
	{	startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
	}	
	//X axis will always be one radius + probe retract position to the left of the BC axis centre in X
	startPosition[0] = ReferenceCenter[0] - CylinderDia / 2 - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue - MAINDllOBJECT->ProbeRadius();// - 1.0;
	startPosition[1] = ReferenceCenter[1];
	startPosition[2] = Z_Level;

	//if (ptsCount < 3) 
	//ptsCount = 12;

	int rotateDir = 1;
	if (startPosition[3] > 0) rotateDir = -1;
	//First Step - Go to Start Position 

	//Now we are sure we are at correct starting position
	//We can start taking points and rotating! We have two steps for each point 
	//one to go and take a point, and next to rotate to next pt!
	double RotateAngle = 2 * M_PI / ptsCount;

	for (int i = 0; i < ptsCount; i ++)
	{
		cPosition[0] = startPosition[0] + RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;// + MAINDllOBJECT->ProbeRadius(); //+ 1.0;
		//Now move in X to correct startposition
		ProbePath_LstEntity = gcnew PointsToGo();											
		ProbePath_LstEntity->Pt_X = cPosition[0];	
		ProbePath_LstEntity->Pt_Y = startPosition[1];
		ProbePath_LstEntity->Pt_Z = Z_Level;
		ProbePath_LstEntity->Pt_R = cPosition[3];
		ProbePath_LstEntity->path = false;
		PointsToGoList->Add(ProbePath_LstEntity);

		if (i == ptsCount - 1) continue;
		cPosition[3] += RotateAngle * rotateDir;
		//Now move in X to correct startposition
		ProbePath_LstEntity = gcnew PointsToGo();											
		ProbePath_LstEntity->Pt_X = startPosition[0] - MAINDllOBJECT->ProbeRadius();	
		ProbePath_LstEntity->Pt_Y = startPosition[1];
		ProbePath_LstEntity->Pt_Z = Z_Level;
		ProbePath_LstEntity->Pt_R = cPosition[3];
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);
	}
	ProbePath_LstEntity = gcnew PointsToGo();											
	ProbePath_LstEntity->Pt_X = ReferenceCenter[0] - CylinderDia / 2 - extraDist;	
	ProbePath_LstEntity->Pt_Y = startPosition[1];
	ProbePath_LstEntity->Pt_Z = Z_Level;
	ProbePath_LstEntity->Pt_R = cPosition[3];
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);
}

void RWrapper::RW_DelphiCamModule::CalculatePtsfor_Runout_Y(int ptsCount, double Z_Level, double dia)
{
	//Calculate points for doing runout correction in Y axis. The other function will do it in X axis. 
	//Eventually let us try to see if we can integrate the two into a single function

	//If already running something, quit the command!!

	if (DoingHobMeasurement || !GotAxis || !HobParametersUpdated) return;

	RWrapper::PointsToGo^ ProbePath_LstEntity;
	PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
	double extraDist = 6.0 - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;

	Listcount = 0; FirstStep = true;
	startPosition = gcnew cli::array<double>(4);
	cPosition = gcnew cli::array<double>(4);
	DoingCalibration = true;

	for (int i = 0; i < 4; i++)
	{
		startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
	}

	// Y axis will always be one radius + probe retract position to the left of the BC axis centre in X
	startPosition[0] = ReferenceCenter[0];
	startPosition[1] = ReferenceCenter[1] - dia / 2 - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue - MAINDllOBJECT->ProbeRadius();// - 1.0;
	startPosition[2] = Z_Level;
	//startPosition[3] = 0;

	if (ptsCount < 3) ptsCount = 12;

	int rotateDir = 1;
	if (startPosition[3] > 0) rotateDir = -1;

	//if current Z is not at target Z level, 
	double Z_Diff = abs(cPosition[2] - Z_Level);
	if (Z_Diff < 2.0 && Z_Diff > 0.1)
	{
		//We are very close to target position. Hence move directly, only in Z. 
		ProbePath_LstEntity = gcnew PointsToGo();
		ProbePath_LstEntity->Pt_X = cPosition[0];
		ProbePath_LstEntity->Pt_Y = cPosition[1];
		ProbePath_LstEntity->Pt_Z = startPosition[2];
		ProbePath_LstEntity->Pt_R = startPosition[3];
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);
	}
	else if (Z_Diff > 2.0)
	{
		//move in Z at current XY position
		ProbePath_LstEntity = gcnew PointsToGo();
		ProbePath_LstEntity->Pt_X = ReferenceCenter[0] - dia / 2 - extraDist;
		ProbePath_LstEntity->Pt_Y = startPosition[1];
		ProbePath_LstEntity->Pt_Z = Z_Level;
		ProbePath_LstEntity->Pt_R = cPosition[3];
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);

		//Now move in X to correct startposition
		ProbePath_LstEntity = gcnew PointsToGo();
		ProbePath_LstEntity->Pt_X = startPosition[0];
		ProbePath_LstEntity->Pt_Y = startPosition[1];
		ProbePath_LstEntity->Pt_Z = Z_Level;
		ProbePath_LstEntity->Pt_R = cPosition[3];
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);
	}

	//Now that we are in correct Z position, let us move in XY to correct position so that probe 
	//is at the front centre in X axis to touch and take points by moving in Y axis
	//Since we are doing in Y, let us first move outside the cylinder in X .
	ProbePath_LstEntity = gcnew PointsToGo();
	ProbePath_LstEntity->Pt_X = ReferenceCenter[0] - dia / 2 - extraDist;
	ProbePath_LstEntity->Pt_Y = cPosition[1];
	ProbePath_LstEntity->Pt_Z = startPosition[2];
	ProbePath_LstEntity->Pt_R = startPosition[3];
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);

	//Now move to correct Y position without moving in X
	ProbePath_LstEntity = gcnew PointsToGo();
	ProbePath_LstEntity->Pt_X = ReferenceCenter[0] - dia / 2 - extraDist;
	ProbePath_LstEntity->Pt_Y = startPosition[1];
	ProbePath_LstEntity->Pt_Z = startPosition[2];
	ProbePath_LstEntity->Pt_R = startPosition[3];
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);

	//Now move in X to correct X position
	ProbePath_LstEntity = gcnew PointsToGo();
	ProbePath_LstEntity->Pt_X = startPosition[0];
	ProbePath_LstEntity->Pt_Y = startPosition[1];
	ProbePath_LstEntity->Pt_Z = startPosition[2];
	ProbePath_LstEntity->Pt_R = startPosition[3];
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);
	
	//Now we are sure we are at correct starting position
	//We can start taking points and rotating! We have two steps for each point 
	//one to go and take a point, and next to rotate to next pt!
	double RotateAngle = 2 * M_PI / ptsCount;

	for (int i = 0; i < ptsCount; i++)
	{
		cPosition[1] = startPosition[1] + RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;// + MAINDllOBJECT->ProbeRadius(); //+ 1.0;
		//Now move in Y to correct startposition
		ProbePath_LstEntity = gcnew PointsToGo();
		ProbePath_LstEntity->Pt_X = startPosition[0];
		ProbePath_LstEntity->Pt_Y = cPosition[1];
		ProbePath_LstEntity->Pt_Z = Z_Level;
		ProbePath_LstEntity->Pt_R = cPosition[3];
		ProbePath_LstEntity->path = false;
		PointsToGoList->Add(ProbePath_LstEntity);

		if (i == ptsCount - 1) continue;
		cPosition[3] += RotateAngle * rotateDir;
		//Now move in X to correct startposition
		ProbePath_LstEntity = gcnew PointsToGo();
		ProbePath_LstEntity->Pt_X = startPosition[0]; // 
		ProbePath_LstEntity->Pt_Y = startPosition[1] - MAINDllOBJECT->ProbeRadius();;
		ProbePath_LstEntity->Pt_Z = Z_Level;
		ProbePath_LstEntity->Pt_R = cPosition[3];
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);
	}

	//ProbePath_LstEntity = gcnew PointsToGo();
	//ProbePath_LstEntity->Pt_X = startPosition[0]; // -HobParameters[3] / 2 - extraDist;
	//ProbePath_LstEntity->Pt_Y = startPosition[1];
	//ProbePath_LstEntity->Pt_Z = Z_Level;
	//ProbePath_LstEntity->Pt_R = cPosition[3];
	//ProbePath_LstEntity->path = true;
	//PointsToGoList->Add(ProbePath_LstEntity);
}

void RWrapper::RW_DelphiCamModule::WritePositionCommandstoFile(System::String^ ProcessType)
{
	//Write down all target position values in csv file here
	System::String^ SfilePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath;
	SfilePath = SfilePath + "\\Log\\AxisLog\\MeasurementCommands_" + Microsoft::VisualBasic::DateAndTime::Now.ToShortDateString() + ".csv";
	System::IO::StreamWriter^ pointsFile = gcnew System::IO::StreamWriter(SfilePath, true);
	RWrapper::PointsToGo^ ProbePath_LstEntity;

	System::String^ currentTime = Microsoft::VisualBasic::DateAndTime::Now.ToShortDateString() + ", " + Microsoft::VisualBasic::DateAndTime::Now.ToString("HH:mm:ss");
	pointsFile->WriteLine(currentTime + ", " + ProcessType);
	for (int i = 0; i < PointsToGoList->Count; i++)
	{
		ProbePath_LstEntity = PointsToGoList[i];
		pointsFile->WriteLine(ProbePath_LstEntity->Pt_X + ", " + ProbePath_LstEntity->Pt_Y + ", " + ProbePath_LstEntity->Pt_Z
			+ ", " + ProbePath_LstEntity->Pt_R + ", " + Convert::ToString(ProbePath_LstEntity->path) + ", " + 
			ProbePath_LstEntity->Sp_X + ", " + ProbePath_LstEntity->Sp_Y + ", " + ProbePath_LstEntity->Sp_Z
			+ ", " + ProbePath_LstEntity->Sp_R);
	}
	pointsFile->WriteLine();
	pointsFile->Close();
}


////7 Pts Hub measurement with touch ONLY in Y-Axis
//void RWrapper::RW_DelphiCamModule::Hub_Position_7Pts(double TouchSearchSpeed, bool DoFirst_Y_Pt, bool TakePtsClock_Wise)
//{
//	if (DoingHobMeasurement || !HobParametersUpdated) return; //|| !GotAxis
//	try
//	{
//		RWrapper::PointsToGo^ ProbePath_LstEntity;
//		PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
//
//		Listcount = 0; FirstStep = true;
//		//startPosition = gcnew cli::array<double>(4);
//		//cPosition = gcnew cli::array<double>(4);
//		DoingCalibration = true;
//		double extraDist = HobParameters[14] / HobSettings[5] - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;
//
//		int pos_Dir = 1;
//		double FastSpeed = TouchSearchSpeed * HobSettings[4];
//		double SlowSpeed = TouchSearchSpeed;
//		//bool TakePtsClock_Wise = false;
//
//		//startPosition = gcnew cli::array<double>(4);
//		//startPosition[0] = ReferenceCenter[0] - HobParameters[14] / 2;
//		//startPosition[1] = ReferenceCenter[1] - 40;
//		//startPosition[2] = 65.5674;
//		//startPosition[3] = 0;
//
//		//if (RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] < ReferenceCenter[1] || startPosition[1] < ReferenceCenter[1])
//		//{
//		//	pos_Dir = -1;
//		//	TakePtsClock_Wise = true;
//		//}
//
//		//First Let us calculate the coordinates of all the target points - they will be 30 degrees apart, starting at 90 degrees and going to 270 degrees.
//		//We ignore the point at 180 degrees since the first point will be taken automatically. We include that in the circle point value, and hence we do not need to 
//		//take it again!
//		System::Collections::Generic::List<RWrapper::PointsToGo^> ^TargetPts = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
//		RWrapper::PointsToGo^ this_targetP;
//		int TargetCt = 0;
//		double Rad = HobParameters[14] / 2 + MAINDllOBJECT->ProbeRadius();
//		for (double angle = M_PI_2; angle <= 3 * M_PI_2; angle += M_PI / 6)
//		{
//			this_targetP = gcnew PointsToGo();
//			this_targetP->Pt_X = ReferenceCenter[0] + Rad * cos(angle);
//			this_targetP->Pt_Y = ReferenceCenter[1] + Rad * sin(angle);
//			this_targetP->Pt_Z = startPosition[2];
//			this_targetP->Pt_R = startPosition[3];
//			//If its the first two or last two points, its going to be a Y touch. Else an X- touch. Let us fix the speed so that we dont bang fast!!
//			if (angle < 3 * M_PI_4 || angle > 5 * M_PI_4) //if angle is less than 135 deg or greater than 225 degrees its Y 
//				this_targetP->Sp_Y = TouchSearchSpeed;
//			else
//				this_targetP->Sp_X = TouchSearchSpeed;
//
//			this_targetP->path = false;
//			TargetPts->Add(this_targetP);
//			TargetCt++;
//		}
//
//		//Now we have all the target points. We now calculate the intermediate pts so that we travel without hitting the component in the middle
//		//We do not need to travel to first pt since we have taken it during the centre locating cycle. 
//		if (!TakePtsClock_Wise)
//		{
//			if (DoFirst_Y_Pt)
//				PointsToGoList->Add(TargetPts[0]);
//
//			
//			//Move to second position in X 
//			//We touch in Y ONLY. The next point has to be a retract in X
//			for (int j = 1; j < 3; j++)
//			{
//				this_targetP = gcnew PointsToGo();
//				this_targetP->Pt_X = TargetPts[j]->Pt_X;
//				this_targetP->Pt_Y = TargetPts[j]->Pt_Y + extraDist; //pos_Dir * RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist; //
//				this_targetP->Pt_Z = startPosition[2];
//				this_targetP->Pt_R = startPosition[3];
//				this_targetP->path = true;
//				PointsToGoList->Add(this_targetP);
//				//Add the touch command now
//				PointsToGoList->Add(TargetPts[j]);
//			}
//
//			//Move to left in X
//			this_targetP = gcnew PointsToGo();
//			this_targetP->Pt_X = TargetPts[3]->Pt_X - extraDist;
//			this_targetP->Pt_Y = TargetPts[2]->Pt_Y + RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist;
//			this_targetP->Pt_Z = startPosition[2];
//			this_targetP->Pt_R = startPosition[3];
//			this_targetP->path = true;
//			PointsToGoList->Add(this_targetP);
//
//			//Move in Y  with extra dist to Pt 4 in list directly. 
//			this_targetP = gcnew PointsToGo();
//			this_targetP->Pt_X = TargetPts[3]->Pt_X - extraDist;
//			this_targetP->Pt_Y = TargetPts[4]->Pt_Y - extraDist; // RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist;
//			this_targetP->Pt_Z = startPosition[2];
//			this_targetP->Pt_R = startPosition[3];
//			this_targetP->path = true;
//			PointsToGoList->Add(this_targetP);
//
//			for (int j = 4; j < 7; j++)
//			{
//				this_targetP = gcnew PointsToGo();
//				this_targetP->Pt_X = TargetPts[j]->Pt_X;
//				this_targetP->Pt_Y = TargetPts[j]->Pt_Y - extraDist; //pos_Dir * RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist; //
//				this_targetP->Pt_Z = startPosition[2];
//				this_targetP->Pt_R = startPosition[3];
//				this_targetP->path = true;
//				PointsToGoList->Add(this_targetP);
//				//Add the touch command now
//				PointsToGoList->Add(TargetPts[j]);
//
//				if (j < 6)
//				{ // Retract in Y to the next point
//						this_targetP = gcnew PointsToGo();
//						this_targetP->Pt_X = TargetPts[j]->Pt_X;
//						this_targetP->Pt_Y = TargetPts[j + 1]->Pt_Y - extraDist; //pos_Dir * RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist; //
//						this_targetP->Pt_Z = startPosition[2];
//						this_targetP->Pt_R = startPosition[3];
//						this_targetP->path = true;
//						PointsToGoList->Add(this_targetP);
//				}
//			}
//		}
//		else
//		{
//			//If we are starting at the opposite side, we need to flip the order of the target points in the list. 
//			System::Collections::Generic::List<RWrapper::PointsToGo^> ^TargetPtsFlipped = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
//			for (int i = TargetPts->Count - 1; i >= 0; i--)
//				TargetPtsFlipped->Add(TargetPts[i]);
//			TargetPts->Clear();
//			TargetPts->AddRange(TargetPtsFlipped);
//
//			if (DoFirst_Y_Pt)
//				PointsToGoList->Add(TargetPts[0]);
//
//			//Move to second position in X 
//			//We touch in Y ONLY. The next point has to be a retract in X
//			for (int j = 1; j < 3; j++)
//			{
//				this_targetP = gcnew PointsToGo();
//				this_targetP->Pt_X = TargetPts[j]->Pt_X;
//				this_targetP->Pt_Y = TargetPts[j]->Pt_Y - extraDist; //pos_Dir * RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist; //
//				this_targetP->Pt_Z = startPosition[2];
//				this_targetP->Pt_R = startPosition[3];
//				this_targetP->path = true;
//				PointsToGoList->Add(this_targetP);
//				//Add the touch command now
//				PointsToGoList->Add(TargetPts[j]);
//			}
//
//			//Move to left in X
//			this_targetP = gcnew PointsToGo();
//			this_targetP->Pt_X = TargetPts[3]->Pt_X - extraDist;
//			this_targetP->Pt_Y = TargetPts[2]->Pt_Y - RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist;
//			this_targetP->Pt_Z = startPosition[2];
//			this_targetP->Pt_R = startPosition[3];
//			this_targetP->path = true;
//			PointsToGoList->Add(this_targetP);
//
//			//Move in Y  with extra dist to Pt 4 in list directly. 
//			this_targetP = gcnew PointsToGo();
//			this_targetP->Pt_X = TargetPts[3]->Pt_X - extraDist;
//			this_targetP->Pt_Y = TargetPts[4]->Pt_Y + extraDist; // RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist;
//			this_targetP->Pt_Z = startPosition[2];
//			this_targetP->Pt_R = startPosition[3];
//			this_targetP->path = true;
//			PointsToGoList->Add(this_targetP);
//
//			for (int j = 4; j < 7; j++)
//			{
//				this_targetP = gcnew PointsToGo();
//				this_targetP->Pt_X = TargetPts[j]->Pt_X;
//				this_targetP->Pt_Y = TargetPts[j]->Pt_Y + extraDist; //pos_Dir * RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist; //
//				this_targetP->Pt_Z = startPosition[2];
//				this_targetP->Pt_R = startPosition[3];
//				this_targetP->path = true;
//				PointsToGoList->Add(this_targetP);
//				//Add the touch command now
//				PointsToGoList->Add(TargetPts[j]);
//
//				if (j < 6)
//				{ // Retract in Y to the next point
//					this_targetP = gcnew PointsToGo();
//					this_targetP->Pt_X = TargetPts[j]->Pt_X;
//					this_targetP->Pt_Y = TargetPts[j + 1]->Pt_Y + extraDist; //pos_Dir * RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist; //
//					this_targetP->Pt_Z = startPosition[2];
//					this_targetP->Pt_R = startPosition[3];
//					this_targetP->path = true;
//					PointsToGoList->Add(this_targetP);
//				}
//			}
//		}
//	}
//	catch (...)
//	{
//
//	}
//}

//7 Pts Hub measurement with touch ONLY in X and Y-Axis (3 in X and 4 in Y)
void RWrapper::RW_DelphiCamModule::Hub_Position_7Pts(double TouchSearchSpeed, bool DoFirst_Y_Pt, bool TakePtsClock_Wise)
{
	if (DoingHobMeasurement || !HobParametersUpdated) return; //|| !GotAxis
	try
	{
		RWrapper::PointsToGo^ ProbePath_LstEntity;
		PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();

		Listcount = 0; FirstStep = true;
		//startPosition = gcnew cli::array<double>(4);
		//cPosition = gcnew cli::array<double>(4);
		DoingCalibration = true;
		double extraDist = HobParameters[14] / HobSettings[5] - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;

		int pos_Dir = 1;
		double FastSpeed = TouchSearchSpeed * HobSettings[4];
		double SlowSpeed = TouchSearchSpeed;
		//bool TakePtsClock_Wise = false;

		//startPosition = gcnew cli::array<double>(4);
		//startPosition[0] = ReferenceCenter[0] - HobParameters[14] / 2;
		//startPosition[1] = ReferenceCenter[1] - 40;
		//startPosition[2] = 65.5674;
		//startPosition[3] = 0;

		//if (RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] < ReferenceCenter[1] || startPosition[1] < ReferenceCenter[1])
		//{
		//	pos_Dir = -1;
		//	TakePtsClock_Wise = true;
		//}

		//First Let us calculate the coordinates of all the target points - they will be 30 degrees apart, starting at 90 degrees and going to 270 degrees.
		//We ignore the point at 180 degrees since the first point will be taken automatically. We include that in the circle point value, and hence we do not need to 
		//take it again!
		System::Collections::Generic::List<RWrapper::PointsToGo^> ^TargetPts = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
		RWrapper::PointsToGo^ this_targetP;
		int TargetCt = 0;
		double Rad = HobParameters[14] / 2 + MAINDllOBJECT->ProbeRadius();
		for (double angle = M_PI_2; angle <= 3 * M_PI_2; angle += M_PI / 6)
		{
			this_targetP = gcnew PointsToGo();
			this_targetP->Pt_X = ReferenceCenter[0] + Rad * cos(angle);
			this_targetP->Pt_Y = ReferenceCenter[1] + Rad * sin(angle);
			this_targetP->Pt_Z = startPosition[2];
			this_targetP->Pt_R = startPosition[3];
			//If its the first two or last two points, its going to be a Y touch. Else an X- touch. Let us fix the speed so that we dont bang fast!!
			if (angle < 3 * M_PI_4 || angle > 5 * M_PI_4) //if angle is less than 135 deg or greater than 225 degrees its Y 
				this_targetP->Sp_Y = TouchSearchSpeed;
			else
				this_targetP->Sp_X = TouchSearchSpeed;

			this_targetP->path = false;
			TargetPts->Add(this_targetP);
			TargetCt++;
		}

		//Now we have all the target points. We now calculate the intermediate pts so that we travel without hitting the component in the middle
		//We do not need to travel to first pt since we have taken it during the centre locating cycle. 
		if (!TakePtsClock_Wise)
		{
			if (DoFirst_Y_Pt)
				PointsToGoList->Add(TargetPts[0]);


			//Move to second position in X 
			//We touch in Y ONLY. The next point has to be a retract in X
			for (int j = 1; j < 2; j++)
			{
				this_targetP = gcnew PointsToGo();
				this_targetP->Pt_X = TargetPts[j]->Pt_X;
				this_targetP->Pt_Y = TargetPts[j]->Pt_Y + extraDist; //pos_Dir * RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist; //
				this_targetP->Pt_Z = startPosition[2];
				this_targetP->Pt_R = startPosition[3];
				this_targetP->path = true;
				PointsToGoList->Add(this_targetP);
				//Add the touch command now
				PointsToGoList->Add(TargetPts[j]);
			}

			//Move to left in X
			this_targetP = gcnew PointsToGo();
			this_targetP->Pt_X = TargetPts[3]->Pt_X - extraDist;
			this_targetP->Pt_Y = TargetPts[2]->Pt_Y + RW_DRO::MYINSTANCE()->RevertDistance; // ProbeParams[1]->RetractDist;
			this_targetP->Pt_Z = startPosition[2];
			this_targetP->Pt_R = startPosition[3];
			this_targetP->path = true;
			PointsToGoList->Add(this_targetP);

			////Move in Y to get to correct position for touch in X
			//this_targetP = gcnew PointsToGo();
			//this_targetP->Pt_X = TargetPts[3]->Pt_X - extraDist;
			//this_targetP->Pt_Y = TargetPts[3]->Pt_Y; // +RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist;
			//this_targetP->Pt_Z = startPosition[2];
			//this_targetP->Pt_R = startPosition[3];
			//this_targetP->path = true;
			//PointsToGoList->Add(this_targetP);

			//Next three points, we touch in X. So lets run a for loop
			for (int j = 2; j < 5; j++)
			{
				this_targetP = gcnew PointsToGo();
				this_targetP->Pt_X = TargetPts[j]->Pt_X - extraDist;
				this_targetP->Pt_Y = TargetPts[j]->Pt_Y; // -extraDist; //pos_Dir * RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist; //
				this_targetP->Pt_Z = startPosition[2];
				this_targetP->Pt_R = startPosition[3];
				this_targetP->path = true;
				this_targetP->Sp_X = 8; this_targetP->Sp_Y = 4;
				PointsToGoList->Add(this_targetP);
				//Add the touch command now
				PointsToGoList->Add(TargetPts[j]);
			}

			//Next two points touch in Y
			for (int j = 5; j < 7; j++)
			{			//Travel in Y to the next point
				this_targetP = gcnew PointsToGo();
				this_targetP->Pt_X = TargetPts[j - 1]->Pt_X;
				this_targetP->Pt_Y = TargetPts[j]->Pt_Y - extraDist; // -extraDist; //pos_Dir * RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist; //
				this_targetP->Pt_Z = startPosition[2];
				this_targetP->Pt_R = startPosition[3];
				this_targetP->path = true;
				this_targetP->Sp_X = 4; this_targetP->Sp_Y = 8;
				PointsToGoList->Add(this_targetP);

				this_targetP = gcnew PointsToGo();
				this_targetP->Pt_X = TargetPts[j]->Pt_X;
				this_targetP->Pt_Y = TargetPts[j]->Pt_Y - extraDist; // -extraDist; //pos_Dir * RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist; //
				this_targetP->Pt_Z = startPosition[2];
				this_targetP->Pt_R = startPosition[3];
				this_targetP->path = true;
				this_targetP->Sp_X = 4; this_targetP->Sp_Y = 8;
				PointsToGoList->Add(this_targetP);

				//Add Target Pt
				PointsToGoList->Add(TargetPts[j]);
			}
		}
		else
		{
			//If we are starting at the opposite side, we need to flip the order of the target points in the list. 
			System::Collections::Generic::List<RWrapper::PointsToGo^> ^TargetPtsFlipped = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
			for (int i = TargetPts->Count - 1; i >= 0; i--)
				TargetPtsFlipped->Add(TargetPts[i]);
			TargetPts->Clear();
			TargetPts->AddRange(TargetPtsFlipped);

			if (DoFirst_Y_Pt)
				PointsToGoList->Add(TargetPts[0]);

			//Move to second position in X 
			//We touch in Y ONLY. The next point has to be a retract in X
			for (int j = 1; j < 2; j++)
			{
				this_targetP = gcnew PointsToGo();
				this_targetP->Pt_X = TargetPts[j]->Pt_X;
				this_targetP->Pt_Y = TargetPts[j]->Pt_Y - extraDist; //pos_Dir * RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist; //
				this_targetP->Pt_Z = startPosition[2];
				this_targetP->Pt_R = startPosition[3];
				this_targetP->path = true;
				PointsToGoList->Add(this_targetP);
				//Add the touch command now
				PointsToGoList->Add(TargetPts[j]);
			}

			//Move to left in X
			this_targetP = gcnew PointsToGo();
			this_targetP->Pt_X = TargetPts[3]->Pt_X - extraDist;
			this_targetP->Pt_Y = TargetPts[2]->Pt_Y - RW_DRO::MYINSTANCE()->RevertDistance; // ProbeParams[1]->RetractDist;
			this_targetP->Pt_Z = startPosition[2];
			this_targetP->Pt_R = startPosition[3];
			this_targetP->path = true;
			PointsToGoList->Add(this_targetP);

			////Move in Y to get to correct position for touch in X
			//this_targetP = gcnew PointsToGo();
			//this_targetP->Pt_X = TargetPts[3]->Pt_X - extraDist;
			//this_targetP->Pt_Y = TargetPts[3]->Pt_Y; // +RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist;
			//this_targetP->Pt_Z = startPosition[2];
			//this_targetP->Pt_R = startPosition[3];
			//this_targetP->path = true;
			//PointsToGoList->Add(this_targetP);

			//Next three points, we touch in X. So lets run a for loop
			for (int j = 2; j < 5; j++)
			{
				this_targetP = gcnew PointsToGo();
				this_targetP->Pt_X = TargetPts[j]->Pt_X - extraDist;
				this_targetP->Pt_Y = TargetPts[j]->Pt_Y; // -extraDist; //pos_Dir * RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist; //
				this_targetP->Pt_Z = startPosition[2];
				this_targetP->Pt_R = startPosition[3];
				this_targetP->path = true;
				this_targetP->Sp_X = 8; this_targetP->Sp_Y = 4;
				PointsToGoList->Add(this_targetP);
				//Add the touch command now
				PointsToGoList->Add(TargetPts[j]);
			}
			for (int j = 5; j < 7; j++)
			{			
				//Travel in Y to the next point
				this_targetP = gcnew PointsToGo();
				this_targetP->Pt_X = TargetPts[j - 1]->Pt_X;
				this_targetP->Pt_Y = TargetPts[j]->Pt_Y + extraDist; // -extraDist; //pos_Dir * RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist; //
				this_targetP->Pt_Z = startPosition[2];
				this_targetP->Pt_R = startPosition[3];
				this_targetP->path = true;
				this_targetP->Sp_X = 4; this_targetP->Sp_Y = 8;
				PointsToGoList->Add(this_targetP);

				this_targetP = gcnew PointsToGo();
				this_targetP->Pt_X = TargetPts[j]->Pt_X;
				this_targetP->Pt_Y = TargetPts[j]->Pt_Y + extraDist; // -extraDist; //pos_Dir * RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist; //
				this_targetP->Pt_Z = startPosition[2];
				this_targetP->Pt_R = startPosition[3];
				this_targetP->path = true;
				this_targetP->Sp_X = 4; this_targetP->Sp_Y = 8;
				PointsToGoList->Add(this_targetP);

				//Add Target Pt
				PointsToGoList->Add(TargetPts[j]);
			}
		}
	}
	catch (...)
	{

	}
}

void RWrapper::RW_DelphiCamModule::Hub_Position_3Pts(double TouchSearchSpeed, bool DoFirst_Y_Pt, bool TakePtsClock_Wise)
{
	if (DoingHobMeasurement || !HobParametersUpdated) return; //|| !GotAxis
	try
	{
		RWrapper::PointsToGo^ ProbePath_LstEntity;
		PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();

		Listcount = 0; FirstStep = true;
		//startPosition = gcnew cli::array<double>(4);
		//cPosition = gcnew cli::array<double>(4);
		DoingCalibration = true;
		double extraDist = HobParameters[14] / HobSettings[5] - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;

		int pos_Dir = 1;
		double FastSpeed = TouchSearchSpeed * HobSettings[4];
		double SlowSpeed = TouchSearchSpeed;
		//bool TakePtsClock_Wise = false;

		//startPosition = gcnew cli::array<double>(4);
		//startPosition[0] = ReferenceCenter[0] - HobParameters[14] / 2;
		//startPosition[1] = ReferenceCenter[1] - 40;
		//startPosition[2] = 65.5674;
		//startPosition[3] = 0;

		//if (RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] < ReferenceCenter[1] || startPosition[1] < ReferenceCenter[1])
		//{
		//	pos_Dir = -1;
		//	TakePtsClock_Wise = true;
		//}

		//First Let us calculate the coordinates of all the target points - they will be 30 degrees apart, starting at 90 degrees and going to 270 degrees.
		//We ignore the point at 180 degrees since the first point will be taken automatically. We include that in the circle point value, and hence we do not need to 
		//take it again!
		System::Collections::Generic::List<RWrapper::PointsToGo^> ^TargetPts = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
		RWrapper::PointsToGo^ this_targetP;
		int TargetCt = 0;
		double Rad = HobParameters[14] / 2 + MAINDllOBJECT->ProbeRadius();
		for (double angle = M_PI_2; angle <= 3 * M_PI_2; angle += M_PI / 6)
		{
			this_targetP = gcnew PointsToGo();
			this_targetP->Pt_X = ReferenceCenter[0] + Rad * cos(angle);
			this_targetP->Pt_Y = ReferenceCenter[1] + Rad * sin(angle);
			this_targetP->Pt_Z = startPosition[2];
			this_targetP->Pt_R = startPosition[3];
			//If its the first two or last two points, its going to be a Y touch. Else an X- touch. Let us fix the speed so that we dont bang fast!!
			if (angle < 3 * M_PI_4 || angle > 5 * M_PI_4) //if angle is less than 135 deg or greater than 225 degrees its Y 
				this_targetP->Sp_Y = TouchSearchSpeed;
			else
				this_targetP->Sp_X = TouchSearchSpeed;

			this_targetP->path = false;
			TargetPts->Add(this_targetP);
			TargetCt++;
		}

		//Now we have all the target points. We now calculate the intermediate pts so that we travel without hitting the component in the middle
		//We do not need to travel to first pt since we have taken it during the centre locating cycle. 
		if (!TakePtsClock_Wise)
		{
			if (DoFirst_Y_Pt)
				PointsToGoList->Add(TargetPts[0]);

			//Move to left in X
			this_targetP = gcnew PointsToGo();
			this_targetP->Pt_X = startPosition[0] - extraDist;
			this_targetP->Pt_Y = ReferenceCenter[1] + HobParameters[14] / 2 + extraDist; // +RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist;
			this_targetP->Pt_Z = startPosition[2];
			this_targetP->Pt_R = startPosition[3];
			this_targetP->path = true;
			PointsToGoList->Add(this_targetP);

			//Now move to the opposite end, if we have already taken first point at the centre
			this_targetP = gcnew PointsToGo();
			this_targetP->Pt_X = startPosition[0] - extraDist;
			this_targetP->Pt_Y = ReferenceCenter[1] - (HobParameters[14] / 2 + extraDist);
			this_targetP->Pt_Z = startPosition[2];
			this_targetP->Pt_R = startPosition[3];
			this_targetP->path = true;
			PointsToGoList->Add(this_targetP);

			//Move to right in X
			this_targetP = gcnew PointsToGo();
			this_targetP->Pt_X = TargetPts[TargetPts->Count - 1]->Pt_X;
			this_targetP->Pt_Y = ReferenceCenter[1] - (HobParameters[14] / 2 + extraDist); // +RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist;
			this_targetP->Pt_Z = startPosition[2];
			this_targetP->Pt_R = startPosition[3];
			this_targetP->path = true;
			PointsToGoList->Add(this_targetP);

			//Add Target Pt
			PointsToGoList->Add(TargetPts[TargetPts->Count - 1]);
		}
		else
		{
			//If we are starting at the opposite side, we need to flip the order of the target points in the list. 
			System::Collections::Generic::List<RWrapper::PointsToGo^> ^TargetPtsFlipped = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
			for (int i = TargetPts->Count - 1; i >= 0; i--)
				TargetPtsFlipped->Add(TargetPts[i]);
			TargetPts->Clear();
			TargetPts->AddRange(TargetPtsFlipped);

			if (DoFirst_Y_Pt)
				PointsToGoList->Add(TargetPts[0]);

			//Move to left in X
			this_targetP = gcnew PointsToGo();
			this_targetP->Pt_X = ReferenceCenter[0] - CylinderDia / 2 - extraDist;
			this_targetP->Pt_Y = ReferenceCenter[1] - (HobParameters[14] / 2 + extraDist);  // TargetPts[0]->Pt_Y - RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist;
			this_targetP->Pt_Z = startPosition[2];
			this_targetP->Pt_R = startPosition[3];
			this_targetP->path = true;
			PointsToGoList->Add(this_targetP);

			//Now move to the opposite end, if we have already taken first point at the centre
			this_targetP = gcnew PointsToGo();
			this_targetP->Pt_X = ReferenceCenter[0] - CylinderDia / 2 - extraDist;
			this_targetP->Pt_Y = ReferenceCenter[1]; // +(HobParameters[14] / 2 + extraDist); // TargetPts[0]->Pt_Y + 2 * (HobParameters[14] + RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist) + extraDist;
			this_targetP->Pt_Z = startPosition[2];
			this_targetP->Pt_R = startPosition[3];
			this_targetP->path = true;
			PointsToGoList->Add(this_targetP);

			PointsToGoList->Add(TargetPts[TargetPts->Count / 2]);

			//Now move to the opposite end, if we have already taken first point at the centre
			this_targetP = gcnew PointsToGo();
			this_targetP->Pt_X = ReferenceCenter[0] - CylinderDia / 2 - extraDist;
			this_targetP->Pt_Y = ReferenceCenter[1] +(HobParameters[14] / 2 + extraDist); // TargetPts[0]->Pt_Y + 2 * (HobParameters[14] + RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist) + extraDist;
			this_targetP->Pt_Z = startPosition[2];
			this_targetP->Pt_R = startPosition[3];
			this_targetP->path = true;
			PointsToGoList->Add(this_targetP);

			//Move to right in X
			this_targetP = gcnew PointsToGo();
			this_targetP->Pt_X = TargetPts[TargetPts->Count - 1]->Pt_X;
			this_targetP->Pt_Y = ReferenceCenter[1] + (HobParameters[14] / 2 + extraDist); // +RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist;
			this_targetP->Pt_Z = startPosition[2];
			this_targetP->Pt_R = startPosition[3];
			this_targetP->path = true;
			PointsToGoList->Add(this_targetP);


			//Add Target Pt
			PointsToGoList->Add(TargetPts[TargetPts->Count - 1]);

		}
	}
	catch (...)
	{

	}
}


//Two Step movement and 7 pt hub
/*void RWrapper::RW_DelphiCamModule::Hub_Position_7Pts(double TouchSearchSpeed, bool DoFirst_Y_Pt, bool TakePtsClock_Wise)
{
	if (DoingHobMeasurement || !HobParametersUpdated) return; //|| !GotAxis
	try
	{
		RWrapper::PointsToGo^ ProbePath_LstEntity;
		PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();

		Listcount = 0; FirstStep = true;
		//startPosition = gcnew cli::array<double>(4);
		//cPosition = gcnew cli::array<double>(4);
		DoingCalibration = true;
		double extraDist = HobParameters[14] / HobSettings[5] - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;

		int pos_Dir = 1;
		double FastSpeed = TouchSearchSpeed * HobSettings[4];
		double SlowSpeed = TouchSearchSpeed;
		//bool TakePtsClock_Wise = false;

		//startPosition = gcnew cli::array<double>(4);
		//startPosition[0] = ReferenceCenter[0] - HobParameters[14] / 2;
		//startPosition[1] = ReferenceCenter[1] - 40;
		//startPosition[2] = 65.5674;
		//startPosition[3] = 0;

		//if (RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] < ReferenceCenter[1] || startPosition[1] < ReferenceCenter[1])
		//{
		//	pos_Dir = -1;
		//	TakePtsClock_Wise = true;
		//}

		//First Let us calculate the coordinates of all the target points - they will be 30 degrees apart, starting at 90 degrees and going to 270 degrees.
		//We ignore the point at 180 degrees since the first point will be taken automatically. We include that in the circle point value, and hence we do not need to 
		//take it again!
		System::Collections::Generic::List<RWrapper::PointsToGo^> ^TargetPts = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
		RWrapper::PointsToGo^ this_targetP;
		int TargetCt = 0;
		double Rad = HobParameters[14] / 2 + MAINDllOBJECT->ProbeRadius();
		for (double angle = M_PI_2; angle <= 3 * M_PI_2; angle += M_PI / 6)
		{
			this_targetP = gcnew PointsToGo();
			this_targetP->Pt_X = ReferenceCenter[0] + Rad * cos(angle);
			this_targetP->Pt_Y = ReferenceCenter[1] + Rad * sin(angle);
			this_targetP->Pt_Z = startPosition[2]; 
			this_targetP->Pt_R = startPosition[3];
			//If its the first two or last two points, its going to be a Y touch. Else an X- touch. Let us fix the speed so that we dont bang fast!!
			if (angle < 3 * M_PI_4 || angle > 5 * M_PI_4) //if angle is less than 135 deg or greater than 225 degrees its Y 
				this_targetP->Sp_Y = TouchSearchSpeed;
			else
				this_targetP->Sp_X = TouchSearchSpeed;

			this_targetP->path = false;
			TargetPts->Add(this_targetP);
			TargetCt++;
		}

		//Now we have all the target points. We now calculate the intermediate pts so that we travel without hitting the component in the middle
		//We do not need to travel to first pt since we have taken it during the centre locating cycle. 
		if (!TakePtsClock_Wise)
		{
			if (DoFirst_Y_Pt)
				PointsToGoList->Add(TargetPts[0]);

			//Move to second position in X 
			//We touch in Y currently. The next point has to be a retract in X
			this_targetP = gcnew PointsToGo();
			this_targetP->Pt_X = TargetPts[1]->Pt_X;
			this_targetP->Pt_Y = TargetPts[1]->Pt_Y + extraDist; //pos_Dir * RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist; //
			this_targetP->Pt_Z = startPosition[2];
			this_targetP->Pt_R = startPosition[3];
			this_targetP->path = true;
			PointsToGoList->Add(this_targetP);
			//Add the touch command now
			PointsToGoList->Add(TargetPts[1]);

			//Run a loop for the 2 points to be taken in X touch.
			for (int j = 2; j < 4; j++)
			{
				//Move to left in X
				this_targetP = gcnew PointsToGo();
				this_targetP->Pt_X = TargetPts[j]->Pt_X - extraDist;
				this_targetP->Pt_Y = TargetPts[j - 1]->Pt_Y + RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist;
				this_targetP->Pt_Z = startPosition[2];
				this_targetP->Pt_R = startPosition[3];
				this_targetP->path = true;
				PointsToGoList->Add(this_targetP);

				//Move in Y to next position
				this_targetP = gcnew PointsToGo();
				this_targetP->Pt_X = TargetPts[j]->Pt_X - extraDist;
				this_targetP->Pt_Y = TargetPts[j]->Pt_Y;
				this_targetP->Pt_Z = startPosition[2];
				this_targetP->Pt_R = startPosition[3];
				this_targetP->path = true;
				PointsToGoList->Add(this_targetP);

				//Add the 3rd touch command (first in X)
				PointsToGoList->Add(TargetPts[j]);
			}

			//For Point #5, the circle is receding. So we need to first move in Y and then in X.
			//Move to left in X
			this_targetP = gcnew PointsToGo();
			this_targetP->Pt_X = TargetPts[3]->Pt_X - extraDist;
			this_targetP->Pt_Y = TargetPts[4]->Pt_Y; // +RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist;
			this_targetP->Pt_Z = startPosition[2];
			this_targetP->Pt_R = startPosition[3];
			this_targetP->path = true;
			PointsToGoList->Add(this_targetP);
			//Add command for Pt #5
			PointsToGoList->Add(TargetPts[4]);

			for (int j = 5; j < 7; j++)
			{		//Goto Point # 6. We move back in X and down in Y extra so that we can move to right and take point in Y axis
				this_targetP = gcnew PointsToGo();
				this_targetP->Pt_X = TargetPts[j - 1]->Pt_X - extraDist; // RW_DRO::MYINSTANCE()->ProbeParams[0]->RetractDist;
				this_targetP->Pt_Y = TargetPts[j]->Pt_Y - extraDist; 
				this_targetP->Pt_Z = startPosition[2];
				this_targetP->Pt_R = startPosition[3];
				this_targetP->path = true;
				PointsToGoList->Add(this_targetP);

				//Now move in X to correct X position
				this_targetP = gcnew PointsToGo();
				this_targetP->Pt_X = TargetPts[j]->Pt_X;
				this_targetP->Pt_Y = TargetPts[j]->Pt_Y - extraDist; 
				this_targetP->Pt_Z = startPosition[2];
				this_targetP->Pt_R = startPosition[3];
				this_targetP->path = true;
				PointsToGoList->Add(this_targetP);

				//Add command for Pt #6
				PointsToGoList->Add(TargetPts[j]);
			}
		}
		else
		{
			//If we are starting at the opposite side, we need to flip the order of the target points in the list. 
			System::Collections::Generic::List<RWrapper::PointsToGo^> ^TargetPtsFlipped = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
			for (int i = TargetPts->Count - 1; i >= 0; i--)
				TargetPtsFlipped->Add(TargetPts[i]);
			TargetPts->Clear();
			TargetPts->AddRange(TargetPtsFlipped);

			if (DoFirst_Y_Pt)
				PointsToGoList->Add(TargetPts[0]);

			//Move to second position in X 
			//We touch in Y currently. The next point has to be a retract in X
			this_targetP = gcnew PointsToGo();
			this_targetP->Pt_X = TargetPts[1]->Pt_X;
			this_targetP->Pt_Y = TargetPts[1]->Pt_Y - extraDist; //pos_Dir * RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist; //
			this_targetP->Pt_Z = startPosition[2];
			this_targetP->Pt_R = startPosition[3];
			this_targetP->path = true;
			PointsToGoList->Add(this_targetP);
			//Add the touch command now
			PointsToGoList->Add(TargetPts[1]);

			//Run a loop for the 2 points to be taken in X touch.
			for (int j = 2; j < 4; j++)
			{
				//Move to left in X
				this_targetP = gcnew PointsToGo();
				this_targetP->Pt_X = TargetPts[j]->Pt_X - extraDist;
				this_targetP->Pt_Y = TargetPts[j - 1]->Pt_Y - RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist;
				this_targetP->Pt_Z = startPosition[2];
				this_targetP->Pt_R = startPosition[3];
				this_targetP->path = true;
				PointsToGoList->Add(this_targetP);

				//Move in Y to next position
				this_targetP = gcnew PointsToGo();
				this_targetP->Pt_X = TargetPts[j]->Pt_X - extraDist;
				this_targetP->Pt_Y = TargetPts[j]->Pt_Y;
				this_targetP->Pt_Z = startPosition[2];
				this_targetP->Pt_R = startPosition[3];
				this_targetP->path = true;
				PointsToGoList->Add(this_targetP);

				//Add the 3rd touch command (first in X)
				PointsToGoList->Add(TargetPts[j]);
			}

			//For Point #5, the circle is receding. So we need to first move in Y and then in X.
			//Move to left in X
			this_targetP = gcnew PointsToGo();
			this_targetP->Pt_X = TargetPts[3]->Pt_X - extraDist;
			this_targetP->Pt_Y = TargetPts[4]->Pt_Y; // +RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist;
			this_targetP->Pt_Z = startPosition[2];
			this_targetP->Pt_R = startPosition[3];
			this_targetP->path = true;
			PointsToGoList->Add(this_targetP);
			//Add command for Pt #5
			PointsToGoList->Add(TargetPts[4]);

			for (int j = 5; j < 7; j++)
			{		//Goto Point # 6. We move back in X and down in Y extra so that we can move to right and take point in Y axis
				this_targetP = gcnew PointsToGo();
				this_targetP->Pt_X = TargetPts[j - 1]->Pt_X - RW_DRO::MYINSTANCE()->ProbeParams[0]->RetractDist;
				this_targetP->Pt_Y = TargetPts[j]->Pt_Y + extraDist;
				this_targetP->Pt_Z = startPosition[2];
				this_targetP->Pt_R = startPosition[3];
				this_targetP->path = true;
				PointsToGoList->Add(this_targetP);

				//Now move in X to correct X position
				this_targetP = gcnew PointsToGo();
				this_targetP->Pt_X = TargetPts[j]->Pt_X;
				this_targetP->Pt_Y = TargetPts[j]->Pt_Y + extraDist;
				this_targetP->Pt_Z = startPosition[2];
				this_targetP->Pt_R = startPosition[3];
				this_targetP->path = true;
				PointsToGoList->Add(this_targetP);

				//Add command for Pt #6
				PointsToGoList->Add(TargetPts[j]);
			}


		}



	}
	catch (...)
	{

	}
}
*/

//Single Step Attempt for 7 Pts. 
/*void RWrapper::RW_DelphiCamModule::Hub_Position_7Pts(double TouchSearchSpeed, bool DoFirst_Y_Pt, bool TakePtsClock_Wise)
{
	if (DoingHobMeasurement || !HobParametersUpdated) return; //|| !GotAxis
	try
	{
		RWrapper::PointsToGo^ ProbePath_LstEntity;
		PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();

		Listcount = 0; FirstStep = true;
		//startPosition = gcnew cli::array<double>(4);
		//cPosition = gcnew cli::array<double>(4);
		DoingCalibration = true;
		double extraDist = HobParameters[14] / HobSettings[5] - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;

		int pos_Dir = 1;
		double FastSpeed = TouchSearchSpeed * HobSettings[4];
		double SlowSpeed = TouchSearchSpeed;
		//bool TakePtsClock_Wise = false;

		//startPosition = gcnew cli::array<double>(4);
		//startPosition[0] = ReferenceCenter[0] - HobParameters[14] / 2;
		//startPosition[1] = ReferenceCenter[1] - 40;
		//startPosition[2] = 65.5674;
		//startPosition[3] = 0;

		//if (RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] < ReferenceCenter[1] || startPosition[1] < ReferenceCenter[1])
		//{
		//	pos_Dir = -1;
		//	TakePtsClock_Wise = true;
		//}

		//First Let us calculate the coordinates of all the target points - they will be 30 degrees apart, starting at 90 degrees and going to 270 degrees.
		//We ignore the point at 180 degrees since the first point will be taken automatically. We include that in the circle point value, and hence we do not need to 
		//take it again!
		System::Collections::Generic::List<RWrapper::PointsToGo^> ^TargetPts = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
		RWrapper::PointsToGo^ this_targetP;
		int TargetCt = 0;
		for (double angle = M_PI_2; angle <= 3 * M_PI_2; angle += M_PI / 6)
		{
			this_targetP = gcnew PointsToGo();
			this_targetP->Pt_X = ReferenceCenter[0] + HobParameters[14] * cos(angle) / 2;
			this_targetP->Pt_Y = ReferenceCenter[1] + HobParameters[14] * sin(angle) / 2;
			this_targetP->Pt_Z = startPosition[2];
			this_targetP->Pt_R = startPosition[3];
			//If its the first two or last two points, its going to be a Y touch. Else an X- touch. Let us fix the speed so that we dont bang fast!!
			if (angle < 3 * M_PI_4 || angle > 5 * M_PI_4) //if angle is less than 135 deg or greater than 225 degrees its Y 
				this_targetP->Sp_Y = TouchSearchSpeed;
			else
				this_targetP->Sp_X = TouchSearchSpeed;

			this_targetP->path = false;
			TargetPts->Add(this_targetP);
			TargetCt++;
		}

		//If we are starting at the opposite side, we need to flip the order of the target points in the list. 
		if (TakePtsClock_Wise)
		{
			System::Collections::Generic::List<RWrapper::PointsToGo^> ^TargetPtsFlipped = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
			for (int i = TargetPts->Count - 1; i >= 0; i--)
				TargetPtsFlipped->Add(TargetPts[i]);
			TargetPts->Clear();
			TargetPts->AddRange(TargetPtsFlipped);
		}
		//Now we have all the target points. We now calculate the intermediate pts so that we travel without hitting the component in the middle
		//We do not need to travel to first pt since we have taken it during the centre locating cycle. 
		for (int i = 0; i < TargetCt; i++)
		{
			if (i == 0)
			{
				if (DoFirst_Y_Pt)
					PointsToGoList->Add(TargetPts[i]);
				continue;
			}

			this_targetP = gcnew PointsToGo();
			if (TargetPts[i]->Pt_Y < ReferenceCenter[1])
				pos_Dir = -1;
			else
				pos_Dir = 1;

			if (TargetPts[i]->Sp_Y == TouchSearchSpeed)
			{
				this_targetP->Pt_X = TargetPts[i]->Pt_X;
				TargetPts[i]->Sp_Y = TouchSearchSpeed * 2;
				TargetPts[i]->Sp_X = FastSpeed;
				if (pos_Dir == 1)
				{
					if (TakePtsClock_Wise)
					{
						//this_targetP->Pt_X = TargetPts[i]->Pt_X - 4;
						this_targetP->Sp_X = SlowSpeed;
						this_targetP->Sp_Y = FastSpeed;
					}
					else
					{
						this_targetP->Sp_X = FastSpeed;
						this_targetP->Sp_Y = SlowSpeed;
						//this_targetP->Pt_Y = TargetPts[i - 1]->Pt_Y + pos_Dir * RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist; //+ pos_Dir * extraDist;
					}
					this_targetP->Pt_Y = TargetPts[i]->Pt_Y + pos_Dir * extraDist; //pos_Dir * RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist; //
				}
				else
				{
					if (TakePtsClock_Wise)
					{
						this_targetP->Sp_X = FastSpeed;
						this_targetP->Sp_Y = SlowSpeed;
					}
					else
					{
						this_targetP->Pt_X = TargetPts[i]->Pt_X - 4;
						this_targetP->Sp_X = SlowSpeed;
						this_targetP->Sp_Y = FastSpeed;
					}
					this_targetP->Pt_Y = TargetPts[i]->Pt_Y + pos_Dir * extraDist;
				}
			}
			else
			{
				this_targetP->Pt_X = TargetPts[i]->Pt_X - extraDist;
				this_targetP->Pt_Y = TargetPts[i - 1]->Pt_Y + pos_Dir * RW_DRO::MYINSTANCE()->ProbeParams[1]->RetractDist;
				TargetPts[i]->Sp_X = TouchSearchSpeed * 2;
				TargetPts[i]->Sp_Y = FastSpeed;

				if (pos_Dir == 1)
				{
					if (TakePtsClock_Wise)
					{
						this_targetP->Sp_X = SlowSpeed;
						this_targetP->Sp_Y = FastSpeed;
					}
					else
					{
						this_targetP->Sp_X = FastSpeed;
						this_targetP->Sp_Y = SlowSpeed;
					}
				}
				else
				{
					if (TakePtsClock_Wise)
					{
						this_targetP->Sp_X = FastSpeed;
						this_targetP->Sp_Y = SlowSpeed;
					}
					else
					{
						this_targetP->Sp_X = SlowSpeed;
						this_targetP->Sp_Y = FastSpeed;
					}
				}
			}
			this_targetP->Pt_Z = startPosition[2];
			this_targetP->Pt_R = startPosition[3];
			this_targetP->path = true;
			PointsToGoList->Add(this_targetP);
			PointsToGoList->Add(TargetPts[i]);
		}


	}
	catch (...)
	{

	}
}
*/
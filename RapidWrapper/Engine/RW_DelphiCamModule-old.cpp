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
double _slope = 0.0, _intercept = 0.0, _radius = 0.0, cir_sec_angle = 0.0;
bool FirstStep, MasterCylinderFirstStep;
bool Doing_Y_Check, Zeroing_in_C_Axis;
int StartToothStepNumber;

RWrapper::RW_DelphiCamModule::RW_DelphiCamModule()
{
	 try
	 {
		 DoingCalibration = false;
		 ManualPoint = false;
		 Getting_BC_Axis = false; GotAxis = false;
		 Listcount = 0;
		 DelphiCamModule = this;
		 ReferenceCenter = gcnew array<double>(4);
		 BC_Values = gcnew array<double>(7);
		 InnerCirclePoint = gcnew array<double>(4);
		 LastTargetValue = gcnew array<double>(4);
		 HobParameters = gcnew array<double>(19);
		 ToothMidPt = gcnew array<double>(4);
		 Hob_RunoutParams = (double*)malloc(7 * sizeof(double)); // gcnew array<double>(7);
		
		 firsttime = true;
		 lastPoint = NULL;
		 DoingHobMeasurement = false;
		 HobMeasureisComplete = false;
		 HobParametersUpdated = false;
		 for (int i = 0; i < 4; i ++) ReferenceCenter[i] = 0;
		 for (int i = 0; i < HobParameters->Length; i ++) HobParameters[i] = 0;
		 for (int i = 0; i < 7; i ++) BC_Values[i] = 0;
		 for (int i = 0; i < 7; i ++) Hob_RunoutParams[i] = 0;
		 InPauseState = false;
	 }
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule001", ex); }
}

RWrapper::RW_DelphiCamModule::~RW_DelphiCamModule()
{
	free(Hob_RunoutParams);
}

RWrapper::RW_DelphiCamModule^ RWrapper::RW_DelphiCamModule::MYINSTANCE()
{
	return DelphiCamModule;
}

void RWrapper::RW_DelphiCamModule::Fill__Circle_Centre(double x_coord, double y_coord, double z_coord, double slope_, double intercept_, double radius_, double sector_angle)
{
	circleCenter[0]=x_coord;															// for next fn.RW_DelphiCamModule, we receive circle,ray,sector_angle parameters here..
	circleCenter[1]=y_coord;
	circleCenter[2]=z_coord;
	_slope=slope_; 
	_intercept=intercept_;
	_radius=radius_;
	cir_sec_angle=sector_angle;
}

bool RWrapper::RW_DelphiCamModule::Start(double MaxRadius, double MinRadius, bool Is_ID_tool)		//min radius used only for OD
{
	try
	{	DelphiCamType = 0;	
		InPauseState = false;
		DoingCalibration = true;
		firsttime = true;
		Getting_BC_Axis = false;
		lastPoint = NULL;
		RCadApp::Set_IsDelphiCam(true);						//set bool in rcadapp so we know that now if point taken by touch probe,dont draw cross mark,looks clumsy 
				
		ReferenceCenter[0] = circleCenter[0];
		ReferenceCenter[1] = circleCenter[1];
		ReferenceCenter[2] = circleCenter[2];

		double interceptPoint[4] = {0};
		//double slope = parenLine->Angle(), intercept = parenLine->Intercept(), radius = parentCircle->Radius();
		if(RMATH2DOBJECT->Line_circleintersection(_slope, _intercept, circleCenter, _radius, interceptPoint) < 2) return false;
		double ProbeOffsetPosition[4] = {MAINDllOBJECT->ProbeOffsetX, MAINDllOBJECT->ProbeOffsetY, 0, 0};
		RWrapper::PointsToGo^ ProbePath_LstEntity;
		Listcount = 0;
		PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
		
		int MeasureCount = 360 / cir_sec_angle;
		if(Is_ID_tool)
		{	DelphiCamType = 1;
			for(int i = 0; i < MeasureCount; i++)						//for ID we choose a expected path and possible way to take touch probe points here....
			{
				double Tmpangle = i * cir_sec_angle * M_PI / 180, ProbePts[4] = {0};
				if(BESTFITOBJECT->RetrieveDelphiCamPts(_slope, Tmpangle, MaxRadius, ProbePts))
				{
					ProbePath_LstEntity = gcnew PointsToGo();												//come back to back position
					ProbePath_LstEntity->Pt_X =  ReferenceCenter[0] + ProbePts[0] - ProbeOffsetPosition[0];
					ProbePath_LstEntity->Pt_Y =  ReferenceCenter[1] +  ProbePts[1] - ProbeOffsetPosition[1];
					ProbePath_LstEntity->Pt_Z =  RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] - ProbeOffsetPosition[2];
					ProbePath_LstEntity->path = true;
					PointsToGoList->Add(ProbePath_LstEntity);

					ProbePath_LstEntity = gcnew PointsToGo();																//target point... so it will hit on the way..
					ProbePath_LstEntity->Pt_X =  ReferenceCenter[0] + ProbePts[2] - ProbeOffsetPosition[0];
					ProbePath_LstEntity->Pt_Y =  ReferenceCenter[1] + ProbePts[3] - ProbeOffsetPosition[1];
					ProbePath_LstEntity->Pt_Z =  RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] - ProbeOffsetPosition[2];
					ProbePath_LstEntity->path = false;
					PointsToGoList->Add(ProbePath_LstEntity);
				}
			}
		}
		else															//for OD we choose a expected path and possible way to take touch probe points here.... vinod
		{
			for(int i = 0; i < MeasureCount; i++)
			{
				double Tmpangle = i * cir_sec_angle * M_PI / 180;													
					ProbePath_LstEntity = gcnew PointsToGo();											//next back side position
					ProbePath_LstEntity->Pt_X =  ReferenceCenter[0] + (MaxRadius-(10*(RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue)))*cos(_slope+Tmpangle);	//(MaxRadius*1.3) safe side, so from this dist. it will come toward center
					ProbePath_LstEntity->Pt_Y =  ReferenceCenter[1] + (MaxRadius-(10*(RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue)))*sin(_slope+Tmpangle);
					ProbePath_LstEntity->Pt_Z =  RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
					ProbePath_LstEntity->path = true;
					PointsToGoList->Add(ProbePath_LstEntity);

					ProbePath_LstEntity = gcnew PointsToGo();								//target point... so it will hit on the way..
					ProbePath_LstEntity->Pt_X =  ReferenceCenter[0] + (MinRadius-(2*(RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue)))*cos(_slope+Tmpangle);
					ProbePath_LstEntity->Pt_Y =  ReferenceCenter[1] + (MinRadius-(2*(RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue)))*sin(_slope+Tmpangle);
					ProbePath_LstEntity->Pt_Z =  RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
					ProbePath_LstEntity->path = false;
					PointsToGoList->Add(ProbePath_LstEntity);

					ProbePath_LstEntity = gcnew PointsToGo();											//come back to same back position.. so touch probe will never crash continuously with tool... this point is for safety.... 
					ProbePath_LstEntity->Pt_X =  ReferenceCenter[0] + (MaxRadius-(10*(RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue)))*cos(_slope+Tmpangle);
					ProbePath_LstEntity->Pt_Y =  ReferenceCenter[1] + (MaxRadius-(10*(RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue)))*sin(_slope+Tmpangle);
					ProbePath_LstEntity->Pt_Z =  RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
					ProbePath_LstEntity->path = true;
					PointsToGoList->Add(ProbePath_LstEntity);
			}
		}
		FirstStep = true;
		SendDrotoNextposition();
		return true;
	}
	catch(Exception^ ex)
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
		if (DoingHobMeasurement || !GotAxis || !HobParametersUpdated) return false;

		//set hobchecker MeasurementNo here .. so we will care for spacing of gashes measurement and rotate all the probe points with same 0 Raxis Reference for correct calculation we use this var in RWDRO (HandleProbeHitCallback fn)
		InPauseState = false;
		DoingCalibration = true;
		firsttime = true;
		Getting_BC_Axis = false;
		DoingHobMeasurement = true;
		lastPoint = NULL;
		RWrapper::PointsToGo^ ProbePath_LstEntity;
		PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();

		startPosition = gcnew array<double>(4);
		cPosition = gcnew array<double>(4);
		double rX[4]; 
		RMATH3DOBJECT->GetC_Axis_XY(RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], MAINDllOBJECT->TIS_CAxis, rX);

		for (int i = 0; i < 4; i ++)
		{	startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
			cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
			ReferenceCenter[i] = rX[i];
		}
		if (HobParameters[15] == 0) HobParameters[15] = HobParameters[3];

		Listcount = 0; FirstStep = true;
		
		double YmovementForBackPosition = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue + 0.4;
		if (FirstToothDone)
		{
			startPosition[2] = ToothMidPt[2];
			startPosition[3] = ToothMidPt[3];
		}

		if(MeasurementNo == 1)
		{	DelphiCamType = 3;
			double StratPositionX = ReferenceCenter[0] - (HobParameters[15] / 2) + HobParameters[1];
			double StratPositionY = ReferenceCenter[1] - (MAINDllOBJECT->ProbeRadius() + YmovementForBackPosition) * HobParameters[4];
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
			//set of commands to come back to home position
			ProbePath_LstEntity = gcnew PointsToGo();											
			ProbePath_LstEntity->Pt_X =  StratPositionX;	
			ProbePath_LstEntity->Pt_Y =  StratPositionY;
			ProbePath_LstEntity->Pt_Z =  startPosition[2]; //ReferenceCenter[2];
			ProbePath_LstEntity->Pt_R =  startPosition[3];
			ProbePath_LstEntity->path = true;
			PointsToGoList->Add(ProbePath_LstEntity);

			ProbePath_LstEntity = gcnew PointsToGo();								
			ProbePath_LstEntity->Pt_X =  startPosition[0];
			ProbePath_LstEntity->Pt_Y =  startPosition[1];
			ProbePath_LstEntity->Pt_Z =  startPosition[2]; //ReferenceCenter[2];
			ProbePath_LstEntity->Pt_R =  startPosition[3];
			ProbePath_LstEntity->path = true;
			PointsToGoList->Add(ProbePath_LstEntity);			
		}
		else if(MeasurementNo == 2)
		{	
			double extraDist = 5.0;
			//RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Cloud Points");
			if (IndexingPts == NULL)
			{
				RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Cloud Points");
				IndexingPts = (CloudPoints*)MAINDllOBJECT->getShapesList().selectedItem();
			}
			else
			{
				//int index = (Shape*);
				MAINDllOBJECT->selectShape(IndexingPts->getId(), false);
				RWrapper::RW_MainInterface::MYINSTANCE()->HandleShapeParameter_Click("Reset Points");
			}
			startPosition[0] = ReferenceCenter[0] - (HobParameters[15] / 2) - extraDist; 
			startPosition[1] = ReferenceCenter[1] - (MAINDllOBJECT->ProbeRadius() + YmovementForBackPosition) * HobParameters[4];

			for (int i = 0; i < 4; i ++)
			{	cPosition[i] = startPosition[i];
			}

			double PitchValue = abs(M_PI * HobParameters[12] * HobParameters[11]); // / cos(HobParameters[10] * M_PI / 180)); //((ODiameter / 2)-Tooth_depth);
			double IndexAngle = M_PI * 2 / HobParameters[6];
			double Move_inZDirection = 0.0;
			Move_inZDirection = PitchValue / HobParameters[6];
			double Rotate_AxisForHelicalLead = 0;
			if (HobParameters[13] != 0) 
			{
				double helixRad = (HobParameters[15] / 2) - 1.125 * HobParameters[12];
				double helixAngle = atan(HobParameters[13] / (M_PI * helixRad * 2));
				Move_inZDirection *= sin(helixAngle);
				Rotate_AxisForHelicalLead = Move_inZDirection * 2 * M_PI / HobParameters[13];
			}
			
			//PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
			if (FirstToothDone)
				cPosition[0] = ReferenceCenter[0] - (HobParameters[15] / 2) + 1.125 * HobParameters[12]; 
			else
				cPosition[0] = startPosition[0] + extraDist + HobParameters[2]; 
			
			if (startPosition[3] + 2 * M_PI > 2 * M_PI + M_PI / 10)
			{
				startPosition[3] -= 2 * M_PI;
				cPosition[3] = startPosition[3];
				ProbePath_LstEntity = gcnew PointsToGo();								
				ProbePath_LstEntity->Pt_X =  startPosition[0];
				ProbePath_LstEntity->Pt_Y =  startPosition[1];
				ProbePath_LstEntity->Pt_Z =  startPosition[2];
				ProbePath_LstEntity->Pt_R =  startPosition[3];
				ProbePath_LstEntity->path = true;
				PointsToGoList->Add(ProbePath_LstEntity);
			}

			for(int i = 0; i <= HobParameters[6]; i++)
			{	
				//Move in Z and rotate to next tooth. when i = 0; it will remain there
				ProbePath_LstEntity = gcnew PointsToGo();								
				ProbePath_LstEntity->Pt_X =  startPosition[0];
				ProbePath_LstEntity->Pt_Y =  cPosition[1];
				ProbePath_LstEntity->Pt_Z =  cPosition[2];
				ProbePath_LstEntity->Pt_R =  cPosition[3];
				ProbePath_LstEntity->path = true;
				PointsToGoList->Add(ProbePath_LstEntity);

				//Now lets enter the tooth. 
				ProbePath_LstEntity = gcnew PointsToGo();			//outer start position for every tooth with appropriate z,r movement							
				ProbePath_LstEntity->Pt_X =  cPosition[0];	
				ProbePath_LstEntity->Pt_Y =  cPosition[1] ;
				ProbePath_LstEntity->Pt_Z =  cPosition[2] ; //ReferenceCenter[2] + (i*Move_inZDirection);
				ProbePath_LstEntity->Pt_R =  cPosition[3]; // + (AngleBWTwoHelicalTooth*i*HobDirectionSign*M_PI/180);
				ProbePath_LstEntity->path = true;
				PointsToGoList->Add(ProbePath_LstEntity);

				//Take the point by moving in Y.
				ProbePath_LstEntity = gcnew PointsToGo();								//go for taking ppoints
				ProbePath_LstEntity->Pt_X =  cPosition[0];// + (2 * Tooth_depth);
				ProbePath_LstEntity->Pt_Y =  cPosition[1] + (YmovementForBackPosition * HobParameters[4]);
				ProbePath_LstEntity->Pt_Z =  cPosition[2];
				ProbePath_LstEntity->Pt_R =  cPosition[3];
				ProbePath_LstEntity->path = false;
				PointsToGoList->Add(ProbePath_LstEntity);

				//Come out of the hob.
				ProbePath_LstEntity = gcnew PointsToGo();								//again back to same position
				ProbePath_LstEntity->Pt_X =  startPosition[0];
				ProbePath_LstEntity->Pt_Y =  cPosition[1];
				ProbePath_LstEntity->Pt_Z =  cPosition[2];
				ProbePath_LstEntity->Pt_R =  cPosition[3];
				ProbePath_LstEntity->path = true;
				PointsToGoList->Add(ProbePath_LstEntity);

				//First Calculate where to move in Z and R. 
				cPosition[3] += IndexAngle - Rotate_AxisForHelicalLead;
				cPosition[2] += Move_inZDirection  * HobParameters[5];
			}
			//GO back to home position
			ProbePath_LstEntity = gcnew PointsToGo();								//again back to same position
			ProbePath_LstEntity->Pt_X =  startPosition[0];
			ProbePath_LstEntity->Pt_Y =  startPosition[1];
			ProbePath_LstEntity->Pt_Z =  startPosition[2];
			ProbePath_LstEntity->Pt_R =  startPosition[3];
			ProbePath_LstEntity->path = true;
			PointsToGoList->Add(ProbePath_LstEntity);
		}
		else 						//default Measurement no. 3				
		{	
			startPosition[0] = ReferenceCenter[0] - (HobParameters[15] / 2) - 10; 
			startPosition[1] = ReferenceCenter[1] - HobParameters[4] * (MAINDllOBJECT->ProbeRadius() + YmovementForBackPosition);
			
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
 
			if (HobParameters[13] != 0)
				cPosition[3] = startPosition[3] - HobParameters[5] * ZmoveforStartTooth * 2 * M_PI / HobParameters[13];

			//We to decide on helical gash hobs if we should first rotate and then move in Z or vice-versa.
			bool rotate_First = false;
			if (HobParameters[4] * HobParameters[5] < 0)
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
						cPosition[3] += HobParameters[5] *  Rotate_AxisForHelicalLead;
					else
						cPosition[2] -= (Move_inZDirection * jumpSize);
					
					ProbePath_LstEntity = gcnew PointsToGo();
					ProbePath_LstEntity->Pt_X =  cPosition[0];
					ProbePath_LstEntity->Pt_Y =  startPosition[1];
					ProbePath_LstEntity->Pt_Z =  cPosition[2];
					ProbePath_LstEntity->Pt_R =  cPosition[3];
					ProbePath_LstEntity->path = true;
					PointsToGoList->Add(ProbePath_LstEntity);
					//First step over. Now move in Z or rotate next
					if (!rotate_First)
						cPosition[3] += HobParameters[5] *  Rotate_AxisForHelicalLead;
					else
						cPosition[2] -= (Move_inZDirection * jumpSize);
					
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
					cPosition[2] -= (Move_inZDirection * jumpSize);
					ProbePath_LstEntity = gcnew PointsToGo();
					ProbePath_LstEntity->Pt_X =  cPosition[0];
					ProbePath_LstEntity->Pt_Y =  startPosition[1];
					ProbePath_LstEntity->Pt_Z =  cPosition[2];
					ProbePath_LstEntity->Pt_R =  cPosition[3];
					ProbePath_LstEntity->path = true;
					PointsToGoList->Add(ProbePath_LstEntity);
				}
			}
			//Get out of hob area
			ProbePath_LstEntity = gcnew PointsToGo();								
			ProbePath_LstEntity->Pt_X =  startPosition[0];
			ProbePath_LstEntity->Pt_Y =  startPosition[1];
			ProbePath_LstEntity->Pt_Z =  cPosition[2];
			ProbePath_LstEntity->Pt_R =  cPosition[3];
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
		startPosition = gcnew array<double>(4);
		cPosition = gcnew array<double>(4);
		CylinderDia = diameter;
		MasterLength = masterL; 
		NumberofLevels = LevelNumber;
		Doing_Y_Check = true;
		//MasterCylinderFirstStep = true;
		if (MasterCylinder == NULL)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Circle");
			MasterCylinder = (Circle*)MAINDllOBJECT->getShapesList().selectedItem();
		}
		else
		{
			MAINDllOBJECT->selectShape(MasterCylinder->getId(), false);
			MAINDllOBJECT->ResetthePoints_selectedshape();
		}
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
	ClearAll();
	RWrapper::RW_MainInterface::MYINSTANCE()->HandleShapeParameter_Click("Reset Points");
}

void RWrapper::RW_DelphiCamModule::StopProcess()
{
	RCadApp::Set_IsDelphiCam(false);		//set bool in rcadapp so we know that process is over and we have to draw a cross whenever need in any drawing 
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
				CylParam[5] = 1;
				MasterCylinder->getParameters(CylParam);
				//MessageBox::Show("Got Master Cylinder values.", "Rapid-I");
			
				//Calculate the point at Z = 0; use this as our BC centre, always. 
				double rr = CylParam[2]/CylParam[5];			
				MAINDllOBJECT->TIS_CAxis[0] = rr * CylParam[3] + CylParam[0];
				MAINDllOBJECT->TIS_CAxis[1] = rr * CylParam[4] + CylParam[1];
				MAINDllOBJECT->TIS_CAxis[2] = 0;
			
				//Copy angle and radius values to TIS_CAxis
				//Also see if direction cosine N is -1. if so, multiply each angle by -1!
				double signchanger = 1;
				if (CylParam[5] < 0) signchanger = -1;

				for (int i = 3; i < 6; i ++)
					MAINDllOBJECT->TIS_CAxis[i] = signchanger * CylParam[i];
				//Write the Diameter of the cylinder; not the radius!
				MAINDllOBJECT->TIS_CAxis[6] = 2 * CylParam[6];
				////Keep the copy of this cylinder since TIS_CAxis will be used for RunoutCorrection
				//for (int i = 0; i < 6; i ++)
				//	MAINDllOBJECT->BC_Axis[i] = MAINDllOBJECT->TIS_CAxis[i]; 
				//MessageBox::Show("Finished setting all values.", "Rapid-I");

				//--------------------Finished TIS C Axis Calculation----- Now assign the values. 
				Getting_BC_Axis = false;
				if (Listcount == PointsToGoList->Count) GotAxis = true;
				ReferenceCenter[0] = MAINDllOBJECT->TIS_CAxis[0]; 
				ReferenceCenter[1] = MAINDllOBJECT->TIS_CAxis[1]; 
				for (int i = 0; i < 6; i ++)
					BC_Values[i] = MAINDllOBJECT->TIS_CAxis[i];
				//Write Master Cylinder Values to file
				System::String^ currentTime = Microsoft::VisualBasic::DateAndTime::Now.ToShortDateString() + ", " + Microsoft::VisualBasic::DateAndTime::Now.ToString("HH:mm:ss");
				System::String^ SfilePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath;
				SfilePath = SfilePath + "\\Log\\AxisLog\\MasterCylinder.csv";
				System::IO::StreamWriter^ pointsFile = gcnew System::IO::StreamWriter(SfilePath, true);
				pointsFile->WriteLine(currentTime);
				pointsFile->WriteLine("X" + ", " + "Y " + ", " + "Z" + ", " + "L" + ", " + "M" + ", " + "N" + ", " + "Dia" + ", "+ "R");			
				for (int ii = 0; ii < 7; ii++)
					 pointsFile->Write(MAINDllOBJECT->TIS_CAxis[ii] + ", "); 
				pointsFile->WriteLine();
				pointsFile->Close();
				//Trial - Make tilt as zero. 
				//MAINDllOBJECT->TIS_CAxis[3] = 0;
				//MAINDllOBJECT->TIS_CAxis[4] = 0;
				//MAINDllOBJECT->TIS_CAxis[5] = 1;
			}
			catch(Exception^ ex)
			{ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule006", ex); }
		}
	}
	DoingHobMeasurement = false;
	MAINDllOBJECT->DeselectAll(false);
	PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
	RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
	ClearAll();
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

		double ApproachDist = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;
		double prog = (double)Listcount/(double)PointsToGoList->Count;
		DelphiCamEvent::raise(prog);
		//if (Listcount == PointsToGoList->Count - 2 && Doing_Y_Check) Doing_Y_Check = false;
		if(Listcount >= PointsToGoList->Count) // + 1)
		{	
			Listcount = -1;
			return;
		}
		else if(Listcount % 2 != 0)
		{
		   if (!DoingCalibration) MAINDllOBJECT->NotedownProbePath();
		}

		double target[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		RWrapper::PointsToGo^ ProbePath_LstEntity = gcnew PointsToGo();
		//RWrapper::PointsToGo^ NextStep_LstEntity = gcnew PointsToGo();
		bool IncrementListCt = true;
		//Check if previous command has been properly executed. Do not call next command until we are sure to have reached the previous one!
		bool GotoNextStep = true;
		double ExtraMoveForTouch = 0.0;
		if  (Listcount == 0) 
		{
			if (!FirstStep) Listcount ++;
		}
		else
		{	ProbePath_LstEntity = PointsToGoList[Listcount]; // - 1];
			Listcount ++;
			if (Listcount == PointsToGoList->Count) 
			{
				DelphiCamEvent::raise(1);
				return;
			}

			if(ProbePath_LstEntity->path)
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
						IncrementListCt = false;
						countinue_pause_delphicammodule(true);
						DelphiCamEvent::raise(-1000);
						return;
					}
				}
			}
		}

		ProbePath_LstEntity = PointsToGoList[Listcount];
		//if (GotoNextStep) 
		//	Listcount++;
		//else
		//	ExtraMoveForTouch = 10.0;
		double delta[4] = {0};
		delta[0] = abs(target[0] - ProbePath_LstEntity->Pt_X);
		delta[1] = abs(target[1] - ProbePath_LstEntity->Pt_Y);
		delta[2] = abs(target[2] - ProbePath_LstEntity->Pt_Z);
		delta[3] = abs(target[3] - ProbePath_LstEntity->Pt_R);
		
		double speedFrac = 0.75;
		for (int ii = 0; ii < 3; ii ++)
		{
			feedrate[ii] = delta[ii] * speedFrac;
			if (feedrate[ii] > 15) feedrate[ii] = 15;
			if (feedrate[ii] < 1.2) feedrate[ii] = 1.2;
			if (feedrate[ii] > 1.5 && feedrate[ii] < 2.2) feedrate[ii] = 2.2;
		}
		if (Doing_Y_Check &&  Listcount == 2) feedrate[1]= 2;
		if (StartToothStepNumber == 103 &&  Listcount == 1 && !ProbePath_LstEntity->path) feedrate[0]= 2;
		if (StartToothStepNumber == 102 && !ProbePath_LstEntity->path) feedrate[2]= 0.5;
		
		feedrate[3] = (delta[3] * 180 / M_PI) * 1.6;
		if (feedrate[3] > 10) feedrate[3] = 10;
		if (feedrate[3] < 0.6) feedrate[3] = 0.6;
		if (feedrate[3] > 1.5 && feedrate[3] < 2.2) feedrate[3] = 1.4;

		target[0] = ProbePath_LstEntity->Pt_X; target[1] = ProbePath_LstEntity->Pt_Y; target[2] = ProbePath_LstEntity->Pt_Z, target[3] = ProbePath_LstEntity->Pt_R;		
	    //probedirection = getMovementdirection(target);
		CurrentTargetType = TargetReachedCallback::DELPHICOMPONENTMODULE;
		if(!ProbePath_LstEntity->path)
		{	//if (!GotoNextStep) Listcount ++;
			probedirection = getMovementdirection(target);
			//if (!GotoNextStep) Listcount --;
			if(RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag)
			{	
				switch(probedirection)
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
				}
				DROInstance->SetProbeDirection(probedirection);
			}
			else
			{
				switch(probedirection)
				{
					case 1:
						target[0] -= ApproachDist + ExtraMoveForTouch;
						break;
					case 3:
						target[0] += ApproachDist + ExtraMoveForTouch;
						break;
					case 5:
						target[1] -= ApproachDist + ExtraMoveForTouch;
						break;
					case 7:
						target[1] += ApproachDist + ExtraMoveForTouch;
						break;
					case 9:
						target[2] -= ApproachDist + ExtraMoveForTouch;
						break;
					case 11:
						target[2] += ApproachDist + ExtraMoveForTouch;
						break;
				}
			}
		}
		if(!ProbePath_LstEntity->path)
		{
			if(!firsttime)
			{
			   //HELPEROBJECT->AddMeasurementForDelphiComponent(parenLine, parentCircle, lastPoint);				//draw measurement b/w 2 points
			}
			else
				firsttime = false;
			if (StartToothStepNumber == 101)
			{
				feedrate[3] = 0.2;
				RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = false;
			}
			else
				RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = true;
		}
		else
		{
			RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = false;

		}
		if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);

		for(int i = 0; i < 4; i++)
			LastTargetValue[i] = target[i];
	    RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], CurrentTargetType);
		if (Listcount == 0) FirstStep = false;
		
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule004", ex); }
}

int RWrapper::RW_DelphiCamModule::getMovementdirection(double* targetPos)
{
	try
	{
		double target[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		if (Listcount > 1)
		{	RWrapper::PointsToGo^ ProbePath_LstEntity = gcnew PointsToGo();
			ProbePath_LstEntity = PointsToGoList[Listcount - 1];
			target[0] = ProbePath_LstEntity->Pt_X; target[1] = ProbePath_LstEntity->Pt_Y; target[2] = ProbePath_LstEntity->Pt_Z, target[3] = ProbePath_LstEntity->Pt_R;		
		}
		double Xdist = targetPos[0] - target[0], Ydist = targetPos[1] - target[1], Zdist = targetPos[2] - target[2];//, Rdist = targetPos[3] - target[3];
		double dist = abs(Xdist);
		int direction = 1;
		if(Xdist < 0)
			direction = 3;
		if(abs(Ydist) > dist)
		{
			dist = abs(Ydist);
			if(Ydist < 0)
				direction = 7;
			else
				direction = 5;
		}
		if(abs(Zdist) > dist)
		{
			if(Zdist < 0)
				direction = 11;
			else
				direction = 9;
		}
		//if(abs(Rdist) > dist)
		//{
		//	if(Rdist < 0)
		//		direction = 15;
		//	else
		//		direction = 13;
		//}
		return direction;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_DelphiCamModule005", ex);return 1; }
}

void RWrapper::RW_DelphiCamModule::Get_Cylinder_Y_Position()
{
	if (GotAxis) return;	
	
	RWrapper::PointsToGo^ ProbePath_LstEntity;
	PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
	double extraDist = CylinderDia / 4 - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;
	DoingCalibration = true;

	Listcount = 0; FirstStep = true;

	for (int i = 0; i < 4; i ++)
	{	startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
	}	
	ManualPoint = false; 

	//Set of commmands to go Positive Y outside.
	ProbePath_LstEntity = gcnew PointsToGo();											
	ProbePath_LstEntity->Pt_X = startPosition[0] - extraDist;	
	ProbePath_LstEntity->Pt_Y =  startPosition[1] + (CylinderDia/2 + extraDist);
	ProbePath_LstEntity->Pt_Z =  startPosition[2];
	ProbePath_LstEntity->Pt_R =  startPosition[3];
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);
	//Set the current position
	cPosition[0] = ProbePath_LstEntity->Pt_X;
	cPosition[1] = ProbePath_LstEntity->Pt_Y;
	//Move right in X axis to the centre
	ProbePath_LstEntity = gcnew PointsToGo();											
	ProbePath_LstEntity->Pt_X = startPosition[0] + CylinderDia/2;	
	ProbePath_LstEntity->Pt_Y =  cPosition[1];// + CylinderDia/2  + extraDist;
	ProbePath_LstEntity->Pt_Z =  cPosition[2];
	ProbePath_LstEntity->Pt_R =  startPosition[3];
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);

	//Move in -Y direction toward the cylinder to get the Y-position, and set that in Start Position and cPosition - 
	ProbePath_LstEntity = gcnew PointsToGo();											
	ProbePath_LstEntity->Pt_X = startPosition[0] + CylinderDia/2;	
	ProbePath_LstEntity->Pt_Y =  cPosition[1] - (CylinderDia/2 + extraDist + 10);// + CylinderDia/2  + extraDist;
	ProbePath_LstEntity->Pt_Z =  cPosition[2];
	ProbePath_LstEntity->Pt_R =  startPosition[3];
	ProbePath_LstEntity->path = false;
	PointsToGoList->Add(ProbePath_LstEntity);

	//Set the Flag for Checking Y position of Cylinder to true so that we know when to start touching for taking points on cylinder...
	Doing_Y_Check = true;

	//Start RUnning all the points in CNC
	SendDrotoNextposition();
}

void RWrapper::RW_DelphiCamModule::CalculatePtsFor_BC_Axis()
{	
	if (GotAxis) return;	
	//if (Doing_Y_Check)
	//{	
	//	//We got the X-position. Let us calculate positions and take the Y position. 
	//	//X position etc will be noted inside this function!
	//	Get_Cylinder_Y_Position();
	//	return;
	//}
	RWrapper::PointsToGo^ ProbePath_LstEntity;
	PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
	double extraDist = CylinderDia / 4 - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;
	DoingCalibration = true;
	ManualPoint = false;
	//MAINDllOBJECT->ReferenceDotShape->selected(true);
	Getting_BC_Axis = true;
	//MessageBox::Show(MasterCylinder->PointsList->Pointscount().ToString(), "Rapid-I");

	Listcount = 0; FirstStep = true;
	double Dist_Z = (float) MasterLength / (float)(NumberofLevels - 1); //200 / NumberofLevels;
	bool PositiveYSide = true;
	if(MasterCylinder->PointsList->Pointscount() > 0)// && NoofShapes == TargetShapeNo)
	{
		for(PC_ITER SptItem = MasterCylinder->PointsList->getList().begin(); SptItem != MasterCylinder->PointsList->getList().end(); SptItem++)
		{	
			SinglePoint* Spt = (*SptItem).second;
			startPosition[1] = Spt->Y - CylinderDia / 2 - MAINDllOBJECT->ProbeRadius();
			break;
		}
	}
	else
		startPosition[1] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] - CylinderDia / 2 - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue - MAINDllOBJECT->ProbeRadius();
	
	RWrapper::RW_MainInterface::MYINSTANCE()->HandleShapeParameter_Click("Reset Points");

	////Set the current position
	//cPosition[0] = ProbePath_LstEntity->Pt_X;
	//cPosition[1] = ProbePath_LstEntity->Pt_Y;
	//for (int i = 0; i < 4; i ++)
	//	cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];

	int z_direction = 1;
	int numberofRows = 5;
	double fracX[5] = {1, 0.293, 0, 0.293, 1};
	double fracY[5] = {1, 0.707, 0, -0.707, -1};
	int dirX[5] = {0, 0, -1, 0, 0};
	int dirY[5] = {1, 1, 0, -1, -1};

	double diaFraction = CylinderDia / (float)(numberofRows - 1);
	for (int j = 0; j < numberofRows; j ++)
	{	
		//Let us move in X and Y to go to the current row
		//Move in Y-axis by a fraction proportional to the number of rows!
		cPosition[0] = startPosition[0] + fracX[j] * CylinderDia / 2 + dirX[j] * extraDist;
		cPosition[1] = startPosition[1] + fracY[j] * CylinderDia / 2 + dirY[j] * extraDist;
		ProbePath_LstEntity = gcnew PointsToGo();								
		ProbePath_LstEntity->Pt_X =  cPosition[0];
		ProbePath_LstEntity->Pt_Y =  cPosition[1];
		ProbePath_LstEntity->Pt_Z =  cPosition[2];
		ProbePath_LstEntity->Pt_R =  startPosition[3];
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);	

		for (int i = 0; i < NumberofLevels; i ++)
		{
			//Set of commands to touch the cylinder and take a point
			ProbePath_LstEntity = gcnew PointsToGo();								
			ProbePath_LstEntity->Pt_X =  cPosition[0] - dirX[j] * extraDist;
			ProbePath_LstEntity->Pt_Y = startPosition[1] + fracY[j] * CylinderDia/2;
			ProbePath_LstEntity->Pt_Z =  cPosition[2];
			ProbePath_LstEntity->Pt_R =  startPosition[3];
			ProbePath_LstEntity->path = false;
			PointsToGoList->Add(ProbePath_LstEntity);	
			
			if (i == NumberofLevels - 1) 
			{
				if (dirX[j] != 0) 
				{	ProbePath_LstEntity = gcnew PointsToGo();								
					ProbePath_LstEntity->Pt_X =  cPosition[0] + dirX[j] * extraDist;;
					ProbePath_LstEntity->Pt_Y =  cPosition[1];
					ProbePath_LstEntity->Pt_Z =  cPosition[2];
					ProbePath_LstEntity->Pt_R =  startPosition[3];
					ProbePath_LstEntity->path = true;
					PointsToGoList->Add(ProbePath_LstEntity);
				}
				continue;
			}
			//Go down in Z and Y 
			cPosition[2] -= Dist_Z * z_direction;
			ProbePath_LstEntity = gcnew PointsToGo();								
			ProbePath_LstEntity->Pt_X =  cPosition[0];
			ProbePath_LstEntity->Pt_Y =  cPosition[1];
			ProbePath_LstEntity->Pt_Z =  cPosition[2];
			ProbePath_LstEntity->Pt_R =  startPosition[3];
			ProbePath_LstEntity->path = true;
			PointsToGoList->Add(ProbePath_LstEntity);	
		}
		//This row is over. So we have to travel in opposite direction in Z now
		z_direction *= -1;
	}	

	//Move in X to start position
	ProbePath_LstEntity = gcnew PointsToGo();											
	ProbePath_LstEntity->Pt_X = startPosition[0];	
	ProbePath_LstEntity->Pt_Y = cPosition[1];				
	ProbePath_LstEntity->Pt_Z =  cPosition[2];
	ProbePath_LstEntity->Pt_R =  startPosition[3];
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);
		
	//Get back to start position
	ProbePath_LstEntity = gcnew PointsToGo();											
	ProbePath_LstEntity->Pt_X = startPosition[0] - extraDist;	
	ProbePath_LstEntity->Pt_Y =  startPosition[1];
	ProbePath_LstEntity->Pt_Z =  startPosition[2];
	ProbePath_LstEntity->Pt_R =  startPosition[3];
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);

	//Start RUnning all the points in CNC
	SendDrotoNextposition();
}

void RWrapper::RW_DelphiCamModule::Set_C_Axis()
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
		array<String^>^ Values;
		String^ delimStr = ",";
		array<Char>^ delimiter = delimStr->ToCharArray( );
		Values = currentLine->Split(delimiter);
		for (int i = 0; i < 7; i ++)
		{
			MAINDllOBJECT->TIS_CAxis[i] = Convert::ToDouble(Values[i]);
			//MAINDllOBJECT->BC_Axis[i] = Convert::ToDouble(Values[i]);
			BC_Values[i] = Convert::ToDouble(Values[i]);
		}
		GotAxis = true;
		Getting_BC_Axis = false;
		DoingCalibration = false;
		ReferenceCenter[0] = MAINDllOBJECT->TIS_CAxis[0]; 
		ReferenceCenter[1] = MAINDllOBJECT->TIS_CAxis[1]; 
}

void RWrapper::RW_DelphiCamModule::Set_Runout()
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
		array<String^>^ Values;
		array<String^>^ StoredValues;
		String^ delimStr = ",";
		array<Char>^ delimiter = delimStr->ToCharArray( );

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
		for (int i = 0; i < 7; i ++)
		{
			MAINDllOBJECT->TIS_CAxis[i] = Convert::ToDouble(Values[i]);
		}
		Values = currentLine3->Split(delimiter);
		for (int ii = 0; ii < 6; ii++)
		{
			Hob_RunoutParams[ii] = Convert::ToDouble(Values[ii]);
			MAINDllOBJECT->BC_Axis[ii] = Convert::ToDouble(Values[ii]);
		}

		DoingCalibration = false;
		RunoutCorrectionStage = 1000;
}

double currentZ = 0.0;

bool RWrapper::RW_DelphiCamModule::Run_RunoutCorrection(int StepNumber, int ptsCt)
{
	//This is the main function that will be called from outside for running runout correction or from the GotoNextPosition..
	// = 0 not running; = 1 running level 1; = 2 running level 2; = 100 completed and working; = -1 not done;
	bool finished = false;
	RunoutCorrectionStage = StepNumber;
	double centreval[6] = {0};
	switch(StepNumber)
	{
	case 1:
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
		CalculatePtsfor_Runout(12, currentZ);
		DoingCalibration = true; Getting_BC_Axis = true;
		//Start Running all the points in CNC
		SendDrotoNextposition();
		finished = true;
		break;

	case 2:
		//currentZ = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
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
		////CalculatePtsfor_Runout(ptsCt, currentZ);
		//CalculatePts_NoRotation(currentZ, 1);
		//DoingCalibration = true; Getting_BC_Axis = true;
		////Start Running all the points in CNC
		//SendDrotoNextposition();
		Move_To_NextHub();
		SendDrotoNextposition();
		finished = true;
		break;

	case 3: //Run at level one or level 2. it will automatically move to correct position!!
		//topC->GetShapeCenter(centreval);
		//if (botC == NULL)
		//{
		//	RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Circle");
		//	botC = (Circle*)MAINDllOBJECT->getShapesList().selectedItem();
		//}
		//else
		//{
		//	MAINDllOBJECT->selectShape(botC->getId(), false);
		//	RWrapper::RW_MainInterface::MYINSTANCE()->HandleShapeParameter_Click("Reset Points");
		//}
		//if (centreval[2] == 0) centreval[2] = currentZ;
		//currentZ = centreval[2] - (HobParameters[9] + HobParameters[17]) / 2;
		////CalculatePtsfor_Runout(ptsCt, currentZ);
		//CalculatePts_NoRotation(currentZ, -1);
		DoingCalibration = true; Getting_BC_Axis = true;
		//RWrapper::PointsToGo^ ProbePath_LstEntity;
		////Get back to home position
		//ProbePath_LstEntity = gcnew PointsToGo();								
		//ProbePath_LstEntity->Pt_X =  startPosition[0];
		//ProbePath_LstEntity->Pt_Y =  cPosition[1];
		//ProbePath_LstEntity->Pt_Z =  startPosition[2];
		//ProbePath_LstEntity->Pt_R =  startPosition[3];
		//ProbePath_LstEntity->path = true;
		//PointsToGoList->Add(ProbePath_LstEntity);	

		//ProbePath_LstEntity = gcnew PointsToGo();								
		//ProbePath_LstEntity->Pt_X =  ReferenceCenter[0] - HobParameters[3] / 2 - 10;
		//ProbePath_LstEntity->Pt_Y =  startPosition[1];
		//ProbePath_LstEntity->Pt_Z =  startPosition[2];
		//ProbePath_LstEntity->Pt_R =  startPosition[3];
		//ProbePath_LstEntity->path = true;
		//PointsToGoList->Add(ProbePath_LstEntity);	

		////Start Running all the points in CNC
		//SendDrotoNextposition();
		//DelphiCamEvent::raise(1.0);
		//finished = true;
		//break;

	case 4:
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
		CalculatePtsfor_Runout(12, currentZ);
		DoingCalibration = true; Getting_BC_Axis = true;
		//Start Running all the points in CNC
		SendDrotoNextposition();
		finished = true;
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

		double topRunout[3] = {0.0};
		double botRunout[3] = {0.0};
		try
		{
			//MessageBox::Show("started Runout Calc", "Rapid-I");

			//DoingCalibration = false;
			//for (int ii =0; ii , 3; ii ++)
			//{
			//	topRunout[ii] = 0;
			//	botRunout[ii] = 0;
			//}
			if (RWrapper::RW_DelphiCamModule::MYINSTANCE()->GetRunoutforShape((ShapeWithList*)topC_RO, 0, topRunout))
			{
				if(RWrapper::RW_DelphiCamModule::MYINSTANCE()->GetRunoutforShape((ShapeWithList*)botC_RO, 0, botRunout))
				{
					//MessageBox::Show("Completed 1", "Rapid-I");

					double cx = 0, cy = 0;
					cx = topRunout[2] * cos(topRunout[1]);
					cy = topRunout[2] * sin(topRunout[1]);
					double tx[2] = {0};
					RMATH3DOBJECT->GetC_Axis_XY(axispts[2], MAINDllOBJECT->TIS_CAxis, tx);
					//MessageBox::Show("Completed 2", "Rapid-I");

					axispts[0] = tx[0] + cx;
					axispts[1] = tx[1] + cy;

					cx = botRunout[2] * cos(botRunout[1]);
					cy = botRunout[2] * sin(botRunout[1]);
					//RMATH3DOBJECT->GetC_Axis_XY(axispts[5], MAINDllOBJECT->TIS_CAxis, tx);
					//MessageBox::Show("Completed 3", "Rapid-I");

					axispts[3] = tx[0] + cx;
					axispts[4] = tx[1] + cy;
					BESTFITOBJECT->Line_BestFit_3D(axispts, 2, &Hob_RunoutParams[0]);
					for (int i = 0; i < 6; i ++)
					{
						if (i < 3) ReferenceCenter[i] = MAINDllOBJECT->TIS_CAxis[i];
						if (i > 2) MAINDllOBJECT->TIS_CAxis[i] = Hob_RunoutParams[i];
						MAINDllOBJECT->BC_Axis[i] = Hob_RunoutParams[i]; 
					}
					finished = true;
				}
				else
				{
					MessageBox::Show("Could not get calculate runout at bottom hub. Please try again.", "Rapid-I");
				}
			}
			else
			{
				MessageBox::Show("Could not get calculate runout at top hub. Please try again.", "Rapid-I");
			}
		}
		catch(...)
		{
			MessageBox::Show("There was a problem in calculating rotary runout", "Rapid-I");
		}
					//finished = true;

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
		//pointsFile->WriteLine(); // << endl;
		for (int ii = 0; ii < 6; ii++)
			pointsFile->Write(Hob_RunoutParams[ii] + ", "); 
		pointsFile->WriteLine(); // << endl;
		for (int ii = 0; ii < 3; ii++)
			pointsFile->Write(topRunout[ii] + ", "); 
		pointsFile->WriteLine(); // << endl;
				for (int ii = 0; ii < 3; ii++)
			pointsFile->Write(botRunout[ii] + ", "); 
		pointsFile->WriteLine(); // << endl;
		
		pointsFile->Close();

		//Finished work. Call stop process to reset all flags and CNC
		StopProcess();
	}
	return finished;
}

void RWrapper::RW_DelphiCamModule::CalculatePtsfor_Runout(int ptsCount, double Z_Level)
{
	//Calculate the points coordinates for giving commands in CNC

	//If already running something, quit the command!!

	if (DoingHobMeasurement || !GotAxis || !HobParametersUpdated) return;

	RWrapper::PointsToGo^ ProbePath_LstEntity;
	PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
	double extraDist = 6.0 - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;

	Listcount = 0; FirstStep = true;
	startPosition = gcnew array<double>(4);
	cPosition = gcnew array<double>(4);
	DoingCalibration = true;

	for (int i = 0; i < 4; i ++)
	{	startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
	}	
	//X axis will always be one radius + probe retract position to the left of the BC axis centre in X
	startPosition[0] = ReferenceCenter[0] - HobParameters[14] / 2 - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue - MAINDllOBJECT->ProbeRadius();// - 1.0;
	startPosition[1] = ReferenceCenter[1];
	startPosition[2] = Z_Level;
	
	int rotateDir = 1;
	if (startPosition[3] > 0) rotateDir = -1;
	//First Step - Go to Start Position 
	//Check where in Y we are. 
	if (abs(cPosition[1] - startPosition[1]) > 2)
	{
		ProbePath_LstEntity = gcnew PointsToGo();											
		ProbePath_LstEntity->Pt_X = startPosition[0];	
		ProbePath_LstEntity->Pt_Y =  cPosition[1];
		ProbePath_LstEntity->Pt_Z =  startPosition[2];
		ProbePath_LstEntity->Pt_R =  startPosition[3];
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);
	}

	//if current Z is not at target Z level, 
	double Z_Diff = abs(cPosition[2] - Z_Level);
	if (Z_Diff < 2.0 && Z_Diff > 0.1)
	{
		//We are very close to target position. Hence move directly, only in Z. 
		ProbePath_LstEntity = gcnew PointsToGo();											
		ProbePath_LstEntity->Pt_X = startPosition[0];	
		ProbePath_LstEntity->Pt_Y =  startPosition[1];
		ProbePath_LstEntity->Pt_Z =  startPosition[2];
		ProbePath_LstEntity->Pt_R =  startPosition[3];
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
	ProbePath_LstEntity->Pt_X = ReferenceCenter[0] - HobParameters[3] / 2 - extraDist;	
	ProbePath_LstEntity->Pt_Y = startPosition[1];
	ProbePath_LstEntity->Pt_Z = Z_Level;
	ProbePath_LstEntity->Pt_R = cPosition[3];
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);
}

void RWrapper::RW_DelphiCamModule::Run_StartToothParameters(int StepNumber)
{
	//We make a cloud pt shape and calculate the Z-axis centre of tooth, and the tip OD of the tooth as well. 
	//First Step, the user will touch at approx the centre of the tooth. After rotating the hob to touch at 
	//Y-centre - Probe-Radius. then the touch probe may be outside the tooth. 

	//We start similar to Form&Pos of Cutting Face measurements 
	InPauseState = false;
	DoingCalibration = true;
	firsttime = true;
	Getting_BC_Axis = false;
	DoingHobMeasurement = true;
	lastPoint = NULL;
	StartToothStepNumber = StepNumber;

	switch (StepNumber)
	{
	case 1: // take points on top and bottom of pressure angle faces. 
		CalculatePts_StartToothCentre();
		//Start Running all the points in CNC
		SendDrotoNextposition();
		break;
	case 101: // take points on top and bottom of pressure angle faces. 
		CalculatePts_for_C_Axis_Zero();
		//Start Running all the points in CNC
		SendDrotoNextposition();
		break;

	case 102:
		//RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2); //Inactivate CNC to take the point into current shape!
		//Sleep(30);
		//RWrapper::RW_CNC::MYINSTANCE()->Activate_AbortDRO(); //Inactivate CNC to take the point into current shape!
		//Sleep(30);
		//RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1); //Inactivate CNC to take the point into current shape!
		
		if(FirstToothPts->PointsList->Pointscount() > 0)// && NoofShapes == TargetShapeNo)
		{
			for(PC_ITER SptItem = FirstToothPts->PointsList->getList().begin(); SptItem != FirstToothPts->PointsList->getList().end(); SptItem++)
			{	
				SinglePoint* Spt = (*SptItem).second;
				ToothMidPt[3] = Spt->R; //Pts[index * 2 + 1] = Spt->Z;
				break;
			}	
		}
		else
		{
			ToothMidPt[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]; //Pts[index * 2 + 1] = Spt->Z;
		}
		//RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
		CalculatePts_StartToothCentre_2Pts();
		//Start Running all the points in CNC
		SendDrotoNextposition();
		break;

	case 103:
		//ShapeWithList* CShape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		if(FirstToothPts->PointsList->Pointscount() >= 2)// && NoofShapes == TargetShapeNo)
		{
			double Pts[2] = {0};
			int index = 0;
			for(PC_ITER SptItem = FirstToothPts->PointsList->getList().begin(); SptItem != FirstToothPts->PointsList->getList().end(); SptItem++)
			{	if (index < 2) 	
				{	SinglePoint* Spt = (*SptItem).second;
					Pts[index] = Spt->Z; //Pts[index * 2 + 1] = Spt->Z;
					index ++;
				}
			}
			ToothMidPt[2] = (Pts[0] + Pts[1]) / 2;
			CalculatePts_StartTooth_OD(ToothMidPt[2]);
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
			if(CShape->PointsList->Pointscount() >= 4)// && NoofShapes == TargetShapeNo)
			{		
				double LP1[4] = {0};
				double LP2[4] = {0};
				int index = 0;
				for(PC_ITER SptItem = CShape->PointsList->getList().begin(); SptItem != CShape->PointsList->getList().end(); SptItem++)
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
					index ++;
				}
				double LParam1[2] = {0};
				double LParam2[2] = {0};

				BESTFITOBJECT->Line_BestFit_2D(LP1, 2, LParam1, false);
				BESTFITOBJECT->Line_BestFit_2D(LP2, 2, LParam2, false);
				double intersectPt[4] = {0};
				//Get intersection pt in the XZ plane
				RMATH2DOBJECT->Line_lineintersection(LP1[0], LP1[1], LP2[0], LP2[1], intersectPt);
				ToothMidPt[2] = intersectPt[1];
				CalculatePts_StartTooth_OD(ToothMidPt[2]);
				SendDrotoNextposition();
			}
		}
		break;
	case 1000:
		//Calculate the radius as the distance between centre at this Z and the tip point
		//RPoint* pt1 = (RPoint*)MAINDllOBJECT->getShapesList().selectedItem();
		double Ctr[4] = {0};
		//pt1->GetShapeCenter(Ctr);
		//ShapeWithList* CShape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
		if(FirstToothPts->PointsList->Pointscount() > 2)// && NoofShapes == TargetShapeNo)
		{
			SinglePoint* Sp = FirstToothPts->PointsList->getList()[2];
			Ctr[0] = Sp->X; Ctr[1] = Sp->Y; Ctr[2] = Sp->Z; Ctr[3] = Sp->R;
			//Ctr[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
			RMATH3DOBJECT->UnRotatePoint_Around_C_Axis(Ctr, MAINDllOBJECT->TIS_CAxis, Ctr);
			ToothMidPt[0] = Ctr[0]; // + ;
			ToothMidPt[1] = Ctr[1];
			ToothMidPt[3] = Ctr[3];
			if (Ctr[0] == 0.0 && Ctr[1] == 0.0 && Ctr[2] == 0.0)
			{
				HobParameters[15] = HobParameters[3];
			}
			else
			{
				//Got the tip coods. Now get the centre
				double rX[4] = {0};
				RMATH3DOBJECT->GetC_Axis_XY(RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], MAINDllOBJECT->TIS_CAxis, rX);
				//Calculate the distance as radius, and assign the OD 
				HobParameters[15] = (RMATH2DOBJECT->Pt2Pt_distance(rX, Ctr) - MAINDllOBJECT->ProbeRadius()) * 2;
			}
			FirstToothDone = true;
		}
		//Finished all things. Call Stop to clear settings and deselect all shapes made!
		StopProcess();
	}

}

void RWrapper::RW_DelphiCamModule::CalculatePts_StartToothCentre()
{
	RWrapper::PointsToGo^ ProbePath_LstEntity;
	PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
	DoingCalibration = true;	
	Listcount = 0;
	startPosition = gcnew array<double>(4);
	cPosition = gcnew array<double>(4);
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

	startPosition = gcnew array<double>(4);
	cPosition = gcnew array<double>(4);
	Listcount = 0;
	//double rX[4]; 
	//RMATH3DOBJECT->GetC_Axis_XY(startPosition[2], MAINDllOBJECT->TIS_CAxis, rX);
	double MoveByForTouch = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue + 1;

	for (int i = 0; i < 4; i ++)
	{	startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
	}
	startPosition[0] = ReferenceCenter[0] - (HobParameters[3] / 2) - 20;
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
	ProbePath_LstEntity->Pt_X =  startPosition[0];	
	ProbePath_LstEntity->Pt_Y =  startPosition[1] ;
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
	startPosition = gcnew array<double>(4);
	cPosition = gcnew array<double>(4);
	//double rX[4]; 
	//RMATH3DOBJECT->GetC_Axis_XY(startPosition[2], MAINDllOBJECT->TIS_CAxis, rX);
	double MoveByForTouch = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;

	for (int i = 0; i < 4; i ++)
	{	startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
	}
	startPosition[0] = MAINDllOBJECT->TIS_CAxis[0] - (HobParameters[3] / 2) - 10; //+ HobParameters[1];
	startPosition[1] -= HobParameters[4] * (2 * (MoveByForTouch + MAINDllOBJECT->ProbeRadius()) + 2);
	startPosition[3] = ToothMidPt[3];
	cPosition[3] = ToothMidPt[3];

	double PitchValue = abs(M_PI * HobParameters[12]); //* HobParameters[11]); 
	//Calculate estimated centre of tooth from given values in X
	cPosition[0] =  MAINDllOBJECT->TIS_CAxis[0] - (HobParameters[3] / 2) + 2.25 * HobParameters[12] / 2;
	//We move out by quarter of tooth depth in X and then come down in Z according to pressure angle and then take a point. 
	cPosition[0] -= 0.5625 * HobParameters[12];
	//Calculate corrections if gash is helical
	double Move_inZDirection = 0.5625 * HobParameters[12] * tan(HobParameters[16]);
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

	//Move up in Z axis by quarter of a pitch + touching distance
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

		//We have to rotate for helical Gashes by this amount - 
		cPosition[3] += Rotate_AxisForHelicalLead * HobParameters[4];
	
	
	if (HobParameters[13] != 0)
	{
		//First Step, we move away from the rake in Y and move out of tooth in X
		ProbePath_LstEntity = gcnew PointsToGo();										
		ProbePath_LstEntity->Pt_X =  cPosition[0];	
		ProbePath_LstEntity->Pt_Y =  startPosition[1]; //RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] - HobParameters[4] * MAINDllOBJECT->ProbeRadius();
		if (rotate_First) 
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
		if (rotate_First) 
		{
			ProbePath_LstEntity->Pt_Z =  cPosition[2]; 
			ProbePath_LstEntity->Pt_R =  startPosition[3]; 
			ProbePath_LstEntity->path = true;
		}
		else
		{
			ProbePath_LstEntity->Pt_Z =  startPosition[2]; 
			ProbePath_LstEntity->Pt_R =  cPosition[3]; 
			ProbePath_LstEntity->path = true;
		}
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
		cPosition[3] -= Rotate_AxisForHelicalLead * HobParameters[4];
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


//void RWrapper::RW_DelphiCamModule::CalculatePts_StartToothCentre_2Pts_old()
//{
//	RWrapper::PointsToGo^ ProbePath_LstEntity;
//	PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
//	DoingCalibration = true;
//	Listcount = 0;
//	startPosition = gcnew array<double>(4);
//	cPosition = gcnew array<double>(4);
//	//double rX[4]; 
//	//RMATH3DOBJECT->GetC_Axis_XY(startPosition[2], MAINDllOBJECT->TIS_CAxis, rX);
//	double MoveByForTouch = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;
//
//	for (int i = 0; i < 4; i ++)
//	{	startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
//		cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
//	}
//	startPosition[0] = ReferenceCenter[0] - (HobParameters[3] / 2) - 20; //+ HobParameters[1];
//	startPosition[1] = ReferenceCenter[1] - HobParameters[4] * (MoveByForTouch + MAINDllOBJECT->ProbeRadius());
//	startPosition[3] = ToothMidPt[3];
//	cPosition[3] = ToothMidPt[3];
//
//	double PitchValue = abs(M_PI * HobParameters[12]); //* HobParameters[11]); 
//	//Calculate estimated centre of tooth from given values in X
//	cPosition[0] =  ReferenceCenter[0] - (HobParameters[3] / 2) + 1.125 * HobParameters[12];
//	//Move to inside the tooth by about 0.5 mm so that we can touch inside top of tooth. 
//	cPosition[1] = ReferenceCenter[1] + HobParameters[4] * (MAINDllOBJECT->ProbeRadius() + 0.2);
//	//We move out by quarter of tooth depth in X and then come down in Z according to pressure angle and then take a point. 
//	cPosition[0] -= 0.5625 * HobParameters[12];
//	double Move_inZDirection = 0.5625 * HobParameters[12] * tan(HobParameters[16]);
//	double Rotate_AxisForHelicalLead = 0;
//	if (HobParameters[13] != 0) 
//	{
//		double helixRad = (HobParameters[3] / 2) - 1.125 * HobParameters[12];
//		double helixAngle = atan(HobParameters[13] / (M_PI * helixRad * 2));
//		PitchValue *= sin(helixAngle);
//		Move_inZDirection *= sin(helixAngle);
//		Rotate_AxisForHelicalLead = Move_inZDirection * 2 * M_PI / HobParameters[13];
//	}
//	double ed = MAINDllOBJECT->ProbeRadius() / 2;
//	//Move up in Z axis by quarter of a pitch + touching distance
//	cPosition[2] += Move_inZDirection + MAINDllOBJECT->ProbeRadius() + MoveByForTouch + ed;
//	//cPosition[2] -= Move_inZDirection;
//	//We have to rotate for helical Gashes by this amount - 
//	cPosition[3] += Rotate_AxisForHelicalLead * HobParameters[4];
//
//	if (FirstToothPts == NULL)
//	{
//		RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Cloud Points");
//		FirstToothPts = (CloudPoints*)MAINDllOBJECT->getShapesList().selectedItem();
//	}
//	else
//	{
//		//int index = (Shape*);
//		MAINDllOBJECT->selectShape(FirstToothPts->getId(), false);
//		RWrapper::RW_MainInterface::MYINSTANCE()->HandleShapeParameter_Click("Reset Points");
//	}
//
//
//	//First ensure we are outside the tooth
//	ProbePath_LstEntity = gcnew PointsToGo();										
//	ProbePath_LstEntity->Pt_X =  startPosition[0];	
//	ProbePath_LstEntity->Pt_Y =  startPosition[1]; //RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] - HobParameters[4] * MAINDllOBJECT->ProbeRadius();
//	ProbePath_LstEntity->Pt_Z =  startPosition[2]; 
//	ProbePath_LstEntity->Pt_R =  startPosition[3]; 
//	ProbePath_LstEntity->path = true;
//	PointsToGoList->Add(ProbePath_LstEntity);
//
//	//We first go up in Zr
//	ProbePath_LstEntity = gcnew PointsToGo();										
//	ProbePath_LstEntity->Pt_X =  startPosition[0];	
//	ProbePath_LstEntity->Pt_Y =  startPosition[1];
//	ProbePath_LstEntity->Pt_Z =  startPosition[2] + PitchValue / 2;//cPosition[2] + MoveByForTouch; 
//	ProbePath_LstEntity->Pt_R =  cPosition[3]; 
//	ProbePath_LstEntity->path = true;
//	PointsToGoList->Add(ProbePath_LstEntity);
//
//	//We first go to the top of the tooth at the centre; inside to touch and take one point.
//	ProbePath_LstEntity = gcnew PointsToGo();										
//	ProbePath_LstEntity->Pt_X =  cPosition[0];	
//	ProbePath_LstEntity->Pt_Y =  cPosition[1] ;
//	ProbePath_LstEntity->Pt_Z =  cPosition[2] ;
//	ProbePath_LstEntity->Pt_R =  cPosition[3]; 
//	ProbePath_LstEntity->path = true;
//	PointsToGoList->Add(ProbePath_LstEntity);
//
//	//We move down in Z to take a point. 
//	ProbePath_LstEntity = gcnew PointsToGo();					
//	ProbePath_LstEntity->Pt_X =  cPosition[0];	
//	ProbePath_LstEntity->Pt_Y =  cPosition[1] ;
//	ProbePath_LstEntity->Pt_Z =  cPosition[2] - MoveByForTouch; 
//	ProbePath_LstEntity->Pt_R =  cPosition[3]; 
//	ProbePath_LstEntity->path = false;
//	PointsToGoList->Add(ProbePath_LstEntity);
//
//	//Come out in X and Y
//	ProbePath_LstEntity = gcnew PointsToGo();										
//	ProbePath_LstEntity->Pt_X =  startPosition[0];	
//	ProbePath_LstEntity->Pt_Y =  startPosition[1];
//	ProbePath_LstEntity->Pt_Z =  cPosition[2];
//	ProbePath_LstEntity->Pt_R =  cPosition[3];
//	ProbePath_LstEntity->path = true;
//	PointsToGoList->Add(ProbePath_LstEntity);
//
//	//Go Down in Z
//	ProbePath_LstEntity = gcnew PointsToGo();										
//	ProbePath_LstEntity->Pt_X =  startPosition[0];	
//	ProbePath_LstEntity->Pt_Y =  startPosition[1];
//	ProbePath_LstEntity->Pt_Z =  cPosition[2] - PitchValue;
//	ProbePath_LstEntity->Pt_R =  cPosition[3];
//	ProbePath_LstEntity->path = true;
//	PointsToGoList->Add(ProbePath_LstEntity);
//
//	//Move to bottom face of tooth at this X
//	Move_inZDirection *= 2; //PitchValue / 4 + 2 * MoveByForTouch;
//	if (HobParameters[13] != 0) 
//		Rotate_AxisForHelicalLead = Move_inZDirection * 2 * M_PI / HobParameters[13];
//	cPosition[2] -= Move_inZDirection + 2 * (MoveByForTouch + MAINDllOBJECT->ProbeRadius() + ed);
//	cPosition[3] -= Rotate_AxisForHelicalLead * HobParameters[4];
//	ProbePath_LstEntity = gcnew PointsToGo();			//outer start position for every tooth with appropriate z,r movement							
//	ProbePath_LstEntity->Pt_X =  cPosition[0];	
//	ProbePath_LstEntity->Pt_Y =  cPosition[1];// - (HobParameters[4] * MoveByForTouch);
//	ProbePath_LstEntity->Pt_Z =  cPosition[2]; //ReferenceCenter[2] + (i*Move_inZDirection);
//	ProbePath_LstEntity->Pt_R =  cPosition[3]; // + (AngleBWTwoHelicalTooth*i*HobDirectionSign*M_PI/180);
//	ProbePath_LstEntity->path = true;
//	PointsToGoList->Add(ProbePath_LstEntity);
//
//	//Move up and take a point
//	ProbePath_LstEntity = gcnew PointsToGo();			//outer start position for every tooth with appropriate z,r movement							
//	ProbePath_LstEntity->Pt_X =  cPosition[0];	
//	ProbePath_LstEntity->Pt_Y =  cPosition[1] ;
//	ProbePath_LstEntity->Pt_Z =  cPosition[2] + MoveByForTouch; //ReferenceCenter[2] + (i*Move_inZDirection);
//	ProbePath_LstEntity->Pt_R =  cPosition[3]; // + (AngleBWTwoHelicalTooth*i*HobDirectionSign*M_PI/180);
//	ProbePath_LstEntity->path = false;
//	PointsToGoList->Add(ProbePath_LstEntity);
//
//	Move_inZDirection /= 2; //PitchValue / 4 + 2 * MoveByForTouch;
//	if (HobParameters[13] != 0) 
//		Rotate_AxisForHelicalLead = Move_inZDirection * 2 * M_PI / HobParameters[13];
//	cPosition[3] += Rotate_AxisForHelicalLead * HobParameters[4];
//	//We move out in X and Y to start position without moving in Z
//	ProbePath_LstEntity = gcnew PointsToGo();			//outer start position for every tooth with appropriate z,r movement							
//	ProbePath_LstEntity->Pt_X =  startPosition[0] - 6;	
//	ProbePath_LstEntity->Pt_Y =  startPosition[1];
//	ProbePath_LstEntity->Pt_Z =  cPosition[2]; //ReferenceCenter[2] + (i*Move_inZDirection);
//	ProbePath_LstEntity->Pt_R =  cPosition[3]; // + (AngleBWTwoHelicalTooth*i*HobDirectionSign*M_PI/180);
//	ProbePath_LstEntity->path = true;
//	PointsToGoList->Add(ProbePath_LstEntity);
//}

void RWrapper::RW_DelphiCamModule::CalculatePts_for_C_Axis_Zero()
{
	RWrapper::PointsToGo^ ProbePath_LstEntity;
	PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
	DoingCalibration = true;
	Listcount = 0;
	startPosition = gcnew array<double>(4);
	cPosition = gcnew array<double>(4);
	double rX[4]; 
	RMATH3DOBJECT->GetC_Axis_XY(startPosition[2], MAINDllOBJECT->TIS_CAxis, rX);

	double MoveByForTouch = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;

	for (int i = 0; i < 4; i ++)
	{	
		startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
	}
	startPosition[0] = rX[0] - (HobParameters[3] / 2) - 20; //+ HobParameters[1];
	startPosition[1] = rX[1] - HobParameters[4] * (MoveByForTouch + MAINDllOBJECT->ProbeRadius());

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

		//17 Jul 14. There are repeatability and consistency problems because Zero position is not automated. So inserting that here
	//First Up, we enter the tooth near the tip
	double initX = startPosition[0] + MAINDllOBJECT->ProbeRadius() + 20 + HobParameters[1];
	double initY = rX[1] - HobParameters[4] *  MAINDllOBJECT->ProbeRadius();
	ProbePath_LstEntity = gcnew PointsToGo();										
	ProbePath_LstEntity->Pt_X =  initX;	
	ProbePath_LstEntity->Pt_Y =  initY;
	ProbePath_LstEntity->Pt_Z =  startPosition[2]; 
	ProbePath_LstEntity->Pt_R =  startPosition[3]; 
	ProbePath_LstEntity->path = true;
	PointsToGoList->Add(ProbePath_LstEntity);

	//Now we rotate in C until we get a touch. 
	ProbePath_LstEntity = gcnew PointsToGo();										
	ProbePath_LstEntity->Pt_X =  initX;	
	ProbePath_LstEntity->Pt_Y =  initY;
	ProbePath_LstEntity->Pt_Z =  startPosition[2]; 
	ProbePath_LstEntity->Pt_R =  startPosition[3] + HobParameters[4] * 2 * M_PI / HobParameters[6]; 
	ProbePath_LstEntity->path = false;
	PointsToGoList->Add(ProbePath_LstEntity);
}

void RWrapper::RW_DelphiCamModule::Reset_RunoutValues()
{
	for (int i = 0; i < 7; i ++)
		MAINDllOBJECT->TIS_CAxis[i] = BC_Values[i];
}

bool RWrapper::RW_DelphiCamModule::GetRunoutforShape(ShapeWithList* CShape, int Runout_Axis_Index, double* answer)
{
	bool finished = false;
	int PtsCt = CShape->PointsList->Pointscount();
	//MessageBox::Show(Convert::ToString(PtsCt), "Rapid-I");
	double* runoutPts = (double*)malloc(sizeof(double) * PtsCt * 2);
	int ii = 0;
	//double* OriPt = (double*)malloc(sizeof(double) * PtsCt * 4);
	double *RotatedPt = (double*)malloc(sizeof(double) * PtsCt * 4);
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
				pointsFile->WriteLine(Spt->X + ", " + Spt->Y + ", " + Spt->Z + ", " + Spt->R);
				ii += 2;
			}
			double RunoutParams[3] = {0};
			if (BESTFITOBJECT->Sinusoid_BestFit(runoutPts, PtsCt, RunoutParams))
			{
				for (int i = 0; i < 3; i ++)
					answer[i] = RunoutParams[i];
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

	free (RotatedPt); free(runoutPts);
	return finished;
}

void RWrapper::RW_DelphiCamModule::CalculatePts_NoRotation(double Z_Level, int touchDir)
{
	if (DoingHobMeasurement || !GotAxis || !HobParametersUpdated) return;

	RWrapper::PointsToGo^ ProbePath_LstEntity;
	PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();

	Listcount = 0; FirstStep = true;
	startPosition = gcnew array<double>(4);
	cPosition = gcnew array<double>(4);
	DoingCalibration = true;
	double extraDist = HobParameters[14] / 4 - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;

	for (int i = 0; i < 4; i ++)
	{	startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
	}	
	int pos_Dir = 1;
	if (startPosition[1] < ReferenceCenter[1]) pos_Dir = -1;
	//X axis will always be one radius + probe retract position to the left of the BC axis centre in X
	startPosition[0] = ReferenceCenter[0] - HobParameters[14] / 2 - RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue - MAINDllOBJECT->ProbeRadius();// - 1.0;
	if (startPosition[3] < 0)
	{
		if (abs(startPosition[3]) > M_PI)
			startPosition[3] = - 2 * M_PI;
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

	if (abs(startPosition[1] - ReferenceCenter[1]) > HobParameters[14] / 2)
	{	
		cPosition[1] = ReferenceCenter[1] + pos_Dir * (HobParameters[3] / 2 + extraDist);
		ProbePath_LstEntity = gcnew PointsToGo();											
		ProbePath_LstEntity->Pt_X = cPosition[0];	
		ProbePath_LstEntity->Pt_Y =  cPosition[1];
		ProbePath_LstEntity->Pt_Z =  startPosition[2];
		ProbePath_LstEntity->Pt_R =  startPosition[3];
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);
	}
	else
	{
		startPosition[1] = ReferenceCenter[1];
	}
	
	startPosition[2] = Z_Level; 
	
	int rotateDir = 1;
	if (startPosition[3] > 0) rotateDir = -1;

	int numberofRows = 5;
	double fracX[5] = {1, 0.293, 0, 0.293, 1};
	double fracY[5] = {1, 0.707, 0, -0.707, -1};
	int dirX[5] = {0, 0, -1, 0, 0};
	int dirY[5] = {1, 1, 0, -1, -1};

		//First Step - Go to Start Position 
	//if current Z is not at target Z level, 
	double Z_Diff = abs(cPosition[2] - Z_Level);
	if (Z_Diff < 2.0 && Z_Diff > 0.1)
	{
		//We are very close to target position. Hence move directly, only in Z. 
		ProbePath_LstEntity = gcnew PointsToGo();											
		ProbePath_LstEntity->Pt_X = cPosition[0];	
		ProbePath_LstEntity->Pt_Y =  cPosition[1];
		ProbePath_LstEntity->Pt_Z =  startPosition[2];
		ProbePath_LstEntity->Pt_R =  startPosition[3];
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);
	}
	else if (Z_Diff > 2.0)
	{
		//Now move in Z at current XY position
		ProbePath_LstEntity = gcnew PointsToGo();											
		ProbePath_LstEntity->Pt_X = cPosition[0]; //ReferenceCenter[0] - HobParameters[3] / 2 - extraDist;	
		ProbePath_LstEntity->Pt_Y = cPosition[1];
		ProbePath_LstEntity->Pt_Z = Z_Level;
		ProbePath_LstEntity->Pt_R = cPosition[3];
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);
		
		//Now move in Y to correct startposition
		ProbePath_LstEntity = gcnew PointsToGo();											
		ProbePath_LstEntity->Pt_X = cPosition[0];	
		ProbePath_LstEntity->Pt_Y = startPosition[1];
		ProbePath_LstEntity->Pt_Z = Z_Level;
		ProbePath_LstEntity->Pt_R = cPosition[3];
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);
	}
	
	if (abs(startPosition[1] - ReferenceCenter[1]) < HobParameters[14] / 2)
	{	
		//Set of commmands to go Positive Y outside.
		ProbePath_LstEntity = gcnew PointsToGo();											
		ProbePath_LstEntity->Pt_X = startPosition[0] - extraDist;	
		ProbePath_LstEntity->Pt_Y =  startPosition[1] + (HobParameters[14] / 2 + extraDist);
		ProbePath_LstEntity->Pt_Z =  startPosition[2];
		ProbePath_LstEntity->Pt_R =  startPosition[3];
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);
		//Set the current position
		cPosition[0] = ProbePath_LstEntity->Pt_X;
		cPosition[1] = ProbePath_LstEntity->Pt_Y;
		//Move right in X axis to the centre
		ProbePath_LstEntity = gcnew PointsToGo();											
		ProbePath_LstEntity->Pt_X = startPosition[0] + HobParameters[14] / 2;	
		ProbePath_LstEntity->Pt_Y =  cPosition[1];
		ProbePath_LstEntity->Pt_Z =  startPosition[2];
		ProbePath_LstEntity->Pt_R =  startPosition[3];
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);
	}

	startPosition[1] = ReferenceCenter[1];
	double diaFraction = CylinderDia / (float)(numberofRows - 1);

	for (int j = 0; j < numberofRows; j ++)
	{	
		//Let us move in X and Y to go to the current row
		//Move in Y-axis by a fraction proportional to the number of rows!
		cPosition[0] = startPosition[0] + fracX[j] * HobParameters[14] / 2 + dirX[j] * extraDist;
		cPosition[1] = startPosition[1] + touchDir * (fracY[j] * HobParameters[14] / 2 + dirY[j] * extraDist);
		ProbePath_LstEntity = gcnew PointsToGo();								
		ProbePath_LstEntity->Pt_X =  cPosition[0];
		ProbePath_LstEntity->Pt_Y =  cPosition[1];
		ProbePath_LstEntity->Pt_Z =  startPosition[2];
		ProbePath_LstEntity->Pt_R =  startPosition[3];
		ProbePath_LstEntity->path = true;
		PointsToGoList->Add(ProbePath_LstEntity);	

		//Set of commands to touch the cylinder and take a point
		ProbePath_LstEntity = gcnew PointsToGo();								
		ProbePath_LstEntity->Pt_X =  cPosition[0] - dirX[j] * extraDist;
		ProbePath_LstEntity->Pt_Y = startPosition[1] + touchDir * (fracY[j] * HobParameters[14] / 2); // + MAINDllOBJECT->ProbeRadius());
		ProbePath_LstEntity->Pt_Z =  startPosition[2];
		ProbePath_LstEntity->Pt_R =  startPosition[3];
		ProbePath_LstEntity->path = false;
		PointsToGoList->Add(ProbePath_LstEntity);

		if (dirX[j] != 0) 
		{	ProbePath_LstEntity = gcnew PointsToGo();								
			ProbePath_LstEntity->Pt_X =  cPosition[0] + dirX[j] * extraDist;;
			ProbePath_LstEntity->Pt_Y =  cPosition[1];
			ProbePath_LstEntity->Pt_Z =  startPosition[2];
			ProbePath_LstEntity->Pt_R =  startPosition[3];
			ProbePath_LstEntity->path = true;
			PointsToGoList->Add(ProbePath_LstEntity);
		}
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
	RMATH3DOBJECT->GetC_Axis_XY(Z_Level, MAINDllOBJECT->TIS_CAxis, axisPt);
	//RMATH3DOBJECT->GetC_Axis_XY(Z_Level, &Hob_RunoutParams[0], RunoutPt);
	return RMATH2DOBJECT->Pt2Pt_distance(axisPt, RunoutPt);
}

void RWrapper::RW_DelphiCamModule::Move_To_NextHub()
{
	//Initialise variables etc
	RWrapper::PointsToGo^ ProbePath_LstEntity;
	PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::PointsToGo^>();
	DoingCalibration = true;
	Listcount = 0;
	startPosition = gcnew array<double>(4);
	cPosition = gcnew array<double>(4);
	//double rX[4]; 
	//RMATH3DOBJECT->GetC_Axis_XY(startPosition[2], MAINDllOBJECT->TIS_CAxis, rX);
	double MoveByForTouch = RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue;

	for (int i = 0; i < 4; i ++)
	{	
		startPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
	}
	startPosition[0] = MAINDllOBJECT->TIS_CAxis[0] - (HobParameters[3] / 2) - 10; //+ HobParameters[1];

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

//#include "..\MAINDll\Helper\Helper.h"
//#include "..\..\MainDLL\Engine\RCadApp.h"
#include "RW_TIS_Control.h"
#include "..\EdgeDetectionClassSingleChannel\Engine\EdgeDetectionClassSingleChannel.h"
#include "..\MAINDll\Helper\Helper.h"

//#include "RW_DRO.h"
//#include "RW_CNC.h"
//#include <math.h>
#include "RW_MainInterface.h"

#pragma warning( disable : 4267)  


namespace RWrapper
{
	TIS_Control::TIS_Control()
	{
		this->currentTIS = this;
		this->RotaryStartPosition = gcnew cli::array<double>(4);
		this->DatumPoint = gcnew cli::array<double>(4);
		this->CommandsList = gcnew System::Collections::Generic::List<TISCommand^>();
		this->ShankAlignColl = gcnew System::Collections::Generic::List<ShankAlignStep^>();
		this->TIS_Axis = gcnew cli::array<double>(6);
		this->CurrentStepIndex = -1;
		this->ShankOrientationPts = (double*)malloc(4000 * sizeof(double));
		this->ShankCorrection = (double*)malloc(6 * sizeof(double));
	}

	TIS_Control::~TIS_Control()
	{
	}

	TIS_Control^ TIS_Control::MYINSTANCE()
	{
		return currentTIS;
	}

	void TIS_Control::Calculate_PtsAround_CAxis()
	{
	}
	

	bool TIS_Control::StartMeasurement(System::Collections::Generic::List<double>^ Parameters)
	{
		bool successfullystarted = false;

		//Initialise the starting position for this scan
		this->RotaryStartPosition = gcnew cli::array < double > {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2],
			RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		//CLear Scanned pts collection so that we do not mix up data from previous scan!
		ScannedPtsCollection = new std::list<Vector>();
		CurrentStepIndex = 0;

		CyclesCount = 0;

		//Generate all the commands required for this step
		if (this->GenerateCommandsList(Parameters) > 0)
		{
			CurrentStepIndex = 0;
			RunCommandsFromList();
		}


		try
		{
			switch (CurrentTool)
			{
				case RWrapper::TISMeasureTool::GetAxis:
				{
					//MAINDllOBJECT->SetImageInEdgeDetection(1, true, 1);
					//double refdotParams[3] = { 0,0,0 };
					////Analyse for reference dot in image
					//int noofpoints = EDSCOBJECT->Analyze_RotatingDot(true, MAINDllOBJECT->RefDot_Diameter(), refdotParams);

					break;
				}
				case RWrapper::TISMeasureTool::ShankAlignment:
					//We setup the shapes and variables required for doing shank alignment. 
					//We need a collection of 3D shapes to calculate a 3D line for shank alignment. 



					break;
				case RWrapper::TISMeasureTool::ShankDia:
					break;
				case RWrapper::TISMeasureTool::CuttingDia:
					break;
				case RWrapper::TISMeasureTool::FaceRunout:
					break;
				case RWrapper::TISMeasureTool::PointAngle:
					break;
				case RWrapper::TISMeasureTool::StepLength:
					break;
				case RWrapper::TISMeasureTool::BallNose:
					break;
				case RWrapper::TISMeasureTool::MaxLine:
					break;
				case RWrapper::TISMeasureTool::MaxArc:
					break;
				case RWrapper::TISMeasureTool::Helix:
					break;
				case RWrapper::TISMeasureTool::CoreDia:
					break;
				case RWrapper::TISMeasureTool::CornerDia:
					break;
				case RWrapper::TISMeasureTool::BackTaper:
					break;
				default:
					break;
			}
			successfullystarted = true;
		}
		catch (System::Exception^ ex)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRTIS0009", ex);
			successfullystarted = false;
		}
		return successfullystarted;
	}

	int TIS_Control::GenerateCommandsList(System::Collections::Generic::List<double>^ Parameters)
	{
		int NumberofCommands = 0;
		CurrentScanDoneCount = 0;
		currentTIS->CommandsList->Clear();
		try
		{
			//Generate Commands according to Selected tool type - 
			switch (CurrentTool)
			{
			case RWrapper::TISMeasureTool::GetAxis:
			{
				//RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Circle");
				//MAINDllOBJECT->selectShape(Convert::ToInt32(RWrapper::RW_ShapeParameter::MYINSTANCE()->ShapeParameter_ArrayList[0]), false);
				//this->CurrentShape = (ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem();
				cli::array<double>^ tPos = gcnew cli::array < double > {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2],
					RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
				for (int i = 0; i < Parameters[0]; i++)
				{
					TISCommand^ newComm = gcnew TISCommand();
					double rotateBy = 2 * M_PI * ((i + 1) % 2); //Math::Pow(-1, i + 1);
					//if (RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] > 0) rotateBy *= ;
					newComm->TargetPosition = gcnew cli::array < double > {tPos[0], tPos[1], tPos[2], tPos[3] + rotateBy};
					newComm->FeedRate = gcnew cli::array < double > {0, 0, 0, 4}; // TIS_Control::MYINSTANCE()->RotaryTravelSpeed};
					newComm->ActionType = TISActionType::RotaryScan;
					currentTIS->CommandsList->Add(newComm);
					NumberofCommands++;
				}
				break;
			}
			case RWrapper::TISMeasureTool::ShankAlignment:
			{
				//We assume we are in the right side of shank to measure shank dia
				int BorderGap = 20;
				//if (Parameters[0] != 0)

				double ShankDia = Parameters[1]; 
				double rotateBy = 2 * M_PI;
				//RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0] - MAINDllOBJECT->TIS_CAxis[0]

				//Step 0 check that camera is in horizontal position
				if (this->A_Axis_isVertical)
					DROInstance->Swivel_A_Axis();
				for (int i = 0; i < 3; i++)
					this->TIS_Axis[i] = MAINDllOBJECT->TIS_CAxis[i] + MAINDllOBJECT->H_CamPosition[i];
				//this->TIS_Axis[2] = this->DatumPoint[2];

				//Step 1 - take FG at the right side. 
				TISCommand^ newComm = gcnew TISCommand();
				newComm->TargetPosition = gcnew cli::array < double > {	MAINDllOBJECT->TIS_CAxis[0] + MAINDllOBJECT->H_CamPosition[0] + ShankDia / 2,
																		MAINDllOBJECT->TIS_CAxis[1] + MAINDllOBJECT->H_CamPosition[1],
																		RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], // DatumPoint[2] + Parameters[1],
																		RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
				newComm->FeedRate = gcnew cli::array < double > {5, 5, 5, 5};
				newComm->ActionType = TISActionType::StaticFrameGrab;
				newComm->ActionParameters = gcnew cli::array<double> {BorderGap, BorderGap, MAINDllOBJECT->currCamWidth - BorderGap, MAINDllOBJECT->currCamHeight - BorderGap };
				newComm->ShapeType = "Line";
				newComm->ShapeFromStep = -1;
				newComm->FG_Type = "FrameGrab Rectangle";
				currentTIS->CommandsList->Add(newComm);

				//Step 2 - take FG on the left side. 
				newComm = gcnew TISCommand();
				newComm->TargetPosition = gcnew cli::array < double > {	MAINDllOBJECT->TIS_CAxis[0] + MAINDllOBJECT->H_CamPosition[0] - ShankDia / 2, 
																		MAINDllOBJECT->TIS_CAxis[1] + MAINDllOBJECT->H_CamPosition[1],
																		RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], // DatumPoint[2] + Parameters[1],
																		RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
				newComm->FeedRate = gcnew cli::array < double > {5, 5, 5, 5};
				newComm->ActionType = TISActionType::StaticFrameGrab;
				BorderGap = 20;
				newComm->ActionParameters = gcnew cli::array<double> {BorderGap, BorderGap, MAINDllOBJECT->currCamWidth - BorderGap, MAINDllOBJECT->currCamHeight - BorderGap };
				newComm->ShapeType = "Line";
				newComm->ShapeFromStep = -1;
				newComm->FG_Type = "FrameGrab Rectangle";
				currentTIS->CommandsList->Add(newComm);

				//Step 3 - Start Rotary Scan 
				newComm = gcnew TISCommand();
				if (RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] > 0) rotateBy *= -1;
				newComm->TargetPosition = gcnew cli::array < double > {	MAINDllOBJECT->TIS_CAxis[0] + MAINDllOBJECT->H_CamPosition[0] - ShankDia / 2, 
																		MAINDllOBJECT->TIS_CAxis[1] + MAINDllOBJECT->H_CamPosition[1],
																		RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], // DatumPoint[2] + Parameters[1],
																		RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] + rotateBy};
				newComm->FeedRate = gcnew cli::array < double > {0, 0, 0, 4};
				newComm->ActionType = TISActionType::RotaryScan;
				newComm->ActionParameters = gcnew cli::array<double> {BorderGap, BorderGap, MAINDllOBJECT->currCamWidth - BorderGap, MAINDllOBJECT->currCamHeight - BorderGap };
				newComm->ShapeType = "Line3D";
				newComm->ShapeFromStep = -1;
				newComm->FG_Type = "FrameGrab Rectangle";
				//Instantiate the array of double for this
				newComm->StepSize = Parameters[Parameters->Count - 1];
				newComm->RotaryScannedPtsArray = gcnew cli::array<double*>((MAINDllOBJECT->currCamHeight - 2 * BorderGap) / newComm->StepSize);
				newComm->RotaryScanPositions = gcnew cli::array<double*>((MAINDllOBJECT->currCamHeight - 2 * BorderGap) / newComm->StepSize);
				for (int i = 0; i < newComm->RotaryScannedPtsArray->Length; i++)
				{
					double* scPts = (double*)malloc(2000 * sizeof(double));
					newComm->RotaryScannedPtsArray[i] = scPts;
					double* pos = (double*)malloc(4 * sizeof(double));
					newComm->RotaryScanPositions[i] = pos;
				}

				
				currentTIS->CommandsList->Add(newComm);

				//Step 4 - Move to next level
				//ShankAlignStep^ NextStep = (ShankAlignStep^)currentTIS->ShankAlignColl[1];
				newComm = gcnew TISCommand();
				double TargetCPosition = currentTIS->CommandsList[currentTIS->CommandsList->Count - 1]->TargetPosition[3];
				newComm->TargetPosition = gcnew cli::array < double > {	MAINDllOBJECT->TIS_CAxis[0] + MAINDllOBJECT->H_CamPosition[0] - ShankDia / 2, 
																		MAINDllOBJECT->TIS_CAxis[1] + MAINDllOBJECT->H_CamPosition[1],
																		RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] + currentTIS->ShankAlignColl[1]->Height, //DatumPoint[2] + Parameters[1] + Parameters[2],
																		TargetCPosition};
				newComm->FeedRate = gcnew cli::array < double > {0, 0, 5, 0}; // TIS_Control::MYINSTANCE()->RotaryTravelSpeed};
				newComm->ActionType = TISActionType::TravelCommand;
				currentTIS->CommandsList->Add(newComm);

				//Step 5 - Rotary Scan at second level
				newComm = gcnew TISCommand();
				rotateBy = 2 * M_PI;
				if (TargetCPosition > 0) rotateBy *= -1;
				newComm->TargetPosition = gcnew cli::array < double > {	MAINDllOBJECT->TIS_CAxis[0] + MAINDllOBJECT->H_CamPosition[0] - ShankDia / 2,
																		MAINDllOBJECT->TIS_CAxis[1] + MAINDllOBJECT->H_CamPosition[1],
																		RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] + currentTIS->ShankAlignColl[1]->Height, //DatumPoint[2] + Parameters[1] + Parameters[2],
																		TargetCPosition + rotateBy};
				newComm->FeedRate = gcnew cli::array < double > {0, 0, 0, 4}; // TIS_Control::MYINSTANCE()->RotaryTravelSpeed};
				newComm->ActionType = TISActionType::RotaryScan;
				newComm->ActionParameters = gcnew cli::array<double> {BorderGap, BorderGap, MAINDllOBJECT->currCamWidth - BorderGap, MAINDllOBJECT->currCamHeight - BorderGap };
				newComm->ShapeType = "Line3D";
				newComm->ShapeFromStep = -1;
				newComm->FG_Type = "FrameGrab Rectangle";
				//Instantiate the array of double for this
				newComm->StepSize = Parameters[Parameters->Count - 1];
				newComm->RotaryScannedPtsArray = gcnew cli::array<double*>((MAINDllOBJECT->currCamHeight - 2 * BorderGap) / newComm->StepSize);
				newComm->RotaryScanPositions = gcnew cli::array<double*>((MAINDllOBJECT->currCamHeight - 2 * BorderGap) / newComm->StepSize);
				for (int i = 0; i < newComm->RotaryScannedPtsArray->Length; i ++)
				{
					double* scPts = (double*)malloc(2000 * sizeof(double)); //we do not expect to get more than 1000 points per rotation scan!
					newComm->RotaryScannedPtsArray[i] = scPts;
					double* pos = (double*)malloc(4 * sizeof(double));
					newComm->RotaryScanPositions[i] = pos;
				}
				currentTIS->CommandsList->Add(newComm);
				ShankPtsCt = 0;
				NumberofCommands = currentTIS->CommandsList->Count;
				break;
			}
			case RWrapper::TISMeasureTool::ShankDia:
				break;
			case RWrapper::TISMeasureTool::CuttingDia:
				break;
			case RWrapper::TISMeasureTool::FaceRunout:
				break;
			case RWrapper::TISMeasureTool::PointAngle:
				break;
			case RWrapper::TISMeasureTool::StepLength:
				break;
			case RWrapper::TISMeasureTool::BallNose:
				break;
			case RWrapper::TISMeasureTool::MaxLine:
				break;
			case RWrapper::TISMeasureTool::MaxArc:
				break;
			case RWrapper::TISMeasureTool::Helix:
				break;
			case RWrapper::TISMeasureTool::CoreDia:
				break;
			case RWrapper::TISMeasureTool::CornerDia:
				break;
			case RWrapper::TISMeasureTool::BackTaper:
				break;
			default:
				break;
			}
		}
		catch (System::Exception^ ex)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRTIS0009", ex);
			NumberofCommands = -1;
		}
		return NumberofCommands;
	}

	void TIS_Control::RunCommandsFromList()
	{
		try
		{
			if (CurrentStepIndex < 0) return;
			TISCommand^ thisCommand;
			if (CurrentStepIndex < currentTIS->CommandsList->Count)
			{
				thisCommand = (TISCommand^)CommandsList[CurrentStepIndex];

				pin_ptr<double> targetPtr = &thisCommand->TargetPosition[0];
				pin_ptr<double> frPtr = &thisCommand->FeedRate[0];
				if (!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode()) RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
				//We are starting. 
				if (thisCommand->ActionType == TISActionType::RotaryScan)
				{
					this->RotaryScanisON = true;
					//Start the thread to do rotaryscan in a separate while loop running on a different thread
					//System::Threading::Thread^ t1 = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(this, &TIS_Control::RefDot_in_SeparateThread));
					//t1->Start();
					currentTIS->RotaryScanForCurrentTask(true);
				}
				RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(targetPtr, frPtr, TargetReachedCallback::TIS_COMMAND_TARGETREACHED);
			}
		}
		catch (System::Exception^ ex)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRTIS0005", ex);
		}

	}

	void TIS_Control::DoCommandActions()
	{
		try
		{
			TISCommand^ thisCommand = currentTIS->CommandsList[CurrentStepIndex];
			switch (thisCommand->ActionType)
			{
			case TISActionType::StaticFrameGrab:
				//First Get the shape 
				if (thisCommand->ShapeFromStep == -1) //We have to make a new shape
				{
					//Create a new shape.
					RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click(thisCommand->ShapeType);
					MAINDllOBJECT->selectShape(Convert::ToInt32(RWrapper::RW_ShapeParameter::MYINSTANCE()->ShapeParameter_ArrayList[0]), false);
					thisCommand->ShapeID = ((ShapeWithList*)MAINDllOBJECT->getShapesList().selectedItem())->getId();
				}
				else //Get the shape from appropriate step. 
				{
					MAINDllOBJECT->selectShape(currentTIS->CommandsList[thisCommand->ShapeFromStep]->ShapeID, false);
				}
				//Choose the correct FrameGrab
				RWrapper::RW_MainInterface::MYINSTANCE()->HandleCrossHairToolbar_Click(thisCommand->FG_Type);
				if (thisCommand->FG_Type == "FrameGrab Rectangle")
				{
					MAINDllOBJECT->OnLeftMouseDown_Video(thisCommand->ActionParameters[0], thisCommand->ActionParameters[1]);
					MAINDllOBJECT->OnLeftMouseDown_Video(thisCommand->ActionParameters[2], thisCommand->ActionParameters[3]);
				}

				break;
			case TISActionType::RotaryScan:
				this->RotaryScanisON = false;
				currentTIS->RotaryScanForCurrentTask(false);
				break;
			default:
				break;
			}
			CurrentStepIndex++;
			if (CurrentStepIndex >= currentTIS->CommandsList->Count)
			{	//We have finished the cycle. Let us finalise and complete.
				Finish_Measurement(true);
				return;
			}
			RunCommandsFromList();
		}
		catch (System::Exception^ ex)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRTIS0006", ex);
		}
	}

	void TIS_Control::RotaryScanForCurrentTask(bool BeginTask)
	{
		try
		{
			if (ScanningIsInProgress || !this->RotaryScanisON) return;
			//Check to see if current Command requires scanning. If not, exit this, since this function will be called everytime rotation happens!
			if (CurrentStepIndex < 0 || ((TISCommand^)CommandsList[CurrentStepIndex])->ActionType != TISActionType::RotaryScan) return;
			ScanningIsInProgress = true;
			switch (CurrentTool)
			{
			case RWrapper::TISMeasureTool::GetAxis:
			{
				if (BeginTask)
				{				////Get Image from Camera
				//MAINDllOBJECT->SetImageInEdgeDetection(1, true, 1);
					System::Threading::Thread^ t1 = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(this, &TIS_Control::RefDot_in_SeparateThread));
					t1->Start();
					//If we have rotated 90 degrees, let us calculate the centre and see if we have to stop and redo from beginning - 
					//if (abs(this->RotaryStartPosition[3] - RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]) > 1.5707)
					//{
					//	Finish_Measurement(false);
					//	if (abs(MAINDllOBJECT->TIS_CAxis[0] - RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0]) > 0.35 || abs(MAINDllOBJECT->TIS_CAxis[1] - RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1]) > 0.275)
					//	{
					//		double target[4] = { MAINDllOBJECT->TIS_CAxis[0], MAINDllOBJECT->TIS_CAxis[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] };
					//		double frate[4] = { 5, 5, 5, 1 };
					//		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(target, frate, TargetReachedCallback::TIS_COMMAND_TARGETREACHED);
					//	}
					//}
				}
				break;
			}
			case RWrapper::TISMeasureTool::ShankAlignment:
			{//TISCommand^ thisCommand = (TISCommand^)CommandsList[CurrentStepIndex];
				//MAINDllOBJECT->SetImageInEdgeDetection(1);
				System::Threading::Thread^ t1 = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(this, &TIS_Control::FrameGrab_in_SeparateThread));
				t1->Start();
				break;
			}
			case RWrapper::TISMeasureTool::ShankDia:
				break;
			case RWrapper::TISMeasureTool::CuttingDia:
				break;
			case RWrapper::TISMeasureTool::FaceRunout:
				break;
			case RWrapper::TISMeasureTool::PointAngle:
				break;
			case RWrapper::TISMeasureTool::StepLength:
				break;
			case RWrapper::TISMeasureTool::BallNose:
				break;
			case RWrapper::TISMeasureTool::MaxLine:
				break;
			case RWrapper::TISMeasureTool::MaxArc:
				break;
			case RWrapper::TISMeasureTool::Helix:
				break;
			case RWrapper::TISMeasureTool::CoreDia:
				break;
			case RWrapper::TISMeasureTool::CornerDia:
				break;
			case RWrapper::TISMeasureTool::BackTaper:
				break;
			default:
				break;
			}
			RotationalScanEvent::raise(ScannedPtsCollection->size());
		}
		catch (System::Exception^ ex)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRTIS0006", ex);
		}
		ScanningIsInProgress = false;
	}

	void TIS_Control::RefDot_in_SeparateThread()
	{
		try
		{
			//int count = 0, ScanCt = 0;
			while (this->RotaryScanisON)
			{
				//if (count == currentTIS->PointPickingDelay)
				//{
				MAINDllOBJECT->SetImageInEdgeDetection(); // 1, true, 1);
					double refdotParams[3] = { 0,0,0 };
					//Analyse for reference dot in image
					int noofpoints = EDSCOBJECT->Analyze_RotatingDot(true); // , MAINDllOBJECT->RefDot_Diameter(), refdotParams);
					////set current position in pixels
					////Get all the points and translate to a native linear double array.
					//double* Temppts = (double*)malloc(sizeof(double) *  noofpoints * 2);
					//for (int n = 0; n < noofpoints; n++)
					//{
					//	Temppts[n * 2] = cx + EDSCOBJECT->DetectedPointsList[n * 2] * upp;
					//	Temppts[n * 2 + 1] = cy - EDSCOBJECT->DetectedPointsList[n * 2 + 1] * upp;
					//}
					////Calculate position by doing a circle
					//BESTFITOBJECT->Circle_BestFit(Temppts, noofpoints, &ans[0], false);

					//Add the centre point X and Y to the scanned points collections. 
					if (noofpoints > 20)
					{
						//ScanCt++;
						currentTIS->CurrentScanDoneCount++;
						ScannedPtsCollection->push_back(Vector(refdotParams[0], refdotParams[1], 0)); // MAINDllOBJECT->getCurrentDRO().getZ()));
						//RotationalScanEvent::raise(ScanCt);
					}
					if (ScannedPtsCollection->size() > 500) break;
					//count = 0;
				//}
				//else
				//	count++;
			}
		}
		catch (Exception^ ex)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRTIS0007", ex);
		}
	}

	void TIS_Control::Finish_Measurement(bool InformAllSubscribers)
	{
		try
		{
			this->CurrentStepIndex = -1;
			switch (CurrentTool)
			{
			case RWrapper::TISMeasureTool::GetAxis:
			{
				//Get all the XY coordinates for the dot positions around the scan and do best-fit circle. 
				int n = 0;
				double* Temppts = (double*)malloc(sizeof(double) * ScannedPtsCollection->size() * 2);
				double ans[3];
				for (std::list<Vector>::iterator itr = ScannedPtsCollection->begin(); itr != ScannedPtsCollection->end(); itr++)
				{
					Temppts[n * 2] = (*itr).getX();
					Temppts[n * 2 + 1] = (*itr).getY();
					n++;
				}
				BESTFITOBJECT->Circle_BestFit(Temppts, ScannedPtsCollection->size(), &ans[0], true);
				double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y, upp = MAINDllOBJECT->getWindow(0).getUppX() / HELPEROBJECT->CamSizeRatio;

				//centre of circle is the axis of rotation!
				MAINDllOBJECT->TIS_CAxis[0] = cx + ans[0] * upp; MAINDllOBJECT->TIS_CAxis[1] = cy - ans[1] * upp; MAINDllOBJECT->TIS_CAxis[2] = MAINDllOBJECT->getCurrentDRO().getZ();
				//We assume there is no tilt of axis of rotation with respect to Z axis now. 15 Jan 2015
				MAINDllOBJECT->TIS_CAxis[3] = 0; MAINDllOBJECT->TIS_CAxis[4] = 0; MAINDllOBJECT->TIS_CAxis[5] = 1;
				//Raise the finished event...
				cli::array<double>^ answer = gcnew cli::array < double > { MAINDllOBJECT->TIS_CAxis[0], MAINDllOBJECT->TIS_CAxis[1] };
				for (int i = 0; i < 6; i++)
					this->TIS_Axis[i] = MAINDllOBJECT->TIS_CAxis[i];

				if (InformAllSubscribers)
				{
					FinishedRotaryScanEvent::raise(answer);
					//Write Values to file.
					System::String^ SfilePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath;
					SfilePath = SfilePath + "\\Log\\AxisLog\\AxisCalib.csv";
					System::IO::StreamWriter^ pointsFile = gcnew System::IO::StreamWriter(SfilePath, true);
					System::String^ currentTime = Microsoft::VisualBasic::DateAndTime::Now.ToShortDateString() + ", " + Microsoft::VisualBasic::DateAndTime::Now.ToString("HH:mm:ss");
					pointsFile->WriteLine(currentTime); // << wch2 << endl;
					for (int ii = 0; ii < 7; ii++)
						pointsFile->Write(MAINDllOBJECT->TIS_CAxis[ii] + ", ");

					pointsFile->WriteLine(); // << endl;
					pointsFile->Close();
				}
				break;
			}

			case RWrapper::TISMeasureTool::ShankAlignment:
				if (BESTFITOBJECT->Line_BestFit_3D(ShankOrientationPts, ShankPtsCt / 3, ShankCorrection))
				{
					System::String^ SfilePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath;
					SfilePath = SfilePath + "\\Log\\AxisLog\\ShankAlignment.csv";
					System::IO::StreamWriter^ pointsFile = gcnew System::IO::StreamWriter(SfilePath, true);
					System::String^ currentTime = Microsoft::VisualBasic::DateAndTime::Now.ToShortDateString() + ", " + Microsoft::VisualBasic::DateAndTime::Now.ToString("HH:mm:ss");
					for (int ii = 0; ii < 6; ii++)
						pointsFile->Write(ShankCorrection[ii] + ", ");
					pointsFile->WriteLine(currentTime);
					//pointsFile->WriteLine(); 
					pointsFile->Close();
					if (InformAllSubscribers)
					{
						cli::array<double>^ answer = gcnew cli::array < double > { ShankCorrection[3], ShankCorrection[4], ShankCorrection[5] };
						FinishedRotaryScanEvent::raise(answer);
					}
				}

				break;
			case RWrapper::TISMeasureTool::ShankDia:
				break;
			case RWrapper::TISMeasureTool::CuttingDia:
				break;
			case RWrapper::TISMeasureTool::FaceRunout:
				break;
			case RWrapper::TISMeasureTool::PointAngle:
				break;
			case RWrapper::TISMeasureTool::StepLength:
				break;
			case RWrapper::TISMeasureTool::BallNose:
				break;
			case RWrapper::TISMeasureTool::MaxLine:
				break;
			case RWrapper::TISMeasureTool::MaxArc:
				break;
			case RWrapper::TISMeasureTool::Helix:
				break;
			case RWrapper::TISMeasureTool::CoreDia:
				break;
			case RWrapper::TISMeasureTool::CornerDia:
				break;
			case RWrapper::TISMeasureTool::BackTaper:
				break;
			default:
				break;
			}
		}
		catch (System::Exception^ ex)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRTIS0007", ex);
		}
	}

	void TIS_Control::FrameGrab_in_SeparateThread()
	{
		//Image is set in EDSC. So let us start framegrab and get the points. 
		TISCommand^ thisCommand = currentTIS->CommandsList[CurrentStepIndex];
		int RectFGArray[5] = { 0 };
		int ptsindex = 0;
		int maxPtsCt = 1000;
		int PtsGotten = 0;
		double* thisPtsList, thisPosition;
		try
		{
			while (RotaryScanisON)
			{
				PtsGotten = EDSCOBJECT->DetectEdgeStraightRect(&RectFGArray[0], EdgeDetectionSingleChannel::ScanDirection::Rightwards);
				if (PtsGotten > 20)
				{
					//Set current DRO position
					for (int ii = 0; ii < 4; ii++)
						thisCommand->RotaryScanPositions[this->CurrentScanDoneCount][ii] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[ii];
					//Get the points from FG and copy to our buffers
					thisPtsList = thisCommand->RotaryScannedPtsArray[this->CurrentScanDoneCount];
					if (PtsGotten < maxPtsCt) maxPtsCt = PtsGotten;
					//We are doing according to step size to minimise calculation time...
					for (int j = 0; j < maxPtsCt; j += thisCommand->StepSize)
					{
						if (EDSCOBJECT->DetectedPointsList[2 * j + 1] == thisCommand->ActionParameters[1] + j * thisCommand->StepSize + 1)
						{
							thisPtsList[ptsindex++] = EDSCOBJECT->DetectedPointsList[2 * j];
							thisPtsList[ptsindex++] = EDSCOBJECT->DetectedPointsList[2 * j + 1];
						}
						//Do a check for overflow on number of points larger than our buffer size. 
						if (ptsindex > thisCommand->RotaryScannedPtsArray->Length - 1) break;
					}
					this->CurrentScanDoneCount++;
				}
				//Buffersize check on the number of frames. 
				if (this->CurrentScanDoneCount > thisCommand->RotaryScannedPtsArray->Length - 1) break;
			}
		}
		catch (Exception^ ex)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRTIS0008", ex);
		}
	}

	void TIS_Control::AxisAlignmentProcessingPerStage_Threaded(TISCommand^ stageCommand)
	{
		try
		{
			double* temppts = (double*)malloc(2000 * sizeof(double));
			int ct = 0;
			int CurrentScanCount = currentTIS->CurrentScanDoneCount;
			double tpt[4] = { 0 }, tRotatedPt[4] = { 0 }, RunoutCorrection[4] = { 0 };
			double upp_x = MAINDllOBJECT->getWindow(0).getUppX() / HELPEROBJECT->CamSizeRatio, upp_y = MAINDllOBJECT->getWindow(0).getUppY() / HELPEROBJECT->CamSizeRatio;
			double HalfScreenX = MAINDllOBJECT->getWindow(0).getWinDim().x * HELPEROBJECT->CamSizeRatio / 2, HalfScreenY = MAINDllOBJECT->getWindow(0).getWinDim().y * HELPEROBJECT->CamSizeRatio / 2;

			for (int y = stageCommand->ActionParameters[1]; y < stageCommand->ActionParameters[3]; y += stageCommand->StepSize)
			{
				for (int i = 0; i < CurrentScanCount; i++)
				{
					for (int j = 1; j < 2000; j++)
					{
						//tpt[0] = cx + EDSCOBJECT->DetectedPointsList[n * 2] * upp_x, cy - EDSCOBJECT->DetectedPointsList[n * 2 + 1] * upp_y, Cdro.getZ())
						//RMATH3DOBJECT->RotatePoint_Around_C_Axis()
						if (stageCommand->RotaryScannedPtsArray[i][j] == y)
						{
							tpt[0] = stageCommand->RotaryScanPositions[i][0] + (stageCommand->RotaryScannedPtsArray[i][j - 1] - HalfScreenX) * upp_x;
							tpt[1] = stageCommand->RotaryScanPositions[i][2] - (stageCommand->RotaryScannedPtsArray[i][j] - HalfScreenY) * upp_y;
							tpt[2] = stageCommand->RotaryScanPositions[i][1];
							tpt[3] = stageCommand->RotaryScanPositions[i][3];
							RMATH3DOBJECT->RotatePoint_Around_C_Axis(tpt, RunoutCorrection, MAINDllOBJECT->TIS_CAxis, tRotatedPt);
							temppts[ct++] = tRotatedPt[0];
							temppts[ct++] = tRotatedPt[1];
						}
					}
				}
				BESTFITOBJECT->Circle_BestFit(temppts, ct / 2, tpt, false);
				ShankOrientationPts[ShankPtsCt++] = tpt[0];
				ShankOrientationPts[ShankPtsCt++] = tpt[1];
			}
		}
		catch (System::Exception^ ex)
		{

		}
	}

	void TIS_Control::ResetTISAxis()
	{
		try
		{
			for (int i = 0; i < 6; i++)
				MAINDllOBJECT->TIS_CAxis[i] = 0;
		}
		catch (System::Exception^ ex)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRTIS0007", ex);
		}
	}

	void TIS_Control::SetPoint(cli::array<double>^ GivenPt, int ForRefPoint, bool RefPtTaken)
	{
		//cli::array<double> ^targetPt;
		if (ForRefPoint == 0)
		{
			DatumPoint = gcnew cli::array<double>(GivenPt->Length);;
			Array::Copy(GivenPt, DatumPoint, GivenPt->Length);
			DatumTaken = RefPtTaken;
		}
		else if (ForRefPoint == 1)
		{
			TopRefPoint = gcnew cli::array<double>(GivenPt->Length);
			Array::Copy(GivenPt, TopRefPoint, GivenPt->Length);
			TopRefTaken = RefPtTaken;
		}
		 
		return;
	}

	void TIS_Control::RestoreAxisofRotation()
	{
		System::String^ SfilePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath;
		SfilePath = SfilePath + "\\Log\\AxisLog\\AxisCalib.csv";
		System::IO::StreamReader^ pointsFile = gcnew System::IO::StreamReader(SfilePath);
		String^ currentLine = "";
		while (!pointsFile->EndOfStream)
		{
			currentLine = pointsFile->ReadLine();
		}
		cli::array<String^>^ axisstring = currentLine->Split(',');
		for (int i = 0; i < 7; i++)
			MAINDllOBJECT->TIS_CAxis[i] = Convert::ToDouble(axisstring[i]);
		pointsFile->Close();
		cli::array<double>^ answer = gcnew cli::array < double > { MAINDllOBJECT->TIS_CAxis[0], MAINDllOBJECT->TIS_CAxis[1] };
		FinishedRotaryScanEvent::raise(answer);
	}

}
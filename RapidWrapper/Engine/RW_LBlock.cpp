#include "stdafx.h"
#include "RW_LBlock.h"
#include "RW_MainInterface.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "..\MainDLL\Shapes\Line.h"	
CloudPoints* LBlockPts;
Cone* CurrentCone;

RWrapper::CNCCommand::CNCCommand(int NoOfAxes)
{
	AxisNumber = NoOfAxes;
	Position = gcnew cli::array<double>(NoOfAxes);
	Speed = gcnew cli::array<double>(NoOfAxes);
}

bool RWrapper::CNCCommand::SetPosition(cli::array<double>^ Target)
{
	if (Target->Length < AxisNumber) return false;
	Array::Copy(Target, Position, AxisNumber);
	return true;
}

bool RWrapper::CNCCommand::SetSpeed(cli::array<double>^ FeedRate)
{
	if (FeedRate->Length < AxisNumber) return false;
	Array::Copy(FeedRate, Speed, AxisNumber);
	return true;
}

void RWrapper::CNCCommand::MoveAxisTo(int AxisIndex, double MoveTo)
{
	Position[AxisIndex] = MoveTo;
}

void RWrapper::CNCCommand::MoveAxisBy(int AxisIndex, double MoveBy)
{
	Position[AxisIndex] += MoveBy;
}

void RWrapper::CNCCommand::SetAxisSpeed(int AxisIndex, double AxisSpeed)
{
	Speed[AxisIndex] = AxisSpeed;
}

RWrapper::CNCCommand::~CNCCommand()
{

}

bool RWrapper::CNCCommand::Set(cli::array<double>^ Target, cli::array<double>^ FeedRate, bool probePt, int ProbDir)
{
	if (FeedRate->Length < AxisNumber) return false;
	if (Target->Length < AxisNumber) return false;
	Array::Copy(Target, Position, AxisNumber);
	Array::Copy(FeedRate, Speed, AxisNumber);
	ProbePoint = probePt;
	ProbeHitDirection = ProbDir;
	return true;
}


RWrapper::RW_LBlock::RW_LBlock()
{
	try
	{
		LBlock = this;
		Params = gcnew cli::array<double>(6);
		StartPosition = gcnew cli::array<double>(4);
		cPosition = gcnew cli::array<double>(4);
		instantiated = true;
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_LBLock_001", ex);
	}
}

RWrapper::RW_LBlock::~RW_LBlock()
{
	instantiated = false;
}

RWrapper::RW_LBlock^ RWrapper::RW_LBlock::MYINSTANCE()
{
	//if (!LBlock)
	//	gcnew RWrapper::RW_LBlock();
	return LBlock;
}

bool RWrapper::RW_LBlock::Initialise_LBlockSettings(cli::array<double>^ LBlockParams, int AxistobeRun, bool NegativeDir, bool byTouch)
{
	try
	{
		//Copy the L Block dimensions into local array so that we do not forget!!
		Array::Copy(LBlockParams, Params, 6);

		//Let us make a cloud Pt shape to hold the points
		RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Cloud Points");
		LBlockPts = (CloudPoints*)MAINDllOBJECT->getShapesList().selectedItem();

		//Initialise start position as the current Position in DRO. 
		for (int i = 0; i < 4; i++)
		{
			StartPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
			cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		}

		//Get Feedrate from Front End
		cli::array<double>^ feedrate = { RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1],
			RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3] };

		//Set direction of touch motion
		direction = 1;
		if (NegativeDir) direction = -1;
		//Set the flag that this is the first step so that we can continue the rest by calculating and run the rest. 
		FirstStepRunning = true;
		currentAxis = AxistobeRun;
		RunningCone = false;
		//Step 1 - explore where the L-Block edge is in the direction of motion at speed 2 in the axis
		RWrapper::CNCCommand^ PathPoint;
		PathPoint = gcnew RWrapper::CNCCommand(4);
		int pDir = 4 * AxistobeRun + 1;
		if (AxistobeRun > 0)
		{
			pDir += 2;
			if (direction < 0) pDir -= 2;
		}
		else
			if (direction < 0) pDir += 2;
		cPosition[AxistobeRun] += direction * (10 + RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue);

		PathPoint->Set(cPosition, feedrate, true, pDir);
		//Send the command to start...
		if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 1)
		{
			//Set Probe Direction
			DROInstance->SetProbeDirection(PathPoint->ProbeHitDirection);
			//Set Probe Flag
			RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = PathPoint->ProbePoint;
			//Activate CNC if its not ON.
			if (!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode()) RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
			// Send command with type of callback
			pin_ptr<double> pPosition = &PathPoint->Position[0];
			pin_ptr<double> pSpeed = &PathPoint->Speed[0];
			
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(pPosition, pSpeed, LBLOCK_MODULE);
		}
		return true;
	}
	catch (Exception^ ex)
	{
		//Initialised_LBlock = false;
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_LBLock_002", ex);
		return false;
	}
}

void RWrapper::RW_LBlock::Run_LBlock()
{
	try
	{
		//First exploratory touch is done. We 
		//We are ready. Let us calculate the points to go and touch...
		FirstStepRunning = false;
		CalculateSteps(currentAxis, direction, true); //R A N 14 Apr 2015. Set to true since first round will be for touch only
		//Once we have calculated, we are ready to go...
		CycleStarted = true;
		HandleCompletedCurrentStep();

	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_LBLock_003", ex);
	}
}

void RWrapper::RW_LBlock::CalculateSteps(int AxistobeRun, int direction, bool forTouch)
{
	try
	{
		if (CycleStarted) return;
		//  Let us get the first Touched Point coordinate. That will be the first point relative to which we will do all other
		//  points calculations. 
		cli::array<double>^ firstPt = gcnew cli::array<double>(4);
		if (LBlockPts->PointsList->Pointscount() > 0)
		{
			for (PC_ITER SptItem = LBlockPts->PointsList->getList().begin(); SptItem != LBlockPts->PointsList->getList().end(); SptItem++)
			{
				SinglePoint* Spt = (*SptItem).second;
				firstPt[0] = Spt->X; firstPt[1] = Spt->Y; firstPt[2] = Spt->Z; firstPt[3] = Spt->R;
				break;
			}
		}
		else // No points were taken . So we have not started yet!!!
			return;

		RWrapper::CNCCommand^ PathPoint;
		CurrentStepNumber = 0;

		//Get Feedrate from Front End
		cli::array<double>^ feedrate = { RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1],
			RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3] };

		//Step 1 - We got first pt. Now we go up in Z a little in axis of movement
		cPosition[2] += Params[1];
		//Similar triangles for first step We have a step of L1 mm for a height travel of H2. We need to move a total of Ht. 
		double tempSpeed = feedrate[AxistobeRun];
		if (Params[5] > 0)
		{
			cPosition[AxistobeRun] += 0.25 * direction * Params[1] * Params[2] / Params[5];
			feedrate[AxistobeRun] *= Params[2] / Params[5];
			if (feedrate[AxistobeRun] < 0.4) feedrate[AxistobeRun] = 0.4;
		}
		else
			cPosition[AxistobeRun] += direction * Params[1];

		PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::CNCCommand^>();

		//Step 1 Move up to the highest position and move in axis by a smaller amount. 
		PathPoint = gcnew RWrapper::CNCCommand(4);
		PathPoint->Set(cPosition, feedrate, false, 0);
		PointsToGoList->Add(PathPoint);

		//Step 2 Move to the left to the outside of the L-Block
		cPosition[AxistobeRun] = StartPosition[AxistobeRun] + direction * (Params[0] + Params[2]);
		feedrate[AxistobeRun] = tempSpeed;
		PathPoint = gcnew RWrapper::CNCCommand(4);
		PathPoint->Set(cPosition, feedrate, false, 0);
		PointsToGoList->Add(PathPoint);

		//Now we have to come down in Z axis by H1 so that we can touch on the other side of L-Block
		cPosition[2] -= Params[4];
		PathPoint = gcnew RWrapper::CNCCommand(4);
		PathPoint->Set(cPosition, feedrate, false, 0);
		PointsToGoList->Add(PathPoint);

		//Now we move to the right by L1 and touch
		cPosition[AxistobeRun] -= direction * (Params[2] + RWrapper::RW_DBSettings::MYINSTANCE()->ProbeStopValue);
		int pDir = 4 * AxistobeRun + 3;
		if (direction > 0) pDir -= 2;

		PathPoint = gcnew RWrapper::CNCCommand(4);
		PathPoint->Set(cPosition, feedrate, true, pDir);
		PointsToGoList->Add(PathPoint);
		//Cycle of touch is complete!!!
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_LBLock_004", ex);
	}
}

void RWrapper::RW_LBlock::HandleCompletedCurrentStep()
{
	//Check if we have come to the last step
	if (CurrentStepNumber == PointsToGoList->Count)
	{
		LBlockStepCompleted::raise(1);
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
	}
	else
	{
		if (CurrentStepNumber > 0) LBlockStepCompleted::raise((float)CurrentStepNumber / (float)PointsToGoList->Count);
		// First let us get the current DRO position
		cli::array<double>^ currentPosition = gcnew cli::array<double>(4);
		for (int i = 0; i < 4; i++)
		{
			currentPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		}
		//Get the current Step we have to implement
		CNCCommand^ CurrentCommand = PointsToGoList[CurrentStepNumber];
		if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag == 1)
		{
			//Set Probe Direction
			DROInstance->SetProbeDirection(CurrentCommand->ProbeHitDirection);
			//Set Probe Flag
			RWrapper::RW_DRO::MYINSTANCE()->TakeProbePointFlag = CurrentCommand->ProbePoint;
			//Activate CNC if its not ON.
			if (!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode()) RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
			// Send command with type of callback
			pin_ptr<double> cPos = &CurrentCommand->Position[0];
			pin_ptr<double> cSpeed = &CurrentCommand->Speed[0];
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(cPos, cSpeed, LBLOCK_MODULE);
		}
		CurrentStepNumber++;
	}
}

int RWrapper::RW_LBlock::GetAnswer(cli::array<double>^ Answers)
{
	if (RunningCone) return -1;

	cli::array<double>^ firstPt = gcnew cli::array<double>(4);
	cli::array<double>^ SecondPt = gcnew cli::array<double>(4);
	int i = 0;
	//Get the two touched points
	if (LBlockPts->PointsList->Pointscount() > 0)
	{
		for (PC_ITER SptItem = LBlockPts->PointsList->getList().begin(); SptItem != LBlockPts->PointsList->getList().end(); SptItem++)
		{
			SinglePoint* Spt = (*SptItem).second;
			if (i == 0)
			{
				firstPt[0] = Spt->X; firstPt[1] = Spt->Y; firstPt[2] = Spt->Z; firstPt[3] = Spt->R;
			}
			else if (i == 1)
			{
				SecondPt[0] = Spt->X; SecondPt[1] = Spt->Y; SecondPt[2] = Spt->Z; SecondPt[3] = Spt->R;
			}
			else
				break;
			i++;
		}
		// fill buffer with values.
		Answers[0] = firstPt[2]; 
		Answers[1] = firstPt[currentAxis];
		Answers[2] = SecondPt[2];
		Answers[3] = SecondPt[currentAxis];
	}
	return i;
}

void RWrapper::RW_LBlock::Stop_LBlock()
{
	//Get CNC to Manual Mode first
	RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);

	CurrentStepNumber = 0;
	if (PointsToGoList) PointsToGoList->Clear();
	CycleStarted = false;
}

void RWrapper::RW_LBlock::CalculateConeSteps(int NumberofLevels, double Z_StepSize, double ConeAngle)
{
	try
	{
		//We iterate through each Z level. 
		//Based on which Z level it is, the first touch will be in X or in Y. 
		//Level
		//Each touch has 2 path position commands and one touch command. 
		//If its the first step in the current level, we have a movement in Z as well. 
		int touchCounter = 0; //Whenever touchCounter is a multiple of 3, we have a touch point
		//int LocationCounter = 0; //When this value is a multiple of 2, we have come to the point of moving and touching a point. 
		RWrapper::CNCCommand^ PathPoint;

		//Let us make a Cone shape to hold the points
		RWrapper::RW_MainInterface::MYINSTANCE()->HandleDrawToolbar_Click("Cone");
		CurrentCone = (Cone*)MAINDllOBJECT->getShapesList().selectedItem();

		//Initialise start position as the current Position in DRO. 
		for (int i = 0; i < 4; i++)
		{
			StartPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
			cPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
		}

		//Get Feedrate from Front End
		cli::array<double>^ feedrate = { RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1],
			RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3] };

		//Lets get the first point with respect to which all other points will be calculated. 
		//array<double>^ firstPt = gcnew cli::array<double>(4);
		cli::array<double>^ CentrePt = gcnew cli::array<double>(4);

		//if (CurrentCone->PointsList->Pointscount() > 0)
		//{
		//	for (PC_ITER SptItem = CurrentCone->PointsList->getList().begin(); SptItem != CurrentCone->PointsList->getList().end(); SptItem++)
		//	{
		//		SinglePoint* Spt = (*SptItem).second;
		//		firstPt[0] = Spt->X; firstPt[1] = Spt->Y; firstPt[2] = Spt->Z; firstPt[3] = Spt->R;
		//		break;
		//	}
		//	//this tip point is to only locate the position of the cone. So let us now delete the point from the PointsList
		//	CurrentCone->PointsList->deletePoint(0);
		//}
		//else // No points were taken . So we have not started yet!!!
		//	return;
		
		PointsToGoList = gcnew System::Collections::Generic::List<RWrapper::CNCCommand^>();

		//First we decide how many mm we move down from the tip of the cone. THis will be independent of the gap given between levels
		int dir = 1; if (Z_StepSize < 0) dir = -1;
		//double MoveinZ = firstPt[2] + dir * (2 * MAINDllOBJECT->ProbeRadius() + 2); // We touch 2 mm below the tip of the cone. 
		double RadialDist = 0; // MoveinZ * tan(ConeAngle / 2) + 4;
		cli::array<int>^ MoveDir = gcnew cli::array<int>(4);
		cli::array<int>^ pDirection = gcnew cli::array<int>(4);
		MoveDir[0] = 1; MoveDir[1] = -1; MoveDir[2] = -1; MoveDir[3] = 1;
		pDirection[0] = 3; pDirection[1] = 7; pDirection[2] = 1; pDirection[3] = 5;
		FirstStepRunning = false;
		CurrentStepNumber = 0;

		Array::Copy(StartPosition, CentrePt, 4);

		for (int level = 0; level < NumberofLevels; level++)
		{
			for (int i = 0; i < 4; i++)
			{
				if (i == 0)
				{
					feedrate[2] = 2;
					if (level == 0)
					{
						cPosition[2] = StartPosition[2] + (2 * MAINDllOBJECT->ProbeRadius() + 2) * dir;
						RadialDist = (2 * MAINDllOBJECT->ProbeRadius() + 2) * tan(ConeAngle / 2) + 2;
					}
					else
					{
						cPosition[2] = StartPosition[2] + level * Z_StepSize;
						RadialDist = abs(level * Z_StepSize) * tan(ConeAngle / 2) + 2;
					}
					CentrePt[2] = cPosition[2];
					//If the last touch was in Y, then do the touch in Y itself. we save 2 movement steps here
					//if (level % 2 == 0)
						cPosition[0] = StartPosition[0] + MoveDir[i] * RadialDist;
					//else
					//	cPosition[1] = StartPosition[1] + MoveDir[i] * RadialDist;

					//Add a goto position command. 
					PathPoint = gcnew RWrapper::CNCCommand(4);
					PathPoint->Set(cPosition, feedrate, false, 0);
					PointsToGoList->Add(PathPoint);
				}

				//Add the first touch command - ask it to come to centre at speed 2...
				PathPoint = gcnew RWrapper::CNCCommand(4);
				//int pDir = LocationCounter % 4 + 1;
				//if (LocationCounter % 4 == 1) pDir = 7;
				//if (LocationCounter % 4 == 3) pDir = 5;
				
				PathPoint->Set(CentrePt, feedrate, true, RWrapper::RW_LBlock::MYINSTANCE()->GetProbeDirection(cPosition, CentrePt));
				PointsToGoList->Add(PathPoint);

				//Move away from cone in the other axis from touch...
				if (i % 2 == 0)
					cPosition[1] = StartPosition[1] + MoveDir[i] * RadialDist;
				else
					cPosition[0] = StartPosition[0] + MoveDir[i] * RadialDist;

				PathPoint = gcnew RWrapper::CNCCommand(4);
				PathPoint->Set(cPosition, feedrate, false, 0);
				PointsToGoList->Add(PathPoint);

				if (i % 2 == 0)
					cPosition[0] = StartPosition[0];// + MoveDir[LocationCounter % 4] * RadialDist;
				else
					cPosition[1] = StartPosition[1]; // +MoveDir[LocationCounter % 4] * RadialDist;

				PathPoint = gcnew RWrapper::CNCCommand(4);
				PathPoint->Set(cPosition, feedrate, false, 0);
				PointsToGoList->Add(PathPoint);
				//LocationCounter++;
			}
		}
		//Lets get back to Home position
		cPosition[2] = StartPosition[2];
		PathPoint = gcnew RWrapper::CNCCommand(4);
		PathPoint->Set(cPosition, feedrate, false, 0);
		PointsToGoList->Add(PathPoint);
		
		PathPoint = gcnew RWrapper::CNCCommand(4);
		PathPoint->Set(StartPosition, feedrate, false, 0);
		PointsToGoList->Add(PathPoint);

		System::String^ SfilePath = RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath;
		SfilePath = SfilePath + "\\Log\\PointsLog\\ConeCommands.csv";
		System::IO::StreamWriter^ pointsFile = gcnew System::IO::StreamWriter(SfilePath, true);
		System::String^ currentTime = Microsoft::VisualBasic::DateAndTime::Now.ToShortDateString() + ", " + Microsoft::VisualBasic::DateAndTime::Now.ToString("HH:mm:ss");
		pointsFile->WriteLine(currentTime); 

		for (int i = 0; i < PointsToGoList->Count; i++)
		{
			PathPoint = PointsToGoList[i];
			pointsFile->WriteLine(PathPoint->Position[0] + ", " + PathPoint->Position[1] + ", " + PathPoint->Position[2] + ", " + PathPoint->Position[3] + ", " + PathPoint->ProbeHitDirection);
		}
		pointsFile->Close();

		CycleStarted = true; RunningCone = true;
		//Activate CNC if its not ON.
		if (!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode()) RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
		HandleCompletedCurrentStep();
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_LBLock_004c", ex);
	}
}

void RWrapper::RW_LBlock::RunAutoCone()
{
	try
	{
		//First exploratory touch is done. We 
		//We are ready. Let us calculate the points to go and touch...
		FirstStepRunning = false;
		CycleStarted = true;
		HandleCompletedCurrentStep();
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_LBLock_003c", ex);
	}
}

int RWrapper::RW_LBlock::GetProbeDirection(cli::array<double>^ CurrentPosition, cli::array<double>^ TargetPosition)
{
	try
	{
		cli::array<double>^ Diff = gcnew cli::array<double>(4);
		for (int i = 0; i < 4; i++)
			Diff[i] = TargetPosition[i] - CurrentPosition[i];
		
		double biggestDiff = 0;

		int direction = 1;
		biggestDiff = abs(Diff[0]);
		if (Diff[0] < 0) direction = 3;
		for (int j = 1; j < 4; j++)
		{
			if (abs(Diff[j]) > biggestDiff)
			{
				biggestDiff = abs(Diff[j]);
				if (Diff[j] < 0)
					direction = 4 * j + 3;
				else
					direction = 4 * j + 1;
			}
		}
		return direction;
	}
	catch (Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_LBlock_008", ex); return 1;
	}
}
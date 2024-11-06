//Include the required header files..
#include "Stdafx.h"
#include "RW_MainInterface.h"
#include "..\MAINDLL\Actions\LinearInterPolationAction.h"

///Default constructor.. Initialise the deafault settings..
RWrapper::RW_LinearInterPolation::RW_LinearInterPolation()
{
	 try
	 {
		 this->LinearInterPolationInstance = this;
		 this->TargetPos = gcnew cli::array<double>(3);
		 this->TargetFlag = false;
	 }
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_LinInt0001", ex); }
}

//Destructor.. Release the memory..
RWrapper::RW_LinearInterPolation::~RW_LinearInterPolation()
{
	try
	{
		
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_LinInt0002", ex); }
}

//Exposes this class instance.. the Application no need to hold the object..
RWrapper::RW_LinearInterPolation^ RWrapper::RW_LinearInterPolation::MYINSTANCE()
{
	return LinearInterPolationInstance;
}


void RWrapper::RW_LinearInterPolation::SetTargetPosition()
{
	try
	{
		this->TargetFlag = true;
		this->TargetPos[0] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0];
		this->TargetPos[1] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1];
		this->TargetPos[2] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_LinInt0003", ex); }
}

void RWrapper::RW_LinearInterPolation::StartLinearInterpolation()
{
	try
	{
		if(!RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag)
		{
			RWrapper::RW_MainInterface::MYINSTANCE()->Update_MsgBoxStatus("RW_LinearInterPolation01", RWrapper::RW_Enum::RW_MSGBOXTYPE::MSG_OK, RWrapper::RW_Enum::RW_MSGBOXICONTYPE::MSG_INFORMATION, false, "");
			return;
		}
		RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMax = 0; RWrapper::RW_CircularInterPolation::MYINSTANCE()->HeightGaugeMin = 0;
		RWrapper::RW_CircularInterPolation::MYINSTANCE()->deleteTempCloudPtsColl();
		if(TargetFlag)
		{
			/*if(MAINDllOBJECT->CurrentStylusType == RapidEnums::STYLUSTYPE::ALONG_ZAXIS)
			{
				TargetPos[2] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
			}
			else if(MAINDllOBJECT->CurrentStylusType == RapidEnums::STYLUSTYPE::ALONG_YAXIS)
			{
				TargetPos[1] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1];
			}
			else if(MAINDllOBJECT->CurrentStylusType == RapidEnums::STYLUSTYPE::ALONG_XAXIS)
			{
				TargetPos[0] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0];
			}*/
			double Target[4] = {TargetPos[0], TargetPos[1], TargetPos[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
			Shape* CShape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
			if(CShape != NULL)
				LinearInterPolationAction::AddLinearInterPolationAction(CShape, Target, RWrapper::RW_CircularInterPolation::MYINSTANCE()->AddPointsFlag);
			else
				LinearInterPolationAction::AddLinearInterPolationAction(Target);
			RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::CIRCULARINTERPOLATION);
			if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
			double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
			DROInstance->GotoCommand(Target, feedrate);
			//RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(Target, feedrate, TargetReachedCallback::CIRCULARINTERPOLATION);
		}
		else
		{
			//please first take target position...
			//RWrapper::RW_MainInterface::MYINSTANCE()->Update_MsgBoxStatus("RW_CircularInterPolation02", RWrapper::RW_Enum::RW_MSGBOXTYPE::MSG_OK, RWrapper::RW_Enum::RW_MSGBOXICONTYPE::MSG_INFORMATION, false, "");
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_LinInt0004", ex); }
}

void RWrapper::RW_LinearInterPolation::ClearAll()
{
	try
	{
		this->TargetFlag = false;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_LinInt0005", ex); }
}

void RWrapper::RW_LinearInterPolation::TakePointForReference()
{
	try
	{
		MAINDllOBJECT->DoOnePointAlignment(RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] + RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_LinInt0006", ex); }
}
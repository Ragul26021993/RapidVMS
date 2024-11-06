#include "StdAfx.h"
#include "RW_MainInterface.h"
#include "RW_RepeatAutoMeasure.h"
#include "..\MAINDll\Engine\RepeatAutoMeasurement.h"


void CALLBACK WindowParamReset(int Id)
{
	try
	{
		RWrapper::RW_RepeatAutoMeasure::MYINSTANCE()->HandleCallBack(Id);
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_AutoMeasure0000", ex); }
}

RWrapper::RW_RepeatAutoMeasure::RW_RepeatAutoMeasure()
{
	try
	{
		AUTOMEASUREOBJECT->ResetParam_CircularRepeatMeasure = &WindowParamReset;
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_AutoMeasure0001", ex); }
}

RWrapper::RW_RepeatAutoMeasure::~RW_RepeatAutoMeasure()
{
}

bool RWrapper::RW_RepeatAutoMeasure::Check_RepeatAutoMeasure_Instance()
{
	if(AutoMeasureInstance == nullptr)
		return false;
	else
		return true;
}

void RWrapper::RW_RepeatAutoMeasure::Close_AutoMeasureWindow()
{
	try
	{
		if(AutoMeasureInstance == nullptr) return;
		RWrapper::RW_RepeatAutoMeasure::MYINSTANCE()->ClearAll();
		RepeatAutoMeasurement::DeleteInstance();
		delete AutoMeasureInstance;
		AutoMeasureInstance = nullptr;
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_AutoMeasure0002", ex); }
}

RWrapper::RW_RepeatAutoMeasure^ RWrapper::RW_RepeatAutoMeasure::MYINSTANCE()
{
	if(AutoMeasureInstance == nullptr)
		AutoMeasureInstance = gcnew RW_RepeatAutoMeasure();
	return AutoMeasureInstance;
}

void RWrapper::RW_RepeatAutoMeasure::SetAutomatedMode(bool flag)
{
	MAINDllOBJECT->AutomatedMeasureModeFlag(flag);
}

void RWrapper::RW_RepeatAutoMeasure::takePointOfRotation(bool flag)
{
	try
	{
		AUTOMEASUREOBJECT->TakeReferencePointFlag = flag;
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_AutoMeasure0003", ex); }
}

void RWrapper::RW_RepeatAutoMeasure::StartAutoMeasurement(double Angle, int Steps, bool AutoFocusEnable)
{
	try
	{
		if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
		AUTOMEASUREOBJECT->StartAutoMeasuremnet(Angle, Steps, AutoFocusEnable);
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_AutoMeasure0004", ex); }
}

void RWrapper::RW_RepeatAutoMeasure::Continue_PauseAutoMeasure(bool PauseFlag)
{
	try
	{
		if(PauseFlag)
		{
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(0);
		}
		else
		{
			if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
			AUTOMEASUREOBJECT->ContinueAutoMeasurement();
		}
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_AutoMeasure0005", ex); }
}

void RWrapper::RW_RepeatAutoMeasure::StopAutoMeasure()
{
	try
	{
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(0);
		AUTOMEASUREOBJECT->ClearAll();
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_AutoMeasure0006", ex); }
}

void RWrapper::RW_RepeatAutoMeasure::ClearAll()
{
	try
	{
		AUTOMEASUREOBJECT->ClearAll();
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_AutoMeasure0007", ex); }
}

void RWrapper::RW_RepeatAutoMeasure::HandleCallBack(int Id)
{
	try
	{
		AutoMeasureEvent::raise(Id);
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_AutoMeasure0008", ex); }
}
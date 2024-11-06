#include "StdAfx.h"
#include "RW_MainInterface.h"
#include "RW_RepeatShapeMeasure.h"
#include "..\MAINDll\Engine\RepeatShapeMeasure.h"


void CALLBACK RW_ShapeMeasureParamReset(int Id)
{
	try
	{
		RWrapper::RW_RepeatShapeMeasure::MYINSTANCE()->HandleCallBack(Id);
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_RepeatShapeMeasure00", ex); }
}

RWrapper::RW_RepeatShapeMeasure::RW_RepeatShapeMeasure()
{
	try
	{
		AUTOSHAPEMEASUREOBJECT->ResetParam_RepeatShapeMeasure = &RW_ShapeMeasureParamReset;
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_RepeatShapeMeasure01", ex); }
}

RWrapper::RW_RepeatShapeMeasure::~RW_RepeatShapeMeasure()
{
	MAINDllOBJECT->AutomatedMeasureModeFlag(false);
}

void RWrapper::RW_RepeatShapeMeasure::StartRecording(bool Flag)
{
	MAINDllOBJECT->AutomatedMeasureModeFlag(Flag);
}

bool RWrapper::RW_RepeatShapeMeasure::Check_RepeatShapeMeasure_Instance()
{
	if(_RepeatShapeMeasureInstance == nullptr)
		return false;
	else
		return true;
}

void RWrapper::RW_RepeatShapeMeasure::Close_RepeatShapeMeasureWindow()
{
	try
	{
		if(_RepeatShapeMeasureInstance == nullptr) return;
		RWrapper::RW_RepeatShapeMeasure::MYINSTANCE()->ClearAll();
		RepeatShapeMeasure::DeleteInstance();
		delete _RepeatShapeMeasureInstance;
		_RepeatShapeMeasureInstance = nullptr;
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_RepeatShapeMeasure02", ex); }
}

RWrapper::RW_RepeatShapeMeasure^ RWrapper::RW_RepeatShapeMeasure::MYINSTANCE()
{
	if(_RepeatShapeMeasureInstance == nullptr)
		_RepeatShapeMeasureInstance = gcnew RW_RepeatShapeMeasure();
	return _RepeatShapeMeasureInstance;
}

void RWrapper::RW_RepeatShapeMeasure::ClearAll()
{
	try
	{
		AUTOSHAPEMEASUREOBJECT->ClearAll();
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_RepeatShapeMeasure03", ex); }
}

void RWrapper::RW_RepeatShapeMeasure::setCoordinateListForAutoShapemeasure(System::Collections::Generic::List<System::Double>^ listcoord)
{
	try
	{
		if(!MAINDllOBJECT->AutomatedMeasureModeFlag()) return;
		 std::list<double> coordinateList;
		 for each(System::Double^ Cvalue in listcoord)
		 {
			  double temp = (double)Cvalue;
			  coordinateList.push_back(temp);
		 }
		 AUTOSHAPEMEASUREOBJECT->AutoMeasurementFor_IdenticalShapeMeasure(&coordinateList);
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_RepeatShapeMeasure04", ex); }
}

void RWrapper::RW_RepeatShapeMeasure::CreateNextGrid()
{
	try
	{
		AUTOSHAPEMEASUREOBJECT->InitialiseNextGrid();
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_RepeatShapeMeasure05", ex); }
}

void RWrapper::RW_RepeatShapeMeasure::RW_SetOrientationOfComponent()
{
	try
	{
		AUTOSHAPEMEASUREOBJECT->SetOrientationOfComponent();
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_RepeatShapeMeasure06", ex); }
}

void RWrapper::RW_RepeatShapeMeasure::HandleCallBack(int Id)
{
	try
	{
		ShapeMeasureEvent::raise(Id);
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_AutoMeasure0008", ex); }
}
#include "StdAfx.h"
#include "RW_MainInterface.h"
#include "RW_ImageDelayCalibration.h"
#include "..\MAINDll\Engine\ImageDelayCalibration.h"


void CALLBACK RW_ImageDelayCallback(double Delay, bool CalibrationSucess)
{
	try
	{
		RWrapper::RW_ImageDelayCalibration::MYINSTANCE()->RaiseImageDelayEvent(Delay, CalibrationSucess);
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_ImageDelayCalibration00", ex); }
}

RWrapper::RW_ImageDelayCalibration::RW_ImageDelayCalibration()
{
	try
	{
		MAINDllOBJECT->DrawRectanle_ImageDelay = true;
		IMAGEDELAYOBJECT->RaiseImageDelayEvent = &RW_ImageDelayCallback;
		MAINDllOBJECT->update_VideoGraphics();
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_ImageDelayCalibration01", ex); }
}

RWrapper::RW_ImageDelayCalibration::~RW_ImageDelayCalibration()
{
	
}

bool RWrapper::RW_ImageDelayCalibration::Check_ImageDelay_Instance()
{
	if(_ImageDelayInstance == nullptr)
		return false;
	else
		return true;
}

void RWrapper::RW_ImageDelayCalibration::Close_ImageDelayWindow()
{
	try
	{
		if(_ImageDelayInstance == nullptr) return;
		RWrapper::RW_ImageDelayCalibration::MYINSTANCE()->ClearAll();
		delete _ImageDelayInstance;
		_ImageDelayInstance = nullptr;
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_ImageDelayCalibration02", ex); }
}

RWrapper::RW_ImageDelayCalibration^ RWrapper::RW_ImageDelayCalibration::MYINSTANCE()
{
	if(_ImageDelayInstance == nullptr)
		_ImageDelayInstance = gcnew RW_ImageDelayCalibration();
	return _ImageDelayInstance;
}

void RWrapper::RW_ImageDelayCalibration::ClearAll()
{
	try
	{
		IMAGEDELAYOBJECT->ClearAll();
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_ImageDelayCalibration03", ex); }
}

void RWrapper::RW_ImageDelayCalibration::StartImageDelayCalibration(double Distance, double speed, double DroChange, double ImageAverageChange)
{
	try
	{
		if(MAINDllOBJECT->CameraConnected())
		{
			MAINDllOBJECT->DrawRectanle_ImageDelay = true;
			IMAGEDELAYOBJECT->StartImageDelayCalibration(DroChange, ImageAverageChange);
			if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
			double target[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0] - Distance, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
			double feedrate[4] = {speed, speed, speed, speed};
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::IMAGEDELAYCALIBRATION);
		}
		else
		{
			MAINDllOBJECT->DrawRectanle_ImageDelay = false;
			RWrapper::RW_ImageDelayCalibration::MYINSTANCE()->RaiseImageDelayEvent(0, false);
		}
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_ImageDelayCalibration04", ex); }
}

void RWrapper::RW_ImageDelayCalibration::StopImageCalibration()
{
	try
	{
		IMAGEDELAYOBJECT->ResetCalibrationFlag();
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_ImageDelayCalibration04", ex); }
}

void RWrapper::RW_ImageDelayCalibration::RaiseImageDelayEvent(double Delay, bool Sucess)
{
	try
	{
		ImageDelayEvent::raise(Delay, Sucess);
	}
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RW_ImageDelayCalibration05", ex); }
}
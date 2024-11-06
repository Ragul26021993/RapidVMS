#include "StdAfx.h"
#include "ImageDelayCalibration.h"
#include "RCadApp.h"
#include "..\MAINDLL\Helper\Helper.h"
#include <process.h>

ImageDelayCalibration* ImageDelayCalibration::_Instance = 0;

ImageDelayCalibration* ImageDelayCalibration::GetImgDelayInstance()
{
	if(_Instance == NULL)
		_Instance = new ImageDelayCalibration();
	return _Instance;
}

ImageDelayCalibration::ImageDelayCalibration()
{
	try
	{
		BoxSize = 20; AverageImageChange = 3; DroChange = 0.005;
		ContinueThread = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ImageDelayCalibration001", __FILE__, __FUNCSIG__); }
}

ImageDelayCalibration::~ImageDelayCalibration()
{
}

bool ImageDelayCalibration::CheckImgDelayInstance()
{
	if(_Instance == NULL)
		return false;
	else 
		return true;
}

void ImageDelayCalibration::DeleteInstance()
{
	try
	{
		if(_Instance != NULL)
		{
			delete _Instance;
			_Instance = NULL;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ImageDelayCalibration002", __FILE__, __FUNCSIG__); }
}
	
void ImageDelayCalibration::ClearAll()
{
	try
	{
		MAINDllOBJECT->DrawRectanle_ImageDelay = false;
		this->ContinueThread = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ImageDelayCalibration003", __FILE__, __FUNCSIG__); }
}

void ImageDelayCalibration::StartImageDelayCalibration(double DroChangeValue, double ImageAverageChange)
{
	try
	{	
		AverageImageChange = ImageAverageChange;
		DroChange = DroChangeValue;
		this->ContinueThread = true;
		_beginthread(&CalculateImageDelay, 0, NULL);	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ImageDelayCalibration004", __FILE__, __FUNCSIG__); }
}

void CalculateImageDelay(void* anything)
{
	try
	{	
		double DroValue = MAINDllOBJECT->getCurrentDRO().getX(), DroCnt = 0, ImageCnt = 0;
		double AvgValue = 0, TmpAvgValue = 0;
		double LeftTop[2] = {(MAINDllOBJECT->getCurrentDRO().getX() - MAINDllOBJECT->getWindow(0).getCam().x) / MAINDllOBJECT->getWindow(0).getUppX(), (MAINDllOBJECT->getWindow(0).getCam().y - MAINDllOBJECT->getCurrentDRO().getY()) / MAINDllOBJECT->getWindow(0).getUppY()};
		HELPEROBJECT->GetAverageImageValue(LeftTop[0] - IMAGEDELAYOBJECT->BoxSize, LeftTop[1] + IMAGEDELAYOBJECT->BoxSize, IMAGEDELAYOBJECT->BoxSize * 2, IMAGEDELAYOBJECT->BoxSize * 2, &TmpAvgValue);
		bool DroFlag = false, ImageFlag = false;
		while(IMAGEDELAYOBJECT->ContinueThread)
		{
			DWORD d1 = GetTickCount();
			if(abs(MAINDllOBJECT->getCurrentDRO().getX() - DroValue) > IMAGEDELAYOBJECT->DroChange && !DroFlag)
			{
				DroCnt = d1;
				DroFlag = true;
			}
			LeftTop[0] = (MAINDllOBJECT->getCurrentDRO().getX() - MAINDllOBJECT->getWindow(0).getCam().x) / MAINDllOBJECT->getWindow(0).getUppX();
			LeftTop[1] = (MAINDllOBJECT->getWindow(0).getCam().y - MAINDllOBJECT->getCurrentDRO().getY()) / MAINDllOBJECT->getWindow(0).getUppY();
			HELPEROBJECT->GetAverageImageValue(LeftTop[0] - IMAGEDELAYOBJECT->BoxSize, LeftTop[1] + IMAGEDELAYOBJECT->BoxSize, IMAGEDELAYOBJECT->BoxSize * 2, IMAGEDELAYOBJECT->BoxSize * 2, &AvgValue);
			DWORD d2 = GetTickCount();
			if(abs(AvgValue - TmpAvgValue) > IMAGEDELAYOBJECT->AverageImageChange && !ImageFlag)
			{
				ImageCnt = d2;
				ImageFlag = true;
			}
			if(ImageFlag && DroFlag)
				IMAGEDELAYOBJECT->ContinueThread = false;
			//IMAGEDELAYOBJECT->WriteValues(d1, MAINDllOBJECT->getCurrentDRO().getX(), d2, AvgValue);
		}
		if(ImageFlag && DroFlag)
			IMAGEDELAYOBJECT->RaiseImageDelayEvent(abs(ImageCnt - DroCnt), true);
		else
			IMAGEDELAYOBJECT->RaiseImageDelayEvent(0, false);
		_endthread();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ImageDelayCalibration005", __FILE__, __FUNCSIG__); _endthread();}
}

void ImageDelayCalibration::ResetCalibrationFlag()
{
	try
	{
		//IMAGEDELAYOBJECT->ContinueThread = false;
		//MAINDllOBJECT->DrawRectanle_ImageDelay = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ImageDelayCalibration006", __FILE__, __FUNCSIG__); }
}

void ImageDelayCalibration::WriteValues(double DroCnt, double DroValue, double ImageCnt, double ImageValue)
{
	try
	{
		std::wofstream myfile;
		myfile.open("D:\\Image_Dro.csv", ios::app);
		if(!myfile) return;
		myfile << DroCnt << "," << DroValue << "," << ImageCnt << "," << ImageValue << endl;
		myfile.close();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ImageDelayCalibration006", __FILE__, __FUNCSIG__); }
}

void ImageDelayCalibration::DrawRectangle()
{
	try
	{
		double cx = MAINDllOBJECT->getWindow(0).getCam().x, cy = MAINDllOBJECT->getWindow(0).getCam().y;
		double Uppx = MAINDllOBJECT->getWindow(0).getUppX(), Uppy = MAINDllOBJECT->getWindow(0).getUppY();
		double point1[2] = {MAINDllOBJECT->getCurrentDRO().getX() - BoxSize * Uppx, MAINDllOBJECT->getCurrentDRO().getY() + BoxSize * Uppx}, point2[2] = { MAINDllOBJECT->getCurrentDRO().getX() + BoxSize * Uppx, MAINDllOBJECT->getCurrentDRO().getY() - BoxSize * Uppx};
		GRAFIX->SetColor_Double(0.4f, 0.4f, 0.4f);
		GRAFIX->drawRectangle(point1[0] + Uppx, point1[1] - Uppx , point2[0] - Uppx, point2[1] + Uppx, true);
		GRAFIX->translateMatrix(0.0, 0.0, 2.0);
		GRAFIX->SetColor_Double(1, 0, 0);
		GRAFIX->drawRectangle(point1[0], point1[1], point2[0], point2[1]);
		GRAFIX->translateMatrix(0.0, 0.0, -2.0);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ImageDelayCalibration007", __FILE__, __FUNCSIG__); }
}
#pragma once
#include "..\MainDLL.h"
#include <map>
#include <list>

#define IMAGEDELAYOBJECT ImageDelayCalibration::GetImgDelayInstance()

class MAINDLL_API ImageDelayCalibration
{
	double BoxSize, AverageImageChange, DroChange;
	ImageDelayCalibration();
	~ImageDelayCalibration();
	static ImageDelayCalibration* _Instance;
	bool ContinueThread;
	void WriteValues(double DroCnt, double DroValue, double ImageCnt, double ImageValue);
public:
	
	static bool CheckImgDelayInstance();
	static ImageDelayCalibration* GetImgDelayInstance();
	static void DeleteInstance();
	void ClearAll();
	void StartImageDelayCalibration(double DroChangeValue, double ImageAverageChange);
	friend void CalculateImageDelay(void* anything);
	void ResetCalibrationFlag();
	void (CALLBACK *RaiseImageDelayEvent)(double Delay, bool Sucess);
	void DrawRectangle();
};


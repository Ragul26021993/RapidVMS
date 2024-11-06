#pragma once
namespace RWrapper
{
	public ref class RW_ImageDelayCalibration
	{
		RW_ImageDelayCalibration();
		~RW_ImageDelayCalibration();
		static RW_ImageDelayCalibration^ _ImageDelayInstance;
	public:
		delegate void ImageDelayEventHandler(double Delay, bool CalibrationSucess);
		event ImageDelayEventHandler^ ImageDelayEvent;
		static bool Check_ImageDelay_Instance();
		static void Close_ImageDelayWindow();		
		static RW_ImageDelayCalibration^ MYINSTANCE();
		void ClearAll();
		void StartImageDelayCalibration(double Distance, double speed, double DroChange, double ImageAverageChange);
		void StopImageCalibration();
		void RaiseImageDelayEvent(double Delay, bool Sucess);
	};
}


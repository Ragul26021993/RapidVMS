#pragma once
namespace RWrapper
{
	public ref class RW_RepeatAutoMeasure
	{
		RW_RepeatAutoMeasure();
		~RW_RepeatAutoMeasure();
		static RW_RepeatAutoMeasure^ AutoMeasureInstance;
	public:
		delegate void RepeatAutoMeasureEventHandler(int Id);

		event RepeatAutoMeasureEventHandler^ AutoMeasureEvent;

		static bool Check_RepeatAutoMeasure_Instance();
		static void Close_AutoMeasureWindow();		
		static RW_RepeatAutoMeasure^ MYINSTANCE();
		void SetAutomatedMode(bool flag);
		void takePointOfRotation(bool flag);
		void StartAutoMeasurement(double Angle, int Steps, bool AutoFocusEnable);
		void Continue_PauseAutoMeasure(bool PauseFlag);
		void StopAutoMeasure();
		void ClearAll();
		void HandleCallBack(int Id);
	};
}


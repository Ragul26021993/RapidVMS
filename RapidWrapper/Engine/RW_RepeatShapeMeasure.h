#pragma once
namespace RWrapper
{
	public ref class RW_RepeatShapeMeasure
	{
		RW_RepeatShapeMeasure();
		~RW_RepeatShapeMeasure();
		static RW_RepeatShapeMeasure^ _RepeatShapeMeasureInstance;
	public:
		delegate void RepeatShapeMeasureEventHandler(int Id);
		event RepeatShapeMeasureEventHandler^ ShapeMeasureEvent;
		static bool Check_RepeatShapeMeasure_Instance();
		static void Close_RepeatShapeMeasureWindow();		
		static RW_RepeatShapeMeasure^ MYINSTANCE();
		void StartRecording(bool Flag);
		void ClearAll();
		void setCoordinateListForAutoShapemeasure(System::Collections::Generic::List<System::Double>^ listcoord);
		void CreateNextGrid();
		void RW_SetOrientationOfComponent();
		void HandleCallBack(int Id);
	};
}


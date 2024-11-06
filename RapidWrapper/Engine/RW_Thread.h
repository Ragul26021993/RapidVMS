#pragma once
namespace RWrapper 
{
public ref class RW_Thread
{
public:
	delegate void ThreadEventHandler();
private:	 
	 static RW_Thread^ ThreadInstance;
	
public:
	 System::String ^ThreadMajorDia, ^ThreadMinorDia, ^ThreadEffDiaG;
	 System::String ^ThreadEffDiaM, ^ThreadPitch, ^ThreadAngle;
	 System::String ^ThreadTopRadius, ^ThreadRootRadius, ^ThreadPitchDia;
	 int CurrentMeasureThCount;
	 bool MajorMinorDiaMode;

	 System::Data::DataTable^ ThreadMeasureTable;

	 event ThreadEventHandler^ DisableMajorMinorDiaControl;
	 event ThreadEventHandler^ ThreadMeasureSelectionChanged;
public:
	 RW_Thread();
	 ~RW_Thread();
	 static RW_Thread^ MYINSTANCE();

	 void IncludeMajorMinorDia(bool flag);
	 void ExternalThread(bool flag);
	 void NewClicked(bool TopSurfaceStatus, bool RootSurfaceStatus);
	 void UpdateThreadCountChange(int ThCount, bool TopSurfaceCurrenStatus, bool RootSurfaceCurrentStatus);
	 void UpdateThreadGraphics();
	 bool CalculateThreadMeasurement();
	 void GetSelectedThreadParameters();

	  //RW_Thread Panel Mouse events handling...
	 void OnThreadLeftMouseDown(double x, double y); 
	 void OnThreadRightMouseDown(double x, double y); 
	 void OnThreadMiddleMouseDown(double x, double y); 
	 void OnThreadLeftMouseUp(double x, double y); 
	 void OnThreadRightMouseUp(double x, double y); 
	 void OnThreadMiddleMouseUp(double x, double y); 
	 void OnThreadMouseMove(double x, double y);
	 void OnThreadMouseWheel(int evalue);
	 void OnThreadMouseEnter();
	 void OnThreadMouseLeave();
};
}

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
#pragma once

namespace RWrapper 
{
public ref class RW_FocusDepth
{
	public:
		delegate void FocusButtonClickEventHandler(System::String^ btntext);
		delegate void FocusEventHandler();

	private:
		int SelectedAxis;	
		//Focusdepth instance...//
		static RW_FocusDepth^ focusDepthInstance;
		//used in Focus Calibration..
		cli::array<double>^ FocusStartPosition;
		System::Collections::ArrayList^ AllFocusMeasurementValues;
	
		double Last_row, Last_Column, Last_width, Last_height, Last_gap, Last_fspeed, Last_fspan;
	public:
		int FramescanCount, FocusdepthStepCnt;
		bool AutoFocusMode, FillFocusBox, Focusflag;
		double focusSpan, focusSpeed, FocusValue, MoveX, MoveY, MoveZ;
		bool FocusClickEventRaised;
		cli::array<double>^ NextCellPosTemp;

		//Expose Focus Values..
		System::String ^CurrentFocusValue, ^CurrentDepthValue;
		System::String ^CurrentDepthZ1Value, ^CurrentDepthZ2Value;
	
		event FocusButtonClickEventHandler^ GenerateFocusBtnClick;
		event FocusEventHandler^ FocusValueChanged;
		event FocusEventHandler^ FocusFinished;
		event FocusEventHandler^ DepthShapeAdded;
		event FocusEventHandler^ DepthShapeRemoved;
		event FocusEventHandler^ UpdateZ1DepthValue;
		event FocusEventHandler^ UpdateZ2DepthValue;
		event FocusEventHandler^ UpdateDepthValues;

	public:
		RW_FocusDepth();
		~RW_FocusDepth();
	
		int Zx1 , Zx2 , Zy1 , Zy2 , DZx , DZy;
		//Focus depth instance..//
		static RW_FocusDepth^ MYINSTANCE();

		//Focus button click handling..//
		void FocusClicked();
		void StartFocus();
		void AbortClicked();
		void ShowHideFocus(bool flag);
		void NewClicked();
		void SetZ1Clicked();
		void SetZ2Clicked();	
		//Get the focus setting paremters when Set is clicked on Focus setting panel...///
		void SetFocusRectangleProperties(double row, double column, double gap, double width, double height, double span, double speed);
		//Set and Get the selected Axis...
		void SetSelectedAxis(int i);
		int GetSelectedAxis();
		void FillFocusValueInFrontend(double fvalue);

		void Update_FocusDepthValues();
		void FocusResetClicked();
		bool DepthResetClicked();
		void SelectDepthDatum(int Depthid);
		
		void RaiseFocusEvents(int Cnt);
	
		void DoFocusCalibration(double MoveX, double MoveY, double moveZ, int rowNo, int colNo, int gap, int width, int height, double span, double speed);
		void FocusCalibration_NextRegion();
		void UpdateFocusCalibration();

		bool Start_AutoFocusScan();
		void Continue_AutoFocusScan(bool pauseflag);
		void Stop_AutoFocusScan();
		void Rotate_AutoFocusScan(double angleofrotation, int step);
		bool Start_AutoContourScan();
		void Continue_AutoContourScan(bool pauseflag);
		void Stop_AutoContourScan();
		void Rotate_AutoContourScan(double angleofrotation, int step);
		void AFS_GotoNextScanPosition();
		void AFS_ScanCurrentPosition(bool Startfocus);

		void SetShapeForDepth(int i);
		//void SetPositionForRefDot();
	};
}

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!

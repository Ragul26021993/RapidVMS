//Class Used for FixtureCalibration..
#pragma once
namespace RWrapper
{
	public ref class RW_FixtureCalibration
	{
		double ReferenceAngle, FixtureOrientationAngle;
		cli::array<double> ^previousDroValue;// , ^PointOfRotation;
		bool firstTime;
		static RW_FixtureCalibration^ FixtureCalInstance; //hold the instance..
		System::Collections::Generic::List<System::Double>^ CellPosition;
		void writeCorrectionValues();
		int CellCount, listCount;

	public:
		int TotalRows, TotalColumns;
		double CellWidth, CellHeight;
		bool FixtureAutomatedCallibration, PointOfRotationFlag;
		bool FixtureCalibValues_Loaded;
		System::Collections::Generic::List<System::Double>^ CorrectionList;
		System::String^ CurrentFixureName;
		cli::array<double> ^PointOfRotation;

		RW_FixtureCalibration(); //Default constructor..
		~RW_FixtureCalibration(); //Destructor.. Clear the memory..
		static RW_FixtureCalibration^ MYINSTANCE(); //Expose the class instance..
		static bool Check_CallibrationInstance();
		static void Close_CallibrationWindow();

		delegate void Fixture_ReferencePointTakenEventHandler(int i);
		event Fixture_ReferencePointTakenEventHandler^ Fixture_RferencePointEvent;

		void StartFixtureCallibration();
		void getFixtureName(System::String^ FixtureName);
		void ClearAll();

		void SetFixtureCallibrationMode();
		void StartAutomatedFixtureCallibration(bool IncrementCellCount);
		void Create_New_Framgrab(bool firstFramgrab);
		void get_Shape_Parameter(bool firstShape);
		void get_FramGrab_Parameter();
		void TakeReferencePoint_FixtureCallibration();
		void ReadCorrectionValues();
		void UpdateFixureDetails_Frontend(System::String^ name, int rcount, int ccount, double rgap, double cgap);
	};
}

//Created by Rahul Singh Bhadauria.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
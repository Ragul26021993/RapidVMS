//This class is used for database interface
//Read the values from the data base.. button list, names, images etc..//
//All constants, i,e. from settings file...//
#pragma once

namespace RWrapper
{
public ref class RW_DBSettings
{
private:
	//This class object..//
	static RW_DBSettings^ rcadSettings;
	//Data base connection initializations
    System::Data::OleDb::OleDbConnection^ con;
    System::Data::OleDb::OleDbCommand ^cmd, ^_toolStripButtonToolTipCommand, ^_toolStripButtonImageCommand;
	//data set and data adapters...
	System::Data::DataSet^ _toolStripButtonSet;
	System::Data::OleDb::OleDbDataAdapter^ _toolStripButtonAdapter;
	System::Data::OleDb::OleDbCommandBuilder^ _toolStripCommandBuilder;
	//Data reader..//
    System::Data::OleDb::OleDbDataReader ^reader, ^imageReader, ^_toolButtonReader;

	//button type, tooltip and other properties
	System::Collections::Generic::List<System::Int32>^ _Buttontype;
	System::Collections::Generic::List<System::Boolean>^ _ButtonCheckedProperty;
	System::Collections::Generic::List<System::String^> ^_ButtonBaseName, ^_ButtonToolTip;
	System::Collections::Generic::List<System::Windows::Controls::Image^>^ _ButtonImage;

	//Pixel width and magnification levels...//
	System::Collections::Generic::List<System::String^>^ MagNameList;
	System::Collections::Generic::List<System::Double> ^R_DistList, ^R_PixelWidth;
	System::Collections::Generic::List<System::Int32> ^R_ZoomLevel;
	System::Collections::Hashtable^ PixelWidthList_X;
	System::Collections::Hashtable^ PixelWidthList_Y;
	System::Collections::Hashtable^ DigitalZoomPixel_X;
	System::Collections::Hashtable^ DigitalZoomPixel_Y;
	//Eleven X Light settings..
	System::Collections::Hashtable^ ElvnXProfileMode;
	
	//RW_DRO Initilize Variables.............
	System::String^ droLabels;
	double *lashValue, *axisMultiplier, *countsPerunit;
	int *movementType, *movementDirection;
	cli::array<unsigned char>^ rawdata;
	System::String^ LangaugeType;
	int LangCid;
	double R_Upper_Correction, R_Lower_Correction, R_Last_Value;

	void GetSavedLightSettings();
	void DeleteOldCalibrationSettings();
	void DeleteOldFocusCalibrationSettings();
	void createToolButtonInterface();
	void GetValuesFromDatabase(bool InitialCall);
	
	System::Windows::Forms::Timer^ MyTimer;
	void RestoreTheCrash(System::Object^ sender, System::EventArgs^ e);

public:
	//Program Path Settings..
	System::String ^ProgramPath, ^ReportPath, ^TemplatesPath, ^DROStyle;

	//Probe Settings...
	double ProbeStopValue, ProbeSpeed;

	//RW_CNC Speed Settings..//
	double SpeedX, SpeedY, SpeedZ, SpeedR, SpeedMinValue, SpeedMaxValue, SpeedStepValue;

	//Grid Programming Settings..
	bool GridProgramModeFlag, RotartAxisPresent , Gridrerun;
	//int GridRowcnt, GridColumnCnt;
	//double GridRowgap, GridColumngap;
	//System::String^ CurrentFixureName;
	bool PartialmageModeForOneShot, CheckForTargetReach;
	int MachineTypeEnum, MeasureNoOfDec;
	System::String ^Camera_Type, ^CurrentMachineNumber;
	
	//Customer Details..
	System::String ^CompanyName, ^CompanyLocation, ^SoftwareVersion, ^MachineType, ^DefaultMagnificationLevel, ^ButtonToolTip;

	//Light Settings...
	bool AuxilaryLightPresent, CoAxialLightPresent, ElevenXProfilePresent;
	bool FiveXLensePresent, ThreeDSoftwareMode, AutoZoomInOutMode;
	System::Collections::Generic::List<System::String^>^ SavedLightSettings_NameList;
	//No of lighting card... Lighting zone...//
	int NoOfCards, NoofMachineAxis;
	int Profile_Zone_No, Profile11X_Zone_No, Aux_Zone_No, CoAx_ZoneNo;
	int ProfileLightLevelFor11X;

	System::Collections::Hashtable ^GradeValueCollection, ^GradeLevelCollection, ^GradeValueEndCollection;
	System::Collections::Hashtable ^Grade1ValueCollection, ^Grade1ValueEndCollection, ^Grade1LevelCollection, ^Grade2ValueEndCollection, ^Grade2ValueCollection, ^Grade2LevelCollection;
	double FirstGradeValue, LastGradevalue;//, GradeStep;
	double RefDotDia_in_mm;
	
public:
	RW_DBSettings();
	~RW_DBSettings();
	static RW_DBSettings^ MYINSTANCE();

	//Initializes the database and reads the values from database...//
	void InitializeRcadSettings();

	//Light Settings..
	void SaveLightSettings(System::String^ LName, cli::array<int, 1>^ lightvalue);
	void UpdateSelectedLightValue(System::String^ LName);
	void DeleteSelectedLightValue(System::String^ LName);
	void UpdateProbeApproachGap_Speed(double ApproachGap, double Speed);
	//void UpdateFixureDetails_Frontend(System::String^ name, int rcount, int ccount, double rgap, double cgap);

	//Function to return the single Image
	System::Windows::Controls::Image^ getOneImage_RunTime(System::String^ baseName, System::String^ toolAlignment, int width, int height);
	//Get the cursor stream from databse... this stream is converted into cursor(icon image)....
	System::IO::Stream^ GetCursor(System::String^ IconName, System::String^ toolAlignment);
	System::Collections::Generic::List<System::Windows::Controls::Image^>^ getImageList(System::String^ toolAlignment, int width, int height);
	System::Collections::Generic::List<System::String^>^ getToolTipList();
	System::Collections::Generic::List<System::String^>^ getButtonBaseNameList();
	System::Collections::Generic::List<System::Int32>^ getButtonTypeList();
	System::Collections::Generic::List<System::Boolean>^ getButtonCheckedPropertyList();
	System::Collections::Generic::List<System::String^>^ GetMagnificationList();
	//Update the magnification (pixel width) for magfication level...//
	void UpdateVideoPixelWidthForDigitalZoom(System::String^ _magLevel, bool CurrentState);
    void UpdateVideoPixelWidth(System::String^ _magLevel);
	void UpdateVideoUpp_DROCallback(double currRvalue);
	void UpdateCameraZoomLevel(System::String^ _magLevel);
	
	bool GetRValueFromPixelWidth(double* Rcount, double _pixelWidth);
	bool GetRValueFromZoomLevel(double* Rcount, int zoomLevel);
	bool GetPixelWidthFromRValue(double Rcount, double *_pixelWidth);
	bool GetZoomLevelFromRValue(double Rcount, int *zoomLevel);
	void UpdateCalibrationSettings_CurrentMachine(System::Collections::ArrayList^ MeasureValues, int TotalCell);
	void UpdateFocusCalibrationSettings_CurrentMachine(System::Collections::ArrayList^ MeasureValues, int TotalCell);
	void UpdateSphereCalibrationSettings_CurrentMachine(int probetype, int CurrentSide, double ProbeRadius, double *CalibratedOffset);
	void UpdateFocusCalibrationGridSettings_CurrentMachine(System::Collections::ArrayList^ CorrectionValues);
	void UpdateCrosshairOffsets();
	void UpdateCalibrationSettings();

	void UpdateSurfaceFGSettings();
	void UpdateGradeSelection(int Index);
	//Clear the databae connection and dispose the object...!
	void FinaliseAll();	
	void SetCameraToCompleteZoom_Exeload();
};
}

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
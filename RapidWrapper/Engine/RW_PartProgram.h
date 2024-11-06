#pragma once

namespace RWrapper
{
public ref class RW_PartProgram
 {
 public:
	 delegate void PartprogramEventHandler();
	 delegate void MeasureUnitEventHandler(System::String^ Str);
	 //delegate for measurement related event handling.......................
	 delegate void PP_ParameterResetEventHandler(RWrapper::RW_Enum::PROGRAM_PARAMETER type);
	 delegate void PPImageEventHandler(int NoOfImages);
	 delegate void PPRunCount(System::String^ Msg);
	 delegate void ChangeZoom(System::String^ ZoomText);

private:
	static RW_PartProgram^ PPInstance;//Partprogram instance...//
	System::String^ ImageFilePath;
	System::String^ BuildFilePath;
	System::String^ LoadedFilePath;
	//Variables used for Grid program....!!
	int MeasurementCount, RowCount, ColumnCount ,CurrentCount;
	//System::Collections::Generic::List<System::Double>^ CorrectionValue;
	double FixtureReferenceAngle;
	void LoadProgram(System::String^ FilePath);
	//void ReadCorrectionValueForGrid();
	bool CorrectRotationalError, SaveReferencePtMode, OneShot_RefImage_Loaded; //PointOfRotationFlag
	int PL_Offset, SF_Offset;
	
public:
	int NumberOfCommandSend, InterPolationCommandCount;
	bool RunningFailedAction, ReferenceModeF, ReturnTohomePosition, PartProgramPaused;
	bool ProgramMode, AutoFocusDuringPartprogram, RunStepsOneByOne, BatchOfCommandCompleted;
	System::Data::DataTable^ ProgramStepTable;
	double *HomePosition;
	System::Collections::Generic::List<System::Int32>^ FailedActionIdList;
	bool SingleComponentinOneShot;
	int SameCommand;
	bool CNC_Assisted_Mode; // Flag to store and retreive actual CNC Assisted mode during PP run

	event PartprogramEventHandler^ UpdateProgramStep;
	event PartprogramEventHandler^ RuntheProgramAgain;
	event PartprogramEventHandler^ RunIsnotValid;
	event PartprogramEventHandler^ GenerateProgramReport;
	event PartprogramEventHandler^ BuildProgramCompleted;
	event PPImageEventHandler^ MoveImageEvent;
	event MeasureUnitEventHandler^ UpdateMeasureUnit;
	event PP_ParameterResetEventHandler^ PP_ParamResetEvent;
	event PPRunCount^ GridRunCountEvent;
	event ChangeZoom^ ChangeZoomSelection;

 public:
	 RW_PartProgram();
	 ~RW_PartProgram();
	 static RW_PartProgram^ MYINSTANCE();
	
	 void SaveCurrentDROValue();
	 void UpdateProgramStepsTable();
	 void UpdateProgramStepsTableForEdit();
	 void UpdateCurrentPPStepStatus(int Fgid, int PointCnt, bool status);
	
	 void PartProgram_EditAlign(System::String^ BtnName);
	 bool PartProgram_Build(System::String^ FilePath);
	 void PartProgram_ContinueBuild(bool same_thread);
	 void PartProgram_Load(System::String^ FilePath);
	 void PartProgram_Edit(System::String^ FilePath);
	 void PartProgram_Run();
	 void PartProgram_GridRun();
	 void PartProgram_Pause();
	 void PartProgram_Continue();
	 void PartProgram_Stop();
	 void PartProgram_HomePosition();
	 bool OneShotpartprogramRun();
	 void Handle_Program_btnClicks(System::String^ Str);


	 void StartPartProgramRun();
	 void HandlePartprogram();
	 int SendCurrentStepLightCommand(char* LightProperty);
	 void CalculateAndMovetoPosition(int SpecialStep);
	 void TakeProbePoint();
	 void PartProgramFinished();
	 void ResetGridProgramCount();
	 void Rerun_Program();
	 void Abort_PartProgram();
	 System::String^ GetFeatureLIst();
	 void ContinuePartProgram_AfterUserInteraction();
	 void RaisePP_ParamResetEvent(RWrapper::RW_Enum::PROGRAM_PARAMETER type);
	 void SetToleranceValueForDeviation(double Uppertolerance, double Lowertolerance);
	 void SetOffsetForRelativePoint(double offset_X, double offset_Y, double offset_Z);
	 void RunFailedAction();
	 void GetActionListOfOutTolMeasure();
	 void SetPointsDirectoryPath(System::String^ PointsDirectPath); 
	 bool SendCommandInBatch();
	 void SaveImageRectangleExtent();
	 bool GetDelphiPPSuccessStatus();
	 bool LoadReferecneImage_OneShot(System::String^ FileName);
	 bool SetLightChangeValue(bool ForSurface, int changeValue);
	};
}

//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!

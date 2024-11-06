//Include the required header files..
#include "Stdafx.h"
#include "RW_MainInterface.h"
#include "..\MAINDll\FixedTool\CalibrationText.h"
#include "..\MAINDll\Engine\CalibrationCalculation.h"
#include "..\MAINDll\Helper\Helper.h"
#include "..\MAINDll\Actions\LocalisedCorrectionAction.h"
#define _USE_MATH_DEFINES
#include <math.h>
int RepeatCntDup;


void CALLBACK GoToPrevious(RAction* preAtn)
{
	try
	{			
		RWrapper::RW_AutoCalibration::MYINSTANCE()->GoToPreviousAction(preAtn);
	}
	catch(Exception^ ex){ }
}

void CALLBACK GoToNext(RAction* curAtn, bool redo)
{
	try
	{			
		RWrapper::RW_AutoCalibration::MYINSTANCE()->GoToNextAction(curAtn, redo);
	}
	catch(Exception^ ex){ }
}

//Default constructor.. Initialise the deafault settings..
RWrapper::RW_AutoCalibration::RW_AutoCalibration()
{
	 try
	 {
		this->AutoCalInstance = this;
		this->StartedRunning = false;
		this->MachineTypeColl = gcnew System::Collections::Generic::List<System::String^>;
		this->SlipGaugeTypeColl = gcnew System::Collections::Generic::List<System::String^>;
		this->MachineTypeColl->Add("40X20"); this->MachineTypeColl->Add("20X15");
		this->MachineTypeColl->Add("R15X15"); this->MachineTypeColl->Add("OneShot");

		this->SlipGaugeTypeColl->Add("1mm"); this->SlipGaugeTypeColl->Add("2mm");
		this->SlipGaugeTypeColl->Add("3mm"); this->SlipGaugeTypeColl->Add("4mm");
		this->SlipGaugeTypeColl->Add("5mm"); this->SlipGaugeTypeColl->Add("10mm"); 
		this->SlipGaugeTypeColl->Add("20mm"); this->SlipGaugeTypeColl->Add("25mm"); 
		this->SlipGaugeTypeColl->Add("30mm"); this->SlipGaugeTypeColl->Add("40mm");  this->SlipGaugeTypeColl->Add("50mm");
		this->SlipGaugeTypeColl->Add("70mm"); this->SlipGaugeTypeColl->Add("100mm");
		this->SlipGaugeTypeColl->Add("150mm"); this->SlipGaugeTypeColl->Add("200mm");
		this->SlipGaugeTypeColl->Add("350mm");// this->SlipGaugeTypeColl->Add("100mm");

		this->AllMeasurementValues = gcnew System::Collections::ArrayList();
		this->RowCount = 0; this->ColumnCount = 0; this->MeasurementCount = 0;
		this->CurrentLineGrabCount = 0; this->LinesAddedForCurrentMeasure = 0;
		this->TotalRows = 0; this->TotalColumns = 0; 
		this->SlipgaugeLength = 0; this->MachineWidthX = 0; this->MachineWidthY = 0;
		this->MachinetypeSetFlag = false; this->SlipGaugeWidthSetFlag = false;
		this->CalculateBothWidthMeasurement = false;
		this->CurrentLType = CURRENTLINETYPE::NOTHING;
		this->SlipGaugeWidth_Frame = 28;
		this->WidthStartDROValue = gcnew cli::array<double>(4);
		this->MeasurementAsMedian = true;
		this->ShiftLength =  1;
		this->OverlapWidth = 5;
		MAINDllOBJECT->GoToPreviousPosition = &GoToPrevious;
		MAINDllOBJECT->GoToNextPosition = &GoToNext;
		CurrentAction = NULL;
	 }
	 catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0001", ex); }
}

//Destructor.. Release the memory..
RWrapper::RW_AutoCalibration::~RW_AutoCalibration()
{
	try
	{
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0002", ex); }
}

//Exposes this class instance.. the Application no need to hold the object..
RWrapper::RW_AutoCalibration^ RWrapper::RW_AutoCalibration::MYINSTANCE()
{
	return AutoCalInstance;
}


//Reset the RW_DRO .. In Hardware!
void RWrapper::RW_AutoCalibration::Handle_ResetDROClicked()
{
	try
	{
		DROInstance->ToReset();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0004", ex); }
}

//Set the current machine type.. Take the width X and Y w.r.to the Machine type..
void RWrapper::RW_AutoCalibration::Handle_SetMachineType(System::String^ Mtype)
{
	try
	{
		CALBCALCOBJECT->OneShotMode(false);
		if(Mtype == "40X20")
		{
			MachineWidthX = 390;
			MachineWidthY = 190;
		}
		else if(Mtype == "20X15")
		{
			MachineWidthX = 190;
			MachineWidthY = 140;
		}
		else if(Mtype == "R15X15")
		{
			MachineWidthX = 140;
			MachineWidthY = 140;
		}
		else if(Mtype == "OneShot")
		{
			CALBCALCOBJECT->OneShotMode(true);
			MachineWidthX = 60;
			MachineWidthY = 40;
		}
		MachineType = Mtype;
		MachinetypeSetFlag = true;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0005", ex); }
}

//Set the slipgauge length... According to the length set the next cell gap..
void RWrapper::RW_AutoCalibration::Handle_SetSlipGaugeWidth(System::String^ Swidth)
{
	try
	{
		if (Swidth->EndsWith("mm"))
			SlipgaugeLength = Convert::ToDouble(Swidth->Substring(0, Swidth->Length - 2));
		else
		{
			try
			{
				SlipgaugeLength = double::Parse(Swidth);
			}
			catch (Exception^ ex)
			{
				RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0006", ex);
				SlipGaugeWidthSetFlag = false;
				return;
			}
		}
		//if(Swidth == "5mm")
		//	SlipgaugeLength = 5;
		//else if(Swidth == "10mm")
		//	SlipgaugeLength = 10;
		//else if(Swidth == "20mm")
		//	SlipgaugeLength = 20;
		//else if(Swidth == "25mm")
		//	SlipgaugeLength = 25;
		//else if(Swidth == "30mm")
		//	SlipgaugeLength = 30;
		//else if(Swidth == "50mm")
		//	SlipgaugeLength = 50;
		//else if(Swidth == "70mm")
		//	SlipgaugeLength = 70;
		//else if(Swidth == "100mm")
		//	SlipgaugeLength = 100;
		NextCellGap = SlipgaugeLength - OverlapWidth;
		//NextCellGap = SlipgaugeLength - 5; 
		if(NextCellGap < OverlapWidth) NextCellGap = OverlapWidth;
		CALBCALCOBJECT->SetSlipGaugeWidth(SlipgaugeLength);
		/*if(Swidth == "10mm")
		   CALBCALCOBJECT->Width(20);
		else
		   CALBCALCOBJECT->Width(SlipgaugeLength);
		CALBCALCOBJECT->Length(SlipgaugeLength); */
		
		SlipGaugeWidthSetFlag = true;
		
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0006", ex); }
}

void RWrapper::RW_AutoCalibration::Handle_CheckBoxChanged(bool flag)
{
	CalculateBothWidthMeasurement = flag;
}

//Handle the run clicked...
void RWrapper::RW_AutoCalibration::Handle_RunClicked()
{
	try
	{
		//check if current ucs is mcs or not..... 
		if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode() && MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::ONE_SHOT)
		{
			MAINDllOBJECT->ShowMsgBoxString("RW_AutoCalibrationHandle_RunClicked01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			ResetButtonState::raise();
			return;
		}
		if(!MachinetypeSetFlag && !SlipGaugeWidthSetFlag)
		{
			MAINDllOBJECT->ShowMsgBoxString("RW_AutoCalibrationHandle_RunClicked02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			ResetButtonState::raise();
		}
		else if(!MachinetypeSetFlag)
		{
			MAINDllOBJECT->ShowMsgBoxString("RW_AutoCalibrationHandle_RunClicked03", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			ResetButtonState::raise();
		}
		else if(!SlipGaugeWidthSetFlag)
		{
			MAINDllOBJECT->ShowMsgBoxString("RW_AutoCalibrationHandle_RunClicked04", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			ResetButtonState::raise();
		}
		else
		{
			CALBCALCOBJECT->ClearAllCalibrationCalcualtions();
			RWrapper::RW_DRO::MYINSTANCE()->AutoCalibrationMode = true;
			this->StartedRunning = true;
			DisableButtonState::raise();
			TotalRows = MachineWidthY/NextCellGap;
			TotalColumns = MachineWidthX/NextCellGap;
			if(MachineType == "OneShot")
			{
				TotalRows -= 1;
				TotalColumns -= 1;
			}
			CALBCALCOBJECT->SetNofRowColumns(TotalRows, TotalColumns);
			CurrentAction = CALBCALCOBJECT->GetLastAction();
			if(CurrentAction == NULL)
			{
				CurrentLType = CURRENTLINETYPE::VERTICAL_LINE;
				CALBCALCOBJECT->CalibrationModeFlag(true);
				if(MachineType == "OneShot")
					CALBCALCOBJECT->SetCurrentMeasurementOrientation((int)CurrentLType, RowCount, ColumnCount, NextCellGap);
				else
					CALBCALCOBJECT->SetCurrentMeasurementOrientation((int)CurrentLType);
				MAINDllOBJECT->ShowMsgBoxString("RW_AutoCalibrationHandle_RunClicked05", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			}
			else
			{
				CALBCALCOBJECT->CalibrationModeFlag(true);
				GoToNextAction(CurrentAction, false);
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0007", ex); }
}

//To handle stop clicked..
void RWrapper::RW_AutoCalibration::Handle_StopClicked()
{
	try
	{
		Reset_Status();
		ResetButtonState::raise();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0008", ex); }
}

//Reset all the settings..
void RWrapper::RW_AutoCalibration::Reset_Status()
{
	try
	{
		RWrapper::RW_DRO::MYINSTANCE()->AutoCalibrationMode = false;
		this->StartedRunning = false;
		CALBCALCOBJECT->CalibrationModeFlag(false);
		CALBCALCOBJECT->ClearLocalisedAction();
		CALBCALCOBJECT->SetCurrentMeasurementOrientation(3);
		RowCount = 0; ColumnCount = 0; MeasurementCount = 0;
		CurrentLineGrabCount = 0; LinesAddedForCurrentMeasure = 0;
		MAINDllOBJECT->update_VideoGraphics();
		CurrentAction = NULL;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0009", ex); }
}

//Measure the current width.. According to the orientation measure the width..
void RWrapper::RW_AutoCalibration::Handle_MeasureWidthClicked()
{
	try
	{
		if(!StartedRunning)
		{
			MAINDllOBJECT->ShowMsgBoxString("RW_AutoCalibrationHandle_MeasureWidthClicked01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			return;
		}
		if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode() && MachineType != "OneShot")
		{
			MAINDllOBJECT->ShowMsgBoxString("RW_AutoCalibrationHandle_MeasureWidthClicked02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			return;
		}
		CurrentAction = new LocalisedCorrectionAction(&RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], (int)CurrentLType);
		if(MachineType == "OneShot")
			CALBCALCOBJECT->AddFirstLine();
		else
			CALBCALCOBJECT->AddFirstLine(false);
		CurrentLineGrabCount = 0;
		LinesAddedForCurrentMeasure = 1;
		FrameGrabPointsToSelectedLine();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0010", ex); }
}

void RWrapper::RW_AutoCalibration::GoToPreviousAction(RAction *preAtn)
{
	Vector pos = ((LocalisedCorrectionAction*)preAtn)->DROPosition;
	double target[4] = {pos.getX(), pos.getY(), pos.getZ(), pos.getR()};
	double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
	CurrentLType = (CURRENTLINETYPE)((LocalisedCorrectionAction*)preAtn)->side;
	MeasurementCount = ((LocalisedCorrectionAction*)preAtn)->MeasureCnt;
	RowCount = ((LocalisedCorrectionAction*)preAtn)->RowCnt;
	ColumnCount = ((LocalisedCorrectionAction*)preAtn)->ColCnt;
	if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
		target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
	RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::AUTO_CALIBRATIONNEXTORNTATN);
	CALBCALCOBJECT->AddAndRemoveMeasure(false, ((LocalisedCorrectionAction*)preAtn)->measurement);
}

void RWrapper::RW_AutoCalibration::GoToNextAction(RAction *curAtn, bool redo)
{
	MeasurementCount = ((LocalisedCorrectionAction*)curAtn)->MeasureCnt;
	RowCount = ((LocalisedCorrectionAction*)curAtn)->RowCnt;
	ColumnCount = ((LocalisedCorrectionAction*)curAtn)->ColCnt;
	LinesAddedForCurrentMeasure = 2;
	CurrentLineGrabCount = 1;
	if(redo)
	{
		CALBCALCOBJECT->AddAndRemoveMeasure(true, ((LocalisedCorrectionAction*)curAtn)->measurement);
		CurrentAction = (LocalisedCorrectionAction*)curAtn;
		GotoNextStep(false);
	}
	else
	{
		Vector pos = ((LocalisedCorrectionAction*)curAtn)->DROPosition;
		double target[4] = {pos.getX(), pos.getY(), pos.getZ(), pos.getR()};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		CurrentLType = (CURRENTLINETYPE)((LocalisedCorrectionAction*)curAtn)->side;
		if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
			target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::AUTO_CALIBRATIONLASTORN);
	}
}

void RWrapper::RW_AutoCalibration::Handle_MeasureSlipGaugeWidthOnClick(int RepeatCount)
{
	if(RepeatCount > 1)
		MAINDllOBJECT->RepeatSGWidth = true;
		
	RepeatCalibCount = RepeatCount;
	RepeatCntDup = RepeatCount;
	Handle_MeasureSlipGaugeWidthClicked();
}
//Measure the slipgauge width... Automatic..
void RWrapper::RW_AutoCalibration::Handle_MeasureSlipGaugeWidthClicked()
{
	try
	{
		if(!SlipGaugeWidthSetFlag)
		{
			MAINDllOBJECT->ShowMsgBoxString("RW_AutoCalibrationHandle_MeasureSlipGaugeWidthClicked01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			ResetButtonState::raise();
			return;
		}
		MAINDllOBJECT->SetStatusCode("RW_AutoCalibration01");
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1); //Cnc Mode..
		
		if(RepeatCalibCount == RepeatCntDup)
			for(int i = 0; i < 4; i++)
				WidthStartDROValue[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];

		RepeatCalibCount -= 1;
		CALBCALCOBJECT->CheckCurrentSlipgaugeParam();
		CALBCALCOBJECT->AddFirstLine();
		if(CalculateBothWidthMeasurement)
			CALBCALCOBJECT->AddFirstLine_WithoutLocalised();
		CurrentLineGrabCount = 1;
		LinesAddedForCurrentMeasure = 1;
		GotoNextStep_WidthMeasurement();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0010.a", ex); }
}

//Calibrate and update the values in DB..
void RWrapper::RW_AutoCalibration::Handle_CalibrateClicked()
{
	try
	{
		AllMeasurementValues->Clear();
		ColumnCount = 0; RowCount = 0;
		double MeasureMedianValues[200];
		for(int i = 0; i < 200; i++){ MeasureMedianValues[i] = SlipgaugeLength;}
		CALBCALCOBJECT->DotheCalibration(MeasureMedianValues, MeasurementAsMedian);
		int Count = (TotalRows + 1) * (TotalColumns + 1);
		for(int i = 0; i < Count; i++)
		{
			AllMeasurementValues->Add(RWrapper::RW_DBSettings::MYINSTANCE()->CurrentMachineNumber); //  MachineNo.
			AllMeasurementValues->Add(MachineType); //Machine Type..
			AllMeasurementValues->Add(SlipgaugeLength); // SlipGauge Length..
			AllMeasurementValues->Add(NextCellGap); // Cell Gap..
			AllMeasurementValues->Add(TotalRows); // Total no of Rows..
			AllMeasurementValues->Add(TotalColumns); // Total no of columns.
			AllMeasurementValues->Add(CALBCALCOBJECT->CorrectedValues[2 * i]); // Correction in X..
			AllMeasurementValues->Add(CALBCALCOBJECT->CorrectedValues[2 * i + 1]); // Correction in Y..
		}
		RWrapper::RW_DBSettings::MYINSTANCE()->UpdateCalibrationSettings_CurrentMachine(AllMeasurementValues, Count);
		MAINDllOBJECT->ShowMsgBoxString("RW_AutoCalibrationHandle_CalibrateClicked01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
		System::Windows::Forms::SaveFileDialog^ saveFileDialog1 = gcnew System::Windows::Forms::SaveFileDialog();
		saveFileDialog1->Filter = "Csv files (*.csv)|*.csv|All files (*.*)|*.*" ;	//filters the visible files..
		saveFileDialog1->FilterIndex = 1 ;
		if(saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			System::IO::StreamWriter^ tst = gcnew System::IO::StreamWriter(saveFileDialog1->FileName);
			if (MachineType == "OneShot")
			{
				for(int i = 0; i < TotalRows * TotalColumns * 2; i++)
					tst->WriteLine(System::Convert::ToString(MeasureMedianValues[i]) + "," + System::Convert::ToString(CALBCALCOBJECT->CorrectedValues[2 * i]) + "," + System::Convert::ToString(CALBCALCOBJECT->CorrectedValues[2 * i + 1]));
			}
			else
			{
				for(int i = 0; i < TotalRows * TotalColumns * 3; i++)
					tst->WriteLine(System::Convert::ToString(MeasureMedianValues[i]) + "," + System::Convert::ToString(CALBCALCOBJECT->CorrectedValues[2 * i]) + "," + System::Convert::ToString(CALBCALCOBJECT->CorrectedValues[2 * i + 1]));
			}
			tst->Close();
		}
		//Read Localised calibration values..
		RWrapper::RW_DRO::MYINSTANCE()->GetLocalisedCalibrationDetails();
		Sleep(100);
		CALBCALCOBJECT->CreateCorrectedMeasurements();
	}
	catch(Exception^ ex){RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0011", ex); }
}

//Update values from a CSV file..
void RWrapper::RW_AutoCalibration::Handle_DoneClicked()
{
	try
	{
		if(!MachinetypeSetFlag && !SlipGaugeWidthSetFlag)
		{
			MAINDllOBJECT->ShowMsgBoxString("RW_AutoCalibrationHandle_DoneClicked01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			return;
		}
		System::Windows::Forms::OpenFileDialog^ OpnFileDialog = gcnew System::Windows::Forms::OpenFileDialog();
		OpnFileDialog->Filter = "Csv files (*.csv)|*.csv|All files (*.*)|*.*" ;	//filters the visible files..
		OpnFileDialog->FilterIndex = 1;
		if(OpnFileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			System::IO::StreamReader^ tst = gcnew System::IO::StreamReader(OpnFileDialog->FileName);
			double x = 0;
			std::list<double> TempValues;
			while(!tst->EndOfStream)
			{
				x = 0;
				System::String^ Str = tst->ReadLine();
				cli::array<System::String^>^ StrSub = Str->Split(',');			
				x = System::Convert::ToDouble(StrSub[0]);
				TempValues.push_back(x);
			}
			CALBCALCOBJECT->SetSlipGaugeWidth(SlipgaugeLength);
			TotalRows = MachineWidthY/NextCellGap;
			TotalColumns = MachineWidthX/NextCellGap;
			/*TotalRows = 6;
			TotalColumns = 10;*/
			CALBCALCOBJECT->SetNofRowColumns(TotalRows, TotalColumns);
			AllMeasurementValues->Clear();
			ColumnCount = 0; RowCount = 0;
			CALBCALCOBJECT->DotheCalibration_Average(&TempValues);
			int Count = (TotalRows + 1) * (TotalColumns + 1);
			for(int i = 0; i < Count; i++)
			{
				AllMeasurementValues->Add(RWrapper::RW_DBSettings::MYINSTANCE()->CurrentMachineNumber); //  MachineNo.
				AllMeasurementValues->Add(MachineType); //Machine Type..
				AllMeasurementValues->Add(SlipgaugeLength); // SlipGauge Length..
				AllMeasurementValues->Add(NextCellGap); // Cell Gap..
				AllMeasurementValues->Add(TotalRows); // Total no of Rows..
				AllMeasurementValues->Add(TotalColumns); // Total no of columns.
				AllMeasurementValues->Add(CALBCALCOBJECT->CorrectedValues[2 * i]); // Correction in X..
				AllMeasurementValues->Add(CALBCALCOBJECT->CorrectedValues[2 * i + 1]); // Correction in Y..
			}
			RWrapper::RW_DBSettings::MYINSTANCE()->UpdateCalibrationSettings_CurrentMachine(AllMeasurementValues, Count);
			MAINDllOBJECT->ShowMsgBoxString("RW_AutoCalibrationHandle_DoneClicked02", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			
			//double MeasureValues[400] = {0};
			//int Cnt = 0;
			//for each(double val in TempValues)
			//{
			//	MeasureValues[Cnt++] = val;
			//}

			//System::Windows::Forms::SaveFileDialog^ saveFileDialog1 = gcnew System::Windows::Forms::SaveFileDialog();
			//saveFileDialog1->Filter = "Csv files (*.csv)|*.csv|All files (*.*)|*.*" ;	//filters the visible files..
			//saveFileDialog1->FilterIndex = 1 ;
			//if(saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			//{
			//	System::IO::StreamWriter^ tst = gcnew System::IO::StreamWriter(saveFileDialog1->FileName);
			//	if (MachineType == "OneShot")
			//	{
			//		for(int i = 0; i < TotalRows * TotalColumns * 2; i++)
			//			tst->WriteLine(System::Convert::ToString(MeasureValues[i]) + "," + System::Convert::ToString(CALBCALCOBJECT->CorrectedValues[2 * i]) + "," + System::Convert::ToString(CALBCALCOBJECT->CorrectedValues[2 * i + 1]));
			//	}
			//	else
			//	{
			//		for(int i = 0; i < TotalRows * TotalColumns * 3; i++)
			//			tst->WriteLine(System::Convert::ToString(MeasureValues[i]) + "," + System::Convert::ToString(CALBCALCOBJECT->CorrectedValues[2 * i]) + "," + System::Convert::ToString(CALBCALCOBJECT->CorrectedValues[2 * i + 1]));
			//	}
			//	tst->Close();
			//}

			//Read Localised calibration values..
			RWrapper::RW_DRO::MYINSTANCE()->GetLocalisedCalibrationDetails();
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0012", ex); }
}

//Clear everything..
void RWrapper::RW_AutoCalibration::Handle_ClearClicked()
{
	try
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->ClearALL();
		CALIBRATIONTEXT->MeasureArray.clear();
		CALIBRATIONTEXT->MeasurementArray.clear();
		CALIBRATIONTEXT->mousedownDetail= false;
		CALIBRATIONTEXT->mousedownTable= false;
		MAINDllOBJECT->PlaceDetail = false;
		MAINDllOBJECT->PlaceTable = false;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0013", ex); }
}

//Calibration window is closed..
void RWrapper::RW_AutoCalibration::Handle_CloseClicked()
{
	try
	{
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0014", ex); }
}
									 
//Finished all the measurements..
void RWrapper::RW_AutoCalibration::FinishedMeasurement()
{
	try
	{
		MAINDllOBJECT->ShowMsgBoxString("RW_AutoCalibrationFinishedMeasurement01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
		RWrapper::RW_AutoCalibration::MYINSTANCE()->Reset_Status();
		RWrapper::RW_AutoCalibration::MYINSTANCE()->ResetButtonState::raise();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0015", ex); }
}

void RWrapper::RW_AutoCalibration::GotoNextStep_WidthMeasurement()
{
	try
	{
		//CALBCALCOBJECT->CheckCurrentSlipgaugeParam();
		double slipgangle = CALBCALCOBJECT->CurrentSlipGaugeEdgeAngle();
		if (abs(slipgangle) > 4 * M_PI) return;
		double FgDirection = slipgangle + M_PI_2;
		double target[4] = {WidthStartDROValue[0], WidthStartDROValue[1], WidthStartDROValue[2], WidthStartDROValue[3]};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		if(LinesAddedForCurrentMeasure == 1)
		{
			if(CurrentLineGrabCount == 1)
			{				
				target[0] = target[0];
				target[1] = target[1];
			}
			else if(!this->DoSingleFramePerSide)
			{
				target[0] = target[0] + cos(FgDirection) * SlipGaugeWidth_Frame;
				target[1] = target[1] + sin(FgDirection) * SlipGaugeWidth_Frame;
			}
		}
		else
		{
			if(this->DoSingleFramePerSide)
			{				
				if (CurrentLineGrabCount == 1)
				{
					if (abs(sin(slipgangle)) > abs(cos(slipgangle))) //Slip is along X axis // - M_PI_2
						target[1] = target[1] + sin(slipgangle) * SlipgaugeLength;
					else
						target[0] = target[0] + cos(slipgangle) * SlipgaugeLength;
				}
			}
			else if (CurrentLineGrabCount == 1)
			{
				target[0] = target[0] + cos(slipgangle) * SlipgaugeLength;
				target[1] = target[1] + sin(slipgangle) * SlipgaugeLength;
			}
			else// if (!this->DoSingleFramePerSide)
			{
				target[0] = target[0] + cos(slipgangle) * SlipgaugeLength + cos(FgDirection) * SlipGaugeWidth_Frame;
				target[1] = target[1] + sin(slipgangle) * SlipgaugeLength + sin(FgDirection) * SlipGaugeWidth_Frame;
			}
		}

		/*if(LinesAddedForCurrentMeasure == 1)
		{
			if(CurrentLineGrabCount == 1)
			{				
				target[0] = target[0] + cos(FgDirection) * SlipGaugeWidth_Frame/2;
				target[1] = target[1] + sin(FgDirection) * SlipGaugeWidth_Frame/2;
			}
			else
			{
				target[0] = target[0] - cos(FgDirection) * SlipGaugeWidth_Frame/2;
				target[1] = target[1] - sin(FgDirection) * SlipGaugeWidth_Frame/2;
			}
		}
		else
		{
			if(CurrentLineGrabCount == 1)
			{				
				target[0] = target[0] + cos(slipgangle) * SlipgaugeLength + cos(FgDirection) * SlipGaugeWidth_Frame/2;
				target[1] = target[1] + sin(slipgangle) * SlipgaugeLength + sin(FgDirection) * SlipGaugeWidth_Frame/2;
			}
			else
			{
				target[0] = target[0] + cos(slipgangle) * SlipgaugeLength - cos(FgDirection) * SlipGaugeWidth_Frame/2;
				target[1] = target[1] + sin(slipgangle) * SlipgaugeLength - sin(FgDirection) * SlipGaugeWidth_Frame/2;
			}
		}*/
		if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
		   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::AUTO_WIDTHMEASUREMENTMODE);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0016", ex); }
}

//Function to goto next step in width measurment..
void RWrapper::RW_AutoCalibration::GotoNextStep(bool addLocalisedaction)
{
	try
	{
		if(LinesAddedForCurrentMeasure == 1) // Add One More Line in Same orientation And goto StartPosition of 2nd line..
		{
			CurrentLineGrabCount = CurrentLineGrabCount + 1;
			if(CurrentLineGrabCount == 1 && MachineType != "OneShot")
			{
				GotoNextGrabPosition();
			}
			else
			{
				CurrentLineGrabCount = 0;
				LinesAddedForCurrentMeasure = 2;
				if(MachineType == "OneShot")
				{
					CALBCALCOBJECT->AddSecondLine();
					FrameGrabPointsToSelectedLine();
				}
				else
				{
					CALBCALCOBJECT->AddSecondLine(false);
					GotoSecondLineStartPosition();
				}
			}
		}
		else if(LinesAddedForCurrentMeasure == 2) // Increment the orientation
		{
			if(addLocalisedaction && CurrentAction != NULL)
			{
				((LocalisedCorrectionAction*)CurrentAction)->SetLocalisedCorrectionProperties(MeasurementCount, RowCount, ColumnCount);
			}
			CurrentLineGrabCount = CurrentLineGrabCount + 1;
			if(CurrentLineGrabCount == 1 && MachineType != "OneShot")
			{
				GotoNextGrabPosition();
			}
			else
			{
				CurrentLineGrabCount = 0;
				MeasurementCount = MeasurementCount + 1;
				if((MeasurementCount == 9 && MachineType != "OneShot") || (MeasurementCount == 2 && MachineType == "OneShot")) // Goto Next Cell..
				{
					MeasurementCount = 0;
					CALBCALCOBJECT->CalculateWidthBnTwoLines(CurrentAction);
					ColumnCount = ColumnCount + 1;
					if(ColumnCount >= TotalColumns)
					{
						ColumnCount = 0;
						RowCount = RowCount + 1;
					}
					if(RowCount >= TotalRows) // Finished all Measurements..
					{
						FinishedMeasurement();
						return;
					}
					CurrentLType = CURRENTLINETYPE::VERTICAL_LINE;
					if(MachineType == "OneShot")
						StartNextOrientation();
					else
						GotoNextCell();
				}
				else if((MeasurementCount % 3 == 0 && MachineType != "OneShot") || (MeasurementCount == 1 && MachineType == "OneShot"))
				{
					CALBCALCOBJECT->CalculateWidthBnTwoLines(CurrentAction);
					GotoNextOrientation();
				}
				else
					RepeatCurrentOrientaion(MeasurementCount % 3);
				if(addLocalisedaction && CurrentAction != NULL)
				{
					MAINDllOBJECT->addAction(CurrentAction);
					CurrentAction = NULL;
				}
			}			
		}			
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0016", ex); }
}

//Function to calculate the next grab position.. If contains more than one frame for a line..
void RWrapper::RW_AutoCalibration::GotoNextGrabPosition()
{
	try
	{
		double target[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		if(CurrentLType == CURRENTLINETYPE::VERTICAL_LINE)
		{
			target[1] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] + SlipGaugeWidth_Frame;
		}
		else if(CurrentLType == CURRENTLINETYPE::HORIZONTAL_LINE)
		{
			target[0] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0] - SlipGaugeWidth_Frame;
		}
		else if(CurrentLType == CURRENTLINETYPE::INCLINED_45_LINE)
		{
			target[0] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0] + cos(3 * M_PI_4) * SlipGaugeWidth_Frame;
			target[1] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] + sin(3 * M_PI_4) * SlipGaugeWidth_Frame;
		}	
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::AUTO_CALIBRATIONMODE);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0017", ex); }
}

//Goto second line grab position...
void RWrapper::RW_AutoCalibration::GotoSecondLineStartPosition()
{
	try
	{
		//FramegrabBase* fg = CALBCALCOBJECT->FgofFirstLine();

		double target[4] = {CurrentAction->DROPosition.getX(), CurrentAction->DROPosition.getY(), CurrentAction->DROPosition.getZ(), CurrentAction->DROPosition.getR()};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		if(CurrentLType == CURRENTLINETYPE::VERTICAL_LINE)
		{
			target[0] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0] + SlipgaugeLength;
		}
		else if(CurrentLType == CURRENTLINETYPE::HORIZONTAL_LINE)
		{
			target[1] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] + SlipgaugeLength;
		}
		else if(CurrentLType == CURRENTLINETYPE::INCLINED_45_LINE)
		{
			target[0] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0] + cos(M_PI_4) * SlipgaugeLength - cos(3 * M_PI_4) * SlipGaugeWidth_Frame;
			target[1] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] + sin(M_PI_4) * SlipgaugeLength - sin(3 * M_PI_4) * SlipGaugeWidth_Frame;
		}	
		if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
		   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::AUTO_CALIBRATIONMODE);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0018", ex); }
}

//Goto the next orientation..
void RWrapper::RW_AutoCalibration::GotoNextOrientation()
{
	try
	{
		//FramegrabBase* fg = CALBCALCOBJECT->FgofFirstLine();
		double target[4] = {CurrentAction->DROPosition.getX(), CurrentAction->DROPosition.getY(), CurrentAction->DROPosition.getZ(), CurrentAction->DROPosition.getR()};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		if(CurrentLType == CURRENTLINETYPE::VERTICAL_LINE)
		{
			CurrentLType = CURRENTLINETYPE::HORIZONTAL_LINE;
			target[0] = target[0] + SlipGaugeWidth_Frame - (2 * ShiftLength);
		}
		else if(CurrentLType == CURRENTLINETYPE::HORIZONTAL_LINE)
		{
			if(MachineType == "OneShot")
				CurrentLType = CURRENTLINETYPE::VERTICAL_LINE;
			else
				CurrentLType = CURRENTLINETYPE::INCLINED_45_LINE;
			//target[0] = target[0] - SlipgaugeLength/2;
			target[0] = target[0] + 0.15 * SlipgaugeLength;
			target[1] = target[1] + (0.15 * SlipgaugeLength) - (2 * ShiftLength);
		}
		else if(CurrentLType == CURRENTLINETYPE::INCLINED_45_LINE)
		{
			CurrentLType = CURRENTLINETYPE::VERTICAL_LINE;
			target[0] = target[0] -  (0.15 * SlipgaugeLength) - 2 * (0.7 * ShiftLength);
			target[1] = target[1] -  (0.15 * SlipgaugeLength) - 2 * (0.7 * ShiftLength);
		}
		if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
		   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
		if(MachineType == "OneShot")
			StartNextOrientation();
		else
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::AUTO_CALIBRATIONNEXTORNTATN);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0019", ex); }
}

void RWrapper::RW_AutoCalibration::RepeatCurrentOrientaion(int i)
{
	try
	{
		/*FramegrabBase* fg = CALBCALCOBJECT->FgofFirstLine();*/
		double target[4] = {CurrentAction->DROPosition.getX(), CurrentAction->DROPosition.getY(), CurrentAction->DROPosition.getZ(), CurrentAction->DROPosition.getR()};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		CALBCALCOBJECT->CalculateWidthBnTwoLines(CurrentAction);
		if(CurrentLType == CURRENTLINETYPE::VERTICAL_LINE)
		{
			target[0] = target[0] + ShiftLength;
		}
		else if(CurrentLType == CURRENTLINETYPE::HORIZONTAL_LINE)
		{
			target[1] = target[1] + ShiftLength;
		}
		else if(CurrentLType == CURRENTLINETYPE::INCLINED_45_LINE)
		{
			target[0] = target[0] + (0.7 * ShiftLength);
			target[1] = target[1] + (0.7 * ShiftLength);
		}
		if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
		   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::AUTO_CALIBRATIONNEXTORNTATN);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0120", ex); }
}

//Start next orientation... Wait untill user keeps the slipgauge properly..
void RWrapper::RW_AutoCalibration::StartNextOrientation()
{
	try
	{
		if(MachineType == "OneShot")
			CALBCALCOBJECT->SetCurrentMeasurementOrientation((int)CurrentLType, RowCount, ColumnCount, NextCellGap);
		else
			CALBCALCOBJECT->SetCurrentMeasurementOrientation((int)CurrentLType);
		MAINDllOBJECT->ShowMsgBoxString("RW_AutoCalibrationStartNextOrientation01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0020", ex); }
}

//Goto the next cell...
void RWrapper::RW_AutoCalibration::GotoNextCell()
{
	try
	{ 
		double target[4] = {0};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		target[0] = ColumnCount * NextCellGap;
		target[1] = RowCount * NextCellGap;
		target[2] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
		target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::AUTO_CALIBRATIONNEXTORNTATN);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0021", ex); }
}

void RWrapper::RW_AutoCalibration::FrameGrabPoints_WidthMeasure()
{
	try
	{
		if(CalculateBothWidthMeasurement)
		{
			if(LinesAddedForCurrentMeasure == 1)
				CALBCALCOBJECT->SelectCurrentGrabbingLine(1);
			else
				CALBCALCOBJECT->SelectCurrentGrabbingLine(2);
			CALBCALCOBJECT->GrabPoints_WidthMeasureToSelectedLine();
			Sleep(50);
			RWrapper::RW_DRO::MYINSTANCE()->DontTakeDroValues = true;
			RWrapper::RW_DRO::MYINSTANCE()->DontUpdateLocalisedCalibration = true;
			bool Previous_UseLocalisedCorrection = MAINDllOBJECT->UseLocalisedCorrection;
			MAINDllOBJECT->UseLocalisedCorrection = false;
			RWrapper::RW_DRO::MYINSTANCE()->Handle_DROCallback(&RWrapper::RW_DRO::MYINSTANCE()->OriginalDROCoordinate[0]);
			if(LinesAddedForCurrentMeasure == 1)
				CALBCALCOBJECT->SelectCurrentGrabbingLine(3);
			else
				CALBCALCOBJECT->SelectCurrentGrabbingLine(4);
			CALBCALCOBJECT->GrabPoints_WidthMeasureToSelectedLine();
			Sleep(50);
			RWrapper::RW_DRO::MYINSTANCE()->DontTakeDroValues = false;
			RWrapper::RW_DRO::MYINSTANCE()->DontUpdateLocalisedCalibration = false;
			MAINDllOBJECT->UseLocalisedCorrection = Previous_UseLocalisedCorrection;
			RWrapper::RW_DRO::MYINSTANCE()->Handle_DROCallback(&RWrapper::RW_DRO::MYINSTANCE()->OriginalDROCoordinate[0]);
		}
		else
		{
			CALBCALCOBJECT->GrabPoints_WidthMeasureToSelectedLine();
			Sleep(50);
		}

		if(LinesAddedForCurrentMeasure == 1)
		{
			if(CurrentLineGrabCount == 1 && !this->DoSingleFramePerSide)
				CurrentLineGrabCount = 2;
			else 
			{
				CALBCALCOBJECT->AddSecondLine();
				if(CalculateBothWidthMeasurement)
					CALBCALCOBJECT->AddSecondLine_WithoutLocalised();
				LinesAddedForCurrentMeasure = 2;
				CurrentLineGrabCount = 1;
			}
		}
		else
		{
			if (CurrentLineGrabCount == 1)
			{
				CurrentLineGrabCount = 2;
				if (this->DoSingleFramePerSide)
				{
					LinesAddedForCurrentMeasure = 3;
					CurrentLineGrabCount = 3;
				}
			}
			else 
			{
				LinesAddedForCurrentMeasure = 3;
				CurrentLineGrabCount = 3;
			}
		}
		if(LinesAddedForCurrentMeasure == 3 && CurrentLineGrabCount == 3) // Finish of width Measurement
		{
			if(!RWrapper::RW_DRO::MYINSTANCE()->AutoCalibrationMode && !RWrapper::RW_DRO::MYINSTANCE()->HomingMovementMode) 
			{
				if(RWrapper::RW_DRO::MYINSTANCE()->LocalisedCalibrationDone && MAINDllOBJECT->UseLocalisedCorrection && RWrapper::RW_DRO::MYINSTANCE()->HomingisDone)
					CALBCALCOBJECT->CalculateSGWidthBnTwoLines_Localised(true);
				else
					CALBCALCOBJECT->CalculateSGWidthBnTwoLines_Localised(false);
			}
			else
				CALBCALCOBJECT->CalculateSGWidthBnTwoLines_Localised(false);
			if(CalculateBothWidthMeasurement)
					CALBCALCOBJECT->CalculateSGWidthBnTwoLines_WithoutLocalised();
			if(RepeatCalibCount>0)
			{
					double target[4] = {WidthStartDROValue[0], WidthStartDROValue[1], WidthStartDROValue[2], WidthStartDROValue[3]};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition( &target[0], &feedrate[0], TargetReachedCallback::AUTO_SGWIDTH);
			}
			else
			{
				MAINDllOBJECT->RepeatSGWidth = false;
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2); //Manual Mode..
			
			 MAINDllOBJECT->SetStatusCode("RW_AutoCalibration02");
			}
			//System::Windows::Forms::MessageBox::Show("Finished Width Measurement", "Rapid-I");
		}
		else
		
			GotoNextStep_WidthMeasurement();

	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0022", ex); }
}

//Grab the points to the selected shape..and goto the next step..
void RWrapper::RW_AutoCalibration::FrameGrabPointsToSelectedLine()
{
	try
	{
		//bool OrignalAvgMode = MAINDllOBJECT->UseAvImage();
		//MAINDllOBJECT->UseAvImage(true);
		if(MachineType == "OneShot")
			CALBCALCOBJECT->GrabPointsToSelectedLine(MeasurementCount, RowCount, ColumnCount, NextCellGap, LinesAddedForCurrentMeasure);
		else
			CALBCALCOBJECT->GrabPointsToSelectedLine((int)(MeasurementCount / 3), false);
		Sleep(50);
		RWrapper::RW_AutoCalibration::MYINSTANCE()->GotoNextStep(true);
		//MAINDllOBJECT->UseAvImage(OrignalAvgMode);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0022", ex); }
}

//Hanlde the home position clicked.. Raise event to show the wait window and send command to goto home position..
void RWrapper::RW_AutoCalibration::Handle_HomeClicked(bool X_Axis, bool Y_Axis, bool Z_Axis)
{
	try
	{
		DROInstance->ResetZeroVal();
		//double target[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0] - 800, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] - 800, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] + 800, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1], RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};
		RWrapper::RW_DRO::MYINSTANCE()->HomingMovementMode = true;
		RWrapper::RW_MainInterface::MYINSTANCE()->ShowWaitStatusMessage("Please wait... Going to Home Position", 10, true);
		if((MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::DSP) && (MAINDllOBJECT->CurrentMahcineType != RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP))
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
		double target[4] = { -500, -500, 500, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] };
		this->Home_X_Axis = X_Axis; this->Home_Y_Axis = Y_Axis; this->Home_Z_Axis = Z_Axis;

		if (!Z_Axis)
			target[2] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
		//if(X_Axis)
		//{
		//	//if(RWrapper::RW_DRO::MYINSTANCE()->XAxisHomeToPositive)
		//	//	target[0] += 500;
		//	//else
		//		target[0] -= 500;
		//}
		//if(Y_Axis)
		//{
		//	//if(RWrapper::RW_DRO::MYINSTANCE()->YAxisHomeToPositive)
		//	//	target[1] += 500;
		//	//else
		//		target[1] -= 500;
		//}
		//if(Z_Axis)
		//	target[2] += 300;
		if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
		   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::MACHINE_HOMINGMODE);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0024", ex); }
}

//Reached Home position.. Move back .. 5mm in X and Y and reset the RW_DRO..
void RWrapper::RW_AutoCalibration::ReachedHomePosition()
{
	try
	{
		DROInstance->ToManual();
		Sleep(50);
		DROInstance->ActivateCNC();
		Sleep(50);
		double target[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0] + 5,  RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] + 5,  RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] - 5, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		////if (RWrapper::RW_DRO::MYINSTANCE()->XAxisHomeToPositive)
		//	target[0] -= 10;
		////if (RWrapper::RW_DRO::MYINSTANCE()->YAxisHomeToPositive)
		//	target[1] -= 10;

		if (!this->Home_Z_Axis)
			target[2] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];
		double feedrate[4] = {5, 5, 5, 5};
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::MACHINE_MOVELITTLE_AFTERHOMING);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0025", ex); }
}

void RWrapper::RW_AutoCalibration::ReachedNearHomePosition()
{
	try
	{
		DROInstance->ToManual();
		Sleep(50);
		DROInstance->ActivateCNC();
		Sleep(50);
		double target[4] = {RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0] - 100,  RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1] - 100,  RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] + 100, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		//if (RWrapper::RW_DRO::MYINSTANCE()->XAxisHomeToPositive)
		//	target[0] += 20;
		//if (RWrapper::RW_DRO::MYINSTANCE()->YAxisHomeToPositive)
		//	target[1] += 20;
		if (!this->Home_Z_Axis)
			target[2] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2];

		double feedrate[4] = {0.5, 0.5, 0.5, 0.5};
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::MACHINE_MOVESLOW_FORHOMING);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0025", ex); }
}

//Reached target position..
void RWrapper::RW_AutoCalibration::ReachedTargetPosition()
{
	try
	{
		RWrapper::RW_CNC::MYINSTANCE()->Activate_ManualModeDRO();
		Sleep(50);
		RWrapper::RW_MainInterface::MYINSTANCE()->StartHideWaitStatusMessage(10);
		Sleep(50);
		//if (RWrapper::RW_MainInterface::MYINSTANCE()->MachineCardFlag < 2)
		//{
			//if (RWrapper::RW_DRO::MYINSTANCE()->XAxisHomeToPositive || RWrapper::RW_DRO::MYINSTANCE()->YAxisHomeToPositive)
			//{
				//double ResetVals[4] = { 0 };
				//if (RWrapper::RW_DRO::MYINSTANCE()->XAxisHomeToPositive)
				//	ResetVals[0] = RWrapper::RW_DRO::MYINSTANCE()->XAxisPositiveHomeValue;
				//if (RWrapper::RW_DRO::MYINSTANCE()->YAxisHomeToPositive)
				//	ResetVals[1] = RWrapper::RW_DRO::MYINSTANCE()->YAxisPositiveHomeValue;
				double ResetVals[4] = { RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2] - 150, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3] };
				if (!this->Home_Z_Axis)
					ResetVals[2] = 0;

				DROInstance->ToReset(ResetVals);
			//}
			//else
			//{
			//	DROInstance->ToReset();
			//}
		//}
		Sleep(50);
		RWrapper::RW_DRO::MYINSTANCE()->UpdateDRO_textbox();	
		RWrapper::RW_DRO::MYINSTANCE()->HomingMovementMode = false;
		RWrapper::RW_DRO::MYINSTANCE()->HomingisDone = true;
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
		Sleep(100);
		MAINDllOBJECT->SetStatusCode("RW_AutoCalibration03");
		
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RWACN0023", ex); }
}

void RWrapper::RW_AutoCalibration::SetMeasurementAsMedian(bool isMedian, double shiftLength)
{
	this->MeasurementAsMedian = isMedian;
	this->ShiftLength = shiftLength;
}

void RWrapper::RW_AutoCalibration::SetOverlapWidth( double overlapWidth)
{
	this->OverlapWidth = overlapWidth;
	NextCellGap = SlipgaugeLength - OverlapWidth;
}

void RWrapper::RW_AutoCalibration::GotoLastPostion()
{
	MAINDllOBJECT->GotoLastLocalisedAction();
}
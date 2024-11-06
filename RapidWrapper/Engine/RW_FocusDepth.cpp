#include "Stdafx.h"
#include "RW_MainInterface.h"
#include "..\MAINDLL\Engine\FocusFunctions.h"
#include "..\MAINDLL\Engine\PartProgramFunctions.h"

void CALLBACK TwoBoxFocusDepthCallback(bool Stratfocus)
{
	RWrapper::RW_FocusDepth::MYINSTANCE()->FillFocusBox = true;
	RWrapper::RW_FocusDepth::MYINSTANCE()->FocusClickEventRaised = true;
	if(Stratfocus)
		RWrapper::RW_FocusDepth::MYINSTANCE()->RaiseFocusEvents(4);
	else
		RWrapper::RW_FocusDepth::MYINSTANCE()->RaiseFocusEvents(5);
	RWrapper::RW_FocusDepth::MYINSTANCE()->FocusClickEventRaised = false;
}

void CALLBACK SetZValueForReferenceDepth()
{
	RWrapper::RW_FocusDepth::MYINSTANCE()->Update_FocusDepthValues();
}

void CALLBACK AFSGotoNextPosition()
{
	RWrapper::RW_FocusDepth::MYINSTANCE()->AFS_GotoNextScanPosition();
}

void CALLBACK AFSsetSpan(double span)
{
	RWrapper::RW_FocusDepth::MYINSTANCE()->focusSpan = span;
}

void CALLBACK FocusBoxValueChanged(double Fvalue)
{
	RWrapper::RW_FocusDepth::MYINSTANCE()->FillFocusValueInFrontend(Fvalue);
}

RWrapper::RW_FocusDepth::RW_FocusDepth()
{
	try
	{
		focusDepthInstance = this;
		this->SelectedAxis = 2;//Selected axis as "Z".. default..//
		this->AutoFocusMode = false;
		this->FocusClickEventRaised = false;
		this->FillFocusBox = false;
		this->focusSpan = 0.4; this->focusSpeed = 0.2;
		this->FocusValue = 0;
		this->FocusdepthStepCnt = 0;
		this->CurrentFocusValue = "";
		this->FocusStartPosition = gcnew cli::array<double>(4);
		this->NextCellPosTemp = gcnew cli::array<double>(4);
		this->AllFocusMeasurementValues = gcnew System::Collections::ArrayList();
		FOCUSCALCOBJECT->StarFocusDepth_BoxDepth = &TwoBoxFocusDepthCallback;
		FOCUSCALCOBJECT->FocusForReferenceDepth = &SetZValueForReferenceDepth;
		FOCUSCALCOBJECT->GotoFocusScanPosition = &AFSGotoNextPosition;
		FOCUSCALCOBJECT->changeFocusSpan = &AFSsetSpan;
		FramescanCount = 0;
		NextCellPosTemp[0] = NextCellPosTemp[1] = NextCellPosTemp[2] = NextCellPosTemp[3] = 0;
		FOCUSCALCOBJECT->UpdateCurrentFocusFillValue = &FocusBoxValueChanged;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0001", ex); }
}

RWrapper::RW_FocusDepth::~RW_FocusDepth()
{
}

RWrapper::RW_FocusDepth^ RWrapper::RW_FocusDepth::MYINSTANCE()
{
	return focusDepthInstance;
}


void RWrapper::RW_FocusDepth::FocusClicked()
{
	try
	{
		if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::AUTOCONTOURSCANHANDLER)
		{
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
			if(!FOCUSCALCOBJECT->StartAutoContourScan())
			{
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
				FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::NOTHING;
				FOCUSCALCOBJECT->FocusOnStatus(false);
			}
			return;
		}
		if(RWrapper::RW_MainInterface::MYINSTANCE()->MachineConnectedStatus)
			 MAINDllOBJECT->ShowHideWaitCursor(1);
		if(MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::FOCUS_DEPTHTWOBOXHANDLER || MAINDllOBJECT->CURRENTHANDLE == RapidEnums::RAPIDHANDLERTYPE::FOCUS_DEPTHFOURBOXHANDLER)
		{
			bool Checkflag = FOCUSCALCOBJECT->FocusClicked_Handler();
			if(Checkflag)
				StartFocus();
			else if(FocusClickEventRaised)
				StartFocus();
		}
		else
		{
			FOCUSCALCOBJECT->FocusStartPosition.set(RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2]);
			if(AutoFocusMode)
			{
				StartFocus();
			}
			else
			{
				this->Focusflag = false;
				DROInstance->notFocusing = true;
				if((MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::DSP) || (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP))
				{
					FOCUSCALCOBJECT->CalculateRectangle();
					this->CurrentFocusValue = "";
					RWrapper::RW_FocusDepth::MYINSTANCE()->RaiseFocusEvents(0);
					this->Focusflag = true;
					//RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::FOCUSDEPTH_POSITIONREACHED);
					//RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
				}
				else
				{
					FOCUSCALCOBJECT->CalculateBestFocus();
					RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
					double target[4] = {0}, feedrate[4] = {5, 5, 5, 5};
					for(int i = 0; i < 4; i++)
					{
						if(i == RW_FocusDepth::MYINSTANCE()->GetSelectedAxis())
							target[i] = FOCUSCALCOBJECT->FocusZValue();
						else
							target[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
					}
					if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
					   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
					RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::FOCUSDEPTH_POSITIONREACHED);
				}
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0002", ex); }
}

void RWrapper::RW_FocusDepth::StartFocus()
{
	try
	{
		FOCUSCALCOBJECT->FocusStartPosition.set(RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[0], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[1], RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[2]);
		FOCUSCALCOBJECT->ClearLastFocusValues(); //Clear the last focus values...
		this->CurrentFocusValue = "";
		RaiseFocusEvents(0);
		if(FOCUSCALCOBJECT->CurrentFocusType != RapidEnums::RAPIDFOCUSMODE::AUTOCONTOURSCANMODE)
			FOCUSCALCOBJECT->CalculateRectangle();
		if((MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::DSP) || (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP))
		{
			RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::FOCUSDEPTH_SPAN);
			RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
		}
		else
		{
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1); //cnc mode
			double feedrate[4] = {5, 5, 5, 5}, target[4] = {0};
			for(int i = 0; i < 4; i++)
			{
				if(i == SelectedAxis)
					target[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i] - this->focusSpan; // Changed sign on 2 april 2011... As per narayanan sir requirement
				else
					target[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];
			}
			if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
			   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::FOCUSDEPTH_SPAN);
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0003", ex); }
}

void RWrapper::RW_FocusDepth::AbortClicked()
{
	try
	{
		this->Focusflag = false;
		DROInstance->notFocusing = true;
		if(MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::DSP || MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP)
		{
			MAINDllOBJECT->ShowHideWaitCursor(2);
			FOCUSCALCOBJECT->CalculateBestFocus();
			if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
				this->CurrentFocusValue = Microsoft::VisualBasic::Strings::Format(FOCUSCALCOBJECT->FocusZValue()/25.4,"0.0000");
			else
				this->CurrentFocusValue = Microsoft::VisualBasic::Strings::Format(FOCUSCALCOBJECT->FocusZValue(),"0.0000");
			RWrapper::RW_FocusDepth::MYINSTANCE()->RaiseFocusEvents(0);
		}
		else
		{
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);	
			FocusResetClicked();
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0004", ex); }
}

void RWrapper::RW_FocusDepth::ShowHideFocus(bool flag)
{
	FOCUSCALCOBJECT->FocusOnStatus(flag);
}

void RWrapper::RW_FocusDepth::NewClicked()
{
	try
	{
		FOCUSCALCOBJECT->AddNewDepthMeasure();
		FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::FOCUSDEPTH_MODE;
		FOCUSCALCOBJECT->FocusOnStatus(true);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0005", ex); }
}

void RWrapper::RW_FocusDepth::SetZ1Clicked()
{
	try
	{
		if(FOCUSCALCOBJECT->FocusZValue() > 160) return;
		if(FOCUSCALCOBJECT->CurrentDepthMeasure == NULL)
		{
			MAINDllOBJECT->ShowMsgBoxString("RW_FocusDepthSetZ1Clicked01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			return;
		}
		if(this->CurrentFocusValue == "") //add dro value
		{
			if(MAINDllOBJECT->ShowMsgBoxString("RW_FocusDepthSetZ1Clicked02", RapidEnums::MSGBOXTYPE::MSG_YES_NO, RapidEnums::MSGBOXICONTYPE::MSG_QUESTION))
			{
				FOCUSCALCOBJECT->AddDepthAction_Zposition(1);
				Update_FocusDepthValues();
			}
		}
		else
		{
			FOCUSCALCOBJECT->AddTheDepthAction(1);
			Update_FocusDepthValues();
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0008", ex); }
}

void RWrapper::RW_FocusDepth::SetZ2Clicked()
{
	try
	{
		if(FOCUSCALCOBJECT->FocusZValue() > 160) return;
		if(FOCUSCALCOBJECT->CurrentDepthMeasure == NULL)
		{
			MAINDllOBJECT->ShowMsgBoxString("RW_FocusDepthSetZ2Clicked01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
			return;
		}
		if(this->CurrentFocusValue == "") //add dro value
		{
			if(MAINDllOBJECT->ShowMsgBoxString("RW_FocusDepthSetZ2Clicked02", RapidEnums::MSGBOXTYPE::MSG_YES_NO, RapidEnums::MSGBOXICONTYPE::MSG_QUESTION))
			{
				FOCUSCALCOBJECT->AddDepthAction_Zposition(2);
				Update_FocusDepthValues();
			}
		}
		else
		{
			FOCUSCALCOBJECT->AddTheDepthAction(2);
			Update_FocusDepthValues();
		}
		FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::NOTHING;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0009", ex); }
}

void RWrapper::RW_FocusDepth::Update_FocusDepthValues()
{
	try
	{
		double CZ1value, CZ2value, CDpthvalue;
		bool Z1value = FOCUSCALCOBJECT->GetZ1_value(&CZ1value);
		bool Z2value = FOCUSCALCOBJECT->GetZ2_value(&CZ2value);
		bool Dpthvalue = FOCUSCALCOBJECT->GetDepth_value(&CDpthvalue);
		if(Z1value)
			CurrentDepthZ1Value = Microsoft::VisualBasic::Strings::Format(CZ1value, "0.0000");
		else
			CurrentDepthZ1Value = "";
		if(Z2value)
			CurrentDepthZ2Value = Microsoft::VisualBasic::Strings::Format(CZ2value, "0.0000");
		else
			CurrentDepthZ2Value = "";
		if(Dpthvalue)
			CurrentDepthValue = Microsoft::VisualBasic::Strings::Format(CDpthvalue, "0.0000");
		else
			CurrentDepthValue = "";
		RaiseFocusEvents(3);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0009", ex); }
}

void RWrapper::RW_FocusDepth::SetFocusRectangleProperties(double row, double column, double gap, double width, double height, double span, double speed)
{
	try
	{	
		FillFocusBox = false;
		/*if(MAINDllOBJECT->MeasureUnitType == RapidEnums::RAPIDUNITS::INCHES)
		{
				row = row * 25.4;
				column = column * 25.4;
				gap = gap * 25.4;
				width = width * 25.4;
				height = height * 25.4;
				span = span * 25.4;
		}*/
		switch(FOCUSCALCOBJECT->CurrentFocusType)
		{
			case RapidEnums::RAPIDFOCUSMODE::FOCUSDEPTH_MODE:
			{
				if(!PPCALCOBJECT->IsPartProgramRunning() && FOCUSCALCOBJECT->ShapeForDepth == FocusFunctions::FOCUSDEPTHSHAPETYPE::POINTSHAPE)
				{	
					row = 1; column = 1;
				}
				FillFocusBox = true;
				break;
			}
			case RapidEnums::RAPIDFOCUSMODE::TWOBOXDEPTH_MODE:
			{
				row = 2; column = 1;
				span = 0.8;
				FillFocusBox = true;
				break;
			}
			case RapidEnums::RAPIDFOCUSMODE::FOURBOXDEPTH_MODE:
			{
				row = 4; column = 1;
				FillFocusBox = true;
				break;
			}
			case RapidEnums::RAPIDFOCUSMODE::CONTOOURSCAN_MODE:
			{
				gap = 1;
				if(column > row)
					row = 1; 
				else
					column = 1;
				FOCUSCALCOBJECT->setContourScanWidth(column + width, row + height);
				break;
			}
			case RapidEnums::RAPIDFOCUSMODE::HOMING_MODE:
			case RapidEnums::RAPIDFOCUSMODE::REFERENCEDOT_SECOND:
			{

				row = 1; column = 1;
				break;
			}
			case RapidEnums::RAPIDFOCUSMODE::NOTHING:
			{
				if(MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::AUTOFOCUSSCANHANDLER && MAINDllOBJECT->CURRENTHANDLE != RapidEnums::RAPIDHANDLERTYPE::AUTOCONTOURSCANHANDLER)
				{
					row = 1; column = 1;
					FillFocusBox = true;
				}
				break;
			}
		}
		if(gap <= 0) gap = 1;
		//Calculates the total number rectangles and its parameters..
		FOCUSCALCOBJECT->setRowColumn(row, column, gap, width, height);
		focusSpan = span; focusSpeed = speed;
		if(PPCALCOBJECT->IsPartProgramRunning())
			FOCUSCALCOBJECT->FocusOnStatus(true);
		MAINDllOBJECT->update_VideoGraphics();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0007", ex); }
}

void RWrapper::RW_FocusDepth::SetSelectedAxis(int i)
{
	try
	{
		SelectedAxis = i;
		FOCUSCALCOBJECT->SetSelectedAxis(i);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0006", ex); }
}

int RWrapper::RW_FocusDepth::GetSelectedAxis()
{
	return SelectedAxis;
}

void RWrapper::RW_FocusDepth::FillFocusValueInFrontend(double fvalue)
{
	try
	{
		FocusValue = fvalue;
		if(FillFocusBox)
		{
			if(FocusValue < 1 && FocusValue > 0)
				FocusValueChanged::raise();
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0011", ex); }
}

void RWrapper::RW_FocusDepth::SelectDepthDatum(int Depthid)
 {
	  if(FOCUSCALCOBJECT->CurrentDepthMeasure == NULL)
	  {
		  MAINDllOBJECT->ShowMsgBoxString("RW_FocusDepthSelectDepthDatum01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION);
		  return;
	  }
	  MAINDllOBJECT->Wait_VideoGraphicsUpdate();
	  MAINDllOBJECT->Wait_RcadGraphicsUpdate();
	  FOCUSCALCOBJECT->AddNewFocusDatum(Depthid);
 }

void RWrapper::RW_FocusDepth::FocusResetClicked()
{
	try
	{
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
		FOCUSCALCOBJECT->ClearLastFocusValues();
		this->CurrentFocusValue = "";
		RaiseFocusEvents(0);
		MAINDllOBJECT->SetStatusCode("RW_FocusDepth01");
		if(!AutoFocusMode)
		{
			MAINDllOBJECT->SetStatusCode("RW_FocusDepth02");
			this->Focusflag = true;
			DROInstance->notFocusing = false;
			FOCUSCALCOBJECT->CalculateRectangle();
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0010", ex); }
}

bool RWrapper::RW_FocusDepth::DepthResetClicked()
{
	if(FOCUSCALCOBJECT->CurrentDepthMeasure == NULL)
	{
		 MAINDllOBJECT->ShowMsgBoxString("RW_FocusDepthDepthResetClicked01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_INFORMATION);
		return false;
	}
	if(MAINDllOBJECT->ShowMsgBoxString("RW_FocusDepthDepthResetClicked02", RapidEnums::MSGBOXTYPE::MSG_YES_NO, RapidEnums::MSGBOXICONTYPE::MSG_QUESTION))
	{
		FOCUSCALCOBJECT->ResetSelectedDepthMeasure();
		return true;
	}
	return false;
}

void RWrapper::RW_FocusDepth::RaiseFocusEvents(int Cnt)
{
	try
	{
		if(Cnt == 0)
			FocusFinished::raise();
		else if(Cnt == 1)
			DepthShapeAdded::raise();
		else if(Cnt == 2)
			DepthShapeRemoved::raise();
		else if(Cnt == 3)
			UpdateDepthValues::raise();
		else if(Cnt == 4)
			GenerateFocusBtnClick::raise("FOCUS");
		else if(Cnt == 5)
			GenerateFocusBtnClick::raise("ABORT");
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0011", ex); }
}

void RWrapper::RW_FocusDepth::DoFocusCalibration(double x, double y, double z, int rowNo, int colNo, int gap, int width, int height, double span, double speed)
{
	try
	{
		if(MAINDllOBJECT->ShowMsgBoxString("RW_FocusDepthDoFocusCalibration01", RapidEnums::MSGBOXTYPE::MSG_YES_NO, RapidEnums::MSGBOXICONTYPE::MSG_QUESTION))
		{
			FOCUSCALCOBJECT->ResetFocusCalibrationSettings();
			FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::CALIBRATION_MODE;
			this->FillFocusBox = false;
			this->MoveX = x; this->MoveY = y; this->MoveZ = z;

			if(gap == 0 || height < 10 || width < 10 || rowNo < 15 || colNo < 20)
			{
				/*MAINDllOBJECT->ShowMsgBoxString("RW_FocusDepthDoFocusCalibration01", RapidEnums::MSGBOXTYPE::MSG_YES_NO, RapidEnums::MSGBOXICONTYPE::MSG_QUESTION);*/
				return;
			}

			FOCUSCALCOBJECT->NoOfRepeatAtPlace = 3;
			try
			{
				const char* temporarychar = (const char*)(Marshal::StringToHGlobalAnsi(RWrapper::RW_MainInterface::MYINSTANCE()->AppDataFolderPath)).ToPointer();
				std::string FilePath = temporarychar;
				FilePath = FilePath + "\\Database";
				wifstream ifile;			
				ifile.open(FilePath + "\\FocusCalibration.txt");	
				if(!ifile.fail())
				{
				   ifile >> FOCUSCALCOBJECT->NoOfRepeatAtPlace;
				}
				ifile.close();
			}
			catch(Exception^ ex)
			{ 
				FOCUSCALCOBJECT->NoOfRepeatAtPlace = 3;
				RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0012a", ex); 
			}
		
			Last_gap = gap;
			Last_height = height;
			Last_width = width;
			Last_row = rowNo;
			Last_Column = colNo;

			int maxrowLimit = int(floor((MAINDllOBJECT->getWindow(0).getWinDim().y - Last_height) / Last_gap)) + 1;
			int maxcolLimit = int(floor((MAINDllOBJECT->getWindow(0).getWinDim().x - Last_width) / Last_gap)) + 1;
			if(Last_row > maxrowLimit)
				Last_row = maxrowLimit;
			if(Last_Column > maxcolLimit)
				Last_Column = maxcolLimit;

			FOCUSCALCOBJECT->FiveFocus =  (double*)calloc(Last_row * Last_Column * FOCUSCALCOBJECT->NoOfRepeatAtPlace, sizeof(double));

			double upp = MAINDllOBJECT->getWindow(0).getUppX();
			double FocusScanwidth = ((Last_Column - 1) * Last_gap + Last_width) * upp;
			double FocusScanHeight = ((Last_row - 1) * Last_gap + Last_width) * upp;
			if(this->MoveX < FocusScanwidth || this->MoveY < FocusScanHeight)
				FOCUSCALCOBJECT->FocusAtOnePositionOnly = true;
			else
				FOCUSCALCOBJECT->FocusAtOnePositionOnly = false;

			FOCUSCALCOBJECT->setRowColumn(Last_row, Last_Column, Last_gap, Last_width, Last_height);

			int rowhalf = floor(Last_row/2);
			int colhalf = floor(Last_Column/2);
			int rowspan = ceil(Last_row/15);
			int colspan = ceil(Last_Column/10);

			if(FOCUSCALCOBJECT->FocusAtOnePositionOnly)
				rowspan *= 3;

			FOCUSCALCOBJECT->lowRowCutoff = rowhalf - (rowspan % 2);
			FOCUSCALCOBJECT->upRowCutoff = FOCUSCALCOBJECT->lowRowCutoff + rowspan;
			FOCUSCALCOBJECT->lowColCutoff = colhalf - (colspan % 2);
			FOCUSCALCOBJECT->upColCutoff = FOCUSCALCOBJECT->lowColCutoff + colspan;

			this->focusSpeed = speed;
			this->focusSpan = span;

			FOCUSCALCOBJECT->FocusOnStatus(true);
			for(int i = 0; i < 4; i++)
				FocusStartPosition[i] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[i];

			FOCUSCALCOBJECT->ClearLastFocusValues(); //Clear the last focus values...
			if((MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::DSP) || (MAINDllOBJECT->CurrentMahcineType == RapidEnums::RAPIDMACHINETYPE::HORIZONTAL_DSP))
			{
				RWrapper::RW_DRO::MYINSTANCE()->setTargetReachedTypeCallback(TargetReachedCallback::FOCUSDEPTH_SPAN);
				RWrapper::RW_DRO::MYINSTANCE()->FunctionTargetReached(false);
			}
			else
			{ 
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1); //RW_CNC mode ...
				FOCUSCALCOBJECT->CalculateRectangle();
				double feedrate[4] = {5, 5, 5, 5}, target[4] = {0};
				//Calcualte the target postion:  current RW_DRO + span;
				for(int i = 0; i < 4; i++)
				{
					if(i == 2)
						target[i] = FocusStartPosition[i] - this->focusSpan;
					else
						target[i] = FocusStartPosition[i];
				}
				if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
					target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
				RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::FOCUSDEPTH_SPAN);
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0012", ex); }
}

void RWrapper::RW_FocusDepth::FocusCalibration_NextRegion()
{
	try
	{
		if(FramescanCount < (FOCUSCALCOBJECT->NoOfRepeatAtPlace - 1))
		{
			FramescanCount ++;
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1); //RW_CNC mode ...
			double feedrate[4] = {5, 5, 5, 5}, target[4] = {0};
			for(int i = 0; i < 4; i++)
			{
				if(i == 2)
					target[i] = NextCellPosTemp[i] + FocusStartPosition[i] - this->focusSpan;
				else
					target[i] = NextCellPosTemp[i] + FocusStartPosition[i];
			}
			if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
				target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::FOCUSDEPTH_SPAN);			
		}
		else
		{
			if(!(FOCUSCALCOBJECT->FocusCalibrationSuccess))
			{
				FramescanCount = 0;
				FOCUSCALCOBJECT->FocusCalibrationSuccess = true;
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
				MAINDllOBJECT->ShowMsgBoxString("RW_FocusDepthFocusCalibration_Failed", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
				FOCUSCALCOBJECT->ResetFocusCalibrationSettings();
				NextCellPosTemp[0] = NextCellPosTemp[1] = NextCellPosTemp[2] = NextCellPosTemp[3] = 0;
				MAINDllOBJECT->ShowHideWaitCursor(2);
			}
			else
			{
				if(FOCUSCALCOBJECT->FocusCalibrationCount() == 4 * FOCUSCALCOBJECT->NoOfRepeatAtPlace)
				{
					UpdateFocusCalibration();
				}
				else
				{
					// Newly added  on 10/01/2013				
					if(FOCUSCALCOBJECT->FocusAtOnePositionOnly)
						 UpdateFocusCalibration();
					else
					{
						int Cnt = FOCUSCALCOBJECT->FocusCalibrationCount();
						FramescanCount = 0;
						NextCellPosTemp[0] = NextCellPosTemp[1] = NextCellPosTemp[2] = NextCellPosTemp[3] = 0;
						if(Cnt == FOCUSCALCOBJECT->NoOfRepeatAtPlace)
						{
							NextCellPosTemp[0] = MoveX;
							NextCellPosTemp[2] = MoveZ;
						}
						else if(Cnt == FOCUSCALCOBJECT->NoOfRepeatAtPlace * 2)
						{
							NextCellPosTemp[0] = MoveX;
							NextCellPosTemp[1] = MoveY;
							NextCellPosTemp[2] = MoveZ;
						}
						else if(Cnt == FOCUSCALCOBJECT->NoOfRepeatAtPlace * 3)
						{						
							NextCellPosTemp[1] = MoveY;
						}
						FOCUSCALCOBJECT->ClearLastFocusValues(); //Clear the last focus values...
						FOCUSCALCOBJECT->CalculateRectangle();
						double feedrate[4] = {5, 5, 5, 5}, target[4] = {0};
						for(int i = 0; i < 4; i++)
						{
							if(i == 2)
								target[i] = NextCellPosTemp[i] + FocusStartPosition[i] - this->focusSpan;
							else
								target[i] = NextCellPosTemp[i] + FocusStartPosition[i];
						}
						if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
						   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
						RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::FOCUSDEPTH_SPAN);
					}
				}
			}
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0013", ex); }
}

void RWrapper::RW_FocusDepth::UpdateFocusCalibration()
{
	try
	{
		FramescanCount = 0;
		FOCUSCALCOBJECT->CalculateFocusCalibrationValues();
		FOCUSCALCOBJECT->FocusOnStatus(false);
		int Count = Last_row * Last_Column;
		for(int i = 0; i < Count; i++)
		{
			AllFocusMeasurementValues->Add(RWrapper::RW_DBSettings::MYINSTANCE()->CurrentMachineNumber); //  MachineNo.
			AllFocusMeasurementValues->Add(FOCUSCALCOBJECT->Rectptr[4 * i]); //Rectangle LeftX
			AllFocusMeasurementValues->Add(FOCUSCALCOBJECT->Rectptr[4 * i + 1]); //Rectangle LeftY
			AllFocusMeasurementValues->Add(FOCUSCALCOBJECT->Rectptr[4 * i + 2]); //Rectangle Width
			AllFocusMeasurementValues->Add(FOCUSCALCOBJECT->Rectptr[4 * i + 3]); //Rectangle Height
			AllFocusMeasurementValues->Add(FOCUSCALCOBJECT->FocusCorrectionValues[i]); // Focus Correction..
		}
		if(FOCUSCALCOBJECT->FocusCorrectionValues!=NULL)
			free(FOCUSCALCOBJECT->FocusCorrectionValues);
		RWrapper::RW_DBSettings::MYINSTANCE()->UpdateFocusCalibrationSettings_CurrentMachine(AllFocusMeasurementValues, Count);
		System::Collections::ArrayList^ GridSetting = gcnew System::Collections::ArrayList();
		GridSetting->Add(Last_row);
		GridSetting->Add(Last_Column);
		GridSetting->Add(Last_width);
		GridSetting->Add(Last_width);
		RWrapper::RW_DBSettings::MYINSTANCE()->UpdateFocusCalibrationGridSettings_CurrentMachine(GridSetting);
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
		MAINDllOBJECT->ShowMsgBoxString("RW_FocusDepthFocusCalibration_NextRegion01", RapidEnums::MSGBOXTYPE::MSG_OK, RapidEnums::MSGBOXICONTYPE::MSG_EXCLAMATION);
		FOCUSCALCOBJECT->ResetFocusCalibrationSettings();
		NextCellPosTemp[0] = NextCellPosTemp[1] = NextCellPosTemp[2] = NextCellPosTemp[3] = 0;
		MAINDllOBJECT->ShowHideWaitCursor(2);
		AllFocusMeasurementValues->Clear();
		free(FOCUSCALCOBJECT->FiveFocus);
		FOCUSCALCOBJECT->FiveFocus = NULL;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0013b", ex); }
}

bool RWrapper::RW_FocusDepth::Start_AutoFocusScan()
{
	try
	{
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
		if(!FOCUSCALCOBJECT->StartAutoFocusScan())
		{
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
			FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::NOTHING;
			FOCUSCALCOBJECT->FocusOnStatus(false);
			return false;
		}
		return true;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0007", ex); }
}

void RWrapper::RW_FocusDepth::Continue_AutoFocusScan(bool conti)
{
	if(conti)
	{
		if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
		FOCUSCALCOBJECT->ContinueAutoFocus();
	}
	else
	{
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
	}
}

void RWrapper::RW_FocusDepth::Stop_AutoFocusScan()
{
	RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
}

void RWrapper::RW_FocusDepth::Rotate_AutoFocusScan(double angleofrotation, int step)
{
	FOCUSCALCOBJECT->RotateInAutoFocus(angleofrotation, step);
}

bool RWrapper::RW_FocusDepth::Start_AutoContourScan()
{
	try
	{
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
		if(!FOCUSCALCOBJECT->StartAutoContourScan())
		{
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
			FOCUSCALCOBJECT->CurrentFocusType = RapidEnums::RAPIDFOCUSMODE::NOTHING;
			FOCUSCALCOBJECT->FocusOnStatus(false);
			return false;
		}
		return true;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0007", ex); }
}

void RWrapper::RW_FocusDepth::Continue_AutoContourScan(bool conti)
{
	if(conti)
	{
		if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
			RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
		FOCUSCALCOBJECT->ContinueAutoContourScan();
	}
	else
	{
		RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
	}
}

void RWrapper::RW_FocusDepth::Stop_AutoContourScan()
{
	RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(2);
	FOCUSCALCOBJECT->AutoContourClearAll();
}

void RWrapper::RW_FocusDepth::Rotate_AutoContourScan(double angleofrotation, int step)
{
	FOCUSCALCOBJECT->RotateInContourScan(angleofrotation, step);
}

void RWrapper::RW_FocusDepth::AFS_GotoNextScanPosition()
{
	try
	{
		double target[4] = {FOCUSCALCOBJECT->AFSGotoPosition.X, FOCUSCALCOBJECT->AFSGotoPosition.Y, FOCUSCALCOBJECT->AFSGotoPosition.Z, RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
		double feedrate[4] = {5, 5, 5, 5};
		if(RWrapper::RW_MainInterface::MYINSTANCE()->IsMultiLevelZoom)
		   target[3] = RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3];
		RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&target[0], &feedrate[0], TargetReachedCallback::FOCUSDEPTH_AFSMODEPOSITION);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0007", ex); }
}

void RWrapper::RW_FocusDepth::AFS_ScanCurrentPosition(bool Startfocus)
{
	try
	{
		if(FOCUSCALCOBJECT->AutoAdjustLighting())
		{
			cli::array<int, 1>^ lightvalue =  RWrapper::RW_CNC::MYINSTANCE()->CurrentLightProperty;
			if(lightvalue[4] != 0)
			{
				lightvalue[4] = 0;
				RWrapper::RW_CNC::MYINSTANCE()->UpdateLight(lightvalue);
			}
			bool checkImageGetFlag = MAINDllOBJECT->SetImageInEdgeDetection();
			double avg = 0;
			bool inlimit1 = true, inlimit2 = true, inlimit3 = true; 
			int MinAvgValue = 20, MaxAvgValue = 190;
			if(checkImageGetFlag)
			{
				avg = MAINDllOBJECT->GetAvgOfImage();		
				int incrementer = 1; 
				//Let us increase light value by 1 and see if we get better illumination
				while (inlimit1)
				{
					if (avg > MaxAvgValue)
						incrementer = -1;
					else if (avg < MinAvgValue)
						incrementer = 1;
					else
						break;

					for (int i = 0; i < 4; i++)
						lightvalue[i] += incrementer;  // = 4;

					if (lightvalue[0] >= 64 || lightvalue[0] < 1)
					{
						//for (int i = 0; i < 4; i++) { lightvalue[i] = 64; }
						inlimit1 = false;
					}
					else
					{
						RWrapper::RW_CNC::MYINSTANCE()->UpdateLight(lightvalue);
						MAINDllOBJECT->SetImageInEdgeDetection();
						avg = MAINDllOBJECT->GetAvgOfImage();
					}
				}

				if (avg < MinAvgValue || avg > MaxAvgValue)
				{
					//We still did not get optimal lighting. let us now try aux light if its present. 
					if (RWrapper::RW_DBSettings::MYINSTANCE()->AuxilaryLightPresent)
					{
						while (inlimit2)
						{
							if (avg > MaxAvgValue)
								incrementer = -1;
							else if (avg < MinAvgValue)
								incrementer = 1;
							else
								break;

							lightvalue[5] += incrementer;
							if (lightvalue[5] >= 64 || lightvalue[5] < 1)
							{
								inlimit2 = false;
							}
							else
							{
								RWrapper::RW_CNC::MYINSTANCE()->UpdateLight(lightvalue);
								MAINDllOBJECT->SetImageInEdgeDetection();
								avg = MAINDllOBJECT->GetAvgOfImage();
							}
						}
					}
				}

				if (avg < MinAvgValue || avg > MaxAvgValue)
				{
					//We still did not get optimal lighting. let us now try aux light if its present. 
					if (RWrapper::RW_DBSettings::MYINSTANCE()->CoAxialLightPresent)
					{
						while (inlimit3)
						{
							if (avg > MaxAvgValue)
								incrementer = -1;
							else if (avg < MinAvgValue)
								incrementer = 1;
							else
								break;

							lightvalue[6] += incrementer;
							if (lightvalue[6] >= 64 || lightvalue[6] < 1)
							{
								inlimit3 = false;
							}
							else
							{
								RWrapper::RW_CNC::MYINSTANCE()->UpdateLight(lightvalue);
								MAINDllOBJECT->SetImageInEdgeDetection();
								avg = MAINDllOBJECT->GetAvgOfImage();
							}
						}
					}
				}
				//if(avg < 90)
				//{
				//	lightvalue[0] = lightvalue[1] = lightvalue[2] = lightvalue[3];
				//	if(RWrapper::RW_DBSettings::MYINSTANCE()->AuxilaryLightPresent)
				//	{
				//		while(avg < 90 && inlimit3)
				//		{
				//			lightvalue[5] += 4;
				//			if(lightvalue[5] >= 64)
				//			{
				//				lightvalue[5] = 64;
				//				inlimit3 = false;
				//			}
				//			RWrapper::RW_CNC::MYINSTANCE()->UpdateLight(lightvalue);
				//			MAINDllOBJECT->SetImageInEdgeDetection();
				//			avg = MAINDllOBJECT->GetAvgOfImage();
				//		}
				//	}
				//	if(RWrapper::RW_DBSettings::MYINSTANCE()->CoAxialLightPresent)
				//	{
				//		while(avg < 90 && (inlimit2 || inlimit1))
				//		{	 
				//			if(inlimit2)
				//			{
				//				lightvalue[6] += 4;
				//				if(lightvalue[6] >= 64)
				//				{
				//					lightvalue[6] = 64;
				//					inlimit2 = false;
				//				}
				//			}
				//			if(inlimit1)
				//			{
				//				for (int i = 0; i < 4; i ++) {lightvalue[i] += 4;}
				//				if(lightvalue[0] >= 64)
				//				{
				//					for (int i = 0; i < 4; i ++) {lightvalue[i] = 64;}
				//					inlimit1 = false;
				//				}
				//			}
				//			RWrapper::RW_CNC::MYINSTANCE()->UpdateLight(lightvalue);
				//			MAINDllOBJECT->SetImageInEdgeDetection();
				//			avg = MAINDllOBJECT->GetAvgOfImage();
				//		}
				//	}
				//	else
				//	{
				//		while(avg < 90 && inlimit1)
				//		{
				//			for (int i = 0; i < 4; i ++) {lightvalue[i] += 4;}
				//			if(lightvalue[0] >= 64)
				//			{
				//				for (int i = 0; i < 4; i ++) {lightvalue[i] = 64;}
				//				inlimit1 = false;
				//			}
				//			RWrapper::RW_CNC::MYINSTANCE()->UpdateLight(lightvalue);
				//			MAINDllOBJECT->SetImageInEdgeDetection();
				//			avg = MAINDllOBJECT->GetAvgOfImage();
				//		}			
				//	}
				//} 
				//else if(avg > 190)
				//{
				//	lightvalue[0] = lightvalue[1] = lightvalue[2] = lightvalue[3];
				//	if(RWrapper::RW_DBSettings::MYINSTANCE()->AuxilaryLightPresent)
				//	{
				//		while(avg > 190 && inlimt3)
				//		{
				//			lightvalue[5] -= 4;
				//			if(lightvalue[5] <= 0)
				//			{
				//				lightvalue[5] = 0;
				//				inlimt3 = false;
				//			}
				//			RWrapper::RW_CNC::MYINSTANCE()->UpdateLight(lightvalue);
				//			MAINDllOBJECT->SetImageInEdgeDetection();
				//			avg = MAINDllOBJECT->GetAvgOfImage();
				//		}
				//	}
				//	if(RWrapper::RW_DBSettings::MYINSTANCE()->CoAxialLightPresent)
				//	{
				//		while(avg > 190 && (inlimit2 || inlimit1))
				//		{	 
				//			if(inlimit2)
				//			{
				//				lightvalue[6] -= 4;
				//				if(lightvalue[6] <= 0)
				//				{
				//					lightvalue[6] = 0;
				//					inlimit2 = false;
				//				}
				//			}
				//			if(inlimit1)
				//			{
				//				for (int i = 0; i < 4; i ++) {lightvalue[i] -= 4;}
				//				if(lightvalue[0] <= 0)
				//				{
				//					for (int i = 0; i < 4; i ++) {lightvalue[i] = 0;}
				//					inlimit1 = false;
				//				}
				//			}
				//			RWrapper::RW_CNC::MYINSTANCE()->UpdateLight(lightvalue);
				//			MAINDllOBJECT->SetImageInEdgeDetection();
				//			avg = MAINDllOBJECT->GetAvgOfImage();
				//		}
				//	}
				//	else
				//	{
				//		while(avg > 190 && inlimit1)
				//		{
				//			for(int i = 0; i < 4; i ++) 
				//			{
				//				lightvalue[i] -= 4;
				//				if(lightvalue[i] <= 4){lightvalue[i] = 4;}
				//			}
				//			if(lightvalue[0] == 4 && lightvalue[1] == 4 && lightvalue[2] == 4 &&lightvalue[3] == 4)
				//				inlimit1 = false;
				//			RWrapper::RW_CNC::MYINSTANCE()->UpdateLight(lightvalue);
				//			MAINDllOBJECT->SetImageInEdgeDetection();
				//			avg = MAINDllOBJECT->GetAvgOfImage();
				//		}		
				//	}
				//}
			}
		}
		if(Startfocus)
			StartFocus();
		else
			RWrapper::RW_VBlockCallibration::MYINSTANCE()->Get_ZvalueOfCylinderAxis();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0007", ex); }
}

void RWrapper::RW_FocusDepth::SetShapeForDepth(int i)
{
	try
	{
		FOCUSCALCOBJECT->ShapeForDepth = FocusFunctions::FOCUSDEPTHSHAPETYPE(i);
		if(FOCUSCALCOBJECT->ShapeForDepth == FocusFunctions::FOCUSDEPTHSHAPETYPE::POINTSHAPE)
			SetFocusRectangleProperties(1, 1, 0, 40, 40, 0.8, 0.2);
		else
			SetFocusRectangleProperties(3, 3, 0, 40, 40, 0.8, 0.2);
	}
	catch(Exception^ ex)
	{ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRFD0007b", ex); }
}

//void RWrapper::RW_FocusDepth::SetPositionForRefDot()
//{
//
//}
#include "Stdafx.h"
#include "RW_MainInterface.h"
#include "..\MAINDLL\Measurement\DimThread.h"
#include "..\MAINDLL\Measurement\DimChildMeasurement.h"
#include "..\MAINDll\Engine\ThreadCalcFunctions.h"

RWrapper::RW_Thread::RW_Thread()
{
	try
	{
		this->ThreadInstance = this;	
		this->MajorMinorDiaMode = false;
		this->CurrentMeasureThCount = 2;
		this->ThreadMajorDia = ""; this->ThreadMinorDia = ""; this->ThreadEffDiaG = "";
		this->ThreadEffDiaM = ""; this->ThreadPitch = ""; this->ThreadAngle = "";
		this->ThreadTopRadius = ""; this->ThreadRootRadius = "";
		this->ThreadMeasureTable = gcnew System::Data::DataTable();
		this->ThreadMeasureTable->Columns->Add("");
		this->ThreadMeasureTable->Columns->Add("");
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRTHRD0001", ex); }
}

RWrapper::RW_Thread::~RW_Thread()
{
}

RWrapper::RW_Thread^ RWrapper::RW_Thread::MYINSTANCE()
{
	return ThreadInstance;
}

void RWrapper::RW_Thread::IncludeMajorMinorDia(bool flag)
{
	THREADCALCOBJECT->IncludeMajorMinroDiaMeasure(flag);
}

void RWrapper::RW_Thread::ExternalThread(bool flag)
{
	THREADCALCOBJECT->ExternalThreadMeasure(flag);
}

void RWrapper::RW_Thread::NewClicked(bool TopSurfaceStatus, bool RootSurfaceStatus)
{
	MAINDllOBJECT->ActionSavingFlag = true;
	THREADCALCOBJECT->AddNewThreadMeasurement(TopSurfaceStatus, RootSurfaceStatus);
	MAINDllOBJECT->ActionSavingFlag = false;
}

void RWrapper::RW_Thread::UpdateThreadCountChange(int ThCount, bool TopSurfaceStatus, bool RootSurfaceStatus)
{
	try
	{
		this->CurrentMeasureThCount = ThCount;
		if(THREADCALCOBJECT->ThreadMeasurementModeStarted())
		{
			if(MAINDllOBJECT->ShowMsgBoxString("RW_ThreadUpdateThreadCountChange01", RapidEnums::MSGBOXTYPE::MSG_YES_NO, RapidEnums::MSGBOXICONTYPE::MSG_QUESTION))
			{
				if(ThCount < 2) ThCount = 2;
				if(ThCount > 15) ThCount = 15;
				THREADCALCOBJECT->InitialiseThreadShapes(ThCount, TopSurfaceStatus, RootSurfaceStatus);
			}
		}
		else
		{
			if(ThCount < 2) ThCount = 2;
			if(ThCount > 15) ThCount = 15;
			THREADCALCOBJECT->InitialiseThreadShapes(ThCount, TopSurfaceStatus, RootSurfaceStatus);
		}
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRTHRD0002", ex); }
}

void RWrapper::RW_Thread::UpdateThreadGraphics()
{
	/*THREADCALCOBJECT->Wait_ThreadGraphicsUpdate();
	THREADCALCOBJECT->Draw_ThreadWindowGraphics();
	THREADCALCOBJECT->Draw_ThreadWindowGraphics();*/
	THREADCALCOBJECT->update_ThreadGraphics();
	//Sleep(1000);
}

bool RWrapper::RW_Thread::CalculateThreadMeasurement()
{
	try
	{
		THREADCALCOBJECT->CalculateThreadParameters();
		THREADCALCOBJECT->ThreadMeasurementMode(false);
		THREADCALCOBJECT->ThreadMeasurementModeStarted(false);
		GetSelectedThreadParameters();	
		return true;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRTHRD0003", ex); }
}

void RWrapper::RW_Thread::GetSelectedThreadParameters()
{
	try
	{
		int CMeasureMode = MAINDllOBJECT->AngleMeasurementMode();
		System::String^ MeasureFormat = "0.0";
		for(int i = 0; i < RWrapper::RW_DBSettings::MYINSTANCE()->MeasureNoOfDec - 1; i++)
			MeasureFormat = MeasureFormat + "0";
		if(THREADCALCOBJECT->CurrentThreadMeasure() == NULL) return;
		DimThread* Cdim = (DimThread*)(THREADCALCOBJECT->CurrentThreadMeasure());

		this->ThreadMajorDia = Microsoft::VisualBasic::Strings::Format(Cdim->DimThrdMajordia->ChildMeasureValue(), MeasureFormat);
		this->ThreadMinorDia = Microsoft::VisualBasic::Strings::Format(Cdim->DimThrdMinordia->ChildMeasureValue(), MeasureFormat);
		this->ThreadEffDiaG = Microsoft::VisualBasic::Strings::Format(Cdim->DimThrdEffdiaG->ChildMeasureValue(), MeasureFormat);
		this->ThreadEffDiaM = Microsoft::VisualBasic::Strings::Format(Cdim->DimThrdEffdiaM->ChildMeasureValue(), MeasureFormat);
		this->ThreadPitch = Microsoft::VisualBasic::Strings::Format(Cdim->DimThrdPitch->ChildMeasureValue(), MeasureFormat);
		this->ThreadTopRadius = Microsoft::VisualBasic::Strings::Format(Cdim->DimThrdTopRadius->ChildMeasureValue(), MeasureFormat);
		this->ThreadRootRadius = Microsoft::VisualBasic::Strings::Format(Cdim->DimThrdRootRadius->ChildMeasureValue(), MeasureFormat);
		this->ThreadPitchDia = Microsoft::VisualBasic::Strings::Format(Cdim->DimThrdPitchDia->ChildMeasureValue(), MeasureFormat);
		this->ThreadAngle = RWrapper::RW_MeasureParameter::MYINSTANCE()->Convert_AngleFormat(Cdim->DimThrdAngle->ChildMeasureValue(), CMeasureMode, MeasureFormat, true);
		this->ThreadMeasureTable->Rows->Clear();
		this->ThreadMeasureTable->Rows->Add("Major Dia", ThreadMajorDia);
		this->ThreadMeasureTable->Rows->Add("Minor Dia", ThreadMinorDia);
		this->ThreadMeasureTable->Rows->Add("Eff Dia_G", ThreadEffDiaG);
		this->ThreadMeasureTable->Rows->Add("Eff Dia_M", ThreadEffDiaM);
		//this->ThreadMeasureTable->Rows->Add("Pitch_Dia", ThreadPitchDia);
		this->ThreadMeasureTable->Rows->Add("Pitch", ThreadPitch);
		this->ThreadMeasureTable->Rows->Add("Angle", ThreadAngle);
		this->CurrentMeasureThCount = THREADCALCOBJECT->CurrentSelectedThreadAction->ThreadCount();
		this->MajorMinorDiaMode = THREADCALCOBJECT->CurrentSelectedThreadAction->IncludeMajorMinorDia();
		ThreadMeasureSelectionChanged::raise();
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRTHRD0004", ex); }
}



void RWrapper::RW_Thread::OnThreadLeftMouseDown(double x, double y)
{
	THREADCALCOBJECT->OnLeftMouseDown_Thread(x, y);
}

void RWrapper::RW_Thread::OnThreadRightMouseDown(double x, double y)
{
	THREADCALCOBJECT->OnRightMouseDown_Thread(x, y);
}

void RWrapper::RW_Thread::OnThreadMiddleMouseDown(double x, double y)
{
	THREADCALCOBJECT->OnMiddleMouseDown_Thread(x, y);
}

void RWrapper::RW_Thread::OnThreadLeftMouseUp(double x, double y)
{
	THREADCALCOBJECT->OnLeftMouseUp_Thread(x, y);
}

void RWrapper::RW_Thread::OnThreadRightMouseUp(double x, double y)
{
	THREADCALCOBJECT->OnRightMouseUp_Thread(x, y);
}

void RWrapper::RW_Thread::OnThreadMiddleMouseUp(double x, double y)
{
	THREADCALCOBJECT->OnMiddleMouseUp_Thread(x, y);
}

void RWrapper::RW_Thread::OnThreadMouseMove(double x, double y)
{
	THREADCALCOBJECT->OnMouseMove_Thread(x, y);
}

void RWrapper::RW_Thread::OnThreadMouseWheel(int evalue)
{
	THREADCALCOBJECT->OnMouseWheel_Thread(evalue);
}

void RWrapper::RW_Thread::OnThreadMouseEnter()
{
	THREADCALCOBJECT->OnMouseEnter_Thread();
}

void RWrapper::RW_Thread::OnThreadMouseLeave()
{
	THREADCALCOBJECT->OnMouseLeave_Thread();
}
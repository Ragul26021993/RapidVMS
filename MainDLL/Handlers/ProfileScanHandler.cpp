#include "StdAfx.h"
#include "ProfileScanHandler.h"
#include "..\Engine\RCadApp.h"
#include "..\Helper\Helper.h"
#include "..\MAINDLL\Actions\ProfileScanAction.h"
#include "..\MAINDll\Engine\PartProgramFunctions.h"

//Default handler .. do nothing.....!!!!
ProfileScanHandler::ProfileScanHandler()
{
	setMaxClicks(3);
}

ProfileScanHandler::~ProfileScanHandler()
{
}

void ProfileScanHandler::SetProfileScanParam()
{
	try
	{
		HELPEROBJECT->profileScanPtsCnt = 2;
		if(HELPEROBJECT->isFirstFrame)
		{
			HELPEROBJECT->ProfileScanDroPt = MAINDllOBJECT->getCurrentDRO();
			double X1 = HELPEROBJECT->profileScanPixelPt[0] * MAINDllOBJECT->getWindow(0).getUppX() + MAINDllOBJECT->getWindow(0).getCam().x;
			double Y1 = MAINDllOBJECT->getWindow(0).getCam().y - HELPEROBJECT->profileScanPixelPt[1] * MAINDllOBJECT->getWindow(0).getUppY();
			double X2 = HELPEROBJECT->profileScanPixelPt[2] * MAINDllOBJECT->getWindow(0).getUppX() + MAINDllOBJECT->getWindow(0).getCam().x;
			double Y2 = MAINDllOBJECT->getWindow(0).getCam().y - HELPEROBJECT->profileScanPixelPt[3] * MAINDllOBJECT->getWindow(0).getUppY();
			HELPEROBJECT->ProfileScanFirstPt.set(X1, Y1);
			HELPEROBJECT->ProfileScanSecondPt.set(X2, Y2);
		}
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
		HELPEROBJECT->ProfileScan();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ProfileScanH0001", __FILE__, __FUNCSIG__); }
}


void ProfileScanHandler::LmouseDown()
{
	try
	{
		double uppx = MAINDllOBJECT->getWindow(0).getUppX();
		double uppy = MAINDllOBJECT->getWindow(0).getUppY();

		if(HELPEROBJECT->profileScanPtsCnt == 1)
		{
			point1 = getClicksValue(getClicksDone() - 1);
			HELPEROBJECT->profileScanPixelPt[0] = (point1.getX() - MAINDllOBJECT->getWindow(0).getCam().x) / uppx;
			HELPEROBJECT->profileScanPixelPt[1] = (MAINDllOBJECT->getWindow(0).getCam().y - point1.getY()) / uppy;
			if(HELPEROBJECT->isFirstFrame)
			{
				HELPEROBJECT->ProfileScanFirstPt = point1;
				HELPEROBJECT->ProfileScanDroPt = MAINDllOBJECT->getCurrentDRO();
			}
		}
		else if(HELPEROBJECT->profileScanPtsCnt == 2)
		{
			point2 = getClicksValue(getClicksDone() - 1);
			HELPEROBJECT->profileScanPixelPt[2] = (point2.getX() - MAINDllOBJECT->getWindow(0).getCam().x) / uppx;
			HELPEROBJECT->profileScanPixelPt[3] = (MAINDllOBJECT->getWindow(0).getCam().y - point2.getY()) / uppy;		
			if(HELPEROBJECT->isFirstFrame)
				HELPEROBJECT->ProfileScanSecondPt = point2;
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
			HELPEROBJECT->ProfileScan();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ProfileScanH0001", __FILE__, __FUNCSIG__); }
}

void ProfileScanHandler::mouseMove()
{
	try
	{
		if(getClicksDone() == 1)
		{
			point2.set(getClicksValue(getClicksDone()));
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ProfileScanH0002", __FILE__, __FUNCSIG__); }
}	

void ProfileScanHandler::draw(int windowno, double WPixelWidth)
{
	try
	{
		if(getClicksDone() == 1)
		{
			GRAFIX->SetColor_Double(0, 1, 0);
			GRAFIX->drawRectangle(point1.getX(), point1.getY(), point2.getX(), point2.getY());
			GRAFIX->drawPartprogramPathArrow(point1.getX(), point1.getY(), point2.getX(), point2.getY(), MAINDllOBJECT->getWindow(0).getUppX());
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ProfileScanH0003", __FILE__, __FUNCSIG__); }
}

void ProfileScanHandler::LmaxmouseDown()
{
	try
	{
		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ProfileScanH0004", __FILE__, __FUNCSIG__); }
}

void ProfileScanHandler::EscapebuttonPress()
{
	HELPEROBJECT->profileScanPtsCnt = 0;
}

void ProfileScanHandler::PartProgramData()
{
	try
	{
		if(PPCALCOBJECT->getCActionType() == RapidEnums::ACTIONTYPE::PROFILE_SCAN_ACTION)
		{
			Vector firstPt = *((ProfileScanAction*)PPCALCOBJECT->getCurrentAction())->GetFirstPt();
			Vector SecondPt = *((ProfileScanAction*)PPCALCOBJECT->getCurrentAction())->GetSecondPt();
			HELPEROBJECT->ProfileLastPtVector = ((ProfileScanAction*)PPCALCOBJECT->getCurrentAction())->GetEndPt();
			HELPEROBJECT->ProfileScanJumpParam = ((ProfileScanAction*)PPCALCOBJECT->getCurrentAction())->GetPixelCount();
			HELPEROBJECT->Scan_Complete_Profile = ((ProfileScanAction*)PPCALCOBJECT->getCurrentAction())->GetProfileInfo();
			bool SurfaceLight = ((ProfileScanAction*)PPCALCOBJECT->getCurrentAction())->GetSurfaceAlgoParam(HELPEROBJECT->SurfaceAlgoParam);
			MAINDllOBJECT->SetSurfaceLightFlag(SurfaceLight);
			
			if(!HELPEROBJECT->Scan_Complete_Profile)
			{
				HELPEROBJECT->ProfileScanStart_Pt.X = HELPEROBJECT->ProfileLastPtVector->getX();
				HELPEROBJECT->ProfileScanStart_Pt.Y = HELPEROBJECT->ProfileLastPtVector->getY();
				HELPEROBJECT->profileScanEndPtPassed = true;
			}
			double uppx = MAINDllOBJECT->getWindow(0).getUppX();// / HELPEROBJECT->CamSizeRatio;
			double uppy = MAINDllOBJECT->getWindow(0).getUppY();// / HELPEROBJECT->CamSizeRatio;
			HELPEROBJECT->profileScanPixelPt[0] = (firstPt.getX() - MAINDllOBJECT->getWindow(0).getCam().x) / uppx;
			HELPEROBJECT->profileScanPixelPt[1] = (MAINDllOBJECT->getWindow(0).getCam().y - firstPt.getY()) / uppy;
			HELPEROBJECT->profileScanPixelPt[2] = (SecondPt.getX() - MAINDllOBJECT->getWindow(0).getCam().x) / uppx;
			HELPEROBJECT->profileScanPixelPt[3] = (MAINDllOBJECT->getWindow(0).getCam().y - SecondPt.getY()) / uppy;
			HELPEROBJECT->profile_scan_auto_thread = false;
			HELPEROBJECT->profile_scan_stopped = false;
			HELPEROBJECT->profile_scan_paused = false;
			HELPEROBJECT->profile_scan_failed = true;
			HELPEROBJECT->skip_cont_constraint = true;
			HELPEROBJECT->profile_scan_running = false;
			HELPEROBJECT->profile_scan_ended = false;
			HELPEROBJECT->profileScanPtsCnt = 0;
			HELPEROBJECT->isFirstFrame = true;		
			HELPEROBJECT->ProfileScan();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ProfileScanH0005", __FILE__, __FUNCSIG__); }
}
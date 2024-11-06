#include "StdAfx.h"
#include "RW_MainInterface.h"
#include "RW_AutoProfileScanMeasure.h"
#include "..\MAINDll\Shapes\CloudPoints.h"
#include "..\MAINDll\Helper\Helper.h"
#include "..\MAINDll\Engine\RCadApp.h"
#include "..\MAINDll\Handlers\ProfileScanHandler.h"

RWrapper::RW_AutoProfileScanMeasure::RW_AutoProfileScanMeasure()
{
	try
	{
		Init();
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRAPSM0000", ex);
	}
}

RWrapper::RW_AutoProfileScanMeasure::~RW_AutoProfileScanMeasure()
{
}

bool RWrapper::RW_AutoProfileScanMeasure::Check_AutoProfileScan_Instance()
{
	if(AutoProfileScanInstance == nullptr)
		return false;
	else
		return true;
}

RWrapper::RW_AutoProfileScanMeasure^ RWrapper::RW_AutoProfileScanMeasure::MYINSTANCE()
{
	if(AutoProfileScanInstance == nullptr)
		AutoProfileScanInstance = gcnew RW_AutoProfileScanMeasure();
	return AutoProfileScanInstance;
}

void RWrapper::RW_AutoProfileScanMeasure::ClearAll()
{
	try
	{
		Init();
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRAPSM0001", ex);
	}
}

void RWrapper::RW_AutoProfileScanMeasure::StartAutoProfileScan(double width, int BestFitNumberOfPts, int Jump, int MeasureMentCount, bool ArcBestFitType, bool _autoContinue)
{
	try
	{
		this->Init();
		this->Width = width; this->MeasureCount = MeasureMentCount;
		this->ArcBestFit = ArcBestFitType;
		this->NumberOfPts = BestFitNumberOfPts; this->SkipParam = Jump;
		Shape* Cshape = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		if(Cshape != NULL)
		{
			if(Cshape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
			{
				ParentShape1 = Cshape;
			}
		}
		if(ParentShape1 == NULL)
		{
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CLOUDPOINTS_HANDLER);
			ParentShape1 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		}
		HELPEROBJECT->AutoProfileScanEnable = true;
		autoContinue = _autoContinue;
		RWrapper::RW_MainInterface::MYINSTANCE()->SetProfileScanParam(SkipParam);
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRAPSM0002", ex);
	}
}

void RWrapper::RW_AutoProfileScanMeasure::Init()
{
	try
	{
		this->Width = 0; this->MeasureCount = 0;
		this->ArcBestFit = false;
		this->NumberOfPts = 0; this->SkipParam = 0;
		this->ParentShape1 = NULL; this->ParentShape2 = NULL;
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRAPSM0003", ex);
	}
}

void RWrapper::RW_AutoProfileScanMeasure::NextprofileScan()
{
	try
	{
		if(ParentShape2 == NULL)
		{
			double Target[4] = {HELPEROBJECT->ProfileScanDroPt.getX(), HELPEROBJECT->ProfileScanDroPt.getY() + Width, HELPEROBJECT->ProfileScanDroPt.getZ(), RWrapper::RW_DRO::MYINSTANCE()->DROCoordinate[3]};
			double feedrate[4] = {RWrapper::RW_DRO::MYINSTANCE()->feedRate[0], RWrapper::RW_DRO::MYINSTANCE()->feedRate[1],RWrapper::RW_DRO::MYINSTANCE()->feedRate[2], RWrapper::RW_DRO::MYINSTANCE()->feedRate[3]};		
			if(!RWrapper::RW_CNC::MYINSTANCE()->getCNCMode())
				RWrapper::RW_CNC::MYINSTANCE()->RaiseCNCEvents(1);
			RWrapper::RW_CNC::MYINSTANCE()->SendDROGotoPosition(&Target[0], &feedrate[0], RWrapper::RW_DRO::MYINSTANCE()->TargetReachedCallback::NEXT_PROFILE_SCAN);
		}
		else
		{
			if(ParentShape1 != NULL)
			{
				MAINDllOBJECT->ClearShapeSelections();
				std::list<int> ShapeIdList;
				ShapeIdList.push_back(ParentShape1->getId());
				ShapeIdList.push_back(ParentShape2->getId());
				MAINDllOBJECT->selectShape(&ShapeIdList);
				MAINDllOBJECT->CompareCloudPts(MeasureCount, ArcBestFit, NumberOfPts);

				MAINDllOBJECT->ClearShapeSelections();
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::MEASURE_HANDLER);
				MAINDllOBJECT->selectShape(ParentShape1->getId(), false);
				MAINDllOBJECT->MeasurementForHighlightedShape(1, true);

				MAINDllOBJECT->ClearShapeSelections();
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER);
				MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::MEASURE_HANDLER);
				MAINDllOBJECT->selectShape(ParentShape2->getId(), false);
				MAINDllOBJECT->MeasurementForHighlightedShape(1, true);
				this->Init();
			}
		}
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRAPSM0004", ex);
	}
}

void RWrapper::RW_AutoProfileScanMeasure::StartNextProfileScan()
{
	try
	{
		if(ParentShape2 == NULL)
		{
			MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::CLOUDPOINTS_HANDLER);
			ParentShape2 = (Shape*)MAINDllOBJECT->getShapesList().selectedItem();
		}
		HELPEROBJECT->AutoProfileScanEnable = true;
		RWrapper::RW_MainInterface::MYINSTANCE()->SetProfileScanParam(SkipParam);
		if(autoContinue)/*!MAINDllOBJECT->getSurface())*/
		{
			((ProfileScanHandler*)MAINDllOBJECT->getCurrentHandler())->SetProfileScanParam();
		}
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRAPSM0005", ex);
	}
}
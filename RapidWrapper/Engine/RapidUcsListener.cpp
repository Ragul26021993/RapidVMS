#include "StdAfx.h"
#include "RW_MainInterface.h"
#include "..\MAINDll\Engine\PartProgramFunctions.h"
RapidUcsListener::RapidUcsListener()
{
}

RapidUcsListener::~RapidUcsListener() 
{
}


void RapidUcsListener::UpdateUCSName(BaseItem* item)
{
	try
	{
		RapidEnums::RAPIDPROJECTIONTYPE Uptype = RapidEnums::RAPIDPROJECTIONTYPE(((UCS*)(item))->UCSProjectionType());
		if(Uptype == RapidEnums::RAPIDPROJECTIONTYPE::XY)
			RWrapper::RW_MainInterface::EntityName += " | XY";
		else if(Uptype == RapidEnums::RAPIDPROJECTIONTYPE::YZ)
			RWrapper::RW_MainInterface::EntityName += " | YZ";
		else if(Uptype == RapidEnums::RAPIDPROJECTIONTYPE::XZ)
			RWrapper::RW_MainInterface::EntityName += " | XZ";
		else if (Uptype == RapidEnums::RAPIDPROJECTIONTYPE::YZm)
			RWrapper::RW_MainInterface::EntityName += " | YZ-";
		else if (Uptype == RapidEnums::RAPIDPROJECTIONTYPE::XZm)
			RWrapper::RW_MainInterface::EntityName += " | XZ-";
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RUCSL0000", ex); }
}

//New Ucs Added
void RapidUcsListener::itemAdded(BaseItem* item, Listenable* sender)
{
	try
	{
		RWrapper::RW_MainInterface::EntityName = gcnew System::String(item->getModifiedName());
		UpdateUCSName(item);
		RWrapper::RW_MainInterface::EntityID = item->getId();
		UCS* ucs = static_cast<UCS*>(item);
		if(ucs->AddingFirstTime())
		{
			ucs->getShapes().addListener(new RapidShapeListener());
			ucs->getDimensions().addListener(new RapidDimListener());
			ucs->getDxfCollecion().addListener(new RapidDxfListener());
		}
		RWrapper::RW_UCSParameter::MYINSTANCE()->RaiseUCSChangedEvents(RWrapper::RW_Enum::RW_UCSPARAMETER_TYPE::UCS_ADDED);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RUCSL0001", ex); }
}

//UCs Cemoved..
void RapidUcsListener::itemRemoved(BaseItem* item, Listenable* sender)
{
	try
	{
		RWrapper::RW_MainInterface::EntityName = gcnew System::String(item->getModifiedName());
		UpdateUCSName(item);
		RWrapper::RW_MainInterface::EntityID = item->getId();
		RWrapper::RW_UCSParameter::MYINSTANCE()->RaiseUCSChangedEvents(RWrapper::RW_Enum::RW_UCSPARAMETER_TYPE::UCS_REMOVED);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RUCSL0002", ex); }
}

//Item Changed
void RapidUcsListener::itemChanged(BaseItem* item, Listenable* sender)
{
	try{ }
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RUCSL0003", ex); }
}

//UCS selection changed event..//
void RapidUcsListener::selectionChanged(Listenable* sender)
{
	try
	{
		RCollectionBase& Shapecoll = MAINDllOBJECT->getShapesList();
		RCollectionBase& Shapecollselected = MAINDllOBJECT->getSelectedShapesList();
		RCollectionBase& Measurecoll = MAINDllOBJECT->getDimList();
		RCollectionBase& Measurecollselected = MAINDllOBJECT->getSelectedDimList();
		RCollectionBase& Dxfcoll = MAINDllOBJECT->getDXFList();
		RCollectionBase& Dxfcollselected = MAINDllOBJECT->getSelectedDXFList();
		
		BaseItem* Cucs_item = MAINDllOBJECT->getUCSList().selectedItem();
		RWrapper::RW_MainInterface::EntityName = gcnew System::String(Cucs_item->getModifiedName());

		UpdateUCSName(Cucs_item);
		RWrapper::RW_UCSParameter::MYINSTANCE()->RaiseUCSChangedEvents(RWrapper::RW_Enum::RW_UCSPARAMETER_TYPE::UCS_SELECTION_CHANGED);
	
		//Added by Rahul on 19 may 12..........
		RWrapper::RW_ShapeParameter::MYINSTANCE()->SetShapeParamForTable();
		RWrapper::RW_ShapeParameter::MYINSTANCE()->RaiseShapeChangedEvents(RWrapper::RW_Enum::RW_SHAPEPARAMTYPE::SHAPE_SELECTION_CHANGED);
		RWrapper::RW_ShapeParameter::MYINSTANCE()->UpdateSelectedShape_PointsTable();	

		RWrapper::RW_MeasureParameter::MYINSTANCE()->SetMeasurementParamForTable();

		if(PPCALCOBJECT->IsPartProgramRunning())return;

		RWrapper::RW_MainInterface::EntityIDList->Clear();
		RWrapper::RW_MainInterface::EntityNameList->Clear();
		for(RC_ITER i = Dxfcoll.getList().begin(); i != Dxfcoll.getList().end(); i++)
		{
			BaseItem* item = &(*i->second);
			RWrapper::RW_MainInterface::EntityIDList->Add(item->getId());
			RWrapper::RW_MainInterface::EntityNameList->Add(gcnew System::String(item->getModifiedName()));
		}
		RWrapper::RW_UCSParameter::MYINSTANCE()->RaiseUCSChangedEvents(RWrapper::RW_Enum::RW_UCSPARAMETER_TYPE::CHANGE_DXF_LIST);

		RWrapper::RW_MainInterface::EntityIDList->Clear();
		RWrapper::RW_MainInterface::EntityNameList->Clear();
		for(RC_ITER i = Dxfcollselected.getList().begin(); i != Dxfcollselected.getList().end(); i++)
			RWrapper::RW_MainInterface::EntityIDList->Add((*i->second).getId());
		RWrapper::RW_UCSParameter::MYINSTANCE()->SelectingDxfMode = true;
		RWrapper::RW_UCSParameter::MYINSTANCE()->RaiseDXFChangedEvents(RWrapper::RW_Enum::RW_DXFPARAMETER_TYPE::DXF_SELECTION_CHANGED);
		RWrapper::RW_UCSParameter::MYINSTANCE()->SelectingDxfMode = false;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RUCSL0004", ex); }
}
#include "StdAfx.h"
#include "RW_MainInterface.h"

RapidDxfListener::RapidDxfListener()
{
}

RapidDxfListener::~RapidDxfListener()
{
}

//Dxf Shape collection Added
void RapidDxfListener::itemAdded(BaseItem* item, Listenable* sender)
{
	try
	{
		RWrapper::RW_MainInterface::EntityName = gcnew System::String(item->getOriginalName());
		RWrapper::RW_MainInterface::EntityID = item->getId();
		RWrapper::RW_UCSParameter::MYINSTANCE()->RaiseDXFChangedEvents(RWrapper::RW_Enum::RW_DXFPARAMETER_TYPE::DXF_ADDED);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RDXFL0001", ex);	}
}

//Dxf Shape collection Removed
void RapidDxfListener::itemRemoved(BaseItem* item, Listenable* sender)
{
	try
	{
		RWrapper::RW_MainInterface::EntityName = gcnew System::String(item->getOriginalName());
		RWrapper::RW_MainInterface::EntityID = item->getId();
		RWrapper::RW_UCSParameter::MYINSTANCE()->RaiseDXFChangedEvents(RWrapper::RW_Enum::RW_DXFPARAMETER_TYPE::DXF_REMOVED);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RDXFL0002", ex);	}
}

//Changed..
void RapidDxfListener::itemChanged(BaseItem* item, Listenable* sender)
{
	try{ }
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RDXFL0003", ex);	}
}

//Dxf shape collection selection changed..
void RapidDxfListener::selectionChanged(Listenable* sender)
{
	try
	{
		RWrapper::RW_MainInterface::EntityIDList->Clear();
		RCollectionBase& Dxfcollselected = MAINDllOBJECT->getSelectedDXFList();
		RWrapper::RW_UCSParameter::MYINSTANCE()->SelectingDxfMode = true;
		if(Dxfcollselected.count() < 1)
		{
			RWrapper::RW_MainInterface::EntityID = 0;
			RWrapper::RW_UCSParameter::MYINSTANCE()->RaiseDXFChangedEvents(RWrapper::RW_Enum::RW_DXFPARAMETER_TYPE::DXF_SELECTION_CHANGED);
		}
		else
		{
			for(RC_ITER i = Dxfcollselected.getList().begin(); i != Dxfcollselected.getList().end(); i++)
				RWrapper::RW_MainInterface::EntityIDList->Add((*i->second).getId());
			RWrapper::RW_UCSParameter::MYINSTANCE()->RaiseDXFChangedEvents(RWrapper::RW_Enum::RW_DXFPARAMETER_TYPE::DXF_SELECTION_CHANGED);
		}
		RWrapper::RW_UCSParameter::MYINSTANCE()->SelectingDxfMode = false;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RDXFL0004", ex);	}
}
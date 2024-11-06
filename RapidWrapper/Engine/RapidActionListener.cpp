#include "StdAfx.h"
#include "RW_MainInterface.h"
#include "..\MAINDll\Engine\PartProgramFunctions.h"

RapidActionListener::RapidActionListener()
{
}

RapidActionListener::~RapidActionListener()
{
}

void RapidActionListener::itemAdded(BaseItem* item, Listenable* sender)
{
	try
	{
		RWrapper::RW_ActionParameter::MYINSTANCE()->Action_EntityID = item->getId();
		if(((RAction*)item)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
			MAINDllOBJECT->AddProbePathPoint((RAction*)item);
		if(RWrapper::RW_ActionParameter::MYINSTANCE()->SetActionParamForFrontend(((RAction*)item)))
		      RWrapper::RW_ActionParameter::MYINSTANCE()->RaiseActionChangedEvents(RWrapper::RW_Enum::RW_ACTIONPARAMTYPE::ACTION_ADDED);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RACTIONL0001", ex); }
}

void RapidActionListener::itemRemoved(BaseItem* item, Listenable* sender)
{
	try
	{
		RWrapper::RW_ActionParameter::MYINSTANCE()->Action_EntityID = item->getId();
		if(((RAction*)item)->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION)
		{
			if(MAINDllOBJECT->ProbePathPoints.find(((RAction*)item)->getId()) != MAINDllOBJECT->ProbePathPoints.end())
			{
				delete [] MAINDllOBJECT->ProbePathPoints[((RAction*)item)->getId()];
				MAINDllOBJECT->ProbePathPoints.erase(((RAction*)item)->getId());
			}
		}	
		RWrapper::RW_ActionParameter::MYINSTANCE()->RaiseActionChangedEvents(RWrapper::RW_Enum::RW_ACTIONPARAMTYPE::ACTION_REMOVED);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RACTIONL0002", ex); }	
}

void RapidActionListener::itemChanged(BaseItem* item, Listenable* sender)
{
	try
	{
		if(RWrapper::RW_ActionParameter::MYINSTANCE()->SetActionParamForFrontend(((RAction*)item)))
		     RWrapper::RW_ActionParameter::MYINSTANCE()->RaiseActionChangedEvents(RWrapper::RW_Enum::RW_ACTIONPARAMTYPE::ACTION_UPDATE);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RACTIONL0003", ex); }
}

void RapidActionListener::selectionChanged(Listenable* sender)
{
	try
	{
		//Here we shall access the main action list and not the arranged action list because all action objects have only the main list as their listner
		//So even if the part program is arranged, and arranged list is used for running it, selection change of action objects will trigger the main list
		//And since we are only interested in the ID of the selected action, it dosent matter if we look into main list for it.
		if(PPCALCOBJECT->getPPActionlist().getSelected().count() > 0)
			RWrapper::RW_ActionParameter::MYINSTANCE()->Action_EntityID = PPCALCOBJECT->getPPActionlist().selectedItem()->getId();
		else
			RWrapper::RW_ActionParameter::MYINSTANCE()->Action_EntityID = -1;
		RWrapper::RW_ActionParameter::MYINSTANCE()->RaiseActionChangedEvents(RWrapper::RW_Enum::RW_ACTIONPARAMTYPE::ACTION_SELECTION_CHANGED);
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("RACTIONL0004", ex); }
}
#include "Stdafx.h"
#include "RW_MainInterface.h"
#include "..\MAINDLL\Actions\AddShapeAction.h"
#include "..\MAINDLL\Engine\PartProgramFunctions.h"
#include "..\MAINDLL\Actions\PerimeterAction.h"

RWrapper::RW_ActionParameter::RW_ActionParameter()
{
	try
	{
		this->AcnParamInstance = this;	
		this->ActionParameter_ArrayList = gcnew System::Collections::ArrayList;
		this->Action_EntityID = 0;
	}
	catch(Exception^ ex){ RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRACPRD0001", ex); }
}

RWrapper::RW_ActionParameter::~RW_ActionParameter()
{
}

RWrapper::RW_ActionParameter^ RWrapper::RW_ActionParameter::MYINSTANCE()
{
	return AcnParamInstance;
}

void RWrapper::RW_ActionParameter::RaiseActionChangedEvents(RWrapper::RW_Enum::RW_ACTIONPARAMTYPE actType)
{
	ActionParameterUpdate::raise(actType);		
}

void RWrapper::RW_ActionParameter::DeleteAction(int id)
 {
	 try
	 {
		   if(!MAINDllOBJECT->getActionsHistoryList().ItemExists(id)) return;
	       RAction* Caction = (RAction*)(MAINDllOBJECT->getActionsHistoryList().getList()[id]);
		   MAINDllOBJECT->getActionsHistoryList().removeItem(Caction, true, true);
	       MAINDllOBJECT->Wait_VideoGraphicsUpdate();
	       MAINDllOBJECT->Wait_RcadGraphicsUpdate();
	 }
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRACPRD0002", ex);
	}
 }

void RWrapper::RW_ActionParameter::SetCriticalAction(int id, bool state)
{
	try
	{
		if(!MAINDllOBJECT->getActionsHistoryList().ItemExists(id)) return;
	    RAction* Caction = (RAction*)(MAINDllOBJECT->getActionsHistoryList().getList()[id]);
    	if(Caction->CurrentActionType == RapidEnums::ACTIONTYPE::ADDPOINTACTION)
		{
		    ((AddPointAction*)Caction)->CriticalAction = state;
		}
	}
    catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRACPRD0003", ex);
	}
}

void RWrapper::RW_ActionParameter::RerunAction(int id)
{
	try
	{
		RWrapper::RW_PartProgram::MYINSTANCE()->FailedActionIdList = gcnew System::Collections::Generic::List<System::Int32>();
		RWrapper::RW_PartProgram::MYINSTANCE()->FailedActionIdList->Clear();
		RWrapper::RW_PartProgram::MYINSTANCE()->FailedActionIdList->Add(id);
		RWrapper::RW_PartProgram::MYINSTANCE()->RunningFailedAction = true;
		PPCALCOBJECT->PPrerunActionList = PPCALCOBJECT->getPPActionlist().getList().begin();
		RWrapper::RW_PartProgram::MYINSTANCE()->RunFailedAction();
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRACPRD0004", ex);
	}
}

bool RWrapper::RW_ActionParameter::SetActionParamForFrontend(RAction* currentAction)
{
	try
	{
		bool addrow = true;
		this->ActionParameter_ArrayList->Clear();
		switch(currentAction->CurrentActionType)
		{
			case RapidEnums::ACTIONTYPE::ADDPOINTACTION:
   			{
				Shape* Cshape = ((AddPointAction*)currentAction)->getShape();
				FramegrabBase* Cframegrab = ((AddPointAction*)currentAction)->getFramegrab();
				this->ActionParameter_ArrayList->Add(currentAction->getId());
				this->ActionParameter_ArrayList->Add(gcnew System::String(currentAction->getModifiedName()));
				this->ActionParameter_ArrayList->Add(gcnew System::String(Cshape->getModifiedName()));
				this->ActionParameter_ArrayList->Add(((AddPointAction*)currentAction)->CriticalAction);
				this->ActionParameter_ArrayList->Add(Cframegrab->noofpts);
				this->ActionParameter_ArrayList->Add(Cframegrab->PartprogrmActionPassStatus());
				this->ActionParameter_ArrayList->Add(true);
				break;
			}
			case RapidEnums::ACTIONTYPE::PERIMETERACTION:
   			{
				Shape* Cshape = ((PerimeterAction*)currentAction)->getShape();
				FramegrabBase* Cframegrab = ((PerimeterAction*)currentAction)->getFramegrab();
				this->ActionParameter_ArrayList->Add(currentAction->getId());
				this->ActionParameter_ArrayList->Add(gcnew System::String(currentAction->getModifiedName()));
				this->ActionParameter_ArrayList->Add(gcnew System::String(Cshape->getModifiedName()));
				this->ActionParameter_ArrayList->Add(((AddPointAction*)currentAction)->CriticalAction);
				this->ActionParameter_ArrayList->Add(Cframegrab->noofpts);
				this->ActionParameter_ArrayList->Add(Cframegrab->PartprogrmActionPassStatus());
				this->ActionParameter_ArrayList->Add(true);
				break;
			}
			case RapidEnums::ACTIONTYPE::COPYSHAPEACTION:
			case RapidEnums::ACTIONTYPE::ADDUCSACTION:
			case RapidEnums::ACTIONTYPE::ADDDIMACTION:
			case RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION:
			case RapidEnums::ACTIONTYPE::IMAGE_ACTION:
			case RapidEnums::ACTIONTYPE::AREA_ACTION:
			{
				this->ActionParameter_ArrayList->Add(currentAction->getId());
				this->ActionParameter_ArrayList->Add(gcnew System::String(currentAction->getModifiedName()));
				this->ActionParameter_ArrayList->Add("");
				this->ActionParameter_ArrayList->Add(false);
				this->ActionParameter_ArrayList->Add(0);
				this->ActionParameter_ArrayList->Add(true);
				this->ActionParameter_ArrayList->Add(false);
				break;
			}
			case RapidEnums::ACTIONTYPE::ADDSHAPEACTION:
			{
				Shape* Cshape = ((AddShapeAction*)currentAction)->getShape();
				this->ActionParameter_ArrayList->Add(currentAction->getId());
				this->ActionParameter_ArrayList->Add(gcnew System::String(currentAction->getModifiedName()));
		        this->ActionParameter_ArrayList->Add(gcnew System::String(Cshape->getModifiedName()));
				this->ActionParameter_ArrayList->Add(false);
				this->ActionParameter_ArrayList->Add(0);
				this->ActionParameter_ArrayList->Add(true);
				this->ActionParameter_ArrayList->Add(false);
				break;
			}
			default:
			{
				addrow = false;
				break;
			}
		}
		return addrow;
	}
	catch(Exception^ ex)
	{
		RWrapper::RW_MainInterface::MYINSTANCE()->WriteErrorLog("WRACPRD0005", ex);
	}
}
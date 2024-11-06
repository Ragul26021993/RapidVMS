#include "StdAfx.h"
#include "DeleteUCSAction.h"
#include "..\Engine\RCadApp.h"

DeleteUCSAction::DeleteUCSAction():RAction(_T("DeleteUCS"))
{
	try
	{
		this->CurrentActionType = RapidEnums::ACTIONTYPE::DELETEUCSACTION;
		delucschangeact = new RCollectionBase(false); 
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDUCS0001", __FILE__, __FUNCSIG__); }
}

DeleteUCSAction::~DeleteUCSAction()
{
	try
	{
		delucschangeact->deleteAll();
		delete delucschangeact;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDM0002", __FILE__, __FUNCSIG__); }
}

bool DeleteUCSAction::execute()
{
	try
	{
		MAINDllOBJECT->dontUpdateGraphcis = true;
		ucs = MAINDllOBJECT->getUCSList().selectedItem();
		MAINDllOBJECT->getUCSList().clearSelection(true);

		UCS* CUcs = (UCS*)ucs;
		RCollectionBase* ActColl = CUcs->RelatedUCSChangeAction;
		for(RC_ITER item = ActColl->getList().begin();	item != ActColl->getList().end(); item++)
		{
			RAction* CAction = (RAction*)(*item).second;
			CAction->ActionStatus(false);
		}
	
		MAINDllOBJECT->getUCSList().removeItem(ucs);
		MAINDllOBJECT->UCStoMCS(*((UCS*)(ucs)));
		MAINDllOBJECT->getUCSList().getList()[0]->selected(true);
		MAINDllOBJECT->dontUpdateGraphcis = false;
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDUCS0001", __FILE__, __FUNCSIG__); return false; }
}

bool DeleteUCSAction::redo()
{
	try
	{
		MAINDllOBJECT->dontUpdateGraphcis = true;
		ucs = MAINDllOBJECT->getUCSList().selectedItem();
		MAINDllOBJECT->getUCSList().clearSelection(true);
		UCS* CUcs = (UCS*)ucs;
		RCollectionBase* ActColl = CUcs->RelatedUCSChangeAction;
		for(RC_ITER item = ActColl->getList().begin();	item != ActColl->getList().end(); item++)
		{
			RAction* CAction = (RAction*)(*item).second;
			CAction->ActionStatus(false);
		}
	

		MAINDllOBJECT->getUCSList().removeItem(ucs);
		MAINDllOBJECT->UCStoMCS(*((UCS*)(ucs)));
		MAINDllOBJECT->getUCSList().getList()[0]->selected(true);
		MAINDllOBJECT->dontUpdateGraphcis = false;
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDUCS0002", __FILE__, __FUNCSIG__); return false; }
}

void DeleteUCSAction::undo()
{
	try
	{
		MAINDllOBJECT->AddingUCSModeFlag = true;
		UCS* CUcs = (UCS*)ucs;
		RCollectionBase* ActColl = CUcs->RelatedUCSChangeAction;
		for(RC_ITER item = ActColl->getList().begin();	item != ActColl->getList().end(); item++)
		{
			RAction* CAction = (RAction*)(*item).second;
			CAction->ActionStatus(true);
		}
	
		MAINDllOBJECT->getUCSList().addItem(ucs);
		MAINDllOBJECT->AddingUCSModeFlag = false;
		MAINDllOBJECT->getUCSList().clearSelection(true);
		ucs->selected(true);
		MAINDllOBJECT->MCStoUCS(MAINDllOBJECT->getCurrentUCS());
		//MAINDllOBJECT->getUCSList().notifySelection();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDUCS0003", __FILE__, __FUNCSIG__); }
}
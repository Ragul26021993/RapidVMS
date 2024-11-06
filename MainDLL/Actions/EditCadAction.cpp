#include "StdAfx.h"
#include "EditCadAction.h"
#include "..\Engine\RCadApp.h"
#include "..\DXF\DXFExpose.h"

EditCadAction::EditCadAction(int ucsid):RAction(_T("EditCadAction"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::EDITCADACTION;
	d_ucsid = ucsid;
}

EditCadAction::~EditCadAction()
{
}

bool EditCadAction::execute()
{
	try
	{
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("EditCad0001", __FILE__, __FUNCSIG__); return false; }
}
 
bool EditCadAction::redo()
{
	try
	{
		RCollectionBase& rshapes = MAINDllOBJECT->getUCS(d_ucsid)->getShapes();
		for (list<Shape*>::iterator i = EditedShapes.begin(); i != EditedShapes.end(); i++)
		{
			BaseItem* item = (*i);
			rshapes.addItem(item);
			if(item->selected())
				MAINDllOBJECT->getSelectedShapesList().addItem(item, false);
		}
		MAINDllOBJECT->getShapesList().notifySelection();
	    ActionStatus(true);
		for (std::list<RAction*>::iterator i = EditedAlignMentAction.begin(); i != EditedAlignMentAction.end(); i++)
	    	(*i)->ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("EditCadAct0002", __FILE__, __FUNCSIG__); return false; }
}

void EditCadAction::undo()
{
	try
	{	
		RCollectionBase& rshapes = MAINDllOBJECT->getUCS(d_ucsid)->getShapes();
		for (std::list<Shape*>::iterator i = EditedShapes.begin(); i != EditedShapes.end(); i++)
			rshapes.removeItem((*i), false);
		ActionStatus(false);
		for (std::list<RAction*>::iterator i = EditedAlignMentAction.begin(); i != EditedAlignMentAction.end(); i++)
	    	(*i)->ActionStatus(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("EditCadAct0003", __FILE__, __FUNCSIG__); }
}








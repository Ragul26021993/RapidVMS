#include "StdAfx.h"
#include "AddThreadPointAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\ThreadCalcFunctions.h"
#include "..\Shapes\ShapeWithList.h"
#include "..\FrameGrab\FramegrabBase.h"
#include "..\Engine\PartProgramFunctions.h"

AddThreadPointAction::AddThreadPointAction():RAction(_T("AddThPt"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::ADDTHREADPOINTACTION;
}

AddThreadPointAction::~AddThreadPointAction()
{	
	try{ delete baseFG; }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHPOINT0001", __FILE__, __FUNCSIG__); }
}

AddThreadPointAction::AddThreadPointAction(ShapeWithList *s, FramegrabBase* v):RAction(_T("AddThPt"))
{	
	setParams(s, v);
}

bool AddThreadPointAction::execute()
{
	try
	{
		if(shape != NULL && baseFG->AllPointsList.Pointscount() > 0)
		{
			shape->ManagePoint(baseFG, shape->ADD_POINT);
			ActionStatus(true);
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHPOINT0002", __FILE__, __FUNCSIG__); return false; }
}

bool AddThreadPointAction::redo()
{
	try
	{
		shape->Modify_Points(baseFG, shape->ADD_POINT);
		shape->PointAtionList.push_back(this);
		ActionStatus(true);
		THREADCALCOBJECT->AddPointsToMainCollectionShape(this->shape);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHPOINT0003", __FILE__, __FUNCSIG__); return false; }
}

void AddThreadPointAction::undo()
{
	try
	{
		ActionStatus(false);
		shape->PointAtionList.remove(this);
		shape->Modify_Points(baseFG, shape->REMOVE_POINT);
		THREADCALCOBJECT->AddPointsToMainCollectionShape(this->shape);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHPOINT0004", __FILE__, __FUNCSIG__); }
}

void AddThreadPointAction::setParams(ShapeWithList *s, FramegrabBase* v)
{
	try
	{
		this->shape = s;
		this->baseFG = v;
		this->ActionStatus(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHPOINT0005", __FILE__, __FUNCSIG__); }
}

void AddThreadPointAction::ThreadpointAction(ShapeWithList* s, FramegrabBase* v)
{
	try
	{
		AddThreadPointAction *action = new AddThreadPointAction(s, v);
		action->CurrentActionType = RapidEnums::ACTIONTYPE::ADDTHREADPOINTACTION;
		s->PointAtionList.push_back(action);
		THREADCALCOBJECT->CurrentSelectedThreadAction->ActionCollection.addItem(action);
		action->execute();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHPOINT0006", __FILE__, __FUNCSIG__); }
}

FramegrabBase* AddThreadPointAction::getFramegrab()
{
	return baseFG;
}

ShapeWithList* AddThreadPointAction::getShape()
{
	return shape;
}

wostream& operator<<(wostream& os, AddThreadPointAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;

		os << "AddThreadPointAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "UcsId:" << action.shape->UcsId() << endl;
		os << "ActionShapeId:" << action.shape->getId() << endl;
		os << "FramegrabBase:values" << endl << (*static_cast <FramegrabBase*>(action.getFramegrab())) << endl;
		os << "EndAddThreadPointAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHPOINT0007", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, AddThreadPointAction& action)
{
	try
	{
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,action);
		}	
		else
		{
			int Ucsid;
			AddThreadMeasureAction* Atma;
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"AddThreadPointAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::ADDTHREADPOINTACTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndAddThreadPointAction")
				{	
					std::wstring Linestr;				
					is >> Linestr;
					if(is.eof())
					{
						MAINDllOBJECT->PPLoadSuccessful(false);	
						break;
					}
					int ColonIndx = Linestr.find(':');
					if(ColonIndx==-1)
					{
						Tagname=Linestr;
					}
					else
					{
						std::wstring Tag = Linestr.substr(0, ColonIndx);
						std::wstring TagVal = Linestr.substr(ColonIndx + 1, Linestr.length() - ColonIndx - 1);
						std::string Val(TagVal.begin(), TagVal.end() );
						Tagname=Tag;
						if(Tagname==L"Id")
						{
							action.setId(atoi((const char*)(Val).c_str()));
						}
					
						else if(Tagname==L"OriginalName")
						{
							action.setOriginalName(TagVal);
						}					
						else if(Tagname==L"UcsId")
						{
							Ucsid=atoi((const char*)(Val).c_str());
							Atma = THREADCALCOBJECT->CurrentSelectedThreadAction;
						}
						else if(Tagname==L"ActionShapeId")
						{
							int n=atoi((const char*)(Val).c_str()); 
							action.shape = (ShapeWithList*)(Atma->ShapeCollection_Actions.getList()[n]);
							action.shape->UcsId(Ucsid);
							if(PPCALCOBJECT->PartprogramisLoadingEditMode())
								action.shape->PointAtionList.push_back(&action);
						}
						else if(Tagname==L"FramegrabBase")
						{						
							action.baseFG = new FramegrabBase();
							action.baseFG->CucsId = Ucsid;
							is >> (*static_cast <FramegrabBase*>(action.getFramegrab()));
						}
					}
				}
			}
			else
			{
				MAINDllOBJECT->PPLoadSuccessful(false);				
			}	
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHPOINT0008", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, AddThreadPointAction& action)
{
	try
	{
		wstring name; int n, Ucsid;
		AddThreadMeasureAction* Atma;
		action.CurrentActionType = RapidEnums::ACTIONTYPE::ADDTHREADPOINTACTION;
		action.ActionStatus(true);
		is >> n; action.setId(n);
		is >> name; action.setOriginalName(name);
		is >> Ucsid; Atma = THREADCALCOBJECT->CurrentSelectedThreadAction;
		is >> n; action.shape = (ShapeWithList*)(Atma->ShapeCollection_Actions.getList()[n]);
		action.shape->UcsId(Ucsid);
		if(PPCALCOBJECT->PartprogramisLoadingEditMode())
			action.shape->PointAtionList.push_back(&action);
		action.baseFG = new FramegrabBase();
		action.baseFG->CucsId = Ucsid;
		is >> (*static_cast <FramegrabBase*>(action.getFramegrab()));
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHPOINT0008", __FILE__, __FUNCSIG__); }
}

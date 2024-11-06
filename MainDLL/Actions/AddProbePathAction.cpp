#include "StdAfx.h"
#include "AddProbePathAction.h"
#include "..\Engine\RCadApp.h"

AddProbePathAction::AddProbePathAction():RAction(_T("AddProbePath"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION;
	PausableAction = false;
}

AddProbePathAction::~AddProbePathAction()
{
	try{ delete baseFG; }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPBPT0001", __FILE__, __FUNCSIG__); }
}

bool AddProbePathAction::execute()
{
	ActionStatus(true);
	return true;
}

bool AddProbePathAction::redo()
{
	ActionStatus(true);
	return true;
}

void AddProbePathAction::undo()
{
	ActionStatus(false);
}

void AddProbePathAction::AddProbePath(FramegrabBase* v)
{
	try
	{
		AddProbePathAction *action = new AddProbePathAction();
		action->CurrentActionType = RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION;
		action->baseFG = v;
		MAINDllOBJECT->addAction(action);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPBPT0002", __FILE__, __FUNCSIG__); }
}

FramegrabBase* AddProbePathAction::getFramegrab()
{
	return baseFG;
}

wostream& operator<<(wostream& os, AddProbePathAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;
		
		os << "AddProbePathAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "PausableAction:" << action.PausableAction << endl; 		
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "FramegrabBase:Values" << endl << (*static_cast <FramegrabBase*>(action.getFramegrab())) << endl;
		os << "EndAddProbePathAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPBPT0003", __FILE__, __FUNCSIG__); return os;}
}

wistream& operator>>(wistream& is, AddProbePathAction& action)
{
	try
	{
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,action);
		}	
		else
		{
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"AddProbePathAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndAddProbePathAction")
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
						if(Tagname==L"CurrentActionType")
						{						
						}
						else if(Tagname==L"Id")
						{
							action.setId(atoi((const char*)(Val).c_str()));
						}					
						else if(Tagname==L"PausableAction")
						{
							if(Val=="0")
								action.PausableAction = false;
							else
								action.PausableAction = true;
						}
						else if(Tagname==L"OriginalName")
						{
							action.setOriginalName(TagVal);
						}
						else if(Tagname==L"FramegrabBase")
						{
							action.baseFG = new FramegrabBase();
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
	catch(...)
	{
		MAINDllOBJECT->PPLoadSuccessful(false); 
		MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPBPT0004", __FILE__, __FUNCSIG__); return is;
	}
}
void ReadOldPP(wistream& is, AddProbePathAction& action)
{
	try
	{
		wstring flagStr, name; int n;
		UCS* ucs;
		action.CurrentActionType = RapidEnums::ACTIONTYPE::ADDPROBEPATHACTION;
		action.ActionStatus(true);
		is >> n; action.setId(n);
		is >> flagStr;
		std::string Str = RMATH2DOBJECT->WStringToString(flagStr);
		if(Str == "PausableAction")
		{
			is >> action.PausableAction;
			is >> name;
			action.setOriginalName(name);
		}
		else
		{
	    	action.setOriginalName(flagStr);
		}
		action.baseFG = new FramegrabBase();
		is >> (*static_cast <FramegrabBase*>(action.getFramegrab()));		
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPBPT0004", __FILE__, __FUNCSIG__);  }
}
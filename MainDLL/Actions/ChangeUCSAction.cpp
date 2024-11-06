#include "StdAfx.h"
#include "ChangeUCSAction.h"
#include "..\Engine\RCadApp.h"

ChangeUCSAction::ChangeUCSAction():RAction(_T("ChangeUCS"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::CHANGEUCSACTION;
}

ChangeUCSAction::~ChangeUCSAction()
{
}

void ChangeUCSAction::setNId(int id)
{
	newid = id;
}

bool ChangeUCSAction::execute()
{
	try
	{
		previousid = MAINDllOBJECT->getCurrentUCS().getId();
		UCS* CUcs = (UCS*)MAINDllOBJECT->getUCSList().getList()[newid];
		CUcs->RelatedUCSChangeAction->addItem(this, false);
		CUcs->selected(true);
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCHUCS0001", __FILE__, __FUNCSIG__); return false; }
}

bool ChangeUCSAction::redo()
{
	try
	{
		previousid = MAINDllOBJECT->getCurrentUCS().getId();
		UCS* CUcs = (UCS*)MAINDllOBJECT->getUCSList().getList()[newid];
		CUcs->RelatedUCSChangeAction->addItem(this, false);
		CUcs->selected(true);
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCHUCS0002", __FILE__, __FUNCSIG__); return false; }
}

void ChangeUCSAction::undo()
{
	try
	{
		ActionStatus(false);
		UCS* CUcs = (UCS*)MAINDllOBJECT->getUCSList().getList()[newid];
		CUcs->RelatedUCSChangeAction->removeItem(this, false);
		MAINDllOBJECT->getUCSList().getList()[previousid]->selected(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCHUCS0003", __FILE__, __FUNCSIG__); }
}

void ChangeUCSAction::changeUCS(int id)
{
	try
	{
		//to be changed according to this class
		ChangeUCSAction* action = new ChangeUCSAction();
		action->setNId(id);
		action->CurrentActionType = RapidEnums::ACTIONTYPE::CHANGEUCSACTION;
		MAINDllOBJECT->addAction(action);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCHUCS0004", __FILE__, __FUNCSIG__); }
}

int ChangeUCSAction::getNID()
{
	return newid;
}

void ChangeUCSAction::setPID(int id)
{
	previousid = id;
}

int ChangeUCSAction::getPID()
{
	return previousid;
}

wostream& operator<<(wostream& os, ChangeUCSAction& action)
{
	try
	{	
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;

		os << "ChangeUCSAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "PID:" << action.getPID() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "NID:" << action.getNID() << endl;
		os << "EndChangeUCSAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCHUCS0005", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, ChangeUCSAction& action)
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
			if(Tagname==L"ChangeUCSAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::CHANGEUCSACTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndChangeUCSAction")
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
						else if(Tagname==L"PID")
						{
							action.setPID(atoi((const char*)(Val).c_str()));
						}	
						else if(Tagname==L"OriginalName")
						{
							action.setOriginalName(TagVal);
						}	
						else if(Tagname==L"NID")
						{
							action.setNId(atoi((const char*)(Val).c_str()));
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCHUCS0006", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, ChangeUCSAction& action)
{
	try
	{
		wstring name; int n;
		action.CurrentActionType = RapidEnums::ACTIONTYPE::CHANGEUCSACTION;
		action.ActionStatus(true);
		is >> n; action.setId(n);
		is >> n; action.setPID(n);
		is >> name; action.setOriginalName(name);
		is >> n; action.setNId(n);
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCHUCS0006", __FILE__, __FUNCSIG__);  }
}
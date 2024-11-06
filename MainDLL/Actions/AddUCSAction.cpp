#include "StdAfx.h"
#include "AddUCSAction.h"
#include "..\Engine\RCadApp.h"

AddUCSAction::AddUCSAction():RAction(_T("AddUCS"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::ADDUCSACTION;
}

AddUCSAction::~AddUCSAction()
{
}

void AddUCSAction::setUCS(UCS* ucs)
{
	this->ucs = ucs;
}

bool AddUCSAction::execute()
{
	try
	{  
		p_ucs = &(MAINDllOBJECT->getCurrentUCS());
		UCS* CUcs = (UCS*)ucs;
		CUcs->RelatedUCSChangeAction->addItem(this);
		MAINDllOBJECT->getUCSList().addItem(ucs);
		CUcs->AddingFirstTime(false);
		ucs->selected(true);	
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTUCS0001", __FILE__, __FUNCSIG__); return false; }
}

bool AddUCSAction::redo()
{
	try
	{
		p_ucs = &(MAINDllOBJECT->getCurrentUCS());
		MAINDllOBJECT->AddingUCSModeFlag = true;
		UCS* CUcs = (UCS*)ucs;
		CUcs->RelatedUCSChangeAction->addItem(this, false);
		MAINDllOBJECT->getUCSList().addItem(ucs);
		MAINDllOBJECT->AddingUCSModeFlag = false;
		ucs->selected(false, false);
		MAINDllOBJECT->changeUCS(ucs->getId());
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTUCS0002", __FILE__, __FUNCSIG__); return false; }
}

void AddUCSAction::undo()
{
	try
	{
		ActionStatus(false);
		MAINDllOBJECT->AddingUCSModeFlag = true;
		UCS* CUcs = (UCS*)ucs;
		CUcs->RelatedUCSChangeAction->removeItem(this, false);
		MAINDllOBJECT->getUCSList().removeItem(ucs);
		MAINDllOBJECT->AddingUCSModeFlag = false;
		MAINDllOBJECT->changeUCS(p_ucs->getId());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTUCS0003", __FILE__, __FUNCSIG__); }
}

void AddUCSAction::setNewId(int id)
{
	NucsId = id;
}

int AddUCSAction::getNewId()
{
	return NucsId;
}

void AddUCSAction::addUCS(UCS* nucs)
{
	try
	{
		//to be changed according to this class
		AddUCSAction* action = new AddUCSAction();
		action->setUCS(nucs);
		action->setNewId(nucs->getId());
		action->CurrentActionType = RapidEnums::ACTIONTYPE::ADDUCSACTION;
		MAINDllOBJECT->addAction(action);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTUCS0004", __FILE__, __FUNCSIG__); }
}

UCS* AddUCSAction::getUCS()
{
	return (UCS*)ucs;
}

UCS* AddUCSAction::getPUCS()
{
	return (UCS*)p_ucs;
}

wostream& operator<<(wostream& os, AddUCSAction& action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;

		os << "AddUCSAction" <<endl;
		os << "Id:" << action.getId() << endl;
		os << "NewId:" << action.getNewId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "ActionUcsID:" << action.getUCS()->getId();
		os << "EndAddUCSAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTUCS0005", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, AddUCSAction& action)
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
			if(Tagname==L"AddUCSAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::ADDUCSACTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndAddUCSAction")
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
						else if(Tagname==L"NewId")
						{
							action.setNewId(atoi((const char*)(Val).c_str()));
						}	
						else if(Tagname==L"OriginalName")
						{
							action.setOriginalName(TagVal);
						}					
						else if(Tagname==L"ActionUcsID")
						{
							int n=atoi((const char*)(Val).c_str());
							action.ucs = MAINDllOBJECT->getUCS(n);
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->PPLoadSuccessful(false);	 MAINDllOBJECT->SetAndRaiseErrorMessage("ACTUCS0006", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, AddUCSAction& action)
{
	try
	{
		wstring name; int n;
		action.CurrentActionType = RapidEnums::ACTIONTYPE::ADDUCSACTION;
		action.ActionStatus(true);
		is >> n; action.setId(n);
		is >> n; action.setNewId(n);
		is >> name; action.setOriginalName(name);
		is >> n; action.ucs = MAINDllOBJECT->getUCS(n);
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("ACTUCS0006", __FILE__, __FUNCSIG__); }
}
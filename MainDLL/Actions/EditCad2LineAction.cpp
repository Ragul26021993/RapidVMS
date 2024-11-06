#include "StdAfx.h"
#include "EditCad2LineAction.h"
#include "..\Engine\RCadApp.h"

EditCad2LineAction::EditCad2LineAction():RAction(_T("Cad2LineAlign"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::EDIT_CAD_2LN_ALIGNACTION;
}

EditCad2LineAction::~EditCad2LineAction()
{
}

bool EditCad2LineAction::execute()
{
	try
	{
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2LNAct0001", __FILE__, __FUNCSIG__); return false; }
}
 
bool EditCad2LineAction::redo()
{
	try
	{
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2LNAct0002", __FILE__, __FUNCSIG__); return false; }
}

void EditCad2LineAction::undo()
{
	try
	{				
		ActionStatus(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2LNAct0003", __FILE__, __FUNCSIG__); }
}

void EditCad2LineAction::SetCad2LineAlign(std::list<int> *ShapeIdList)
{
	try
	{		
		EditCad2LineAction* CptAct = new EditCad2LineAction();
		for each(int id in *ShapeIdList)
			CptAct->IDList.push_back(id);
		MAINDllOBJECT->addAction(CptAct);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2LNAct0004", __FILE__, __FUNCSIG__); }
}

void EditCad2LineAction::getLineIdList(map<int, int>* LIdList)
{
	try
	{				
		int n = 0;
		for each(int id in IDList)
		{
			(*LIdList)[n] = id;
			n++;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2LNAct0005", __FILE__, __FUNCSIG__); }
}

wostream& operator<<(wostream& os, EditCad2LineAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;

		os << "EditCad2LineAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "ActionIDListsize:" << action.IDList.size() << endl;
		for each(int id in action.IDList)
			os << "ActionIDList:" << id << endl;
		os << "EndEditCad2LineAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2LNAct0006", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, EditCad2LineAction& action)
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
			if(Tagname==L"EditCad2LineAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::EDIT_CAD_2LN_ALIGNACTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndEditCad2LineAction")
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
						else if(Tagname==L"ActionIDList")
						{	
							int n=atoi((const char*)(Val).c_str());
							action.IDList.push_back(n);
						}
						else if(Tagname==L"ActionIDListsize")
						{
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2LNAct0007", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, EditCad2LineAction& action)
{
	try
	{
		wstring name; int n, Idcnt;
		UCS* ucs;
		action.CurrentActionType = RapidEnums::ACTIONTYPE::EDIT_CAD_2LN_ALIGNACTION;
		action.ActionStatus(true);
		is >> n; action.setId(n);
		is >> name; action.setOriginalName(name);	
		is >> Idcnt;
		for(int i = 0; i < Idcnt; i++)
		{
			is >> n;
			action.IDList.push_back(n);
		}
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2LNAct0007", __FILE__, __FUNCSIG__); }
}
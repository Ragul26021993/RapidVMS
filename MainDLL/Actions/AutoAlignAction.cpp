#include "StdAfx.h"
#include "AutoAlignAction.h"
#include "..\Engine\RCadApp.h"

AutoAlignAction::AutoAlignAction():RAction(_T("AutoAlign"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::AUTOALIGNACTION;
}

AutoAlignAction::~AutoAlignAction()
{
}

bool AutoAlignAction::execute()
{
	try
	{
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0001", __FILE__, __FUNCSIG__); return false; }
}
 
bool AutoAlignAction::redo()
{
	try
	{
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0002", __FILE__, __FUNCSIG__); return false; }
}

void AutoAlignAction::undo()
{
	try
	{				
		ActionStatus(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0003", __FILE__, __FUNCSIG__); }
}

void AutoAlignAction::SetAutoAlignAction(std::list<int>* ShapeIdList, bool selectedShapes, int Alignment_mode)
{
	try
	{		
		AutoAlignAction* CptAct = new AutoAlignAction();
		CptAct->IsSelectedShapes = selectedShapes;
		CptAct->AlignmentMode = Alignment_mode;
		for each(int id in *ShapeIdList)
			CptAct->IDlist.push_back(id);
		MAINDllOBJECT->addAction(CptAct);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0004", __FILE__, __FUNCSIG__); }
}

bool AutoAlignAction::getshapeSelectionFlag()
{
	try
	{				
		return IsSelectedShapes;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0005", __FILE__, __FUNCSIG__); }
}

int AutoAlignAction::getAlignmentMode()
{
	try
	{				
		return AlignmentMode;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0006", __FILE__, __FUNCSIG__); }
}

void AutoAlignAction::getShapeIdList(std::list<int>* ShapeIdList)
{
	try
	{				
		for each(int id in IDlist)
			ShapeIdList->push_back(id);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0007", __FILE__, __FUNCSIG__); }
}

wostream& operator<<(wostream& os, AutoAlignAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;

		os << "AutoAlignAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "AlignmentMode:" << action.AlignmentMode << endl;
		os << "IsSelectedShapes:" << action.IsSelectedShapes << endl;
		os << "ActionIDlistSize:" << action.IDlist.size() << endl;
		for each(int id in action.IDlist)
			os << "ActionIdListValue:" << id << endl;
		os << "EndAutoAlignAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0008", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, AutoAlignAction& action)
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
			if(Tagname==L"AutoAlignAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::AUTOALIGNACTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndAutoAlignAction")
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
						else if(Tagname==L"AlignmentMode")
						{
							action.AlignmentMode = atoi((const char*)(Val).c_str());
						}	
						else if(Tagname==L"OriginalName")
						{
							action.setOriginalName(TagVal);
						}					
						else if(Tagname==L"IsSelectedShapes")
						{
							if(Val=="0")
								action.IsSelectedShapes=false;							
							else
								action.IsSelectedShapes=true;
						}
						else if(Tagname==L"ActionIDlistSize")
						{						
						}
						else if(Tagname==L"ActionIdListValue")
						{	
							action.IDlist.push_back(atoi((const char*)(Val).c_str()));
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0009", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, AutoAlignAction& action)
{
	try
	{
		wstring name; int n, Idcnt;
		bool flag;
		UCS* ucs;
		action.CurrentActionType = RapidEnums::ACTIONTYPE::AUTOALIGNACTION;
		action.ActionStatus(true);
		is >> n; action.setId(n);
		is >> name; action.setOriginalName(name);		
		is >> n; action.AlignmentMode = n;
		is >> flag; action.IsSelectedShapes = flag;
		is >> Idcnt;
		for(int i = 0; i < Idcnt; i++)
		{
			is >> n;
			action.IDlist.push_back(n);
		}
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0009", __FILE__, __FUNCSIG__);}
}








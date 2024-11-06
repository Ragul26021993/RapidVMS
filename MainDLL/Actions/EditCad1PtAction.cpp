#include "StdAfx.h"
#include "EditCad1PtAction.h"
#include "..\Engine\RCadApp.h"

EditCad1PtAction::EditCad1PtAction():RAction(_T("Cad1PtAlign"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::EDIT_CAD_1PT_ALIGNACTION;
}

EditCad1PtAction::~EditCad1PtAction()
{
}

bool EditCad1PtAction::execute()
{
	try
	{
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1PtAct0001", __FILE__, __FUNCSIG__); return false; }
}
 
bool EditCad1PtAction::redo()
{
	try
	{
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1PtAct0002", __FILE__, __FUNCSIG__); return false; }
}

void EditCad1PtAction::undo()
{
	try
	{				
		ActionStatus(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1PtAct0003", __FILE__, __FUNCSIG__); }
}

void EditCad1PtAction::SetCad1PtAlign(Vector& pt1, FramegrabBase* v, int ID)
{
	try
	{		
		EditCad1PtAction* CptAct = new EditCad1PtAction();
		CptAct->Point1 = pt1;
		CptAct->baseFG = v;
		CptAct->UcsID = ID;
		MAINDllOBJECT->addAction(CptAct);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1PtAct0004", __FILE__, __FUNCSIG__); }
}

Vector* EditCad1PtAction::getPoint1()
{
	try
	{
		return &(this->Point1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1PtAct0006", __FILE__, __FUNCSIG__); }
}

void EditCad1PtAction::SetPoint1(Vector& pt1)
{
	try
	{
		this->Point1 = pt1;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1PtAct0007", __FILE__, __FUNCSIG__); }
}

FramegrabBase* EditCad1PtAction::getFramegrab()
{
	return baseFG;
}

int EditCad1PtAction::getUcsId()
{
	return this->UcsID;
}

wostream& operator<<(wostream& os, EditCad1PtAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;

		os << "EditCad1PtAction"<< endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "UcsID:" << action.UcsID << endl;
		os << "FramegrabBase:values"<< endl << (*static_cast <FramegrabBase*>(action.getFramegrab())) << endl;
		os << "getPoint1:Values" << endl << (*static_cast<Vector*>(action.getPoint1())) << endl;
		os << "EndEditCad1PtAction"<< endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1PtAct0008", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, EditCad1PtAction& action)
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
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"EditCad1PtAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::EDIT_CAD_1PT_ALIGNACTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndEditCad1PtAction")
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
						else if(Tagname==L"UcsID")
						{	
							Ucsid=atoi((const char*)(Val).c_str());
							action.UcsID = Ucsid;
						}
						else if(Tagname==L"FramegrabBase")
						{
							action.baseFG = new FramegrabBase();
							action.baseFG->CucsId = Ucsid;	
							is >> (*static_cast <FramegrabBase*>(action.getFramegrab()));
						}	
						else if(Tagname==L"getPoint1")
						{	
							is >> (*static_cast<Vector*>(&action.Point1));						
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1PtAct0009", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, EditCad1PtAction& action)
{
	try
	{
		wstring name; int n;
		UCS* ucs;
		action.CurrentActionType = RapidEnums::ACTIONTYPE::EDIT_CAD_1PT_ALIGNACTION;
		action.ActionStatus(true);
		is >> n; action.setId(n);
		is >> name; action.setOriginalName(name);	
		is >> n;
		action.baseFG = new FramegrabBase();
		action.baseFG->CucsId = n;
		action.UcsID = n;
		is >> (*static_cast <FramegrabBase*>(action.getFramegrab()));
		is >> (*static_cast<Vector*>(&action.Point1));
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1PtAct0009", __FILE__, __FUNCSIG__); }
}
#include "StdAfx.h"
#include "EditCad2PtAction.h"
#include "..\Engine\RCadApp.h"

EditCad2PtAction::EditCad2PtAction():RAction(_T("Cad2PtAlign"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION;
	this->ActionCnt = 0;
}

EditCad2PtAction::~EditCad2PtAction()
{
}

bool EditCad2PtAction::execute()
{
	try
	{
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2PtAct0001", __FILE__, __FUNCSIG__); return false; }
}
 
bool EditCad2PtAction::redo()
{
	try
	{
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2PtAct0002", __FILE__, __FUNCSIG__); return false; }
}

void EditCad2PtAction::undo()
{
	try
	{				
		ActionStatus(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2PtAct0003", __FILE__, __FUNCSIG__); }
}

void EditCad2PtAction::SetCad1PtAlign(Vector& pt1, FramegrabBase* v, bool firstAction)
{
	try
	{		
		EditCad2PtAction* CptAct = new EditCad2PtAction();
		if(firstAction)
		{
			CptAct->Point1 = pt1;
			CptAct->ActionCnt = 1;
		}
		else
		{
			CptAct->Point2 = pt1;
			CptAct->ActionCnt = 2;
		}
		CptAct->baseFG = v;
		MAINDllOBJECT->addAction(CptAct);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2PtAct0004", __FILE__, __FUNCSIG__); }
}

Vector* EditCad2PtAction::getPoint1()
{
	try
	{
		return &(this->Point1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2PtAct0005", __FILE__, __FUNCSIG__); }
}

Vector* EditCad2PtAction::getPoint2()
{
	try
	{
		return &(this->Point2);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2PtAct0006", __FILE__, __FUNCSIG__); }
}

int EditCad2PtAction::getActionCount()
{
	try
	{
		return this->ActionCnt;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2PtAct0007", __FILE__, __FUNCSIG__); }
}

void EditCad2PtAction::SetPoint1(Vector& pt1)
{
	try
	{
		this->Point1 = pt1;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2PtAct0007", __FILE__, __FUNCSIG__); }
}

void EditCad2PtAction::SetPoint2(Vector& pt2)
{
	try
	{
		this->Point2 = pt2;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2PtAct0007", __FILE__, __FUNCSIG__); }
}

FramegrabBase* EditCad2PtAction::getFramegrab()
{
	return baseFG;
}

wostream& operator<<(wostream& os, EditCad2PtAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;

		os << "EditCad2PtAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "getFramegrab:values" << endl << (*static_cast <FramegrabBase*>(action.getFramegrab())) << endl;
		os << "ActionCnt:" << action.ActionCnt << endl;
		if(action.ActionCnt == 1)
			os << "getPoint1:" << (*static_cast<Vector*>(action.getPoint1())) << endl;
		else if(action.ActionCnt == 2)
			os << "getPoint2:" << (*static_cast<Vector*>(action.getPoint2())) << endl;
		os << "EndEditCad2PtAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2PtAct0008", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, EditCad2PtAction& action)
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
			if(Tagname==L"EditCad2PtAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndEditCad2PtAction")
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
						else if(Tagname==L"FramegrabBase")
						{
							action.baseFG = new FramegrabBase();
							is >> (*static_cast <FramegrabBase*>(action.getFramegrab()));
						}
						else if(Tagname==L"ActionCnt")
						{
							action.ActionCnt = atoi((const char*)(Val).c_str());
						}
						else if(Tagname==L"getPoint1")
						{
							is >> (*static_cast<Vector*>(&action.Point1));
						}
						else if(Tagname==L"getPoint2")
						{
							is >> (*static_cast<Vector*>(&action.Point2));
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1PtAct0009", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, EditCad2PtAction& action)
{
	try
	{
		wstring name; int n;
		UCS* ucs;
		action.CurrentActionType = RapidEnums::ACTIONTYPE::EDIT_CAD_2PT_ALIGNACTION;
		action.ActionStatus(true);
		is >> n; action.setId(n);
		is >> name; action.setOriginalName(name);		
		action.baseFG = new FramegrabBase();
		is >> (*static_cast <FramegrabBase*>(action.getFramegrab()));
		is >> n; action.ActionCnt = n;
		if(n == 1)
			is >> (*static_cast<Vector*>(&action.Point1));
		else if(n == 2)
			is >> (*static_cast<Vector*>(&action.Point2));
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1PtAct0009", __FILE__, __FUNCSIG__); }
}
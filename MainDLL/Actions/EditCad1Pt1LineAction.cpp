#include "StdAfx.h"
#include "EditCad1Pt1LineAction.h"
#include "..\Engine\RCadApp.h"

EditCad1Pt1LineAction::EditCad1Pt1LineAction():RAction(_T("Cad1Pt1lineAlign"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::EDIT_CAD_1PT1LN_ALIGNACTION;
	this->ShapeId[0] = 0; this->ShapeId[1] = 0;
}

EditCad1Pt1LineAction::~EditCad1Pt1LineAction()
{
}

bool EditCad1Pt1LineAction::execute()
{
	try
	{
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0001", __FILE__, __FUNCSIG__); return false; }
}
 
bool EditCad1Pt1LineAction::redo()
{
	try
	{
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0002", __FILE__, __FUNCSIG__); return false; }
}

void EditCad1Pt1LineAction::undo()
{
	try
	{				
		ActionStatus(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0003", __FILE__, __FUNCSIG__); }
}

void EditCad1Pt1LineAction::SetCad1Pt1LineAlign(double* Shpid, int ID)
{
	try
	{		
		EditCad1Pt1LineAction* CptAct = new EditCad1Pt1LineAction();
		CptAct->UcsID = ID;
		CptAct->ShapeId[0] = Shpid[0];
		CptAct->ShapeId[1] = Shpid[1];
		CptAct->firstActionFlag = true;
		MAINDllOBJECT->addAction(CptAct);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0004", __FILE__, __FUNCSIG__); }
}

void EditCad1Pt1LineAction::SetCad1Pt1LineAlign(Vector& pt, FramegrabBase* v, int ID)
{
	try
	{		
		EditCad1Pt1LineAction* CptAct = new EditCad1Pt1LineAction();
		CptAct->Point = pt;
		CptAct->baseFG = v;
		CptAct->UcsID = ID;
		CptAct->firstActionFlag = false;
		MAINDllOBJECT->addAction(CptAct);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0004", __FILE__, __FUNCSIG__); }
}

Vector* EditCad1Pt1LineAction::getPoint1()
{
	try
	{
		return &(this->Point);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0005", __FILE__, __FUNCSIG__); }
}

void EditCad1Pt1LineAction::SetPoint1(Vector& pt1)
{
	try
	{
		this->Point = pt1;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0006", __FILE__, __FUNCSIG__); }
}

FramegrabBase* EditCad1Pt1LineAction::getFramegrab()
{
	if(firstActionFlag)
		return NULL;
	else
		return baseFG;
}

int EditCad1Pt1LineAction::getUcsId()
{
	return this->UcsID;
}

void EditCad1Pt1LineAction::getShapeId(double* ShpID)
{
	ShpID[0] = this->ShapeId[0];
	ShpID[1] = this->ShapeId[1];
}
bool EditCad1Pt1LineAction::getActionFlag()
{
	return this->firstActionFlag;
}

wostream& operator<<(wostream& os, EditCad1Pt1LineAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;

		os << "EditCad1Pt1LineAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "firstActionFlag:" << action.firstActionFlag << endl;
		os << "UcsID:" << action.UcsID << endl;
		if(action.firstActionFlag)
		{
			os << "ShapeId0:" << action.ShapeId[0] << endl;
			os << "ShapeId1:" << action.ShapeId[1] << endl;
		}
		else
		{
			os << "FramegrabBase:values" << endl << (*static_cast <FramegrabBase*>(action.getFramegrab())) << endl;
			os << "getPoint1:values" << endl << (*static_cast<Vector*>(action.getPoint1())) << endl;
		}
		os << "EndEditCad1Pt1LineAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0007", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, EditCad1Pt1LineAction& action)
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
			if(Tagname==L"EditCad1Pt1LineAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::EDIT_CAD_1PT1LN_ALIGNACTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndEditCad1Pt1LineAction")
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
						else if(Tagname==L"firstActionFlag")
						{
							if(Val=="0")
								action.firstActionFlag = false;						
							else
								action.firstActionFlag = true;
						}					
						else if(Tagname==L"UcsID")
						{	
							Ucsid=atoi((const char*)(Val).c_str());
							action.UcsID = Ucsid;
						}
						else if(Tagname==L"ShapeId0")
						{	
							action.ShapeId[0] = atoi((const char*)(Val).c_str());
						}
						else if(Tagname==L"ShapeId1")
						{	
							action.ShapeId[1] = atoi((const char*)(Val).c_str());						
						}
						else if(Tagname==L"FramegrabBase")
						{
							action.baseFG = new FramegrabBase();
							action.baseFG->CucsId = Ucsid;	
							is >> (*static_cast <FramegrabBase*>(action.getFramegrab()));
						}	
						else if(Tagname==L"getPoint1")
						{	
							is >> (*static_cast<Vector*>(&action.Point));						
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0008", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, EditCad1Pt1LineAction& action)
{
	try
	{
		wstring name; int n;
		double value; bool flag;
		action.CurrentActionType = RapidEnums::ACTIONTYPE::EDIT_CAD_1PT1LN_ALIGNACTION;
		action.ActionStatus(true);
		is >> n; action.setId(n);
		is >> name; action.setOriginalName(name);	
		is >> flag; action.firstActionFlag = flag;
		is >> n;
		action.UcsID = n;
		if(flag)
		{
			is >> value; action.ShapeId[0] = value;
			is >> value; action.ShapeId[1] = value;
		}
		else
		{
			action.baseFG = new FramegrabBase();
			action.baseFG->CucsId = n;	
			is >> (*static_cast <FramegrabBase*>(action.getFramegrab()));
			is >> (*static_cast<Vector*>(&action.Point));
		}	
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0008", __FILE__, __FUNCSIG__); }
}

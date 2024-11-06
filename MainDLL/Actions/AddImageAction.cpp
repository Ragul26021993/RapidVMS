#include "StdAfx.h"
#include "AddImageAction.h"
#include "..\Engine\RCadApp.h"

AddImageAction::AddImageAction():RAction(_T("Image"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::IMAGE_ACTION;
	PausableAction = false;
	this->ImageCount = 0;
}

AddImageAction::~AddImageAction()
{
	try{ delete baseFG; }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IM001", __FILE__, __FUNCSIG__); }
}

bool AddImageAction::execute()
{
	ActionStatus(true);
	return true;
}

bool AddImageAction::redo()
{
	MAINDllOBJECT->ImageAction_Count++;
	ActionStatus(true);
	return true;
}

void AddImageAction::undo()
{
	MAINDllOBJECT->ImageAction_Count--;
	ActionStatus(false);
}

void AddImageAction::AddImage_DroValue(FramegrabBase* v)
{
	try
	{
		MAINDllOBJECT->ImageAction_Count++;
		AddImageAction *action = new AddImageAction();
		action->CurrentActionType = RapidEnums::ACTIONTYPE::IMAGE_ACTION;
		action->ImageCount = MAINDllOBJECT->ImageAction_Count;
		action->baseFG = v;
		MAINDllOBJECT->addAction(action);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IM002", __FILE__, __FUNCSIG__); }
}

FramegrabBase* AddImageAction::getFramegrab()
{
	return baseFG;
}

wostream& operator<<(wostream& os, AddImageAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;
		os << "AddImageAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "PausableAction:" << action.PausableAction << endl; 		
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "FramegrabBase:Values" << endl << (*static_cast <FramegrabBase*>(action.getFramegrab())) << endl;
		os << "ImageCount:" << action.ImageCount << endl;
		os << "EndAddImageAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("IM003", __FILE__, __FUNCSIG__); return os;}
}

wistream& operator>>(wistream& is, AddImageAction& action)
{
	try
	{
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"AddImageAction")
		{
			action.CurrentActionType = RapidEnums::ACTIONTYPE::IMAGE_ACTION;
			action.ActionStatus(true);
			while(Tagname!=L"EndAddImageAction")
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
					else if(Tagname==L"ImageCount")
					{
							action.ImageCount=atoi((const char*)(Val).c_str());
					}
				}
			}
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("IM004", __FILE__, __FUNCSIG__); return is; }
}
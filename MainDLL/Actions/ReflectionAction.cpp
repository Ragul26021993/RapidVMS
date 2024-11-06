#include "StdAfx.h"
#include "ReflectionAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\Shape.h"

ReflectionAction::ReflectionAction():RAction(_T("ReflectionShape"))
{
	try
	{
		this->CurrentActionType = RapidEnums::ACTIONTYPE::REFLECTIONACTION;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0001", __FILE__, __FUNCSIG__); }
}

ReflectionAction::~ReflectionAction()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0002", __FILE__, __FUNCSIG__); }
}

bool ReflectionAction::execute()
{
	try
	{
	   RCollectionBase& rshapes = MAINDllOBJECT->getCurrentUCS().getShapes();
	   for(std::list<int>::iterator i = ReflectedShapesId.begin(); i!=ReflectedShapesId.end(); i++)
			ReflectedShapes[*i] = (Shape*)rshapes.getList()[*i];
	  	ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0003", __FILE__, __FUNCSIG__); return false; }
}

bool ReflectionAction::redo()
{
	try
	{
		RCollectionBase& rshapes = MAINDllOBJECT->getCurrentUCS().getShapes();
		for (map<int, Shape*>::iterator i = ReflectedShapes.begin(); i != ReflectedShapes.end(); i++)
			rshapes.addItem(i->second, false);
		MAINDllOBJECT->getShapesList().notifySelection();
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0004", __FILE__, __FUNCSIG__); return false; }
}

void ReflectionAction::undo()
{
	try
	{
		RCollectionBase& rshapes = MAINDllOBJECT->getCurrentUCS().getShapes();
		for (map<int, Shape*>::iterator i = ReflectedShapes.begin(); i != ReflectedShapes.end(); i++)
			rshapes.removeItem(i->second, false);
		MAINDllOBJECT->getShapesList().notifySelection();
		ActionStatus(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0007", __FILE__, __FUNCSIG__); }
}

void ReflectionAction:: AddshapesColl(list<int>selectedShapeId, list<int>reflectedShapeId, int reflectionSurfaceId)
{
	try
	{
		SelectedshapesId = selectedShapeId;
		ReflectedShapesId = reflectedShapeId;
	    int ReflectionSurfaceId = reflectionSurfaceId;
		MAINDllOBJECT->addAction(this);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0006", __FILE__, __FUNCSIG__); }
}
 
wostream& operator<<(wostream& os, ReflectionAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;

		os << "ReflectionAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "ReflectionSurfaceId:" << action.ReflectionSurfaceId << endl;
		os << "ReflectedShapesIdsize:" << action.ReflectedShapesId.size() << endl;
		for(list<int>::iterator It =  action.ReflectedShapesId.begin(); It !=  action.ReflectedShapesId.end(); It++)
			os << "ReflectedShapesId:" << *It << endl;
		os << "SelectedshapesIdsize:" << action.SelectedshapesId.size() << endl;
		for(list<int>::iterator It =  action.SelectedshapesId.begin(); It !=  action.SelectedshapesId.end(); It++)
			os << "SelectedshapesId:" << *It << endl;
		os << "EndReflectionAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0007", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, ReflectionAction& action)
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
			if(Tagname==L"ReflectionAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::REFLECTIONACTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndReflectionAction")
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
						else if(Tagname==L"ReflectionSurfaceId")
						{
							action.ReflectionSurfaceId=atoi((const char*)(Val).c_str());
						}	
						else if(Tagname==L"ReflectedShapesId")
						{
							action.ReflectedShapesId.push_back(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname==L"SelectedshapesId")
						{
							action.SelectedshapesId.push_back(atoi((const char*)(Val).c_str()));
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0008", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, ReflectionAction& action)
{
	try
	{
		wstring name; int n, k, Ucsid;
		UCS* ucs;
		action.CurrentActionType = RapidEnums::ACTIONTYPE::REFLECTIONACTION;
		action.ActionStatus(true);
		is >> n; action.setId(n);
		is >> name; action.setOriginalName(name);
		is >> action.ReflectionSurfaceId;
		is >> n; 
		for(int i = 0; i < n; i++)
		{
			is >> k;
			action.ReflectedShapesId.push_back(k);
		}
		is >> n; 
		for(int i = 0; i < n; i++)
		{
			is >> k;
			action.SelectedshapesId.push_back(k);
		}
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0008", __FILE__, __FUNCSIG__); }
}
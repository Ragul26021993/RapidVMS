#include "StdAfx.h"
#include "TranslationAction.h"
#include "..\Engine\RCadApp.h"

TranslationAction::TranslationAction():RAction(_T("ReflectionShape"))
{
	try
	{
		this->CurrentActionType = RapidEnums::ACTIONTYPE::TRANSLATIONACTION;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0001", __FILE__, __FUNCSIG__); }
}

TranslationAction::~TranslationAction()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0002", __FILE__, __FUNCSIG__); }
}

bool TranslationAction::execute()
{
	try
	{
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0003", __FILE__, __FUNCSIG__); return false; }
}

bool TranslationAction::redo()
{
	try
	{
		RCollectionBase& rshapes = MAINDllOBJECT->getCurrentUCS().getShapes();
		double translateagain[3] = {TranslateDirection.getX(), TranslateDirection.getY(), TranslateDirection.getZ()};
		for (std::list<int>::iterator i = TranslatedShapesId.begin(); i!=TranslatedShapesId.end(); i++)
		{    
			ShapeWithList *currentShape = (ShapeWithList*)(rshapes.getList()[*i]);
			if(currentShape->PointsList->getList().empty())
			   currentShape->Translate(TranslateDirection);
			else
			   currentShape->TranslateShape(translateagain);
		}
		MAINDllOBJECT->getShapesList().notifySelection();
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0004", __FILE__, __FUNCSIG__); return false; }
}

void TranslationAction::undo()
{
	try
	{
		RCollectionBase& rshapes = MAINDllOBJECT->getCurrentUCS().getShapes();
		double translateBack[3] = {-TranslateDirection.getX(), -TranslateDirection.getY(), -TranslateDirection.getZ()};
		for (std::list<int>::iterator i = TranslatedShapesId.begin(); i!=TranslatedShapesId.end(); i++)
		{    
			ShapeWithList *currentShape = (ShapeWithList*)(rshapes.getList()[*i]);
			if(currentShape->PointsList->getList().empty())
			   currentShape->Translate(Vector(-TranslateDirection.getX(), -TranslateDirection.getY(), -TranslateDirection.getZ()));
			else
			       currentShape->TranslateShape(translateBack);
		}
		MAINDllOBJECT->getShapesList().notifySelection();
		ActionStatus(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0007", __FILE__, __FUNCSIG__); }
}


wostream& operator<<(wostream& os, TranslationAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;

		os << "TranslationAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "TranslatedShapesIdsize:" << action.TranslatedShapesId.size() << endl;
		for(list<int>::iterator It =  action.TranslatedShapesId.begin(); It !=  action.TranslatedShapesId.end(); It++)
			os << "TranslatedShapesId:" << *It << endl;
		os << "TranslateDirection:values" << (*static_cast<Vector*>(&(action.TranslateDirection))) << endl;
		os << "EndTranslationAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0007", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, TranslationAction& action)
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
			if(Tagname==L"TranslationAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::TRANSLATIONACTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndTranslationAction")
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
						else if(Tagname==L"TranslatedShapesId")
						{
							action.TranslatedShapesId.push_back(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname==L"TranslateDirection")
						{
							is >> (*static_cast<Vector*>(&action.TranslateDirection));
						}
						else if(Tagname==L"TranslatedShapesIdsize")
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0008", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, TranslationAction& action)
{
	try
	{
		wstring name; int n, k, Ucsid;
		UCS* ucs;
		action.CurrentActionType = RapidEnums::ACTIONTYPE::TRANSLATIONACTION;
		action.ActionStatus(true);
		is >> n; action.setId(n);
		is >> name; action.setOriginalName(name);
		is >> n; 
		for(int i = 0; i < n; i++)
		{
			is >> k;
			action.TranslatedShapesId.push_back(k);
		}
		is >> (*static_cast<Vector*>(&action.TranslateDirection));
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0008", __FILE__, __FUNCSIG__); }
}
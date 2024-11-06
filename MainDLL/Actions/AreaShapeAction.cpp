#include "StdAfx.h"
#include "AreaShapeAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\AreaShape.h"

AreaShapeAction::AreaShapeAction():RAction(_T("AreaAction"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::AREA_ACTION;
	PausableAction = false;	
}

AreaShapeAction::~AreaShapeAction()
{
	try{ delete baseFG; }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AreaAction001", __FILE__, __FUNCSIG__); }
}

bool AreaShapeAction::execute()
{
	ActionStatus(true);
	return true;
}

bool AreaShapeAction::redo()
{
	((AreaShape*)this->ShapePtr)->UpdateArea(AreaValue);
	ActionStatus(true);
	return true;
}

void AreaShapeAction::undo()
{
	((AreaShape*)this->ShapePtr)->UpdateArea();
	ActionStatus(false);
}

void AreaShapeAction::AddAreaShapeDroValue(Shape* Cshape, FramegrabBase* v, double value)
{
	try
	{
		AreaShapeAction *action = new AreaShapeAction();
		action->CurrentActionType = RapidEnums::ACTIONTYPE::AREA_ACTION;		
		action->ShapePtr = Cshape;
		action->AreaValue = value;
		action->baseFG = v;
		MAINDllOBJECT->addAction(action);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AreaAction002", __FILE__, __FUNCSIG__); }
}

Shape* AreaShapeAction::getShape()
{
	return this->ShapePtr;
}

FramegrabBase* AreaShapeAction::getFramegrab()
{
	return baseFG;
}

wostream& operator<<(wostream& os, AreaShapeAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;
		os << "AreaShapeAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "PausableAction:" << action.PausableAction << endl; 		
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "UcsId:" << action.ShapePtr->UcsId() << endl;
		os << "ActionShapeId:" << action.ShapePtr->getId() << endl;
		os << "FramegrabBase:Values" << endl << (*static_cast <FramegrabBase*>(action.getFramegrab())) << endl;
		os << "EndAreaShapeAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AreaAction003", __FILE__, __FUNCSIG__); return os;}
}

wistream& operator>>(wistream& is, AreaShapeAction& action)
{
	try
	{
		int Ucsid;
		UCS* ucs;
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"AreaShapeAction")
		{
			action.CurrentActionType = RapidEnums::ACTIONTYPE::AREA_ACTION;
			action.ActionStatus(true);
			while(Tagname!=L"EndAreaShapeAction")
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
					else if(Tagname==L"UcsId")
					{
						Ucsid = atoi((const char*)(Val).c_str());
						ucs = MAINDllOBJECT->getUCS(Ucsid);
					}
					else if(Tagname==L"ActionShapeId")
					{
						int n = atoi((const char*)(Val).c_str()); 
						if(ucs->getShapes().ItemExists(n))
						{
							action.ShapePtr = (Shape*)(ucs->getShapes().getList()[n]);
							action.ShapePtr->UcsId(Ucsid);
						}
					}
					else if(Tagname==L"FramegrabBase")
					{
						action.baseFG = new FramegrabBase();
						is >> (*static_cast <FramegrabBase*>(action.getFramegrab()));
					}
				}
			}
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("AreaAction004", __FILE__, __FUNCSIG__); return is; }
}
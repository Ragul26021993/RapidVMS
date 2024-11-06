#include "StdAfx.h"
#include "AddShapeAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\ShapeWithList.h"

AddShapeAction::AddShapeAction():RAction(_T("AddShape"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::ADDSHAPEACTION;
	this->PPStatusAction = false;
}

AddShapeAction::~AddShapeAction()
{
	((ShapeWithList*)this->shape)->CurrentShapeAddAction = NULL;
}

void AddShapeAction::setShape(Shape* shape)
{
	this->shape = shape;
}

bool AddShapeAction::execute()
{
	try
	{
		if(PPStatusAction)
			ActionStatus(false);
		else
			ActionStatus(true);
		MAINDllOBJECT->getShapesList().addItem(shape);
		if(shape->selected())
			MAINDllOBJECT->getSelectedShapesList().addItem(shape);
		((ShapeWithList*)shape)->CurrentShapeAddAction = this;
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTSH0001", __FILE__, __FUNCSIG__); return false;}
}
 
bool AddShapeAction::redo()
{
	try
	{
		ActionStatus(true);
		MAINDllOBJECT->getShapesList().addItem(shape);
		if(shape->selected())
			MAINDllOBJECT->getSelectedShapesList().addItem(shape);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTSH0002", __FILE__, __FUNCSIG__); return false;}
}

void AddShapeAction::undo()
{
	try
	{
		ActionStatus(false);
		MAINDllOBJECT->getShapesList().removeItem((BaseItem*)shape);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTSH0003", __FILE__, __FUNCSIG__); }
}

void AddShapeAction::addShape(Shape *shape, bool SelectShape, bool DisableActionFlag)
{
	try
	{
		AddShapeAction* action = new AddShapeAction();
		action->setShape(shape);
		action->PPStatusAction = DisableActionFlag;
		action->CurrentActionType = RapidEnums::ACTIONTYPE::ADDSHAPEACTION;
		MAINDllOBJECT->addAction(action);
		if(SelectShape)MAINDllOBJECT->selectShape(shape->getId(),false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTSH0004", __FILE__, __FUNCSIG__); }
}

Shape* AddShapeAction::getShape()
{
	return shape;
}

wostream& operator<<(wostream& os, AddShapeAction& action)
{
	try
	{
		os << "CurrentActionType:" << action.CurrentActionType << endl;

		os << "AddShapeAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "ActionShapeId:" << action.getShape()->getId() << endl;
		os << "EndAddShapeAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTSH0006", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, AddShapeAction& action)
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
			if(Tagname==L"AddShapeAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::ADDSHAPEACTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndAddShapeAction")
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
						else if(Tagname==L"OriginalName")
						{
							action.setOriginalName(TagVal);
						}
						else if(Tagname==L"ActionShapeId")
						{
							int b = atoi((const char*)(Val).c_str());
							if(MAINDllOBJECT->getCurrentUCS().getShapes().ItemExists(b))
							  action.shape = (ShapeWithList*)(MAINDllOBJECT->getCurrentUCS().getShapes().getList()[b]);
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);	 MAINDllOBJECT->SetAndRaiseErrorMessage("ACTSH0005", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, AddShapeAction& action)
{
	try
	{
		wstring name; int n;
		action.CurrentActionType = RapidEnums::ACTIONTYPE::ADDSHAPEACTION;
		action.ActionStatus(true);
		is >> n; action.setId(n);
		is >> name; action.setOriginalName(name);
		is >> n; 
		if(MAINDllOBJECT->getCurrentUCS().getShapes().ItemExists(n))
		  action.shape = (ShapeWithList*)(MAINDllOBJECT->getCurrentUCS().getShapes().getList()[n]);		
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("ACTSH0005", __FILE__, __FUNCSIG__); }
}
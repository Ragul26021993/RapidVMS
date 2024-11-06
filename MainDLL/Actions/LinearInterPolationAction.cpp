#include "StdAfx.h"
#include "LinearInterPolationAction.h"
#include "..\Engine\RCadApp.h"

LinearInterPolationAction::LinearInterPolationAction():RAction(_T("LinInt"))
{
	try
	{ 
		this->PausableAction = false;
		this->shape = NULL; this->ShapeUsed = true; this->AddPtFlag = false;
		this->CurrentActionType = RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LinInt001", __FILE__, __FUNCSIG__); }
}

LinearInterPolationAction::~LinearInterPolationAction()
{
	try{ }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LinInt002", __FILE__, __FUNCSIG__); }
}

bool LinearInterPolationAction::execute()
{
	ActionStatus(true);
	return true;
}

bool LinearInterPolationAction::redo()
{
	ActionStatus(true);
	((ShapeWithList*)this->shape)->PointAtionList.push_back(this);
	return true;
}

void LinearInterPolationAction::undo()
{
	((ShapeWithList*)this->shape)->PointAtionList.remove(this);
	ActionStatus(false);
}

void LinearInterPolationAction::AddLinearInterPolationAction(Shape* s, double* Target, bool AddpointFlag)
{
	try
	{ 
		LinearInterPolationAction *action = new LinearInterPolationAction();
		((ShapeWithList*)s)->PointAtionList.push_back(action);
		action->shape = s;
		action->TargetPosition.set(Target[0], Target[1], Target[2]);
		action->AddPtFlag = AddpointFlag;
		action->ShapeUsed = true;
		MAINDllOBJECT->addAction(action);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LinInt003", __FILE__, __FUNCSIG__); }
}

void LinearInterPolationAction::AddLinearInterPolationAction(double* Target)
{
	try
	{ 
		LinearInterPolationAction *action = new LinearInterPolationAction();
		action->ShapeUsed = false;		
		action->TargetPosition.set(Target[0], Target[1], Target[2]);
		MAINDllOBJECT->addAction(action);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LinInt004", __FILE__, __FUNCSIG__); }
}

void LinearInterPolationAction::Translate(Vector pt1)
{
	try
	{
		TargetPosition += pt1;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LinInt005", __FILE__, __FUNCSIG__); }
}

Vector* LinearInterPolationAction::GetTargetPosition()
{
	return &(this->TargetPosition);
}

void LinearInterPolationAction::SetTargetPosition(Vector& pt)
{
	this->TargetPosition.set(pt.getX(), pt.getY(), pt.getZ());
}

void LinearInterPolationAction::Transform(double* Tmatrix)
{
}

Shape* LinearInterPolationAction::getShape()
{
	return this->shape;
}

bool LinearInterPolationAction::GetShapeInfo()
{
	return this->ShapeUsed;
}

bool LinearInterPolationAction::GetAddPointInfo()
{
	return this->AddPtFlag;
}

wostream& operator<<(wostream& os, LinearInterPolationAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;
		os << "LinearInterPolationAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "PausableAction:" << action.PausableAction << endl; 		
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "UcsId:" << action.shape->UcsId() << endl;
		os << "AddPtFlag:" << action.AddPtFlag << endl;
		os << "ShapeUsedFlag:" << action.ShapeUsed << endl;
		if(action.ShapeUsed)
			os << "ActionShapeId:" << action.shape->getId() << endl;
		os << "TargetPosition:values" << endl << (*static_cast<Vector*>(action.GetTargetPosition())) << endl;
		os << "EndLinearInterPolationAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LinInt006", __FILE__, __FUNCSIG__); return os;}
}

wistream& operator>>(wistream& is, LinearInterPolationAction& action)
{
	try
	{
		int Ucsid;
		UCS* ucs;
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"LinearInterPolationAction")
		{
			action.CurrentActionType = RapidEnums::ACTIONTYPE::LINEAR_INTERPOLATION_ACTION;
			action.ActionStatus(true);
			while(Tagname!=L"EndLinearInterPolationAction")
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
					else if(Tagname==L"AddPtFlag")
					{
						if(Val=="0")
							action.AddPtFlag = false;
						else
							action.AddPtFlag = true;
					}
					else if(Tagname==L"ShapeUsedFlag")
					{
						if(Val=="0")
							action.ShapeUsed = false;
						else
							action.ShapeUsed = true;
					}
					else if(Tagname==L"ActionShapeId")
					{
						int n = atoi((const char*)(Val).c_str()); 
						action.shape = (Shape*)(ucs->getShapes().getList()[n]);
						action.shape->UcsId(Ucsid);
					}
					else if(Tagname==L"TargetPosition")
					{	
						is >> (*static_cast<Vector*>(&action.TargetPosition));						
					}
				}
			}
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("LinInt007", __FILE__, __FUNCSIG__); return is; }
}
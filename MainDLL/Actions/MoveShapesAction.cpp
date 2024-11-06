#include "StdAfx.h"
#include "MoveShapesAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\Shape.h"
#include "..\Helper\Helper.h"

MoveShapesAction::MoveShapesAction():RAction(_T("MoveShapes"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::MOVESHAPEACTION;
	this->WriteActionInPPFlag = false;
}

MoveShapesAction::~MoveShapesAction()
{
}

bool MoveShapesAction::execute()
{
	try
	{
		if(doaction)
			HELPEROBJECT->moveShapeCollection(MoveShapeList, movement, isNudge, angle);
		else
			doaction = true;
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTMV0001", __FILE__, __FUNCSIG__); return false; }
}
 
bool MoveShapesAction::redo()
{
	try
	{
		if(doaction)
			HELPEROBJECT->moveShapeCollection(MoveShapeList, movement, isNudge, angle);
		else
			doaction = true;
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTMV0002", __FILE__, __FUNCSIG__); return false; }
}

void MoveShapesAction::undo()
{
	try
	{
		if(doaction)
		{
			Vector inverse(-movement.getX(), -movement.getY(), -movement.getZ());
			if(isNudge)
				HELPEROBJECT->moveShapeCollection(MoveShapeList, inverse, isNudge, -angle);
			else
				HELPEROBJECT->moveShapeCollection(MoveShapeList, movement, isNudge, -angle);
		}
		ActionStatus(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTMV0003", __FILE__, __FUNCSIG__); }
}

void MoveShapesAction::moveShapes( Vector &v, std::list<Shape*> collection, bool flag, bool isNudge, double angle)
{
	try
	{
		if(collection.size() == 0) return;
		MoveShapesAction* msa = new MoveShapesAction();
		msa->doaction = flag;
		msa->movement = v;
		msa->isNudge = isNudge;
		msa->angle = angle;
		msa->MoveShapeList = collection;
		msa->WriteActionInPPFlag = false;
		MAINDllOBJECT->addAction(msa);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTMV0004", __FILE__, __FUNCSIG__); }
}

std::list<Shape*> MoveShapesAction::getShape()
{
	return MoveShapeList;
}

Vector& MoveShapesAction::getPosition()
{
	return movement;
}

void MoveShapesAction::SetActionStatus()
{
	try
	{
		MoveShapesAction* msa = new MoveShapesAction();
		msa->doaction = false;
		Vector movment;
		msa->movement = movment;
		msa->isNudge = false;
		msa->angle = 0;
		msa->WriteActionInPPFlag = true;
		MAINDllOBJECT->addAction(msa);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTMV0005", __FILE__, __FUNCSIG__); }
}

wostream& operator<<(wostream& os, MoveShapesAction &action)
{
	try
	{
		if(!action.WriteActionInPPFlag) return os;
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;

		os << "MoveShapesAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "EndMoveShapesAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1PtAct0008", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, MoveShapesAction& action)
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
			if(Tagname==L"MoveShapesAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::MOVESHAPEACTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndMoveShapesAction")
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
void ReadOldPP(wistream& is, MoveShapesAction& action)
{
	try
	{
		wstring name; int n;
		action.CurrentActionType = RapidEnums::ACTIONTYPE::MOVESHAPEACTION;
		action.ActionStatus(true);
		is >> n; action.setId(n);
		is >> name; action.setOriginalName(name);	
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1PtAct0009", __FILE__, __FUNCSIG__);  }
}
#include "StdAfx.h"
#include "AddPointAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"

AddPointAction::AddPointAction():RAction(_T("AddPoint"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::ADDPOINTACTION;
	CriticalAction = false;
	PausableAction = false;
	AddToShape = true;
}

AddPointAction::AddPointAction(ShapeWithList *s, FramegrabBase* v):RAction(_T("AddPoint"))
{
	setParams(s, v);
	CriticalAction = false;
	PausableAction = false;
	AddToShape = true;
}

AddPointAction::~AddPointAction()
{	
	try{ delete baseFG; }
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0001", __FILE__, __FUNCSIG__);
	}
}

bool AddPointAction::execute()
{
	try
	{
		if(shape != NULL && baseFG->AllPointsList.Pointscount() > 0)
		{
			if(this->AddToShape)
		    	shape->ManagePoint(baseFG, shape->ADD_POINT);
			else
				baseFG->AllPointsList.deleteAll();
			ActionStatus(true);
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0002", __FILE__, __FUNCSIG__); return false; }
}

bool AddPointAction::redo()
{
	try
	{
		shape->PointAtionList.push_back(this);
		shape->Modify_Points(baseFG, shape->ADD_POINT);
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0003", __FILE__, __FUNCSIG__); return false; }
}

void AddPointAction::undo()
{
	try
	{
		ActionStatus(false);
		shape->PointAtionList.remove(this);
		shape->Modify_Points(baseFG, shape->REMOVE_POINT);	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0004", __FILE__, __FUNCSIG__); }
}
 
void AddPointAction::pointAction(ShapeWithList* s, FramegrabBase* v, bool addtoshape)
{
	try
	{
		if(s->ShapeType == RapidEnums::SHAPETYPE::AREA) return;
		AddPointAction *action = new AddPointAction(s, v);
		action->CurrentActionType = RapidEnums::ACTIONTYPE::ADDPOINTACTION;
		s->PointAtionList.push_back(action);
		if(s->ShapeUsedForTwoStepHoming())
			v->ActionForTwoStepHoming(true);
		if(MAINDllOBJECT->DigitalZoomIsOn)
			v->UseDigitalZoom = true;
		action->AddToShape = addtoshape;
		MAINDllOBJECT->addAction(action);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0006", __FILE__, __FUNCSIG__); }
}

void AddPointAction::setParams(ShapeWithList *s, FramegrabBase* v)
{
	try
	{
		this->shape = s;
		this->baseFG = v;
		ActionStatus(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0005", __FILE__, __FUNCSIG__); }
}

FramegrabBase* AddPointAction::getFramegrab()
{
	return baseFG;
}

ShapeWithList* AddPointAction::getShape()
{
	return shape;
}

wostream& operator<<(wostream& os, AddPointAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;

		os << "AddPointAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "CriticalAction:" <<  action.CriticalAction << endl; 
		os << "PausableAction:" << action.PausableAction << endl; 
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "UcsId:" << action.shape->UcsId() << endl;
		os << "ActionShapeId:" << action.shape->getId() << endl;
		os << "FramegrabBase:Values" << endl << (*static_cast <FramegrabBase*>(action.getFramegrab())) << endl;
		os << "EndAddPointAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0007", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, AddPointAction& action)
{
	try
	{
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,action);
		}	
		else
		{
			UCS* ucs;
			int Ucsid;
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"AddPointAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::ADDPOINTACTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndAddPointAction")
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
						else if(Tagname==L"CriticalAction")
						{
							if(Val=="0")
								action.CriticalAction = false;
							else
								action.CriticalAction = true;
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
							Ucsid=atoi((const char*)(Val).c_str());
							ucs = MAINDllOBJECT->getUCS(Ucsid);
						}
						else if(Tagname==L"ActionShapeId")
						{
							int n=atoi((const char*)(Val).c_str()); 
							if(ucs->getShapes().ItemExists(n))
							{
								action.shape = (ShapeWithList*)(ucs->getShapes().getList()[n]);
								action.shape->UcsId(Ucsid);
								if(PPCALCOBJECT->PartprogramisLoadingEditMode())
									action.shape->PointAtionList.push_back(&action);
							}
						}
						else if(Tagname==L"FramegrabBase")
						{						
							action.baseFG = new FramegrabBase();
							action.baseFG->CucsId = Ucsid;
							is >> (*static_cast <FramegrabBase*>(action.getFramegrab()));
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
	catch(...)
	{
		MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0008", __FILE__, __FUNCSIG__);
		return is;
	}
}

void ReadOldPP(wistream& is, AddPointAction& action)
{
	try
	{
		wstring name, flagStr, flagStr2; int n, Ucsid;
		UCS* ucs;
		action.CurrentActionType = RapidEnums::ACTIONTYPE::ADDPOINTACTION;
		action.ActionStatus(true);
		is >> n; action.setId(n);
		is >> flagStr;
		std::string Str = RMATH2DOBJECT->WStringToString(flagStr);
		if(Str == "CriticalAction")
		{
			is >> flagStr2;
			std::string Str2 = RMATH2DOBJECT->WStringToString(flagStr2);
			if(Str2 == "PausableAction")
			{
				is >> action.PausableAction;
				is >> action.CriticalAction;		
				is >> name;
				action.setOriginalName(name);
			}
			else
			{
				if(Str2 == "1")
					action.CriticalAction = true;
				else
					action.CriticalAction = false;
				is >> name;
				action.setOriginalName(name);
			}
		}
		else
		{
			action.setOriginalName(flagStr);
		}
		is >> Ucsid; ucs = MAINDllOBJECT->getUCS(Ucsid);
		is >> n; 
		if(ucs->getShapes().ItemExists(n))
		{
			action.shape = (ShapeWithList*)(ucs->getShapes().getList()[n]);
			action.shape->UcsId(Ucsid);
			if(PPCALCOBJECT->PartprogramisLoadingEditMode())
				action.shape->PointAtionList.push_back(&action);
		}
		action.baseFG = new FramegrabBase();
		action.baseFG->CucsId = Ucsid;
		is >> (*static_cast <FramegrabBase*>(action.getFramegrab()));		
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0008", __FILE__, __FUNCSIG__); }
}
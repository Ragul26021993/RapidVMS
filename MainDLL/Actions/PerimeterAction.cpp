#include "StdAfx.h"
#include "PerimeterAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"
#include "..\Shapes\Perimeter.h"

PerimeterAction::PerimeterAction():RAction(_T("PerimeterAction"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::PERIMETERACTION;	
	PausableAction = false;
}

PerimeterAction::PerimeterAction(Shape* PerimeterShape, ShapeWithList *s, FramegrabBase* v):RAction(_T("PerimeterAction"))
{
	setParams(PerimeterShape, s, v);
	PausableAction = false;
}

PerimeterAction::~PerimeterAction()
{	
	try{ delete baseFG; }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERACTION0001", __FILE__, __FUNCSIG__); }
}

bool PerimeterAction::execute()
{
	try
	{
		if(shape != NULL && baseFG->AllPointsList.Pointscount() > 0)
		{		
		    shape->ManagePoint(baseFG, shape->ADD_POINT);			
			ActionStatus(true);
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERACTION0002", __FILE__, __FUNCSIG__); return false; }
}

bool PerimeterAction::redo()
{
	try
	{
		shape->PointAtionList.push_back(this);
		shape->Modify_Points(baseFG, shape->ADD_POINT);
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERACTION0003", __FILE__, __FUNCSIG__); return false; }
}

void PerimeterAction::undo()
{
	try
	{
		ActionStatus(false);
		shape->PointAtionList.remove(this);
		shape->Modify_Points(baseFG, shape->REMOVE_POINT);	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERACTION0004", __FILE__, __FUNCSIG__); }
}
 
void PerimeterAction::AddPerimeterAction(Shape* PShape, ShapeWithList* s, FramegrabBase* v)
{
	try
	{
		PerimeterAction *action = new PerimeterAction(PShape, s, v);
		action->CurrentActionType = RapidEnums::ACTIONTYPE::PERIMETERACTION;
		s->PointAtionList.push_back(action);
		MAINDllOBJECT->addAction(action);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERACTION0005", __FILE__, __FUNCSIG__); }
}


void PerimeterAction::setParams(Shape* PShape, ShapeWithList *s, FramegrabBase* v)
{
	try
	{
		this->PerimeterShape = PShape;
		this->shape = s;
		this->baseFG = v;
		ActionStatus(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERACTION0006", __FILE__, __FUNCSIG__); }
}

FramegrabBase* PerimeterAction::getFramegrab()
{
	return baseFG;
}

ShapeWithList* PerimeterAction::getShape()
{
	return this->shape;
}

wostream& operator<<(wostream& os, PerimeterAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;
		os << "PerimeterAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "PausableAction:" << action.PausableAction << endl; 
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "UcsId:" << action.shape->UcsId() << endl;
		os << "PerimeterShapeId:" << action.PerimeterShape->getId() << endl;
		os << "ActionShapeId:" << action.shape->getId() << endl;
		os << "FramegrabBase:Values" << endl << (*static_cast <FramegrabBase*>(action.getFramegrab())) << endl;
		os << "EndPerimeterAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PERACTION0007", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, PerimeterAction& action)
{
	try
	{
		
		UCS* ucs;
		int Ucsid;
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"PerimeterAction")
		{
			action.CurrentActionType = RapidEnums::ACTIONTYPE::PERIMETERACTION;
			action.ActionStatus(true);
			while(Tagname!=L"EndPerimeterAction")
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
					else if(Tagname==L"UcsId")
					{
						Ucsid=atoi((const char*)(Val).c_str());
						ucs = MAINDllOBJECT->getUCS(Ucsid);
					}
					else if(Tagname==L"PerimeterShapeId")
					{
						int n = atoi((const char*)(Val).c_str()); 
						action.PerimeterShape = (ShapeWithList*)(ucs->getShapes().getList()[n]);
						action.PerimeterShape->UcsId(Ucsid);
						if(PPCALCOBJECT->PartprogramisLoadingEditMode())
							((ShapeWithList*)action.PerimeterShape)->PointAtionList.push_back(&action);
					}
					else if(Tagname==L"ActionShapeId")
					{
						int n = atoi((const char*)(Val).c_str()); 
						action.shape = (ShapeWithList*)((Perimeter*)action.PerimeterShape)->PerimeterShapecollection[n];
						action.PerimeterShape->UcsId(Ucsid);
						if(PPCALCOBJECT->PartprogramisLoadingEditMode())
							action.shape->PointAtionList.push_back(&action);
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
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("PERACTION0008", __FILE__, __FUNCSIG__); return is; }
}
#include "StdAfx.h"
#include "Cad1PtAlignAction.h"
#include "..\Engine\RCadApp.h"
#include "..\DXF\DXFExpose.h"

Cad1PtAlignAction::Cad1PtAlignAction():RAction(_T("Cad1PtAlign"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::CAD_1PT_ALIGNACTION;
	this->IsEdited = false;
}

Cad1PtAlignAction::~Cad1PtAlignAction()
{
	if(windowNo == 0)
		delete TargetPnt;
	TargetPnt = NULL;
}

bool Cad1PtAlignAction::execute()
{
	try
	{
		ActionStatus(true);
		DXFEXPOSEOBJECT->CurrentDXFNonEditedActionList.push_back(this);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("1PtAct0001", __FILE__, __FUNCSIG__); return false; }
}
 
bool Cad1PtAlignAction::redo()
{
	try
	{
		if(!IsEdited)
		{
			Vector Shift;
			Shift.set(TargetPnt->getX() - this->OriginalPos.getX(), TargetPnt->getY() - this->OriginalPos.getY(), TargetPnt->getZ() - this->OriginalPos.getZ());
			TranslateShape(Shift);
			ActionStatus(true);
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("1PtAct0002", __FILE__, __FUNCSIG__); return false; }
}

void Cad1PtAlignAction::undo()
{
	try
	{	if(!IsEdited)
		{		
			ActionStatus(false);
			Vector Shift;
			Shift.set(this->OriginalPos.getX() - TargetPnt->getX(), this->OriginalPos.getY() - TargetPnt->getY(), this->OriginalPos.getZ() - TargetPnt->getZ());
			TranslateShape(Shift);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("1PtAct0003", __FILE__, __FUNCSIG__); }
}

void Cad1PtAlignAction::SetCad1PtAlign(Vector& pt, Vector* v, int ucsID, int windowno, std::list<Shape*> DXFShapeList)
{
	try
	{		
		Cad1PtAlignAction* CptAct = new Cad1PtAlignAction();
		CptAct->OriginalPos = pt;
		CptAct->Point1.set(v->getX(), v->getY(), v->getZ());
	    CptAct->TargetPnt = v;
		CptAct->UcsID = ucsID;
		CptAct->windowNo = windowno;
		for(list<Shape*>::iterator It = DXFShapeList.begin(); It != DXFShapeList.end(); It++)
		{
		    CptAct->AlignedShape[(*It)->getId()]  = *It; 
		}
		MAINDllOBJECT->addAction(CptAct);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("1PtAct0005", __FILE__, __FUNCSIG__); }
}

void Cad1PtAlignAction::EditCurrentAlignment(map<int, int> shapemap)
{
	try
	{
		IsEdited = true;
		map<int, Shape*> tmpList;
		for(map<int,Shape*>::iterator It = AlignedShape.begin(); It != AlignedShape.end(); It++)
		{
            if(MAINDllOBJECT->getUCS(UcsID)->getShapes().ItemExists(shapemap[It->first]))
			{
    			if(shapemap.find(It->first) != shapemap.end())
				{
					tmpList[It->first] = (Shape*)MAINDllOBJECT->getUCS(UcsID)->getShapes().getList()[shapemap[It->first]];
				}
			}
		}
		AlignedShape.clear();
		AlignedShape = tmpList;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("1PtAct0005b", __FILE__, __FUNCSIG__); }
}

void Cad1PtAlignAction::TranslateShape(Vector& shift)
{
	for(map<int,Shape*>::iterator It = AlignedShape.begin(); It != AlignedShape.end(); It++)
	{
    	It->second->Translate(shift);
	}
	MAINDllOBJECT->DXFShape_Updated();
}

Vector* Cad1PtAlignAction::getPoint1()
{
	try
	{
		return &(this->Point1);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("1PtAct0006", __FILE__, __FUNCSIG__); }
}

void Cad1PtAlignAction::SetPoint1(Vector& pt1)
{
	try
	{
		this->Point1 = pt1;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0006", __FILE__, __FUNCSIG__); }
}

int Cad1PtAlignAction::getUcsId()
{
	return this->UcsID;
}

wostream& operator<<(wostream& os, Cad1PtAlignAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;
		os << "Cad1PtAlignAction"<< endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "UcsID:" << action.UcsID << endl;
		os << "WindowNo:" << action.windowNo << endl;
		os << "getPoint1:Values" << endl << (*static_cast<Vector*>(action.getPoint1())) << endl;
		os << "IsEdited:" << action.IsEdited << endl;
		for(map<int, Shape*>::iterator shapeiterator = action.AlignedShape.begin(); shapeiterator != action.AlignedShape.end(); shapeiterator++)
			os << "getshapeiterator:" << shapeiterator->second->getId() << endl;
		os << "EndCad1PtAlignAction"<< endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1PtAct0008", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, Cad1PtAlignAction& action)
{
	try
	{
			int Ucsid;
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"Cad1PtAlignAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::CAD_1PT_ALIGNACTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndCad1PtAlignAction")
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
						else if(Tagname==L"UcsID")
						{	
							Ucsid=atoi((const char*)(Val).c_str());
							action.UcsID = Ucsid;
						}
						else if(Tagname==L"WindowNo")
						{	
							Ucsid=atoi((const char*)(Val).c_str());
							action.windowNo = Ucsid;
						}
						else if(Tagname==L"getPoint1")
						{	
							is >> (*static_cast<Vector*>(&action.Point1));	
							UCS* ucs = MAINDllOBJECT->getUCS(action.UcsID);
							if(action.windowNo == 1)
						    	action.TargetPnt = MAINDllOBJECT->getVectorPointer_UCS(&action.Point1, ucs, true);			
							else
								action.TargetPnt = new Vector(action.Point1);
						}
						else if(Tagname==L"IsEdited")
						{
							if(Val=="0")
							{
								action.IsEdited = false;
							}
							else
								action.IsEdited = true;
						}
						else if(Tagname==L"getshapeiterator")
						{
							int j =atoi((const char*)(Val).c_str());
							if(action.IsEdited)
							{
								if(MAINDllOBJECT->getCurrentUCS().getShapes().ItemExists(j))
								   action.AlignedShape[j] = (Shape*)MAINDllOBJECT->getUCS(Ucsid)->getShapes().getList()[j];
							}
							else if(DXFEXPOSEOBJECT->PPDXFShapeList.find(j) != DXFEXPOSEOBJECT->PPDXFShapeList.end())
								    action.AlignedShape[j] = DXFEXPOSEOBJECT->PPDXFShapeList.find(j)->second;
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1PtAct0009", __FILE__, __FUNCSIG__); return is; }
}
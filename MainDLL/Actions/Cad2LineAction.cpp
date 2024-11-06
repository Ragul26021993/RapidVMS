#include "StdAfx.h"
#include "Cad2LineAction.h"
#include "..\Engine\RCadApp.h"
#include "..\DXF\DXFExpose.h"

Cad2LineAction::Cad2LineAction():RAction(_T("Cad2LineAlign"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::CAD_2LN_ALIGNACTION;
}

Cad2LineAction::~Cad2LineAction()
{
}

bool Cad2LineAction::execute()
{
	try
	{
		ActionStatus(true);
		DXFEXPOSEOBJECT->CurrentDXFNonEditedActionList.push_back(this);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2LNAct0001", __FILE__, __FUNCSIG__); return false; }
}
 
bool Cad2LineAction::redo()
{
	try
	{
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2LNAct0002", __FILE__, __FUNCSIG__); return false; }
}

void Cad2LineAction::undo()
{
	try
	{				
		ActionStatus(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2LNAct0003", __FILE__, __FUNCSIG__); }
}

void Cad2LineAction::SetCad2LineAlign(Shape *dxfShape1, Shape *dxfShape2, Shape *mainShape1, Shape *mainShape2, int ucsID, int windowno, std::list<Shape*> DXFShapeList)
{
	try
	{		
		Cad2LineAction* CptAct = new Cad2LineAction();
		CptAct->DxfShape1 = dxfShape1;
		CptAct->DxfShape2 = dxfShape2;
		CptAct->MainShape1 = mainShape1;
		CptAct->MainShape2 = mainShape2;
		CptAct->UcsID = ucsID;
		CptAct->windowNo = windowno;
		for(list<Shape*>::iterator It = DXFShapeList.begin(); It != DXFShapeList.end(); It++)
		{
		    CptAct->AlignedShape[(*It)->getId()]  = *It; 
		}
		MAINDllOBJECT->addAction(CptAct);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2LNAct0004", __FILE__, __FUNCSIG__); }
}

int Cad2LineAction::getUcsId()
{
	return this->UcsID;
}

void Cad2LineAction::EditCurrentAlignment(map<int, int> shapemap)
{
	try
	{
		IsEdited = true;
		map<int, Shape*> tmpList;
		for(map<int,Shape*>::iterator It = AlignedShape.begin(); It != AlignedShape.end(); It++)
		{

    		if(shapemap.find(It->first) != shapemap.end())
				tmpList[It->first] = (Shape*)MAINDllOBJECT->getUCS(UcsID)->getShapes().getList()[shapemap[It->first]];
		}
		DxfShape1 = (Shape*)MAINDllOBJECT->getUCS(UcsID)->getShapes().getList()[shapemap[DxfShape1->getId()]];
		DxfShape2 = (Shape*)MAINDllOBJECT->getUCS(UcsID)->getShapes().getList()[shapemap[DxfShape2->getId()]];
		AlignedShape.clear();
		AlignedShape = tmpList;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("1PtAct0005b", __FILE__, __FUNCSIG__); }
}

void Cad2LineAction::TranslateShape(Vector& shift)
{
	for(map<int,Shape*>::iterator It = AlignedShape.begin(); It != AlignedShape.end(); It++)
	{
    	It->second->Translate(shift);
	}
	MAINDllOBJECT->DXFShape_Updated();
}

void Cad2LineAction::TransformShape(double* matrix)
{
	for(map<int,Shape*>::iterator It = AlignedShape.begin(); It != AlignedShape.end(); It++)
	{
    	It->second->Transform(matrix);
	}
	MAINDllOBJECT->DXFShape_Updated();
}


wostream& operator<<(wostream& os, Cad2LineAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;
		os << "Cad2LineAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "UcsID:" << action.UcsID << endl;
		os << "WindowNo:" << action.windowNo << endl;
		os << "ShapeId0:" << action.DxfShape1->getId() << endl;
		os << "ShapeId1:" << action.DxfShape2->getId() << endl;
		os << "ShapeId2:" << action.MainShape1->getId() << endl;
		os << "ShapeId3:" << action.MainShape2->getId() << endl;
		for(map<int, Shape*>::iterator shapeiterator = action.AlignedShape.begin(); shapeiterator != action.AlignedShape.end(); shapeiterator++)
			os << "getshapeiterator:" << shapeiterator->second->getId() << endl;
		os << "EndCad2LineAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2LNAct0006", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, Cad2LineAction& action)
{
	try
	{
		int id;
		std::wstring Tagname;
		RC_ITER Item_ucs = MAINDllOBJECT->getUCSList().getList().begin();
		is >> Tagname;
		if(Tagname==L"Cad2LineAction")
		{
			action.CurrentActionType = RapidEnums::ACTIONTYPE::CAD_2LN_ALIGNACTION;
			action.ActionStatus(true);
			while(Tagname!=L"EndCad2LineAction")
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
						id=atoi((const char*)(Val).c_str());
						action.UcsID = id;
					}
					else if(Tagname==L"WindowNo")
					{	
						id=atoi((const char*)(Val).c_str());
						action.windowNo = id;
					}
					else if(Tagname==L"ShapeId0")
					{	
						id = atoi((const char*)(Val).c_str());
						if(action.IsEdited)
						{
							if(MAINDllOBJECT->getCurrentUCS().getShapes().ItemExists(id))
							   action.DxfShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[id];
						}
						else if(DXFEXPOSEOBJECT->PPDXFShapeList.find(id) != DXFEXPOSEOBJECT->PPDXFShapeList.end())
							   action.DxfShape1 = DXFEXPOSEOBJECT->PPDXFShapeList.find(id)->second;
					}
					else if(Tagname==L"ShapeId1")
					{	
						id = atoi((const char*)(Val).c_str());
						if(action.IsEdited)
						{
							if(MAINDllOBJECT->getCurrentUCS().getShapes().ItemExists(id))
							   action.DxfShape2 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[id];
						}
						else if(DXFEXPOSEOBJECT->PPDXFShapeList.find(id) != DXFEXPOSEOBJECT->PPDXFShapeList.end())
							   action.DxfShape2 = DXFEXPOSEOBJECT->PPDXFShapeList.find(id)->second;
					}
					else if(Tagname==L"ShapeId2")
					{	
						id = atoi((const char*)(Val).c_str());		
						if(MAINDllOBJECT->getCurrentUCS().getShapes().ItemExists(id))
							action.MainShape1 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[id];
					}		
					else if(Tagname==L"ShapeId3")
					{	
						id = atoi((const char*)(Val).c_str());		
						if(MAINDllOBJECT->getCurrentUCS().getShapes().ItemExists(id))
							action.MainShape2 = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[id];
					}		
					else if(Tagname==L"getshapeiterator")
					{
						int j =atoi((const char*)(Val).c_str());
						if(action.IsEdited)
						{
							if(MAINDllOBJECT->getCurrentUCS().getShapes().ItemExists(j))
								action.AlignedShape[j] = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[j];
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2LNAct0007", __FILE__, __FUNCSIG__); return is; }
}

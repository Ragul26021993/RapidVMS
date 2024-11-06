#include "StdAfx.h"
#include "Cad1Pt1LineAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\Line.h"
#include "..\DXF\DXFExpose.h"

Cad1Pt1LineAction::Cad1Pt1LineAction():RAction(_T("Cad1Pt1lineAlign"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::CAD_1PT1LN_ALIGNACTION;
	IsEdited = false;
}

Cad1Pt1LineAction::~Cad1Pt1LineAction()
{
	if(windowNo == 0)
		delete targetP;
	targetP = NULL;
}

bool Cad1Pt1LineAction::execute()
{
	try
	{
		ActionStatus(true);
		DXFEXPOSEOBJECT->CurrentDXFNonEditedActionList.push_back(this);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0001", __FILE__, __FUNCSIG__); return false; }
}
 
bool Cad1Pt1LineAction::redo()
{
	try
	{
		if(!IsEdited)
		{
			double pt1[2] = {OldPoint.getX(), OldPoint.getY()}, pt2[2] = {Point.getX(), Point.getY()};
			double slope1 = ((Line*)DxfShape)->Angle(), slope2 = ((Line*)MainShape)->Angle(), transform[9] = {0};
			double RotatedAngle, ShiftedOrigin[2] = { 0 };
			RMATH2DOBJECT->TransformM_PointLineAlign(&pt1[0], ((Line*)DxfShape)->Angle(), ((Line*)DxfShape)->Intercept(), &pt2[0], 
							((Line*)MainShape)->Angle(), ((Line*)MainShape)->Intercept(), &transform[0], &RotatedAngle, ShiftedOrigin);
			Vector TransPt;
			TransPt.set(transform[2], transform[5]);
			transform[2] = 0; transform[5] = 0;
			TransformShape(&transform[0]);
			TranslateShape(TransPt);
			ActionStatus(true);
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0002", __FILE__, __FUNCSIG__); return false; }
}

void Cad1Pt1LineAction::undo()
{
	try
	{				
		if(!IsEdited)
		{
			double pt1[2] = {OldPoint.getX(), OldPoint.getY()}, pt2[2] = {Point.getX(), Point.getY()};
			double transform[9] = {0};
			double RotatedAngle, ShiftedOrigin[2] = { 0 };
			RMATH2DOBJECT->TransformM_PointLineAlign(&pt2[0], ((Line*)MainShape)->Angle(), ((Line*)MainShape)->Intercept(), &pt1[0], 
							((Line*)DxfShape)->Angle(), ((Line*)DxfShape)->Intercept(), &transform[0], &RotatedAngle, ShiftedOrigin);
			Vector TransPt;
			TransPt.set(transform[2], transform[5]);
			transform[2] = 0; transform[5] = 0;
			TransformShape(&transform[0]);
			TranslateShape(TransPt);
			ActionStatus(false);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0003", __FILE__, __FUNCSIG__); }
}

void Cad1Pt1LineAction::SetCad1Pt1LineAlign(Shape *dxfShape, Shape *videoShape, Vector& pt, Vector* v, int ID, int windowno, std::list<Shape*> DXFShapeList)
{
	try
	{		
		Cad1Pt1LineAction* CptAct = new Cad1Pt1LineAction();
		CptAct->OldPoint = pt;
		CptAct->Point.set(v->getX(), v->getY());
		CptAct->targetP = v;
		CptAct->UcsID = ID;
		CptAct->DxfShape = dxfShape;
		CptAct->MainShape = videoShape;
		CptAct->windowNo = windowno;
		for(list<Shape*>::iterator It = DXFShapeList.begin(); It != DXFShapeList.end(); It++)
		{
			CptAct->AlignedShape[(*It)->getId()]  = *It; 
		}
		MAINDllOBJECT->addAction(CptAct);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0004", __FILE__, __FUNCSIG__); }
}

Vector* Cad1Pt1LineAction::getPoint1()
{
	try
	{
		return &(this->Point);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0005", __FILE__, __FUNCSIG__); }
}

void Cad1Pt1LineAction::SetPoint1(Vector& pt1)
{
	try
	{
		this->Point = pt1;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0006", __FILE__, __FUNCSIG__); }
}

int Cad1Pt1LineAction::getUcsId()
{
	return this->UcsID;
}

void Cad1Pt1LineAction::EditCurrentAlignment(map<int, int> shapemap)
{
	try
	{
		IsEdited = true;
		map<int, Shape*> tmpList;
		for(map<int,Shape*>::iterator It = AlignedShape.begin(); It != AlignedShape.end(); It++)
		{

			if(shapemap.find(It->first) != shapemap.end())
			{
				if(MAINDllOBJECT->getUCS(UcsID)->getShapes().ItemExists(shapemap[It->first]))
					tmpList[It->first] = (Shape*)MAINDllOBJECT->getUCS(UcsID)->getShapes().getList()[shapemap[It->first]];
			}
		}
		if(MAINDllOBJECT->getUCS(UcsID)->getShapes().ItemExists(DxfShape->getId()))
			DxfShape = (Shape*)MAINDllOBJECT->getUCS(UcsID)->getShapes().getList()[shapemap[DxfShape->getId()]];
		AlignedShape.clear();
		AlignedShape = tmpList;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("1PtAct0005b", __FILE__, __FUNCSIG__); }
}

void Cad1Pt1LineAction::TranslateShape(Vector& shift)
{
	for(map<int,Shape*>::iterator It = AlignedShape.begin(); It != AlignedShape.end(); It++)
	{
		It->second->Translate(shift);
	}
	MAINDllOBJECT->DXFShape_Updated();
}

void Cad1Pt1LineAction::TransformShape(double* matrix)
{
	for(map<int,Shape*>::iterator It = AlignedShape.begin(); It != AlignedShape.end(); It++)
	{
		It->second->Transform(matrix);
	}
	MAINDllOBJECT->DXFShape_Updated();
}

wostream& operator<<(wostream& os, Cad1Pt1LineAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;
		os << "Cad1Pt1LineAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "IsEdited:" << action.IsEdited << endl;
		os << "UcsID:" << action.UcsID << endl;
		os << "WindowNo:" << action.windowNo << endl;
		os << "ShapeId0:" << action.DxfShape->getId() << endl;
		os << "ShapeId1:" << action.MainShape->getId() << endl;
		os << "getPoint1:values" << endl << (*static_cast<Vector*>(action.targetP)) << endl;
		for(map<int, Shape*>::iterator shapeiterator = action.AlignedShape.begin(); shapeiterator != action.AlignedShape.end(); shapeiterator++)
			os << "getshapeiterator:" << shapeiterator->second->getId() << endl;
		os << "EndCad1Pt1LineAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0007", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, Cad1Pt1LineAction& action)
{
	try
	{
		int id;
		std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"Cad1Pt1LineAction")
		{
			action.CurrentActionType = RapidEnums::ACTIONTYPE::CAD_1PT1LN_ALIGNACTION;
			action.ActionStatus(true);
			while(Tagname!=L"EndCad1Pt1LineAction")
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
					else if(Tagname==L"ShapeId0")
					{	
						id = atoi((const char*)(Val).c_str());
						if(action.IsEdited)
						{
							if(MAINDllOBJECT->getCurrentUCS().getShapes().ItemExists(id))
							   action.DxfShape = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[id];
						}
						else if(DXFEXPOSEOBJECT->PPDXFShapeList.find(id) != DXFEXPOSEOBJECT->PPDXFShapeList.end())
							   action.DxfShape = DXFEXPOSEOBJECT->PPDXFShapeList.find(id)->second;
					}
					else if(Tagname==L"ShapeId1")
					{	
						id = atoi((const char*)(Val).c_str());		
						if(MAINDllOBJECT->getCurrentUCS().getShapes().ItemExists(id))
							action.MainShape = (Shape*)MAINDllOBJECT->getCurrentUCS().getShapes().getList()[id];
					}		
					else if(Tagname==L"WindowNo")
					{	
						id=atoi((const char*)(Val).c_str());
						action.windowNo = id;
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
					else if(Tagname==L"getPoint1")
					{	
						is >> (*static_cast<Vector*>(&action.Point));	
						UCS* ucs = MAINDllOBJECT->getUCS(action.UcsID);
						if(action.windowNo == 1)
							action.targetP = MAINDllOBJECT->getVectorPointer_UCS(&action.Point, ucs, true);			
						else
							action.targetP = new Vector(action.Point);
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0008", __FILE__, __FUNCSIG__); return is; }
}

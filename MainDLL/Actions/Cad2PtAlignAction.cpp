#include "StdAfx.h"
#include "Cad2PtAlignAction.h"
#include "..\Engine\RCadApp.h"
#include "..\DXF\DXFExpose.h"
#include "..\Shapes\ShapeWithList.h"

Cad2PtAlignAction::Cad2PtAlignAction():RAction(_T("Cad1PtAlign"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION;
	action1Finished = false;
	action2Finished = false;
	IsEdited = false;
}

Cad2PtAlignAction::~Cad2PtAlignAction()
{
	if(clickWindow1 == 0)
		delete targetP1;
	if(clickWindow2 == 0)
		delete targetP2;
}

bool Cad2PtAlignAction::execute()
{
	try
	{
		ActionStatus(true);
		DXFEXPOSEOBJECT->CurrentDXFNonEditedActionList.push_back(this);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("1PtAct0001", __FILE__, __FUNCSIG__); return false; }
}
 
bool Cad2PtAlignAction::redo()
{
	try
	{
		if(!IsEdited)
		{
			Vector Shift;
			Shift.set(Points[0].getX() - OriginalPnts[0].getX(), Points[0].getY() - OriginalPnts[0].getY(), Points[0].getZ() - OriginalPnts[0].getZ());
			TranslateShape(Shift);
			double Angle1 = RMATH2DOBJECT->ray_angle(OriginalPnts[0].getX(), OriginalPnts[0].getY(), OriginalPnts[1].getX(), OriginalPnts[1].getY());
			double Angle2 = RMATH2DOBJECT->ray_angle(Points[0].getX(), Points[0].getY(), Points[1].getX(), Points[1].getY());
			double AngDiff = Angle2 - Angle1;
			double pt[3] = {Points[0].getX(), Points[0].getY(), Points[0].getZ()};
			double transform[9] = {0};
			RMATH2DOBJECT->TransformM_RotateAboutPoint(&pt[0], AngDiff, &transform[0]);
			Vector TransPt;
			TransPt.set(transform[2], transform[5]);
			transform[2] = 0; transform[5] = 0;
			TransformShape(&transform[0]);
			TranslateShape(TransPt);
			ActionStatus(true);
		}
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("1PtAct0002", __FILE__, __FUNCSIG__); return false; }
}

void Cad2PtAlignAction::undo()
{
	try
	{		
		if(!IsEdited)
		{
			Vector Shift;
			Shift.set(OriginalPnts[0].getX() - Points[0].getX(), OriginalPnts[0].getY() - Points[0].getY(), OriginalPnts[0].getZ() - Points[0].getZ());
			TranslateShape(Shift);
			double Angle1 = RMATH2DOBJECT->ray_angle(Points[0].getX(), Points[0].getY(), Points[1].getX(), Points[1].getY());
			double Angle2 = RMATH2DOBJECT->ray_angle(OriginalPnts[0].getX(), OriginalPnts[0].getY(), OriginalPnts[1].getX(), OriginalPnts[1].getY());
			double AngDiff = Angle2 - Angle1;
			double pt[3] = {OriginalPnts[0].getX(), OriginalPnts[0].getY(), OriginalPnts[0].getZ()};
			double transform[9] = {0};
			RMATH2DOBJECT->TransformM_RotateAboutPoint(&pt[0], AngDiff, &transform[0]);
			Vector TransPt;
			TransPt.set(transform[2], transform[5]);
			transform[2] = 0; transform[5] = 0;
			TransformShape(&transform[0]);
			TranslateShape(TransPt);
			ActionStatus(false);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("1PtAct0003", __FILE__, __FUNCSIG__); }
}

void Cad2PtAlignAction::SetCad2PtAlign(Vector& pt1, Vector& pt2, Vector *targetPnt1, Vector *targetPnt2,int ucsId, int *windowNo, std::list<Shape*> DXFShapeList)
{
	try
	{
		Cad2PtAlignAction* CptAct = new Cad2PtAlignAction();
		CptAct->OriginalPnts[0] = pt1;
		CptAct->OriginalPnts[1] = pt2;
		CptAct->Points[0] = Vector(targetPnt1->getX(), targetPnt1->getY(), targetPnt1->getZ());
		CptAct->Points[1] = Vector(targetPnt2->getX(), targetPnt2->getY(), targetPnt2->getZ());
		CptAct->targetP1 = targetPnt1;
		CptAct->targetP2 = targetPnt2;
		CptAct->clickWindow1 = windowNo[0];
		CptAct->clickWindow2 = windowNo[1];
		CptAct->UcsId = ucsId;
		for(list<Shape*>::iterator It = DXFShapeList.begin(); It != DXFShapeList.end(); It++)
		{
		    CptAct->AlignedShape[(*It)->getId()]  = *It; 
		}
		MAINDllOBJECT->addAction(CptAct);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("1PtAct0006", __FILE__, __FUNCSIG__); }
}

int Cad2PtAlignAction::getUcsId()
{
	return this->UcsId;
}

Vector* Cad2PtAlignAction::getPoint1()
{
	try
	{
		return &Points[0];
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("1PtAct0008", __FILE__, __FUNCSIG__); }
}

Vector* Cad2PtAlignAction::getPoint2()
{
	try
	{
		return &Points[1];
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("1PtAct0009", __FILE__, __FUNCSIG__); }
}

void Cad2PtAlignAction::SetPoint1(Vector& pt1)
{
	try
	{
		this->Points[0] = pt1;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0006", __FILE__, __FUNCSIG__); }
}

void Cad2PtAlignAction::SetPoint2(Vector& pt1)
{
	try
	{
		this->Points[1] = pt1;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1Pt1LnAct0006", __FILE__, __FUNCSIG__); }
}

void Cad2PtAlignAction::EditCurrentAlignment(map<int, int> shapemap)
{
	try
	{
		IsEdited = true;
		map<int, Shape*> tmpList;
		for(map<int,Shape*>::iterator It = AlignedShape.begin(); It != AlignedShape.end(); It++)
		{

    		if(shapemap.find(It->first) != shapemap.end())
				tmpList[It->first] = (Shape*)MAINDllOBJECT->getUCS(UcsId)->getShapes().getList()[shapemap[It->first]];
		}
		AlignedShape.clear();
		AlignedShape = tmpList;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("1PtAct0005b", __FILE__, __FUNCSIG__); }
}

void Cad2PtAlignAction::TranslateShape(Vector& shift)
{
	for(map<int,Shape*>::iterator It = AlignedShape.begin(); It != AlignedShape.end(); It++)
	{
    	It->second->Translate(shift);
	}
	MAINDllOBJECT->DXFShape_Updated();
}

void Cad2PtAlignAction::TransformShape(double* matrix)
{
	for(map<int,Shape*>::iterator It = AlignedShape.begin(); It != AlignedShape.end(); It++)
	{
    	It->second->Transform(matrix);
	}
	MAINDllOBJECT->DXFShape_Updated();
}

wostream& operator<<(wostream& os, Cad2PtAlignAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;
		os << "Cad2PtAlignAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "IsEdited:" << action.IsEdited << endl;
		os << "UcsID:" << action.UcsId << endl;
		os << "WindowNo1:" << action.clickWindow1 << endl;
		os << "WindowNo2:" << action.clickWindow2 << endl;
		os << "getPoint1:" << endl << (*static_cast<Vector*>(action.targetP1)) << endl;
		os << "getPoint2:" << endl << (*static_cast<Vector*>(action.targetP2)) << endl;
		for(map<int, Shape*>::iterator shapeiterator = action.AlignedShape.begin(); shapeiterator != action.AlignedShape.end(); shapeiterator++)
			os << "getshapeiterator:" << shapeiterator->second->getId() << endl;
		os << "EndCad2PtAlignAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("Ed2PtAct0008", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, Cad2PtAlignAction& action)
{
	try
	{
		int id;
    	std::wstring Tagname ;
		is >> Tagname;
		if(Tagname==L"Cad2PtAlignAction")
		{
			action.CurrentActionType = RapidEnums::ACTIONTYPE::CAD_2PT_ALIGNACTION;
			action.ActionStatus(true);
			while(Tagname!=L"EndCad2PtAlignAction")
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
						id = atoi((const char*)(Val).c_str());
						action.UcsId = id;
					}
					else if(Tagname==L"WindowNo1")
					{	
						id=atoi((const char*)(Val).c_str());
						action.clickWindow1 = id;
					}
					else if(Tagname==L"WindowNo2")
					{	
						id=atoi((const char*)(Val).c_str());
						action.clickWindow2 = id;
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
						is >> (*static_cast<Vector*>(&action.Points[0]));
						UCS* ucs = MAINDllOBJECT->getUCS(action.UcsId);
						if(action.clickWindow1 == 1)
							action.targetP1 = MAINDllOBJECT->getVectorPointer_UCS(&action.Points[0], ucs, true);			
						else
							action.targetP1 = new Vector(action.Points[0]);
					}
					else if(Tagname==L"getPoint2")
					{	
						is >> (*static_cast<Vector*>(&action.Points[1]));	
						UCS* ucs = MAINDllOBJECT->getUCS(action.UcsId);
						if(action.clickWindow2 == 1)
							action.targetP2 = MAINDllOBJECT->getVectorPointer_UCS(&action.Points[1], ucs, true);			
						else
							action.targetP2 = new Vector(action.Points[1]);
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("Ed1PtAct0009", __FILE__, __FUNCSIG__); return is; }
}
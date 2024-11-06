#include "StdAfx.h"
#include "CopyShapesAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Shapes\Shape.h"
#include "..\Shapes\CloudPoints.h"
CopyShapesAction::CopyShapesAction():RAction(_T("CopyShapes"))
{
	try
	{
		this->CurrentActionType = RapidEnums::ACTIONTYPE::COPYSHAPEACTION;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0001", __FILE__, __FUNCSIG__); }
}

CopyShapesAction::~CopyShapesAction()
{
	try
	{
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0002", __FILE__, __FUNCSIG__); }
}

bool CopyShapesAction::execute()
{
	try
	{
		RCollectionBase& rshapes = MAINDllOBJECT->getUCS(targetUcsId)->getShapes();
		if(sourceUcsId != targetUcsId)
		{
			UCS *sourceUcs = MAINDllOBJECT->getUCS(sourceUcsId);
			UCS *targetUcs = MAINDllOBJECT->getUCS(targetUcsId);
			if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 4)
			{
				for(std::list<Shape*>::iterator item = copiedshapes.begin();item != copiedshapes.end();item++)
				{
					Shape* ParentShape = (Shape*)(*item);
					Shape* Cshape = ParentShape->Clone(0, false);
					Cshape->UcsId(targetUcsId);
					Cshape->adducsCopyRelation(ParentShape);
					Cshape->CopyOriginalProperties(ParentShape);
					if(Cshape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
						((CloudPoints*)Cshape)->CopyPoints(ParentShape);
					Cshape->AlignToBasePlane(&(MAINDllOBJECT->getCurrentUCS().transform[0]));
					rshapes.addItem(Cshape);
					PastedShapes.push_back(Cshape);
					PastedShapesId.push_back(Cshape->getId());
					Cshape->UpdateEnclosedRectangle();
				}
			} 
			else if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 2)
			{
				double p1[2] = {sourceUcs->UCSPoint.getX(), sourceUcs->UCSPoint.getY()}, p2[2] = {targetUcs->UCSPoint.getX(), targetUcs->UCSPoint.getY()};
				double SourceAngle = sourceUcs->UCSangle(), DstAngle = targetUcs->UCSangle(), transformMatrix[9];
				double SourceIntercept = sourceUcs->UCSIntercept(), DstIntercept = targetUcs->UCSIntercept();
				double RotatedAngle, ShiftedOrigin[2] = { 0 };
				RMATH2DOBJECT->TransformM_PointLineAlign(&p2[0], DstAngle, DstIntercept, &p1[0], SourceAngle, SourceIntercept, &transformMatrix[0], &RotatedAngle, ShiftedOrigin);
				transformMatrix[2] = 0; transformMatrix[5] = 0;
				double transformMParent[9] = {0};
				if(targetUcs->ParentUcsId() != 0)
				{
					if(sourceUcs->UCSProjectionType() == 1)  
					{
						transformMParent[1] = 1;   transformMParent[5] = 1;   transformMParent[6] = 1; 
					}
					else if(sourceUcs->UCSProjectionType() == 0)
					{
						transformMParent[0] = 1;   transformMParent[4] = 1;   transformMParent[8] = 1; 
					}
					else if(sourceUcs->UCSProjectionType() == 2) 
					{
						transformMParent[0] = 1;   transformMParent[5] = 1;   transformMParent[7] = 1; 
					}	
				}
				for(std::list<Shape*>::iterator item = copiedshapes.begin();item != copiedshapes.end();item++)
				{
					Shape* ParentShape = (Shape*)(*item);
					Shape* Cshape = ParentShape->Clone(0, false);
					Cshape->adducsCopyRelation(ParentShape);
					Cshape->CopyOriginalProperties(ParentShape);
					if(Cshape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
						((CloudPoints*)Cshape)->CopyPoints(ParentShape);
					Cshape->UcsId(targetUcsId);
					if((targetUcs->ParentUcsId() != 0) && (ParentShape->UcsId() == 0))
				    {
						Vector tmpv = Vector(- p1[0], -p1[1], -sourceUcs->UCSPoint.getZ());
					    Cshape->Translate(tmpv);
					    Cshape->Transform(transformMParent);
					}
					Vector tmpv = Vector(- p2[0], - p2[1], -targetUcs->UCSPoint.getZ());
					Cshape->Translate(tmpv);
					Cshape->Transform(transformMatrix);				   
					rshapes.addItem(Cshape);
					PastedShapes.push_back(Cshape);
					PastedShapesId.push_back(Cshape->getId());
					Cshape->UpdateEnclosedRectangle();
					Cshape->notifyAll(ITEMSTATUS::DATACHANGED, Cshape);
				}	
			}
			else if(MAINDllOBJECT->getCurrentUCS().UCSMode() == 1)
			{
				double transformM[9] = {0};
				if(targetUcs->UCSProjectionType() == 1)  
				{
				      transformM[1] = 1;   transformM[5] = 1;   transformM[6] = 1; 
				}
				else if(targetUcs->UCSProjectionType() == 0)
				{
				     transformM[0] = 1;   transformM[4] = 1;   transformM[8] = 1; 
				}
				else if(targetUcs->UCSProjectionType() == 2) 
				{
				      transformM[0] = 1;   transformM[5] = 1;   transformM[7] = 1; 
				}
				double p1[2] = {sourceUcs->UCSPoint.getX(), sourceUcs->UCSPoint.getY()}, p2[2] = {targetUcs->UCSPoint.getX(), targetUcs->UCSPoint.getY()};
				for(std::list<Shape*>::iterator item = copiedshapes.begin();item != copiedshapes.end();item++)
				{
					Shape* ParentShape = (Shape*)(*item);
					Shape* Cshape = ParentShape->Clone(0, false);
					Cshape->adducsCopyRelation(ParentShape);
					Cshape->CopyOriginalProperties(ParentShape);
					if(Cshape->ShapeType == RapidEnums::SHAPETYPE::CLOUDPOINTS)
						((CloudPoints*)Cshape)->CopyPoints(ParentShape);
					Cshape->UcsId(targetUcsId);
					Vector tmpv = Vector(p1[0] - p2[0], p1[1] - p2[1], sourceUcs->UCSPoint.getZ() -targetUcs->UCSPoint.getZ());
					Cshape->Translate(tmpv);
					Cshape->Transform(transformM);
				  	rshapes.addItem(Cshape);
					PastedShapes.push_back(Cshape);
					PastedShapesId.push_back(Cshape->getId());
					Cshape->UpdateEnclosedRectangle();
				}	
			}
		}
		else
		{
			if(copiedshapes.size() > 0)
			{
				MAINDllOBJECT->getShapesList().clearSelection();
				for (std::list<Shape*>::iterator item = copiedshapes.begin(); item !=copiedshapes.end(); item++)
				{
					Shape* Cshape = ((Shape*)(*item))->Clone(0);
					Cshape->selected(true, false);
			        rshapes.addItem(Cshape);
					MAINDllOBJECT->getSelectedShapesList().addItem(Cshape, false);
					MAINDllOBJECT->CurrentCopiedShapeList.push_back(Cshape);
					PastedShapes.push_back(Cshape);
					PastedShapesId.push_back(Cshape->getId());
				}
		 	}
		}
		MAINDllOBJECT->getShapesList().notifySelection();
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0003", __FILE__, __FUNCSIG__); return false; }
}

bool CopyShapesAction::redo()
{
	try
	{
		RCollectionBase& rshapes = MAINDllOBJECT->getUCS(targetUcsId)->getShapes();
		for (list<Shape*>::iterator i = PastedShapes.begin(); i != PastedShapes.end(); i++)
		{
			BaseItem* item = (*i);
			rshapes.addItem(item);
			if(item->selected())
				MAINDllOBJECT->getSelectedShapesList().addItem(item, false);
		}
		MAINDllOBJECT->getShapesList().notifySelection();
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0004", __FILE__, __FUNCSIG__); return false; }
}

void CopyShapesAction::undo()
{
	try
	{
		RCollectionBase& rshapes = MAINDllOBJECT->getUCS(targetUcsId)->getShapes();
		for (std::list<Shape*>::iterator i = PastedShapes.begin(); i != PastedShapes.end(); i++)
			rshapes.removeItem((*i), false);
		ActionStatus(false);
		MAINDllOBJECT->getShapesList().notifySelection();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0007", __FILE__, __FUNCSIG__); }
}

void CopyShapesAction::setCopiedShape(std::list<Shape*> shapescoll, int sourceId, int targetId)
{
	try
	{
		CopyShapesAction* action = new CopyShapesAction();
		action->sourceUcsId = sourceId;
		action->targetUcsId = targetId;
		action->AddshapesColl(shapescoll);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0005", __FILE__, __FUNCSIG__); }
}

void CopyShapesAction::AddshapesColl(std::list<Shape*> shapescoll)
{
	try
	{
		for (std::list<Shape*>::iterator It = shapescoll.begin(); It != shapescoll.end(); It++)
		{
			if((*It)->ShapeAs3DShape() || MAINDllOBJECT->getUCS(targetUcsId)->UCSMode() != 4)
		    	copiedshapes.push_back(*It);
		}
		if(copiedshapes.size() > 0)
		{
			this->CurrentActionType = RapidEnums::ACTIONTYPE::COPYSHAPEACTION;
			MAINDllOBJECT->addAction(this);
		}
		else
			delete this;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTCSH0006", __FILE__, __FUNCSIG__); }
}

void CopyShapesAction::CopyShapesDuringPartProgram()
{
	UCS *sourceUcs = MAINDllOBJECT->getUCS(sourceUcsId);
	UCS *targetUcs = MAINDllOBJECT->getUCS(targetUcsId);
    for(list<int>::iterator it = PastedShapesId.begin(); it != PastedShapesId.end(); it++)
	{
	    if(targetUcs->getShapes().ItemExists(*it))
	    {
			Shape *pastedShape = (Shape *)targetUcs->getShapes().getList()[*it];
			Shape *copiedShape = (Shape *)(pastedShape->getMCSParentShape());
		    pastedShape->CopyOriginalProperties(copiedShape);
		}
	}
}
 

wostream& operator<<(wostream& os, CopyShapesAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;

		os << "CopyShapesAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "sourceUcsId:" << action.sourceUcsId << endl;
		os << "targetUcsId:" << action.targetUcsId << endl;
		os << "ActionPastedShapesIdsize:" << action.PastedShapesId.size() << endl;
		for(list<int>::iterator It =  action.PastedShapesId.begin(); It !=  action.PastedShapesId.end(); It++)
			os << "ActionPastedShapesId:" << *It << endl;
		os << "EndCopyShapesAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0007", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, CopyShapesAction& action)
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
			if(Tagname==L"CopyShapesAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::COPYSHAPEACTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndCopyShapesAction")
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
						else if(Tagname==L"sourceUcsId")
						{
							action.sourceUcsId=atoi((const char*)(Val).c_str());
						}						
						else if(Tagname==L"targetUcsId")
						{
							action.targetUcsId=atoi((const char*)(Val).c_str());
						}
						else if(Tagname==L"ActionPastedShapesIdsize")
						{							
						}
						else if(Tagname==L"ActionPastedShapesId")
						{	
							action.PastedShapesId.push_back(atoi((const char*)(Val).c_str()));
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
void ReadOldPP(wistream& is, CopyShapesAction& action)
{
	try
	{
		wstring name; int n, k, Ucsid;
		UCS* ucs;
		action.CurrentActionType = RapidEnums::ACTIONTYPE::COPYSHAPEACTION;
		action.ActionStatus(true);
		is >> n; action.setId(n);
		is >> name; action.setOriginalName(name);
		is >> n; action.sourceUcsId = n;
		is >> n; action.targetUcsId = n;
		is >> n; 
		for(int i = 0; i < n; i++)
		{
			is >> k;
			action.PastedShapesId.push_back(k);
		}
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0008", __FILE__, __FUNCSIG__); }
}
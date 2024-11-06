#include "StdAfx.h"
#include "AddDimAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\IsectPt.h"
#include "..\Measurement\DimPointCoordinate.h"

AddDimAction::AddDimAction():RAction(_T("AddDim"))
{	
	this->CurrentActionType = RapidEnums::ACTIONTYPE::ADDDIMACTION;
	this->PPStatusAction = false;
}

AddDimAction::~AddDimAction()
{
	this->dim->CurrentDimAddAction = NULL;
}

void AddDimAction::setDim(DimBase* dim)
{
	this->dim = dim;
}

bool AddDimAction::execute()
{
	try
	{
		if(!ActionStatus())
		{
			switch(dim->MeasurementType)
			{
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCEONAXIS:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCE3D:
					if(!dim->ChildMeasurementType())
					{
						dim->ParentPoint1 = MAINDllOBJECT->getVectorPointer(dim->ParentPoint1);
						dim->ParentPoint2 = MAINDllOBJECT->getVectorPointer(dim->ParentPoint2);
						if(dim->ParentPoint1->IntersectionPoint)
							((IsectPt*)dim->ParentPoint1)->SetRelatedDim(dim);
						if(dim->ParentPoint2->IntersectionPoint)
							((IsectPt*)dim->ParentPoint2)->SetRelatedDim(dim);
					}
					break;
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCONEDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOLINEDISTANCE3D:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPLANEDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCYLINDERDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCONICS3DDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCIRCLE3DDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOLINEDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCIRCLEDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE:
				case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEX:
				case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEY:
				case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEZ:
					if(!dim->ChildMeasurementType())
					{
						dim->ParentPoint1 = MAINDllOBJECT->getVectorPointer(dim->ParentPoint1);
						if(dim->ParentPoint1->IntersectionPoint)
							((IsectPt*)dim->ParentPoint1)->SetRelatedDim(dim);
					}
					break;			
			}
		}
		if(PPStatusAction)
			ActionStatus(false);
		else
			ActionStatus(true);
		MAINDllOBJECT->getDimList().addItem((BaseItem*)dim);
			if(dim->selected())
				MAINDllOBJECT->getSelectedDimList().addItem(dim);
		dim->CurrentDimAddAction = this;
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDIM0001", __FILE__, __FUNCSIG__); return false; }
}

bool AddDimAction::redo()
{
	try
	{
		if(!ActionStatus())
		{
			switch(dim->MeasurementType)
			{
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCEONAXIS:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPOINTDISTANCE3D:
					if(!dim->ChildMeasurementType())
					{
						dim->ParentPoint1 = MAINDllOBJECT->getVectorPointer(dim->ParentPoint1);
						dim->ParentPoint2 = MAINDllOBJECT->getVectorPointer(dim->ParentPoint2);
						if(dim->ParentPoint1->IntersectionPoint)
							((IsectPt*)dim->ParentPoint1)->SetRelatedDim(dim);
						if(dim->ParentPoint2->IntersectionPoint)
							((IsectPt*)dim->ParentPoint2)->SetRelatedDim(dim);
					}
					break;
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCONEDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOLINEDISTANCE3D:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOPLANEDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCYLINDERDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCONICS3DDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCIRCLE3DDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOLINEDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTTOCIRCLEDISTANCE:
				case RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE:
				case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEX:
				case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEY:
				case RapidEnums::MEASUREMENTTYPE::DIM_ORDINATEZ:
					if(!dim->ChildMeasurementType())
					{
						dim->ParentPoint1 = MAINDllOBJECT->getVectorPointer(dim->ParentPoint1);
						if(dim->ParentPoint1->IntersectionPoint)
							((IsectPt*)dim->ParentPoint1)->SetRelatedDim(dim);
					}
					break;
			}
		}
		if(dim->MeasureAsGDnTMeasurement())
		{
			int rfgd = dim->ParentShape2->RefernceDatumForgdnt();
			rfgd++;
			dim->ParentShape2->RefernceDatumForgdnt(rfgd);
			if(dim->ParentShape3 != NULL)
			{
				rfgd = dim->ParentShape3->RefernceDatumForgdnt();
				rfgd++;
				dim->ParentShape3->RefernceDatumForgdnt(rfgd);
			}
		}
		MAINDllOBJECT->getDimList().addItem((BaseItem*)dim);
		if(dim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE)
		{
			DimPointCoordinate* CdimPt = (DimPointCoordinate*)dim;
			for (list<BaseItem*>::iterator childDimIter = CdimPt->getMchild().begin(); childDimIter !=  CdimPt->getMchild().end(); childDimIter++)
			{
				DimBase * dimCur = (DimBase*)(*childDimIter);
				MAINDllOBJECT->getDimList().addItem((BaseItem*)dimCur, false);
			}
		}
		if(dim->selected())
		{
			MAINDllOBJECT->getSelectedDimList().addItem(dim);
			MAINDllOBJECT->getDimList().notifySelection();
		}
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDIM0002", __FILE__, __FUNCSIG__); return false; }
}

void AddDimAction::undo()
{
	try
	{
		if(dim->MeasureAsGDnTMeasurement())
		{
			int rfgd = dim->ParentShape2->RefernceDatumForgdnt();
			if(rfgd != 0)rfgd--;
			dim->ParentShape2->RefernceDatumForgdnt(rfgd);
			if(dim->ParentShape3 != NULL)
			{
				rfgd = dim->ParentShape3->RefernceDatumForgdnt();
				if(rfgd != 0)rfgd--;
				dim->ParentShape3->RefernceDatumForgdnt(rfgd);
			}
		}	
		MAINDllOBJECT->getDimList().removeItem((BaseItem*)dim, false);
		if(dim->MeasurementType == RapidEnums::MEASUREMENTTYPE::DIM_POINTCOORDINATE)
		{
			DimPointCoordinate* CdimPt = (DimPointCoordinate*)dim;
			for each(int Mid in CdimPt->PChildMeasureIdCollection)
			{
				if(MAINDllOBJECT->getDimList().ItemExists(Mid))
				{
					DimBase* dimCur = (DimBase*)MAINDllOBJECT->getDimList().getList()[Mid];
					MAINDllOBJECT->getDimList().removeItem((BaseItem*)dimCur, false, false);
				}
			}
		}
		ActionStatus(false);
	}
	catch(...){ ActionStatus(false); MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDIM0003", __FILE__, __FUNCSIG__); }
}

void AddDimAction::addDim(DimBase* dim, bool DisableAction)
{
	try
	{
		AddDimAction* action = new AddDimAction();
		action->setDim(dim);
		action->PPStatusAction = DisableAction;
		action->CurrentActionType = RapidEnums::ACTIONTYPE::ADDDIMACTION;
		MAINDllOBJECT->addAction(action);
		MAINDllOBJECT->selectMeasurement(dim->getId(), false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDIM0004", __FILE__, __FUNCSIG__); }
}

DimBase* AddDimAction::getDim()
{
	return dim;
}

wostream& operator<<(wostream& os, AddDimAction& action)
{
	try
	{
		os << "CurrentActionType:" << action.CurrentActionType << endl;

		os << "AddDimAction" << endl ;		
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "ActionDimId:" << action.getDim()->getId() << endl;
		os << "EndAddDimAction" << endl ;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDIM0005", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, AddDimAction& action)
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
			if(Tagname==L"DimAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::ADDDIMACTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndDimAction")
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
						else if(Tagname==L"ActionDimId")
						{
							int b = atoi((const char*)(Val).c_str());
							if(MAINDllOBJECT->getDimList().ItemExists(b))
							  action.dim = (DimBase*)(MAINDllOBJECT->getDimList().getList()[b]);
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDIM0006", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, AddDimAction& action)
{
	try
	{
		wstring name; int n;
		action.CurrentActionType = RapidEnums::ACTIONTYPE::ADDDIMACTION;
		action.ActionStatus(true);
		is >> n; action.setId(n);
		is >> name; action.setOriginalName(name);
		is >> n; 
		if(MAINDllOBJECT->getDimList().ItemExists(n))
		  action.dim = (DimBase*)(MAINDllOBJECT->getDimList().getList()[n]);		
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("ACTDIM0006", __FILE__, __FUNCSIG__); }
}

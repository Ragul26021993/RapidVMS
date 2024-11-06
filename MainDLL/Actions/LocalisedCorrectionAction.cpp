#include "StdAfx.h"
#include "LocalisedCorrectionAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\PartProgramFunctions.h"

LocalisedCorrectionAction::LocalisedCorrectionAction(double *curPos, int _side):RAction(_T("Correction"))
{
	DROPosition.set(curPos[0], curPos[1], curPos[2]);
	side = _side;
	this->CurrentActionType = RapidEnums::ACTIONTYPE::LOCALISEDCORRECTION;
}

LocalisedCorrectionAction::LocalisedCorrectionAction():RAction(_T("Correction"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::LOCALISEDCORRECTION;
}

LocalisedCorrectionAction::~LocalisedCorrectionAction()
{	
}

bool LocalisedCorrectionAction::execute()
{
	try
	{
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0002", __FILE__, __FUNCSIG__); return false; }
}

bool LocalisedCorrectionAction::redo()
{
	try
	{
		RCollectionBase& rshapes = MAINDllOBJECT->getShapesList();
		rshapes.addItem(shape1);
		if(shape1->selected())
			MAINDllOBJECT->getSelectedShapesList().addItem(shape1);
		rshapes.addItem(shape2);
		if(shape2->selected())
			MAINDllOBJECT->getSelectedShapesList().addItem(shape2);
	
		if(!measurement->ChildMeasurementType())
		{
			measurement->ParentPoint1 = MAINDllOBJECT->getVectorPointer(measurement->ParentPoint1);
			if(measurement->ParentPoint1->IntersectionPoint)
				((IsectPt*)measurement->ParentPoint1)->SetRelatedDim(measurement);
		}
		RCollectionBase& rmeasure = MAINDllOBJECT->getDimList();
		rmeasure.addItem(measurement);
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0003", __FILE__, __FUNCSIG__); return false; }
}

void LocalisedCorrectionAction::undo()
{
	try
	{
		RCollectionBase& rmeasure = MAINDllOBJECT->getDimList();
		rmeasure.removeItem(measurement);
		RCollectionBase& rshapes = MAINDllOBJECT->getShapesList();
		rshapes.removeItem(shape2);
		rshapes.removeItem(shape1);
		ActionStatus(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0004", __FILE__, __FUNCSIG__); }
}
 
void LocalisedCorrectionAction::AddLocalisedCorrectionAction(ShapeWithList* _shape1, ShapeWithList* _shape2, DimBase* _measurement)
{
	try
	{
		shape1 = _shape1;
		shape2 = _shape2;
		measurement = _measurement;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0006", __FILE__, __FUNCSIG__); }
}

void LocalisedCorrectionAction::SetLocalisedCorrectionProperties(int _measureCnt, int _rowCnt, int _colCnt)
{
	try
	{
		MeasureCnt = _measureCnt;
		RowCnt = _rowCnt;
		ColCnt = _colCnt;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0006", __FILE__, __FUNCSIG__); }
}

wostream& operator<<(wostream& os, LocalisedCorrectionAction &action)
{
	try
	{
		Vector DROPosition;

		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;
		os << "LocalisedCorrectionAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "DROPosition:values" << endl << (static_cast<Vector>(action.DROPosition)) << endl;	
		os << "MeasureId:" << action.measurement->getId() << endl;
		os << "ActionShape1Id:" << action.shape1->getId() << endl;
		os << "ActionShape2Id:" << action.shape2->getId() << endl;
		os << "Side:" << action.side << endl;
		os << "MeasureCnt:" << action.MeasureCnt << endl;
		os << "RowCnt:" << action.RowCnt << endl;
		os << "ColCnt:" << action.ColCnt << endl;	
		os << "EndLocalisedCorrectionAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0007", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, LocalisedCorrectionAction& action)
{
	try
	{
			UCS* ucs = MAINDllOBJECT->getUCS(0);
			int Ucsid;
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"LocalisedCorrectionAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::LOCALISEDCORRECTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndLocalisedCorrectionAction")
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
						else if(Tagname==L"DROPosition")
						{
							is >> (*static_cast<Vector*>(&action.DROPosition));
						}
						else if(Tagname==L"MeasureId")
						{
							int n=atoi((const char*)(Val).c_str()); 
							action.measurement = (DimBase*)(MAINDllOBJECT->getDimList().getList()[n]);
						}
						else if(Tagname==L"ActionShape1Id")
						{
							int n=atoi((const char*)(Val).c_str()); 
							action.shape1 = (ShapeWithList*)(ucs->getShapes().getList()[n]);
						}
						else if(Tagname==L"ActionShape2Id")
						{
							int n=atoi((const char*)(Val).c_str()); 
							action.shape2 = (ShapeWithList*)(ucs->getShapes().getList()[n]);
						}
						else if(Tagname==L"Side")
						{
							action.side = atoi((const char*)(Val).c_str());
						}
							else if(Tagname==L"MeasureCnt")
						{
							action.MeasureCnt = atoi((const char*)(Val).c_str());
						}
						else if(Tagname==L"RowCnt")
						{
						    action.RowCnt = atoi((const char*)(Val).c_str());
						}
						else if(Tagname==L"ColCnt")
						{
							action.ColCnt = atoi((const char*)(Val).c_str());
						}
					}
				}
			}
	  return is;
	}
	catch(...)
	{
		MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("ACTPOINT0008", __FILE__, __FUNCSIG__); return is; }
}
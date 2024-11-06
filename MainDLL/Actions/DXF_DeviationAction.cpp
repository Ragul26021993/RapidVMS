#include "StdAfx.h"
#include "DXF_DeviationAction.h"
#include "..\Engine\RCadApp.h"

DXF_DeviationAction::DXF_DeviationAction():RAction(_T("DXF_Deviation"))
{
	this->UcsId = 0;
	this->CurrentActionType = RapidEnums::ACTIONTYPE::DXF_DEVIATIONACTION;
	this->UpperCutOffMeasurement = 0;
}

DXF_DeviationAction::~DXF_DeviationAction()
{
}

bool DXF_DeviationAction::execute()
{
	try
	{
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevAct0001", __FILE__, __FUNCSIG__); return false; }
}
 
bool DXF_DeviationAction::redo()
{
	try
	{
		RCollectionBase& rshapes = MAINDllOBJECT->getShapesList();
		RCollectionBase& rMeasure = MAINDllOBJECT->getDimList();
		for (list<Shape*>::iterator i = DeviationShape.begin(); i != DeviationShape.end(); i++)
		{
			BaseItem* item = (*i);
			rshapes.addItem(item);
			if(item->selected())
				MAINDllOBJECT->getSelectedShapesList().addItem(item, false);
		}
		for (list<DimBase*>::iterator i = DeviationMeasure.begin(); i != DeviationMeasure.end(); i++)
		{
			BaseItem* item = (*i);
			rMeasure.addItem(item);
			if(item->selected())
				MAINDllOBJECT->getSelectedDimList().addItem(item, false);
		}
		MAINDllOBJECT->getShapesList().notifySelection();
		MAINDllOBJECT->getDimList().notifySelection();
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevAct0002", __FILE__, __FUNCSIG__); return false; }
}

void DXF_DeviationAction::undo()
{
	try
	{				
		RCollectionBase& rshapes = MAINDllOBJECT->getShapesList();
		RCollectionBase& rMeasure = MAINDllOBJECT->getDimList();
		for (std::list<Shape*>::iterator i = DeviationShape.begin(); i != DeviationShape.end(); i++)
			rshapes.removeItem((*i), false);
		for (std::list<DimBase*>::iterator i = DeviationMeasure.begin(); i != DeviationMeasure.end(); i++)
			rMeasure.removeItem((*i), false);
		ActionStatus(false);
		MAINDllOBJECT->getShapesList().notifySelection();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevAct0003", __FILE__, __FUNCSIG__); }
}

void DXF_DeviationAction::SetDXF_DeviationParam(double UpperCutoff, double tolerance, double interval, int IntervalType, bool ClosedLoop, bool TwoD_Deviation, std::list<int>* SelectedShapeIdList, std::list<Shape*> CreatedShapeList, std::list<DimBase*> CreatedMeasureList)
{
	try
	{		
		DXF_DeviationAction* CptAct = new DXF_DeviationAction();
		CptAct->TwoD_Deviation = TwoD_Deviation;
		CptAct->ClosedLoop = ClosedLoop;
		CptAct->IntervalType = IntervalType;
		CptAct->tolerance = tolerance;
		CptAct->interval = interval;
		CptAct->UpperCutOffMeasurement = UpperCutoff;
		CptAct->UcsId = MAINDllOBJECT->getCurrentUCS().getId();
		for each(int id in *SelectedShapeIdList)
			CptAct->IDlist.push_back(id);
		for each(Shape* Shp in CreatedShapeList)
			CptAct->DeviationShape.push_back(Shp);
		for each(DimBase* Measure in CreatedMeasureList)
			CptAct->DeviationMeasure.push_back(Measure);
		MAINDllOBJECT->addAction(CptAct);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevAct0004", __FILE__, __FUNCSIG__); }
}

void DXF_DeviationAction::GetDXF_DeviationParam(std::list<int>* SelectedShapeIdList, std::list<int>* LineIdList, double* UpperCutoff, double* tolerance, double* interval, int* IntervalType, bool* ClosedLoop,  bool* TwoD_Deviation)
{
	try
	{				
		for each(int id in IDlist)
			SelectedShapeIdList->push_back(id);
		for each(Shape* Shp in DeviationShape)
			LineIdList->push_back(Shp->getId());
		*UpperCutoff = this->UpperCutOffMeasurement;
		*tolerance = this->tolerance;
		*ClosedLoop = this->ClosedLoop;
		*TwoD_Deviation = this->TwoD_Deviation;
		*IntervalType = this->IntervalType;
		*interval = this->interval;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevAct0005", __FILE__, __FUNCSIG__); }
}

void DXF_DeviationAction::SetTolerance(double tolerance)
{
	try
	{				
		this->tolerance = tolerance;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevAct0006", __FILE__, __FUNCSIG__); }
}

int DXF_DeviationAction::getUcsId()
{
	return this->UcsId;
}

wostream& operator<<(wostream& os, DXF_DeviationAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;
		os << "DXF_DeviationAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "UcsId:" << action.UcsId << endl;
		os << "TwoDdeviation:" << action.TwoD_Deviation << endl;
		os << "ClosedLoop:" << action.ClosedLoop << endl;
		os << "IntervalType:" << action.IntervalType << endl;
		os << "UpperCutOffMeasurement:" << action.UpperCutOffMeasurement << endl;
		os << "tolerance:" << action.tolerance << endl;
		os << "interval:" << action.interval << endl;
		os << "DeviationShapeSize:" << action.DeviationShape.size() << endl;
		for each(Shape* Shp in action.DeviationShape)
			os << Shp->getId() << endl;
		os << "DeviationMeasureSize:" << action.DeviationMeasure.size() << endl;
		for each(DimBase* Measure in action.DeviationMeasure)
			os << Measure->getId() << endl;
		os << "IDlistSize:" << action.IDlist.size() << endl;
		for each(int id in action.IDlist)
			os << id << endl;
		os << "EndDXF_DeviationAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevAct0007", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, DXF_DeviationAction& action)
{
	try
	{
		UCS* ucs;
		std::wstring Tagname;
		is >> Tagname;
		if(Tagname==L"DXF_DeviationAction")
		{
			action.CurrentActionType = RapidEnums::ACTIONTYPE::DXF_DEVIATIONACTION;
			action.ActionStatus(true);
			while(Tagname!=L"EndDXF_DeviationAction")
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
					else if(Tagname==L"TwoDdeviation")
					{
						if(Val=="0")
							action.TwoD_Deviation = false;						
						else
							action.TwoD_Deviation = true;
					}
					else if(Tagname==L"ClosedLoop")
					{
						if(Val=="0")
							action.ClosedLoop = false;						
						else
							action.ClosedLoop = true;
					}
					else if(Tagname==L"IntervalType")
					{
						action.IntervalType = atoi((const char*)(Val).c_str());								
					}
					else if(Tagname==L"tolerance")
					{
						action.tolerance = atof((const char*)(Val).c_str());
					}
					else if(Tagname==L"UpperCutOffMeasurement")
					{
						action.UpperCutOffMeasurement = atof((const char*)(Val).c_str());
					}
					else if(Tagname==L"interval")
					{
						action.interval = atof((const char*)(Val).c_str());
					}
					else if(Tagname==L"UcsId")
					{
						action.UcsId = atoi((const char*)(Val).c_str());
						ucs = MAINDllOBJECT->getUCS(action.UcsId);
					}
					else if(Tagname==L"DeviationShapeSize")
					{
						int n = 0;
						int Count = atoi((const char*)(Val).c_str()); 
						for(int i = 0; i < Count; i++)
						{
							is >> n;
							Shape* Shp = (Shape*)(ucs->getShapes().getList()[n]);
							action.DeviationShape.push_back(Shp);
						}
					}					
					else if(Tagname==L"DeviationMeasureSize")
					{
						int n = 0;
						int Count = atoi((const char*)(Val).c_str()); 
						for(int i = 0; i < Count; i++)
						{
							is >> n;
							DimBase* Measure = (DimBase*)(ucs->getDimensions().getList()[n]);
							action.DeviationMeasure.push_back(Measure);
						}
					}
					else if(Tagname==L"IDlistSize")
					{
						int n = 0;
						int Count = atoi((const char*)(Val).c_str()); 
						for(int i = 0; i < Count; i++)
						{
							is >> n;
							action.IDlist.push_back(n);
						}
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DXFDevAct0008", __FILE__, __FUNCSIG__); return is; }
}		
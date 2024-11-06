#include "StdAfx.h"
#include "DeviationAction.h"
#include "..\Engine\RCadApp.h"

DeviationAction::DeviationAction():RAction(_T("Deviation"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::DEVIATIONACTION;
}

DeviationAction::~DeviationAction()
{
}

bool DeviationAction::execute()
{
	try
	{
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DevAct0001", __FILE__, __FUNCSIG__); return false; }
}
 
bool DeviationAction::redo()
{
	try
	{
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DevAct0002", __FILE__, __FUNCSIG__); return false; }
}

void DeviationAction::undo()
{
	try
	{				
		ActionStatus(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DevAct0003", __FILE__, __FUNCSIG__); }
}

void DeviationAction::SetDeviationParam(Vector& pt, std::list<int>* SelectedShapeIdList, std::list<int>* LineIdList, bool deviationtype, int measureRefrence, bool BestfitArc)
{
	try
	{		
		DeviationAction* CptAct = new DeviationAction();
		CptAct->Dparameter = pt;	
		CptAct->TwoDdeviation = deviationtype;
		CptAct->BestfitArcflag = BestfitArc;
		CptAct->measureRefrenceAxis = measureRefrence;
		for each(int id in *SelectedShapeIdList)
			CptAct->IDlist.push_back(id);
		for each(int id in *LineIdList)
			CptAct->LineIDlist.push_back(id);
		MAINDllOBJECT->addAction(CptAct);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DevAct0004", __FILE__, __FUNCSIG__); }
}

Vector* DeviationAction::GetDeviationParam()
{
	try
	{		
		return &(this->Dparameter);		
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DevAct0005", __FILE__, __FUNCSIG__); }
}

void DeviationAction::getShapeIdList(std::list<int>* ShapeIdList, bool* BestfitFlag, int* measureRefrence)
{
	try
	{				
		for each(int id in IDlist)
			ShapeIdList->push_back(id);
		*BestfitFlag = BestfitArcflag;
		*measureRefrence = measureRefrenceAxis;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0006", __FILE__, __FUNCSIG__); }
}

void DeviationAction::getLineIdList(map<int, int>* LIdList)
{
	try
	{				
		int n = 0;
		for each(int id in LineIDlist)
		{
			(*LIdList)[n] = id;
			n++;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0007", __FILE__, __FUNCSIG__); }
}

bool DeviationAction::getdeviationTypeFlag()
{
	return this->TwoDdeviation;
}

void DeviationAction::SetTolerance(double Uppertolerance, double Lowertolerance)
{
	try
	{				
		Vector param;
		param.set(Uppertolerance, Lowertolerance, Dparameter.getZ(), Dparameter.getR(), Dparameter.getP());
		this->Dparameter.set(param);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("AutoAlign0008", __FILE__, __FUNCSIG__); }
}

wostream& operator<<(wostream& os, DeviationAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;
		os << "DeviationAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "DeviationParam:values" << endl << (*static_cast<Vector*>(action.GetDeviationParam())) << endl;
		os << "TwoDdeviation:" << action.TwoDdeviation << endl;
		os << "BestfitArcflag:" << action.BestfitArcflag << endl;
		os << "measureRefrenceAxis:" << action.measureRefrenceAxis << endl;
		for each(int id in action.IDlist)
			os << "ActionIDlist:" << id << endl;
		for each(int id in action.LineIDlist)
			os << "ActionLineIDlist:" << id << endl;
		os << "EndDeviationAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("DevAct0009", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, DeviationAction& action)
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
			if(Tagname==L"DeviationAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::DEVIATIONACTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndDeviationAction")
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
						else if(Tagname==L"DeviationParam")
						{
							is >> (*static_cast<Vector*>(&action.Dparameter));
						}						
						else if(Tagname==L"TwoDdeviation")
						{
							if(Val=="0")
								action.TwoDdeviation = false;						
							else
								action.TwoDdeviation = true;
						}
						else if(Tagname==L"BestfitArcflag")
						{			
							if(Val=="0")
								action.BestfitArcflag = false;						
							else
								action.BestfitArcflag = true;
						}
						else if(Tagname==L"measureRefrenceAxis")
						{	
							action.measureRefrenceAxis = atoi((const char*)(Val).c_str());
						}
						else if(Tagname==L"ActionIDlist")
						{	
							action.IDlist.push_back(atoi((const char*)(Val).c_str()));						
						}
						else if(Tagname==L"ActionLineIDlist")
						{	
							action.LineIDlist.push_back(atoi((const char*)(Val).c_str()));	
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DevAct0010", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, DeviationAction& action)
{
	try
	{
		wstring name; int n, Idcnt;
		bool flag;
		action.CurrentActionType = RapidEnums::ACTIONTYPE::DEVIATIONACTION;
		action.ActionStatus(true);
		is >> n; action.setId(n);
		is >> name; action.setOriginalName(name);	
		is >> (*static_cast<Vector*>(&action.Dparameter));		
		is >> flag; action.TwoDdeviation = flag;	
		is >> flag; action.BestfitArcflag = flag;	
		is >> n; action.measureRefrenceAxis = n;
		is >> Idcnt;
		for(int i = 0; i < Idcnt; i++)
		{
			is >> n;
			action.IDlist.push_back(n);
		}
		is >> Idcnt;
		for(int i = 0; i < Idcnt; i++)
		{
			is >> n;
			action.LineIDlist.push_back(n);
		}
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("DevAct0010", __FILE__, __FUNCSIG__); }
}
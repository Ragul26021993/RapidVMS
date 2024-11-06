#include "StdAfx.h"
#include "AddThreadMeasureAction.h"
#include "..\Engine\RCadApp.h"
#include "..\Engine\ThreadCalcFunctions.h"
#include "..\Measurement\DimThread.h"
#include "..\Shapes\Shape.h"
#include "AddThreadPointAction.h"

AddThreadMeasureAction::AddThreadMeasureAction():RAction(_T("AddThM"))
{
	this->IncludeMajorMinorDia(true);
	this->ExternalThreadMeasurement(true);
	this->ThreadCount(2);
	this->CurrentActionType = RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION;
	this->PausableAction = false;
	this->TopSurfaceFlat(false);
	this->RootSurfaceFlat(false);
}

AddThreadMeasureAction::~AddThreadMeasureAction()
{	
}

void AddThreadMeasureAction::setDim(DimBase* dim)
{
	this->dim = dim;
}

bool AddThreadMeasureAction::execute()
{
	try
	{
		MAINDllOBJECT->getDimList().addItem((BaseItem*)dim);
		if(dim->selected())
			MAINDllOBJECT->getSelectedDimList().addItem(dim);
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHM0001", __FILE__, __FUNCSIG__); return false;}
}

bool AddThreadMeasureAction::redo()
{
	try
	{
		MAINDllOBJECT->getDimList().addItem((BaseItem*)dim);
		if(dim->selected())
			MAINDllOBJECT->getSelectedDimList().addItem(dim);
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHM0002", __FILE__, __FUNCSIG__); return false;}
}

void AddThreadMeasureAction::undo()
{
	try
	{
		MAINDllOBJECT->getDimList().removeItem((BaseItem*)dim, false);
		ActionStatus(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHM0003", __FILE__, __FUNCSIG__); }
}

void AddThreadMeasureAction::addDim(DimBase* dim)
{
	try
	{
		AddThreadMeasureAction* action = new AddThreadMeasureAction();
		action->setDim(dim);
		((DimThread*)dim)->PointerToAction = action;
		MAINDllOBJECT->PointerToShapeCollection = action;
		action->CurrentActionType = RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION;
		MAINDllOBJECT->addAction(action);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHM0004", __FILE__, __FUNCSIG__); }
}

DimBase* AddThreadMeasureAction::getDim()
{
	return dim;
}

void AddThreadMeasureAction::SelectTheFirstShape()
{
	try
	{
		this->ThreadShapePointer = this->ShapeCollection_ThreadWindow.getList().begin();
		CurrentSelectedThreadShape_Threadwin = (Shape*)(*ThreadShapePointer).second;
		CurrentSelectedThreadShape = (Shape*)this->ShapeCollection_Actions.getList()[CurrentSelectedThreadShape_Threadwin->getId()];
		SelectTheProcessingShape();
		THREADCALCOBJECT->ThreadMeasurementModeStartedFromBegin(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHM0005", __FILE__, __FUNCSIG__); }
}

void AddThreadMeasureAction::IncrementShapePointer()
{
	try
	{
		if(this->ThreadShapePointer != this->ShapeCollection_ThreadWindow.getList().end())
			this->ThreadShapePointer++;
		if(!IncludeMajorMinorDia())
		{
			if(this->ThreadShapePointer != this->ShapeCollection_ThreadWindow.getList().end())
				this->ThreadShapePointer++;
		}
		if(this->ThreadShapePointer != this->ShapeCollection_ThreadWindow.getList().end())
		{
			CurrentSelectedThreadShape_Threadwin = (Shape*)(*ThreadShapePointer).second;
			CurrentSelectedThreadShape = (Shape*)this->ShapeCollection_Actions.getList()[CurrentSelectedThreadShape_Threadwin->getId()];
			SelectTheProcessingShape();
		}
		if(this->ThreadShapePointer == this->ShapeCollection_ThreadWindow.getList().end())
		{
			this->ThreadShapePointer = this->ShapeCollection_ThreadWindow.getList().begin();
			CurrentSelectedThreadShape_Threadwin = (Shape*)(*ThreadShapePointer).second;
			CurrentSelectedThreadShape = (Shape*)this->ShapeCollection_Actions.getList()[CurrentSelectedThreadShape_Threadwin->getId()];
			SelectTheProcessingShape();
			/*if(!THREADCALCOBJECT->ThreadMeasurementModeStartedFromBegin())
			{
				this->ThreadShapePointer = this->ShapeCollection_ThreadWindow.getList().begin();
				CurrentSelectedThreadShape_Threadwin = (Shape*)(*ThreadShapePointer).second;
				CurrentSelectedThreadShape = (Shape*)this->ShapeCollection_Actions.getList()[CurrentSelectedThreadShape_Threadwin->getId()];
				SelectTheProcessingShape();
			}*/
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHM0006", __FILE__, __FUNCSIG__); }
}

void AddThreadMeasureAction::SelecttheCurrentPointer(int id)
{
	try
	{
		this->ThreadShapePointer = this->ShapeCollection_ThreadWindow.getList().find(id);
		if(!THREADCALCOBJECT->ThreadMeasurementModeStarted())
		{
			if(id == 0)
				THREADCALCOBJECT->ThreadMeasurementModeStartedFromBegin(true);
			else
				THREADCALCOBJECT->ThreadMeasurementModeStartedFromBegin(false);
		}
		CurrentSelectedThreadShape_Threadwin = (Shape*)(*ThreadShapePointer).second;
		CurrentSelectedThreadShape = (Shape*)this->ShapeCollection_Actions.getList()[CurrentSelectedThreadShape_Threadwin->getId()];
		SelectTheProcessingShape();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHM0007", __FILE__, __FUNCSIG__); }
}

void AddThreadMeasureAction::SelectTheProcessingShape()
{
	try
	{
		for(RC_ITER i = ShapeCollection_ThreadWindow.getList().begin(); i != ShapeCollection_ThreadWindow.getList().end(); i++)
			(*i).second->selected(false, false);
		for(RC_ITER i = ShapeCollection_Actions.getList().begin(); i != ShapeCollection_Actions.getList().end(); i++)
			(*i).second->selected(false, false);
		CurrentSelectedThreadShape_Threadwin->selected(true, false);
		CurrentSelectedThreadShape->selected(true, false);
		//MAINDllOBJECT->getShapesList().getList()[CurrentSelectedThreadShape->getId()]->selected(true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHM0008", __FILE__, __FUNCSIG__); }
}

void AddThreadMeasureAction::ResetAllActions(bool PartProgramLoad)
{
	try
	{
		if(!PartProgramLoad)
		{
			for(std::map<int, int>::iterator itr = THREADCALCOBJECT->ThreadShape_MainCollection_Relation.begin(); itr != THREADCALCOBJECT->ThreadShape_MainCollection_Relation.end(); itr++)
			{
				int ShapeId = (*itr).second;
				if(MAINDllOBJECT->getShapesList().ItemExists(ShapeId)) 
					MAINDllOBJECT->getShapesList().removeItem(MAINDllOBJECT->getShapesList().getList()[ShapeId], true);
			}
		}
		ShapeCollection_ThreadWindow.deleteAll();
		ShapeCollection_Actions.deleteAll();
		CurrentSelectedThreadShape_Threadwin = NULL;
		CurrentSelectedThreadShape = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHM0009", __FILE__, __FUNCSIG__); }
}

RAction* AddThreadMeasureAction::getCurrentThAction()
{
	try
	{
		if(ThreadppCurrentaction == ActionCollection.getList().end())
			return NULL;
		else 
			return (RAction*)(*ThreadppCurrentaction).second;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHM0010", __FILE__, __FUNCSIG__); return NULL; }
}

wostream& operator<<(wostream& os, AddThreadMeasureAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;

		os << "AddThreadMeasureAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "PausableAction:" << action.PausableAction << endl; 		
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "ThreadCount:" << action.ThreadCount() << endl;
		os << "IncludeMajorMinorDia:" << action.IncludeMajorMinorDia() << endl;
		os << "ExternalThreadMeasurement:" << action.ExternalThreadMeasurement() << endl;
		os << "TopSurfaceFlat:" << action.TopSurfaceFlat() << endl;
		os << "RootSurfaceFlat:" << action.RootSurfaceFlat() << endl;
		os << "NumberOfShapes:" << THREADCALCOBJECT->ThreadShape_MainCollection_Relation.size() << endl;
		for(std::map<int, int>::iterator itr = THREADCALCOBJECT->ThreadShape_MainCollection_Relation.begin(); itr != THREADCALCOBJECT->ThreadShape_MainCollection_Relation.end(); itr++)
			os << (*itr).second << endl;
		os << "ActionDimId:" << action.dim->getId() << endl;		
		os << L"StartThreadMeasure:Values" << endl;
		for(RC_ITER Citem1 = action.ActionCollection.getList().begin(); Citem1 != action.ActionCollection.getList().end(); Citem1++)
		{
			RAction* MyAction = (RAction*)((*Citem1).second);
			switch(MyAction->CurrentActionType)
			{
			case RapidEnums::ACTIONTYPE::ADDTHREADPOINTACTION:
				os << (*(AddThreadPointAction*)MyAction);
				os << endl;
				break;
			}
		}
		os << L"EndThreadMeasure" << endl;
		os << "EndAddThreadMeasureAction" << endl;		
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHM0011", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, AddThreadMeasureAction& action)
{
	try
	{
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,action);
		}	
		else
		{
			int n;
			bool enteringFlag = false;
			std::wstring Tagname,buffer;
			is >> Tagname;
			if(Tagname==L"AddThreadMeasureAction")
			{
				action.CurrentActionType = RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION;
				action.ActionStatus(true);
				while(Tagname!=L"EndAddThreadMeasureAction")
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
						else if(Tagname==L"ThreadCount")
						{
							action.ThreadCount(atoi((const char*)(Val).c_str()));
						}
						else if(Tagname==L"IncludeMajorMinorDia")
						{
							if(Val=="0")
								action.IncludeMajorMinorDia(false);
							else
								action.IncludeMajorMinorDia(true);
						}
						else if(Tagname==L"ExternalThreadMeasurement")
						{
							if(Val=="0")
								action.ExternalThreadMeasurement(false);
							else
								action.ExternalThreadMeasurement(true);
						}
						else if(Tagname==L"RootSurfaceFlat")
						{
							if(Val=="0")
								action.RootSurfaceFlat(false);
							else
								action.RootSurfaceFlat(true);
						}
						else if(Tagname==L"TopSurfaceFlat")
						{
							if(Val=="0")
								action.TopSurfaceFlat(false);
							else
								action.TopSurfaceFlat(true);
						}
						else if(Tagname==L"NumberOfShapes")
						{
							THREADCALCOBJECT->ThreadShape_MainCollection_Relation.clear();
							int ShapeCnt = atoi((const char*)(Val).c_str());
							for(int i = 0; i < ShapeCnt; i++)
								is >> THREADCALCOBJECT->ThreadShape_MainCollection_Relation[i];
						}
						else if(Tagname==L"ActionDimId")
						{
							int n = atoi((const char*)(Val).c_str());
							if(MAINDllOBJECT->getDimList().ItemExists(n))
							{
								action.dim = (DimBase*)(MAINDllOBJECT->getDimList().getList()[n]);
								((DimThread*)action.dim)->PointerToAction = &action;
							}
							MAINDllOBJECT->PointerToShapeCollection = &action;
							THREADCALCOBJECT->CurrentSelectedThreadAction = &action;
							THREADCALCOBJECT->InitialiseThreadShapes(action.ThreadCount(), action.TopSurfaceFlat(), action.RootSurfaceFlat(), true);
						}
						else if(Tagname==L"StartThreadMeasure")
						{
							is >> buffer;
							while(!is.eof())//buffer != L"EndThreadMeasure")
							{
								if (buffer == L"EndThreadMeasure")
									break;

								if(buffer == L"Action")
								{
									std::wstring SubLinestr,SubTagname;
									is >> SubLinestr;
									int SubColonIndx = SubLinestr.find(':');
									if(SubColonIndx==-1)
									{
										SubTagname=SubLinestr;
									}
									else
									{
										std::wstring SubTag = SubLinestr.substr(0, SubColonIndx);
										std::wstring SubTagVal = SubLinestr.substr(SubColonIndx + 1, SubLinestr.length() - SubColonIndx - 1);
										std::string SubVal(SubTagVal.begin(), SubTagVal.end() );
										SubTagname=SubTag;	
										buffer = SubTagname;
										//is >> n;
										//buffer = n;
										n=atoi((const char*)(SubVal).c_str());
										if(SubTagname == L"CurrentActionType")
										{
											RapidEnums::ACTIONTYPE temp = RapidEnums::ACTIONTYPE(n);
											RAction *item;
											enteringFlag = false;
											switch(temp)
											{
											case RapidEnums::ACTIONTYPE::ADDTHREADPOINTACTION:
												enteringFlag = true;
												item = new AddThreadPointAction();
												is >> (*static_cast<AddThreadPointAction*>(item));
												break;
											}
											if(enteringFlag)
											{
												action.ActionCollection.addItem(item, false);
											}
										}
									}
								}
								is >> buffer;
							}
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
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHM0012", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, AddThreadMeasureAction& action)
{
	try
	{
		std::wstring flagStr, buffer;
		wstring name;
		int n; bool flag;
		bool enteringFlag = false;
		action.CurrentActionType = RapidEnums::ACTIONTYPE::ADDTHREADMEASUREACTION;
		action.ActionStatus(true);
		is >> n; action.setId(n);
		is >> flagStr;
		std::string Str = RMATH2DOBJECT->WStringToString(flagStr);
		if(Str == "PausableAction")
		{
			is >> action.PausableAction;
			is >> name;
			action.setOriginalName(name);
		}
		else
		{
	    	action.setOriginalName(flagStr);
		}
		is >> n; action.ThreadCount(n);
		is >> flag; action.IncludeMajorMinorDia(flag);
		is >> flag; action.ExternalThreadMeasurement(flag);
		is >> n;
		if(MAINDllOBJECT->getDimList().ItemExists(n))
		{
			action.dim = (DimBase*)(MAINDllOBJECT->getDimList().getList()[n]);
			((DimThread*)action.dim)->PointerToAction = &action;
		}
		MAINDllOBJECT->PointerToShapeCollection = &action;
		THREADCALCOBJECT->CurrentSelectedThreadAction = &action;
		THREADCALCOBJECT->InitialiseThreadShapes(action.ThreadCount(), action.TopSurfaceFlat(), action.RootSurfaceFlat());
		is >> buffer;
		while(!is.eof())//buffer != L"EndThreadMeasure")
		{
			if (buffer == L"EndThreadMeasure")
				break;

			if(buffer == L"Action")
			{
				is >> n;
				buffer = n;
				RapidEnums::ACTIONTYPE temp = RapidEnums::ACTIONTYPE(n);
				RAction *item;
				enteringFlag = false;
				switch(temp)
				{
				case RapidEnums::ACTIONTYPE::ADDTHREADPOINTACTION:
					enteringFlag = true;
					item = new AddThreadPointAction();
					is >> (*static_cast<AddThreadPointAction*>(item));
					break;
				}
				if(enteringFlag)
				{
					action.ActionCollection.addItem(item, false);
				}
			}
			is >> buffer;
		}
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("ACTTHM0012", __FILE__, __FUNCSIG__); }
}
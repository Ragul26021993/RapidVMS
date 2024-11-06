#include "StdAfx.h"
#include "CloudComparisionAction.h"
#include "..\Engine\RCadApp.h"

CloudComparisionAction::CloudComparisionAction():RAction(_T("DXF_Deviation"))
{
	this->CurrentActionType = RapidEnums::ACTIONTYPE::CLOUD_COMPARISION_ACTION;
	ParentShape1 = NULL; ParentShape2 = NULL; ParentShape3 = NULL; Interval = 0; ArcBestFit = true; ClosedLoop = false;
	BestFitPtsCount = 0; MeasurementsCount = 0; SurfaceOnFlag = false;
}

CloudComparisionAction::~CloudComparisionAction()
{
}

bool CloudComparisionAction::execute()
{
	try
	{
		ActionStatus(true);
		return true;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCAct0001", __FILE__, __FUNCSIG__); return false; }
}
 
bool CloudComparisionAction::redo()
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
		for (list<BaseItem*>::iterator i = DeviationMeasure.begin(); i != DeviationMeasure.end(); i++)
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCAct0002", __FILE__, __FUNCSIG__); return false; }
}

void CloudComparisionAction::undo()
{
	try
	{				
		RCollectionBase& rshapes = MAINDllOBJECT->getShapesList();
		RCollectionBase& rMeasure = MAINDllOBJECT->getDimList();
		this->DeviationMeasure.clear();
		for each(Shape* Shp in this->DeviationShape)
		{
			if(Shp->getMchild().size() > 0)
			{
				list<BaseItem*>::iterator childMeasureIter = Shp->getMchild().begin();
				this->DeviationMeasure.push_back(*childMeasureIter);
				rMeasure.removeItem(*childMeasureIter, false);
			}
			rshapes.removeItem(Shp, false);
		}
		ActionStatus(false);
		MAINDllOBJECT->getShapesList().notifySelection();
		MAINDllOBJECT->getDimList().notifySelection();
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCAct0003", __FILE__, __FUNCSIG__); }
}

void CloudComparisionAction::SetCloudCompareParam(double interval, ShapeWithList* PShape1, ShapeWithList* PShape2, bool ArcBestFit, int NumberOfPts, std::list<Shape*> CreatedShapeList)
{
	try
	{		
		CloudComparisionAction* CptAct = new CloudComparisionAction();
		CptAct->ArcBestFit = ArcBestFit;
		CptAct->BestFitPtsCount = NumberOfPts;
		CptAct->Interval = interval;
		CptAct->ParentShape1 = PShape1;
		CptAct->ParentShape2 = PShape2;
		for each(Shape* Shp in CreatedShapeList)
			CptAct->DeviationShape.push_back(Shp);
		MAINDllOBJECT->addAction(CptAct);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCAct0004", __FILE__, __FUNCSIG__); }
}

void CloudComparisionAction::SetCloudCompareParam(bool SurfaceOn, int MeasureCount, ShapeWithList* PShape1, ShapeWithList* PShape2, bool ArcBestFit, int NumberOfPts, std::list<Shape*> CreatedShapeList)
{
	try
	{		
		CloudComparisionAction* CptAct = new CloudComparisionAction();
		CptAct->ArcBestFit = ArcBestFit;
		CptAct->ClosedLoop = true;
		CptAct->MeasurementsCount = MeasureCount;
		CptAct->BestFitPtsCount = NumberOfPts;
		CptAct->ParentShape1 = PShape1;
		CptAct->ParentShape2 = PShape2;
		CptAct->SurfaceOnFlag = SurfaceOn;
		for each(Shape* Shp in CreatedShapeList)
			CptAct->DeviationShape.push_back(Shp);
		MAINDllOBJECT->addAction(CptAct);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCAct0005", __FILE__, __FUNCSIG__); }
}

bool CloudComparisionAction::GetCloudComparisionParam(std::list<int>* ShapeList, double* interval, int* ParentShapeId, int* NumberOfPts)
{
	try
	{		
		for each(Shape* Shp in this->DeviationShape)
			ShapeList->push_back(Shp->getId());
		*interval = this->Interval;
		*NumberOfPts = this->BestFitPtsCount;
		ParentShapeId[0] = this->ParentShape1->getId();
		ParentShapeId[1] = this->ParentShape2->getId();
		return this->ArcBestFit;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCAct0006", __FILE__, __FUNCSIG__); return false;}
}

bool CloudComparisionAction::GetCloudComparisionParam(std::list<int>* ShapeList, int* MeasureCount, int* ParentShapeId, int* NumberOfPts)
{
	try
	{		
		for each(Shape* Shp in this->DeviationShape)
			ShapeList->push_back(Shp->getId());
		*MeasureCount = this->MeasurementsCount;
		*NumberOfPts = this->BestFitPtsCount;
		ParentShapeId[0] = this->ParentShape1->getId();
		ParentShapeId[1] = this->ParentShape2->getId();
		return this->ArcBestFit;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCAct0007", __FILE__, __FUNCSIG__); return false;}
}

bool CloudComparisionAction::GetClosedLoopFlag()
{
	return this->ClosedLoop;
}

bool CloudComparisionAction::GetSurfaceLightFlag()
{
	return this->SurfaceOnFlag;
}

ShapeWithList* CloudComparisionAction::GetThirdParentShape()
{
	return this->ParentShape3;
}

wostream& operator<<(wostream& os, CloudComparisionAction &action)
{
	try
	{
		os << endl << L"Action" << endl;
		os << "CurrentActionType:" << action.CurrentActionType << endl;
		os << "CloudComparisionAction" << endl;
		os << "Id:" << action.getId() << endl;
		os << "OriginalName:" << action.getOriginalName() << endl;
		os << "ArcBestFit:" << action.ArcBestFit << endl;
		os << "ClosedLoop:" << action.ClosedLoop << endl;
		os << "SurfaceOnFlag:" << action.SurfaceOnFlag << endl;
		os << "MeasurementsCount:" << action.MeasurementsCount << endl;
		os << "BestFitPtsCount:" << action.BestFitPtsCount << endl;
		os << "Interval:" << action.Interval << endl;
		os << "UcsId:" << action.ParentShape1->UcsId() << endl;
		os << "ParentShape1:" << action.ParentShape1->getId() << endl;
		os << "ParentShape2:" << action.ParentShape2->getId() << endl;
		if(action.ParentShape3 != NULL)
			os << "ParentShape3:" << action.ParentShape3->getId() << endl;
		os << "ShapeIDlistsize:" << action.DeviationShape.size() << endl;
		for each(Shape* Shp in action.DeviationShape)
			os << Shp->getId() << endl;
		os << "EndCloudComparisionAction" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCAct0008", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>(wistream& is, CloudComparisionAction& action)
{
	try
	{
		int Ucsid = 0;
		UCS* ucs;
		std::wstring Tagname;
		is >> Tagname;
		if(Tagname==L"CloudComparisionAction")
		{
			action.CurrentActionType = RapidEnums::ACTIONTYPE::CLOUD_COMPARISION_ACTION;
			action.ActionStatus(true);
			while(Tagname!=L"EndCloudComparisionAction")
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
					else if(Tagname==L"ArcBestFit")
					{
						if(Val=="0")
							action.ArcBestFit = false;						
						else
							action.ArcBestFit = true;
					}
					else if(Tagname==L"ClosedLoop")
					{
						if(Val=="0")
							action.ClosedLoop = false;						
						else
							action.ClosedLoop = true;
					}
					else if(Tagname==L"SurfaceOnFlag")
					{
						if(Val=="0")
							action.SurfaceOnFlag = false;						
						else
							action.SurfaceOnFlag = true;
					}
					else if(Tagname==L"MeasurementsCount")
					{
						action.MeasurementsCount = atoi((const char*)(Val).c_str());
					}
					else if(Tagname==L"Interval")
					{
						action.Interval = atof((const char*)(Val).c_str());
					}
					else if(Tagname==L"BestFitPtsCount")
					{
						action.BestFitPtsCount = atoi((const char*)(Val).c_str());
					}
					else if(Tagname==L"UcsId")
					{
						Ucsid = atoi((const char*)(Val).c_str());
						ucs = MAINDllOBJECT->getUCS(Ucsid);
					}
					else if(Tagname==L"ParentShape1")
					{
						int n = atoi((const char*)(Val).c_str()); 
						if(ucs->getShapes().ItemExists(n))
						{
							action.ParentShape1 = (ShapeWithList*)(ucs->getShapes().getList()[n]);
							action.ParentShape1->UcsId(Ucsid);
						}
					}
					else if(Tagname==L"ParentShape2")
					{
						int n = atoi((const char*)(Val).c_str()); 
						if(ucs->getShapes().ItemExists(n))
						{
							action.ParentShape2 = (ShapeWithList*)(ucs->getShapes().getList()[n]);
							action.ParentShape2->UcsId(Ucsid);
						}
					}
					else if(Tagname==L"ParentShape3")
					{
						int n = atoi((const char*)(Val).c_str()); 
						if(ucs->getShapes().ItemExists(n))
						{
							action.ParentShape3 = (ShapeWithList*)(ucs->getShapes().getList()[n]);
							action.ParentShape3->UcsId(Ucsid);
						}
					}
					else if(Tagname==L"ShapeIDlistsize")
					{
						int n = 0;
						int Count = atoi((const char*)(Val).c_str()); 
						for(int i = 0; i < Count; i++)
						{
							is >> n;
							if(ucs->getShapes().ItemExists(n))
							{
								Shape* Shp = (Shape*)(ucs->getShapes().getList()[n]);
								action.DeviationShape.push_back(Shp);
							}
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
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCAct0009", __FILE__, __FUNCSIG__); return is; }
}

void CloudComparisionAction::SetCloudCompareParam(double Angle, ShapeWithList* PShape1, ShapeWithList* PShape2, ShapeWithList* PShape3, bool ArcBestFit, int NumberOfPts, std::list<Shape*> CreatedShapeList)
{
	try
	{		
		CloudComparisionAction* CptAct = new CloudComparisionAction();
		CptAct->ArcBestFit = ArcBestFit;
		CptAct->BestFitPtsCount = NumberOfPts;
		CptAct->Interval = Angle;
		CptAct->ParentShape1 = PShape1;
		CptAct->ParentShape2 = PShape2;
		CptAct->ParentShape3 = PShape3;
		for each(Shape* Shp in CreatedShapeList)
			CptAct->DeviationShape.push_back(Shp);
		MAINDllOBJECT->addAction(CptAct);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("CCAct0010", __FILE__, __FUNCSIG__); }
}

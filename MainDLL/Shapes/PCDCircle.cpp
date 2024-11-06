#include "StdAfx.h"
#include "PCDCircle.h"
#include "Sphere.h"
#include "RPoint.h"
#include "Circle.h"
#include "PCDCircle.h"
#include "..\Engine\PointCollection.h"
#include "..\Engine\RCadApp.h"


PCDCircle::PCDCircle(TCHAR* myname , bool is3D):Circle(genName(myname), true,is3D)
{
	Is3D=is3D;
	this->CircleType = RapidEnums::CIRCLETYPE::PCDCIRCLE;
}

PCDCircle::~PCDCircle()
{
	RemovedMeasureColl.deleteAll();
}

PCDCircle::PCDCircle(bool simply):Circle(false, RapidEnums::SHAPETYPE::CIRCLE)
{
	Is3D=false;
	this->CircleType = RapidEnums::CIRCLETYPE::PCDCIRCLE;
}

PCDCircle::PCDCircle(PCDCircle* s):Circle(s)
{
}

TCHAR* PCDCircle::genName(const TCHAR* prefix)
{
	try
	{
		_tcscpy_s(name, 10, prefix);
		TCHAR shapenumstr[10];
		PCDshapenumber++;
		_itot_s(PCDshapenumber, shapenumstr, 10, 10);
		_tcscat_s(name, 10, shapenumstr);
		return name;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCD0001", __FILE__, __FUNCSIG__); return name; }
}

void PCDCircle::AddParentShapes(BaseItem* Csh)
{
	try
	{
		RCollectionBase& mshapes = MAINDllOBJECT->getDimList();
		bool Addrelation = true;
		for each(BaseItem* bi in PCDParentCollection)
		{
			if(bi->getId() == Csh->getId())
			{
				Addrelation = false;
				break;
			}
		}
		if(Addrelation)
		{
			PCDParentCollection.push_back(Csh);
	/*		for each(DimBase* Cdim in ((Shape*)Csh)->getMchild())
			{
				Cdim->IsValid(true);
				RemovedMeasureColl.removeItem(Cdim, false);
				mshapes.addItem(Cdim);
			}*/
		}
		this->addParent(Csh);
		((Shape*)Csh)->addChild(this);
		CalculatePcdCircle();
		this->notifyAll(ITEMSTATUS::DATACHANGED, this);	
		/*for each(DimBase* Cdim in this->getMchild())
			Cdim->UpdateMeasurement();
		if(this->getMchild().size() > 0)
		{
			MAINDllOBJECT->Measurement_updated();
			MAINDllOBJECT->getDimList().notifySelection();
		}*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCD0002", __FILE__, __FUNCSIG__); }
}

void PCDCircle::RemoveParentShapes(BaseItem* Csh)
{
	try
	{
		RCollectionBase& mshapes = MAINDllOBJECT->getDimList();
		PCDParentCollection.remove(Csh);
		//for each(DimBase* Cdim in ((Shape*)Csh)->getMchild())
		//{
		//	Cdim->IsValid(false);
		//	RemovedMeasureColl.addItem(Cdim, false);
		//	mshapes.removeItem(Cdim);
		//}
		this->RemoveParent(Csh);
		((Shape*)Csh)->RemoveChild(this);
		CalculatePcdCircle();
		this->notifyAll(ITEMSTATUS::DATACHANGED, this);	
	/*	for each(DimBase* Cdim in this->getMchild())
			Cdim->UpdateMeasurement();
		if(this->getMchild().size() > 0)
		{
			MAINDllOBJECT->Measurement_updated();
			MAINDllOBJECT->getDimList().notifySelection();
		}*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCD0003", __FILE__, __FUNCSIG__); }
}

void PCDCircle::RemoveSelectedShapes()
{
	try
	{
		MAINDllOBJECT->dontUpdateGraphcis = true;
		list<int> PtIdList;
		for(PC_ITER Item = this->PointsList->getList().begin(); Item != this->PointsList->getList().end(); Item++)
		{
			SinglePoint* Spt = (*Item).second;
			if(Spt->IsSelected)
				PtIdList.push_back(PCDShapePointRelation[Spt->PtID]);
		}
		for each(int Ptid in  PtIdList)
			((PCDCircle*)this)->RemoveParentShapes(MAINDllOBJECT->getShapesList().getList()[Ptid]);
		MAINDllOBJECT->dontUpdateGraphcis = false;
		MAINDllOBJECT->UpdateShapesChanged();
	}
	catch(...){MAINDllOBJECT->dontUpdateGraphcis = false; MAINDllOBJECT->SetAndRaiseErrorMessage("PCD0003", __FILE__, __FUNCSIG__); }
}

void PCDCircle::CalculatePcdCircle()
{
	try
	{
		this->IsValid(false);
		deleteAllPoints();
		this->ResetShapeParameters();
		PCDShapePointRelation.clear();
		if(PCDParentCollection.size() > 1)
		{
			for each(BaseItem* bi in PCDParentCollection)
			{
				Shape* CShape = (Shape*)bi;
				Vector Temp;
				if(CShape->ShapeType == RapidEnums::SHAPETYPE::CIRCLE || CShape->ShapeType == RapidEnums::SHAPETYPE::ARC)
					Temp = *((Circle*)CShape)->getCenter();
				else if(CShape->ShapeType == RapidEnums::SHAPETYPE::SPHERE)
				{
					Temp = *((Sphere*)CShape)->getCenter();
				}
				else
					Temp = *((RPoint*)CShape)->getPosition();
				SinglePoint* Spt1 = new SinglePoint(Temp.getX(), Temp.getY(), Temp.getZ());
				this->PointsList->Addpoint(Spt1);
				this->PointsListOriginal->Addpoint(new SinglePoint(Temp.getX(), Temp.getY(), Temp.getZ()));
				PCDShapePointRelation[Spt1->PtID] = CShape->getId();
			}
			int PointsCnt = this->PointsList->Pointscount();
			if(this->pts != NULL){free(pts); pts = NULL;}	
			if(this->filterpts != NULL){free(filterpts); filterpts = NULL;}	
			if(Is3D)
			{
				if(PointsCnt>2)
				{
				int n = 0, j= 0;
				this->pts = (double*)malloc(sizeof(double) *  PointsCnt * 3);
				this->filterpts = (double*)malloc(sizeof(double) *  PointsCnt * 3);
				filterptsCnt = 0;
		    	for(PC_ITER Spt = this->PointsListOriginal->getList().begin(); Spt != this->PointsListOriginal->getList().end(); Spt++)
					{
						SinglePoint* Pt = (*Spt).second;
						this->pts[n++] = Pt->X;
						this->pts[n++] = Pt->Y;		
						this->pts[n++] = Pt->Z;
						this->filterpts[j++] = Pt->X;
						this->filterpts[j++] = Pt->Y;		
						this->filterpts[j++] = Pt->Z;
						filterptsCnt++;
					}
					double ans[7] = {0};
					if(PointsCnt == 3)RMATH3DOBJECT->Circle_3Pt_3D(pts,&ans[0]);
					else BESTFITOBJECT->ThreeDCircle_BestFit(pts,PointsCnt,&ans[0]);
					this->setCenter(Vector(ans[0],ans[1],ans[2],ans[6]));
					this->dir_l(ans[3]);
					this->dir_m(ans[4]);
					this->dir_n(ans[5]);
					this->Radius(ans[6]);
					this->calculateAttributes();
					this->IsValid(true);
				}
			}
			else
			{
					this->pts = (double*)malloc(sizeof(double) *  PointsCnt * 2);
			    	this->filterpts = (double*)malloc(sizeof(double) *  PointsCnt * 2);
					filterptsCnt = 0;
					int n = 0, j = 0;
					for(PC_ITER Spt = this->PointsListOriginal->getList().begin(); Spt != this->PointsListOriginal->getList().end(); Spt++)
					{
						SinglePoint* Pt = (*Spt).second;
						this->pts[n++] = Pt->X;
						this->pts[n++] = Pt->Y;
						this->filterpts[j++] = Pt->X;
						this->filterpts[j++] = Pt->Y;
						filterptsCnt++;
					}
					double ans[3] = {0};
					if(PointsCnt == 2) RMATH2DOBJECT->Circle_2Pt(pts, pts + 2, &ans[0], &ans[2]);
					else if(PointsCnt == 3) RMATH2DOBJECT->Circle_3Pt(pts, pts + 2, pts + 4, &ans[0], &ans[2]);
					else BESTFITOBJECT->Circle_BestFit(pts, PointsCnt, &ans[0], MAINDllOBJECT->SigmaModeFlag());
					this->setCenter(Vector(ans[0], ans[1], MAINDllOBJECT->getCurrentDRO().getZ()));
					this->Radius(ans[2]); this->OriginalRadius(ans[2]);
					this->calculateAttributes();
					this->IsValid(true);
			}
		}
		else
		{
			this->IsValid(false);
			for each(DimBase* Cdim in this->getMchild())
				Cdim->IsValid(false);
			if(this->getMchild().size() > 0)
				MAINDllOBJECT->Measurement_updated();
			MAINDllOBJECT->Shape_Updated();
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCD0004", __FILE__, __FUNCSIG__); }
}

void PCDCircle::calculateAttributes()
{
}

int PCDCircle::PCDshapenumber = 0;
void PCDCircle::reset()
{
	PCDshapenumber = 0;
}

wostream& operator<<(wostream& os, PCDCircle& x)
{
	try
	{
		os << (*static_cast<Circle*>(&x));
		os<< "PCDCircle" << endl;
		os << "PCDParentCollection:" << x.PCDParentCollection.size() << endl;
		for(list<BaseItem*>::iterator shapeiterator = x.PCDParentCollection.begin(); shapeiterator != x.PCDParentCollection.end(); shapeiterator++)
			os << "shapeiterator:" << ((Shape*)(*shapeiterator))->getId() << endl;
		os<< "EndPCDCircle" << endl;
		return os;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PCD0005", __FILE__, __FUNCSIG__); return os; }
}

wistream& operator>>( wistream& is, PCDCircle& x )
{
	try
	{
		is >> (*(Circle*)&x);
		if(MAINDllOBJECT->IsOldPPFormat())		
		{
			ReadOldPP(is,x);
		}	
		else
		{	
			std::wstring Tagname ;
			is >> Tagname;
			if(Tagname==L"PCDCircle")
			{
			while(Tagname!=L"EndPCDCircle")
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
						std::string Val(TagVal.begin(), TagVal.end());
						Tagname=Tag;					
						if(Tagname==L"PCDParentCollection"){								
						}					
						else if(Tagname==L"shapeiterator")
						{
							int j = atoi((const char*)(Val).c_str());						
							x.PCDParentCollection.push_back(MAINDllOBJECT->getCurrentUCS().getShapes().getList()[j]);
						}
					}
				}
				x.calculateAttributes();
			}
			else
			{
				MAINDllOBJECT->PPLoadSuccessful(false);				
			}
		}
		return is;
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false); MAINDllOBJECT->SetAndRaiseErrorMessage("PCD0006", __FILE__, __FUNCSIG__); return is; }
}
void ReadOldPP(wistream& is, PCDCircle& x)
{
	try
	{
		wstring name;
		int n;
		is >> n;
		for(int i = 0; i < n; i++)
		{
			int j;
			is >> j;
			x.PCDParentCollection.push_back(MAINDllOBJECT->getCurrentUCS().getShapes().getList()[j]);
		}
		x.calculateAttributes();
	}
	catch(...){MAINDllOBJECT->PPLoadSuccessful(false);MAINDllOBJECT->SetAndRaiseErrorMessage("PCD0006", __FILE__, __FUNCSIG__); }
}
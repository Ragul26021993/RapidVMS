#include "StdAfx.h"
#include "PointCollection.h"
#include "RCadApp.h"

PointCollection::PointCollection()
{
	index = 0;
	SelectedPointsCnt = 0;
	allitems.clear();
}

PointCollection::~PointCollection()
{	
	try
	{
		while(allitems.size() != 0)
		{
			PC_ITER i = allitems.begin();
			SinglePoint* Spt = (*i).second;
			allitems.erase(Spt->PtID);
			delete Spt;
		}
	}
	catch(...)
	{
		allitems.clear();
		MAINDllOBJECT->SetAndRaiseErrorMessage("PTCOLL0001", __FILE__, __FUNCSIG__);
	}
}

void PointCollection::Addpoint(SinglePoint* Spt)
{
	try
	{
		Spt->PtID = index;
		allitems[index] = Spt;
		index++;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTCOLL0002", __FILE__, __FUNCSIG__); }
}

void PointCollection::Addpoint(SinglePoint* Spt, int id)
{
	try
	{
		Spt->PtID = id;
		allitems[id] = Spt;
		index++;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTCOLL0002", __FILE__, __FUNCSIG__); }
}

void PointCollection::deletePoint(SinglePoint* Spt)
{
	try
	{
		allitems.erase(Spt->PtID);
		delete Spt;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTCOLL0003", __FILE__, __FUNCSIG__); }
}
void PointCollection::deletePoint(int id)
{
	try
	{
		double z = allitems[id]->Z;
		delete allitems[id];
		allitems.erase(id);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTCOLL0004", __FILE__, __FUNCSIG__); }
}

void PointCollection::ErasePoint(int id)
{
	try{ allitems.erase(id); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTCOLL0005", __FILE__, __FUNCSIG__); }
}

void PointCollection::EraseAll()
{
	index = 0;
	SelectedPointsCnt = 0;
	allitems.clear();
}

void PointCollection::SetPoint(SinglePoint* Spt, int Cindex)
{
	try
	{
		Spt->PtID = Cindex;
		allitems[Cindex] = Spt;	
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTCOLL0006", __FILE__, __FUNCSIG__); }
}
	
void PointCollection::deleteAll()
{
	try
	{
		while (allitems.size() != 0)
		{
			PC_ITER i = allitems.begin();
			SinglePoint* Spt = (*i).second;
			allitems.erase(Spt->PtID);
			delete Spt;
		}
		SelectedPointsCnt = 0;
		index = 0;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTCOLL0007", __FILE__, __FUNCSIG__); }
}

map<int,SinglePoint*>& PointCollection::getList()
{
	return allitems;
}

int PointCollection::Pointscount()
{
	return allitems.size();
}

void PointCollection::CopyAllPoints(PointCollection* OriginalColl)
{
	try
	{
		for(PC_ITER PtItem = OriginalColl->getList().begin(); PtItem != OriginalColl->getList().end(); PtItem++)
		{
			SinglePoint* Spt = PtItem->second;
			this->Addpoint(new SinglePoint(Spt->X, Spt->Y, Spt->Z));
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTCOLL0008", __FILE__, __FUNCSIG__); }
}

void PointCollection::ArrangePointsInOrder(PointCollection::POINTARRANGEMENTTYPE type)
{
	try
	{
		if(this->Pointscount() < 2) return;
		PointCollection TempCollection;
		SinglePoint* fpt = getFirstPoint(type);
		SinglePoint Firstptcopy(fpt->X, fpt->Y, fpt->Z);
		int TotalPtCnt = this->Pointscount() - 1;
		this->deletePoint(fpt->PtID);
		TempCollection.CopyAllPoints(this);
		this->deleteAll();
		this->Addpoint(new SinglePoint(Firstptcopy.X, Firstptcopy.Y, Firstptcopy.Z));

		for(int i = 0; i < TotalPtCnt; i++)
		{
			SinglePoint* CurrentPt = this->getList()[i];
			PC_ITER PtItem2 = TempCollection.getList().begin();
			SinglePoint* NearestPt = PtItem2->second;
			double mindist = RMATH2DOBJECT->Pt2Pt_distance(CurrentPt->X, CurrentPt->Y, NearestPt->X, NearestPt->Y);
			PtItem2++;
			for(;PtItem2 != TempCollection.getList().end(); PtItem2++)
			{
				SinglePoint* TempPt = PtItem2->second;
				double dist = RMATH2DOBJECT->Pt2Pt_distance(CurrentPt->X, CurrentPt->Y, TempPt->X, TempPt->Y);
				if(dist < mindist)
				{
					NearestPt = TempPt;
					mindist = dist;
				}
			}
			this->Addpoint(new SinglePoint(NearestPt->X, NearestPt->Y, NearestPt->Z));
			TempCollection.deletePoint(NearestPt->PtID);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTCOLL0009", __FILE__, __FUNCSIG__); }
}

SinglePoint* PointCollection::getFirstPoint(PointCollection::POINTARRANGEMENTTYPE type)
{
	try
	{
		PC_ITER PtItem = getList().begin();
		if(type == PointCollection::POINTARRANGEMENTTYPE::NORMAL) return PtItem->second;

		SinglePoint* topPoint = PtItem->second;
		PtItem++;
		if(type == PointCollection::POINTARRANGEMENTTYPE::FROMTOP)
		{
			for(;PtItem != getList().end(); PtItem++)
			{
				SinglePoint* Spt = PtItem->second;
				if(Spt->Y > topPoint->Y)
					topPoint = Spt;
			}
		}
		else if(type == PointCollection::POINTARRANGEMENTTYPE::FROMLEFT)
		{
			for(;PtItem != getList().end(); PtItem++)
			{
				SinglePoint* Spt = PtItem->second;
				if(Spt->X < topPoint->X)
					topPoint = Spt;
			}
		}
		else if(type == PointCollection::POINTARRANGEMENTTYPE::FROMRIGHT)
		{
			for(;PtItem != getList().end(); PtItem++)
			{
				SinglePoint* Spt = PtItem->second;
				if(Spt->X > topPoint->X)
					topPoint = Spt;
			}
		}
		else if(type == PointCollection::POINTARRANGEMENTTYPE::FROMBOTTOM)
		{
			for(;PtItem != getList().end(); PtItem++)
			{
				SinglePoint* Spt = PtItem->second;
				if(Spt->Y < topPoint->Y)
					topPoint = Spt;
			}
		}
		return topPoint;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTCOLL0010", __FILE__, __FUNCSIG__); return NULL;}
}

void PointCollection::drawAllpoints(double pointsize, double r, double g, double b)
{
	try
	{
		GRAFIX->SetColor_Double(r, g, b);
		for(PC_ITER PtItem = getList().begin(); PtItem != getList().end(); PtItem++)
		{
			SinglePoint* Spt = PtItem->second;
			GRAFIX->drawPoint(Spt->X, Spt->Y, Spt->Z, pointsize);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("PTCOLL0011", __FILE__, __FUNCSIG__);}
}
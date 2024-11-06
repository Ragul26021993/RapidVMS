#include "StdAfx.h"
#include "IsectPtCollection.h"
#include "RCadApp.h"

IsectPtCollection::IsectPtCollection()
{
}

IsectPtCollection::~IsectPtCollection()
{	
}

void IsectPtCollection::Addpoint(IsectPt* IsnPt)
{
	try{ allitems[IsnPt->IsnPtId] = IsnPt;}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISECTCOLL0001", __FILE__, __FUNCSIG__); }
}

map<int,IsectPt*>& IsectPtCollection::getList()
{
	return allitems;
}

void IsectPtCollection::deletePoint(int id)
{
	try
	{
		delete allitems[id];
		allitems.erase(id);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISECTCOLL0002", __FILE__, __FUNCSIG__); }
}

void IsectPtCollection::ErasePoint(int id)
{
	try{ allitems.erase(id); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISECTCOLL0003", __FILE__, __FUNCSIG__); }
}

void IsectPtCollection::EraseAll()
{
	allitems.clear();
}

void IsectPtCollection::deleteAll()
{
	try
	{
		while (allitems.size() != 0)
		{
			ISN_ITER i = allitems.begin();
			IsectPt* IsnPt = (*i).second;
			allitems.erase(IsnPt->IsnPtId);
			delete IsnPt;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("ISECTCOLL0004", __FILE__, __FUNCSIG__); }
}
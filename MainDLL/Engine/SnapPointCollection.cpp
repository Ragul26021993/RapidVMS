#include "StdAfx.h"
#include "SnapPointCollection.h"
#include "RCadApp.h"

SnapPointCollection::SnapPointCollection()
{
}

SnapPointCollection::~SnapPointCollection()
{	
	try
	{
		while (allitems.size() != 0)
		{
			SPC_ITER i = allitems.begin();
			SnapPoint* Snpt = (*i).second;
			allitems.erase(Snpt->SPtId);
			delete Snpt;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("SNPTCOLL0001", __FILE__, __FUNCSIG__); }
}

void SnapPointCollection::Addpoint(SnapPoint* Snpt, int id)
{
	allitems[id] = Snpt;
}

map<int,SnapPoint*>& SnapPointCollection::getList()
{
	return allitems;
}

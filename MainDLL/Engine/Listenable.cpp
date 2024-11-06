#include "StdAfx.h"
#include "Listenable.h"
#include "..\Engine\RCadApp.h"

Listenable::Listenable()
{
}

Listenable::~Listenable()
{
	listeners.clear();
}

//Add the listeners..//
void Listenable::addListener(CollectionListener *l)
{
	try{ listeners.push_back(l); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LIS0001", __FILE__, __FUNCSIG__); }
}

//Remove the listener from the list...//
void Listenable::removeListener(CollectionListener *l)
{
	try{ listeners.remove(l); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LIS0002", __FILE__, __FUNCSIG__); }
}

//notify the itemadded...//
void Listenable::notifyAdd(BaseItem *item)
{
	try
	{
		for each(CollectionListener* li in listeners)
			li->itemAdded(item, this);
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("LIS0003", __FILE__, __FUNCSIG__); 
	}
}

//notify the item remove.//
void Listenable::notifyRemove(BaseItem *item)
{
	try
	{
		for each(CollectionListener* li in listeners)
			li->itemRemoved(item, this);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LIS0004", __FILE__, __FUNCSIG__); }
}

//notify the item change action...//
void Listenable::notifyChange(BaseItem* item)
{
	try
	{
		for each(CollectionListener* li in listeners)
			li->itemChanged(item, this);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LIS0005", __FILE__, __FUNCSIG__); }
}

//notify the selection changed.....//
void Listenable::notifySelection()
{
	try
	{
		for each(CollectionListener* li in listeners)
			li->selectionChanged(this);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LIS0006", __FILE__, __FUNCSIG__); }
}
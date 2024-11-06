#include "StdAfx.h"
#include "ListenableItem.h"
#include "..\Engine\RCadApp.h"

ListenableItem::ListenableItem()
{
}

ListenableItem::~ListenableItem()
{
	listeners.clear();
}

//Add listeners ..
void ListenableItem::addListener(ItemListener *l)
{
	try{ listeners.push_back(l); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LISITEM0001", __FILE__, __FUNCSIG__); }
}
//Remove listeners.
void ListenableItem::removeListener(ItemListener *l)
{
	try{ listeners.remove(l); }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LISITEM0002", __FILE__, __FUNCSIG__); }
}

//Notify all... Entity Changed : Data Modified/ Selection Changed..!
void ListenableItem::notifyAll(ITEMSTATUS i, BaseItem *item)
{
	try
	{
		for (list<ItemListener*>::iterator iter = listeners.begin(); iter != listeners.end(); iter++)
		{
			(*iter)->EntityParamChanged(i, item);
		}
		/*for (list<ItemListener*>::reverse_iterator iter = listeners.rbegin(); iter != listeners.rend(); iter++)
		{
			(*iter)->EntityParamChanged(i, item);
		}*/
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("LISITEM0003", __FILE__, __FUNCSIG__); }
}
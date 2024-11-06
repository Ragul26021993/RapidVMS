#include "StdAfx.h"
#include "RCollectionBase.h"
#include "RCadApp.h"
//The RCollectionBase class.
//there is another Cpllection inside the collection to hold SelectedItems.
//To avoid creating nested SelectedItems list creation, the flag 'createselectedlist' is used in the constructor

RCollectionBase::RCollectionBase(bool createselectedlist)
{
	try
	{
		ItemId = 0;
		this->hasselectedlist = createselectedlist;
		this->multiselect(false);
		this->selecteditems = NULL;
		this->CurrentSelectedItem = NULL;
		if(createselectedlist) 
			this->selecteditems = new RCollectionBase(false);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCOLL0001", __FILE__, __FUNCSIG__); }
}

RCollectionBase::~RCollectionBase()
{	
	try
	{
		if(!hasselectedlist)
		{
			allitems.erase(allitems.begin(),allitems.end());
			return;
		}
		for(RC_ITER i = allitems.begin(); i != allitems.end(); i++)
			((*i).second)->removeListener(this);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCOLL0002", __FILE__, __FUNCSIG__); }
}

void RCollectionBase::addItem(BaseItem* i, bool notify)
{
	try
	{
		allitems[i->getId()] = i;
		if(notify)
		{
			notifyAdd(i);
			if(hasselectedlist)
				i->addListener(this);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCOLL0003", __FILE__, __FUNCSIG__); }
}

void RCollectionBase::addItemWithoutId(BaseItem* item)
{
	try
	{
		item->setId(ItemId);
		allitems[ItemId] = item;
		ItemId++;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCOLL0004", __FILE__, __FUNCSIG__); }
}

void RCollectionBase::removeItem(BaseItem* i, bool deleteitem, bool notify)
{
	try
	{
		if(this->hasselectedlist && notify)
			getSelected().EraseItem(i, notify);
		allitems.erase(i->getId());
		if(notify)
			notifyRemove(i);
		if(this->hasselectedlist && notify)
			i->removeListener(this);
		if(deleteitem) 
			delete i;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCOLL0005", __FILE__, __FUNCSIG__); }
}

void RCollectionBase::SetSeelectedItem()
{
	try
	{
		if(getSelected().count() > 0)
			CurrentSelectedItem = (*getSelected().getList().begin()).second;
		else 
			CurrentSelectedItem = NULL;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCOLL0005", __FILE__, __FUNCSIG__); }
}

 void RCollectionBase::SelectItemById(int id)
 {
  /*  CurrentSelectedItem = &(*getSelected().getList()[id]);*/
 }

bool RCollectionBase::ItemExists(int itemKey)
{
try
	{
	  return (allitems.find(itemKey) != allitems.end());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCOLL0026", __FILE__, __FUNCSIG__); }
}

void RCollectionBase::clear(bool deleteitem, bool notify)
{
	try
	{
		while (allitems.size() != 0)
		{
			RC_ITER i = allitems.begin();
			this->removeItem((*i).second, deleteitem, notify);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCOLL0006", __FILE__, __FUNCSIG__); }
}

void RCollectionBase::EraseItem(BaseItem* i, bool notify)
{
	try
	{
		if(notify)
			notifyRemove(i);
		allitems.erase(i->getId());
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCOLL0007", __FILE__, __FUNCSIG__); }
}

void RCollectionBase::clearSelection(bool dontnotify)
{
	try
	{
		if(this->hasselectedlist && getSelected().count() !=  0)
		{
			if(this == &MAINDllOBJECT->getCurrentUCS().getShapes())
			{
				//Lock resource..//i.e. the shape collection ..List 
				//WaitForSingleObject(MAINDllOBJECT->ShapeMutex, INFINITE);
				//ReleaseMutex(MAINDllOBJECT->ShapeMutex);
				WaitForSingleObject(MAINDllOBJECT->SelectedShapeMutex, INFINITE);
				//Chandar - Apr 20 2012 - this should be done at end...
				//ReleaseMutex(MAINDllOBJECT->SelectedShapeMutex);
			}
			else if(this == &MAINDllOBJECT->getCurrentUCS().getDimensions())
			{
				//Lock resource..//i.e. the measurement collection ..List 
				//WaitForSingleObject(MAINDllOBJECT->MeasureMutex, INFINITE);
				//ReleaseMutex(MAINDllOBJECT->MeasureMutex);
				WaitForSingleObject(MAINDllOBJECT->SelectedMeasureMutex, INFINITE);
				//Chandar - Apr 20 2012 - this should be done at end...
				//ReleaseMutex(MAINDllOBJECT->SelectedMeasureMutex);
			}
			try
			{
				for(RC_ITER i = getSelected().getList().begin(); i != getSelected().getList().end();	i++)
					(*i).second->selected(false, false);
				getSelected().clearAll();

				if(this == &MAINDllOBJECT->getCurrentUCS().getShapes())
				{
					ReleaseMutex(MAINDllOBJECT->SelectedShapeMutex);
				}
				else if(this == &MAINDllOBJECT->getCurrentUCS().getDimensions())
				{
					ReleaseMutex(MAINDllOBJECT->SelectedMeasureMutex);
				}
				CurrentSelectedItem = NULL;
				if(dontnotify) return;
				notifySelection();
			}
			catch(...)
			{
				if(this == &MAINDllOBJECT->getCurrentUCS().getShapes())
				{
					ReleaseMutex(MAINDllOBJECT->SelectedShapeMutex);
				}
				else if(this == &MAINDllOBJECT->getCurrentUCS().getDimensions())
				{
					ReleaseMutex(MAINDllOBJECT->SelectedMeasureMutex);
				}
				throw "clearselectionexcp";
			}
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCOLL0008", __FILE__, __FUNCSIG__); }
}

void RCollectionBase::clearAll()
{
	try
	{ 
		if(allitems.size() > 0)
			allitems.erase(allitems.begin(),allitems.end()); 
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCOLL0009", __FILE__, __FUNCSIG__); }
}

void RCollectionBase::deleteAll()
{
	try
	{
		while (allitems.size() != 0)
		{
			RC_ITER i = allitems.begin();
			BaseItem* bi = (*i).second;
			allitems.erase(bi->getId());
			delete bi;
		}
	}
	catch(...)
	{
		MAINDllOBJECT->SetAndRaiseErrorMessage("RCOLL0010", __FILE__, __FUNCSIG__); 
	}
	allitems.clear();
}

RCollectionBase& RCollectionBase::getSelected()
{
	return *selecteditems;
}

BaseItem* RCollectionBase::selectedItem()
{
	try{ return CurrentSelectedItem; }
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCOLL0011", __FILE__, __FUNCSIG__); return NULL; }
}

map<int,BaseItem*>& RCollectionBase::getList()
{
	return allitems;
}

int RCollectionBase::count()
{
	return (this->allitems).size();
} 

void RCollectionBase::EntityParamChanged(ITEMSTATUS i, BaseItem* item)
{
	try
	{
		switch(i)
		{
			case SELECTIONCHANGED:
				if(item->selected())
				{
					if(!multiselect())
						clearSelection(true);
					getSelected().addItem(item, false);	
				}
				else
					getSelected().EraseItem(item);
				if(getSelected().count() > 0)
					CurrentSelectedItem = (*getSelected().getList().begin()).second;
				else 
					CurrentSelectedItem = NULL;
				notifySelection();
				break;
			case DATACHANGED:
				this->notifyChange(item);
				break;
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("RCOLL0014", __FILE__, __FUNCSIG__); }
}
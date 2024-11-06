//Main collection base.. Used for all entity(Shape, Measurement, UCS, DXF, Actions etc..) collections..
//Contains Functions to add, remove, erase, delete, deleteall items !!
//Stored in integer index base list.. We can say Hash table!
//Contains a object of same class. for SelectedItems(Collection of selected items)..
#pragma once
#include <map>
#include "BaseItem.h"
#include "Listenable.h"
#include "ItemListener.h"
#include "..\MainDLL.h"
#include "..\Helper\General.h"
#include <exception>
using namespace std;

#define RC_ITER map<int,BaseItem*>::iterator

class MAINDLL_API RCollectionBase:
	public Listenable, public ItemListener
{
private:
	map<int,BaseItem*> allitems;
	map<int,BaseItem*>::iterator ai;
	//Selected Collection...
	RCollectionBase* selecteditems;
	bool hasselectedlist;
	int ItemId;
	BaseItem* CurrentSelectedItem;

public:
	// Creates a Collection for selected items also.
	RCollectionBase(bool createselectedlist = true);
	~RCollectionBase();

	//Add, remove items to/from the collection..
	void addItem(BaseItem* i, bool notify = true);
	void addItemWithoutId(BaseItem* item);
	void removeItem(BaseItem* i, bool deleteitem = false, bool notify = true);
	void SetSeelectedItem();
	void SelectItemById(int id);

	bool ItemExists(int itemKey);
	// Clear, erase, delete the items..
	void clear(bool deleteitem = false, bool notify = true);
	void EraseItem(BaseItem* i, bool notify = false);
	void clearAll();
	void deleteAll();
	// Clear the selected items collection..
	void clearSelection(bool dontnotify = false);	

	// Return items.. list/selected / current selected item etc..
	RCollectionBase& getSelected();	
	BaseItem* selectedItem();
	map<int,BaseItem*>& getList();
	// Item count//
	int count();
	//Listen whenever entity is modified/ Selection Changed..
	virtual void EntityParamChanged(ITEMSTATUS i, BaseItem* item);
	// Multiselect property..Decides whether we need keep last selected item while adding a new item as Selected..
	RapidProperty<bool> multiselect;
};
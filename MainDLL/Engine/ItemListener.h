//Virtual base class.. Entity and collection base are implemented from this..
//Whenever a Item is changed notify the listeners that sender is changed(selection/datachanged)

#pragma once

enum ITEMSTATUS
{
	DATACHANGED, //i.e. the entity param is changed.. Action / Point etc..	
	SELECTIONCHANGED, //Selection Changed.. select/ deselect..
	//If Required we can more status and Handled according to the Type..
};

class BaseItem;

class ItemListener
{
public:
	virtual void EntityParamChanged(ITEMSTATUS, BaseItem* sender) = 0;
};

//Created by Rajavanya.. Modified by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
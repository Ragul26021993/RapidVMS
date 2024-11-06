//Listener for UCS
#pragma once
#include "Engine\CollectionListener.h"
#include "Engine\BaseItem.h"

class RapidUcsListener: 
	public CollectionListener
{
	void UpdateUCSName(BaseItem* item);

public:
	RapidUcsListener();	
	virtual ~RapidUcsListener();
	virtual void itemAdded(BaseItem* item, Listenable* sender);
    virtual void itemRemoved(BaseItem* item, Listenable* sender);
	virtual void itemChanged(BaseItem* item, Listenable* sender);
    virtual void selectionChanged(Listenable* sender);
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
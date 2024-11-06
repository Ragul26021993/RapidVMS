//Listener for Action..
#pragma once
#include "Engine\CollectionListener.h"
#include "Engine\BaseItem.h"

class RapidActionListener: 
	public CollectionListener
{
public:
    RapidActionListener();
    ~RapidActionListener();
    virtual void itemAdded(BaseItem* item, Listenable* sender);
    virtual void itemRemoved(BaseItem* item, Listenable* sender);
    virtual void itemChanged(BaseItem* item, Listenable* sender);
    virtual void selectionChanged(Listenable* sender);
};

//Created by Amit.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
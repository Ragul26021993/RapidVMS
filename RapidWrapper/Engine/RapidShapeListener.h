//Listener for Shape..
#pragma once
#include "Engine\CollectionListener.h"
#include "Engine\BaseItem.h"

class RapidShapeListener: 
	public CollectionListener
{
public:
    RapidShapeListener();
    virtual ~RapidShapeListener();
    virtual void itemAdded(BaseItem* item, Listenable* sender);
    virtual void itemRemoved(BaseItem* item, Listenable* sender);
    virtual void itemChanged(BaseItem* item, Listenable* sender);
    virtual void selectionChanged(Listenable* sender);
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
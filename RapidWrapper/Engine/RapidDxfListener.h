//Listener for DXF
#pragma once
#include "Engine\CollectionListener.h"
#include "Engine\BaseItem.h"

public class RapidDxfListener:
	public CollectionListener
{
public:
	RapidDxfListener();
	virtual ~RapidDxfListener();
	virtual void itemAdded(BaseItem* item, Listenable* sender);
    virtual void itemRemoved(BaseItem* item, Listenable* sender);
	virtual void itemChanged(BaseItem* item, Listenable* sender);
	virtual void selectionChanged(Listenable* sender);
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
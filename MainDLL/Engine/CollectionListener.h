//Listener class..
//Vitrual base abstract(pure virtual functions) class...
//Functions declarations to handle the item added, removed, changed property...
// RcadApp, RapidShapeListener, RcaddimListener, Rcaducslistener, Dxflistener are derived from this Class
#pragma once
#include "BaseItem.h"

class Listenable;

class MAINDLL_API CollectionListener
{
public:
	virtual void itemAdded(BaseItem* item, Listenable* sender) = 0;
	virtual void itemRemoved(BaseItem* item, Listenable* sender) = 0;
	virtual void itemChanged(BaseItem* item, Listenable* sender) = 0;
	virtual void selectionChanged(Listenable* sender) = 0;
};

//Created by Rajavanya.. Modified by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
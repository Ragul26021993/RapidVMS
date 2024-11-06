//Base Class for BaseItem class..
//Used to maitain the listenrs for the entity.. 
//Add/removing relationship object pointers
//notify whenever data related to this entity is changed.. i.e update the related entities/collection base..
#pragma once
#include <list>
#include <iostream>
#include "ItemListener.h"
#include "..\MainDLL.h"
using namespace std;

class MAINDLL_API ListenableItem
{
	//List of listeners.. RcollectionBase, All derived shapes, Measurements etc..
	list<ItemListener*> listeners;

public:
	ListenableItem();
	~ListenableItem();
	void addListener(ItemListener* listener);
	void removeListener(ItemListener* listener);
	void notifyAll(ITEMSTATUS, BaseItem* sender);
};

//Created by Rajavanya.. Modified by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
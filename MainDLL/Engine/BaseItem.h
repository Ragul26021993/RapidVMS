//Base item class... Inermediate Base Class for entities.. Shape/Measurement/Ucs/Actions/DxfCollection
//Derived from listenable item...
//Maintain the id, Item type and selected property of the entities...
#pragma once
#include <TCHAR.H>
#include "ListenableItem.h"
#include "..\MainDLL.h"
#include "..\Engine\RapidEnums.h"
#include "..\RapidGraphics\Engine\RGraphicsFont.h"

class MAINDLL_API BaseItem:
	public ListenableItem
{
private:
	unsigned int id;
	static int itemnumber;
	bool isselected;
	std::wstring WEntityName;
	std::string EntityName;

public:
	//Constructor and destructor..
	BaseItem(TCHAR* name);
	virtual ~BaseItem(); 

	//Set and get the Id of the entity
	void setId(int i, bool IncrementId = false);
	int getId();

	//Selected property of the entity..
	void selected(bool value, bool needtoNotify = true);
	bool selected();

	//Set Original name of the entity..
	void setOriginalName(TCHAR* name);
	void setOriginalName(std::wstring name);
	//Se Modified name of the entity
	void setModifiedName(char* name);
	void setModifiedName(std::string name);
	//return the entity name: orginal and modified
	TCHAR* getOriginalName();
	char* getModifiedName();
	
	//Reset the id..//
	static void reset();
};

//Created by Rajavanya.. Modified by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
#include <StdAfx.h>
#include "BaseItem.h"
#include "..\Engine\RCadApp.h"

//Constructor
BaseItem::BaseItem(TCHAR* name)
{
	try
	{
		setOriginalName(name);
		setId(itemnumber++);
		this->isselected = false;
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("BITEM0001", __FILE__, __FUNCSIG__); }
}

//Destructor..//
BaseItem::~BaseItem()
{
}


//Set the item id..//
void BaseItem::setId(int i, bool IncrementId)
{
	if(itemnumber < i)
	{
		itemnumber = i;
		if(IncrementId)
			itemnumber++;
	}
	this->id = i;
}

//Returns the Id of the entity..//
int BaseItem::getId()
{
	return this->id;
}


//Selected Property of the entity..//If already selected then don't to anything//
void BaseItem::selected(bool value, bool needtoNotify)
{
	try
	{
		if(isselected != value)
		{
			this->isselected = value;
			if(needtoNotify) notifyAll(SELECTIONCHANGED, this);
		}
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("BITEM0002", __FILE__, __FUNCSIG__); }
}

//Get the selected property of the current entity
bool BaseItem::selected()
{
	return this->isselected;
}


//Set the entity name.//
void BaseItem::setOriginalName(TCHAR* name)
{
	this->WEntityName = name;
	this->EntityName = RMATH2DOBJECT->WStringToString(WEntityName);
}

//Set the entity name.//
void BaseItem::setOriginalName(std::wstring name)
{
	this->WEntityName = name;
	this->EntityName = RMATH2DOBJECT->WStringToString(name);
}

//Set the entity name.//
void BaseItem::setModifiedName(char* name)
{
	this->EntityName = name;
}

//Set the entity name.//
void BaseItem::setModifiedName(std::string name)
{
	this->EntityName = name;
}


//Returns the entity(item) name.//
TCHAR* BaseItem::getOriginalName()
{
	return (TCHAR*)WEntityName.c_str();
}

//Returns the entity(item) name.//
char* BaseItem::getModifiedName()
{
	return (char*)EntityName.c_str();
}


//Initialize the static member..Entity Id counter.//
int BaseItem::itemnumber = 0;
//Reset the item Id number//
void BaseItem::reset()
{
	itemnumber = 0;
}
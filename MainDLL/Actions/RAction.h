#pragma once
#include <iostream>
#include "..\Engine\BaseItem.h"
#include "..\Engine\RCollectionBase.h"
#include "..\FrameGrab\FramegrabBase.h"
#include "..\Helper\General.h"
//base class of all Rapid Actions.. Used in Partprogram and undo, redo of the actions..
//Derived from item base... Maintains the id...//
class MAINDLL_API RAction: 
	public BaseItem
{

public:
	//Constructor and destructor..
	RAction(TCHAR* name);
	virtual ~RAction();

	//Virtual Functions..
	virtual bool execute() = 0;
	virtual void undo() = 0;
	virtual bool redo() = 0;
	
	RapidEnums::ACTIONTYPE CurrentActionType; //Action Type
	RapidProperty<bool> ActionStatus; //Flag to make action active/ inactive..
	RapidProperty<bool> DontSaveInPartProgram;
};

//Created by Sathyanarayana.
//Copyright (c) Customised Technologies (P) Ltd.  ALL RIGHTS RESERVED.!
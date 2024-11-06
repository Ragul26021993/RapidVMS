#include "StdAfx.h"
#include "RAction.h"

RAction::RAction(TCHAR* name):BaseItem(name)
{
	this->DontSaveInPartProgram(false);
}

RAction::~RAction()
{
}
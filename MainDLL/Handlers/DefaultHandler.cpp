#include "StdAfx.h"
#include "DefaultHandler.h"
#include "..\Engine\RCadApp.h"
//Default handler .. do nothing.....!!!!
DefaultHandler::DefaultHandler(bool flag)
{
	setMaxClicks(1);
	if(flag)
	{
		MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER;
		MAINDllOBJECT->CURRENTRAPIDHANDLER = RapidEnums::RAPIDDRAWHANDLERTYPE::RAPIDDEFAULT_HANLDER;
	}
}

DefaultHandler::~DefaultHandler()
{
}

void DefaultHandler::LmouseDown()
{
}

void DefaultHandler::mouseMove()
{
}	


void DefaultHandler::draw(int windowno, double WPixelWidth)
{
}

void DefaultHandler::LmaxmouseDown()
{
}
void DefaultHandler::EscapebuttonPress()
{
}

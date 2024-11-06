#include "StdAfx.h"
#include "ProbeHandler.h"
#include "..\Engine\RCadApp.h"
//Default handler .. do nothing.....!!!!
ProbeHandler::ProbeHandler(bool flag)
{
	setMaxClicks(1);
	if(flag)
	{
		//FinishedCurrentDrawing = true;
		MAINDllOBJECT->CURRENTHANDLE = RapidEnums::RAPIDHANDLERTYPE::PROBE_HANDLER;
		MAINDllOBJECT->CURRENTRAPIDHANDLER = RapidEnums::RAPIDDRAWHANDLERTYPE::RAPIDDEFAULT_HANLDER;
	}
}

ProbeHandler::~ProbeHandler()
{
}

void ProbeHandler::LmouseDown()
{
}

void ProbeHandler::mouseMove()
{
}	


void ProbeHandler::draw(int windowno, double WPixelWidth)
{
}

void ProbeHandler::LmaxmouseDown()
{
}
void ProbeHandler::EscapebuttonPress()
{
}

#include "stdafx.h"
#include "../Engine/RCadApp.h"
#include "UCSPointHandle.h"

//Constructor...//
UCSPointHandle::UCSPointHandle()
{
	try
	{
		setMaxClicks(1);
		/*MAINDllOBJECT->SetStatusCode("UCS001");*/
		MAINDllOBJECT->SetStatusCode("UCSPointHandle01");
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("UCSPTMH0001", __FILE__, __FUNCSIG__); }
}

UCSPointHandle::~UCSPointHandle()
{

}

void UCSPointHandle::mouseMove()
{
}

void UCSPointHandle::LmouseDown()
{
}

void UCSPointHandle::EscapebuttonPress()
{
}

void UCSPointHandle::draw(int windowno, double WPixelWidth)
{
}

void UCSPointHandle::LmaxmouseDown()
{
	try
	{
		//If there is no highlighted point then return...
		if(MAINDllOBJECT->highlightedPoint() == NULL)
		{
			setClicksDone(getMaxClicks() - 1);
			return;
		}
		UCS::CreateUcs(0, MAINDllOBJECT->highlightedPoint());
		MAINDllOBJECT->SetStatusCode("Finished");
		MAINDllOBJECT->changeHandler(RapidEnums::RAPIDHANDLERTYPE::DEFAULT_HANDLER, true);
	}
	catch(...){ MAINDllOBJECT->SetAndRaiseErrorMessage("UCSPTMH0002", __FILE__, __FUNCSIG__); }
}